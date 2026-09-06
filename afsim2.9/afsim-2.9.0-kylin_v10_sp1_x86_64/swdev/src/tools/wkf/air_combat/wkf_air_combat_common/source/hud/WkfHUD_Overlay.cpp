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

#include "WkfHUD_Overlay.hpp"

#include <chrono>
#include <iostream>
#include <math.h>

#include <QApplication>
#include <QDesktopWidget>
#include <osg/Depth>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/StateSet>
#include <osg/Stencil>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtPath.hpp"
#include "UtSphericalEarth.hpp"
#include "Uto.hpp"
#include "VaCamera.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"

// Set this to true when debugging
bool debugFlagHUD = false;

wkf::HUD::HUD()
   : HUD("hud", cOVERLAY_WORLD, 50.0)
{
}

wkf::HUD::HUD(const std::string& aName)
   : HUD(aName, cOVERLAY_WORLD, 50.0)
{
}

wkf::HUD::HUD(const std::string& aName, vespa::VaOverlay::OverlayType aType, double aWindowFullFovY_deg)
   : vespa::VaOverlay(aName, aType)
   , mWindowFullFovY_deg(aWindowFullFovY_deg)
{
}

wkf::HUD::HUD(const HUD& aSrc)
   : vespa::VaOverlay(aSrc)
   , mFont(nullptr)
   , mHudGeode(aSrc.mHudGeode)
   , mHudState(aSrc.mHudState)
   , mHudColor(aSrc.mHudColor)
   , mHudProjection(aSrc.mHudProjection)
   , mHudModelViewMatrix(aSrc.mHudModelViewMatrix)
   , mRawShapePtr(aSrc.mRawShapePtr)
   , mHUD_HalfWidth(aSrc.mHUD_HalfWidth)
   , mHUD_HalfHeight(aSrc.mHUD_HalfHeight)
   , mWindowFullFovY_deg(aSrc.mWindowFullFovY_deg)
   , mHUDFullFovX_deg(aSrc.mHUDFullFovX_deg)
   , mHUDFullFovY_deg(aSrc.mHUDFullFovY_deg)
   , mHudRedColor(aSrc.mHudRedColor)
   , mHudGreenColor(aSrc.mHudGreenColor)
   , mHudBlueColor(aSrc.mHudBlueColor)
   , mHudAlphaColor(aSrc.mHudAlphaColor)
   , mDebugFlag(aSrc.mDebugFlag)
   , mPixelsPerDegree(aSrc.mPixelsPerDegree)
   , mHudMode(aSrc.mHudMode)
{
}

wkf::HUD::~HUD()
{
   UnbindShape("hud");
}

wkf::HUD* wkf::HUD::Clone() const
{
   return new HUD(*this);
}

bool wkf::HUD::Initialize()
{
   bool initialized = false;

   // Turn off the MFDs for now
   // mDrawMFDs = false;

   UtoRawShape rawShapePtr;
   mRawShapePtr = dynamic_cast<UtoRawShape*>(BindShape("hud", rawShapePtr));
   if (mRawShapePtr == nullptr)
   {
      auto out = ut::log::warning() << "***** WARNING: Couldn't bind shape: hud";
   }
   else
   {
      mRawShapePtr->SetColor(UtoColor(0, 0, 0, 128));
      SetVisible(true);
      initialized = true;
   }

   if (!mHudGeode)
   {
      mHudGeode = new osg::Geode;
   }

   if (!mHudState)
   {
      mHudState = new osg::StateSet;
      // Disable depth testing so geometry is draw regardless of depth values
      // of geometry already draw.
      mHudState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      mHudState->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

      // Need to make sure this geometry is draw last. RenderBins are handled
      // in numerical order so set bin number to 11
      mHudState->setRenderBinDetails(11, "RenderBin");

      mHudGeode->setStateSet(mHudState);
   }

   if (!mHudProjection)
   {
      mHudProjection = new osg::Projection;
   }

   if (!mHudColor)
   {
      mHudColor = new osg::Vec4Array;
      mHudColor->push_back(osg::Vec4(mHudRedColor, mHudGreenColor, mHudBlueColor, mHudAlphaColor));
   }

   if (mHudProjection != nullptr && GetViewer() != nullptr)
   {
      SetupHudProjection(mWindowFullFovY_deg);

      if (mHudModelViewMatrix == nullptr)
      {
         mHudModelViewMatrix = new osg::MatrixTransform;
         mHudModelViewMatrix->setMatrix(osg::Matrix::identity());
         mHudProjection->addChild(mHudModelViewMatrix.get());
         mHudModelViewMatrix->addChild(mHudGeode.get());

         if (mRawShapePtr != nullptr)
         {
            mRawShapePtr->GetTree()->addChild(mHudProjection.get());
         }
      }
   }

   return initialized;
}

void wkf::HUD::SetupHudProjection(float aFovY)
{
   // Set window full FOV member attribute
   mWindowFullFovY_deg = aFovY;

   int viewHeight = GetViewer()->GetHeight() / QApplication::desktop()->devicePixelRatio();
   int viewWidth  = GetViewer()->GetWidth() / QApplication::desktop()->devicePixelRatio();

   mPixelsPerDegree = viewHeight / aFovY;
   double ratio     = (double)viewWidth / (double)viewHeight;

   // NOTE: The HUD uses an "extent" of roughly +/-14 deg off bore-sight in both the
   // vertical and horizontal directions. Consider a 1080 vertical window with a 44 deg
   // vertical FOV. In this case, the mPixelsPerDegree is roughly 24.5 pixel/deg. Hence,
   // the HUD extents are approx 343 pixels (14x24.5). We determine the number of pixels
   // for the current configuration based on the pixels per degree and the HUD vertical
   // extent.

   const double hudVertExtent_deg = 14.0;
   float        pixels            = mPixelsPerDegree * hudVertExtent_deg;
   pixels                         = (float)viewHeight * 0.5f;

   if (mHudProjection)
   {
      mHudProjection->setMatrix(osg::Matrix::ortho2D(-ratio * pixels, ratio * pixels, -pixels, pixels));
   }
}

void wkf::HUD::Update()
{
   double lookAngle_Az_deg = 0.0;
   double lookAngle_El_deg = 0.0;

   double time = mPlatData.simTime;

   double result = fmod(time, 2.0);
   if (result < 1.0)
   {
      mSlowBlink = true;
   }
   else
   {
      mSlowBlink = false;
   }

   result = fmod(time, 1.0);
   if (result < 0.5)
   {
      mMediumBlink = true;
   }
   else
   {
      mMediumBlink = false;
   }

   result = fmod(time, 0.5);
   if (result < 0.25)
   {
      mFastBlink = true;
   }
   else
   {
      mFastBlink = false;
   }

   result = fmod(time, 0.2);
   if (result < 0.1)
   {
      mSuperFastBlink = true;
   }
   else
   {
      mSuperFastBlink = false;
   }

   vespa::VaCamera* camera = GetViewer()->GetCamera();
   if (camera)
   {
      double dist_m = 0.0;
      lookAngle_El_deg *= -1.0; // Convention is strange -- negate it
      camera->GetAzElDist(lookAngle_Az_deg, lookAngle_El_deg, dist_m);
   }

   // Set font size and line width, based on window dimensions
   int  height    = GetViewer()->GetHeight() / QApplication::desktop()->devicePixelRatio();
   int  width     = GetViewer()->GetWidth() / QApplication::desktop()->devicePixelRatio();
   bool valuesSet = false;

   if (height == 540 || height == 520)
   {
      if (width == 960 || height == 940)
      {
         mFontSizeMult  = 1;
         mLineWidthMult = 1.0f;
         valuesSet      = true;
      }
   }
   else if (height == 1080 || height == 1060)
   {
      if (width == 960 || width == 940 || width == 1920 || width == 1900)
      {
         mFontSizeMult  = 2;
         mLineWidthMult = 2.0f;
         valuesSet      = true;
      }
   }
   else if (height == 1200 || height == 1180)
   {
      if (width == 960 || width == 940 || width == 1920 || width == 1900)
      {
         mFontSizeMult  = 2;
         mLineWidthMult = 2.2f;
         valuesSet      = true;
      }
   }
   else if (height == 2160 || height == 2140)
   {
      if (width == 1920 || width == 1900 || width == 3840 || width == 3820)
      {
         mFontSizeMult  = 4;
         mLineWidthMult = 4.0f;
         valuesSet      = true;
      }
   }

   Draw(0.5 * (double)width, 0.5 * (double)height, mWindowFullFovY_deg, lookAngle_Az_deg, lookAngle_El_deg);

   if (!valuesSet)
   {
      auto out = ut::log::warning() << "Head-Up View does not support a window resolution of " << width << "x" << height;
   }
}

