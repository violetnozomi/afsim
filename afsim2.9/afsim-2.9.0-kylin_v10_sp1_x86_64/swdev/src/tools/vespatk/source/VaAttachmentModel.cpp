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

#include "VaAttachmentModel.hpp"

#include "UtoEntity.hpp"
#include "UtoShaders.hpp"
#include "UtoViewer.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaModelDatabase.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{

VaAttachmentModel::VaAttachmentModel(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName)
   : VaAttachment(aParent, aViewerPtr, aName)
   , mModelId(0)
   , mTeamColor(1.0f, 1.0f, 1.0f, 1.0f)
#ifndef VAHWINST
   , mDB_ModelRefPtr(0)
#else
   , mHighlightNode(nullptr)
#endif
   , mIconName()
   , mModelScale(1.0f)
   , mIgnoreTeamColorUniform(false)
   , mUseTeamColorUniform(false)
   , mIgnoreLightSpecularUniform(false)
   , mUseLightSpecularUniform(false)
   , mIgnoreBillboard(false)
   , mUseBillboard(false)
{
   VaObject::SetName(GetName() + "-" + GetUniqueIdString());
   SetType<VaAttachmentModel>();
#ifdef VAHWINST
   mTransformNode = new osg::MatrixTransform;
#endif
}

// virtual
VaAttachmentModel::~VaAttachmentModel()
{
#ifndef VAHWINST
   if (mDB_ModelRefPtr.valid())
   {
      mShapePtr->GetRoot()->removeChild(0, 1);
      VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mIconName, mDB_ModelRefPtr, mViewerPtr);
   }
#else
   if (mHighlightNode.valid())
   {
      // make a copy since calls to highlight will be invalidate iterators to mHighlightColorStack
      auto highlightColorsCopy = mHighlightColorStack;
      for (const auto& highlightColor : highlightColorsCopy)
      {
         VaAttachmentModel::Highlight(GetViewer(), 0, false, highlightColor);
      }
   }
   mTransformNode = nullptr;

   if (0 != mModelId)
   {
      VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mModelId);
   }
#endif
}

bool VaAttachmentModel::PrivateInitialize()
{
#ifndef VAHWINST
   UtoRawShape rawShape;
   mShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(GetName(), rawShape)); // todo, include a name or id
   mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
   mShapePtr->GetRoot()->getOrCreateStateSet()->addUniform(new osg::Uniform("VA_TeamColor", mTeamColor));
   if (!mIconName.empty())
   {
      SetIconPrivate(mIconName);
   }
#else
   if (!mIconName.empty())
   {
      SetIconPrivate(mIconName);
   }
   mCallbacks.Add(VaObserver::ViewMatrixChanged.Connect(&VaAttachmentModel::ViewMatrixChanged, this));
   mCallbacks.Add(VaObserver::EntityMoved.Connect(&VaAttachmentModel::EntityMovedCB, this, GetParent().GetUniqueId()));
   mCallbacks.Add(VaObserver::MapProjectionChanged.Connect(&VaAttachmentModel::MapProjectionChangedCB, this));
   mModelMatrix.makeIdentity();
   mViewMatrix.makeIdentity();
   mScaleMatrix.makeIdentity();
#endif
   return true;
}

void VaAttachmentModel::SetIcon(const std::string& aName)
{
#ifdef VAHWINST
   bool restoreHighlight = false;

   auto highlightColorsCopy = mHighlightColorStack;
   if (mHighlightNode.valid())
   {
      restoreHighlight = true;
      for (const auto& highlightColor : highlightColorsCopy)
      {
         Highlight(GetViewer(), 0, false, highlightColor);
      }
      // restore the stack of colors, that was just removed the color stack, due to calls to disable highlight.
   }
#endif
   if (IsInitialized())
   {
      SetIconPrivate(aName);
   }
#ifdef VAHWINST
   mIconName = aName;
   if (restoreHighlight)
   {
      for (const auto& highlightColor : highlightColorsCopy)
      {
         Highlight(GetViewer(), 0, true, highlightColor);
      }
   }
#endif
}

