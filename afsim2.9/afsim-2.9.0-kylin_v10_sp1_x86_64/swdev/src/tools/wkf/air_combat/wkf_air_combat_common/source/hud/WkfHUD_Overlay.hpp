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

#ifndef WKFHUD_OVERLAY_HPP
#define WKFHUD_OVERLAY_HPP

#include "wkf_air_combat_common_export.h"

#include <osg/Group>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/Transform>
#include <osg/ref_ptr>
#include <osgText/Font>
#include <osgText/Text>

#include "UtoRawShape.hpp"
#include "VaOverlay.hpp"
#include "WkfHUD_DataContainer.hpp"

namespace wkf
{
class WKF_AIR_COMBAT_COMMON_EXPORT HUD : public vespa::VaOverlay
{
public:
   HUD();
   explicit HUD(const std::string& aName);
   HUD(const std::string& aName, vespa::VaOverlay::OverlayType aType, double aWindowFullFovY_deg);
   HUD(const HUD& aSrc);
   HUD& operator=(const HUD& aSrc) = delete;
   ~HUD() override;

   /** Provides a clone of the HUD. */
   HUD* Clone() const override;

   /** Initialize the HUD and its data. */
   bool Initialize() override;

   /** Update the HUD. */
   void Update() override;

   /** Set the FOV for the HUD.
    * @param aFovY vertical FOV to set.
    */
   void SetupHudProjection(float aFovY);

   // Access the data directly (no need for protection)
   HUD_DataContainer::PlatformData mPlatData;

   /** This sets the HUD mode.
    * @param aMode mode to set.
    */
   void SetHudMode(HUD_DataContainer::eHudMode aMode) { mHudMode = aMode; }

   /** Return the current HUD mode. */
   enum HUD_DataContainer::eHudMode GetHudMode() const { return mHudMode; }

   /** This sets if the connection to the HUD has been destroyed/ lost (draw x over screen).
    * @param aState state to set.
    */
   void SetConnectionDestroyed(const bool aState) { mConnectionDestroyed = aState; }

   // Planned improvements:
   // void LockRadarTarget(double& aLockX, double& aLockY);
   // void RadarTargetDesignationCoords(double& aX, double& aY);
   // double RadarDisplayRange_nm() { return mRadarRng_nm; }

   // This is called to lock the target closest to the target designation symbol.
   // It returns true if the lock-on was successful, providing the name of the
   // locked platform in aLockedTargetName, otherwise it returns false.
   // bool LockRadarTarget(std::string& aLockedTargetName);

protected:
   class RegionExtents
   {
   public:
      RegionExtents()
         : mX1(0.0)
         , mY1(0.0)
         , mX2(0.0)
         , mY2(0.0)
      {
      }

      RegionExtents(double aX1, double aY1, double aX2, double aY2)
         : mX1(aX1)
         , mY1(aY1)
         , mX2(aX2)
         , mY2(aY2)
      {
      }

      void SetExtents(double aX1, double aY1, double aX2, double aY2)
      {
         mX1 = aX1;
         mY1 = aY1;
         mX2 = aX2;
         mY2 = aY2;
      }

      double X1() const { return mX1; }

      double Y1() const { return mY1; }

      double X2() const { return mX2; }

      double Y2() const { return mY2; }

   protected:
      double mX1;
      double mY1;
      double mX2;
      double mY2;
   };

   /** Draw the debug region.
    * @param aExtents extents to draw to.
    */
   void DrawDebugRegion(const RegionExtents& aExtents);

   /** Draw the HUD.
    * @param aHUD_HalfWidth half the HUD's width.
    * @param aHUD_HalfHeight half the HUD's height.
    * @param aFullFovY_deg vertical fov.
    * @param aLookAngle_Az_deg azimuth look angle.
    * @param aLookAngle_El_deg elevation look angle.
    */
   void Draw(double aHUD_HalfWidth,
             double aHUD_HalfHeight,
             double aFullFovY_deg,
             double aLookAngle_Az_deg,
             double aLookAngle_El_deg);

