// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFP6DOF_MOVER_HPP
#define WSFP6DOF_MOVER_HPP

#include "wsf_p6dof_export.h"

#include <list>

#include "P6DofPilotObject.hpp"
#include "P6DofRoute.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicle.hpp"
#include "P6DofVehicleType.hpp"
#include "P6DofWaypoint.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtQuaternion.hpp"
#include "WsfMover.hpp"
#include "WsfP6DOF_MoverTestObject.hpp"
#include "WsfRoute.hpp"
#include "WsfWaypoint.hpp"

class P6DofPilotManager;
class UtInput;
class WsfDraw;
class WsfManeuver;
class WsfManeuverSequence;
class WsfMoverGuidance;
class WsfVisualPart;

// ====================================================================================================================
//! This is a pseudo-6DOF (P6DOF) mover. It includes 6 degrees-of-freedom, but includes a slightly simplified
//! rotation kinematics model. This model provides many 6DOF capabilities but is easier to use, easier to understand,
//! and faster running than a full-up 6DOF model.
// ====================================================================================================================

class WSF_P6DOF_EXPORT WsfP6DOF_Mover : public WsfMover
{
public:
   //! Constructor
   explicit WsfP6DOF_Mover(WsfScenario& aScenario);
   WsfP6DOF_Mover(const WsfP6DOF_Mover& aSrc);

   //! Virtual destructor
   ~WsfP6DOF_Mover() override;

   WsfP6DOF_Mover& operator=(const WsfP6DOF_Mover& other) = delete;

   WsfMover* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;
   void Update(double aSimTime) override;

   // TODO -- At the present time, only the AIR domain is specified, but P6DOF can support AIR and SPACE domains.
   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   // Returns a pointer to the P6DOF vehicle object
   P6DofVehicle* GetP6DofVehicle() const { return mP6DofVehiclePtr; }

   static int GetTypeId() { return WsfStringId::UnsafeGetNumber("WSF_P6DOF_MOVER"); }

   // Return the index of the planned route
   size_t GetPlannedRouteIndex() const;

   // Return the index of the temporary route
   size_t GetTempRouteIndex() const;

   // Sets the P6DOF vehicle to the specified NED heading (in radians)
   void SetHeading(double aHeading) override;

   // Sets the P6DOF vehicle to the specified NED pitch angle (in radians)
   void SetPitch(double aPitch) override;

   // Sets the P6DOF vehicle to the specified NED roll angle (in radians)
   void SetRoll(double aRoll) override;

   // Sets the orientation of the P6DOF vehicle to the specified values (in radians)
   void SetOrientationNED(double aHeading, double aPitch, double aRoll) override;

   // This provides a set of top-level state data for the P6DOF vehicle
   void GetStateData(double& aSimTime_sec,
                     double& aLat,
                     double& aLon,
                     double& aAlt_m,
                     double& aLocalHeading_deg,
                     double& aLocalPitch_deg,
                     double& aLocalRoll_deg,
                     double& aAlpha_deg,
                     double& aBeta_deg,
                     double& aSpeed_kts,
                     double& aVertSpeed_fpm,
                     double& aYawRate_dps,
                     double& aPitchRate_dps,
                     double& aRollRate_dps) const;

   // This returns the P6DOF vehicle type name for the WsfP6DOF_Mover
   std::string GetP6DofVehicleTypeName() const;

   // This returns the P6DOF vehicle type from which the vehicle of the WsfP6DOF_Mover was derived.
   std::string GetP6DofVehicleTypeDerivedFromName() const;

   // This returns the base name for the P6DOF vehicle type if it is a subobject. If it is not a subobject,
   // an empty string will be returned.
   std::string GetP6DofVehicleTypeSubobjectBaseName() const;

   // ====================================================================================================================

   // State Variable Accessors -- Get platform state info specific to P6DOF objects

   // Returns vertical speed in m/sec
   double GetVerticalSpeed();

   // Returns yaw rate in rad/sec
   double GetYawRate();

   // Returns pitch rate in rad/sec
   double GetPitchRate();

   // Returns roll rate in rad/sec
   double GetRollRate();

   // Returns flight path angle in radians
   const double GetFlightPathAngle_rad();

   // Returns flight path angle in degrees
   double GetFlightPathAngle();

   // Returns the current g-load in the vehicle's z-axis. Positive g-load is in the negative z direction.
   double GetGLoad();

   // OnPlatformBrokenEvent is called for all platform parts when a platform is "broken", which is typically the result
   // of combat damage or destruction. By default, most platform parts are "turned off" with this function, but
   // WsfP6DOF_Mover overrides the function to prevent being turned off, since the mover may continue to move.
   void OnPlatformBrokenEvent(double aSimTime) override;

   // ====================================================================================================================

   // Direct Control Input -- These functions allow direct control input from AFSIM script

   // This should be called to take direct control of the P6DOF object before using SetDirectStickAndRudderInput,
   // SetDirectThrottleInput, and SetDirectControlInputs.
   void TakeDirectControlInput();

   // This should be called to release direct control of the P6DOF object, which will disable the use of
   // SetDirectStickAndRudderInput, SetDirectThrottleInput, and SetDirectControlInputs.
   void ReleaseDirectControlInput();

   // The aStickX value assumes that 1 is full right stick, 0 is centered, and -1 is full left stick.
   // The aStickY value assumes that 1 is full back stick, 0 is centered, and -1 is full forward stick.
   // The aRudder value assumes that 1 is full right rudder, 0 is centered, and -1 is full left rudder.
   // The aThrottleLeverPosition value assumes that 0 is idle, 1 is full power without augmentation/afterburner
   // (military power), and 2 is full power with full augmentation/afterburner (full afterburner).
   // This function requires that TakeManualControl be called prior to use.
   void SetDirectControlInputs(double aStickX, double aStickY, double aRudder, double aThrottleLeverPosition);

   // ====================================================================================================================

   // CommonController/autopilot control commands

   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;

   bool TurnToHeading(double                 aSimTime,
                      double                 aHeading,     // radians
                      double                 aRadialAccel, // meters/second^2
                      WsfPath::TurnDirection aTurnDirection) override;