void VaAttachmentModel::UseTeamColorUniform(bool aShow)
{
   // A delay is necessary because an instance id is required to change uniforms, but the id might not be assigned until later
   if (aShow)
   {
      mUseTeamColorUniform = true;
   }
   else
   {
      mIgnoreTeamColorUniform = true;
   }
}

void VaAttachmentModel::UseLightSpecularUniform(bool aUse)
{
   // A delay is necessary because an instance id is required to change uniforms, but the id might not be assigned until later
   if (aUse)
   {
      mUseLightSpecularUniform = true;
   }
   else
   {
      mIgnoreLightSpecularUniform = true;
   }
}

void VaAttachmentModel::UseBillboard(bool aUse)
{
   if (aUse)
   {
      mUseBillboard = true;
   }
   else
   {
      mIgnoreBillboard = true;
   }
}

void VaAttachmentModel::SetScale(const float aScale)
{
   mScaleMatrix = mScaleMatrix.scale(aScale, aScale, aScale);
   if (0 != mModelId)
   {
      VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId, mScaleMatrix * mModelMatrix, mViewMatrix, mTeamColor);
      mTransformNode->setMatrix(mScaleMatrix * mModelMatrix);
   }
}

void VaAttachmentModel::SetColor(const UtColor& aColor)
{
   mTeamColor = osg::Vec4(aColor[0], aColor[1], aColor[2], aColor[3]);
#ifndef VAHWINST
   if (IsInitialized())
   {
      mShapePtr->GetRoot()->getOrCreateStateSet()->addUniform(new osg::Uniform("VA_TeamColor", mTeamColor));
   }
#else
   if (0 != mModelId)
   {
      VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId, mScaleMatrix * mModelMatrix, mViewMatrix, mTeamColor);
   }
#endif
}

