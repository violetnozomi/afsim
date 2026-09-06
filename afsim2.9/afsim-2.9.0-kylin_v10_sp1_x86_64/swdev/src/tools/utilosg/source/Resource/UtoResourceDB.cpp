// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoResourceDBImp.cpp : implementation file

#include "UtoResourceDB.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

#include <limits.h>

#include "UtMemory.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"
#include "UtoShape.hpp"
#include "UtoShapeFactory.hpp"
#include "UtoShapeImp.hpp"
#include "UtoTerrain.hpp"
#include "UtoTerrainFactory.hpp"
#include "UtoTerrainImp.hpp"

#ifdef _WIN32
#include <string.h>

#include <direct.h>
#define STRNCASECMP _strnicmp
#include <windows.h>
#define MAX_PATH_LENGTH MAX_PATH
#else
#include <strings.h>
#include <unistd.h>
#define STRNCASECMP strncasecmp
#define MAX_PATH_LENGTH PATH_MAX
#endif
#include "UtoObjectManager.hpp"

class ResourceDefinitionScope;

typedef std::list<ResourceDefinitionScope*> ScopeList;
typedef std::vector<char>                   BufferType;

/////////////////////////////////////////////////////////////////////////////
class ResourceDefinitionScope
{
public:
   ResourceDefinitionScope() {}
   virtual ~ResourceDefinitionScope() {}
   virtual int process(const std::string& buf, ScopeList& scope) = 0;
};

class ResourceDefinitionScope_File : public ResourceDefinitionScope
{
public:
   ResourceDefinitionScope_File(UtoResourceDB* resdb, const std::string& search, const std::string& base);
   ~ResourceDefinitionScope_File() override;
   int            process(const std::string& buf, ScopeList& scope) override;
   UtoResourceDB* m_ResDB;
   std::string    m_SearchName;
   std::string    m_BasePath;
};

class ResourceDefinitionScope_Shape : public ResourceDefinitionScope
{
public:
   ResourceDefinitionScope_Shape(UtoResourceDB* ResDB, const std::string& type, const std::string& name, bool next = true);
   ~ResourceDefinitionScope_Shape() override;
   int            process(const std::string& buf, ScopeList& scope) override;
   UtoResourceDB* m_ResDB;
   UtoShape*      m_pShape;
   std::string    m_Name;
   bool           m_ProcessNext;
};

class ResourceDefinitionScope_Terrain : public ResourceDefinitionScope
{
public:
   ResourceDefinitionScope_Terrain(UtoResourceDB* ResDB, const std::string& type, const std::string& name, bool next = true);
   ~ResourceDefinitionScope_Terrain() override;
   int            process(const std::string& buf, ScopeList& scope) override;
   UtoResourceDB* m_ResDB;
   UtoTerrain*    m_pTerrain;
   std::string    m_Name;
   bool           m_ProcessNext;
};

/////////////////////////////////////////////////////////////////////////////

ResourceDefinitionScope_File::ResourceDefinitionScope_File(UtoResourceDB*     ResDB,
                                                           const std::string& name,
                                                           const std::string& base)
   : m_ResDB(ResDB)
   , m_SearchName(name)
   , m_BasePath(base)
{
   // add the base file location to the models search path too!
   UtoOptions::Instance()->AddModelsPath(m_BasePath);
}

ResourceDefinitionScope_File::~ResourceDefinitionScope_File() {}

