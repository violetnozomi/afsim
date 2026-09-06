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

#include "WkfAttachmentSatelliteTetherTrack.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/MatrixTransform>

#include "UtMath.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaDecoratorNode.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

wkf::AttachmentSatelliteTetherTrack::AttachmentSatelliteTetherTrack(vespa::VaEntity& aParentEntity,
                                                                    vespa::VaViewer& aViewer,
                                                                    const QColor&    aColor)
   : vespa::VaAttachment(aParentEntity, &aViewer, "track")
   , mColor(aColor)
{
   SetType<AttachmentSatelliteTetherTrack>();
   Rebuild();
}

void wkf::AttachmentSatelliteTetherTrack::SetColor(const QColor& aColor)
{
   mColor = aColor;
   Rebuild();
}

void wkf::AttachmentSatelliteTetherTrack::ShowNames(bool aState)
{
   if (aState && !mLabelNodePtr)
   {
      mLabelNodePtr = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cSuppressSmallFeatureCulling);
      QString str   = GetParent().GetName().c_str();
      mLabelNodePtr->SetText(str, Qt::transparent, Qt::white);
      mTransformNodePtr->addChild(mLabelNodePtr);
   }
   else if (!aState && mLabelNodePtr)
   {
      mTransformNodePtr->removeChild(mLabelNodePtr); // this will delete the object
      mLabelNodePtr = nullptr;
   }
}

void wkf::AttachmentSatelliteTetherTrack::Rebuild()
{
   if (!mTransformNodePtr)
   {
      UtoRawShape raw;
      raw.SetReferenceFrame(UtoShape::cFRAME_ENTITY);
      std::string name = GetName();
      name += "_";
      name += std::to_string(GetUniqueId());
      auto shapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(name, raw));
      shapePtr->SetName(GetName());
      shapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
      mTransformNodePtr = shapePtr->GetRoot();
   }
   mTransformNodePtr->removeChild(0, mTransformNodePtr->getNumChildren());
   // mLabelNodePtr should be null for ShowNames to work properly. Once it is removed from mTransformNodePtr it should
   // be made null.
   mLabelNodePtr = nullptr;

   auto geom  = new osg::Geometry();
   auto geode = new osg::Geode();
   geode->addDrawable(geom);
   geom->setComputeBoundingBoxCallback(new ComputeInstancedBoundingBoxCallback());
   mTransformNodePtr->addChild(geode);
   mTransformNodePtr->setCullingActive(false);
   mTransformNodePtr->getOrCreateStateSet();

   osg::StateSet* ss = mTransformNodePtr->getOrCreateStateSet();
   ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   ss->setNestRenderBins(false);
   ss->setRenderBinDetails(1000, "RenderBin");
   ss->setAttributeAndModes(new osg::BlendFunc());

   auto vr     = new osg::Vec3Array;
   auto carray = new osg::Vec4Array;
   carray->push_back(osg::Vec4(mColor.redF(), mColor.greenF(), mColor.blueF(), 0.9f));
   geom->setVertexArray(vr);
   geom->setColorArray(carray);
   geom->setColorBinding(osg::Geometry::BIND_OVERALL);

   osg::StateSet* gss = geode->getOrCreateStateSet();
   gss->setAttributeAndModes(UtoShaders::UseProgram("simplebb.vert", "simplecolor.frag"));
   gss->getOrCreateUniform("VA_TrueScale", osg::Uniform::FLOAT)->set(0.0f);
   gss->getOrCreateUniform("VA_ScreenScale", osg::Uniform::FLOAT)->set(20.0f);
   gss->setAttributeAndModes(new osg::LineWidth(2));
   gss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));

   unsigned int v = 0;

   vr->push_back(osg::Vec3(-10, 0, 0));
   vr->push_back(osg::Vec3(10, 0, 0));
   vr->push_back(osg::Vec3(0, -10, 0));
   vr->push_back(osg::Vec3(0, 10, 0));
   v = 4;
   geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 4));

   unsigned int vstart = v;
   for (double ang = 0; ang < UtMath::cTWO_PI; ang += UtMath::cPI / 90.0)
   {
      float x = 5 * sin(ang);
      float y = 5 * cos(ang);
      vr->push_back(osg::Vec3(x, y, 0));
      ++v;
   }
   geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, vstart, v - vstart));
}
