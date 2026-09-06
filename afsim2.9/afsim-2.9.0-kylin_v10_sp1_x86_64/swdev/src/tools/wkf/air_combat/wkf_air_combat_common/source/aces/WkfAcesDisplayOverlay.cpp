// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "aces/WkfAcesDisplayOverlay.hpp"

#include <iomanip>
#include <sstream>

#include <QTime>
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
#include "WkfScenario.hpp"

namespace
{
constexpr float STANDARD_LINE_WIDTH{4.0f};
const osg::Vec4 HIGHLIGHT_COLOR{1.0f, 1.0f, 1.0f, 1.0f};
constexpr float HIGHLIGHT_WIDTH{8.0f};
} // namespace

wkf::AcesDisplay::Overlay::Overlay(const char* aName, vespa::VaViewer* aViewerPtr, const size_t aSourcePlatformIndex)
   : vespa::VaOverlay(aName, vespa::VaOverlay::cOVERLAY_WINDOW)
   , mHandleEventMode(vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   , mSourcePlatform(aSourcePlatformIndex)
{
   mScreenWidth  = aViewerPtr->GetWidth();
   mScreenHeight = aViewerPtr->GetHeight();

   aViewerPtr->AddOverlay(this);

   SetVisible(true);
   SetDrawFunction(std::bind(&Overlay::Draw, this));

   // Set the left click mouse action
   SetButton1ClickAction(
      [this](int x, int y, int s) -> bool
      {
         // Only handle click events when visible
         if (IsVisible())
         {
            SetHasFocus(true);
            HandleClickRegions(x, y);
         }
         return false;
      });

   // Set the mouse move action
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

   // Initialize the displays and what positions they will be at
   mDisplays[DisplayType::cTSD] = DrawRegion::cLEFT;
   mDisplays[DisplayType::cSAD] = DrawRegion::cRIGHT;
   mDisplays[DisplayType::cPFD] = DrawRegion::cTOP;
   mDisplays[DisplayType::cTWD] = DrawRegion::cBOTTOM;

   // Set the initial compass radii for the TSD and SAD
   mTSD_CompassRadius = GetDrawVector(mDisplays[DisplayType::cTSD]).z();
   mSAD_CompassRadius = GetDrawVector(mDisplays[DisplayType::cSAD]).z();

   // Create the animation timers for the 3 settings tabs
   mLeftSettings.animationTimer  = new QElapsedTimer();
   mRightSettings.animationTimer = new QElapsedTimer();
   mTopSettings.animationTimer   = new QElapsedTimer();

   // Initialize the buttons for the 3 settings tabs
   mSettingsButtons[DrawRegion::cLEFT]           = Button();
   mSettingsButtons[DrawRegion::cLEFT].region    = QRectF(-0.4925, -0.075, 0.015, 0.1);
   mSettingsButtons[DrawRegion::cLEFT].function  = std::bind(&Overlay::OpenLeftSettings, this);
   mSettingsButtons[DrawRegion::cRIGHT]          = Button();
   mSettingsButtons[DrawRegion::cRIGHT].region   = QRectF(0.4925, -0.075, 0.015, 0.1);
   mSettingsButtons[DrawRegion::cRIGHT].function = std::bind(&Overlay::OpenRightSettings, this);
   mSettingsButtons[DrawRegion::cTOP]            = Button();
   mSettingsButtons[DrawRegion::cTOP].region     = QRectF(-0.11, 0.345, 0.06, 0.03);
   mSettingsButtons[DrawRegion::cTOP].function   = std::bind(&Overlay::OpenTopSettings, this);

   // Create the buttons for going to the next target on the engagement region
   Button engagementsNextButton;
   engagementsNextButton.region   = QRectF(0.47, 0.339, 0.05, 0.023);
   engagementsNextButton.function = std::bind(&Overlay::NextTarget, this);
   mButtons.push_back(engagementsNextButton);

   // Create the buttons for going to the previous target on the engagement region
   Button engagementsPreviousButton;
   engagementsPreviousButton.region   = QRectF(0.415, 0.339, 0.05, 0.023);
   engagementsPreviousButton.function = std::bind(&Overlay::PreviousTarget, this);
   mButtons.push_back(engagementsPreviousButton);

   // Create the button for turning display sync on and off
   Button displaySyncButton;
   displaySyncButton.region   = QRectF(0.0, -0.075, 0.055, 0.04);
   displaySyncButton.function = std::bind(&Overlay::ToggleDisplaySync, this);
   mButtons.push_back(displaySyncButton);

   // Create the button for swapping the top and left displays
   Button swapTopLeftButton;
   swapTopLeftButton.region   = QRectF(-0.025, 0.2, 0.05, 0.2);
   swapTopLeftButton.function = std::bind(&Overlay::SwapDisplays, this, DrawRegion::cTOP, DrawRegion::cLEFT);
   mButtons.push_back(swapTopLeftButton);

   // Create the button for swapping the top and right displays
   Button swapTopRightButton;
   swapTopRightButton.region   = QRectF(0.025, 0.2, 0.05, 0.2);
   swapTopRightButton.function = std::bind(&Overlay::SwapDisplays, this, DrawRegion::cTOP, DrawRegion::cRIGHT);
   mButtons.push_back(swapTopRightButton);

   // Create the button for swapping the bottom and left displays
   Button swapBottomLeftButton;
   swapBottomLeftButton.region   = QRectF(-0.025, -0.35, 0.05, 0.2);
   swapBottomLeftButton.function = std::bind(&Overlay::SwapDisplays, this, DrawRegion::cBOTTOM, DrawRegion::cLEFT);
   mButtons.push_back(swapBottomLeftButton);

   // Create the button for swapping the bottom and right displays
   Button swapBottomRightButton;
   swapBottomRightButton.region   = QRectF(0.025, -0.35, 0.05, 0.2);
   swapBottomRightButton.function = std::bind(&Overlay::SwapDisplays, this, DrawRegion::cBOTTOM, DrawRegion::cRIGHT);
   mButtons.push_back(swapBottomRightButton);

   // Set the default settings numbers for each display with settings.
   // Only the TSD, SAD, and Engagement region have settings.
   // Settings numbers are a combination of enum values combined with
   // bit arithmetic.
   mSettings[DisplayType::cTSD]    = static_cast<size_t>(TSD_SETTINGS::cDEFAULT);
   mSettings[DisplayType::cSAD]    = static_cast<size_t>(SAD_SETTINGS::cDEFAULT);
   mSettings[DisplayType::cENGAGE] = static_cast<size_t>(ENGAGEMENT_SETTINGS::cDEFAULT);

   SetType<wkf::AcesDisplay::Overlay>();
}

wkf::AcesDisplay::Overlay* wkf::AcesDisplay::Overlay::Clone() const
{
   return new wkf::AcesDisplay::Overlay(*this);
}

const double wkf::AcesDisplay::Overlay::pW() const
{
   return mRect.width();
}

const double wkf::AcesDisplay::Overlay::pH() const
{
   return mRect.height();
}

const double wkf::AcesDisplay::Overlay::rW() const
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

const double wkf::AcesDisplay::Overlay::rH() const
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

bool wkf::AcesDisplay::Overlay::Initialize()
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

void wkf::AcesDisplay::Overlay::Update()
{
   auto epsilon = std::numeric_limits<float>::epsilon() * 10;

   // Update the left settings tab animation timer.
   if (mLeftSettings.animationTimer)
   {
      // If the menu is opening or closing, update its position
      // based on the time elapsed
      if (mLeftSettings.menuState == MenuState::cOPENING)
      {
         auto pos          = static_cast<float>(mLeftSettings.animationTimer->elapsed() / 500.0) / 1.0f;
         mLeftSettings.pos = pos < 0.3f ? pos : 0.3f;
         if (fabs(mLeftSettings.pos - 0.3f) < epsilon)
         {
            mLeftSettings.menuState = MenuState::cOPEN;
         }
      }
      else if (mLeftSettings.menuState == MenuState::cCLOSING)
      {
         auto pos          = static_cast<float>(mLeftSettings.animationTimer->elapsed() / 500.0) / 1.0f;
         mLeftSettings.pos = pos < 0.3f ? 0.3f - pos : 0.0f;
         if (fabs(mLeftSettings.pos - 0.0f) < epsilon)
         {
            mLeftSettings.menuState                       = MenuState::cCLOSED;
            mLeftSettings.visible                         = false;
            mSettingsButtons[DrawRegion::cLEFT].clickable = true;
         }
      }
   }

   // Update the right settings tab animation timer.
   if (mRightSettings.animationTimer)
   {
      // If the menu is opening or closing, update its position
      // based on the time elapsed
      if (mRightSettings.menuState == MenuState::cOPENING)
      {
         auto pos           = static_cast<float>(mRightSettings.animationTimer->elapsed() / 500.0) / 1.0f;
         mRightSettings.pos = pos < 0.3f ? pos : 0.3f;
         if (fabs(mRightSettings.pos - 0.3f) < epsilon)
         {
            mRightSettings.menuState = MenuState::cOPEN;
         }
      }
      else if (mRightSettings.menuState == MenuState::cCLOSING)
      {
         auto pos           = static_cast<float>(mRightSettings.animationTimer->elapsed() / 500.0) / 1.0f;
         mRightSettings.pos = pos < 0.3f ? 0.3f - pos : 0.0f;
         if (fabs(mRightSettings.pos - 0.0f) < epsilon)
         {
            mRightSettings.menuState                       = MenuState::cCLOSED;
            mRightSettings.visible                         = false;
            mSettingsButtons[DrawRegion::cRIGHT].clickable = true;
         }
      }
   }

   // Update the top settings tab animation timer.
   if (mTopSettings.animationTimer)
   {
      // If the menu is opening or closing, update its position
      // based on the time elapsed
      if (mTopSettings.menuState == MenuState::cOPENING)
      {
         auto pos         = static_cast<float>(mTopSettings.animationTimer->elapsed() / 500.0) / 1.0f;
         mTopSettings.pos = pos < 0.25f ? pos : 0.25f;
         if (fabs(mTopSettings.pos - 0.25f) < epsilon)
         {
            mTopSettings.menuState = MenuState::cOPEN;
         }
      }
      else if (mTopSettings.menuState == MenuState::cCLOSING)
      {
         auto pos         = static_cast<float>(mTopSettings.animationTimer->elapsed() / 500.0) / 1.0f;
         mTopSettings.pos = pos < 0.25f ? 0.25f - pos : 0.0f;
         if (fabs(mTopSettings.pos - 0.0f) < epsilon)
         {
            mTopSettings.menuState                       = MenuState::cCLOSED;
            mTopSettings.visible                         = false;
            mSettingsButtons[DrawRegion::cTOP].clickable = true;
         }
      }
   }

   // If an entity is being hovered over/clicked on,
   // update it's info if available
   UpdateEntityInfo();

   // Update the priority targets and threats lists
   // If a platform that was currently showing data
   // is destroyed we need to update the list and show
   // another target if available.
   UpdatePriorityTargetsThreats();

   // Clear out the geodes drawables
   mOverlayGeode->removeDrawables(0, mOverlayGeode->getNumDrawables());

   // Draw
   if (mDrawFunction)
   {
      mDrawFunction();
   }
}

void wkf::AcesDisplay::Overlay::DrawOwnship(osg::ref_ptr<osg::Group> aVectorGroup,
                                            const double             aX,
                                            const double             aY,
                                            const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

   // Draw the aircraft in the center
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d(0.0, 0.0166 * aR * rH()));
   linesArray->push_back(osg::Vec2d(0.0, -0.032 * aR * rH()));
   linesArray->push_back(osg::Vec2d(-0.02266 * aR * rW(), 0.0));
   linesArray->push_back(osg::Vec2d(0.02266 * aR * rW(), 0.0));
   linesArray->push_back(osg::Vec2d(-0.012 * aR * rW(), -0.024 * aR * rH()));
   linesArray->push_back(osg::Vec2d(0.012 * aR * rW(), -0.024 * aR * rH()));
   vectorXform->addChild(CreateGeometry(linesArray, GL_LINES, Color::White).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::DrawFOV(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

   // FOV Lines
   double                       halfFov = mFOV_deg / 2.0;
   double                       ang_rad = halfFov * UtMath::cRAD_PER_DEG;
   double                       x       = 10.0 * sin(ang_rad);
   double                       y       = 10.0 * cos(ang_rad);
   osg::ref_ptr<osg::Vec2Array> fovLinesArray(new osg::Vec2Array);
   fovLinesArray->push_back(osg::Vec2d(0.0, 0.0));
   fovLinesArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   fovLinesArray->push_back(osg::Vec2d(0.0, 0.0));
   fovLinesArray->push_back(osg::Vec2d(-x * rW(), y * rH()));
   vectorXform->addChild(CreateGeometry(fovLinesArray, GL_LINES, Color::SlateBlue).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::SAD_DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup,
                                                const double             aX,
                                                const double             aY,
                                                const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

   size_t rings            = 2;
   double distBetweenRings = aR / rings;
   // Draw our rings, and the range labels on them.
   for (size_t r = 1; r <= rings + 3; r++)
   {
      // Skip the second ring if the ownship is centered to avoid artifacting.
      if (r == 2 && (mSAD_CenterPoint.x() == 0.0 && mSAD_CenterPoint.y() == 0.0) && (mSAD_YOffset == 0.0))
      {
         continue;
      }

      osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
         double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);
         ringArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
      vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::CompassLines).get());

      // Draw the ranges for each ring, where the final ring is mSADRange_nm.
      float             fontSize     = 0.04f * aR * rW();
      double            ring_ang_rad = 60.0 * UtMath::cRAD_PER_DEG;
      double            x            = (0.0 + (distBetweenRings * r)) * cos(ring_ang_rad);
      double            y            = (0.0 + (distBetweenRings * r)) * sin(ring_ang_rad);
      std::stringstream nm;
      nm << std::fixed << std::setprecision(1) << ((mSADRange_nm / static_cast<double>(rings)) * r);
      // We need a small circle so the range is easy to see
      osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double cX      = 0.04 * aR * cos(ang_rad);
         double cY      = 0.04 * aR * sin(ang_rad);
         circleArray->push_back(osg::Vec2d((x + cX) * rW(), (y + cY) * rH()));
      }
      // Draw a circle with the range inside of it
      auto circle = CreateGeometry(circleArray, GL_POLYGON, Color::Background);
      circle->addChild(
         AddTextItem(nm.str(), x * rW(), y * rH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
      vectorXform->addChild(circle.get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

bool wkf::AcesDisplay::Overlay::Btn1Click(int aMouseX, int aMouseY, int aState)
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

bool wkf::AcesDisplay::Overlay::Btn1DblClick(int aMouseX, int aMouseY, int aState)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER && mBtn1Function)
   {
      return mBtn1Function(aMouseX, aMouseY, aState);
   }
   else
   {
      return false;
   }
   return true;
}

bool wkf::AcesDisplay::Overlay::MouseMove(int aMouseX, int aMouseY)
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

bool wkf::AcesDisplay::Overlay::SelectableAt(int aMouseX, int aMouseY) const
{
   if (IsVisible() && (aMouseX > mRect.x()) && (aMouseX < mRect.x() + mRect.width()) && (aMouseY > mRect.y()) &&
       (aMouseY < mRect.y() + mRect.height()))
   {
      return true;
   }
   return false;
}


void wkf::AcesDisplay::Overlay::DrawRoute(osg::ref_ptr<osg::Group> aVectorGroup,
                                          const double             aUnitsPerNm,
                                          const double             aX,
                                          const double             aY)
{
   float fontSize = 0.025f * rW();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

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
         circleArray->push_back(osg::Vec2d(((pos.x() * aUnitsPerNm) + rX) * rW(), ((pos.y() * aUnitsPerNm) + rY) * rH()));
      }
      // Draw a gray circle with the waypoint number inside.
      vectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, Color::Gray));
      vectorXform->addChild(AddTextItem(std::to_string(i),
                                        pos.x() * aUnitsPerNm * rW(),
                                        pos.y() * aUnitsPerNm * rH(),
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
         double lX = ((lastPos.x() * aUnitsPerNm) + rX) * rW();
         double lY = ((lastPos.y() * aUnitsPerNm) + rY) * rH();
         double nX = ((pos.x() * aUnitsPerNm) - rX) * rW();
         double nY = ((pos.y() * aUnitsPerNm) - rY) * rH();
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

osg::ref_ptr<osg::StateSet> wkf::AcesDisplay::Overlay::GetStencilDrawWhereMatch()
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

void wkf::AcesDisplay::Overlay::DrawStencilBufferRegion(osg::ref_ptr<osg::Group>           aVectorGroup,
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

const QPointF wkf::AcesDisplay::Overlay::SAD_CalculateEntityPosition(const Entity& aEntity)
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

const QPointF wkf::AcesDisplay::Overlay::CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm)
{
   // Calculate the orientation of the entity
   double az_rad = NormalizeRadians(aBearingRad - (mData.platformData.heading_deg * UtMath::cRAD_PER_DEG));

   // Calculate the position of the entity
   double tgtX_nm = aRangeNm * sin(az_rad);
   double tgtY_nm = aRangeNm * cos(az_rad);

   return QPointF(tgtX_nm, tgtY_nm);
}

const QRectF wkf::AcesDisplay::Overlay::SAD_CalculateEntityHoverRegion(Entity&      aEntity,
                                                                       const double aX,
                                                                       const double aY,
                                                                       double       aW,
                                                                       const double aH)
{
   auto    drawVector   = GetDrawVector(mDisplays[DisplayType::cSAD]);
   QPointF regionCenter = QPointF(drawVector.x() * pW(), drawVector.y() * pH());

   // Create a polygon with the basic width and height of the entity.
   QPolygonF poly;
   poly.append(QPointF(-(aW * 0.5), (aH * 0.5)));
   poly.append(QPointF((aW * 0.5), (aH * 0.5)));
   poly.append(QPointF((aW * 0.5), -(aH * 0.5)));
   poly.append(QPointF(-(aW * 0.5), -(aH * 0.5)));

   // Create a transform that will translate and then rotate the polygon.
   QTransform t = QTransform().translate(aX * SAD_UnitsPerNm(),
                                         aY * SAD_UnitsPerNm()); // .rotateRadians(-(aEntity.trackData.tgtHdg_rad
                                                                 // - mData.heading));
   // Translate and rotate the polygon to align with the entity.
   QPolygonF nPoly = t.map(poly);
   // Create a bounding rect surrounding the new polygon.
   QRectF bR = nPoly.boundingRect();
   // Set the hover region rect for our entity here, converting to pixel coordinates to match mouse move events.
   QPointF compassCenterPixels = QPointF(mRect.center().x() + regionCenter.x(), mRect.center().y() + regionCenter.y());
   QPointF centerPointOffsetPixels =
      QPointF(mSAD_CenterPoint.x() * SAD_UnitsPerNm() * rW(), mSAD_CenterPoint.y() * SAD_UnitsPerNm() * rH());
   aEntity.hoverRegion.setRect((compassCenterPixels.x() - centerPointOffsetPixels.x()) + (bR.x() * rW()),
                               ((compassCenterPixels.y() - centerPointOffsetPixels.y()) + (mSAD_YOffset * rH())) +
                                  (bR.y() * rH()),
                               bR.width() * rW(),
                               bR.height() * rH());

   // Return the bounding rect in case it will be drawn (debugging).
   return bR;
}

const QPointF wkf::AcesDisplay::Overlay::CalculateMousePosInUnits(const double aMouseX, const double aMouseY)
{
   // Calculate the mouse position in drawing units by dividing by screen size
   double x = (((aMouseX - mRect.center().x()) / pW()));
   double y = (((aMouseY - mRect.center().y()) / pH()));

   return QPointF(x, y);
}

const QPointF wkf::AcesDisplay::Overlay::CalculateMousePosInNm(const double      aMouseX,
                                                               const double      aMouseY,
                                                               const DisplayType aDisplayType)
{
   // Calculate the mouse position in nm for the compasses, using the
   // appropriate yoffset and unit scaling based on where the mouse is.
   auto drawVector = GetDrawVector(mDisplays[aDisplayType]);
   auto yoffset    = (aDisplayType == DisplayType::cTSD) ? mTSD_YOffset : mSAD_YOffset;
   auto unitsPerNm = (aDisplayType == DisplayType::cTSD) ? TSD_UnitsPerNm() : SAD_UnitsPerNm();

   double x = (((aMouseX - (mRect.center().x() + (drawVector.x() * pW()))) / rW())) / unitsPerNm;
   double y = (((aMouseY - (mRect.center().y() + (drawVector.y() * pH()))) / rH()) - yoffset) / unitsPerNm;

   return QPointF(x, y);
}

const osg::Vec4 wkf::AcesDisplay::Overlay::GetEntityColor(const Identification aIdentification) const
{
   switch (aIdentification)
   {
   case Identification::ID_BANDIT:
      return Color::Red;
   case Identification::ID_FRIENDLY:
      return Color::Green;
   case Identification::ID_NEUTRAL:
      return Color::White;
   case Identification::ID_BOGIE:
   case Identification::ID_UNKNOWN:
   default:
      return Color::Yellow;
   }
}

osg::ref_ptr<osg::Geode> wkf::AcesDisplay::Overlay::SAD_GetEntityDrawable(const Entity& aEntity,
                                                                          const double  aW,
                                                                          const double  aH)
{
   osg::ref_ptr<osg::Geode> retGeode(new osg::Geode);

   // Create a drawable geode for an entity based off its identification
   auto entityColor = GetEntityColor(aEntity.perception.identification);
   if (aEntity.perception.isCoasting)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }
   switch (aEntity.perception.identification)
   {
   case Identification::ID_BANDIT:
   {
      if (aEntity.perception.isMissile)
      {
         // Filled Triangle
         osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
         boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
         boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.25) * rH()));
         boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.25) * rH()));
         retGeode->addChild(CreateGeometry(boxRectArray, GL_POLYGON, entityColor).get());

         // Line with arrow
         osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
         arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
         retGeode->addChild(CreateGeometry(arrowLines, GL_LINES, entityColor).get());

         // Line through the middle
         osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
         middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
         middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
         retGeode->addChild(CreateGeometry(middleLine, GL_LINES, entityColor).get());
      }
      else
      {
         // Triangle outline
         osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
         boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
         boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cIMPORTANCE) && aEntity.perception.isImportant)
         {
            SetLineWidthMultiplier(HIGHLIGHT_WIDTH);
            retGeode->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, HIGHLIGHT_COLOR).get());
            SetLineWidthMultiplier(STANDARD_LINE_WIDTH);
         }
         retGeode->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, entityColor).get());

         // 40% filled triangle looks better than half fill
         osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
         boxFillArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.3) * rH()));
         boxFillArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.3) * rH()));
         boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         retGeode->addChild(CreateGeometry(boxFillArray, GL_POLYGON, entityColor).get());

         // Radar dish and heading lines
         double                       r = (aW * 0.5);
         osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

         // Heading line
         radarArray->push_back(osg::Vec2d(0.0, 0.0));
         radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

         // Radar dish
         for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = r * cos(ang_rad);
            double y       = r + r * sin(ang_rad);
            radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
            // Add every element but the first and last twice for GL_LINES
            if (ang_deg != 180 && ang_deg != 360)
            {
               radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
            }
         }
         if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cIMPORTANCE) && aEntity.perception.isImportant)
         {
            SetLineWidthMultiplier(HIGHLIGHT_WIDTH);
            retGeode->addChild(CreateGeometry(radarArray, GL_LINES, HIGHLIGHT_COLOR).get());
            SetLineWidthMultiplier(STANDARD_LINE_WIDTH);
         }
         retGeode->addChild(CreateGeometry(radarArray, GL_LINES, entityColor).get());
      }
      break;
   }
   case Identification::ID_FRIENDLY:
   {
      if (aEntity.perception.isMissile)
      {
         // Circle
         double                       r = (aW * 0.25);
         osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
         for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = r * cos(ang_rad);
            double y       = -r + r * sin(ang_rad);
            circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         }
         retGeode->addChild(CreateGeometry(circleArray, GL_POLYGON, entityColor).get());

         // Line with arrow
         osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
         arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
         retGeode->addChild(CreateGeometry(arrowLines, GL_LINES, entityColor).get());

         // Line through the middle
         osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
         middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
         middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
         retGeode->addChild(CreateGeometry(middleLine, GL_LINES, entityColor).get());
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
            circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         }
         retGeode->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, entityColor).get());

         // Half filled circle
         osg::ref_ptr<osg::Vec2Array> circleFillArray(new osg::Vec2Array);
         for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = r * cos(ang_rad);
            double y       = -r + r * sin(ang_rad);
            circleFillArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         }
         retGeode->addChild(CreateGeometry(circleFillArray, GL_POLYGON, entityColor).get());

         // Radar dish and heading lines
         osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

         // Heading line
         radarArray->push_back(osg::Vec2d(0.0, 0.0));
         radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

         // Radar dish
         for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = r * cos(ang_rad);
            double y       = r + r * sin(ang_rad);
            radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
            // Add every element but the first and last twice for GL_LINES
            if (ang_deg != 180 && ang_deg != 360)
            {
               radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
            }
         }
         retGeode->addChild(CreateGeometry(radarArray, GL_LINES, entityColor).get());
      }
      break;
   }
   default: // ID_NEUTRAL, ID_BOGIE, ID_UNKNOWN
   {
      if (aEntity.perception.isMissile)
      {
         // Box
         osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
         boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), 0.0));
         boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), 0.0));
         boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.25) * rH()));
         boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.25) * rH()));
         retGeode->addChild(CreateGeometry(boxRectArray, GL_POLYGON, entityColor).get());

         // Line with arrow
         osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
         arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
         arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
         retGeode->addChild(CreateGeometry(arrowLines, GL_LINES, entityColor).get());

         // Line through the middle
         osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
         middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
         middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
         retGeode->addChild(CreateGeometry(middleLine, GL_LINES, entityColor).get());
      }
      else
      {
         // Box outline
         osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
         boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), 0.0));
         boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), 0.0));
         boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         retGeode->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, entityColor).get());

         // Half filled box
         osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
         boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.25) * rH()));
         boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.25) * rH()));
         boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         retGeode->addChild(CreateGeometry(boxFillArray, GL_POLYGON, entityColor).get());

         // Radar dish and heading lines
         double                       r = (aW * 0.5);
         osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

         // Heading line
         radarArray->push_back(osg::Vec2d(0.0, 0.0));
         radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

         // Radar dish
         for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = r * cos(ang_rad);
            double y       = r + r * sin(ang_rad);
            radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
            // Add every element but the first and last twice for GL_LINES
            if (ang_deg != 180 && ang_deg != 360)
            {
               radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
            }
         }
         retGeode->addChild(CreateGeometry(radarArray, GL_LINES, entityColor).get());
      }
      break;
   }
   }

   return retGeode;
}

const osg::Vec3 wkf::AcesDisplay::Overlay::GetDrawVector(const DrawRegion aDrawRegion) const
{
   // Return a position and radius for display for
   // a given draw region.  The main displays are left and right,
   // the secondary displays are top and bottom.
   switch (aDrawRegion)
   {
   case DrawRegion::cLEFT:
      return osg::Vec3(-0.25, -0.075, 0.375);
   case DrawRegion::cRIGHT:
      return osg::Vec3(0.25, -0.075, 0.375);
   case DrawRegion::cTOP:
      return osg::Vec3(0.0, 0.2, 0.1);
   case DrawRegion::cBOTTOM:
      return osg::Vec3(0.0, -0.35, 0.1);
   default:
      return osg::Vec3(0, 0, 0);
   }
}

const QPointF wkf::AcesDisplay::Overlay::GetLabelPos(const DrawRegion aDrawRegion) const
{
   // Based on draw region, return a position for the display
   // label to be drawn.
   switch (aDrawRegion)
   {
   case DrawRegion::cLEFT:
      return QPointF(-0.475, 0.2);
   case DrawRegion::cRIGHT:
      return QPointF(0.475, 0.2);
   case DrawRegion::cTOP:
      return QPointF(0.0, 0.325);
   case DrawRegion::cBOTTOM:
      return QPointF(0.0, -0.225);
   default:
      return QPointF(0, 0);
   }
}

const std::vector<std::string> wkf::AcesDisplay::Overlay::GetLabelString(const DrawRegion  aDrawRegion,
                                                                         const DisplayType aDisplayType)
{
   // Based on draw region, get the label to be drawn.
   // Multiple lines will be drawn for the long string
   // (left or right display) to fit the text in a small space.
   bool longString = false;
   switch (aDrawRegion)
   {
   case DrawRegion::cLEFT:
   case DrawRegion::cRIGHT:
      longString = true;
      break;
   case DrawRegion::cTOP:
   case DrawRegion::cBOTTOM:
   default:
      break;
   }

   switch (aDisplayType)
   {
   case DisplayType::cPFD:
      return (longString) ? std::vector<std::string>{"Primary", "Flight", "Display", "(PFD)"} :
                            std::vector<std::string>{"PFD"};
   case DisplayType::cSAD:
      return (longString) ? std::vector<std::string>{"Situation", "Awareness", "Display", "(SAD)"} :
                            std::vector<std::string>{"SAD"};
   case DisplayType::cTSD:
      return (longString) ? std::vector<std::string>{"Tactical", "Situation", "Display", "(TSD)"} :
                            std::vector<std::string>{"TSD"};
   case DisplayType::cTWD:
      return (longString) ? std::vector<std::string>{"Threat", "Warning", "Display", "(TWD)"} :
                            std::vector<std::string>{"TWD"};
   default:
      return std::vector<std::string>{};
   }
}

void wkf::AcesDisplay::Overlay::DrawDisplay(osg::ref_ptr<osg::Group> aVectorGroup,
                                            const DisplayType        aDisplayType,
                                            const DrawRegion         aDrawRegion)
{
   // Get the draw vector for the draw region, and draw
   // the matching display
   auto drawVector = GetDrawVector(aDrawRegion);
   switch (aDisplayType)
   {
   case DisplayType::cPFD:
      DrawPFD(aVectorGroup, drawVector.x(), drawVector.y(), drawVector.z());
      break;
   case DisplayType::cSAD:
      DrawSAD(aVectorGroup, drawVector.x(), drawVector.y(), drawVector.z());
      break;
   case DisplayType::cTSD:
      DrawTSD(aVectorGroup, drawVector.x(), drawVector.y(), drawVector.z());
      break;
   case DisplayType::cTWD:
      DrawTWD(aVectorGroup, drawVector.x(), drawVector.y(), drawVector.z());
      break;
   default:
      break;
   }
   DrawDisplayLabel(aVectorGroup, aDrawRegion, aDisplayType);
}

void wkf::AcesDisplay::Overlay::DrawPFD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Update the transform
   if (mPFD_HorzMatrixNode)
   {
      // Update the rotation of the mPFD_HorzMatrixNode
      osg::Matrix mat;
      float       poffset = mData.platformData.pitch_deg * 6;
      mat.preMultRotate(osg::Quat(mData.platformData.roll_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, 1)));
      mat.preMultTranslate(osg::Vec3(0, -poffset, 0));
      mPFD_HorzMatrixNode->setMatrix(mat);
   }

   float fontSize = 0.12f * aR * rW();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Circle enclosing the region
   osg::ref_ptr<osg::Vec2Array> circleRegionArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad) * rW();
      double y       = aR * sin(ang_rad) * rH();
      circleRegionArray->push_back(osg::Vec2d(x + (aX * pW()), y + (aY * pH())));
   }

   DrawStencilBufferRegion(aVectorGroup, circleRegionArray, 1); // Enable Stencil Buffer
   vectorXform->addChild(mPFD_PosMatrixNode.get());
   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());

   // Circle and text for KCAS
   double                       cX = aR * 0.45 * cos(45 * UtMath::cRAD_PER_DEG) * pW();
   double                       cY = aR * 0.45 * sin(45 * UtMath::cRAD_PER_DEG) * pH();
   osg::ref_ptr<osg::Vec2Array> kcasCircleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * 0.2 * cos(ang_rad) * rW();
      double y       = aR * 0.2 * sin(ang_rad) * rH();
      kcasCircleArray->push_back(osg::Vec2d(x + -cX, y + cY));
   }
   auto              kcasCircle = CreateGeometry(kcasCircleArray, GL_POLYGON, Color::DarkGray);
   std::stringstream kcas;
   kcas << std::setfill('0') << std::setw(3) << static_cast<size_t>(mData.platformData.kcas);
   kcasCircle->addChild(AddTextItem(kcas.str(), -cX, cY, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(kcasCircle.get());
   vectorXform->addChild(CreateGeometry(kcasCircleArray, GL_LINE_LOOP, Color::LightGray).get());

   // Circle and text for Altitude
   osg::ref_ptr<osg::Vec2Array> altCircleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * 0.2 * cos(ang_rad) * rW();
      double y       = aR * 0.2 * sin(ang_rad) * rH();
      altCircleArray->push_back(osg::Vec2d(x + cX, y + cY));
   }
   std::stringstream alt;
   alt << std::setw(5) << std::setprecision(0) << std::setfill('0') << static_cast<size_t>(mData.platformData.altBaro_ft);
   auto altCircle = CreateGeometry(altCircleArray, GL_POLYGON, Color::DarkGray);
   altCircle->addChild(AddTextItem(alt.str(), cX, cY, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(altCircle.get());
   vectorXform->addChild(CreateGeometry(altCircleArray, GL_LINE_LOOP, Color::LightGray).get());

   // Circle and text for G-Load
   osg::ref_ptr<osg::Vec2Array> gloadCircleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * 0.2 * cos(ang_rad) * rW();
      double y       = aR * 0.2 * sin(ang_rad) * rH();
      gloadCircleArray->push_back(osg::Vec2d(x, y - (aR * 0.7 * pH())));
   }
   auto              gloadCircle = CreateGeometry(gloadCircleArray, GL_POLYGON, Color::DarkGray);
   std::stringstream gload;
   auto              gloadVal = mData.platformData.gLoad;
   gload << std::fixed << std::setprecision(1);
   if (gloadVal >= 0.0)
   {
      gload << "+";
   }
   gload << gloadVal;
   gloadCircle->addChild(
      AddTextItem(gload.str(), 0.0, -(aR * 0.7 * pH()), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(gloadCircle.get());
   vectorXform->addChild(CreateGeometry(gloadCircleArray, GL_LINE_LOOP, Color::LightGray).get());

   // Boxes
   double bX = aR * 0.45 * cos(45 * UtMath::cRAD_PER_DEG) * pW();
   double bY = aR * 0.45 * sin(30 * UtMath::cRAD_PER_DEG) * pH();

   // Box and text for Vertical Speed
   osg::ref_ptr<osg::Vec2Array> vertSpeedArray(new osg::Vec2Array);
   vertSpeedArray->push_back(osg::Vec2d(bX + (-(aR * 0.15) * pW()), -bY + ((aR * 0.075) * pH())));
   vertSpeedArray->push_back(osg::Vec2d(bX + ((aR * 0.15) * pW()), -bY + ((aR * 0.075) * pH())));
   vertSpeedArray->push_back(osg::Vec2d(bX + ((aR * 0.15) * pW()), -bY + (-(aR * 0.075) * pH())));
   vertSpeedArray->push_back(osg::Vec2d(bX + (-(aR * 0.15) * pW()), -bY + (-(aR * 0.075) * pH())));
   auto              vertSpeedBox = CreateGeometry(vertSpeedArray, GL_POLYGON, Color::DarkGray);
   std::stringstream vertSpeed;
   auto              vs = static_cast<int>(mData.platformData.vertSpd_fpm);
   vertSpeed << std::setw(5) << std::setprecision(0);
   if (vs >= 0)
   {
      vertSpeed << "+";
   }
   vertSpeed << vs;
   vertSpeedBox->addChild(
      AddTextItem(vertSpeed.str(), bX + ((aR * 0.14) * pW()), -bY, fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());
   vectorXform->addChild(vertSpeedBox.get());
   vectorXform->addChild(CreateGeometry(vertSpeedArray, GL_LINE_LOOP, Color::LightGray).get());

   // Box and text for Angle of Attack
   osg::ref_ptr<osg::Vec2Array> aoaArray(new osg::Vec2Array);
   aoaArray->push_back(osg::Vec2d(-bX + (-(aR * 0.15) * pW()), -bY + ((aR * 0.075) * pH())));
   aoaArray->push_back(osg::Vec2d(-bX + ((aR * 0.15) * pW()), -bY + ((aR * 0.075) * pH())));
   aoaArray->push_back(osg::Vec2d(-bX + ((aR * 0.15) * pW()), -bY + (-(aR * 0.075) * pH())));
   aoaArray->push_back(osg::Vec2d(-bX + (-(aR * 0.15) * pW()), -bY + (-(aR * 0.075) * pH())));
   auto              aoaBox = CreateGeometry(aoaArray, GL_POLYGON, Color::DarkGray);
   std::stringstream angleOfAttack;
   auto              aoa = static_cast<int>(mData.platformData.alpha_deg);
   angleOfAttack << std::setprecision(0) << "A";
   if (aoa >= 0)
   {
      angleOfAttack << "+";
   }
   angleOfAttack << aoa;
   aoaBox->addChild(AddTextItem(angleOfAttack.str(), -bX, -bY, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(aoaBox.get());
   vectorXform->addChild(CreateGeometry(aoaArray, GL_LINE_LOOP, Color::LightGray).get());

   aVectorGroup->addChild(vectorXform.get());
   DrawStencilBufferRegion(aVectorGroup, circleRegionArray, 0); // Release Stencil Buffer
}

void wkf::AcesDisplay::Overlay::DrawSAD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Positioning doesn't use aspect ratio
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Circle enclosing the region
   osg::ref_ptr<osg::Vec2Array> circleRegionArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad) * rW();
      double y       = aR * sin(ang_rad) * rH();
      circleRegionArray->push_back(osg::Vec2d(x, y));
   }

   // Draw the background
   vectorXform->addChild(CreateGeometry(circleRegionArray, GL_POLYGON, Color::Background).get());

   DrawStencilBufferRegion(vectorXform, circleRegionArray, 1); // Enable Stencil Buffer

   // Ownship position to be re-used in multiple functions
   auto ownshipPos =
      QPointF(-(mSAD_CenterPoint.x() * SAD_UnitsPerNm()), mSAD_YOffset - (mSAD_CenterPoint.y() * SAD_UnitsPerNm()));

   // Draw the compass/rings
   SAD_DrawCompass(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);
   // DrawBox(compassGroup, 0, 0, 1.0, 1.0, 1); // Debugging for Stencil Buffer

   // Draw all SAD entities
   SAD_DrawEntities(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);

   // Draw route if visible
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cROUTE))
   {
      DrawRoute(vectorXform, SAD_UnitsPerNm(), ownshipPos.x(), ownshipPos.y());
   }

   // Draw the ownship in the center
   DrawOwnship(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);

   // Draw the FOV lines if visible
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cFOV))
   {
      DrawFOV(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);
   }
   DrawStencilBufferRegion(vectorXform, circleRegionArray, 0); // Release Stencil Buffer

   // Draw the heading above if this display is in one of the main regions (left/right)
   if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT || mDisplays[DisplayType::cSAD] == DrawRegion::cRIGHT)
   {
      DrawHeading(vectorXform, 0.0, aR * rH(), Color::CompassLines);
   }
   vectorXform->addChild(CreateGeometry(circleRegionArray, GL_LINE_LOOP, Color::CompassLines).get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawTSD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Positioning doesn't use aspect ratio
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Circle enclosing the region
   osg::ref_ptr<osg::Vec2Array> circleRegionArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad) * rW();
      double y       = aR * sin(ang_rad) * rH();
      circleRegionArray->push_back(osg::Vec2d(x, y));
   }

   // Background and outer ring
   vectorXform->addChild(CreateGeometry(circleRegionArray, GL_POLYGON, Color::DarkGray).get());
   vectorXform->addChild(CreateGeometry(circleRegionArray, GL_LINE_LOOP, Color::LightGray).get());

   DrawStencilBufferRegion(vectorXform, circleRegionArray, 1); // Enable Stencil Buffer

   // Ownship position to be re-used in multiple functions
   auto ownshipPos =
      QPointF(-(mTSD_CenterPoint.x() * TSD_UnitsPerNm()), mTSD_YOffset - (mTSD_CenterPoint.y() * TSD_UnitsPerNm()));

   // Draw the compass/rings
   TSD_DrawCompass(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);
   // DrawBox(compassGroup, 0, 0, 1.0, 1.0, 1); // Debugging for Stencil Buffer

   // Draw all TSD entities
   TSD_DrawEntities(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);

   // Draw the route if visible
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cROUTE))
   {
      DrawRoute(vectorXform, TSD_UnitsPerNm(), ownshipPos.x(), ownshipPos.y());
   }

   // Draw the ownship in the center of the compass
   DrawOwnship(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);

   // Draw the FOV if visible
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cFOV))
   {
      DrawFOV(vectorXform, ownshipPos.x(), ownshipPos.y(), aR);
   }

   DrawStencilBufferRegion(vectorXform, circleRegionArray, 0); // Release Stencil Buffer

   // Draw the heading above if this display is in one of the main regions (left/right)
   if (mDisplays[DisplayType::cTSD] == DrawRegion::cLEFT || mDisplays[DisplayType::cTSD] == DrawRegion::cRIGHT)
   {
      DrawHeading(vectorXform, 0.0, aR * rH(), Color::White);
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawTWD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR)
{
   // Circle enclosing the region
   osg::ref_ptr<osg::Vec2Array> circleRegionArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad) * rW();
      double y       = aR * sin(ang_rad) * rH();
      circleRegionArray->push_back(osg::Vec2d(x + (aX * pW()), y + (aY * pH())));
   }

   // Draw the background
   aVectorGroup->addChild(CreateGeometry(circleRegionArray, GL_POLYGON, Color::DarkGray).get());

   DrawStencilBufferRegion(aVectorGroup, circleRegionArray, 1); // Enable Stencil Buffer

   // Draw the compass/rings
   DrawTWDCompass(aVectorGroup, aX, aY, aR);

   // Check for a ESM/RWR track processor and draw its tracks if available
   for (auto& trackProcessor : mData.trackProcessorList)
   {
      // Handle ESM/RWR tracks if we have an ESM track processor
      if (trackProcessor.trackProcessorName == mData.esmTrackprocessor)
      {
         // Add any tracks from an ESM/RWR processor to our track list if it has a valid bearing
         for (auto& track : trackProcessor.trackList.mTracks)
         {
            if (track.second.bearingValid)
            {
               DrawTWDTrack(aVectorGroup, track.second, aX, aY, aR);
            }
         }
      }
   }

   DrawStencilBufferRegion(aVectorGroup, circleRegionArray, 0); // Release Stencil Buffer

   aVectorGroup->addChild(CreateGeometry(circleRegionArray, GL_LINE_LOOP, Color::LightGray).get());
}

void wkf::AcesDisplay::Overlay::DrawHeading(osg::ref_ptr<osg::Group> aVectorGroup,
                                            const double             aX,
                                            const double             aY,
                                            const osg::Vec4&         aColor)
{
   double fontSize = 0.03f * pH();

   // Heading text
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

   // Heading
   std::stringstream hdg;
   hdg << std::setfill('0') << std::setw(3) << hdg_deg;
   aVectorGroup->addChild(AddTextItem(hdg.str(), aX, aY + fontSize, fontSize, aColor, osgText::Text::CENTER_CENTER).get());
}

