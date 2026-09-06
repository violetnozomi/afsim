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

#include "sa_display/WkfSA_DisplayOverlay.hpp"

#include <iomanip>
#include <sstream>

#include <QTransform>
#include <osg/Depth>
#include <osg/Hint>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/Scissor>
#include <osg/StateSet>
#include <osg/Stencil>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtPath.hpp"
#include "UtSphericalEarth.hpp"
#include "Uto.hpp"
#include "VaUtils.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"


wkf::SA_Display::Overlay::Overlay(const char* aName, vespa::VaViewer* aViewerPtr)
   : vespa::VaOverlay(aName, vespa::VaOverlay::cOVERLAY_WINDOW)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   , mFontSizeMult(1.0f)
   , mCenterPoint(0, 0)
{
   mScreenWidth  = aViewerPtr->GetWidth();
   mScreenHeight = aViewerPtr->GetHeight();

   aViewerPtr->AddOverlay(this);

   SetPosition(0, 0);
   SetSize(aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
   SetVisible(true);
   SetDrawFunction(std::bind(&Overlay::Draw, this));
   SetButton1ClickAction(
      [this](int x, int y, int s) -> bool
      {
         // Only handle click events when visible
         if (IsVisible())
         {
            HandleClickRegions(x, y);
         }
         return false;
      });
   SetMouseMoveAction(
      [this](int x, int y) -> bool
      {
         // Only handle move events when visible
         if (IsVisible())
         {
            mMousePos.setX(x);
            mMousePos.setY(y);
            HandleHoverRegions(x, y);
         }
         return false;
      });

   SetType<wkf::SA_Display::Overlay>();
}

wkf::SA_Display::Overlay* wkf::SA_Display::Overlay::Clone() const
{
   return new wkf::SA_Display::Overlay(*this);
}

const double wkf::SA_Display::Overlay::pW() const
{
   if (mHorizontalPriority)
   {
      return mRect.width();
   }
   else
   {
      return mRect.width() * mRatio;
   }
}

const double wkf::SA_Display::Overlay::pH() const
{
   if (mHorizontalPriority)
   {
      return mRect.height() * mRatio;
   }
   else
   {
      return mRect.height();
   }
}

bool wkf::SA_Display::Overlay::Initialize()
{
   bool initialized = false;

   UtoRawShape shapePtr;
   mRawShapePtr = dynamic_cast<UtoRawShape*>(BindShape("sa_display_gl", shapePtr));
   if (mRawShapePtr == nullptr)
   {
      auto out = ut::log::warning() << "Couldn't bind shape: sa_display_gl";
   }
   else
   {
      SetVisible(true);
      initialized = true;

      if (!mOverlayGeode)
      {
         mOverlayGeode = new osg::Geode;
      }

      if (!mOverlayState)
      {
         mOverlayState = new osg::StateSet;
         // Disable depth testing so geometry is draw regardless of depth values
         // of geometry already draw.
         mOverlayState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
         mOverlayState->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

         // Need to make sure this geometry is draw last. RenderBins are handled
         // in numerical order so set bin number to 11
         mOverlayState->setRenderBinDetails(11, "RenderBin");

         mOverlayGeode->setStateSet(mOverlayState);
      }

      mOverlayModelViewMatrix = new osg::MatrixTransform; // screen location
      mOverlayModelViewMatrix->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      mOverlayModelViewMatrix->getOrCreateStateSet()->setMode(GL_BLEND,
                                                              osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
      mOverlayModelViewMatrix->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH,
                                                              osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

      mRawShapePtr->GetRoot()->setCullingActive(false);
      mRawShapePtr->GetRoot()->addChild(mOverlayModelViewMatrix.get());

      mOverlayModelViewMatrix->addChild(mOverlayGeode.get());
   }

   return initialized;
}

void wkf::SA_Display::Overlay::Update()
{
   // Clear out the geodes drawables
   mOverlayGeode->removeDrawables(0, mOverlayGeode->getNumDrawables());

   if (mDrawFunction)
   {
      mDrawFunction();
   }
}

void wkf::SA_Display::Overlay::DrawOwnshipFOV(osg::ref_ptr<osg::Group> aVectorGroup,
                                              const double             aX,
                                              const double             aY,
                                              const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // If we are drawing the FOV lines
   if (mDrawFOV)
   {
      // FOV Lines
      double                       halfFov = mFOV_deg / 2.0;
      double                       ang_rad = halfFov * UtMath::cRAD_PER_DEG;
      double                       x       = 10.0 * sin(ang_rad);
      double                       y       = 10.0 * cos(ang_rad);
      osg::ref_ptr<osg::Vec2Array> fovLinesArray(new osg::Vec2Array);
      fovLinesArray->push_back(osg::Vec2d(0.0, 0.0));
      fovLinesArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      fovLinesArray->push_back(osg::Vec2d(0.0, 0.0));
      fovLinesArray->push_back(osg::Vec2d(-x * pW(), y * pH()));
      vectorXform->addChild(CreateGeometry(fovLinesArray, GL_LINES, Color::SlateBlue).get());
   }

   // Draw the aircraft in the center
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d(0.0, 0.0125 * 0.5 * pH()));
   linesArray->push_back(osg::Vec2d(0.0, -0.024 * 0.5 * pH()));
   linesArray->push_back(osg::Vec2d(-0.017 * 0.5 * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(0.017 * 0.5 * pW(), 0.0));
   linesArray->push_back(osg::Vec2d(-0.009 * 0.5 * pW(), -0.018 * 0.5 * pH()));
   linesArray->push_back(osg::Vec2d(0.009 * 0.5 * pW(), -0.018 * 0.5 * pH()));
   vectorXform->addChild(CreateGeometry(linesArray, GL_LINES, Color::White).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::SA_Display::Overlay::DrawOwnshipRings(osg::ref_ptr<osg::Group> aVectorGroup,
                                                const double             aX,
                                                const double             aY,
                                                const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   size_t rings            = 2;
   double distBetweenRings = aR / rings;
   // Draw our rings, and the range labels on them.
   for (size_t r = 1; r <= rings + 3; r++)
   {
      // Skip the second ring if the ownship is centered to avoid artifacting.
      if (r == 2 && (mCenterPoint.x() == 0.0 && mCenterPoint.y() == 0.0))
      {
         continue;
      }

      osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
         double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);
         ringArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
      vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::CompassLines).get());

      // Draw the ranges for each ring, where the final ring is mRange_nm.
      float             fontSize     = 0.015f * pW();
      double            ring_ang_rad = 60.0 * UtMath::cRAD_PER_DEG;
      double            x            = (0.0 + (distBetweenRings * r)) * cos(ring_ang_rad);
      double            y            = (0.0 + (distBetweenRings * r)) * sin(ring_ang_rad);
      std::stringstream nm;
      nm << std::fixed << std::setw(2) << std::setprecision(0) << ((mRange_nm / rings) * r);
      // We need a small circle so the range is easy to see
      osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double cX      = 0.0175 * cos(ang_rad);
         double cY      = 0.0175 * sin(ang_rad);
         circleArray->push_back(osg::Vec2d((x + cX) * pW(), (y + cY) * pH()));
      }
      // Draw a circle with the range inside of it
      auto circle = CreateGeometry(circleArray, GL_POLYGON, Color::Background);
      circle->addChild(
         AddTextItem(nm.str(), x * pW(), y * pH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
      vectorXform->addChild(circle.get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

bool wkf::SA_Display::Overlay::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER && mBtn1Function)
   {
      return mBtn1Function(aMouseX, aMouseY, aState);
   }
   else
   {
      return false;
   }
}

bool wkf::SA_Display::Overlay::MouseMove(int aMouseX, int aMouseY)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER && mMouseMoveFunction)
   {
      return mMouseMoveFunction(aMouseX, aMouseY);
   }
   else
   {
      return false;
   }
}

bool wkf::SA_Display::Overlay::SelectableAt(int aMouseX, int aMouseY) const
{
   if (IsVisible() && (aMouseX > mRect.x()) && (aMouseX < mRect.x() + mRect.width()) && (aMouseY > mRect.y()) &&
       (aMouseY < mRect.y() + mRect.height()))
   {
      return true;
   }
   return false;
}

void wkf::SA_Display::Overlay::DrawRoute(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   if (mDrawRoute)
   {
      float fontSize = 0.025f * pW();

      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

      QPointF lastPos;
      for (size_t i = 0; i < mData.navData.waypointData.waypoints.size(); i++)
      {
         // Get the heading and distance to the waypoint.
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                         mData.platformData.lon_deg,
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
         vectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, Color::Gray));
         vectorXform->addChild(AddTextItem(std::to_string(i),
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
            vectorXform->addChild(CreateGeometry(lineArray, GL_LINES, Color::Gray));
         }

         // Set the last position so we can draw a line to the next waypoint.
         lastPos = pos;
      }

      vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
      aVectorGroup->addChild(vectorXform);
   }
}

