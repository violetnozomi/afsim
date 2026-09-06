// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaModelDatabase.hpp"

#include <assert.h>
#include <iostream>

#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>

#include "UtInputFile.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtPath.hpp"
#include "UtoMemory.hpp"
#include "UtoShaders.hpp"
#include "VaEnvironment.hpp"
#include "VaModelVisitor.hpp"
#include "VaObserver.hpp"
#include "VaOverlayModelInstantiations.hpp"
#include "VaViewer.hpp"

vespa::VaModelDefinition::Model* vespa::VaModelDefinition::msUnknownModelPtr = nullptr;

namespace
{
void ReadTransformation(UtInput& aInput, osg::Matrix& aMatrix)
{
   std::string command;
   while (aInput.TryReadCommand(command))
   {
      if (command == "scale")
      {
         float scale;
         aInput.ReadValue(scale);
         aMatrix.postMultScale(osg::Vec3(scale, scale, scale));
      }
      else if (command == "translate")
      {
         float x;
         float y;
         float z;
         aInput.ReadValue(x);
         aInput.ReadValue(y);
         aInput.ReadValue(z);
         aMatrix.postMultTranslate(osg::Vec3(x, y, z));
      }
      else if (command == "rotate")
      {
         std::string axisstr;
         float       angle;
         aInput.ReadCommand(axisstr);
         aInput.ReadValue(angle);
         osg::Vec3 axis(0.0f, 0.0f, 0.0f);
         if ((axisstr[0] == 'x') || (axisstr[0] == 'X'))
         {
            axis.set(1.0f, 0.0f, 0.0f);
         }
         else if ((axisstr[0] == 'y') || (axisstr[0] == 'Y'))
         {
            axis.set(0.0f, 1.0f, 0.0f);
         }
         else if ((axisstr[0] == 'z') || (axisstr[0] == 'Z'))
         {
            axis.set(0.0f, 0.0f, 1.0f);
         }
         aMatrix.postMultRotate(osg::Quat(angle * UtMath::cRAD_PER_DEG, axis));
      }
      else if (command == "end_pre_xform")
      {
         return;
      }
      else
      {
         auto out = ut::log::error() << "Error reading pre_xform, unknown command.";
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }
   }
}


osg::ref_ptr<osg::Node> BuildImageNode(osg::Image* aImagePtr)
{
   osg::ref_ptr<osg::Geode> imageGeode = ut::osg::make_ref<osg::Geode>();

   osg::ref_ptr<osg::Geometry> defaultGeometry = ut::osg::make_ref<osg::Geometry>();
   // create and bind a new vertex buffer
   osg::ref_ptr<osg::Vec3Array> pVertices = ut::osg::make_ref<osg::Vec3Array>();

   pVertices->setName("model_Vertex");
   /*   defaultGeometry->setVertexArray(pVertices);
      defaultGeometry->setTexCoordArray(0, tCoord);*/

   // obtain the color pointers

   // set the new size of the vertex buffer
   unsigned int nVertSize = 4;

   pVertices->resize(nVertSize);

   // obtain an iterator to the first vertex
   osg::Vec3Array::iterator itVertex = pVertices->begin();

   osg::ref_ptr<osg::DrawElementsUInt> face  = ut::osg::make_ref<osg::DrawElementsUInt>(GL_POLYGON);
   int                                 index = 0;

   osg::ref_ptr<osg::Vec4Array> pTCoords = ut::osg::make_ref<osg::Vec4Array>();
   pTCoords->setName("model_MultiTexCoord0");
   pTCoords->resize(nVertSize);
   osg::Vec4Array::iterator itTCoord = pTCoords->begin();

   int   s      = aImagePtr->s();
   int   t      = aImagePtr->t();
   float aspect = static_cast<float>(t) / static_cast<float>(s);
   itVertex->set(osg::Vec3(-0.5f * aspect, -0.5f, 0.0f));
   itTCoord->set(0.0f, 0.0f, 0.0f, 0.0f);
   face->push_back(index++);
   ++itVertex;
   ++itTCoord;
   itVertex->set(osg::Vec3(0.5f * aspect, -0.5f, 0.0f));
   itTCoord->set(0.0f, 1.0f, 0.0f, 0.0f);
   face->push_back(index++);
   ++itVertex;
   ++itTCoord;
   itVertex->set(osg::Vec3(0.5f * aspect, 0.5f, 0.0f));
   itTCoord->set(1.0f, 1.0f, 0.0f, 0.0f);
   face->push_back(index++);
   ++itVertex;
   ++itTCoord;
   itVertex->set(osg::Vec3(-0.5f * aspect, 0.5f, 0.0f));
   itTCoord->set(1.0f, 0.0f, 0.0f, 0.0f);
   face->push_back(index++);
   ++itVertex;
   ++itTCoord;

   defaultGeometry->setVertexAttribArray(0, pVertices);
   defaultGeometry->setVertexAttribNormalize(0, false);
   defaultGeometry->setVertexAttribBinding(0, osg::Geometry::BIND_PER_VERTEX);

   defaultGeometry->setVertexAttribArray(10, pTCoords);
   defaultGeometry->setVertexAttribNormalize(10, true);
   defaultGeometry->setVertexAttribBinding(10, osg::Geometry::BIND_PER_VERTEX);

   defaultGeometry->addPrimitiveSet(face);

   defaultGeometry->dirtyBound();

   std::set<std::string> nulldefs;
   imageGeode->getOrCreateStateSet()->addUniform(ut::osg::make_ref<osg::Uniform>("VA_ScreenScale", 1.0f));
   osg::ref_ptr<osg::Texture> texture = ut::osg::make_ref<osg::Texture2D>(aImagePtr);
   texture->setResizeNonPowerOfTwoHint(false);
   defaultGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
   defaultGeometry->getOrCreateStateSet()->setRenderBinDetails(14, "DepthSortedBin");
   defaultGeometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
   defaultGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::BlendFunc>(),
                                                                osg::StateAttribute::ON);
   osg::ref_ptr<osg::Material> smaterial = ut::osg::make_ref<osg::Material>();
   smaterial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
   smaterial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
   smaterial->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
   defaultGeometry->getOrCreateStateSet()
      ->getOrCreateUniform("material_ambient", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(0, 0, 0, 1));
   defaultGeometry->getOrCreateStateSet()
      ->getOrCreateUniform("material_diffuse", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(0, 0, 0, 1));
   defaultGeometry->getOrCreateStateSet()
      ->getOrCreateUniform("material_emission", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(1, 1, 1, 1));
   defaultGeometry->getOrCreateStateSet()->setAssociatedModes(smaterial, osg::StateAttribute::ON);