#ifdef VAHWINST
void VaAttachmentModel::UpdateFrame(double aTime)
{
   if (0 == mModelId)
   {
      if (IsVisible())
      {
         VaHitEntry hit;
         VaHitEntry hover;
         switch (GetPickMode())
         {
         case (cPICK_ENTITY):
            hit = VaHitEntry::FromEntity(GetParent().GetUniqueId());
            break;
         case (cPICK_ATTACHMENT):
            hit = VaHitEntry::FromAttachment(*this);
            break;
         default:
            break;
         }
         switch (GetHoverMode())
         {
         case (cPICK_ENTITY):
            hover = VaHitEntry::FromEntity(GetParent().GetUniqueId());
            break;
         case (cPICK_ATTACHMENT):
            hover = VaHitEntry::FromAttachment(*this);
            break;
         default:
            break;
         }
         mModelId = VaEnvironment::Instance().GetModelDatabase()->RequestModel(mIconName, GetViewer(), hit, hover);
         const VaModelDefinition* def = VaEnvironment::Instance().GetModelDatabase()->GetModelDefinition(mIconName);
         if (def)
         {
            mModelScale =
               def->GetModelScale(VaEnvironment::Instance().GetModelDatabase()->GetViewerModelSet(*GetViewer()));
            VaObserver::ModelScaleChanged[GetUniqueId()](mModelScale);
         }
         else
         {
            mModelScale = 1.0f;
            VaObserver::ModelScaleChanged[GetUniqueId()](mModelScale);
         }

         VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId,
                                                                   mScaleMatrix * mModelMatrix,
                                                                   mViewMatrix,
                                                                   mTeamColor);
         mViewMatrix = GetViewer()->GetViewMatrix();
         EntityMovedCB(&GetParent(), true);

         if (mRelativeToParentTransform.valid() && mHighlightNode.valid() && !mTransformNode->containsNode(mHighlightNode))
         {
            // The sub model became visible again while the parent is highlighted, therefore re-enable
            // highlighting if it is not already enabled by adding the node to highlight back into the
            // transform node.
            mTransformNode->addChild(mHighlightNode);
         }
      }
   }
   if (0 != mModelId)
   {
      if (mIgnoreTeamColorUniform)
      {
         VaEnvironment::Instance().GetModelDatabase()->ResetModelColor(mModelId, true);
         VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId,
                                                                   mScaleMatrix * mModelMatrix,
                                                                   mViewMatrix,
                                                                   mTeamColor);
         mIgnoreTeamColorUniform = false;
      }
      if (mUseTeamColorUniform)
      {
         VaEnvironment::Instance().GetModelDatabase()->ResetModelColor(mModelId, false);
         VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId,
                                                                   mScaleMatrix * mModelMatrix,
                                                                   mViewMatrix,
                                                                   mTeamColor);
         mUseTeamColorUniform = false;
      }
      if (mUseLightSpecularUniform)
      {
         VaEnvironment::Instance().GetModelDatabase()->ResetModelLighting(mModelId, true);
         mUseLightSpecularUniform = false;
      }
      if (mIgnoreLightSpecularUniform)
      {
         VaEnvironment::Instance().GetModelDatabase()->ResetModelLighting(mModelId, false);
         mIgnoreLightSpecularUniform = false;
      }
      if (mUseBillboard)
      {
         VaEnvironment::Instance().GetModelDatabase()->ResetModelBillboard(mModelId, true);
         mUseBillboard = false;
      }
      if (mIgnoreBillboard)
      {
         VaEnvironment::Instance().GetModelDatabase()->ResetModelBillboard(mModelId, false);
         mIgnoreBillboard = false;
      }
      if (!IsVisible())
      {
         VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mModelId);
         mModelId = 0;

         if (mRelativeToParentTransform.valid() && mHighlightNode.valid() && mTransformNode->containsNode(mHighlightNode))
         {
            // don't highlight invisible sub-models by removing the highlightnode from the transform node.
            mTransformNode->removeChild(0, 1);
         }
         return;
      }
   }
}