void wkf::AcesDisplay::Overlay::DrawHeaderEng(osg::ref_ptr<osg::Group> aVectorGroup,
                                              const double             aX,
                                              const double             aY,
                                              const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   SetLineWidthMultiplier(8.0f);

   // Green ring for 270 degrees
   osg::ref_ptr<osg::Vec2Array> greenArray(new osg::Vec2Array);
   for (int ang_deg = 90; ang_deg >= -180; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad) * rW();
      double y       = aR * sin(ang_rad) * rH();
      greenArray->push_back(osg::Vec2d(x, y));
   }
   vectorXform->addChild(CreateGeometry(greenArray, GL_LINE_STRIP, Color::Green).get());

   // Red ring for 60 degrees
   osg::ref_ptr<osg::Vec2Array> redArray(new osg::Vec2Array);
   for (int ang_deg = -180; ang_deg >= -240; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad) * rW();
      double y       = aR * sin(ang_rad) * rH();
      redArray->push_back(osg::Vec2d(x, y));
   }
   vectorXform->addChild(CreateGeometry(redArray, GL_LINE_STRIP, Color::Red).get());

   // Thrust needle, slightly smaller radius than the ring size
   double needleAng_deg = 0.0;
   if (mData.flightControlsData.thrustNormalized >= 1.0)
   {
      double abThrustPos = mData.flightControlsData.thrustNormalized - 1.0;
      if (abThrustPos > 0.0)
      {
         // Using afterburner
         needleAng_deg = 270.0 + 60.0 * abThrustPos;
      }
      else
      {
         // No afterburner but full MIL
         needleAng_deg = 270.0;
      }
   }
   else
   {
      needleAng_deg = 270.0 * mData.flightControlsData.thrustNormalized;
   }

   osg::ref_ptr<osg::MatrixTransform> needleXform(new osg::MatrixTransform());
   needleXform->postMult(osg::Matrix::rotate(osg::Quat(needleAng_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, -1))));
   osg::ref_ptr<osg::Vec2Array> needleArray(new osg::Vec2Array);
   needleArray->push_back(osg::Vec2d(0.0, 0.0));
   needleArray->push_back(osg::Vec2d(0.0, aR * 0.8 * rH()));

   // Set the line width for the needle
   SetLineWidthMultiplier(4.0f);

   // If the needle is in the red region, use red, otherwise use green.
   osg::Vec4 needleColor = (needleAng_deg > 270.0) ? Color::Red : Color::Green;
   needleXform->addChild(CreateGeometry(needleArray, GL_LINES, needleColor).get());

   // if (mData.throttleValid)
   if (true)
   {
      double throttleAng_deg = 0.0;
      if (mData.flightControlsData.throttleInput >= 1.0)
      {
         double abThrottlePos = mData.flightControlsData.throttleInput - 1.0;
         if (abThrottlePos > 0.0)
         {
            // Using afterburner
            throttleAng_deg = 270.0 + 60.0 * abThrottlePos;
         }
         else
         {
            // No afterburner but full MIL
            throttleAng_deg = 270.0;
         }
      }
      else
      {
         throttleAng_deg = 270.0 * mData.flightControlsData.throttleInput;
      }

      // Throttle input needle, goes on the outside of the ring
      osg::ref_ptr<osg::MatrixTransform> throttleXform(new osg::MatrixTransform());
      throttleXform->postMult(osg::Matrix::rotate(osg::Quat(throttleAng_deg * UtMath::cRAD_PER_DEG, osg::Vec3(0, 0, -1))));
      osg::ref_ptr<osg::Vec2Array> throttleArray(new osg::Vec2Array);
      throttleArray->push_back(osg::Vec2d(0.0, aR * 0.925 * rH()));
      throttleArray->push_back(osg::Vec2d(0.0, aR * 1.2 * rH()));

      osg::Vec4 throttleColor = (throttleAng_deg > 270.0) ? Color::Red : Color::Green;
      throttleXform->addChild(CreateGeometry(throttleArray, GL_LINES, throttleColor).get());

      vectorXform->addChild(throttleXform.get());
   }

   // Reset the line width
   SetLineWidthMultiplier(STANDARD_LINE_WIDTH);
   vectorXform->addChild(needleXform.get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawHeaderWeight(osg::ref_ptr<osg::Group> aVectorGroup,
                                                 const double             aX,
                                                 const double             aY,
                                                 const double             aW,
                                                 const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double fontSize = 0.02f * pH();

   // Draw our labels.
   vectorXform->addChild(
      AddTextItem("GW", -(aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("TOT", -(aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Green, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("INT", -(aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Green, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("EXT", -(aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Green, osgText::Text::LEFT_CENTER).get());

   // If our fuel data is valid, fill it in, otherwise use N/A for values.
   if (mData.fuelData.fuelValid)
   {
      std::stringstream weight;
      weight << std::fixed << std::setw(3) << std::setprecision(1) << (mData.fuelData.grossWgt_lbs / 1000.0);
      vectorXform->addChild(
         AddTextItem(weight.str(), (aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());

      double            totFuelRemaining = (mData.fuelData.fuelInternal_lbs + mData.fuelData.fuelExternal_lbs) / 1000.0;
      std::stringstream totFuel;
      totFuel << std::fixed << std::setw(3) << std::setprecision(1) << totFuelRemaining;
      vectorXform->addChild(
         AddTextItem(totFuel.str(), (aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());

      std::stringstream intFuel;
      intFuel << std::fixed << std::setw(3) << std::setprecision(1) << (mData.fuelData.fuelInternal_lbs / 1000.0);
      vectorXform->addChild(
         AddTextItem(intFuel.str(), (aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());

      std::stringstream extFuel;
      extFuel << std::fixed << std::setw(3) << std::setprecision(1) << (mData.fuelData.fuelExternal_lbs / 1000.0);
      vectorXform->addChild(
         AddTextItem(extFuel.str(), (aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());
   }
   else
   {
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawHeaderWeapons(osg::ref_ptr<osg::Group> aVectorGroup,
                                                  const double             aX,
                                                  const double             aY,
                                                  const double             aW,
                                                  const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double fontSize = 0.02f * pH();

   // One of 4 possible slots in the box
   const double pos[] = {0.03, 0.01, -0.01, -0.03};
   size_t       index = 0;
   // Draw each weapon and quantity until we have to break.
   // Selected weapons are drawn in white
   for (auto&& weapon : mData.weaponData.weaponList)
   {
      std::string weaponName         = weapon.weaponName;
      std::string selectedWeaponName = mData.weaponData.selectedWeapon;
      for (auto& c : weaponName)
      {
         c = toupper(c);
      }
      for (auto& c : selectedWeaponName)
      {
         c = toupper(c);
      }
      osg::Vec4 tempColor = (weaponName.compare(selectedWeaponName) == 0) ? Color::White : Color::Green;
      vectorXform->addChild(
         AddTextItem(weaponName, -(aW * 0.5) * pW(), pos[index] * pH(), fontSize, tempColor, osgText::Text::LEFT_CENTER).get());
      std::stringstream str;
      str << std::setw(3) << weapon.quantity;
      vectorXform->addChild(
         AddTextItem(str.str(), (aW * 0.5) * pW(), pos[index] * pH(), fontSize, tempColor, osgText::Text::RIGHT_CENTER).get());

      // Break if we've hit 4 weapons.
      index++;
      if (index > 3)
      {
         break;
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawHeaderFuel(osg::ref_ptr<osg::Group> aVectorGroup,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double fontSize = 0.02f * pH();

   // Draw our labels.
   vectorXform->addChild(
      AddTextItem("FF", -(aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::Green, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("JOKER", -(aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Green, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("BINGO", -(aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Green, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("G-LOAD", -(aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());

   // If our fuel data is valid, fill it in, otherwise use N/A for values.
   if (mData.fuelData.fuelValid)
   {
      std::stringstream ff;
      ff << std::fixed << std::setw(5) << std::setprecision(0) << mData.fuelData.fuelFlow_pph;
      vectorXform->addChild(
         AddTextItem(ff.str(), (aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());

      std::stringstream joker;
      joker << std::fixed << std::setw(5) << std::setprecision(0) << mData.fuelData.joker_lbs;
      vectorXform->addChild(
         AddTextItem(joker.str(), (aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());

      std::stringstream bingo;
      bingo << std::fixed << std::setw(5) << std::setprecision(0) << mData.fuelData.bingo_lbs;
      vectorXform->addChild(
         AddTextItem(bingo.str(), (aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER)
            .get());

      std::stringstream gload;
      auto              gloadVal = mData.platformData.gLoad;
      gload << std::fixed << std::setprecision(1);
      if (gloadVal >= 0.0)
      {
         gload << "+";
      }
      gload << gloadVal;
      vectorXform->addChild(
         AddTextItem(gload.str(), (aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Magenta, osgText::Text::RIGHT_CENTER).get());
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawHeaderAltSpeed(osg::ref_ptr<osg::Group> aVectorGroup,
                                                   const double             aX,
                                                   const double             aY,
                                                   const double             aW,
                                                   const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double fontSize = 0.02f * pH();

   // Draw our labels.
   vectorXform->addChild(
      AddTextItem("ALT", -(aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("KCAS", -(aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("KTAS", -(aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("MACH", -(aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());

   // Altitude
   std::stringstream alt;
   auto              altVal = static_cast<size_t>(mData.platformData.altBaro_ft);
   if (altVal != 0)
   {
      alt << std::setw(6) << std::setprecision(0) << altVal;
      vectorXform->addChild(
         AddTextItem(alt.str(), (aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), 0.03 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }

   // KCAS
   if (mData.platformData.kcasValid)
   {
      std::stringstream kcas;
      kcas << std::setfill('0') << std::setw(3) << static_cast<size_t>(mData.platformData.kcas);
      vectorXform->addChild(
         AddTextItem(kcas.str(), (aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), 0.01 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }

   // KTAS
   std::stringstream ktas;
   auto              ktasVal = static_cast<size_t>(mData.platformData.ktas);
   if (ktasVal != 0)
   {
      ktas << std::setfill('0') << std::setw(3) << ktasVal;
      vectorXform->addChild(
         AddTextItem(ktas.str(), (aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), -0.01 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }

   // Mach
   std::stringstream mach;
   if (mData.platformData.mach != 0.0)
   {
      mach << std::setw(3) << std::setprecision(2) << mData.platformData.mach;
      vectorXform->addChild(
         AddTextItem(mach.str(), (aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }
   else
   {
      vectorXform->addChild(
         AddTextItem("N/A", (aW * 0.5) * pW(), -0.03 * pH(), fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawEngagementRegion(osg::ref_ptr<osg::Group> aVectorGroup,
                                                     const double             aX,
                                                     const double             aY,
                                                     const double             aW,
                                                     const double             aH)
{
   bool useKilometers = mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cKM);
   auto kmConversion  = UtMath::cM_PER_NM / 1000.0;

   auto sourcePlatEngageData = mData.engagementData.find(mSourcePlatform);
   auto targetPlatEngageData = mData.engagementData.end();
   if (mTargetPlatform != 0)
   {
      targetPlatEngageData = mData.engagementData.find(mTargetPlatform);
   }

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double fontSize = 0.016f * pH();
   double padding  = (aH * 0.08) * rH();

   // Region rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));   // Top right
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));  // Bottom right
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH())); // Bottom left
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));  // Top left
   vectorXform->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, Color::LightGray).get());
   vectorXform->addChild(AddTextItem("ENGAGEMENT",
                                     -(aW * 0.495) * pW(),
                                     ((aH * 0.5) * pH()) - (padding * 2),
                                     fontSize,
                                     Color::LightGray,
                                     osgText::Text::LEFT_CENTER)
                            .get());

   // Range to target
   std::stringstream range;
   range << "Range: ";

   // If our source has data on the target and the target has data on the source, then draw the data
   if (sourcePlatEngageData != mData.engagementData.end() && targetPlatEngageData != mData.engagementData.end())
   {
      bool   rangeValid    = false;
      double rangeToTarget = 0.0;
      if (sourcePlatEngageData->second.mCurrentRangeMap_nm.find(mTargetPlatform) !=
          sourcePlatEngageData->second.mCurrentRangeMap_nm.end())
      {
         rangeValid    = true;
         rangeToTarget = sourcePlatEngageData->second.mCurrentRangeMap_nm[mTargetPlatform];
         if (useKilometers)
         {
            range << sourcePlatEngageData->second.mCurrentRangeMap_nm[mTargetPlatform] * kmConversion << " km";
         }
         else
         {
            range << sourcePlatEngageData->second.mCurrentRangeMap_nm[mTargetPlatform] << " nm";
         }
      }

      // Max width is 90% of the detection box
      double boxRightLimit = ((aW * 0.5) * pW()) - padding;
      double maxW          = (aW * 0.4) * 0.9;
      double startX        = (aW * 0.1);
      float  sourceDet     = targetPlatEngageData->second.mRawDetectionAgainstTargetMap_nm[mSourcePlatform];
      float  targetDet     = sourcePlatEngageData->second.mRawDetectionAgainstTargetMap_nm[mTargetPlatform];
      double scaleDet      = (sourceDet > targetDet) ? sourceDet : targetDet;

      // DETECTION blue rect
      osg::ref_ptr<osg::Vec2Array> dBlueBoxArray(new osg::Vec2Array);
      double                       sourceW = ((sourceDet / scaleDet) * maxW);
      dBlueBoxArray->push_back(osg::Vec2d((startX + sourceW) * pW(), ((aH * 0.5) * pH()) - padding));  // Top right
      dBlueBoxArray->push_back(osg::Vec2d((startX + sourceW) * pW(), ((aH * 0.42) * pH()) - padding)); // Bottom right
      dBlueBoxArray->push_back(osg::Vec2d(startX * pW(), ((aH * 0.42) * pH()) - padding));             // Bottom left
      dBlueBoxArray->push_back(osg::Vec2d(startX * pW(), ((aH * 0.5) * pH()) - padding));              // Top left
      vectorXform->addChild(CreateGeometry(dBlueBoxArray, GL_POLYGON, Color::EngagementBlue).get());

      // DETECTION red rect
      double                       targetW = ((targetDet / scaleDet) * maxW);
      osg::ref_ptr<osg::Vec2Array> dRedBoxArray(new osg::Vec2Array);
      dRedBoxArray->push_back(osg::Vec2d((startX + targetW) * pW(), ((aH * 0.42) * pH()) - padding)); // Top right
      dRedBoxArray->push_back(osg::Vec2d((startX + targetW) * pW(), ((aH * 0.34) * pH()) - padding)); // Bottom right
      dRedBoxArray->push_back(osg::Vec2d(startX * pW(), ((aH * 0.34) * pH()) - padding));             // Bottom left
      dRedBoxArray->push_back(osg::Vec2d(startX * pW(), ((aH * 0.42) * pH()) - padding));             // Top left
      vectorXform->addChild(CreateGeometry(dRedBoxArray, GL_POLYGON, Color::EngagementRed).get());

      if (rangeValid)
      {
         // DETECTION slant range line
         auto rangeW = ((rangeToTarget / scaleDet) * maxW);
         auto xPos   = (startX + rangeW) * pW();
         // Only draw the line if it is within the box
         if (xPos <= boxRightLimit)
         {
            osg::ref_ptr<osg::Vec2Array> detSlantLineArray(new osg::Vec2Array);
            detSlantLineArray->push_back(osg::Vec2d(xPos, ((aH * 0.34) * pH()) - padding)); // Bottom
            detSlantLineArray->push_back(osg::Vec2d(xPos, ((aH * 0.5) * pH()) - padding));  // Top
            SetLineWidthMultiplier(4.5f);
            vectorXform->addChild(CreateGeometry(detSlantLineArray, GL_LINES, Color::Yellow).get());
            SetLineWidthMultiplier(STANDARD_LINE_WIDTH);
         }
      }

      maxW            = (aW * 0.4) * 0.8; // 80% rather than 90%, we want a gap on the left now
      startX          = (aW * 0.14);      // add the extra 10% to the startX
      auto sourceRmin = targetPlatEngageData->second.mWEZMinRangeMap_nm[mSourcePlatform];
      auto sourceRne  = targetPlatEngageData->second.mWEZNoEscapeMap_nm[mSourcePlatform];
      auto sourceRmax = targetPlatEngageData->second.mWEZMaxRangeMap_nm[mSourcePlatform];

      auto targetRmin = sourcePlatEngageData->second.mWEZMinRangeMap_nm[mTargetPlatform];
      auto targetRne  = sourcePlatEngageData->second.mWEZNoEscapeMap_nm[mTargetPlatform];
      auto targetRmax = sourcePlatEngageData->second.mWEZMaxRangeMap_nm[mTargetPlatform];

      auto scaleWez = (sourceRmax > targetRmax) ? sourceRmax : targetRmax;

      // WEZ source rect
      auto                         sourceWezRMaxW = ((sourceRmax / scaleWez) * maxW);
      auto                         sourceWezRneW  = ((sourceRne / scaleWez) * maxW);
      auto                         sourceWezRMinW = ((sourceRmin / scaleWez) * maxW);
      osg::ref_ptr<osg::Vec2Array> wBlueBoxArray(new osg::Vec2Array);
      wBlueBoxArray->push_back(osg::Vec2d((startX + sourceWezRMaxW) * pW(), (-(aH * 0.0) * pH()) - padding)); // Top right
      wBlueBoxArray->push_back(osg::Vec2d((startX + sourceWezRMaxW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Bottom right
      wBlueBoxArray->push_back(osg::Vec2d((startX + sourceWezRMinW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Bottom left
      wBlueBoxArray->push_back(osg::Vec2d((startX + sourceWezRMinW) * pW(), (-(aH * 0.0) * pH()) - padding)); // Top left
      vectorXform->addChild(CreateGeometry(wBlueBoxArray, GL_POLYGON, Color::EngagementBlue).get());

      // WEZ target rect
      auto                         targetWezRMaxW = ((targetRmax / scaleWez) * maxW);
      auto                         targetWezRneW  = ((targetRne / scaleWez) * maxW);
      auto                         targetWezRMinW = ((targetRmin / scaleWez) * maxW);
      osg::ref_ptr<osg::Vec2Array> wRedBoxArray(new osg::Vec2Array);
      wRedBoxArray->push_back(osg::Vec2d((startX + targetWezRMaxW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Top right
      wRedBoxArray->push_back(osg::Vec2d((startX + targetWezRMaxW) * pW(), (-(aH * 0.16) * pH()) - padding)); // Bottom right
      wRedBoxArray->push_back(osg::Vec2d((startX + targetWezRMinW) * pW(), (-(aH * 0.16) * pH()) - padding)); // Bottom left
      wRedBoxArray->push_back(osg::Vec2d((startX + targetWezRMinW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Top left
      vectorXform->addChild(CreateGeometry(wRedBoxArray, GL_POLYGON, Color::EngagementRed).get());

      // Draw lines
      SetLineWidthMultiplier(4.5f);

      // WEZ source Rmin marker
      osg::ref_ptr<osg::Vec2Array> wezSourceRminLineArray(new osg::Vec2Array);
      wezSourceRminLineArray->push_back(osg::Vec2d((startX + sourceWezRMinW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Bottom
      wezSourceRminLineArray->push_back(osg::Vec2d((startX + sourceWezRMinW) * pW(), (-(aH * 0.0) * pH()) - padding)); // Top
      vectorXform->addChild(CreateGeometry(wezSourceRminLineArray, GL_LINES, Color::White).get());
      vectorXform->addChild(AddTextItem("Rmin",
                                        (startX + sourceWezRMinW) * pW(),
                                        (-(aH * 0.0) * pH()) - (padding * 0.5),
                                        fontSize,
                                        Color::White,
                                        osgText::Text::CENTER_BOTTOM)
                               .get());

      // WEZ source Rne marker
      osg::ref_ptr<osg::Vec2Array> wezSourceRneLineArray(new osg::Vec2Array);
      wezSourceRneLineArray->push_back(osg::Vec2d((startX + sourceWezRneW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Bottom
      wezSourceRneLineArray->push_back(osg::Vec2d((startX + sourceWezRneW) * pW(), (-(aH * 0.0) * pH()) - padding)); // Top
      vectorXform->addChild(CreateGeometry(wezSourceRneLineArray, GL_LINES, Color::White).get());
      vectorXform->addChild(AddTextItem("Rne",
                                        (startX + sourceWezRneW) * pW(),
                                        (-(aH * 0.0) * pH()) - (padding * 0.5),
                                        fontSize,
                                        Color::White,
                                        osgText::Text::CENTER_BOTTOM)
                               .get());

      // WEZ source Rmax marker
      osg::ref_ptr<osg::Vec2Array> wezSourceRmaxLineArray(new osg::Vec2Array);
      wezSourceRmaxLineArray->push_back(osg::Vec2d((startX + sourceWezRMaxW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Bottom
      wezSourceRmaxLineArray->push_back(osg::Vec2d((startX + sourceWezRMaxW) * pW(), (-(aH * 0.0) * pH()) - padding)); // Top
      vectorXform->addChild(CreateGeometry(wezSourceRmaxLineArray, GL_LINES, Color::White).get());
      vectorXform->addChild(AddTextItem("Rmax",
                                        (startX + sourceWezRMaxW) * pW(),
                                        (-(aH * 0.0) * pH()) - (padding * 0.5),
                                        fontSize,
                                        Color::White,
                                        osgText::Text::CENTER_BOTTOM)
                               .get());

      // WEZ target Rmin marker
      osg::ref_ptr<osg::Vec2Array> wezTargetRminLineArray(new osg::Vec2Array);
      wezTargetRminLineArray->push_back(osg::Vec2d((startX + targetWezRMinW) * pW(), (-(aH * 0.16) * pH()) - padding)); // Bottom
      wezTargetRminLineArray->push_back(osg::Vec2d((startX + targetWezRMinW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Top
      vectorXform->addChild(CreateGeometry(wezTargetRminLineArray, GL_LINES, Color::White).get());
      vectorXform->addChild(AddTextItem("Rmin",
                                        (startX + targetWezRMinW) * pW(),
                                        (-(aH * 0.16) * pH()) - (padding * 1.5),
                                        fontSize,
                                        Color::White,
                                        osgText::Text::CENTER_TOP)
                               .get());

      // WEZ target Rne marker
      osg::ref_ptr<osg::Vec2Array> wezTargetRneLineArray(new osg::Vec2Array);
      wezTargetRneLineArray->push_back(osg::Vec2d((startX + targetWezRneW) * pW(), (-(aH * 0.16) * pH()) - padding)); // Bottom
      wezTargetRneLineArray->push_back(osg::Vec2d((startX + targetWezRneW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Top
      vectorXform->addChild(CreateGeometry(wezTargetRneLineArray, GL_LINES, Color::White).get());
      vectorXform->addChild(AddTextItem("Rne",
                                        (startX + targetWezRneW) * pW(),
                                        (-(aH * 0.16) * pH()) - (padding * 1.5),
                                        fontSize,
                                        Color::White,
                                        osgText::Text::CENTER_TOP)
                               .get());

      // WEZ target Rmax marker
      osg::ref_ptr<osg::Vec2Array> wezTargetRmaxLineArray(new osg::Vec2Array);
      wezTargetRmaxLineArray->push_back(osg::Vec2d((startX + targetWezRMaxW) * pW(), (-(aH * 0.16) * pH()) - padding)); // Bottom
      wezTargetRmaxLineArray->push_back(osg::Vec2d((startX + targetWezRMaxW) * pW(), (-(aH * 0.08) * pH()) - padding)); // Top
      vectorXform->addChild(CreateGeometry(wezTargetRmaxLineArray, GL_LINES, Color::White).get());
      vectorXform->addChild(AddTextItem("Rmax",
                                        (startX + targetWezRMaxW) * pW(),
                                        (-(aH * 0.16) * pH()) - (padding * 1.5),
                                        fontSize,
                                        Color::White,
                                        osgText::Text::CENTER_TOP)
                               .get());

      if (rangeValid)
      {
         // WEZ slant line
         auto rangeW = ((rangeToTarget / scaleWez) * maxW);
         // Only draw the line if it is within the box
         auto xPos = (startX + rangeW) * pW();
         if (xPos <= boxRightLimit)
         {
            osg::ref_ptr<osg::Vec2Array> wezSlantLineArray(new osg::Vec2Array);
            wezSlantLineArray->push_back(osg::Vec2d(xPos, (-(aH * 0.16) * pH()) - padding)); // Bottom
            wezSlantLineArray->push_back(osg::Vec2d(xPos, (-(aH * 0.0) * pH()) - padding));  // Top
            vectorXform->addChild(CreateGeometry(wezSlantLineArray, GL_LINES, Color::Yellow).get());
         }
      }

      // Done drawing lines
      SetLineWidthMultiplier(STANDARD_LINE_WIDTH);
   }

   vectorXform->addChild(AddTextItem(range.str(),
                                     -(aW * 0.35) * pW(),
                                     ((aH * 0.5) * pH()) - (padding * 2),
                                     fontSize,
                                     Color::LightGray,
                                     osgText::Text::LEFT_CENTER)
                            .get());

   // DETECTION rect
   osg::ref_ptr<osg::Vec2Array> detectionBoxArray(new osg::Vec2Array);
   detectionBoxArray->push_back(osg::Vec2d(((aW * 0.5) * pW()) - padding, ((aH * 0.5) * pH()) - padding)); // Top right
   detectionBoxArray->push_back(osg::Vec2d(((aW * 0.5) * pW()) - padding, ((aH * 0.34) * pH()) - padding)); // Bottom right
   detectionBoxArray->push_back(osg::Vec2d((aW * 0.1) * pW(), ((aH * 0.34) * pH()) - padding)); // Bottom left
   detectionBoxArray->push_back(osg::Vec2d((aW * 0.1) * pW(), ((aH * 0.5) * pH()) - padding));  // Top left
   vectorXform->addChild(CreateGeometry(detectionBoxArray, GL_LINE_LOOP, Color::LightGray).get());
   vectorXform->addChild(AddTextItem("DETECTION",
                                     (aW * 0.08) * pW(),
                                     ((aH * 0.5) * pH()) - (padding * 2),
                                     fontSize,
                                     Color::LightGray,
                                     osgText::Text::RIGHT_CENTER)
                            .get());

   // WEZ rect
   osg::ref_ptr<osg::Vec2Array> wezBoxArray(new osg::Vec2Array);
   wezBoxArray->push_back(osg::Vec2d(((aW * 0.5) * pW()) - padding, (-(aH * 0.0) * pH()) - padding));  // Top right
   wezBoxArray->push_back(osg::Vec2d(((aW * 0.5) * pW()) - padding, (-(aH * 0.16) * pH()) - padding)); // Bottom right
   wezBoxArray->push_back(osg::Vec2d((aW * 0.1) * pW(), (-(aH * 0.16) * pH()) - padding));             // Bottom left
   wezBoxArray->push_back(osg::Vec2d((aW * 0.1) * pW(), (-(aH * 0.0) * pH()) - padding));              // Top left
   vectorXform->addChild(CreateGeometry(wezBoxArray, GL_LINE_LOOP, Color::LightGray).get());
   vectorXform->addChild(
      AddTextItem("WEZ", (aW * 0.08) * pW(), ((aH * 0.0) * pH()) - (padding * 2), fontSize, Color::LightGray, osgText::Text::RIGHT_CENTER)
         .get());

   // Platform, target, aspect/angleoff det rmin/rne/rmax
   double startY = ((aH * 0.0) * pH()) + padding;
   double row    = -(padding * 0.5) - fontSize;
   vectorXform->addChild(
      AddTextItem("Platforms", -(aW * 0.49) * pW(), startY, fontSize, Color::EngagementGray, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("Aspect/AngleOff", -(aW * 0.35) * pW(), startY, fontSize, Color::EngagementGray, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      AddTextItem("Det", -(aW * 0.18) * pW(), startY, fontSize, Color::EngagementGray, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(
      AddTextItem("Rmin/Rne/Rmax", -(aW * 0.08) * pW(), startY, fontSize, Color::EngagementGray, osgText::Text::CENTER_CENTER)
         .get());

   // If the target is valid
   if (mTargetPlatform != 0)
   {
      wkf::Scenario* scenario = vaEnv.GetStandardScenario();
      if (scenario != nullptr)
      {
         const auto* source = scenario->FindPlatformByIndex(mSourcePlatform);
         const auto* target = scenario->FindPlatformByIndex(mTargetPlatform);
         if (source != nullptr && target != nullptr)
         {
            vectorXform->addChild(AddTextItem(source->GetName(),
                                              -(aW * 0.485) * pW(),
                                              startY + row,
                                              fontSize,
                                              Color::EngagementBlue,
                                              osgText::Text::LEFT_CENTER)
                                     .get());
            vectorXform->addChild(AddTextItem(target->GetName(),
                                              -(aW * 0.485) * pW(),
                                              startY + (row * 2),
                                              fontSize,
                                              Color::EngagementRed,
                                              osgText::Text::LEFT_CENTER)
                                     .get());
         }
      }

      // Fill in the source platforms info from the target platforms engagement data
      if (targetPlatEngageData != mData.engagementData.end())
      {
         std::stringstream aspectAngleOff;
         aspectAngleOff << std::fixed << std::setprecision(0);

         // Aspect
         auto aspectAngleData_deg = targetPlatEngageData->second.mAspectAngleMap_deg.find(mSourcePlatform);
         if (aspectAngleData_deg != targetPlatEngageData->second.mAspectAngleMap_deg.end())
         {
            if (aspectAngleData_deg->second > 0)
            {
               aspectAngleOff << aspectAngleData_deg->second << "R";
            }
            else if (aspectAngleData_deg->second < 0)
            {
               aspectAngleOff << aspectAngleData_deg->second * -1 << "L";
            }
            else
            {
               aspectAngleOff << aspectAngleData_deg->second;
            }
         }
         // else
         //{
         //    aspectAngleOff << 0;
         // }

         aspectAngleOff << " / ";

         // Angle off
         auto angleOffData_deg = targetPlatEngageData->second.mAngleOffMap_deg.find(mSourcePlatform);
         if (angleOffData_deg != targetPlatEngageData->second.mAngleOffMap_deg.end())
         {
            aspectAngleOff << angleOffData_deg->second;
         }
         // else
         //{
         //    aspectAngleOff << 0;
         // }
         vectorXform->addChild(AddTextItem(aspectAngleOff.str(),
                                           -(aW * 0.28) * pW(),
                                           startY + row,
                                           fontSize,
                                           Color::EngagementBlue,
                                           osgText::Text::CENTER_CENTER)
                                  .get());

         // Detection
         std::stringstream det;
         det << std::fixed << std::setprecision(0);
         auto rawDetection_nm = targetPlatEngageData->second.mRawDetectionAgainstTargetMap_nm.find(mSourcePlatform);
         if (rawDetection_nm != targetPlatEngageData->second.mRawDetectionAgainstTargetMap_nm.end())
         {
            if (useKilometers)
            {
               det << rawDetection_nm->second * kmConversion;
            }
            else
            {
               det << rawDetection_nm->second;
            }
         }
         // else
         //{
         //    det << 0;
         // }
         vectorXform->addChild(
            AddTextItem(det.str(), -(aW * 0.18) * pW(), startY + row, fontSize, Color::EngagementBlue, osgText::Text::CENTER_CENTER)
               .get());

         std::stringstream rminRneRmax;
         rminRneRmax << std::fixed << std::setprecision(0);

         // Rmin
         auto minimumRange_nm = targetPlatEngageData->second.mWEZMinRangeMap_nm.find(mSourcePlatform);
         if (minimumRange_nm != targetPlatEngageData->second.mWEZMinRangeMap_nm.end())
         {
            if (useKilometers)
            {
               rminRneRmax << minimumRange_nm->second * kmConversion;
            }
            else
            {
               rminRneRmax << minimumRange_nm->second;
            }
         }
         // else
         //{
         //    rminRneRmax << 0;
         // }

         rminRneRmax << " / ";

         // Rne
         auto noEscapeRange_nm = targetPlatEngageData->second.mWEZNoEscapeMap_nm.find(mSourcePlatform);
         if (noEscapeRange_nm != targetPlatEngageData->second.mWEZNoEscapeMap_nm.end())
         {
            if (useKilometers)
            {
               rminRneRmax << noEscapeRange_nm->second * kmConversion;
            }
            else
            {
               rminRneRmax << noEscapeRange_nm->second;
            }
         }
         // else
         //{
         //    rminRneRmax << 0;
         // }

         rminRneRmax << " / ";

         // Rmax
         auto maximumRange_nm = targetPlatEngageData->second.mWEZMaxRangeMap_nm.find(mSourcePlatform);
         if (maximumRange_nm != targetPlatEngageData->second.mWEZMaxRangeMap_nm.end())
         {
            if (useKilometers)
            {
               rminRneRmax << maximumRange_nm->second * kmConversion;
            }
            else
            {
               rminRneRmax << maximumRange_nm->second;
            }
         }
         // else
         //{
         //    rminRneRmax << 0;
         // }
         vectorXform->addChild(
            AddTextItem(rminRneRmax.str(), -(aW * 0.08) * pW(), startY + row, fontSize, Color::EngagementBlue, osgText::Text::CENTER_CENTER)
               .get());
      }

      // Fill in the target platforms info from the source platforms engagement data
      if (sourcePlatEngageData != mData.engagementData.end())
      {
         std::stringstream aspectAngleOff;
         aspectAngleOff << std::fixed << std::setprecision(0);

         // Aspect
         auto aspectAngleData_deg = sourcePlatEngageData->second.mAspectAngleMap_deg.find(mTargetPlatform);
         if (aspectAngleData_deg != sourcePlatEngageData->second.mAspectAngleMap_deg.end())
         {
            if (aspectAngleData_deg->second > 0)
            {
               aspectAngleOff << aspectAngleData_deg->second << "R";
            }
            else if (aspectAngleData_deg->second < 0)
            {
               aspectAngleOff << aspectAngleData_deg->second * -1 << "L";
            }
            else
            {
               aspectAngleOff << aspectAngleData_deg->second;
            }
         }
         // else
         //{
         //    aspectAngleOff << 0;
         // }

         aspectAngleOff << " / ";

         // Angle off
         auto angleOffData_deg = sourcePlatEngageData->second.mAngleOffMap_deg.find(mTargetPlatform);
         if (angleOffData_deg != sourcePlatEngageData->second.mAngleOffMap_deg.end())
         {
            aspectAngleOff << angleOffData_deg->second;
         }
         // else
         //{
         //    aspectAngleOff << 0;
         // }
         vectorXform->addChild(AddTextItem(aspectAngleOff.str(),
                                           -(aW * 0.28) * pW(),
                                           startY + (row * 2),
                                           fontSize,
                                           Color::EngagementRed,
                                           osgText::Text::CENTER_CENTER)
                                  .get());

         // Detection
         std::stringstream det;
         det << std::fixed << std::setprecision(0);
         auto rawDetection_nm = sourcePlatEngageData->second.mRawDetectionAgainstTargetMap_nm.find(mTargetPlatform);
         if (rawDetection_nm != sourcePlatEngageData->second.mRawDetectionAgainstTargetMap_nm.end())
         {
            det << rawDetection_nm->second;
         }
         // else
         //{
         //    det << 0;
         // }
         vectorXform->addChild(
            AddTextItem(det.str(), -(aW * 0.18) * pW(), startY + (row * 2), fontSize, Color::EngagementRed, osgText::Text::CENTER_CENTER)
               .get());

         std::stringstream rminRneRmax;
         rminRneRmax << std::fixed << std::setprecision(0);

         // Rmin
         auto minimumRange_nm = sourcePlatEngageData->second.mWEZMinRangeMap_nm.find(mTargetPlatform);
         if (minimumRange_nm != sourcePlatEngageData->second.mWEZMinRangeMap_nm.end())
         {
            if (useKilometers)
            {
               rminRneRmax << minimumRange_nm->second * kmConversion;
            }
            else
            {
               rminRneRmax << minimumRange_nm->second;
            }
         }
         // else
         //{
         //    rminRneRmax << 0;
         // }

         rminRneRmax << " / ";

         // Rne
         auto noEscapeRange_nm = sourcePlatEngageData->second.mWEZNoEscapeMap_nm.find(mTargetPlatform);
         if (noEscapeRange_nm != sourcePlatEngageData->second.mWEZNoEscapeMap_nm.end())
         {
            if (useKilometers)
            {
               rminRneRmax << noEscapeRange_nm->second * kmConversion;
            }
            else
            {
               rminRneRmax << noEscapeRange_nm->second;
            }
         }
         // else
         //{
         //    rminRneRmax << 0;
         // }

         rminRneRmax << " / ";

         // Rmax
         auto maximumRange_nm = sourcePlatEngageData->second.mWEZMaxRangeMap_nm.find(mTargetPlatform);
         if (maximumRange_nm != sourcePlatEngageData->second.mWEZMaxRangeMap_nm.end())
         {
            if (useKilometers)
            {
               rminRneRmax << maximumRange_nm->second * kmConversion;
            }
            else
            {
               rminRneRmax << maximumRange_nm->second;
            }
         }
         // else
         //{
         //    rminRneRmax << 0;
         // }
         vectorXform->addChild(AddTextItem(rminRneRmax.str(),
                                           -(aW * 0.08) * pW(),
                                           startY + (row * 2),
                                           fontSize,
                                           Color::EngagementRed,
                                           osgText::Text::CENTER_CENTER)
                                  .get());
      }
   }
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawEngagementSelectMode(osg::ref_ptr<osg::Group> aVectorGroup,
                                                         const double             aX,
                                                         const double             aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.016f * pH();

   std::stringstream selectMode;
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cMANUAL))
   {
      selectMode << "Manual Mode";
   }
   else if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS))
   {
      selectMode << "Targets Mode";
   }
   else if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS))
   {
      selectMode << "Threats Mode";
   }

   vectorXform->addChild(
      AddTextItem(selectMode.str(), 0.0, 0.0, fontSize, Color::LightGray, osgText::Text::RIGHT_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawBehaviorHistory(osg::ref_ptr<osg::Group> aVectorGroup,
                                                    const double             aX,
                                                    const double             aY,
                                                    const double             aW,
                                                    const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   double fontSize = 0.016f * rH();

   // Background rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), (aH * 0.5) * pH()));   // Top right
   boxArray->push_back(osg::Vec2d((aW * 0.5) * pW(), -(aH * 0.5) * pH()));  // Bottom right
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), -(aH * 0.5) * pH())); // Bottom left
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * pW(), (aH * 0.5) * pH()));  // Top left
   auto boxGeode = CreateGeometry(boxArray, GL_POLYGON, Color::BehaviorHistory);

   // If we have behavior history to draw
   if (mData.behaviorHistory.size() > 0)
   {
      // Estimate the font ratio is around 0.6, this is
      // used to position text elements relative to each other
      auto              fontRatio  = 0.6;
      auto              charWidth  = fontSize * fontRatio;
      auto&             newestNode = mData.behaviorHistory.back();
      std::stringstream otherNodes;
      for (int i = 0; i < static_cast<int>(mData.behaviorHistory.size()) - 1; i++)
      {
         otherNodes << mData.behaviorHistory[i] << " | ";
      }

      // Draw our labels.
      auto startX = (aW * 0.5) * pW() - charWidth;
      boxGeode->addChild(AddTextItem(newestNode, startX, 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());
      boxGeode->addChild(
         AddTextItem(otherNodes.str(), startX - (charWidth * newestNode.size()), 0.0, fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER)
            .get());
      boxGeode->addChild(AddTextItem("BEHAVIOR HISTORY: ",
                                     startX - ((charWidth * otherNodes.str().size()) + (charWidth * newestNode.size())),
                                     0.0,
                                     fontSize,
                                     Color::Green,
                                     osgText::Text::RIGHT_CENTER)
                            .get());
   }

   vectorXform->addChild(boxGeode.get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawPlatformSelection(osg::ref_ptr<osg::Group> aVectorGroup,
                                                      const double             aX,
                                                      const double             aY,
                                                      const double             aW,
                                                      const double             aH)
{
   auto disabledColor = Color::LightGray;
   disabledColor.set(disabledColor.x(), disabledColor.y(), disabledColor.z(), disabledColor.w() * 0.4);

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Draw the main box
   double                       fontSize = 0.0175f * pH();
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(aW * 0.5, aH * 0.5));   // Top right
   boxArray->push_back(osg::Vec2d(aW * 0.5, -aH * 0.5));  // Bottom right
   boxArray->push_back(osg::Vec2d(-aW * 0.5, -aH * 0.5)); // Bottom left
   boxArray->push_back(osg::Vec2d(-aW * 0.5, aH * 0.5));  // Top left
   vectorXform->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, disabledColor).get());

   // If our source platform is valid draw its name
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      const auto* source = scenario->FindPlatformByIndex(mSourcePlatform);
      if (source != nullptr)
      {
         vectorXform->addChild(
            AddTextItem(source->GetName(), (-aW * 0.5) + (0.01 * rH()), 0.0, fontSize, disabledColor, osgText::Text::LEFT_CENTER)
               .get());
      }
   }

   osg::ref_ptr<osg::Vec2Array> arrowBoxArray(new osg::Vec2Array);
   double                       sW = (aH * 0.42);
   arrowBoxArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.08), sW));             // Top right
   arrowBoxArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.08), -sW));            // Bottom right
   arrowBoxArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.08) - (sW * 2), -sW)); // Bottom left
   arrowBoxArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.08) - (sW * 2), sW));  // Top left
   vectorXform->addChild(CreateGeometry(arrowBoxArray, GL_LINE_LOOP, disabledColor).get());

   osg::ref_ptr<osg::Vec2Array> arrowArray(new osg::Vec2Array);
   double                       rW = (aH * 0.32);
   arrowArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.18), rW));            // Top right
   arrowArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.18) - rW, -rW));      // Bottom middle
   arrowArray->push_back(osg::Vec2d((aW * 0.5) - (aH * 0.18) - (rW * 2), rW)); // Top left
   vectorXform->addChild(CreateGeometry(arrowArray, GL_POLYGON, disabledColor).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawNextButton(osg::ref_ptr<osg::Group> aVectorGroup,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH,
                                               const osg::Vec4&         aColor)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.02f * pH();

   // Next Button
   osg::ref_ptr<osg::Vec2Array> nextBoxArray(new osg::Vec2Array);
   nextBoxArray->push_back(osg::Vec2d(aW * 0.5, aH * 0.5));   // Top right
   nextBoxArray->push_back(osg::Vec2d(aW * 0.5, -aH * 0.5));  // Bottom right
   nextBoxArray->push_back(osg::Vec2d(-aW * 0.5, -aH * 0.5)); // Bottom left
   nextBoxArray->push_back(osg::Vec2d(-aW * 0.5, aH * 0.5));  // Top left
   vectorXform->addChild(CreateGeometry(nextBoxArray, GL_LINE_LOOP, aColor).get());
   vectorXform->addChild(AddTextItem("Next", 0.0, 0.0, fontSize, aColor, osgText::Text::CENTER_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawPrevButton(osg::ref_ptr<osg::Group> aVectorGroup,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH,
                                               const osg::Vec4&         aColor)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.02f * pH();

   // Previous Button
   osg::ref_ptr<osg::Vec2Array> prevBoxArray(new osg::Vec2Array);
   prevBoxArray->push_back(osg::Vec2d(aW * 0.5, aH * 0.5));   // Top right
   prevBoxArray->push_back(osg::Vec2d(aW * 0.5, -aH * 0.5));  // Bottom right
   prevBoxArray->push_back(osg::Vec2d(-aW * 0.5, -aH * 0.5)); // Bottom left
   prevBoxArray->push_back(osg::Vec2d(-aW * 0.5, aH * 0.5));  // Top left
   vectorXform->addChild(CreateGeometry(prevBoxArray, GL_LINE_LOOP, aColor).get());
   vectorXform->addChild(AddTextItem("Prev", 0.0, 0.0, fontSize, aColor, osgText::Text::CENTER_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                                                    const double             aX,
                                                    const double             aY,
                                                    const double             aW,
                                                    const double             aH)
{
   float fontSize = 0.016f * pH();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Chaff
   vectorXform->addChild(
      AddTextItem("CHAFF", -(aW * 0.5), (aH * 0.5), fontSize, Color::Cyan, osgText::Text::LEFT_TOP).get());
   std::stringstream chaff;
   chaff << std::setw(3) << std::setfill('0') << mData.weaponData.numChaff;
   vectorXform->addChild(
      AddTextItem(chaff.str(), (aW * 0.5), (aH * 0.5), fontSize, Color::LightGray, osgText::Text::RIGHT_TOP).get());

   // Decoys
   vectorXform->addChild(AddTextItem("DECOYS", -(aW * 0.5), 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   std::stringstream decoys;
   decoys << std::setw(3) << std::setfill('0') << mData.weaponData.numDecoys;
   vectorXform->addChild(
      AddTextItem(decoys.str(), (aW * 0.5), 0.0, fontSize, Color::LightGray, osgText::Text::RIGHT_CENTER).get());

   // Flares
   vectorXform->addChild(
      AddTextItem("FLARES", -(aW * 0.5), -(aH * 0.5), fontSize, Color::Cyan, osgText::Text::LEFT_BOTTOM).get());
   std::stringstream flares;
   flares << std::setw(3) << std::setfill('0') << mData.weaponData.numFlares;
   vectorXform->addChild(
      AddTextItem(flares.str(), (aW * 0.5), -(aH * 0.5), fontSize, Color::LightGray, osgText::Text::RIGHT_BOTTOM).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawSelectedWeapon(osg::ref_ptr<osg::Group> aVectorGroup,
                                                   const double             aX,
                                                   const double             aY,
                                                   const double             aW,
                                                   const double             aH)
{
   if (!mData.weaponData.selectedWeapon.empty())
   {
      float fontSize = 0.012f * pH();

      // Transformation
      osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
      vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

      // Box rect
      osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
      boxArray->push_back(osg::Vec2d(-(aW * 0.5), (aH * 0.5)));
      boxArray->push_back(osg::Vec2d((aW * 0.5), (aH * 0.5)));
      boxArray->push_back(osg::Vec2d((aW * 0.5), -(aH * 0.5)));
      boxArray->push_back(osg::Vec2d(-(aW * 0.5), -(aH * 0.5)));

      // Set the border color for the selected weapon box.
      // While the weapon is in safe mode, set to cyan.
      // While armed, set to green.
      // If the selected weapon is out of ammo, set to red.
      osg::Vec4 borderColor;
      if (mData.weaponData.masterArmActive)
      {
         borderColor = Color::Green;
      }
      else
      {
         borderColor = Color::Cyan;
      }
      if (mData.weaponData.numSelectedWeapon <= 0)
      {
         borderColor = Color::Red;
      }
      vectorXform->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, borderColor).get());

      // Draw the selected weapon
      std::string weaponName = mData.weaponData.selectedWeapon;
      for (auto& c : weaponName)
      {
         c = toupper(c);
      }
      vectorXform->addChild(
         AddTextItem("SELECTED:", 0.0, (aH * 0.35), fontSize, Color::White, osgText::Text::CENTER_TOP).get());
      vectorXform->addChild(
         AddTextItem(weaponName, 0.0, -(aH * 0.35), fontSize, Color::White, osgText::Text::CENTER_BOTTOM).get());

      aVectorGroup->addChild(vectorXform.get());
   }
}

void wkf::AcesDisplay::Overlay::DrawDisplaySync(osg::ref_ptr<osg::Group> aVectorGroup,
                                                const double             aX,
                                                const double             aY,
                                                const double             aW,
                                                const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.016f * pH();

   // If display sync is on draw brighter, otherwise dimmer
   auto textColor = mDisplaySync ? Color::LightGray : Color::Gray;
   auto boxColor  = mDisplaySync ? Color::Cyan : Color::DarkCyan;

   // Previous Button
   osg::ref_ptr<osg::Vec2Array> displaySyncBoxArray(new osg::Vec2Array);
   displaySyncBoxArray->push_back(osg::Vec2d(aW * 0.3, aH * 0.5));   // Top right
   displaySyncBoxArray->push_back(osg::Vec2d(aW * 0.5, 0.0));        // Middle right
   displaySyncBoxArray->push_back(osg::Vec2d(aW * 0.3, -aH * 0.5));  // Bottom right
   displaySyncBoxArray->push_back(osg::Vec2d(-aW * 0.3, -aH * 0.5)); // Bottom left
   displaySyncBoxArray->push_back(osg::Vec2d(-aW * 0.5, 0.0));       // Middle left
   displaySyncBoxArray->push_back(osg::Vec2d(-aW * 0.3, aH * 0.5));  // Top left
   vectorXform->addChild(CreateGeometry(displaySyncBoxArray, GL_LINE_LOOP, boxColor).get());
   vectorXform->addChild(AddTextItem("DISPLAY", 0.0, (aH * 0.35), fontSize, textColor, osgText::Text::CENTER_TOP).get());
   vectorXform->addChild(AddTextItem("SYNC", 0.0, -(aH * 0.35), fontSize, textColor, osgText::Text::CENTER_BOTTOM).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawSettingsTab(osg::ref_ptr<osg::Group> aVectorGroup,
                                                const double             aX,
                                                const double             aY,
                                                const double             aW,
                                                const double             aH,
                                                const DrawRegion         aDrawRegion)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   // Aligning the text to screen makes it ignore rotation
   bool alignTextToScreen = false;
   if (aDrawRegion == DrawRegion::cLEFT)
   {
      vectorXform->setAttitude(osg::Quat(-90 * UtMath::cRAD_PER_DEG, osg::Vec3(0.0f, 0.0f, 1.0f)));
   }
   else if (aDrawRegion == DrawRegion::cRIGHT)
   {
      vectorXform->setAttitude(osg::Quat(90 * UtMath::cRAD_PER_DEG, osg::Vec3(0.0f, 0.0f, 1.0f)));
   }
   else if (aDrawRegion == DrawRegion::cTOP)
   {
      vectorXform->setAttitude(osg::Quat(180 * UtMath::cRAD_PER_DEG, osg::Vec3(0.0f, 0.0f, 1.0f)));
      alignTextToScreen = true;
   }

   double fontSize = 0.016f * pH();

   // Settings box
   osg::ref_ptr<osg::Vec2Array> settingsBoxArray(new osg::Vec2Array);
   settingsBoxArray->push_back(osg::Vec2d(aW * 0.4, aH * 0.5));   // Top right
   settingsBoxArray->push_back(osg::Vec2d(aW * 0.5, -aH * 0.5));  // Bottom right
   settingsBoxArray->push_back(osg::Vec2d(-aW * 0.5, -aH * 0.5)); // Bottom left
   settingsBoxArray->push_back(osg::Vec2d(-aW * 0.4, aH * 0.5));  // Top left
   vectorXform->addChild(CreateGeometry(settingsBoxArray, GL_LINE_LOOP, Color::LightGray).get());
   vectorXform->addChild(
      AddTextItem("SETTINGS", 0.0, 0.0, fontSize, Color::LightGray, osgText::Text::CENTER_CENTER, alignTextToScreen).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::TSD_DrawEntityInfo(osg::ref_ptr<osg::Group> aVectorGroup,
                                                   const double             aX,
                                                   const double             aY,
                                                   const double             aW,
                                                   const double             aH)
{
   double fontSize = 0.02f * pH();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Box fill rect
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::Black).get());

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, Color::White).get());

   if (mTSD_CurrentEntity.type == TSD_EntityType::TRACK)
   {
      vectorXform->addChild(
         AddTextItem("TGT:", -(aW * 0.45) * rW(), (aH * 0.42) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      // Shrink the name size if necessary (long names) This needs improved (font width/height ratio is around 0.6)
      if (mTSD_CurrentEntity.track.tgtPlatformName.size() > 13)
      {
         fontSize = (13 * fontSize / mTSD_CurrentEntity.track.tgtPlatformName.size());
      }
      std::stringstream tgt;
      tgt << mTSD_CurrentEntity.track.tgtPlatformName;
      vectorXform->addChild(
         AddTextItem(tgt.str(), (aW * 0.45) * rW(), (aH * 0.42) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      vectorXform->addChild(
         AddTextItem("ID:", -(aW * 0.45) * rW(), (aH * 0.315) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      std::stringstream id;
      // Shrink the id size if necessary (long ids)
      if (mTSD_CurrentEntity.track.tgtIdString.size() > 14)
      {
         fontSize = (14 * fontSize / mTSD_CurrentEntity.track.tgtIdString.size());
      }
      id << mTSD_CurrentEntity.track.tgtIdString;
      vectorXform->addChild(
         AddTextItem(id.str(), (aW * 0.45) * rW(), (aH * 0.315) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      vectorXform->addChild(
         AddTextItem("ALT:", -(aW * 0.45) * rW(), (aH * 0.21) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.track.altitudeValid)
      {
         std::stringstream alt;
         alt << std::fixed << std::setprecision(0) << std::setw(5) << std::setfill('0')
             << mTSD_CurrentEntity.track.tgtAlt_ft;
         vectorXform->addChild(
            AddTextItem(alt.str(), (aW * 0.45) * rW(), (aH * 0.21) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         AddTextItem("SPD:", -(aW * 0.45) * rW(), (aH * 0.105) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.track.targetSpeedValid)
      {
         std::stringstream spd;
         spd << std::fixed << std::setprecision(0) << std::setw(5) << mTSD_CurrentEntity.track.tgtSpeed_kts;
         vectorXform->addChild(
            AddTextItem(spd.str(), (aW * 0.45) * rW(), (aH * 0.105) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         AddTextItem("HDG:", -(aW * 0.45) * rW(), 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
      if (mTSD_CurrentEntity.track.targetHdgValid)
      {
         double heading_deg = mTSD_CurrentEntity.track.tgtHdg_deg;
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
            AddTextItem(hdg.str(), (aW * 0.45) * rW(), 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());
      }

      vectorXform->addChild(
         AddTextItem("RNG:", -(aW * 0.45) * rW(), (aH * -0.105) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.track.rangeValid)
      {
         std::stringstream rng;
         rng << std::fixed << std::setprecision(0) << std::setw(5) << mTSD_CurrentEntity.track.tgtRange_nm;
         vectorXform->addChild(
            AddTextItem(rng.str(), (aW * 0.45) * rW(), (aH * -0.105) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         AddTextItem("RATE:", -(aW * 0.45) * rW(), (aH * -0.21) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.track.rangeRateValid)
      {
         std::stringstream rate;
         rate << std::fixed << std::setprecision(0) << std::setw(5) << mTSD_CurrentEntity.track.tgtRangeRate_kts;
         vectorXform->addChild(
            AddTextItem(rate.str(), (aW * 0.45) * rW(), (aH * -0.21) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         AddTextItem("ASPECT:", -(aW * 0.45) * rW(), (aH * -0.315) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.track.targetAspectValid)
      {
         std::stringstream aspect;
         aspect << std::fixed << std::setprecision(0) << std::setw(4) << mTSD_CurrentEntity.track.tgtAspect_deg;
         vectorXform->addChild(
            AddTextItem(aspect.str(), (aW * 0.45) * rW(), (aH * -0.315) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }
   }
   else if (mTSD_CurrentEntity.type == TSD_EntityType::ASSET)
   {
      vectorXform->addChild(
         AddTextItem("TGT:", -(aW * 0.45) * rW(), (aH * 0.42) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      // Shrink the name size if necessary (long names)
      if (mTSD_CurrentEntity.perceivedData.perceivedName.size() > 13)
      {
         fontSize = (13 * fontSize / mTSD_CurrentEntity.perceivedData.perceivedName.size());
      }
      std::stringstream tgt;
      tgt << mTSD_CurrentEntity.perceivedData.perceivedName;
      vectorXform->addChild(
         AddTextItem(tgt.str(), (aW * 0.45) * rW(), (aH * 0.42) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      vectorXform->addChild(
         AddTextItem("ID:", -(aW * 0.45) * rW(), (aH * 0.315) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      std::stringstream id;
      // Shrink the id size if necessary (long ids)
      if (mTSD_CurrentEntity.perceivedData.idString.size() > 14)
      {
         fontSize = (14 * fontSize / mTSD_CurrentEntity.perceivedData.idString.size());
      }
      id << mTSD_CurrentEntity.perceivedData.idString;
      vectorXform->addChild(
         AddTextItem(id.str(), (aW * 0.45) * rW(), (aH * 0.315) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      vectorXform->addChild(
         AddTextItem("ALT:", -(aW * 0.45) * rW(), (aH * 0.21) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.perceivedData.altitudeValid)
      {
         std::stringstream alt;
         alt << std::fixed << std::setprecision(0) << std::setw(5) << std::setfill('0')
             << mTSD_CurrentEntity.perceivedData.altitude_ft;
         vectorXform->addChild(
            AddTextItem(alt.str(), (aW * 0.45) * rW(), (aH * 0.21) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         AddTextItem("SPD:", -(aW * 0.45) * rW(), (aH * 0.105) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      if (mTSD_CurrentEntity.perceivedData.speedValid)
      {
         std::stringstream spd;
         spd << std::fixed << std::setprecision(0) << std::setw(5) << mTSD_CurrentEntity.perceivedData.speed_kts;
         vectorXform->addChild(
            AddTextItem(spd.str(), (aW * 0.45) * rW(), (aH * 0.105) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
               .get());
      }

      vectorXform->addChild(
         AddTextItem("HDG:", -(aW * 0.45) * rW(), 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
      if (mTSD_CurrentEntity.perceivedData.headingValid)
      {
         double heading_deg = mTSD_CurrentEntity.perceivedData.heading_deg;
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
            AddTextItem(hdg.str(), (aW * 0.45) * rW(), 0.0, fontSize, Color::White, osgText::Text::RIGHT_CENTER).get());
      }

      vectorXform->addChild(
         AddTextItem("RNG:", -(aW * 0.45) * rW(), (aH * -0.105) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      mTSD_CurrentEntity.perceivedData.lat_deg,
                                                      mTSD_CurrentEntity.perceivedData.lon_deg,
                                                      heading_deg,
                                                      distance_m);
      std::stringstream rng;
      rng << std::fixed << std::setprecision(0) << std::setw(5) << distance_m * UtMath::cNM_PER_M;
      vectorXform->addChild(
         AddTextItem(rng.str(), (aW * 0.45) * rW(), (aH * -0.105) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      vectorXform->addChild(
         AddTextItem("RATE:", -(aW * 0.45) * rW(), (aH * -0.21) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());

      vectorXform->addChild(
         AddTextItem("ASPECT:", -(aW * 0.45) * rW(), (aH * -0.315) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
            .get());
   }

   // Make sure the text draws on top of the polygons
   osg::StateSet* ss = vectorXform->getOrCreateStateSet();
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::SAD_DrawEntityInfo(osg::ref_ptr<osg::Group> aVectorGroup,
                                                   const double             aX,
                                                   const double             aY,
                                                   const double             aW,
                                                   const double             aH)
{
   double fontSize = 0.02f * pH();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Box fill rect
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::Black).get());

   // Box rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
   boxArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxArray, GL_LINE_LOOP, Color::White).get());

   vectorXform->addChild(
      AddTextItem("TRUTH", -(aW * 0.45) * rW(), (aH * 0.42) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("  NAME:", -(aW * 0.45) * rW(), (aH * 0.315) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      AddTextItem("  TYPE:", -(aW * 0.45) * rW(), (aH * 0.21) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      AddTextItem("  RANGE(nm):", -(aW * 0.45) * rW(), (aH * 0.105) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   vectorXform->addChild(
      AddTextItem("PERCEPTION", -(aW * 0.45) * rW(), 0.0, fontSize, Color::White, osgText::Text::LEFT_CENTER).get());
   vectorXform->addChild(
      AddTextItem("  TYPE:", -(aW * 0.45) * rW(), (aH * -0.105) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   vectorXform->addChild(
      AddTextItem("  RANGE(nm):", -(aW * 0.45) * rW(), (aH * -0.21) * rH(), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   if (mSAD_CurrentEntity.isTruth)
   {
      std::stringstream truthName;
      // Shrink the size if necessary
      if (mSAD_CurrentEntity.truth.name.size() > 10)
      {
         fontSize = (10 * fontSize / mSAD_CurrentEntity.truth.name.size());
      }
      truthName << mSAD_CurrentEntity.truth.name;
      vectorXform->addChild(
         AddTextItem(truthName.str(), (aW * 0.45) * rW(), (aH * 0.315) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      std::stringstream truthType;
      // Shrink the size if necessary
      if (mSAD_CurrentEntity.truth.type.size() > 10)
      {
         fontSize = (10 * fontSize / mSAD_CurrentEntity.truth.type.size());
      }
      truthType << mSAD_CurrentEntity.truth.type;
      vectorXform->addChild(
         AddTextItem(truthType.str(), (aW * 0.45) * rW(), (aH * 0.21) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      double heading_deg = 0.0;
      double distance_m  = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                      mData.platformData.lon_deg,
                                                      mSAD_CurrentEntity.truth.lat_deg,
                                                      mSAD_CurrentEntity.truth.lon_deg,
                                                      heading_deg,
                                                      distance_m);
      std::stringstream truthRange;
      truthRange << std::fixed << std::setprecision(2) << QString::number(distance_m * UtMath::cNM_PER_M).toStdString();
      vectorXform->addChild(
         AddTextItem(truthRange.str(), (aW * 0.45) * rW(), (aH * 0.105) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());
   }
   else
   {
      std::string       type = mSAD_CurrentEntity.isGroup ? mSAD_CurrentEntity.groupPerception.groupName :
                                                            mSAD_CurrentEntity.perception.perceivedType;
      std::stringstream perceptionType;
      // Shrink the size if necessary
      if (type.size() > 10)
      {
         fontSize = (10 * fontSize / type.size());
      }
      perceptionType << type;
      vectorXform->addChild(
         AddTextItem(perceptionType.str(), (aW * 0.45) * rW(), (aH * -0.105) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());

      fontSize = 0.02f * rH();

      double heading_deg = 0.0;
      double distance_m  = 0.0;
      if (mSAD_CurrentEntity.isGroup)
      {
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                         mData.platformData.lon_deg,
                                                         mSAD_CurrentEntity.groupPerception.centroidLat_deg,
                                                         mSAD_CurrentEntity.groupPerception.centroidLon_deg,
                                                         heading_deg,
                                                         distance_m);
      }
      else
      {
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                         mData.platformData.lon_deg,
                                                         mSAD_CurrentEntity.perception.lat_deg,
                                                         mSAD_CurrentEntity.perception.lon_deg,
                                                         heading_deg,
                                                         distance_m);
      }
      std::stringstream perceptionRange;
      perceptionRange << std::fixed << std::setprecision(2)
                      << QString::number(distance_m * UtMath::cNM_PER_M).toStdString();
      vectorXform->addChild(
         AddTextItem(perceptionRange.str(), (aW * 0.45) * rW(), (aH * -0.21) * rH(), fontSize, Color::White, osgText::Text::RIGHT_CENTER)
            .get());
   }

   // Make sure the text draws on top of the polygons
   osg::StateSet* ss = vectorXform->getOrCreateStateSet();
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawPlatformComment(osg::ref_ptr<osg::Group> aVectorGroup,
                                                    const double             aX,
                                                    const double             aY,
                                                    const double             aW,
                                                    const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double            fontSize = 0.016f * pH();
   std::stringstream platformComment;
   if (mPlatformComment.empty())
   {
      platformComment << "** Platform Comment Region **";
   }
   else
   {
      platformComment << mPlatformComment;
   }
   vectorXform->addChild(
      AddTextItem(platformComment.str(), 0.0, 0.0, fontSize, Color::Cyan, osgText::Text::RIGHT_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::OpenLeftSettings()
{
   mLeftSettings.animationTimer->start();
   mLeftSettings.visible                         = true;
   mSettingsButtons[DrawRegion::cLEFT].clickable = false;
   mLeftSettings.menuState                       = MenuState::cOPENING;
}

void wkf::AcesDisplay::Overlay::CloseLeftSettings()
{
   mLeftSettings.animationTimer->start();
   mLeftSettings.menuState = MenuState::cCLOSING;
}

void wkf::AcesDisplay::Overlay::OpenRightSettings()
{
   mRightSettings.animationTimer->start();
   mRightSettings.visible                         = true;
   mSettingsButtons[DrawRegion::cRIGHT].clickable = false;
   mRightSettings.menuState                       = MenuState::cOPENING;
}

void wkf::AcesDisplay::Overlay::CloseRightSettings()
{
   mRightSettings.animationTimer->start();
   mRightSettings.menuState = MenuState::cCLOSING;
}

void wkf::AcesDisplay::Overlay::OpenTopSettings()
{
   mTopSettings.animationTimer->start();
   mTopSettings.visible                         = true;
   mSettingsButtons[DrawRegion::cTOP].clickable = false;
   mTopSettings.menuState                       = MenuState::cOPENING;
}

void wkf::AcesDisplay::Overlay::CloseTopSettings()
{
   mTopSettings.animationTimer->start();
   mTopSettings.menuState = MenuState::cCLOSING;
}

void wkf::AcesDisplay::Overlay::DrawTime(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));

   double fontSize = 0.016f * pH();
   vectorXform->addChild(AddTextItem("TIME:", 0.0, 0.0, fontSize, Color::Cyan, osgText::Text::LEFT_CENTER).get());
   // Time value label
   UtCalendar c = UtCalendar();
   c.AdvanceTimeBy(12 * 3600);
   c.AdvanceTimeBy(mData.platformData.simTime);
   QTime       time(c.GetHour(), c.GetMinute(), c.GetSecond());
   std::string timestr = time.toString(Qt::ISODate).toStdString();
   vectorXform->addChild(
      AddTextItem(timestr, fontSize * 3, 0.0, fontSize, Color::LightGray, osgText::Text::LEFT_CENTER).get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawDisplayLabel(osg::ref_ptr<osg::Group> aVectorGroup,
                                                 const DrawRegion         aDrawRegion,
                                                 const DisplayType        aDisplayType)
{
   QPointF labelPos = GetLabelPos(aDrawRegion);
   auto    strings  = GetLabelString(aDrawRegion, aDisplayType);

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(labelPos.x() * pW(), labelPos.y() * pH(), 0.0));

   // Align the text based on where the label is being drawn
   auto textAlign = osgText::Text::CENTER_CENTER;
   switch (aDrawRegion)
   {
   case DrawRegion::cLEFT:
      textAlign = osgText::Text::LEFT_CENTER;
      break;
   case DrawRegion::cRIGHT:
      textAlign = osgText::Text::RIGHT_CENTER;
      break;
   default:
      break;
   }

   // Loop through and draw each string, each lower than the next
   double yPos     = 0.0;
   double fontSize = 0.016f * pH();
   for (auto& string : strings)
   {
      vectorXform->addChild(AddTextItem(string.c_str(), 0.0, yPos, fontSize, Color::LightGray, textAlign).get());
      yPos -= fontSize;
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::InitializePFD()
{
   // This creates the drawables and the transform for the PFD.
   mPFD_PosMatrixNode  = new osg::MatrixTransform;
   mPFD_HorzMatrixNode = new osg::MatrixTransform;
   mPFD_PosMatrixNode->addChild(mPFD_HorzMatrixNode.get());

   osg::ref_ptr<osg::Geode> horzGeode  = new osg::Geode;
   osg::ref_ptr<osg::Geode> horzGeode2 = new osg::Geode;
   mPFD_HorzMatrixNode->addChild(horzGeode);
   mPFD_HorzMatrixNode->addChild(horzGeode2);
   osg::ref_ptr<osg::Geometry> geom1 = new osg::Geometry;
   osg::Vec3Array*             v1    = new osg::Vec3Array;
   osg::Vec4Array*             c1    = new osg::Vec4Array;
   osg::Vec4                   brown(0.294f, 0.192f, 0.196f, 1.0f);
   osg::Vec4                   dkbrown(brown.x() * 0.5f, brown.y() * 0.5f, brown.z() * 0.5f, 1.0f);
   osg::Vec4                   blue(0.133f, 0.353f, 0.553f, 1.0f);
   osg::Vec4                   white(1.0f, 1.0f, 1.0f, 1.0f);
   v1->push_back(osg::Vec3(-500, -1000, -1));
   v1->push_back(osg::Vec3(500, -1000, -1));
   v1->push_back(osg::Vec3(500, 0, -1));
   v1->push_back(osg::Vec3(-500, 0, -1));
   v1->push_back(osg::Vec3(-500, 0, -1));
   v1->push_back(osg::Vec3(500, 0, -1));
   v1->push_back(osg::Vec3(500, 1000, -1));
   v1->push_back(osg::Vec3(-500, 1000, -1));
   c1->push_back(dkbrown);
   c1->push_back(dkbrown);
   c1->push_back(brown);
   c1->push_back(brown);
   c1->push_back(blue);
   c1->push_back(blue);
   c1->push_back(blue);
   c1->push_back(blue);

   v1->push_back(osg::Vec3(-80, 0, 0));
   c1->push_back(white);
   v1->push_back(osg::Vec3(-20, 0, 0));
   c1->push_back(white);
   v1->push_back(osg::Vec3(20, 0, 0));
   c1->push_back(white);
   v1->push_back(osg::Vec3(80, 0, 0));
   c1->push_back(white);
   for (int i = 10; i < 90; i += 10)
   {
      int y = i * 6;
      v1->push_back(osg::Vec3(-60, y - 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-20, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, y - 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(20, y, 0));
      c1->push_back(white);

      v1->push_back(osg::Vec3(-60, -y + 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(-20, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, -y + 6, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(60, -y, 0));
      c1->push_back(white);
      v1->push_back(osg::Vec3(20, -y, 0));
      c1->push_back(white);
      horzGeode2->addChild(
         AddTextItem(QString::number(i).toStdString(), -55, y - 5, 12, Color::White, osgText::Text::LEFT_TOP).get());
      horzGeode2->addChild(
         AddTextItem(QString::number(-i).toStdString(), -55, -y + 5, 12, Color::White, osgText::Text::LEFT_BOTTOM).get());
   }

   geom1->setVertexArray(v1);
   geom1->setColorArray(c1);
   geom1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
   geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 8));
   geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 8, 132));
   horzGeode->addDrawable(geom1);

   osg::ref_ptr<osg::Geode>    centerNode = new osg::Geode;
   osg::ref_ptr<osg::Geometry> geom2      = new osg::Geometry;
   centerNode->addDrawable(geom2);
   mPFD_PosMatrixNode->addChild(centerNode.get());
   osg::Vec3Array* v2 = new osg::Vec3Array;
   v2->push_back(osg::Vec3(-15, 0, 0));
   v2->push_back(osg::Vec3(-10, 0, 0));
   v2->push_back(osg::Vec3(-5, -7, 0));
   v2->push_back(osg::Vec3(0, 0, 0));
   v2->push_back(osg::Vec3(5, -7, 0));
   v2->push_back(osg::Vec3(10, 0, 0));
   v2->push_back(osg::Vec3(15, 0, 0));
   osg::Vec4Array* c2 = new osg::Vec4Array;
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   c2->push_back(white);
   geom2->setVertexArray(v2);
   geom2->setColorArray(c2);
   geom2->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
   geom2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 7));
   // mOverlayGeode->addChild(mPFD_PosMatrixNode.get());
}

void wkf::AcesDisplay::Overlay::InitializeLeftSettings()
{
   mLeftSettings.transform = new osg::MatrixTransform;
   mLeftSettings.buttons.clear();

   double bW = 0.3 * pW();
   double bH = 0.75 * pH();

   // Test box
   osg::ref_ptr<osg::Vec2Array> testBoxArray(new osg::Vec2Array);
   testBoxArray->push_back(osg::Vec2d(bW * 0.5, bH * 0.5));   // Top right
   testBoxArray->push_back(osg::Vec2d(bW * 0.5, -bH * 0.5));  // Bottom right
   testBoxArray->push_back(osg::Vec2d(-bW * 0.5, -bH * 0.5)); // Bottom left
   testBoxArray->push_back(osg::Vec2d(-bW * 0.5, bH * 0.5));  // Top left
   auto bgBox = CreateGeometry(testBoxArray, GL_POLYGON, Color::SettingsGrayTransparent);
   mLeftSettings.transform->addChild(bgBox.get());
   mLeftSettings.transform->addChild(CreateGeometry(testBoxArray, GL_LINE_LOOP, Color::LightGray).get());

   // Initialize the correct settings in the correct spot,
   // taking into account the other settings position
   if (mDisplays[DisplayType::cTSD] == DrawRegion::cLEFT)
   {
      InitializeTSDSettings(bgBox, true);
   }
   else if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT)
   {
      InitializeSADSettings(bgBox, true);
   }
   else
   {
      if (mDisplays[DisplayType::cTSD] == DrawRegion::cRIGHT)
      {
         InitializeSADSettings(bgBox, true);
      }
      else
      {
         InitializeTSDSettings(bgBox, true);
      }
   }
}

void wkf::AcesDisplay::Overlay::InitializeRightSettings()
{
   mRightSettings.transform = new osg::MatrixTransform;
   mRightSettings.buttons.clear();

   double bW = 0.3 * pW();
   double bH = 0.75 * pH();

   // Test box
   osg::ref_ptr<osg::Vec2Array> testBoxArray(new osg::Vec2Array);
   testBoxArray->push_back(osg::Vec2d(bW * 0.5, bH * 0.5));   // Top right
   testBoxArray->push_back(osg::Vec2d(bW * 0.5, -bH * 0.5));  // Bottom right
   testBoxArray->push_back(osg::Vec2d(-bW * 0.5, -bH * 0.5)); // Bottom Right
   testBoxArray->push_back(osg::Vec2d(-bW * 0.5, bH * 0.5));  // Top Right
   auto bgBox = CreateGeometry(testBoxArray, GL_POLYGON, Color::SettingsGrayTransparent);
   mRightSettings.transform->addChild(bgBox.get());
   mRightSettings.transform->addChild(CreateGeometry(testBoxArray, GL_LINE_LOOP, Color::LightGray).get());

   // Initialize the correct settings in the correct spot,
   // taking into account the other settings position
   if (mDisplays[DisplayType::cSAD] == DrawRegion::cRIGHT)
   {
      InitializeSADSettings(bgBox, false);
   }
   else if (mDisplays[DisplayType::cTSD] == DrawRegion::cRIGHT)
   {
      InitializeTSDSettings(bgBox, false);
   }
   else
   {
      if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT)
      {
         InitializeTSDSettings(bgBox, false);
      }
      else
      {
         InitializeSADSettings(bgBox, false);
      }
   }
}

void wkf::AcesDisplay::Overlay::InitializeTopSettings()
{
   mTopSettings.transform = new osg::MatrixTransform;
   mTopSettings.buttons.clear();

   double bW = 0.3 * pW();
   double bH = 0.25 * pH();

   // Test box
   osg::ref_ptr<osg::Vec2Array> testBoxArray(new osg::Vec2Array);
   testBoxArray->push_back(osg::Vec2d(bW * 0.5, bH * 0.5));   // Top Top
   testBoxArray->push_back(osg::Vec2d(bW * 0.5, -bH * 0.5));  // Bottom Top
   testBoxArray->push_back(osg::Vec2d(-bW * 0.5, -bH * 0.5)); // Bottom Top
   testBoxArray->push_back(osg::Vec2d(-bW * 0.5, bH * 0.5));  // Top Top
   auto bgBox = CreateGeometry(testBoxArray, GL_POLYGON, Color::SettingsGrayTransparent);
   mTopSettings.transform->addChild(bgBox.get());
   mTopSettings.transform->addChild(CreateGeometry(testBoxArray, GL_LINE_LOOP, Color::LightGray).get());

   // Top settings will always be for the engagement region
   InitializeEngagementSettings(bgBox);
}

void wkf::AcesDisplay::Overlay::ToggleSetting(DisplayType aDisplay, size_t aOption)
{
   mSettings[aDisplay] ^= aOption;
}

void wkf::AcesDisplay::Overlay::ToggleDisplaySync()
{
   mDisplaySync = !mDisplaySync;
   if (mDisplaySync)
   {
      // Use the left display as the reference range for display sync
      if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT)
      {
         mTSDRange_nm     = mSADRange_nm;
         mTSD_CenterPoint = mSAD_CenterPoint;

         // Make sure both displays are using the same offset
         if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER))
         {
            SetCenterPosition(DisplayType::cTSD);
         }
         else if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE))
         {
            SetOffsetPosition(DisplayType::cTSD);
         }
         else if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM))
         {
            SetBottomPosition(DisplayType::cTSD);
         }
      }
      else
      {
         mSADRange_nm     = mTSDRange_nm;
         mSAD_CenterPoint = mTSD_CenterPoint;
         // Make sure both displays are using the same offset
         if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER))
         {
            SetCenterPosition(DisplayType::cSAD);
         }
         else if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE))
         {
            SetOffsetPosition(DisplayType::cSAD);
         }
         else if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM))
         {
            SetBottomPosition(DisplayType::cSAD);
         }
      }
      // Re-initialize the settings to make the offset
      // radio buttons match
      InitializeLeftSettings();
      InitializeRightSettings();
   }
}

void wkf::AcesDisplay::Overlay::SetRadioSetting(DisplayType aDisplay, size_t aOption)
{
   mSettings[aDisplay] |= aOption;

   // Radio settings will have functions that run when they are chosen
   switch (aDisplay)
   {
   case DisplayType::cTSD:
   {
      if (aOption == static_cast<size_t>(TSD_SETTINGS::cHOVER))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(TSD_SETTINGS::cCLICK));
      }
      else if (aOption == static_cast<size_t>(TSD_SETTINGS::cCLICK))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(TSD_SETTINGS::cHOVER));
      }
      else if (aOption == static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER))
      {
         SetCenterPosition(DisplayType::cTSD);
      }
      else if (aOption == static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE))
      {
         SetOffsetPosition(DisplayType::cTSD);
      }
      else if (aOption == static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM))
      {
         SetBottomPosition(DisplayType::cTSD);
      }
      break;
   }
   case DisplayType::cSAD:
   {
      if (aOption == static_cast<size_t>(SAD_SETTINGS::cHOVER))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(SAD_SETTINGS::cCLICK));
      }
      else if (aOption == static_cast<size_t>(SAD_SETTINGS::cCLICK))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(SAD_SETTINGS::cHOVER));
      }
      else if (aOption == static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER))
      {
         SetCenterPosition(DisplayType::cSAD);
      }
      else if (aOption == static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE))
      {
         SetOffsetPosition(DisplayType::cSAD);
      }
      else if (aOption == static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM))
      {
         SetBottomPosition(DisplayType::cSAD);
      }
      break;
   }
   case DisplayType::cENGAGE:
   {
      if (aOption == static_cast<size_t>(ENGAGEMENT_SETTINGS::cNM))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cKM));
      }
      else if (aOption == static_cast<size_t>(ENGAGEMENT_SETTINGS::cKM))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cNM));
      }
      else if (aOption == static_cast<size_t>(ENGAGEMENT_SETTINGS::cMANUAL))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS));
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS));
         if (mTargetPlatform == 0 && mTargetPlatforms.size() > 0)
         {
            wkf::Scenario* scenario = vaEnv.GetStandardScenario();
            if (scenario != nullptr)
            {
               mTargetPlatform = mTargetPlatforms.front();
            }
         }
      }
      else if (aOption == static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cMANUAL));
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS));
      }
      else if (aOption == static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS))
      {
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cMANUAL));
         mSettings[aDisplay] &= ~(static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS));
      }
      break;
   }
   case DisplayType::cPFD:
   case DisplayType::cTWD:
   default:
      break;
   }
}

void wkf::AcesDisplay::Overlay::DrawLeftSettings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));
   vectorXform->addChild(mLeftSettings.transform.get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawRightSettings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));
   vectorXform->addChild(mRightSettings.transform.get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawTopSettings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX, aY, 0.0));
   vectorXform->addChild(mTopSettings.transform.get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::InitializeSADSettings(osg::ref_ptr<osg::Group> aVectorGroup, bool aLeftSettings)
{
   auto& buttons = aLeftSettings ? mLeftSettings.buttons : mRightSettings.buttons;

   double fontSize = 0.016f * pH();

   double bW     = 0.3 * pW();
   double bH     = 0.75 * pH();
   double startX = -0.4 * bW;
   double startY = 0.45 * bH;

   // checkbox size
   double cbS = bH / 40.0;

   // checkbox padding
   double cP = cbS / 5.0;

   // radio button radius
   double rbR = cbS / 1.8;

   // Title
   aVectorGroup->addChild(
      AddTextItem("SA Display Settings", startX - (cbS * 0.5), startY, fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   // Ownship Position
   aVectorGroup->addChild(
      AddTextItem("Ownship Position", startX - (cbS * 0.5), startY - (cbS * 3), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   osg::ref_ptr<osg::Vec2Array> radioButtonArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = rbR * cos(ang_rad);
      double y       = rbR * sin(ang_rad);
      radioButtonArray->push_back(osg::Vec2d(x, y));
   }
   osg::ref_ptr<osg::Vec2Array> radioButtonSelectedArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = rbR * 0.8 * cos(ang_rad);
      double y       = rbR * 0.8 * sin(ang_rad);
      radioButtonSelectedArray->push_back(osg::Vec2d(x, y));
   }

   // Ownship Position (Center)
   osg::ref_ptr<osg::PositionAttitudeTransform> ownshipCenterXform(new osg::PositionAttitudeTransform());
   ownshipCenterXform->setPosition(osg::Vec3d(startX + cbS, startY - (cbS * 4) - (cP * 1), 0.0));
   ownshipCenterXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER))
   {
      ownshipCenterXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Center", startX + (cbS * 2), startY - (cbS * 4) - (cP * 1), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(ownshipCenterXform.get());
   Button ownshipCenter;
   ownshipCenter.region = QRectF(startX + cbS, startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   ownshipCenter.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER));
   buttons.push_back(ownshipCenter);

   // Ownship Position (Offset)
   osg::ref_ptr<osg::PositionAttitudeTransform> ownshipOffsetXform(new osg::PositionAttitudeTransform());
   ownshipOffsetXform->setPosition(osg::Vec3d(startX + cbS + (0.06 * pW()), startY - (cbS * 4) - (cP * 1), 0.0));
   ownshipOffsetXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE))
   {
      ownshipOffsetXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Offset",
                                      startX + (cbS * 2) + (0.06 * pW()),
                                      startY - (cbS * 4) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(ownshipOffsetXform.get());
   Button ownshipOffset;
   ownshipOffset.region = QRectF(startX + cbS + (0.06 * pW()), startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   ownshipOffset.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE));
   buttons.push_back(ownshipOffset);

   // Ownship Position (Bottom)
   osg::ref_ptr<osg::PositionAttitudeTransform> ownshipBottomXform(new osg::PositionAttitudeTransform());
   ownshipBottomXform->setPosition(osg::Vec3d(startX + cbS + (0.12 * pW()), startY - (cbS * 4) - (cP * 1), 0.0));
   ownshipBottomXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM))
   {
      ownshipBottomXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Bottom",
                                      startX + (cbS * 2) + (0.12 * pW()),
                                      startY - (cbS * 4) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(ownshipBottomXform.get());
   Button ownshipBottom;
   ownshipBottom.region = QRectF(startX + cbS + (0.12 * pW()), startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   ownshipBottom.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM));
   buttons.push_back(ownshipBottom);

   // Checkbox arrays to be re-used
   osg::ref_ptr<osg::Vec2Array> cbArray(new osg::Vec2Array);
   cbArray->push_back(osg::Vec2d(cbS * 0.5, cbS * 0.5));   // Top right
   cbArray->push_back(osg::Vec2d(cbS * 0.5, -cbS * 0.5));  // Bottom right
   cbArray->push_back(osg::Vec2d(-cbS * 0.5, -cbS * 0.5)); // Bottom left
   cbArray->push_back(osg::Vec2d(-cbS * 0.5, cbS * 0.5));  // Top left
   osg::ref_ptr<osg::Vec2Array> checkedArray(new osg::Vec2Array);
   checkedArray->push_back(osg::Vec2d(cbS * 0.5, cbS * 0.5));   // Top right
   checkedArray->push_back(osg::Vec2d(-cbS * 0.5, -cbS * 0.5)); // Bottom left
   checkedArray->push_back(osg::Vec2d(cbS * 0.5, -cbS * 0.5));  // Bottom right
   checkedArray->push_back(osg::Vec2d(-cbS * 0.5, cbS * 0.5));  // Top left

   // Show Perception
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(startX, startY - (cbS * 7), 0.0));
   vectorXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPERCEPTION))
   {
      vectorXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Perception", startX + (cbS * 1), startY - (cbS * 7), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(vectorXform.get());
   Button showPerception;
   showPerception.region = QRectF(startX, startY - (cbS * 7), cbS, cbS);
   showPerception.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cPERCEPTION));
   buttons.push_back(showPerception);

   // Show Truth (Air)
   osg::ref_ptr<osg::PositionAttitudeTransform> showTruthAirXform(new osg::PositionAttitudeTransform());
   showTruthAirXform->setPosition(osg::Vec3d(startX, startY - (cbS * 8) - (cP * 1), 0.0));
   showTruthAirXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cTRUTHAIR))
   {
      showTruthAirXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Truth (Air)",
                                      startX + (cbS * 1),
                                      startY - (cbS * 8) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showTruthAirXform.get());
   Button showTruthAir;
   showTruthAir.region = QRectF(startX, startY - (cbS * 8) - (cP * 1), cbS, cbS);
   showTruthAir.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cTRUTHAIR));
   buttons.push_back(showTruthAir);

   // Show Truth (No-Air, Other)
   osg::ref_ptr<osg::PositionAttitudeTransform> showTruthNoAirXform(new osg::PositionAttitudeTransform());
   showTruthNoAirXform->setPosition(osg::Vec3d(startX, startY - (cbS * 9) - (cP * 2), 0.0));
   showTruthNoAirXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cTRUTHGROUND))
   {
      showTruthNoAirXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Truth (No-Air, Other)",
                                      startX + (cbS * 1),
                                      startY - (cbS * 9) - (cP * 2),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showTruthNoAirXform.get());
   Button showTruthNoAir;
   showTruthNoAir.region = QRectF(startX, startY - (cbS * 9) - (cP * 2), cbS, cbS);
   showTruthNoAir.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cTRUTHGROUND));
   buttons.push_back(showTruthNoAir);

   // Show Friendlies
   osg::ref_ptr<osg::PositionAttitudeTransform> showFriendliesXform(new osg::PositionAttitudeTransform());
   showFriendliesXform->setPosition(osg::Vec3d(startX, startY - (cbS * 10) - (cP * 3), 0.0));
   showFriendliesXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cFRIENDLIES))
   {
      showFriendliesXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Friendlies",
                                      startX + (cbS * 1),
                                      startY - (cbS * 10) - (cP * 3),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showFriendliesXform.get());
   Button showFriendlies;
   showFriendlies.region = QRectF(startX, startY - (cbS * 10) - (cP * 3), cbS, cbS);
   showFriendlies.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cFRIENDLIES));
   buttons.push_back(showFriendlies);

   // Show Flight Members
   osg::ref_ptr<osg::PositionAttitudeTransform> showFlightXform(new osg::PositionAttitudeTransform());
   showFlightXform->setPosition(osg::Vec3d(startX, startY - (cbS * 11) - (cP * 4), 0.0));
   showFlightXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cFLIGHT))
   {
      showFlightXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Flight Members",
                                      startX + (cbS * 1),
                                      startY - (cbS * 11) - (cP * 4),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showFlightXform.get());
   Button showFlight;
   showFlight.region = QRectF(startX, startY - (cbS * 11) - (cP * 4), cbS, cbS);
   showFlight.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cFLIGHT));
   buttons.push_back(showFlight);

   // Show Priority Threats
   osg::ref_ptr<osg::PositionAttitudeTransform> showPriorityThreatsXform(new osg::PositionAttitudeTransform());
   showPriorityThreatsXform->setPosition(osg::Vec3d(startX, startY - (cbS * 13) - (cP * 6), 0.0));
   showPriorityThreatsXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPRIORITYTHREATS))
   {
      showPriorityThreatsXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Priority Threats",
                                      startX + (cbS * 1),
                                      startY - (cbS * 13) - (cP * 6),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showPriorityThreatsXform.get());
   Button showPriorityThreats;
   showPriorityThreats.region = QRectF(startX, startY - (cbS * 13) - (cP * 6), cbS, cbS);
   showPriorityThreats.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cPRIORITYTHREATS));
   buttons.push_back(showPriorityThreats);

   // Show Priority Targets
   osg::ref_ptr<osg::PositionAttitudeTransform> showPriorityTargetsXform(new osg::PositionAttitudeTransform());
   showPriorityTargetsXform->setPosition(osg::Vec3d(startX, startY - (cbS * 14) - (cP * 7), 0.0));
   showPriorityTargetsXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPRIORITYTARGETS))
   {
      showPriorityTargetsXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Priority Targets",
                                      startX + (cbS * 1),
                                      startY - (cbS * 14) - (cP * 7),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showPriorityTargetsXform.get());
   Button showPriorityTargets;
   showPriorityTargets.region = QRectF(startX, startY - (cbS * 14) - (cP * 7), cbS, cbS);
   showPriorityTargets.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cPRIORITYTARGETS));
   buttons.push_back(showPriorityTargets);

   // Show Altitude
   osg::ref_ptr<osg::PositionAttitudeTransform> showAltitudeXform(new osg::PositionAttitudeTransform());
   showAltitudeXform->setPosition(osg::Vec3d(startX, startY - (cbS * 16) - (cP * 9), 0.0));
   showAltitudeXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cALTITUDE))
   {
      showAltitudeXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Altitude", startX + (cbS * 1), startY - (cbS * 16) - (cP * 9), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showAltitudeXform.get());
   Button showAltitude;
   showAltitude.region = QRectF(startX, startY - (cbS * 16) - (cP * 9), cbS, cbS);
   showAltitude.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cALTITUDE));
   buttons.push_back(showAltitude);

   // Show Importance
   osg::ref_ptr<osg::PositionAttitudeTransform> showImportanceXform(new osg::PositionAttitudeTransform());
   showImportanceXform->setPosition(osg::Vec3d(startX, startY - (cbS * 17) - (cP * 10), 0.0));
   showImportanceXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cIMPORTANCE))
   {
      showImportanceXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Importance",
                                      startX + (cbS * 1),
                                      startY - (cbS * 17) - (cP * 10),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showImportanceXform.get());
   Button showImportance;
   showImportance.region = QRectF(startX, startY - (cbS * 17) - (cP * 10), cbS, cbS);
   showImportance.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cIMPORTANCE));
   buttons.push_back(showImportance);

   // Show Visibility
   osg::ref_ptr<osg::PositionAttitudeTransform> showVisibilityXform(new osg::PositionAttitudeTransform());
   showVisibilityXform->setPosition(osg::Vec3d(startX, startY - (cbS * 18) - (cP * 11), 0.0));
   showVisibilityXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cVISIBILITY))
   {
      showVisibilityXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Visibility",
                                      startX + (cbS * 1),
                                      startY - (cbS * 18) - (cP * 11),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showVisibilityXform.get());
   Button showVisibility;
   showVisibility.region = QRectF(startX, startY - (cbS * 18) - (cP * 11), cbS, cbS);
   showVisibility.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cVISIBILITY));
   buttons.push_back(showVisibility);

   // Show ESM/RWR Tracks
   osg::ref_ptr<osg::PositionAttitudeTransform> showESM_RWR_TracksXform(new osg::PositionAttitudeTransform());
   showESM_RWR_TracksXform->setPosition(osg::Vec3d(startX, startY - (cbS * 20) - (cP * 13), 0.0));
   showESM_RWR_TracksXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cESMRWR))
   {
      showESM_RWR_TracksXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show ESM/RWR Tracks",
                                      startX + (cbS * 1),
                                      startY - (cbS * 20) - (cP * 13),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showESM_RWR_TracksXform.get());
   Button showESM_RWR_Tracks;
   showESM_RWR_Tracks.region = QRectF(startX, startY - (cbS * 20) - (cP * 13), cbS, cbS);
   showESM_RWR_Tracks.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cESMRWR));
   buttons.push_back(showESM_RWR_Tracks);

   // Show Angle Only Tracks
   osg::ref_ptr<osg::PositionAttitudeTransform> showAngleOnlyTracksXform(new osg::PositionAttitudeTransform());
   showAngleOnlyTracksXform->setPosition(osg::Vec3d(startX, startY - (cbS * 21) - (cP * 14), 0.0));
   showAngleOnlyTracksXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cANGLEONLY))
   {
      showAngleOnlyTracksXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Show Angle-Only Tracks",
                                      startX + (cbS * 1),
                                      startY - (cbS * 21) - (cP * 14),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(showAngleOnlyTracksXform.get());
   Button showAngleOnlyTracks;
   showAngleOnlyTracks.region = QRectF(startX, startY - (cbS * 21) - (cP * 14), cbS, cbS);
   showAngleOnlyTracks.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cANGLEONLY));
   buttons.push_back(showAngleOnlyTracks);

   // Show Routes
   osg::ref_ptr<osg::PositionAttitudeTransform> showRoutesXform(new osg::PositionAttitudeTransform());
   showRoutesXform->setPosition(osg::Vec3d(startX, startY - (cbS * 23) - (cP * 16), 0.0));
   showRoutesXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cROUTE))
   {
      showRoutesXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Route", startX + (cbS * 1), startY - (cbS * 23) - (cP * 16), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showRoutesXform.get());
   Button showRoutes;
   showRoutes.region = QRectF(startX, startY - (cbS * 23) - (cP * 16), cbS, cbS);
   showRoutes.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cROUTE));
   buttons.push_back(showRoutes);

   // Show FOV
   osg::ref_ptr<osg::PositionAttitudeTransform> showFOVXform(new osg::PositionAttitudeTransform());
   showFOVXform->setPosition(osg::Vec3d(startX, startY - (cbS * 24) - (cP * 17), 0.0));
   showFOVXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cFOV))
   {
      showFOVXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show FOV", startX + (cbS * 1), startY - (cbS * 24) - (cP * 17), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showFOVXform.get());
   Button showFOV;
   showFOV.region = QRectF(startX, startY - (cbS * 24) - (cP * 17), cbS, cbS);
   showFOV.function = std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cFOV));
   buttons.push_back(showFOV);

   // Show Groups
   osg::ref_ptr<osg::PositionAttitudeTransform> showGroupsXform(new osg::PositionAttitudeTransform());
   showGroupsXform->setPosition(osg::Vec3d(startX, startY - (cbS * 25) - (cP * 18), 0.0));
   showGroupsXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cGROUPS))
   {
      showGroupsXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Groups", startX + (cbS * 1), startY - (cbS * 25) - (cP * 18), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showGroupsXform.get());
   Button showGroups;
   showGroups.region = QRectF(startX, startY - (cbS * 25) - (cP * 18), cbS, cbS);
   showGroups.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cGROUPS));
   buttons.push_back(showGroups);

   // Show Entity Select Mode
   aVectorGroup->addChild(AddTextItem("Show Entity Select Mode",
                                      startX - (cbS * 0.5),
                                      startY - (cbS * 27) - (cP * 20),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());

   // Entity Select Mode (Hover)
   osg::ref_ptr<osg::PositionAttitudeTransform> entitySelectHoverXform(new osg::PositionAttitudeTransform());
   entitySelectHoverXform->setPosition(osg::Vec3d(startX + cbS, startY - (cbS * 28) - (cP * 21), 0.0));
   entitySelectHoverXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cHOVER))
   {
      entitySelectHoverXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Hover", startX + (cbS * 2), startY - (cbS * 28) - (cP * 21), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(entitySelectHoverXform.get());
   Button entitySelectHover;
   entitySelectHover.region = QRectF(startX + cbS, startY - (cbS * 28) - (cP * 21), rbR * 2, rbR * 2);
   entitySelectHover.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cHOVER));
   buttons.push_back(entitySelectHover);

   // Entity Select Mode (Click)
   osg::ref_ptr<osg::PositionAttitudeTransform> entitySelectClickXform(new osg::PositionAttitudeTransform());
   entitySelectClickXform->setPosition(osg::Vec3d(startX + cbS + (0.06 * pW()), startY - (cbS * 28) - (cP * 21), 0.0));
   entitySelectClickXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cCLICK))
   {
      entitySelectClickXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Click",
                                      startX + (cbS * 2) + (0.06 * pW()),
                                      startY - (cbS * 28) - (cP * 21),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(entitySelectClickXform.get());
   Button entitySelectClick;
   entitySelectClick.region = QRectF(startX + cbS + (0.06 * pW()), startY - (cbS * 28) - (cP * 21), rbR * 2, rbR * 2);
   entitySelectClick.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cCLICK));
   buttons.push_back(entitySelectClick);

   double buttonW = bW / 6.0;
   double buttonH = buttonW / 2.5;

   // Button box array
   osg::ref_ptr<osg::Vec2Array> buttonArray(new osg::Vec2Array);
   buttonArray->push_back(osg::Vec2d(buttonW * 0.5, buttonH * 0.5));   // Top right
   buttonArray->push_back(osg::Vec2d(buttonW * 0.5, -buttonH * 0.5));  // Bottom right
   buttonArray->push_back(osg::Vec2d(-buttonW * 0.5, -buttonH * 0.5)); // Bottom left
   buttonArray->push_back(osg::Vec2d(-buttonW * 0.5, buttonH * 0.5));  // Top left

   // Apply Button - Leaving this here in case we end up re-enabling it
   // osg::ref_ptr<osg::PositionAttitudeTransform> applyButtonXform(new osg::PositionAttitudeTransform());
   // applyButtonXform->setPosition(osg::Vec3d((0.5 * bW) - (buttonW * 1.5) - (buttonW * 0.2), (-0.5 * bH) + buttonH,
   // 0.0)); applyButtonXform->addChild(CreateGeometry(buttonArray, GL_LINE_LOOP, Color::White).get());
   // applyButtonXform->addChild(AddTextItem("APPLY", 0.0, 0.0, fontSize, Color::White,
   // osgText::Text::CENTER_CENTER).get()); aVectorGroup->addChild(applyButtonXform.get()); Button applyButton;
   // applyButton.region = QRectF((0.5 * bW) - (buttonW * 1.5) - (buttonW * 0.2), (-0.5 * bH) + buttonH, buttonW,
   // buttonH); applyButton.function = std::bind(aLeftSettings ? &Overlay::CloseLeftSettings :
   // &Overlay::CloseRightSettings, this); buttons.push_back(applyButton);

   // Close Button
   osg::ref_ptr<osg::PositionAttitudeTransform> closeButtonXform(new osg::PositionAttitudeTransform());
   closeButtonXform->setPosition(osg::Vec3d((0.5 * bW) - (buttonW * 0.5) - (buttonW * 0.1), (-0.5 * bH) + buttonH, 0.0));
   closeButtonXform->addChild(CreateGeometry(buttonArray, GL_LINE_LOOP, Color::White).get());
   closeButtonXform->addChild(AddTextItem("Close", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   aVectorGroup->addChild(closeButtonXform.get());
   Button closeButton;
   closeButton.region = QRectF((0.5 * bW) - (buttonW * 0.5) - (buttonW * 0.1), (-0.5 * bH) + buttonH, buttonW, buttonH);
   closeButton.function = std::bind(aLeftSettings ? &Overlay::CloseLeftSettings : &Overlay::CloseRightSettings, this);
   buttons.push_back(closeButton);
}

void wkf::AcesDisplay::Overlay::InitializeTSDSettings(osg::ref_ptr<osg::Group> aVectorGroup, bool aLeftSettings)
{
   auto& buttons = aLeftSettings ? mLeftSettings.buttons : mRightSettings.buttons;

   double fontSize = 0.016f * pH();

   double bW     = 0.3 * pW();
   double bH     = 0.75 * pH();
   double startX = -0.4 * bW;
   double startY = 0.45 * bH;

   // checkbox size
   double cbS = bH / 40.0;

   // checkbox padding
   double cP = cbS / 5.0;

   // radio button radius
   double rbR = cbS / 1.8;

   // Title
   aVectorGroup->addChild(
      AddTextItem("Tactical Situation Display Settings", startX - (cbS * 0.5), startY, fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   // Ownship Position
   aVectorGroup->addChild(
      AddTextItem("Ownship Position", startX - (cbS * 0.5), startY - (cbS * 3), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   osg::ref_ptr<osg::Vec2Array> radioButtonArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = rbR * cos(ang_rad);
      double y       = rbR * sin(ang_rad);
      radioButtonArray->push_back(osg::Vec2d(x, y));
   }
   osg::ref_ptr<osg::Vec2Array> radioButtonSelectedArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = rbR * 0.8 * cos(ang_rad);
      double y       = rbR * 0.8 * sin(ang_rad);
      radioButtonSelectedArray->push_back(osg::Vec2d(x, y));
   }

   // Ownship Position (Center)
   osg::ref_ptr<osg::PositionAttitudeTransform> ownshipCenterXform(new osg::PositionAttitudeTransform());
   ownshipCenterXform->setPosition(osg::Vec3d(startX + cbS, startY - (cbS * 4) - (cP * 1), 0.0));
   ownshipCenterXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER))
   {
      ownshipCenterXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Center", startX + (cbS * 2), startY - (cbS * 4) - (cP * 1), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(ownshipCenterXform.get());
   Button ownshipCenter;
   ownshipCenter.region = QRectF(startX + cbS, startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   ownshipCenter.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER));
   buttons.push_back(ownshipCenter);

   // Ownship Position (Offset)
   osg::ref_ptr<osg::PositionAttitudeTransform> ownshipOffsetXform(new osg::PositionAttitudeTransform());
   ownshipOffsetXform->setPosition(osg::Vec3d(startX + cbS + (0.06 * pW()), startY - (cbS * 4) - (cP * 1), 0.0));
   ownshipOffsetXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE))
   {
      ownshipOffsetXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Offset",
                                      startX + (cbS * 2) + (0.06 * pW()),
                                      startY - (cbS * 4) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(ownshipOffsetXform.get());
   Button ownshipOffset;
   ownshipOffset.region = QRectF(startX + cbS + (0.06 * pW()), startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   ownshipOffset.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE));
   buttons.push_back(ownshipOffset);

   // Ownship Position (Bottom)
   osg::ref_ptr<osg::PositionAttitudeTransform> ownshipBottomXform(new osg::PositionAttitudeTransform());
   ownshipBottomXform->setPosition(osg::Vec3d(startX + cbS + (0.12 * pW()), startY - (cbS * 4) - (cP * 1), 0.0));
   ownshipBottomXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM))
   {
      ownshipBottomXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Bottom",
                                      startX + (cbS * 2) + (0.12 * pW()),
                                      startY - (cbS * 4) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(ownshipBottomXform.get());
   Button ownshipBottom;
   ownshipBottom.region = QRectF(startX + cbS + (0.12 * pW()), startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   ownshipBottom.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM));
   buttons.push_back(ownshipBottom);

   // Checkbox arrays to be re-used
   osg::ref_ptr<osg::Vec2Array> cbArray(new osg::Vec2Array);
   cbArray->push_back(osg::Vec2d(cbS * 0.5, cbS * 0.5));   // Top right
   cbArray->push_back(osg::Vec2d(cbS * 0.5, -cbS * 0.5));  // Bottom right
   cbArray->push_back(osg::Vec2d(-cbS * 0.5, -cbS * 0.5)); // Bottom left
   cbArray->push_back(osg::Vec2d(-cbS * 0.5, cbS * 0.5));  // Top left
   osg::ref_ptr<osg::Vec2Array> checkedArray(new osg::Vec2Array);
   checkedArray->push_back(osg::Vec2d(cbS * 0.5, cbS * 0.5));   // Top right
   checkedArray->push_back(osg::Vec2d(-cbS * 0.5, -cbS * 0.5)); // Bottom left
   checkedArray->push_back(osg::Vec2d(cbS * 0.5, -cbS * 0.5));  // Bottom right
   checkedArray->push_back(osg::Vec2d(-cbS * 0.5, cbS * 0.5));  // Top left

   // Show Air
   osg::ref_ptr<osg::PositionAttitudeTransform> showAirXform(new osg::PositionAttitudeTransform());
   showAirXform->setPosition(osg::Vec3d(startX, startY - (cbS * 7), 0.0));
   showAirXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cAIR))
   {
      showAirXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Air", startX + (cbS * 1), startY - (cbS * 7), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showAirXform.get());
   Button showAir;
   showAir.region = QRectF(startX, startY - (cbS * 7), cbS, cbS);
   showAir.function = std::bind(&Overlay::ToggleSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cAIR));
   buttons.push_back(showAir);

   // Show Ground
   osg::ref_ptr<osg::PositionAttitudeTransform> showGroundXform(new osg::PositionAttitudeTransform());
   showGroundXform->setPosition(osg::Vec3d(startX, startY - (cbS * 8) - (cP * 1), 0.0));
   showGroundXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cGROUND))
   {
      showGroundXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Ground", startX + (cbS * 1), startY - (cbS * 8) - (cP * 1), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showGroundXform.get());
   Button showGround;
   showGround.region = QRectF(startX, startY - (cbS * 8) - (cP * 1), cbS, cbS);
   showGround.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cGROUND));
   buttons.push_back(showGround);

   // Show Routes
   osg::ref_ptr<osg::PositionAttitudeTransform> showRoutesXform(new osg::PositionAttitudeTransform());
   showRoutesXform->setPosition(osg::Vec3d(startX, startY - (cbS * 9) - (cP * 2), 0.0));
   showRoutesXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cROUTE))
   {
      showRoutesXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show Routes", startX + (cbS * 1), startY - (cbS * 9) - (cP * 2), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showRoutesXform.get());
   Button showRoutes;
   showRoutes.region = QRectF(startX, startY - (cbS * 9) - (cP * 2), cbS, cbS);
   showRoutes.function =
      std::bind(&Overlay::ToggleSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cROUTE));
   buttons.push_back(showRoutes);

   // Show FOV
   osg::ref_ptr<osg::PositionAttitudeTransform> showFOVXform(new osg::PositionAttitudeTransform());
   showFOVXform->setPosition(osg::Vec3d(startX, startY - (cbS * 10) - (cP * 3), 0.0));
   showFOVXform->addChild(CreateGeometry(cbArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cFOV))
   {
      showFOVXform->addChild(CreateGeometry(checkedArray, GL_LINES, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Show FOV", startX + (cbS * 1), startY - (cbS * 10) - (cP * 3), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(showFOVXform.get());
   Button showFOV;
   showFOV.region = QRectF(startX, startY - (cbS * 10) - (cP * 3), cbS, cbS);
   showFOV.function = std::bind(&Overlay::ToggleSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cFOV));
   buttons.push_back(showFOV);

   // Show Entity Select Mode
   aVectorGroup->addChild(AddTextItem("Show Entity Select Mode",
                                      startX - (cbS * 0.5),
                                      startY - (cbS * 12) - (cP * 5),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());

   // Entity Select Mode (Hover)
   osg::ref_ptr<osg::PositionAttitudeTransform> entitySelectHoverXform(new osg::PositionAttitudeTransform());
   entitySelectHoverXform->setPosition(osg::Vec3d(startX + cbS, startY - (cbS * 13) - (cP * 6), 0.0));
   entitySelectHoverXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cHOVER))
   {
      entitySelectHoverXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Hover", startX + (cbS * 2), startY - (cbS * 13) - (cP * 6), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(entitySelectHoverXform.get());
   Button entitySelectHover;
   entitySelectHover.region = QRectF(startX + cbS, startY - (cbS * 13) - (cP * 6), rbR * 2, rbR * 2);
   entitySelectHover.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cHOVER));
   buttons.push_back(entitySelectHover);

   // Entity Select Mode (Click)
   osg::ref_ptr<osg::PositionAttitudeTransform> entitySelectClickXform(new osg::PositionAttitudeTransform());
   entitySelectClickXform->setPosition(osg::Vec3d(startX + cbS + (0.06 * pW()), startY - (cbS * 13) - (cP * 6), 0.0));
   entitySelectClickXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cCLICK))
   {
      entitySelectClickXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Click",
                                      startX + (cbS * 2) + (0.06 * pW()),
                                      startY - (cbS * 13) - (cP * 6),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(entitySelectClickXform.get());
   Button entitySelectClick;
   entitySelectClick.region = QRectF(startX + cbS + (0.06 * pW()), startY - (cbS * 13) - (cP * 6), rbR * 2, rbR * 2);
   entitySelectClick.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cCLICK));
   buttons.push_back(entitySelectClick);

   double buttonW = bW / 6.0;
   double buttonH = buttonW / 2.5;

   // Button box array
   osg::ref_ptr<osg::Vec2Array> buttonArray(new osg::Vec2Array);
   buttonArray->push_back(osg::Vec2d(buttonW * 0.5, buttonH * 0.5));   // Top right
   buttonArray->push_back(osg::Vec2d(buttonW * 0.5, -buttonH * 0.5));  // Bottom right
   buttonArray->push_back(osg::Vec2d(-buttonW * 0.5, -buttonH * 0.5)); // Bottom left
   buttonArray->push_back(osg::Vec2d(-buttonW * 0.5, buttonH * 0.5));  // Top left

   // Apply Button - Leaving this here in case we end up re-enabling it
   // osg::ref_ptr<osg::PositionAttitudeTransform> applyButtonXform(new osg::PositionAttitudeTransform());
   // applyButtonXform->setPosition(osg::Vec3d((0.5 * bW) - (buttonW * 1.5) - (buttonW * 0.2), (-0.5 * bH) + buttonH,
   // 0.0)); applyButtonXform->addChild(CreateGeometry(buttonArray, GL_LINE_LOOP, Color::White).get());
   // applyButtonXform->addChild(AddTextItem("APPLY", 0.0, 0.0, fontSize, Color::White,
   // osgText::Text::CENTER_CENTER).get()); aVectorGroup->addChild(applyButtonXform.get()); Button applyButton;
   // applyButton.region = QRectF((0.5 * bW) - (buttonW * 1.5) - (buttonW * 0.2), (-0.5 * bH) + buttonH, buttonW,
   // buttonH); applyButton.function = std::bind(aLeftSettings ? &Overlay::CloseLeftSettings :
   // &Overlay::CloseRightSettings, this); buttons.push_back(applyButton);

   // Cancel Button
   osg::ref_ptr<osg::PositionAttitudeTransform> closeButtonXform(new osg::PositionAttitudeTransform());
   closeButtonXform->setPosition(osg::Vec3d((0.5 * bW) - (buttonW * 0.5) - (buttonW * 0.1), (-0.5 * bH) + buttonH, 0.0));
   closeButtonXform->addChild(CreateGeometry(buttonArray, GL_LINE_LOOP, Color::White).get());
   closeButtonXform->addChild(AddTextItem("Close", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   aVectorGroup->addChild(closeButtonXform.get());
   Button closeButton;
   closeButton.region = QRectF((0.5 * bW) - (buttonW * 0.5) - (buttonW * 0.1), (-0.5 * bH) + buttonH, buttonW, buttonH);
   closeButton.function = std::bind(aLeftSettings ? &Overlay::CloseLeftSettings : &Overlay::CloseRightSettings, this);
   buttons.push_back(closeButton);
}

void wkf::AcesDisplay::Overlay::InitializeEngagementSettings(osg::ref_ptr<osg::Group> aVectorGroup)
{
   double fontSize = 0.016f * pH();

   double bW     = 0.3 * pW();
   double bH     = 0.25 * pH();
   double startX = -0.4 * bW;
   double startY = 0.4 * bH;

   // checkbox size
   double cbS = bH / 13.3;

   // checkbox padding
   double cP = cbS / 5.0;

   // radio button radius
   double rbR = cbS / 1.8;

   // Title
   aVectorGroup->addChild(
      AddTextItem("Engagement Settings", startX - (cbS * 0.5), startY, fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   osg::ref_ptr<osg::Vec2Array> radioButtonArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = rbR * cos(ang_rad);
      double y       = rbR * sin(ang_rad);
      radioButtonArray->push_back(osg::Vec2d(x, y));
   }
   osg::ref_ptr<osg::Vec2Array> radioButtonSelectedArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = rbR * 0.8 * cos(ang_rad);
      double y       = rbR * 0.8 * sin(ang_rad);
      radioButtonSelectedArray->push_back(osg::Vec2d(x, y));
   }

   // Checkbox arrays to be re-used
   osg::ref_ptr<osg::Vec2Array> cbArray(new osg::Vec2Array);
   cbArray->push_back(osg::Vec2d(cbS * 0.5, cbS * 0.5));   // Top right
   cbArray->push_back(osg::Vec2d(cbS * 0.5, -cbS * 0.5));  // Bottom right
   cbArray->push_back(osg::Vec2d(-cbS * 0.5, -cbS * 0.5)); // Bottom left
   cbArray->push_back(osg::Vec2d(-cbS * 0.5, cbS * 0.5));  // Top left
   osg::ref_ptr<osg::Vec2Array> checkedArray(new osg::Vec2Array);
   checkedArray->push_back(osg::Vec2d(cbS * 0.5, cbS * 0.5));   // Top right
   checkedArray->push_back(osg::Vec2d(-cbS * 0.5, -cbS * 0.5)); // Bottom left
   checkedArray->push_back(osg::Vec2d(cbS * 0.5, -cbS * 0.5));  // Bottom right
   checkedArray->push_back(osg::Vec2d(-cbS * 0.5, cbS * 0.5));  // Top left


   // Engagement select type
   aVectorGroup->addChild(
      AddTextItem("Engagement Select Type", startX - (cbS * 0.5), startY - (cbS * 3), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   // Manual
   osg::ref_ptr<osg::PositionAttitudeTransform> manualEngagementsXform(new osg::PositionAttitudeTransform());
   manualEngagementsXform->setPosition(osg::Vec3d(startX + cbS, startY - (cbS * 4) - (cP * 1), 0.0));
   manualEngagementsXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cMANUAL))
   {
      manualEngagementsXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(
      AddTextItem("Manual", startX + (cbS * 2), startY - (cbS * 4) - (cP * 1), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());
   aVectorGroup->addChild(manualEngagementsXform.get());
   Button manualEngagements;
   manualEngagements.region = QRectF(startX + cbS, startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   manualEngagements.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cENGAGE, static_cast<size_t>(ENGAGEMENT_SETTINGS::cMANUAL));
   mTopSettings.buttons.push_back(manualEngagements);

   // Threats
   osg::ref_ptr<osg::PositionAttitudeTransform> threatEngagementsXform(new osg::PositionAttitudeTransform());
   threatEngagementsXform->setPosition(osg::Vec3d(startX + cbS + (0.06 * pW()), startY - (cbS * 4) - (cP * 1), 0.0));
   threatEngagementsXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS))
   {
      threatEngagementsXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Threats",
                                      startX + (cbS * 2) + (0.06 * pW()),
                                      startY - (cbS * 4) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(threatEngagementsXform.get());
   Button threatEngagements;
   threatEngagements.region = QRectF(startX + cbS + (0.06 * pW()), startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   threatEngagements.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cENGAGE, static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS));
   mTopSettings.buttons.push_back(threatEngagements);

   // Targets
   osg::ref_ptr<osg::PositionAttitudeTransform> targetEngagementsXform(new osg::PositionAttitudeTransform());
   targetEngagementsXform->setPosition(osg::Vec3d(startX + cbS + (0.12 * pW()), startY - (cbS * 4) - (cP * 1), 0.0));
   targetEngagementsXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS))
   {
      targetEngagementsXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Targets",
                                      startX + (cbS * 2) + (0.12 * pW()),
                                      startY - (cbS * 4) - (cP * 1),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(targetEngagementsXform.get());
   Button targetEngagements;
   targetEngagements.region = QRectF(startX + cbS + (0.12 * pW()), startY - (cbS * 4) - (cP * 1), rbR * 2, rbR * 2);
   targetEngagements.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cENGAGE, static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS));
   mTopSettings.buttons.push_back(targetEngagements);


   // Unit type
   aVectorGroup->addChild(
      AddTextItem("Units", startX - (cbS * 0.5), startY - (cbS * 6) - (cP * 3), fontSize, Color::White, osgText::Text::LEFT_CENTER)
         .get());

   // Nautical Miles (nm)
   osg::ref_ptr<osg::PositionAttitudeTransform> entitySelectHoverXform(new osg::PositionAttitudeTransform());
   entitySelectHoverXform->setPosition(osg::Vec3d(startX + cbS, startY - (cbS * 7) - (cP * 4), 0.0));
   entitySelectHoverXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cNM))
   {
      entitySelectHoverXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Nautical Miles (nm)",
                                      startX + (cbS * 2),
                                      startY - (cbS * 7) - (cP * 4),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(entitySelectHoverXform.get());
   Button entitySelectHover;
   entitySelectHover.region = QRectF(startX + cbS, startY - (cbS * 7) - (cP * 4), rbR * 2, rbR * 2);
   entitySelectHover.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cENGAGE, static_cast<size_t>(ENGAGEMENT_SETTINGS::cNM));
   mTopSettings.buttons.push_back(entitySelectHover);

   // Kilometers (km)
   osg::ref_ptr<osg::PositionAttitudeTransform> entitySelectClickXform(new osg::PositionAttitudeTransform());
   entitySelectClickXform->setPosition(osg::Vec3d(startX + cbS + (0.12 * pW()), startY - (cbS * 7) - (cP * 4), 0.0));
   entitySelectClickXform->addChild(CreateGeometry(radioButtonArray, GL_LINE_LOOP, Color::White).get());
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cKM))
   {
      entitySelectClickXform->addChild(CreateGeometry(radioButtonSelectedArray, GL_POLYGON, Color::White).get());
   }
   aVectorGroup->addChild(AddTextItem("Kilometers (km)",
                                      startX + (cbS * 2) + (0.12 * pW()),
                                      startY - (cbS * 7) - (cP * 4),
                                      fontSize,
                                      Color::White,
                                      osgText::Text::LEFT_CENTER)
                             .get());
   aVectorGroup->addChild(entitySelectClickXform.get());
   Button entitySelectClick;
   entitySelectClick.region = QRectF(startX + cbS + (0.12 * pW()), startY - (cbS * 7) - (cP * 4), rbR * 2, rbR * 2);
   entitySelectClick.function =
      std::bind(&Overlay::SetRadioSetting, this, DisplayType::cENGAGE, static_cast<size_t>(ENGAGEMENT_SETTINGS::cKM));
   mTopSettings.buttons.push_back(entitySelectClick);

   double buttonW = bW / 6.0;
   double buttonH = buttonW / 2.5;

   // Button box array
   osg::ref_ptr<osg::Vec2Array> buttonArray(new osg::Vec2Array);
   buttonArray->push_back(osg::Vec2d(buttonW * 0.5, buttonH * 0.5));   // Top right
   buttonArray->push_back(osg::Vec2d(buttonW * 0.5, -buttonH * 0.5));  // Bottom right
   buttonArray->push_back(osg::Vec2d(-buttonW * 0.5, -buttonH * 0.5)); // Bottom left
   buttonArray->push_back(osg::Vec2d(-buttonW * 0.5, buttonH * 0.5));  // Top left

   // Apply Button - Leaving this here in case we end up re-enabling it
   // osg::ref_ptr<osg::PositionAttitudeTransform> applyButtonXform(new osg::PositionAttitudeTransform());
   // applyButtonXform->setPosition(osg::Vec3d((0.5 * bW) - (buttonW * 1.5) - (buttonW * 0.2), (-0.5 * bH) + buttonH,
   // 0.0)); applyButtonXform->addChild(CreateGeometry(buttonArray, GL_LINE_LOOP, Color::White).get());
   // applyButtonXform->addChild(AddTextItem("APPLY", 0.0, 0.0, fontSize, Color::White,
   // osgText::Text::CENTER_CENTER).get()); aVectorGroup->addChild(applyButtonXform.get()); Button applyButton;
   // applyButton.region = QRectF((0.5 * bW) - (buttonW * 1.5) - (buttonW * 0.2), (-0.5 * bH) + buttonH, buttonW,
   // buttonH); applyButton.function = std::bind(&Overlay::CloseTopSettings, this);
   // mTopSettings.buttons.push_back(applyButton);

   // Close Button
   osg::ref_ptr<osg::PositionAttitudeTransform> closeButtonXform(new osg::PositionAttitudeTransform());
   closeButtonXform->setPosition(osg::Vec3d((0.5 * bW) - (buttonW * 0.5) - (buttonW * 0.1), (-0.5 * bH) + buttonH, 0.0));
   closeButtonXform->addChild(CreateGeometry(buttonArray, GL_LINE_LOOP, Color::White).get());
   closeButtonXform->addChild(AddTextItem("Close", 0.0, 0.0, fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
   aVectorGroup->addChild(closeButtonXform.get());
   Button closeButton;
   closeButton.region = QRectF((0.5 * bW) - (buttonW * 0.5) - (buttonW * 0.1), (-0.5 * bH) + buttonH, buttonW, buttonH);
   closeButton.function = std::bind(&Overlay::CloseTopSettings, this);
   mTopSettings.buttons.push_back(closeButton);
}

void wkf::AcesDisplay::Overlay::DrawButtonRegions(osg::ref_ptr<osg::Group> aVectorGroup)
{
   // This is for debugging.  This will draw green boxes over button regions.
   for (auto& button : mButtons)
   {
      if (button.clickable)
      {
         // Transformation
         osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
         vectorXform->setPosition(osg::Vec3d(button.region.x() * pW(), button.region.y() * pH(), 0.0));

         // Box rect
         osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5) * pW(), (button.region.height() * 0.5) * pH()));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5) * pW(), (button.region.height() * 0.5) * pH()));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5) * pW(), -(button.region.height() * 0.5) * pH()));
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5) * pW(), -(button.region.height() * 0.5) * pH()));
         vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, Color::Green).get());

         aVectorGroup->addChild(vectorXform.get());
      }
   }

   for (auto& button : mSettingsButtons)
   {
      if (button.second.clickable)
      {
         // Transformation
         osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
         vectorXform->setPosition(osg::Vec3d(button.second.region.x() * pW(), button.second.region.y() * pH(), 0.0));

         // Box rect
         osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
         boxArray->push_back(
            osg::Vec2d(-(button.second.region.width() * 0.5) * pW(), (button.second.region.height() * 0.5) * pH()));
         boxArray->push_back(
            osg::Vec2d((button.second.region.width() * 0.5) * pW(), (button.second.region.height() * 0.5) * pH()));
         boxArray->push_back(
            osg::Vec2d((button.second.region.width() * 0.5) * pW(), -(button.second.region.height() * 0.5) * pH()));
         boxArray->push_back(
            osg::Vec2d(-(button.second.region.width() * 0.5) * pW(), -(button.second.region.height() * 0.5) * pH()));
         vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, Color::Green).get());

         aVectorGroup->addChild(vectorXform.get());
      }
   }

   if (mLeftSettings.visible && (mLeftSettings.menuState == MenuState::cOPEN))
   {
      for (auto& button : mLeftSettings.buttons)
      {
         double xOffset = -0.35 * pW();
         double yOffset = -0.075 * pH();

         // Transformation
         osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
         vectorXform->setPosition(osg::Vec3d(button.region.x() + xOffset, button.region.y() + yOffset, 0.0));

         // Box rect
         osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5), (button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5), (button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5), -(button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5), -(button.region.height() * 0.5)));
         vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, Color::Green).get());

         aVectorGroup->addChild(vectorXform.get());
      }
   }

   if (mRightSettings.visible && (mRightSettings.menuState == MenuState::cOPEN))
   {
      for (auto& button : mRightSettings.buttons)
      {
         double xOffset = 0.35 * pW();
         double yOffset = -0.075 * pH();

         // Transformation
         osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
         vectorXform->setPosition(osg::Vec3d(button.region.x() + xOffset, button.region.y() + yOffset, 0.0));

         // Box rect
         osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5), (button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5), (button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5), -(button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5), -(button.region.height() * 0.5)));
         vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, Color::Green).get());

         aVectorGroup->addChild(vectorXform.get());
      }
   }

   if (mTopSettings.visible && (mTopSettings.menuState == MenuState::cOPEN))
   {
      for (auto& button : mTopSettings.buttons)
      {
         double xOffset = 0.0 * pW();
         double yOffset = 0.375 * pH();

         // Transformation
         osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
         vectorXform->setPosition(osg::Vec3d(button.region.x() + xOffset, button.region.y() + yOffset, 0.0));

         // Box rect
         osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5), (button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5), (button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d((button.region.width() * 0.5), -(button.region.height() * 0.5)));
         boxArray->push_back(osg::Vec2d(-(button.region.width() * 0.5), -(button.region.height() * 0.5)));
         vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, Color::Green).get());

         aVectorGroup->addChild(vectorXform.get());
      }
   }
}