   bool GoToSpeed(double aSimTime,
                  double aSpeed, // meters/sec
                  double aLinearAccel,
                  bool   aKeepRoute) override;

   bool GoToAltitude(double aSimTime,
                     double aAltitude, // meters
                     double aClimbDiveRate,
                     bool   aKeepRoute) override;

   bool TurnToRelativeHeading(double                 aSimTime,
                              double                 aHeadingChange,
                              double                 aRadialAccel,
                              WsfPath::TurnDirection aTurnDirection) override;

   // This allows enabling/disabling the autopilot
   void EnableAutopilot(bool aState);

   // Returns the minimum potential thrust in lbs at the current flight conditions
   double GetMinimumPotentialThrust_lbs() const;

   // Returns the maximum potential thrust in lbs at the current flight conditions
   double GetMaximumPotentialThrust_lbs() const;

   // Returns the current alpha (angle of attack) in radians
   double GetAlpha_rad() const;

   // Returns the current alpha (angle of attack) in degrees
   double GetAlpha_deg() const;

   // Returns the current beta (angle of sideslip) in radians
   double GetBeta_rad() const;

   // Returns the current beta (angle of sideslip) in degrees
   double GetBeta_deg() const;

   // Returns the current angle of attack rate (alpha-dot) in radians/sec
   double GetAlphaDot_rps() const;

   // Returns the current angle of attack rate (alpha-dot) in degrees/sec
   double GetAlphaDot_dps() const;

   // This returns the current angle of sideslip rate (beta-dot) in radians/sec
   double GetBetaDot_rps() const;

   // This returns the current angle of sideslip rate (beta-dot) in degrees/sec
   double GetBetaDot_dps() const;

   // This returns the current center of gravity in meters
   UtVec3dX GetCg() const;

   // This returns the current heading in radians
   double GetHeading_rad() const;

   // This returns the current heading in degrees
   double GetHeading() const;

   // This returns the current pitch in radians
   double GetPitch_rad() const;

   // This returns the current pitch in degrees
   double GetPitch() const;

   // This returns the current roll in radians
   double GetRoll_rad() const;

   // This returns the current roll in degrees
   double GetRoll() const;

   // Returns the most recent lift in newtons
   double GetLift_nt() const;

   // Returns the most recent lift in lbs
   double GetLift_lbs() const;

   // Returns the most recent drag in newtons
   double GetDrag_nt() const;

   // Returns the most recent drag in lbs
   double GetDrag_lbs() const;

   // Returns the most recent side force in newtons
   double GetSideForce_nt() const;

   // Returns the most recent side force in lbs
   double GetSideForce_lbs() const;

   // This returns the thrust magnitude in newtons
   double GetThrustMagnitude_nt() const;

   // This returns the thrust magnitude in lbs
   double GetThrustMagnitude_lbs() const;

   // Returns the most recent yaw moment in newton*meters
   double GetYawMoment_nm() const;

   // Returns the most recent yaw moment in ft*lbs
   double GetYawMoment_ftlbs() const;

   // Returns the most recent pitch moment in newton*meters
   double GetPitchMoment_nm() const;

   // Returns the most recent pitch moment in ft*lbs
   double GetPitchMoment_ftlbs() const;

   // Returns the most recent roll moment in newton*meters
   double GetRollMoment_nm() const;

   // Returns the most recent roll moment in ft*lbs
   double GetRollMoment_ftlbs() const;

   // Returns the current speed in ft/sec
   double GetSpeed_fps() const;

   // Returns the current speed in KIAS
   double GetSpeed_KIAS() const;

   // Returns the current speed in KCAS
   double GetSpeed_KCAS() const;

   // Returns the current speed in KTAS
   double GetSpeed_KTAS() const;

   // Returns the current speed in Mach
   double GetSpeed_Mach() const;

   // Returns the current mass in lbs
   double GetCurrentWeight_lbs() const;

   // Returns the current mass in kg
   double GetCurrentWeight_kg() const;

   // Returns the empty mass in lbs
   double GetEmptyWeight_lbs() const;

   // Returns the empty mass in kg
   double GetEmptyWeight_kg() const;

   // Returns the design weight for max-g maneuvering in lbs
   double GetDesignWeightForMaxG_lbs() const; // Note, this currently uses empty weight

   // Returns the current altitude in meters
   double GetAlt_m() const;

   // Returns the wing area in feet
   double GetWingArea_sqft() const;

   // Returns the forward acceleration in body coordinates in gees
   double GetNx_g() const;

   // Returns the side acceleration in body coordinates in gees
   double GetNy_g() const;

   // Returns the downward acceleration in body coordinates in gees
   double GetNz_g() const;

   // This returns a 'handle' to a control surface. If there is no control surface with the specified name, the function
   // returns 0;
   size_t GetControlSurfaceHandle(const std::string& aControlSurfaceName) const;

   // This returns a 'handle' to the first control surface that contains the specified string. If there is no control
   // surface that contains the name, the function returns 0;
   size_t GetControlSurfaceHandleContainingString(const std::string& aString) const;

   // This returns a 'handle' to the first control surface that contains the two specified strings. If there is no
   // control surface that contains the names, the function returns 0;
   size_t GetControlSurfaceHandleContainingTwoStrings(const std::string& aString1, const std::string& aString2) const;

   // Returns the position (deg) of the specified control surface, or returns zero if no surface exists
   double GetAngleOfControlSurface(std::string aControlSurfaceName) const;

   // Returns the position (deg) of the specified control surface (based on handle), or returns zero if no surface exists
   double GetAngleOfControlSurface(size_t aHandle) const;

   // Returns a normalized (either +/-1 or 0-1) angle of the specified control surface (based on handle),
   // or returns zero if no surface exists
   double GetNormalizedAngleOfControlSurface(size_t aHandle) const;

   // Returns a list(vector) containing the names of the control surfaces
   std::vector<std::string> GetListOfControlSurfaceNames() const;

   // Returns the position (deg) of the ailerons (common position)
   double GetAileronsBothPosition() const;

   // Returns the position (deg) of the left stabilator
   double GetStabilatorLeftPosition() const;