int ResourceDefinitionScope_File::process(const std::string& buf, ScopeList& scope)
{
   int result = 0;

   if ((STRNCASECMP(buf.c_str(), "models_path", 11) == 0) || (STRNCASECMP(buf.c_str(), "res_path", 8) == 0))
   {
      // obtain the location for ' '
      std::string::size_type nLoc = buf.find(' ');

      // this is a file variable for the models path.
      std::string models_path(buf.begin() + nLoc + 1, buf.end());

      // StripLeadingTrailing(models_path);

      // check each path in <models_path> and convert relative to absolute
      std::string::size_type pos  = 0;
      std::string::size_type len  = models_path.find_first_of(UtoSearchDelimiter);
      UtoOptions*            opts = UtoOptions::Instance();

      while (pos != std::string::npos)
      {
         std::string temp;
         temp.assign(models_path, pos, len - pos);

         StripLeadingTrailing(temp);

         // add path to models path.
         const std::string& abs_path = UtoAbsolutePathOf(temp, m_BasePath);
         opts->AddModelsPath(abs_path);

         if (len != std::string::npos)
         {
            pos = len + 1;
            len = models_path.find_first_of(UtoSearchDelimiter, len + 1);
         }
         else
         {
            pos = len;
         }
      }
   }
   else
   {
      // look for section name.
      std::string::size_type loc = buf.find(' ');
      if (loc != buf.npos)
      {
         std::string name(buf, loc + 1, buf.size() - (loc + 1));

         if (m_SearchName.empty() || name == m_SearchName)
         {
            // Store the type
            std::string shape_type(buf, 0, loc);

            if (UtoShapeFactory::Instance()->Supports(shape_type))
               scope.push_front(new ResourceDefinitionScope_Shape(m_ResDB, shape_type, name, m_SearchName.empty()));
            else if (UtoTerrainFactory::Instance()->Supports(shape_type))
               scope.push_front(new ResourceDefinitionScope_Terrain(m_ResDB, shape_type, name, m_SearchName.empty()));
         }
      }
   }
   return result;
}

/////////////////////////////////////////////////////////////////////////////

ResourceDefinitionScope_Shape::ResourceDefinitionScope_Shape(UtoResourceDB*     ResDB,
                                                             const std::string& type,
                                                             const std::string& name,
                                                             bool               next)
   : m_ResDB(ResDB)
   , m_pShape(nullptr)
   , m_Name(name)
   , m_ProcessNext(next)
{
   // call virtual constructor.
   m_pShape = UtoShapeFactory::Instance()->CreateShape(type);
   assert(m_pShape);

   // start the load process
   m_pShape->Impl()->PreLoad();
}

ResourceDefinitionScope_Shape::~ResourceDefinitionScope_Shape()
{
   // end the load process
   if (m_pShape)
   {
      // try to post load the model shape
      bool bLoaded = m_pShape->Impl()->PostLoad();

      // if the shape was loaded correctly, add to the database
      if (bLoaded)
      {
         m_ResDB->AddShape_i(m_pShape, m_Name);
      }
      else
      {
         delete m_pShape;
      }
   }
}

int ResourceDefinitionScope_Shape::process(const std::string& buf, ScopeList& scope)
{
   int result = 0;
   // split into attribute & remainder
   std::string::size_type loc = buf.find(' ');
   std::string            attr, remainder;
   if (loc != buf.npos)
   {
      attr.assign(buf, 0, loc);
      remainder.assign(buf, loc + 1, buf.size() + (loc + 1));
   }
   else
   {
      // treat as an attribute without remainder
      attr = buf;
   }

   // test for shape type
   if (UtoShapeFactory::Instance()->Supports(attr))
   {
      // finish loading the current shape.
      bool bLoaded = m_pShape->Impl()->PostLoad();

      // add the resource to the database if it was loaded correcly
      if (bLoaded)
      {
         m_ResDB->AddShape_i(m_pShape, m_Name);
      }
      else
      {
         delete m_pShape;
      }

      // nullify the shape since it was already added
      m_pShape = nullptr;

      if (m_ProcessNext)
      {
         // begin with a new shape.
         m_Name   = remainder;
         m_pShape = UtoShapeFactory::Instance()->CreateShape(attr);
         assert(m_pShape);

         // start the load process
         m_pShape->Impl()->PreLoad();
      }
      else
         result = 1; // done
   }
   // test for terrain type
   else if (UtoTerrainFactory::Instance()->Supports(attr))
   {
      scope.pop_front();
      // need to begin loading a terrain
      scope.push_front(new ResourceDefinitionScope_Terrain(m_ResDB, attr, remainder));
      result = -1; // scope switched
   }
   // read shape attribute
   else if (!m_pShape->Impl()->ReadAttribute(attr, remainder))
   {
      // LOG("ResourceDB : Invalid file entry: %s\n", buf.c_str());
   }

   return result;
}

/////////////////////////////////////////////////////////////////////////////