void wkf::SA_Display::Overlay::DrawHeading(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   float fontSize = 0.012 * 2.0 * pW();

   // Heading
   double heading_deg = mData.platformData.heading_deg;

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
   aVectorGroup->addChild(
      AddTextItem(heading.str(), aX * pW(), aY * pH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
}

void wkf::SA_Display::Overlay::DrawLockOnTarget(osg::ref_ptr<osg::Group> aVectorGroup,
                                                Entity&                  aEntity,
                                                const double             aX,
                                                const double             aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double heading_deg = 0.0;
   double distance_m  = 0.0;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                   mData.platformData.lon_deg,
                                                   mCurrentEntity.truth.lat_deg,
                                                   mCurrentEntity.truth.lon_deg,
                                                   heading_deg,
                                                   distance_m);

   const QPointF pos =
      CalculatePositionFromBearingAndRange(aEntity.perception.bearing_deg * UtMath::cRAD_PER_DEG, distance_m);

   double lineLength = mCompassRadius / 40; // Set it to take 40 dash increments to hit the edge of the compass
   int    ticks      = (distance_m * UtMath::cNM_PER_M * UnitsPerNm()) / lineLength; // dashes/ticks required to target
   osg::ref_ptr<osg::Vec2Array> lineArray(new osg::Vec2Array);
   for (int i = 0; i < ticks; i += 2)
   {
      lineArray->push_back(osg::Vec2d((0.0 + ((pos.x() / ticks) * i)) * UnitsPerNm() * pW(),
                                      (0.0 + ((pos.y() / ticks) * i)) * UnitsPerNm() * pH()));
      lineArray->push_back(osg::Vec2d((0.0 + ((pos.x() / ticks) * (i + 1))) * UnitsPerNm() * pW(),
                                      (0.0 + ((pos.y() / ticks) * (i + 1))) * UnitsPerNm() * pH()));
   }
   vectorXform->addChild(CreateGeometry(lineArray, GL_LINES, Color::SlateBlue));

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::SA_Display::Overlay::DrawLockedEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                Entity&                  aEntity,
                                                const double             aX,
                                                const double             aY,
                                                const double             aW,
                                                const double             aH)
{
   // Symbol size
   double sW = (aW * 0.25);
   double sH = (aW * 0.25);
   double r  = sW;

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

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
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   osg::Vec4 entityColor = GetEntityColor(aEntity.perception.identification);

   // Since our target is locked, instead of getting the top and bottom half
   // and using GL_LINES, draw a GL_POLYGON
   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   entityArray->push_back(osg::Vec2d(-sW * pW(), 0.0));
   entityArray->push_back(osg::Vec2d(0.0, sH * pH()));
   entityArray->push_back(osg::Vec2d(sW * pW(), 0.0));
   entityArray->push_back(osg::Vec2d(0.0, -sH * pH()));
   vectorXform->addChild(CreateGeometry(entityArray, GL_POLYGON, entityColor).get());

   // Heading line
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> headingXform(new osg::PositionAttitudeTransform());
   headingXform->setAttitude(
      osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                osg::Vec3(0.0f, 0.0f, -1.0f)));
   osg::ref_ptr<osg::Vec2Array> headingLineArray(new osg::Vec2Array);
   double                       startY = sqrt(2 * r * r);
   headingLineArray->push_back(osg::Vec2d(0.0, startY * pH()));
   headingLineArray->push_back(osg::Vec2d(0.0, (aH * 0.6) * pH()));
   headingXform->addChild(CreateGeometry(headingLineArray, GL_LINES, entityColor).get());
   vectorXform->addChild(headingXform.get());

   // Draw altitude if visible
   if (mDrawAltitude)
   {
      // Altitude (in thousands of ft)
      float             fontSize = 0.008 * 2.0 * pW();
      std::stringstream alt;
      alt << static_cast<int>(aEntity.perception.altitude_ft) / 1000;
      vectorXform->addChild(
         AddTextItem(alt.str(), (aW * 0.4) * pW(), 0.0, fontSize, entityColor, osgText::Text::LEFT_CENTER).get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

osg::ref_ptr<osg::StateSet> wkf::SA_Display::Overlay::GetStencilDrawWhereMatch()
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

void wkf::SA_Display::Overlay::DrawStencilBufferRegion(osg::ref_ptr<osg::Group>           aVectorGroup,
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
   osg::ref_ptr<osg::Geode> stencilGeode = CreateGeometry(aArray, GL_POLYGON, Color::White).get();
   stencilGeode->setStateSet(stencilStateSet.get());
   aVectorGroup->addChild(stencilGeode.get());
}

const QPointF wkf::SA_Display::Overlay::CalculateEntityPosition(const Entity& aEntity)
{
   double heading_deg = 0.0;
   double distance_m  = 0.0;
   // If the entity is a group, use the centroid lat lon.
   if (aEntity.isGroup)
   {
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      aEntity.groupPerception.centroidLat_deg,
                                                      aEntity.groupPerception.centroidLon_deg,
                                                      heading_deg,
                                                      distance_m);
   }
   // If the entity is truth, use the truth lat lon.
   else if (aEntity.isTruth)
   {
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      aEntity.truth.lat_deg,
                                                      aEntity.truth.lon_deg,
                                                      heading_deg,
                                                      distance_m);
   }
   // Otherwise, use the perception lat lon.
   else
   {
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      aEntity.perception.lat_deg,
                                                      aEntity.perception.lon_deg,
                                                      heading_deg,
                                                      distance_m);
   }
   return CalculatePositionFromBearingAndRange(heading_deg * UtMath::cRAD_PER_DEG, distance_m * UtMath::cNM_PER_M);
}

const QPointF wkf::SA_Display::Overlay::CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm)
{
   // Calculate the orientation of the entity
   double az_rad = aBearingRad - (mData.platformData.heading_deg * UtMath::cRAD_PER_DEG);
   if (az_rad < -UtMath::cPI)
   {
      az_rad += UtMath::cTWO_PI;
   }
   if (az_rad > UtMath::cPI)
   {
      az_rad -= UtMath::cTWO_PI;
   }

   // Calculate the position of the entity
   double tgtX_nm = aRangeNm * sin(az_rad);
   double tgtY_nm = aRangeNm * cos(az_rad);

   return QPointF(tgtX_nm, tgtY_nm);
}

const QRectF wkf::SA_Display::Overlay::CalculateEntityHoverRegion(Entity&      aEntity,
                                                                  const double aX,
                                                                  const double aY,
                                                                  double       aW,
                                                                  const double aH)
{
   // Create a polygon with the basic width and height of the entity.
   QPolygonF poly;
   poly.append(QPointF(-(aW * 0.5), (aH * 0.5)));
   poly.append(QPointF((aW * 0.5), (aH * 0.5)));
   poly.append(QPointF((aW * 0.5), -(aH * 0.5)));
   poly.append(QPointF(-(aW * 0.5), -(aH * 0.5)));

   // Create a transform that will translate and then rotate the polygon
   QTransform t =
      QTransform().translate(aX * UnitsPerNm(), aY * UnitsPerNm()); // .rotateRadians(-(aEntity.trackData.tgtHdg_rad
                                                                    // - mData.heading));
   // Translate and rotate the polygon to align with the entity
   QPolygonF nPoly = t.map(poly);
   // Create a bounding rect surrounding the new polygon
   QRectF bR = nPoly.boundingRect();
   // Set the hover region rect for our entity here, converting to pixel coordinates
   // to match mouse move events.
   aEntity.hoverRegion.setRect(((bR.x() + mCompassX - (mCenterPoint.x() * UnitsPerNm())) * pW()) + mRect.center().x(),
                               ((bR.y() + mCompassY + mYOffset - (mCenterPoint.y() * UnitsPerNm())) * pH()) +
                                  mRect.center().y(),
                               bR.width() * pW(),
                               bR.height() * pH());

   // Return the bounding rect in case it will be drawn (debugging).
   return bR;
}

const QPointF wkf::SA_Display::Overlay::CalculateMousePosInUnits(const double aMouseX, const double aMouseY)
{
   double x = (((aMouseX - mRect.center().x()) / pW()));
   double y = (((aMouseY - mRect.center().y()) / pH()));

   return QPointF(x, y);
}

const QPointF wkf::SA_Display::Overlay::CalculateMousePosInNm(const double aMouseX, const double aMouseY)
{
   double x = (((aMouseX - mRect.center().x()) / pW())) / UnitsPerNm();
   double y = (((aMouseY - mRect.center().y()) / pH()) - mYOffset) / UnitsPerNm();

   return QPointF(x, y);
}

const osg::Vec4 wkf::SA_Display::Overlay::GetEntityColor(const Identification aIdentification) const
{
   switch (aIdentification)
   {
   case ID_BANDIT:
      return Color::Red;
   case ID_FRIENDLY:
      return Color::Green;
   case ID_NEUTRAL:
      return Color::White;
   case ID_BOGIE:
   case ID_UNKNOWN:
   default:
      return Color::Yellow;
   }
}

void wkf::SA_Display::Overlay::SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction)
{
   mHandleEventMode = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mBtn1Function    = aFunction;
}

void wkf::SA_Display::Overlay::SetMouseMoveAction(const std::function<bool(int, int)>& aFunction)
{
   mHandleEventMode   = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mMouseMoveFunction = aFunction;
}

osg::ref_ptr<osg::Geode> wkf::SA_Display::Overlay::CreateGeometry(osg::ref_ptr<osg::Vec2Array> aArray,
                                                                  int                          aDrawMode,
                                                                  const osg::Vec4&             aColor)
{
   osg::ref_ptr<osg::Geometry> geom(new osg::Geometry);
   osg::ref_ptr<osg::Geode>    retGeode(new osg::Geode);

   osg::ref_ptr<osg::DrawArrays> geomArray(new osg::DrawArrays(aDrawMode, 0, aArray->size()));
   geom->setVertexArray(aArray.get());
   geom->addPrimitiveSet(geomArray.get());

   osg::ref_ptr<osg::Vec4Array> colorArray(new osg::Vec4Array);
   colorArray->push_back(aColor);

   geom->setColorArray(colorArray);
   geom->setColorBinding(osg::Geometry::BIND_OVERALL);

   osg::ref_ptr<osg::StateSet>  lineState(new osg::StateSet);
   osg::ref_ptr<osg::LineWidth> lineWidthAttr(new osg::LineWidth);

   lineWidthAttr->setWidth(1.0f * mLineWidthMult);

   lineState->setAttribute(lineWidthAttr.get());
   lineState->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   lineState->setMode(GL_BLEND, osg::StateAttribute::ON);
   retGeode->setStateSet(lineState.get());

   retGeode->addDrawable(geom.get());
   return retGeode;
}

