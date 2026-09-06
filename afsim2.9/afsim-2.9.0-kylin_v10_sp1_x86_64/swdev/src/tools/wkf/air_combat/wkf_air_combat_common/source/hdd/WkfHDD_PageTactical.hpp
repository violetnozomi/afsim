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

#ifndef WKFHDD_PAGETSD_HPP
#define WKFHDD_PAGETSD_HPP

#include "wkf_air_combat_common_export.h"

#include <QRect>

#include "WkfHDD_DataContainer.hpp"
#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageTactical : public Page
{
public:
   enum EntityDomain
   {
      AIR,
      GROUND
   };
   enum EntityType
   {
      TRACK,
      ASSET
   }; ///< This is used for checking if the entity has track or perception data

   struct Entity
   {
      Entity() {}

      EntityDomain domain{EntityDomain::AIR}; ///< The domain of this entity, AIR or GROUND
      EntityType   type;
      bool         inFlight{false}; ///< True if the entity is part of this aircrafts flight, false otherwise
      double       range{-1.0};     ///< Range in nm of a ground entity.  Only use range if domain is GROUND (SAMs)
      SimData::TargetTrackItem      track;         ///< Track data for this entity (Master track list)
      SimData::EntityPerceptionData perceivedData; ///< Asset only - this stores perceived data rather than track data
      QRectF hoverRegion; ///< The hover region for this entity, used to hover and view entity info
   };

   struct TSD_Data
   {
      bool        masterArmActive{false}; ///< True if the master arm is active, false otherwise
      std::string selectedWeapon{""};     ///< String representing the currently selected weapon
      int         numSelectedWeapon{0};   ///< Quantity of the currently selected weapon

      double         lat_deg{0.0};     ///< Latitude of this aircraft in degrees
      double         lon_deg{0.0};     ///< Longitude of this aircraft in degrees
      double         heading_deg{0.0}; ///< Heading of this aircraft in degrees
      size_t         fov_deg{120};     ///< FOV of this aircraft in degrees
      unsigned short flightId{0};      ///< Flight id of this platform

      // Countermeasures
      size_t chaff{0};  ///< Number of chaff
      size_t flares{0}; ///< Number of flares
      size_t decoys{0}; ///< Number of decoys

      SimData::NavData navData; ///< Nav data for this aircraft. Used for drawing waypoints/routes
   };

   PageTactical(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr, const size_t aTSDIndex);
   ~PageTactical() override = default;

   /** Set the heading of this page in degrees.  The compass is aligned to this heading.
    * @param aInput heading to set.
    */
   void SetOwnshipHeading_deg(const double aInput_deg) { mData.heading_deg = aInput_deg; }

   /** Set the flight id for the TSD.  This will be used on the asset list to determine drawing
    * either a flight(blue) or friendly(green).
    * @param aInput heading to set.
    */
   void SetFlightId(const unsigned short aId) { mData.flightId = aId; }

   /** Set the latitude and longitude of the TSD in degrees.  This is used for calculating
    * waypoint positioning to certain entities.
    * @param aLat latitude to set.
    * @param aLon longitude to set.
    */
   void SetLatLon(const double aLat, const double aLon)
   {
      mData.lat_deg = aLat;
      mData.lon_deg = aLon;
   }

   /** Set the track data for the TSD.  This is used to draw entities.
    * @param aTrackData track data to set.
    */
   void SetTrackData(const SimData::TargetTrackItemList& aTrackData);

   /** Set the assets list data for the TSD.  This is used to draw assets.
    * @param aData assets list data to use.
    */
   void SetAssetsListData(const SimData::AssetsList& aData);

   /** Set the nav data for the TSD.  This is used to draw waypoints/routes
    * @param aNavData nav data to set.
    */
   void SetNavData(const SimData::NavData& aNavData);

   /** Set the currently selected weapon and if the master arm is active
    * @param aSelectedWeapon currently selected weapon
    * @param aNumSelectedWeapon quantity of currently selected weapon
    * @param aMasterArmActive if the master arm is active
    */
   void SetWeaponSelectedAndArmed(const std::string& aSelectedWeapon,
                                  const int          aNumSelectedWeapon,
                                  const bool         aMasterArmActive);

private:
   /** Loop through any entities on the compass and if the mouse cursor was clicked inside
    * its hover region, show the entity info window for said entity, until clicked off of
    * @param aMouseX mouse x position, in pixel coords
    * @param aMouseY mouse y position, in pixel coords
    */
   void HandleEntityClickRegions(const int aMouseX, const int aMouseY);

   /** Loop through any entities on the compass and if the mouse cursor was inside
    * its hover region, show the entity info window for said entity
    * @param aMouseX mouse x position, in pixel coords
    * @param aMouseY mouse y position, in pixel coords
    */
   void HandleHoverRegions(const int aMouseX, const int aMouseY);

   /** This is called when the compass scale is increased. */
   void RangeIncreaseClicked();

   /** This is called when the compass scale is decreased. */
   void RangeDecreaseClicked();

   /** This is called when the toggle air button is clicked.
    * Toggle whether air domain entities are visible on the compass or not.
    */
   void ToggleShowAir() { mShowingAir = !mShowingAir; }

   /** This is called when the toggle ground button is clicked.
    * Toggle whether ground domain entities are visible on the compass or not.
    */
   void ToggleShowGround() { mShowingGround = !mShowingGround; }

   /** This is called when the toggle waypoints button is clicked.
    * Toggle whether waypoints are visible on the compass or not.
    */
   void ToggleShowWaypoint() { mShowingWaypoint = !mShowingWaypoint; }

   /** Draw this page. All draw code starts here. */
   void Draw() override;

   /** Draw the countermeasures counts.  This is for the small version of the display only.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH,
                            const double             aScale);

   /** Draw the click regions for this page (Debugging).
    * @param aVectorGroup vector group to add geometry to.
    */
   void DrawClickRegions(osg::ref_ptr<osg::Group> aVectorGroup);

   /** Draw the TSD compass.  This will draw differently based on the current range.
    * The compass rotates with the platform's heading.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR total radius of the compass.
    * @param aScale scale of the drawing.
    */
   void DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR, const double aScale);

   /** Draw the waypoints/route for the platform on the compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aScale scale of the drawing.
    */
   void DrawWaypoints(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aScale);

   /** Draw the heading in degrees.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawHeading(osg::ref_ptr<osg::Group> aVectorGroup,
                    const double             aX,
                    const double             aY,
                    const double             aW,
                    const double             aH,
                    const double             aScale);

   /** Draw the aircraft in the center of the compass, with its fov lines going outwards.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass.
    */
   void DrawOwnshipFOV(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Returns the color of an entity, based on its identification.
    * @param aEntity entity to check.
    */
   static osg::Vec4 GetEntityColor(const Entity& aEntity);

   /** Returns true if an entity is angle-only, based on its data.
    * @param aEntity entity to check.
    */
   static bool IsEntityAngleOnly(const Entity& aEntity);

   /** Draw an angle only entity.  This is drawn as a triangle immediately outside of the compass, pointing in the
    * direction of the entity from the ownship.
    * @param aVectorGroup vector group to which to add geometry.
    * @param aEntity entity from which to pull data.
    * @param aX x position at which to draw.
    * @param aY y position at which to draw.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawAngleOnly(osg::ref_ptr<osg::Group> aVectorGroup,
                      Entity&                  aEntity,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH);

   /** Draw an unknown(bogie) aircraft.  This is drawn as a half-filled yellow box, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawBogie(osg::ref_ptr<osg::Group> aVectorGroup,
                  Entity&                  aEntity,
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
   void DrawBogieMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                         Entity&                  aEntity,
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
   void DrawNeutral(osg::ref_ptr<osg::Group> aVectorGroup,
                    Entity&                  aEntity,
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
   void DrawNeutralMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                           Entity&                  aEntity,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH);

   /** Draw a bandit (enemy) aircraft.  This is drawn as a half-filled red triangle, and is heading aligned.
    * @param aVectorGroup vector group to add geometry to.
    * @param aEntity entity to pull draw data from.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawBandit(osg::ref_ptr<osg::Group> aVectorGroup,
                   Entity&                  aEntity,
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
   void DrawBanditMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                          Entity&                  aEntity,
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
   void DrawGroundThreat(osg::ref_ptr<osg::Group> aVectorGroup,
                         Entity&                  aEntity,
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
   void DrawAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                  Entity&                  aEntity,
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
   void DrawAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                         Entity&                  aEntity,
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
   void DrawFlightAsset(osg::ref_ptr<osg::Group> aVectorGroup,
                        Entity&                  aEntity,
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
   void DrawFlightAssetMissile(osg::ref_ptr<osg::Group> aVectorGroup,
                               Entity&                  aEntity,
                               const double             aX,
                               const double             aY,
                               const double             aW,
                               const double             aH);

   /** Draw all current entities to the compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    */
   void DrawEntities(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY);

   /** Draw the master arm box (If we have a weapon currently armed).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawArmed(osg::ref_ptr<osg::Group> aVectorGroup,
                  const double             aX,
                  const double             aY,
                  const double             aW,
                  const double             aH,
                  const double             aScale);

   /** Draw the selected weapon box (If we have a weapon currently selected).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawSelected(osg::ref_ptr<osg::Group> aVectorGroup,
                     const double             aX,
                     const double             aY,
                     const double             aW,
                     const double             aH,
                     const double             aScale);

   /** Draw the range indicator and arrows.  Clicking the up arrow
    * or down arrow will change the range of the compass.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawRangeIndicator(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH,
                           const double             aScale);

   /** Draw a box (Debugging).
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawBox(osg::ref_ptr<osg::Group> aVectorGroup,
                const double             aX,
                const double             aY,
                const double             aW,
                const double             aH,
                const double             aScale);

   /** Draw the visibility options (Show air domain, show ground domain, show waypoints)
    * Clicking any of these will toggle visibility for that entity type.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawVisibilityOptions(osg::ref_ptr<osg::Group> aVectorGroup,
                              const double             aX,
                              const double             aY,
                              const double             aW,
                              const double             aH,
                              const double             aScale);

   /** Draw the entity info box in the bottom right.  This shows any available
    * information known about the entity.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawEntityInfo(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const double             aScale);

   /** Calculate the hover region rectangle for an entity, in pixel coordinates.
    * This is used for hovering over an entity to get more info.
    * @param aEntity entity to calculate the hover region for.
    * @param aX x position of the rect.
    * @param aY y position of the rect.
    * @param aW width of the rect.
    * @param aH height of the rect.
    */
   const QRectF CalculateEntityHoverRegion(Entity& aEntity, const double aX, const double aY, double aW, const double aH);

   /** Calculate the position of an entity on the compass, given certain information
    * from its track.  This will check for certain information in order.
    * First, it will check for a valid range and bearing.
    * Second, it will check for a valid lat, lon, and heading.
    * Lastly, it will check for a valid bearing (Angle only).
    * @param aEntity entity to calculate the position of.
    */
   const QPointF CalculateEntityPosition(const Entity& aEntity);

   /** Helper function for CalculateEntityPosition.  This will return a position in compass coordinates,
    * given a bearing in radians and range in nm.
    * @param aBearingRad bearing in radians.
    * @param aRangeNm range in nm.
    */
   const QPointF CalculatePositionFromBearingAndRange(const double aBearingRad, const double aRangeNm);

   /** Return a pointer to the stencil state set where the stencil matches.
    * If applied to an osg node inside of a stencil buffer region, it will
    * clip the geometry once it reaches the outside of the region.
    */
   osg::ref_ptr<osg::StateSet> GetStencilDrawWhereMatch();

   /** Draw a stencil buffer region, that will clip and hide geometry outside of the region.
    * @param aVectorGroup group to attach the buffer region.
    * @param aArray array to use to draw the geometry, using GL_POLYGON.
    * @param aStencilValue 1 to enable the stencil buffer region, 0 to disable it.
    */
   void DrawStencilBufferRegion(osg::ref_ptr<osg::Group>           aVectorGroup,
                                const osg::ref_ptr<osg::Vec2Array> aArray,
                                int                                aStencilValue);

   /** Set the current entity.  If there is a current entity, the hover window is showing,
    * and we are displaying its info.
    * @param aEntity entity to set as the current entity.
    */
   void SetCurrentEntity(const Entity& aEntity) { mCurrentEntity = aEntity; }

   /** Return compass units per nm.  This is used for positioning entities on the compass. */
   const double UnitsPerNm() const { return mCompassRadius / mRange_nm; }

   TSD_Data mData;                  ///< Data for this page.
   bool   mDrawHoverRegions{false}; ///< True if drawing the hover regions around entities, false otherwise. (Debugging)
   bool   mDrawClickRegions{false}; ///< True if drawing the click regions on the TSD, false otherwise. (Debugging)
   bool   mShowingAir{true};        ///< True if showing air domain entities on the compass, false otherwise.
   bool   mShowingGround{true};     ///< True if showing ground domain entities on the compass, false otherwise.
   bool   mShowingWaypoint{true};   ///< True if showing waypoints/routes on the compass, false otherwise.
   size_t mRange_nm{160};           ///< Current range of the compass, in nm.
   QPointF mMousePos;               ///< Current position of the mouse, used for handling hover/click events.

   // Compass vars
   double mCompassX;      ///< Compass x position, this will be used to translate draw code to match.
   double mCompassY;      ///< Compass y position, this will be used to translate draw code to match.
   double mCompassRadius; ///< Radius of the compass, this will help calculate compass coordinates.

   // Entity info window vars
   bool   mShowingEntityInfo{false};  ///< If true, we are showing entity info at the bottom right of the page.
   bool   mDrawEntityInfoRight{true}; ///< Draw the entity info box on the right, otherwise draw on the left.
   bool   mEntityClicked{false};      ///< True if an entity was clicked, false otherwise.
   Entity mCurrentEntity;             ///< Current entity being hovered over.  Pull its data from here.
   std::vector<Entity> mEntities;     ///< List of entities we are drawing.  These are updated in SetTrackData.
   std::vector<Entity> mAssets; ///< List of asset entities we are drawing.  These use SetAssetsListData, rather than track data.
};
} // namespace HDD
#endif // WKFHDD_PAGETSD_HPP
