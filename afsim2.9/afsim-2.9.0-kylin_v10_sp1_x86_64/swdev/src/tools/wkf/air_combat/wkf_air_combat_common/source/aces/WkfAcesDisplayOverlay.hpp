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

#ifndef ACESDISPLAYOVERLAY_HPP
#define ACESDISPLAYOVERLAY_HPP

#include "wkf_air_combat_common_export.h"

#include <functional>

#include <QElapsedTimer>
#include <QLabel>
#include <QRadioButton>
#include <QRect>
#include <QTreeWidgetItem>
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
#include "WkfPlatform.hpp"
#include "aces/WkfAcesDisplayDataContainer.hpp"

namespace wkf
{
namespace AcesDisplay
{
// Colors for ACES Display (a lot of these colors are shared with the HDD/TSD Page and the SA View)
namespace Color
{
static const osg::Vec4 White(0.9, 0.9, 0.9, 1.0);
static const osg::Vec4 LightGray(0.8, 0.8, 0.8, 1.0);
static const osg::Vec4 Gray(0.5, 0.5, 0.5, 1.0);
static const osg::Vec4 DarkGray(0.04, 0.04, 0.04, 1.0);
static const osg::Vec4 Black(0.0, 0.0, 0.0, 1.0);
static const osg::Vec4 SlateBlue(0.2, 0.51, 0.67, 1.0);
static const osg::Vec4 Cyan(0.0, 1.0, 1.0, 1.0);
static const osg::Vec4 DarkCyan(0.0, 0.5, 0.5, 1.0);
static const osg::Vec4 Magenta(0.85, 0.0, 1.0, 1.0);

static const osg::Vec4 Background(0.04, 0.08, 0.16, 1.0);
static const osg::Vec4 CompassLines(0.15, 0.16, 0.5, 1.0);
static const osg::Vec4 Red(0.863, 0.157, 0.157, 1.0);
static const osg::Vec4 Yellow(0.863, 0.824, 0.314, 1.0);
static const osg::Vec4 Green(0.0, 0.706, 0.235, 1.0);
static const osg::Vec4 BehaviorHistory(0.118, 0.157, 0.157, 1.0);

static const osg::Vec4 EngagementBlue(0.0, 0.392, 0.784, 1.0);
static const osg::Vec4 EngagementRed(0.863, 0.157, 0.157, 1.0);
static const osg::Vec4 EngagementGray(0.65, 0.65, 0.65, 1.0);

static const osg::Vec4 SettingsGrayTransparent(0.25, 0.25, 0.25, 0.9);
} // namespace Color

class WKF_AIR_COMBAT_COMMON_EXPORT Overlay : public vespa::VaOverlay
{
public:
   // This enum is used to index maps for displays and settings,
   // for drawing and retrieving/setting settings.
   enum class DisplayType
   {
      cPFD,
      cSAD,
      cTSD,
      cTWD,
      cENGAGE,
   };

   // This enum is used to draw one of the current circular displays
   // in one of 4 regions; The two main large regions (left, right),
   // and the two secondary regions (top, bottom).
   enum class DrawRegion
   {
      cLEFT,
      cRIGHT,
      cTOP,
      cBOTTOM
   };

   // Container for drawing track data
   struct TrackData
   {
      double tgtBearing_rad;
      double tgtAz_rad;
      double tgtEl_rad;
      double tgtRng_m;
      double tgtSpd_kts;
      double tgtAlt_ft;
      double tgtHdg_rad;
      double tgtRangeRate_fps;
   };

   // Container for drawing routes
   struct WaypointData
   {
      size_t               numWaypoints;
      std::vector<QPointF> waypoints;
      size_t               currentWaypoint;
      double               headingBug_deg;
      double               distToWaypoint_nm;
      double               timeToWaypoint_sec;
   };

   // Container for drawing SAD entities
   struct Entity
   {
      Entity() {}

      // Flags
      bool isTarget{false};
      bool inFlight{false};
      bool isTruth{false};
      bool isGroup{false};

      // Containers for perception data.
      EntityPerception perception;
      EntityTruth      truth;
      PerceivedGroup   groupPerception;

      // Rect for the hover region.
      QRectF hoverRegion;
   };

   // Describes the domain of a TSD Entity, either air or ground.
   enum class TSD_EntityDomain
   {
      AIR,
      GROUND
   };

   // Describes the type of TSD Entity, aka is it populated with
   // track data(TRACK), or perception data(ASSET)?
   enum class TSD_EntityType
   {
      TRACK,
      ASSET
   }; ///< This is used for checking if the entity has track or perception data.

   // Container for drawing TSD entities
   struct TSD_Entity
   {
      TSD_Entity() {}
      TSD_EntityDomain domain{TSD_EntityDomain::AIR}; ///< The domain of this entity, AIR or GROUND.
      TSD_EntityType   type;            ///< The type of this entity, TRACK or ASSET.  This changes data used.
      bool             inFlight{false}; ///< True if the entity is part of this aircrafts flight, false otherwise.
      double           range{-1.0};     ///< Range in nm of a ground entity.  Only use range if domain is GROUND (SAMs).
      TargetTrackItem  track;           ///< Track data for this entity (Master track list).
      EntityPerception perceivedData;   ///< Asset only - this stores perceived data rather than track data.
      QRectF           hoverRegion;     ///< The hover region for this entity, used to hover and view entity info.
   };

   // Container to group button vars.
   struct Button
   {
      QRectF                region;
      std::function<void()> function;
      bool                  clickable{true};
   };

   Overlay(const char* aName, vespa::VaViewer* aViewerPtr, const size_t aSourcePlatformIndex);
   Overlay* Clone() const override;
   ~Overlay() override = default;