void wkf::HUD::Draw(double aHUD_HalfWidth,
                    double aHUD_HalfHeight,
                    double aFullFovY_deg,
                    double aLookAngle_Az_deg,
                    double aLookAngle_El_deg)
{
   double ppd = mPixelsPerDegree;

   // Clear out the geodes drawables
   mHudGeode->removeDrawables(0, mHudGeode->getNumDrawables());

   // Set regions
   mHeadingRegion.SetExtents(-8.0 * ppd, 5.0 * ppd, 8.0 * ppd, 7.0 * ppd);

   mSpeedRegion.SetExtents(-8.0 * ppd, 0.5 * ppd, -5.0 * ppd, 2.0 * ppd);
   mLeftRegion.SetExtents(-9.0 * ppd, -11.0 * ppd, -5.0 * ppd, 0.5 * ppd);

   mAltitudeRegion.SetExtents(5.0 * ppd, 0.5 * ppd, 9.0 * ppd, 2.0 * ppd);
   mRightRegion.SetExtents(5.0 * ppd, -11.0 * ppd, 9.0 * ppd, 0.5 * ppd);

   // mMfdLftRegion.SetExtents(-36.0 * ppd, -20.0 * ppd, -16.0 * ppd, 0.0 * ppd);
   // mMfdRgtRegion.SetExtents(16.0 * ppd, -20.0 * ppd, 36.0 * ppd, 0.0 * ppd);
   mUfcRegion.SetExtents(-14.0 * ppd, -22.0 * ppd, 14.0 * ppd, -14.0 * ppd);

   // Set HUD background color
   osg::ref_ptr<osg::Vec4Array> hudBackgroundColor(new osg::Vec4Array);
   hudBackgroundColor->push_back(osg::Vec4(0.0, 0.15, 0.0, 0.55));

   osg::ref_ptr<osg::Vec4Array> hudEdgeColor;
   hudEdgeColor = new osg::Vec4Array;
   hudEdgeColor->push_back(osg::Vec4(0.0f, 0.6f, 0.1f, 0.65f));

   // HUD Outer Border
   osg::ref_ptr<osg::Vec2Array> vertsHudEdge(new osg::Vec2Array);
   vertsHudEdge->push_back(osg::Vec2d(5.0 * ppd, 12.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(11.0 * ppd, 6.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(11.0 * ppd, -12.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(7.0 * ppd, -14.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(-7.0 * ppd, -14.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(-11.0 * ppd, -12.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(-11.0 * ppd, 6.0 * ppd));
   vertsHudEdge->push_back(osg::Vec2d(-5.0 * ppd, 12.0 * ppd));

   mHudGeode->addChild(CreateHudGeometry(vertsHudEdge, GL_POLYGON, hudBackgroundColor));
   mHudGeode->addChild(CreateHudGeometry(vertsHudEdge, GL_LINE_LOOP, hudEdgeColor).get());

   // HUD Inner Border
   if (mHudPolygonRegion == nullptr)
   {
      mHudPolygonRegion = new osg::Vec2Array;
   }
   else
   {
      mHudPolygonRegion->clear();
   }

   mHudPolygonRegion->push_back(osg::Vec2d(4.0 * ppd, 9.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(10.0 * ppd, 6.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(10.0 * ppd, -11.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(6.0 * ppd, -13.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(-6.0 * ppd, -13.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(-10.0 * ppd, -11.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(-10.0 * ppd, 6.0 * ppd));
   mHudPolygonRegion->push_back(osg::Vec2d(-4.0 * ppd, 9.0 * ppd));

   // NOTE: This flag can be used for testing. It should be false for release.
   mDebugFlag = false;

   if (mDebugFlag)
   {
      osg::ref_ptr<osg::Vec4Array> debugColor(new osg::Vec4Array);
      debugColor->push_back(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));
      mHudGeode->addChild(CreateHudGeometry(mHudPolygonRegion, GL_LINE_LOOP, debugColor));

      DrawDebugRegion(mHeadingRegion);
      DrawDebugRegion(mSpeedRegion);
      DrawDebugRegion(mAltitudeRegion);
      DrawDebugRegion(mLeftRegion);
      DrawDebugRegion(mRightRegion);
   }

   // Start Heading Region -----------------------------------------
   DrawStencilBufferRegion(mHeadingRegion, 1);
   DrawHeadingRegion(mPixelsPerDegree);
   DrawStencilBufferRegion(mHeadingRegion, 0);
   // End Heading Region

   // Draw Waterline and velocity vector ---------------------------
   DrawWaterlineSymbol();
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawVelocityVector();
   DrawStencilBufferRegion(mHudPolygonRegion, 0);

   // Draw CenterCircle --------------------------------------------
   DrawCenterCircle();

   // Start Speed Region -------------------------------------------
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawSpeedRegion();
   DrawStencilBufferRegion(mHudPolygonRegion, 0);
   // End Speed Region

   // Start Altitude Region ----------------------------------------
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawAltitudeRegion();
   DrawStencilBufferRegion(mHudPolygonRegion, 0);
   // End Altitude Region

   // Start Left Region --------------------------------------------
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawLeftRegion();
   DrawStencilBufferRegion(mHudPolygonRegion, 0);
   // End Left Region

   // Start Right Region -------------------------------------------
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawRightRegion();
   DrawStencilBufferRegion(mHudPolygonRegion, 0);
   // End Right Region

   // Start Pitch Ladder -------------------------------------------
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawStencilBufferRegion(mHeadingRegion, 0);  // Clear-out the heading region
   DrawStencilBufferRegion(mSpeedRegion, 0);    // Clear-out the speed region
   DrawStencilBufferRegion(mAltitudeRegion, 0); // Clear-out the altitude region
   DrawStencilBufferRegion(mLeftRegion, 0);     // Clear-out the left region
   DrawStencilBufferRegion(mRightRegion, 0);    // Clear-out the right region
   DrawPitchLadder();
   DrawRollPointer();

   // Draw selected weapon data
   if (mPlatData.selectedWeaponValid)
   {
      std::string weapnStr = mPlatData.selectedWeapon;
      DrawWeaponSelected(weapnStr, !mPlatData.masterArmActive);
   }

   // Draw bingo warning
   if (mPlatData.bingoFlagActive)
   {
      DrawBingo();
   }

   // Draw shoot flag
   if (mPlatData.shootCueActive)
   {
      DrawShoot();
   }

   DrawStencilBufferRegion(mHudPolygonRegion, 0);
   // End Pitch Ladder

   // Start Target Box ---------------------------------------------
   DrawStencilBufferRegion(mHudPolygonRegion, 1);
   DrawTargetBoxOnHud();
   DrawStencilBufferRegion(mHudPolygonRegion, 0);
   // End Target Box

   // UFC
   DrawStencilBufferRegion(mUfcRegion, 1); // Draw region
   DrawUFC();
   DrawStencilBufferRegion(mUfcRegion, 0); // Restore region

   if (mConnectionDestroyed)
   {
      DrawConnectionDestroyed();
   }
}

void wkf::HUD::SetHudColors(float aHudRedColor, float aHudGreenColor, float aHudBlueColor, float aHudAlphaColor)
{
   mHudRedColor   = aHudRedColor;
   mHudGreenColor = aHudGreenColor;
   mHudBlueColor  = aHudBlueColor;
   mHudAlphaColor = aHudAlphaColor;
}

void wkf::HUD::DrawDebugRegion(const RegionExtents& aExtents)
{
   double                       hudHalfWidth  = 0;
   double                       hudHalfHeight = 0;
   osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);
   verts->push_back(osg::Vec2d(aExtents.X1() + hudHalfWidth, aExtents.Y1() + hudHalfHeight));
   verts->push_back(osg::Vec2d(aExtents.X2() + hudHalfWidth, aExtents.Y1() + hudHalfHeight));
   verts->push_back(osg::Vec2d(aExtents.X2() + hudHalfWidth, aExtents.Y2() + hudHalfHeight));
   verts->push_back(osg::Vec2d(aExtents.X1() + hudHalfWidth, aExtents.Y2() + hudHalfHeight));

   osg::ref_ptr<osg::Vec4Array> debugColor(new osg::Vec4Array);
   debugColor->push_back(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

   mHudGeode->addChild(CreateHudGeometry(verts, GL_LINE_LOOP, debugColor).get());
}

void wkf::HUD::DrawWaterlineSymbol()
{
   float ppd = mPixelsPerDegree;

   osg::ref_ptr<osg::Geometry>  geom(new osg::Geometry);
   osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);
   verts->push_back(osg::Vec2d(-0.8 * ppd, 0.0));
   verts->push_back(osg::Vec2d(-0.4 * ppd, 0.0));
   verts->push_back(osg::Vec2d(-0.2 * ppd, -0.3 * ppd));
   verts->push_back(osg::Vec2d(0.0 * ppd, 0.0));
   verts->push_back(osg::Vec2d(0.2 * ppd, -0.3 * ppd));
   verts->push_back(osg::Vec2d(0.4 * ppd, 0.0));
   verts->push_back(osg::Vec2d(0.8 * ppd, 0.0));
   osg::ref_ptr<osg::DrawArrays> lineArray(new osg::DrawArrays(GL_LINE_STRIP, 0, verts->size()));
   mHudGeode->addChild(CreateHudGeometry(verts, GL_LINE_STRIP, mHudColor).get());
}

void wkf::HUD::DrawVelocityVector()
{
   float ppd = mPixelsPerDegree;

   osg::ref_ptr<osg::Geode>     vectorGeode(new osg::Geode);
   osg::ref_ptr<osg::Group>     vectorGroup(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);

   // Beta convention is opposite of alpha convention
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(mPlatData.beta_deg * ppd, -mPlatData.alpha_deg * ppd, 0.0));

   const int numPts = 12;
   double    dang   = 2.0 * 3.1415927 / (double)numPts;
   for (int i = 0; i < numPts; ++i)
   {
      double     ang = (double)i * dang;
      double     x   = 0.3 * ppd * cos(ang);
      double     y   = 0.3 * ppd * sin(ang);
      osg::Vec2d v(x, y);
      verts->push_back(v);
   }

   osg::ref_ptr<osg::DrawArrays> circleArray(new osg::DrawArrays(GL_LINE_LOOP, 0, verts->size()));
   osg::ref_ptr<osg::Vec2Array>  vertArray(new osg::Vec2Array);
   vertArray->push_back(osg::Vec2d(-0.6 * ppd, 0.0));
   vertArray->push_back(osg::Vec2d(-0.3 * ppd, 0.0));
   vertArray->push_back(osg::Vec2d(0.3 * ppd, 0.0));
   vertArray->push_back(osg::Vec2d(0.6 * ppd, 0.0));
   vertArray->push_back(osg::Vec2d(0.0, 0.3 * ppd));
   vertArray->push_back(osg::Vec2d(0.0, 0.6 * ppd));

   vectorGeode->addChild(CreateHudGeometry(vertArray, GL_LINES, mHudColor).get());
   vectorGeode->addChild(CreateHudGeometry(verts, GL_LINE_LOOP, mHudColor).get());
   vectorXform->addChild(vectorGeode.get());
   vectorGroup->addChild(vectorXform.get());
   vectorGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(vectorGroup.get());
}

void wkf::HUD::DrawCenterCircle()
{
   if (mPlatData.shootCueActive)
   {
      osg::ref_ptr<osg::Group>     circleGroup(new osg::Group);
      osg::ref_ptr<osg::Geometry>  geom(new osg::Geometry);
      osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);

      for (int i = 0; i < 20; ++i)
      {
         double ang_rad = 2.0 * 3.1415927 * ((double)i / 20.0);
         double x       = 20.0 * cos(ang_rad);
         double y       = 20.0 * sin(ang_rad);
         vertArray->push_back(osg::Vec2d(x, y));
      }

      circleGroup->addChild(CreateHudGeometry(vertArray, GL_LINE_LOOP, mHudColor).get());
      mHudGeode->addChild(circleGroup.get());
   }
}

void wkf::HUD::DrawStencilBufferRegion(const RegionExtents& aExtents, int aStencilValue)
{
   osg::ref_ptr<osg::StateSet> stencilStateSet(new osg::StateSet);
   osg::ref_ptr<osg::Stencil>  stencilStateAttribute(new osg::Stencil);
   ;

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
   mHudState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

   // Draw region
   double x1 = aExtents.X1();
   double y1 = aExtents.Y1();
   double x2 = aExtents.X2();
   double y2 = aExtents.Y2();

   osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);

   verts->push_back(osg::Vec2d(x1, y1));
   verts->push_back(osg::Vec2d(x2, y1));
   verts->push_back(osg::Vec2d(x2, y2));
   verts->push_back(osg::Vec2d(x1, y2));

   osg::ref_ptr<osg::Geode> stencilGeode = CreateHudGeometry(verts, GL_QUADS, mHudColor).get();
   stencilGeode->setStateSet(stencilStateSet.get());
   mHudGeode->addChild(stencilGeode.get());
}

void wkf::HUD::DrawStencilBufferRegion(const osg::ref_ptr<osg::Vec2Array> aArray, int aStencilValue)
{
   osg::ref_ptr<osg::StateSet> stencilStateSet(new osg::StateSet);
   osg::ref_ptr<osg::Stencil>  stencilStateAttribute(new osg::Stencil);
   ;

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
   mHudState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

   // Draw region
   osg::ref_ptr<osg::Geode> stencilGeode = CreateHudGeometry(aArray, GL_POLYGON, mHudColor).get();
   stencilGeode->setStateSet(stencilStateSet.get());
   mHudGeode->addChild(stencilGeode.get());
}

osg::ref_ptr<osg::StateSet> wkf::HUD::GetStencilDrawWhereMatch()
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

void wkf::HUD::DrawHeadingRegion(double aPixelsPerDegree)
{
   double ppd = aPixelsPerDegree;

   int fontSize = 11 * mFontSizeMult;

   // Heading
   double tmpHdg_deg = mPlatData.heading_deg;

   if (tmpHdg_deg < 0.0)
   {
      tmpHdg_deg += 360.0;
   }
   if (tmpHdg_deg > 360.0)
   {
      tmpHdg_deg -= 360.0;
   }

   int hdg_deg = (int)(tmpHdg_deg + 0.5);
   if (hdg_deg == 0)
   {
      hdg_deg = 360.0;
   }
   if (hdg_deg > 360)
   {
      hdg_deg -= 360.0;
   }

   // We first draw the main heading "box"

   // Top heading text
   std::stringstream ss;
   ss << std::setw(3) << std::setfill('0') << hdg_deg;
   mHudGeode->addChild(AddTextItem(ss.str(), 0.0f, 6.3f * ppd, fontSize, osgText::Text::CENTER_CENTER).get());

   // Border around top heading
   osg::ref_ptr<osg::Group>     headingGroup(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);
   vertArray->push_back(osg::Vec2d(-1.0 * ppd, 5.8 * ppd));
   vertArray->push_back(osg::Vec2d(1.0 * ppd, 5.8 * ppd));
   vertArray->push_back(osg::Vec2d(1.0 * ppd, 6.8 * ppd));
   vertArray->push_back(osg::Vec2d(-1.0 * ppd, 6.8 * ppd));
   mHudGeode->addChild(CreateHudGeometry(vertArray, GL_LINE_LOOP, mHudColor).get());

   // Now, we prevent drawing into the box
   RegionExtents boxRegion;
   boxRegion.SetExtents(-1.1 * ppd, 5.7 * ppd, 1.1 * ppd, 6.9 * ppd);
   DrawStencilBufferRegion(boxRegion, 0);

   // Multiplier for moving the heading tape side-to-side, based on heading. Note that
   // we don't use the standard aPixelsPerDegree, we use the headingPixelsPerDegree
   double headingPixelsPerDegree = 0.5 * aPixelsPerDegree;

   // Set the font size for all other text for the heading region
   fontSize = 9 * mFontSizeMult;

   osg::ref_ptr<osg::PositionAttitudeTransform> xform(new osg::PositionAttitudeTransform());
   xform->setPosition(osg::Vec3d(-tmpHdg_deg * headingPixelsPerDegree, 6.3 * ppd, 0.0));

   // Heading numerals
   std::string curHdgStr = "";

   // This needs to have an extra 30 deg on either side
   for (int i = -4; i < 40; i++)
   {
      curHdgStr = "";
      if (i < 0)
      {
         int num = 36 + i;
         curHdgStr.append(std::to_string(num));
      }
      else if (i < 10)
      {
         curHdgStr.append("0");
         curHdgStr.append(std::to_string(i));
      }
      else if (i > 36)
      {
         int num = i - 36;
         curHdgStr.append("0");
         curHdgStr.append(std::to_string(num));
      }
      else
      {
         curHdgStr.append(std::to_string(i));
      }

      xform->addChild(
         AddTextItem(curHdgStr, (10.0 * i) * headingPixelsPerDegree, 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
   }

   // Tick lines

   // Center tick (Note that this does not use the xform)
   osg::ref_ptr<osg::Vec2Array> centerTickVerts(new osg::Vec2Array);
   centerTickVerts->push_back(osg::Vec2d(0.0f, 5.4 * ppd));
   centerTickVerts->push_back(osg::Vec2d(0.0f, 5.1 * ppd));
   mHudGeode->addChild(CreateHudGeometry(centerTickVerts, GL_LINES, mHudColor).get());

   // Heading bug
   if (mPlatData.wayptDataValid)
   {
      // We draw the bug twice, since it may not be seen depending on the ownship heading

      // Standard draw
      osg::ref_ptr<osg::Vec2Array> hdgBugVerts1(new osg::Vec2Array);
      float                        bugX = (float)(headingPixelsPerDegree * mPlatData.headingBug_deg);
      hdgBugVerts1->push_back(osg::Vec2d(bugX - 0.2 * ppd, -1.2 * ppd));
      hdgBugVerts1->push_back(osg::Vec2d(bugX, -0.9 * ppd));
      hdgBugVerts1->push_back(osg::Vec2d(bugX + 0.2 * ppd, -1.2 * ppd));
      xform->addChild(CreateHudGeometry(hdgBugVerts1, GL_LINE_STRIP, mHudColor).get());

      // Draw with 360 deg offset
      osg::ref_ptr<osg::Vec2Array> hdgBugVerts2(new osg::Vec2Array);
      bugX = (float)(headingPixelsPerDegree * (mPlatData.headingBug_deg + 360.0));
      hdgBugVerts2->push_back(osg::Vec2d(bugX - 0.2 * ppd, -1.2 * ppd));
      hdgBugVerts2->push_back(osg::Vec2d(bugX, -0.9 * ppd));
      hdgBugVerts2->push_back(osg::Vec2d(bugX + 0.2 * ppd, -1.2 * ppd));
      xform->addChild(CreateHudGeometry(hdgBugVerts2, GL_LINE_STRIP, mHudColor).get());
   }

   // Multiple tick lines
   int                          count = 0;
   osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);

   for (int i = -60; i <= 400; i += 5)
   {
      double num  = (double)i;
      double xpos = num * headingPixelsPerDegree;
      if (count == 0)
      {
         verts->push_back(osg::Vec2d(xpos, -0.5 * ppd));
         verts->push_back(osg::Vec2d(xpos, -0.8 * ppd));
      }
      else if (count == 1)
      {
         verts->push_back(osg::Vec2d(xpos, -0.55 * ppd));
         verts->push_back(osg::Vec2d(xpos, -0.75 * ppd));
      }
      ++count;
      if (count > 1)
      {
         count = 0;
      }
   }
   xform->addChild(CreateHudGeometry(verts, GL_LINES, mHudColor).get());
   headingGroup->addChild(xform.get());

   headingGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(headingGroup.get());
}

// ---------------------------------------------------------------------------------------------------

void wkf::HUD::DrawSpeedRegion()
{
   double ppd = mPixelsPerDegree;

   osg::ref_ptr<osg::Group>     speedGroup(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);

   vertArray->push_back(osg::Vec2(-8.0 * ppd, 0.5 * ppd));
   vertArray->push_back(osg::Vec2(-5.0 * ppd, 0.5 * ppd));
   vertArray->push_back(osg::Vec2(-5.0 * ppd, 2.0 * ppd));
   vertArray->push_back(osg::Vec2(-8.0 * ppd, 2.0 * ppd));

   speedGroup->addChild(CreateHudGeometry(vertArray, GL_LINE_LOOP, mHudColor).get());

   int fontSizeLarger  = 13 * mFontSizeMult;
   int fontSizeSmaller = 8 * mFontSizeMult;

   std::stringstream kcas;
   if (mPlatData.kcasValid)
   {
      kcas << std::setw(4) << (int)mPlatData.kcas;
   }
   else
   {
      kcas << "XXX";
   }
   speedGroup->addChild(AddTextItem(kcas.str(), -5.3 * ppd, 1.25 * ppd, fontSizeLarger, osgText::Text::RIGHT_CENTER).get());

   std::stringstream ktas;
   ktas << "GS " << std::setw(4) << (int)mPlatData.ktas;

   speedGroup->addChild(AddTextItem(ktas.str(), -5.3 * ppd, 2.5 * ppd, fontSizeSmaller, osgText::Text::RIGHT_CENTER).get());

   speedGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(speedGroup.get());
}

void wkf::HUD::DrawAltitudeRegion()
{
   double ppd = mPixelsPerDegree;

   double sy = 80.0;

   osg::ref_ptr<osg::Group>     altitudeGroup(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);

   vertArray->push_back(osg::Vec2(5.0 * ppd, 0.5 * ppd));
   vertArray->push_back(osg::Vec2(9.0 * ppd, 0.5 * ppd));
   vertArray->push_back(osg::Vec2(9.0 * ppd, 2.0 * ppd));
   vertArray->push_back(osg::Vec2(5.0 * ppd, 2.0 * ppd));

   altitudeGroup->addChild(CreateHudGeometry(vertArray, GL_LINE_LOOP, mHudColor).get());

   int fontSizeLarge  = 13 * mFontSizeMult;
   int fontSizeMiddle = 11 * mFontSizeMult;
   int fontSizeSmall  = 9 * mFontSizeMult;

   int ft  = (int)mPlatData.altitudeMSL_ft;
   int kft = ft / 1000;
   ft -= (kft * 1000);

   // Note: If altitude is >= 100,000 feet it will only display xx,xxx (hundred-thousands will not be shown)
   sy -= 5;
   if (kft >= 1)
   {
      std::stringstream kftstr;
      kftstr << std::setw(3) << kft;
      std::stringstream ftstr;
      ftstr << std::setw(3) << std::setfill('0') << ft;

      altitudeGroup->addChild(
         AddTextItem(kftstr.str(), 6.7 * ppd, 1.25 * ppd, fontSizeLarge, osgText::Text::RIGHT_CENTER).get());

      altitudeGroup->addChild(
         AddTextItem(ftstr.str(), 8.6 * ppd, 1.25 * ppd, fontSizeMiddle, osgText::Text::AlignmentType::RIGHT_CENTER).get());
   }
   else
   {
      std::stringstream ftstr;
      ftstr << std::setw(3) << ft;
      altitudeGroup->addChild(
         AddTextItem(ftstr.str(), 8.6 * ppd, 1.25 * ppd, fontSizeMiddle, osgText::Text::AlignmentType::RIGHT_CENTER).get());
   }

   std::stringstream vs;
   vs << "VS" << std::showpos << std::setw(6) << (int)mPlatData.verticalSpeed_fpm << std::noshowpos;
   altitudeGroup->addChild(
      AddTextItem(vs.str(), 8.6 * ppd, 2.5 * ppd, fontSizeSmall, osgText::Text::AlignmentType::RIGHT_CENTER).get());

   // Planned improvement: (if desiring an explicit plus sign)
   //
   //   if (mPlatData.verticalSpeed_fpm > 0)
   //   {
   //      vs << "VS +" << std::setw(5) << (int)mPlatData.verticalSpeed_fpm;
   //      altitudeGroup->addChild(AddTextItem(vs.str(), 8.6*ppd, 1.5*ppd, fontSizeSmall,
   //      osgText::Text::AlignmentType::RIGHT_CENTER).get());
   //   }
   //   else
   //   {
   //      vs << "VS " << std::setw(6) << (int)mPlatData.verticalSpeed_fpm;
   //      altitudeGroup->addChild(AddTextItem(vs.str(), 8.6*ppd, 1.5*ppd, fontSizeSmall,
   //      osgText::Text::AlignmentType::RIGHT_CENTER).get());
   //   }

   // Planned improvement: (this will be for radar altitude, in addition to nominal baro altitude)
   //   std::stringstream ra;
   //   ra << "RA " << std::setw(6) << (int)mPlatData.altitudeMSL_ft;
   //   altitudeGroup->addChild(AddTextItem(ra.str(), sx, sy - 20.0, fontSizeSmall).get());

   altitudeGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(altitudeGroup.get());
}

void wkf::HUD::DrawLeftRegion()
{
   switch (mHudMode)
   {
   case wkf::HUD_DataContainer::eHUD_NAV_MODE:
      DrawLeftRegionNavWaypt();
      break;
   case wkf::HUD_DataContainer::eHUD_WPN_MODE:
      DrawLeftRegionMissile();
      DrawLeftRegionWeapon();
      break;
   default:
      // Do nothing
      break;
   }
}

void wkf::HUD::DrawLeftRegionNavWaypt()
{
   double ppd = mPixelsPerDegree;

   osg::ref_ptr<osg::Group> leftGroup(new osg::Group);

   int fontSize = 8 * mFontSizeMult;

   double y = -0.5 * ppd;

   DrawAlphaSymbol(leftGroup, -5.3 * ppd - 2.5 * ppd, y, ppd * 0.5, ppd * 0.5);
   std::stringstream alpha;
   if (mPlatData.alphaValid)
   {
      alpha << std::fixed << std::setprecision(1) << std::setw(6) << mPlatData.alpha_deg;
   }
   else
   {
      alpha << std::fixed << std::setprecision(1) << std::setw(6) << "XXX";
   }
   leftGroup->addChild(AddTextItem(alpha.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd; // Blank line

   y -= ppd;
   std::stringstream mach;
   mach << "M " << std::fixed << std::setprecision(2) << std::setw(5) << mPlatData.mach;
   leftGroup->addChild(AddTextItem(mach.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream g;
   if (mPlatData.nzValid)
   {
      g << "G " << std::fixed << std::setprecision(1) << std::setw(5) << mPlatData.gLoad;
   }
   else
   {
      g << "G " << std::fixed << std::setprecision(1) << std::setw(5) << "XXX";
   }
   leftGroup->addChild(AddTextItem(g.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   y -= ppd;
   y -= ppd;

   y -= ppd;
   std::stringstream wp;
   wp << "NAV WAYPT";
   leftGroup->addChild(AddTextItem(wp.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream cm;
   cm << "C" << std::setw(3) << mPlatData.numChaff << " F" << std::setw(3) << mPlatData.numFlares;
   leftGroup->addChild(AddTextItem(cm.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream ab;
   int               iAB = (int)(mPlatData.engineThrottleNormalizedAfterburner * 100.0);
   if (iAB > 0)
   {
      ab << "AB " << std::setw(3) << iAB;
      leftGroup->addChild(AddTextItem(ab.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());
   }

   y -= ppd;
   std::stringstream sb;
   int               iSB = (int)(mPlatData.speedBrakeNormalized * 100.0);
   if (iSB > 0)
   {
      sb << "SB " << std::setfill('0') << std::setw(2) << iSB;
      leftGroup->addChild(AddTextItem(sb.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());
   }

   leftGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(leftGroup);
}

void wkf::HUD::DrawLeftRegionMissile()
{
   DrawLeftRegionCommonWeapon();
}

void wkf::HUD::DrawLeftRegionCommonWeapon()
{
   double ppd = mPixelsPerDegree;

   osg::ref_ptr<osg::Group> leftGroup(new osg::Group);

   int fontSize = 8 * mFontSizeMult;

   double y = -0.5 * ppd;

   DrawAlphaSymbol(leftGroup, -5.3 * ppd - 2.5 * ppd, y, ppd * 0.5, ppd * 0.5);
   std::stringstream alpha;
   alpha << std::fixed << std::setprecision(1) << std::setw(6) << mPlatData.alpha_deg;
   leftGroup->addChild(AddTextItem(alpha.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd; // Blank line

   y -= ppd;
   std::stringstream mach;
   mach << "M " << std::fixed << std::setprecision(2) << std::setw(5) << mPlatData.mach;
   leftGroup->addChild(AddTextItem(mach.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream g;
   g << "G " << std::fixed << std::setprecision(1) << std::setw(5) << mPlatData.gLoad;
   leftGroup->addChild(AddTextItem(g.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   y -= ppd;
   y -= ppd;
   y -= ppd;

   y -= ppd;
   std::stringstream cm;
   cm << "C" << std::setw(3) << mPlatData.numChaff << " F" << std::setw(3) << mPlatData.numFlares;
   leftGroup->addChild(AddTextItem(cm.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream ab;
   int               iAB = (int)(mPlatData.engineThrottleNormalizedAfterburner * 100.0);
   if (iAB > 0)
   {
      ab << "AB " << std::setw(3) << iAB;
      leftGroup->addChild(AddTextItem(ab.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());
   }

   y -= ppd;
   std::stringstream sb;
   int               iSB = (int)(mPlatData.speedBrakeNormalized * 100.0);
   if (iSB > 0)
   {
      sb << "SB " << std::setfill('0') << std::setw(2) << iSB;
      leftGroup->addChild(AddTextItem(sb.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());
   }

   leftGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(leftGroup);
}

void wkf::HUD::DrawLeftRegionWeapon()
{
   double ppd = mPixelsPerDegree;

   osg::ref_ptr<osg::Group> leftGroup(new osg::Group);

   int fontSize = 8 * mFontSizeMult;

   double y = -0.5 * ppd;

   DrawAlphaSymbol(leftGroup, -5.3 * ppd - 2.5 * ppd, y, ppd * 0.5, ppd * 0.5);
   std::stringstream alpha;
   alpha << std::fixed << std::setprecision(1) << std::setw(6) << mPlatData.alpha_deg;
   leftGroup->addChild(AddTextItem(alpha.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream beta;
   beta << "B" << std::fixed << std::setprecision(1) << std::setw(6) << mPlatData.beta_deg;
   leftGroup->addChild(AddTextItem(beta.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream mach;
   mach << "M " << std::fixed << std::setprecision(2) << std::setw(5) << mPlatData.mach;
   leftGroup->addChild(AddTextItem(mach.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream g;
   g << "G " << std::fixed << std::setprecision(1) << std::setw(5) << mPlatData.gLoad;
   leftGroup->addChild(AddTextItem(g.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream gAvail;
   gAvail << std::fixed << std::setprecision(1) << std::setw(5) << mPlatData.gAvail;
   leftGroup->addChild(AddTextItem(gAvail.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;

   y -= ppd;
   std::stringstream powerCmd;
   powerCmd << "POW:" << std::fixed << std::setprecision(2) << std::setw(5) << mPlatData.throttleInput;
   leftGroup->addChild(AddTextItem(powerCmd.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   y -= ppd;
   y -= ppd;

   y -= ppd;
   std::stringstream wp;
   wp << "*WEAPON* ";
   leftGroup->addChild(AddTextItem(wp.str(), -5.3 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   leftGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(leftGroup);
}

void wkf::HUD::DrawRightRegion()
{
   switch (mHudMode)
   {
   case wkf::HUD_DataContainer::eHUD_NAV_MODE:
      DrawRightRegionNavWaypt();
      break;
   case wkf::HUD_DataContainer::eHUD_WPN_MODE:
      DrawRightRegionMissile();
      break;
   default:
      // Do nothing
      break;
   }
}

void wkf::HUD::DrawAlphaSymbol(osg::ref_ptr<osg::Group>& aGroup, double aX, double aY, double aWidth, double aHeight)
{
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);

   double dx = aWidth * 0.5;
   double dy = aHeight * 0.5;

   vertArray->push_back(osg::Vec2(aX + dx, aY + dy));
   vertArray->push_back(osg::Vec2(aX + 0.250 * dx, aY - 0.667 * dy));
   vertArray->push_back(osg::Vec2(aX - 0.250 * dx, aY - dy));
   vertArray->push_back(osg::Vec2(aX - 0.750 * dx, aY - 0.667 * dy));
   vertArray->push_back(osg::Vec2(aX - dx, aY));
   vertArray->push_back(osg::Vec2(aX - 0.750 * dx, aY + 0.667 * dy));
   vertArray->push_back(osg::Vec2(aX - 0.250 * dx, aY + dy));
   vertArray->push_back(osg::Vec2(aX + 0.250 * dx, aY + 0.667 * dy));
   vertArray->push_back(osg::Vec2(aX + dx, aY - dy));

   aGroup->addChild(CreateHudGeometry(vertArray, GL_LINE_STRIP, mHudColor).get());
}

void wkf::HUD::DrawRightRegionNavWaypt()
{
   double ppd = mPixelsPerDegree;

   int fontSize = 8 * mFontSizeMult;

   osg::ref_ptr<osg::Group> rightGroup(new osg::Group);

   double            y = -0.5 * ppd;
   std::stringstream rdrAlt;
   rdrAlt << "RA " << std::setw(6) << (int)mPlatData.radarAltitude_ft;
   rightGroup->addChild(
      AddTextItem(rdrAlt.str(), 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());

   y -= ppd * 2; // Two blank lines

   if ((mPlatData.numWaypoints > 0) && (mPlatData.currentWaypoint < mPlatData.numWaypoints))
   {
      y -= ppd;
      std::stringstream waypt;
      waypt << "WPT " << std::setw(3) << (int)mPlatData.currentWaypoint;
      rightGroup->addChild(
         AddTextItem(waypt.str(), 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());

      y -= ppd;
      if (mPlatData.wayptDataValid)
      {
         int hdg = (int)mPlatData.headingBug_deg;
         if (hdg <= 0)
         {
            hdg += 360;
         }
         if (hdg > 360)
         {
            hdg -= 360;
         }

         std::stringstream wayHdg;
         wayHdg << "HDG " << std::setw(3) << hdg;
         rightGroup->addChild(
            AddTextItem(wayHdg.str(), 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());
      }

      y -= ppd;
      if (mPlatData.wayptDataValid)
      {
         std::stringstream wayRng;
         wayRng << "RNG " << std::fixed << std::setw(5) << std::setprecision(1) << mPlatData.distanceToWaypoint_nm;
         rightGroup->addChild(
            AddTextItem(wayRng.str(), 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());
         y -= ppd;
         std::string timeStr = GetTimeToString(mPlatData.timeToWaypoint_sec / 3600.0); // Convert to hours
         rightGroup->addChild(
            AddTextItem(timeStr, 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());
      }
   }
   else
   {
      y -= ppd * 4;
   }

   y -= ppd * 2; // Two blank lines

   y -= ppd;
   std::string mode = GetStringForHudMode();
   rightGroup->addChild(AddTextItem(mode, 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());

   // Control Mode
   y -= ppd;
   std::string pilotType = GetPilotTypeString();
   rightGroup->addChild(AddTextItem(pilotType, 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());

   rightGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(rightGroup);
}

void wkf::HUD::DrawRightRegionMissile()
{
   double ppd = mPixelsPerDegree;

   int fontSize = 8 * mFontSizeMult;

   osg::ref_ptr<osg::Group> rightGroup(new osg::Group);

   double            y = -0.5 * ppd;
   std::stringstream jam;
   if (mPlatData.jammingDetected)
   {
      jam << "**JAM**";
      rightGroup->addChild(
         AddTextItem(jam.str(), 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());
   }

   y -= ppd * 2.0; // Two blank lines

   y -= ppd;
   std::string mode = GetStringForHudMode();
   rightGroup->addChild(AddTextItem(mode, 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());

   // Control Mode
   y -= ppd;
   std::string pilotType = GetPilotTypeString();
   rightGroup->addChild(AddTextItem(pilotType, 8.6 * ppd, y, fontSize, osgText::Text::AlignmentType::RIGHT_CENTER).get());

   rightGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(rightGroup);
}

void wkf::HUD::DrawTargetBoxOnHud()
{
   if (mPlatData.engagedTargetValid)
   {
      double ppd = mPixelsPerDegree;
      double cx  = mPlatData.engagedTargetData.targetAz_deg * ppd;
      double cy  = mPlatData.engagedTargetData.targetEl_deg * ppd;

      osg::ref_ptr<osg::Group>     boxGroup(new osg::Group);
      osg::ref_ptr<osg::Vec2Array> boxArray(new osg::Vec2Array);
      boxArray->push_back(osg::Vec2(cx - 0.6 * ppd, cy - 0.6 * ppd));
      boxArray->push_back(osg::Vec2(cx + 0.6 * ppd, cy - 0.6 * ppd));
      boxArray->push_back(osg::Vec2(cx + 0.6 * ppd, cy + 0.6 * ppd));
      boxArray->push_back(osg::Vec2(cx - 0.6 * ppd, cy + 0.6 * ppd));
      boxGroup->addChild(CreateHudGeometry(boxArray, GL_LINE_LOOP, mHudColor).get());

      boxGroup->setStateSet(GetStencilDrawWhereMatch().get());
      mHudGeode->addChild(boxGroup.get());
   }
}

void wkf::HUD::DrawPitchLadder()
{
   float ppd = mPixelsPerDegree;

   int fontSize = 8 * mFontSizeMult;

   osg::ref_ptr<osg::MatrixTransform> xform(new osg::MatrixTransform());
   xform->postMult(osg::Matrix::translate(0.0, -mPlatData.pitch_deg * ppd, 0.0));
   xform->postMult(osg::Matrix::rotate(osg::Quat(osg::DegreesToRadians(mPlatData.roll_deg), osg::Vec3(0, 0, 1))));

   // --------------------------------------------------
   // Horizon line
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);

   vertArray->push_back(osg::Vec2(-1.0 * ppd, 0.0));
   vertArray->push_back(osg::Vec2(-20.0 * ppd, 0.0));
   vertArray->push_back(osg::Vec2(1.0 * ppd, 0.0));
   vertArray->push_back(osg::Vec2(20.0 * ppd, 0.0));

   xform->addChild(CreateHudGeometry(vertArray, GL_LINES, mHudColor).get());

   // --------------------------------------------------
   // Upper lines
   osg::ref_ptr<osg::Group> upperLineGroup(new osg::Group);
   std::string              numString[8] = {"10", "20", "30", "40", "50", "60", "70", "80"};
   for (int i = 1; i < 9; ++i)
   {
      double hud_pitchAngle_deg = 10.0 * (double)i;
      double yVal               = hud_pitchAngle_deg * ppd;

      osg::ref_ptr<osg::Vec2Array> vertsOne(new osg::Vec2Array);
      vertsOne->push_back(osg::Vec2(1.0 * ppd, yVal));
      vertsOne->push_back(osg::Vec2(3.5 * ppd, yVal));
      vertsOne->push_back(osg::Vec2(3.5 * ppd, yVal - 0.5 * ppd));
      upperLineGroup->addChild(CreateHudGeometry(vertsOne, GL_LINE_STRIP, mHudColor).get());

      osg::ref_ptr<osg::Vec2Array> vertsTwo(new osg::Vec2Array);
      vertsTwo->push_back(osg::Vec2(-1.0 * ppd, yVal));
      vertsTwo->push_back(osg::Vec2(-3.5 * ppd, yVal));
      vertsTwo->push_back(osg::Vec2(-3.5 * ppd, yVal - 0.5 * ppd));
      upperLineGroup->addChild(CreateHudGeometry(vertsTwo, GL_LINE_STRIP, mHudColor).get());

      upperLineGroup->addChild(
         AddTextItem(numString[i - 1], -4.0 * ppd, yVal - 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
      upperLineGroup->addChild(
         AddTextItem(numString[i - 1], 4.0 * ppd, yVal - 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
   }

   xform->addChild(upperLineGroup.get());

   // Extra 5-deg line
   double      hud_pitchAngle_deg = 5.0;
   double      yVal               = hud_pitchAngle_deg * ppd;
   std::string numStr             = "5";

   osg::ref_ptr<osg::Group>     fiveDegLine(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> firstSet(new osg::Vec2Array);
   firstSet->push_back(osg::Vec2(1.0 * ppd, yVal));
   firstSet->push_back(osg::Vec2(3.5 * ppd, yVal));
   firstSet->push_back(osg::Vec2(3.5 * ppd, yVal - 0.5 * ppd));
   fiveDegLine->addChild(CreateHudGeometry(firstSet, GL_LINE_STRIP, mHudColor));

   osg::ref_ptr<osg::Vec2Array> secondSet(new osg::Vec2Array);
   secondSet->push_back(osg::Vec2(-1.0 * ppd, yVal));
   secondSet->push_back(osg::Vec2(-3.5 * ppd, yVal));
   secondSet->push_back(osg::Vec2(-3.5 * ppd, yVal - 0.5 * ppd));
   fiveDegLine->addChild(CreateHudGeometry(secondSet, GL_LINE_STRIP, mHudColor));

   fiveDegLine->addChild(AddTextItem(numStr, -4.0 * ppd, yVal - 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
   fiveDegLine->addChild(AddTextItem(numStr, 4.0 * ppd, yVal - 0.0, fontSize, osgText::Text::CENTER_CENTER).get());

   xform->addChild(fiveDegLine.get());

   // --------------------------------------------------
   // Lower lines
   osg::ref_ptr<osg::Group>       lowerLineGroup(new osg::Group);
   osg::ref_ptr<osg::Geode>       lowerLineStateGeode(new osg::Geode);
   osg::ref_ptr<osg::StateSet>    lowerLineStateSet(new osg::StateSet);
   osg::ref_ptr<osg::LineStipple> lowerLineStipple(new osg::LineStipple);
   lowerLineStipple->setPattern((GLushort)0x0F0F);
   lowerLineStateSet->setAttribute(lowerLineStipple.get());
   lowerLineStateSet->setMode(GL_LINE_STIPPLE, osg::StateAttribute::ON);
   lowerLineStateGeode->setStateSet(lowerLineStateSet.get());

   for (int i = 1; i < 9; ++i)
   {
      double hud_pitchAngle_deg = 10.0 * (double)i;
      double yVal               = -hud_pitchAngle_deg * ppd;

      osg::ref_ptr<osg::Vec2Array> vertsOne(new osg::Vec2Array);
      vertsOne->push_back(osg::Vec2(1.0 * ppd, yVal));
      vertsOne->push_back(osg::Vec2(3.5 * ppd, yVal));
      vertsOne->push_back(osg::Vec2(3.5 * ppd, yVal + 0.5 * ppd));
      lowerLineGroup->addChild(CreateHudGeometry(vertsOne, GL_LINE_STRIP, mHudColor).get());

      osg::ref_ptr<osg::Vec2Array> vertsTwo(new osg::Vec2Array);
      vertsTwo->push_back(osg::Vec2(-1.0 * ppd, yVal));
      vertsTwo->push_back(osg::Vec2(-3.5 * ppd, yVal));
      vertsTwo->push_back(osg::Vec2(-3.5 * ppd, yVal + 0.5 * ppd));
      lowerLineGroup->addChild(CreateHudGeometry(vertsTwo, GL_LINE_STRIP, mHudColor).get());

      lowerLineGroup->addChild(
         AddTextItem(numString[i - 1], -4.0 * ppd, yVal + 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
      lowerLineGroup->addChild(
         AddTextItem(numString[i - 1], 4.0 * ppd, yVal + 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
   }

   lowerLineStateGeode->addChild(lowerLineGroup.get());

   // Extra 5-deg line
   hud_pitchAngle_deg = -5.0;
   yVal               = hud_pitchAngle_deg * ppd;

   osg::ref_ptr<osg::Group>     bottomFiveDegLine(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> bottomFirstSet(new osg::Vec2Array);
   bottomFirstSet->push_back(osg::Vec2(1.0 * ppd, yVal));
   bottomFirstSet->push_back(osg::Vec2(3.5 * ppd, yVal));
   bottomFirstSet->push_back(osg::Vec2(3.5 * ppd, yVal + 0.5 * ppd));
   bottomFiveDegLine->addChild(CreateHudGeometry(bottomFirstSet, GL_LINE_STRIP, mHudColor));

   osg::ref_ptr<osg::Vec2Array> bottomSecondSet(new osg::Vec2Array);
   bottomSecondSet->push_back(osg::Vec2(-1.0 * ppd, yVal));
   bottomSecondSet->push_back(osg::Vec2(-3.5 * ppd, yVal));
   bottomSecondSet->push_back(osg::Vec2(-3.5 * ppd, yVal + 0.5 * ppd));
   bottomFiveDegLine->addChild(CreateHudGeometry(bottomSecondSet, GL_LINE_STRIP, mHudColor));

   bottomFiveDegLine->addChild(AddTextItem(numStr, -4.0 * ppd, yVal + 0.0, fontSize, osgText::Text::CENTER_CENTER).get());
   bottomFiveDegLine->addChild(AddTextItem(numStr, 4.0 * ppd, yVal + 0.0, fontSize, osgText::Text::CENTER_CENTER).get());

   lowerLineStateGeode->addChild(bottomFiveDegLine.get());

   xform->addChild(lowerLineStateGeode.get());
   xform->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(xform.get());
}

std::string wkf::HUD::GetStringForHudMode() const
{
   std::string mode;

   switch (mHudMode)
   {
   case wkf::HUD_DataContainer::eHUD_NAV_MODE:
      mode = "NAV MODE";
      break;
   case wkf::HUD_DataContainer::eHUD_WPN_MODE:
      mode = "WPN MODE";
      break;
   default:
      mode = "XXX";
      break;
   }

   return mode;
}

std::string wkf::HUD::GetPilotTypeString() const
{
   std::string pilotType = "XXX";

   if (mPlatData.pilotType == "Guidance_BTT")
   {
      pilotType = "Guidance";
   }
   else if (mPlatData.pilotType == "Guidance_STT")
   {
      pilotType = "Guidance";
   }
   else if (mPlatData.pilotType == "Autopilot_BTT")
   {
      pilotType = "Autopilot";
   }
   else if (mPlatData.pilotType == "Autopilot_STT")
   {
      pilotType = "Autopilot";
   }
   else if (mPlatData.pilotType == "ManualPilot")
   {
      pilotType = "Manual";
   }
   else if (mPlatData.pilotType == "Manual-CAS")
   {
      pilotType = "CAS";
   }
   else if (mPlatData.pilotType == "Manual-Simple")
   {
      pilotType = "FCS-S";
   }
   else if (mPlatData.pilotType == "SyntheticPilot")
   {
      pilotType = "Synthetic";
   }

   return pilotType;
}

std::string wkf::HUD::GetTimeToString(double aTimeRemaining_hr) const
{
   int iHours   = 0;
   int iMinutes = 0;
   int iSeconds = 0;

   if (mPlatData.fuelFlow_pph > 0.0)
   {
      iHours                   = (int)aTimeRemaining_hr;
      double timeRemaining_min = (aTimeRemaining_hr - (double)iHours) * 60.0;

      iMinutes                 = (int)timeRemaining_min;
      double timeRemaining_sec = (timeRemaining_min - (double)iMinutes) * 60.0;

      iSeconds = (int)timeRemaining_sec;
   }

   std::stringstream timeStr;
   timeStr << "T" << std::setw(2) << iHours << ":" << std::setw(2) << std::setfill('0') << iMinutes << ":"
           << std::setw(2) << std::setfill('0') << iSeconds;

   std::string retString = timeStr.str();
   return retString;
}

void wkf::HUD::DrawRollPointer()
{
   float ppd = mPixelsPerDegree;

   // Fixed roll ticks
   osg::ref_ptr<osg::MatrixTransform> xformTicks(new osg::MatrixTransform());
   osg::ref_ptr<osg::Vec2Array>       vertArrayTicks(new osg::Vec2Array);
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(-60.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(-60.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.7 * sin(-60.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.7 * cos(-60.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(-40.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(-40.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.7 * sin(-40.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.7 * cos(-40.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(-20.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(-20.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.7 * sin(-20.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.7 * cos(-20.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(-10.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(-10.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.4 * sin(-10.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.4 * cos(-10.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(
      osg::Vec2(8.0 * sin(0.0 * UtMath::cRAD_PER_DEG) * ppd, -4.0 * ppd - 8.0 * cos(0.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(
      osg::Vec2(8.7 * sin(0.0 * UtMath::cRAD_PER_DEG) * ppd, -4.0 * ppd - 8.7 * cos(0.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(10.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(10.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.4 * sin(10.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.4 * cos(10.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(20.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(20.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.7 * sin(20.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.7 * cos(20.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(40.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(40.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.7 * sin(40.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.7 * cos(40.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.0 * sin(60.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.0 * cos(60.0 * UtMath::cRAD_PER_DEG) * ppd));
   vertArrayTicks->push_back(osg::Vec2(8.7 * sin(60.0 * UtMath::cRAD_PER_DEG) * ppd,
                                       -4.0 * ppd - 8.7 * cos(60.0 * UtMath::cRAD_PER_DEG) * ppd));
   xformTicks->addChild(CreateHudGeometry(vertArrayTicks, GL_LINES, mHudColor).get());
   xformTicks->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(xformTicks.get());

   // Movable roll pointer
   double roll_deg = mPlatData.roll_deg;

   // Only draw pointer with limited roll (60 deg)
   if (fabs(roll_deg) < 60.0)
   {
      osg::ref_ptr<osg::MatrixTransform> xformArrow(new osg::MatrixTransform());
      xformArrow->postMult(osg::Matrix::rotate(osg::Quat(osg::DegreesToRadians(mPlatData.roll_deg), osg::Vec3(0, 0, 1))));
      xformArrow->postMult(osg::Matrix::translate(0.0, -4.0 * ppd, 0.0));
      osg::ref_ptr<osg::Vec2Array> vertArrayArrow(new osg::Vec2Array);
      vertArrayArrow->push_back(osg::Vec2(0.00 * ppd, -8.0 * ppd));
      vertArrayArrow->push_back(osg::Vec2(0.15 * ppd, -7.6 * ppd));
      vertArrayArrow->push_back(osg::Vec2(-0.15 * ppd, -7.6 * ppd));
      xformArrow->addChild(CreateHudGeometry(vertArrayArrow, GL_LINE_LOOP, mHudColor).get());
      xformArrow->setStateSet(GetStencilDrawWhereMatch().get());
      mHudGeode->addChild(xformArrow.get());
   }
}

void wkf::HUD::DrawWeaponSelected(std::string aWeaponString, bool aDrawX)
{
   // This is only drawn when in WPN mode
   if (mHudMode == wkf::HUD_DataContainer::eHUD_WPN_MODE)
   {
      float ppd = mPixelsPerDegree;

      int fontSize = 12 * mFontSizeMult;

      osg::ref_ptr<osg::MatrixTransform> xform(new osg::MatrixTransform());
      xform->postMult(osg::Matrix::translate(0.0, -9.0 * ppd, 0.0));

      osg::ref_ptr<osg::Group> weaponGroup(new osg::Group);
      weaponGroup->addChild(AddTextItem(aWeaponString, 0.0, 0.0, fontSize, osgText::Text::CENTER_CENTER).get());

      if (aDrawX)
      {
         osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);
         verts->push_back(osg::Vec2(-2.0 * ppd, 0.6 * ppd));
         verts->push_back(osg::Vec2(2.0 * ppd, -0.6 * ppd));
         verts->push_back(osg::Vec2(-2.0 * ppd, -0.6 * ppd));
         verts->push_back(osg::Vec2(2.0 * ppd, 0.6 * ppd));
         weaponGroup->addChild(CreateHudGeometry(verts, GL_LINES, mHudColor).get());
      }

      xform->addChild(weaponGroup.get());

      xform->setStateSet(GetStencilDrawWhereMatch().get());
      mHudGeode->addChild(xform.get());
   }
}

void wkf::HUD::DrawBingo()
{
   if (mSlowBlink)
   {
      float ppd = mPixelsPerDegree;

      int fontSize = 12 * mFontSizeMult;

      osg::ref_ptr<osg::MatrixTransform> xform(new osg::MatrixTransform());
      xform->postMult(osg::Matrix::translate(0.0, -10.5 * ppd, 0.0));

      osg::ref_ptr<osg::Group> bingoGroup(new osg::Group);
      std::string              bingoStr = "BINGO";
      bingoGroup->addChild(AddTextItem(bingoStr, 0.0, 0.0, fontSize, osgText::Text::CENTER_CENTER).get());

      xform->addChild(bingoGroup.get());

      xform->setStateSet(GetStencilDrawWhereMatch().get());
      mHudGeode->addChild(xform.get());
   }
}

void wkf::HUD::DrawShoot()
{
   if (mFastBlink)
   {
      float ppd = mPixelsPerDegree;

      int fontSize = 12 * mFontSizeMult;

      osg::ref_ptr<osg::MatrixTransform> xform(new osg::MatrixTransform());
      xform->postMult(osg::Matrix::translate(0.0, 4.0 * ppd, 0.0));

      osg::ref_ptr<osg::Group> shootGroup(new osg::Group);
      std::string              shootStr = "SHOOT";
      shootGroup->addChild(AddTextItem(shootStr, 0.0, 0.0, fontSize, osgText::Text::CENTER_CENTER).get());

      xform->addChild(shootGroup.get());

      xform->setStateSet(GetStencilDrawWhereMatch().get());
      mHudGeode->addChild(xform.get());
   }
}

osg::ref_ptr<osg::Geode> wkf::HUD::CreateHudGeometry(osg::ref_ptr<osg::Vec2Array>       aArray,
                                                     int                                aDrawMode,
                                                     const osg::ref_ptr<osg::Vec4Array> aColor)
{
   osg::ref_ptr<osg::Geometry> geom(new osg::Geometry);
   osg::ref_ptr<osg::Geode>    retGeode(new osg::Geode);

   osg::ref_ptr<osg::DrawArrays> geomArray(new osg::DrawArrays(aDrawMode, 0, aArray->size()));
   geom->setVertexArray(aArray.get());
   geom->addPrimitiveSet(geomArray.get());

   geom->setColorArray(aColor);
   geom->setColorBinding(osg::Geometry::BIND_OVERALL);

   osg::ref_ptr<osg::StateSet>  lineState(new osg::StateSet);
   osg::ref_ptr<osg::LineWidth> lineWidthAttr(new osg::LineWidth);
   lineWidthAttr->setWidth(1.0 * mLineWidthMult);
   lineState->setAttribute(lineWidthAttr.get());
   lineState->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   lineState->setMode(GL_BLEND, osg::StateAttribute::ON);
   retGeode->setStateSet(lineState.get());

   retGeode->addDrawable(geom.get());
   return retGeode;
}

osg::ref_ptr<osg::Geode> wkf::HUD::AddTextItem(std::string                   aStr,
                                               float                         aXPos,
                                               float                         aYPos,
                                               int                           aFontSize,
                                               osg::ref_ptr<osg::Vec4Array>& aColor,
                                               osgText::Text::AlignmentType  aAlignment)
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
   textItem->setColor((*aColor)[0]);

   osg::ref_ptr<osg::Geode> geoNode(new osg::Geode);
   geoNode->addDrawable(textItem);
   return geoNode;
}

osg::ref_ptr<osg::Geode>
wkf::HUD::AddTextItem(std::string aStr, float aXPos, float aYPos, int aFontSize, osgText::Text::AlignmentType aAlignment)
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
   textItem->setColor((*mHudColor)[0]);

   osg::ref_ptr<osg::Geode> geoNode(new osg::Geode);
   geoNode->addDrawable(textItem);
   return geoNode;
}

void wkf::HUD::DrawUFC()
{
   float ppd = mPixelsPerDegree;

   double cx = 0.0 * ppd;
   double cy = -18.0 * ppd;

   osg::ref_ptr<osg::Geode>     vectorGeode(new osg::Geode);
   osg::ref_ptr<osg::Group>     vectorGroup(new osg::Group);
   osg::ref_ptr<osg::Vec2Array> verts(new osg::Vec2Array);

   // Transformation
   osg::ref_ptr<osg::PositionAttitudeTransform> vectorXform(new osg::PositionAttitudeTransform());
   vectorXform->setPosition(osg::Vec3d(cx, cy, 0.0));

   // Set background color
   osg::ref_ptr<osg::Vec4Array> backgroundColor(new osg::Vec4Array);
   backgroundColor->push_back(osg::Vec4(0.0, 0.0, 0.0, 0.3));

   // UFC background
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);
   vertArray->push_back(osg::Vec2d(-14.0 * ppd, -3.0 * ppd));
   vertArray->push_back(osg::Vec2d(14.0 * ppd, -3.0 * ppd));
   vertArray->push_back(osg::Vec2d(14.0 * ppd, 1.0 * ppd));
   vertArray->push_back(osg::Vec2d(12.0 * ppd, 3.0 * ppd));
   vertArray->push_back(osg::Vec2d(-12.0 * ppd, 3.0 * ppd));
   vertArray->push_back(osg::Vec2d(-14.0 * ppd, 1.0 * ppd));
   vectorGeode->addChild(CreateHudGeometry(vertArray, GL_POLYGON, backgroundColor).get());

   // Green color
   osg::ref_ptr<osg::Vec4Array> greenColor(new osg::Vec4Array);
   greenColor->push_back(osg::Vec4(0.0, 0.9, 0.3, 0.7));

   // Yellow color
   osg::ref_ptr<osg::Vec4Array> yellowColor(new osg::Vec4Array);
   yellowColor->push_back(osg::Vec4(0.9, 0.8, 0.0, 0.7));

   // Red color
   osg::ref_ptr<osg::Vec4Array> redColor(new osg::Vec4Array);
   redColor->push_back(osg::Vec4(0.9, 0.2, 0.0, 0.7));

   // Engine MIL ring
   osg::ref_ptr<osg::Vec2Array> ringArray(new osg::Vec2Array);
   for (int ang_deg = 90; ang_deg >= -180; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = (-10.0 * ppd) + (2.0 * ppd) * cos(ang_rad);
      double y       = (2.0 * ppd) * sin(ang_rad);
      ringArray->push_back(osg::Vec2d(x, y));
   }
   vectorGeode->addChild(CreateHudGeometry(ringArray, GL_LINE_STRIP, greenColor).get());

   // Engine AB ring
   osg::ref_ptr<osg::Vec2Array> abRingArray(new osg::Vec2Array);
   for (int ang_deg = -180; ang_deg >= -240; ang_deg -= 10)
   {
      double ang_rad = (double)ang_deg * UtMath::cRAD_PER_DEG;
      double x       = (-10.0 * ppd) + (2.0 * ppd) * cos(ang_rad);
      double y       = (2.0 * ppd) * sin(ang_rad);
      abRingArray->push_back(osg::Vec2d(x, y));
   }
   vectorGeode->addChild(CreateHudGeometry(abRingArray, GL_LINE_STRIP, redColor).get());

   if (mPlatData.throttleValid)
   {
      double tickAng_deg = 0.0;
      if (mPlatData.throttleMilitaryControllerPosition >= 1.0)
      {
         if (mPlatData.throttleAfterburnerControllerPosition > 0.0)
         {
            // Using afterburner
            tickAng_deg = 270.0 + 60.0 * mPlatData.throttleAfterburnerControllerPosition;
         }
         else
         {
            // No afterburner but full MIL
            tickAng_deg = 270.0;
         }
      }
      else
      {
         tickAng_deg = 270.0 * mPlatData.throttleMilitaryControllerPosition;
      }
      double                             tickAng_rad = tickAng_deg * UtMath::cRAD_PER_DEG;
      osg::ref_ptr<osg::MatrixTransform> tickXform(new osg::MatrixTransform());
      tickXform->postMult(osg::Matrix::rotate(osg::Quat(tickAng_rad, osg::Vec3(0, 0, -1))));
      tickXform->postMult(osg::Matrix::translate(-10.0 * ppd, 0.0, 0.0));
      osg::ref_ptr<osg::Vec2Array> tickArray(new osg::Vec2Array);
      tickArray->push_back(osg::Vec2d(0.0, 2.0 * ppd));
      tickArray->push_back(osg::Vec2d(0.0, 2.4 * ppd));

      if (tickAng_deg > 270.0)
      {
         tickXform->addChild(CreateHudGeometry(tickArray, GL_LINES, redColor).get());
      }
      else
      {
         tickXform->addChild(CreateHudGeometry(tickArray, GL_LINES, greenColor).get());
      }

      vectorGeode->addChild(tickXform.get());

      double needleAng_deg = 0.0;
      if (mPlatData.engineThrottleNormalizedMilitary >= 1.0)
      {
         if (mPlatData.engineThrottleNormalizedAfterburner > 0.0)
         {
            // Using afterburner
            needleAng_deg = 270.0 + 60.0 * mPlatData.engineThrottleNormalizedAfterburner;
         }
         else
         {
            // No afterburner but full MIL
            needleAng_deg = 270.0;
         }
      }
      else
      {
         needleAng_deg = 270.0 * mPlatData.engineThrottleNormalizedMilitary;
      }
      double                             needleAng_rad = needleAng_deg * UtMath::cRAD_PER_DEG;
      osg::ref_ptr<osg::MatrixTransform> needleXform(new osg::MatrixTransform());
      needleXform->postMult(osg::Matrix::rotate(osg::Quat(needleAng_rad, osg::Vec3(0, 0, -1))));
      needleXform->postMult(osg::Matrix::translate(-10.0 * ppd, 0.0, 0.0));
      osg::ref_ptr<osg::Vec2Array> needleArray(new osg::Vec2Array);
      needleArray->push_back(osg::Vec2d(0.0, 0.0));
      needleArray->push_back(osg::Vec2d(0.0, 2.0 * ppd));
      needleArray->push_back(osg::Vec2d(0.0, 2.0 * ppd));
      needleArray->push_back(osg::Vec2d(0.2 * ppd, 1.6 * ppd));
      needleArray->push_back(osg::Vec2d(0.0, 2.0 * ppd));
      needleArray->push_back(osg::Vec2d(-0.2 * ppd, 1.6 * ppd));
      if (needleAng_deg > 270.0)
      {
         needleXform->addChild(CreateHudGeometry(needleArray, GL_LINES, yellowColor).get());
      }
      else
      {
         needleXform->addChild(CreateHudGeometry(needleArray, GL_LINES, greenColor).get());
      }
      vectorGeode->addChild(needleXform.get());
   }
   else
   {
      int               fontSize = 8 * mFontSizeMult;
      std::stringstream throttle;
      double            x = -10.0 * ppd;
      double            y = 0.0 * ppd;
      throttle << "XXX";
      vectorGeode->addChild(AddTextItem(throttle.str(), x, y, fontSize, osgText::Text::CENTER_CENTER));
   }

   int fontSize = 8 * mFontSizeMult;

   double            y = 1.5 * ppd;
   std::stringstream totWgt;
   if (mPlatData.fuelValid)
   {
      totWgt << "WGT " << std::fixed << std::setprecision(1) << std::setw(4) << mPlatData.grossWeight_lbs * 0.001;
   }
   else
   {
      totWgt << "WGT " << std::fixed << std::setprecision(1) << std::setw(4) << "XXX";
   }
   vectorGeode->addChild(AddTextItem(totWgt.str(), 0.0 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream totFuel;
   if (mPlatData.fuelValid)
   {
      totFuel << "TOT " << std::fixed << std::setprecision(1) << std::setw(4)
              << mPlatData.currentTotalFuelQuantity_lbs * 0.001;
   }
   else
   {
      totFuel << "TOT " << std::fixed << std::setprecision(1) << std::setw(4) << "XXX";
   }
   vectorGeode->addChild(AddTextItem(totFuel.str(), 0.0 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream intFuel;
   if (mPlatData.fuelValid)
   {
      intFuel << "INT " << std::fixed << std::setprecision(1) << std::setw(4)
              << mPlatData.currentInternalFuelQuantity_lbs * 0.001;
   }
   else
   {
      intFuel << "INT " << std::fixed << std::setprecision(1) << std::setw(4) << "XXX";
   }
   vectorGeode->addChild(AddTextItem(intFuel.str(), 0.0 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   y -= ppd;
   std::stringstream extFuel;
   if (mPlatData.fuelValid)
   {
      double extFuel_lbs = mPlatData.currentExternalFuelQuantity_lbs;
      extFuel << "EXT " << std::fixed << std::setprecision(1) << std::setw(4) << extFuel_lbs * 0.001;
   }
   else
   {
      extFuel << "EXT " << std::fixed << std::setprecision(1) << std::setw(4) << "XXX";
   }
   vectorGeode->addChild(AddTextItem(extFuel.str(), 0.0 * ppd, y, fontSize, osgText::Text::RIGHT_CENTER).get());

   vectorXform->addChild(vectorGeode.get());
   vectorGroup->addChild(vectorXform.get());
   vectorGroup->setStateSet(GetStencilDrawWhereMatch().get());
   mHudGeode->addChild(vectorGroup.get());
}

void wkf::HUD::DrawConnectionDestroyed()
{
   int h = GetViewer()->GetHeight() / QApplication::desktop()->devicePixelRatio();
   int w = GetViewer()->GetWidth() / QApplication::desktop()->devicePixelRatio();

   // Black color
   osg::ref_ptr<osg::Vec4Array> blackColor(new osg::Vec4Array);
   blackColor->push_back(osg::Vec4(0.0, 0.0, 0.0, 0.5));

   // Red color
   osg::ref_ptr<osg::Vec4Array> redColor(new osg::Vec4Array);
   redColor->push_back(osg::Vec4(0.9, 0.0, 0.0, 1.0));

   // Fade background
   osg::ref_ptr<osg::Vec2Array> bgArray(new osg::Vec2Array);
   bgArray->push_back(osg::Vec2d(-0.5 * w, 0.5 * h));
   bgArray->push_back(osg::Vec2d(0.5 * w, 0.5 * h));
   bgArray->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   bgArray->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   mHudGeode->addChild(CreateHudGeometry(bgArray, GL_POLYGON, blackColor).get());

   // Top left to bottom right slash
   osg::ref_ptr<osg::Vec2Array> x1array(new osg::Vec2Array);
   x1array->push_back(osg::Vec2d(-0.5 * w, 0.5 * h));
   x1array->push_back(osg::Vec2d(-0.42 * w, 0.5 * h));
   x1array->push_back(osg::Vec2d(0.5 * w, -0.5 * h));
   x1array->push_back(osg::Vec2d(0.42 * w, -0.5 * h));
   mHudGeode->addChild(CreateHudGeometry(x1array, GL_POLYGON, redColor).get());

   // Bottom left to top right slash
   osg::ref_ptr<osg::Vec2Array> x2array(new osg::Vec2Array);
   x2array->push_back(osg::Vec2d(-0.42 * w, -0.5 * h));
   x2array->push_back(osg::Vec2d(-0.5 * w, -0.5 * h));
   x2array->push_back(osg::Vec2d(0.42 * w, 0.5 * h));
   x2array->push_back(osg::Vec2d(0.5 * w, 0.5 * h));
   mHudGeode->addChild(CreateHudGeometry(x2array, GL_POLYGON, redColor).get());
}

void wkf::HUD::DrawCircle(double                        aCx,
                          double                        aCy,
                          double                        aRadius,
                          osg::ref_ptr<osg::Vec4Array>& aColor,
                          osg::ref_ptr<osg::Geode>&     aGeode,
                          int                           aNumPts,
                          bool                          aFilled)
{
   osg::ref_ptr<osg::Vec2Array> vertArray(new osg::Vec2Array);

   double deltaAng_rad = UtMath::cTWO_PI / static_cast<double>(aNumPts);
   for (double ang = 0; ang < UtMath::cTWO_PI; ang += deltaAng_rad)
   {
      vertArray->push_back(osg::Vec2d(aCx + aRadius * cos(ang), aCy + aRadius * sin(ang)));
   }

   if (aFilled)
   {
      aGeode->addChild(CreateHudGeometry(vertArray, GL_POLYGON, aColor).get());
   }
   else
   {
      aGeode->addChild(CreateHudGeometry(vertArray, GL_LINE_LOOP, aColor).get());
   }
}