ResourceDefinitionScope_Terrain::ResourceDefinitionScope_Terrain(UtoResourceDB*     ResDB,
                                                                 const std::string& type,
                                                                 const std::string& name,
                                                                 bool               next)
   : m_ResDB(ResDB)
   , m_pTerrain(nullptr)
   , m_Name(name)
   , m_ProcessNext(next)
{
   // call virtual constructor.
   m_pTerrain = UtoTerrainFactory::Instance()->CreateTerrain(type);
   assert(m_pTerrain);

   // start the load process
   m_pTerrain->Impl()->PreLoad();
}

ResourceDefinitionScope_Terrain::~ResourceDefinitionScope_Terrain()
{
   // end the load process
   if (m_pTerrain)
   {
      m_pTerrain->Impl()->PostLoad();
      m_ResDB->AddTerrain_i(m_pTerrain, m_Name);
   }
}

int ResourceDefinitionScope_Terrain::process(const std::string& buf, ScopeList& scope)
{
   int result = 0;
   // split into attribute & remainder
   std::string::size_type loc = buf.find(' ');
   std::string            attr, remainder;
   if (loc != buf.npos)
   {
      attr.assign(buf, 0, loc);
      remainder.assign(buf, loc + 1, buf.size() + (loc + 1));
   }
   else
   {
      // treat as an attribute without remainder
      attr = buf;
   }

   // test for shape type
   if (UtoShapeFactory::Instance()->Supports(attr))
   {
      scope.pop_front();
      scope.push_front(new ResourceDefinitionScope_Shape(m_ResDB, attr, remainder));
      result = -1;
   }
   // test for terrain type
   else if (UtoTerrainFactory::Instance()->Supports(attr))
   {
      if (m_pTerrain)
      {
         // finish loading the current terrain.
         m_pTerrain->Impl()->PostLoad();
         m_ResDB->AddTerrain_i(m_pTerrain, m_Name);
         m_pTerrain = nullptr;
      }

      if (m_ProcessNext)
      {
         // begin with a new terrain.
         m_Name     = remainder;
         m_pTerrain = UtoTerrainFactory::Instance()->CreateTerrain(attr);
         assert(m_pTerrain);

         // start the load process
         m_pTerrain->Impl()->PreLoad();
      }
      else
         result = 1; // done
   }
   // read terrain attribute
   else if (m_pTerrain && !m_pTerrain->Impl()->ReadAttribute(attr, remainder))
   {
      // LOG("ResourceDB : Invalid file entry: %s\n", buf.c_str());

      // destroy the terrain type, invalid attribute
      delete m_pTerrain;
      // set to null so the destructor does not add the terrain
      m_pTerrain = nullptr;

      // indicate that this scope is over
      scope.pop_front();
      result = -1;
   }

   // if (!m_ProcessNext && result == -1)
   //    result = 1;

   return result;
}

/////////////////////////////////////////////////////////////////////////////

UtoResourceDB::UtoResourceDB()
   : m_bLoading(false)
{
}

UtoResourceDB::~UtoResourceDB()
{
   terrain_iterator ti, tend(m_Terrains.end());
   for (ti = m_Terrains.begin(); ti != tend; ++ti)
   {
      delete (ti->second);
   }
   m_Terrains.clear();

   shape_iterator si, send(m_Shapes.end());
   for (si = m_Shapes.begin(); si != send; ++si)
   {
      delete (si->second);
   }
   m_Shapes.clear();
}

UtoResourceDB::shape_iterator UtoResourceDB::LoadShape(const std::string& name, const std::string& file_name)
{
   // check for existing model name and return the end iterator if found.
   shape_iterator it = m_Shapes.find(name);
   shape_iterator end(m_Shapes.end());
   if (it != end)
      return end;

   // load only one shape.
   if (LoadResources(file_name, name))
   {
      it = m_Shapes.find(name);
   }
   else
   {
      it = end;
   }

   return it;
}

UtoResourceDB::shape_iterator UtoResourceDB::LoadShape(const std::string& name, std::istream& input)
{
   // check for existing model name and return the end iterator if found.
   shape_iterator it = m_Shapes.find(name);
   shape_iterator end(m_Shapes.end());
   if (it != end)
      return end;

   // load only one shape.
   if (LoadResources(input, name))
   {
      it = m_Shapes.find(name);
   }
   else
   {
      it = end;
   }

   return it;
}