   imageGeode->addDrawable(defaultGeometry);
   return imageGeode;
}
} // namespace

void vespa::VaModelDatabase::Initialize()
{
   osg::ref_ptr<osg::Geode> unknownGeode = ut::osg::make_ref<osg::Geode>();

   osg::ref_ptr<osg::Geometry> defaultGeometry = ut::osg::make_ref<osg::Geometry>();
   osg::ref_ptr<osg::Geometry> outlineGeometry = ut::osg::make_ref<osg::Geometry>();
   // create and bind a new vertex buffer
   osg::ref_ptr<osg::Vec3Array> pVertices = ut::osg::make_ref<osg::Vec3Array>();
   pVertices->setName("model_Vertex");
   defaultGeometry->setVertexAttribArray(0, pVertices);
   defaultGeometry->setVertexAttribBinding(0, osg::Geometry::BIND_PER_VERTEX);
   outlineGeometry->setVertexAttribArray(0, pVertices);
   outlineGeometry->setVertexAttribBinding(0, osg::Geometry::BIND_PER_VERTEX);
   defaultGeometry->getOrCreateStateSet()
      ->getOrCreateUniform("material_ambient", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(1, 1, 1, 1));
   defaultGeometry->getOrCreateStateSet()
      ->getOrCreateUniform("material_diffuse", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(1, 1, 1, 1));
   defaultGeometry->getOrCreateStateSet()
      ->getOrCreateUniform("material_emission", osg::Uniform::FLOAT_VEC4)
      ->set(osg::Vec4(0, 0, 0, 1));

   // obtain the color pointers

   // set the new size of the vertex buffer
   unsigned int nVertSize = 16; // DONE, updated for range division

   pVertices->resize(nVertSize);

   // obtain an iterator to the first vertex
   osg::Vec3Array::iterator itVertex = pVertices->begin();

   osg::ref_ptr<osg::DrawElementsUInt> face    = ut::osg::make_ref<osg::DrawElementsUInt>(GL_POLYGON);
   osg::ref_ptr<osg::DrawElementsUInt> outline = ut::osg::make_ref<osg::DrawElementsUInt>(GL_LINE_LOOP);
   int                                 index   = 0;

   itVertex->set(osg::Vec3(-1.0f, 0.43f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(-0.43f, 1.0f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(0.43f, 1.0f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(1.0f, 0.43f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(1.0f, -0.43f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(0.43f, -1.0f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(-0.43f, -1.0f, 0.0f));
   face->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(-1.0f, -0.43f, 0.0f));
   face->push_back(index++);
   ++itVertex;

   itVertex->set(osg::Vec3(-1.0f, 0.43f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(-0.43f, 1.0f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(0.43f, 1.0f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(1.0f, 0.43f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(1.0f, -0.43f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(0.43f, -1.0f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(-0.43f, -1.0f, -0.1f));
   outline->push_back(index++);
   ++itVertex;
   itVertex->set(osg::Vec3(-1.0f, -0.43f, -0.1f));
   outline->push_back(index++);

   defaultGeometry->addPrimitiveSet(face);
   outlineGeometry->addPrimitiveSet(outline);

   defaultGeometry->dirtyBound();
   outlineGeometry->dirtyBound();

   std::set<std::string> nulldefs;
   unknownGeode->getOrCreateStateSet()->setAttributeAndModes(UtoShaders::UseProgram("icon.vert", "icon.frag", nulldefs));
   unknownGeode->getOrCreateStateSet()->addUniform(ut::osg::make_ref<osg::Uniform>("VA_ScreenScale", 1.0f));

   osg::ref_ptr<osg::Material> smaterial = ut::osg::make_ref<osg::Material>();
   smaterial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
   smaterial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
   defaultGeometry->getOrCreateStateSet()->setAttributeAndModes(smaterial);
   osg::ref_ptr<osg::Material> material = ut::osg::make_ref<osg::Material>();
   material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
   material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
   outlineGeometry->getOrCreateStateSet()->setAttributeAndModes(material);
   outlineGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(2.0f));

   unknownGeode->addDrawable(defaultGeometry);
   unknownGeode->addDrawable(outlineGeometry);
   std::vector<std::string> descVec = unknownGeode->getDescriptions();
   descVec.insert(descVec.begin(), "VA_ScreenScaled");
   unknownGeode->setDescriptions(descVec);

   mModelDefinitionMap["unknown"].SetSharedIconNode("", unknownGeode);
   mModelDefinitionMap["unknown"].SetDefinitionSourceFile("", "");
   mUnknownModelPtr = &mModelDefinitionMap["unknown"];
   VaModelDefinition::SetUnknownModel(mModelDefinitionMap["unknown"]);
}

void vespa::VaModelDatabase::LoadModelSet(const std::string& aFilename, bool aReadOnly, const std::string& aLocationPath)
{
   UtPath root(aLocationPath);
   if (aLocationPath.empty())
   {
      root = aFilename;
      root.Up();
   }

   UtInput aInput;
   try
   {
      auto file    = ut::make_unique<UtInputFile>(aFilename);
      auto filePtr = file.get();
      aInput.PushInput(std::move(file));

      std::string command;
      size_t      offsetStart = filePtr->GetOffset();
      while (aInput.TryReadCommand(command))
      {
         if (command == "model")
         {
            std::string           name;
            std::set<std::string> sets;
            aInput.ReadCommand(name);

            UtStringUtil::ToLower(name);
            mModelDefinitionMap[name].ReadModel(aInput, root, name, sets);
            mAliasMap[name]  = name; // reclaim any aliases for this specific name
            size_t offsetEnd = filePtr->GetOffset();
            for (auto&& set : sets)
            {
               mSetList.insert(set);
               mModelDefinitionMap[name].SetDefinitionSourceFile(set, aFilename);
               mModelDefinitionMap[name].SetDefinitionRange(set, offsetStart, offsetEnd);
               mModelDefinitionMap[name].SetReadOnly(set, aReadOnly);
            }
         }
         else if (command == "alias")
         {
            std::string alias;
            std::string model;
            aInput.ReadCommand(alias);
            aInput.ReadCommand(model);
            UtStringUtil::ToLower(alias);
            mAliasMap[alias] = model;
         }
         else
         {
            auto out = ut::log::error() << "Error reading file, unknown command.";
            out.AddNote() << "File: " << aFilename;
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
            // read error
         }
         offsetStart = filePtr->GetOffset();
      }
   }
   catch (UtInput::ExceptionBase& exception)
   {
      auto out = ut::log::error() << "VaModelDatabase Exception:";
      out.AddNote() << exception.GetMessage();
      out.AddNote() << "Location: " << exception.GetLocation();
   }
   catch (UtInputFile::OpenError&)
   {
      auto out = ut::log::error() << "Unable to open model input file.";
      out.AddNote() << "File: " << aFilename;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception thrown in model input file.";
      out.AddNote() << "File: " << aFilename;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      throw;
   }
}

void vespa::VaModelDatabase::AddCustomModelDefinition(const std::string& aName, const VaModelDefinition& aDefinition)
{
   std::string name = aName;
   UtStringUtil::ToLower(name);
   mModelDefinitionMap[name] = aDefinition;
   mAliasMap[name]           = name;
}

void vespa::VaModelDatabase::ReleaseModel(const std::string& aName, osg::ref_ptr<osg::Node>& aRef, vespa::VaViewer* aViewerPtr)
{
   if (aName == "unknown") // we never release the default model (until shutdown)
   {
      aRef = nullptr;
      return;
   }

   std::string lookup = FindAlias(aName);
   auto        it     = mModelDefinitionMap.find(lookup);

   if (it != mModelDefinitionMap.end())
   {
      it->second.DereferenceSharedResource(GetViewerModelSet(*aViewerPtr));
   }
   else // we probably defaulted or had a unique model
   {
      aRef = nullptr;
      return;
   }
}

osg::Node* vespa::VaModelDatabase::RequestSharedModel(const std::string& aName, vespa::VaViewer* aViewerPtr)
{
   std::string tag = GetViewerModelSet(*aViewerPtr);
   return RequestSharedModel(aName, tag);
}

osg::Node* vespa::VaModelDatabase::RequestSharedModel(const std::string& aName, const std::string& aSet)
{
   std::string lookup = FindAlias(aName);

   auto it = mModelDefinitionMap.find(lookup);

   if (it == mModelDefinitionMap.end())
   {
      it = mModelDefinitionMap.find("unknown");
   }
   assert(it != mModelDefinitionMap.end());
   if (aSet != "")
   {
      osg::Node* n = it->second.RequestSharedModel(aSet);
      if (n)
      {
         return n;
      }
   }
   osg::Node* n = it->second.RequestSharedModel("");
   if (n)
   {
      return n;
   }

   return mUnknownModelPtr->RequestSharedModel("");
}

void vespa::VaModelDatabase::BeginTreatNode(osg::Node* aNodePtr, osg::StateSet* aStateSetPtr)
{
   if (aStateSetPtr->getTextureAttributeList().empty())
   {
      unsigned char* textureData = new unsigned char[1];
      textureData[0]             = 255;

      osg::ref_ptr<osg::Image> image = ut::osg::make_ref<osg::Image>();
      image->setImage(1, 1, 1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, textureData, osg::Image::USE_NEW_DELETE);
      osg::ref_ptr<osg::Texture2D> texture = ut::osg::make_ref<osg::Texture2D>();
      texture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      texture->setImage(image.get());
      aStateSetPtr->setTextureAttributeAndModes(0, texture);
   }

   osgUtil::Optimizer optimizer;
   optimizer.optimize(aNodePtr,
                      osgUtil::Optimizer::INDEX_MESH | osgUtil::Optimizer::VERTEX_PRETRANSFORM |
                         osgUtil::Optimizer::VERTEX_POSTTRANSFORM);
}

void vespa::VaModelDatabase::EndTreatNode(osg::Node*     aNodePtr,
                                          osg::StateSet* aStateSetPtr,
                                          float          aScreenScale,
                                          bool           aBillboard,
                                          bool           aTeamColor)
{
   osg::BoundingSphere sphere = aNodePtr->getBound();

   float scalar = sphere.radius() / aScreenScale;
   // 1. transform the vertices of the model by its pre_xform_matrix
   std::set<std::string>    nulldefs;
   std::vector<std::string> descVec = aNodePtr->getDescriptions();
   descVec.insert(descVec.begin(), "VA_ScreenScaled");
   if (aBillboard)
   {
      osg::Program* prog = UtoShaders::UseProgram("billboard.vert", "model.frag", nulldefs);
      prog->addBindAttribLocation("model_Vertex", 0);
      prog->addBindAttribLocation("model_Normal", 9);
      prog->addBindAttribLocation("model_MultiTexCoord0", 10);
      aStateSetPtr->setAttributeAndModes(prog);
      descVec.emplace_back("VA_Billboarded");
   }
   else
   {
      std::string   type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
      osg::Program* prog = UtoShaders::UseProgram("model" + type + ".vert", "model" + type + ".frag", nulldefs);
      prog->addBindAttribLocation("model_Vertex", 0);
      prog->addBindAttribLocation("model_Normal", 9);
      prog->addBindAttribLocation("model_MultiTexCoord0", 10);
      aStateSetPtr->setAttributeAndModes(prog);
   }
   if (!aTeamColor)
   {
      aStateSetPtr->addUniform(ut::osg::make_ref<osg::Uniform>("VA_SuppressTeamColor", 1.0f));
   }
   aStateSetPtr->addUniform(ut::osg::make_ref<osg::Uniform>("VA_ScreenScale", scalar));
   aNodePtr->setDescriptions(descVec);
}

osg::Node* vespa::VaModelDatabase::RequestUniqueModel(const std::string& aName, const std::string& aTag)
{
   std::string lookup = FindAlias(aName);

   auto it = mModelDefinitionMap.find(lookup);

   if (it == mModelDefinitionMap.end())
   {
      it = mModelDefinitionMap.find("unknown");
   }
   assert(it != mModelDefinitionMap.end());
   // if we have a tag, check it first
   if (!aTag.empty())
   {
      osg::Node* n = it->second.RequestUniqueModel(aTag);
      if (n)
      {
         return n;
      }
   }
   osg::Node* n = it->second.RequestUniqueModel("");
   if (n)
   {
      return n;
   }
   return mUnknownModelPtr->RequestUniqueModel("");
}

osg::Node* vespa::VaModelDatabase::RequestUniqueModel(const std::string& aName, vespa::VaViewer* aViewerPtr)
{
   std::string tag = GetViewerModelSet(*aViewerPtr);
   return RequestUniqueModel(aName, tag);
}

unsigned int vespa::VaModelDatabase::RequestModel(const std::string& aName,
                                                  VaViewer*          aViewerPtr,
                                                  const VaHitEntry&  aHitEntry,
                                                  const VaHitEntry&  aHoverEntry)
{
   std::string  viewerTag = GetViewerModelSet(*aViewerPtr);
   unsigned int id        = mIdAssigner++;
   VaViewer*    viewer    = aViewerPtr;
   if (nullptr == viewer)
   {
      viewer =
         VaEnvironment::Instance().GetFirstViewer(); // hey this means we only work on the first viewer... just another
                                                     // reason to stop letting people set their viewer to 0...
   }
   VaOverlayModelInstantiations& mo     = viewer->GetOrCreateModelInstantiationsOverlay();
   mIdToOverlay[id]                     = &mo;
   const VaModelDefinition* def         = GetModelDefinition(aName);
   bool                     needsUnique = false;
   std::set<std::string>    articulationNames;
   if ((def != nullptr) && (!viewerTag.empty()))
   {
      def->GetArticulations(GetViewerModelSet(*viewer), articulationNames);
      needsUnique = !articulationNames.empty();
   }
   std::string name = aName;
   if (needsUnique)
   {
      name = aName + " " + std::to_string(id); // file defined names won't have spaces, so this should be safe
   }

   if (!mo.HasModel(name))
   {
      osg::Node* node = nullptr;
      if (needsUnique)
      {
         node = RequestUniqueModel(aName, viewerTag);
      }
      else
      {
         node = RequestSharedModel(aName, viewerTag);
      }

      mo.AddModel(name, node, articulationNames);
   }
   mo.CreateInstance(id, name, aHitEntry, aHoverEntry);
   return id;
}

void vespa::VaModelDatabase::ResetModelColor(unsigned int aId, bool aHide)
{
   if (0 != aId)
   {
      auto it = mIdToOverlay.find(aId);
      if (it != mIdToOverlay.end())
      {
         it->second->SetTeamColorUniform(aId, aHide);
      }
   }
}

void vespa::VaModelDatabase::ResetModelLighting(unsigned int aId, bool aShow)
{
   if (0 != aId)
   {
      auto it = mIdToOverlay.find(aId);
      if (it != mIdToOverlay.end())
      {
         if (aShow)
         {
            it->second->SetLightSpecularUniform(aId, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
         }
         else
         {
            it->second->SetLightSpecularUniform(aId, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
         }
      }
   }
}

void vespa::VaModelDatabase::ResetModelBillboard(unsigned int aId, bool aShow)
{
   if (0 != aId)
   {
      auto it = mIdToOverlay.find(aId);
      if (it != mIdToOverlay.end())
      {
         it->second->SetBillboard(aId, aShow);
      }
   }
}

void vespa::VaModelDatabase::UpdateModel(unsigned int       aId,
                                         const osg::Matrix& aModelMatrix,
                                         const osg::Matrix& aViewMatrix,
                                         const osg::Vec4f&  aColor)
{
   if (0 != aId)
   {
      auto it = mIdToOverlay.find(aId);
      if (it != mIdToOverlay.end())
      {
         it->second->UpdateInstance(aId, aModelMatrix, aViewMatrix, aColor);
      }
   }
}


void vespa::VaModelDatabase::ArticulateModel(unsigned int aId, const std::string& aPartName, const osg::Matrix& aPartMatrix)
{
   if (0 != aId)
   {
      if (mIdToOverlay.find(aId) != mIdToOverlay.end())
      {
         mIdToOverlay[aId]->ArticulateModel(aId, aPartName, aPartMatrix);
      }
   }
}

void vespa::VaModelDatabase::ReleaseModel(unsigned int aInstanceId)
{
   if (0 != aInstanceId)
   {
      auto it = mIdToOverlay.find(aInstanceId);
      if (it != mIdToOverlay.end())
      {
         it->second->ReleaseInstance(aInstanceId);
         mIdToOverlay.erase(it);
      }
   }
}

const vespa::VaModelDefinition* vespa::VaModelDatabase::GetModelDefinition(const std::string& aName) const
{
   std::string name = aName;
   UtStringUtil::ToLower(name);

   const auto iter = mModelDefinitionMap.find(name);
   if (iter != mModelDefinitionMap.end())
   {
      return &iter->second;
   }
   else
   {
      // failed to find aName try again with aliases of aName
      auto       alias = FindAlias(name);
      const auto it    = mModelDefinitionMap.find(alias);
      if (it != mModelDefinitionMap.end())
      {
         return &it->second;
      }
   }

   return nullptr;
}

std::pair<std::string, vespa::VaModelDefinition*>
vespa::VaModelDatabase::LoadTemporaryDefinition(const std::string& aDefinition, const UtPath& aRoot)
{
   VaModelDefinition* def  = nullptr;
   std::string        name = "";

   UtInput input;
   input.PushInput(ut::make_unique<UtInputString>(aDefinition));

   std::string command;
   while (input.TryReadCommand(command))
   {
      if (command == "model")
      {
         std::set<std::string> sets;
         def = new VaModelDefinition;
         input.ReadCommand(name);
         UtStringUtil::ToLower(name);
         def->ReadModel(input, aRoot, name, sets);
         for (auto&& set : sets)
         {
            def->SetDefinitionSourceFile(set, aDefinition);
            def->SetDefinitionRange(set, 0, 0); // this indicates the definition is in the source string
            def->SetReadOnly(set, false);
         }
         break;
      }
      else
      {
         throw UtInput::UnknownCommand(input);
         // read error
      }
   }
   if (def)
   {
      def->Load("");
   }
   else
   {
      throw UtInput::BadValue(input);
   }
   return std::make_pair(name, def);
}

void vespa::VaModelDatabase::TreatNode(osg::Node*         aNodePtr,
                                       const osg::Matrix& aPreTransform,
                                       float              aScreenScale,
                                       bool               aBillboard,
                                       bool               aTeamColor)
{
   osg::StateSet* ss = aNodePtr->getOrCreateStateSet();
   BeginTreatNode(aNodePtr, ss);

   std::map<std::string, osg::Node*> nodemap;
   ModelVisitor                      v(aPreTransform, nodemap);
   aNodePtr->accept(v);

   EndTreatNode(aNodePtr, ss, aScreenScale, aBillboard, aTeamColor);
}

void vespa::VaModelDatabase::TreatUniqueNode(osg::Node*                                aNodePtr,
                                             const osg::Matrix&                        aPreTransform,
                                             float                                     aScreenScale,
                                             bool                                      aBillboard,
                                             bool                                      aTeamColor,
                                             const std::map<std::string, std::string>& aDofMap)
{
   osg::StateSet* ss = aNodePtr->getOrCreateStateSet();
   BeginTreatNode(aNodePtr, ss);

   std::map<std::string, osg::Node*> nodeCollector;
   for (auto&& it : aDofMap)
   {
      nodeCollector[it.first] = nullptr;
   }

   ModelVisitor v(aPreTransform, nodeCollector, false);
   aNodePtr->accept(v);

   for (auto&& jt : nodeCollector)
   {
      if (jt.second != nullptr)
      {
         auto&& kt = aDofMap.find(jt.first);
         if (kt != aDofMap.end())
         {
            std::string                        dofname = kt->second;
            osg::Group*                        group   = jt.second->asGroup();
            osg::Geode*                        geode   = jt.second->asGeode();
            osg::ref_ptr<osg::MatrixTransform> dof     = ut::osg::make_ref<osg::MatrixTransform>();
            dof->setName(dofname);
            if ((group != nullptr) && (geode == nullptr))
            {
               unsigned int nc = group->getNumChildren();
               for (unsigned int i = 0; i < nc; ++i)
               {
                  dof->addChild(group->getChild(i));
               }
               group->removeChildren(0, nc);
               group->addChild(dof);
               // put the node under the group
            }
            else
            {
               osg::Group* parent = jt.second->getParent(0);
               dof->addChild(jt.second);
               parent->removeChild(jt.second);
               parent->addChild(dof);
               // put the DOF node above the node
            }
         }
      }
   }

   EndTreatNode(aNodePtr, ss, aScreenScale, aBillboard, aTeamColor);
}

void vespa::VaModelDatabase::Reset()
{
   mModelDefinitionMap.clear();
   Initialize(); // clear all model definitions but unknown
   mAliasMap.clear();
   mSetList.clear();
}

void vespa::VaModelDatabase::ReloadModels()
{
   std::unique_lock<std::mutex> lock(mReloadLock, std::try_to_lock);
   if (lock.owns_lock())
   {
      std::set<unsigned int> visited;
      for (auto&& it : mIdToOverlay)
      {
         if (visited.find(it.second->GetUniqueId()) == visited.end())
         {
            it.second->Reload(this);
            visited.insert(it.second->GetUniqueId());
         }
      }
      VaObserver::ModelsReloaded();
   }
}

void vespa::VaModelDatabase::SetViewerModelSet(VaViewer* aViewerPtr, const std::string& aModelSet)
{
   bool reloadRequired = (mViewerModelSetMap.find(aViewerPtr->GetUniqueId()) != mViewerModelSetMap.end());
   mViewerModelSetMap[aViewerPtr->GetUniqueId()] = aModelSet;
   if (reloadRequired)
   {
      ReloadModels();
   }
}

std::string vespa::VaModelDatabase::GetViewerModelSet(const VaViewer& aViewer)
{
   const auto& mapping = mViewerModelSetMap.find(aViewer.GetUniqueId());
   if (mapping != mViewerModelSetMap.end())
   {
      return mapping->second;
   }
   return "";
}

std::string vespa::VaModelDatabase::FindAlias(const std::string& aInput) const
{
   std::string input = aInput;
   UtStringUtil::ToLower(input);
   auto it = mAliasMap.find(input);
   if (it != mAliasMap.end())
   {
      if (mModelDefinitionMap.find(it->second) != mModelDefinitionMap.end())
      {
         return it->second;
      }
      else
      {
         return FindAlias(it->second);
      }
   }
   return input;
}

std::set<std::string> vespa::VaModelDatabase::GetNameList()
{
   std::set<std::string> namelist;
   for (auto&& it : mModelDefinitionMap)
   {
      namelist.insert(it.first);
   }
   for (auto&& it : mAliasMap)
   {
      namelist.insert(it.first);
   }
   return namelist;
}

vespa::VaModelDefinition::VaModelDefinition(const std::string& aIconFile,
                                            float              aScreenScale,
                                            const osg::Vec3&   aWingTip,
                                            bool               aBillboarded,
                                            bool               aTeamColored)
{
   mModels[""].mFileName    = aIconFile;
   mModels[""].mScreenScale = aScreenScale;
   mModels[""].mWingTip     = aWingTip;
   mModels[""].mBillboard   = aBillboarded;
   mModels[""].mTeamColor   = aTeamColored;
}

void vespa::VaModelDefinition::SetSharedIconNode(const std::string& aTag, osg::Node* aNodePtr)
{
   mModels[aTag].mSharedResource = aNodePtr;
}

void vespa::VaModelDefinition::SetDefinitionSourceFile(const std::string& aTag, const std::string& aPath)
{
   mModels[aTag].mDefinitionSource = aPath;
}

std::string vespa::VaModelDefinition::GetDefinitionSourceFile(const std::string& aTag) const
{
   return GetBestModel(aTag).mDefinitionSource;
}

void vespa::VaModelDefinition::SetDefinitionRange(const std::string& aTag, size_t aRangeLow, size_t aRangeHigh)
{
   mModels[aTag].mDefinitionRange = std::make_pair(aRangeLow, aRangeHigh);
}

std::pair<size_t, size_t> vespa::VaModelDefinition::GetDefinitionRange(const std::string& aTag) const
{
   return GetBestModel(aTag).mDefinitionRange;
}

void vespa::VaModelDefinition::SetReadOnly(const std::string& aTag, bool aReadOnly)
{
   mModels[aTag].mReadOnly = aReadOnly;
}

void vespa::VaModelDefinition::ReadModel(UtInput&               aInput,
                                         const UtPath&          aRoot,
                                         const std::string&     aName,
                                         std::set<std::string>& aSets)
{
   osg::Matrix                        prexform = osg::Matrix::identity();
   std::string                        name;
   std::string                        filename    = "";
   float                              screenScale = 1.0f;
   osg::Vec3                          wingTip(0, 0, 0);
   std::vector<osg::Vec4>             engineList;
   std::string                        command;
   std::string                        category  = "uncategorized";
   bool                               billboard = false;
   bool                               teamcolor = true;
   std::map<std::string, std::string> dofs;
   while (aInput.TryReadCommand(command))
   {
      if (command == "filename")
      {
         aInput.ReadLine(filename, false);
         size_t first = filename.find_first_not_of(" \n\r\t");
         size_t last  = filename.find_last_not_of(" \n\r\t");
         filename     = filename.substr(first, (last - first + 1));
      }
      else if (command == "wing_tip")
      {
         float x;
         float y;
         float z;
         aInput.ReadValue(x);
         aInput.ReadValue(y);
         aInput.ReadValue(z);
         wingTip = {x, y, z};
      }
      else if (command == "engine")
      {
         ReadEngineData(aInput, engineList);
      }
      else if (command == "pre_xform")
      {
         ReadTransformation(aInput, prexform);
      }
      else if (command == "category")
      {
         aInput.ReadValue(category);
         UtStringUtil::ToLower(category);
      }
      else if (command == "screen_scale")
      {
         aInput.ReadValue(screenScale);
      }
      else if (command == "billboard")
      {
         billboard = true;
      }
      else if (command == "noteamcolor")
      {
         teamcolor = false;
      }
      else if (command == "set")
      {
         std::string set;
         aInput.ReadValue(set);
         UtStringUtil::ToLower(set);
         aSets.insert(set);
      }
      else if (command == "default_set")
      {
         aSets.insert("");
      }
      else if (command == "articulation")
      {
         std::string name;
         std::string node;
         aInput.ReadValue(name);
         aInput.ReadValue(node);
         dofs[node] = name;
      }
      else if (command == "end_model")
      {
         if (aSets.empty())
         {
            aSets.insert(""); // assume with sets unspecified it is meant for the default set
         }
         if (!filename.empty())
         {
            bool   found = false;
            UtPath sys(filename);
            if (sys.Exists())
            {
               found = true;
            }
            else
            {
               UtPath rel = aRoot;
               rel += filename;
               if (rel.Exists())
               {
                  filename = rel.GetSystemPath();
                  found    = true;
               }
            }
            if (found)
            {
               for (auto&& set : aSets)
               {
                  mModels[set].mFileName     = filename;
                  mModels[set].mPreTransform = prexform;
                  mModels[set].mScreenScale  = screenScale;
                  mModels[set].mBillboard    = billboard;
                  mModels[set].mTeamColor    = teamcolor;
                  mModels[set].mWingTip      = wingTip;

                  for (const auto& engine : engineList)
                  {
                     mModels[set].mEngines.emplace_back(engine);
                  }

                  mModels[set].mDofMap = dofs;
               }
               mCategory = category;
            }
            else
            {
               auto out = ut::log::warning() << "Could not find model.";
               out.AddNote() << "Model: " << aName;
               out.AddNote() << "File: " << filename;
            }
         }
         else
         {
            auto out = ut::log::warning() << "Model has no filename defined.";
            out.AddNote() << "Model: " << aName;
         }
         return;
      }
      else
      {
         auto out = ut::log::error() << "Error reading model, unknown command.";
         out.AddNote() << "Model: " << aName;
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }
   }
   throw UtInput::EndOfData(aInput);
}

void vespa::VaModelDefinition::ReadEngineData(UtInput& aInput, std::vector<osg::Vec4>& aEngineList)
{
   osg::Vec4   engineData = {0, 0, 0, 0};
   std::string command;
   while (aInput.TryReadCommand(command))
   {
      if (command == "position")
      {
         float x, y, z;
         aInput.ReadValue(x);
         aInput.ReadValue(y);
         aInput.ReadValue(z);
         engineData[0] = x;
         engineData[1] = y;
         engineData[2] = z;
      }
      else if (command == "diameter")
      {
         float w;
         aInput.ReadValue(w);
         engineData[3] = w;
      }
      else if (command == "end_engine")
      {
         aEngineList.emplace_back(engineData);
         return;
      }
      else
      {
         auto out = ut::log::error() << "Error reading engine data; unknown command.";
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

osg::Vec3 vespa::VaModelDefinition::GetWingTip(const std::string& aTag) const
{
   return GetBestModel(aTag).mWingTip;
}

std::vector<osg::Vec4> vespa::VaModelDefinition::GetEngines(const std::string& aTag) const
{
   return GetBestModel(aTag).mEngines;
}

void vespa::VaModelDefinition::GetArticulations(const std::string& aTag, std::set<std::string>& aArticulationSet) const
{
   const VaModelDefinition::Model& mod = GetBestModel(aTag);
   for (auto&& art : mod.mDofMap)
   {
      aArticulationSet.insert(art.second);
   }
}

bool vespa::VaModelDefinition::IsReadOnly(const std::string& aTag) const
{
   return GetBestModel(aTag).mReadOnly;
}

osg::Node* vespa::VaModelDefinition::RequestSharedModel(const std::string& aTag) const
{
   const auto& m = mModels.find(aTag);
   if (m != mModels.end())
   {
      if (m->second.mSharedResource.valid())
      {
         return m->second.mSharedResource.get();
      }
      else
      {
         if (Load(aTag))
         {
            return m->second.mSharedResource.get();
         }
      }
   }
   return nullptr;
}

osg::Node* vespa::VaModelDefinition::RequestUniqueModel(const std::string& aTag) const
{
   const auto& m = mModels.find(aTag);
   if (m != mModels.end())
   {
      if (m->second.mUniqueResource.valid())
      {
         return new osg::MatrixTransform(*m->second.mUniqueResource, osg::CopyOp::DEEP_COPY_ALL);
      }
      else
      {
         if (LoadUnique(aTag))
         {
            return new osg::MatrixTransform(*m->second.mUniqueResource, osg::CopyOp::DEEP_COPY_ALL);
         }
      }
   }
   return nullptr;
}

bool vespa::VaModelDefinition::Load(const std::string& aTag) const
{
   const auto& model = mModels.find(aTag);
   if (model != mModels.end())
   {
      osg::ref_ptr<osg::Object> obj = osgDB::readRefObjectFile(model->second.mFileName);

      osg::ref_ptr<osg::Node> node;
      if (obj == nullptr) // why does everything seem to work fine except of "obj" files?
      {
         node = osgDB::readRefNodeFile(model->second.mFileName);
      }
      else
      {
         node = dynamic_cast<osg::Node*>(obj.get());
      }
      if (nullptr == node)
      {
         // maybe it is an image
         auto image = dynamic_cast<osg::Image*>(obj.get());
         if (nullptr != image)
         {
            node = BuildImageNode(image);
         }
         else
         {
            model->second.mFailedToLoad = true;
            return false;
         }
      }
      if (nullptr != node)
      {
         VaModelDatabase::TreatNode(node,
                                    model->second.mPreTransform,
                                    model->second.mScreenScale,
                                    model->second.mBillboard,
                                    model->second.mTeamColor);

         model->second.mSharedResource = node;
      }
      return true;
   }
   return false;
}

bool vespa::VaModelDefinition::LoadUnique(const std::string& aTag) const
{
   const auto& model = mModels.find(aTag);
   if (model != mModels.end())
   {
      osg::ref_ptr<osg::Object> obj = osgDB::readRefObjectFile(model->second.mFileName);

      osg::ref_ptr<osg::Node> node = dynamic_cast<osg::Node*>(obj.get());

      if (nullptr == node)
      {
         // maybe it is an image
         auto image = dynamic_cast<osg::Image*>(obj.get());
         if (nullptr != image)
         {
            node = BuildImageNode(image);
         }
         else
         {
            model->second.mFailedToLoad = true;
            return false;
         }
      }
      if (nullptr != node)
      {
         model->second.mUniqueResource = new osg::MatrixTransform();
         model->second.mUniqueResource->addChild(node);
         VaModelDatabase::TreatUniqueNode(model->second.mUniqueResource,
                                          model->second.mPreTransform,
                                          model->second.mScreenScale,
                                          model->second.mBillboard,
                                          model->second.mTeamColor,
                                          model->second.mDofMap);
      }
      return true;
   }
   return false;
}

float vespa::VaModelDefinition::GetModelScale(const std::string& aTag) const
{
   osg::Node* node = RequestSharedModel(aTag);
   if (node)
   {
      return node->getBound().radius();
   }
   return 1.0f;
}

void vespa::VaModelDefinition::SetUnknownModel(VaModelDefinition& aModelDef)
{
   msUnknownModelPtr = &aModelDef.mModels[""];
}

std::set<std::string> vespa::VaModelDefinition::GetSets()
{
   std::set<std::string> set;
   for (auto&& m : mModels)
   {
      set.insert(m.first);
   }
   return set;
}

const vespa::VaModelDefinition::Model& vespa::VaModelDefinition::GetBestModel(const std::string& aTag) const
{
   const auto& m = mModels.find(aTag);
   if (m != mModels.end())
   {
      if (!m->second.mFailedToLoad)
      {
         return m->second;
      }
   }
   const auto& defaultm = mModels.find("");
   if (defaultm != mModels.end())
   {
      if (!defaultm->second.mFailedToLoad)
      {
         return defaultm->second;
      }
   }
   return *msUnknownModelPtr;
}

void vespa::VaModelDefinition::DereferenceSharedResource(const std::string& aTag)
{
   const auto& m = mModels.find(aTag);
   if (m != mModels.end())
   {
      if (m->second.mSharedResource.valid())
      {
         if (m->second.mSharedResource->referenceCount() == 1)
         {
            m->second.mSharedResource = nullptr;
            return;
         }
      }
   }
   const auto& defaultm = mModels.find("");
   if (defaultm != mModels.end())
   {
      if (defaultm->second.mSharedResource.valid())
      {
         if (defaultm->second.mSharedResource->referenceCount() == 1)
         {
            defaultm->second.mSharedResource = nullptr;
            return;
         }
      }
   }
}
