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

#ifndef WSFSIXDOFMOVERTESTOBJECT_HPP
#define WSFSIXDOFMOVERTESTOBJECT_HPP

#include "wsf_six_dof_export.h"

class UtInput;
class WsfDraw;
class WsfMoverGuidance;
#include "WsfSixDOF_Engine.hpp"
#include "WsfSixDOF_Utils.hpp"

namespace wsf
{
namespace six_dof
{
class Environment;
class Mover;
class PilotObject;
class ThrustProducerObject;

class WSF_SIX_DOF_EXPORT MoverTestObject
{
public:
   virtual Mover* GetParentVehicle() const = 0;

   std::unique_ptr<Engine> GetTestEngineFromThrustProducer(ThrustProducerObject* aEngine) const;

   void Update(double aSimTime);

   // This returns the great circle distance (in km) of the vehicle from its creation point
   double GetRangeSinceStart_km() const;

   // These commands control lag/freeze functionality, which are typically used during testing.
   // They are also used during tuning of the vehicle autopilot.

   // This enabled/disables system lags during testing
   void SetMasterNoLagTesting(bool aEnable);

   // This enabled/disables freezing of the vehicle in its current location (Lat/Lon) during testing
   void SetMasterFreezeLocation(bool aEnable);

   // This enabled/disables freezing of the vehicle at its current altitude during testing
   void SetMasterFreezeAltitude(bool aEnable);

   // This enabled/disables freezing of the vehicle at its current vertical speed during testing
   void SetMasterFreezeVerticalSpeed(bool aEnable);

   // This enabled/disables freezing of the vehicle at its current pitch angle during testing
   void SetMasterFreezePitch(bool aEnable);

   // This enabled/disables freezing of the vehicle at its current roll angle during testing
   void SetMasterFreezeRoll(bool aEnable);

   // This enabled/disables freezing of the vehicle at its current yaw angle during testing
   void SetMasterFreezeYaw(bool aEnable);

   // This enabled/disables freezing of the vehicle fuel burn during testing.
   // When enabled, it will not burn fuel.
   void SetMasterFreezeFuelBurn(bool aEnable);

   // This enabled/disables freezing of the vehicle speed during testing.
   void SetMasterFreezeSpeed(bool aEnable);

   // This enabled/disables freezing of the vehicle angle of attack (alpha) during testing.
   void SetMasterNoAlphaTesting(bool aEnable);

   // This creates an autopilot config file will the current autopilot data. it can be used
   // the Autotune function (above).
   virtual void CreateAutopilotConfigFile(const std::string& aFilename) = 0;

   // This function is used to determine if the vehicle can fly at the specified altitude (in feet),
   // velocity (in ft/sec), throttle setting, and flaps setting. The throttle input assumes that
   // 0 is idle, 1 is full power without augmentation/afterburner (military power), and 2 is
   // full power with full augmentation/afterburner (full afterburner). The flaps input assumes that
   // 0 is fully retracted and 1 is fully deployed. The function returns a CanFlyAtReturnValue,
   // which can be CAN_NOT_FLY, CONTROL_FAIL, or CAN_FLY.
   virtual utils::CanFlyAtReturnValue
   CanFlyAt(double aAltitude_ft, double aVelocity_fps, double aThrottle, double aFlaps = 0.0) = 0;

   // This returns the turn rate (deg/sec) with the specified specific excess power (in ft/sec),
   // true airspeed (in ft/sec), and throttle setting (where 0 is idle, 1 is military power
   // (full power without afterburner), and 2 is full afterburner).
   virtual double TurnRateAt(double aSpecificExcessPower_fps, double aTrueSpeed_fps, double aThrottleSetting) = 0;

   // This returns the turn rate (deg/sec) with the required specific excess power (ft/sec)
   // at the the specified conditions, including current mass (lbs), maximum g-load,
   // altitude (ft), airspeed (ft/sec), and throttle setting (where 0 is idle, 1 is
   // military power (full power without afterburner), and 2 is full afterburner).
   virtual double TurnRateWithConditions(double aSpecificExcessPower_fps,
                                         double aCurrentMass_lbs,
                                         double aMaxGLoad,
                                         double aAltitude_ft,
                                         double aAirspeed_fps,
                                         double aThrottleSetting) = 0;

   // This returns the horizontal thrust (in lbs) produced in level flight at the specified
   // altitude (ft) and speed (Mach) with the specified throttle setting and gross weight.
   // The throttle input assumes that 0 is idle, 1 is military power (full power without
   // afterburner), and 2 is full afterburner. Note that the thrust returned is the
   // horizontal component. The total thrust is likely greater due to vehicle pitch attitude.
   double GetLevelFlightHorizontalThrust(double aAltitude_ft, double aMach, double aThrottle, double aGrossWeight_lbs);

   // This returns the drag (in lbs) when in level flight at the specified altitude (ft) and
   // speed (Mach) with the specified gross weight.
   double GetLevelFlightDrag(double aAltitude_ft, double aMach, double aGrossWeight_lbs);

   // This returns the fuel burn rate (in lbs/hr) when in level flight at the specified
   // altitude (ft) and speed (Mach) with the specified gross weight.
   double GetLevelFlightFuelBurnRate(double aAltitude_ft, double aMach, double aGrossWeight_lbs);