void wkf::AcesDisplay::Overlay::UpdateEntityInfo()
{
   // Handle updating entity info if there is one hovered/clicked on currently
   if (mSAD_ShowingEntityInfo)
   {
      bool currentEntityFound = false;
      for (auto& entity : mSAD_Entities)
      {
         if (entity.isGroup)
         {
            // If this is the track matching the current entity, update it.
            if (entity.groupPerception.groupName == mSAD_CurrentEntity.groupPerception.groupName)
            {
               currentEntityFound = true;
               SAD_SetCurrentEntity(entity);
            }
         }
         else
         {
            // If this is the track matching the current entity, update it.
            if (entity.perception.perceivedName == mSAD_CurrentEntity.perception.perceivedName)
            {
               currentEntityFound = true;
               SAD_SetCurrentEntity(entity);
            }
         }
      }

      for (auto& entity : mSAD_TruthEntities)
      {
         // If this is the track matching the current entity, update it.
         if (entity.truth.name == mSAD_CurrentEntity.truth.name)
         {
            currentEntityFound = true;
            SAD_SetCurrentEntity(entity);
         }
      }

      // If we didn't find a matching entity, close the entity window and
      // reset vars
      if (!currentEntityFound)
      {
         mSAD_ShowingEntityInfo = false;
         mSAD_EntityClicked     = false;
      }
   }

   // Handle updating entity info if there is one hovered/clicked on currently
   if (mTSD_ShowingEntityInfo)
   {
      bool currentEntityFound = false;
      for (auto& entity : mTSD_Entities)
      {
         // If this is the track matching the current entity, update it.
         if (entity.track.tgtPlatformIndex == mTSD_CurrentEntity.track.tgtPlatformIndex)
         {
            currentEntityFound = true;
            TSD_SetCurrentEntity(entity);
         }
      }

      for (auto& entity : mTSD_Assets)
      {
         // If this is the track matching the current entity, update it.
         if (entity.perceivedData.perceivedName == mTSD_CurrentEntity.perceivedData.perceivedName)
         {
            currentEntityFound = true;
            TSD_SetCurrentEntity(entity);
         }
      }

      // If we didn't find a matching entity, close the entity window and
      // reset vars
      if (!currentEntityFound)
      {
         mTSD_ShowingEntityInfo = false;
         mTSD_EntityClicked     = false;
      }
   }
}