bool UtoResourceDB::LoadResources(const std::string& file_name, const std::string& name, const UtoResourceCallback& callback)
{
   std::vector<char> cwdir_buf(MAX_PATH_LENGTH, '\0');
   const char*       cwdir = getcwd(&cwdir_buf[0], cwdir_buf.size());
   if (cwdir == nullptr)
   {
      // LOG("ResourceDB : Failed to get current working directory\n");
      return false;
   }

   // store original models_path
   std::string models_path_plus_cwdir(cwdir);
   std::string models_path_orig = UtoOptions::Instance()->ModelsPath();
   // Don't add an empty models_path_orig, with some other "bugs" it will
   // search the root directory.
   if (!models_path_orig.empty())
      models_path_plus_cwdir += ";" + models_path_orig;

   std::string file_path = UtoFindFirstPath(file_name, models_path_plus_cwdir);
   if (file_path.empty())
   {
      // LOG("ResourceDB : Failed to find resource file %s\n", file_name.c_str());
      return false;
   }

   m_bLoading = true;

   ScopeList          Scope;
   const std::string& abs_path = UtoAbsolutePathOf(file_path, cwdir);
   Scope.push_front(new ResourceDefinitionScope_File(this, name, abs_path));

   bool          success = false;
   std::ifstream file;
   file.open(file_path.c_str());
   if (file.is_open())
   {
      std::string buffer;
      buffer.reserve(128);

      // seek to section in file, & determine type
      int nCount = 0;
      while (std::getline(file, buffer))
      {
         StripLeadingTrailing(buffer);

         // this is a comment, or a blank line.
         if (buffer.empty() || buffer[0] == '#' || buffer[0] == '\0')
            continue;

         // get first word in buffer line
         std::string::size_type pos = buffer.find_first_of(' ');
         std::string            shape_type(buffer, 0, pos);

         // count the initial resource lines
         if (UtoShapeFactory::Instance()->Supports(shape_type) || UtoTerrainFactory::Instance()->Supports(shape_type))
         {
            ++nCount;
         }
      }

      callback.NumResourcesToLoad(nCount);

      // Must clear for eof or bad or seek won't happen.
      if (file.eof() || file.bad())
         file.clear();
      // seek to the start for real loading
      file.seekg(0);

      // seek to section in file, & determine type
      nCount = 0;
      std::string resource_name; // current resource being processed
      while (std::getline(file, buffer))
      {
         StripLeadingTrailing(buffer);

         // this is a comment, or a blank line.
         if (buffer.empty() || buffer[0] == '#' || buffer[0] == '\0')
            continue;

         // inform the user callback that loading is beginning
         std::string::size_type loc = buffer.find(' ');
         if (loc != buffer.npos)
         {
            std::string name(buffer, loc + 1, buffer.size() - (loc + 1));

            // Store the type
            std::string shape_type(buffer, 0, loc);

            if (UtoShapeFactory::Instance()->Supports(shape_type) || UtoTerrainFactory::Instance()->Supports(shape_type))
            {
               // Starting a new resource, close out the old one, and abort
               // if requested.
               if (!resource_name.empty() && !callback.OnLoadFinish(resource_name))
               {
                  // leave resource_name empty for after the while
                  resource_name.clear();
                  break;
               }
               resource_name = name;
               callback.OnLoadBegin(name, shape_type, nCount++);
            }
         }

         ResourceDefinitionScope* cur_scope = Scope.front();
         int                      result    = cur_scope->process(buffer, Scope);

         if (result < 0)
         {
            delete cur_scope;
         }
         else if (result > 0)
         {
            break;
         }
      }

      // Last resource if an entry currently exists, don't bother with the
      // return check, it's the last entry anyway.
      if (!resource_name.empty())
         callback.OnLoadFinish(resource_name);

      while (!Scope.empty())
      {
         delete Scope.front();
         Scope.pop_front();
      }
   }
   else
   {
      // LOG("ResourceDB : Failed to open resource file %s\n", file_name.c_str());
   }

   m_bLoading = false;

   // reset the original path.
   UtoOptions::Instance()->SetModelsPath(models_path_orig);

   // unsigned int amount = vtRenderContext::Textures().MemoryUsage();
   // DEBUG_LEV(2,("Texture memory usage: %g Mb {%d bytes}\n",
   //           (double)amount /(1024*1024), amount));

   //  if ((m_Shapes.size() + m_Terrains.size()) != count)
   {
      success = true;
   }

   return success;
}

