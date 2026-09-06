// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MapOverlayScale.hpp"

#include <iostream>
#include <sstream>

#include <QColor>
#include <QFont>
#include <osg/BlendFunc>

#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "Uto.hpp"
#include "UtoAttrPolyLineShape.hpp"
#include "VaCamera.hpp"
#include "VaEnvironment.hpp"
#include "VaPosition.hpp"
#include "VaViewer.hpp"

namespace
{
constexpr int cMEASURE_WIDTH = 100;
}

Map::OverlayScale::OverlayScale()
   : vespa::VaOverlay("scale_overlay")
{
   SetType<OverlayScale>();
}

Map::OverlayScale::OverlayScale(const std::string& aName)
   : vespa::VaOverlay(aName)
{
   SetType<OverlayScale>();
}

Map::OverlayScale::OverlayScale(const OverlayScale& aSrc)
   : vespa::VaOverlay(aSrc)
{
   SetType<OverlayScale>();
}

Map::OverlayScale::~OverlayScale()
{
   UnbindShape("scale");
}

Map::OverlayScale* Map::OverlayScale::Clone() const
{
   return new OverlayScale(*this);
}

bool Map::OverlayScale::Initialize()
{
   bool initialized = false;

   UtoRawShape scale;
   mShape = dynamic_cast<UtoRawShape*>(BindShape("scale", scale));
   if (mShape == nullptr)
   {
      std::cerr << "Could not bind shape: scale" << std::endl;
   }
   mTransform = new osg::MatrixTransform();
   mShape->GetRoot()->addChild(mTransform);
   mDrawings = new osg::Geode();
   mTransform->addChild(mDrawings);
   mTextNode = new vespa::VaDecoratorNode();
   mTransform->addChild(mTextNode);
   mTransform->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc);

   if (mShape != nullptr)
   {
      SetVisible(true);
      initialized = true;
   }
   return initialized;
}

void Map::OverlayScale::Update()
{
   if (IsVisible() && (mShape != nullptr))
   {
      if (GetViewer()->GetCamera()->GetMotionType() == vespa::VaCameraMotion::cGEOCENTRIC)
      {
         vespa::VaPosition camRefPos = GetViewer()->GetCamera()->GetCameraMotion()->GetReferencePosition();

         double camxyz[3];
         GetViewer()->WorldToScreen(camRefPos, camxyz[0], camxyz[1], camxyz[2]);

         vespa::VaPosition srcPos;
         vespa::VaPosition dstPos;
         GetViewer()->ScreenToWorld((int)(camxyz[0] - 0.5 * cMEASURE_WIDTH), (int)camxyz[1], srcPos);
         GetViewer()->ScreenToWorld((int)(camxyz[0] + 0.5 * cMEASURE_WIDTH), (int)camxyz[1], dstPos);

         double hdg, dist;
         UtSphericalEarth::GreatCircleHeadingAndDistance(srcPos.GetLat(),
                                                         srcPos.GetLon(),
                                                         dstPos.GetLat(),
                                                         dstPos.GetLon(),
                                                         hdg,
                                                         dist);

         unsigned int format = (vespa::VaEnvironment::Instance().GetUnitFormat().mDistFormat & UtReal::FmtUnitMask);

         // Add precision to the scale as we zoom in.
         if (dist < UtMath::cM_PER_NM)
         {
            format += 2;
         }
         else if (dist < 10 * UtMath::cM_PER_NM)
         {
            format += 1;
         }

         UtLength distance(dist, format);

         std::ostringstream oss1;
         oss1 << distance;

         QFont f;
         f.setPointSize(12);
         mTextNode->SetText(QString::fromStdString(oss1.str()), QColor(0, 0, 0), f);
      }
      else
      {
         QFont f;
         f.setPointSize(12);
         mTextNode->SetText(QString("No Data"), QColor(128, 128, 128), f);
      }
      mTransform->setMatrix(
         osg::Matrix::translate(osg::Vec3(GetViewer()->GetWidth() * 0.5 - mTextNode->GetWidth() * 0.5, 12.0, 0.0)));
      BuildDrawings();
   }
}

void Map::OverlayScale::BuildDrawings()
{
   float w1 = (mTextNode->GetWidth() + cMEASURE_WIDTH) * 0.5;
   float w2 = (mTextNode->GetWidth() - cMEASURE_WIDTH) * 0.5;

   mDrawings->removeDrawables(0, mDrawings->getNumDrawables());
   osg::ref_ptr<osg::Geometry> window = new osg::Geometry;

   mDrawings->addDrawable(window);

   osg::Vec3Array* vertArray     = new osg::Vec3Array;
   osg::Vec4Array* winColorArray = new osg::Vec4Array;

   winColorArray->push_back(osg::Vec4(1.0F, 1.0F, 1.0F, 0.6F));
   window->setVertexArray(vertArray);
   window->setColorArray(winColorArray);
   window->setColorBinding(osg::Geometry::BIND_OVERALL);
   vertArray->push_back(osg::Vec3(std::min(w1 - 6.0F, -6.0F), -12, 0));
   vertArray->push_back(osg::Vec3(std::max(w2 + 6.0F, mTextNode->GetWidth() + 6.0F), -12, 0));
   vertArray->push_back(osg::Vec3(std::max(w2 + 6.0F, mTextNode->GetWidth() + 6.0F), mTextNode->GetHeight(), 0));
   vertArray->push_back(osg::Vec3(std::min(w1 - 6.0F, -6.0F), mTextNode->GetHeight(), 0));

   window->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

   osg::ref_ptr<osg::Geometry> bar = new osg::Geometry;

   mDrawings->addDrawable(bar);

   osg::Vec4Array* barColorArray = new osg::Vec4Array;

   barColorArray->push_back(osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));
   bar->setVertexArray(vertArray);
   bar->setColorArray(barColorArray);
   bar->setColorBinding(osg::Geometry::BIND_OVERALL);
   vertArray->push_back(osg::Vec3(w1, -6, 0));
   vertArray->push_back(osg::Vec3(w1, -3, 0));
   vertArray->push_back(osg::Vec3(w2, -3, 0));
   vertArray->push_back(osg::Vec3(w2, -6, 0));

   bar->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
   bar->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 4, 4));
}