void VaAttachmentModel::EntityMovedCB(VaEntity* aEntityPtr, bool aMoveInTime)
{
   if (0 != mModelId)
   {
      double    hpr[3];
      double    xyz[3];
      osg::Quat q;
      GetParent().GetPosition().GetSceneXYZ(xyz, GetViewer()->GetUniqueId());
      if (GetViewer()->GetMapProjection().projection == 0)
      {
         GetParent().GetUtoEntity()->GetOrientationWCS(hpr[0], hpr[1], hpr[2]);
         q = osg::Quat(hpr[2], osg::Vec3(1, 0, 0), hpr[1], osg::Vec3(0, 1, 0), hpr[0], osg::Vec3(0, 0, 1));
      }
      else
      {
         GetParent().GetUtoEntity()->GetOrientationNED(hpr[0], hpr[1], hpr[2]);
         q = osg::Quat(osg::PI + hpr[2],
                       osg::Vec3(1, 0, 0),
                       hpr[1],
                       osg::Vec3(0, 1, 0),
                       -hpr[0] + osg::PI_2,
                       osg::Vec3(0, 0, 1));
      }
      if ((xyz[0] != mXYZ[0]) || (xyz[1] != mXYZ[1]) || (xyz[2] != mXYZ[2]) || (hpr[0] != mHPR[0]) ||
          (hpr[1] != mHPR[1]) || (hpr[2] != mHPR[2])) // || renewed)
      {
         mXYZ[0] = xyz[0];
         mXYZ[1] = xyz[1];
         mXYZ[2] = xyz[2];
         mHPR[0] = hpr[0];
         mHPR[1] = hpr[1];
         mHPR[2] = hpr[2];

         mModelMatrix.makeTranslate(osg::Vec3d(xyz[0], xyz[1], xyz[2]));
         mModelMatrix.preMultRotate(q);
         if (mRelativeToParentTransform.valid())
         {
            // The relative to parent transform was valid, therefore assuming this model to be a sub model
            // of the parent entity.

            // The scaling needed to match the icon scaling of the base model icon, after the normal icon
            // scaling of the sub model icon (this classes icon)
            float additionalScalingNeeded = 1.0f;

            // if true scale is off, we need to compute the scaling that will occur on both the base
            // model's icon, and this sub model's icon. and adjust our scaling now so that we can match the
            // base model's scaling.
            if ((!mViewerPtr->GetTrueScale()) && (GetViewer()->GetViewer()->GetOSGViewer()))
            {
               float VA_IconScale = GetViewer()->GetIconScale();
               // the above check for the validity of OSGViewer should confirm the validity of this matrix
               osg::Matrixd projectionMatrix = *GetViewer()->GetViewer()->GetProjectionMatrix();
               osg::Matrixd mvp              = mScaleMatrix * mModelMatrix * mViewMatrix * projectionMatrix;
               osg::Vec4    zero             = mvp * osg::Vec4(0, 0, 0, 1);

               float baseModelScreenScale = -1.0f;
               float subModelScreenScale  = -1.0f;
               VaEnvironment::Instance()
                  .GetModelDatabase()
                  ->RequestSharedModel(mIconName, GetViewer())
                  ->getStateSet()
                  ->getUniform("VA_ScreenScale")
                  ->get(subModelScreenScale);
               // In the past, the parent entity's icon was stored in the VaEntity, however that information
               // was unfortunately refactored out, and put into a VaEntity deriving class, WkfPlatform, which
               // this library does not have include access to. instead we will use a round about, inefficient,
               // method to find the icon name of the parent entity, under the assumption that it has a
               // VaAttachmentModel using said icon, and said attachmentModel is the only attachmentModel that
               // has an invalid mRelativeToParentTransform. This assumption should be correct as per the
               // documentation on SetRelativeToParentTransform.
               for (const auto& attchPtr : GetParent().GetAttachments())
               {
                  auto* asModelPtr = dynamic_cast<VaAttachmentModel*>(attchPtr.get());
                  if (asModelPtr && !asModelPtr->mRelativeToParentTransform.valid())
                  {
                     VaEnvironment::Instance()
                        .GetModelDatabase()
                        ->RequestSharedModel(asModelPtr->mIconName, GetViewer())
                        ->getStateSet()
                        ->getUniform("VA_ScreenScale")
                        ->get(baseModelScreenScale);
                     break;
                  }
               }
               if (baseModelScreenScale < 0)
               {
                  // could not find icon for base model, substitue to be the same as the sub model
                  baseModelScreenScale = subModelScreenScale;
               }

               float invScalar = 1.0;
               // invScalar=mix(min(UTO_Viewport.w / zero.w / VA_IconScale * VA_ScreenScale * 0.1, 1.0), 1.0 /
               // UTO_OrthographicZoom / VA_IconScale * VA_ScreenScale * 0.1, UTO_OrthographicMode);
               if (GetViewer()->GetMapProjection().projection != 0)
               {
                  // orthographic view
                  invScalar = 1.0f / GetViewer()->GetViewer()->GetOrtho()->Zoom() / VA_IconScale * 0.1;
               }
               else
               {
                  // perspective view
                  invScalar = GetViewer()->GetViewer()->GetViewport()->height() / zero.w() / VA_IconScale * 0.1;
               }

               // note the sub/base invScalars are inverse scalars,
               // where scaling is done by division instead of multiplication
               float baseInvScalar = invScalar * baseModelScreenScale;
               float subInvScalar  = invScalar * subModelScreenScale;
               baseInvScalar       = std::min(baseInvScalar, 1.0f);
               subInvScalar        = std::min(subInvScalar, 1.0f);

               additionalScalingNeeded = subInvScalar / baseInvScalar; //(1 / baseInvScalar) / (1 / subInvScalar);

               // translate the additionally needed distance to match scaling of base model, so that the model
               // is placed where it belongs relative to the parent/base model icon.
               auto relTranslation = mRelativeToParentTransform->getTrans();
               mModelMatrix.preMultTranslate(relTranslation / baseInvScalar - relTranslation);
            }
            mModelMatrix.preMult(*mRelativeToParentTransform);

            if (additionalScalingNeeded != 1.0f)
            {
               mModelMatrix.preMultScale(
                  osg::Vec3f{additionalScalingNeeded, additionalScalingNeeded, additionalScalingNeeded});
            }
         }
         mTransformNode->setMatrix(mScaleMatrix * mModelMatrix);

         VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId,
                                                                   mScaleMatrix * mModelMatrix,
                                                                   mViewMatrix,
                                                                   mTeamColor);
      }
   }
}