osg::ref_ptr<osg::Geode> wkf::SA_Display::Overlay::AddTextItem(const std::string&                 aStr,
                                                               const double                       aXPos,
                                                               const double                       aYPos,
                                                               const float                        aFontSize,
                                                               const osg::Vec4&                   aColor,
                                                               const osgText::Text::AlignmentType aAlignment)
{
   osg::ref_ptr<osgText::Text> textItem(new osgText::Text());
   textItem->setCharacterSize(aFontSize);

   // Set alignment
   textItem->setAlignment(aAlignment);

   if (mFont == nullptr)
   {
      std::string resourceDir = "";
      vespa::VaUtils::GetPathPrioritizedFilename("fonts/DejaVuSansMono.ttf", resourceDir);
      UtPath path(resourceDir);
      path.Up();

      // Find the font in the available resource directories
      // If an empty string is returned, OSG will find some font to use

      UtPath wd = UtPath::WorkingDirectory();
      path.SetWorkingDirectory();
      mFont = osgText::readFontFile("DejaVuSansMono.ttf");
      wd.SetWorkingDirectory();
   }

   textItem->setFont(mFont);
   textItem->setText(aStr);
   textItem->setAxisAlignment(osgText::Text::SCREEN);
   textItem->setPosition(osg::Vec3d(aXPos, aYPos, 0));
   textItem->setColor(aColor);

   osg::ref_ptr<osg::Geode> geoNode(new osg::Geode);
   geoNode->addDrawable(textItem);
   return geoNode;
}

void wkf::SA_Display::Overlay::DrawBackground(const osg::Vec4& aColor)
{
   // Background rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.5, mRect.height() * 0.5));   // Top right
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.5, -mRect.height() * 0.5));  // Bottom right
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.5, -mRect.height() * 0.5)); // Bottom left
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.5, mRect.height() * 0.5));  // Top left
   mOverlayGeode->addChild(CreateGeometry(boxArray, GL_POLYGON, aColor).get());
}

void wkf::SA_Display::Overlay::DrawOutline(const osg::Vec4& aColor)
{
   // Box
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.4995, mRect.height() * 0.4995));   // Top right
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.4995, -mRect.height() * 0.4995));  // Bottom right
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.4995, -mRect.height() * 0.4995)); // Bottom left
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.4995, mRect.height() * 0.4995));  // Top left

   mLineWidthMult = 2.0f;
   mOverlayGeode->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, aColor).get());
   mLineWidthMult = 1.0f;
}

void wkf::SA_Display::Overlay::DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
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
   vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, osg::Vec4(0.0, 0.1, 0.0, 1.0)).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::SA_Display::Overlay::DrawLabel(const double                 aXPos,
                                         const double                 aYPos,
                                         const std::string&           aText,
                                         const float                  aFontSize,
                                         const osg::Vec4&             aColor,
                                         osgText::Text::AlignmentType aAlignment)
{
   // Positioning transform
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aXPos, aYPos, 0.0));

   // Text label
   vectorXform->addChild(AddTextItem(aText, 0.0, 0.0, aFontSize, aColor, aAlignment).get());

   // Add the label to the geode
   mOverlayGeode->addChild(vectorXform.get());
}

void wkf::SA_Display::Overlay::SetPosition(float aX, float aY)
{
   mRect.setX(aX);
   mRect.setY(aY);
   mOverlayModelViewMatrix->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
}

void wkf::SA_Display::Overlay::SetSize(float aW, float aH)
{
   mRect.setWidth(aW);
   mRect.setHeight(aH);

   UpdatePriority();

   mOverlayModelViewMatrix->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
   osg::StateSet* ss = mRawShapePtr->GetRoot()->getOrCreateStateSet();
   ss->setAttributeAndModes(new osg::Scissor(mRect.x(), mRect.y(), mRect.width(), mRect.height()));
   Update();
}

void wkf::SA_Display::Overlay::SetSA_Data(const SA_Data& aData)
{
   // Set our data
   mData = aData;

   // Clear the entities and populate the new list from our new data.
   mEntities.clear();
   for (auto& entityPerception : mData.assetsList)
   {
      Entity e;
      e.perception = entityPerception;
      mEntities.push_back(e);
   }
   for (auto& entityPerception : mData.bogiesList)
   {
      Entity e;
      e.perception = entityPerception;
      mEntities.push_back(e);
   }
   for (auto& entityPerception : mData.banditsList)
   {
      Entity e;
      e.perception = entityPerception;
      mEntities.push_back(e);
   }
   for (auto& groupPerception : mData.groupsList)
   {
      Entity e;
      e.isGroup         = true;
      e.groupPerception = groupPerception;
      mEntities.push_back(e);
   }
}

void wkf::SA_Display::Overlay::SetSA_TruthPlatforms(const std::vector<SA_Display::EntityTruth>& aData)
{
   // Clear our truth entities and repopulate.
   mTruthEntities.clear();
   for (auto& entityTruth : aData)
   {
      Entity e;
      e.truth   = entityTruth;
      e.isTruth = true;
      mTruthEntities.push_back(e);
   }
}

void wkf::SA_Display::Overlay::SetTruthWidgetData()
{
   // Name
   mTruthDataWidget->child(0)->setText(1, QString::fromStdString(mCurrentEntity.truth.name));

   // Type
   mTruthDataWidget->child(1)->setText(1, QString::fromStdString(mCurrentEntity.truth.type));

   double heading_deg = 0.0;
   double distance_m  = 0.0;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                   mData.platformData.lon_deg,
                                                   mCurrentEntity.truth.lat_deg,
                                                   mCurrentEntity.truth.lon_deg,
                                                   heading_deg,
                                                   distance_m);

   // Range
   mTruthDataWidget->child(2)->setText(1, QString::number(distance_m * UtMath::cNM_PER_M));
}

void wkf::SA_Display::Overlay::SetPerceivedWidgetData()
{
   if (mCurrentEntity.isGroup)
   {
      // Id
      mPerceivedDataWidget->child(0)->setText(1, QString::fromStdString(mCurrentEntity.groupPerception.groupName));

      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      mCurrentEntity.groupPerception.centroidLat_deg,
                                                      mCurrentEntity.groupPerception.centroidLon_deg,
                                                      heading_deg,
                                                      distance_m);

      double rng_nm = distance_m * UtMath::cNM_PER_M;

      // Range
      mPerceivedDataWidget->child(1)->setText(1, QString::number(rng_nm));
   }
   else
   {
      // Id
      mPerceivedDataWidget->child(0)->setText(1, QString::fromStdString(mCurrentEntity.perception.idString));

      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      mCurrentEntity.perception.lat_deg,
                                                      mCurrentEntity.perception.lon_deg,
                                                      heading_deg,
                                                      distance_m);

      double rng_nm = distance_m * UtMath::cNM_PER_M;

      // Range
      mPerceivedDataWidget->child(1)->setText(1, QString::number(rng_nm));
   }
}

void wkf::SA_Display::Overlay::EmptyTruthWidgetData()
{
   // Name
   mTruthDataWidget->child(0)->setText(1, "");

   // Type
   mTruthDataWidget->child(1)->setText(1, "");

   // Range
   mTruthDataWidget->child(2)->setText(1, "");
}

void wkf::SA_Display::Overlay::EmptyPerceivedWidgetData()
{
   // Id
   mPerceivedDataWidget->child(0)->setText(1, "");

   // Range
   mPerceivedDataWidget->child(1)->setText(1, "");
}

void wkf::SA_Display::Overlay::ResetCenterPoint()
{
   mCenterPoint = QPointF(0, 0);
}

void wkf::SA_Display::Overlay::UpdatePriority()
{
   mHorizontalPriority = mRect.width() > mRect.height() ? false : true;
   if (mOffsetBottom)
   {
      mHorizontalPriority = false;
   }

   if (mHorizontalPriority)
   {
      mRatio = mRect.width() / mRect.height();
   }
   else
   {
      mRatio = mRect.height() / mRect.width();
   }
}

void wkf::SA_Display::Overlay::SetCenterPosition()
{
   mYOffset      = 0.0;
   mOffsetMiddle = false;
   mOffsetBottom = false;
   UpdatePriority();
   Update();
}

void wkf::SA_Display::Overlay::SetOffsetPosition()
{
   mYOffset      = -0.24;
   mOffsetMiddle = true;
   mOffsetBottom = false;
   ResetCenterPoint();
   UpdatePriority();
   Update();
}

void wkf::SA_Display::Overlay::SetBottomPosition()
{
   mYOffset      = -0.42;
   mOffsetMiddle = false;
   mOffsetBottom = true;
   ResetCenterPoint();
   UpdatePriority();
   Update();
}

void wkf::SA_Display::Overlay::SetCenterPoint()
{
   mCenterPoint += CalculateMousePosInNm(mMousePos.x(), mMousePos.y());
   mCenterPositionPtr->toggle();
}

