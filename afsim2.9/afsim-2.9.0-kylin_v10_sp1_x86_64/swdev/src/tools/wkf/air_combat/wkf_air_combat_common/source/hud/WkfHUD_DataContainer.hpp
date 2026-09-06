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

#ifndef HUD_DATACONTAINER_HPP
#define HUD_DATACONTAINER_HPP

#include "wkf_air_combat_common_export.h"

#include <set>
#include <string>

#include <QObject>

#include "UtCalendar.hpp"

namespace wkf
{
// The wkf::HUD_DataContainer contains most of the
// state data of the controlled platform (and the controlled platform). This
// provides thread-safe communications using SimEvents from the SimInterface.

class WKF_AIR_COMBAT_COMMON_EXPORT HUD_DataContainer : public QObject
{
   Q_OBJECT

public:
   struct HudWaypointData
   {
      double lat_deg = 0.0;
      double lon_deg = 0.0;
   };

   struct HudEngagedTargetData
   {
      size_t targetIndex          = 0;
      double rangeMax_nm          = 0.0;
      double rangeNe_nm           = 0.0;
      double rangeMin_nm          = 0.0;
      double rangeCurrent_nm      = 0.0;
      double rangeClosureRate_kts = 0.0;
      double timeToIntercept_sec  = 0.0;
      double timeToActive_sec     = 0.0;
      double targetKTAS           = 0.0;
      double targetAspect_deg     = 0.0;
      double targetAz_deg         = 0.0;
      double targetEl_deg         = 0.0;
   };

   enum eHudMode
   {
      eHUD_NAV_MODE = 0,
      eHUD_WPN_MODE = 1
   };

   // This is the main data structure used in the plug-in
   struct PlatformData
   {
      PlatformData() = default;

      PlatformData(const PlatformData& aSrc) = default;

      double      simTime           = 0.0;
      double      lat_deg           = 0.0;
      double      lon_deg           = 0.0;
      double      kcas              = 0.0;
      double      ktas              = 0.0;
      double      mach              = 0.0;
      double      altitudeMSL_ft    = 0.0;
      double      radarAltitude_ft  = 0.0;
      double      verticalSpeed_fpm = 0.0;
      double      alpha_deg         = 0.0;
      double      beta_deg          = 0.0;
      double      gLoad             = 0.0;
      double      gAvail            = 0.0;
      double      heading_deg       = 0.0;
      double      pitch_deg         = 0.0;
      double      roll_deg          = 0.0;
      double      gX                = 0.0;
      double      gY                = 0.0;
      std::string pilotType         = "";
      bool        afterburnerOn     = {false};
      bool        contrailing       = {false};
      bool        stallWarning      = {false};
      bool        alphaValid        = {false};
      bool        betaValid         = {false};
      bool        kcasValid         = {false};
      bool        nxValid           = {false};
      bool        nyValid           = {false};
      bool        nzValid           = {false};
      bool        gAvailValid       = {false};

      // Control inputs
      double speedBrakeInput                       = {0.0};
      double speedBrakeNormalized                  = {0.0};
      double throttleInput                         = {0.0};
      double thrustNormalized                      = {0.0};
      bool   throttleValid                         = {false};
      double engineThrottleNormalizedMilitary      = 0.0;
      double engineThrottleNormalizedAfterburner   = 0.0;
      double throttleMilitaryControllerPosition    = 0.0;
      double throttleAfterburnerControllerPosition = 0.0;

      // Nav/waypoint data
      size_t                       numWaypoints = 0;
      std::vector<HudWaypointData> waypoints;
      size_t                       currentWaypoint       = 0;
      double                       headingBug_deg        = 0.0;
      double                       distanceToWaypoint_nm = 0.0;
      double                       timeToWaypoint_sec    = 0.0;
      bool                         wayptDataValid        = false;

      // Fuel data
      double currentInternalFuelQuantity_lbs = 0.0;
      double currentExternalFuelQuantity_lbs = 0.0;
      double currentTotalFuelQuantity_lbs    = 0.0;
      double internalFuelCapacity_lbs        = 0.0;
      double externalFuelCapacity_lbs        = 0.0;
      double totalFuelCapacity_lbs           = 0.0;
      double grossWeight_lbs                 = 0.0;
      double joker_lbs                       = 0.0;
      double bingo_lbs                       = 0.0;
      bool   bingoFlagActive                 = false;
      double fuelFlow_pph                    = 0.0;
      bool   fuelValid                       = false;

      // Weapons and expendable countermeasures data

      std::string          selectedWeapon;
      int                  numSelectedWeapon   = 0;
      bool                 selectedWeaponValid = false;
      HudEngagedTargetData engagedTargetData;
      bool                 engagedTargetValid = false;
      bool                 weaponBayDoorsOpen = false;
      bool                 supportingWeapon   = false;
      bool                 masterWarning      = false;
      bool                 masterCaution      = false;
      bool                 shootCueActive     = false;
      bool                 shootCueBlink      = false;
      bool                 masterArmActive    = false;
      bool                 jammingDetected    = false;
      int                  numChaff           = 0;
      int                  numFlares          = 0;
      int                  numDecoys          = 0;
   };

   HUD_DataContainer() = default;
   HUD_DataContainer(const HUD_DataContainer& aSrc);

   /** Returns true if the simulation has been initialized/reset, false otherwise. */
   bool GetSimulationInitialized() const;

   /** Set if the simulation is initialized or not.  Values reset on true.
    * @param aValue value to set.
    */
   void SetSimulationInitialized(bool aValue);

   /** Return a const reference to the platform data. */
   const std::map<std::string, PlatformData>& GetPlatformData() const { return mPlatformDatas; }

   /** Set the platform data.
    * @param aPlatformDatas data to set.
    */
   void SetPlatformData(const std::map<std::string, PlatformData>& aPlatformDatas);

signals:
   void Update_DataContainer();

private:
   bool mSimulationInitialized = false; ///< True if the simulation is initialized/reset, false otherwise.
   std::map<std::string, PlatformData> mPlatformDatas; ///< Map of platforms to their data containers.
};
} // namespace wkf

#endif // HUD_DATACONTAINER_HPP
