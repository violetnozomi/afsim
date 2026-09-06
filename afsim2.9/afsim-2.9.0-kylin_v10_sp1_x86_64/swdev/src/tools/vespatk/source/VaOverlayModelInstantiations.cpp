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

#include "VaOverlayModelInstantiations.hpp"

#include <sstream>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/LightModel>
#include <osg/LineWidth>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Texture2D>
#include <osg/Transform>
#include <osg/Uniform>
#include <osgUtil/IntersectionVisitor>

#include "UtLog.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "UtoViewer.hpp"
#include "VaModelDatabase.hpp"
#include "VaViewer.hpp"

// this may need to be adjusted (if we use over 24 scalars of uniform data in our shaders)
// it may also need to be dynamic, right now we are assuming 1024 scalars of uniform data available
// we are using 20 scalars per instance and reserving 24 scalars for other purposes
#define VA_MAX_NUM_HW_INSTANCES 50

namespace vespa
{
class VaVBO_Visitor : public osg::NodeVisitor
{
public:
   VaVBO_Visitor(std::vector<osg::Geometry*>& aGeometryVector, std::map<std::string, osg::MatrixTransform*>& aDofMap)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , mGeometryVector(aGeometryVector)
      , mDofMap(aDofMap)
   {
   }
   void apply(osg::MatrixTransform& aTransform) override
   {
      auto dof = mDofMap.find(aTransform.getName());
      if (dof != mDofMap.end())
      {
         dof->second = &aTransform;
      }
      traverse(aTransform);
   }
   void apply(osg::Geometry& aGeometry) override
   {
      aGeometry.setUseDisplayList(false);
      aGeometry.setUseVertexBufferObjects(true);
      aGeometry.setCullingActive(false);
      aGeometry.dirtyBound();
      mGeometryVector.push_back(&aGeometry);
   }

private:
   std::vector<osg::Geometry*>&                  mGeometryVector;
   std::map<std::string, osg::MatrixTransform*>& mDofMap;
};

VaOverlayModelInstantiations::VaOverlayModelInstantiations()
   : VaOverlay("VaOverlayModelInstantiations", cOVERLAY_WORLD)
   , mShapePtr(nullptr)
   , mShapeName("")
{
   SetType<VaOverlayModelInstantiations>();
}


VaOverlayModelInstantiations::VaOverlayModelInstantiations(const std::string& aName)
   : VaOverlay(aName, cOVERLAY_WORLD)
   , mShapePtr(nullptr)
   , mShapeName("")
{
   SetType<VaOverlayModelInstantiations>();
}

VaOverlayModelInstantiations* VaOverlayModelInstantiations::Clone() const
{
   return new VaOverlayModelInstantiations(*this);
}

bool VaOverlayModelInstantiations::Initialize()
{
   bool initialized = false;

   std::ostringstream  oss;
   static unsigned int uid = 0;
   oss << "VaOverlayModelInstantiations"
       << "_" << uid++;
   mShapeName = oss.str().c_str();

   UtoRawShape shape;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape(mShapeName, shape));
   if (mShapePtr == nullptr)
   {
      ut::log::warning() << "Could not bind shape: model_instantiations";
   }

   if (mShapePtr != nullptr)
   {
      SetVisible(true);
      initialized = true;
      mShapePtr->SetCullingActive(false);
   }
   osg::StateSet* ss = mShapePtr->GetRoot()->getOrCreateStateSet();
   ss->setAttributeAndModes(new osg::LineWidth(1.0));
   ss->setAttributeAndModes(new osg::BlendFunc());
   ss->setAttributeAndModes(new osg::Depth(), osg::StateAttribute::ON);
   osg::Image*    img     = new osg::Image();
   unsigned char* texData = new unsigned char[1];
   texData[0]             = 255;
   img->setImage(1, 1, 1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, texData, osg::Image::USE_NEW_DELETE);
   ss->setTextureAttributeAndModes(0, new osg::Texture2D(img));
   ss->setRenderBinDetails(2, "RenderBin");

   return initialized;
}

VaOverlayModelInstantiations::~VaOverlayModelInstantiations()
{
   for (auto&& it : mModelMap)
   {
      for (auto&& jt : it.second)
      {
         ModelBatch* batch = jt;
         delete batch;
      }
      it.second.clear();
   }
   UnbindShape(mShapeName);
}

bool VaOverlayModelInstantiations::HasModel(const std::string& aName) const
{
   return (mModelMap.find(aName) != mModelMap.end());
}