void wkf::SA_Display::Overlay::Draw()
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Default radius an position
   mCompassRadius = 0.425;
   mCompassX      = 0.0;
   mCompassY      = 0.0;

   mLineWidthMult = 1.5f; // Use a modified line width

   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);
   osg::ref_ptr<osg::Group> compassGroup(new osg::Group);

   DrawBackground(Color::Background);

   // The compass is 0.375 by default, split into two rings.
   // For each offset (middle, bottom) we'll want to change
   // how big the compass is, and how many rings it has in DrawCompass
   if (mOffsetMiddle)
   {
      mCompassRadius = 0.375;
      mCompassRadius += (mCompassRadius / 2.0);
   }
   else if (mOffsetBottom)
   {
      mCompassRadius += mCompassRadius;
   }

   // Compass stencil buffer region array.
   osg::ref_ptr<osg::Vec2Array> compassStencilArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = mCompassRadius * cos(ang_rad);
      double y       = mCompassRadius * sin(ang_rad);
      compassStencilArray->push_back(osg::Vec2d((x + mCompassX) * pW(), (y + mCompassY + mYOffset) * pH()));
   }

   DrawStencilBufferRegion(compassGroup, compassStencilArray, 1); // Enable Stencil Buffer
   if (!mOffsetMiddle && !mOffsetBottom)
   {
      DrawOwnshipRings(compassGroup,
                       mCompassX - (mCenterPoint.x() * UnitsPerNm()),
                       mCompassY + mYOffset - (mCenterPoint.y() * UnitsPerNm()),
                       mCompassRadius);
   }
   // DrawBox(compassGroup, 0, 0, 1.0, 1.0, 1); // Debugging for Stencil Buffer
   DrawEntities(compassGroup,
                mCompassX - (mCenterPoint.x() * UnitsPerNm()),
                mCompassY + mYOffset - (mCenterPoint.y() * UnitsPerNm()));
   DrawRoute(compassGroup,
             mCompassX - (mCenterPoint.x() * UnitsPerNm()),
             mCompassY + mYOffset - (mCenterPoint.y() * UnitsPerNm()));
   DrawOwnshipFOV(compassGroup,
                  mCompassX - (mCenterPoint.x() * UnitsPerNm()),
                  mCompassY + mYOffset - (mCenterPoint.y() * UnitsPerNm()),
                  mCompassRadius);
   DrawStencilBufferRegion(compassGroup, compassStencilArray, 0); // Release Stencil Buffer

   // Draw the compass
   DrawCompass(vectorGroup, mCompassX, mCompassY + mYOffset, mCompassRadius, 1);

   // Draw the heading
   DrawHeading(vectorGroup, 0.0, mCompassY + mYOffset + mCompassRadius + 0.05);

   // Add the finished drawing
   mOverlayGeode->addChild(vectorGroup.get());
   mOverlayGeode->addChild(compassGroup.get());

   // If we are showing entity info set it, otherwise clear it.
   if (mShowingEntityInfo)
   {
      if (mCurrentEntity.isTruth)
      {
         SetTruthWidgetData();
         EmptyPerceivedWidgetData();
      }
      else
      {
         SetPerceivedWidgetData();
         EmptyTruthWidgetData();
      }
   }
   else
   {
      EmptyTruthWidgetData();
      EmptyPerceivedWidgetData();
   }

   if (mConnectionDestroyed)
   {
      DrawConnectionDestroyed();
   }

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup,
                                           const double             aX,
                                           const double             aY,
                                           const double             aR,
                                           const double             aScale)
{
   if (mDrawBullseye)
   {
      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

      if (mOffsetMiddle || mOffsetBottom)
      {
         // Start with three rings on our compass.
         double rings = 2;
         // For the middle offset, add one ring.
         if (mOffsetMiddle)
         {
            rings = 3;
         }
         // For the bottom offset, add three rings.
         else if (mOffsetBottom)
         {
            rings = 4;
         }
         double distBetweenRings = aR / rings;
         // Draw our rings, and the range labels on them.
         for (int r = 1; r <= rings; r++)
         {
            osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
            for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
            {
               double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
               double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
               double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);
               ringArray->push_back(osg::Vec2d(x * pW(), y * pH()));
            }
            // Draw the final ring as gray rather than blue
            if (r == rings)
            {
               vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::Gray).get());
            }
            else
            {
               vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::CompassLines).get());
            }

            // Draw the ranges for each ring, where the final ring is mRange_nm.
            float             fontSize     = 0.015f * pW() * aScale;
            double            ring_ang_rad = 60.0 * UtMath::cRAD_PER_DEG;
            double            x            = (0.0 + (distBetweenRings * r)) * cos(ring_ang_rad);
            double            y            = (0.0 + (distBetweenRings * r)) * sin(ring_ang_rad);
            std::stringstream nm;
            nm << std::fixed << std::setw(2) << std::setprecision(0) << ((mRange_nm / rings) * r);
            // We need a small circle so the range is easy to see
            osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
            for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
            {
               double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
               double cX      = 0.0175 * cos(ang_rad);
               double cY      = 0.0175 * sin(ang_rad);
               circleArray->push_back(osg::Vec2d((x + cX) * pW(), (y + cY) * pH()));
            }
            // Draw a circle with the range inside of it
            auto circle = CreateGeometry(circleArray, GL_POLYGON, Color::Background);
            circle->addChild(
               AddTextItem(nm.str(), x * pW(), y * pH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
            vectorXform->addChild(circle.get());
         }
      }
      else
      {
         // Compass stencil buffer region array.
         osg::ref_ptr<osg::Vec2Array> compassArray(new osg::Vec2Array);
         for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = mCompassRadius * cos(ang_rad);
            double y       = mCompassRadius * sin(ang_rad);
            compassArray->push_back(osg::Vec2d((x + mCompassX) * pW(), (y + mCompassY + mYOffset) * pH()));
         }
         vectorXform->addChild(CreateGeometry(compassArray, GL_LINE_LOOP, Color::Gray).get());

         // Draw the ranges for each ring, where the final ring is mRange_nm.
         float             fontSize     = 0.015f * pW() * aScale;
         double            ring_ang_rad = 60.0 * UtMath::cRAD_PER_DEG;
         double            x            = mCompassRadius * cos(ring_ang_rad);
         double            y            = mCompassRadius * sin(ring_ang_rad);
         std::stringstream nm;
         nm << std::fixed << std::setw(2) << std::setprecision(0) << mRange_nm;
         // We need a small circle so the range is easy to see
         osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
         for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double cX      = 0.0175 * cos(ang_rad);
            double cY      = 0.0175 * sin(ang_rad);
            circleArray->push_back(osg::Vec2d((x + cX) * pW(), (y + cY) * pH()));
         }
         // Draw a circle with the range inside of it
         auto circle = CreateGeometry(circleArray, GL_POLYGON, Color::Background);
         circle->addChild(
            AddTextItem(nm.str(), x * pW(), y * pH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
         vectorXform->addChild(circle.get());
      }
      aVectorGroup->addChild(vectorXform.get());
   }
}

