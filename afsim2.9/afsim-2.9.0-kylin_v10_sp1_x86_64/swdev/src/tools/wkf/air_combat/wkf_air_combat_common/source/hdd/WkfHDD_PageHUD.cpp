// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfHDD_PageHUD.hpp"

#include <iomanip>
#include <sstream>

#include <osg/Point>
#include <osg/PositionAttitudeTransform>

#include "UtMath.hpp"

HDD::PageHUD::PageHUD(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr = new Overlay(("hdd_hud" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
   aViewerPtr->AddOverlay(mOverlayPtr);
   SetPosition(aPosition);
   mOverlayPtr->SetVisible(true);
   mOverlayPtr->SetButton1ClickAction(
      [this](int x, int y, int s) -> bool
      {
         // Only handle click events when visible
         if (IsVisible())
         {
            emit PageClicked(this);
         }
         return false;
      });
   mOverlayPtr->SetDrawFunction(std::bind(&PageHUD::Draw, this));
   mPageName = "HUD";
}

void HDD::PageHUD::Draw()
{
   double scale    = 1.0;
   double w        = static_cast<float>(mOverlayPtr->GetWidth()) * scale;
   double h        = static_cast<float>(mOverlayPtr->GetHeight()) * scale * mRatio;
   float  fontSize = 0.1f * w;
   double dotScale = 2.0f;

   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   // Draw the background
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   // If the page is small, draw it's label in the bottom center
   if (mSize == Page::Small)
   {
      // Draw a text box
      float labelFontSize = 0.125f * static_cast<float>(mOverlayPtr->GetWidth());
      mOverlayPtr->DrawLabel(0.0,
                             -0.45 * static_cast<float>(mOverlayPtr->GetHeight()),
                             mPageName.toStdString().c_str(),
                             labelFontSize,
                             osg::Vec4(0.0, 0.8, 0.8, 1.0),
                             osgText::Text::CENTER_BOTTOM);
   }
   else if (mSize == Page::Medium || mSize == Page::Large)
   {
      dotScale = 3.0f;
   }
   else if (mSize == Page::Full)
   {
      dotScale = 4.0f;
   }

   // Speed (Left Circle)
   // Angle for speed needle (space between each dot is 10 units)
   double speedAngleRad = ((360.0 * UtMath::cRAD_PER_DEG) / 100.0) * mData.ktas;
   DrawCircleWithDots(vectorGroup, -0.25, 0.0, 0.2, speedAngleRad, dotScale, scale);
   vectorGroup->addChild(
      mOverlayPtr
         ->AddTextItem(std::to_string((int)mData.ktas).c_str(), -0.25 * w, 0.0, fontSize * 0.8, Color::Green, osgText::Text::CENTER_CENTER)
         .get());

   // Altitude (Right Circle)
   // Angle for altitude needle (space between each dot is 100 units)
   double altitudeAngleRad = ((360.0 * UtMath::cRAD_PER_DEG) / 1000.0) * mData.altitude_ft;
   DrawCircleWithDots(vectorGroup, 0.25, 0.0, 0.2, altitudeAngleRad, dotScale, scale);
   vectorGroup->addChild(mOverlayPtr
                            ->AddTextItem(std::to_string((int)mData.altitude_ft).c_str(),
                                          0.25 * w,
                                          0.0,
                                          fontSize * 0.8,
                                          Color::Green,
                                          osgText::Text::CENTER_CENTER)
                            .get());

   // Heading text
   double heading_deg = mData.heading_deg;

   if (heading_deg < 0.0)
   {
      heading_deg += 360.0;
   }
   if (heading_deg > 360.0)
   {
      heading_deg -= 360.0;
   }

   int hdg_deg = (int)(heading_deg + 0.5);
   if (hdg_deg == 0)
   {
      hdg_deg = 360.0;
   }
   if (hdg_deg > 360)
   {
      hdg_deg -= 360.0;
   }

   std::stringstream heading;
   heading << std::setw(3) << std::setfill('0') << hdg_deg;
   vectorGroup->addChild(
      mOverlayPtr->AddTextItem(heading.str(), 0.0, 0.38 * h, fontSize, Color::Green, osgText::Text::CENTER_CENTER).get());

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageHUD::DrawCircleWithDots(osg::ref_ptr<osg::Group> aVectorGroup,
                                      const double             aX,
                                      const double             aY,
                                      const double             aRadius,
                                      const double             aNeedleAngleRad,
                                      const double             aDotScale,
                                      const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Circle dots every 36 degrees (10 dots total)
   osg::ref_ptr<osg::Vec2Array> dotRingArray(new osg::Vec2Array);
   for (int ang_deg = -90; ang_deg <= 270; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aX + (aRadius * cos(ang_rad));
      double y       = aY + (aRadius * sin(ang_rad));
      dotRingArray->push_back(osg::Vec2d(x * w, y * h));
   }

   // State set for smaller, smoother points
   osg::ref_ptr<osg::Geode>    vectorGeode(new osg::Geode);
   osg::ref_ptr<osg::StateSet> pointState(new osg::StateSet);
   osg::ref_ptr<osg::Point>    pointSizeAttr(new osg::Point);
   pointSizeAttr->setSize(aDotScale);
   pointState->setAttribute(pointSizeAttr.get());
   pointState->setMode(GL_POINT_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   pointState->setMode(GL_BLEND, osg::StateAttribute::ON);
   vectorGeode->setStateSet(pointState.get());

   vectorGeode->addChild(mOverlayPtr->CreateGeometry(dotRingArray, GL_POINTS, Color::Green).get());
   aVectorGroup->addChild(vectorGeode);

   // Needle rotation and positioning
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXForm(new osg::PositionAttitudeTransform());
   vectorXForm->setAttitude(osg::Quat(-aNeedleAngleRad, osg::Vec3(0.0f, 0.0f, 1.0f)));
   vectorXForm->setPosition(osg::Vec3d(aX * w, aY * h, 0.0));

   // Needle
   osg::ref_ptr<osg::Vec2Array> needleArray(new osg::Vec2Array);
   needleArray->push_back(osg::Vec2d(0.0, 0.12 * h));
   needleArray->push_back(osg::Vec2d(0.0, 0.18 * h));

   mOverlayPtr->SetLineWidthMultiplier(2.0f);
   vectorXForm->addChild(mOverlayPtr->CreateGeometry(needleArray, GL_LINES, Color::Green).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);
   aVectorGroup->addChild(vectorXForm);
}
