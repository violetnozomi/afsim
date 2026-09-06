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
 * OGL loader for Open Scene Graph
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

class ReaderWriterOGL : public osgDB::ReaderWriter
{
public:
   ReaderWriterOGL() { supportsExtension("ogl", "Text file openGL commands"); }

   const char* className() const override { return "OGL Reader"; }

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
   bool        ReadCommand(std::istream& fin, osg::Group* aNode, osg::MatrixList& aMatrixList) const;
   bool        ReadVertices(std::istream& fin, osg::Vec3Array* aArray, const osg::Matrix& aModelView) const;
   osg::Geode* GetGeode(osg::Group* aGroup) const;
};

// register with Registry to instantiate the above reader/writer.
REGISTER_OSGPLUGIN(ogl, ReaderWriterOGL)

// read file and convert to OSG.
osgDB::ReaderWriter::ReadResult ReaderWriterOGL::readNode(const std::string&                  file,
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

      osg::MatrixList matrixList;
      matrixList.push_back(osg::Matrix::identity());
      osg::Group* node = new osg::Group;

      while (!fin.eof())
      {
         if (!ReadCommand(fin, node, matrixList))
         {
            node = nullptr;
            return ReadResult::ERROR_IN_READING_FILE;
         }
      }
      return node;
      /*
              obj::Model model;
              model.setDatabasePath(osgDB::getFilePath(fileName.c_str()));
              model.readOBJ(fin, local_opt.get());

              ObjOptionsStruct localOptions = parseOptions(options);

              osg::Node* node = convertModelToSceneGraph(model, localOptions, local_opt.get());
              return node;*/
   }
   else
   {
      osg::notify(osg::INFO) << "ReaderWriterOGL::readNode could not open file: " << fileName.c_str() << std::endl;
   }

   return ReadResult::FILE_NOT_HANDLED;
}

osgDB::ReaderWriter::ReadResult ReaderWriterOGL::readNode(std::istream& fin, const Options* options) const
{
   if (fin)
   {
      fin.imbue(std::locale::classic());

      osg::MatrixList matrixList;
      matrixList.push_back(osg::Matrix::identity());
      osg::Group* node = new osg::Group;

      while (!fin.eof())
      {
         if (!ReadCommand(fin, node, matrixList))
         {
            node = nullptr;
            return ReadResult::ERROR_IN_READING_FILE;
         }
      }
      return node;
      /*
              obj::Model model;
              model.readOBJ(fin, options);

              ObjOptionsStruct localOptions = parseOptions(options);

              osg::Node* node = convertModelToSceneGraph(model, localOptions, options);
              return node;*/
   }

   return ReadResult::FILE_NOT_HANDLED;
}