void wkf::AcesDisplay::Overlay::UpdatePriorityTargetsThreats()
{
   auto sourcePlatformEngagementData = mData.engagementData.find(mSourcePlatform);
   // If we have data, loop through each engagement and update the priority target and threat lists
   if (sourcePlatformEngagementData != mData.engagementData.end())
   {
      wkf::Scenario* scenario = vaEnv.GetStandardScenario();
      if (scenario != nullptr)
      {
         // lambda function for checking if a target should be removed from the list
         auto shouldRemoveTarget = [&](const size_t aPlatformIndex)
         {
            bool shouldRemove = true;
            for (const auto& targetName : sourcePlatformEngagementData->second.mPrioritizedTargetNames)
            {
               wkf::Platform* platform = scenario->FindPlatformByIndex(aPlatformIndex);
               if (platform != nullptr)
               {
                  if (platform->GetName() == targetName)
                  {
                     shouldRemove = false;
                     break;
                  }
               }
            }

            return shouldRemove;
         };

         // Remove any targets that should no longer be in the list
         auto removetarget = std::find_if(mPrioritizedTargets.begin(), mPrioritizedTargets.end(), shouldRemoveTarget);
         while (removetarget != mPrioritizedTargets.end())
         {
            mPrioritizedTargets.erase(removetarget);
            removetarget = std::find_if(mPrioritizedTargets.begin(), mPrioritizedTargets.end(), shouldRemoveTarget);
         }

         // lambda function for checking if a target should be added to the list
         auto shouldAddTarget = [&](const std::string& aTargetName)
         {
            bool shouldAdd = true;

            // Check if the target platform exists.
            if (scenario->FindPlatformByName(aTargetName) == nullptr)
            {
               shouldAdd = false;
            }
            else
            {
               // Check if the target platform is already in the prioritized list.
               for (const auto& targetIndex : mPrioritizedTargets)
               {
                  wkf::Platform* target = scenario->FindPlatformByIndex(targetIndex);
                  if (target != nullptr)
                  {
                     if (target->GetName() == aTargetName)
                     {
                        shouldAdd = false;
                        break;
                     }
                  }
               }
            }

            return shouldAdd;
         };

         // Add any targets that should be in the list
         auto addTarget = std::find_if(sourcePlatformEngagementData->second.mPrioritizedTargetNames.begin(),
                                       sourcePlatformEngagementData->second.mPrioritizedTargetNames.end(),
                                       shouldAddTarget);
         while (addTarget != sourcePlatformEngagementData->second.mPrioritizedTargetNames.end())
         {
            wkf::Platform* target = scenario->FindPlatformByName((*addTarget));
            if (target != nullptr)
            {
               mPrioritizedTargets.emplace_back(target->GetIndex());
            }

            addTarget = std::find_if(sourcePlatformEngagementData->second.mPrioritizedTargetNames.begin(),
                                     sourcePlatformEngagementData->second.mPrioritizedTargetNames.end(),
                                     shouldAddTarget);
         }

         // lambda function for checking if a threat should be removed from the list
         auto shouldRemoveThreat = [&](const size_t aPlatformIndex)
         {
            bool shouldRemove = true;
            for (const auto& threatName : sourcePlatformEngagementData->second.mPrioritizedThreatNames)
            {
               wkf::Platform* platform = scenario->FindPlatformByIndex(aPlatformIndex);
               if (platform != nullptr)
               {
                  if (platform->GetName() == threatName)
                  {
                     shouldRemove = false;
                     break;
                  }
               }
            }

            return shouldRemove;
         };

         // Remove any threats that should no longer be in the list
         auto removeThreat = std::find_if(mPrioritizedThreats.begin(), mPrioritizedThreats.end(), shouldRemoveThreat);
         while (removeThreat != mPrioritizedThreats.end())
         {
            mPrioritizedThreats.erase(removeThreat);
            removeThreat = std::find_if(mPrioritizedThreats.begin(), mPrioritizedThreats.end(), shouldRemoveThreat);
         }

         // lambda function for checking if a threat should be added to the list
         auto shouldAddThreat = [&](const std::string& aThreatName)
         {
            bool shouldAdd = true;

            // Check if the threat platform exists.
            if (scenario->FindPlatformByName(aThreatName) == nullptr)
            {
               shouldAdd = false;
            }
            else
            {
               // Check if the threat platform is already in the prioritized list.
               for (const auto& threatIndex : mPrioritizedThreats)
               {
                  wkf::Platform* threat = scenario->FindPlatformByIndex(threatIndex);
                  if (threat != nullptr)
                  {
                     if (threat->GetName() == aThreatName)
                     {
                        shouldAdd = false;
                        break;
                     }
                  }
               }
            }

            return shouldAdd;
         };

         // Add any threats that should be in the list
         auto addThreat = std::find_if(sourcePlatformEngagementData->second.mPrioritizedThreatNames.begin(),
                                       sourcePlatformEngagementData->second.mPrioritizedThreatNames.end(),
                                       shouldAddThreat);
         while (addThreat != sourcePlatformEngagementData->second.mPrioritizedThreatNames.end())
         {
            wkf::Platform* threat = scenario->FindPlatformByName((*addThreat));
            if (threat != nullptr)
            {
               mPrioritizedThreats.emplace_back(threat->GetIndex());
            }

            addThreat = std::find_if(sourcePlatformEngagementData->second.mPrioritizedThreatNames.begin(),
                                     sourcePlatformEngagementData->second.mPrioritizedThreatNames.end(),
                                     shouldAddThreat);
         }
      }

      // If the engagement region is showing prioritized targets, make
      // sure that the current target is still in the current list,
      // otherwise go to another platform that is in the list and show it instead
      if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS))
      {
         // Attempt to find the current target
         bool found = false;
         if (mTargetPlatform != 0)
         {
            for (const auto& targetIndex : mPrioritizedTargets)
            {
               if (mTargetPlatform == targetIndex)
               {
                  found = true;
                  break;
               }
            }
         }

         // If we can't find the current target anymore, default
         // to the first target in the list currently
         if (!found)
         {
            if (mPrioritizedTargets.size() > 0)
            {
               mTargetPlatform = mPrioritizedTargets.front();
            }
            else
            {
               mTargetPlatform = 0;
            }
         }
      }
      else if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS))
      {
         // Attempt to find the current threat
         bool found = false;
         if (mTargetPlatform != 0)
         {
            for (const auto& targetIndex : mPrioritizedThreats)
            {
               if (mTargetPlatform == targetIndex)
               {
                  found = true;
                  break;
               }
            }
         }

         // If we can't find the current threat anymore, default
         // to the first target in the list currently
         if (!found)
         {
            if (mPrioritizedThreats.size() > 0)
            {
               mTargetPlatform = mPrioritizedThreats.front();
            }
            else
            {
               mTargetPlatform = 0;
            }
         }
      }
   }
}

void wkf::AcesDisplay::Overlay::NextTarget()
{
   // Set the next target based on the engagement mode chosen
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS))
   {
      SetNewTarget(mPrioritizedTargets, true);
   }
   else if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS))
   {
      SetNewTarget(mPrioritizedThreats, true);
   }
   else
   {
      SetNewTarget(mTargetPlatforms, true);
   }
}

void wkf::AcesDisplay::Overlay::PreviousTarget()
{
   // Set the previous target based on the engagement mode chosen
   if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTARGETS))
   {
      SetNewTarget(mPrioritizedTargets, false);
   }
   else if (mSettings[DisplayType::cENGAGE] & static_cast<size_t>(ENGAGEMENT_SETTINGS::cTHREATS))
   {
      SetNewTarget(mPrioritizedThreats, false);
   }
   else
   {
      SetNewTarget(mTargetPlatforms, false);
   }
}