   // Returns the position (deg) of the right stabilator
   double GetStabilatorRightPosition() const;

   // Returns the position (deg) of the rudder(s) (common position)
   double GetRudderPosition() const;

   // Returns the position (deg) of the landing gear (common position)
   double GetLandingGearPosition() const;

   // Returns the position of the landing gear lever
   double GetLandingGearLeverPosition() const;

   // Returns the position (rad) of the speed brakes (common position)
   double GetSpeedBrakePosition_rad() const;

   // Returns the position (deg) of the speed brakes (common position)
   double GetSpeedBrakePosition() const;

   // Returns the position (deg) of the flaps (common position)
   double GetFlapsPosition() const;

   // Returns the position of the flaps lever
   double GetFlapsLeverPosition() const;

   // Returns the position (deg) of the spoilers (common position)
   double GetSpoilersPosition() const;

   // Returns the position of the spoilers lever
   double GetSpoilersLeverPosition() const;

   // Returns the position (deg) of the nose wheel steering angle
   double GetNoseWheelAngle() const;

   // These functions return the control surface angle in degrees
   double GetAngleAileronLeft() const;
   double GetAngleAileronRight() const;
   double GetAngleFlapLeft() const;
   double GetAngleFlapRight() const;
   double GetAngleSpoilerLeft() const;
   double GetAngleSpoilerRight() const;
   double GetAngleStabilizerLeft() const;
   double GetAngleStabilizerRight() const;
   double GetAngleElevator() const;
   double GetAngleRudderLeft() const;
   double GetAngleRudder() const;
   double GetAngleRudderRight() const;
   double GetAngleSpeedBrake() const;
   double GetAngleLandingGear() const;
   double GetAngleLandingGearNose() const;
   double GetAngleLandingGearMainLeft() const;
   double GetAngleLandingGearMainRight() const;

   // These functions return the normalized control surface angle
   double GetNormalizedAileronLeft() const;
   double GetNormalizedAileronRight() const;
   double GetNormalizedFlapLeft() const;
   double GetNormalizedFlapRight() const;
   double GetNormalizedSpoilerLeft() const;
   double GetNormalizedSpoilerRight() const;
   double GetNormalizedStabilizerLeft() const;
   double GetNormalizedStabilizerRight() const;
   double GetNormalizedElevator() const;
   double GetNormalizedRudderLeft() const;
   double GetNormalizedRudder() const;
   double GetNormalizedRudderRight() const;
   double GetNormalizedSpeedBrake() const;
   double GetNormalizedLandingGear() const;
   double GetNormalizedLandingGearNose() const;
   double GetNormalizedLandingGearMainLeft() const;
   double GetNormalizedLandingGearMainRight() const;

   // These functions return true if the data for that control surface is valid
   bool AileronLeftValid() const;
   bool AileronRightValid() const;
   bool FlapLeftValid() const;
   bool FlapRightValid() const;
   bool SpoilerLeftValid() const;
   bool SpoilerRightValid() const;
   bool StabilizerLeftValid() const;
   bool StabilizerRightValid() const;
   bool ElevatorValid() const;
   bool RudderLeftValid() const;
   bool RudderValid() const;
   bool RudderRightValid() const;
   bool SpeedBrakeValid() const;
   bool LandingGearValid() const;
   bool LandingGearNoseValid() const;
   bool LandingGearMainLeftValid() const;
   bool LandingGearMainRightValid() const;
   bool ThrottleValid() const;

   // These functions return the normalized control input (lever) value
   double GetStickRightInput() const;
   double GetStickBackInput() const;
   double GetRudderRightInput() const;
   double GetThrottleInput() const;
   double GetSpeedBrakeInput() const;
   double GetSpoilerInput() const;
   double GetLandingGearInput() const;

   // This returns true if the positive-side stall warning is occurring
   bool StallWarningAlert() const;

   // GetAeroParametersAtAltitude returns several parameters for a vehicle at the specified altitude (ft) and
   // speed (ft/sec). Parameters include air density [ ((lbf*sec^2)/ft^4) or slugs/ft^3 ], dynamic pressure (lbf/sq-ft),
   // static pressure (lbf/sq-ft), and Mach.
   void GetAeroParametersAtAltitude(double  aAltitude_ft,
                                    double  aSpeed_fps,
                                    double& aAirDensity,
                                    double& aDynamicPressure_psf,
                                    double& aStaticPressure_psf,
                                    double& aSpeedOfSound_fps,
                                    double& aMach) const;

   // Returns the current dynamic pressure for the object in lbs/sq-ft
   double GetDynamicPressure_psf() const;

   // This returns the CLmax value for the specified Mach
   double GetCLMaxAtMach(double aMach) const;

   // This returns the alpha (in degrees) at CLmax for the specified Mach
   double GetAlphaAtCLMaxAtMach_deg(double aMach) const;

   // This returns the type of the active pilot object as a string
   std::string GetActivePilot() const;

   // This makes the manual pilot active (if present)
   void ActivateManualPilot();

   // This makes the manual pilot active (if present)
   void ActivateSyntheticPilot();

   // This makes the manual pilot active (if present)
   void ActivateHardwareAutopilot();

   // This makes the manual pilot active (if present)
   void ActivateGuidanceAutopilot();

   // This returns the autopilot's lateral mode as an enum
   P6DOF::Lateral::Mode GetAutopilotLateralModeEnum() const;

   // This returns the autopilot's lateral mode as a string
   std::string GetAutopilotLateralMode() const;

   // This returns the value of the autopilot's lateral mode
   float GetAutopilotLateralModeValue() const;

   // This returns the autopilot's vertical mode as an enum
   P6DOF::Vertical::Mode GetAutopilotVerticalModeEnum() const;

   // This returns the autopilot's vertical mode as a string
   std::string GetAutopilotVerticalMode() const;

   // This returns the value of the autopilot's vertical mode
   float GetAutopilotVerticalModeValue() const;

   // This returns the autopilot's speed mode as an enum
   P6DOF::Speed::Mode GetAutopilotSpeedModeEnum() const;

   // This returns the autopilot's speed mode as a string
   std::string GetAutopilotSpeedMode() const;