/*************************************************************************************************
 * @brief   Sets the transformation applied on the model relative to the parent entity.
 * @note It is assumed that setting a relative to parent transform makes this object a sub-model
 *       of the entity and not the base model. As such, the sub-model will have screen scaling
 *       equivalent to the base model's icon, instead of using the sub-model's icon.
 * @param   aTransform  The transform to be used for transforming the model relative to the
 *                      parent entity.
 * @date 7/27/2017
 **************************************************************************************************/
void VaAttachmentModel::SetRelativeToParentTransform(osg::RefMatrix* aTransform)
{
   mRelativeToParentTransform = aTransform;
   // SetType("VaAttachmentSubModel");
   EntityMovedCB(&GetParent(), true);
}

void VaAttachmentModel::MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection)
{
   if (aViewerId == GetViewer()->GetUniqueId())
   {
      EntityMovedCB(&GetParent(), true);
   }
}

void VaAttachmentModel::ArticulateModel(const std::string& aPartName, const osg::Matrix& aMatrix)
{
   VaEnvironment::Instance().GetModelDatabase()->ArticulateModel(mModelId, aPartName, aMatrix);
}

#endif

// private
void VaAttachmentModel::SetIconPrivate(const std::string& aName)
{
#ifndef VAHWINST
   if (mDB_ModelRefPtr.valid())
   {
      mShapePtr->GetRoot()->removeChild(0, 1);
      VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mIconName, mDB_ModelRefPtr, mViewerPtr);
   }

   if (0) // if unique model required and the requested model is configurable
   {
      // mDB_ModelHolder.Use(VaEnvironment::Instance().GetModelDatabase()->RequestUniqueModel(aName));
      // configure it as necessary (articulations, switches, and decorations)
   }
   else
   {
      mDB_ModelRefPtr = VaEnvironment::Instance().GetModelDatabase()->RequestSharedModel(aName);
   }
   assert(mDB_ModelRefPtr.valid());
   mShapePtr->GetRoot()->addChild(mDB_ModelRefPtr.get());
#else
   if (false) // if unique model required and the requested mode is configurable
   {
      // mDB_ModelHolder.Use(VaEnvironment::Instance().GetModelDatabase()->RequestUniqueModel(aName));
      // configure it as necessary (articulations, switches, and decorations)
   }
   else
   {
      if (0 != mModelId)
      {
         VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mModelId);

         const VaModelDefinition* def = VaEnvironment::Instance().GetModelDatabase()->GetModelDefinition(mIconName);
         if (def)
         {
            mModelScale =
               def->GetModelScale(VaEnvironment::Instance().GetModelDatabase()->GetViewerModelSet(*GetViewer()));
            VaObserver::ModelScaleChanged[GetUniqueId()](mModelScale);
         }
         else
         {
            mModelScale = 1.0f;
            VaObserver::ModelScaleChanged[GetUniqueId()](mModelScale);
         }
         mModelId = 0;
      }
   }
#endif
}

