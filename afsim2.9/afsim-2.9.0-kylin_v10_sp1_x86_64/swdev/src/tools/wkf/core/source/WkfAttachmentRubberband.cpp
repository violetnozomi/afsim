// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAttachmentRubberband.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/LineStipple>
#include <osg/LineWidth>

#include "UtSphericalEarth.hpp"
#include "UtoMemory.hpp"
#include "UtoRawShape.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaViewerManager.hpp"
#include "WkfViewer.hpp"

wkf::AttachmentRubberband::AttachmentRubberband(vespa::VaEntity&                                        aParent,
                                                vespa::VaViewer*                                        aViewerPtr,
                                                const std::function<void(AttachmentRubberband&)>&       aUpdateFunc,
                                                const std::function<void(const AttachmentRubberband&)>& aFinishFunc,
                                                const vespa::VaEntity&                                  aSource)
   : vespa::VaAttachment(aParent, aViewerPtr, "rubberband")
   , mUpdateFunc(aUpdateFunc)
   , mFinishFunc(aFinishFunc)
   , mColor(87, 166, 74)
{
   SetType<AttachmentRubberband>();
   int x;
   int y;

   mMeasureSrc.SetEntity(&aSource);

   aViewerPtr->GetMousePosition(x, y);
   UpdateCursorPositionCB(aViewerPtr, x, y);
}

void wkf::AttachmentRubberband::PrivateInitializeCallbacks()
{
   if (!IsInitialized())
   {
      mCallbacks.Add(
         vespa::VaObserver::EntityMoved.Connect(&AttachmentRubberband::EntityMovedCB, this, GetParent().GetUniqueId()));
      mCallbacks.Add(vespa::VaObserver::UpdateCursorPosition.Connect(&AttachmentRubberband::UpdateCursorPositionCB, this));
   }
}

void wkf::AttachmentRubberband::GetRelativePosition(const vespa::VaPosition& aLLA, double aRelXYZ[3]) const
{
   double xyz[3];
   aLLA.GetSceneXYZ(xyz, 0);

   double parentXYZ[3];
   GetParent().GetPosition().GetSceneXYZ(parentXYZ, 0);

   UtVec3d::Subtract(aRelXYZ, xyz, parentXYZ);
}

void wkf::AttachmentRubberband::SetSrcPosition(const vespa::VaPosition& aPosition)
{
   mMeasureSrc.SetPosition(aPosition);
}

void wkf::AttachmentRubberband::SetLineColor(const UtoColor& aColor)
{
   mColor = aColor;
   if (mShapePtr)
   {
      mShapePtr->SetColor(mColor);
   }
}

void wkf::AttachmentRubberband::End()
{
   mFinishFunc(*this);
}

void wkf::AttachmentRubberband::SetSrcPosition(const vespa::VaEntity& aEntity)
{ // Disconnect the old 'EntityMoved' callback if it exists.
   if (mMeasureSrcCallbackId != -1)
   {
      mCallbacks.Disconnect(mMeasureSrcCallbackId);
   }
   // Add the new 'EntityMoved' callback.
   mMeasureSrcCallbackId = mCallbacks.Add(
      vespa::VaObserver::EntityMoved.Connect(&AttachmentRubberband::EntityMovedCB, this, aEntity.GetUniqueId()));

   mMeasureSrc.SetEntity(&aEntity);
}

void wkf::AttachmentRubberband::PrivateLoad()
{
   if (mShapeName.empty())
   {
      static unsigned int uid = 0;
      mShapeName              = "Rubberband_" + std::to_string(uid++);
   }

   // Remove previous models
   RemoveShapeFromParent(mShapeName);
   mShapePtr = nullptr;

   UtoRawShape tempLine;
   tempLine.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   tempLine.OverrideRenderBin(1000, false);
   mShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(mShapeName, tempLine));

   osg::Group*              grp   = mShapePtr->GetTree();
   osg::ref_ptr<osg::Geode> geode = ut::osg::make_ref<osg::Geode>();
   grp->addChild(geode);
   mGeometry = ut::osg::make_ref<osg::Geometry>();
   mGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   mGeometry->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(2.0));
   geode->addDrawable(mGeometry);

   geode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::LineStipple>(1, 0xF0F0));
   geode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::Depth>(osg::Depth::ALWAYS));
   geode->getOrCreateStateSet()->setAttributeAndModes(ut::osg::make_ref<osg::BlendFunc>());

   Build();
}

void wkf::AttachmentRubberband::Build(bool aForce)
{
   if (nullptr != mShapePtr)
   {
      mGeometry->removePrimitiveSet(0, mGeometry->getNumPrimitiveSets());

      osg::Vec4 color(mColor.RedF(), mColor.GreenF(), mColor.BlueF(), mColor.AlphaF());

      osg::ref_ptr<osg::Vec3Array> v = ut::osg::make_ref<osg::Vec3Array>();
      mGeometry->setVertexArray(v);
      osg::ref_ptr<osg::Vec4Array> c = ut::osg::make_ref<osg::Vec4Array>();
      mGeometry->setColorArray(c);
      mGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      double zecef[3];
      double pecef[3];
      double tecef[3];
      GetParent().GetPosition().GetECEF(zecef);
      mMeasureSrc.GetPosition().GetECEF(pecef);
      v->push_back(osg::Vec3(pecef[0] - zecef[0], pecef[1] - zecef[1], pecef[2] - zecef[2]));
      c->push_back(color);

      vespa::VaPosition dst = vespa::VaEnvironment::Instance().GetCurrentCursorPosition();

      dst.GetECEF(tecef);

      v->push_back(osg::Vec3(tecef[0] - zecef[0], tecef[1] - zecef[1], tecef[2] - zecef[2]));
      c->push_back(color);

      mGeometry->addPrimitiveSet(ut::osg::make_ref<osg::DrawArrays>(osg::PrimitiveSet::LINES, 0, 2));
   }
}

void wkf::AttachmentRubberband::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   if (aEntityPtr != nullptr)
   {
      unsigned int id = aEntityPtr->GetUniqueId();
      if (id == GetParent().GetUniqueId())
      {
         Build();
      }
      else if (aEntityPtr == mMeasureSrc.GetEntity())
      {
         Build();
      }
   }
   mUpdateFunc(*this);
}

void wkf::AttachmentRubberband::UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   mViewerPtr->ScreenToWorld(aMouseX, aMouseY, mHoverLocation);
   mHoverEntityPtr = nullptr;
   vespa::VaHitList hits;
   mViewerPtr->PickFromScreen(hits, aMouseX, aMouseY);
   for (auto&& hit : hits)
   {
      mHoverEntityPtr = hit.FindEntity();
      if (mHoverEntityPtr)
      {
         break;
      }
   }
   Build();
   mUpdateFunc(*this);
}