   enum class SAD_SETTINGS
   {
      cPERCEPTION      = 1u << 0,  // default true
      cTRUTHGROUND     = 1u << 1,  // default false
      cTRUTHAIR        = 1u << 2,  // default false
      cFRIENDLIES      = 1u << 3,  // default true
      cFLIGHT          = 1u << 4,  // default true
      cPRIORITYTHREATS = 1u << 5,  // default true
      cPRIORITYTARGETS = 1u << 6,  // default true
      cALTITUDE        = 1u << 7,  // default true
      cESMRWR          = 1u << 8,  // default true
      cANGLEONLY       = 1u << 9,  // default true
      cROUTE           = 1u << 10, // default true
      cFOV             = 1u << 11, // default true
      cGROUPS          = 1u << 12, // default true
      cHOVER           = 1u << 13, // default true
      cCLICK           = 1u << 14, // default false
      cOFFSET_CENTER   = 1u << 15, // default true
      cOFFSET_MIDDLE   = 1u << 16, // default false
      cOFFSET_BOTTOM   = 1u << 17, // default false
      cIMPORTANCE      = 1u << 18, // default true
      cVISIBILITY      = 1u << 19, // default true
      cDEFAULT = (cPERCEPTION | cFRIENDLIES | cFLIGHT | cPRIORITYTHREATS | cPRIORITYTARGETS | cALTITUDE | cESMRWR |
                  cANGLEONLY | cROUTE | cFOV | cGROUPS | cHOVER | cOFFSET_CENTER | cIMPORTANCE | cVISIBILITY)
   };

   enum class TSD_SETTINGS
   {
      cGROUND        = 1u << 0, // default true
      cAIR           = 1u << 1, // default true
      cROUTE         = 1u << 2, // default true
      cFOV           = 1u << 3, // default true
      cHOVER         = 1u << 4, // default true
      cCLICK         = 1u << 5, // default false
      cOFFSET_CENTER = 1u << 6, // default true
      cOFFSET_MIDDLE = 1u << 7, // default false
      cOFFSET_BOTTOM = 1u << 8, // default false
      cDEFAULT       = (cGROUND | cAIR | cROUTE | cFOV | cHOVER | cOFFSET_CENTER)
   };

   enum class ENGAGEMENT_SETTINGS
   {
      cMANUAL  = 1u << 0, // default true
      cTHREATS = 1u << 1, // default false
      cTARGETS = 1u << 2, // default false
      cNM      = 1u << 3, // default true
      cKM      = 1u << 4, // default false
      cDEFAULT = (cMANUAL | cNM)
   };

   // Animation state for a SettingsMenu
   enum class MenuState
   {
      cOPEN,
      cOPENING,
      cCLOSED,
      cCLOSING
   };

   // Container to group settings menu vars
   struct SettingsMenu
   {
      QElapsedTimer*                     animationTimer{nullptr}; ///< Timer for animating the settings window.
      bool                               visible{false}; ///< True if the settings menu is visible, false otherwise.
      MenuState                          menuState{MenuState::cCLOSED}; ///< State of the settings menu.
      float                              pos{0.0f};                     ///< Current position of the settings menu.
      osg::ref_ptr<osg::MatrixTransform> transform;                     ///< Transform holding settings menu drawables.
      std::vector<Button>                buttons;                       ///< Buttons for the settings menu.
   };

   /** Draw this overlay using the draw function. */
   void Update() override;

   /** Return a pointer to the geode for this overlay. */
   osg::ref_ptr<osg::Geode> GetGeode() { return mOverlayGeode; }

   /** Return a pointer to the state set for this overlay. */
   osg::ref_ptr<osg::StateSet> GetState() { return mOverlayState; }

   /** Return a pointer to the color for this overlay. */
   osg::ref_ptr<osg::Vec4Array> GetColor() { return mOverlayColor; }

   /** Return a pointer to the projection for this overlay. */
   osg::ref_ptr<osg::Projection> GetProjection() { return mOverlayProjection; }

   /** Return a pointer to the model view matrix for this overlay. */
   osg::ref_ptr<osg::MatrixTransform> GetModelViewMatrix() { return mOverlayModelViewMatrix; }

   /** Return the font size multiplier. */
   const float GetFontSizeMultiplier() const { return mFontSizeMult; }

   /** Set the font size multiplier.
    * @param aValue value to set.
    */
   void SetFontSizeMultiplier(const float aValue) { mFontSizeMult = aValue; }

   /** Return the line width multiplier. */
   const float GetLineWidthMultiplier() const { return mLineWidthMult; }

   /** Set the line width multiplier.
    * @param aValue value to set.
    */
   void SetLineWidthMultiplier(const float aValue) { mLineWidthMult = aValue; }

   /** Return the width of this overlay in pixels. */
   const int GetWidth() const { return mRect.width(); }

   /** Return the height of this overlay in pixels. */
   const int GetHeight() const { return mRect.height(); }

   /** Return the pixel width of this page from the overlay.  This is used to scale objects from
    * relative coordinates to pixel coordinates when the drawing.
    */
   const double pW() const;

   /** Return the pixel height of this page from the overlay.  This is used to scale objects from
    * relative coordinates to pixel coordinates when the drawing.
    */
   const double pH() const;

   /** Return the pixel width of this page from the overlay, scaled from the w/h aspect ratio.
    * This is used to scale objects from relative coordinates to pixel coordinates when the drawing.
    */
   const double rW() const;

   /** Return the pixel height of this page from the overlay, scaled from the w/h aspect ratio.
    * This is used to scale objects from relative coordinates to pixel coordinates when the drawing.
    */
   const double rH() const;

   /** Return the rect for this overlay. */
   const QRectF& GetRect() const { return mRect; }

   /** Initialize the overlay. */
   bool Initialize() override;