void VaOverlayModelInstantiations::AddModel(const std::string&           aName,
                                            osg::Node*                   aModelNode,
                                            const std::set<std::string>& aDofList)
{
   VaOverlayModelInstantiations::ModelBatch* batch = new VaOverlayModelInstantiations::ModelBatch;
   batch->mModel = dynamic_cast<osg::Node*>(aModelNode->clone(osg::CopyOp::DEEP_COPY_ALL));
   for (auto& dofName : aDofList)
   {
      batch->mDofList[dofName] = nullptr;
   }
   VaVBO_Visitor v(batch->mGeometryList, batch->mDofList);
   batch->mModel->accept(v);
   std::set<std::string> nulldefs;
   std::string           type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
   if ((aModelNode->getNumDescriptions() > 1) && (aModelNode->getDescription(1) == "VA_Billboarded"))
   {
      osg::Program* prog = UtoShaders::UseProgram("billboard" + type + ".vert", "modelinst" + type + ".frag", nulldefs);
      prog->addBindAttribLocation("model_Vertex", 0);
      prog->addBindAttribLocation("model_Normal", 9);
      prog->addBindAttribLocation("model_MultiTexCoord0", 10);
      batch->mModel->getOrCreateStateSet()->setAttributeAndModes(prog);
   }
   else
   {
      if (!aDofList.empty())
      {
         osg::Program* prog = UtoShaders::UseProgram("model" + type + ".vert", "model" + type + ".frag", nulldefs);
         prog->addBindAttribLocation("model_Vertex", 0);
         prog->addBindAttribLocation("model_Normal", 9);
         prog->addBindAttribLocation("model_MultiTexCoord0", 10);
         batch->mModel->getOrCreateStateSet()->setAttributeAndModes(prog);
      }
      else
      {
         osg::Program* prog = UtoShaders::UseProgram("modelinst" + type + ".vert", "modelinst" + type + ".frag", nulldefs);
         prog->addBindAttribLocation("model_Vertex", 0);
         prog->addBindAttribLocation("model_Normal", 9);
         prog->addBindAttribLocation("model_MultiTexCoord0", 10);
         batch->mModel->getOrCreateStateSet()->setAttributeAndModes(prog);
      }
   }

   mShapePtr->GetRoot()->addChild(batch->mModel.get());
   batch->mModelMatrixUniformArray =
      new osg::Uniform(osg::Uniform::FLOAT_MAT4, "instanceModelMatrix", VA_MAX_NUM_HW_INSTANCES);
   batch->mColorUniformArray = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "instanceTeamColor", VA_MAX_NUM_HW_INSTANCES);
   batch->mModel->getOrCreateStateSet()->addUniform(batch->mModelMatrixUniformArray);
   batch->mModel->getOrCreateStateSet()->addUniform(batch->mColorUniformArray);
   batch->mInstances = 0;
   batch->mIsUnique  = !aDofList.empty();
   mModelMap[aName].push_back(batch);
}

VaOverlayModelInstantiations::ModelBatch* VaOverlayModelInstantiations::AddBatch(const std::string& aName)
{
   auto batch = new VaOverlayModelInstantiations::ModelBatch;
   mModelMap[aName].push_back(batch);
   batch->mModel = static_cast<osg::Node*>(mModelMap[aName][0]->mModel->clone(osg::CopyOp::DEEP_COPY_ALL));
   std::map<std::string, osg::MatrixTransform*> emptyMap;
   VaVBO_Visitor                                v(batch->mGeometryList, emptyMap);
   batch->mModel->accept(v);
   mShapePtr->GetRoot()->addChild(batch->mModel.get());
   batch->mModelMatrixUniformArray =
      new osg::Uniform(osg::Uniform::FLOAT_MAT4, "instanceModelMatrix", VA_MAX_NUM_HW_INSTANCES);
   batch->mColorUniformArray = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "instanceTeamColor", VA_MAX_NUM_HW_INSTANCES);
   osg::StateSet* ss         = batch->mModel->getOrCreateStateSet();
   ss->addUniform(batch->mModelMatrixUniformArray);
   ss->addUniform(batch->mColorUniformArray);
   batch->mInstances = 0;
   return batch;
}