void wkf::AcesDisplay::Overlay::SetNewTarget(std::vector<size_t>& aPlatformList, bool aNext)
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      // If we have a current target, attempt to find the next
      // or previous target in the list from its position in the list
      if (mTargetPlatform != 0)
      {
         // Get an iterator and go to the next or previous target, looping if the edge
         // of the list is hit
         auto itr = std::find(aPlatformList.begin(), aPlatformList.end(), mTargetPlatform);
         if (itr != aPlatformList.end())
         {
            auto index = std::distance(aPlatformList.begin(), itr);
            if (aNext)
            {
               auto nextIndex = index + 1;
               nextIndex > static_cast<int>(aPlatformList.size() - 1) ? mTargetPlatform = aPlatformList.front() :
                                                                        mTargetPlatform = aPlatformList.at(nextIndex);
            }
            else
            {
               auto prevIndex = index - 1;
               prevIndex < 0 ? mTargetPlatform = aPlatformList.back() : mTargetPlatform = aPlatformList.at(prevIndex);
            }
         }
         else
         {
            aPlatformList.size() > 0 ? mTargetPlatform = aPlatformList.front() : mTargetPlatform = 0;
         }
      }
   }
}

void wkf::AcesDisplay::Overlay::SwapDisplays(const DrawRegion aRegion1, const DrawRegion aRegion2)
{
   // Get the display at region 1 and 2, then swap them.
   // Use a reversed map for this. A short easy algorithm works since this map is 1-to-1.
   std::map<DrawRegion, DisplayType> reversed;
   for (std::map<DisplayType, DrawRegion>::iterator it = mDisplays.begin(); it != mDisplays.end(); ++it)
   {
      reversed[it->second] = it->first;
   }

   auto display1 = reversed.at(aRegion1);
   auto display2 = reversed.at(aRegion2);

   // Swap the display values in the map
   std::swap(mDisplays.at(display1), mDisplays.at(display2));

   // Set the compass radii for TSD and SAD if applicable,
   // and re initialize the settings tabs if they need swapped
   if (display1 == DisplayType::cSAD || display2 == DisplayType::cSAD)
   {
      mSAD_CompassRadius = GetDrawVector(mDisplays[DisplayType::cSAD]).z();
      InitializeLeftSettings();
      InitializeRightSettings();
   }
   if (display1 == DisplayType::cTSD || display2 == DisplayType::cTSD)
   {
      mTSD_CompassRadius = GetDrawVector(mDisplays[DisplayType::cTSD]).z();
      InitializeLeftSettings();
      InitializeRightSettings();
   }
}

const std::string wkf::AcesDisplay::Overlay::GetTrackIDString(const TargetTrackItem& aTrack)
{
   // Prioritize: Missile -> Type -> Side -> IFF -> Aux
   if (aTrack.isMissile)
   {
      // aTrack.tgtIdString should have a missile value, use it to look up a mapping
      return "M";
   }
   else if (aTrack.idTypeValid)
   {
      // aTrack.tgtIdString should be the type of the platform, use it to look up a mapping
   }
   else if (aTrack.idSideValid)
   {
      // aTrack.tgtIdString should have a side value, use it to look up a mapping
   }
   else if (aTrack.idIffValid)
   {
      // aTrack.tgtIdString should have an IFF value (friend, foe, unknown)
   }
   else if (aTrack.idAuxValid)
   {
      // aTrack.tgtIdString should have an aux data value, use it to look up a mapping
   }
   return "U";
}

const wkf::AcesDisplay::Overlay::DrawRegion wkf::AcesDisplay::Overlay::GetMouseDrawRegion()
{
   // If the mouse is on the left side of the screen,
   // the draw region is on the left, otherwise the right
   if (mMousePos.x() <= (0.5 * pW()))
   {
      return DrawRegion::cLEFT;
   }
   else
   {
      return DrawRegion::cRIGHT;
   }
}

double wkf::AcesDisplay::Overlay::NormalizeRadians(const double aRadians)
{
   // Keep the radians in a display safe range
   double aNewRadians = aRadians;
   if (aNewRadians < -UtMath::cPI)
   {
      aNewRadians += UtMath::cTWO_PI;
   }
   if (aNewRadians > UtMath::cPI)
   {
      aNewRadians -= UtMath::cTWO_PI;
   }
   return aNewRadians;
}

void wkf::AcesDisplay::Overlay::TSD_DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup,
                                                const double             aX,
                                                const double             aY,
                                                const double             aR)
{
   float fontSize = 0.04f * aR * rW();

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

   // Draw the rings
   double distBetweenRings = aR / 4;
   for (int r = ((mTSDRange_nm > 10) ? 1 : 2); r <= 7; (mTSDRange_nm > 10) ? r++ : r += 2)
   {
      // Skip the fourth ring if the ownship is centered to avoid artifacting.
      if (r == 4 && (mTSD_CenterPoint.x() == 0.0 && mTSD_CenterPoint.y() == 0.0) && (mTSD_YOffset == 0.0))
      {
         continue;
      }

      osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
         double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);
         ringArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
      vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::LightGray).get());

      // Draw the range in nm on each ring
      std::stringstream nm;
      nm << std::fixed << std::setprecision(1) << (mTSDRange_nm * (r / 4.0));
      double ang_rad = 60.0 * UtMath::cRAD_PER_DEG;
      double x       = (0.0 + (distBetweenRings * r)) * cos(ang_rad);
      double y       = (0.0 + (distBetweenRings * r)) * sin(ang_rad);

      // We need a small black circle so the range is easy to see
      osg::ref_ptr<osg::Vec2Array> blackCircleArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double cX      = 0.04 * aR * cos(ang_rad);
         double cY      = 0.04 * aR * sin(ang_rad);
         blackCircleArray->push_back(osg::Vec2d((x + cX) * rW(), (y + cY) * rH()));
      }
      // Draw a black circle with the range inside of it
      auto circle = CreateGeometry(blackCircleArray, GL_POLYGON, Color::DarkGray);
      circle->addChild(
         AddTextItem(nm.str(), x * rW(), y * rH(), fontSize, Color::White, osgText::Text::CENTER_CENTER).get());
      vectorXform->addChild(circle.get());
   }

   // Orient based on the heading
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorHeadingXForm(new osg::PositionAttitudeTransform());
   vectorHeadingXForm->setAttitude(
      osg::Quat((mData.platformData.heading_deg + 90) * UtMath::cRAD_PER_DEG, osg::Vec3(0.0f, 0.0f, 1.0f)));

   // Parts that should rotate with the heading add to vectorXForm
   // Ticks every 10 degrees around the large ring.
   osg::ref_ptr<osg::Vec2Array> angleTicksArray(new osg::Vec2Array);
   double                       tickLength = 0.052 * aR;
   double                       tickRadius = (distBetweenRings * ((mTSDRange_nm > 10) ? 3 : 4)) - (tickLength * 0.5);

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
               AddTextItem("N", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 30:
            vectorHeadingXForm->addChild(
               AddTextItem("30", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 60:
            vectorHeadingXForm->addChild(
               AddTextItem("60", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 90:
            vectorHeadingXForm->addChild(
               AddTextItem("E", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 120:
            vectorHeadingXForm->addChild(
               AddTextItem("120", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 150:
            vectorHeadingXForm->addChild(
               AddTextItem("150", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 180:
            vectorHeadingXForm->addChild(
               AddTextItem("S", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 210:
            vectorHeadingXForm->addChild(
               AddTextItem("210", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 240:
            vectorHeadingXForm->addChild(
               AddTextItem("240", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 270:
            vectorHeadingXForm->addChild(
               AddTextItem("W", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 300:
            vectorHeadingXForm->addChild(
               AddTextItem("300", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
            break;
         case 330:
            vectorHeadingXForm->addChild(
               AddTextItem("330", x * rW(), y * rH(), fontSize, Color::Gray, osgText::Text::CENTER_CENTER).get());
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
         angleTicksArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         angleTicksArray->push_back(osg::Vec2d(x2 * rW(), y2 * rH()));
      }
   }
   vectorHeadingXForm->addChild(CreateGeometry(angleTicksArray, GL_LINES, Color::Gray).get());
   vectorXform->addChild(vectorHeadingXForm);

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::DrawTWDCompass(osg::ref_ptr<osg::Group> aVectorGroup,
                                               const double             aX,
                                               const double             aY,
                                               const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Draw the three larger rings
   for (int r = 0; r < 3; r++)
   {
      osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg <= 360; ang_deg += 3)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = ((aR * 0.25) + ((aR * 0.375) * r)) * cos(ang_rad);
         double y       = ((aR * 0.25) + ((aR * 0.375) * r)) * sin(ang_rad);
         ringArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
      vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::LightGray).get());
   }

   // Orient based on the heading
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorHeadingXForm(new osg::PositionAttitudeTransform());
   vectorHeadingXForm->setAttitude(
      osg::Quat((mData.platformData.heading_deg * UtMath::cRAD_PER_DEG), osg::Vec3(0.0f, 0.0f, 1.0f)));

   // Draw the 4 lines (two horizontal, two vertical) going
   // from the first large ring to the third ring.
   osg::ref_ptr<osg::Vec2Array> linesArray(new osg::Vec2Array);
   linesArray->push_back(osg::Vec2d((aR * 0.25) * rH(), 0.0));
   linesArray->push_back(osg::Vec2d((aR * 1.1) * rH(), 0.0));
   linesArray->push_back(osg::Vec2d(-(aR * 0.25) * rH(), 0.0));
   linesArray->push_back(osg::Vec2d(-(aR * 1.1) * rH(), 0.0));
   linesArray->push_back(osg::Vec2d(0.0, (aR * 0.25) * rH()));
   linesArray->push_back(osg::Vec2d(0.0, (aR * 1.1) * rH()));
   linesArray->push_back(osg::Vec2d(0.0, -(aR * 0.25) * rH()));
   linesArray->push_back(osg::Vec2d(0.0, -(aR * 1.1) * rH()));
   vectorHeadingXForm->addChild(CreateGeometry(linesArray, GL_LINES, Color::LightGray).get());

   // Parts that should rotate with the heading add to vectorXForm.
   // Ticks every 10 degrees around the large ring.
   osg::ref_ptr<osg::Vec2Array> angleTicksArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 15)
   {
      double tickLength = (aR * 0.125);
      if (ang_deg == 45 || ang_deg == 135 || ang_deg == 225 || ang_deg == 315)
      {
         tickLength = (aR * 0.25);
      }
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = aR * cos(ang_rad);
      double x2      = (aR - tickLength) * cos(ang_rad);
      double y       = aR * sin(ang_rad);
      double y2      = (aR - tickLength) * sin(ang_rad);
      angleTicksArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      angleTicksArray->push_back(osg::Vec2d(x2 * rW(), y2 * rH()));
   }
   vectorHeadingXForm->addChild(CreateGeometry(angleTicksArray, GL_LINES, Color::LightGray).get());
   vectorXform->addChild(vectorHeadingXForm);

   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::DrawTWDTrack(osg::ref_ptr<osg::Group> aVectorGroup,
                                             TargetTrackItem&         aTrack,
                                             const double             aX,
                                             const double             aY,
                                             const double             aR)
{
   // Position to match the compass
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * pW(), aY * pH(), 0.0));

   // Calculate the position and orientation of the entity
   double az_rad = NormalizeRadians(aTrack.tgtBearing_rad - (mData.platformData.heading_deg * UtMath::cRAD_PER_DEG));

   // Position the drawing in the center of the last ring.
   double tgtX = (aR * 0.8125) * sin(az_rad);
   double tgtY = (aR * 0.8125) * cos(az_rad);

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> entityXform(new osg::PositionAttitudeTransform());
   entityXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

   osg::Vec4 iconColor = Color::Yellow; // Assume bogie (unknown)
   if (aTrack.identification == Identification::ID_BANDIT)
   {
      iconColor = Color::Red;
   }
   else if (aTrack.identification == Identification::ID_FRIENDLY)
   {
      iconColor = Color::Green;
   }
   else if (aTrack.identification == Identification::ID_NEUTRAL)
   {
      iconColor = Color::White;
   }

   float       fontSize = 0.12f * aR * rW();
   std::string id       = GetTrackIDString(aTrack);
   double      size     = aR * 0.2;
   // Draw a square symbol by default, but draw a diamond for a missile
   if (aTrack.isMissile)
   {
      // Entity diamond
      osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
      entityArray->push_back(osg::Vec2d(0.0, -(size * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d((size * 0.5) * rW(), 0.0));
      entityArray->push_back(osg::Vec2d(0.0, (size * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d(-(size * 0.5) * rW(), 0.0));
      entityXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, iconColor).get());
   }
   else
   {
      // Entity square
      osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
      entityArray->push_back(osg::Vec2d(-(size * 0.5) * rW(), -(size * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d(-(size * 0.5) * rW(), (size * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d((size * 0.5) * rW(), (size * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d((size * 0.5) * rW(), -(size * 0.5) * rH()));
      entityXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, iconColor).get());
   }
   entityXform->addChild(AddTextItem(id, 0.0, 0.0, fontSize, iconColor, osgText::Text::CENTER_CENTER).get());
   vectorXform->addChild(entityXform.get());
   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction)
{
   mHandleEventMode = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mBtn1Function    = aFunction;
}

void wkf::AcesDisplay::Overlay::SetMouseMoveAction(const std::function<bool(int, int)>& aFunction)
{
   mHandleEventMode   = vespa::VaOverlay::cHANDLE_EVENTS_FOCUS;
   mMouseMoveFunction = aFunction;
}

osg::ref_ptr<osg::Geode> wkf::AcesDisplay::Overlay::CreateGeometry(osg::ref_ptr<osg::Vec2Array> aArray,
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
   lineWidthAttr->setWidth(0.5f * mLineWidthMult);
   lineState->setAttribute(lineWidthAttr.get());
   lineState->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   lineState->setMode(GL_BLEND, osg::StateAttribute::ON);
   retGeode->setStateSet(lineState.get());

   retGeode->addDrawable(geom.get());
   return retGeode;
}

osg::ref_ptr<osg::Geode> wkf::AcesDisplay::Overlay::AddTextItem(const std::string&                 aStr,
                                                                const double                       aXPos,
                                                                const double                       aYPos,
                                                                const float                        aFontSize,
                                                                const osg::Vec4&                   aColor,
                                                                const osgText::Text::AlignmentType aAlignment,
                                                                const bool                         aAlignToScreen)
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
   if (aAlignToScreen)
   {
      textItem->setAxisAlignment(osgText::Text::SCREEN);
   }
   textItem->setPosition(osg::Vec3d(aXPos, aYPos, 0));
   textItem->setColor(aColor);

   osg::ref_ptr<osg::Geode> geoNode(new osg::Geode);
   geoNode->addDrawable(textItem);
   return geoNode;
}

void wkf::AcesDisplay::Overlay::DrawBackground(const osg::Vec4& aColor)
{
   // Background rect
   osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.5, mRect.height() * 0.5));   // Top right
   boxArray->push_back(osg::Vec2d(mRect.width() * 0.5, -mRect.height() * 0.5));  // Bottom right
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.5, -mRect.height() * 0.5)); // Bottom left
   boxArray->push_back(osg::Vec2d(-mRect.width() * 0.5, mRect.height() * 0.5));  // Top left
   mOverlayGeode->addChild(CreateGeometry(boxArray, GL_POLYGON, aColor).get());
}

void wkf::AcesDisplay::Overlay::DrawOutline(const osg::Vec4& aColor)
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

void wkf::AcesDisplay::Overlay::DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
                                        const osg::Vec4&         aColor,
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
   vectorXform->addChild(CreateGeometry(boxArray, GL_POLYGON, aColor).get());

   // vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::DrawLabel(const double                 aXPos,
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

void wkf::AcesDisplay::Overlay::Resize()
{
   SetPosition(0, 0);
   SetSize(GetViewer()->GetWidth(), GetViewer()->GetHeight());
   InitializeDrawables();
}

void wkf::AcesDisplay::Overlay::SetPosition(float aX, float aY)
{
   mRect.setX(aX);
   mRect.setY(aY);
   mOverlayModelViewMatrix->setMatrix(
      osg::Matrix::translate(osg::Vec3(mRect.x() + mRect.width() * 0.5, mRect.y() + mRect.height() * 0.5, 0)));
}

void wkf::AcesDisplay::Overlay::SetSize(float aW, float aH)
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

void wkf::AcesDisplay::Overlay::SetAcesData(const AcesData& aData)
{
   // Set our data
   mData = aData;

   // Clear the entities and populate the new list from our new data.
   mSAD_Entities.clear();
   for (auto& entityPerception : mData.assetsList)
   {
      Entity e;
      e.perception = entityPerception;
      mSAD_Entities.push_back(e);
   }
   for (auto& entityPerception : mData.bogiesList)
   {
      Entity e;
      e.perception = entityPerception;
      mSAD_Entities.push_back(e);
   }
   for (auto& entityPerception : mData.banditsList)
   {
      Entity e;
      e.perception = entityPerception;
      mSAD_Entities.push_back(e);
   }
   for (auto& groupPerception : mData.groupsList)
   {
      Entity e;
      e.isGroup         = true;
      e.groupPerception = groupPerception;
      mSAD_Entities.push_back(e);
   }

   // TSD
   mTSD_Entities.clear();
   // This bool tracks whether or not we found a matching track for the
   // current entity, if there is one.
   for (auto& track : aData.masterTrackListData.mTracks)
   {
      TSD_Entity entity;
      entity.type  = TSD_EntityType::TRACK;
      entity.track = track.second;
      mTSD_Entities.push_back(entity);
   }

   mTSD_Assets.clear();
   // This bool tracks whether or not we found a matching track for the
   // current entity, if there is one.
   // currentEntityFound = false;
   for (auto& perception : aData.assetsList)
   {
      TSD_Entity entity;
      entity.type          = TSD_EntityType::ASSET;
      entity.perceivedData = perception;
      mTSD_Assets.push_back(entity);
   }
}

void wkf::AcesDisplay::Overlay::SetAcesTruthPlatforms(const std::vector<AcesDisplay::EntityTruth>& aData)
{
   // Clear our truth entities and repopulate.
   mSAD_TruthEntities.clear();
   for (auto& entityTruth : aData)
   {
      Entity e;
      e.truth   = entityTruth;
      e.isTruth = true;
      mSAD_TruthEntities.push_back(e);
   }
}

void wkf::AcesDisplay::Overlay::ResetCenterPoint()
{
   // Reset both TSD and SAD to center
   if (mDisplaySync)
   {
      mSAD_CenterPoint = mTSD_CenterPoint = QPointF(0, 0);
   }
   // Figure out which display is being moused over and
   // only reset that one
   else
   {
      auto drawRegion = GetMouseDrawRegion();
      if (mDisplays[DisplayType::cSAD] == drawRegion)
      {
         mSAD_CenterPoint = QPointF(0, 0);
      }
      else if (mDisplays[DisplayType::cTSD] == drawRegion)
      {
         mTSD_CenterPoint = QPointF(0, 0);
      }
   }
}

void wkf::AcesDisplay::Overlay::UpdatePriority()
{
   mHorizontalPriority = mRect.width() > mRect.height() ? false : true;

   if (mHorizontalPriority)
   {
      mRatio = mRect.width() / mRect.height();
   }
   else
   {
      mRatio = mRect.height() / mRect.width();
   }
}

void wkf::AcesDisplay::Overlay::SetAcesPlatformComment(const std::string& aPlatformCommentStr)
{
   mPlatformComment = aPlatformCommentStr;
}

void wkf::AcesDisplay::Overlay::SetCenterPosition(const DisplayType aDisplayType)
{
   if (mDisplaySync)
   {
      mSAD_YOffset = mTSD_YOffset = 0.0;
      mSAD_OffsetMiddle = mTSD_OffsetMiddle = false;
      mSAD_OffsetBottom = mTSD_OffsetBottom = false;
      mSettings[DisplayType::cSAD] |= static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER);
      mSettings[DisplayType::cSAD] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE));
      mSettings[DisplayType::cSAD] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM));
      mSettings[DisplayType::cTSD] |= static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER);
      mSettings[DisplayType::cTSD] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE));
      mSettings[DisplayType::cTSD] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM));
   }
   else
   {
      if (aDisplayType == DisplayType::cSAD)
      {
         mSAD_YOffset      = 0.0;
         mSAD_OffsetMiddle = false;
         mSAD_OffsetBottom = false;
         mSettings[aDisplayType] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE));
         mSettings[aDisplayType] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM));
      }
      else if (aDisplayType == DisplayType::cTSD)
      {
         mTSD_YOffset      = 0.0;
         mTSD_OffsetMiddle = false;
         mTSD_OffsetBottom = false;
         mSettings[aDisplayType] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE));
         mSettings[aDisplayType] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM));
      }
   }
   UpdatePriority();
   Update();
}

void wkf::AcesDisplay::Overlay::SetOffsetPosition(const DisplayType aDisplayType)
{
   if (mDisplaySync)
   {
      mSAD_YOffset      = -mSAD_CompassRadius * 0.5;
      mTSD_YOffset      = -mTSD_CompassRadius * 0.5;
      mSAD_OffsetMiddle = mTSD_OffsetMiddle = true;
      mSAD_OffsetBottom = mTSD_OffsetBottom = false;
      mSettings[DisplayType::cSAD] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER));
      mSettings[DisplayType::cSAD] |= static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE);
      mSettings[DisplayType::cSAD] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM));
      mSettings[DisplayType::cTSD] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER));
      mSettings[DisplayType::cTSD] |= static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE);
      mSettings[DisplayType::cTSD] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM));
   }
   else
   {
      if (aDisplayType == DisplayType::cSAD)
      {
         mSAD_YOffset      = -mSAD_CompassRadius * 0.5;
         mSAD_OffsetMiddle = true;
         mSAD_OffsetBottom = false;
         mSettings[aDisplayType] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER));
         mSettings[aDisplayType] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM));
      }
      else if (aDisplayType == DisplayType::cTSD)
      {
         mTSD_YOffset      = -mTSD_CompassRadius * 0.5;
         mTSD_OffsetMiddle = true;
         mTSD_OffsetBottom = false;
         mSettings[aDisplayType] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER));
         mSettings[aDisplayType] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM));
      }
   }
   ResetCenterPoint();
   UpdatePriority();
   Update();
}

void wkf::AcesDisplay::Overlay::SetBottomPosition(const DisplayType aDisplayType)
{
   if (mDisplaySync)
   {
      mSAD_YOffset      = -mSAD_CompassRadius;
      mTSD_YOffset      = -mTSD_CompassRadius;
      mSAD_OffsetMiddle = mTSD_OffsetMiddle = false;
      mSAD_OffsetBottom = mTSD_OffsetBottom = true;
      mSettings[DisplayType::cSAD] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER));
      mSettings[DisplayType::cSAD] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE));
      mSettings[DisplayType::cSAD] |= static_cast<size_t>(SAD_SETTINGS::cOFFSET_BOTTOM);
      mSettings[DisplayType::cTSD] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER));
      mSettings[DisplayType::cTSD] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE));
      mSettings[DisplayType::cTSD] |= static_cast<size_t>(TSD_SETTINGS::cOFFSET_BOTTOM);
   }
   else
   {
      if (aDisplayType == DisplayType::cSAD)
      {
         mSAD_YOffset      = -mSAD_CompassRadius;
         mSAD_OffsetMiddle = false;
         mSAD_OffsetBottom = true;
         mSettings[aDisplayType] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER));
         mSettings[aDisplayType] &= ~(static_cast<size_t>(SAD_SETTINGS::cOFFSET_MIDDLE));
      }
      else if (aDisplayType == DisplayType::cTSD)
      {
         mTSD_YOffset      = -mTSD_CompassRadius;
         mTSD_OffsetMiddle = false;
         mTSD_OffsetBottom = true;
         mSettings[aDisplayType] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER));
         mSettings[aDisplayType] &= ~(static_cast<size_t>(TSD_SETTINGS::cOFFSET_MIDDLE));
      }
   }
   ResetCenterPoint();
   UpdatePriority();
   Update();
}

void wkf::AcesDisplay::Overlay::SetCenterPoint()
{
   auto drawRegion = GetMouseDrawRegion();

   if (mDisplays[DisplayType::cSAD] == drawRegion)
   {
      mSAD_CenterPoint += CalculateMousePosInNm(mMousePos.x(), mMousePos.y(), DisplayType::cSAD);
      if (mDisplaySync)
      {
         mTSD_CenterPoint = mSAD_CenterPoint;
      }
      SetRadioSetting(DisplayType::cSAD, static_cast<size_t>(SAD_SETTINGS::cOFFSET_CENTER));
   }
   else if (mDisplays[DisplayType::cTSD] == drawRegion)
   {
      mTSD_CenterPoint += CalculateMousePosInNm(mMousePos.x(), mMousePos.y(), DisplayType::cTSD);
      if (mDisplaySync)
      {
         mSAD_CenterPoint = mTSD_CenterPoint;
      }
      SetRadioSetting(DisplayType::cTSD, static_cast<size_t>(TSD_SETTINGS::cOFFSET_CENTER));
   }
   // Refresh the settings windows since the center radio button gets set.
   InitializeLeftSettings();
   InitializeRightSettings();
}

void wkf::AcesDisplay::Overlay::Draw()
{
   double padding = 0.01 * rH();
   // Button width/height
   double bW = 0.05 * pW();
   double bH = 0.023 * pH();

   SetLineWidthMultiplier(STANDARD_LINE_WIDTH);

   osg::ref_ptr<osg::Group> vectorGroup(new osg::Group);
   osg::ref_ptr<osg::Group> compassGroup(new osg::Group);

   DrawBackground(Color::Black);

   // Four circle displays (left, right, top, bottom)
   for (auto& entry : mDisplays)
   {
      DrawDisplay(vectorGroup, entry.first, entry.second);
   }

   // Header regions (top left)
   DrawHeaderEng(vectorGroup, -0.465, 0.44, 0.04);
   DrawHeaderWeight(vectorGroup, -0.4, 0.44, 0.05, 0.08);
   DrawHeaderWeapons(vectorGroup, -0.34, 0.44, 0.05, 0.08);
   DrawHeaderFuel(vectorGroup, -0.27, 0.44, 0.07, 0.08);
   DrawHeaderAltSpeed(vectorGroup, -0.195, 0.44, 0.06, 0.08);

   // Engagement region (top right)
   DrawEngagementRegion(vectorGroup, 0.17, 0.425, 0.65, 0.13);
   DrawPrevButton(vectorGroup, (0.5 * pW()) - (bW * 1.5) - (padding * 2), (0.36 * pH()) - (bH * 0.5) - padding, bW, bH);
   DrawNextButton(vectorGroup, (0.5 * pW()) - (bW * 0.5) - padding, (0.36 * pH()) - (bH * 0.5) - padding, bW, bH);
   DrawEngagementSelectMode(vectorGroup, (0.5 * pW()) - padding, (0.36 * pH()) - (bH * 1.5) - (padding * 2));

   // Platform Selection box (non-functional)
   DrawPlatformSelection(vectorGroup,
                         (-0.5 * pW()) + bW + (padding * 1.5),
                         (0.36 * pH()) - (bH * 0.5) - padding,
                         (bW * 2) + padding,
                         bH);

   auto disabledColor = Color::LightGray;
   disabledColor.set(disabledColor.x(), disabledColor.y(), disabledColor.z(), disabledColor.w() * 0.4);
   DrawPrevButton(vectorGroup,
                  (-0.5 * pW()) + (bW * 0.5) + padding,
                  (0.36 * pH()) - (bH * 1.5) - (padding * 2),
                  bW,
                  bH,
                  disabledColor);
   DrawNextButton(vectorGroup,
                  (-0.5 * pW()) + (bW * 1.5) + (padding * 2),
                  (0.36 * pH()) - (bH * 1.5) - (padding * 2),
                  bW,
                  bH,
                  disabledColor);

   // Sim Time
   DrawTime(vectorGroup, (-0.5 * pW()) + padding, (0.36 * pH()) - (bH * 2.5) - (padding * 2.5));

   // Display sync button in the center
   DrawDisplaySync(vectorGroup, 0.0, -0.075 * pH(), 0.055 * pW(), 0.04 * pH());

   // Settings tabs (left, right, top)
   DrawSettingsTab(vectorGroup, (-0.5 * pW()) + (bH * 0.5), -0.075 * pH(), 0.06 * pW(), 0.03 * pH(), DrawRegion::cLEFT);
   DrawSettingsTab(vectorGroup, (0.5 * pW()) - (bH * 0.5), -0.075 * pH(), 0.06 * pW(), 0.03 * pH(), DrawRegion::cRIGHT);
   DrawSettingsTab(vectorGroup,
                   -(0.125 * pW()) + (0.015 * pW()),
                   (0.36 * pH()) - (0.015 * pH()),
                   0.06 * pW(),
                   0.03 * pH(),
                   DrawRegion::cTOP);

   // Countermeasures (bottom left)
   DrawCountermeasures(vectorGroup, (-0.5 * pW()) + (bW * 0.5) + padding, -(0.36 * pH()), bW, 0.048 * pH());
   DrawSelectedWeapon(vectorGroup, (-0.5 * pW()) + (bW * 0.5) + padding, -(0.4 * pH()) - padding, bW, 0.036 * pH());

   // Platform Comments (bottom right)
   DrawPlatformComment(vectorGroup, (0.5 * pW()) - padding, -(0.45 * pH()), 0.1 * pW(), 0.03 * pH());

   // Behavior history (bottom right)
   DrawBehaviorHistory(vectorGroup, 0.0, -0.485, 1.0, 0.03);

   if (mDrawButtonRegions)
   {
      DrawButtonRegions(vectorGroup);
   }

   // Draw the settings menus if they are visible
   if (mLeftSettings.visible)
   {
      double xPos = (-0.65 + mLeftSettings.pos) * pW();
      DrawLeftSettings(vectorGroup, xPos, -0.075 * pH());
   }

   if (mRightSettings.visible)
   {
      double xPos = (0.65 - mRightSettings.pos) * pW();
      DrawRightSettings(vectorGroup, xPos, -0.075 * pH());
   }

   if (mTopSettings.visible)
   {
      double yPos = (0.625 - mTopSettings.pos) * pH();
      DrawTopSettings(vectorGroup, 0, yPos);
   }

   // Draw entity info if visible, in the correct position
   if (mSAD_ShowingEntityInfo)
   {
      // Position the entity info window based on it's region.
      // If it is on the top or bottom, check if the TSD is
      // occupying left or right, and use the open slot.
      if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT)
      {
         SAD_DrawEntityInfo(vectorGroup, -0.15, -0.38, 0.3, 0.16);
      }
      else if (mDisplays[DisplayType::cSAD] == DrawRegion::cRIGHT)
      {
         SAD_DrawEntityInfo(vectorGroup, 0.15, -0.38, 0.3, 0.16);
      }
      else
      {
         if (mDisplays[DisplayType::cTSD] == DrawRegion::cLEFT)
         {
            SAD_DrawEntityInfo(vectorGroup, 0.15, -0.38, 0.3, 0.16);
         }
         else if (mDisplays[DisplayType::cTSD] == DrawRegion::cRIGHT)
         {
            SAD_DrawEntityInfo(vectorGroup, -0.15, -0.38, 0.3, 0.16);
         }
         // If neither of the displays are left or right, just default to right for SAD
         else
         {
            SAD_DrawEntityInfo(vectorGroup, 0.15, -0.38, 0.3, 0.16);
         }
      }
   }

   // Draw entity info if visible, in the correct position
   if (mTSD_ShowingEntityInfo)
   {
      // Position the entity info window based on it's region.
      // If it is on the top or bottom, check if the SAD is
      // occupying left or right, and use the open slot.
      if (mDisplays[DisplayType::cTSD] == DrawRegion::cLEFT)
      {
         TSD_DrawEntityInfo(vectorGroup, -0.15, -0.38, 0.3, 0.16);
      }
      else if (mDisplays[DisplayType::cTSD] == DrawRegion::cRIGHT)
      {
         TSD_DrawEntityInfo(vectorGroup, 0.15, -0.38, 0.3, 0.16);
      }
      else
      {
         if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT)
         {
            TSD_DrawEntityInfo(vectorGroup, 0.15, -0.38, 0.3, 0.16);
         }
         else if (mDisplays[DisplayType::cSAD] == DrawRegion::cRIGHT)
         {
            TSD_DrawEntityInfo(vectorGroup, -0.15, -0.38, 0.3, 0.16);
         }
         // If neither of the displays are left or right, just default to left for TSD
         else
         {
            TSD_DrawEntityInfo(vectorGroup, -0.15, -0.38, 0.3, 0.16);
         }
      }
   }

   //// Add the finished drawing
   mOverlayGeode->addChild(vectorGroup.get());
   mOverlayGeode->addChild(compassGroup.get());

   if (mConnectionDestroyed)
   {
      DrawConnectionDestroyed();
   }
}

