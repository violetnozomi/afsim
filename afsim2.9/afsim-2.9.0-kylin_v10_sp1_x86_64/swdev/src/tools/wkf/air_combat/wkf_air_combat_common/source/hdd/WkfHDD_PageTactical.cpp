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

#include "WkfHDD_PageTactical.hpp"

#include <iomanip>
#include <sstream>

#include <QTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Stencil>

#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"

HDD::PageTactical::PageTactical(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr, const size_t aTSDIndex)
   : Page(aViewerPtr, aPosition, aParentPtr)
{
   mPageName   = QString::fromStdString(std::string("TSD-") + std::to_string(aTSDIndex));
   mOverlayPtr = new Overlay(mPageName.toStdString().c_str(), aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
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
            HandleClickRegions(x, y);
            HandleEntityClickRegions(x, y);
         }
         return false;
      });
   mOverlayPtr->SetMouseMoveAction(
      [this](int x, int y) -> bool
      {
         // Only handle move events when visible
         if (IsVisible())
         {
            HandleHoverRegions(x, y);
         }
         return false;
      });
   mOverlayPtr->SetDrawFunction(std::bind(&PageTactical::Draw, this));

   // Click region for the range increase function
   ClickRegion rangeIncrease;
   rangeIncrease.rect[Page::Medium] = QRectF(-0.425, 0.36, 0.06, 0.06);
   rangeIncrease.rect[Page::Large]  = QRectF(-0.425, 0.56, 0.06, 0.06);
   rangeIncrease.rect[Page::Full]   = QRectF(-0.45, 0.255, 0.03, 0.03);
   rangeIncrease.function           = std::bind(&PageTactical::RangeIncreaseClicked, this);
   AddClickRegion(rangeIncrease);

   // Click region for the range decrease function
   ClickRegion rangeDecrease;
   rangeDecrease.rect[Page::Medium] = QRectF(-0.425, 0.24, 0.06, 0.06);
   rangeDecrease.rect[Page::Large]  = QRectF(-0.425, 0.44, 0.06, 0.06);
   rangeDecrease.rect[Page::Full]   = QRectF(-0.45, 0.195, 0.03, 0.03);
   rangeDecrease.function           = std::bind(&PageTactical::RangeDecreaseClicked, this);
   AddClickRegion(rangeDecrease);

   // Click region for toggling the visibility of air units.
   ClickRegion toggleAir;
   toggleAir.rect[Page::Medium] = QRectF(-0.35, -0.23, 0.21, 0.07);
   toggleAir.rect[Page::Large]  = QRectF(-0.35, -0.33, 0.21, 0.07);
   toggleAir.rect[Page::Full]   = QRectF(-0.43875, -0.115, 0.0525, 0.035);
   toggleAir.function           = std::bind(&PageTactical::ToggleShowAir, this);
   AddClickRegion(toggleAir);

   // Click region for toggling the visibility of ground units.
   ClickRegion toggleGnd;
   toggleGnd.rect[Page::Medium] = QRectF(-0.35, -0.30, 0.21, 0.07);
   toggleGnd.rect[Page::Large]  = QRectF(-0.35, -0.40, 0.21, 0.07);
   toggleGnd.rect[Page::Full]   = QRectF(-0.43875, -0.15, 0.0525, 0.035);
   toggleGnd.function           = std::bind(&PageTactical::ToggleShowGround, this);
   AddClickRegion(toggleGnd);

   // Click region for toggling the visibility of waypoints.
   ClickRegion toggleWaypt;
   toggleWaypt.rect[Page::Medium] = QRectF(-0.35, -0.37, 0.21, 0.07);
   toggleWaypt.rect[Page::Large]  = QRectF(-0.35, -0.47, 0.21, 0.07);
   toggleWaypt.rect[Page::Full]   = QRectF(-0.4125, -0.185, 0.105, 0.035);
   toggleWaypt.function           = std::bind(&PageTactical::ToggleShowWaypoint, this);
   AddClickRegion(toggleWaypt);
}

void HDD::PageTactical::RangeIncreaseClicked()
{
   switch (mRange_nm)
   {
   case 5:
      mRange_nm = 10;
      break;
   case 10:
      mRange_nm = 20;
      break;
   case 20:
      mRange_nm = 40;
      break;
   case 40:
      mRange_nm = 80;
      break;
   case 80:
      mRange_nm = 160;
      break;
   case 160:
      mRange_nm = 320;
      break;
   case 320:
      mRange_nm = 640;
      break;
   default:
      break;
   }
}

void HDD::PageTactical::RangeDecreaseClicked()
{
   switch (mRange_nm)
   {
   case 640:
      mRange_nm = 320;
      break;
   case 320:
      mRange_nm = 160;
      break;
   case 160:
      mRange_nm = 80;
      break;
   case 80:
      mRange_nm = 40;
      break;
   case 40:
      mRange_nm = 20;
      break;
   case 20:
      mRange_nm = 10;
      break;
   case 10:
      mRange_nm = 5;
      break;
   default:
      break;
   }
}