   /** Draw the heading region of the HUD.
    * @param aPixelsPerDegree how many pixels per degree to use.
    */
   void DrawHeadingRegion(double aPixelsPerDegree);

   /** Draw the waterline symbol. */
   void DrawWaterlineSymbol();

   /** Draw the speed region. */
   void DrawSpeedRegion();

   /** Draw the left region of the HUD. */
   void DrawLeftRegion();

   /** Draw the left region in nav waypoint mode. */
   void DrawLeftRegionNavWaypt();

   /** Draw the left region in missile mode. */
   void DrawLeftRegionMissile();

   /** Draw the left region in common weapon mode. */
   void DrawLeftRegionCommonWeapon();

   /** Draw the left region in weapon test mode. */
   void DrawLeftRegionWeapon();

   /** Draw the alpha symbol. */
   void DrawAlphaSymbol(osg::ref_ptr<osg::Group>& aGroup, double aX, double aY, double aWidth, double aHeight);

   /** Draw the altitude region. */
   void DrawAltitudeRegion();

   /** Draw the right region of the HUD. */
   void DrawRightRegion();

   /** Draw the right region in nav waypoint mode. */
   void DrawRightRegionNavWaypt();

   /** Draw the right region in missile mode. */
   void DrawRightRegionMissile();

   /** Draw a target box on the HUD. */
   void DrawTargetBoxOnHud();

   /** Return the HUD mode as a string. */
   std::string GetStringForHudMode() const;

   /** Return the pilot type as a string. */
   std::string GetPilotTypeString() const;

   /** Return the time remaining as a string.
    * @param aTimeRemaining_hr time remaining in hours.
    */
   std::string GetTimeToString(double aTimeRemaining_hr) const;

   /** Draw the roll pointer. */
   void DrawRollPointer();

   /** Draw the velocity vector. */
   void DrawVelocityVector();

   /** Draw the pitch ladder. */
   void DrawPitchLadder();

   /** Draw the selected weapon.
    * @param aWeaponString weapon name.
    * @param aDrawX true to draw the x, false otherwise.
    */
   void DrawWeaponSelected(std::string aWeaponString, bool aDrawX);

   /** Draw "Bingo". */
   void DrawBingo();

   /** Draw "Shoot". */
   void DrawShoot();

   /** Draw a circle in the center. */
   void DrawCenterCircle();

   /** Draw the stencil buffer region.
    * @param aExtents extents to create the geometry.
    * @param aStencilValue stencil value to use.
    */
   void DrawStencilBufferRegion(const RegionExtents& aExtents, int aStencilValue);

   /** Draw the stencil buffer region.
    * @param aArray array of coords to creat the geometry.
    * @param aStencilValue stencil value to use.
    */
   void DrawStencilBufferRegion(const osg::ref_ptr<osg::Vec2Array> aArray, int aStencilValue);

   /** Draw the up-front controller. */
   void DrawUFC();

   /** Draw a black fade over the screen with a red x, indicating the connection to the platform has been lost/destroyed. */
   void DrawConnectionDestroyed();

   /** Create HUD geometry.
    * @param aArray array of points to use to create the geometry.
    * @param aDrawMode draw mode to use.
    * @param aColor color to use.
    */
   osg::ref_ptr<osg::Geode> CreateHudGeometry(osg::ref_ptr<osg::Vec2Array>       aArray,
                                              int                                aDrawMode,
                                              const osg::ref_ptr<osg::Vec4Array> aColor);

   /** Return a pointer to the stencil state set. */
   osg::ref_ptr<osg::StateSet> GetStencilDrawWhereMatch();