   // This returns the value of the autopilot's speed mode
   float GetAutopilotSpeedModeValue() const;

   // ====================================================================================================================

   bool ReturnToRoute(double aSimTime) override;

   // FollowRoute_1: FollowRoute(WsfRoute aRoute);
   // FollowRoute_2: FollowRoute(WsfRoute aRoute, string aRouteAction);
   // FollowRoute_3: FollowRoute(string aRouteName);
   // FollowRoute_4: FollowRoute(string aRouteName, string aRouteAction);
   // FollowRoute_5: FollowRoute(WsfRoute aRoute, int aIndex);
   // FollowRoute_6: FollowRoute(string aRouteName, int aIndex);
   // Implementing UpdateRoute() will enable FollowRoute_1 & FollowRoute_3
   bool UpdateRoute(double aSimTime, const WsfRoute& aRoute) override;

   // Implementing GoToClosestPoint() & GoToLabel() will enable FollowRoute_2 & FollowRoute_4
   // virtual bool GoToClosestPoint(double aSimTime);
   // virtual bool GoToLabel(double             aSimTime,
   //                        const WsfStringId& aLabelId);

   // Implementing GoToWaypoint() will enable FollowRoute_5 & FollowRoute_6

   // Fly to a waypoint on the planned route specified by the waypoint index
   bool GoToWaypoint(double, // aSimTime,
                     unsigned int aDestination) override;

   // Update the planned route with a new route
   bool SetDefaultRoute(const WsfRoute& aRoute);

   // Update the temporary route with a new route
   bool SetTempRoute(const WsfRoute& aRoute);

   // Replace the temporary route with a new route, and move the platform to the first waypoint
   bool SetRoute(double aSimTime, const WsfRoute& aRoute) override;

   const WsfRoute* GetRoute() const override { return mWsfRoutePtr; }

   const WsfRoute* GetDefaultRoute() const override { return mDefaultWsfRoutePtr; }

   int RoutePointIndex() const override;

   // ====================================================================================================================

   // These functions support guidance (WsfP6DOF_GuidanceComputer)

   //! Set the pointer to the mover guidance object. Ownership is retained by the caller.
   void SetGuidance(WsfMoverGuidance* aGuidancePtr) { mGuidancePtr = aGuidancePtr; }

   //! Get the pointer to the mover guidance object
   WsfMoverGuidance* GetGuidance() const { return mGuidancePtr; }

   // WsfGuidedMover and WsfGuidanceComputer use SetYandZForces(double aYForce,double aZForce), but WsfP6DOF_Mover uses
   // SetYandZGLoads(double aYGLoad, double aZGLoad) instead. This is because the P6DOF autopilot accepts g-load
   // commands for pitch and yaw. This function is used by WsfP6DOF_GuidanceComputer to command a P6DOF vehicle.
   void SetYandZGLoads(double aYGLoad, double aZGLoad);

   // This sets a yaw rate (rad/sec) command for the P6DOF autopilot for use in guidance
   void SetCommandedYawRate(double aRate) { mCommandedYawRate = aRate; }

   // This sets a pitch rate (rad/sec) command for the P6DOF autopilot for use in guidance
   void SetCommandedPitchRate(double aRate) { mCommandedPitchRate = aRate; }

   // This sets a roll rate (rad/sec) command for the P6DOF autopilot for use in guidance
   void SetCommandedRollRate(double aRate) { mCommandedRollRate = aRate; }

   // This sets a speed (m/sec) command for the P6DOF autopilot for use in guidance
   void SetCommandedSpeed(double aCommandedSpeed) { mCommandedSpeed = aCommandedSpeed; }

   // void SetCommandedEngineStopTime(double aStopTime) { mCommandedEngineStopTime = aStopTime; }

   // This sets the minimum height above terrain (in meters) for terrain following
   void SetMinimumHeightAboveTerrain(double aMinimum) { mMinimumHeightAboveTerrain = aMinimum; }

   // This returns the current stage index
   int CurrentStage() const { return (int)mStageIndex; }

   // This sets the current target when using guidance
   void SetTargetData(P6DOF::CurrentTargetData& aCurrentTarget) { mCurrentTarget = aCurrentTarget; }

   //! @name Callback objects for route changes and stage transitions.
   //! Another object can 'connect' to these objects to receive notification when a stage transition occurs.
   //! The first argument  in the callback is the current simulation time.
   //! The second argument is the actual time of the event. This may less than the current simulation time.
   //@{
   UtCallbackListN<void(double)>         RouteChanged;
   UtCallbackListN<void(double, double)> StageIgnition;
   UtCallbackListN<void(double, double)> StageBurnout;
   UtCallbackListN<void(double, double)> StageSeparation;
   //@}

   // This outputs the current kinematic status. It is Used by WsfP6DOF_GuidanceComputer.
   // This is deprecated.
   void WriteKinematicStatus(ut::log::MessageStream& aStream);

   // ====================================================================================================================

   // Functions to support scripting

   const char* GetScriptClassName() const override { return "WsfP6DOF_Mover"; }

   // This allows an external force (such as a ground tug) to apply a force to the nose gear. The angle is measured as
   // zero being forward and positive angles being to the right.
   void ApplyExternalForce(double aForceMagnitude_lbs, double aAngle_deg);

   // This removes any external forces on the nose gear.
   void RemoveExternalForce();

   // This should be called to enable direct braking commands in script.
   void EnableDirectBraking();

   // This sets the parking brake, if equipped.
   void SetParkingBrake();

   // This applies the left main gear wheel brake with a normalized value or 0 for no braking and 1.0 for full braking.
   void ApplyLeftGearBrake(double aValue);

   // This applies the right main gear wheel brake with a normalized value or 0 for no braking and 1.0 for full braking.
   void ApplyRightGearBrake(double aValue);

   // This releases both wheel brakes.
   void ReleaseWheelBrakes();

   // This should be called to disable any further direct braking commands in script.
   void ReleaseDirectBraking();

   // This starts up the engines. It is a simple, instant-on startup.
   void StartupEngines(double aTimeSec);

   // This shuts down up the engines. It is a simple, instant shutdown.
   void ShutdownEngines(double aTimeSec);

