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

#ifndef SA_DISPLAYOVERLAY_HPP
#define SA_DISPLAYOVERLAY_HPP

#include "wkf_air_combat_common_export.h"

#include <functional>

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
#include "sa_display/WkfSA_DisplayDataContainer.hpp"

namespace wkf
{
namespace SA_Display
{
// Colors for SA Display (a lot of these colors are shared with the HDD/TSD Page)
namespace Color
{
static const osg::Vec4 White(0.9, 0.9, 0.9, 1.0);
static const osg::Vec4 LightGray(0.8, 0.8, 0.8, 1.0);
static const osg::Vec4 Gray(0.5, 0.5, 0.5, 1.0);
static const osg::Vec4 Black(0.0, 0.0, 0.0, 1.0);
static const osg::Vec4 SlateBlue(0.2, 0.51, 0.67, 1.0);
static const osg::Vec4 Cyan(0.0, 1.0, 1.0, 1.0);
static const osg::Vec4 Magenta(0.85, 0.0, 1.0, 1.0);
static const osg::Vec4 Background(0.04, 0.08, 0.16, 1.0);
static const osg::Vec4 CompassLines(0.15, 0.16, 0.5, 1.0);
static const osg::Vec4 Red(0.863, 0.157, 0.157, 1.0);
static const osg::Vec4 Yellow(0.863, 0.824, 0.314, 1.0);
static const osg::Vec4 Green(0.0, 0.706, 0.235, 1.0);
} // namespace Color

class WKF_AIR_COMBAT_COMMON_EXPORT Overlay : public vespa::VaOverlay
{
public:
   // Tracks
   typedef struct
   {
      double tgtBearing_rad;
      double tgtAz_rad;
      double tgtEl_rad;
      double tgtRng_m;
      double tgtSpd_kts;
      double tgtAlt_ft;
      double tgtHdg_rad;
      double tgtRangeRate_fps;
   } TrackData;

   struct WaypointData
   {
      size_t               numWaypoints;
      std::vector<QPointF> waypoints;
      size_t               currentWaypoint;
      double               headingBug_deg;
      double               distToWaypoint_nm;
      double               timeToWaypoint_sec;
   };

   // This struct is the container we will draw off of,
   // It holds all data necessary for drawing an entity.
   struct Entity
   {
      Entity() {}

      // Flags
      bool isTarget{false};
      bool inFlight{false};
      bool isTruth{false};
      bool isGroup{false};

      // Containers for perception data
      EntityPerception perception;
      EntityTruth      truth;
      PerceivedGroup   groupPerception;

      // Rect for the hover region
      QRectF hoverRegion;
   };

   Overlay(const char* aName, vespa::VaViewer* aViewerPtr);
   Overlay* Clone() const override;
   ~Overlay() override = default;

   // Functions that set visibility flags
   void ShowPerception(bool aShow) { mDrawPerception = aShow; }
   void ShowTruthGround(bool aShow) { mDrawTruthGround = aShow; }
   void ShowTruthAir(bool aShow) { mDrawTruthAir = aShow; }
   void ShowFriends(bool aShow) { mDrawFriends = aShow; }
   void ShowFlight(bool aShow) { mDrawFlight = aShow; }
   void ShowPriorityThreats(bool aShow) { mDrawPriorityThreats = aShow; }
   void ShowPriorityTargets(bool aShow) { mDrawPriorityTargets = aShow; }
   void ShowAltitude(bool aShow) { mDrawAltitude = aShow; }
   void ShowESM_RWR(bool aShow) { mDrawESM_RWR = aShow; }
   void ShowAngleOnly(bool aShow) { mDrawAngleOnly = aShow; }
   void ShowOwnEngagements(bool aShow) { mDrawOwnEngagements = aShow; }
   void ShowFlightEngagements(bool aShow) { mDrawFlightEngagements = aShow; }
   void ShowThreatEngagements(bool aShow) { mDrawThreatEngagements = aShow; }
   void ShowSAMs(bool aShow) { mDrawSAMs = aShow; }
   void ShowRoute(bool aShow) { mDrawRoute = aShow; }
   void ShowBullseye(bool aShow) { mDrawBullseye = aShow; }
   void ShowFOV(bool aShow) { mDrawFOV = aShow; }
   void ShowGroups(bool aShow) { mDrawGroups = aShow; }