void VaOverlayModelInstantiations::CreateInstance(unsigned int             aId,
                                                  const std::string&       aName,
                                                  const vespa::VaHitEntry& aHitEntry,
                                                  const vespa::VaHitEntry& aHoverEntry)
{
   ModelBatch* batch = nullptr;
   for (auto b : mModelMap[aName])
   {
      if (b->mInstances < VA_MAX_NUM_HW_INSTANCES)
      {
         batch = b;
         break;
      }
   }
   if (!batch)
   {
      batch = AddBatch(aName);
   }
   if (batch)
   {
      batch->mInstances++;
      for (auto&& it : batch->mGeometryList)
      {
         it->setNodeMask(0xFF);
         for (unsigned int i = 0; i < it->getNumPrimitiveSets(); ++i)
         {
            it->getPrimitiveSet(i)->setNumInstances(batch->mInstances);
         }
      }

      InstanceHolder holder;
      holder.mInstanceId = batch->mInstances - 1;
      holder.mBatchPtr   = batch;
      batch->mHits.push_back(aHitEntry);
      batch->mHovers.push_back(aHoverEntry);
      batch->mIds.push_back(aId);
      mIdToInstance[aId] = holder;
   }
}

void VaOverlayModelInstantiations::ReleaseInstance(unsigned int aId)
{
   auto         instance   = mIdToInstance.find(aId);
   unsigned int instanceId = instance->second.mInstanceId;
   ModelBatch*  batch      = instance->second.mBatchPtr;

   for (unsigned int i = instanceId; i < batch->mHits.size() - 1; ++i)
   {
      osg::Vec4 color;
      batch->mColorUniformArray->getElement(i + 1, color);
      batch->mColorUniformArray->setElement(i, color);
      osg::Matrix mat;
      batch->mModelMatrixUniformArray->getElement(i + 1, mat);
      batch->mModelMatrixUniformArray->setElement(i, mat);
      batch->mHits[i]   = batch->mHits[i + 1];
      batch->mHovers[i] = batch->mHovers[i + 1];
   }                    // shift our uniforms to the left
   batch->mInstances--; // inform the batch of the new instance count
   batch->mHits.resize(batch->mInstances);
   batch->mHovers.resize(batch->mInstances);

   for (auto&& it : batch->mGeometryList)
   {
      if (batch->mInstances == 0)
      {
         it->setNodeMask(0x00);
      }
      else
      {
         for (unsigned int i = 0; i < it->getNumPrimitiveSets(); ++i)
         {
            it->getPrimitiveSet(i)->setNumInstances(batch->mInstances);
            // inform the geometry of the new instance count
         }
      }
   }
   for (unsigned int i = instanceId + 1; i < batch->mIds.size(); ++i)
   {
      int id = batch->mIds[i];
      mIdToInstance[id].mInstanceId--;
      batch->mIds[i - 1] = batch->mIds[i];
   }
   batch->mIds.resize(batch->mInstances);
   // TODOTODOTODO inform the other instances in the batch of their new instance ids
   mIdToInstance.erase(aId); // remove the entry from mIdToInstance
}

void VaOverlayModelInstantiations::UpdateInstance(unsigned int       aId,
                                                  const osg::Matrix& aModelMat,
                                                  const osg::Matrix& aViewMat,
                                                  const osg::Vec4&   aColor)
{
   auto it = mIdToInstance.find(aId);
   if (it != mIdToInstance.end())
   {
      it->second.mBatchPtr->mModelMatrixUniformArray->setElement(it->second.mInstanceId, aModelMat * aViewMat);
      it->second.mBatchPtr->mColorUniformArray->setElement(it->second.mInstanceId, aColor);
      if (it->second.mBatchPtr->mIsUnique)
      {
         osg::MatrixTransform* mtrans = dynamic_cast<osg::MatrixTransform*>(it->second.mBatchPtr->mModel.get());
         if (mtrans)
         {
            mtrans->getOrCreateStateSet()->getOrCreateUniform("VA_TeamColor", osg::Uniform::FLOAT_VEC4)->set(aColor);
            mtrans->setMatrix(aModelMat);
         }
      }
   }
}

void VaOverlayModelInstantiations::ArticulateModel(unsigned int aId, const std::string& aPartName, const osg::Matrix& aPartMatrix)
{
   auto it = mIdToInstance.find(aId);
   if (it != mIdToInstance.end())
   {
      auto part = it->second.mBatchPtr->mDofList.find(aPartName);
      if ((part != it->second.mBatchPtr->mDofList.end()) && (part->second != nullptr))
      {
         part->second->setMatrix(aPartMatrix);
      }
   }
}

void VaOverlayModelInstantiations::SetTeamColorUniform(unsigned int aId, bool aHide)
{
   auto it = mIdToInstance.find(aId);
   if (it != mIdToInstance.end())
   {
      osg::StateSet* ss = it->second.mBatchPtr->mModel->getOrCreateStateSet();
      ss->getOrCreateUniform("VA_SuppressTeamColor", osg::Uniform::FLOAT)->set(aHide ? 1.0f : 0.0f);
   }
}