   // Returns mass in lbs
   double GetTotalWeight_lbs() const;

   // Returns mass in kg
   double GetTotalWeight_kg() const;

   // This retracts the landing gear
   void RetractLandingGear();

   // This lowers the landing gear
   void LowerLandingGear();

   // Set the flaps position in normalized values ranging from 0 when flaps are retracted to 1 when flaps are fully
   // deployed.
   void SetFlapsPosition(double aPosition);

   // Set the spoilers position in normalized values ranging from 0 when spoilers are retracted to 1 when spoilers are
   // fully deployed.
   void SetSpoilersPosition(double aPosition);

   void SetThrustReverserPosition(double aPosition);
   void SetEnableNWS(bool aNWS_Enabled);

   // Add fuel (in kg) to vehicle
   void AddFuel(double aFuelMass_kg);

   // Returns total fuel capacity (internal and external) in kg
   double GetTotalFuelCapacity() const;

   // Returns total internal fuel capacity in kg
   double GetInternalFuelCapacity() const;

   // Returns total external fuel capacity in kg
   double GetExternalFuelCapacity() const;

   // Returns total fuel remaining (internal and external) in kg
   double GetTotalFuelRemaining() const;

   // Returns internal fuel remaining in kg
   double GetInternalFuelRemaining() const;

   // Returns external fuel remaining in kg
   double GetExternalFuelRemaining() const;

   // Returns fuel tank capacity in kg
   double GetFuelTankCapacity(const std::string& aTankName) const;

   // This returns the current amount of fuel in the specified tank in kg
   double GetFuelInTank(const std::string& aTankName) const;

   // This will set the fuel in a specified tank to the specified level (kg), as long as the tank capacity can contain
   // it -- otherwise, the tank will be filled and the excess fuel will be "wasted".
   void SetFuelInTank(const std::string& aTankName, double aFuel_kg);

   // This returns the total fuel flow (kg/sec) for all engines on the vehicle
   double GetTotalFuelFlow() const;

   // This will instantly empty all internal and external fuel tanks
   void EmptyAllInternalandExternalTanks();

   // This will instantly empty all internal fuel tanks
   void EmptyAllInternalTanks();

   // This will instantly empty all external fuel tanks
   void EmptyAllExternalTanks();

   // This sets the Joker fuel state (kg)
   void SetJokerFuelState(double aJokerFuel_kg);

   // This sets the Bingo fuel state (kg)
   void SetBingoFuelState(double aBingoFuel_kg);

   // This returns the Joker fuel state (kg)
   double GetJokerFuelState();

   // This returns the Bingo fuel state (kg)
   double GetBingoFuelState();

   // This returns true if Joker fuel state has been reached
   bool JokerFuelReached();

   // This returns true if Bingo fuel state has been reached
   bool BingoFuelReached();

   // This enables/disables controls. When controls are disabled,
   // all control inputs are set to zero.
   void EnableControls(bool aEnabled);

   // This returns the controls enabled/disabled state.
   bool ControlsEnabled() const;

   // Returns true if there is weight on any wheel
   bool GetWeightOnWheels() const;

   // Returns true if there is weight on the nose wheel
   bool GetWeightOnNoseWheel() const;

   // This returns true if at least one engine (thrust producer) is producing thrust
   bool IsProducingThrust() const;

   // This returns thrust magnitude in Newtons
   double GetTotalThrust() const;

   // This returns forward thrust in Newtons
   double GetTotalForwardThrust() const;

   // This returns true if any afterburner is on
   bool GetAfterburnerOn() const;

   // This returns thrust in Newtons for the specified engine
   double GetEngineThrust(const std::string& aEngineName) const;

   // This returns the fuel flow (kg/sec) for the specified engine
   double GetEngineFuelFlowRate(const std::string& aEngineName) const;

   // This returns true if the afterburner is active for the specified engine
   bool GetEngineAfterburnerOn(const std::string& aEngineName) const;

   // This returns the total fuel flow rate in kg/sec
   double GetTotalFuelFlowRate() const;

   // This adds a new fuel transfer to the propulsion system. The fuel transfer name must be unique.
   // It returns true if the transfer is created.
   bool AddFuelTransfer(std::string aFuelTransferName, std::string aSourceTankName, std::string aTargetTankName);

   // This removes the fuel transfer with the specified name. It returns true if the removal is successful.
   bool RemoveFuelTransfer(std::string aFuelTransferName);

   // This specifies the fuel feed (input tank name) for the specified engine.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string aEngineName, std::string aFuelTankName);

   // This specifies the fuel feed (input tank name) for all engines.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string aFuelTankName);

   // EnableThrustVectoring enables/disable thrust vectoring
   void EnableThrustVectoring(bool aEnableThrustVectoring);

   // Limits and settings functions

   // This reverts all autopilot current limit settings to the default limits
   void RevertLimitsToDefaults();

   // This returns the minimum pitch g-load for the autopilot
   double GetPitchGLoadMin();

   // This returns the maximum pitch g-load for the autopilot
   double GetPitchGLoadMax();

   // This returns the minimum angle of attack (alpha) in degrees
   double GetAlphaMin();

   // This returns the maximum angle of attack (alpha) in degrees
   double GetAlphaMax();

   // This returns the minimum pitch rate in deg/sec
   double GetPitchRateMin();

   // This returns the maximum pitch rate in deg/sec
   double GetPitchRateMax();

   // This returns the minimum vertical speed in ft/min
   double GetVerticalSpeedMin();

   // This returns the maximum vertical speed in ft/min
   double GetVerticalSpeedMax();

   // This returns the maximum yaw g-load
   double GetYawGLoadMax();

   // This returns the maximum angle of sideslip (beta) in degrees
   double GetBetaMax();

   // This returns the maximum yaw rate in deg/sec
   double GetYawRateMax();

   // This returns the maximum roll rate in deg/sec
   double GetRollRateMax();

   // This returns the maximum bank angle in degrees
   double GetBankAngleMax();

   // This returns the minimum forward acceleration in gees
   double GetForwardAccelMin();

   // This returns the maximum forward acceleration in gees
   double GetForwardAccelMax();

