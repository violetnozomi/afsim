// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// -*-c++-*-

/*
 * MOD loader for Open Scene Graph
 *
 */

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <map>
#include <set>
#include <stdlib.h>
#include <string>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Notify>
#include <osg/Quat>
#include <osg/StateSet>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/Texture2D>
#include <osg/Vec3f>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Tessellator>
#include <osgUtil/TriStripVisitor>

class ReaderWriterMod : public osgDB::ReaderWriter
{
public:
   ReaderWriterMod() { supportsExtension("mod", "Model definition file"); }

   const char* className() const override { return "Mod Reader"; }

   ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const override;

   ReadResult readNode(std::istream& fin, const Options* options) const override;

   WriteResult writeObject(const osg::Object& obj, const std::string& fileName, const Options* options = nullptr) const override
   {
      return WriteResult(WriteResult::FILE_NOT_HANDLED);
   }

   WriteResult writeNode(const osg::Node& node, const std::string& fileName, const Options* options = nullptr) const override
   {
      return WriteResult(WriteResult::FILE_NOT_HANDLED);
   }

   WriteResult writeObject(const osg::Object& obj, std::ostream& fout, const Options* options = nullptr) const override
   {
      return WriteResult(WriteResult::FILE_NOT_HANDLED);
   }

   WriteResult writeNode(const osg::Node& node, std::ostream& fout, const Options* = nullptr) const override
   {
      return WriteResult(WriteResult::FILE_NOT_HANDLED);
   }


protected:
   bool ReadCommand(std::istream&                         fin,
                    osg::Group*                           aNode,
                    osg::Geometry*&                       aGeom,
                    std::map<unsigned int, unsigned int>& aIndexLookup) const;
};

// register with Registry to instantiate the above reader/writer.
REGISTER_OSGPLUGIN(mod, ReaderWriterMod)

// read file and convert to OSG.
osgDB::ReaderWriter::ReadResult ReaderWriterMod::readNode(const std::string&                  file,
                                                          const osgDB::ReaderWriter::Options* options) const
{
   std::string ext = osgDB::getLowerCaseFileExtension(file);
   if (!acceptsExtension(ext))
      return ReadResult::FILE_NOT_HANDLED;

   std::string fileName = osgDB::findDataFile(file, options);
   if (fileName.empty())
      return ReadResult::FILE_NOT_FOUND;

   osgDB::ifstream fin(fileName.c_str());
   if (fin)
   {
      // code for setting up the database path so that internally referenced file are searched for on relative paths.
      osg::ref_ptr<Options> local_opt =
         options ? static_cast<Options*>(options->clone(osg::CopyOp::SHALLOW_COPY)) : new Options;
      local_opt->getDatabasePathList().push_front(osgDB::getFilePath(fileName));
      std::map<unsigned int, unsigned int> indexLookup;

      osg::Group* node = new osg::Group;

      osg::Material* material = new osg::Material();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
      node->getOrCreateStateSet()->setAttributeAndModes(material);
      osg::Geometry* geom = nullptr;

      while (!fin.eof())
      {
         if (!ReadCommand(fin, node, geom, indexLookup))
         {
            node = nullptr;
            return ReadResult::ERROR_IN_READING_FILE;
         }
      }
      return node;
   }
   else
   {
      osg::notify(osg::INFO) << "ReaderWriterMod::readNode could not open file: " << fileName.c_str() << std::endl;
   }

   return ReadResult::FILE_NOT_HANDLED;
}

osgDB::ReaderWriter::ReadResult ReaderWriterMod::readNode(std::istream& fin, const Options* options) const
{
   if (fin)
   {
      fin.imbue(std::locale::classic());

      osg::Group* node = new osg::Group;

      osg::Material* material = new osg::Material();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
      node->getOrCreateStateSet()->setAttributeAndModes(material);
      osg::Geometry*                       geom = nullptr;
      std::map<unsigned int, unsigned int> indexLookup;

      while (!fin.eof())
      {
         if (!ReadCommand(fin, node, geom, indexLookup))
         {
            node = nullptr;
            return ReadResult::ERROR_IN_READING_FILE;
         }
      }
      return node;
   }

   return ReadResult::FILE_NOT_HANDLED;
}

bool ReaderWriterMod::ReadCommand(std::istream&                         fin,
                                  osg::Group*                           aNode,
                                  osg::Geometry*&                       aGeom,
                                  std::map<unsigned int, unsigned int>& aIndexLookup) const
{
   std::string command;
   fin >> command;
   if (fin.eof())
   {
      return true;
   }
   else if (command[0] == '!')
   {
      char line[1024];
      fin.getline(line, 1024);
      return true;
   }
   else if (command == "AT")
   {
      char line[1024];
      fin.getline(line, 1024);
      // TODO this is a color / material lookup
      return true;
   }
   else if (command == "MH")
   {
      std::string name;
      fin >> name;
      aNode->setName(name);
      return true;
   }
   else if (command == "OH")
   {
      aIndexLookup.clear();
      std::string name;
      fin >> name;
      osg::Geode* geode = new osg::Geode();
      geode->setName(name);
      aNode->addChild(geode);
      osg::Geometry* geometry = new osg::Geometry;
      geode->addDrawable(geometry);
      osg::Vec3Array* varray = new osg::Vec3Array;
      geometry->setVertexArray(varray);
      aGeom = geometry;
      return true;
      // create separate geode for the object definition
   }
   else if (command == "VERT")
   {
      int index;
      fin >> index;
      float x;
      float y;
      float z;
      fin >> x;
      fin >> y;
      fin >> z;
      if (nullptr != aGeom)
      {
         osg::Vec3Array* varray = dynamic_cast<osg::Vec3Array*>(aGeom->getVertexArray());
         aIndexLookup[index]    = varray->size();
         varray->push_back(osg::Vec3(x, y, z));
      }
      return true;
   }
   else if (command == "FA")
   {
      osg::DrawElementsUInt* poly = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
      std::string            mat;
      fin >> mat;

      char line[2048];
      fin.getline(line, 2048);
      std::stringstream strstr(line);
      while (!strstr.eof())
      {
         std::string cmd;
         strstr >> cmd;
         if ((cmd[0] == '!') || (cmd.empty()))
         {
            break;
         }
         else
         {
            std::stringstream strstr2(cmd.c_str());
            unsigned int      index;
            strstr2 >> index;
            if (0 != index)
            {
               poly->push_back(aIndexLookup[index]);
            }
         }
      }
      aGeom->addPrimitiveSet(poly);

      return true;
   }
   else
   {
      osg::notify(osg::INFO) << "ReaderWriterMod::ReadCommand unknown OGL file command: " << command << std::endl;
      return false;
   }
}