   // Sets the hover mode (click or hover)
   void HoverMode(bool aState) { mSelectModeHover = aState; }

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

   /** Return the font size multiplier */
   const float GetFontSizeMultiplier() const { return mFontSizeMult; }

   /** Set the font size multiplier
    * @param aValue value to set
    */
   void SetFontSizeMultiplier(const float aValue) { mFontSizeMult = aValue; }

   /** Return the line width multiplier */
   const float GetLineWidthMultiplier() const { return mLineWidthMult; }

   /** Set the line width multiplier
    * @param aValue value to set
    */
   void SetLineWidthMultiplier(const float aValue) { mLineWidthMult = aValue; }

   /** Return the width of this overlay in pixels */
   const int GetWidth() const { return mRect.width(); }

   /** Return the height of this overlay in pixels */
   const int GetHeight() const { return mRect.height(); }

   /** Return the pixel width of this page from the overlay. This is used to scale objects
    * from relative coordinates to pixel coordinates when the drawing.
    */
   const double pW() const;

   /** Return the pixel height of this page from the overlay, scaled from the w/h aspect ratio.
    * This is used to scale objects from relative coordinates to pixel coordinates when the drawing.
    */
   const double pH() const;

   /** Return the rect for this overlay */
   const QRectF& GetRect() const { return mRect; }

   /** Initialize the overlay */
   bool Initialize() override;

   /** Get the handle event mode for this overlay */
   HandleEventMode GetHandleEventMode() const override { return mHandleEventMode; }

   /** Set the function to run when Button 1 is clicked
    * @param aFunction function to set
    */
   void SetButton1ClickAction(const std::function<bool(int, int, int)>& aFunction);

   /** Set the function to run when the mouse has been moved
    * @param aFunction function to set
    */
   void SetMouseMoveAction(const std::function<bool(int, int)>& aFunction);

   /** Set the function used for drawing
    * @param aDrawFunction function to set
    */
   void SetDrawFunction(const std::function<void()>& aDrawFunction) { mDrawFunction = aDrawFunction; }

   /** Create geometry using an opengl vec2 array
    * @param aArray array of points to use to create the geometry
    * @param aDrawMode draw mode to use
    * @param aColor color to use
    */
   osg::ref_ptr<osg::Geode> CreateGeometry(osg::ref_ptr<osg::Vec2Array> aArray, int aDrawMode, const osg::Vec4& aColor);

   // Alignment types include:  LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
   //                           CENTER_TOP, CENTER_CENTER, CENTER_BOTTOM,
   //                           RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM,
   //                           LEFT_BASE_LINE, CENTER_BASE_LINE, RIGHT_BASE_LINE,
   //                           LEFT_BOTTOM_BASE_LINE, CENTER_BOTTOM_BASE_LINE,
   //                           and RIGHT_BOTTOM_BASE_LINE
   /** Add a text item
    * @param aStr text to add
    * @param aXPos x position of the text
    * @param aYPos y position of the text
    * @param aFontSize size of the font
    * @param aColor color to use
    * @param aAlignment alignment of the text
    */
   osg::ref_ptr<osg::Geode> AddTextItem(const std::string&                 aStr,
                                        const double                       aXPos,
                                        const double                       aYPos,
                                        const float                        aFontSize,
                                        const osg::Vec4&                   aColor,
                                        const osgText::Text::AlignmentType aAlignment = osgText::Text::LEFT_BASE_LINE);

   /** Fill the background
    * @param aColor color of the background
    */
   void DrawBackground(const osg::Vec4& aColor);