void HDD::PageTactical::Draw()
{
   double scale   = 1.0;
   mCompassRadius = 0.375;
   mCompassX      = 0.0;
   mCompassY      = 0.05;

   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);
   osg::ref_ptr<osg::Group> compassGroup(new osg::Group);

   // Draw the background
   mOverlayPtr->DrawBackground(Color::Black);
   mOverlayPtr->DrawOutline(Color::Gray);

   // If the page is small, draw it's label in the bottom center
   if (mSize == Page::Small)
   {
      DrawSelected(vectorGroup, 0.0, 0.45, 0.9, 0.15, scale);
      DrawArmed(vectorGroup, 0.0, 0.25, 0.9, 0.15, scale);
      DrawCountermeasures(vectorGroup, 0.0, 0.0, 0.9, 0.3, scale);
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
      DrawSelected(vectorGroup, 0.0, 0.6, 0.3, 0.1, scale);
      DrawArmed(vectorGroup, 0.325, 0.6, 0.3, 0.1, scale);
      DrawCompass(vectorGroup, mCompassX, mCompassY, mCompassRadius, 0.5);
      DrawHeading(vectorGroup, 0.0, 0.475, 0.05, 0.04, scale);
      DrawRangeIndicator(vectorGroup, -0.425, 0.3, 0.06, 0.18, scale);
      DrawVisibilityOptions(vectorGroup, -0.35, -0.3, 0.21, 0.21, scale);

      // If we are mousing over an entity, show its info.
      if (mShowingEntityInfo)
      {
         DrawEntityInfo(vectorGroup, 0.30, -0.20, 0.35, 0.30, 0.5);
      }

      // Compass stencil buffer region array.
      osg::ref_ptr<osg::Vec2Array> compassStencilArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = mCompassRadius * cos(ang_rad);
         double y       = mCompassRadius * sin(ang_rad);
         compassStencilArray->push_back(osg::Vec2d((x + mCompassX) * pW(), (y + mCompassY) * pH()));
      }

      DrawStencilBufferRegion(compassGroup, compassStencilArray, 1); // Enable Stencil Buffer
      DrawEntities(compassGroup, mCompassX, mCompassY);
      DrawWaypoints(compassGroup, mCompassX, mCompassY, 0.5);
      DrawStencilBufferRegion(compassGroup, compassStencilArray, 0); // Release Stencil Buffer
   }
   else if (mSize == Page::Large)
   {
      DrawSelected(vectorGroup, 0.0, 0.88125, 0.3, 0.1, scale);
      DrawArmed(vectorGroup, 0.325, 0.88125, 0.3, 0.1, scale);
      DrawCompass(vectorGroup, mCompassX, mCompassY, mCompassRadius, 0.5);
      DrawHeading(vectorGroup, 0.0, 0.475, 0.05, 0.04, scale);
      DrawRangeIndicator(vectorGroup, -0.425, 0.5, 0.06, 0.18, scale);
      DrawVisibilityOptions(vectorGroup, -0.35, -0.4, 0.21, 0.21, scale);

      // If we are mousing over an entity, show its info.
      if (mShowingEntityInfo)
      {
         DrawEntityInfo(vectorGroup, 0.30, -0.20, 0.35, 0.30, 0.5);
      }

      // Compass stencil buffer region array.
      osg::ref_ptr<osg::Vec2Array> compassStencilArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = mCompassRadius * cos(ang_rad);
         double y       = mCompassRadius * sin(ang_rad);
         compassStencilArray->push_back(osg::Vec2d((x + mCompassX) * pW(), (y + mCompassY) * pH()));
      }

      DrawStencilBufferRegion(compassGroup, compassStencilArray, 1); // Enable Stencil Buffer
      DrawEntities(compassGroup, mCompassX, mCompassY);
      DrawWaypoints(compassGroup, mCompassX, mCompassY, 0.5);
      DrawStencilBufferRegion(compassGroup, compassStencilArray, 0); // Release Stencil Buffer
   }
   else if (mSize == Page::Full)
   {
      scale = 0.5;

      DrawSelected(vectorGroup, 0.4125, 0.375, 0.15, 0.05, scale);
      DrawArmed(vectorGroup, 0.4125, 0.4375, 0.15, 0.05, scale);
      DrawCompass(vectorGroup, mCompassX, mCompassY, mCompassRadius, scale);
      DrawHeading(vectorGroup, 0.0, 0.45, 0.05, 0.04, scale);
      DrawRangeIndicator(vectorGroup, -0.45, 0.225, 0.03, 0.09, scale);
      DrawVisibilityOptions(vectorGroup, -0.4125, -0.15, 0.105, 0.105, scale);

      // If we are mousing over an entity, show its info.
      if (mShowingEntityInfo)
      {
         DrawEntityInfo(vectorGroup, 0.30, -0.20, 0.35, 0.30, scale);
      }

      // Compass stencil buffer region array.
      osg::ref_ptr<osg::Vec2Array> compassStencilArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = mCompassRadius * cos(ang_rad);
         double y       = mCompassRadius * sin(ang_rad);
         compassStencilArray->push_back(osg::Vec2d((x + mCompassX) * pW(), (y + mCompassY) * pH()));
      }

      DrawStencilBufferRegion(compassGroup, compassStencilArray, 1); // Enable Stencil Buffer
      DrawEntities(compassGroup, mCompassX, mCompassY);
      DrawWaypoints(compassGroup, mCompassX, mCompassY, scale);
      DrawStencilBufferRegion(compassGroup, compassStencilArray, 0); // Release Stencil Buffer
   }

   // If we're drawing click regions (debugging)
   if (mDrawClickRegions)
   {
      DrawClickRegions(vectorGroup.get());
   }

   // Add the finished drawing
   mOverlayPtr->GetGeode()->addChild(compassGroup.get());
   mOverlayPtr->GetGeode()->addChild(vectorGroup.get());
}