   // This returns the maximum ground taxi speed in ft/sec
   double GetTaxiSpeedMax();

   // This returns the maximum ground taxi yaw rate in deg/sec
   double GetTaxiYawRateMax();

   // This sets the minimum pitch g-load for the autopilot
   void SetPitchGLoadMin(double aGees);

   // This sets the maximum pitch g-load for the autopilot
   void SetPitchGLoadMax(double aGees);

   // This sets the minimum angle of attack (alpha) in degrees
   void SetAlphaMin(double aAlpha);

   // This sets the maximum angle of attack (alpha) in degrees
   void SetAlphaMax(double aAlpha);

   // This sets the minimum pitch rate in deg/sec
   void SetPitchRateMin(double aPitchRate);

   // This sets the maximum pitch rate in deg/sec
   void SetPitchRateMax(double aPitchRate);

   // This sets the minimum vertical speed in ft/min
   void SetVerticalSpeedMin(double aVertSpeed);

   // This sets the maximum vertical speed in ft/min
   void SetVerticalSpeedMax(double aVertSpeed);

   // This sets the maximum yaw g-load
   void SetYawGLoadMax(double aYawGLoad);

   // This sets the maximum angle of sideslip (beta) in degrees
   void SetBetaMax(double aBeta);

   // This sets the maximum yaw rate in deg/sec
   void SetYawRateMax(double aYawRate);

   // This sets the maximum roll rate in deg/sec
   void SetRollRateMax(double aRollRate);

   // This sets the maximum bank angle in degrees
   void SetBankAngleMax(double aBankAngle);

   // This sets the minimum forward acceleration in gees
   void SetForwardAccelMin(double aForwardAccel);

   // This sets the maximum forward acceleration in gees
   void SetForwardAccelMax(double aForwardAccel);

   // This sets the maximum ground taxi speed in ft/sec
   void SetTaxiSpeedMax(double aSpeedMax);

   // This sets the maximum ground taxi yaw rate in deg/sec
   void SetTaxiYawRateMax(double aSpeedMax);

   // This sets the turn roll-in multiplier for the autopilot when following routes
   void SetTurnRollInMultiplier(double aValue);

   // Sets the route allowable error in radians
   void SetRouteAllowableAngleError(double aValue_rad);

   // This returns the current turn roll-in multiplier for the autopilot when following routes
   double GetCurrentTurnRollInMultiplier();

   // This returns the default turn roll-in multiplier for the autopilot when following routes
   double GetDefaultTurnRollInMultiplier();

   // Gets the current route allowable error in radians
   double GetCurrentRouteAllowableAngleError();

   // Gets the default route allowable error in radians
   double GetDefaultRouteAllowableAngleError();

   // Get the maximum bank angle is deg.
   double GetBankAngleMax() const;

   // Autopilot commands

   // This enable/disables taxi mode
   void SetTaxiMode(bool aTaxiModeEnabled);

   // This sets the turn radius (in feet) used for taxiing
   void SetTaxiRadius(double aTaxiRadius_ft);

   // This sets the autopilot to use pitch angle mode with the specified pitch angle (in degrees)
   void SetAutopilotPitchAngle(double aPitchAngle_deg);

   // This sets the autopilot to use flight path angle mode with the specified flight path angle (in degrees)
   void SetAutopilotFlightPathAngle(double aFlightPathAngle_deg);

   // This sets the autopilot to use vertical speed mode with the specified vertical speed (in ft/min)
   void SetAutopilotVerticalSpeed(double aVerticalSpeed_fpm);

   // This sets the autopilot to use altitude mode with the specified altitude (in feet)
   void SetAutopilotAltitude(double aAltitude_ft);

   // This sets the autopilot to use roll/bank angle mode with the specified roll/bank angle (in degrees)
   void SetAutopilotRollAngle(double aRollAngle_deg);

   // This sets the autopilot to use roll rate mode with the specified roll rate (in deg/sec)
   void SetAutopilotRollRate(double aRollRate_dps);

   // This sets the autopilot to use pitch g-load mode with the specified g-load
   void SetPitchGLoad(double aGLoad);

   // This sets the autopilot to use pitch rate mode with the specified pitch rate (in deg/sec)
   void SetAutopilotPitchRate(double aPitchRate_dps);

   // This sets the autopilot to use delta pitch mode with the specified delta pitch (in degrees)
   void SetAutopilotDeltaPitch(double DeltaPitchAngle_deg);

   // This sets the autopilot to use delta roll mode with the specified delta roll (in degrees)
   void SetAutopilotDeltaRoll(double DeltaRollAngle_deg);

   // This sets the autopilot to hold maintain the specified speed (in KTAS, knots true air speed)
   void SetAutopilotSpeedKTAS(double aSpeed_KTAS);

   // This sets the autopilot to hold maintain the specified speed (in KCAS, knots calibrated air speed)
   void SetAutopilotSpeedKCAS(double aSpeed_KCAS);

   // This sets the autopilot to hold maintain the specified speed (in Mach)
   void SetAutopilotSpeedMach(double aSpeed_Mach);

   // This sets the autopilot to use waypoint mode in the lateral channel
   void SetAutopilotLateralWaypointMode();

   // This sets the autopilot to use waypoint mode in the vertical channel
   void SetAutopilotVerticalWaypointMode();

   // This sets the autopilot to use waypoint mode in the speed channel
   void SetAutopilotSpeedWaypointMode();

   // This sets the autopilot to use waypoint mode
   void SetAutopilotWaypointMode();

   // This sets the autopilot to hold all controls in a centered/neutralized position
   void SetAutopilotNoControl();

   // This allows the use of direct throttle input rather than through the autopilot
   void EnableDirectThrottleInput();

   // This allows direct throttle input, setting the throttle to idle
   void MoveThrottleToIdle();

   // This allows direct throttle input, setting the throttle to full (military) power
   void MoveThrottleToFull();

   // This allows direct throttle input, setting the throttle to full afterburner
   void MoveThrottleToAfterburner();

   // This restores normal throttle input control to the autopilot
   void ReleaseDirectThrottleInput();