void VaOverlayModelInstantiations::SetLightSpecularUniform(unsigned int aId, const osg::Vec4& aUniform)
{
   auto it = mIdToInstance.find(aId);
   if (it != mIdToInstance.end())
   {
      osg::StateSet* ss = it->second.mBatchPtr->mModel->getOrCreateStateSet();
      ss->getOrCreateUniform("uto_LightSourceSpecular", osg::Uniform::FLOAT_VEC4)->set(aUniform);
   }
}

void VaOverlayModelInstantiations::SetBillboard(unsigned int aId, bool aUse)
{
   std::set<std::string> nulldefs;
   auto                  it = mIdToInstance.find(aId);
   if (it != mIdToInstance.end())
   {
      osg::StateSet* ss   = it->second.mBatchPtr->mModel->getOrCreateStateSet();
      std::string    type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
      if (aUse)
      {
         osg::Program* prog = UtoShaders::UseProgram("billboard" + type + ".vert", "modelinst" + type + ".frag", nulldefs);
         prog->addBindAttribLocation("model_Vertex", 0);
         prog->addBindAttribLocation("model_Normal", 9);
         prog->addBindAttribLocation("model_MultiTexCoord0", 10);
         ss->setAttributeAndModes(prog);
      }
      else
      {
         std::string type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
         osg::Program* prog = UtoShaders::UseProgram("modelinst" + type + ".vert", "modelinst" + type + ".frag", nulldefs);
         prog->addBindAttribLocation("model_Vertex", 0);
         prog->addBindAttribLocation("model_Normal", 9);
         prog->addBindAttribLocation("model_MultiTexCoord0", 10);
         ss->setAttributeAndModes(prog);
      }
   }
}

void VaOverlayModelInstantiations::Intersect(VaHitList&         aHitList,
                                             int                aMouseX,
                                             int                aMouseY,
                                             const osg::Matrix& aView,
                                             const osg::Matrix& aProjection,
                                             int                aVP_Width,
                                             int                aVP_Height,
                                             bool               aScaled,
                                             float              aIconScalar,
                                             bool               aHover)
{
   float orthoScalar = 1.0f;
   bool  ortho       = false;
   if (GetViewer()->GetMapProjection().projection != 0)
   {
      orthoScalar = 1.0f / GetViewer()->GetViewer()->GetOrtho()->Zoom() / aIconScalar * 0.1;
      ortho       = true;
   }
   for (auto&& it : mModelMap)
   {
      for (auto&& jt : it.second)
      {
         for (unsigned int i = 0; i < jt->mInstances; ++i)
         {
            osg::Matrix mv;
            jt->mModelMatrixUniformArray->getElement(i, mv);
            osg::Matrix mvp         = mv * aProjection;
            osg::Vec4   modelOrigin = osg::Vec4(0, 0, 0, 1) * mv * aProjection;
            if (modelOrigin.z() / modelOrigin.w() > -1) // if it is infront of the camera
            {
               // this commented block is a start at blocking selection / hover of things behind the earth.  Still work
               // to be done here
               /*               osg::Vec4 sceneOrigin = osg::Vec4(0, 0, 0, 1) * aView * aProjection;
                              if (sceneOrigin.z() / sceneOrigin.w() > -1) // if the world is infront of the camera
                              {
                                 if (!aHover)
                                 {
                                    int i = 0;
                                 }
                                 // if the depth of the world is less than the depth of the model
                                 if (sceneOrigin.z() / sceneOrigin.w() < modelOrigin.z() / modelOrigin.w())
                                 {
                                    break; // skip this one
                                 }
                              }*/
               float screenScalarUniform;
               jt->mModel->getStateSet()->getUniform("VA_ScreenScale")->get(screenScalarUniform);
               // mix(min(UTO_Viewport.w / zero.w / VA_IconScale * VA_ScreenScale * 0.1, 1.0), 1.0 /
               // UTO_OrthographicZoom / VA_IconScale * VA_ScreenScale * 0.1, UTO_OrthographicMode);
               float scalar;
               if (ortho)
               {
                  scalar = orthoScalar * screenScalarUniform;
               }
               else
               {
                  scalar = std::min(aVP_Height / modelOrigin.w() / aIconScalar * screenScalarUniform * 0.1, 1.0);
               }
               float     modelX = (modelOrigin.x() / modelOrigin.w() + 1) * aVP_Width * 0.5;
               float     modelY = (modelOrigin.y() / modelOrigin.w() + 1) * aVP_Height * 0.5;
               float     dist = sqrt((aMouseX - modelX) * (aMouseX - modelX) + (aMouseY - modelY) * (aMouseY - modelY));
               osg::Vec4 unit(1, 0, 0, 0);
               unit             = unit * mv;
               float unitLength = unit.length();

               if (ortho)
               {
                  if (dist < aIconScalar * 10)
                  {
                     if (aHover)
                     {
                        aHitList.push_back(jt->mHovers[i]);
                     }
                     else
                     {
                        aHitList.push_back(jt->mHits[i]);
                     }
                  }
               }
               else if (aScaled && (scalar < 1))
               {
                  if (dist < aIconScalar * unitLength * 16) // screenScalarUniform doesn't work... needs to be split
                                                            // between model to screen and user defined scalar
                  {
                     if (aHover)
                     {
                        aHitList.push_back(jt->mHovers[i]);
                     }
                     else
                     {
                        aHitList.push_back(jt->mHits[i]);
                     }
                  }
               }
               else
               {
                  if (dist < aIconScalar * unitLength * screenScalarUniform * aVP_Height / modelOrigin.w() *
                                1.6) // screenScalarUniform doesn't work... needs to be split between model to screen
                                     // and user defined scalar
                  {
                     if (aHover)
                     {
                        aHitList.push_back(jt->mHovers[i]);
                     }
                     else
                     {
                        aHitList.push_back(jt->mHits[i]);
                     }
                  }
               }
            }
         }
      }
   }
}