   /** Get the handle event mode for this overlay. */
   HandleEventMode GetHandleEventMode() const override { return mHandleEventMode; }

   /** Set the function to run when Button 1 is clicked.
    * @param aFunction function to set.
    */
   void SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction);

   /** Set the function to run when the mouse has been moved.
    * @param aFunction function to set.
    */
   void SetMouseMoveAction(const std::function<bool(int, int)>& aFunction);

   /** Set the function used for drawing.
    * @param aDrawFunction function to set.
    */
   void SetDrawFunction(const std::function<void()>& aDrawFunction) { mDrawFunction = aDrawFunction; }

   /** Create geometry using an opengl vec2 array.
    * @param aArray array of points to use to create the geometry.
    * @param aDrawMode draw mode to use.
    * @param aColor color to use.
    */
   osg::ref_ptr<osg::Geode> CreateGeometry(osg::ref_ptr<osg::Vec2Array> aArray, int aDrawMode, const osg::Vec4& aColor);

   // Alignment types include:  LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
   //                           CENTER_TOP, CENTER_CENTER, CENTER_BOTTOM,
   //                           RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM,
   //                           LEFT_BASE_LINE, CENTER_BASE_LINE, RIGHT_BASE_LINE,
   //                           LEFT_BOTTOM_BASE_LINE, CENTER_BOTTOM_BASE_LINE, and RIGHT_BOTTOM_BASE_LINE
   /** Add a text item.
    * @param aStr text to add.
    * @param aXPos x position of the text.
    * @param aYPos y position of the text.
    * @param aFontSize size of the font.
    * @param aColor color to use.
    * @param aAlignment alignment of the text.
    */
   osg::ref_ptr<osg::Geode> AddTextItem(const std::string&                 aStr,
                                        const double                       aXPos,
                                        const double                       aYPos,
                                        const float                        aFontSize,
                                        const osg::Vec4&                   aColor,
                                        const osgText::Text::AlignmentType aAlignment = osgText::Text::LEFT_BASE_LINE,
                                        const bool                         aAlignToScreen = true);

   /** Fill the background.
    * @param aColor color of the background.
    */
   void DrawBackground(const osg::Vec4& aColor);

   /** Draw an outline around the page.
    * @param aColor color of the outline.
    */
   void DrawOutline(const osg::Vec4& aColor);

   /** Draw text on the overlay.
    * @param aXPos x position.
    * @param aYPos y position.
    * @param aText text to draw.
    * @param aFontSize font size to use.
    * @param aColor color to use.
    * @param aAlignment text alignment to use.
    */
   void DrawLabel(const double                 aXPos,
                  const double                 aYPos,
                  const std::string&           aText,
                  const float                  aFontSize,
                  const osg::Vec4&             aColor,
                  osgText::Text::AlignmentType aAlignment);

   void Resize() override;

   /** Set the position of the overlay.
    * @param aX x position.
    * @param aY y position.
    */
   void SetPosition(float aX, float aY);

   /** Set the size of the overlay.
    * @param aW width of the overlay.
    * @param aH height of the overlay.
    */
   void SetSize(float aW, float aH);

   /** Set the ACES Data for this overlay.
    * @param aData data to set.
    */
   void SetAcesData(const AcesData& aData);

   /** Set the Truth data for this overlay.
    * @param aData data to set.
    */
   void SetAcesTruthPlatforms(const std::vector<EntityTruth>& aData);

   /** Set the platform comment for this overlay.
    * @param aPlatformCommentStr platform comment to set.
    */
   void SetAcesPlatformComment(const std::string& aPlatformCommentStr);

   /** Set no y offset for either the SAD and/or TSD.
    * @param aDisplayType display type to set the offset for.
    */
   void SetCenterPosition(const DisplayType aDisplayType);

   /** Set the half y offset for either the SAD and/or TSD.
    * @param aDisplayType display type to set the offset for.
    */
   void SetOffsetPosition(const DisplayType aDisplayType);

   /** Set the bottom y offset for either the SAD and/or TSD.
    * @param aDisplayType display type to set the offset for.
    */
   void SetBottomPosition(const DisplayType aDisplayType);

   /** Set the center point of focus for either the SAD and/or the TSD. */
   void SetCenterPoint();

   /** Reset the center point back to 0 for either the SAD and/or the TSD. */
   void ResetCenterPoint();

   /** Update the scaling priority and the scaling ratio. */
   void UpdatePriority();

   /** This sets if the connection to the display has been destroyed/ lost (draw x over screen).
    * @param aState state to set.
    */
   void SetConnectionDestroyed(const bool aState) { mConnectionDestroyed = aState; }

   /** Draw a black fade over the screen with a red x, indicating the connection to the platform has been lost/destroyed. */
   void DrawConnectionDestroyed();

   /** Initialize any drawables here. */
   void InitializeDrawables();

   /** Return if this display has focus (if it does platform selections will
    * update the mTargetPlatforms list). */
   const bool GetHasFocus() { return mHasFocus; }

   /** Set if this display has focus.
    * @param aHasFocus state to set.
    */
   void SetHasFocus(const bool aHasFocus) { mHasFocus = aHasFocus; }

   /** Triggered when platform selection has changed and this display
    * is in focus.
    * @param aSelected newly selected platforms.
    * @param aDeselected newly deselected platforms.
    */
   void PlatformSelectionChanged(const wkf::PlatformList& aSelected, const wkf::PlatformList& aDeselected);

   /** Set the target platforms list.
    * @param aTargetPlatforms list to set.
    */
   void SetTargetPlatforms(const std::vector<size_t>& aTargetPlatforms);

private:
   /** Handle a Button 1 click.
    * @param aMouseX X mouse position.
    * @param aMouseY Y mouse position.
    * @param aState state of the button.
    */
   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;