   // This allows the use of direct speed brake input rather than through the autopilot
   void EnableDirectSpeedBrakeInput();

   // This allows direct speed brake input, setting the speed brake to fully open/extended
   void OpenSpeedBrake();

   // This allows direct speed brake input, setting the speed brake to fully close/retracted
   void CloseSpeedBrake();

   // This restores normal speed brake input control to the autopilot
   void ReleaseDirectSpeedBrakeInput();

   // Returns the current stick back control input position
   double GetStickBackPosition() const;

   // Returns the current stick right control input position
   double GetStickRightPosition() const;

   // This returns the current rudder right control input position
   double GetRudderRightPosition() const;

   // This returns the current military (non-afterburner) throttle control input position
   double GetThrottleMilitaryPosition() const;

   // This returns the current afterburner throttle control input position
   double GetThrottleAfterburnerPosition() const;

   // This returns the current thrust reverser control input position
   double GetThrustReverserControlPosition() const;

   // This returns the current speed brake control input position
   double GetSpeedBrakeControlPosition() const;

   // This activates the sequencer with the specified name. If no such sequencer exists, it does nothing.
   bool ActivateSequencer(const std::string& aSequencerName);

   // This returns a pointer to the test object. It should only be used in testing.
   WsfP6DOF_MoverTestObject* GetTestSupportObject() const { return mTestSupportObjectPtr.get(); }

   // Damage and Destruction

   // This sets the damage factor for the mover
   void SetDamageFactor(double aDamageFactor);

   // This destroys the mover
   void SetDestroyed();

   // This returns true if the mover is destroyed
   bool GetIsDestroyed();

   // Appearance

   // This returns true if one or more engines are operating
   bool GetEngineIsOperating() const;

   // This returns true if one or more engines are producing visible smoke
   bool GetEngineIsSmoking() const;

   // This returns true if one or more engines have an operating afterburner
   bool GetEngineAfterburnerIsOn() const;

   // This allows control to have the engines produce visible smoke
   void MakeEnginesSmoke(bool aEngineSmoking);

   // This returns true if one or more engines are producing contrails
   bool GetContrailTrailingEffect() const;

   // This returns true if the mover is producing visible rocket smoke
   bool GetRocketSmokeTrailingEffect() const;

   // This returns true if the mover is producing visible damage smoke
   bool GetDamageSmokeTrailingEffect() const;

   // This allows control to have the mover produce visible damage smoke
   void SetDamageSmokeTrailingEffect(bool aEffectActive);

   // This returns true if the mover is producing a visible rising smoke plume
   bool GetRisingSmokePlumeIsPresent() const;

   // This allows control to have the mover produce a visible rising smoke plume
   void SetRisingSmokePlumeIsPresent(bool aSmoking);

   // This returns true if the mover is producing visible launch smoke
   bool GetLaunchFlashSmokeIsPresent() const;

   // This allows control to have the mover produce a visible launch smoke
   void SetLaunchFlashSmokeIsPresent(bool aLaunchFlashSmoke);

   // This returns true if the mover is producing visible flames
   bool GetFlamesArePresent() const;

   // This allows control to have the mover produce a visible flames
   void SetFlamesArePresent(bool aFlaming);

   // This returns true if the mover is lightly damaged
   bool GetIsLightlyDamaged() const;

   // This allows control to have the mover be lightly damaged
   void SetIsLightlyDamaged();

   // This returns true if the mover is heavily damaged
   bool GetIsHeavilyDamaged() const;

   // This allows control to have the mover be heavily damaged
   void SetIsHeavilyDamaged();

   // This is used to prevent crashes when passing through the ground/sea surface or the artificial "floor" of sea
   // level. It is usually set to true when testing.
   void SetTestingIgnoreAllCrashes(bool aValue);

   // This returns true when crashes with the ground will be ignored
   bool GetTestingIgnoreAllCrashes() const;

   // This is used to prevent the vehicle from going below sea level at any time. The effectively limits the altitude to
   // zero. It is sometimes set to true when testing.
   void SetTestingClampToSealLevelMinAlt(bool aValue);

   // This returns true when the vehicle will be prevented from going below sea level at any time.
   // The altitude will effectively be limited to zero.
   bool GetTestingClampToSealLevelMinAlt() const;

   // Maneuver related

   // This returns the current maneuver that is being executed, or nullptr if there is none.
   WsfManeuver* GetCurrentManeuver() const;

   // This returns the sequence that is currently being executed, or nullptr if there is none.
   WsfManeuverSequence* GetManeuverSequence() const;

   // This executes the given maneuver. This will replace any current sequence with the single manuever provided.
   void ExecuteManeuver(double aSimTime, std::unique_ptr<WsfManeuver>&& aManeuverPtr);

   // This executes the given sequence of maneuvers. This will replace any current sequence with the given sequence.
   void ExecuteManeuverSequence(double aSimTime, std::unique_ptr<WsfManeuverSequence>&& aSequencePtr);

   // This will cancel the currently executing maneuver sequence.
   void CancelManeuvers();

   // Formation related

   void GetInitialState(double aLocWCS[3], double aVelWCS[3], double aOrientationNED[3]) const;
   void SetInitialLatLon(double aLat, double aLon);
   void SetInitialAlt(double aAlt_m);
   void SetInitialNEDHeading(double aHeading_rad);
   void SetInitialNEDPitch(double aPitch_rad);
   void SetInitialNEDRoll(double aRoll_rad);
   void SetInitialNEDVelocity(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps);

   void ResetPosition(double aSimTime) override;

   // Subobject related

   // This will jettison the specified subobject, returning a pointer to the platform if successful, or null if it
   // fails. Note that this function does not add the platform to the simulation -- this must be performed outside of
   // this function.
   WsfPlatform* DirectJettisonSubobjectPlatform(P6DofVehicle* aJettisonedObject, double aSimTime_sec);

   // ====================================================================================================================

protected:
   UtVec3d mVectorToTarget_NED;
   double  mMaxGees;
   double  mSpeed;
   UtVec3d mManeuverPlane;

   void SetP6DofVehicle(std::unique_ptr<P6DofVehicle> aP6DofVehicle);