void HDD::PageTactical::DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                                            const double             aX,
                                            const double             aY,
                                            const double             aW,
                                            const double             aH,
                                            const double             aScale)
{
   float fontSize = 0.09f * pW();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Countermeasures
   // Chaff
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("CHAFF", -(aW * 0.5) * pW(), 0.1 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream chaff;
   chaff << std::setw(3) << std::setfill('0') << mData.chaff;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(chaff.str(), (aW * 0.5) * pW(), 0.1 * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   // Decoys
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("DECOYS", -(aW * 0.5) * pW(), 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream decoys;
   decoys << std::setw(3) << std::setfill('0') << mData.decoys;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(decoys.str(), (aW * 0.5) * pW(), 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   // Flares
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("FLARES", -(aW * 0.5) * pW(), -0.1 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER)
         .get());
   std::stringstream flares;
   flares << std::setw(3) << std::setfill('0') << mData.flares;
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem(flares.str(), (aW * 0.5) * pW(), -0.1 * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTactical::DrawClickRegions(osg::ref_ptr<osg::Group> aVectorGroup)
{
   for (auto region : mClickRegions)
   {
      if (region.rect.find(mSize) != region.rect.end())
      {
         QRectF r = region.rect.at(mSize);

         // Transformation
         osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
         vectorXform->setPosition(osg::Vec3d(r.x() * pW(), r.y() * pH(), 0.0));

         // Box rect
         osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
         boxArray->push_back(osg::Vec2d(-(r.width() * 0.5) * pW(), (r.height() * 0.5) * pH()));
         boxArray->push_back(osg::Vec2d((r.width() * 0.5) * pW(), (r.height() * 0.5) * pH()));
         boxArray->push_back(osg::Vec2d((r.width() * 0.5) * pW(), -(r.height() * 0.5) * pH()));
         boxArray->push_back(osg::Vec2d(-(r.width() * 0.5) * pW(), -(r.height() * 0.5) * pH()));
         vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, Color::Green).get());
         aVectorGroup->addChild(vectorXform.get());
      }
   }
}

void HDD::PageTactical::DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup,
                                    const double             aX,
                                    const double             aY,
                                    const double             aR,
                                    const double             aScale)
{
   float fontSize = 0.035f * pW() * aScale;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Draw the rings
   double distBetweenRings = aR / 4;
   for (int r = ((mRange_nm > 10) ? 1 : 2); r <= 4; (mRange_nm > 10) ? r++ : r += 2)
   {
      osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
         double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);
         ringArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
      vectorXform->addChild(mOverlayPtr->CreateGeometry(ringArray, GL_LINE_LOOP, Color::LightGray).get());

      // Draw the range in nm on each ring
      std::stringstream nm;
      // Draw 2.5 instead of 3, otherwise just draw ints
      if ((mRange_nm * (r / 4.0)) < 5)
      {
         nm << std::fixed << std::setw(2) << std::setprecision(1);
      }
      nm << (mRange_nm * (r / 4.0));
      double ang_rad = 60.0 * UtMath::cRAD_PER_DEG;
      double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
      double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);

      // We need a small black circle so the range is easy to see
      osg::ref_ptr<osg::Vec2Array> blackCircleArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double cX      = 0.015 * cos(ang_rad);
         double cY      = 0.015 * sin(ang_rad);
         blackCircleArray->push_back(osg::Vec2d((x + cX) * pW(), (y + cY) * pH()));
      }
      // Draw a black circle with the range inside of it
      auto circle = mOverlayPtr->CreateGeometry(blackCircleArray, GL_POLYGON, Color::Black);
      circle->addChild(
         mOverlayPtr->AddTextItem(nm.str(), x * pW(), y * pH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
      vectorXform->addChild(circle.get());
   }

   // Orient based on the heading
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorHeadingXForm(new osg::PositionAttitudeTransform());
   vectorHeadingXForm->setAttitude(osg::Quat((mData.heading_deg + 90) * UtMath::cRAD_PER_DEG, osg::Vec3(0.0f, 0.0f, 1.0f)));

   // Parts that should rotate with the heading add to vectorXForm
   // Ticks every 10 degrees around the large ring.
   osg::ref_ptr<osg::Vec2Array> angleTicksArray(new osg::Vec2Array);
   double                       tickLength = 0.02;
   double                       tickRadius = (distBetweenRings * ((mRange_nm > 10) ? 3 : 4)) - (tickLength * 0.5);

   // Draw a tick every 10 degrees, the angle number every 30, and the cardinal direction every 90.
   for (int ang_deg = 0; ang_deg > -360; ang_deg -= 10)
   {
      int pos_deg = -ang_deg;
      if (pos_deg == 0 || (pos_deg % 30 == 0))
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = (tickRadius - (tickLength * 0.5)) * cos(ang_rad);
         double y       = (tickRadius - (tickLength * 0.5)) * sin(ang_rad);
         switch (pos_deg)
         {
         case 0:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("N", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 30:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("30", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 60:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("60", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 90:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("E", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 120:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("120", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 150:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("150", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 180:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("S", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 210:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("210", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 240:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("240", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 270:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("W", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 300:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("300", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 330:
            vectorHeadingXForm->addChild(
               mOverlayPtr->AddTextItem("330", x * pW(), y * pH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         default:
            break;
         }
      }
      else
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = tickRadius * cos(ang_rad);
         double x2      = (tickRadius - tickLength) * cos(ang_rad);
         double y       = tickRadius * sin(ang_rad);
         double y2      = (tickRadius - tickLength) * sin(ang_rad);
         angleTicksArray->push_back(osg::Vec2d(x * pW(), y * pH()));
         angleTicksArray->push_back(osg::Vec2d(x2 * pW(), y2 * pH()));
      }
   }
   vectorHeadingXForm->addChild(mOverlayPtr->CreateGeometry(angleTicksArray, GL_LINES, Color::Gray).get());
   vectorXform->addChild(vectorHeadingXForm);

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform);

   // Draw the ship and FOV lines
   DrawOwnshipFOV(aVectorGroup, aX, aY, aR);
}

void HDD::PageTactical::DrawWaypoints(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aScale)
{
   if (mShowingWaypoint)
   {
      float fontSize = 0.05f * pW() * aScale;

      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

      mOverlayPtr->SetLineWidthMultiplier(1.5f);

      QPointF lastPos;
      for (size_t i = 0; i < mData.navData.waypointData.waypoints.size(); i++)
      {
         // Get the heading and distance to the waypoint.
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.lat_deg,
                                                         mData.lon_deg,
                                                         mData.navData.waypointData.waypoints[i].lat_deg,
                                                         mData.navData.waypointData.waypoints[i].lon_deg,
                                                         heading_deg,
                                                         distance_m);

         // Calculate the position of the current waypoint on the compass.
         const QPointF pos =
            CalculatePositionFromBearingAndRange(heading_deg * UtMath::cRAD_PER_DEG, distance_m * UtMath::cNM_PER_M);

         // Circle array
         osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
         for (int ang_deg = 0; ang_deg <= 360; ang_deg += 18)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double rX      = 0.015 * cos(ang_rad);
            double rY      = 0.015 * sin(ang_rad);
            circleArray->push_back(
               osg::Vec2d(((pos.x() * UnitsPerNm()) + rX) * pW(), ((pos.y() * UnitsPerNm()) + rY) * pH()));
         }
         // Draw a gray circle with the waypoint number inside.
         vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_LINE_LOOP, Color::Gray));
         vectorXform->addChild(mOverlayPtr
                                  ->AddTextItem(std::to_string(i),
                                                pos.x() * UnitsPerNm() * pW(),
                                                pos.y() * UnitsPerNm() * pH(),
                                                fontSize,
                                                Color::Gray,
                                                osgText::Text::CENTER_CENTER)
                                  .get());

         // Draw a line from the last waypoint to this one. (Skip first waypoint)
         if (i > 0)
         {
            double xDist    = pos.x() - lastPos.x();
            double yDist    = pos.y() - lastPos.y();
            double line_ang = atan2(yDist, xDist);
            double rX       = 0.015 * cos(line_ang);
            double rY       = 0.015 * sin(line_ang);

            osg::ref_ptr<osg::Vec2Array> lineArray(new osg::Vec2Array);
            // Calculate where the line would go on the edge of the circle, rather than draw it
            // in the center of the waypoint circle.
            double lX = ((lastPos.x() * UnitsPerNm()) + rX) * pW();
            double lY = ((lastPos.y() * UnitsPerNm()) + rY) * pH();
            double nX = ((pos.x() * UnitsPerNm()) - rX) * pW();
            double nY = ((pos.y() * UnitsPerNm()) - rY) * pH();
            lineArray->push_back(osg::Vec2d(lX, lY));
            lineArray->push_back(osg::Vec2d(nX, nY));
            vectorXform->addChild(mOverlayPtr->CreateGeometry(lineArray, GL_LINES, Color::Gray));
         }

         // Set the last position so we can draw a line to the next waypoint.
         lastPos = pos;
      }

      mOverlayPtr->SetLineWidthMultiplier(1.0f);

      vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
      aVectorGroup->addChild(vectorXform);
   }
}

void HDD::PageTactical::DrawHeading(osg::ref_ptr<osg::Group> aVectorGroup,
                                    const double             aX,
                                    const double             aY,
                                    const double             aW,
                                    const double             aH,
                                    const double             aScale)
{
   float fontSize = 0.06f * pW() * aScale;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Heading
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

   // Draw the heading
   std::stringstream heading;
   heading << std::setw(3) << std::setfill('0') << hdg_deg;
   vectorXform->addChild(
      mOverlayPtr->AddTextItem(heading.str(), 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTactical::DrawOwnshipFOV(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // FOV Lines
   double                       halfFov = mData.fov_deg / 2.0;
   double                       ang_rad = halfFov * UtMath::cRAD_PER_DEG;
   double                       x       = aR * sin(ang_rad);
   double                       y       = aR * cos(ang_rad);
   osg::ref_ptr<osg::Vec2Array> fovLinesArray(new osg::Vec2Array);
   fovLinesArray->push_back(osg::Vec2d(0.0, 0.0));
   fovLinesArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   fovLinesArray->push_back(osg::Vec2d(0.0, 0.0));
   fovLinesArray->push_back(osg::Vec2d(-x * pW(), y * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(fovLinesArray, GL_LINES, Color::SlateBlue).get());

   // Draw the aircraft in the center
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d(0.0, 0.009375 * pH()));
   linesArray->push_back(osg::Vec2d(0.0, -0.021125 * pH()));
   linesArray->push_back(osg::Vec2d(-0.01375 * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(0.01375 * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(-0.0078125 * pW(), -0.015625 * pH()));
   linesArray->push_back(osg::Vec2d(0.0078125 * pW(), -0.015625 * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(linesArray, GL_LINES, Color::White).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform);
}

osg::Vec4 HDD::PageTactical::GetEntityColor(const Entity& aEntity)
{
   if (aEntity.inFlight)
   {
      return Color::SlateBlue;
   }

   switch (aEntity.track.identification)
   {
   default:
   case SimData::ID_UNKNOWN:
   case SimData::ID_BOGIE:
   {
      return Color::Yellow;
   }
   case SimData::ID_NEUTRAL:
   {
      return Color::White;
   }
   case SimData::ID_FRIENDLY:
   {
      return Color::Green;
   }
   case SimData::ID_BANDIT:
   {
      return Color::Red;
   }
   }
}

bool HDD::PageTactical::IsEntityAngleOnly(const Entity& aEntity)
{
   if (aEntity.track.targetHdgValid)
   {
      return false;
   }
   else if (aEntity.track.bearingValid)
   {
      return true;
   }
   return false;
}

void HDD::PageTactical::DrawAngleOnly(osg::ref_ptr<osg::Group> aVectorGroup,
                                      Entity&                  aEntity,
                                      const double             aX,
                                      const double             aY,
                                      const double             aW,
                                      const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   vectorXform->setAttitude(osg::Quat(aEntity.track.tgtBearing_rad - ((mData.heading_deg + 180.0) * UtMath::cRAD_PER_DEG),
                                      osg::Vec3(0.0f, 0.0f, -1.0f)));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Entity triangle
   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
   entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
   entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));

   // Check for missiles, which are drawn with filled polygon, rather than hollow polygon
   if (aEntity.track.isMissile)
   {
      // This is a missile, so use a filled polygon
      vectorXform->addChild(mOverlayPtr->CreateGeometry(entityArray, GL_POLYGON, GetEntityColor(aEntity)).get());
   }
   else
   {
      // This is NOT a missile, so use a hollow polygon
      vectorXform->addChild(mOverlayPtr->CreateGeometry(entityArray, GL_LINE_LOOP, GetEntityColor(aEntity)).get());
   }

   aVectorGroup->addChild(vectorXform);

   mOverlayPtr->SetLineWidthMultiplier(1.0f);
}

void HDD::PageTactical::DrawBogie(osg::ref_ptr<osg::Group> aVectorGroup,
                                  Entity&                  aEntity,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Box outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Yellow).get());

   // Half filled box
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.25) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.25) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxFillArray, GL_POLYGON, Color::Yellow).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(radarArray, GL_LINES, Color::Yellow).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawBogieMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                         Entity&                  aEntity,
                                         const double             aX,
                                         const double             aY,
                                         const double             aW,
                                         const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Box
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.25) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.25) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_POLYGON, Color::Yellow).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(arrowLines, GL_LINES, Color::Yellow).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(middleLine, GL_LINES, Color::Yellow).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawNeutral(osg::ref_ptr<osg::Group> aVectorGroup,
                                    Entity&                  aEntity,
                                    const double             aX,
                                    const double             aY,
                                    const double             aW,
                                    const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Box outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::White).get());

   // Half filled box
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.25) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.25) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxFillArray, GL_POLYGON, Color::White).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(radarArray, GL_LINES, Color::White).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawNeutralMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                           Entity&                  aEntity,
                                           const double             aX,
                                           const double             aY,
                                           const double             aW,
                                           const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Box
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.25) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.25) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_POLYGON, Color::White).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(arrowLines, GL_LINES, Color::White).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(middleLine, GL_LINES, Color::White).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawBandit(osg::ref_ptr<osg::Group> aVectorGroup,
                                   Entity&                  aEntity,
                                   const double             aX,
                                   const double             aY,
                                   const double             aW,
                                   const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Triangle outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Red).get());

   // 40% filled triangle looks better than half fill
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.3) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.3) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxFillArray, GL_POLYGON, Color::Red).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(radarArray, GL_LINES, Color::Red).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawBanditMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                          Entity&                  aEntity,
                                          const double             aX,
                                          const double             aY,
                                          const double             aW,
                                          const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Filled Triangle
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.25) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.25) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_POLYGON, Color::Red).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(arrowLines, GL_LINES, Color::Red).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(middleLine, GL_LINES, Color::Red).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawGroundThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                                         Entity&                  aEntity,
                                         const double             aX,
                                         const double             aY,
                                         const double             aW,
                                         const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   // We don't rotate ground units
   // vectorXform->setAttitude(osg::Quat(aEntity.trackData.tgtHdg_rad - mData.heading, osg::Vec3(0.0f, 0.0f, -1.0f)));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Triangle outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Red).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(radarArray, GL_LINES, Color::Red).get());

   // Calculate the distance in coordinate space per nm, then multiply that times the entities range radius.
   double range = UnitsPerNm() * aEntity.range;

   // Range ring
   osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = range * cos(ang_rad);
      double y       = range * sin(ang_rad);
      ringArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(ringArray, GL_LINE_LOOP, Color::Red).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTactical::DrawAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                                  Entity&                  aEntity,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   if (aEntity.type == TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == ASSET)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.perceivedData.heading_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Circle outline
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_LINE_LOOP, Color::Green).get());

   // Half filled circle
   osg::ref_ptr<osg::Vec2Array> circleFillArray(new osg::Vec2Array);
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleFillArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleFillArray, GL_POLYGON, Color::Green).get());

   // Radar dish and heading lines
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(radarArray, GL_LINES, Color::Green).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                         Entity&                  aEntity,
                                         const double             aX,
                                         const double             aY,
                                         const double             aW,
                                         const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   if (aEntity.type == TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == ASSET)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.perceivedData.heading_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Circle
   double                       r = (aW * 0.25);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_POLYGON, Color::Green).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(arrowLines, GL_LINES, Color::Green).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(middleLine, GL_LINES, Color::Green).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawFlightAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                                        Entity&                  aEntity,
                                        const double             aX,
                                        const double             aY,
                                        const double             aW,
                                        const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   if (aEntity.type == TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == ASSET)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.perceivedData.heading_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Circle outline
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_LINE_LOOP, Color::SlateBlue).get());

   // Half filled circle
   osg::ref_ptr<osg::Vec2Array> circleFillArray(new osg::Vec2Array);
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleFillArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleFillArray, GL_POLYGON, Color::SlateBlue).get());

   // Radar dish and heading lines
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(radarArray, GL_LINES, Color::SlateBlue).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawFlightAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                               Entity&                  aEntity,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   if (aEntity.type == TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == ASSET)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.perceivedData.heading_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(mOverlayPtr->CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Circle
   double                       r = (aW * 0.25);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(circleArray, GL_POLYGON, Color::SlateBlue).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(arrowLines, GL_LINES, Color::SlateBlue).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(middleLine, GL_LINES, Color::SlateBlue).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   // Transformation - Shift the region to match the compass
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Draw our entities based on type and domain
   for (auto& entity : mEntities)
   {
      const QPointF pos = CalculateEntityPosition(entity);

      switch (entity.track.identification)
      {
      case SimData::ID_UNKNOWN:
      case SimData::ID_BOGIE:
      {
         if (mShowingAir)
         {
            if (IsEntityAngleOnly(entity))
            {
               DrawAngleOnly(vectorXform, entity, pos.x(), pos.y(), 0.03, 0.03);
            }
            else
            {
               if (entity.track.isMissile)
               {
                  DrawBogieMissile(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
               }
               else
               {
                  DrawBogie(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
               }
            }
         }
         break;
      }
      case SimData::ID_NEUTRAL:
      {
         if (mShowingAir)
         {
            if (IsEntityAngleOnly(entity))
            {
               DrawAngleOnly(vectorXform, entity, pos.x(), pos.y(), 0.03, 0.03);
            }
            else
            {
               if (entity.track.isMissile)
               {
                  DrawNeutralMissile(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
               }
               else
               {
                  DrawNeutral(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
               }
            }
         }
         break;
      }
      case SimData::ID_FRIENDLY:
      {
         if (mShowingAir)
         {
            if (entity.inFlight)
            {
               if (IsEntityAngleOnly(entity))
               {
                  DrawAngleOnly(vectorXform, entity, pos.x(), pos.y(), 0.03, 0.03);
               }
               else
               {
                  if (entity.track.isMissile)
                  {
                     DrawFlightAssetMissile(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
                  }
                  else
                  {
                     DrawFlightAsset(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
                  }
               }
            }
            else
            {
               if (IsEntityAngleOnly(entity))
               {
                  DrawAngleOnly(vectorXform, entity, pos.x(), pos.y(), 0.03, 0.03);
               }
               else
               {
                  if (entity.track.isMissile)
                  {
                     DrawAssetMissile(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
                  }
                  else
                  {
                     DrawAsset(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
                  }
               }
            }
         }
         break;
      }
      case SimData::ID_BANDIT:
      {
         if (mShowingAir && entity.domain == EntityDomain::AIR)
         {
            if (IsEntityAngleOnly(entity))
            {
               DrawAngleOnly(vectorXform, entity, pos.x(), pos.y(), 0.03, 0.03);
            }
            else
            {
               if (entity.track.isMissile)
               {
                  DrawBanditMissile(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
               }
               else
               {
                  DrawBandit(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
               }
            }
         }
         else if (mShowingGround && entity.domain == EntityDomain::GROUND)
         {
            if (entity.range != -1.0)
            {
               DrawGroundThreat(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.04);
            }
         }
         break;
      }
      }
   }

   for (auto& asset : mAssets)
   {
      const QPointF pos = CalculateEntityPosition(asset);

      if (mShowingAir)
      {
         if (asset.perceivedData.flightId == mData.flightId)
         {
            if (asset.perceivedData.isMissile)
            {
               DrawFlightAssetMissile(vectorXform, asset, pos.x(), pos.y(), 0.02, 0.04);
            }
            else
            {
               DrawFlightAsset(vectorXform, asset, pos.x(), pos.y(), 0.02, 0.04);
            }
         }
         else
         {
            if (asset.perceivedData.isMissile)
            {
               DrawAssetMissile(vectorXform, asset, pos.x(), pos.y(), 0.02, 0.04);
            }
            else
            {
               DrawAsset(vectorXform, asset, pos.x(), pos.y(), 0.02, 0.04);
            }
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTactical::DrawArmed(osg::ref_ptr<osg::Group> aVectorGroup,
                                  const double             aX,
                                  const double             aY,
                                  const double             aW,
                                  const double             aH,
                                  const double             aScale)
{
   float fontSize =
      0.07f * pW() * ((mSize == Page::Small) ? 2.0 : 1.0) * aScale; // If we're on the small page, double the font size

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));

   // Set the border color for the master arm box.
   // While the weapon is in safe mode, set to cyan.
   // While armed, set to green.
   osg::Vec4 borderColor;
   if (mData.masterArmActive)
   {
      borderColor = Color::Green;
   }
   else
   {
      borderColor = Color::Cyan;
   }
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, borderColor).get());

   if (mData.masterArmActive)
   {
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("ARMED", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         mOverlayPtr->AddTextItem("SAFE", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTactical::DrawSelected(osg::ref_ptr<osg::Group> aVectorGroup,
                                     const double             aX,
                                     const double             aY,
                                     const double             aW,
                                     const double             aH,
                                     const double             aScale)
{
   if (!mData.selectedWeapon.empty())
   {
      float fontSize = 0.04f * pW() * ((mSize == Page::Small) ? 2.0 : 1.0) *
                       aScale; // If we're on the small page, double the font size

      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

      // Box rect
      osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
      boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
      boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
      boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));

      // Set the border color for the selected weapon box.
      // While the weapon is in safe mode, set to cyan.
      // While armed, set to green.
      // If the selected weapon is out of ammo, set to red.
      osg::Vec4 borderColor;
      if (mData.masterArmActive)
      {
         borderColor = Color::Green;
      }
      else
      {
         borderColor = Color::Cyan;
      }
      if (mData.numSelectedWeapon <= 0)
      {
         borderColor = Color::Red;
      }
      vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, borderColor).get());

      std::string weaponName = mData.selectedWeapon;
      for (auto& c : weaponName)
      {
         c = toupper(c);
      }
      if (mSize == Page::Small)
      {
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem("SEL:", -(aW * 0.45) * pW(), 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER)
               .get());
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(weaponName, (aW * 0.45) * pW(), 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }
      else
      {
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem("SELECTED:", 0.0, (aH * 0.35) * pH(), fontSize, Color::White, osgText::Text::CENTER_TOP)
               .get());
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(weaponName, 0.0, -(aH * 0.35) * pH(), fontSize, Color::White, osgText::Text::CENTER_BOTTOM)
               .get());
      }

      aVectorGroup->addChild(vectorXform.get());
   }
}

void HDD::PageTactical::DrawRangeIndicator(osg::ref_ptr<osg::Group> aVectorGroup,
                                           const double             aX,
                                           const double             aY,
                                           const double             aW,
                                           const double             aH,
                                           const double             aScale)
{
   float fontSize = 0.045f * pW() * aScale;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Up range triangle
   osg::ref_ptr<osg::Vec2Array> upTriArray(new osg::Vec2Array);
   upTriArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
   upTriArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.17) * pH()));
   upTriArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.17) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(upTriArray, GL_POLYGON, Color::Cyan).get());

   // Down range triangle
   osg::ref_ptr<osg::Vec2Array> downTriArray(new osg::Vec2Array);
   downTriArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
   downTriArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.17) * pH()));
   downTriArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.17) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(downTriArray, GL_POLYGON, Color::Cyan).get());

   vectorXform->addChild(
      mOverlayPtr->AddTextItem(std::to_string(mRange_nm), 0.0, 0.0, fontSize, Color::Cyan, osgText::Text::CENTER_CENTER).get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                const double             aX,
                                const double             aY,
                                const double             aW,
                                const double             aH,
                                const double             aScale)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_POLYGON, osg::Vec4(0.1, 0.0, 0.0, 1.0)).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void HDD::PageTactical::DrawVisibilityOptions(osg::ref_ptr<osg::Group> aVectorGroup,
                                              const double             aX,
                                              const double             aY,
                                              const double             aW,
                                              const double             aH,
                                              const double             aScale)
{
   float fontSize = 0.06f * pW() * aScale;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("AIR", -(aW * 0.5) * pW(), (aH * 0.5) * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_TOP)
         .get());
   vectorXform->addChild(
      mOverlayPtr->AddTextItem("GND", -(aW * 0.5) * pW(), 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      mOverlayPtr
         ->AddTextItem("WAYPT", -(aW * 0.5) * pW(), -(aH * 0.5) * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_BOTTOM)
         .get());

   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   aVectorGroup->addChild(vectorXform);
}

