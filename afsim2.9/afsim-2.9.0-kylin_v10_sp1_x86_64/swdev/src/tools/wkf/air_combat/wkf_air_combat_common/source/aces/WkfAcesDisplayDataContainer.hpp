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

#ifndef ACESDISPLAYDATACONTAINER_HPP
#define ACESDISPLAYDATACONTAINER_HPP

#include "wkf_air_combat_common_export.h"

#include <set>
#include <string>
#include <vector>

#include <QObject>

namespace wkf
{
namespace AcesDisplay
{

// This sets how many behavior history messages to retrieve
// for the overlay
static constexpr size_t cBEHAVIOR_HISTORY_COUNT{10};

// Enumeration for Indentification of an entity(drawable) on the
// ACES Display.  This value will help determine the color/object
// being drawn to the display
enum class Identification
{
   ID_UNKNOWN  = 0,
   ID_FRIENDLY = 1,
   ID_NEUTRAL  = 2,
   ID_BOGIE    = 3,
   ID_BANDIT   = 4
};

// Container for track data, this is the main container
// used for drawing track based entities, such as entities
// on the Tactical Situation Display
struct TargetTrackItem
{
   size_t         tgtPlatformIndex{0};
   double         tgtLat_deg{0.0};
   double         tgtLon_deg{0.0};
   double         tgtAlt_ft{0.0};
   double         tgtBearing_rad{0.0};
   double         tgtElevation_rad{0.0};
   double         tgtRange_nm{0.0};
   double         tgtRangeRate_kts{0.0};
   double         tgtSpeed_kts{0.0};
   double         tgtHdg_deg{0.0};
   double         tgtAspect_deg{0.0};
   std::string    tgtIdString{""};
   std::string    tgtPlatformName{""};
   std::string    tgtTypeName{""};
   Identification identification{Identification::ID_UNKNOWN};
   bool           latLonValid{false};
   bool           altitudeValid{false};
   bool           bearingValid{false};
   bool           elevationValid{false};
   bool           rangeValid{false};
   bool           rangeRateValid{false};
   bool           targetSpeedValid{false};
   bool           targetHdgValid{false};
   bool           targetAspectValid{false};
   bool           idIffValid{false};
   bool           idAuxValid{false};
   bool           idSideValid{false};
   bool           idTypeValid{false};
   bool           isMissile{false};
};

// Container for track item lists, mapping target index to the track
struct TargetTrackItemList
{
   std::map<size_t, TargetTrackItem> mTracks = {};
};

// Container for a track processor, its name, whether it is the master processor
// and its list of tracks.
struct TrackProcessor
{
   std::string         trackProcessorName;
   bool                isMasterProcessor = {false};
   TargetTrackItemList trackList;
};

// Weapons data
struct WeaponNameQty
{
   std::string weaponName{""};
   size_t      quantity{0};
};

// WEZ and Range data for a target platform
struct EngagedTargetData
{
   size_t targetIndex{0};
   double rangeMax_nm{0.0};
   double rangeNe_nm{0.0};
   double rangeMin_nm{0.0};
   double rangeCurrent_nm{0.0};
   double rangeClosureRate_kts{0.0};
   double timeToIntercept_sec{0.0};
   double timeToActive_sec{0.0};
   double targetKTAS{0.0};
   double targetAspect_deg{0.0};
   double targetAz_deg{0.0};
   double targetEl_deg{0.0};
};

// A summary of all weapon related variables for the platform
struct WeaponDataSummary
{
   size_t                         platformIndex{0};
   std::vector<WeaponNameQty>     weaponList;
   std::string                    selectedWeapon{""};
   int                            numSelectedWeapon{0};
   std::vector<EngagedTargetData> engagedTargetList;
   int                            numChaff{0};
   int                            numFlares{0};
   int                            numDecoys{0};
   bool                           weaponBayDoorsOpen{false};
   bool                           supportingWeapon{false};
   bool                           masterWarning{false};
   bool                           masterCaution{false};
   bool                           shootCueActive{false};
   bool                           shootCueBlink{false};
   bool                           masterArmActive{false};
   bool                           jammingDetected{false};
};

// Container combining lat/lon into a waypoint
struct WaypointLatLon
{
   double lat_deg{0.0};
   double lon_deg{0.0};
};

// Container for Waypoint/Route data, providing a list of waypoints,
// the current waypoint index, and values to the next waypoint
struct WaypointData
{
   int                         numWaypoints{0};
   std::vector<WaypointLatLon> waypoints;
   int                         currentWaypoint{0};
   double                      headingBug_deg{0.0};
   double                      distanceToWaypoint_nm{0.0};
   double                      timeToWaypoint_sec{0.0};
};

// Container of waypoint data for a platform and if the data is valid
struct NavData
{
   size_t       platformIndex{0};
   WaypointData waypointData;
   bool         wayptDataValid{false};
};

// Fuel data for the platform, such as internal and external fuel weight
// in lbs and joker/bingo values.  Total weight is also provided here
struct FuelData
{
   size_t platformIndex{0};
   double fuelInternal_lbs{0.0};
   double fuelExternal_lbs{0.0};
   double fuelCapInt_lbs{0.0};
   double fuelCapExt_lbs{0.0};
   double grossWgt_lbs{0.0};
   double joker_lbs{0.0};
   double bingo_lbs{0.0};
   double fuelFlow_pph{0.0};
   double normalizedFuelQty{0.0};
   int    numExtTanks{0};
   bool   dropTanksPresent{false};
   bool   fuelValid{false};
};

// General kinematics data for the platform
struct FlightKinematicsData
{
   size_t      platformIndex{0};
   double      simTime{0.0};
   int         flightId{0};
   std::string idFlag{""};
   double      lat_deg{0.0};
   double      lon_deg{0.0};
   double      kcas{0.0};
   double      ktas{0.0};
   double      mach{0.0};
   double      altBaro_ft{0.0};
   double      altRadar_ft{0.0};
   double      vertSpd_fpm{0.0};
   double      alpha_deg{0.0};
   double      beta_deg{0.0};
   double      gLoad{0.0};
   double      gAvail{0.0};
   double      heading_deg{0.0};
   double      pitch_deg{0.0};
   double      roll_deg{0.0};
   double      gX{0.0};
   double      gY{0.0};
   std::string pilotType{""};
   bool        afterburnerOn{false};
   bool        contrailing{false};
   bool        stallWarning{false};
   bool        alphaValid{false};
   bool        betaValid{false};
   bool        kcasValid{false};
   bool        nxValid{false};
   bool        nyValid{false};
   bool        nzValid{false};
   bool        gAvailValid{false};
};

// Container for flight controls data and the normalized
// input values
struct FlightControlsData
{
   size_t platformIndex           = {0};
   double stickRgtInput           = {0.0};
   double stickBackInput          = {0.0};
   double rudderRgtInput          = {0.0};
   double speedBrakeInput         = {0.0};
   double throttleInput           = {0.0}; // (0-2)
   double aileronRgtNormalized    = {0.0};
   double aileronLftNormalized    = {0.0};
   double flapRgtNormalized       = {0.0};
   double flapLftNormalized       = {0.0};
   double stabilizerRgtNormalized = {0.0};
   double stabilizerLftNormalized = {0.0};
   double rudderRgtNormalized     = {0.0};
   double rudderLftNormalized     = {0.0};
   double rudderNormalized        = {0.0};
   double speedBrakeNormalized    = {0.0};
   double thrustNormalized        = {0.0}; // (0-2)
   double landingGear             = {0.0};
   bool   throttleValid           = {false};
   bool   aileronsValid           = {false};
   bool   flapsValid              = {false};
   bool   twinRuddersValid        = {false};
   bool   singleRudderValid       = {false};
};

// Container for entity perception data, such as perceived
// location and identification values.  This is the main data
// used to draw on the Situation Awareness Display
struct EntityPerception
{
   double         lat_deg{0};
   double         lon_deg{0};
   double         altitude_ft{0};
   double         bearing_deg{0};
   double         speed_kts{0};
   double         heading_deg{0};
   double         threatLevel{0};
   double         targetValue{0};
   double         risk{0};
   double         defensiveness{0};
   double         urgency{0};
   std::string    idString{""};
   size_t         perceivedIndex{0};
   std::string    perceivedName{""};
   std::string    perceivedType{""};
   uint16_t       flightId{0};
   std::string    idFlag{""};
   Identification identification{Identification::ID_UNKNOWN};
   bool           friendlyAsset{false};
   bool           angleOnly{false};
   bool           idIffFriend{false};
   bool           idIffFoe{false};
   bool           idIffNeutral{false};
   bool           idIffUnknown{false};
   bool           idAuxFriend{false};
   bool           idAuxFoe{false};
   bool           idAuxNeutral{false};
   bool           idAuxUnknown{false};
   bool           idSideFriend{false};
   bool           idSideFoe{false};
   bool           idSideNeutral{false};
   bool           idSideUnknown{false};
   bool           idTypeFriend{false};
   bool           idTypeFoe{false};
   bool           idTypeNeutral{false};
   bool           idTypeUnknown{false};
   bool           isHostile{false};
   bool           altitudeValid{false};
   bool           speedValid{false};
   bool           headingValid{false};
   bool           isMissile{false};
   bool           focus{true};
   bool           isCoasting{false};
   bool           isImportant{false};
};

// Container for truth location and identification, used to draw
// truth entities on the Situation Awareness Display
struct EntityTruth
{
   double      lat_deg{0};
   double      lon_deg{0};
   double      altitude_ft{0};
   double      bearing_deg{0};
   double      speed_kts{0};
   double      heading_deg{0};
   size_t      index{0};
   std::string idString{""};
   std::string name{""};
   std::string type{""};
   std::string domain{""};
   bool        entityValid{false};
   bool        altitudeValid{false};
   bool        speedValid{false};
   bool        headingValid{false};
   bool        isMissile{false};
};

// Container for a perceived group, with values that help
// with drawing the circle/centroid of the group on the
// Situation Awareness Display
struct PerceivedGroup
{
   std::string                   groupName{""};
   double                        centroidLat_deg{0.0};
   double                        centroidLon_deg{0.0};
   float                         centroidAlt_ft{0.0f};
   float                         centroidHeading_deg{0.0f};
   float                         centroidSpeed_kts{0.0f};
   float                         radiusCurrent_nm{0.0f};
   float                         radiusMinimum_nm{0.0f};
   bool                          focus{true};
   bool                          isCoasting{false};
   bool                          isImportant{false};
   std::vector<EntityPerception> elementList;
};

// Container for all engagement data for a given platform.  This will
// contain a map of data for all platform indices a given platform is
// tracking, if available.  This is used for drawing the engagement
// region at the top right of the ACES Display.
struct EngagementData
{
   // Prioritized targets and threats
   std::vector<std::string> mPrioritizedTargetNames;
   std::vector<std::string> mPrioritizedThreatNames;

   // Platforms that are currently being tracked
   std::vector<std::string> mTrackedPlatformNames;

   // Engagement data
   std::map<size_t, float>       mAngleOffMap_deg;
   std::map<size_t, float>       mAspectAngleMap_deg;
   std::map<size_t, float>       mCurrentRangeMap_nm;
   std::map<size_t, float>       mCurrentDeltaAltMap_ft;
   std::map<size_t, float>       mTrackQuality;
   std::map<size_t, std::string> mTargetCID;
   std::map<size_t, uint8_t>     mTargetThreatLevel;
   std::map<size_t, float>       mRawDetectionAgainstTargetMap_nm;
   std::map<size_t, float>       mRawWEZAgainstTargetMap_nm;
   std::map<size_t, float>       mWEZMinRangeMap_nm;
   std::map<size_t, float>       mWEZMaxRangeMap_nm;
   std::map<size_t, float>       mWEZNoEscapeMap_nm;
   std::map<size_t, float>       mRiskMap;
   std::map<size_t, float>       mDefensivenessMap;
   std::map<size_t, float>       mUrgencyMap;
   std::map<size_t, bool>        mTargetLocked;
   std::map<size_t, bool>        mAcceptableWEZ;
};

// This is the main data structure used in the plug-in
struct AcesData
{
   AcesData()                     = default;
   AcesData(const AcesData& aSrc) = default;

