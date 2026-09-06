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

#include "WkfHDD_PageEngine.hpp"

#include <osg/PositionAttitudeTransform>

#include "UtMath.hpp"

HDD::PageEngine::PageEngine(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mOverlayPtr =
      new Overlay(("hdd_engine" + std::to_string(mId)).c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
   mOverlayPtr->SetDrawFunction(std::bind(&PageEngine::Draw, this));
   mPageName = "ENG";
}

void HDD::PageEngine::SetThrustThrottle(const double aThrust, const bool aThrottleValid, const double aThrottle)
{
   mData.thrustNormalized = aThrust;
   mData.throttleValid    = aThrottleValid;
   if (mData.throttleValid)
   {
      mData.throttleInput = aThrottle;
   }
}

void HDD::PageEngine::Draw()
{
   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);

   double scale = 1.0;

   // Draw the background
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   // If the page is small, draw it's label in the bottom center
   if (mSize == Page::Small)
   {
      DrawSmallEngText(vectorGroup, 0.0, 0.1, 0.75, 0.75, scale);

      // Draw a text box
      float labelFontSize = 0.125f * static_cast<float>(mOverlayPtr->GetWidth());
      mOverlayPtr->DrawLabel(0.0,
                             -0.45 * static_cast<float>(mOverlayPtr->GetHeight()),
                             mPageName.toStdString().c_str(),
                             labelFontSize,
                             osg::Vec4(0.0, 0.8, 0.8, 1.0),
                             osgText::Text::CENTER_BOTTOM);
   }
   else if (mSize == Page::Medium)
   {
      DrawGauge(vectorGroup, -0.15, 0.3, 0.06, scale, "THRUST", mData.thrustNormalized);
      DrawGauge(vectorGroup, -0.15, -0.1, 0.06, scale, "THROTTLE", mData.throttleInput);
      DrawFuelFlow(vectorGroup, 0.0, -0.45, 0.65, 0.1, scale);
   }
   else if (mSize == Page::Large)
   {
      DrawGauge(vectorGroup, -0.15, 0.55, 0.06, scale, "THRUST", mData.thrustNormalized);
      DrawGauge(vectorGroup, -0.15, -0.05, 0.06, scale, "THROTTLE", mData.throttleInput);
      DrawFuelFlow(vectorGroup, 0.0, -0.6, 0.65, 0.1, scale);
   }
   else if (mSize == Page::Full)
   {
      scale = 0.5;
      DrawGauge(vectorGroup, -0.15, 0.55, 0.06, scale, "THRUST", mData.thrustNormalized);
      DrawGauge(vectorGroup, -0.15, -0.05, 0.06, scale, "THROTTLE", mData.throttleInput);
      DrawFuelFlow(vectorGroup, 0.0, -0.6, 0.65, 0.1, scale);
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageEngine::DrawGauge(osg::ref_ptr<osg::Group> aVectorGroup,
                                const double             aX,
                                const double             aY,
                                const double             aR,
                                const double             aScale,
                                const std::string&       aText,
                                const double             aValue)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the gauge
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   mOverlayPtr->SetLineWidthMultiplier(6.0f);

   // Green ring for 270 degrees
   osg::ref_ptr<osg::Vec2Array> greenArray(new osg::Vec2Array);
   for (int ang_deg = 90; ang_deg >= -180; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = (2.0 * aR) * cos(ang_rad);
      double y       = (2.0 * aR) * sin(ang_rad);
      greenArray->push_back(osg::Vec2d((aX + x) * w, (aY + y) * h));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(greenArray, GL_LINE_STRIP, Color::Green).get());

   // Red ring for 60 degrees
   osg::ref_ptr<osg::Vec2Array> redArray(new osg::Vec2Array);
   for (int ang_deg = -180; ang_deg >= -240; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = (2.0 * aR) * cos(ang_rad);
      double y       = (2.0 * aR) * sin(ang_rad);
      redArray->push_back(osg::Vec2d((aX + x) * w, (aY + y) * h));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(redArray, GL_LINE_STRIP, Color::Red).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   // Gauge Needle, slightly smaller radius than the ring size
   double needleAng_deg = 0.0;
   if (aValue >= 1.0)
   {
      double abPos = aValue - 1.0;
      if (abPos > 0.0)
      {
         // Using afterburner
         needleAng_deg = 270.0 + 60.0 * abPos;
      }
      else
      {
         // No afterburner but full MIL
         needleAng_deg = 270.0;
      }
   }
   else
   {
      needleAng_deg = 270.0 * aValue;
   }

   osg::ref_ptr<osg::MatrixTransform> needleXform(new osg::MatrixTransform());
   needleXform->postMult(osg::Matrix::rotate(osg::Quat(needleAng_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, -1))));
   needleXform->postMult(osg::Matrix::translate(osg::Vec3d(aX * w, aY * h, 0.0)));
   osg::ref_ptr<osg::Vec2Array> needleArray(new osg::Vec2Array);
   needleArray->push_back(osg::Vec2d(0.0, 0.0));
   needleArray->push_back(osg::Vec2d(0.0, (2.0 * (aR * 0.8)) * h));

   // Set the line width to 2 for the needle
   mOverlayPtr->SetLineWidthMultiplier(3.0f);
   // If the needle is in the red region, use red, otherwise use green.
   osg::Vec4 gaugeColor = (needleAng_deg > 270.5) ? Color::Red : Color::Green;

   // Draw the thrust needle
   needleXform->addChild(mOverlayPtr->CreateGeometry(needleArray, GL_LINES, gaugeColor).get());

   // Reset the line width
   mOverlayPtr->SetLineWidthMultiplier(1.0f);
   vectorXform->addChild(needleXform.get());

   // Text label
   float fontSize = 0.05f * w;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(aText, aX * w, (-0.1 + aY - aR) * h, fontSize, Color::Green, osgText::Text::CENTER_TOP).get());

   double boxX = aX + aR + 0.225;
   double boxW = 0.1;
   double boxH = 0.06;
   // Bar rect
   osg::ref_ptr<osg::Vec2Array> barArray(new osg::Vec2Array);
   barArray->push_back(osg::Vec2d((boxX - boxW) * w, (aY + boxH) * h));
   barArray->push_back(osg::Vec2d((boxX + boxW) * w, (aY + boxH) * h));
   barArray->push_back(osg::Vec2d((boxX + boxW) * w, (aY - boxH) * h));
   barArray->push_back(osg::Vec2d((boxX - boxW) * w, (aY - boxH) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(barArray, GL_LINE_LOOP, Color::Green).get());

   // Percentage text label
   int perc = static_cast<int>(round(aValue * 100));
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(std::to_string(perc) + "%", boxX * w, aY * h, fontSize, gaugeColor, osgText::Text::CENTER_CENTER));

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageEngine::DrawFuelFlow(osg::ref_ptr<osg::Group> aVectorGroup,
                                   const double             aX,
                                   const double             aY,
                                   const double             aW,
                                   const double             aH,
                                   const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the objects
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   float fontSize = 0.05f * w;
   // Draw the labels
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("FUEL FLOW", (aX - (aW * 0.5)) * w, aY * h, fontSize, Color::Green, osgText::Text::LEFT_CENTER));

   // Fuel flow box rect
   double                       ffBoxX = (aX + (aW * 0.5));
   osg::ref_ptr<osg::Vec2Array> ffBoxArray(new osg::Vec2Array);
   ffBoxArray->push_back(osg::Vec2d((ffBoxX + aX - (aW * 0.4)) * w, (aY + (aH * 0.5)) * h));
   ffBoxArray->push_back(osg::Vec2d(ffBoxX * w, (aY + (aH * 0.5)) * h));
   ffBoxArray->push_back(osg::Vec2d(ffBoxX * w, (aY - (aH * 0.5)) * h));
   ffBoxArray->push_back(osg::Vec2d((ffBoxX + aX - (aW * 0.4)) * w, (aY - (aH * 0.5)) * h));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(ffBoxArray, GL_LINE_LOOP, Color::Green).get());

   std::stringstream ff;
   ff << std::fixed << std::setprecision(0) << mData.fuelFlow_pph;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(ff.str(), (ffBoxX - (aW * 0.2)) * w, aY * h, fontSize, Color::Green, osgText::Text::CENTER_CENTER));

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageEngine::DrawSmallEngText(osg::ref_ptr<osg::Group> aVectorGroup,
                                       const double             aX,
                                       const double             aY,
                                       const double             aW,
                                       const double             aH,
                                       const double             aScale)
{
   double w = static_cast<float>(mOverlayPtr->GetWidth()) * aScale;
   double h = static_cast<float>(mOverlayPtr->GetHeight()) * aScale * mRatio;

   // Transformation - Position the objects
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Font size
   float fontSize = 0.1f * w;

   // Draw the labels, checking whether or not thrust and throttle need to be drawn red or green.
   // Draw thrust.
   vectorXform->addChild(mOverlayPtr->AddTextItem("THRUST",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * 0.375)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));

   // Gauge Needle, slightly smaller radius than the ring size
   double tmpAngle = 0.0;
   if (mData.thrustNormalized >= 1.0)
   {
      double abPos = mData.thrustNormalized - 1.0;
      if (abPos > 0.0)
      {
         // Using afterburner
         tmpAngle = 270.0 + 60.0 * abPos;
      }
      else
      {
         // No afterburner but full MIL
         tmpAngle = 270.0;
      }
   }
   else
   {
      tmpAngle = 270.0 * mData.thrustNormalized;
   }

   osg::Vec4 tempColor = (tmpAngle > 270.5) ? Color::Red : Color::Green;
   int       perc      = static_cast<int>(round(mData.thrustNormalized * 100));
   perc                = (perc > 100) ? perc - 100 : perc;
   vectorXform->addChild(mOverlayPtr->AddTextItem(std::to_string(perc) + "%",
                                                  (aX + (aW * 0.5)) * w,
                                                  (aY + (aH * 0.375)) * h,
                                                  fontSize,
                                                  tempColor,
                                                  osgText::Text::RIGHT_CENTER));

   // Draw throttle.
   vectorXform->addChild(mOverlayPtr->AddTextItem("THROTTLE",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * 0.225)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));

   // Gauge Needle, slightly smaller radius than the ring size
   tmpAngle = 0.0;
   if (mData.throttleInput >= 1.0)
   {
      double abPos = mData.throttleInput - 1.0;
      if (abPos > 0.0)
      {
         // Using afterburner
         tmpAngle = 270.0 + 60.0 * abPos;
      }
      else
      {
         // No afterburner but full MIL
         tmpAngle = 270.0;
      }
   }
   else
   {
      tmpAngle = 270.0 * mData.throttleInput;
   }

   tempColor = (tmpAngle > 270.5) ? Color::Red : Color::Green;
   perc      = static_cast<int>(round(mData.throttleInput * 100));
   perc      = (perc > 100) ? perc - 100 : perc;
   vectorXform->addChild(mOverlayPtr->AddTextItem(std::to_string(perc) + "%",
                                                  (aX + (aW * 0.5)) * w,
                                                  (aY + (aH * 0.225)) * h,
                                                  fontSize,
                                                  tempColor,
                                                  osgText::Text::RIGHT_CENTER));

   // Draw fuel flow.
   vectorXform->addChild(mOverlayPtr->AddTextItem("FF",
                                                  (aX - (aW * 0.5)) * w,
                                                  (aY + (aH * 0.075)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::LEFT_CENTER));
   std::stringstream ff;
   ff << std::fixed << std::setprecision(0) << std::setw(5) << mData.fuelFlow_pph;
   vectorXform->addChild(mOverlayPtr->AddTextItem(ff.str(),
                                                  (aX + (aW * 0.5)) * w,
                                                  (aY + (aH * 0.075)) * h,
                                                  fontSize,
                                                  Color::Green,
                                                  osgText::Text::RIGHT_CENTER));

   aVectorGroup->addChild(vectorXform.get());
}