void wkf::SA_Display::Overlay::DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   // Transformation - Shift the region to match the compass
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Create temp asset lists so we can draw them last (on top)
   std::vector<Entity*> mAssetList;
   std::vector<Entity*> mFlightList;

   // Draw each entity
   for (auto& entity : mEntities)
   {
      const QPointF pos = CalculateEntityPosition(entity);

      // If our entity is a group and we are drawing groups
      if (entity.isGroup)
      {
         if (mDrawGroups)
         {
            DrawGroupEntity(vectorXform, entity, pos.x(), pos.y());
         }
      }
      // If we are a normal perception, and we're drawing perceptions
      else
      {
         if (mDrawPerception)
         {
            switch (entity.perception.identification)
            {
            // Friendlies get drawn after everything else to make them easier to see in clutter
            case ID_FRIENDLY:
            {
               if (entity.perception.flightId == mData.platformData.flightId)
               {
                  if (mDrawFlight)
                  {
                     mFlightList.push_back(&entity);
                  }
                  break;
               }

               if (mDrawFriends)
               {
                  mAssetList.push_back(&entity);
               }
               break;
            }
            default: // ID_BANDIT, ID_BOGIE, ID_NEUTRAL, ID_UNKNOWN
            {
               if (entity.perception.angleOnly)
               {
                  if (mDrawAngleOnly)
                  {
                     DrawAngleOnlyEntity(aVectorGroup, entity, 0.02, 0.02);
                  }
               }
               else
               {
                  DrawEntity(vectorXform, entity, pos.x(), pos.y(), 0.015, 0.03);
               }
               break;
            }
            }
         }
      }
   }

   // Draw the assets after bogies/bandits/unknown/neutral so we can see them in clutter easier
   for (auto& entity : mAssetList)
   {
      const QPointF pos = CalculateEntityPosition(*entity);
      if (!entity->perception.idFlag.empty())
      {
         DrawFlagEntity(vectorXform, *entity, pos.x(), pos.y(), 0.01875, 0.0375);
      }
      else if (entity->perception.angleOnly)
      {
         if (mDrawAngleOnly)
         {
            DrawAngleOnlyEntity(aVectorGroup, *entity, 0.02, 0.02);
         }
      }
      else
      {
         DrawEntity(vectorXform, *entity, pos.x(), pos.y(), 0.015, 0.03);
      }
   }

   // Draw flights after assets
   for (auto& entity : mFlightList)
   {
      const QPointF pos = CalculateEntityPosition(*entity);
      DrawFlightEntity(vectorXform, *entity, pos.x(), pos.y(), 0.01875, 0.0375);
   }

   // Draw priority threat and target numbers
   if (mDrawPerception)
   {
      if (mDrawPriorityTargets)
      {
         for (size_t i = 0; i < mData.prioritizedTargetsList.size(); i++)
         {
            Entity entity;
            entity.perception = mData.prioritizedTargetsList[i];
            const QPointF pos = CalculateEntityPosition(entity);
            if (entity.perception.angleOnly)
            {
               if (mDrawAngleOnly)
               {
                  DrawAngleOnlyPriorityTargetThreatNumber(aVectorGroup, entity, i + 1, 0.02, 0.02, false);
               }
            }
            else
            {
               DrawEntityPriorityTargetThreatNumber(vectorXform, entity, i + 1, pos.x(), pos.y(), 0.04, 0.04, false);
            }
         }
      }

      if (mDrawPriorityThreats)
      {
         for (size_t i = 0; i < mData.prioritizedThreatsList.size(); i++)
         {
            Entity entity;
            entity.perception = mData.prioritizedThreatsList[i];
            const QPointF pos = CalculateEntityPosition(entity);
            if (entity.perception.angleOnly)
            {
               if (mDrawAngleOnly)
               {
                  DrawAngleOnlyPriorityTargetThreatNumber(aVectorGroup, entity, i + 1, 0.02, 0.02, true);
               }
            }
            else
            {
               DrawEntityPriorityTargetThreatNumber(vectorXform, entity, i + 1, pos.x(), pos.y(), 0.04, 0.04, true);
            }
         }
      }
   }

   // Draw truth data if it is visible
   if (mDrawTruthAir || mDrawTruthGround)
   {
      for (auto& entity : mTruthEntities)
      {
         // If we have a ground truth entity and we are drawing ground truth
         if (entity.truth.domain == "land")
         {
            if (mDrawTruthGround)
            {
               const QPointF pos = CalculateEntityPosition(entity);
               DrawTruthEntity(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.02);
            }
         }
         else
         {
            // If we have a air truth entity and we are drawing air truth
            if (mDrawTruthAir)
            {
               const QPointF pos = CalculateEntityPosition(entity);
               DrawTruthEntity(vectorXform, entity, pos.x(), pos.y(), 0.02, 0.02);
            }
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::SA_Display::Overlay::DrawTruthEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                               Entity&                  aEntity,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);
   // If we're drawing the hover regions to see where they are
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   // Land domain gets drawn as a square
   if (aEntity.truth.domain == "land")
   {
      entityArray->push_back(osg::Vec2d(-aW * 0.3 * pW(), aH * 0.3 * pH()));
      entityArray->push_back(osg::Vec2d(aW * 0.3 * pW(), aH * 0.3 * pH()));
      entityArray->push_back(osg::Vec2d(aW * 0.3 * pW(), -aH * 0.3 * pH()));
      entityArray->push_back(osg::Vec2d(-aW * 0.3 * pW(), -aH * 0.3 * pH()));
      vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, Color::Magenta).get());
   }
   // Otherwise draw an arrow
   else
   {
      osg::ref_ptr<osg::PositionAttitudeTransform> headingXform(new osg::PositionAttitudeTransform());
      headingXform->setAttitude(osg::Quat((aEntity.truth.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                          osg::Vec3(0.0f, 0.0f, -1.0f)));
      entityArray->push_back(osg::Vec2d(-aW * 0.3 * pW(), -aH * 0.5 * pH()));
      entityArray->push_back(osg::Vec2d(0.0, aH * 0.5 * pH()));
      entityArray->push_back(osg::Vec2d(aW * 0.3 * pW(), -aH * 0.5 * pH()));
      entityArray->push_back(osg::Vec2d(0.0, -aH * 0.35 * pH()));
      headingXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, Color::Magenta).get());
      vectorXform->addChild(headingXform.get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::SA_Display::Overlay::DrawEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                          Entity&                  aEntity,
                                          const double             aX,
                                          const double             aY,
                                          const double             aW,
                                          const double             aH)
{
   // Note: This function should not be used for angle-only entities

   switch (aEntity.perception.identification)
   {
   case ID_BANDIT:
   {
      if (!aEntity.perception.angleOnly)
      {
         DrawBanditEntity(aVectorGroup, aEntity, aX, aY, aW, aH, aEntity.perception.isMissile);
      }
      break;
   }
   case ID_FRIENDLY:
   {
      if (!aEntity.perception.angleOnly)
      {
         DrawFriendlyEntity(aVectorGroup, aEntity, aX, aY, aW, aH, aEntity.perception.isMissile);
      }
      break;
   }
   case ID_NEUTRAL:
   {
      if (!aEntity.perception.angleOnly)
      {
         DrawNeutralEntity(aVectorGroup, aEntity, aX, aY, aW, aH, aEntity.perception.isMissile);
      }
      break;
   }
   default: // ID_UNKNOWN, ID_BOGIE
   {
      if (!aEntity.perception.angleOnly)
      {
         DrawBogieEntity(aVectorGroup, aEntity, aX, aY, aW, aH, aEntity.perception.isMissile);
      }
      break;
   }
   }
}