   bool ExecuteTriggersCB(unsigned int aPointIndex);
   bool ExecuteTriggers(double aSimTime, unsigned int aPointIndex) override;

   void CrashIntoGroundCB(double aSimTime);

   void UpdatePlatformFromP6DofVehicle(P6DofVehicle* aP6DofVehicle, UtEntity* aPlatform);

   void CheckForNewlyJettisonedPlatforms(double aSimTime);

   // This is a callback to handle a jettison
   void HandleSubobjectJettison(std::unique_ptr<P6DofVehicle>& aJettisonedObject);

   // This is used to perform common tasks required when a subobject is jettisoned. If it succeeds, it returns a
   // pointer to a newly created platform. If it fails, it returns null.
   WsfPlatform* CommonSubobjectJettisonTasks(std::unique_ptr<P6DofVehicle> aJettisonedObject);

   P6DofRoute* ConvertWsfRouteToP6DofRoute(const WsfRoute* aWsfRoute);

   void UpdateAppearance(double aSimTime);

   void InitializeSubobject(double              aSimTime,
                            std::string&        aRootName,
                            P6DofVehicle*       aObject,
                            const UtQuaternion& aRotation,
                            const UtVec3d&      aOffset);

   std::string LateralModeToString(P6DOF::Lateral::Mode aMode) const;
   std::string VerticalModeToString(P6DOF::Vertical::Mode aMode) const;
   std::string SpeedModeToString(P6DOF::Speed::Mode aMode) const;

   WsfDraw*                       mDrawPtr;
   ut::CloneablePtr<P6DofVehicle> mP6DofVehiclePtr;
   const P6DofVehicleType*        mP6DofVehicleTypePtr;
   P6DofPilotManager*             mPilotManagerPtr;
   P6DofVehicle*                  mP6DofVehicleCopyPtr;
   bool                           mUseWashInInitConditions;
   bool                           mFollowVerticalTrack;

   std::queue<WsfPlatform*> mNewlyJettisonedPlatforms;

   double mInitialPosition_Latitude;
   double mInitialPosition_Longitude;
   double mInitialAltitude_m;
   double mInitialNEDHeading;
   double mInitialNEDPitch;
   double mInitialNEDRoll;
   double mTargetPosition_Latitude;
   double mTargetPosition_Longitude;
   double mInitialSpeed_NED_X_fps;
   double mInitialSpeed_NED_Y_fps;
   double mInitialSpeed_NED_Z_fps;
   bool   mValidInitialPosition;
   bool   mValidInitialAltitude_ft;
   bool   mValidInitialNEDHeading;
   bool   mValidInitialNEDPitch;
   bool   mValidInitialNEDRoll;
   bool   mValidInitialSpeed_NED;
   double mLastStickX;
   double mLastStickY;
   double mLastRudder;
   double mLastThrottle;
   double mLastSpdBrakeLeverPosition;
   double mLastSpoilerLeverPosition;
   double mLastFlapsLeverPosition;
   double mLastLandingGearLeverPosition;
   double mLastThrustReverserLeverPosition;
   double mLastThrustVectorYawRightPosition;
   double mLastThrustVectorPitchUpPosition;
   double mLastThrustVectorRollRightPosition;
   double mLastNoseWheelSteeringRightPosition;
   double mLastWheelBrakeLeftPosition;
   double mLastWheelBrakeRightPosition;
   bool   mLastNWS_Enabled;

   // The WsfRoute route the basis of P6DofRoute. This is kept within WsfP6DOF_Mover to avoid storing it within the
   // "framework agnostic" core of P6DOF.
   WsfRoute* mWsfRoutePtr;
   WsfRoute* mDefaultWsfRoutePtr;

   // These functions are for guidance
   WsfMoverGuidance* mGuidancePtr;
   double            mYGLoad;
   double            mZGLoad;
   double            mCommandedYawRate;
   double            mCommandedPitchRate;
   double            mCommandedRollRate;
   double            mCommandedSpeed;

   //! The absolute simulation time when the engines are commanded explicitly commanded to stop. It can be set either
   //! from the launch computer or via explicit script command. If not defined, then the engines will burn until they
   //! run out of fuel.
   double mCommandedEngineStopTime;

   //! The minimum height above terrain required for terrain following.
   double mMinimumHeightAboveTerrain;

   unsigned int mStageIndex; //!< The index of the current stage

   //! Information captured at time of 'first missile motion' (needed by trajectory output)
   //@{
   double mFMM_Time;
   double mFMM_LocWCS[3];
   double mFMM_Heading;
   //@}

   P6DOF::CurrentTargetData mCurrentTarget;

   bool   mEnginesOffAtStart; // Allows engines to be "off" at startup
   bool   mLandingGearDownAtStart;
   bool   mParkingBrakeOnAtStart;
   bool   mThrottleIdleAtStart;
   bool   mThrottleFullAtStart;
   bool   mThrottleAfterburnerAtStart;
   bool   mTaxiModeEnabledAtStart;
   bool   mNWSEnabledAtStart;
   bool   mAutopilotNoControlAtStart;
   bool   mEnableThrustVectoringAtStart;
   bool   mEnableControlsAtStart;
   bool   mProducesLaunchSmoke;
   double mLaunchSmokeTime_sec;
   bool   mRisingSmokePlume;
   bool   mLaunchFlashSmoke;
   bool   mLightDamage;
   bool   mHeavyDamage;
   bool   mDestroyed;
   double mDamageFactor;
   double mJokerFuel_lbs;
   double mBingoFuel_lbs;

   // Event Pipe Manager for P6dof Data.  Maps a message name to an update rate and number of updates.
   std::map<std::string, std::pair<double, int>>      mEventPipeManager;
   UtCallbackHolder                                   mCallbacks;
   std::map<std::string, std::vector<WsfVisualPart*>> mVisualPartManager;
   std::unique_ptr<WsfManeuverSequence>               mManeuverPtr;

   // TODO Parking Brake, Throttle Idle/Full/AB, Taxi Mode, NWS, AP No Control, Wash-In at Start

   std::unique_ptr<WsfP6DOF_MoverTestObject> mTestSupportObjectPtr;
};

#endif