   AcesDisplay::FlightKinematicsData             platformData;
   AcesDisplay::FuelData                         fuelData;
   AcesDisplay::FlightControlsData               flightControlsData;
   AcesDisplay::NavData                          navData;
   AcesDisplay::WeaponDataSummary                weaponData;
   std::map<size_t, AcesDisplay::EngagementData> engagementData;
   std::vector<AcesDisplay::TrackProcessor>      trackProcessorList;
   std::string                                   esmTrackprocessor;
   std::string                                   mwsTrackprocessor;
   std::string                                   radarTrackprocessor;
   std::string                                   irstTrackprocessor;
   std::string                                   dasTrackprocessor;
   std::string                                   flirTrackprocessor;
   std::string                                   eyesTrackprocessor;
   AcesDisplay::TargetTrackItemList              masterTrackListData;
   std::vector<AcesDisplay::EntityPerception>    assetsList;
   std::vector<AcesDisplay::EntityPerception>    bogiesList;
   std::vector<AcesDisplay::EntityPerception>    banditsList;
   std::vector<AcesDisplay::EntityPerception>    prioritizedThreatsList;
   std::vector<AcesDisplay::EntityPerception>    prioritizedTargetsList;
   std::vector<AcesDisplay::PerceivedGroup>      groupsList;
   std::vector<std::string>                      behaviorHistory;
};

// The wkf::DataContainer contains the data relevant for rendering the Aces Display
class WKF_AIR_COMBAT_COMMON_EXPORT DataContainer : public QObject
{
   Q_OBJECT

public:
   DataContainer() = default;
   DataContainer(const DataContainer& aSrc);

