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

#ifndef SA_DISPLAYDATACONTAINER_HPP
#define SA_DISPLAYDATACONTAINER_HPP

#include "wkf_air_combat_common_export.h"

#include <set>
#include <string>
#include <vector>

#include <QObject>

namespace wkf
{
namespace SA_Display
{
enum Identification
{
   ID_UNKNOWN  = 0,
   ID_FRIENDLY = 1,
   ID_NEUTRAL  = 2,
   ID_BOGIE    = 3,
   ID_BANDIT   = 4
};

// Track data

struct TargetTrackItem
{
   size_t         tgtPlatformIndex  = {0};
   double         tgtLat_deg        = {0.0};
   double         tgtLon_deg        = {0.0};
   double         tgtAlt_ft         = {0.0};
   double         tgtBearing_rad    = {0.0};
   double         tgtElevation_rad  = {0.0};
   double         tgtRange_nm       = {0.0};
   double         tgtRangeRate_kts  = {0.0};
   double         tgtSpeed_kts      = {0.0};
   double         tgtHdg_deg        = {0.0};
   double         tgtAspect_deg     = {0.0};
   std::string    tgtIdString       = {""};
   std::string    tgtPlatformName   = {""};
   std::string    tgtTypeName       = {""};
   Identification identification    = {ID_UNKNOWN};
   bool           latLonValid       = {false};
   bool           altitudeValid     = {false};
   bool           bearingValid      = {false};
   bool           elevationValid    = {false};
   bool           rangeValid        = {false};
   bool           rangeRateValid    = {false};
   bool           targetSpeedValid  = {false};
   bool           targetHdgValid    = {false};
   bool           targetAspectValid = {false};
   bool           idIffValid        = {false};
   bool           idAuxValid        = {false};
   bool           idSideValid       = {false};
   bool           idTypeValid       = {false};
   bool           isMissile         = {false};
};

// Weapons data

struct WeaponNameQty
{
   std::string weaponName;
   size_t      quantity = {0};
};

struct EngagedTargetData
{
   size_t targetIndex          = {0};
   double rangeMax_nm          = {0.0};
   double rangeNe_nm           = {0.0};
   double rangeMin_nm          = {0.0};
   double rangeCurrent_nm      = {0.0};
   double rangeClosureRate_kts = {0.0};
   double timeToIntercept_sec  = {0.0};
   double timeToActive_sec     = {0.0};
   double targetKTAS           = {0.0};
   double targetAspect_deg     = {0.0};
   double targetAz_deg         = {0.0};
   double targetEl_deg         = {0.0};
};

struct WeaponDataSummary
{
   size_t                         platformIndex = {0};
   std::vector<WeaponNameQty>     weaponList;
   std::string                    selectedWeapon    = {""};
   int                            numSelectedWeapon = {0};
   std::vector<EngagedTargetData> engagedTargetList;
   int                            numChaff           = {0};
   int                            numFlares          = {0};
   int                            numDecoys          = {0};
   bool                           weaponBayDoorsOpen = {false};
   bool                           supportingWeapon   = {false};
   bool                           masterWarning      = {false};
   bool                           masterCaution      = {false};
   bool                           shootCueActive     = {false};
   bool                           shootCueBlink      = {false};
   bool                           masterArmActive    = {false};
   bool                           jammingDetected    = {false};
};

// Navigation/waypoint data

struct WaypointLatLon
{
   double lat_deg = {0.0};
   double lon_deg = {0.0};
};

struct WaypointData
{
   int                         numWaypoints = {0};
   std::vector<WaypointLatLon> waypoints;
   int                         currentWaypoint       = {0};
   double                      headingBug_deg        = {0.0};
   double                      distanceToWaypoint_nm = {0.0};
   double                      timeToWaypoint_sec    = {0.0};
};

struct NavData
{
   size_t       platformIndex = {0};
   WaypointData waypointData;
   bool         wayptDataValid = {false};
};

// Fuel system data

struct FuelData
{
   size_t platformIndex     = {0};
   double fuelInternal_lbs  = {0.0};
   double fuelExternal_lbs  = {0.0};
   double fuelCapInt_lbs    = {0.0};
   double fuelCapExt_lbs    = {0.0};
   double grossWgt_lbs      = {0.0};
   double joker_lbs         = {0.0};
   double bingo_lbs         = {0.0};
   double fuelFlow_pph      = {0.0};
   double normalizedFuelQty = {0.0};
   int    numExtTanks       = {0};
   bool   dropTanksPresent  = {false};
   bool   fuelValid         = {false};
};

struct FlightKinematicsData
{
   size_t      platformIndex = {0};
   double      simTime       = {0.0};
   int         flightId      = {0};
   std::string idFlag        = {""};
   double      lat_deg       = {0.0};
   double      lon_deg       = {0.0};
   double      kcas          = {0.0};
   double      ktas          = {0.0};
   double      mach          = {0.0};
   double      altBaro_ft    = {0.0};
   double      altRadar_ft   = {0.0};
   double      vertSpd_fpm   = {0.0};
   double      alpha_deg     = {0.0};
   double      beta_deg      = {0.0};
   double      gLoad         = {0.0};
   double      gAvail        = {0.0};
   double      heading_deg   = {0.0};
   double      pitch_deg     = {0.0};
   double      roll_deg      = {0.0};
   double      gX            = {0.0};
   double      gY            = {0.0};
   std::string pilotType     = {""};
   bool        afterburnerOn = {false};
   bool        contrailing   = {false};
   bool        stallWarning  = {false};
   bool        alphaValid    = {false};
   bool        betaValid     = {false};
   bool        kcasValid     = {false};
   bool        nxValid       = {false};
   bool        nyValid       = {false};
   bool        nzValid       = {false};
   bool        gAvailValid   = {false};
};

// SA Entity Perception

struct EntityPerception
{
   double         lat_deg        = {0};
   double         lon_deg        = {0};
   double         altitude_ft    = {0};
   double         bearing_deg    = {0};
   double         speed_kts      = {0};
   double         heading_deg    = {0};
   double         threatLevel    = {0};
   double         targetValue    = {0};
   double         risk           = {0};
   double         defensiveness  = {0};
   double         urgency        = {0};
   std::string    idString       = {""};
   size_t         perceivedIndex = {0};
   std::string    perceivedName  = {""};
   std::string    perceivedType  = {""};
   uint16_t       flightId       = {0};
   std::string    idFlag         = {""};
   Identification identification = {ID_UNKNOWN};
   bool           friendlyAsset  = {false};
   bool           angleOnly      = {false};
   bool           idIffFriend    = {false};
   bool           idIffFoe       = {false};
   bool           idIffNeutral   = {false};
   bool           idIffUnknown   = {false};
   bool           idAuxFriend    = {false};
   bool           idAuxFoe       = {false};
   bool           idAuxNeutral   = {false};
   bool           idAuxUnknown   = {false};
   bool           idSideFriend   = {false};
   bool           idSideFoe      = {false};
   bool           idSideNeutral  = {false};
   bool           idSideUnknown  = {false};
   bool           idTypeFriend   = {false};
   bool           idTypeFoe      = {false};
   bool           idTypeNeutral  = {false};
   bool           idTypeUnknown  = {false};
   bool           isHostile      = {false};
   bool           altitudeValid  = {false};
   bool           speedValid     = {false};
   bool           headingValid   = {false};
   bool           isMissile      = {false};
};

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
   bool        altitudeValid{false};
   bool        speedValid{false};
   bool        headingValid{false};
   bool        isMissile{false};
};

struct PerceivedGroup
{
   std::string                   groupName           = {""};
   double                        centroidLat_deg     = {0.0};
   double                        centroidLon_deg     = {0.0};
   float                         centroidAlt_ft      = {0.0f};
   float                         centroidHeading_deg = {0.0f};
   float                         centroidSpeed_kts   = {0.0f};
   float                         radiusCurrent_nm    = {0.0f};
   float                         radiusMinimum_nm    = {0.0f};
   float                         simtimeAtCoast_sec  = {0.0f};
   float                         maxCoastingTime_sec = {0.0f};
   std::vector<EntityPerception> elementList;
};

// This is the main data structure used in the plug-in
struct SA_Data
{
   SA_Data()                    = default;
   SA_Data(const SA_Data& aSrc) = default;