bool ReaderWriterOGL::ReadCommand(std::istream& fin, osg::Group* aNode, osg::MatrixList& aMatrixList) const
{
   std::string command;
   fin >> command;
   if (fin.eof())
   {
      return true;
   }
   else if (command[0] == '#')
   {
      char line[1024];
      fin.getline(line, 1024);
      return true;
   }
   else if (command == "glScale")
   {
      float xyz[3];
      fin >> xyz[0];
      fin >> xyz[1];
      fin >> xyz[2];
      aMatrixList.back().preMultScale(osg::Vec3f(xyz[0], xyz[1], xyz[2]));
      return true;
   }
   else if (command == "glRotate")
   {
      float ang;
      float xyz[3];
      fin >> ang;
      fin >> xyz[0];
      fin >> xyz[1];
      fin >> xyz[2];
      osg::Quat quat(osg::DegreesToRadians(ang), osg::Vec3d(xyz[0], xyz[1], xyz[2]));
      aMatrixList.back().preMultRotate(quat);
      return true;
   }
   else if (command == "glTranslate")
   {
      float xyz[3];
      fin >> xyz[0];
      fin >> xyz[1];
      fin >> xyz[2];
      aMatrixList.back().preMultTranslate(osg::Vec3f(xyz[0], xyz[1], xyz[2]));
      return true;
   }
   else if (command == "glColor")
   {
      std::string first;
      fin >> first;
      if (first == "team")
      {
         osg::Geode* geode = new osg::Geode();
         aNode->addChild(geode);
         osg::Material* material = new osg::Material();
         material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
         material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
         material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
         geode->getOrCreateStateSet()->setAttributeAndModes(material);
      }
      else if (first == "edge")
      {
         osg::Geode* geode = new osg::Geode();
         aNode->addChild(geode);
         osg::Material* material = new osg::Material();
         material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
         material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
         material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
         geode->getOrCreateStateSet()->setAttributeAndModes(material);
      }
      else
      {
         float             rgba[4];
         std::stringstream strstr(first);
         strstr >> rgba[0];
         fin >> rgba[1];
         fin >> rgba[2];
         fin >> rgba[3];
         osg::Geode* geode = new osg::Geode();
         aNode->addChild(geode);
         osg::Material* material = new osg::Material();
         material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
         material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
         material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
         geode->getOrCreateStateSet()->setAttributeAndModes(material);
      }
      return true;
   }
   else if (command == "glBegin")
   {
      osg::PrimitiveSet::Mode mode;
      std::string             subcommand;
      fin >> subcommand;
      if (subcommand == "GL_LINES")
      {
         mode = osg::PrimitiveSet::LINES;
      }
      else if (subcommand == "GL_LINE_STRIP")
      {
         mode = osg::PrimitiveSet::LINE_STRIP;
      }
      else if (subcommand == "GL_LINE_LOOP")
      {
         mode = osg::PrimitiveSet::LINE_LOOP;
      }
      else if (subcommand == "GL_TRIANGLES")
      {
         mode = osg::PrimitiveSet::TRIANGLES;
      }
      else if (subcommand == "GL_TRIANGLE_STRIP")
      {
         mode = osg::PrimitiveSet::TRIANGLE_STRIP;
      }
      else if (subcommand == "GL_TRIANGLE_FAN")
      {
         mode = osg::PrimitiveSet::TRIANGLE_FAN;
      }
      else if (subcommand == "GL_QUADS")
      {
         mode = osg::PrimitiveSet::QUADS;
      }
      else if (subcommand == "GL_QUAD_STRIP")
      {
         mode = osg::PrimitiveSet::QUAD_STRIP;
      }
      else if (subcommand == "GL_POLYGON")
      {
         mode = osg::PrimitiveSet::POLYGON;
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown OGL primitive: " << subcommand << std::endl;
         return false;
      }

      osg::Vec3Array* varray = new osg::Vec3Array;
      if (ReadVertices(fin, varray, aMatrixList.back()))
      {
         if (varray->size() != 0)
         {
            osg::Geometry* pGeomShape = new osg::Geometry;
            pGeomShape->setVertexArray(varray);
            osg::DrawElementsUInt* pset = new osg::DrawElementsUInt(mode);
            for (size_t i = 0; i < varray->size(); ++i)
            {
               pset->push_back(i);
            }
            pGeomShape->addPrimitiveSet(pset);
            GetGeode(aNode)->addDrawable(pGeomShape);
         }
         else
         {
            varray = nullptr;
         }
         return true;
      }
      else
      {
         varray = nullptr;
         return false;
      }
   }
   else if (command == "glEnable")
   {
      std::string subcommand;
      fin >> subcommand;
      if (subcommand == "GL_POLYGON_OFFSET_FILL")
      {
         // TODO DO SOMETHING HERE
         return true;
      }
      else if (subcommand == "GL_BLEND")
      {
         // TODO DO SOMETHING HERE
         return true;
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown OGL state: " << subcommand << std::endl;
         return false;
      }
   }
   else if (command == "glDisable")
   {
      std::string subcommand;
      fin >> subcommand;
      if (subcommand == "GL_POLYGON_OFFSET_FILL")
      {
         // TODO DO SOMETHING HERE
         return true;
      }
      else if (subcommand == "GL_BLEND")
      {
         // TODO DO SOMETHING HERE
         return true;
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown OGL state: " << subcommand << std::endl;
         return false;
      }
   }
   else if (command == "glPushMatrix")
   {
      aMatrixList.push_back(aMatrixList.back());
      return true;
   }
   else if (command == "glPopMatrix")
   {
      aMatrixList.pop_back();
      return true;
   }
   else if (command == "glBlendFunc")
   {
      std::string src;
      std::string dst;
      fin >> src;
      fin >> dst;
      if (src == "GL_SRC_ALPHA")
      {
      }
      else if (src == "GL_ONE_MINUS_SRC_ALPHA")
      {
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown blend src: " << src << std::endl;
         return false;
      }
      if (dst == "GL_SRC_ALPHA")
      {
      }
      else if (dst == "GL_ONE_MINUS_SRC_ALPHA")
      {
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown blend dst: " << dst << std::endl;
         return false;
      }
      // TODO DO SOMETHING WITH THIS
      return true;
   }
   else if (command == "glRect")
   {
      float xyzw[4];
      fin >> xyzw[0];
      fin >> xyzw[1];
      fin >> xyzw[2];
      fin >> xyzw[3];
      osg::Vec3Array* vecs = new osg::Vec3Array;
      vecs->push_back(osg::Vec3(xyzw[0], xyzw[1], 0) * aMatrixList.back());
      vecs->push_back(osg::Vec3(xyzw[2], xyzw[1], 0) * aMatrixList.back());
      vecs->push_back(osg::Vec3(xyzw[2], xyzw[3], 0) * aMatrixList.back());
      vecs->push_back(osg::Vec3(xyzw[0], xyzw[3], 0) * aMatrixList.back());

      osg::Geometry* pGeomShapeB = new osg::Geometry;
      pGeomShapeB->setVertexArray(vecs);
      osg::DrawElementsUInt* psetB = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
      for (size_t i = 0; i < vecs->size(); ++i)
      {
         psetB->push_back(i);
      }
      pGeomShapeB->addPrimitiveSet(psetB);
      GetGeode(aNode)->addDrawable(pGeomShapeB);

      return true;
   }
   else if (command == "gluCylinder")
   {
      float baserad;
      float toprad;
      float height;
      float slices;
      float stacks;
      fin >> baserad;
      fin >> toprad;
      fin >> height;
      fin >> slices;
      fin >> stacks;
      // make bottom
      osg::Vec3Array* bottomVerts = new osg::Vec3Array;
      for (int i = 0; i < slices + 1; ++i)
      {
         float ang = osg::PI * 2.0 * i / slices;
         bottomVerts->push_back(osg::Vec3(sin(ang) * baserad, cos(ang) * baserad, 0) * aMatrixList.back());
      }
      osg::Geometry* pGeomShapeB = new osg::Geometry;
      pGeomShapeB->setVertexArray(bottomVerts);
      osg::DrawElementsUInt* psetB = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
      for (size_t i = 0; i < bottomVerts->size(); ++i)
      {
         psetB->push_back(i);
      }
      pGeomShapeB->addPrimitiveSet(psetB);
      GetGeode(aNode)->addDrawable(pGeomShapeB);

      // make top
      osg::Vec3Array* topVerts = new osg::Vec3Array;
      for (int i = 0; i < slices + 1; ++i)
      {
         float ang = -osg::PI * 2.0 * i / slices;
         topVerts->push_back(osg::Vec3(sin(ang) * toprad, cos(ang) * toprad, height) * aMatrixList.back());
      }
      osg::Geometry* pGeomShapeT = new osg::Geometry;
      pGeomShapeT->setVertexArray(topVerts);
      osg::DrawElementsUInt* psetT = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON);
      for (size_t i = 0; i < topVerts->size(); ++i)
      {
         psetT->push_back(i);
      }
      pGeomShapeT->addPrimitiveSet(psetT);
      GetGeode(aNode)->addDrawable(pGeomShapeT);

      osg::Vec3Array* verts = new osg::Vec3Array;
      for (int i = 0; i < stacks; ++i)
      {
         int   oldsize = verts->size();
         float factor  = i / stacks;
         float radLow  = factor * toprad + (1 - factor) * baserad;
         float radHi   = (i + 1) / stacks * toprad + (1 - (i + 1) / stacks) * baserad;
         for (int j = 0; j < slices + 1; ++j)
         {
            float ang = osg::PI * 2.0 * j / slices;
            verts->push_back(osg::Vec3(sin(ang) * radHi, cos(ang) * radHi, (i + 1) / stacks * height) * aMatrixList.back());
            verts->push_back(osg::Vec3(sin(ang) * radLow, cos(ang) * radLow, i / stacks * height) * aMatrixList.back());
         }
         osg::Geometry* pGeomShapeS = new osg::Geometry;
         pGeomShapeS->setVertexArray(verts);
         osg::DrawElementsUInt* pset = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP);
         for (size_t i = oldsize; i < verts->size(); ++i)
         {
            pset->push_back(i);
         }
         pGeomShapeS->addPrimitiveSet(pset);
         GetGeode(aNode)->addDrawable(pGeomShapeS);
      }
      return true;
   }
   else if (command == "gluDisk")
   {
      float innerrad;
      float outerrad;
      float slices;
      float loops;
      fin >> innerrad;
      fin >> outerrad;
      fin >> slices;
      fin >> loops;

      osg::Vec3Array* verts = new osg::Vec3Array;
      for (int i = 0; i < loops; ++i)
      {
         float oldsize = verts->size();
         float factor  = i / loops;
         float factor2 = (i + 1) / loops;
         float radlow  = innerrad * factor + outerrad * (1 - factor);
         float radhi   = innerrad * factor2 + outerrad * (1 - factor2);
         for (int j = 0; j < slices + 1; ++j)
         {
            float ang = osg::PI * 2.0 * j / slices;
            verts->push_back(osg::Vec3(sin(ang) * radhi, cos(ang) * radhi, 0) * aMatrixList.back());
            verts->push_back(osg::Vec3(sin(ang) * radlow, cos(ang) * radlow, 0) * aMatrixList.back());
         }

         osg::Geometry* pGeomShapeS = new osg::Geometry;
         pGeomShapeS->setVertexArray(verts);
         osg::DrawElementsUInt* pset = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP);
         for (size_t i = oldsize; i < verts->size(); ++i)
         {
            pset->push_back(i);
         }
         pGeomShapeS->addPrimitiveSet(pset);
         GetGeode(aNode)->addDrawable(pGeomShapeS);
      }
      return true;
   }
   else if (command == "gluSphere")
   {
      float radius;
      float slices;
      float stacks;
      fin >> radius;
      fin >> slices;
      fin >> stacks;
      // TODO DO SOMETHING WITH THIS
      return true;
   }
   else if (command == "gluQuadricDrawStyle")
   {
      std::string subcommand;
      fin >> subcommand;
      if (subcommand == "GLU_SILHOUETTE")
      {
         // TODO DO SOMETHING WITH THIS
         return true;
      }
      if (subcommand == "GLU_FILL")
      {
         // TODO DO SOMETHING WITH THIS
         return true;
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown gluQuadricDrawStyle: " << subcommand << std::endl;
         return false;
      }
   }
   else
   {
      osg::notify(osg::INFO) << "ReaderWriterOGL::ReadCommand unknown OGL file command: " << command << std::endl;
      return false;
   }
}

bool ReaderWriterOGL::ReadVertices(std::istream& fin, osg::Vec3Array* aArray, const osg::Matrix& aModelView) const
{
   while (!fin.eof())
   {
      std::string command;
      fin >> command;
      if (command == "glEnd")
      {
         return true;
      }
      else if (command == "glVertex")
      {
         float xyz[3];
         fin >> xyz[0];
         fin >> xyz[1];
         fin >> xyz[2];
         aArray->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]) * aModelView);
      }
      else
      {
         osg::notify(osg::INFO) << "ReaderWriterOGL::ReadVertices unknown OGL command inside glBegin: " << command
                                << std::endl;
         return false;
      }
   }
   osg::notify(osg::INFO) << "ReaderWriterOGL::ReadVertices reached end-of-file within glBegin" << std::endl;
   return false;
}

osg::Geode* ReaderWriterOGL::GetGeode(osg::Group* aGroup) const
{
   int i = aGroup->getNumChildren();
   if (0 == i)
   {
      osg::Geode* geode = new osg::Geode;
      aGroup->addChild(geode);
      return geode;
   }
   else
   {
      return dynamic_cast<osg::Geode*>(aGroup->getChild(i - 1));
   }
}
