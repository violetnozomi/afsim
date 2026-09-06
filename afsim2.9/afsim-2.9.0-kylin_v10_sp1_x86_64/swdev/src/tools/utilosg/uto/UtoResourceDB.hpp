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
// UtoResourceDBImp.hpp : header file

#if !defined(_UTORESOURCEDBIMP_HPP_)
#define _UTORESOURCEDBIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <iostream>
#include <map>
#include <unordered_map>

#include "UtoCallback.hpp"
#include "UtoPriv.hpp"
#include "UtoResourceCallback.hpp"
#include "UtoTypes.hpp"

class UtoResourceDB;
class UtoResource;
class UtoShape;
class UtoTerrain;
class UtoShapeResourceImp;

class UTILOSG_EXPORT UtoResourceDB
{
public:
   static UtoResourceDB& Instance();

   // Use UtoHashCompare for hash and less than for windows hash_map and
   // the equal operator for SGI hash_map or unordered_map by letting the
   // forth argument be empty.
   using TerrainMap = std::unordered_map<std::string, UtoTerrain*, UtoHashCompare>;
   using ShapeMap   = std::unordered_map<std::string, UtoShape*, UtoHashCompare>;

   typedef TerrainMap::iterator                          terrain_iterator;
   typedef ShapeMap::iterator                            shape_iterator;
   typedef std::pair<terrain_iterator, terrain_iterator> terrain_iterator_pair;
   typedef std::pair<shape_iterator, shape_iterator>     shape_iterator_pair;

   UtoResourceDB();
   virtual ~UtoResourceDB();

   bool LoadResources(const std::string& file, const UtoResourceCallback& callback = UtoResourceCallback())
   {
      // Empty name to load all.
      return LoadResources(file, "", callback);
   }

   shape_iterator LoadShape(const std::string& name, const std::string& file);
   shape_iterator LoadShape(const std::string& name, std::istream& input);

   shape_iterator Bind(const std::string& name, const UtoShape& shape);
   shape_iterator Lookup(const std::string& name);
   UtoShape*      FindShape(const std::string& aName);
   bool           IsBound(const std::string& name);
   bool           Unbind(const std::string& name);
   bool           Rebind(const std::string& name, const UtoShape& shape);

   terrain_iterator LoadTerrain(const std::string& name, const std::string& file);
   terrain_iterator LoadTerrain(const std::string& name, std::istream& input);
   terrain_iterator FindTerrain(const std::string& name);
   bool             UnloadTerrain(terrain_iterator it);
   terrain_iterator AddTerrain(UtoTerrain& Terrain, const std::string& Name);


   terrain_iterator_pair Terrains();
   shape_iterator_pair   Shapes();

   shape_iterator   AddShape_i(UtoShape* shape, const std::string& name);
   terrain_iterator AddTerrain_i(UtoTerrain* Terrain, const std::string& name);

protected:
private:
   UtoResourceDB(const UtoResourceDB&);
   UtoResourceDB& operator=(const UtoResourceDB&);

   bool LoadResources(const std::string&         file,
                      const std::string&         name,
                      const UtoResourceCallback& callback = UtoResourceCallback());
   bool LoadResources(std::istream& input, const std::string& name);

   TerrainMap m_Terrains;
   ShapeMap   m_Shapes;
   bool       m_bLoading;
};

#endif // !defined(_UTORESOURCEDBIMP_HPP_)
