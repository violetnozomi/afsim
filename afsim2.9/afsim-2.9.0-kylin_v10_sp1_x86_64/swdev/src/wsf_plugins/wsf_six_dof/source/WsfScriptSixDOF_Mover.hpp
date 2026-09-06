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

#ifndef WSFSCRIPTSIXDOFMOVER_HPP
#define WSFSCRIPTSIXDOFMOVER_HPP

#include "wsf_six_dof_export.h"

#include "script/WsfScriptMoverClass.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
//! WsfScriptSixDOF_Mover is a WsfScriptObjectClass that defines the methods in
//! WsfSixDOF_Mover. This makes the methods available for use in script.
class WSF_SIX_DOF_EXPORT Mover : public WsfScriptMoverClass
{
public:
   Mover(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // -----------------------------------------------------------------------

   // VEHICLE KINEMATIC STATE SUPPORT

   // Altitude
   UT_DECLARE_SCRIPT_METHOD(GetAltitude); // Altitude in meters

   // Attitude
   UT_DECLARE_SCRIPT_METHOD(GetHeading); // Heading in degrees
   UT_DECLARE_SCRIPT_METHOD(GetPitch);   // Pitch in degrees
   UT_DECLARE_SCRIPT_METHOD(GetRoll);    // Roll in degrees

   // Vertical Speed/Angle
   UT_DECLARE_SCRIPT_METHOD(GetVerticalSpeed);   // Vertical speed in meters/sec
   UT_DECLARE_SCRIPT_METHOD(GetFlightPathAngle); // Gamma in degrees

   // Angular Rates
   UT_DECLARE_SCRIPT_METHOD(GetYawRate);   // Yaw rate in deg/sec
   UT_DECLARE_SCRIPT_METHOD(GetPitchRate); // Pitch rate in deg/sec
   UT_DECLARE_SCRIPT_METHOD(GetRollRate);  // Roll rate in deg/sec

   // Accelerations
   UT_DECLARE_SCRIPT_METHOD(GetGLoad); // G-Load in Gees
   UT_DECLARE_SCRIPT_METHOD(GetNx);    // Nx in Gees
   UT_DECLARE_SCRIPT_METHOD(GetNy);    // Ny in Gees
   UT_DECLARE_SCRIPT_METHOD(GetNz);    // Nz in Gees

   // Airspeed and Mach
   UT_DECLARE_SCRIPT_METHOD(GetKCAS); // Speed in knots calibrated air speed
   UT_DECLARE_SCRIPT_METHOD(GetKIAS); // Speed in knots indicated air speed
   UT_DECLARE_SCRIPT_METHOD(GetKTAS); // Speed in knots true air speed
   UT_DECLARE_SCRIPT_METHOD(GetMach); // Speed in Mach

   // Dynamic Pressure
   UT_DECLARE_SCRIPT_METHOD(GetDynamicPressure); // Q in Pa

   // -----------------------------------------------------------------------

   // MASS PROPERTIES SUPPORT

   // Weights
   UT_DECLARE_SCRIPT_METHOD(GetTotalWeight);   // Weight/mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetCurrentWeight); // Weight/mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetEmptyWeight);   // Weight/mass in kg

   // Center of gravity/mass (CG/CM) relative to reference point
   UT_DECLARE_SCRIPT_METHOD(GetCgX); // CG location in meters
   UT_DECLARE_SCRIPT_METHOD(GetCgY); // CG location in meters
   UT_DECLARE_SCRIPT_METHOD(GetCgZ); // CG location in meters

   // -----------------------------------------------------------------------

   // ENGINE SUPPORT

   UT_DECLARE_SCRIPT_METHOD(StartupEngines);
   UT_DECLARE_SCRIPT_METHOD(ShutdownEngines);
   UT_DECLARE_SCRIPT_METHOD(IsProducingThrust);
   UT_DECLARE_SCRIPT_METHOD(GetTotalThrust);
   UT_DECLARE_SCRIPT_METHOD(GetEngineThrust);
   UT_DECLARE_SCRIPT_METHOD(GetAfterburnerOn);
   UT_DECLARE_SCRIPT_METHOD(GetEngineFuelFlowRate);
   UT_DECLARE_SCRIPT_METHOD(GetEngineAfterburnerOn);
   UT_DECLARE_SCRIPT_METHOD(SetFuelFeed);
   UT_DECLARE_SCRIPT_METHOD(SetFuelFeedAllEngines);

   // -----------------------------------------------------------------------

   // FUEL SYSTEM SUPPORT

   // Fuel System
   UT_DECLARE_SCRIPT_METHOD(AddFuel);                  // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetTotalFuelCapacity);     // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetInternalFuelCapacity);  // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetExternalFuelCapacity);  // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetTotalFuelRemaining);    // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetInternalFuelRemaining); // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetExternalFuelRemaining); // Fuel mass in kg

   // Fuel Tanks
   UT_DECLARE_SCRIPT_METHOD(GetFuelTankCapacity); // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(GetFuelInTank);       // Fuel mass in kg
   UT_DECLARE_SCRIPT_METHOD(SetFuelInTank);       // Fuel mass in kg

   // Fuel Flow
   UT_DECLARE_SCRIPT_METHOD(GetTotalFuelFlowRate); // Fuel flow in kg/sec

   // Fuel Transfers
   UT_DECLARE_SCRIPT_METHOD(AddFuelTransfer);
   UT_DECLARE_SCRIPT_METHOD(RemoveFuelTransfer);

   // Joker fuel state
   UT_DECLARE_SCRIPT_METHOD(SetJokerFuelState);   // Sets Joker fuel (kg)
   UT_DECLARE_SCRIPT_METHOD(GetJokerFuelState);   // Returns Joker fuel (kg)
   UT_DECLARE_SCRIPT_METHOD(GetJokerFuelReached); // Returns true if Joker

   // Bingo fuel state
   UT_DECLARE_SCRIPT_METHOD(SetBingoFuelState);   // Sets Bingo fuel (kg)
   UT_DECLARE_SCRIPT_METHOD(GetBingoFuelState);   // Returns Bingo fuel (kg)
   UT_DECLARE_SCRIPT_METHOD(GetBingoFuelReached); // Returns true if Bingo

   // -----------------------------------------------------------------------

   // SEQUENCER SUPPORT

   // Sequencer Activation
   UT_DECLARE_SCRIPT_METHOD(ActivateSequencer);

   // -----------------------------------------------------------------------

   // AUTOPILOT SUPPORT

   // Autopilot commands
   UT_DECLARE_SCRIPT_METHOD(EnableAutopilot);

   UT_DECLARE_SCRIPT_METHOD(SetAutopilotAltitude);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotVerticalSpeed);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotPitchAngle);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotFlightPathAngle);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotPitchRate);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotDeltaPitch);
   UT_DECLARE_SCRIPT_METHOD(SetPitchGLoad);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotRollAngle);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotRollRate);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotDeltaRoll);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotSpeedKTAS);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotSpeedKCAS);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotSpeedKIAS);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotSpeedMach);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotThrottle);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotLateralWaypointMode);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotVerticalWaypointMode);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotSpeedWaypointMode);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotWaypointMode);
   UT_DECLARE_SCRIPT_METHOD(SetAutopilotNoControl);

   // Returns the active pilot/controller
   UT_DECLARE_SCRIPT_METHOD(GetActivePilot);

   // Sets the active pilot/controller
   UT_DECLARE_SCRIPT_METHOD(ActivateSimpleManualPilot);
   UT_DECLARE_SCRIPT_METHOD(ActivateAugmentedManualPilot);
   UT_DECLARE_SCRIPT_METHOD(ActivateSyntheticPilot);

   // These provide modes/values for the three autopilot channels
   UT_DECLARE_SCRIPT_METHOD(GetAutopilotLateralMode);
   UT_DECLARE_SCRIPT_METHOD(GetAutopilotLateralModeValue);
   UT_DECLARE_SCRIPT_METHOD(GetAutopilotVerticalMode);
   UT_DECLARE_SCRIPT_METHOD(GetAutopilotVerticalModeValue);
   UT_DECLARE_SCRIPT_METHOD(GetAutopilotSpeedMode);
   UT_DECLARE_SCRIPT_METHOD(GetAutopilotSpeedModeValue);

   // Current autopilot limits
   UT_DECLARE_SCRIPT_METHOD(GetPitchGLoadMin);
   UT_DECLARE_SCRIPT_METHOD(GetPitchGLoadMax);
   UT_DECLARE_SCRIPT_METHOD(GetAlphaMin);
   UT_DECLARE_SCRIPT_METHOD(GetAlphaMax);
   UT_DECLARE_SCRIPT_METHOD(GetPitchRateMin);
   UT_DECLARE_SCRIPT_METHOD(GetPitchRateMax);
   UT_DECLARE_SCRIPT_METHOD(GetVerticalSpeedMin);
   UT_DECLARE_SCRIPT_METHOD(GetVerticalSpeedMax);
   UT_DECLARE_SCRIPT_METHOD(GetYawGLoadMax);
   UT_DECLARE_SCRIPT_METHOD(GetBetaMax);
   UT_DECLARE_SCRIPT_METHOD(GetYawRateMax);
   UT_DECLARE_SCRIPT_METHOD(GetRollRateMax);
   UT_DECLARE_SCRIPT_METHOD(GetBankAngleMax);
   UT_DECLARE_SCRIPT_METHOD(GetForwardGLoadMin);
   UT_DECLARE_SCRIPT_METHOD(GetForwardGLoadMax);

   // Set autopilot limits
   UT_DECLARE_SCRIPT_METHOD(SetPitchGLoadMin);
   UT_DECLARE_SCRIPT_METHOD(SetPitchGLoadMax);
   UT_DECLARE_SCRIPT_METHOD(SetAlphaMin);
   UT_DECLARE_SCRIPT_METHOD(SetAlphaMax);
   UT_DECLARE_SCRIPT_METHOD(SetPitchRateMin);
   UT_DECLARE_SCRIPT_METHOD(SetPitchRateMax);
   UT_DECLARE_SCRIPT_METHOD(SetVerticalSpeedMin);
   UT_DECLARE_SCRIPT_METHOD(SetVerticalSpeedMax);
   UT_DECLARE_SCRIPT_METHOD(SetYawGLoadMax);
   UT_DECLARE_SCRIPT_METHOD(SetBetaMax);
   UT_DECLARE_SCRIPT_METHOD(SetYawRateMax);
   UT_DECLARE_SCRIPT_METHOD(SetRollRateMax);
   UT_DECLARE_SCRIPT_METHOD(SetBankAngleMax);
   UT_DECLARE_SCRIPT_METHOD(SetForwardGLoadMin);
   UT_DECLARE_SCRIPT_METHOD(SetForwardGLoadMax);

   // Turn roll-in controls
   UT_DECLARE_SCRIPT_METHOD(SetTurnRollInMultiplier);
   UT_DECLARE_SCRIPT_METHOD(GetCurrentTurnRollInMultiplier);
   UT_DECLARE_SCRIPT_METHOD(GetDefaultTurnRollInMultiplier);

   // Allowable angle error values
   UT_DECLARE_SCRIPT_METHOD(SetRouteAllowableAngleError);
   UT_DECLARE_SCRIPT_METHOD(GetCurrentRouteAllowableAngleError);
   UT_DECLARE_SCRIPT_METHOD(GetDefaultRouteAllowableAngleError);

   // Revert autopilot limits to default values
   UT_DECLARE_SCRIPT_METHOD(RevertLimitsToDefaults);

   // -----------------------------------------------------------------------

   // MANEUVER SUPPORT

   UT_DECLARE_SCRIPT_METHOD(GetCurrentManeuver);
   UT_DECLARE_SCRIPT_METHOD(GetManeuverSequence);
   UT_DECLARE_SCRIPT_METHOD(ExecuteManeuver);
   UT_DECLARE_SCRIPT_METHOD(ExecuteManeuverSequence);
   UT_DECLARE_SCRIPT_METHOD(CancelManeuvers);

   // -----------------------------------------------------------------------

   // APPEARANCE SUPPORT

   // Engine Operation Effects
   UT_DECLARE_SCRIPT_METHOD(GetEngineIsOperating);
   UT_DECLARE_SCRIPT_METHOD(GetEngineIsSmoking);
   UT_DECLARE_SCRIPT_METHOD(GetEngineAfterburnerIsOn);

   // Trailing Effects
   UT_DECLARE_SCRIPT_METHOD(GetContrailTrailingEffect);
   UT_DECLARE_SCRIPT_METHOD(GetRocketSmokeTrailingEffect);
   UT_DECLARE_SCRIPT_METHOD(GetDamageSmokeTrailingEffect);

   // Launch Effects
   UT_DECLARE_SCRIPT_METHOD(GetLaunchFlashSmokeIsPresent);

   // Damage Appearance
   UT_DECLARE_SCRIPT_METHOD(GetRisingSmokePlumeIsPresent);
   UT_DECLARE_SCRIPT_METHOD(GetFlamesArePresent);
   UT_DECLARE_SCRIPT_METHOD(GetIsLightlyDamaged);
   UT_DECLARE_SCRIPT_METHOD(GetIsHeavilyDamaged);

   // -----------------------------------------------------------------------

   // DAMAGE AND DESTRUCTION SUPPORT

   // Damaged
   UT_DECLARE_SCRIPT_METHOD(SetLightDamage);
   UT_DECLARE_SCRIPT_METHOD(SetHeavyDamage);

   // Destroyed
   UT_DECLARE_SCRIPT_METHOD(SetDestroyed);

   // -----------------------------------------------------------------------

   // CONTROL INPUTS SUPPORT

   // Thrust Vector Control
   UT_DECLARE_SCRIPT_METHOD(EnableThrustVectoring);

   // Enable/disable Overall Control
   UT_DECLARE_SCRIPT_METHOD(EnableControls);

   // Direct Control Functions
   UT_DECLARE_SCRIPT_METHOD(TakeDirectControlInput);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectControlInput);
   UT_DECLARE_SCRIPT_METHOD(SetDirectControlInputs);

   // Direct throttle controls
   UT_DECLARE_SCRIPT_METHOD(EnableDirectThrottleInput);
   UT_DECLARE_SCRIPT_METHOD(MoveThrottleToIdle);
   UT_DECLARE_SCRIPT_METHOD(MoveThrottleToFull);
   UT_DECLARE_SCRIPT_METHOD(MoveThrottleToMilitary);
   UT_DECLARE_SCRIPT_METHOD(MoveThrottleToAfterburner);
   UT_DECLARE_SCRIPT_METHOD(SetDirectThrottleInput);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectThrottleInput);

   // Direct stick and rudder controls
   UT_DECLARE_SCRIPT_METHOD(EnableDirectStickBackInput);
   UT_DECLARE_SCRIPT_METHOD(SetDirectStickBackInput);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectStickBackInput);

   UT_DECLARE_SCRIPT_METHOD(EnableDirectStickRightInput);
   UT_DECLARE_SCRIPT_METHOD(SetDirectStickRightInput);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectStickRightInput);

   UT_DECLARE_SCRIPT_METHOD(EnableDirectRudderRightInput);
   UT_DECLARE_SCRIPT_METHOD(SetDirectRudderRightInput);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectRudderRightInput);

   // Direct speed brake controls
   UT_DECLARE_SCRIPT_METHOD(EnableDirectSpeedBrakeInput);
   UT_DECLARE_SCRIPT_METHOD(OpenSpeedBrake);
   UT_DECLARE_SCRIPT_METHOD(CloseSpeedBrake);
   UT_DECLARE_SCRIPT_METHOD(ReleaseDirectSpeedBrakeInput);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedBrakePosition); // Returns the position/angle (deg)

   // Flaps controls
   UT_DECLARE_SCRIPT_METHOD(SetFlapsPosition);
   UT_DECLARE_SCRIPT_METHOD(GetFlapsPosition); // Returns the position/angle (deg)

   // Spoilers controls
   UT_DECLARE_SCRIPT_METHOD(SetSpoilersPosition);
   UT_DECLARE_SCRIPT_METHOD(GetSpoilersPosition); // Returns the position/angle (deg)

   // Flight control inputs
   UT_DECLARE_SCRIPT_METHOD(GetStickRightPosition);          // Range of -1 to +1
   UT_DECLARE_SCRIPT_METHOD(GetStickBackPosition);           // Range of -1 to +1
   UT_DECLARE_SCRIPT_METHOD(GetRudderRightPosition);         // Range of -1 to +1
   UT_DECLARE_SCRIPT_METHOD(GetThrottleMilitaryPosition);    // Range of 0 to 1
   UT_DECLARE_SCRIPT_METHOD(GetThrottleAfterburnerPosition); // Range of 0 to 1
   UT_DECLARE_SCRIPT_METHOD(GetSpeedBrakeControlPosition);   // Range of 0 to 1
   UT_DECLARE_SCRIPT_METHOD(GetFlapsLeverPosition);          // Range of 0 to 1
   UT_DECLARE_SCRIPT_METHOD(GetSpoilersLeverPosition);       // Range of 0 to 1

   // -----------------------------------------------------------------------

   // BRAWLER MOVER COMMAND SUPPORT

   UT_DECLARE_SCRIPT_METHOD(Slice_1);
   UT_DECLARE_SCRIPT_METHOD(Slice_2);
   UT_DECLARE_SCRIPT_METHOD(Prlvl);
   UT_DECLARE_SCRIPT_METHOD(Prlvlg);

   // -----------------------------------------------------------------------

   // FORCES AND MOMENTS (F&M) SUPPORT

   // Forces and Moments (F&M)
   UT_DECLARE_SCRIPT_METHOD(GetLift);            // Lift in Newtons
   UT_DECLARE_SCRIPT_METHOD(GetDrag);            // Drag in Newtons
   UT_DECLARE_SCRIPT_METHOD(GetSideForce);       // Side force in Newtons
   UT_DECLARE_SCRIPT_METHOD(GetThrustMagnitude); // Thrust magnitude in Newtons

   // -----------------------------------------------------------------------

   // AERODYNAMICS SUPPORT

   // Alpha (angle of attack)
   UT_DECLARE_SCRIPT_METHOD(GetAlpha);    // Alpha in degrees
   UT_DECLARE_SCRIPT_METHOD(GetAlphaDot); // Alpha-Dot in degrees/second

   // Beta (angle of sideslip)
   UT_DECLARE_SCRIPT_METHOD(GetBeta);    // Beta in degrees
   UT_DECLARE_SCRIPT_METHOD(GetBetaDot); // Beta-Dot in degrees/second

   // Provides CLmax at the specified Mach
   UT_DECLARE_SCRIPT_METHOD(GetCLMaxAtMach);

   // Provides alpha (degrees) at CLmax for the specified Mach
   UT_DECLARE_SCRIPT_METHOD(GetAlphaAtCLMaxAtMach);

   // Provides maximum g-load possible (in the primary lift direction)
   UT_DECLARE_SCRIPT_METHOD(MaxPotentialManeuverGLoad);

   // -----------------------------------------------------------------------

   // AIRSPEED CALCULATIONS SUPPORT

   // Airspeed/Mach calculation support
   UT_DECLARE_SCRIPT_METHOD(CalcKtasFromKcas);
   UT_DECLARE_SCRIPT_METHOD(CalcKtasFromMach);
   UT_DECLARE_SCRIPT_METHOD(CalcKtasFromFps);
   UT_DECLARE_SCRIPT_METHOD(CalcKcasFromKtas);
   UT_DECLARE_SCRIPT_METHOD(CalcKcasFromMach);
   UT_DECLARE_SCRIPT_METHOD(CalcKcasFromFps);
   UT_DECLARE_SCRIPT_METHOD(CalcMachFromKcas);
   UT_DECLARE_SCRIPT_METHOD(CalcMachFromKtas);
   UT_DECLARE_SCRIPT_METHOD(CalcMachFromFps);
   UT_DECLARE_SCRIPT_METHOD(CalcFpsFromKcas);
   UT_DECLARE_SCRIPT_METHOD(CalcFpsFromKtas);
   UT_DECLARE_SCRIPT_METHOD(CalcFpsFromMach);
   UT_DECLARE_SCRIPT_METHOD(CalcDynamicPressure);
   UT_DECLARE_SCRIPT_METHOD(CalcFpsFromAltitudeDynamicPressure);

   // -----------------------------------------------------------------------

   // TESTING SUPPORT

   // The following functions are for Testing and should not be documented,
   //  to hide them from normal users
   UT_DECLARE_SCRIPT_METHOD(GetTestSupportObject);
   UT_DECLARE_SCRIPT_METHOD(SetTestingIgnoreAllCrashes);
   UT_DECLARE_SCRIPT_METHOD(GetTestingIgnoreAllCrashes);
   UT_DECLARE_SCRIPT_METHOD(SetTestingClampToSeaLevelMinAlt);
   UT_DECLARE_SCRIPT_METHOD(GetTestingClampToSeaLevelMinAlt);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif
