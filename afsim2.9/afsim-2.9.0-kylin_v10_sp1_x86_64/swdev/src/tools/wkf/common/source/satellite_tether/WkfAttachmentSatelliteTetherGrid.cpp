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

#include "WkfAttachmentSatelliteTetherGrid.hpp"

#include <QColor>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include "UtMath.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "UtoViewer.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "annotation/WkfAttachmentDecorator.hpp"

wkf::AttachmentSatelliteTetherGrid::AttachmentSatelliteTetherGrid(vespa::VaEntity& aParentEntity, vespa::VaViewer& aViewer)
   : vespa::VaAttachment(aParentEntity, &aViewer, "grid")
{
   SetType<AttachmentSatelliteTetherGrid>();
   Rebuild();
   mCallbacks.Add(vespa::VaObserver::ViewMatrixChanged.Connect(&AttachmentSatelliteTetherGrid::ViewMatrixChanged, this));
}

void wkf::AttachmentSatelliteTetherGrid::SetAlignment(const Alignment& aAlignment)
{
   if (mAlignment != aAlignment)
   {
      mAlignment = aAlignment;
      osg::Matrix mat;
      if (mAlignment == IR)
      {
         mat.setRotate(osg::Quat(osg::PI_2, osg::Vec3d(0, 1, 0)));
      }
      else if (mAlignment == RC)
      {
         mat.setRotate(osg::Quat(-osg::PI_2, osg::Vec3d(0, 0, 1)));
      }
      mAlignmentTransform->setMatrix(mat);
   }
}

void wkf::AttachmentSatelliteTetherGrid::SetCartesian(const bool aState)
{
   if (mCartesian != aState)
   {
      mCartesian = aState;
      Rebuild();
   }
}

void wkf::AttachmentSatelliteTetherGrid::Rebuild()
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
      mTransformNodePtr = new osg::MatrixTransform();
      shapePtr->GetRoot()->addChild(mTransformNodePtr);
      mAlignmentTransform = new osg::MatrixTransform();
      mTransformNodePtr->addChild(mAlignmentTransform);
   }
   mAlignmentTransform->removeChild(0, mAlignmentTransform->getNumChildren());

   auto geom  = new osg::Geometry();
   auto geode = new osg::Geode();
   geode->addDrawable(geom);
   mAlignmentTransform->addChild(geode);
   mAlignmentTransform->setCullingActive(false);
   mAlignmentTransform->getOrCreateStateSet();

   auto vr     = new osg::Vec3Array;
   auto carray = new osg::Vec4Array;
   carray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.6f));
   geom->setVertexArray(vr);
   geom->setColorArray(carray);
   geom->setColorBinding(osg::Geometry::BIND_OVERALL);
   auto ss = mAlignmentTransform->getOrCreateStateSet();
   ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   ss->setNestRenderBins(false);
   ss->setRenderBinDetails(1000, "DepthSortedBin");
   ss->setAttributeAndModes(UtoShaders::UseProgram("simplelogsupport.vert", "simplecolor.frag"));
   ss->setAttributeAndModes(new osg::BlendFunc());

   int          step  = pow(10, mFactor);
   int          limit = 20 * step;
   unsigned int v     = 0;

   if (mCartesian)
   {
      for (int i = -limit; i <= limit; i += step)
      {
         vr->push_back(osg::Vec3(0, i, 0));
         vr->push_back(osg::Vec3(0, i, limit));
         vr->push_back(osg::Vec3(0, i, 0));
         vr->push_back(osg::Vec3(0, i, -limit));
         vr->push_back(osg::Vec3(0, 0, i));
         vr->push_back(osg::Vec3(0, limit, i));
         vr->push_back(osg::Vec3(0, 0, i));
         vr->push_back(osg::Vec3(0, -limit, i));
         v += 8;

         auto pat = new osg::PositionAttitudeTransform;
         mAlignmentTransform->addChild(pat);
         pat->setPosition(osg::Vec3(0, i, 0));
         auto pat2 = new osg::PositionAttitudeTransform;
         mAlignmentTransform->addChild(pat2);
         pat2->setPosition(osg::Vec3(0, 0, i));
         auto    dnode = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cSuppressSmallFeatureCulling);
         QString str;
         if (step >= 1000)
         {
            str = QString::number(i / 1000) + " k";
         }
         else
         {
            str = QString::number(i);
         }
         dnode->SetText(str, Qt::transparent, Qt::white);
         pat->addChild(dnode);
         pat2->addChild(dnode);
      }
      geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, v));
   }
   else
   {
      for (double ang = 0; ang < UtMath::cPI; ang += UtMath::cPI / 6.0)
      {
         float x = limit * sin(ang);
         float y = limit * cos(ang);
         vr->push_back(osg::Vec3(0, -x, -y));
         vr->push_back(osg::Vec3(0, x, y));
         v += 2;
      }
      geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, v));
      for (int i = step; i <= limit; i += step)
      {
         unsigned int vstart = v;
         for (double ang = 0; ang < UtMath::cTWO_PI; ang += UtMath::cPI / 90.0)
         {
            float x = i * sin(ang);
            float y = i * cos(ang);
            vr->push_back(osg::Vec3(0, x, y));
            ++v;
         }
         geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, vstart, v - vstart));

         auto pat = new osg::PositionAttitudeTransform;
         mAlignmentTransform->addChild(pat);
         pat->setPosition(osg::Vec3(0, i, 0));
         auto    dnode = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cSuppressSmallFeatureCulling);
         QString str;
         if (step >= 1000)
         {
            str = QString::number(i / 1000) + " k";
         }
         else
         {
            str = QString::number(i);
         }
         dnode->SetText(str, Qt::transparent, Qt::white);
         pat->addChild(dnode);
      }
   }
}

void wkf::AttachmentSatelliteTetherGrid::ViewMatrixChanged(vespa::VaViewer* aViewerPtr)
{
   if (aViewerPtr->GetUniqueId() == GetViewer()->GetUniqueId())
   {
      double ecef[3];
      GetParent().GetPosition().GetECEF(ecef);

      osg::Vec4 vc = osg::Vec4(ecef[0], ecef[1], ecef[2], 1.0) * aViewerPtr->GetViewMatrix() *
                     *(aViewerPtr->GetViewer()->GetProjectionMatrix());
      int f = 4 - log10(100 * 0.15 * 1920 / vc.w());
      f     = (f >= 0) ? f : 0;
      if (f != mFactor)
      {
         mFactor = f;
         Rebuild();
      }
   }
}

void wkf::AttachmentSatelliteTetherGrid::UpdateTransform(const osg::Matrix& aMat)
{
   if (mTransformNodePtr)
   {
      mTransformNodePtr->setMatrix(aMat);
   }
}