   // This uses maximum/full throttle (value=2.0, afterburner). If the vehicle lacks an
   // afterburner, it will use a full throttle setting of 1.0 (military power) instead.
   void FlyStraightLevelFullThrottle();

   // This creates the autopilot support file for the current vehicle, outputting to the
   // specified filename.
   virtual void CreateAutopilotSupportFile(const std::string& aFilename) = 0;

   // This creates the autopilot support file for the current vehicle, outputting to the
   // specified filename. This variant includes a limit to max mach.
   virtual void CreateAutopilotSupportFileWithMaxMach(const std::string& aFilename, const double& aMaxMach) = 0;

   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, this will limit alpha to stay under the g-load limit.
   virtual void LimitAlphaByMaxGLoad(double aMach, double aMaxGLoad, double aDynPressure_ppsqft, double& aAlpha_rad) = 0;

   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, this will limit alpha to stay under the g-load limit.
   virtual void LimitAlphaByMaxGLoad(double  aMach,
                                     double  aMaxGLoad,
                                     double  aDynPressure_ppsqft,
                                     double  aCurrentMass_lbs,
                                     double& aAlpha_rad) = 0;

   // Modifies base vehicle mass, does NOT modify fuel mass. Used for testing.
   virtual void SetVehicleMass(double aMass_lbs) = 0;

   // Returns the max g-load as limited by autopilot. If no autopilot exists, it
   // returns zero. If more than one autopilot exists, it selects the autopilot
   // based on the following order:  synthetic pilot, augmented manual pilot,
   // guidance, and hardware autopilot.
   double GetMaxGLoad();

   // This provides the maximum instantaneous g-load as a function of altitude (ft) and
   // speed (Mach) for a specified mass (lbs). This is the maximum pitch g-load based on
   // conditions and does not consider any thrust effects. Hence, this can calculate the
   // g-load for speeds that may not be attainable under normal operation. This function
   // can be used for both aircraft and weapons.
   virtual double MaxInstantaneousGLoad(double aAltitude_ft, double aMach, double aCurrentMass_lbs) = 0;

protected:
   // This returns the angle of attack (alpha) in radians at the specified altitude (ft),
   // speed (ft/sec), and desired drag (lbs).
   virtual double AlphaAtDesiredDrag(double aAlt_ft, double aSpeed_fps, double aDrag_lbs, bool& aOk) = 0;

   // This provides the angle of attack (alpha) in radians at the specified altitude (ft),
   // speed (Mach), and desired drag (lbs) with the specified current mass and while
   // limited to the specified max g-load.
   virtual bool AlphaAtDesiredDrag(double  aAlt_ft,
                                   double  aMach,
                                   double  aDrag_lbs,
                                   double  aCurrentMass_lbs,
                                   double  aMaxGLoad,
                                   double& aAlpha_rad) = 0;

   // This is used to setup/configure for running in test mode
   void SetupForTestingMode(bool aFreezeFuelBurn);

   // This is used to restore from running in test mode
   void RestoreFromTestingMode();

   // This calculates the drag (lbs) and angle of attack (alpha, in deg) when in level flight
   // at the specified altitude (ft) and speed (Mach) with the specified gross weight.
   // Returns true if successful.
   virtual bool CalcLevelFlightDragAndAlpha(double  aAltitude_ft,
                                            double  aMach,
                                            double  aGrossWeight_lbs,
                                            double& aDrag_lbs,
                                            double& aAlpha_deg) = 0;

   // For the specified engine (thrust producer), this returns the throttle setting
   // (where 0 is idle, 1 is military power (full power without afterburner), and
   // 2 is full afterburner) for the specified altitude (ft) and speed (Mach) with
   // the specified required thrust (lbs).
   double ThrottleSettingToProduceThrust(ThrustProducerObject* aEngine,
                                         const Environment*    aAtm,
                                         double                aAltitude_ft,
                                         double                aMach,
                                         double                aRequiredThrust_lbs);

   // For the specified engine (thrust producer), this returns the fuel burn rate (lb/hr)
   // at the specified altitude(ft) and speed (Mach) with the specified required
   // thrust (lbs).
   double FuelBurnAtRequiredThrust(ThrustProducerObject* aEngine,
                                   const Environment*    aAtm,
                                   double                aAltitude_ft,
                                   double                aMach,
                                   double                aRequiredThrust_lbs);

   // For the specified engine (thrust producer), this returns the fuel burn rate (lb/hr)
   // at the specified altitude(ft) and speed (Mach) with the specified throttle setting
   // (where 0 is idle, 1 is military power (full power without afterburner), and 2 is
   // full afterburner).
   double FuelBurnAtThrottleSetting(ThrustProducerObject* aEngine,
                                    const Environment*    aAtm,
                                    double                aAltitude_ft,
                                    double                aMach,
                                    double                aThrottle);

   // For the specified engine (thrust producer), this returns the thrust (lbs)
   // at the specified altitude(ft) and speed (Mach) with the specified throttle setting
   // (where 0 is idle, 1 is military power (full power without afterburner), and 2 is
   // full afterburner).
   double ThrustAtThrottleSetting(ThrustProducerObject* aEngine,
                                  const Environment*    aEnv,
                                  double                aAltitude_ft,
                                  double                aMach,
                                  double                aThrottle);

   const Environment* GetEnvironment();

   bool mTestingNoLag          = false;
   bool mTestingFreezeFuelBurn = false;
};
} // namespace six_dof
} // namespace wsf

#endif