void wkf::AcesDisplay::Overlay::SAD_DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup,
                                                 const double             aX,
                                                 const double             aY,
                                                 const double             aR)
{
   // Transformation - Shift the region to match the compass.
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

   // Create temp asset lists so we can draw them last (on top)
   std::vector<Entity*> mAssetList;
   std::vector<Entity*> mFlightList;

   // Use this for truth entities
   double angleOnlyEntitySize = aR / 15.0; // 0.375 / 15 = 0.025
   double entitySize          = aR / 22.5; // 0.375 / 22.5 = ~0.167;

   // Draw each entity.
   for (auto& entity : mSAD_Entities)
   {
      const QPointF pos = SAD_CalculateEntityPosition(entity);

      // If our entity is a group and we are drawing groups
      if (entity.isGroup)
      {
         if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cGROUPS) &&
             (!(mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPERCEPTION)) ||
              entity.groupPerception.elementList.size() > 1))
         {
            SAD_DrawGroupEntity(vectorXform, entity, pos.x(), pos.y(), aR);
         }
      }
      // If we are a normal perception, and we're drawing perceptions
      else
      {
         if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPERCEPTION))
         {
            switch (entity.perception.identification)
            {
            // Friendlies get drawn after everything else to make them easier to see in clutter.
            case Identification::ID_FRIENDLY:
            {
               if (entity.perception.flightId == mData.platformData.flightId)
               {
                  if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cFLIGHT))
                  {
                     mFlightList.push_back(&entity);
                  }
                  break;
               }

               if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cFRIENDLIES))
               {
                  mAssetList.push_back(&entity);
               }
               break;
            }
            case Identification::ID_BANDIT:
            case Identification::ID_BOGIE:
            case Identification::ID_NEUTRAL:
            case Identification::ID_UNKNOWN:
            {
               if (entity.perception.focus)
               {
                  if (entity.perception.angleOnly)
                  {
                     if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cANGLEONLY))
                     {
                        SAD_DrawAngleOnlyEntity(aVectorGroup, entity, angleOnlyEntitySize, angleOnlyEntitySize, aR);
                     }
                  }
                  else
                  {
                     SAD_DrawEntity(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2, aR);
                  }
               }
               break;
            }
            default:
               break;
            }
         }
      }
   }

   // Draw the assets after bogies/bandits/unknown/neutral so we can see them in clutter easier.
   for (auto& entity : mAssetList)
   {
      const QPointF pos = SAD_CalculateEntityPosition(*entity);
      if (entity->perception.angleOnly)
      {
         if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cANGLEONLY))
         {
            SAD_DrawAngleOnlyEntity(aVectorGroup, *entity, angleOnlyEntitySize, angleOnlyEntitySize, aR);
         }
      }
      else if (!entity->perception.idFlag.empty())
      {
         SAD_DrawFlagFlightEntity(vectorXform, *entity, pos.x(), pos.y(), entitySize, entitySize * 2, aR, false);
      }
      else
      {
         SAD_DrawEntity(vectorXform, *entity, pos.x(), pos.y(), entitySize, entitySize * 2, aR);
      }
   }

   // Draw flights after assets
   for (auto& entity : mFlightList)
   {
      const QPointF pos = SAD_CalculateEntityPosition(*entity);
      SAD_DrawFlagFlightEntity(vectorXform, *entity, pos.x(), pos.y(), entitySize, entitySize * 2, aR, true);
   }

   // Draw priority threat and target numbers
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPERCEPTION))
   {
      if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPRIORITYTARGETS))
      {
         for (size_t i = 0; i < mData.prioritizedTargetsList.size(); i++)
         {
            Entity e;
            e.perception      = mData.prioritizedTargetsList[i];
            const QPointF pos = SAD_CalculateEntityPosition(e);
            if (e.perception.focus)
            {
               if (e.perception.angleOnly)
               {
                  if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cANGLEONLY))
                  {
                     SAD_DrawAngleOnlyPriorityTarget(aVectorGroup, e, i + 1, angleOnlyEntitySize, angleOnlyEntitySize, aR);
                  }
               }
               else
               {
                  SAD_DrawEntityPriorityTargetThreatNumber(vectorXform,
                                                           e,
                                                           i + 1,
                                                           pos.x(),
                                                           pos.y(),
                                                           entitySize,
                                                           entitySize * 2,
                                                           aR,
                                                           false);
               }
            }
         }
      }

      if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cPRIORITYTHREATS))
      {
         for (size_t i = 0; i < mData.prioritizedThreatsList.size(); i++)
         {
            Entity e;
            e.perception      = mData.prioritizedThreatsList[i];
            const QPointF pos = SAD_CalculateEntityPosition(e);
            if (e.perception.focus)
            {
               if (e.perception.angleOnly)
               {
                  if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cANGLEONLY))
                  {
                     SAD_DrawAngleOnlyPriorityThreat(aVectorGroup, e, i + 1, angleOnlyEntitySize, angleOnlyEntitySize, aR);
                  }
               }
               else
               {
                  SAD_DrawEntityPriorityTargetThreatNumber(vectorXform,
                                                           e,
                                                           i + 1,
                                                           pos.x(),
                                                           pos.y(),
                                                           entitySize,
                                                           entitySize * 2,
                                                           aR,
                                                           true);
               }
            }
         }
      }
   }

   // Draw truth data if it is visible.
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cTRUTHAIR) ||
       mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cTRUTHGROUND))
   {
      double truthEntitySize = aR / 18.0; // 0.375 / 18.0 = ~0.021

      for (auto& entity : mSAD_TruthEntities)
      {
         if (entity.truth.entityValid)
         {
            // If we have a ground truth entity and we are drawing ground truth
            if (entity.truth.domain == "land")
            {
               if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cTRUTHGROUND))
               {
                  const QPointF pos = SAD_CalculateEntityPosition(entity);
                  SAD_DrawTruthEntity(vectorXform, entity, pos.x(), pos.y(), truthEntitySize, truthEntitySize);
               }
            }
            else
            {
               // If we have a air truth entity and we are drawing air truth
               if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cTRUTHAIR))
               {
                  const QPointF pos = SAD_CalculateEntityPosition(entity);
                  SAD_DrawTruthEntity(vectorXform, entity, pos.x(), pos.y(), truthEntitySize, truthEntitySize);
               }
            }
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::SAD_DrawTruthEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                    Entity&                  aEntity,
                                                    const double             aX,
                                                    const double             aY,
                                                    const double             aW,
                                                    const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = SAD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);
   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   // Land domain gets drawn as a square
   if (aEntity.truth.domain == "land")
   {
      entityArray->push_back(osg::Vec2d(-aW * 0.3 * rW(), aH * 0.3 * rH()));
      entityArray->push_back(osg::Vec2d(aW * 0.3 * rW(), aH * 0.3 * rH()));
      entityArray->push_back(osg::Vec2d(aW * 0.3 * rW(), -aH * 0.3 * rH()));
      entityArray->push_back(osg::Vec2d(-aW * 0.3 * rW(), -aH * 0.3 * rH()));
      vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, Color::Magenta).get());
   }
   // Otherwise draw an arrow
   else
   {
      osg::ref_ptr<osg::PositionAttitudeTransform> headingXform(new osg::PositionAttitudeTransform());
      headingXform->setAttitude(osg::Quat((aEntity.truth.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                          osg::Vec3(0.0f, 0.0f, -1.0f)));
      entityArray->push_back(osg::Vec2d(-aW * 0.3 * rW(), -aH * 0.5 * rH()));
      entityArray->push_back(osg::Vec2d(0.0, aH * 0.5 * rH()));
      entityArray->push_back(osg::Vec2d(aW * 0.3 * rW(), -aH * 0.5 * rH()));
      entityArray->push_back(osg::Vec2d(0.0, -aH * 0.35 * rH()));
      headingXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, Color::Magenta).get());
      vectorXform->addChild(headingXform.get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::SAD_DrawEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                               Entity&                  aEntity,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH,
                                               const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = SAD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);
   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // Get our entity draw type and color.
   osg::Vec4 entityColor = GetEntityColor(aEntity.perception.identification);

   if (aEntity.perception.isCoasting)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }

   // Get the bottom and top half of the entity and draw it.
   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   auto                         entityDrawable = SAD_GetEntityDrawable(aEntity, aW, aH);
   rotVectorXform->addChild(entityDrawable.get());

   // Draw altitude if visible and valid
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cALTITUDE))
   {
      SAD_DrawAltitudeText(aEntity, aW, entityColor, vectorXform, aR);
   }
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cVISIBILITY))
   {
      SAD_DrawVisualIndicator(aEntity, aW, entityColor, vectorXform, aR);
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);
}

void wkf::AcesDisplay::Overlay::SAD_DrawAltitudeText(Entity&                                      aEntity,
                                                     const double                                 aW,
                                                     osg::Vec4                                    aEntityColor,
                                                     osg::ref_ptr<osg::PositionAttitudeTransform> aXform,
                                                     const double                                 aR)
{
   if (aEntity.perception.altitudeValid)
   {
      float fontSize = 0.04f * aR * rW();
      // Altitude (in thousands of ft)
      std::stringstream alt;
      alt << static_cast<int>(aEntity.perception.altitude_ft) / 1000;
      aXform->addChild(
         AddTextItem(alt.str(), (aW * 0.8) * rW(), 0.0, fontSize, aEntityColor, osgText::Text::LEFT_CENTER).get());
   }
}

void wkf::AcesDisplay::Overlay::SAD_DrawVisualIndicator(Entity&                                      aEntity,
                                                        const double                                 aW,
                                                        osg::Vec4                                    aEntityColor,
                                                        osg::ref_ptr<osg::PositionAttitudeTransform> aXform,
                                                        const double                                 aR)
{
   // First, we need to check that this perception is based on the pilot's vision.
   wkf::AcesDisplay::TrackProcessor* eyes{nullptr};
   for (auto& processor : mData.trackProcessorList)
   {
      if (processor.trackProcessorName == mData.eyesTrackprocessor)
      {
         eyes = &processor;
         break;
      }
   }
   if (eyes == nullptr)
   {
      return;
   }
   bool visual = false;
   for (auto& track : eyes->trackList.mTracks)
   {
      if (track.second.tgtPlatformIndex == aEntity.perception.perceivedIndex)
      {
         visual = true;
         break;
      }
   }
   // If it is, then draw an eyeball to the left of the perception icon.
   if (visual)
   {
      double iconRadius = aW * 0.20;
      // The iris
      osg::ref_ptr<osg::Vec2Array> irisArray(new osg::Vec2Array);
      for (int ang_deg = 0; ang_deg < 360; ang_deg += 18)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = iconRadius * cos(ang_rad) * 0.8;
         double y       = iconRadius * sin(ang_rad) * 0.8;
         irisArray->push_back(osg::Vec2d((x - aW) * rW(), y * rH()));
      }
      // The outline
      osg::ref_ptr<osg::Vec2Array> outlineArray(new osg::Vec2Array);
      for (int ang_deg = 30; ang_deg < 150; ang_deg += 12)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = iconRadius * cos(ang_rad) * 2;
         double y       = iconRadius * sin(ang_rad) * 2;
         outlineArray->push_back(osg::Vec2d((x - aW) * rW(), (y - iconRadius) * rH()));
      }
      for (int ang_deg = 210; ang_deg < 330; ang_deg += 12)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = iconRadius * cos(ang_rad) * 2;
         double y       = iconRadius * sin(ang_rad) * 2;
         outlineArray->push_back(osg::Vec2d((x - aW) * rW(), (y + iconRadius) * rH()));
      }

      aXform->addChild(CreateGeometry(irisArray, GL_POLYGON, aEntityColor));
      aXform->addChild(CreateGeometry(outlineArray, GL_LINE_LOOP, aEntityColor));
   }
}

void wkf::AcesDisplay::Overlay::SAD_DrawEntityPriorityTargetThreatNumber(osg::ref_ptr<osg::Group> aVectorGroup,
                                                                         Entity&                  aEntity,
                                                                         const size_t             aPriorityNumber,
                                                                         const double             aX,
                                                                         const double             aY,
                                                                         const double             aW,
                                                                         const double             aH,
                                                                         const double             aR,
                                                                         bool                     aIsThreat)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));

   osg::Vec4 entityColor = GetEntityColor(aEntity.perception.identification);

   if (aEntity.perception.isCoasting)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }

   float fontSize = 0.03f * aR * rW();
   if (aIsThreat)
   {
      vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                        -aW * 0.8 * rW(),
                                        aH * 0.25 * rH(),
                                        fontSize,
                                        entityColor,
                                        osgText::Text::RIGHT_BOTTOM)
                               .get());
   }
   else
   {
      vectorXform->addChild(AddTextItem(std::to_string(aPriorityNumber),
                                        -aW * 0.8 * rW(),
                                        -aH * 0.25 * rH(),
                                        fontSize,
                                        entityColor,
                                        osgText::Text::RIGHT_TOP)
                               .get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::SAD_DrawFlagFlightEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                         Entity&                  aEntity,
                                                         const double             aX,
                                                         const double             aY,
                                                         const double             aW,
                                                         const double             aH,
                                                         const double             aR,
                                                         const bool               aIsFlight)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));
   osg::ref_ptr<osg::PositionAttitudeTransform> rotVectorXform(new osg::PositionAttitudeTransform());
   rotVectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.perception.headingValid)
   {
      rotVectorXform->setAttitude(
         osg::Quat((aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = SAD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);
   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // Set the color to slate blue for flight entities, green otherwise.
   osg::Vec4 entityColor = aIsFlight ? Color::SlateBlue : Color::Green;

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
         circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
      rotVectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, entityColor).get());

      // Radar dish and heading lines
      osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

      // Heading line
      radarArray->push_back(osg::Vec2d(0.0, 0.0));
      radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

      // Radar dish
      for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
      {
         double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
         double x       = r * cos(ang_rad);
         double y       = r + r * sin(ang_rad);
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         // Add every element but the first and last twice for GL_LINES
         if (ang_deg != 180 && ang_deg != 360)
         {
            radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         }
      }
      rotVectorXform->addChild(CreateGeometry(radarArray, GL_LINES, entityColor).get());
   }

   // Draw altitude if visible and valid
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cALTITUDE))
   {
      SAD_DrawAltitudeText(aEntity, aW, entityColor, vectorXform, aR);
   }

   // Text in center of circle
   float       fontSize = 0.04f * aR * rW();
   std::string flag     = aEntity.perception.idFlag.substr(0, 1); // Limit string to a single

   double x = 0.0;
   double y = 0.0;
   double r = aW * 0.5;
   if (aEntity.perception.headingValid)
   {
      double deltaAngle_rad = (aEntity.perception.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG;
      x                     = -r * sin(deltaAngle_rad) * rW();
      y                     = -r * cos(deltaAngle_rad) * rH();
      vectorXform->addChild(AddTextItem(flag, x, y, fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }
   else
   {
      vectorXform->addChild(AddTextItem(flag, 0.0, (-r * rH()), fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
   rotVectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(rotVectorXform);
}

void wkf::AcesDisplay::Overlay::SAD_DrawGroupEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                    Entity&                  aEntity,
                                                    const double             aX,
                                                    const double             aY,
                                                    const double             aR)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * SAD_UnitsPerNm() * rW(), aY * SAD_UnitsPerNm() * rH(), 0.0));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = SAD_CalculateEntityHoverRegion(aEntity, aX, aY, 0.03, 0.03);
   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // If all elements of a group are bandits draw red.  Otherwise, draw yellow.
   osg::Vec4 entityColor = Color::Red;
   for (auto& entity : aEntity.groupPerception.elementList)
   {
      if (entity.identification != Identification::ID_BANDIT)
      {
         entityColor = Color::Yellow;
         break;
      }
   }

   // If the group is coasting, draw at reduced alpha.
   {
      bool isCoasting = true;
      // A group is only coasting if all of the entities it contains are coasting.
      for (auto& element : aEntity.groupPerception.elementList)
      {
         if (!element.isCoasting)
         {
            isCoasting = false;
         }
      }
      if (isCoasting)
      {
         entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
      }
   }

   // Calculate the distance in coordinate space per nm, then multiply that times the range radius.
   double range = SAD_UnitsPerNm() * aEntity.groupPerception.radiusCurrent_nm;

   // Range ring
   osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = range * cos(ang_rad);
      double y       = range * sin(ang_rad);
      ringArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }

   float fontSize = 0.04f * aR * rW();
   vectorXform->addChild(AddTextItem(aEntity.groupPerception.groupName,
                                     range * cos(45 * UtMath::cRAD_PER_DEG) * rW(),
                                     range * sin(45 * UtMath::cRAD_PER_DEG) * rH(),
                                     fontSize,
                                     entityColor,
                                     osgText::Text::LEFT_BOTTOM)
                            .get());

   // Highlight if important
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cIMPORTANCE) && aEntity.groupPerception.isImportant)
   {
      SetLineWidthMultiplier(HIGHLIGHT_WIDTH);
      vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, HIGHLIGHT_COLOR).get());
      SetLineWidthMultiplier(STANDARD_LINE_WIDTH);
   }

   vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, entityColor).get());

   // Add 20% alpha fill and number of elements if unfocused
   if (!aEntity.groupPerception.focus)
   {
      vectorXform->addChild(
         CreateGeometry(ringArray, GL_POLYGON, osg::Vec4(entityColor.r(), entityColor.g(), entityColor.b(), 0.2f)));
      std::stringstream size;
      size << aEntity.groupPerception.elementList.size();
      vectorXform->addChild(AddTextItem(size.str(), 0.0, 0.0, fontSize, entityColor, osgText::Text::CENTER_CENTER).get());
   }
   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::SAD_DrawESM_RWR_Entity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                       Entity&                  aEntity,
                                                       const double             aW,
                                                       const double             aH)
{
   osg::Vec4 entityColor = GetEntityColor(aEntity.perception.identification);

   // Calculate the position and orientation of the entity
   double az_rad =
      NormalizeRadians((aEntity.perception.bearing_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

   double tgtX = (mSAD_CompassRadius - (aH * 0.75)) * sin(az_rad);
   double tgtY = (mSAD_CompassRadius - (aH * 0.75)) * cos(az_rad);

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = SAD_CalculateEntityHoverRegion(aEntity,
                                                           tgtX * mSADRange_nm / mSAD_CompassRadius,
                                                           tgtY * mSADRange_nm / mSAD_CompassRadius,
                                                           aW,
                                                           aH);
   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
   }

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));
   vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

   SetLineWidthMultiplier(4.5f);
   // Entity triangle
   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
   entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
   entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(entityArray, GL_LINE_LOOP, entityColor).get());
   SetLineWidthMultiplier(STANDARD_LINE_WIDTH);

   float fontSize = 0.0125 * pW();
   vectorXform->addChild(
      AddTextItem(aEntity.perception.idString.substr(0, 2), 0.0, (aH * 0.2) * rH(), fontSize, entityColor, osgText::Text::CENTER_CENTER)
         .get());

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::SAD_DrawAngleOnlyEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                                        Entity&                  aEntity,
                                                        const double             aW,
                                                        const double             aH,
                                                        const double             aR)
{
   osg::Vec4                                    entityColor = GetEntityColor(aEntity.perception.identification);
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   double                                       angleOnlyDist  = aR + (aH * 0.75);
   bool                                         drawDebugLines = false;

   if (aEntity.perception.isCoasting)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }

   // If we're centered, draw as normal.
   if (mSAD_CenterPoint.x() == 0.0 && mSAD_CenterPoint.y() == 0.0 && mSAD_YOffset == 0.0)
   {
      // Calculate the position and orientation of the entity
      double az_rad =
         NormalizeRadians((aEntity.perception.bearing_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(az_rad);
      double tgtY = angleOnlyDist * cos(az_rad);

      // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
      const QRectF hoverRect =
         SAD_CalculateEntityHoverRegion(aEntity, tgtX * mSADRange_nm / aR, tgtY * mSADRange_nm / aR, aW, aH);
      // If we're drawing the hover regions to see where they are.
      if (mDrawHoverRegions)
      {
         // Hover region outline
         osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
         hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), (hoverRect.topLeft().y()) * rH()));
         hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), (hoverRect.topRight().y()) * rH()));
         hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), (hoverRect.bottomRight().y()) * rH()));
         hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), (hoverRect.bottomLeft().y()) * rH()));
         aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
      }

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));
      vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      // Draw the entity
      vectorXform->addChild(
         CreateGeometry(SAD_GetAngleOnlyEntityDrawArray(aEntity, aW, aH), GL_LINE_LOOP, entityColor).get());
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
      line.translate(-mSAD_CenterPoint);

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
      double R = mSADRange_nm;
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
      double compassAngle_rad = NormalizeRadians((c2ang - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(compassAngle_rad);
      double tgtY = angleOnlyDist * cos(compassAngle_rad); //(angleOnlyDist + mSAD_YOffset) * cos(compassAngle_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

      QLineF ownshipToAngleOnlyEntity = QLineF(QPointF(tgtX, tgtY), -mSAD_CenterPoint * SAD_UnitsPerNm());
      // Calculate the orientation of the angle only entity (point towards ownship)
      double az_rad =
         NormalizeRadians(((ownshipToAngleOnlyEntity.angle()) - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      osg::ref_ptr<osg::PositionAttitudeTransform> vectorX2form(new osg::PositionAttitudeTransform());
      vectorX2form->setAttitude(osg::Quat(compassAngle_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
      const QRectF hoverRect =
         SAD_CalculateEntityHoverRegion(aEntity,
                                        (tgtX / SAD_UnitsPerNm()) + mSAD_CenterPoint.x(),
                                        ((tgtY - mSAD_YOffset) / SAD_UnitsPerNm()) + mSAD_CenterPoint.y(),
                                        aW,
                                        aH);
      // If we're drawing the hover regions to see where they are.
      if (mDrawHoverRegions)
      {
         QRectF rect = hoverRect;
         rect.translate(-mSAD_CenterPoint * SAD_UnitsPerNm());
         // Hover region outline
         osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
         hoverRegionArray->push_back(osg::Vec2d(rect.topLeft().x() * rW(), (rect.topLeft().y() + mSAD_YOffset) * rH()));
         hoverRegionArray->push_back(osg::Vec2d(rect.topRight().x() * rW(), (rect.topRight().y() + mSAD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(rect.bottomRight().x() * rW(), (rect.bottomRight().y() + mSAD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(rect.bottomLeft().x() * rW(), (rect.bottomLeft().y() + mSAD_YOffset) * rH()));
         aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
      }

      if (drawDebugLines)
      {
         osg::ref_ptr<osg::Vec2Array> a123Array(new osg::Vec2Array);
         a123Array->push_back(osg::Vec2d(ownshipToAngleOnlyEntity.x1() * rW(), ownshipToAngleOnlyEntity.y1() * rH()));
         a123Array->push_back(osg::Vec2d(ownshipToAngleOnlyEntity.x2() * rW(), ownshipToAngleOnlyEntity.y2() * rH()));
         aVectorGroup->addChild(CreateGeometry(a123Array, GL_LINES, Color::Yellow).get());

         osg::ref_ptr<osg::Vec2Array> lineArray(new osg::Vec2Array);
         lineArray->push_back(osg::Vec2d(line.x1() * SAD_UnitsPerNm() * rW(), line.y1() * SAD_UnitsPerNm() * rH()));
         lineArray->push_back(osg::Vec2d(line.x2() * SAD_UnitsPerNm() * rW(), line.y2() * SAD_UnitsPerNm() * rH()));
         aVectorGroup->addChild(CreateGeometry(lineArray, GL_LINES, Color::Red).get());

         QLineF                       l = QLineF::fromPolar(1000, -90);
         osg::ref_ptr<osg::Vec2Array> line4Array(new osg::Vec2Array);
         line4Array->push_back(osg::Vec2d(l.x1() * SAD_UnitsPerNm() * rW(), l.y1() * SAD_UnitsPerNm() * rH()));
         line4Array->push_back(osg::Vec2d(l.x2() * SAD_UnitsPerNm() * rW(), l.y2() * SAD_UnitsPerNm() * rH()));
         vectorX2form->addChild(CreateGeometry(line4Array, GL_LINES, Color::Green).get());
         aVectorGroup->addChild(vectorX2form.get());
      }

      if (collision)
      {
         // Draw the entity
         vectorXform->addChild(
            CreateGeometry(SAD_GetAngleOnlyEntityDrawArray(aEntity, aW, aH), GL_LINE_LOOP, entityColor).get());
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

osg::ref_ptr<osg::Vec2Array> wkf::AcesDisplay::Overlay::SAD_GetAngleOnlyEntityDrawArray(Entity&      aEntity,
                                                                                        const double aW,
                                                                                        const double aH)
{
   osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
   if (aEntity.perception.isMissile)
   {
      // Entity Diamond
      entityArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), 0.0));
      entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), 0.0));
   }
   else
   {
      switch (aEntity.perception.identification)
      {
      case Identification::ID_BANDIT:
      {
         // Entity triangle
         entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
         break;
      }
      case Identification::ID_FRIENDLY:
      {
         // Entity circle
         double r = (aW * 0.5);
         for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
         {
            double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
            double x       = r * cos(ang_rad);
            double y       = r * sin(ang_rad);
            entityArray->push_back(osg::Vec2d(x * rW(), y * rH()));
         }
         break;
      }
      default: // ID_BOGIE, ID_NEUTRAL, ID_UNKNOWN
      {
         // Entity square
         entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), (aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), (aH * 0.5) * rH()));
         break;
      }
      }
   }
   return entityArray;
}

bool wkf::AcesDisplay::Overlay::MouseWheel(int aMouseX, int aMouseY, int aDelta)
{
   if (mHandleEventMode != vespa::VaOverlay::cHANDLE_EVENTS_NEVER)
   {
      auto change = aDelta / 24;
      if (mDisplaySync)
      {
         // aDelta comes in increments of 120 for some reason.
         // We want to scroll in increments of 5 nm, so just divide by 24
         auto newRange = static_cast<int>(mTSDRange_nm) + change;
         mTSDRange_nm = mSADRange_nm = newRange <= 5 ? 5 : newRange;
      }
      else
      {
         // If the mouse x is on the left side of the screen, scroll the left region display;
         // otherwise, scroll the right region display
         if (aMouseX <= (0.5 * pW()))
         {
            if (mDisplays[DisplayType::cSAD] == DrawRegion::cLEFT)
            {
               auto newRange = static_cast<int>(mSADRange_nm) + change;
               mSADRange_nm  = newRange <= 5 ? 5 : newRange;
            }
            else if (mDisplays[DisplayType::cTSD] == DrawRegion::cLEFT)
            {
               auto newRange = static_cast<int>(mTSDRange_nm) + change;
               mTSDRange_nm  = newRange <= 5 ? 5 : newRange;
            }
         }
         else
         {
            if (mDisplays[DisplayType::cSAD] == DrawRegion::cRIGHT)
            {
               auto newRange = static_cast<int>(mSADRange_nm) + change;
               mSADRange_nm  = newRange <= 5 ? 5 : newRange;
            }
            else if (mDisplays[DisplayType::cTSD] == DrawRegion::cRIGHT)
            {
               auto newRange = static_cast<int>(mTSDRange_nm) + change;
               mTSDRange_nm  = newRange <= 5 ? 5 : newRange;
            }
         }
      }
      return true;
   }
   else
   {
      return false;
   }
}

void wkf::AcesDisplay::Overlay::SAD_DrawAngleOnlyPriorityTarget(osg::ref_ptr<osg::Group> aVectorGroup,
                                                                Entity&                  aEntity,
                                                                const size_t             aPriority,
                                                                const double             aW,
                                                                const double             aH,
                                                                const double             aR)
{
   osg::Vec4                                    entityColor = GetEntityColor(aEntity.perception.identification);
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   double                                       angleOnlyDist = aR + (aH * 0.75);
   float                                        fontSize      = 0.04f * aR * rW();

   if (aEntity.perception.isCoasting)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }

   // If we're centered, draw as normal.
   if (mSAD_CenterPoint.x() == 0.0 && mSAD_CenterPoint.y() == 0.0 && mSAD_YOffset == 0.0)
   {
      // Calculate the position and orientation of the entity
      double az_rad =
         NormalizeRadians((aEntity.perception.bearing_deg - mData.platformData.heading_deg - 1) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(az_rad);
      double tgtY = angleOnlyDist * cos(az_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

      // Draw the priority target number
      vectorXform->addChild(
         AddTextItem(std::to_string(aPriority), -aW * 0.25 * rW(), -aH * 0.25 * rH(), fontSize, entityColor, osgText::Text::RIGHT_TOP)
            .get());
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
      line.translate(-mSAD_CenterPoint);

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
      double R = mSADRange_nm;
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
      double compassAngle_rad = NormalizeRadians((c2ang - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(compassAngle_rad);
      double tgtY = angleOnlyDist * cos(compassAngle_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

      if (collision)
      {
         // Draw the priority target number
         double compassAngle_deg = compassAngle_rad * UtMath::cDEG_PER_RAD;
         if (compassAngle_deg > 0 && compassAngle_deg <= 180)
         {
            vectorXform->addChild(AddTextItem(std::to_string(aPriority),
                                              aW * 0.75 * rW(),
                                              -aH * 0.25 * rH(),
                                              fontSize,
                                              entityColor,
                                              osgText::Text::LEFT_TOP)
                                     .get());
         }
         else
         {
            vectorXform->addChild(AddTextItem(std::to_string(aPriority),
                                              -aW * 0.75 * rW(),
                                              -aH * 0.25 * rH(),
                                              fontSize,
                                              entityColor,
                                              osgText::Text::RIGHT_TOP)
                                     .get());
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::SAD_DrawAngleOnlyPriorityThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                                                                Entity&                  aEntity,
                                                                const size_t             aPriority,
                                                                const double             aW,
                                                                const double             aH,
                                                                const double             aR)
{
   osg::Vec4                                    entityColor = GetEntityColor(aEntity.perception.identification);
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   double                                       angleOnlyDist = aR + (aH * 0.75);
   float                                        fontSize      = 0.04f * aR * rW();

   if (aEntity.perception.isCoasting)
   {
      entityColor.set(entityColor.x(), entityColor.y(), entityColor.z(), 0.4);
   }

   // If we're centered, draw as normal.
   if (mSAD_CenterPoint.x() == 0.0 && mSAD_CenterPoint.y() == 0.0 && mSAD_YOffset == 0.0)
   {
      // Calculate the position and orientation of the entity
      double az_rad =
         NormalizeRadians((aEntity.perception.bearing_deg - mData.platformData.heading_deg - 1) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(az_rad);
      double tgtY = angleOnlyDist * cos(az_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

      // Draw the priority threat number
      vectorXform->addChild(
         AddTextItem(std::to_string(aPriority), -aW * 0.25 * rW(), aH * 0.25 * rH(), fontSize, entityColor, osgText::Text::RIGHT_BOTTOM)
            .get());
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
      line.translate(-mSAD_CenterPoint);

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
      double R = mSADRange_nm;
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
      double compassAngle_rad = NormalizeRadians((c2ang - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(compassAngle_rad);
      double tgtY = angleOnlyDist * cos(compassAngle_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

      if (collision)
      {
         // Draw the priority target number
         double compassAngle_deg = compassAngle_rad * UtMath::cDEG_PER_RAD;
         if (compassAngle_deg > 0 && compassAngle_deg <= 180)
         {
            vectorXform->addChild(AddTextItem(std::to_string(aPriority),
                                              aW * 0.75 * rW(),
                                              aH * 0.25 * rH(),
                                              fontSize,
                                              entityColor,
                                              osgText::Text::LEFT_BOTTOM)
                                     .get());
         }
         else
         {
            vectorXform->addChild(AddTextItem(std::to_string(aPriority),
                                              -aW * 0.75 * rW(),
                                              aH * 0.25 * rH(),
                                              fontSize,
                                              entityColor,
                                              osgText::Text::RIGHT_BOTTOM)
                                     .get());
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::HandleClickRegions(const int aMouseX, const int aMouseY)
{
   // Set the newest mouse position
   mMousePos.setX(aMouseX);
   mMousePos.setY(aMouseY);

   for (auto& button : mButtons)
   {
      if (button.clickable)
      {
         // Get the pixel region of the button region:
         // Fix the position, shift to the center of the screen (+0.5, +0.5), and then subtract 0.5w and 0.5h.  Finish
         // buy scaling by pW and pH
         QRectF pRegion = QRectF((0.5 + button.region.x() - (button.region.width() * 0.5)) * pW(),
                                 (0.5 + button.region.y() - (button.region.height() * 0.5)) * pH(),
                                 button.region.width() * pW(),
                                 button.region.height() * pH());
         if (pRegion.contains(mMousePos))
         {
            button.function();
         }
      }
   }

   for (auto& button : mSettingsButtons)
   {
      if (button.second.clickable)
      {
         // Get the pixel region of the button region:
         // Fix the position, shift to the center of the screen (+0.5, +0.5), and then subtract 0.5w and 0.5h.  Finish
         // buy scaling by pW and pH
         QRectF pRegion = QRectF((0.5 + button.second.region.x() - (button.second.region.width() * 0.5)) * pW(),
                                 (0.5 + button.second.region.y() - (button.second.region.height() * 0.5)) * pH(),
                                 button.second.region.width() * pW(),
                                 button.second.region.height() * pH());
         if (pRegion.contains(mMousePos))
         {
            button.second.function();
         }
      }
   }

   if (mLeftSettings.visible && (mLeftSettings.menuState == MenuState::cOPEN))
   {
      double xOffset = -0.35 * pW();
      double yOffset = -0.075 * pH();
      for (auto& button : mLeftSettings.buttons)
      {
         // Get the pixel region of the button region:
         // Fix the position, shift to the center of the screen (+0.5, +0.5), and then subtract 0.5w and 0.5h.  Finish
         // buy scaling by pW and pH
         QRectF pRegion = QRectF((0.5 * pW()) + xOffset + button.region.x() - (button.region.width() * 0.5),
                                 (0.5 * pH()) + yOffset + button.region.y() - (button.region.height() * 0.5),
                                 button.region.width(),
                                 button.region.height());
         if (pRegion.contains(mMousePos))
         {
            button.function();
            InitializeLeftSettings();
            InitializeRightSettings();
         }
      }
   }

   if (mRightSettings.visible && (mRightSettings.menuState == MenuState::cOPEN))
   {
      double xOffset = 0.35 * pW();
      double yOffset = -0.075 * pH();
      for (auto& button : mRightSettings.buttons)
      {
         // Get the pixel region of the button region:
         // Fix the position, shift to the center of the screen (+0.5, +0.5), and then subtract 0.5w and 0.5h.  Finish
         // buy scaling by pW and pH
         QRectF pRegion = QRectF((0.5 * pW()) + xOffset + button.region.x() - (button.region.width() * 0.5),
                                 (0.5 * pH()) + yOffset + button.region.y() - (button.region.height() * 0.5),
                                 button.region.width(),
                                 button.region.height());
         if (pRegion.contains(mMousePos))
         {
            button.function();
            InitializeRightSettings();
            InitializeLeftSettings();
         }
      }
   }

   if (mTopSettings.visible && (mTopSettings.menuState == MenuState::cOPEN))
   {
      double xOffset = 0.0 * pW();
      double yOffset = 0.375 * pH();
      for (auto& button : mTopSettings.buttons)
      {
         // Get the pixel region of the button region:
         // Fix the position, shift to the center of the screen (+0.5, +0.5), and then subtract 0.5w and 0.5h.  Finish
         // buy scaling by pW and pH
         QRectF pRegion = QRectF((0.5 * pW()) + xOffset + button.region.x() - (button.region.width() * 0.5),
                                 (0.5 * pH()) + yOffset + button.region.y() - (button.region.height() * 0.5),
                                 button.region.width(),
                                 button.region.height());
         if (pRegion.contains(mMousePos))
         {
            button.function();
            InitializeTopSettings();
         }
      }
   }

   if (!(mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cHOVER)))
   {
      // Loop through each entity and check if we're mousing over it.  Multiple entities hover regions may be hovered
      // over at the same time, so check each entity and set the current entity to the one closest to the mouse.
      mSAD_ShowingEntityInfo = false; // Set this to true if any entity is being moused over.
      mSAD_EntityClicked     = false;
      auto drawVector        = GetDrawVector(mDisplays[DisplayType::cSAD]);

      std::vector<Entity> sadEntities;
      for (auto& e : mSAD_Entities)
      {
         sadEntities.push_back(e);
      }
      for (auto& e : mSAD_TruthEntities)
      {
         sadEntities.push_back(e);
      }

      for (auto& entity : sadEntities)
      {
         // The center rect needs to be shifted to the center of the compass.
         QPointF compassCenter =
            QPointF(mRect.center().x() + (drawVector.x() * pW()), mRect.center().y() + (drawVector.y() * pH()));
         QPointF rectFromCenter = QPointF((entity.hoverRegion.center().x() - compassCenter.x()) / pW(),
                                          ((entity.hoverRegion.center().y() - compassCenter.y()) / pH()) - mSAD_YOffset);
         double distFromCenter = sqrt(rectFromCenter.x() * rectFromCenter.x() + rectFromCenter.y() * rectFromCenter.y());
         // Don't hover over entities drawn behind the stencil buffer.
         // If we are within hover range, evaluate the entity.
         if (distFromCenter <= mSAD_CompassRadius || entity.perception.angleOnly)
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               mSAD_EntityClicked = true;
               // Compare this entity to the current one.  If the newer distance is less than or equal to the current
               // one update the current entity.
               if (mSAD_ShowingEntityInfo)
               {
                  double currentEntityDist =
                     std::abs(mSAD_CurrentEntity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  double newEntityDist =
                     std::abs(entity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  if (newEntityDist <= currentEntityDist)
                  {
                     SAD_SetCurrentEntity(entity);
                  }
               }
               // If this is the first entity contained, set it as we have nothing to compare to yet.
               else
               {
                  SAD_SetCurrentEntity(entity);
                  mSAD_ShowingEntityInfo = true;
               }
            }
         }
      }
   }

   if (!(mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cHOVER)))
   {
      // Loop through each entity and check if we're mousing over it.  Multiple entities hover regions may be hovered
      // over at the same time, so check each entity and set the current entity to the one closest to the mouse.
      mTSD_ShowingEntityInfo = false; // Set this to true if any entity is being moused over.
      mTSD_EntityClicked     = false;
      auto drawVector        = GetDrawVector(mDisplays[DisplayType::cTSD]);

      std::vector<TSD_Entity> tsdEntities;
      for (auto& e : mTSD_Entities)
      {
         tsdEntities.push_back(e);
      }
      for (auto& e : mTSD_Assets)
      {
         tsdEntities.push_back(e);
      }

      for (auto& entity : tsdEntities)
      {
         // The center rect needs to be shifted to the center of the compass.
         QPointF compassCenter =
            QPointF(mRect.center().x() + (drawVector.x() * pW()), mRect.center().y() + (drawVector.y() * pH()));
         QPointF rectFromCenter = QPointF((entity.hoverRegion.center().x() - compassCenter.x()) / pW(),
                                          ((entity.hoverRegion.center().y() - compassCenter.y()) / pH()) - mTSD_YOffset);
         double distFromCenter = sqrt(rectFromCenter.x() * rectFromCenter.x() + rectFromCenter.y() * rectFromCenter.y());
         // Don't hover over entities drawn behind the stencil buffer.
         // If we are within hover range, evaluate the entity.
         if (distFromCenter <= mTSD_CompassRadius || entity.perceivedData.angleOnly)
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               mTSD_EntityClicked = true;
               // Compare this entity to the current one.  If the newer distance is less than or equal to the current
               // one update the current entity.
               if (mTSD_ShowingEntityInfo)
               {
                  double currentEntityDist =
                     std::abs(mTSD_CurrentEntity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  double newEntityDist =
                     std::abs(entity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  if (newEntityDist <= currentEntityDist)
                  {
                     TSD_SetCurrentEntity(entity);
                  }
               }
               // If this is the first entity contained, set it as we have nothing to compare to yet.
               else
               {
                  TSD_SetCurrentEntity(entity);
                  mTSD_ShowingEntityInfo = true;
               }
            }
         }
      }
   }
}

void wkf::AcesDisplay::Overlay::HandleHoverRegions(const int aMouseX, const int aMouseY)
{
   if (mSettings[DisplayType::cSAD] & static_cast<size_t>(SAD_SETTINGS::cHOVER))
   {
      // Set the newest mouse position
      mMousePos.setX(aMouseX);
      mMousePos.setY(aMouseY);

      // Loop through each entity and check if we're mousing over it.  Multiple entities hover regions may be hovered
      // over at the same time, so check each entity and set the current entity to the one closest to the mouse.
      mSAD_ShowingEntityInfo = false; // Set this to true if any entity is being moused over.
      auto drawVector        = GetDrawVector(mDisplays[DisplayType::cSAD]);

      std::vector<Entity> sadEntities;
      for (auto& e : mSAD_Entities)
      {
         sadEntities.push_back(e);
      }
      for (auto& e : mSAD_TruthEntities)
      {
         sadEntities.push_back(e);
      }

      for (auto& entity : sadEntities)
      {
         // The center rect needs to be shifted to the center of the compass.
         QPointF compassCenter =
            QPointF(mRect.center().x() + (drawVector.x() * pW()), mRect.center().y() + (drawVector.y() * pH()));
         QPointF rectFromCenter = QPointF((entity.hoverRegion.center().x() - compassCenter.x()) / rW(),
                                          ((entity.hoverRegion.center().y() - compassCenter.y()) / rH()));
         double distFromCenter = sqrt(rectFromCenter.x() * rectFromCenter.x() + rectFromCenter.y() * rectFromCenter.y());
         // Don't hover over entities drawn behind the stencil buffer.
         // If we are within hover range, evaluate the entity.
         if (distFromCenter <= mSAD_CompassRadius || entity.perception.angleOnly)
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               // Compare this entity to the current one.  If the newer distance is less than or equal to the current
               // one update the current entity.
               if (mSAD_ShowingEntityInfo)
               {
                  double currentEntityDist =
                     std::abs(mSAD_CurrentEntity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  double newEntityDist =
                     std::abs(entity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  if (newEntityDist <= currentEntityDist)
                  {
                     SAD_SetCurrentEntity(entity);
                  }
               }
               // If this is the first entity contained, set it as we have nothing to compare to yet.
               else
               {
                  SAD_SetCurrentEntity(entity);
                  mSAD_ShowingEntityInfo = true;
               }
            }
         }
      }
   }

   if (mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cHOVER))
   {
      // Set the newest mouse position
      mMousePos.setX(aMouseX);
      mMousePos.setY(aMouseY);

      // Loop through each entity and check if we're mousing over it.  Multiple entities hover regions may be hovered
      // over at the same time, so check each entity and set the current entity to the one closest to the mouse.
      mTSD_ShowingEntityInfo = false; // Set this to true if any entity is being moused over.
      auto drawVector        = GetDrawVector(mDisplays[DisplayType::cTSD]);

      std::vector<TSD_Entity> tsdEntities;
      for (auto& e : mTSD_Entities)
      {
         tsdEntities.push_back(e);
      }
      for (auto& e : mTSD_Assets)
      {
         tsdEntities.push_back(e);
      }

      for (auto& entity : tsdEntities)
      {
         // The center rect needs to be shifted to the center of the compass.
         QPointF compassCenter =
            QPointF(mRect.center().x() + (drawVector.x() * pW()), mRect.center().y() + (drawVector.y() * pH()));
         QPointF rectFromCenter = QPointF((entity.hoverRegion.center().x() - compassCenter.x()) / rW(),
                                          ((entity.hoverRegion.center().y() - compassCenter.y()) / rH()));
         double distFromCenter = sqrt(rectFromCenter.x() * rectFromCenter.x() + rectFromCenter.y() * rectFromCenter.y());
         // Don't hover over entities drawn behind the stencil buffer.
         // If we are within hover range, evaluate the entity.
         if (distFromCenter <= mTSD_CompassRadius || (entity.perceivedData.angleOnly || TSD_IsEntityAngleOnly(entity)))
         {
            // If our mouse is inside the entities hover region
            if (entity.hoverRegion.contains(mMousePos))
            {
               // Compare this entity to the current one.  If the newer distance is less than or equal to the current
               // one update the current entity.
               if (mTSD_ShowingEntityInfo)
               {
                  double currentEntityDist =
                     std::abs(mTSD_CurrentEntity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  double newEntityDist =
                     std::abs(entity.hoverRegion.center().manhattanLength() - mMousePos.manhattanLength());
                  if (newEntityDist <= currentEntityDist)
                  {
                     TSD_SetCurrentEntity(entity);
                  }
               }
               // If this is the first entity contained, set it as we have nothing to compare to yet.
               else
               {
                  TSD_SetCurrentEntity(entity);
                  mTSD_ShowingEntityInfo = true;
               }
            }
         }
      }
   }
}

void wkf::AcesDisplay::Overlay::DrawConnectionDestroyed()
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

void wkf::AcesDisplay::Overlay::InitializeDrawables()
{
   InitializePFD();
   InitializeLeftSettings();
   InitializeRightSettings();
   InitializeTopSettings();
}

void wkf::AcesDisplay::Overlay::PlatformSelectionChanged(const wkf::PlatformList& aSelected,
                                                         const wkf::PlatformList& aDeselected)
{
   // Add any newly selected targets
   for (const auto& selectedPlatform : aSelected)
   {
      if (selectedPlatform->GetIndex() != mSourcePlatform)
      {
         mTargetPlatforms.push_back(selectedPlatform->GetIndex());
      }
   }

   // Remove any deselected targets if found
   for (const auto& deselectedPlatform : aDeselected)
   {
      if (deselectedPlatform->GetIndex() != mSourcePlatform)
      {
         auto target = std::find(mTargetPlatforms.begin(), mTargetPlatforms.end(), deselectedPlatform->GetIndex());
         if (target != mTargetPlatforms.end())
         {
            mTargetPlatforms.erase(target);
         }
      }
   }
}

void wkf::AcesDisplay::Overlay::SetTargetPlatforms(const std::vector<size_t>& aTargetPlatforms)
{
   mTargetPlatforms = aTargetPlatforms;

   // When the new target platforms list, check mTargetPlatform
   // to see if it is still in the list.  If it isn't, update
   // and default to the first target in the list
   bool found = false;
   if (mTargetPlatform != 0)
   {
      for (const auto& targetIndex : mTargetPlatforms)
      {
         if (mTargetPlatform == targetIndex)
         {
            found = true;
            break;
         }
      }
   }

   // Didn't find the target platform in the current list,
   // default to a target that does exist
   if (!found)
   {
      if (mTargetPlatforms.size() > 0)
      {
         wkf::Scenario* scenario = vaEnv.GetStandardScenario();
         if (scenario != nullptr)
         {
            mTargetPlatform = mTargetPlatforms.front();
         }
      }
      else
      {
         mTargetPlatform = 0;
      }
   }
}

void wkf::AcesDisplay::Overlay::TSD_DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup,
                                                 const double             aX,
                                                 const double             aY,
                                                 const double             aR)
{
   bool drawAir    = mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cAIR);
   bool drawGround = mSettings[DisplayType::cTSD] & static_cast<size_t>(TSD_SETTINGS::cGROUND);

   // Transformation - Shift the region to match the compass
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * rW(), aY * rH(), 0.0));

   double angleOnlyEntitySize = aR / 15.0; // 0.375 / 15 = 0.025
   double entitySize          = aR / 22.5; // 0.375 / 22.5 = ~0.167;

   // Draw our entities based on type and domain
   for (auto& entity : mTSD_Entities)
   {
      const QPointF pos = TSD_CalculateEntityPosition(entity);

      switch (entity.track.identification)
      {
      case Identification::ID_UNKNOWN:
      case Identification::ID_BOGIE:
      {
         if (drawAir)
         {
            if (TSD_IsEntityAngleOnly(entity))
            {
               TSD_DrawAngleOnly(aVectorGroup, entity, pos.x(), pos.y(), angleOnlyEntitySize, angleOnlyEntitySize, aR);
            }
            else
            {
               if (entity.track.isMissile)
               {
                  TSD_DrawBogieMissile(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
               }
               else
               {
                  TSD_DrawBogie(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
               }
            }
         }
         break;
      }
      case Identification::ID_NEUTRAL:
      {
         if (drawAir)
         {
            if (TSD_IsEntityAngleOnly(entity))
            {
               TSD_DrawAngleOnly(aVectorGroup, entity, pos.x(), pos.y(), angleOnlyEntitySize, angleOnlyEntitySize, aR);
            }
            else
            {
               if (entity.track.isMissile)
               {
                  TSD_DrawNeutralMissile(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
               }
               else
               {
                  TSD_DrawNeutral(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
               }
            }
         }
         break;
      }
      case Identification::ID_FRIENDLY:
      {
         if (drawAir)
         {
            if (entity.inFlight)
            {
               if (TSD_IsEntityAngleOnly(entity))
               {
                  TSD_DrawAngleOnly(aVectorGroup, entity, pos.x(), pos.y(), angleOnlyEntitySize, angleOnlyEntitySize, aR);
               }
               else
               {
                  if (entity.track.isMissile)
                  {
                     TSD_DrawFlightAssetMissile(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
                  }
                  else
                  {
                     TSD_DrawFlightAsset(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
                  }
               }
            }
            else
            {
               if (TSD_IsEntityAngleOnly(entity))
               {
                  TSD_DrawAngleOnly(aVectorGroup, entity, pos.x(), pos.y(), angleOnlyEntitySize, angleOnlyEntitySize, aR);
               }
               else
               {
                  if (entity.track.isMissile)
                  {
                     TSD_DrawAssetMissile(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
                  }
                  else
                  {
                     TSD_DrawAsset(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
                  }
               }
            }
         }
         break;
      }
      case Identification::ID_BANDIT:
      {
         if (drawAir && entity.domain == TSD_EntityDomain::AIR)
         {
            if (TSD_IsEntityAngleOnly(entity))
            {
               TSD_DrawAngleOnly(aVectorGroup, entity, pos.x(), pos.y(), angleOnlyEntitySize, angleOnlyEntitySize, aR);
            }
            else
            {
               if (entity.track.isMissile)
               {
                  TSD_DrawThreatMissile(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
               }
               else
               {
                  TSD_DrawThreat(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
               }
            }
         }
         else if (drawGround && entity.domain == TSD_EntityDomain::GROUND)
         {
            if (entity.range != -1.0)
            {
               TSD_DrawGroundThreat(vectorXform, entity, pos.x(), pos.y(), entitySize, entitySize * 2);
            }
         }
         break;
      }
      }
   }

   for (auto& asset : mTSD_Assets)
   {
      const QPointF pos = TSD_CalculateEntityPosition(asset);

      if (drawAir)
      {
         if (asset.perceivedData.flightId == mData.platformData.flightId)
         {
            if (asset.perceivedData.isMissile)
            {
               TSD_DrawFlightAssetMissile(vectorXform, asset, pos.x(), pos.y(), entitySize, entitySize * 2);
            }
            else
            {
               TSD_DrawFlightAsset(vectorXform, asset, pos.x(), pos.y(), entitySize, entitySize * 2);
            }
         }
         else
         {
            if (asset.perceivedData.isMissile)
            {
               TSD_DrawAssetMissile(vectorXform, asset, pos.x(), pos.y(), entitySize, entitySize * 2);
            }
            else
            {
               TSD_DrawAsset(vectorXform, asset, pos.x(), pos.y(), entitySize, entitySize * 2);
            }
         }
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

bool wkf::AcesDisplay::Overlay::TSD_IsEntityAngleOnly(const TSD_Entity& aEntity)
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

void wkf::AcesDisplay::Overlay::TSD_DrawAngleOnly(osg::ref_ptr<osg::Group> aVectorGroup,
                                                  TSD_Entity&              aEntity,
                                                  const double             aX,
                                                  const double             aY,
                                                  const double             aW,
                                                  const double             aH,
                                                  const double             aR)
{
   osg::Vec4 entityColor = (aEntity.inFlight) ? Color::SlateBlue : GetEntityColor(aEntity.track.identification);
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   double                                       angleOnlyDist  = aR + (aH * 0.75);
   bool                                         drawDebugLines = false;

   auto drawType = GL_LINE_LOOP;
   // Check for missiles, which are drawn with filled polygon, rather than hollow polygon
   if (aEntity.track.isMissile)
   {
      drawType = GL_POLYGON;
   }

   // If we're centered, draw as normal.
   if (mTSD_CenterPoint.x() == 0.0 && mTSD_CenterPoint.y() == 0.0 && mTSD_YOffset == 0.0)
   {
      // Calculate the position and orientation of the entity
      // vectorXform->setAttitude(osg::Quat(aEntity.track.tgtBearing_rad - ((mData.platformData.heading_deg + 180.0) *
      // UtMath::cRAD_PER_DEG), osg::Vec3(0.0f, 0.0f, -1.0f)));
      double az_rad =
         NormalizeRadians(aEntity.track.tgtBearing_rad - (mData.platformData.heading_deg * UtMath::cRAD_PER_DEG));

      double tgtX = angleOnlyDist * sin(az_rad);
      double tgtY = angleOnlyDist * cos(az_rad);

      // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
      const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity,
                                                              tgtX * mTSDRange_nm / mTSD_CompassRadius,
                                                              tgtY * mTSDRange_nm / mTSD_CompassRadius,
                                                              aW,
                                                              aH);
      // If we're drawing the hover regions to see where they are.
      if (mDrawHoverRegions)
      {
         // Hover region outline
         osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.topLeft().x() * rW(), (hoverRect.topLeft().y() + mTSD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.topRight().x() * rW(), (hoverRect.topRight().y() + mTSD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.bottomRight().x() * rW(), (hoverRect.bottomRight().y() + mTSD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(hoverRect.bottomLeft().x() * rW(), (hoverRect.bottomLeft().y() + mTSD_YOffset) * rH()));
         aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
      }

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));
      vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      // Entity triangle
      osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
      entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
      entityArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
      vectorXform->addChild(CreateGeometry(entityArray, drawType, entityColor).get());
   }
   else // Otherwise, calculate the line-circle intersection and draw and rotate appropriately
   {
      // If a TSD entity is angle only it won't have a valid lat lon to calculate a heading and distance, just use the bearing.
      double heading_deg = aEntity.track.tgtBearing_rad * UtMath::cDEG_PER_RAD;
      // UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg, mData.platformData.lon_deg,
      // aEntity.track.tgtLat_deg, aEntity.track.tgtLon_deg, heading_deg, distance_m);
      double ang = (heading_deg - 90) * UtMath::cRAD_PER_DEG;

      // Line extending from ownship through the angle only target
      QLineF line = QLineF(QPointF(0, 0), QPointF(1000 * sin(ang), 1000 * cos(ang)));
      line.translate(-mTSD_CenterPoint);

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
      double R = mTSDRange_nm;
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
      double compassAngle_rad = NormalizeRadians((c2ang - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      double tgtX = angleOnlyDist * sin(compassAngle_rad);
      double tgtY = angleOnlyDist * cos(compassAngle_rad); //(angleOnlyDist + mTSD_YOffset) * cos(compassAngle_rad);

      // Transformation
      vectorXform->setPosition(osg::Vec3d(tgtX * rW(), tgtY * rH(), 0.0));

      QLineF ownshipToAngleOnlyEntity = QLineF(QPointF(tgtX, tgtY), -mTSD_CenterPoint * TSD_UnitsPerNm());
      // Calculate the orientation of the angle only entity (point towards ownship)
      double az_rad =
         NormalizeRadians(((ownshipToAngleOnlyEntity.angle()) - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);

      vectorXform->setAttitude(osg::Quat(az_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      osg::ref_ptr<osg::PositionAttitudeTransform> vectorX2form(new osg::PositionAttitudeTransform());
      vectorX2form->setAttitude(osg::Quat(compassAngle_rad, osg::Vec3(0.0f, 0.0f, -1.0f)));

      // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
      const QRectF hoverRect =
         TSD_CalculateEntityHoverRegion(aEntity,
                                        (tgtX / TSD_UnitsPerNm()) + mTSD_CenterPoint.x(),
                                        ((tgtY - mTSD_YOffset) / TSD_UnitsPerNm()) + mTSD_CenterPoint.y(),
                                        aW,
                                        aH);
      // If we're drawing the hover regions to see where they are.
      if (mDrawHoverRegions)
      {
         QRectF rect = hoverRect;
         rect.translate(-mTSD_CenterPoint * TSD_UnitsPerNm());
         // Hover region outline
         osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
         hoverRegionArray->push_back(osg::Vec2d(rect.topLeft().x() * rW(), (rect.topLeft().y() + mTSD_YOffset) * rH()));
         hoverRegionArray->push_back(osg::Vec2d(rect.topRight().x() * rW(), (rect.topRight().y() + mTSD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(rect.bottomRight().x() * rW(), (rect.bottomRight().y() + mTSD_YOffset) * rH()));
         hoverRegionArray->push_back(
            osg::Vec2d(rect.bottomLeft().x() * rW(), (rect.bottomLeft().y() + mTSD_YOffset) * rH()));
         aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Gray).get());
      }

      if (drawDebugLines)
      {
         osg::ref_ptr<osg::Vec2Array> a123Array(new osg::Vec2Array);
         a123Array->push_back(osg::Vec2d(ownshipToAngleOnlyEntity.x1() * rW(), ownshipToAngleOnlyEntity.y1() * rH()));
         a123Array->push_back(osg::Vec2d(ownshipToAngleOnlyEntity.x2() * rW(), ownshipToAngleOnlyEntity.y2() * rH()));
         aVectorGroup->addChild(CreateGeometry(a123Array, GL_LINES, Color::Yellow).get());

         osg::ref_ptr<osg::Vec2Array> lineArray(new osg::Vec2Array);
         lineArray->push_back(osg::Vec2d(line.x1() * TSD_UnitsPerNm() * rW(), line.y1() * TSD_UnitsPerNm() * rH()));
         lineArray->push_back(osg::Vec2d(line.x2() * TSD_UnitsPerNm() * rW(), line.y2() * TSD_UnitsPerNm() * rH()));
         aVectorGroup->addChild(CreateGeometry(lineArray, GL_LINES, Color::Red).get());

         QLineF                       l = QLineF::fromPolar(1000, -90);
         osg::ref_ptr<osg::Vec2Array> line4Array(new osg::Vec2Array);
         line4Array->push_back(osg::Vec2d(l.x1() * TSD_UnitsPerNm() * rW(), l.y1() * TSD_UnitsPerNm() * rH()));
         line4Array->push_back(osg::Vec2d(l.x2() * TSD_UnitsPerNm() * rW(), l.y2() * TSD_UnitsPerNm() * rH()));
         vectorX2form->addChild(CreateGeometry(line4Array, GL_LINES, Color::Green).get());
         aVectorGroup->addChild(vectorX2form.get());
      }

      if (collision)
      {
         // Entity triangle
         osg::ref_ptr<osg::Vec2Array> entityArray(new osg::Vec2Array);
         entityArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
         entityArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
         vectorXform->addChild(CreateGeometry(entityArray, drawType, entityColor).get());
      }
   }

   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::TSD_DrawBogie(osg::ref_ptr<osg::Group> aVectorGroup,
                                              TSD_Entity&              aEntity,
                                              const double             aX,
                                              const double             aY,
                                              const double             aW,
                                              const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Box outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Yellow).get());

   // Half filled box
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.25) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.25) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::Yellow).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
   }
   vectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Yellow).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawBogieMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                                     TSD_Entity&              aEntity,
                                                     const double             aX,
                                                     const double             aY,
                                                     const double             aW,
                                                     const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Box
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.25) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.25) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_POLYGON, Color::Yellow).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
   vectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::Yellow).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
   vectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::Yellow).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawNeutral(osg::ref_ptr<osg::Group> aVectorGroup,
                                                TSD_Entity&              aEntity,
                                                const double             aX,
                                                const double             aY,
                                                const double             aW,
                                                const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Box outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::White).get());

   // Half filled box
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.25) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.25) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::White).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
   }
   vectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::White).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawNeutralMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                                       TSD_Entity&              aEntity,
                                                       const double             aX,
                                                       const double             aY,
                                                       const double             aW,
                                                       const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Box
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.25) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.25) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_POLYGON, Color::White).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
   vectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::White).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
   vectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::White).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                                               TSD_Entity&              aEntity,
                                               const double             aX,
                                               const double             aY,
                                               const double             aW,
                                               const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Triangle outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Red).get());

   // 40% filled triangle looks better than half fill
   osg::ref_ptr<osg::Vec2Array> boxFillArray(new osg::Vec2Array);
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.3) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.3) * rH()));
   boxFillArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxFillArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxFillArray, GL_POLYGON, Color::Red).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
   }
   vectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Red).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawThreatMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                                      TSD_Entity&              aEntity,
                                                      const double             aX,
                                                      const double             aY,
                                                      const double             aW,
                                                      const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   if (aEntity.track.targetHdgValid)
   {
      vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                         osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Filled Triangle
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.25) * rW(), -(aH * 0.25) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.25) * rW(), -(aH * 0.25) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_POLYGON, Color::Red).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
   vectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::Red).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
   vectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::Red).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawGroundThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                                                     TSD_Entity&              aEntity,
                                                     const double             aX,
                                                     const double             aY,
                                                     const double             aW,
                                                     const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));
   // We don't rotate ground units
   // vectorXform->setAttitude(osg::Quat(aEntity.trackData.tgtHdg_rad - mData.heading, osg::Vec3(0.0f, 0.0f, -1.0f)));

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Triangle outline
   osg::ref_ptr<osg::Vec2Array> boxRectArray(new osg::Vec2Array);
   boxRectArray->push_back(osg::Vec2d(0.0, 0.0));
   boxRectArray->push_back(osg::Vec2d((aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   boxRectArray->push_back(osg::Vec2d(-(aW * 0.5) * rW(), -(aH * 0.5) * rH()));
   vectorXform->addChild(CreateGeometry(boxRectArray, GL_LINE_LOOP, Color::Red).get());

   // Radar dish and heading lines
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
   }
   vectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Red).get());

   // Calculate the distance in coordinate space per nm, then multiply that times the entities range radius.
   double range = TSD_UnitsPerNm() * aEntity.range;

   // Range ring
   osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 9)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = range * cos(ang_rad);
      double y       = range * sin(ang_rad);
      ringArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(ringArray, GL_LINE_LOOP, Color::Red).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform.get());
}