   /** Handle a Button 1 double click. As of now this will
    * just do exactly what the normal click does.  Due to
    * some other backend code, the clicking felt really
    * sluggish due to a timer, enabling this click function
    * seems to patch it.
    * @param aMouseX X mouse position.
    * @param aMouseY Y mouse position.
    * @param aState state of the button.
    */
   bool Btn1DblClick(int aMouseX, int aMouseY, int aState) override;

   /** Handle mouse movement.
    * @param aMouseX X mouse position.
    * @param aMouseY Y mouse position.
    */
   bool MouseMove(int aMouseX, int aMouseY) override;

   /** Handle mouse wheel movement.
    * @param aMouseX X mouse position.
    * @param aMouseY Y mouse position.
    */
   bool MouseWheel(int aMouseX, int aMouseY, int aDelta) override;

   /** Loop through any entities and if the mouse cursor was clicked inside the region, show the data for said entity. */
   void HandleClickRegions(const int aMouseX, const int aMouseY);

   /** Loop through any entities and if the mouse cursor was inside the hover region, show the data for said entity. */
   void HandleHoverRegions(const int aMouseX, const int aMouseY);

   /** Set the selectable region for this overlay.
    * @param aMouseX X region.
    * @param aMouseY Y region.
    */
   bool SelectableAt(int aMouseX, int aMouseY) const override;

   /** Draw function for ACES Display. */
   void Draw();

   /** Get the draw vector for a given region.
    * This returns an x,y position and a radius for the display.
    * @param aDrawRegion region to get the vector for.
    */
   const osg::Vec3 GetDrawVector(const DrawRegion aDrawRegion) const;

   /** Draw the source platforms heading.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aColor color of the text drawn.
    */
   void DrawHeading(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const osg::Vec4& aColor);


   /** Draw the header engine region (thrust/throttle).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the circle.
    */
   void DrawHeaderEng(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the header weight region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawHeaderWeight(osg::ref_ptr<osg::Group> aVectorGroup,
                         const double             aX,
                         const double             aY,
                         const double             aW,
                         const double             aH);

   /** Draw the header weapons region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawHeaderWeapons(osg::ref_ptr<osg::Group> aVectorGroup,
                          const double             aX,
                          const double             aY,
                          const double             aW,
                          const double             aH);

   /** Draw the header fuel region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawHeaderFuel(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the header platform alt and speed region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawHeaderAltSpeed(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH);

   /** Draw the engagement region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawEngagementRegion(osg::ref_ptr<osg::Group> aVectorGroup,
                             const double             aX,
                             const double             aY,
                             const double             aW,
                             const double             aH);

   /** Draw a the current engagement select mode (Manual, Targets, Threats)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void DrawEngagementSelectMode(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Draw the behavior history region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawBehaviorHistory(osg::ref_ptr<osg::Group> aVectorGroup,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH);

   // This region of the screen is non-functional at the moment
   // and will be added next pass
   /** Draw the platform selection region (drop-down box).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawPlatformSelection(osg::ref_ptr<osg::Group> aVectorGroup,
                              const double             aX,
                              const double             aY,
                              const double             aW,
                              const double             aH);

   /** Draw a next button region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawNextButton(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const osg::Vec4&         aColor = Color::LightGray);

   /** Draw a previous button region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawPrevButton(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const osg::Vec4&         aColor = Color::LightGray);

   /** Draw the simulation time.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void DrawTime(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Draw the label for a given display.
    * @param aVectorGroup vector group to add geometry to.
    * @param aDrawRegion draw region for the label.
    * @param aDisplayType display type to get matching label.
    */
   void DrawDisplayLabel(osg::ref_ptr<osg::Group> aVectorGroup, const DrawRegion aDrawRegion, const DisplayType aDisplayType);

   /** Draw the countermeasures region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH);

   /** Draw the currently selected weapon if present.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawSelectedWeapon(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH);

   /** Draw the display sync button in the center.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawDisplaySync(osg::ref_ptr<osg::Group> aVectorGroup,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH);

   /** Draw a settings tab for the given draw region (left, right, and top only).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawSettingsTab(osg::ref_ptr<osg::Group> aVectorGroup,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH,
                        const DrawRegion         aDrawRegion);


   /** Draw a display based on the type and region given.
    * @param aVectorGroup vector group to add geometry to.
    * @param aDrawRegion draw region for the display.
    * @param aDisplayType display type to draw.
    */
   void DrawDisplay(osg::ref_ptr<osg::Group> aVectorGroup, const DisplayType aDisplayType, const DrawRegion aDrawRegion);

   /** Draw the Primary Flight Display (PFD)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the display.
    */
   void DrawPFD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the Situation Awareness Display (SAD)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the display.
    */
   void DrawSAD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the Tactical Situation Display (TSD)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the display.
    */
   void DrawTSD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the Threat Warning Display (TWD)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the display.
    */
   void DrawTWD(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Return a pointer to the stencil state set. */
   osg::ref_ptr<osg::StateSet> GetStencilDrawWhereMatch();

   /** Draw a stencil buffer region.
    * @param aVectorGroup group to draw into.
    * @param aArray region of the stencil buffer.
    * @param aStencilValue enable or disable the stencil buffer.
    */
   void DrawStencilBufferRegion(osg::ref_ptr<osg::Group>           aVectorGroup,
                                const osg::ref_ptr<osg::Vec2Array> aArray,
                                int                                aStencilValue);

   /** Draw a box (debug).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
                const osg::Vec4&         aColor,
                const double             aX,
                const double             aY,
                const double             aW,
                const double             aH,
                const double             aScale);

   /** Draw the ownship in the center of the compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass.
    */
   void DrawOwnship(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the fov lines going outwards.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass.
    */
   void DrawFOV(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the route/waypoints for the platform.
    * @param aVectorGroup vector group to add geometry to.
    * @param aUnitsPerNm unit scaling to use.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aScale scale of the drawing.
    */
   void DrawRoute(osg::ref_ptr<osg::Group> aVectorGroup, const double aUnitsPerNm, const double aX, const double aY);

   /** Return the color of an entity using it's identification.
    * @param aIdentification identification of the entity (bogie, bandit, friendly, etc...)
    */
   const osg::Vec4 GetEntityColor(const Identification aIdentification) const;

   /** Draw range rings outwards from the ownship.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void SAD_DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw entities to the compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius used to size entities.
    */
   void SAD_DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw a truth entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void SAD_DrawTruthEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                            Entity&                  aEntity,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH);

   /** Draw an entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aR radius for scaling font size.
    */
   void SAD_DrawEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                       Entity&                  aEntity,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const double             aR);

   /** Draw an angle only priority target or threat number.
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aPriorityNumber priority number of the entity
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aR radius to scale font
    * @param aIsThreat true if entity is a threat, else is a target
    */
   void SAD_DrawEntityPriorityTargetThreatNumber(osg::ref_ptr<osg::Group> aVectorGroup,
                                                 Entity&                  aEntity,
                                                 const size_t             aPriorityNumber,
                                                 const double             aX,
                                                 const double             aY,
                                                 const double             aW,
                                                 const double             aH,
                                                 const double             aR,
                                                 bool                     aIsThreat);

   /** Draw a flag or flight entity, depending on if it is in a flight.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aR radius for scaling font size.
    * @param aIsFlight true if the entity is in a flight, false otherwise.
    */
   void SAD_DrawFlagFlightEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                 Entity&                  aEntity,
                                 const double             aX,
                                 const double             aY,
                                 const double             aW,
                                 const double             aH,
                                 const double             aR,
                                 const bool               aIsFlight);

   /** Draw a group entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius to get scaling font size.
    */
   void SAD_DrawGroupEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                            Entity&                  aEntity,
                            const double             aX,
                            const double             aY,
                            const double             aR);