   SA_Display::FlightKinematicsData          platformData;
   SA_Display::FuelData                      fuelData;
   SA_Display::NavData                       navData;
   SA_Display::WeaponDataSummary             weaponData;
   std::vector<SA_Display::TargetTrackItem>  trackList;
   std::vector<SA_Display::EntityPerception> assetsList;
   std::vector<SA_Display::EntityPerception> bogiesList;
   std::vector<SA_Display::EntityPerception> banditsList;
   std::vector<SA_Display::EntityPerception> prioritizedThreatsList;
   std::vector<SA_Display::EntityPerception> prioritizedTargetsList;
   std::vector<SA_Display::PerceivedGroup>   groupsList;
};

// The wkf::SA_Display::DataContainer contains the data relevant for rendering the SA Display.
// This includes perception and truth data, as well as any other SA related data.
class WKF_AIR_COMBAT_COMMON_EXPORT DataContainer : public QObject
{
   Q_OBJECT

public:
   DataContainer() = default;
   DataContainer(const DataContainer& aSrc);

   /** Return a const reference to all SA's data. */
   const std::map<size_t, SA_Data>& GetSA_Data() const { return mSA_Data; }

   /** Return a const reference to all SA's truth platform objects. */
   const std::vector<SA_Display::EntityTruth>& GetSA_TruthPlatforms() const { return mSA_TruthPlatforms; }

   /** Set the current SA truth platforms data using a temporary container.
    * @param aSA_TruthPlatforms Container to use to populate.
    */
   void SetSA_TruthPlatforms(const std::vector<SA_Display::EntityTruth>& aSA_TruthPlatforms);

   /** Set the current SA data using a temporary SA data container.
    * @param aSA_Data Container to use to populate.
    */
   void SetSA_Data(const std::map<size_t, SA_Data>& aSA_Data);

   /** Returns true if the simulation has been initialized/reset, false otherwise. */
   bool GetSimulationInitialized() const { return mSimulationInitialized; }

   /** Set if the simulation is initialized or not.  Values reset on true.
    * @param aValue value to set.
    */
   void SetSimulationInitialized(bool aValue) { mSimulationInitialized = aValue; }

private:
   std::map<size_t, SA_Data>            mSA_Data; ///< Container for platforms SA data, mapped by platform index.
   std::vector<SA_Display::EntityTruth> mSA_TruthPlatforms; ///< Container for storing truth data into a drawable format
                                                            ///< for the SA Display.
   bool mSimulationInitialized{false}; ///< True if the simulation is initialized/reset, false otherwise.
};
} // namespace SA_Display
} // namespace wkf

#endif // SA_DISPLAYDATACONTAINER_HPP