   // MFDs are being moved to the HDD.
   // void DrawLftMFD(wkf::HUD_DataContainer::eMfdMode aMfdMode, bool aActive);
   // void DrawRgtMFD(wkf::HUD_DataContainer::eMfdMode aMfdMode, bool aActive);
   // void DrawMFD_Radar(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_RWR(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_IRST(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_FLIR(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_Datalink(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_Weapons(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_Fuel(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawMFD_Engines(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawCenterTextForUnsupportedDisplay(osg::ref_ptr<osg::Geode>&     aVectorGeode,
   //                                          bool                          aActive,
   //                                          osg::ref_ptr<osg::Vec4Array>& aGridColor,
   //                                          const std::string&            aString1,
   //                                          const std::string&            aString2);
   // void DrawMfdHighlightBox(osg::ref_ptr<osg::Geode>& aVectorGeode, bool aActive);
   // void DrawRadarTarget(osg::ref_ptr<osg::Geode>&          aVectorGeode,
   //                     double                             aCx,
   //                     double                             aCy,
   //                     double                             aOwnshipHeading_deg,
   //                     double                             aTargetHeading_deg,
   //                     int                                aAlt_kft,
   //                     const osg::ref_ptr<osg::Vec4Array> aColor,
   //                     bool                               aPrimaryTgt = false,
   //                     bool                               aSecondaryTgt = false);

private:
   /** Set the colors for the HUD.
    * @param aHudRedColor red color to set.
    * @param aHudGreenColor green color to set.
    * @param aHudBlueColor blue color to set.
    * @param aHudAlphaColor alpha to set.
    */
   void SetHudColors(float aHudRedColor, float aHudGreenColor, float aHudBlueColor, float aHudAlphaColor);

   // Alignment types include:  LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
   //                           CENTER_TOP, CENTER_CENTER, CENTER_BOTTOM,
   //                           RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM,
   //                           LEFT_BASE_LINE, CENTER_BASE_LINE, RIGHT_BASE_LINE,
   //                           LEFT_BOTTOM_BASE_LINE, CENTER_BOTTOM_BASE_LINE, and RIGHT_BOTTOM_BASE_LINE
   /** Add a text item to the HUD.
    * @param aStr text to add.
    * @param aXPos x position of the text.
    * @param aYPos y position of the text.
    * @param aFontSize size of the font.
    * @param aColor color of the text.
    * @param aAlignment alignment of the text.
    */
   osg::ref_ptr<osg::Geode> AddTextItem(std::string                   aStr,
                                        float                         aXPos,
                                        float                         aYPos,
                                        int                           aFontSize,
                                        osg::ref_ptr<osg::Vec4Array>& aColor,
                                        osgText::Text::AlignmentType  aAlignment = osgText::Text::LEFT_BASE_LINE);

   // Alignment types include:  LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
   //                           CENTER_TOP, CENTER_CENTER, CENTER_BOTTOM,
   //                           RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM,
   //                           LEFT_BASE_LINE, CENTER_BASE_LINE, RIGHT_BASE_LINE,
   //                           LEFT_BOTTOM_BASE_LINE, CENTER_BOTTOM_BASE_LINE, and RIGHT_BOTTOM_BASE_LINE
   /** Add a text item to the HUD.
    * @param aStr text to add.
    * @param aXPos x position of the text.
    * @param aYPos y position of the text.
    * @param aFontSize size of the font.
    * @param aAlignment alignment of the text.
    */
   osg::ref_ptr<osg::Geode> AddTextItem(std::string                  aStr,
                                        float                        aXPos,
                                        float                        aYPos,
                                        int                          aFontSize,
                                        osgText::Text::AlignmentType aAlignment = osgText::Text::LEFT_BASE_LINE);

   /** Draw a circle on the HUD.
    * @param aCx x position of the circle.
    * @param aCy y position of the circle.
    * @param aRadius radius of the circle.
    * @param aColor color of the circle.
    * @param aGeode geode to attach to.
    * @param aNumPts points to use for the circle (smoothness).
    * @param aFilled true to fill the circle, false otherwise.
    */
   void DrawCircle(double                        aCx,
                   double                        aCy,
                   double                        aRadius,
                   osg::ref_ptr<osg::Vec4Array>& aColor,
                   osg::ref_ptr<osg::Geode>&     aGeode,
                   int                           aNumPts = 20,
                   bool                          aFilled = false);