void wkf::SA_Display::Overlay::DrawBanditEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                Entity&                  aEntity,
                                                const double             aX,
                                                const double             aY,
                                                const double             aW,
                                                const double             aH,
                                                bool                     aIsMissile)
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> baseVectorXform(new osg::PositionAttitudeTransform());
   baseVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   mLineWidthMult = 1.5f; // Use a modified line width

   if (aIsMissile)
   {
      // Filled Triangle
      osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
      boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.25) * pH()));
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.25) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxRectArray, GL_POLYGON, Color::Red).get());

      // Line with arrow
      osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
      arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
      rotVectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::Red).get());

      // Line through the middle
      osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
      middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
      middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
      rotVectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::Red).get());
   }
   else
   {
      // Triangle outline
      osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
      boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Red).get());

      // 40% filled triangle looks better than half fill
      osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
      boxFillArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.3) * pH()));
      boxFillArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.3) * pH()));
      boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::Red).get());

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
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Red).get());
   }

   // Draw altitude if visible and valid
   if (mDrawAltitude)
   {
      DrawAltitudeText(aEntity, aW, Color::Red, baseVectorXform);
   }

   baseVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(baseVectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawFriendlyEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                  Entity&                  aEntity,
                                                  const double             aX,
                                                  const double             aY,
                                                  const double             aW,
                                                  const double             aH,
                                                  bool                     aIsMissile)
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> baseVectorXform(new osg::PositionAttitudeTransform());
   baseVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
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
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   mLineWidthMult = 1.5f; // Use a modified line width

   if (aIsMissile)
   {
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
      rotVectorXform->addChild(CreateGeometry(circleArray, GL_POLYGON, Color::Green).get());

      // Line with arrow
      osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
      arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
      rotVectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::Green).get());

      // Line through the middle
      osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
      middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
      middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
      rotVectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::Green).get());
   }
   else
   {
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
      rotVectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, Color::Green).get());

      // Half filled circle
      osg::ref_ptr<osg::Vec2Array> circleFillArray(new osg::Vec2Array);
      for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = r * cos(ang_rad);
         double y       = -r + r * sin(ang_rad);
         circleFillArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
      rotVectorXform->addChild(CreateGeometry(circleFillArray, GL_POLYGON, Color::Green).get());

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
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Green).get());
   }

   // Draw altitude if visible and valid
   if (mDrawAltitude)
   {
      DrawAltitudeText(aEntity, aW, Color::Green, baseVectorXform);
   }

   baseVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(baseVectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawNeutralEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                 Entity&                  aEntity,
                                                 const double             aX,
                                                 const double             aY,
                                                 const double             aW,
                                                 const double             aH,
                                                 bool                     aIsMissile)
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> baseVectorXform(new osg::PositionAttitudeTransform());
   baseVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
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
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   mLineWidthMult = 1.5f; // Use a modified line width

   if (aIsMissile)
   {
      // Box
      osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.25) * pH()));
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.25) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxRectArray, GL_POLYGON, Color::White).get());

      // Line with arrow
      osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
      arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
      rotVectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::White).get());

      // Line through the middle
      osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
      middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
      middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
      rotVectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::White).get());
   }
   else
   {
      // Box outline
      osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::White).get());

      // Half filled box
      osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
      boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.25) * pH()));
      boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.25) * pH()));
      boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::White).get());

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
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::White).get());
   }

   // Draw altitude if visible and valid
   if (mDrawAltitude)
   {
      DrawAltitudeText(aEntity, aW, Color::White, baseVectorXform);
   }

   baseVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(baseVectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawBogieEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                               Entity&                  aEntity,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH,
                                               bool                     aIsMissile)
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> baseVectorXform(new osg::PositionAttitudeTransform());
   baseVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   mLineWidthMult = 1.5f; // Use a modified line width

   if (aIsMissile)
   {
      // Box
      osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.25) * pW(), -(aH * 0.25) * pH()));
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * pW(), -(aH * 0.25) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxRectArray, GL_POLYGON, Color::Yellow).get());

      // Line with arrow
      osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
      arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * pW(), (aH * 0.3) * pH()));
      arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
      arrowLines->push_back(osg::Vec2d((aW * 0.2) * pW(), (aH * 0.3) * pH()));
      rotVectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::Yellow).get());

      // Line through the middle
      osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
      middleLine->push_back(osg::Vec2d(-(aW * 0.15) * pW(), 0.0));
      middleLine->push_back(osg::Vec2d((aW * 0.15) * pW(), 0.0));
      rotVectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::Yellow).get());
   }
   else
   {
      // Box outline
      osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
      boxRectArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Yellow).get());

      // Half filled box
      osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
      boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.25) * pH()));
      boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.25) * pH()));
      boxFillArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
      rotVectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::Yellow).get());

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
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Yellow).get());
   }

   // Draw altitude if visible and valid
   if (mDrawAltitude)
   {
      DrawAltitudeText(aEntity, aW, Color::Yellow, baseVectorXform);
   }

   baseVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(baseVectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawEntityPriorityTargetThreatNumber(osg::ref_ptr<osg::Group> aVectorGroup,
                                                                    Entity&                  aEntity,
                                                                    const size_t             aPriorityNumber,
                                                                    const double             aX,
                                                                    const double             aY,
                                                                    const double             aW,
                                                                    const double             aH,
                                                                    bool                     aIsThreat)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   osg::Vec4 entityColor = GetEntityColor(aEntity.perception.identification);

   float fontSize = 0.005 * 2.0 * pW();
   if (aIsThreat)
   {
      vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                        -aW * 0.25 * pW(),
                                        aH * 0.25 * pH(),
                                        fontSize,
                                        entityColor,
                                        osgText::Text::RIGHT_BOTTOM)
                               .get());
   }
   else
   {
      vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                        -aW * 0.25 * pW(),
                                        -aH * 0.25 * pH(),
                                        fontSize,
                                        entityColor,
                                        osgText::Text::RIGHT_TOP)
                               .get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::SA_Display::Overlay::DrawFlagEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                              Entity&                  aEntity,
                                              const double             aX,
                                              const double             aY,
                                              const double             aW,
                                              const double             aH)
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> baseVectorXform(new osg::PositionAttitudeTransform());
   baseVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // Set the color
   osg::Vec4 entityColor = Color::Green;

   mLineWidthMult = 1.5f; // Use a modified line width

   if (aEntity.perception.isMissile)
   {
      // Do nothing -- this should not occur, since a missile should not be a FlagEntity or FlightEntity
   }
   else
   {
      // Circle outline
      double                       r = (aW * 0.5);
      osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 18)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = r * cos(ang_rad);
         double y       = -r + r * sin(ang_rad);
         circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
      rotVectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, entityColor).get());

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
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, entityColor).get());
   }

   // Draw altitude if visible and valid
   if (mDrawAltitude)
   {
      DrawAltitudeText(aEntity, aW, entityColor, baseVectorXform);
   }

   // Text in center of circle
   float       fontSize = 0.007 * 2.0 * pW();
   std::string flag     = aEntity.perception.idFlag.substr(0, 1); // Limit string to a single

   double x = 0.0;
   double y = 0.0;
   double r = aW * 0.5;
   if (aEntity.perception.headingValid)
   {
      double deltaAngle_rad = (aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;
      x                     = -r * sin(deltaAngle_rad) * pW();
      y                     = -r * cos(deltaAngle_rad) * pH();
      baseVectorXform->addChild(AddTextItem(flag, x, y, fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }
   else
   {
      baseVectorXform->addChild(
         AddTextItem(flag, 0.0, (-r * pH()), fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }

   baseVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(baseVectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawFlightEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                Entity&                  aEntity,
                                                const double             aX,
                                                const double             aY,
                                                const double             aW,
                                                const double             aH)
{
   float initialLineWidth = mLineWidthMult; // Store the initial width -- we will restore it later

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> baseVectorXform(new osg::PositionAttitudeTransform());
   baseVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // Set the color
   osg::Vec4 entityColor = Color::SlateBlue;

   mLineWidthMult = 1.5f; // Use a modified line width

   if (aEntity.perception.isMissile)
   {
      // Do nothing -- this should not occur, since a missile should not be a FlagEntity or FlightEntity
   }
   else
   {
      // Circle outline
      double                       r = (aW * 0.5);
      osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 18)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = r * cos(ang_rad);
         double y       = -r + r * sin(ang_rad);
         circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
      }
      rotVectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, entityColor).get());

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
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, entityColor).get());
   }

   // Draw altitude if visible and valid
   if (mDrawAltitude)
   {
      DrawAltitudeText(aEntity, aW, entityColor, baseVectorXform);
   }

   // Text in center of circle
   float       fontSize = 0.007 * 2.0 * pW();
   std::string flag     = aEntity.perception.idFlag.substr(0, 1); // Limit string to a single

   double x = 0.0;
   double y = 0.0;
   double r = aW * 0.5;
   if (aEntity.perception.headingValid)
   {
      double deltaAngle_rad = (aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;
      x                     = -r * sin(deltaAngle_rad) * pW();
      y                     = -r * cos(deltaAngle_rad) * pH();
      baseVectorXform->addChild(AddTextItem(flag, x, y, fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }
   else
   {
      baseVectorXform->addChild(
         AddTextItem(flag, 0.0, (-r * pH()), fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }

   baseVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(baseVectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);

   mLineWidthMult = initialLineWidth; // Restore to original width
}

void wkf::SA_Display::Overlay::DrawAltitudeText(Entity&                                      aEntity,
                                                const double                                 aW,
                                                osg::Vec4                                    aEntityColor,
                                                osg::ref_ptr<osg::PositionAttitudeTransform> aXform)
{
   if (aEntity.perception.altitudeValid)
   {
      float fontSize = 0.008 * 2.0 * pW();
      // Altitude (in thousands of ft)
      std::stringstream alt;
      alt << static_cast<int>(aEntity.perception.altitude_ft) / 1000;
      aXform->addChild(
         AddTextItem(alt.str(), (aW * 0.8) * pW(), 0.0, fontSize, aEntityColor, osgText::Text::LEFT_CENTER).get());
   }
}

void wkf::SA_Display::Overlay::DrawGroupEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                               Entity&                  aEntity,
                                               const double             aX,
                                               const double             aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * UnitsPerNm() * pW(), aY * UnitsPerNm() * pH(), 0.0));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
   const QRectF hoverRect = CalculateEntityHoverRegion(aEntity, aX, aY, 0.03, 0.03);

   // If we're drawing the hover regions to see where they are
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * pW(), hoverRect.topLeft().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * pW(), hoverRect.topRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * pW(), hoverRect.bottomRight().y() * pH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * pW(), hoverRect.bottomLeft().y() * pH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // If all elements of a group are bandits draw red.  Otherwise, draw yellow.
   osg::Vec4 entityColor = Color::Red;
   for (auto& entity : aEntity.groupPerception.elementList)
   {
      if (entity.identification != ID_BANDIT)
      {
         entityColor = Color::Yellow;
         break;
      }
   }

   // If the group is coasting, draw at reduced alpha.
   if (aEntity.groupPerception.simtimeAtCoast_sec > 0)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }

   // Calculate the distance in coordinate space per nm, then multiply that times the range radius
   double range = UnitsPerNm() * aEntity.groupPerception.radiusCurrent_nm;

   // Range ring
   osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = range * cos(ang_rad);
      double y       = range * sin(ang_rad);
      ringArray->push_back(osg::Vec2d(x * pW(), y * pH()));
   }

   float fontSize = 0.008 * 2.0 * pW();
   vectorXform->addChild(AddTextItem(aEntity.groupPerception.groupName,
                                     range * cos(45 * UtMath::cRAD_PER_DEG) * pW(),
                                     range * sin(45 * UtMath::cRAD_PER_DEG) * pH(),
                                     fontSize,
                                     entityColor,
                                     osgText::Text::LEFT_BOTTOM)
                            .get());

   vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, entityColor).get());
   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::SA_Display::Overlay::DrawAngleOnlyEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                   Entity&                  aEntity,
                                                   const double             aW,
                                                   const double             aH)
{
   osg::Vec4                                    entityColor = GetEntityColor(aEntity.perception.identification);
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   double                                       angleOnlyDist  = mCompassRadius + (aH * 0.75);
   bool                                         drawDebugLines = false;

   // If we're centered, draw as normal.
   if (mCenterPoint.x() == 0.0 && mCenterPoint.y() == 0.0)
   {
      // Calculate the position and orientation of the entity
      double az_rad = (aEntity.perception.bearing_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;
      if (az_rad < -UtMath::cPI)
      {
         az_rad += UtMath::cTWO_PI;
      }
      if (az_rad > UtMath::cPI)
      {
         az_rad -= UtMath::cTWO_PI;
      }

      double tgtX = angleOnlyDist * sin(az_rad);
      double tgtY = angleOnlyDist * cos(az_rad);

      // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
      const QRectF hoverRect =
         CalculateEntityHoverRegion(aEntity, tgtX * mRange_nm / mCompassRadius, tgtY * mRange_nm / mCompassRadius, aW, aH);

      // If we're drawing the hover regions to see where they are
      if (mDrawHoverRegions)
      {
         // Hover region outline
         osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.topLeft().x() * pW(), (hoverRect.topLeft().y() + mYOffset) * pH()));
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.topRight().x() * pW(), (hoverRect.topRight().y() + mYOffset) * pH()));
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.bottomRight().x() * pW(), (hoverRect.bottomRight().y() + mYOffset) * pH()));
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.bottomLeft().x() * pW(), (hoverRect.bottomLeft().y() + mYOffset) * pH()));
         aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
      }

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * pW(), (tgtY + mYOffset) * pH(), 0.0));
      vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      if (aEntity.perception.isMissile)
      {
         // Entity Diamond
         osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
         entityArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
         entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
         entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
         entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
         vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
      }
      else
      {
         if (aEntity.perception.identification == ID_BANDIT)
         {
            // Entity triangle
            osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
            entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
            vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
         }
         else if (aEntity.perception.identification == ID_FRIENDLY)
         {
            // Entity circle
            double                       r = (aW * 0.5);
            osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
            for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
            {
               double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
               double x       = r * cos(ang_rad);
               double y       = r * sin(ang_rad);
               circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
            }
            vectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, entityColor).get());
         }
         else // ID_BOGIE ID_NEUTRAL ID_UNKNOWN
         {
            // Entity square
            osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
            entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
            vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
         }
      }
   }
   else // Otherwise, calculate the line-circle intersection and draw and rotate appropriately
   {
      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      aEntity.perception.lat_deg,
                                                      aEntity.perception.lon_deg,
                                                      heading_deg,
                                                      distance_m);
      double ang = (heading_deg - 90) * UtMath::cRAD_PER_DEG;

      // Line extending from ownship through the angle only target
      QLineF line = QLineF(QPointF(0, 0), QPointF(1000 * sin(ang), 1000 * cos(ang)));
      line.translate(-mCenterPoint);

      QPointF A          = QPointF(line.x1(), line.y1());
      QPointF B          = QPointF(line.x2(), line.y2());
      QPointF C          = QPointF(0, 0);
      QPointF collision1 = QPointF(0, 0);
      QPointF collision2 = QPointF(0, 0);
      bool    collision  = false;

      // compute the euclidean distance between A and B
      double LAB = sqrt(pow((B.x() - A.x()), 2) + pow((B.y() - A.y()), 2));

      // compute the direction vector D from A to B
      QPointF D = QPointF((B.x() - A.x()) / LAB, (B.y() - A.y()) / LAB);

      // the equation of the line AB is x = Dx*t + Ax, y = Dy*t + Ay with 0 <= t <= LAB.
      // compute the distance between the points A and E, where
      // E is the point of AB closest the circle center (Cx, Cy)
      double t = D.x() * (C.x() - A.x()) + D.y() * (C.y() - A.y());

      // compute the coordinates of the point E
      QPointF E = QPointF(t * D.x() + A.x(), t * D.y() + A.y());

      // compute the euclidean distance between E and C
      double LEC = sqrt(pow((E.x() - C.x()), 2) + pow((E.y() - C.y()), 2));

      // test if the line intersects the circle
      double R = mRange_nm;
      if (LEC < R)
      {
         collision = true;

         // compute distance from t to circle intersection point
         double dt = sqrt(R * R - LEC * LEC);

         // compute first intersection point
         collision1 = QPointF((t - dt) * D.x() + A.x(), (t - dt) * D.y() + A.y());

         // compute second intersection point
         collision2 = QPointF((t + dt) * D.x() + A.x(), (t + dt) * D.y() + A.y());
      }
      else if (LEC == R)
      {
         collision = true;
         // else test if the line is tangent to circle
         // tangent point to circle is E
         collision2 = E;
      }
      else
      {
         // line doesn't touch circle
      }

      // Calculate the angle based on the intersection line
      QLineF lineCol2         = QLineF(QPointF(0, 0), collision2);
      double c2ang            = lineCol2.angle() - 180;
      double compassAngle_rad = (c2ang - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;

      if (compassAngle_rad < -UtMath::cPI)
      {
         compassAngle_rad += UtMath::cTWO_PI;
      }
      if (compassAngle_rad > UtMath::cPI)
      {
         compassAngle_rad -= UtMath::cTWO_PI;
      }

      double tgtX = angleOnlyDist * sin(compassAngle_rad);
      double tgtY = (angleOnlyDist + mYOffset) * cos(compassAngle_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * pW(), tgtY * pH(), 0.0));

      QLineF ownshipToAngleOnlyEntity = QLineF(QPointF(tgtX, tgtY), -mCenterPoint * UnitsPerNm());
      // Calculate the orientation of the angle only entity (point towards ownship)
      double az_rad = ((ownshipToAngleOnlyEntity.angle()) - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;

      if (az_rad < -UtMath::cPI)
      {
         az_rad += UtMath::cTWO_PI;
      }
      if (az_rad > UtMath::cPI)
      {
         az_rad -= UtMath::cTWO_PI;
      }

      vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      osg::ref_ptr<osg::PositionAttitudeTransform> vectorX2form(new osg::PositionAttitudeTransform());
      vectorX2form->setAttitude(osg::Quat(compassAngle_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      // Calculate and set the hover region for this entity, returning a rect that we can draw if we want
      const QRectF hoverRect = CalculateEntityHoverRegion(aEntity,
                                                          (tgtX / UnitsPerNm()) + mCenterPoint.x(),
                                                          (tgtY / UnitsPerNm()) + mCenterPoint.y(),
                                                          aW,
                                                          aH);

      // If we're drawing the hover regions to see where they are
      if (mDrawHoverRegions)
      {
         QRectF rect = hoverRect;
         rect.translate(-mCenterPoint * UnitsPerNm());
         // Hover region outline
         osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
         hoverRegionArray->push_back(osg::Vec2d(rect.topLeft().x() * pW(), (rect.topLeft().y() + mYOffset) * pH()));
         hoverRegionArray->push_back(osg::Vec2d(rect.topRight().x() * pW(), (rect.topRight().y() + mYOffset) * pH()));
         hoverRegionArray->push_back(osg::Vec2d(rect.bottomRight().x() * pW(), (rect.bottomRight().y() + mYOffset) * pH()));
         hoverRegionArray->push_back(osg::Vec2d(rect.bottomLeft().x() * pW(), (rect.bottomLeft().y() + mYOffset) * pH()));
         aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
      }

      if (drawDebugLines)
      {
         osg::ref_ptr<osg::Vec2Array> a123Array(new osg::Vec2Array);
         a123Array->push_back(osg::Vec2d(ownshipToAngleOnlyEntity.x1() * pW(), ownshipToAngleOnlyEntity.y1() * pH()));
         a123Array->push_back(osg::Vec2d(ownshipToAngleOnlyEntity.x2() * pW(), ownshipToAngleOnlyEntity.y2() * pH()));
         aVectorGroup->addChild(CreateGeometry(a123Array, GL_LINES, Color::Yellow).get());

         osg::ref_ptr<osg::Vec2Array> lineArray(new osg::Vec2Array);
         lineArray->push_back(osg::Vec2d(line.x1() * UnitsPerNm() * pW(), line.y1() * UnitsPerNm() * pH()));
         lineArray->push_back(osg::Vec2d(line.x2() * UnitsPerNm() * pW(), line.y2() * UnitsPerNm() * pH()));
         aVectorGroup->addChild(CreateGeometry(lineArray, GL_LINES, Color::Red).get());

         QLineF                       l = QLineF::fromPolar(1000, -90);
         osg::ref_ptr<osg::Vec2Array> line4Array(new osg::Vec2Array);
         line4Array->push_back(osg::Vec2d(l.x1() * UnitsPerNm() * pW(), l.y1() * UnitsPerNm() * pH()));
         line4Array->push_back(osg::Vec2d(l.x2() * UnitsPerNm() * pW(), l.y2() * UnitsPerNm() * pH()));
         vectorX2form->addChild(CreateGeometry(line4Array, GL_LINES, Color::Green).get());
         aVectorGroup->addChild(vectorX2form.get());
      }

      if (collision)
      {
         if (aEntity.perception.isMissile)
         {
            // Entity Diamond
            osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
            entityArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), 0.0));
            entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
            entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), 0.0));
            vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
         }
         else
         {
            if (aEntity.perception.identification == ID_BANDIT)
            {
               // Entity triangle
               osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
               entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
               entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
               entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * pH()));
               vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
            }
            else if (aEntity.perception.identification == ID_FRIENDLY)
            {
               // Entity circle
               double                       r = (aW * 0.5);
               osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
               for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
               {
                  double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
                  double x       = r * cos(ang_rad);
                  double y       = r * sin(ang_rad);
                  circleArray->push_back(osg::Vec2d(x * pW(), y * pH()));
               }
               vectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, entityColor).get());
            }
            else // ID_BOGIE ID_NEUTRAL ID_UNKNOWN
            {
               // Entity square
               osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
               entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH()));
               entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));
               entityArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));
               entityArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));
               vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
            }
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