void VaOverlayModelInstantiations::Reload(vespa::VaModelDatabase* aDatabase)
{
   mShapePtr->GetRoot()->removeChildren(0, mShapePtr->GetRoot()->getNumChildren());
   for (auto&& it : mModelMap)
   {
      for (auto&& jt : it.second)
      {
         osg::Node* originNode = nullptr;
         if (jt->mIsUnique)
         {
            std::string sharedName = it.first.substr(0, it.first.find(" "));
            originNode             = aDatabase->RequestUniqueModel(sharedName, GetViewer());
         }
         else
         {
            originNode = aDatabase->RequestSharedModel(it.first, GetViewer());
         }
         osg::Node* newNode = dynamic_cast<osg::Node*>(originNode->clone(osg::CopyOp::DEEP_COPY_ALL));
         jt->mGeometryList.clear();
         for (auto&& d : jt->mDofList)
         {
            d.second = nullptr;
         }
         VaVBO_Visitor v(jt->mGeometryList, jt->mDofList);
         newNode->accept(v);
         std::set<std::string> nulldefs;
         std::string           type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
         if ((originNode->getNumDescriptions() > 1) && (originNode->getDescription(1) == "VA_Billboarded"))
         {
            osg::Program* prog =
               UtoShaders::UseProgram("billboard" + type + ".vert", "modelinst" + type + ".frag", nulldefs);
            prog->addBindAttribLocation("model_Vertex", 0);
            prog->addBindAttribLocation("model_Normal", 9);
            prog->addBindAttribLocation("model_MultiTexCoord0", 10);
            newNode->getOrCreateStateSet()->setAttributeAndModes(prog);
         }
         else
         {
            if (jt->mIsUnique)
            {
               osg::Program* prog = UtoShaders::UseProgram("model" + type + ".vert", "model" + type + ".frag", nulldefs);
               prog->addBindAttribLocation("model_Vertex", 0);
               prog->addBindAttribLocation("model_Normal", 9);
               prog->addBindAttribLocation("model_MultiTexCoord0", 10);
               newNode->getOrCreateStateSet()->setAttributeAndModes(prog);
            }
            else
            {
               osg::Program* prog =
                  UtoShaders::UseProgram("modelinst" + type + ".vert", "modelinst" + type + ".frag", nulldefs);
               prog->addBindAttribLocation("model_Vertex", 0);
               prog->addBindAttribLocation("model_Normal", 9);
               prog->addBindAttribLocation("model_MultiTexCoord0", 10);
               newNode->getOrCreateStateSet()->setAttributeAndModes(prog);
            }
         }
         jt->mModel = nullptr;
         jt->mModel = newNode;
         mShapePtr->GetRoot()->addChild(newNode);
         jt->mModel->getOrCreateStateSet()->addUniform(jt->mModelMatrixUniformArray);
         jt->mModel->getOrCreateStateSet()->addUniform(jt->mColorUniformArray);
         for (auto&& kt : jt->mGeometryList)
         {
            if (jt->mInstances == 0)
            {
               kt->setNodeMask(0x00);
            }
            else
            {
               for (unsigned int i = 0; i < kt->getNumPrimitiveSets(); ++i)
               {
                  kt->getPrimitiveSet(i)->setNumInstances(jt->mInstances);
                  // inform the geometry of the new instance count
               }
            }
         }
      }
   }
}

} // namespace vespa