void wkf::AcesDisplay::Overlay::TSD_DrawAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                                              TSD_Entity&              aEntity,
                                              const double             aX,
                                              const double             aY,
                                              const double             aW,
                                              const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));

   if (aEntity.type == TSD_EntityType::TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == TSD_EntityType::ASSET)
   {
      vectorXform->setAttitude(
         osg::Quat((aEntity.perceivedData.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Circle outline
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, Color::Green).get());

   // Half filled circle
   osg::ref_ptr<osg::Vec2Array> circleFillArray(new osg::Vec2Array);
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleFillArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(circleFillArray, GL_POLYGON, Color::Green).get());

   // Radar dish and heading lines
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
   }
   vectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::Green).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                                     TSD_Entity&              aEntity,
                                                     const double             aX,
                                                     const double             aY,
                                                     const double             aW,
                                                     const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));

   if (aEntity.type == TSD_EntityType::TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == TSD_EntityType::ASSET)
   {
      vectorXform->setAttitude(
         osg::Quat((aEntity.perceivedData.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Circle
   double                       r = (aW * 0.25);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(circleArray, GL_POLYGON, Color::Green).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
   vectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::Green).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
   vectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::Green).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawFlightAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                                                    TSD_Entity&              aEntity,
                                                    const double             aX,
                                                    const double             aY,
                                                    const double             aW,
                                                    const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));

   if (aEntity.type == TSD_EntityType::TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == TSD_EntityType::ASSET)
   {
      vectorXform->setAttitude(
         osg::Quat((aEntity.perceivedData.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Circle outline
   double                       r = (aW * 0.5);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(circleArray, GL_LINE_LOOP, Color::SlateBlue).get());

   // Half filled circle
   osg::ref_ptr<osg::Vec2Array> circleFillArray(new osg::Vec2Array);
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleFillArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(circleFillArray, GL_POLYGON, Color::SlateBlue).get());

   // Radar dish and heading lines
   osg::ref_ptr<osg::Vec2Array> radarArray(new osg::Vec2Array);

   // Heading line
   radarArray->push_back(osg::Vec2d(0.0, 0.0));
   radarArray->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));

   // Radar dish
   for (int ang_deg = 180; ang_deg <= 360; ang_deg += 18)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = r + r * sin(ang_rad);
      radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      // Add every element but the first and last twice for GL_LINES
      if (ang_deg != 180 && ang_deg != 360)
      {
         radarArray->push_back(osg::Vec2d(x * rW(), y * rH()));
      }
   }
   vectorXform->addChild(CreateGeometry(radarArray, GL_LINES, Color::SlateBlue).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

void wkf::AcesDisplay::Overlay::TSD_DrawFlightAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                                           TSD_Entity&              aEntity,
                                                           const double             aX,
                                                           const double             aY,
                                                           const double             aW,
                                                           const double             aH)
{
   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(aX * TSD_UnitsPerNm() * rW(), aY * TSD_UnitsPerNm() * rH(), 0.0));

   if (aEntity.type == TSD_EntityType::TRACK)
   {
      if (aEntity.track.targetHdgValid)
      {
         vectorXform->setAttitude(osg::Quat((aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                                            osg::Vec3(0.0f, 0.0f, -1.0f)));
      }
   }
   else if (aEntity.type == TSD_EntityType::ASSET)
   {
      vectorXform->setAttitude(
         osg::Quat((aEntity.perceivedData.heading_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG,
                   osg::Vec3(0.0f, 0.0f, -1.0f)));
   }

   // Calculate and set the hover region for this entity, returning a rect that we can draw if we want.
   const QRectF hoverRect = TSD_CalculateEntityHoverRegion(aEntity, aX, aY, aW, aH);

   // If we're drawing the hover regions to see where they are.
   if (mDrawHoverRegions)
   {
      // Hover region outline
      osg::ref_ptr<osg::Vec2Array> hoverRegionArray(new osg::Vec2Array);
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topLeft().x() * rW(), hoverRect.topLeft().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.topRight().x() * rW(), hoverRect.topRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomRight().x() * rW(), hoverRect.bottomRight().y() * rH()));
      hoverRegionArray->push_back(osg::Vec2d(hoverRect.bottomLeft().x() * rW(), hoverRect.bottomLeft().y() * rH()));
      aVectorGroup->addChild(CreateGeometry(hoverRegionArray, GL_LINE_LOOP, Color::Green).get());
   }

   // Circle
   double                       r = (aW * 0.25);
   osg::ref_ptr<osg::Vec2Array> circleArray(new osg::Vec2Array);
   for (int ang_deg = 0; ang_deg <= 360; ang_deg += 36)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = r * cos(ang_rad);
      double y       = -r + r * sin(ang_rad);
      circleArray->push_back(osg::Vec2d(x * rW(), y * rH()));
   }
   vectorXform->addChild(CreateGeometry(circleArray, GL_POLYGON, Color::SlateBlue).get());

   // Line with arrow
   osg::ref_ptr<osg::Vec2Array> arrowLines(new osg::Vec2Array);
   arrowLines->push_back(osg::Vec2d(0.0, -(aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d(-(aW * 0.2) * rW(), (aH * 0.3) * rH()));
   arrowLines->push_back(osg::Vec2d(0.0, (aH * 0.5) * rH()));
   arrowLines->push_back(osg::Vec2d((aW * 0.2) * rW(), (aH * 0.3) * rH()));
   vectorXform->addChild(CreateGeometry(arrowLines, GL_LINES, Color::SlateBlue).get());

   // Line through the middle
   osg::ref_ptr<osg::Vec2Array> middleLine(new osg::Vec2Array);
   middleLine->push_back(osg::Vec2d(-(aW * 0.15) * rW(), 0.0));
   middleLine->push_back(osg::Vec2d((aW * 0.15) * rW(), 0.0));
   vectorXform->addChild(CreateGeometry(middleLine, GL_LINES, Color::SlateBlue).get());

   vectorXform->setStateSet(GetStencilDrawWhereMatch().get());
   aVectorGroup->addChild(vectorXform);
}

const QRectF wkf::AcesDisplay::Overlay::TSD_CalculateEntityHoverRegion(TSD_Entity&  aEntity,
                                                                       const double aX,
                                                                       const double aY,
                                                                       double       aW,
                                                                       const double aH)
{
   auto    drawVector   = GetDrawVector(mDisplays[DisplayType::cTSD]);
   QPointF regionCenter = QPointF(drawVector.x() * pW(), drawVector.y() * pH());

   if (aEntity.type == TSD_EntityType::TRACK)
   {
      // Create a polygon with the basic width and height of the entity.
      QPolygonF poly;
      poly.append(QPointF(-(aW * 0.5), (aH * 0.5)));
      poly.append(QPointF((aW * 0.5), (aH * 0.5)));
      poly.append(QPointF((aW * 0.5), -(aH * 0.5)));
      poly.append(QPointF(-(aW * 0.5), -(aH * 0.5)));

      // Create a transform that will translate and then rotate the polygon.
      QTransform t =
         QTransform()
            .translate((aX * TSD_UnitsPerNm()), (aY * TSD_UnitsPerNm()))
            .rotateRadians(-(aEntity.track.tgtHdg_deg - mData.platformData.heading_deg) * UtMath::cRAD_PER_DEG);
      // Translate and rotate the polygon to align with the entity.
      QPolygonF nPoly = t.map(poly);
      // Create a bounding rect surrounding the new polygon.
      QRectF bR = nPoly.boundingRect();
      // Set the hover region rect for our entity here, converting to pixel coordinates to match mouse move events.
      QPointF compassCenterPixels = QPointF(mRect.center().x() + regionCenter.x(), mRect.center().y() + regionCenter.y());
      QPointF centerPointOffsetPixels =
         QPointF(mTSD_CenterPoint.x() * TSD_UnitsPerNm() * rW(), mTSD_CenterPoint.y() * TSD_UnitsPerNm() * rH());
      aEntity.hoverRegion.setRect((compassCenterPixels.x() - centerPointOffsetPixels.x()) + (bR.x() * rW()),
                                  ((compassCenterPixels.y() - centerPointOffsetPixels.y()) + (mTSD_YOffset * rH())) +
                                     (bR.y() * rH()),
                                  bR.width() * rW(),
                                  bR.height() * rH());

      // Return the bounding rect in case it will be drawn (debugging).
      return bR;
   }
   else if (aEntity.type == TSD_EntityType::ASSET)
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
                           .translate((aX * TSD_UnitsPerNm()), (aY * TSD_UnitsPerNm()))
                           .rotateRadians(-(aEntity.perceivedData.heading_deg - mData.platformData.heading_deg) *
                                          UtMath::cRAD_PER_DEG);
         // Translate and rotate the polygon to align with the entity.
         QPolygonF nPoly = t.map(poly);
         // Create a bounding rect surrounding the new polygon.
         QRectF bR = nPoly.boundingRect();
         // Set the hover region rect for our entity here, converting to pixel coordinates to match mouse move events.
         QPointF compassCenterPixels =
            QPointF(mRect.center().x() + regionCenter.x(), mRect.center().y() + regionCenter.y());
         QPointF centerPointOffsetPixels =
            QPointF(mTSD_CenterPoint.x() * TSD_UnitsPerNm() * rW(), mTSD_CenterPoint.y() * TSD_UnitsPerNm() * rH());
         aEntity.hoverRegion.setRect((compassCenterPixels.x() - centerPointOffsetPixels.x()) + (bR.x() * rW()),
                                     ((compassCenterPixels.y() - centerPointOffsetPixels.y()) + (mTSD_YOffset * rH())) +
                                        (bR.y() * rH()),
                                     bR.width() * rW(),
                                     bR.height() * rH());
         // Return the bounding rect in case it will be drawn (debugging).
         return bR;
      }
   }
   return QRectF();
}

const QPointF wkf::AcesDisplay::Overlay::TSD_CalculateEntityPosition(const TSD_Entity& aEntity)
{
   if (aEntity.type == TSD_EntityType::TRACK)
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
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                         mData.platformData.lon_deg,
                                                         aEntity.track.tgtLat_deg,
                                                         aEntity.track.tgtLon_deg,
                                                         heading_deg,
                                                         distance_m);

         return CalculatePositionFromBearingAndRange(heading_deg * UtMath::cRAD_PER_DEG, distance_m * UtMath::cNM_PER_M);
      }
      // If we have just bearing
      else if (aEntity.track.bearingValid)
      {
         return CalculatePositionFromBearingAndRange(aEntity.track.tgtBearing_rad, (mTSDRange_nm * 1.04));
      }
   }
   else if (aEntity.type == TSD_EntityType::ASSET)
   {
      // If we have a valid heading.  (Assume lat and lon is valid)
      if (aEntity.perceivedData.headingValid)
      {
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mData.platformData.lat_deg,
                                                         mData.platformData.lon_deg,
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
                                                     (mTSDRange_nm * 1.06));
      }
   }
   return QPointF(-1.0, -1.0);
}

const QPointF wkf::AcesDisplay::Overlay::TSD_CalculatePositionFromBearingAndRange(const double aBearingRad,
                                                                                  const double aRangeNm)
{
   // Calculate the position and orientation of the entity
   double az_rad = NormalizeRadians(aBearingRad - (mData.platformData.heading_deg * UtMath::cRAD_PER_DEG));

   double tgtX_nm = aRangeNm * sin(az_rad);
   double tgtY_nm = aRangeNm * cos(az_rad);

   return QPointF(tgtX_nm, tgtY_nm);
}