   /** Draw an ESM/RWR entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aTgtBearing bearing to the entity.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void SAD_DrawESM_RWR_Entity(osg::ref_ptr<osg::Group> aVectorGroup, Entity& aEntity, const double aW, const double aH);

   /** Draw an angle only entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aR radius of the SAD for positioning.
    */
   void SAD_DrawAngleOnlyEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                                Entity&                  aEntity,
                                const double             aW,
                                const double             aH,
                                const double             aR);

   /** Return a new draw array for drawing the angle only entity.
    * @param aEntity entity used to calculate the angle only draw array.
    * @param aW width of the entity.
    * @param aH height of the entity.
    */
   osg::ref_ptr<osg::Vec2Array> SAD_GetAngleOnlyEntityDrawArray(Entity& aEntity, const double aW, const double aH);

   /** Draw an angle only priority target number.
    * @param aVectorGroup vector group to add geometry to.
    * @param aTgtBearing bearing to the entity.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aR radius for positioning the label.
    */
   void SAD_DrawAngleOnlyPriorityTarget(osg::ref_ptr<osg::Group> aVectorGroup,
                                        Entity&                  aEntity,
                                        const size_t             aPriority,
                                        const double             aW,
                                        const double             aH,
                                        const double             aR);

   /** Draw an angle only priority threat number.
    * @param aVectorGroup vector group to add geometry to.
    * @param aTgtBearing bearing to the entity.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aR radius for positioning the label.
    */
   void SAD_DrawAngleOnlyPriorityThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                                        Entity&                  aEntity,
                                        const size_t             aPriority,
                                        const double             aW,
                                        const double             aH,
                                        const double             aR);

   /** Get the draw array of the entity.
    * @param aGeode drawable to populate.
    * @param aEntity entity to use to get draw array.
    * @param aW width of the entity.
    * @param aH height of the entity.
    */
   osg::ref_ptr<osg::Geode> SAD_GetEntityDrawable(const Entity& aEntity, const double aW, const double aH);

   /** Draw altitude text for an entity.
    * @param aW width of the entity
    * @param aEntityColor color of the entity.
    * @param aXform transform to add to.
    * @param aR radius of the compass for text scaling.
    */
   void SAD_DrawAltitudeText(Entity&                                      aEntity,
                             const double                                 aW,
                             osg::Vec4                                    aEntityColor,
                             osg::ref_ptr<osg::PositionAttitudeTransform> aXform,
                             const double                                 aR);

   /** Draw an indicator for an entity which is derived from a visual track.
    * @param aW width of the entity
    * @param aEntityColor color of the entity.
    * @param aXform transform to add to.
    * @param aR radius of the compass for text scaling.
    */
   void SAD_DrawVisualIndicator(Entity&                                      aEntity,
                                const double                                 aW,
                                osg::Vec4                                    aEntityColor,
                                osg::ref_ptr<osg::PositionAttitudeTransform> aXform,
                                const double                                 aR);

   /** Set the current entity, this is the entity that will have it's info shown in the bottom right.
    * @param aEntity entity to set.
    */
   void SAD_SetCurrentEntity(const Entity& aEntity) { mSAD_CurrentEntity = aEntity; }

   /** Calculate the position of an entity on the compass, given certain information.
    * @param aEntity entity to calculate the position of.
    */
   const QPointF SAD_CalculateEntityPosition(const Entity& aEntity);

   /** Calculate the hover region for an entity given its position and size.
    * @param aX x position.
    * @param aY y position.
    * @param aW width of the entity.
    * @param aH height of the entity.
    */
   const QRectF SAD_CalculateEntityHoverRegion(Entity& aEntity, const double aX, const double aY, double aW, const double aH);

   /** Return compass units per nm for positioning entitites. */
   const double SAD_UnitsPerNm() const { return mSAD_CompassRadius / mSADRange_nm; }

   /** Draw the entity info box.  This shows any available information known about the entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void SAD_DrawEntityInfo(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH);

   /** Return compass units per nm for positioning entitites. */
   const double TSD_UnitsPerNm() const { return mTSD_CompassRadius / mTSDRange_nm; }

   /** Set the current entity, this is the entity that will have it's info shown in the bottom right.
    * @param aEntity entity to set.
    */
   void TSD_SetCurrentEntity(const TSD_Entity& aEntity) { mTSD_CurrentEntity = aEntity; }

   /** Draw the compass (rings, range/heading markings).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass.
    */
   void TSD_DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw all current entities to the compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass used to size entities.
    */
   void TSD_DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Returns true if an entity is angle-only, based on its data.
    * @param aEntity entity to check.
    */
   static bool TSD_IsEntityAngleOnly(const TSD_Entity& aEntity);

   /** Draw an angle only entity.
    * @param aVectorGroup vector group to which to add geometry.
    * @param aEntity entity from which to pull data.
    * @param aX x position at which to draw.
    * @param aY y position at which to draw.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aR radius for positioning the entity.
    */
   void TSD_DrawAngleOnly(osg::ref_ptr<osg::Group> aVectorGroup,
                          TSD_Entity&              aEntity,
                          const double             aX,
                          const double             aY,
                          const double             aW,
                          const double             aH,
                          const double             aR);

   /** Draw an unknown(bogie) aircraft.  This is drawn as a half-filled yellow box, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawBogie(osg::ref_ptr<osg::Group> aVectorGroup,
                      TSD_Entity&              aEntity,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH);

   /** Draw an unknown(bogie) missile.  This is drawn as a half-filled yellow box, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawBogieMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                             TSD_Entity&              aEntity,
                             const double             aX,
                             const double             aY,
                             const double             aW,
                             const double             aH);

   /** Draw a neutral (white air) aircraft.  This is drawn as a half-filled white box, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawNeutral(osg::ref_ptr<osg::Group> aVectorGroup,
                        TSD_Entity&              aEntity,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH);

   /** Draw a neutral (white air) missile.  This is drawn as a half-filled white box, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawNeutralMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                               TSD_Entity&              aEntity,
                               const double             aX,
                               const double             aY,
                               const double             aW,
                               const double             aH);

   /** Draw a threat/enemy aircraft.  This is drawn as a half-filled red triangle, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                       TSD_Entity&              aEntity,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH);

   /** Draw a bandit (enemy) missile.  This is drawn as a half-filled red triangle, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawThreatMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                              TSD_Entity&              aEntity,
                              const double             aX,
                              const double             aY,
                              const double             aW,
                              const double             aH);

   /** Draw a ground threat/enemy.  This is drawn as an empty red triangle, and is not heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawGroundThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                             TSD_Entity&              aEntity,
                             const double             aX,
                             const double             aY,
                             const double             aW,
                             const double             aH);

   /** Draw a friendly aircraft.  This is drawn as a half-filled green circle, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                      TSD_Entity&              aEntity,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH);

   /** Draw a friendly missile.  This is drawn as a half-filled green circle, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                             TSD_Entity&              aEntity,
                             const double             aX,
                             const double             aY,
                             const double             aW,
                             const double             aH);

   /** Draw a friendly flight aircraft.  This is drawn as an empty blue circle with a flight number, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawFlightAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                            TSD_Entity&              aEntity,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH);

   /** Draw a friendly flight missile.  This is drawn as an empty blue circle with a flight number, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawFlightAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                                   TSD_Entity&              aEntity,
                                   const double             aX,
                                   const double             aY,
                                   const double             aW,
                                   const double             aH);

   /** Calculate the hover region rectangle for an entity, in pixel coordinates.
    * This is used for hovering over an entity to get more info.
    * @param aEntity entity to calculate the hover region for.
    * @param aX x position of the rect.
    * @param aY y position of the rect.
    * @param aW width of the rect.
    * @param aH height of the rect.
    */
   const QRectF
   TSD_CalculateEntityHoverRegion(TSD_Entity& aEntity, const double aX, const double aY, double aW, const double aH);

   /** Calculate the position of an entity on the compass, given certain information
    * from its track.  This will check for certain information in order.
    * First, it will check for a valid range and bearing.
    * Second, it will check for a valid lat, lon, and heading.
    * Lastly, it will check for a valid bearing (Angle only).
    * @param aEntity entity to calculate the position of.
    */
   const QPointF TSD_CalculateEntityPosition(const TSD_Entity& aEntity);

   /** Helper function for CalculateEntityPosition.  This will return a position in compass coordinates,
    * given a bearing in radians and range in nm.
    * @param aBearingRad bearing in radians.
    * @param aRangeNm range in nm.
    */
   const QPointF TSD_CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm);

   /** Draw the entity info box.  This shows any available
    * information known about the entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void TSD_DrawEntityInfo(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH);

   /** Draw the platform comment region at the bottom right.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawPlatformComment(osg::ref_ptr<osg::Group> aVectorGroup,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH);

   /** Draw the Threat Warning Display (TWD) compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass.
    */
   void DrawTWDCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw a track on the TWD.
    * @param aVectorGroup vector group to add geometry to.
    * @param aTrack track to draw from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass for positioning.
    */
   void DrawTWDTrack(osg::ref_ptr<osg::Group> aVectorGroup,
                     TargetTrackItem&         aTrack,
                     const double             aX,
                     const double             aY,
                     const double             aR);

   /** Initialize drawables for the PFD. */
   void InitializePFD();

   /** Initialize drawables for the left settings region. */
   void InitializeLeftSettings();

   /** Initialize drawables for the right settings region. */
   void InitializeRightSettings();

   /** Initialize drawables for the top settings region. */
   void InitializeTopSettings();

   /** Toggle a visibility setting for a display.
    * @param aDisplay display to set the setting for.
    * @param aOption option to toggle.
    */
   void ToggleSetting(DisplayType aDisplay, size_t aOption);

   /** Choose a radio button setting for a display.
    * @param aDisplay display to set the setting for.
    * @param aOption option to set.
    */
   void SetRadioSetting(DisplayType aDisplay, size_t aOption);

   /** Toggle whether the SAD and TSD displays are synced. */
   void ToggleDisplaySync();

   /** Draw the left settings region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void DrawLeftSettings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Open the left settings menu. */
   void OpenLeftSettings();

   /** Close the left settings menu. */
   void CloseLeftSettings();

   /** Draw the right settings region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void DrawRightSettings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Open the right settings menu. */
   void OpenRightSettings();

   /** Close the right settings menu. */
   void CloseRightSettings();

   /** Draw the top settings region.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void DrawTopSettings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Open the top settings menu. */
   void OpenTopSettings();

   /** Close the top settings menu. */
   void CloseTopSettings();

   /** Get the label position for a given draw region.
    * @param aDrawRegion draw region to return label pos for.
    */
   const QPointF GetLabelPos(const DrawRegion aDrawRegion) const;

   /** Get the label string for a given draw region and display.
    * @param aDrawRegion draw region to return label for.
    * @param aDisplayType display type to return label for.
    */
   const std::vector<std::string> GetLabelString(const DrawRegion aDrawRegion, const DisplayType aDisplayType);

   /** Initialize the SAD Settings drawables/buttons.
    * @param aVectorGroup vector group to add geometry to.
    * @param aLeftSettings True if the settings are going to be drawn on the left settings tab.
    */
   void InitializeSADSettings(osg::ref_ptr<osg::Group> aVectorGroup, bool aLeftSettings);

   /** Initialize the TSD Settings drawables/buttons.
    * @param aVectorGroup vector group to add geometry to.
    * @param aLeftSettings True if the settings are going to be drawn on the left settings tab.
    */
   void InitializeTSDSettings(osg::ref_ptr<osg::Group> aVectorGroup, bool aLeftSettings);

   /** Initialize the Engagement Region Settings drawables/buttons.
    * @param aVectorGroup vector group to add geometry to.
    */
   void InitializeEngagementSettings(osg::ref_ptr<osg::Group> aVectorGroup);

   /** Debug - Draw button regions.
    * @param aVectorGroup vector group to add geometry to.
    */
   void DrawButtonRegions(osg::ref_ptr<osg::Group> aVectorGroup);

   /** Update entity info if there is one showing currently. */
   void UpdateEntityInfo();

   /** Update priority targets and threats if they have updated. */
   void UpdatePriorityTargetsThreats();

   /** Engagement Region - Go to the next target/threat. */
   void NextTarget();

   /** Engagement Region - Go to the previous target/threat. */
   void PreviousTarget();

   /** Set a new target if it is valid. Either go forward or backwards
    * in the list and check the platform exists.
    * @param aPlatformList platform list to search.
    * @param true to increment in the list, false to decrement.
    */
   void SetNewTarget(std::vector<size_t>& aPlatformList, bool aNext);

   /** Swap two displays given their draw regions.
    * @param aRegion1 region for the first display.
    * @param aRegion2 region for the second display.
    */
   void SwapDisplays(const DrawRegion aRegion1, const DrawRegion aRegion2);

   /** Attempt to get an ID string for the track drawing.
    * @param aTrack track to check for ID.
    */
   const std::string GetTrackIDString(const TargetTrackItem& aTrack);

   /** Get the draw region for the current mouse position.
    * This will return either left or right, and is used
    * to determine which display (SAD/TSD) will get a mouse action.
    */
   const DrawRegion GetMouseDrawRegion();

   /** Normalize radians into a -PI(180) to PI(180) range.
    * @param aRadians radians to normalize.
    */
   double NormalizeRadians(const double aRadians);

   /** Calculate the mouse position in units (drawing offset).
    * @param aMouseX x position.
    * @param aMouseY y position.
    */
   const QPointF CalculateMousePosInUnits(const double aMouseX, const double aMouseY);

   /** Calculate the mouse position in Nm (used for setting offsets).
    * @param aMouseX x position.
    * @param aMouseY y position.
    * @param aDisplayType display type to calculate for.
    */
   const QPointF CalculateMousePosInNm(const double aMouseX, const double aMouseY, const DisplayType aDisplayType);

   /** Helper function for CalculateEntityPosition.  This will return a position in compass coordinates,
    * given a bearing in radians and range in nm.
    * @param aBearingRad bearing in radians.
    * @param aRangeNm range in nm.
    */
   const QPointF CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm);

   // Overlay vars
   osgText::Font*                mFont{nullptr};              ///< Pointer to the font object.
   osg::ref_ptr<osg::Geode>      mOverlayGeode{nullptr};      ///< Pointer to the geode used by the overlay.
   osg::ref_ptr<osg::StateSet>   mOverlayState{nullptr};      ///< Pointer to the state set for the overlay.
   osg::ref_ptr<osg::Vec4Array>  mOverlayColor{nullptr};      ///< Pointer to the color used by the overlay.
   osg::ref_ptr<osg::Projection> mOverlayProjection{nullptr}; ///< Pointer to the projection matrix used by the overlay.
   osg::ref_ptr<osg::MatrixTransform> mOverlayModelViewMatrix{
      nullptr}; ///< Pointer to the model view matrix used by the overlay.  This positions the center of the page.
   UtoRawShape*                       mRawShapePtr{nullptr}; ///< Pointer to the raw shape used by the overlay.
   HandleEventMode                    mHandleEventMode;      ///< How to handle events.
   std::function<bool(int, int)>      mMouseMoveFunction;    ///< Mouse move function.
   std::function<bool(int, int, int)> mBtn1Function;         ///< Button 1 click function.
   QRectF                             mRect;                 ///< Rectangle representing geometry.
   std::function<void()>              mDrawFunction;         ///< Draw function.
   int                                mScreenWidth{0};       ///< Screen width.
   int                                mScreenHeight{0};      ///< Screen height.
   float                              mLineWidthMult{1.0f};  ///< Multiplier for the line width.
   float                              mFontSizeMult{1.0f};   ///< Multiplier for the font size.
   double                             mRatio{1.0};           ///< Width to height ratio
   QPointF                            mMousePos{0, 0};       ///< Current mouse position, used for hover/click events.
   bool mHorizontalPriority{false}; ///< True if the screen has horizontal priority (height scaled to width)
   bool mHasFocus{
      true}; ///< True if this overlay has focus, false otherwise.  Defaults to true (will have focus when window opens)

   // Draw flags
   bool mConnectionDestroyed{false}; ///< True to draw connection destroyed (x over screen), false otherwise.
   bool mDrawHoverRegions{false};    ///< True to draw hover regions for entities, false otherwise.
   bool mDrawClickRegions{false};    ///< True to draw click regions for entities, false otherwise.
   bool mDrawButtonRegions{false};   ///< True to draw button regions for the display, false otherwise.
   bool mDisplaySync{false};         ///< True to if the SAD and TSD should be synced, false otherwise.

   // Data Container
   AcesData mData; ///< ACES data we use to populate entities/draw.

   // ACES Display specific
   std::vector<Button>          mButtons;         ///< Buttons for the overlay.
   std::map<DrawRegion, Button> mSettingsButtons; ///< Buttons that open the settings windows.
   SettingsMenu                 mLeftSettings;    ///< Settings menu vars for the left region.
   SettingsMenu                 mRightSettings;   ///< Settings menu vars for the right region.
   SettingsMenu                 mTopSettings;     ///< Settings menu vars for the top region.
   std::map<DisplayType, DrawRegion> mDisplays; ///< Circular displays in the ACES Display, mapped by display type to draw region.
   std::map<DisplayType, size_t> mSettings;     ///< Map of displays to their visibility settings.
   size_t                        mFOV_deg{120}; ///< FOV of the aircraft.
   std::string mPlatformComment{""}; ///< String containing the most recent platform comment for the source platform.

   // PFD Specific
   osg::ref_ptr<osg::MatrixTransform> mPFD_PosMatrixNode{nullptr};  ///< Transform for positioning the PFD.
   osg::ref_ptr<osg::MatrixTransform> mPFD_HorzMatrixNode{nullptr}; ///< Transform for rotating drawables in the PFD.

   // SAD Specific
   std::vector<Entity> mSAD_Entities;      ///< List of entities.
   std::vector<Entity> mSAD_TruthEntities; ///< List of truth entities.
   bool    mSAD_ShowingEntityInfo{false};  ///< True if we are currently showing entity info, false otherwise.
   bool    mSAD_EntityClicked{false};      ///< True if an entity was clicked, false otherwise.
   Entity  mSAD_CurrentEntity;             ///< The current entity being hovered/clicked on.
   size_t  mSADRange_nm{160};              ///< Current range for the SAD. Defaults to 160 nm.
   QPointF mSAD_CenterPoint{0, 0};   ///< Current center point of the compass, can be centered on ownship or a point.
   double  mSAD_YOffset{0.0};        ///< Y offset for the compass.
   double  mSAD_CompassRadius{0.0};  ///< Current radius of the SAD.
   bool    mSAD_OffsetMiddle{false}; ///< True if the compass is offset into the middle.
   bool    mSAD_OffsetBottom{false}; ///< True if the compass is offset to the bottom.

   // TSD Specific
   std::vector<TSD_Entity> mTSD_Entities; ///< List of track entities we are drawing.
   std::vector<TSD_Entity> mTSD_Assets; ///< List of asset entities we are drawing.  These draw on top of the track entities
   bool       mTSD_ShowingEntityInfo{false}; ///< True if we are currently showing entity info, false otherwise.
   bool       mTSD_EntityClicked{false};     ///< True if an entity was clicked, false otherwise.
   TSD_Entity mTSD_CurrentEntity;            ///< The current entity being hovered/clicked on.
   size_t     mTSDRange_nm{160};             ///< Current range for the TSD. Defaults to 160 nm.
   QPointF    mTSD_CenterPoint{0, 0};   ///< Current center point of the compass, can be centered on ownship or a point.
   double     mTSD_YOffset{0.0};        ///< Y offset for the compass.
   double     mTSD_CompassRadius{0.0};  ///< Current radius of the TSD.
   bool       mTSD_OffsetMiddle{false}; ///< True if the compass is offset into the middle.
   bool       mTSD_OffsetBottom{false}; ///< True if the compass is offset to the bottom.

   // Engagement Region Specific
   std::vector<size_t> mTargetPlatforms;       ///< List of manually selected target platforms.
   std::vector<size_t> mPrioritizedThreats;    ///< List of prioritized threats.
   std::vector<size_t> mPrioritizedTargets;    ///< List of prioritized targets.
   size_t              mSourcePlatform{0};     ///< Index of the source platform.
   size_t              mTargetPlatform{0};     ///< Index of the target platform.
   size_t              mCurrentTargetIndex{0}; ///< Index of the current target, to load the appropriate data.
};
} // namespace AcesDisplay
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AcesDisplay::Overlay)
#endif // ACESDISPLAYOVERLAY_HPP