   osgText::Font*                     mFont          = nullptr; ///< Pointer to the font object for the HUD.
   osg::ref_ptr<osg::Geode>           mHudGeode      = nullptr; ///< Pointer to the geode used by the HUD.
   osg::ref_ptr<osg::StateSet>        mHudState      = nullptr; ///< Pointer to the state set for the HUD.
   osg::ref_ptr<osg::Vec4Array>       mHudColor      = nullptr; ///< Pointer to the color used by the HUD.
   osg::ref_ptr<osg::Projection>      mHudProjection = nullptr; ///< Pointer to the projection matrix used by the HUD.
   osg::ref_ptr<osg::MatrixTransform> mHudModelViewMatrix = nullptr; ///< Pointer to the model view matrix used by the HUD.
   UtoRawShape*                       mRawShapePtr        = nullptr; ///< Pointer to the raw shape used by the HUD.
   double                             mHUD_HalfWidth      = 0.0;     ///< Half the width of the HUD.
   double                             mHUD_HalfHeight     = 0.0;     ///< Half the height of the HUD.
   double                             mWindowFullFovY_deg = 0.0;     ///< Full vertical FOV of the HUD.
   double                             mHUDFullFovX_deg    = 0.0;     ///< Full horizontal FOV of the HUD.
   double                             mHUDFullFovY_deg    = 0.0;     ///< Full vertical FOV of the HUD.
   RegionExtents                      mUfcRegion;                    ///< Region for the UFC.
   RegionExtents                      mHeadingRegion;                ///< Region for the heading info.
   RegionExtents                      mSpeedRegion;                  ///< Region for the speed info.
   RegionExtents                      mLeftRegion;                   ///< Region for the left-side of the HUD.
   RegionExtents                      mAltitudeRegion;               ///< Region for the altitude info.
   RegionExtents                      mRightRegion;                  ///< Region for the right-side of the HUD.
   float                              mHudRedColor     = 0.0f;       ///< Red color for the HUD.
   float                              mHudGreenColor   = 1.0f;       ///< Green color for the HUD.
   float                              mHudBlueColor    = 0.0f;       ///< Blue color for the HUD.
   float                              mHudAlphaColor   = 1.0f;       ///< Alpha (transparency) for the HUD.
   bool                               mDebugFlag       = false;      ///< True for debug info on, false otherwise.
   float                              mPixelsPerDegree = 0.0f;       ///< Pixels per degree on the HUD.
   HUD_DataContainer::eHudMode        mHudMode =
      HUD_DataContainer::eHUD_NAV_MODE;                      ///< Mode for the HUD. Different modes show different info.
   int                          mFontSizeMult     = 1;       ///< Multiplier for the font size.
   float                        mLineWidthMult    = 1.0f;    ///< Multiplier for the line width.
   osg::ref_ptr<osg::Vec2Array> mHudPolygonRegion = nullptr; ///< Pointer to a polygon region for the HUD.
   bool                         mSlowBlink        = false;   ///< True to slow blink, false otherwise.
   bool                         mMediumBlink      = false;   ///< True to medium blink, false otherwise.
   bool                         mFastBlink        = false;   ///< True to fast blink, false otherwise.
   bool                         mSuperFastBlink   = false;   ///< True to super fast blink, false otherwise.
   bool mConnectionDestroyed = false; ///< True to draw connection destroyed (x over screen), false otherwise.

   // MFD code will be moved into HDD.
   // bool                              mDrawMFDs             = true;
   // RegionExtents                     mMfdLftRegion;
   // RegionExtents                     mMfdRgtRegion;
   // double                            mRadarTgtDesignatorX  = 0.0;
   // double                            mRadarTgtDesignatorY  = 0.0;
   // int                               mRadarRng_nm          = 20;
   // double                            mMfdRadarTime_sec     = 0.0;
   // bool                              mMfdRadarUpdateActive = false;
   // double                            mMfdRwrTime_sec       = 0.0;
   // bool                              mMfdRwrUpdateActive   = false;
   // double                            mMfdIrstTime_sec      = 0.0;
   // bool                              mMfdIrstUpdateActive  = false;
   // double                            mMfdFlirTime_sec      = 0.0;
   // bool                              mMfdFlirUpdateActive  = false;
};
} // namespace wkf
#endif // WKFHUD_OVERLAY_HPP