   /** Draw an outline around the page
    * @param aColor color of the outline
    */
   void DrawOutline(const osg::Vec4& aColor);

   /** Draw text on the overlay
    * @param aXPos x position
    * @param aYPos y position
    * @param aText text to draw
    * @param aFontSize font size to use
    * @param aColor color to use
    * @param aAlignment text alignment to use
    */
   void DrawLabel(const double                 aXPos,
                  const double                 aYPos,
                  const std::string&           aText,
                  const float                  aFontSize,
                  const osg::Vec4&             aColor,
                  osgText::Text::AlignmentType aAlignment);

   /** Set the position of the overlay
    * @param aX x position
    * @param aY y position
    */
   void SetPosition(float aX, float aY);

   /** Set the size of the overlay
    * @param aW width of the overlay
    * @param aH height of the overlay
    */
   void SetSize(float aW, float aH);

   /** Set the SA_Data for this overlay. We will use this data to draw perceptions.
    * @param aData data to set
    */
   void SetSA_Data(const SA_Data& aData);

   /** Set the Truth data for this overlay. We will use this data to draw truths.
    * @param aData data to set
    */
   void SetSA_TruthPlatforms(const std::vector<SA_Display::EntityTruth>& aData);

   // Set/clear widget values for truths/perceptions
   void SetTruthWidgetData();
   void SetPerceivedWidgetData();
   void EmptyTruthWidgetData();
   void EmptyPerceivedWidgetData();

   // Set pointers to the widgets, for filling data
   void SetTruthWidget(QTreeWidgetItem* aTruthDataWidget) { mTruthDataWidget = aTruthDataWidget; }
   void SetPerceivedWidget(QTreeWidgetItem* aPerceivedDataWidget) { mPerceivedDataWidget = aPerceivedDataWidget; }
   void SetRangeLabel(QLabel* aRangeLabel) { mRangeLabel = aRangeLabel; }
   void SetCenterPositionRadioButton(QRadioButton* aRadioButtonPtr) { mCenterPositionPtr = aRadioButtonPtr; }

   /** Set the range in nm for the compass
    * @param aRangeNm range to set in nm
    */
   void SetRangeNm(const double aRangeNm) { mRange_nm = aRangeNm; }

   // Set the offset of the compass
   void SetCenterPosition();
   void SetOffsetPosition();
   void SetBottomPosition();
   void SetCenterPoint();
   void ResetCenterPoint();
   void UpdatePriority();

   /** This sets if the connection to the HUD has been destroyed/ lost (draw x over screen)
    * @param aState state to set
    */
   void SetConnectionDestroyed(const bool aState) { mConnectionDestroyed = aState; }

   /** Draw a black fade over the screen with a red x, indicating the connection to the platform
    * has been lost/destroyed. */
   void DrawConnectionDestroyed();

private:
   /** Handle a Button 1 click
    * @param aMouseX X mouse position
    * @param aMouseY Y mouse position
    * @param aState state of the button
    */
   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;

   /** Handle mouse movement
    * @param aMouseX X mouse position
    * @param aMouseY Y mouse position
    */
   bool MouseMove(int aMouseX, int aMouseY) override;

   /** Handle mouse wheel movement
    * @param aMouseX X mouse position
    * @param aMouseY Y mouse position
    */
   bool MouseWheel(int aMouseX, int aMouseY, int aDelta) override;

   /** Loop through any entities and if the mouse cursor was clicked inside the region, show the data
    * for said entity */
   void HandleClickRegions(const int aMouseX, const int aMouseY);

   /** Loop through any entities and if the mouse cursor was inside the hover region, show the data
    * for said entity */
   void HandleHoverRegions(const int aMouseX, const int aMouseY);

   /** Set the selectable region for this overlay
    * @param aMouseX X region
    * @param aMouseY Y region
    */
   bool SelectableAt(int aMouseX, int aMouseY) const override;