#ifdef VAHWINST
void VaAttachmentModel::ViewMatrixChanged(VaViewer* aViewerPtr)
{
   if (IsMyViewer(aViewerPtr))
   {
      mViewMatrix = aViewerPtr->GetViewMatrix();
      if (0 != mModelId)
      {
         if (mRelativeToParentTransform.valid())
         {
            // a viewer change on a sub model requires more in depth calculations to adjust for screen
            // scaling, which will adjust mModelMatrix. base-model would not have the model matrix change
            // on a view adjustment.
            mXYZ[0] = -mXYZ[0]; // force renewal of model matrix, technically we only want a renewal on a zoom, but for
                                // now we renew on any change.
            EntityMovedCB(&GetParent(), true);
         }
         else
         {
            VaEnvironment::Instance().GetModelDatabase()->UpdateModel(mModelId,
                                                                      mScaleMatrix * mModelMatrix,
                                                                      mViewMatrix,
                                                                      mTeamColor);
         }
      }
   }
}
#endif

//! every call to to this function to highlight=true a color, must have a matching
//! call to highlight=false that color, otherwise this model will remain highlighted.
//! i.e. calling highlight(viewer,0,true,white) twice and only calling highlight(viewer,0,false,white) once will cause
//! this model to remain highlighted white.
//!
//! undoing current highlights restores previous highlights that have not yet been undone themselves.
bool VaAttachmentModel::Highlight(VaViewer*                aViewerPtr,
                                  const VaAttachmentSubId& aSubpartId,
                                  bool                     aHighlight,
                                  const UtColor&           aColor)
{
#ifdef VAHWINST
   if (aHighlight)
   {
      mHighlightColorStack.emplace_back(aColor);

      if (!mHighlightNode.valid())
      {
         // if not already highlighted, we need to fetch the model
         mHighlightNode = dynamic_cast<osg::Node*>(
            VaEnvironment::Instance().GetModelDatabase()->RequestSharedModel(mIconName, GetViewer()));
         if ((mHighlightNode->getNumDescriptions() > 1) &&
             (mHighlightNode->getDescription(1) == "VA_Billboarded")) // if the node is billboarded
         {
            osg::Program* prog = UtoShaders::UseProgram("billboard.vert", "highlightObject.frag");
            mHighlightNode->getOrCreateStateSet()->setAttributeAndModes(prog, osg::StateAttribute::PROTECTED);
         }
      }
      if ((!mRelativeToParentTransform.valid() || IsVisible()) && mTransformNode->getNumChildren() == 0)
      {
         // sub models shouldn't be highlighted while invisible. base models shall be highlighted while
         // invisible (simply because that was the implemented behavior before the sub/base model logic
         // was implemented with mRelativeToParentTransform).
         mTransformNode->addChild(mHighlightNode);
      }
      aViewerPtr->HighlightNode(mTransformNode.get(), aHighlight, aColor);
   }
   else
   {
      auto riter = std::find(mHighlightColorStack.rbegin(), mHighlightColorStack.rend(), aColor);
      if (riter != mHighlightColorStack.rend())
      {
         mHighlightColorStack.erase(
            (++riter).base()); // to erase a reverse iter, must call erase((riter+1).base()), not erase(riter)
      }

      if (mHighlightNode.valid())
      {
         aViewerPtr->HighlightNode(mTransformNode.get(), aHighlight, aColor);
         if (mHighlightColorStack.empty())
         {
            // there was no underlying highlight color, under the highlight that was just disabled.
            // now that this attachment has no highlights, release the highlight model
            VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(mIconName, mHighlightNode, mViewerPtr);
            mTransformNode->removeChild(0, 1);
            mHighlightNode = nullptr;
         }
         else
         {
            // there was an underlying highlight color, under the highlight we just disabled.
            // restore highlight of underlying highlight color.
            aViewerPtr->HighlightNode(mTransformNode.get(), true, mHighlightColorStack.back());
         }
      }
   }
   return true;
#else
   return false; // let the standard highlight system handle it
#endif
}


} // namespace vespa