   /** Return a const reference to all ACES data. */
   const std::map<size_t, AcesData>& GetAcesData() const { return mAcesData; }

   /** Return a const reference to all ACES truth platform objects. */
   std::vector<EntityTruth>& GetAcesTruthPlatforms() { return mAcesTruthPlatforms; }

   /** Return a const reference to all platform comments. */
   const std::map<size_t, std::map<double, std::string>>& GetPlatformComments() const { return mPlatformComments; }

   /** Return a const reference to all platforms with a Situation Awareness Processor. */
   const std::set<size_t>& GetPlatformsWithSAP() const { return mPlatformsWithSAP; }

   /** Set the current ACES truth platforms data using a temporary container.
    * @param aAcesTruthPlatforms Container to use to populate.
    */
   void SetAcesTruthPlatforms(const std::vector<EntityTruth>& aAcesTruthPlatforms);

   /** Set the current ACES data using a temporary ACES data container.
    * @param aAcesData Container to use to populate.
    */
   void SetAcesData(const std::map<size_t, AcesData>& aAcesData);

   /** Set the current platform comment data using a temporary data container.
    * @param aPlatformComments Container to use to populate.
    */
   void SetPlatformComments(const std::map<size_t, std::map<double, std::string>>& aPlatformComments);

   /** Add a platform comment to the platform comment container.
    * @param aPlatformIndex index of the platform to add a comment for.
    * @param aSimTime time that the comment occurs.
    * @param aMessage text for the comment.
    */
   void AddPlatformComment(const size_t aPlatformIndex, const double aSimTime, const std::string& aMessage);

   /** Add a platform with a Situation Awareness Processor to the container.
    * This platform can open an ACES Display.
    * @param aPlatformIndex index of the platform to add.
    */
   void AddPlatformWithSAP(const size_t aPlatformIndex);

   /** Add an EntityTruth object to the container.
    * This will draw a truth entity in the SAD portion of
    * the ACES Display.
    * @param aEntity entity to add.
    */
   void AddEntityTruth(EntityTruth& aEntity); // aEntity is non-const as we will modify it's data.

   /** Remove an EntityTruth object from the container if found.
    * @param aPlatformIndex index to remove.
    */
   void RemoveEntityTruth(const size_t aPlatformIndex);

   /** Returns true if the simulation has been initialized/reset, false otherwise. */
   bool GetSimulationInitialized() const { return mSimulationInitialized; }

   /** Set if the simulation is initialized or not.  Values reset on true.
    * @param aValue value to set.
    */
   void SetSimulationInitialized(bool aValue) { mSimulationInitialized = aValue; }

private:
   std::map<size_t, AcesData> mAcesData; ///< Container for platforms Aces data, mapped by platform index.
   std::vector<EntityTruth> mAcesTruthPlatforms; ///< Container for storing truth data into a drawable format for the Aces Display.
   std::map<size_t, std::map<double, std::string>> mPlatformComments; ///< Map of platform indices to a map of time to comments
   std::set<size_t> mPlatformsWithSAP;             ///< Set of platforms with SituationAwarenessProcessors.
   bool             mSimulationInitialized{false}; ///< True if the simulation is initialized/reset, false otherwise.
};
} // namespace AcesDisplay
} // namespace wkf

#endif // ACESDISPLAYDATACONTAINER_HPP
