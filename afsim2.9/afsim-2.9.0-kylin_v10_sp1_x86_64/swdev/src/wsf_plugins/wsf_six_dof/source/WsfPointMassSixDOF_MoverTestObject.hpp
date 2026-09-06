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

#ifndef WSFPOINTMASSSIXDOFMOVERTESTOBJECT_HPP
#define WSFPOINTMASSSIXDOFMOVERTESTOBJECT_HPP

#include "WsfPointMassSixDOF_Utils.hpp"
#include "WsfSixDOF_MoverTestObject.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PointMassMoverTestObject : public MoverTestObject
{
public:
   //! Constructor
   explicit PointMassMoverTestObject(PointMassMover* aParentMover);

   Mover* GetParentVehicle() const override;

   // This creates an autopilot config file will the current autopilot data. it can be used
   // the Autotune function (above).
   void CreateAutopilotConfigFile(const std::string& aFilename) override;

   // This function is used to determine if the vehicle can fly at the specified altitude (in feet),
   // velocity (in ft/sec), throttle setting, and flaps setting. The throttle input assumes that
   // 0 is idle, 1 is full power without augmentation/afterburner (military power), and 2 is
   // full power with full augmentation/afterburner (full afterburner). The flaps input assumes that
   // 0 is fully retracted and 1 is fully deployed. The function returns a CanFlyAtReturnValue,
   // which can be CAN_NOT_FLY, CONTROL_FAIL, or CAN_FLY.
   utils::CanFlyAtReturnValue CanFlyAt(double aAltitude_ft, double aVelocity_fps, double aThrottle, double aFlaps = 0.0) override;

   // This returns the turn rate (deg/sec) with the specified specific excess power (in ft/sec),
   // true airspeed (in ft/sec), and throttle setting (where 0 is idle, 1 is military power
   // (full power without afterburner), and 2 is full afterburner).
   double TurnRateAt(double aSpecificExcessPower_fps, double aTrueSpeed_fps, double aThrottleSetting) override;

   // This returns the turn rate (deg/sec) with the required specific excess power (ft/sec)
   // at the the specified conditions, including current mass (lbs), maximum g-load,
   // altitude (ft), airspeed (ft/sec), and throttle setting (where 0 is idle, 1 is
   // military power (full power without afterburner), and 2 is full afterburner).
   double TurnRateWithConditions(double aSpecificExcessPower_fps,
                                 double aCurrentMass_lbs,
                                 double aMaxGLoad,
                                 double aAltitude_ft,
                                 double aAirspeed_fps,
                                 double aThrottleSetting) override;

   // This creates the autopilot support file for the current vehicle, outputting to the
   // specified filename.
   void CreateAutopilotSupportFile(const std::string& aFilename) override;

   // This creates the autopilot support file for the current vehicle, outputting to the
   // specified filename. This variant includes a limit to max mach.
   void CreateAutopilotSupportFileWithMaxMach(const std::string& aFilename, const double& aMaxMach) override;

   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, this will limit alpha to stay under the g-load limit.
   void LimitAlphaByMaxGLoad(double aMach, double aMaxGLoad, double aDynPressure_ppsqft, double& aAlpha_rad) override;

   // In addition to limiting alpha, we may also need to limit the g-load.
   // If necessary, this will limit alpha to stay under the g-load limit.
   void LimitAlphaByMaxGLoad(double  aMach,
                             double  aMaxGLoad,
                             double  aDynPressure_ppsqft,
                             double  aCurrentMass_lbs,
                             double& aAlpha_rad) override;

   // Modifies base vehicle mass, does NOT modify fuel mass. Used for testing.
   void SetVehicleMass(double aMass_lbs) override;

   // This provides the maximum instantaneous g-load as a function of altitude (ft) and
   // speed (Mach) for a specified mass (lbs). This is the maximum pitch g-load based on
   // conditions and does not consider any thrust effects. Hence, this can calculate the
   // g-load for speeds that may not be attainable under normal operation. This function
   // can be used for both aircraft and weapons.
   double MaxInstantaneousGLoad(double aAltitude_ft, double aMach, double aCurrentMass_lbs) override;

protected:
   // This returns the angle of attack (alpha) in radians at the specified altitude (ft),
   // speed (ft/sec), and desired drag (lbs).
   double AlphaAtDesiredDrag(double aAlt_ft, double aSpeed_fps, double aDrag_lbs, bool& aOk) override;

   // This provides the angle of attack (alpha) in radians at the specified altitude (ft),
   // speed (Mach), and desired drag (lbs) with the specified current mass and while
   // limited to the specified max g-load.
   bool AlphaAtDesiredDrag(double  aAlt_ft,
                           double  aMach,
                           double  aDrag_lbs,
                           double  aCurrentMass_lbs,
                           double  aMaxGLoad,
                           double& aAlpha_rad) override;

   // This calculates the drag (lbs) and angle of attack (alpha, in deg) when in level flight
   // at the specified altitude (ft) and speed (Mach) with the specified gross weight.
   // Returns true if successful.
   bool CalcLevelFlightDragAndAlpha(double  aAltitude_ft,
                                    double  aMach,
                                    double  aGrossWeight_lbs,
                                    double& aDrag_lbs,
                                    double& aAlpha_deg) override;

   PointMassMover* mParentMover = nullptr;
};
} // namespace six_dof
} // namespace wsf

#endif
