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

#ifndef WKFHDD_DATACONTAINER_HPP
#define WKFHDD_DATACONTAINER_HPP

#include "wkf_air_combat_common_export.h"

#include <vector>

#include <QList>
#include <QObject>
#include <QVector3D>

namespace HDD
{
namespace SimData
{
// Track data

enum Identification
{
   ID_UNKNOWN  = 0,
   ID_FRIENDLY = 1,
   ID_NEUTRAL  = 2,
   ID_BOGIE    = 3,
   ID_BANDIT   = 4
};

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

struct TargetTrackItemList
{
   std::map<size_t, TargetTrackItem> mTracks = {};
};

struct TrackProcessor
{
   std::string         trackProcessorName;
   bool                isMasterProcessor = {false};
   TargetTrackItemList trackList;
};

// Friendly assets data

struct EntityPerceptionData
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
   unsigned short flightId       = {0};
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

struct AssetsList
{
   size_t                            platformIndex = {0};
   std::vector<EntityPerceptionData> assetList;
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

// Flight controls data

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

struct PlatformData
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
}; // namespace SimData

struct HDD_Data
{
   HDD_Data()                     = default;
   HDD_Data(const HDD_Data& aSrc) = default;

   SimData::PlatformData                platformData;
   SimData::FuelData                    fuelData;
   SimData::NavData                     navData;
   SimData::FlightControlsData          flightControlsData;
   SimData::WeaponDataSummary           weaponData;
   std::vector<SimData::TrackProcessor> trackProcessorList;
   std::string                          esmTrackprocessor;
   std::string                          mwsTrackprocessor;
   std::string                          radarTrackprocessor;
   std::string                          irstTrackprocessor;
   std::string                          dasTrackprocessor;
   std::string                          flirTrackprocessor;
   std::string                          eyesTrackprocessor;
   SimData::TargetTrackItemList         masterTrackListData;
   SimData::AssetsList                  assetsList;
};

class WKF_AIR_COMBAT_COMMON_EXPORT DataContainer : public QObject
{
   Q_OBJECT

public:
   DataContainer() = default;
   DataContainer(const DataContainer& aSrc);

   /** Return a const reference to all HDDs' data. */
   const std::map<QString, HDD_Data>& GetHDDData() const { return mHddData; }

   /** Set the current HDD data using another HDD data container.
    * @param aHddData Container to use to populate.
    */
   void SetHDDData(const std::map<QString, HDD_Data>& aHddData);

private:
   std::map<QString, HDD_Data> mHddData; ///< Container for HDDs' data (Track, Sensors, Platform).
};
} // namespace HDD

#endif // WKFHDD_DATACONTAINER_HPP