void HDD::PageTactical::DrawEntityInfo(osg::ref_ptr<osg::Group> aVectorGroup,
                                       const double             aX,
                                       const double             aY,
                                       const double             aW,
                                       const double             aH,
                                       const double             aScale)
{
   float fontSize = 0.06f * pW() * aScale;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   mOverlayPtr->SetLineWidthMultiplier(1.5f);

   // Box fill rect
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxFillArray, GL_POLYGON, Color::Black).get());

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
   vectorXform->addChild(mOverlayPtr->CreateGeometry(boxArray, GL_LINE_LOOP, Color::White).get());
   mOverlayPtr->SetLineWidthMultiplier(1.0f);

   if (mCurrentEntity.type == TRACK)
   {
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("TGT:", -(aW * 0.45) * pW(), (aH * 0.4) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      // Shrink the name size if necessary (long names)
      if (mCurrentEntity.track.tgtPlatformName.size() > 13)
      {
         fontSize = (13 * fontSize / mCurrentEntity.track.tgtPlatformName.size());
      }
      std::stringstream tgt;
      tgt << mCurrentEntity.track.tgtPlatformName;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(tgt.str(), (aW * 0.45) * pW(), (aH * 0.4) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.06f * pW() * aScale;

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("ID:", -(aW * 0.45) * pW(), (aH * 0.3) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      std::stringstream id;
      // Shrink the id size if necessary (long ids)
      if (mCurrentEntity.track.tgtIdString.size() > 14)
      {
         fontSize = (14 * fontSize / mCurrentEntity.track.tgtIdString.size());
      }
      id << mCurrentEntity.track.tgtIdString;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(id.str(), (aW * 0.45) * pW(), (aH * 0.3) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.06f * pW() * aScale;

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("ALT:", -(aW * 0.45) * pW(), (aH * 0.2) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.track.altitudeValid)
      {
         std::stringstream alt;
         alt << std::fixed << std::setprecision(0) << std::setw(5) << std::setfill('0') << mCurrentEntity.track.tgtAlt_ft;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(alt.str(), (aW * 0.45) * pW(), (aH * 0.2) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("SPD:", -(aW * 0.45) * pW(), (aH * 0.1) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.track.targetSpeedValid)
      {
         std::stringstream spd;
         spd << std::fixed << std::setprecision(0) << std::setw(5) << mCurrentEntity.track.tgtSpeed_kts;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(spd.str(), (aW * 0.45) * pW(), (aH * 0.1) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr->AddTextItem("HDG:", -(aW * 0.45) * pW(), 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
      if (mCurrentEntity.track.targetHdgValid)
      {
         double heading_deg = mCurrentEntity.track.tgtHdg_deg;
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
         std::stringstream hdg;
         hdg << std::setw(3) << std::setfill('0') << hdg_deg;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(hdg.str(), (aW * 0.45) * pW(), 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("RNG:", -(aW * 0.45) * pW(), (aH * -0.1) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.track.rangeValid)
      {
         std::stringstream rng;
         rng << std::fixed << std::setprecision(0) << std::setw(5) << mCurrentEntity.track.tgtRange_nm;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(rng.str(), (aW * 0.45) * pW(), (aH * -0.1) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("RATE:", -(aW * 0.45) * pW(), (aH * -0.2) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.track.rangeRateValid)
      {
         std::stringstream rate;
         rate << std::fixed << std::setprecision(0) << std::setw(5) << mCurrentEntity.track.tgtRangeRate_kts;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(rate.str(), (aW * 0.45) * pW(), (aH * -0.2) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("ASPECT:", -(aW * 0.45) * pW(), (aH * -0.3) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.track.targetAspectValid)
      {
         std::stringstream aspect;
         aspect << std::fixed << std::setprecision(0) << std::setw(4) << mCurrentEntity.track.tgtAspect_deg;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(aspect.str(), (aW * 0.45) * pW(), (aH * -0.3) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }
   }
   else if (mCurrentEntity.type == ASSET)
   {
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("TGT:", -(aW * 0.45) * pW(), (aH * 0.4) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      // Shrink the name size if necessary (long names)
      if (mCurrentEntity.perceivedData.perceivedName.size() > 13)
      {
         fontSize = (13 * fontSize / mCurrentEntity.perceivedData.perceivedName.size());
      }
      std::stringstream tgt;
      tgt << mCurrentEntity.perceivedData.perceivedName;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(tgt.str(), (aW * 0.45) * pW(), (aH * 0.4) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.06f * pW() * aScale;

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("ID:", -(aW * 0.45) * pW(), (aH * 0.3) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      std::stringstream id;
      // Shrink the id size if necessary (long ids)
      if (mCurrentEntity.perceivedData.idString.size() > 14)
      {
         fontSize = (14 * fontSize / mCurrentEntity.perceivedData.idString.size());
      }
      id << mCurrentEntity.perceivedData.idString;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(id.str(), (aW * 0.45) * pW(), (aH * 0.3) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.06f * pW() * aScale;

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("ALT:", -(aW * 0.45) * pW(), (aH * 0.2) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.perceivedData.altitudeValid)
      {
         std::stringstream alt;
         alt << std::fixed << std::setprecision(0) << std::setw(5) << std::setfill('0')
             << mCurrentEntity.perceivedData.altitude_ft;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(alt.str(), (aW * 0.45) * pW(), (aH * 0.2) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("SPD:", -(aW * 0.45) * pW(), (aH * 0.1) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mCurrentEntity.perceivedData.speedValid)
      {
         std::stringstream spd;
         spd << std::fixed << std::setprecision(0) << std::setw(5) << mCurrentEntity.perceivedData.speed_kts;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(spd.str(), (aW * 0.45) * pW(), (aH * 0.1) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr->AddTextItem("HDG:", -(aW * 0.45) * pW(), 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());

      if (mCurrentEntity.perceivedData.headingValid)
      {
         double heading_deg = mCurrentEntity.perceivedData.heading_deg;
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
         std::stringstream hdg;
         hdg << std::setw(3) << std::setfill('0') << hdg_deg;
         vectorXform->addChild(
            mOverlayPtr
               ->AddTextItem(hdg.str(), (aW * 0.45) * pW(), 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("RNG:", -(aW * 0.45) * pW(), (aH * -0.1) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.lat_deg,
                                                      mData.lon_deg,
                                                      mCurrentEntity.perceivedData.lat_deg,
                                                      mCurrentEntity.perceivedData.lon_deg,
                                                      heading_deg,
                                                      distance_m);
      std::stringstream rng;
      rng << std::fixed << std::setprecision(0) << std::setw(5) << distance_m * UtMath::cNM_PER_M;
      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem(rng.str(), (aW * 0.45) * pW(), (aH * -0.1) * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("RATE:", -(aW * 0.45) * pW(), (aH * -0.2) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());

      vectorXform->addChild(
         mOverlayPtr
            ->AddTextItem("ASPECT:", -(aW * 0.45) * pW(), (aH * -0.3) * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
   }

   // Make sure the text draws on top of the polygons
   osg::StateSet* ss = vectorXform->getOrCreateStateSet();
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   aVectorGroup->addChild(vectorXform.get());
}

const QRectF HDD::PageTactical::CalculateEntityHoverRegion(Entity&      aEntity,
                                                           const double aX,
                                                           const double aY,
                                                           const double aW,
                                                           const double aH)
{
   if (aEntity.type == TRACK)
   {
      // Create a polygon with the basic width and height of the entity.
      QPolygonF poly;
      poly.append(QPointF(-(aW * 0.5), (aH * 0.5)));
      poly.append(QPointF((aW * 0.5), (aH * 0.5)));
      poly.append(QPointF((aW * 0.5), -(aH * 0.5)));
      poly.append(QPointF(-(aW * 0.5), -(aH * 0.5)));

      // Create a transform that will translate and then rotate the polygon.
      QTransform t = QTransform()
                        .translate((aX * UnitsPerNm()), (aY * UnitsPerNm()))
                        .rotateRadians(-(aEntity.track.tgtHdg_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG);
      // Translate and rotate the polygon to align with the entity.
      QPolygonF nPoly = t.map(poly);
      // Create a bounding rect surrounding the new polygon.
      QRectF bR = nPoly.boundingRect();
      // Set the hover region rect for our entity here, converting to pixel coordinates to match mouse move events.
      aEntity.hoverRegion.setRect(((bR.x() + mCompassX) * pW()) + mOverlayPtr->GetRect().center().x(),
                                  ((bR.y() + mCompassY) * pH()) + mOverlayPtr->GetRect().center().y(),
                                  bR.width() * pW(),
                                  bR.height() * pH());

      // Return the bounding rect in case it will be drawn (debugging).
      return bR;
   }
   else if (aEntity.type == ASSET)
   {
      if (aEntity.perceivedData.headingValid)
      {
         // Create a polygon with the basic width and height of the entity.
         QPolygonF poly;
         poly.append(QPointF(-(aW * 0.5), (aH * 0.5)));
         poly.append(QPointF((aW * 0.5), (aH * 0.5)));
         poly.append(QPointF((aW * 0.5), -(aH * 0.5)));
         poly.append(QPointF(-(aW * 0.5), -(aH * 0.5)));

         // Create a transform that will translate and then rotate the polygon.
         QTransform t = QTransform()
                           .translate((aX * UnitsPerNm()), (aY * UnitsPerNm()))
                           .rotateRadians(-(aEntity.perceivedData.heading_deg - mData.heading_deg) * UtMath::cRAD_PER_DEG);
         // Translate and rotate the polygon to align with the entity.
         QPolygonF nPoly = t.map(poly);
         // Create a bounding rect surrounding the new polygon.
         QRectF bR = nPoly.boundingRect();
         // Set the hover region rect for our entity here, converting to pixel
         // coordinates to match mouse move events.
         aEntity.hoverRegion.setRect(((bR.x() + mCompassX) * pW()) + mOverlayPtr->GetRect().center().x(),
                                     ((bR.y() + mCompassY) * pH()) + mOverlayPtr->GetRect().center().y(),
                                     bR.width() * pW(),
                                     bR.height() * pH());

         // Return the bounding rect in case it will be drawn (debugging)
         return bR;
      }
   }
   return QRectF();
}

const QPointF HDD::PageTactical::CalculateEntityPosition(const Entity& aEntity)
{
   if (aEntity.type == TRACK)
   {
      // If we have a range and bearing
      if (aEntity.track.rangeValid && aEntity.track.bearingValid)
      {
         return CalculatePositionFromBearingAndRange(aEntity.track.tgtBearing_rad, aEntity.track.tgtRange_nm);
      }
      // If we have a lat lon and heading
      else if (aEntity.track.latLonValid && aEntity.track.targetHdgValid)
      {
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.lat_deg,
                                                         mData.lon_deg,
                                                         aEntity.track.tgtLat_deg,
                                                         aEntity.track.tgtLon_deg,
                                                         heading_deg,
                                                         distance_m);

         return CalculatePositionFromBearingAndRange(heading_deg * UtMath::cRAD_PER_DEG, distance_m * UtMath::cNM_PER_M);
      }
      // If we have just bearing
      else if (aEntity.track.bearingValid)
      {
         return CalculatePositionFromBearingAndRange(aEntity.track.tgtBearing_rad, (mRange_nm * 1.06));
      }
   }
   else if (aEntity.type == ASSET)
   {
      // If we have a valid heading.  (Assume lat and lon is valid)
      if (aEntity.perceivedData.headingValid)
      {
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.lat_deg,
                                                         mData.lon_deg,
                                                         aEntity.perceivedData.lat_deg,
                                                         aEntity.perceivedData.lon_deg,
                                                         heading_deg,
                                                         distance_m);

         return CalculatePositionFromBearingAndRange(heading_deg * UtMath::cRAD_PER_DEG, distance_m * UtMath::cNM_PER_M);
      }
      // Else assume bearing is valid
      else
      {
         return CalculatePositionFromBearingAndRange(aEntity.perceivedData.bearing_deg * UtMath::cRAD_PER_DEG,
                                                     (mRange_nm * 1.06));
      }
   }
   return QPointF(-1.0, -1.0);
}

const QPointF HDD::PageTactical::CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm)
{
   // Calculate the position and orientation of the entity
   double az_rad = aBearingRad - (mData.heading_deg * UtMath::cRAD_PER_DEG);
   if (az_rad < -UtMath::cPI)
   {
      az_rad += UtMath::cTWO_PI;
   }
   if (az_rad > UtMath::cPI)
   {
      az_rad -= UtMath::cTWO_PI;
   }

   double tgtX_nm = aRangeNm * sin(az_rad);
   double tgtY_nm = aRangeNm * cos(az_rad);

   return QPointF(tgtX_nm, tgtY_nm);
}

osg::ref_ptr<osg::StateSet> HDD::PageTactical::GetStencilDrawWhereMatch()
{
   osg::ref_ptr<osg::StateSet> stencilStateSet(new osg::StateSet);
   osg::ref_ptr<osg::Stencil>  stencilStateAttribute(new osg::Stencil);
   ;

   stencilStateAttribute->setFunction(osg::Stencil::Function::EQUAL, 1, 1); // Always passes, 1 bit plane, 1 as mask
   stencilStateAttribute->setOperation(osg::Stencil::KEEP,
                                       osg::Stencil::KEEP,
                                       osg::Stencil::KEEP); // Set stencil to 1 where we draw

   stencilStateSet->setAttributeAndModes(stencilStateAttribute.get(), osg::StateAttribute::ON);
   osg::ref_ptr<osg::ColorMask> cMask(new osg::ColorMask);
   cMask->setMask(true, true, true, true);
   stencilStateSet->setAttributeAndModes(cMask.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
   return stencilStateSet;
}

void HDD::PageTactical::DrawStencilBufferRegion(osg::ref_ptr<osg::Group>           aVectorGroup,
                                                const osg::ref_ptr<osg::Vec2Array> aArray,
                                                int                                aStencilValue)
{
   osg::ref_ptr<osg::StateSet> stencilStateSet(new osg::StateSet);
   osg::ref_ptr<osg::Stencil>  stencilStateAttribute(new osg::Stencil);

   stencilStateAttribute->setFunction(osg::Stencil::Function::ALWAYS,
                                      aStencilValue,
                                      aStencilValue); // Always passes, 1 bit plane, 1 as mask
   stencilStateAttribute->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::REPLACE); // Set stencil to
                                                                                                       // 1 where we draw
   stencilStateSet->setAttributeAndModes(stencilStateAttribute.get(), osg::StateAttribute::ON);

   // Disable drawing into the color buffer
   osg::ref_ptr<osg::ColorMask> cMask(new osg::ColorMask);
   cMask->setMask(false, false, false, false); // Similar to glColorMask(0, 0, 0, 0);
   stencilStateSet->setAttributeAndModes(cMask.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

   // Disable the depth buffer
   osg::ref_ptr<osg::StateSet> ss = aVectorGroup->getOrCreateStateSet();
   ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

   // Draw region
   osg::ref_ptr<osg::Geode> stencilGeode = mOverlayPtr->CreateGeometry(aArray, GL_POLYGON, Color::White).get();
   stencilGeode->setStateSet(stencilStateSet.get());
   aVectorGroup->addChild(stencilGeode.get());
}

void HDD::PageTactical::SetTrackData(const SimData::TargetTrackItemList& aTrackData)
{
   mEntities.clear();
   // This bool tracks whether or not we found a matching track for the
   // current entity, if there is one.
   bool mCurrentEntityFound = false;
   for (auto&& it : aTrackData.mTracks)
   {
      Entity entity;
      entity.type  = TRACK;
      entity.track = it.second;

      // If we have the entity window up because of a click
      // we need to update the entity every update.
      if (mShowingEntityInfo && mEntityClicked && mCurrentEntity.type == TRACK)
      {
         // If this is the track matching the current entity, update it.
         if (entity.track.tgtPlatformIndex == mCurrentEntity.track.tgtPlatformIndex)
         {
            mCurrentEntityFound = true;
            SetCurrentEntity(entity);
         }
      }

      mEntities.push_back(entity);
   }

   // If we didn't find a matching entity, close the entity window and
   // reset vars
   if (mShowingEntityInfo && mEntityClicked && mCurrentEntity.type == TRACK && !mCurrentEntityFound)
   {
      mShowingEntityInfo = false;
      mEntityClicked     = false;
   }
}

void HDD::PageTactical::SetAssetsListData(const SimData::AssetsList& aData)
{
   mAssets.clear();
   // This bool tracks whether or not we found a matching track for the
   // current entity, if there is one.
   bool mCurrentEntityFound = false;
   for (auto&& it : aData.assetList)
   {
      Entity entity;
      entity.type          = ASSET;
      entity.perceivedData = it;

      // If we have the entity window up because of a click
      // we need to update the entity every update.
      if (mShowingEntityInfo && mEntityClicked && mCurrentEntity.type == ASSET)
      {
         // If this is the track matching the current entity, update it.
         if (entity.perceivedData.perceivedName.compare(mCurrentEntity.perceivedData.perceivedName) == 0)
         {
            mCurrentEntityFound = true;
            SetCurrentEntity(entity);
         }
      }

      mAssets.push_back(entity);
   }

   // If we didn't find a matching entity, close the entity window and
   // reset vars
   if (mShowingEntityInfo && mEntityClicked && mCurrentEntity.type == ASSET && !mCurrentEntityFound)
   {
      mShowingEntityInfo = false;
      mEntityClicked     = false;
   }
}

void HDD::PageTactical::SetNavData(const SimData::NavData& aNavData)
{
   mData.navData = aNavData;
}

void HDD::PageTactical::SetWeaponSelectedAndArmed(const std::string& aSelectedWeapon,
                                                  const int          aNumSelectedWeapon,
                                                  const bool         aMasterArmActive)
{
   mData.selectedWeapon    = aSelectedWeapon;
   mData.numSelectedWeapon = aNumSelectedWeapon;
   mData.masterArmActive   = aMasterArmActive;
}

void HDD::PageTactical::HandleEntityClickRegions(const int aMouseX, const int aMouseY)
{
   // Set the newest mouse position
   mMousePos.setX(aMouseX);
   mMousePos.setY(aMouseY);

   // Loop through each entity and check if we're mousing over it.  Multiple entities hover
   // regions may be hovered over at the same time, so check each entity and set the current
   // entity to the one closest to the mouse.
   mShowingEntityInfo = false; // Set this to true if any entity is being moused over.
   mEntityClicked     = false;

   std::vector<Entity> allEntities;
   for (auto& e : mEntities)
   {
      allEntities.push_back(e);
   }
   for (auto& e : mAssets)
   {
      allEntities.push_back(e);
   }
   for (auto& entity : allEntities)
   {
      // Don't hover over entities drawn behind the stencil buffer.
      double rng_nm;
      if (entity.type == TRACK)
      {
         rng_nm = entity.track.tgtRange_nm;
      }
      else /*if (entity.type == ASSET)*/
      {
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.lat_deg,
                                                         mData.lon_deg,
                                                         entity.perceivedData.lat_deg,
                                                         entity.perceivedData.lon_deg,
                                                         heading_deg,
                                                         distance_m);
         rng_nm = distance_m * UtMath::cNM_PER_M;
      }
      // If we are within hover range, evaluate the entity.
      if (rng_nm < mRange_nm)
      {
         // If our mouse is inside the entities hover region
         if (entity.hoverRegion.contains(mMousePos))
         {
            // We have clicked on an entity.
            mEntityClicked = true;

            // Compare this entity to the current one.  If the newer distance is less than
            // or equal to the current one update the current entity.
            if (mShowingEntityInfo)
            {
               double currentEntityDist =
                  std::abs(mCurrentEntity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
               double newEntityDist =
                  std::abs(entity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
               if (newEntityDist <= currentEntityDist)
               {
                  SetCurrentEntity(entity);
               }
            }
            // If this is the first entity contained, set it as we have nothing to compare to yet
            else
            {
               SetCurrentEntity(entity);
               mShowingEntityInfo = true;
            }
         }
      }
   }
}

void HDD::PageTactical::HandleHoverRegions(const int aMouseX, const int aMouseY)
{
   if (!mEntityClicked)
   {
      // Set the newest mouse position
      mMousePos.setX(aMouseX);
      mMousePos.setY(aMouseY);

      // Loop through each entity and check if we're mousing over it.  Multiple entities
      // hover regions may be hovered over at the same time, so check each entity and set
      // the current entity to the one closest to the mouse.
      mShowingEntityInfo = false; // Set this to true if any entity is being moused over.

      std::vector<Entity> allEntities;
      for (auto& e : mEntities)
      {
         allEntities.push_back(e);
      }
      for (auto& e : mAssets)
      {
         allEntities.push_back(e);
      }
      for (auto& entity : allEntities)
      {
         // Don't hover over entities drawn behind the stencil buffer.
         double rng_nm;
         if (entity.type == TRACK)
         {
            rng_nm = entity.track.tgtRange_nm;
         }
         else /*if (entity.type == ASSET)*/
         {
            double heading_deg = 0.0;
            double distance_m  = 0.0;
            UtSphericalEarth::GreatCircleHeadingAndDistance(mData.lat_deg,
                                                            mData.lon_deg,
                                                            entity.perceivedData.lat_deg,
                                                            entity.perceivedData.lon_deg,
                                                            heading_deg,
                                                            distance_m);
            rng_nm = distance_m * UtMath::cNM_PER_M;
         }

         // If we are within hover range, evaluate the entity.
         if (rng_nm < mRange_nm)
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               // Compare this entity to the current one.  If the newer distance is less
               // than or equal to the current one update the current entity.
               if (mShowingEntityInfo)
               {
                  double currentEntityDist =
                     std::abs(mCurrentEntity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  double newEntityDist =
                     std::abs(entity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  if (newEntityDist <= currentEntityDist)
                  {
                     SetCurrentEntity(entity);
                  }
               }
               // If this is the first entity contained, set it as we have nothing to compare to yet
               else
               {
                  SetCurrentEntity(entity);
                  mShowingEntityInfo = true;
               }
            }
         }
      }
   }
}