   /** Draw function for the SA Display overlay */
   void Draw();

   /** Draw the TSD compass. This will draw differently based on the current range.
    * The compass rotates with the platform's heading.
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aR total radius of the compass
    * @param aScale scale of the drawing
    */
   void DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR, const double aScale);

   /** Draw a box (debug)
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aScale scale of the drawing
    */
   void DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
                const double             aX,
                const double             aY,
                const double             aW,
                const double             aH,
                const double             aScale);

   /** Draw the ship in the center of the compass, with the fov lines going outwards
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aR radius of the compass
    */
   void DrawOwnshipFOV(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw range rings outwards from the ownship
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    */
   void DrawOwnshipRings(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw entities to the compass
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    */
   void DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Draw a truth entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    */
   void DrawTruthEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                        Entity&                  aEntity,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH);

   /** Draw an entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    */
   void DrawEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                   Entity&                  aEntity,
                   const double             aX,
                   const double             aY,
                   const double             aW,
                   const double             aH);

   /** Draw a bandit entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aIsMissile true if perceived as a missile
    */
   void DrawBanditEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                         Entity&                  aEntity,
                         const double             aX,
                         const double             aY,
                         const double             aW,
                         const double             aH,
                         bool                     aIsMissile);

   /** Draw a friendly entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aIsMissile true if perceived as a missile
    */
   void DrawFriendlyEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                           Entity&                  aEntity,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH,
                           bool                     aIsMissile);

   /** Draw a neutral entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aIsMissile true if perceived as a missile
    */
   void DrawNeutralEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                          Entity&                  aEntity,
                          const double             aX,
                          const double             aY,
                          const double             aW,
                          const double             aH,
                          bool                     aIsMissile);

   /** Draw a bogie entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aIsMissile true if perceived as a missile
    */
   void DrawBogieEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                        Entity&                  aEntity,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH,
                        bool                     aIsMissile);

   /** Draw an angle only priority target or threat number
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aPriorityNumber priority number of the entity
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    * @param aIsThreat true if entity is a threat, else is a target
    */
   void DrawEntityPriorityTargetThreatNumber(osg::ref_ptr<osg::Group> aVectorGroup,
                                             Entity&                  aEntity,
                                             const size_t             aPriorityNumber,
                                             const double             aX,
                                             const double             aY,
                                             const double             aW,
                                             const double             aH,
                                             bool                     aIsThreat);

   /** Draw an entity with a id flag character in the center
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    */
   void DrawFlagEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                       Entity&                  aEntity,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH);

   /** Draw an flight entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    */
   void DrawFlightEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                         Entity&                  aEntity,
                         const double             aX,
                         const double             aY,
                         const double             aW,
                         const double             aH);

   /** Draw an flight entity
    * @param aEntity entity being drawn
    * @param aW width of the region
    * @param aEntityColor color of the entity
    * @param aXform position/attitude transform
    */
   void DrawAltitudeText(Entity&                                      aEntity,
                         const double                                 aW,
                         osg::Vec4                                    aEntityColor,
                         osg::ref_ptr<osg::PositionAttitudeTransform> aXform);

   /** Draw a group entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    */
   void DrawGroupEntity(osg::ref_ptr<osg::Group> aVectorGroup, Entity& aEntity, const double aX, const double aY);

   /** Draw an angle only entity
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aW width of the region
    * @param aH height of the region
    */
   void DrawAngleOnlyEntity(osg::ref_ptr<osg::Group> aVectorGroup, Entity& aEntity, const double aW, const double aH);

   /** Draw an angle only priority target or threat number
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aPriorityNumber priority number of the entity
    * @param aW width of the region
    * @param aH height of the region
    * @param aIsThreat true if entity is a threat, else is a target
    */
   void DrawAngleOnlyPriorityTargetThreatNumber(osg::ref_ptr<osg::Group> aVectorGroup,
                                                Entity&                  aEntity,
                                                const size_t             aPriorityNumber,
                                                const double             aW,
                                                const double             aH,
                                                bool                     aIsThreat);

   /** Draw the route/waypoints for the platform
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aScale scale of the drawing
    */
   void DrawRoute(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Draw the ownship's heading at the top of the display
    * @param aVectorGroup vector group to add geometry to
    * @param aX x position to draw at
    * @param aY y position to draw at
    */
   void DrawHeading(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Draw a dotted line to a locked on target
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    */
   void DrawLockOnTarget(osg::ref_ptr<osg::Group> aVectorGroup, Entity& aEntity, const double aX, const double aY);

   /** Draw a locked on entity (red diamond)
    * @param aVectorGroup vector group to add geometry to
    * @param aEntity entity being drawn
    * @param aX x position to draw at
    * @param aY y position to draw at
    * @param aW width of the region
    * @param aH height of the region
    */
   void DrawLockedEntity(osg::ref_ptr<osg::Group> aVectorGroup,
                         Entity&                  aEntity,
                         const double             aX,
                         const double             aY,
                         const double             aW,
                         const double             aH);


   /** Return a pointer to the stencil state set */
   osg::ref_ptr<osg::StateSet> GetStencilDrawWhereMatch();

   /** Draw a stencil buffer region
    * @param aVectorGroup group to draw into
    * @param aArray region of the stencil buffer
    * @param aStencilValue enable or disable the stencil buffer
    */
   void DrawStencilBufferRegion(osg::ref_ptr<osg::Group>           aVectorGroup,
                                const osg::ref_ptr<osg::Vec2Array> aArray,
                                int                                aStencilValue);

   /** Calculate the position of an entity on the compass, given certain information
    * @param aEntity entity to calculate the position of
    */
   const QPointF CalculateEntityPosition(const Entity& aEntity);

   /** Helper function for CalculateEntityPosition. This will return a position in compass coordinates,
    * given a bearing in radians and range in nm
    * @param aBearingRad bearing in radians
    * @param aRangeNm range in nm
    */
   const QPointF CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm);

   /** Calculate the over region for an entity given its position and size
    * @param aEntity entity being calculated
    * @param aX x position
    * @param aY y position
    * @param aW width of the entity
    * @param aH height of the entity
    */
   const QRectF CalculateEntityHoverRegion(Entity& aEntity, const double aX, const double aY, double aW, const double aH);

   /** Calculate the mouse position in units (drawing offset)
    * @param aMouseX x position
    * @param aMouseY y position
    */
   const QPointF CalculateMousePosInUnits(const double aMouseX, const double aMouseY);

   /** Calculate the mouse position in Nm (used for setting offsets)
    * @param aMouseX x position
    * @param aMouseY y position
    */
   const QPointF CalculateMousePosInNm(const double aMouseX, const double aMouseY);

   /** Return compass units per nm for positioning entities */
   const double UnitsPerNm() const { return mCompassRadius / mRange_nm; }

   /** Return the color of an entity using it's identification
    * @param aIdentification identification of the entity (bogie, bandit, friendly, etc...)
    */
   const osg::Vec4 GetEntityColor(const Identification aIdentification) const;

   /** Set the current entity, this is the entity that will have it's info shown in the bottom right
    * @param aEntity entity to set
    */
   void SetCurrentEntity(const Entity& aEntity) { mCurrentEntity = aEntity; }

   HandleEventMode                    mHandleEventMode;        ///< How to handle events
   std::function<bool(int, int)>      mMouseMoveFunction;      ///< Mouse move function
   std::function<bool(int, int, int)> mBtn1Function;           ///< Button 1 click function
   QRectF                             mRect;                   ///< Rectangle representing geometry
   std::function<void()>              mDrawFunction;           ///< Draw function
   int                                mScreenWidth;            ///< Screen width
   int                                mScreenHeight;           ///< Screen height
   osgText::Font*                     mFont         = nullptr; ///< Pointer to the font object
   osg::ref_ptr<osg::Geode>           mOverlayGeode = nullptr; ///< Pointer to the geode used by the overlay
   osg::ref_ptr<osg::StateSet>        mOverlayState = nullptr; ///< Pointer to the state set for the overlay
   osg::ref_ptr<osg::Vec4Array>       mOverlayColor = nullptr; ///< Pointer to the color used by the overlay
   osg::ref_ptr<osg::Projection> mOverlayProjection = nullptr; ///< Pointer to the projection matrix used by the overlay
   osg::ref_ptr<osg::MatrixTransform> mOverlayModelViewMatrix =
      nullptr; ///< Pointer to the model view matrix used by the overlay. This positions the center of the page.
   UtoRawShape* mRawShapePtr   = nullptr; ///< Pointer to the raw shape used by the overlay
   float        mLineWidthMult = 1.0f;    ///< Multiplier for the line width
   float        mFontSizeMult  = 1.0f;    ///< Multiplier for the font size
   double       mRatio         = 1.0;

   // Compass vars
   double              mCompassX{0.0};        ///< Compass x position, this will be used to translate the hover regions.
   double              mCompassY{0.0};        ///< Compass y position, this will be used to translate the hover regions.
   double              mCompassRadius{0.375}; ///< Radius of the compass.
   double              mYOffset{0.0};         ///< Y offset for the compass.
   bool                mOffsetMiddle{false};  ///< True if the compass is offset into the middle.
   bool                mOffsetBottom{false};  ///< True if the compass is offset to the bottom.
   size_t              mFOV_deg{120};         ///< FOV of the aircraft.
   std::vector<Entity> mEntities;             ///< List of entities.
   std::vector<Entity> mTruthEntities;        ///< List of truth entities.
   int                 mRange_nm{160};        ///< The current range of the compass.  Defaults to 160.
   bool                mSelectModeHover{true};    ///< True if the select mode is hover, false if is set to click mode.
   bool                mShowingEntityInfo{false}; ///< True if we are currently showing entity info, false otherwise.
   Entity              mCurrentEntity;            ///< The current entity being hovered/clicked on.
   QPointF             mMousePos;                 ///< Current mouse position, used for hover/click events.
   QPointF             mCenterPoint; ///< Current center point of the compass, can be centered on ownship or a point.
   SA_Data             mData;        ///< SA data we use to populate entities/draw.
   bool mConnectionDestroyed{false}; ///< True to draw connection destroyed (x over screen), false otherwise.
   bool mHorizontalPriority{false};

   // Draw flags
   bool mDrawHoverRegions{false};
   bool mDrawClickRegions{false};
   bool mDrawAir{true};
   bool mDrawGround{true};
   bool mDrawWaypoint{true};
   bool mDrawFriends{true};
   bool mDrawFlight{true};
   bool mDrawPerception{true};
   bool mDrawTruthGround{false};
   bool mDrawTruthAir{false};
   bool mDrawPriorityTargets{true};
   bool mDrawPriorityThreats{true};
   bool mDrawAltitude{true};
   bool mDrawESM_RWR{true};
   bool mDrawAngleOnly{true};
   bool mDrawOwnEngagements{true};
   bool mDrawFlightEngagements{true};
   bool mDrawThreatEngagements{true};
   bool mDrawSAMs{true};
   bool mDrawRoute{true};
   bool mDrawBullseye{true};
   bool mDrawFOV{true};
   bool mDrawGroups{true};

   // Widget pointers from DockWidget
   QTreeWidgetItem* mTruthDataWidget;
   QTreeWidgetItem* mPerceivedDataWidget;
   QLabel*          mRangeLabel;
   QRadioButton*    mCenterPositionPtr;
};
} // namespace SA_Display
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::SA_Display::Overlay)
#endif // SA_DISPLAYOVERLAY_HPP