bool UtoResourceDB::LoadResources(std::istream& input, const std::string& name)
{
   std::vector<char> cwdir_buf(MAX_PATH_LENGTH, '\0');
   const char*       cwdir = getcwd(&cwdir_buf[0], cwdir_buf.size());
   if (cwdir == nullptr)
   {
      // LOG("ResourceDB : Failed to get current working directory\n");
      return false;
   }

   // store original models_path
   std::string models_path_plus_cwdir(cwdir);
   std::string models_path_orig = UtoOptions::Instance()->ModelsPath();
   // Don't add an empty models_path_orig, with some other "bugs" it will
   // search the root directory.
   if (!models_path_orig.empty())
      models_path_plus_cwdir += ";" + models_path_orig;

   m_bLoading = true;

   ScopeList         Scope;
   const std::string abs_path = "";
   Scope.push_front(new ResourceDefinitionScope_File(this, name, abs_path));

   bool success = false;
   if (input.good())
   {
      std::string buffer;
      buffer.reserve(128);

      // seek to section in input, & determine type
      while (std::getline(input, buffer))
      {
         ResourceDefinitionScope* cur_scope = Scope.front();

         if (!buffer.empty())
         {
            StripLeadingTrailing(buffer);
         }

         // this is a comment, or a blank line.
         if (buffer.empty() || buffer[0] == '#' || buffer[0] == '\0')
            continue;

         int result = cur_scope->process(buffer, Scope);
         if (result < 0)
         {
            // scope was switched, so delete this scope
            delete cur_scope;
         }
         else if (result > 0)
         {
            break;
         }
      }

      while (!Scope.empty())
      {
         delete Scope.front();
         Scope.pop_front();
      }
   }
   else
   {
      // LOG("ResourceDB : Failed to open resource input %s\n", name.c_str());
   }

   m_bLoading = false;

   // reset the original path.
   UtoOptions::Instance()->SetModelsPath(models_path_orig);

   // unsigned int amount = vtRenderContext::Textures ().MemoryUsage ();
   // DEBUG_LEV (2, ("Texture memory usage: %g Mb {%d bytes}\n",
   //            (double)amount / (1024*1024), amount));

   //  if ((int)(m_Shapes.size() + m_Terrains.size()) != count)
   {
      success = true;
   }

   return success;
}

// bool UtoResourceDBImp::WriteShapes (const std::string& file);

UtoResourceDB::shape_iterator UtoResourceDB::Bind(const std::string& name, const UtoShape& shape)
{
   shape_iterator end(m_Shapes.end());
   shape_iterator it = m_Shapes.find(name);

   if (it == end)
   {
      UtoShape* p = shape.Clone();
      if (p == nullptr)
      {
         // DEBUG(("Failed to clone shape %s during bind.", name.c_str()));
         it = end;
      }
      else
      {
         it = AddShape_i(p, name);
      }
   }
   else
   {
      it = end;
   }

   return it;
}

UtoResourceDB::shape_iterator UtoResourceDB::Lookup(const std::string& name)
{
   return m_Shapes.find(name);
}

UtoShape* UtoResourceDB::FindShape(const std::string& aName)
{
   ShapeMap::iterator i = m_Shapes.find(aName);
   if (i != m_Shapes.end())
   {
      return i->second;
   }
   return nullptr;
}

bool UtoResourceDB::IsBound(const std::string& name)
{
   shape_iterator it = m_Shapes.find(name);
   return (it != m_Shapes.end());
}

bool UtoResourceDB::Unbind(const std::string& name)
{
   shape_iterator it = m_Shapes.find(name);
   if (it != m_Shapes.end())
   {
      // Unref the shape
      delete (it->second);

      // remove the shape from the list
      m_Shapes.erase(it);

      return true;
   }
   return false;
}

