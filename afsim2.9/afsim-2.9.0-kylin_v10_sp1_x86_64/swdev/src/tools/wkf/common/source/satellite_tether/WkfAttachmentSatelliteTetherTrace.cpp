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

#include "WkfAttachmentSatelliteTetherTrace.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/MatrixTransform>

#include "UtMath.hpp"
#include "UtRelativeGeometry.hpp"
#include "UtoMemory.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"

wkf::AttachmentSatelliteTetherTrace::AttachmentSatelliteTetherTrace(vespa::VaEntity& aParentEntity,
                                                                    vespa::VaViewer& aViewer,
                                                                    vespa::VaEntity& aRelativeToEntity,
                                                                    const QColor&    aColor)
   : vespa::VaAttachment(aParentEntity, &aViewer, "trace")
   , mOriginEntityPtr(&aRelativeToEntity)
   , mColor(aColor)
{
   SetType<AttachmentSatelliteTetherTrace>();
   mCallbacks.Add(vespa::VaObserver::EntityDeleted.Connect(&AttachmentSatelliteTetherTrace::OriginDeletedCB,
                                                           this,
                                                           mOriginEntityPtr->GetUniqueId()));
   Build();
}

void wkf::AttachmentSatelliteTetherTrace::UpdateTransform(const osg::Matrix& aMat)
{
   if (mTransformNodePtr)
   {
      mTransformNodePtr->setMatrix(aMat);
   }
}

unsigned int wkf::AttachmentSatelliteTetherTrace::GetOriginPlatformId() const
{
   return mOriginEntityPtr->GetUniqueId();
}

std::string wkf::AttachmentSatelliteTetherTrace::GetOriginPlatformName() const
{
   return mOriginEntityPtr->GetName();
}

void wkf::AttachmentSatelliteTetherTrace::OriginDeletedCB(vespa::VaEntity* aEntityPtr)
{
   GetParent().DeleteLater(this);
}

void wkf::AttachmentSatelliteTetherTrace::Build()
{
   if (!mTransformNodePtr)
   {
      UtoRawShape raw;
      raw.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
      std::string name = GetName();
      name += "_";
      name += std::to_string(GetUniqueId());
      auto shapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(name, raw));
      shapePtr->SetName(GetName());
      shapePtr->SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
      mTransformNodePtr = ut::osg::make_ref<osg::MatrixTransform>();
      shapePtr->GetRoot()->addChild(mTransformNodePtr);
   }
   mTransformNodePtr->removeChild(0, mTransformNodePtr->getNumChildren());

   osg::StateSet* ss = mTransformNodePtr->getOrCreateStateSet();
   ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   ss->setNestRenderBins(false);
   ss->setRenderBinDetails(1000, "RenderBin");
   ss->setAttributeAndModes(ut::osg::make_ref<osg::BlendFunc>());
   ss->setAttributeAndModes(ut::osg::make_ref<osg::LineWidth>(2));
   ss->setAttributeAndModes(UtoShaders::UseProgram("simplelogsupport.vert", "simplecolor.frag"));

   mGeometryPtr = ut::osg::make_ref<osg::Geometry>();
   auto geode   = ut::osg::make_ref<osg::Geode>();
   geode->addDrawable(mGeometryPtr);
   mGeometryPtr->setComputeBoundingBoxCallback(ut::osg::make_ref<ComputeInstancedBoundingBoxCallback>());
   mTransformNodePtr->addChild(geode);
   mGeometryPtr->setUseDisplayList(false);

   mVertexArray = ut::osg::make_ref<osg::Vec3Array>();
   mColorArray  = ut::osg::make_ref<osg::Vec4Array>();
   mGeometryPtr->setVertexArray(mVertexArray);
   mGeometryPtr->setColorArray(mColorArray);
   mGeometryPtr->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
}

void wkf::AttachmentSatelliteTetherTrace::ShowFutureTracks(bool aState)
{
   mFutureAlpha = aState ? 0.3f : 0.0f;
   Update(vespa::VaEnvironment::Instance().GetTime()->GetCurrentTime());
}

void wkf::AttachmentSatelliteTetherTrace::ResetDrawing()
{
   mGeometryPtr->removePrimitiveSet(0, mGeometryPtr->getNumPrimitiveSets());
}

constexpr double wkf::AttachmentSatelliteTetherTrace::cTOLERANCE_SQUARED;