bool wkf::SA_Display::Overlay::MouseWheel(int aMouseX, int aMouseY, int aDelta)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   {
      // aDelta comes in increments of 120 for some reason.
      // We want to scroll in increments of 5 nm, so just divide by 24
      auto change = aDelta / 24;
      mRange_nm   = (mRange_nm + change) <= 5 ? 5 : mRange_nm + change;
      mRangeLabel->setText(QString::number(mRange_nm));
      return true;
   }
   else
   {
      return false;
   }
}

void wkf::SA_Display::Overlay::DrawAngleOnlyPriorityTargetThreatNumber(osg::ref_ptr<osg::Group> aVectorGroup,
                                                                       Entity&                  aEntity,
                                                                       const size_t             aPriorityNumber,
                                                                       const double             aW,
                                                                       const double             aH,
                                                                       bool                     aIsThreat)
{
   osg::Vec4                                    entityColor = GetEntityColor(aEntity.perception.identification);
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   double                                       angleOnlyDist = mCompassRadius + (aH * 0.75);

   // If we're centered, draw as normal
   if (mCenterPoint.x() == 0.0 && mCenterPoint.y() == 0.0)
   {
      // Calculate the position and orientation of the entity
      double az_rad = (aEntity.perception.bearing_deg - mData.platformData.heading_deg - 1) * UtMath::cRAD_PER_DEG;
      if (az_rad < -UtMath::cPI)
      {
         az_rad += UtMath::cTWO_PI;
      }
      if (az_rad > UtMath::cPI)
      {
         az_rad -= UtMath::cTWO_PI;
      }

      double tgtX = angleOnlyDist * sin(az_rad);
      double tgtY = angleOnlyDist * cos(az_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * pW(), tgtY * pH(), 0.0));

      // Draw the priority threat number
      float fontSize = 0.015 * pH();

      if (aIsThreat)
      {
         vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                           -aW * 0.25 * pW(),
                                           aH * 0.25 * pH(),
                                           fontSize,
                                           entityColor,
                                           osgText::Text::RIGHT_BOTTOM)
                                  .get());
      }
      else
      {
         vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                           -aW * 0.25 * pW(),
                                           -aH * 0.25 * pH(),
                                           fontSize,
                                           entityColor,
                                           osgText::Text::RIGHT_TOP)
                                  .get());
      }
   }
   else // Otherwise, calculate the line-circle intersection and draw and rotate appropriately
   {
      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      aEntity.perception.lat_deg,
                                                      aEntity.perception.lon_deg,
                                                      heading_deg,
                                                      distance_m);
      double ang = (heading_deg - 90) * UtMath::cRAD_PER_DEG;

      // Line extending from ownship through the angle only target
      QLineF line = QLineF(QPointF(0, 0), QPointF(1000 * sin(ang), 1000 * cos(ang)));
      line.translate(-mCenterPoint);

      QPointF A          = QPointF(line.x1(), line.y1());
      QPointF B          = QPointF(line.x2(), line.y2());
      QPointF C          = QPointF(0, 0);
      QPointF collision1 = QPointF(0, 0);
      QPointF collision2 = QPointF(0, 0);
      bool    collision  = false;

      // compute the euclidean distance between A and B
      double LAB = sqrt(pow((B.x() - A.x()), 2) + pow((B.y() - A.y()), 2));

      // compute the direction vector D from A to B
      QPointF D = QPointF((B.x() - A.x()) / LAB, (B.y() - A.y()) / LAB);

      // the equation of the line AB is x = Dx*t + Ax, y = Dy*t + Ay with 0 <= t <= LAB.
      // compute the distance between the points A and E, where
      // E is the point of AB closest the circle center (Cx, Cy)
      double t = D.x() * (C.x() - A.x()) + D.y() * (C.y() - A.y());

      // compute the coordinates of the point E
      QPointF E = QPointF(t * D.x() + A.x(), t * D.y() + A.y());

      // compute the euclidean distance between E and C
      double LEC = sqrt(pow((E.x() - C.x()), 2) + pow((E.y() - C.y()), 2));

      // test if the line intersects the circle
      double R = mRange_nm;
      if (LEC < R)
      {
         collision = true;

         // compute distance from t to circle intersection point
         double dt = sqrt(R * R - LEC * LEC);

         // compute first intersection point
         collision1 = QPointF((t - dt) * D.x() + A.x(), (t - dt) * D.y() + A.y());

         // compute second intersection point
         collision2 = QPointF((t + dt) * D.x() + A.x(), (t + dt) * D.y() + A.y());
      }
      else if (LEC == R)
      {
         collision = true;
         // else test if the line is tangent to circle
         // tangent point to circle is E
         collision2 = E;
      }
      else
      {
         // line doesn't touch circle
      }

      // Calculate the angle based on the intersection line
      QLineF lineCol2         = QLineF(QPointF(0, 0), collision2);
      double c2ang            = lineCol2.angle() - 180;
      double compassAngle_rad = (c2ang - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;

      if (compassAngle_rad < -UtMath::cPI)
      {
         compassAngle_rad += UtMath::cTWO_PI;
      }
      if (compassAngle_rad > UtMath::cPI)
      {
         compassAngle_rad -= UtMath::cTWO_PI;
      }

      double tgtX = angleOnlyDist * sin(compassAngle_rad);
      double tgtY = angleOnlyDist * cos(compassAngle_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * pW(), (tgtY + mYOffset) * pH(), 0.0));

      QLineF ownshipToAngleOnlyEntity = QLineF(QPointF(tgtX, tgtY), -mCenterPoint * UnitsPerNm());
      // Calculate the orientation of the angle only entity (point towards ownship)
      double az_rad = ((ownshipToAngleOnlyEntity.angle()) - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;

      if (az_rad < -UtMath::cPI)
      {
         az_rad += UtMath::cTWO_PI;
      }
      if (az_rad > UtMath::cPI)
      {
         az_rad -= UtMath::cTWO_PI;
      }

      if (collision)
      {
         // Draw the priority target number
         float fontSize = 0.015 * pH();

         double compassAngle_deg = compassAngle_rad * UtMath::cDEG_PER_RAD;
         if (compassAngle_deg > 0 && compassAngle_deg <= 180)
         {
            if (aIsThreat)
            {
               vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                                 aW * 0.75 * pW(),
                                                 aH * 0.25 * pH(),
                                                 fontSize,
                                                 entityColor,
                                                 osgText::Text::LEFT_BOTTOM)
                                        .get());
            }
            else
            {
               vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                                 aW * 0.75 * pW(),
                                                 -aH * 0.25 * pH(),
                                                 fontSize,
                                                 entityColor,
                                                 osgText::Text::LEFT_TOP)
                                        .get());
            }
         }
         else
         {
            if (aIsThreat)
            {
               vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                                 -aW * 0.75 * pW(),
                                                 aH * 0.25 * pH(),
                                                 fontSize,
                                                 entityColor,
                                                 osgText::Text::RIGHT_BOTTOM)
                                        .get());
            }
            else
            {
               vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                                 -aW * 0.75 * pW(),
                                                 -aH * 0.25 * pH(),
                                                 fontSize,
                                                 entityColor,
                                                 osgText::Text::RIGHT_TOP)
                                        .get());
            }
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::SA_Display::Overlay::HandleClickRegions(const int aMouseX, const int aMouseY)
{
   if (!mSelectModeHover)
   {
      // Set the newest mouse position
      mMousePos.setX(aMouseX);
      mMousePos.setY(aMouseY);

      // Loop through each entity and check if we're mousing over it.  Multiple entities hover regions
      // may be hovered over at the same time, so check each entity and set the current entity to the
      // one closest to the mouse.
      mShowingEntityInfo = false; // Set this to true if any entity is being moused over.

      std::vector<Entity> allEntities;
      for (auto& e : mEntities)
      {
         allEntities.push_back(e);
      }
      for (auto& e : mTruthEntities)
      {
         allEntities.push_back(e);
      }
      for (auto& entity : allEntities)
      {
         QPointF rectFromCenter = QPointF((entity.hoverRegion.center().x() - mRect.center().x()) / pW(),
                                          (entity.hoverRegion.center().y() - mRect.center().y()) / pH());
         double distFromCenter = sqrt(rectFromCenter.x() * rectFromCenter.x() + rectFromCenter.y() * rectFromCenter.y());
         // Don't hover over entities drawn behind the stencil buffer.
         // If we are within hover range, evaluate the entity.
         if (distFromCenter <= mCompassRadius)
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               // Compare this entity to the current one.  If the newer distance is less than or equal
               // to the current one update the current entity.
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
               // If this is the first entity contained, set it as we have nothing to compare to yet.
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

void wkf::SA_Display::Overlay::HandleHoverRegions(const int aMouseX, const int aMouseY)
{
   if (mSelectModeHover)
   {
      // Set the newest mouse position
      mMousePos.setX(aMouseX);
      mMousePos.setY(aMouseY);

      // Loop through each entity and check if we're mousing over it.  Multiple entities hover regions
      // may be hovered over at the same time, so check each entity and set the current entity to the
      // one closest to the mouse.
      mShowingEntityInfo = false; // Set this to true if any entity is being moused over.

      std::vector<Entity> allEntities;
      for (auto& e : mEntities)
      {
         allEntities.push_back(e);
      }
      for (auto& e : mTruthEntities)
      {
         allEntities.push_back(e);
      }

      for (auto& entity : allEntities)
      {
         QPointF rectFromCenter = QPointF((entity.hoverRegion.center().x() - mRect.center().x()) / pW(),
                                          ((entity.hoverRegion.center().y() - mRect.center().y()) / pH()) - mYOffset);
         double distFromCenter = sqrt(rectFromCenter.x() * rectFromCenter.x() + rectFromCenter.y() * rectFromCenter.y());
         // Don't hover over entities drawn behind the stencil buffer.
         // If we are within hover range, evaluate the entity.
         if (distFromCenter <= mCompassRadius || entity.perception.angleOnly)
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               // Compare this entity to the current one.  If the newer distance is less than or equal to
               // the current one update the current entity.
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
               // If this is the first entity contained, set it as we have nothing to compare to yet.
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

void wkf::SA_Display::Overlay::DrawConnectionDestroyed()
{
   int h = mRect.height();
   int w = mRect.width();

   // Black color
   const osg::Vec4 blackColor = osg::Vec4(0.0, 0.0, 0.0, 0.5);

   // Red color
   const osg::Vec4 redColor = osg::Vec4(0.9, 0.0, 0.0, 1.0);

   // Fade background
   osg::ref_ptr<osg::Vec2Array> bgArray(new osg::Vec2Array);
   bgArray->push_back(osg::Vec2d(-0.5 * w, 0.5 * h));
   bgArray->push_back(osg::Vec2d(0.5 * w, 0.5 * h));
   bgArray->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   bgArray->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   mOverlayGeode->addChild(CreateGeometry(bgArray, GL_POLYGON, blackColor).get());

   // Top left to bottom right slash
   osg::ref_ptr<osg::Vec2Array> x1array(new osg::Vec2Array);
   x1array->push_back(osg::Vec2d(-0.5 * w, 0.5 * h));
   x1array->push_back(osg::Vec2d(-0.42 * w, 0.5 * h));
   x1array->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   x1array->push_back(osg::Vec2d(0.42 * w, -0.5 * h));
   mOverlayGeode->addChild(CreateGeometry(x1array, GL_POLYGON, redColor).get());

   // Bottom left to top right slash
   osg::ref_ptr<osg::Vec2Array> x2array(new osg::Vec2Array);
   x2array->push_back(osg::Vec2d(-0.42 * w, -0.5 * h));
   x2array->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   x2array->push_back(osg::Vec2d(0.42 * w, 0.5 * h));
   x2array->push_back(osg::Vec2d(0.5 * w, 0.5 * h));
   mOverlayGeode->addChild(CreateGeometry(x2array, GL_POLYGON, redColor).get());
}