bool UtoResourceDB::Rebind(const std::string& name, const UtoShape& shape)
{
   bool      success = true;
   UtoShape* p       = shape.Clone();
   if (p == nullptr)
   {
      // DEBUG(("Failed to clone shape %s during rebind.", name.c_str()));
      success = false;
   }
   else
   {
      ShapeMap::iterator it = m_Shapes.find(name);
      if (it != m_Shapes.end())
      {
         // remove the existing shape.
         delete (it->second);
         it->second = p;
      }
      else
      {
         success = (AddShape_i(p, name) != m_Shapes.end());
      }
   }

   return success;
}

UtoResourceDB::terrain_iterator UtoResourceDB::LoadTerrain(const std::string& name, const std::string& file)
{
   // check for existing model name and return the end iterator if found.
   terrain_iterator it = m_Terrains.find(name);
   terrain_iterator end(m_Terrains.end());
   if (it != end)
      return end;

   // load only one shape.
   if (LoadResources(file, name))
   {
      it = m_Terrains.find(name);
   }
   else
   {
      it = end;
   }

   return it;
}

UtoResourceDB::terrain_iterator UtoResourceDB::LoadTerrain(const std::string& name, std::istream& input)
{
   // check for existing model name and return the end iterator if found.
   terrain_iterator it = m_Terrains.find(name);
   terrain_iterator end(m_Terrains.end());
   if (it != end)
      return end;

   // load only one shape.
   if (LoadResources(input, name))
   {
      it = m_Terrains.find(name);
   }
   else
   {
      it = end;
   }

   return it;
}

UtoResourceDB::terrain_iterator UtoResourceDB::FindTerrain(const std::string& name)
{
   return m_Terrains.find(name);
}

bool UtoResourceDB::UnloadTerrain(terrain_iterator it)
{
   m_Terrains.erase(it);
   return true;
}

UtoResourceDB::terrain_iterator UtoResourceDB::AddTerrain(UtoTerrain& Terrain, const std::string& Name)
{
   return AddTerrain_i(Terrain.Clone(), Name);
}

UtoResourceDB::terrain_iterator_pair UtoResourceDB::Terrains()
{
   terrain_iterator_pair ip = std::make_pair(m_Terrains.begin(), m_Terrains.end());
   return ip;
}

UtoResourceDB::shape_iterator_pair UtoResourceDB::Shapes()
{
   shape_iterator_pair ip = std::make_pair(m_Shapes.begin(), m_Shapes.end());
   return ip;
}

UtoResourceDB::shape_iterator UtoResourceDB::AddShape_i(UtoShape* shape, const std::string& name)
{
   std::pair<ShapeMap::iterator, bool> ib = m_Shapes.insert(ShapeMap::value_type(name, shape));
   if (!ib.second)
   {
      // DEBUG(("Failed to insert shape %s.", name.c_str()));
      delete shape;
   }
   else if (!m_bLoading)
   {
   }

   return ib.first;
}

UtoResourceDB::terrain_iterator UtoResourceDB::AddTerrain_i(UtoTerrain* Terrain, const std::string& name)
{
   Terrain->SetName(name);
   std::pair<TerrainMap::iterator, bool> ib = m_Terrains.insert(TerrainMap::value_type(name, Terrain));
   if (!ib.second)
   {
      // DEBUG(("Failed to insert terrain %s.", name.c_str()));
      delete Terrain;
   }
   return ib.first;
}

int ResDBDestroyer(UtoResourceDB* obj)
{
   delete obj;
   return 0;
}

/**
\brief Retrieves the singleton resource database.

Retrieves the singleton resource database.  The resource database is
created during UtoInit.
\return UtoResourceDB & - a reference to the database.
*/
UtoResourceDB& UtoResourceDB::Instance()
{
   UtoResourceDB* db = nullptr;

   UtoObjectManager* omgr = UtoObjectManager::Instance();
   if (!omgr->IsRegistered("ResDB"))
   {
      // create the resource DB.
      auto obj = ut::make_unique<UtoResourceDB>();

      // insert DB into ObjectManager.
      if (!omgr->Register("ResDB", obj.get(), ResDBDestroyer))
         throw 0;

      db = obj.release();
   }
   else
   {
      db = UtoObjectSingleton<UtoResourceDB>::Instance("ResDB");
   }

   assert(db != nullptr);

   return *db;
}
