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

#ifndef WSFSIXDOFMOVER_HPP
#define WSFSIXDOFMOVER_HPP

#include "wsf_six_dof_export.h"

#include <list>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtQuaternion.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
#include "WsfMover.hpp"
class WsfMoverGuidance;
#include "WsfRoute.hpp"
class WsfVisualPart;
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_MassProperties.hpp"
#include "WsfSixDOF_Route.hpp"
#include "WsfSixDOF_Sequencer.hpp"
#include "WsfSixDOF_SizeFactor.hpp"
#include "WsfSixDOF_ThrustProducerObject.hpp"
#include "WsfSixDOF_Utils.hpp"
#include "WsfSixDOF_VehicleData.hpp"
#include "WsfWaypoint.hpp"
#include "maneuvers/WsfSixDOF_Maneuver.hpp"
#include "maneuvers/WsfSixDOF_ManeuverSequence.hpp"

namespace wsf
{
namespace six_dof
{
class AeroCoreObject;
class ForceAndMomentsObject;
class ForceAndRotationObject;
class Integrator;
class MoverTestObject;
class PilotManager;
class PilotObject;

//! This is a base class for detailed air movers. This umbrella can cover RB6 or PM6 models.
class WSF_SIX_DOF_EXPORT Mover : public WsfMover
{
   friend class TypeManager; // This relationship allows us to build subobject lists of varying fidelity levels

public:
   //! Constructor
   explicit Mover(const WsfScenario& aScenario);

   virtual ~Mover();

   // TODO -- At the present time, only the AIR domain is specified, but SixDOF can support AIR and SPACE domains.
   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   bool ProcessInput(UtInput& aInput) override;
   bool ProcessVehicleInput(UtInput& aInput);

   bool Initialize(double aSimTime) override;

   //// Virtual utilities

   //// WSF Things

   void UpdateEventPipeData(double aSimTime);

   // Sets the vehicle to the specified NED heading (in radians)
   void SetHeading(double aHeading) override;

   // Sets the vehicle to the specified NED pitch angle (in radians)
   void SetPitch(double aPitch) override;

   // Sets the vehicle to the specified NED roll angle (in radians)
   void SetRoll(double aRoll) override;

   // Sets the orientation of the vehicle to the specified values (in radians)
   void SetOrientationNED(double aHeading, double aPitch, double aRoll) override;

   // This provides a set of top-level state data for the SixDOF vehicle
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

   // Return the index of the planned route
   size_t GetPlannedRouteIndex() const;

   // Return the index of the temporary route
   size_t GetTempRouteIndex() const;

   //// State accessors

   // Returns the current altitude in meters
   double GetAlt_m() const { return GetAltitudeMSL_m(); }

   // Returns vertical speed in m/sec
   double GetVerticalSpeed() { return GetVertSpeed_fpm() * UtMath::cM_PER_FT / 60.0; }

   // Returns flight path angle in degrees
   double GetFlightPathAngle() { return GetFlightPathAngle_deg(); }

   // Returns the current g-load in the vehicle's z-axis. Positive g-load is in the negative z direction.
   double GetGLoad() { return -GetNz_g(); }

   // This returns the current heading in degrees
   double GetHeading() const { return GetHeading_deg(); }

   // This returns the current pitch in degrees
   double GetPitch() const { return GetPitch_deg(); }

   // This returns the current roll in degrees
   double GetRoll() const { return GetRoll_deg(); }

   // Returns yaw rate in rad/sec
   double GetYawRate() { return GetYawRate_rps(); }

   // Returns pitch rate in rad/sec
   double GetPitchRate() { return GetPitchRate_rps(); }

   // Returns roll rate in rad/sec
   double GetRollRate() { return GetRollRate_rps(); }

   // Returns the most recent lift in newtons
   double GetLift_nt() const { return GetLift_lbs() * UtMath::cNT_PER_LB; }

   // Returns the most recent drag in newtons
   double GetDrag_nt() const { return GetDrag_lbs() * UtMath::cNT_PER_LB; }

   // Returns the most recent side force in newtons
   double GetSideForce_nt() const { return GetSideForce_lbs() * UtMath::cNT_PER_LB; }

   // This returns the thrust magnitude in newtons
   double GetThrustMagnitude_nt() const { return GetThrustMagnitude_lbs() * UtMath::cNT_PER_LB; }

   // Returns the current speed in KIAS
   double GetSpeed_KIAS() const { return GetKIAS(); }

   // Returns the current speed in KCAS
   double GetSpeed_KCAS() const { return GetKCAS(); }

   // Returns the current speed in KTAS
   double GetSpeed_KTAS() const { return GetKTAS(); }

   // Returns the current speed in Mach
   double GetSpeed_Mach() const { return GetMach(); }

   // Returns the current mass in kg
   double GetCurrentWeight_kg() const { return GetCurrentWeight_lbs() * UtMath::cKG_PER_LB; }

   // Returns the empty mass in kg
   double GetEmptyWeight_kg() const { return GetEmptyWeight_lbs() * UtMath::cKG_PER_LB; }

   // Returns mass in lbs
   double GetTotalWeight_lbs() const;

   // Returns mass in kg
   double GetTotalWeight_kg() const;

   // Returns true if there is weight on any wheel. Returns false by default.
   virtual bool GetWeightOnWheels() const { return false; }

   // This returns true if at least one engine (thrust producer) is producing thrust
   bool IsProducingThrust() const;

   // This returns thrust magnitude in Newtons
   double GetTotalThrust();

   // This returns forward thrust in Newtons
   double GetTotalForwardThrust();

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

   // EnableThrustVectoring enables/disable thrust vectoring
   void EnableThrustVectoring(bool aEnableThrustVectoring);


   //// Autopilot setters

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

   // This sets the autopilot to maintain the specified speed (in KTAS, knots true air speed)
   void SetAutopilotSpeedKTAS(double aSpeed_KTAS);

   // This sets the autopilot to maintain the specified speed (in KCAS, knots calibrated air speed)
   void SetAutopilotSpeedKCAS(double aSpeed_KCAS);

   // This sets the autopilot to  maintain the specified speed (in Mach)
   void SetAutopilotSpeedMach(double aSpeed_Mach);

   // This sets the autopilot to hold the specified throttle position
   void SetAutopilotThrottle(double aThrottle);

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

   // This restores normal throttle input control to the autopilot
   void ReleaseDirectThrottleInput();

   // This allows direct throttle input, setting the throttle to idle
   void MoveThrottleToIdle();

   // This allows direct throttle input, setting the throttle to full (military) power
   void MoveThrottleToFull();

   // This allows direct throttle input, setting the throttle to full afterburner
   void MoveThrottleToAfterburner();

   // This allows the use of direct speed brake input rather than through the autopilot
   void EnableDirectSpeedBrakeInput();

   // This restores normal speed brake input control to the autopilot
   void ReleaseDirectSpeedBrakeInput();

   // This allows direct speed brake input, setting the speed brake to fully open/extended
   void OpenSpeedBrake();

   // This allows direct speed brake input, setting the speed brake to fully close/retracted
   void CloseSpeedBrake();

   // Set the flaps position in normalized values ranging from 0 when flaps are retracted
   // to 1 when flaps are full deployed.
   virtual void SetFlapsPosition(double aPosition) = 0;

   // Set the spoilers position in normalized values ranging from 0 when spoilers are retracted
   // to 1 when spoilers are full deployed.
   virtual void SetSpoilersPosition(double aPosition) = 0;

   //// Virtual autopilot limits mutators

   // This returns the calculated maximum pitching acceleration in gees
   double GetPotentialManeuverGLoadMax() { return MaxPotentialManeuverGLoad(); };

   //// Direct Control Input -- These functions allow direct control input from AFSIM script

   // This should be called to take direct control of the SixDOF object before using SetDirectStickAndRudderInput,
   // SetDirectThrottleInput, and SetDirectControlInputs.
   void TakeDirectControlInput();

   // This should be called to release direct control of the SixDOF object, which will disable the use of
   // SetDirectStickAndRudderInput, SetDirectThrottleInput, and SetDirectControlInputs.
   void ReleaseDirectControlInput();

   // The aStickX value assumes that 1 is full right stick, 0 is centered, and -1 is full left stick.
   // The aStickY value assumes that 1 is full back stick, 0 is centered, and -1 is full forward stick.
   // The aRudder value assumes that 1 is full right rudder, 0 is centered, and -1 is full left rudder.
   // The aThrottleLeverPosition value assumes that 0 is idle, 1 is full power without augmentation/afterburner
   // (military power), and 2 is full power with full augmentation/afterburner (full afterburner).
   // This function requires that TakeManualControl be called prior to use.
   void SetDirectControlInputs(double aStickX, double aStickY, double aRudder, double aThrottleLeverPosition);

   // Activates direct control of the SixDOF roll command only
   void EnableDirectStickRightInput();

   // Activates direct control of the SixDOF pitch command only
   void EnableDirectStickBackInput();

   // Activates direct control of the SixDOF yaw command only
   void EnableDirectRudderRightInput();

   // Restores control of the roll command to the autopilot
   void ReleaseDirectStickRightInput();

   // Restores control of the pitch command to the autopilot
   void ReleaseDirectStickBackInput();

   // Restores control of the yaw command to the autopilot
   void ReleaseDirectRudderRightInput();

   // The aStickX value assumes that 1 is full right stick, 0 is centered, and -1 is full left stick.
   void SetDirectStickRightInput(double aStickX);

   // The aStickY value assumes that 1 is full back stick, 0 is centered, and -1 is full forward stick.
   void SetDirectStickBackInput(double aStickY);

   // The aRudder value assumes that 1 is full right rudder, 0 is centered, and -1 is full left rudder.
   void SetDirectRudderRightInput(double aRudderValue);

   // The aThrottleLeverPosition value assumes that 0 is idle, 1 is full power without augmentation/afterburner
   // (military power), and 2 is full power with full augmentation/afterburner (full afterburner).
   void SetDirectThrottleInput(double aThrottleLeverPosition);

   // This returns true if the positive-side stall warning is occurring
   bool StallWarningAlert() const;

   //// Route and Waypoint commands

   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;

   bool TurnToHeading(double                 aSimTime,
                      double                 aHeading_rad,
                      double                 aRadialAccel_mps2,
                      WsfPath::TurnDirection aTurnDirection) override;

   bool GoToSpeed(double aSimTime, double aSpeed_mps, double aLinearAccel_mps2, bool aKeepRoute) override;

   bool GoToAltitude(double aSimTime,
                     double aAltitude_m, // meters
                     double aClimbDiveRate_mps,
                     bool   aKeepRoute) override;

   bool TurnToRelativeHeading(double                 aSimTime,
                              double                 aHeadingChange_rad,
                              double                 aRadialAccel_mps2,
                              WsfPath::TurnDirection aTurnDirection) override;

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
   bool GoToWaypoint(double, // aSimTime,
                     unsigned int aDestination) override;

   // Update the planned route with a new route
   bool SetDefaultRoute(const WsfRoute& aRoute);

   // Update the temporary route with a new route
   bool SetTempRoute(const WsfRoute& aRoute);

   // Replace the temporary route with a new route, and move the platform to the first waypoint
   bool SetRoute(double aSimTime, const WsfRoute& aRoute) override;

   const WsfRoute* GetRoute() const override { return mWsfRoutePtr.get(); }

   const WsfRoute* GetDefaultRoute() const override { return mDefaultWsfRoutePtr.get(); }

   int RoutePointIndex() const override;

   bool FlyRates(const UtVec3d& aAngularRates,                // radians/sec
                 const UtVec3d& aLongitudinalAccel) override; // meters/sec^2

   bool FlyHeadingSpeedAltitude(double aHeading,            // deg
                                double aSpeed,              // meters/sec
                                double aAltitude,           // meters
                                double aMaxGees,            // Gs
                                double aMaxClimb) override; // meters/sec

   bool TurnInPlaneWithSpeed(const UtVec3d& aPlane,   // world coordinates (not body coordinates)
                             double         aGees,    // Gs
                             double         aSpeed) override; // meters/sec

   bool TurnInPlaneWithThrottle(const UtVec3d& aPlane,      // world coordinates (not body coordinates)
                                double         aGees,       // Gs
                                double         aThrottle) override; //[0-3]

   bool FlyVectorWithSpeed(const UtVec3d& aVelocityVector, // world coordinates (not body coordinates) (magnitude does
                                                           // not matter, just direction)
                           double aMaxGees,                // Gs
                           double aSpeed) override;        // meters/sec

   bool FlyVectorWithThrottle(const UtVec3d& aVelocityVector, // world coordinates (not body coordinates) (magnitude
                                                              // does not matter, just direction)
                              double aMaxGees,                // Gs
                              double aThrottle) override;     //[0-3]

   void CommandSlice(double aTargetHeading_deg, double aTargetAltitude_m, double aTargetMach = 0.0);

   void CommandLevelTurn(double aTargetHeading_deg, double aTargetMach, double aTargetGees = 0.0);

   //// CommonController/autopilot control commands

   // This allows enabling/disabling the autopilot
   void EnableAutopilot(bool aState);

   // This makes the manual pilot active (if present)
   void ActivateSimpleManualPilot();

   // This makes the manual pilot active (if present)
   void ActivateAugmentedManualPilot();

   // This makes the manual pilot active (if present)
   void ActivateSyntheticPilot();

   //// Autopilot limits and settings

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

   // This returns the CLmax value for the specified Mach
   virtual double GetCLMaxAtMach(double aMach) const;

   // This returns the alpha (in degrees) at CLmax for the specified Mach
   virtual double GetAlphaAtCLMaxAtMach_deg(double aMach) const;

   // This returns the alpha (in degrees) at the specified pitching load for the specified Mach
   virtual void CalculateAlphaAtSpecifiedGLoad_deg(double aMach, double& aAlpha_deg) const = 0;

   // This returns the beta (in degrees) at the specified yawing load for the specified Mach
   virtual void CalculateBetaAtSpecifiedGLoad_deg(double aMach, double& aBeta_deg) const = 0;

   // Returns the design weight for max-g maneuvering in lbs
   double GetDesignWeightForMaxG_lbs() const { return GetEmptyWeight_lbs(); }

   //// Virtual maneuver functions

   // This returns the current maneuver that is being executed, or nullptr if there is none.
   wsf::six_dof::Maneuver* GetCurrentManeuver() const;

   // This returns the sequence that is currently being executed, or nullptr if there is none.
   wsf::six_dof::ManeuverSequence* GetManeuverSequence() const;

   // This executes the given maneuver. This will replace any current sequence with the single manuever provided.
   void ExecuteManeuver(double aSimTime, std::unique_ptr<wsf::six_dof::Maneuver>&& aManeuverPtr);

   // This executes the given sequence of maneuvers. This will replace any current sequence with the given sequence.
   void ExecuteManeuverSequence(double aSimTime, std::unique_ptr<wsf::six_dof::ManeuverSequence>&& aSequencePtr);

   // This will cancel the currently executing maneuver sequence.
   void CancelManeuvers();

   //// Formation functions

   void GetInitialState(double aLocWCS[3], double aVelWCS[3], double aOrientationNED[3]) const;
   void SetInitialLatLon(double aLat, double aLon);
   void SetInitialAlt(double aAlt_m);
   void SetInitialNEDHeading(double aHeading_rad);
   void SetInitialNEDPitch(double aPitch_rad);
   void SetInitialNEDRoll(double aRoll_rad);
   void SetInitialNEDVelocity(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps);

   //// Non-virtual functions

   void Update(double aSimTime) override;
   void UpdateAppearance(double aSimTime);

   void UpdatePlatformFromVehicle(Mover* aVehicle, WsfPlatform* aPlatform);

   // bool Initialize(double aSimTime) override;

   // This returns the type of the active pilot object as a string
   std::string GetActivePilot() const;

   // This returns the autopilot's lateral mode as an enum
   Lateral::Mode GetAutopilotLateralModeEnum() const;

   // This returns the autopilot's lateral mode as a string
   std::string GetAutopilotLateralMode() const;
   std::string LateralModeToString(Lateral::Mode aMode) const;

   // This returns the value of the autopilot's lateral mode
   float GetAutopilotLateralModeValue() const;

   // This returns the autopilot's vertical mode as an enum
   Vertical::Mode GetAutopilotVerticalModeEnum() const;

   // This returns the autopilot's vertical mode as a string
   std::string GetAutopilotVerticalMode() const;
   std::string VerticalModeToString(Vertical::Mode aMode) const;

   // This returns the value of the autopilot's vertical mode
   float GetAutopilotVerticalModeValue() const;

   // This returns the autopilot's speed mode as an enum
   Speed::Mode GetAutopilotSpeedModeEnum() const;

   // This returns the autopilot's speed mode as a string
   std::string GetAutopilotSpeedMode() const;
   std::string SpeedModeToString(Speed::Mode aMode) const;

   // This returns the value of the autopilot's speed mode
   float GetAutopilotSpeedModeValue() const;

   // These functions return the normalized control surface angle, defaulting to 0.0
   virtual double GetNormalizedAileronLeft() const { return 0.0; }
   virtual double GetNormalizedAileronRight() const { return 0.0; }
   virtual double GetNormalizedFlapLeft() const { return 0.0; }
   virtual double GetNormalizedFlapRight() const { return 0.0; }
   virtual double GetNormalizedSpoilerLeft() const { return 0.0; }
   virtual double GetNormalizedSpoilerRight() const { return 0.0; }
   virtual double GetNormalizedStabilizerLeft() const { return 0.0; }
   virtual double GetNormalizedStabilizerRight() const { return 0.0; }
   virtual double GetNormalizedElevator() const { return 0.0; }
   virtual double GetNormalizedRudderLeft() const { return 0.0; }
   virtual double GetNormalizedRudder() const { return 0.0; }
   virtual double GetNormalizedRudderRight() const { return 0.0; }
   virtual double GetNormalizedSpeedBrake() const { return 0.0; }
   virtual double GetNormalizedLandingGear() const { return 0.0; }
   virtual double GetNormalizedLandingGearNose() const { return 0.0; }
   virtual double GetNormalizedLandingGearMainLeft() const { return 0.0; }
   virtual double GetNormalizedLandingGearMainRight() const { return 0.0; }

   // These functions return true if the data for that control surface is valid, defaulting false
   virtual bool AileronLeftValid() const { return false; }
   virtual bool AileronRightValid() const { return false; }
   virtual bool FlapLeftValid() const { return false; }
   virtual bool FlapRightValid() const { return false; }
   virtual bool SpoilerLeftValid() const { return false; }
   virtual bool SpoilerRightValid() const { return false; }
   virtual bool StabilizerLeftValid() const { return false; }
   virtual bool StabilizerRightValid() const { return false; }
   virtual bool ElevatorValid() const { return false; }
   virtual bool RudderLeftValid() const { return false; }
   virtual bool RudderValid() const { return false; }
   virtual bool RudderRightValid() const { return false; }
   virtual bool SpeedBrakeValid() const { return false; }
   virtual bool LandingGearValid() const { return false; }
   virtual bool LandingGearNoseValid() const { return false; }
   virtual bool LandingGearMainLeftValid() const { return false; }
   virtual bool LandingGearMainRightValid() const { return false; }
   virtual bool ThrottleValid() const = 0;

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

   // Returns the position (deg) of the speed brakes (common position)
   virtual double GetSpeedBrakePosition() const = 0;

   // Returns the position of the lending gear lever
   double GetLandingGearLeverPosition() const;

   // Returns the position of the flaps lever
   double GetFlapsLeverPosition() const;

   // Returns the position of the flaps
   virtual double GetFlapsPosition() const = 0;

   // Returns the position of the spoilers lever
   double GetSpoilersLeverPosition() const;

   // Returns the position (deg) of the spoilers (common position)
   virtual double GetSpoilersPosition() const = 0;

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

   // This is used to verify that a subobject's fuel load can be included as external fuel
   bool IsExternalFuelTank();

   // This will instantly empty all internal and external fuel tanks
   void EmptyAllInternalandExternalTanks();

   // This will instantly empty all internal fuel tanks
   void EmptyAllInternalTanks();

   // This will instantly empty all external fuel tanks
   void EmptyAllExternalTanks();

   // This starts up the engines. It is a simple, instant-on startup.
   void StartupEngines(double aSimTime_sec);

   // This shuts down up the engines. It is a simple, instant shutdown.
   void ShutdownEngines(double aSimTime_sec);

   // This sets the Joker fuel state (kg)
   void SetJokerFuelState(double aJokerFuel_kg) { mJokerFuel_lbs = aJokerFuel_kg * UtMath::cLB_PER_KG; }

   // This sets the Bingo fuel state (kg)
   void SetBingoFuelState(double aBingoFuel_kg) { mBingoFuel_lbs = aBingoFuel_kg * UtMath::cLB_PER_KG; }

   // This returns the Joker fuel state (kg)
   double GetJokerFuelState() { return mJokerFuel_lbs * UtMath::cKG_PER_LB; }

   // This returns the Bingo fuel state (kg)
   double GetBingoFuelState() { return mBingoFuel_lbs * UtMath::cKG_PER_LB; }

   // This returns true if Joker fuel state has been reached
   bool JokerFuelReached();

   // This returns true if Bingo fuel state has been reached
   bool BingoFuelReached();

   /** Check an input for frequency units (hz) and convert to seconds if so. */
   void CheckForFrequencyUnits(double& value, UtInput& aInput)
   {
      std::string units;
      aInput.ReadValue(value);
      aInput.ReadValue(units);
      UtStringUtil::ToLower(units);
      UtInput::ValueType valueType = UtInput::GetValueTypeOf(units);
      // If the user input a frequency instead of seconds, convert it for them
      if (valueType == UtInput::ValueType::cFREQUENCY)
      {
         // If our value us not 0, convert it to seconds.
         if (value != 0.0)
         {
            value = (1.0 / value);
         }
      }
   }

   // ====================================================================================================

   // These functions support guidance (GuidanceComputer)

   //! Set the pointer to the mover guidance object. Ownership is retained by the caller.
   void SetGuidance(WsfMoverGuidance* aGuidancePtr) { mGuidancePtr = aGuidancePtr; }

   //! Get the pointer to the mover guidance object
   WsfMoverGuidance* GetGuidance() const { return mGuidancePtr; }

   // WsfGuidedMover and WsfGuidanceComputer use SetYandZForces(double aYForce,double aZForce), but SixDOF Mover
   // uses SetYandZGLoads(double aYGLoad, double aZGLoad) instead. This is because the SixDOF autopilot accepts g-load
   // commands for pitch and yaw. This function is used by WsfSixDOF_GuidanceComputer to command a SixDOF vehicle.
   void SetYandZGLoads(double aYGLoad, double aZGLoad);

   // This sets a yaw rate (rad/sec) command for the SixDOF autopilot for use in guidance
   void SetCommandedYawRate(double aRate) { mCommandedYawRate = aRate; }

   // This sets a pitch rate (rad/sec) command for the SixDOF autopilot for use in guidance
   void SetCommandedPitchRate(double aRate) { mCommandedPitchRate = aRate; }

   // This sets a roll rate (rad/sec) command for the SixDOF autopilot for use in guidance
   void SetCommandedRollRate(double aRate) { mCommandedRollRate = aRate; }

   // This sets a speed (m/sec) command for the SixDOF autopilot for use in guidance
   void SetCommandedSpeed(double aCommandedSpeed) { mCommandedSpeed = aCommandedSpeed; }

   // void SetCommandedEngineStopTime(double aStopTime) { mCommandedEngineStopTime = aStopTime; }

   void WriteKinematicStatus(ut::log::MessageStream& aStream);

   // This sets the minimum height above terrain (in meters) for terrain following
   void SetMinimumHeightAboveTerrain(double aMinimum) { mMinimumHeightAboveTerrain = aMinimum; }

   // This returns the current stage index
   int CurrentStage() const { return (int)mStageIndex; }

   //// Damage and Destruction

   // This sets the damage factor for the mover
   void SetDamageFactor(double aDamageFactor);

   // OnPlatformBrokenEvent is called for all platform parts when a platform is "broken", which is typically the result
   // of combat damage or destruction. By default, most platform parts are "turned off" with this function, but
   // Mover overrides the function to prevent being turned off, since the mover may continue to move.
   void OnPlatformBrokenEvent(double aSimTime) override { SetDestroyed(); }

   void ResetPosition(double aSimTime) override;

   //// Appearance

   // This returns true if one or more engines are operating
   bool GetEngineIsOperating() const { return AnEngineIsOperating(); }

   // This returns true if one or more engines are producing visible smoke
   bool GetEngineIsSmoking() const { return AnEngineIsSmoking(); }

   // This returns true if one or more engines have an operating afterburner
   bool GetEngineAfterburnerIsOn() const { return AnEngineHasAfterburnerOn(); }

   // This allows control to have the engines produce visible smoke
   void MakeEnginesSmoke(bool aEngineSmoking) { MakeEnginesSmoke(); }

   // This returns true if one or more engines are producing contrails
   bool GetContrailTrailingEffect() const { return ContrailTrailingEffectPresent(); }

   // This returns true if the mover is producing visible rocket smoke
   bool GetRocketSmokeTrailingEffect() const { return RocketSmokeTrailingEffectPresent(); }

   // This returns true if the mover is producing visible damage smoke
   bool GetDamageSmokeTrailingEffect() const { return DamageSmokeTrailingEffectPresent(); }

   // This allows control to have the mover produce visible damage smoke
   void SetDamageSmokeTrailingEffect(bool aEffectActive) { ActivateDamageSmokeTrailingEffect(aEffectActive); }

   // This returns true if the mover is producing a visible rising smoke plume
   bool GetRisingSmokePlumeIsPresent() const { return mRisingSmokePlume; }

   // This allows control to have the mover produce a visible rising smoke plume
   void SetRisingSmokePlumeIsPresent(bool aSmoking) { mRisingSmokePlume = aSmoking; }

   // This returns true if the mover is producing visible launch smoke
   bool GetLaunchFlashSmokeIsPresent() const { return mLaunchFlashSmoke; }

   // This allows control to have the mover produce a visible launch smoke
   void SetLaunchFlashSmokeIsPresent(bool aLaunchFlashSmoke) { mLaunchFlashSmoke = aLaunchFlashSmoke; }

   // This returns true if the mover is producing visible flames
   bool GetFlamesArePresent() const { return FlamesArePresent(); }

   // This allows control to have the mover produce a visible flames
   void SetFlamesArePresent(bool aFlaming) { ActivateFlamesEffect(aFlaming); }

   // This returns true if the mover is lightly damaged
   bool GetIsLightlyDamaged() const { return mLightDamage; }

   // This allows control to have the mover be lightly damaged
   void SetIsLightlyDamaged() { mLightDamage = true; }

   // This returns true if the mover is heavily damaged
   bool GetIsHeavilyDamaged() const { return mHeavyDamage; }

   // This allows control to have the mover be heavily damaged
   void SetIsHeavilyDamaged() { mHeavyDamage = true; }

   //// Subobject related

   // This will jettison the specified subobject, returning a pointer to the platform if successful, or null if it
   // fails. Note that this function does not add the platform to the simulation -- this must be performed outside of
   // this function.
   WsfPlatform* DirectJettisonSubobjectPlatform(Mover* aJettisonedObject, double aSimTime_sec);

   //// SixDOF_Vehicle

   // ParentVehicle is the Vehicle this object is attached to (This may be different from Parent)
   Mover* GetParentVehicle() const { return mParentVehicle; }
   void   SetParentVehicle(Mover* aParentVehicle) { mParentVehicle = aParentVehicle; }

   // ====================================================================================================================

   // Initialization functions

   void DeriveFrom(const Mover& aSrc);

   bool Initialize(double aSimTime_sec, const std::string& aPrefixName);

   bool InitializeConditions(double aSimTime_sec);

   bool InitializeSequencers(double aSimTime_sec);
   bool InitializeSubobjects(double aSimTime_sec);

   // This "washes-in" (smooths) initial conditions, helping to eliminate "jumps"
   // as the vehicle reaches a stable state.
   void SetWashInInitConditions(bool aUseWashInInitConditions) { mUseWashInInitConditions = aUseWashInInitConditions; }

   // ====================================================================================================================

   // Update functions

   void         Update(int64_t aSimTime_nanosec);
   virtual void UpdateObject(int64_t aSimTime_nanosec);

   virtual void UpdateFlightControls(int64_t aSimTime_nanosec) = 0;

   // ====================================================================================================================
   // Misc object functions

   virtual const Integrator* GetIntegrator() const       = 0;
   virtual PilotManager*     GetPilotManager() const     = 0;
   virtual PropulsionSystem* GetPropulsionSystem() const = 0;

   virtual AeroCoreObject* GetAeroCoreObject() const = 0;

   // This returns the pilot controller object. Null is returned when
   // no pilot controller is present.
   PilotObject* GetActivePilotObject() const;

   virtual bool HasLandingGear() { return false; }

   // ====================================================================================================================
   // Misc object functions

   // This returns true if the object has crashed
   bool Crashed() const { return mCrashed; }

   // This enables/disables the size factor. Size factor effects aerodynamics as
   // a size multiplier for reference area.
   void SetSizeFactorEnabled(bool aSetting) { mSizeFactor.SetSizeFactorEnabled(aSetting); }

   // This is used to move to the preposition location (starting location) at the specified altitude in meters.
   // This is often used in testing.
   void SetPrepositionAlt_m(double aAltitude_m);

   // ====================================================================================================================

   // Kinematics state functions
   // This returns the kinematics state.
   KinematicState* GetKinematicState() { return &mKinematicState; }

   // ====================================================================================================================

   // Kinematics state functions
   // This returns the kinematics state.
   FreezeFlags* GetFreezeFlags() { return &mFreezeFlags; }

   // ====================================================================================================================

   // Type manager functions
   // This returns the scenario-common environment.
   const Environment* GetEnvironment() const { return mEnvironment; }

   // ====================================================================================================================

   //// Kinematic state accessors

   // This provides the latitude/longitude where the object was created
   void GetStartingLatLon(double& aStartingLat, double& aStartingLon) const
   {
      mKinematicState.GetStartingLatLon(aStartingLat, aStartingLon);
   }

   // This returns the x-component of acceleration (in gees) in body coordinates
   double GetNx_g() const { return mKinematicState.GetNx_g(); }

   // This returns the y-component of acceleration (in gees) in body coordinates
   double GetNy_g() const { return mKinematicState.GetNy_g(); }

   // This returns the z-component of acceleration (in gees) in body coordinates
   double GetNz_g() const { return mKinematicState.GetNz_g(); }

   // This returns the lift in lbs
   double GetLift_lbs() const { return mKinematicState.GetLift(); }

   // This returns the drag in lbs
   double GetDrag_lbs() const { return mKinematicState.GetDrag(); }

   // This returns the side force in lbs
   double GetSideForce_lbs() const { return mKinematicState.GetSideForce(); }

   // This returns the thrust magnitude in lbs
   double GetThrustMagnitude_lbs() const { return mKinematicState.GetThrust(); }

   // This returns a vector of the acceleration (in meters/sec^2) in NED coordinates
   UtVec3dX GetAccelNED_mps2() const;

   double GetVertSpeed_fpm() const { return mKinematicState.GetVerticalSpeed_fpm(); }

   // This returns the current gamma (flight path angle) in radians
   double GetFlightPathAngle_rad() const { return mKinematicState.GetFlightPathAngle_rad(); }
   // This returns the current gamma (flight path angle) in degrees
   double GetFlightPathAngle_deg() const { return mKinematicState.GetFlightPathAngle_deg(); }

   // This provides the current WCS yaw, pitch, and roll in radians
   void GetOrientationWCS_rad(double& aWcsYaw_rad, double& aWcsPitch_rad, double& aWcsRoll_rad) const;

   // This provides the current NED heading, pitch, and roll in radians
   void GetAttitudeNED_rad(double& aHeading, double& aPitch, double& aRoll);

   // This returns the current heading in radians
   double GetHeading_rad() const { return mKinematicState.GetLocalHeading_rad(); }
   // This returns the current heading in degrees
   double GetHeading_deg() const { return mKinematicState.GetLocalHeading_deg(); }

   // This returns the current pitch in degrees
   double GetPitch_rad() const { return mKinematicState.GetLocalPitch_rad(); }
   // This returns the current pitch in degrees
   double GetPitch_deg() const { return mKinematicState.GetLocalPitch_deg(); }

   // This returns the current roll in degrees
   double GetRoll_rad() const { return mKinematicState.GetLocalRoll_rad(); }
   // This returns the current roll in degrees
   double GetRoll_deg() const { return mKinematicState.GetLocalRoll_deg(); }

   // This returns the current altitude (in meters)
   double GetAltitudeMSL_m() const { return mKinematicState.GetAlt_m(); }
   // This returns the current altitude (in feet above sea level)
   double GetAltitudeMSL_ft() const { return mKinematicState.GetAlt_ft(); }

   // This returns the maximum observed altitude (in Km above sea level)
   double GetApogee_km() const { return mKinematicState.GetApogee_km(); }

   // This returns the current great-circle range (in kilometers) from the starting location to the current location
   double GetRangeSinceStart_km() const { return mKinematicState.GetRangeSinceStart_km(); }

   // This provides the current lat/lon (in degrees) and altitude (in meters)
   void GetLocationLLA(double& aLat, double& aLon, double& aAlt_m);

   // This provides x,y,z values for the current location in WCS
   void GetLocationWCS_m(double& aWcsPosX_m, double& aWcsPosY_m, double& aWcsPosZ_m) const;

   // This returns a vector for the current location in WCS
   UtVec3dX GetLocationWCS_m() const;

   // This returns the current latitude in degrees
   double GetLatitude_deg() const { return mKinematicState.GetLat(); }
   // This returns the current longitude in degrees
   double GetLongitude_deg() const { return mKinematicState.GetLon(); }

   // This provides the vx, vy, vz WCS velocity components in meters/sec
   void GetVelocityWCS_mps(double& aWcsVelX_mps, double& aWcsVelY_mps, double& aWcsVelZ_mps) const;

   // This returns a vector of the WCS velocity in meters/sec
   UtVec3dX GetVelocityWCS_mps() const;

   // This returns a vector of the NED velocity in meters/sec
   UtVec3dX GetVelocityNED_mps() const;

   // This returns the indicated airspeed in knots based on the current altitude
   double GetKIAS() const { return mKinematicState.GetSpeed_kias(); }
   // This returns the indicated airspeed in knots based on the current altitude
   double GetKCAS() const { return GetKIAS(); }
   // This returns the true airspeed in knots
   double GetKTAS() const { return mKinematicState.GetSpeed_ktas(); }
   // This returns the Mach, based on the current speed and altitude
   double GetMach() const { return mKinematicState.GetSpeed_Mach(); }
   // This returns the true speed (inertial, not airspeed) in ft/sec
   double GetSpeed_fps() const { return mKinematicState.GetSpeed_fps(); }
   // This returns the true speed (inertial, not airspeed) in m/sec
   double GetSpeed_mps() const { return mKinematicState.GetSpeed_mps(); }

   // This provides the current body rates about each axis in radians/sec
   void GetBodyRates_rps(double& xAxis_rps, double& yAxis_rps, double& zAxis_rps) const
   {
      mKinematicState.GetOmegaBody().Get(xAxis_rps, yAxis_rps, zAxis_rps);
   }

   // This returns the current yaw rate in radians/sec
   double GetYawRate_rps() const { return mKinematicState.GetYawRate_rps(); }
   // This returns the current yaw rate in degrees/sec
   double GetYawRate_dps() const { return mKinematicState.GetYawRate_dps(); }

   // This returns the current pitch rate in radians/sec
   double GetPitchRate_rps() const { return mKinematicState.GetPitchRate_rps(); }
   // This returns the current pitch rate in degrees/sec
   double GetPitchRate_dps() const { return mKinematicState.GetPitchRate_dps(); }

   // This returns the current roll rate in radians/sec
   double GetRollRate_rps() const { return mKinematicState.GetRollRate_rps(); }
   // This returns the current roll rate in degrees/sec
   double GetRollRate_dps() const { return mKinematicState.GetRollRate_dps(); }

   // This returns the current alpha (angle of attack) in radians
   double GetAlpha_rad() const { return mKinematicState.GetAlpha_rad(); }
   // This returns the current alpha (angle of attack) in degrees
   double GetAlpha_deg() const { return mKinematicState.GetAlpha_deg(); }

   // This returns the current angle of attack rate (alpha-dot) in radians/sec
   double GetAlphaDot_rps() const { return mKinematicState.GetAlphaDot_rps(); }
   // This returns the current angle of attack rate (alpha-dot) in degrees/sec
   double GetAlphaDot_dps() const { return mKinematicState.GetAlphaDot_dps(); }

   // This returns the current beta (angle of sideslip) in radians
   double GetBeta_rad() const { return mKinematicState.GetBeta_rad(); }
   // This returns the current beta (angle of sideslip) in degrees
   double GetBeta_deg() const { return mKinematicState.GetBeta_deg(); }

   // This returns the current angle of sideslip rate (beta-dot) in radians/sec
   double GetBetaDot_rps() const { return mKinematicState.GetBetaDot_rps(); }
   // This returns the current angle of sideslip rate (beta-dot) in degrees/sec
   double GetBetaDot_dps() const { return mKinematicState.GetBetaDot_dps(); }

   // This provides the current body angular acceleration about each axis in radians/sec^2
   void GetBodyAngularAccel_rps2(double& xAxis_rps2, double& yAxis_rps2, double& zAxis_rps2) const
   {
      mKinematicState.GetOmegaBodyDot().Get(xAxis_rps2, yAxis_rps2, zAxis_rps2);
   };

   // This returns dynamic pressure in lbs/sq-ft
   double GetDynamicPressure_psf() const { return mKinematicState.GetDynamicPressure_psf(); }

   // This returns pressure pressure in lbs/sq-ft
   double GetStaticPressure_psf() const { return mKinematicState.GetStaticPressure_psf(); }

   // This returns the last Nx_g. It is used in sequencers to detect if g-loads are increasing or decreasing.
   double GetLastNx_g() const { return mKinematicState.GetLastNx_g(); }

   // This returns the last Ny_g. It is used in sequencers to detect if g-loads are increasing or decreasing.
   double GetLastNy_g() const { return mKinematicState.GetLastNy_g(); }

   // This returns the last Nz_g. It is used in sequencers to detect if g-loads are increasing or decreasing.
   double GetLastNz_g() const { return mKinematicState.GetLastNz_g(); }

   // This returns the last dynamic pressure (Q). It is used in sequencers to detect if Q is increasing or decreasing.
   double GetLastDynamicPressure_psf() const { return mKinematicState.GetLastDynamicPressure_psf(); }

   // This returns the last static pressure (P). It is used in sequencers to detect if P is increasing or decreasing.
   double GetLastStaticPressure_psf() const { return mKinematicState.GetLastStaticPressure_psf(); }

   // This returns the last altitude in feet. It is used in sequencers to detect if the altitude is increasing or decreasing.
   double GetLastAltitudeMSL_ft() const { return mKinematicState.GetLastAltitudeMSL_ft(); }

   // This returns the last captive state (true means the object is captive to
   // its parent). It is used in sequencers to detect if the captive state
   // has changed.
   bool GetLastIsCaptive() const { return mLastIsCaptive; }

   //// Kinematic state mutators

   // This sets the location to the lat, lon, altitude specified
   void SetLocationLLA(double aLat, double aLon, double aAlt_m);

   // This This sets the location to the x, y, z specified in WCS
   void SetLocationWCS_m(double aWcsPosX_m, double aWcsPosY_m, double aWcsPosZ_m);

   // This sets the velocity using the vx, vy, vz WCS velocity components in meters/sec
   void SetVelocityWCS_mps(double aWcsVelX_mps, double aWcsVelY_mps, double aWcsVelZ_mps);

   // This sets the velocity using the NED velocity components in meters/sec
   void SetVelocityNED_mps(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps);

   // This sets the WCS yaw, pitch, and roll in radians
   void SetOrientationWCS_rad(double aWcsYaw_rad, double aWcsPitch_rad, double aWcsRoll_rad);

   // This set the NED heading, pitch, and roll in radians
   void SetAttitudeNED_rad(double aHeading_rad, double aPitch_rad, double aRoll_rad);

   // Zeros body rates, used for autopilot tuning
   void ZeroBodyRates();

   // This returns the current center of gravity in feet from the reference point
   virtual UtVec3dX GetCenterOfGravity_ft() const = 0;

   // This returns the mass properties.
   const MassProperties& GetMassProperties() const { return mMassProperties; }

   double GetEmptyWeight_lbs() const { return mMassProperties.GetBaseMass_lbs(); }
   double GetCurrentWeight_lbs() const { return mMassProperties.GetMass_lbs(); }

   virtual void CalculateCurrentMassProperties() = 0;

   virtual double MaxPotentialManeuverGLoad() const = 0;

   // ====================================================================================================================

   virtual bool   GetBooleanInput(size_t) const            = 0;
   virtual bool   GetLastBooleanInput(size_t) const        = 0;
   virtual size_t GetInputHandle(const std::string&) const = 0;

   // This sets the mLastSimTime_nanosec to the specified sim time in seconds. If the aIncludeSubobjects flag is true,
   // all subobjects will also have their last time values set.
   virtual void SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects = false) = 0;

   // ====================================================================================================================

   // Aerodynamics functions

   // Calulate the stall angle
   virtual void CalcStallAngle();

   // This function is used to calculate the lift, drag, side force, and moments
   // using the current state. The reference point is typically (0,0,0), but
   // may be a non-origin reference due to subobjects. This function will include
   // aero contributions by subobjects, if appropriate.
   virtual void CalculateAeroBodyForceAndMoments(UtVec3dX& aLiftBodyVector_lbs,
                                                 UtVec3dX& aDragBodyVector_lbs,
                                                 UtVec3dX& aSideForceBodyVector_lbs,
                                                 UtVec3dX& aMoment_ftlbs,
                                                 UtVec3dX& aReferencePt_ft) = 0;

   virtual void CalculateAeroBodyForceAndRotation(UtVec3dX& aLiftBodyVector_lbs,
                                                  UtVec3dX& aDragBodyVector_lbs,
                                                  UtVec3dX& aSideForceBodyVector_lbs,
                                                  UtVec3dX& aRotationAccelerationLimits_rps2,
                                                  UtVec3dX& aStabilizingFrequency_rps) = 0;

   virtual void CalculateObjectPropulsion(double                 aSimTime_sec,
                                          double                 aDeltaT_sec,
                                          double                 aAlt_ft,
                                          double                 aDynPress_lbsqft,
                                          double                 aStatPress_lbssqft,
                                          double                 aSpeed_fps,
                                          double                 aMach,
                                          double                 aAlpha_rad,
                                          double                 aBeta_rad,
                                          ForceAndMomentsObject& aForceAndMoment,
                                          double&                aFuelBurnRate_pps,
                                          double&                aFuelBurned_lbs) = 0;

   virtual void CalculateObjectPropulsion(double                  aSimTime_sec,
                                          double                  aDeltaT_sec,
                                          double                  aAlt_ft,
                                          double                  aDynPress_lbsqft,
                                          double                  aStatPress_lbssqft,
                                          double                  aSpeed_fps,
                                          double                  aMach,
                                          double                  aAlpha_rad,
                                          double                  aBeta_rad,
                                          ForceAndRotationObject& aForceAndRotation,
                                          double&                 aFuelBurnRate_pps,
                                          double&                 aFuelBurned_lbs) = 0;

   // This updates the fuel/propellant consumption for propulsion system, but
   // does not update burn for any subobjects, which is handled by those objects
   void UpdateFuelBurn(int64_t aSimTime_nanosec, double aDeltaT_sec, const KinematicState& aState);


   // Parent-object functions

   bool IsCaptive() const { return mIsCaptive; }
   void SetCaptiveState(bool aIsCaptive) { mIsCaptive = aIsCaptive; }
   void SetLastCaptiveState(bool aLastIsCaptive) { mLastIsCaptive = aLastIsCaptive; }
   void SetAngRelToParentYPR_rad(const UtVec3dX& aAngRelToParentYPR_rad)
   {
      mAngRelToParentYPR_rad = aAngRelToParentYPR_rad;
   }
   void GetPositionRelativeToParent(UtVec3dX& aPosRelToParent_m, UtVec3dX& aAngRelToParentYPR_rad);

   void SetYawRelToParent_rad(double aYawAngle_rad);
   void SetPitchRelToParent_rad(double aYawAngle_rad);
   void SetRollRelToParent_rad(double aYawAngle_rad);

   void AdjustForParentInducedEffects();
   void ImpartSeparationEffects();

   // Subobject functions

   size_t GetNumSubobjects() const { return mSubObjectList.size(); }
   bool   JettisonSubobject(const std::string& aSubobjectName, int64_t aSimTime_nanosec);
   bool   JettisonSubobject(Mover* aSubobject, int64_t aSimTime_nanosec);

   // This will remove/jettison the specified subobject, transferring ownership to the caller if successful. Note that
   // this does not create a platform or any other action. It merely removes the subobject from the parent and removes
   // the captive flag.
   std::unique_ptr<Mover> DirectJettisonOfSubobject(Mover* aSubobject, double aSimTime_sec);

   // This adds the designated object as a subobject
   void AddSubObject(std::unique_ptr<Mover> aSubObject);

   // This returns a reference to the list of subobjects
   const std::list<std::unique_ptr<Mover>>& GetSubobjectList() const;

   Mover* GetSubobjectByName(const std::string& aSubobjectName) const;
   Mover* GetSubobjectByBaseName(const std::string& aSubobjectName) const;

   // GetThrustProducerObjectByName returns the engine with the specified name
   virtual ThrustProducerObject* GetThrustProducerObjectByName(const std::string& aName) const;

   virtual ThrustProducerObject* GetThrustProducerObjectByName(const std::string& aName,
                                                               const Mover*       aCallingObject,
                                                               int                aCallingLevel = 0) const;

   // This returns true if afterburners are present
   bool HasAfterburner() const;

   // GetFuelTankByName returns the fuel source with the specified name
   FuelTank* GetFuelTankByName(const std::string& aName) const;

   FuelTank* GetFuelTankByName(const std::string& aName, const Mover* aCallingObject, int aCallingLevel = 0) const;

   // This specifies the fuel feed (input tank name) for the specified engine. It returns true if the assignment is
   // successful.
   bool SetFuelFeed(std::string aEngineName, std::string aFuelTankName);

   // This specifies the fuel feed (input tank name) for all engines.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string aFuelTankName);

   // This returns the maximum total capacity of fuel in tanks for the object and all of its subobjects. Note that
   // this is capacity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets.
   double GetTotalFuelTankCapacity_lbs() const;

   // This returns the current total amount of fuel in tanks for the object and all of its subobjects. Note that this
   // is quantity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets. This returns
   // the total amount of fuel in tanks for the object and all of its subobjects.
   double GetCurrentTotalFuelTankQuantity_lbs() const;

   // This returns the maximum total capacity of fuel in tanks for the object, but not including any of its subobjects.
   // Note that this is capacity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets.
   double GetInternalFuelTankCapacity_lbs() const;

   // This returns the current total amount of fuel in tanks for the object, but not including any of its subobjects.
   // Note that this is quantity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets.
   double GetCurrentInternalFuelTankQuantity_lbs() const;

   // This returns the maximum total capacity of fuel in external tanks for the object. It does this by summing the fuel
   // in the first level subobjects. Note that this is capacity of fuel in fuel tanks, and does not include integral
   // fuel in solid-propellant rockets.
   double GetExternalFuelTankCapacity_lbs() const;

   // This returns the maximum total capacity of fuel in external tanks for the object. It does this by summing the fuel
   // in the first level subobjects. Note that this is capacity of fuel in fuel tanks, and does not include integral
   // fuel in solid-propellant rockets.
   double GetCurrentExternalFuelTankQuantity_lbs() const;

   // Returns the current fuel remaining (in percent)
   double GetRemainingFuelPercent() const;

   // This adds fuel to the object in a simplistic manner. It does not use the "fill rate" limit of tanks -- hence all
   // of the fuel is put into the tank(s) at once. Later, we can add a "proper" fuel fill that is more realistic and
   // deals with a fill rate that will require a finite amount of time to transfer. This function will fill internal
   // tanks first and then any external/drop tanks and subobjects will be filled next. However, for simplicity, it
   // will make all non-internal tanks filled to the same percent full level, even if they already contained more
   // fuel -- in other words, it will redistribute non-internal fuel to keep the percent full consistent.
   void AddFuelQuantity_lbs(double aFuelToAdd_lbs);

   // This will set the fuel in a specified tank to the specified level, as long as the tank capacity can contain
   // it -- otherwise, the tank will be filled and the excess fuel will be "wasted". This function performs the
   // transfer instantly, regardless of the transfer rates supported by the tank and fuel system. It also ignores any
   // fuel that already may be in the tank.
   void SetFuelInTank_lbs(const std::string& aTankName, double aFuel_lbs);

   // Returns the number of external fuel tanks (subobjects that can contain fuel) being carried.
   // Note -- This only checks the first level of subobjects.
   int GetNumberOfExternalTanks() const;

   // This returns the total fuel flow (lbs/hr) for all engines on the vehicle
   virtual double GetTotalVehicleFuelFlow_pph() const = 0;

   // This returns the total fuel flow (lbs/hr) for the main engines on the vehicle. It does not include any subobjects.
   virtual double GetTotalFuelFlow_pph() const = 0;

   // Ignite all of the engines in the propulsion object
   void IgniteObject(int64_t aIgniteTimeInFrame_nanosec);

   // This terminates thrust from the propulsion object but not any subobjects aTerminateTime_sec is the time
   // termination occurs
   void ShutdownObject(int64_t aTerminateTime_nanosec = 0);

   // This terminates thrust from the propulsion object and all subobjects aTerminateTime_sec is the time
   // termination occurs
   void TerminateThrust(int64_t aTerminateTime_nanosec = 0);

   // This enables/disables controls. When controls are disabled, all control inputs are set to zero.
   void EnableControls(bool aEnabled);

   // This returns the controls enabled/disabled state.
   bool ControlsEnabled() const;

   // This returns true if an autopilot is present
   bool AutopilotAvailable() const;

   virtual double GetCurrentThrust_lbs()                = 0;
   virtual double GetCurrentForwardThrust_lbs()         = 0;
   virtual double GetMaximumPotentialThrust_lbs() const = 0;
   virtual double GetMinimumPotentialThrust_lbs() const = 0;

   // This provides the current normalized military-power thrust produced by engines on the vehicle. It includes
   // thrust from any first-level/immediate subobjects and may range from 0.0 to 1.0. Note -- This uses the first
   // engine on the vehicle (which may differ from other engines).
   double GetNormalizedMilitaryThrust() const;

   // This provides the current normalized afterburner thrust produced by engines on the vehicle. It includes
   // thrust from any first-level/immediate subobjects and may range from 0.0 to 1.0. Note -- This uses the first
   // engine on the vehicle (which may differ from other engines).
   double GetNormalizedAfterburnerThrust() const;

   // This provides the current normalized total thrust produced by engines on the vehicle. It includes thrust
   // from any first-level/immediate subobjects and may range from 0.0 to 2.0, where 0.0 is idle thrust, 1.0 is
   // full military power, and 2.0 is full afterburner. Note -- This uses the first engine on the vehicle (which
   // may differ from other engines).
   double GetNormalizedTotalThrust() const;

   virtual void SetActiveAeroObject(const std::string& aName) = 0;

   // ====================================================================================================================

   // Sequencer functions

   Sequencer* GetSequencerByName(const std::string& aName) const;

   // This determines when the next time-based events (either sequencer event or a guidance and control event) will
   // occur, within a goal simulation time. It returns true if the event time is valid, else returns false, indicating
   // that there are no currently queued sequencer or guidance/control events.
   bool PendingEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTime_nanosec);

   // This determines when the next time-based sequencer events will occur, within a goal simulation time. It returns true
   // if the event time is valid, else returns false, indicating that there are no currently queued sequencer or
   // guidance/control events.
   bool PendingSequencerEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTime_nanosec);

   // This determines when the next time-based guidance or control event will occur, within a goal simulation time. It
   // returns true if the event time is valid, else returns false, indicating that there are no currently queued
   // sequencer or guidance/control events.
   bool PendingGuidanceControlEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTime_nanosec);

   // This will attempt to activate a sequencer with the specified name. Returns true if the sequencer was activated. It
   // will return false if the sequencer was already activated or cannot be activated.
   bool ActivateSequencer(const std::string& aSequencerName);

   // This will attempt to activate the specified sequencer on the specified subobject. If there is no first-level
   // subobjects with the specified name, the command will be passed to subobject, which will recurse the command to its
   // subobjects, and so on. Returns true if the sequencer was activated. It will return false if the sequencer was
   // already activated or cannot be activated.
   bool ActivateSequencer(const std::string& aSubobjectName, const std::string& aSequencerName);

   // ====================================================================================================================

   // Timing functions

   // This returns the nominal timestep (in seconds) of the object
   double GetStepSize_sec() const { return utils::TimeToTime(mStepSize_nanosec); }

   // This returns the last update time (in seconds) of the object
   double GetLastUpdateTime_sec() const { return utils::TimeToTime(mLastSimTime_nanosec); }

   // This returns the total lifetime (in seconds) of the object, including time when the object was free or captive
   // to a parent
   double GetLifeTime_sec() const { return utils::TimeToTime(mLifeTime_nanosec); }

   // This returns the time of flight (in seconds) of the object, when it was free from being captive to a parent
   double GetTimeOfFlight_sec() const { return utils::TimeToTime(mTimeOfFlight_nanosec); }

   // This returns the total lifetime (in integer nanoseconds) of the object, including time when the object was free or
   // captive to a parent
   int64_t GetLifeTime_nanosec() const { return mLifeTime_nanosec; }

   // This returns the time of flight (in integer nanoseconds) of the object, when it was free from being captive to a parent
   int64_t GetTimeOfFlight_nanosec() const { return mTimeOfFlight_nanosec; }

   // ====================================================================================================================

   // Naming functions

   // This gets the type name
   const std::string& GetName() const { return mTypeName; }

   // This gets the base name
   std::string GetBaseName() const { return mBaseName; }

   // This gets the derived from object name
   std::string GetDerivedFromName() const { return mDerivedFromName; }

   // This is called to (possibly) add a prefix to the name of the object instance
   void SetNamePrefix(const std::string& aPrefixName);

   // This sets the basename
   void SetBaseName(const std::string& aName) { mBaseName = aName; }

   // This sets the type name
   void SetName(const std::string& aName) { mTypeName = aName; }

   // ====================================================================================================

   // Testing functions

   virtual MoverTestObject* GetTestSupportObject() const = 0;

   // This function is used to drive SixDOF in external programs, especially for performance calculations. It
   // automatically resets internal SixDOF data as well as setting the altitude, speed, pitch angle, stick and throttle
   // as specified. It also sets flags for freeze location, altitude, fuel burn, and no lag testing as well as
   // performing a single Update() function, automatically a synthetic sim time argument. Note: the stick back value may
   // be +/-1 while flaps are are 0-1. Throttle forward assumes that 1 is full (afterburner) and 0.5 is military power.
   virtual void TestingUpdate(double aAltitude_ft,
                              double aSpeed_fps,
                              double aPitchAngle_deg,
                              double aStickBack,
                              double aThrottleForward,
                              double aFlapsDown);

   virtual void ZeroKinematicsDuringTesting() = 0;

   double GetNominalEstimateMaxMach() const { return mMaxMach; }
   double GetNominalEstimateMaxAlpha_deg() const { return mMaxAlpha_deg; }
   double GetNominalEstimateMinAlpha_deg() const { return mMinAlpha_deg; }
   double GetNominalEstimateMaxBeta_deg() const { return mMaxBeta_deg; }

   // These functions return the normalized control input (lever) value
   virtual double GetStickRightInput() const  = 0;
   virtual double GetStickBackInput() const   = 0;
   virtual double GetRudderRightInput() const = 0;
   virtual double GetThrottleInput() const    = 0;
   virtual double GetSpeedBrakeInput() const  = 0;
   virtual double GetSpoilerInput() const     = 0;
   virtual double GetLandingGearInput() const = 0;

   // ====================================================================================================================

   // Damage and Destruction

   // This destroys the mover
   void SetDestroyed();

   bool GetIsDestroyed() const { return mIsDestroyed; }

   // ====================================================================================================================

   // Appearance functions

   // This group of functions provide support for appearance attributes, which are often used by visualization tools and
   // networked simulation, such as DIS. These functions are included in the SixDOF_Vehicle class, since the vehicle is
   // best able to determine these appearances.

   virtual void UpdateAppearance(){};

   // External appearance control. These functions allow the appearance to be set from an external source.
   bool AnEngineIsOperating() const;
   bool AnEngineIsSmoking() const;
   bool AnEngineHasAfterburnerOn() const;
   bool ContrailTrailingEffectPresent() const;
   bool RocketSmokeTrailingEffectPresent() const;
   bool DamageSmokeTrailingEffectPresent() const;
   bool FlamesArePresent() const;

   void MakeEnginesSmoke();
   void ActivateDamageSmokeTrailingEffect(bool aEffectActive);
   void ActivateFlamesEffect(bool aFlamesPresent);

   // This returns true if the specified altitude is within the contrailing band. If it is not within the band or
   // contrailing conditions do not exist, it returns false.
   bool WithinContrailAltitudeBand(double aAltitude_ft) const;

   virtual void CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
                                             double& aCurrentTotalFuel_lbs,
                                             double& aMaxTotalFuel_lbs) = 0;

   // The returns true is a spherical-earth model is being used instead of an oblate-earth model.
   bool UsingSphericalEarth() const { return mKinematicState.UseSphericalEarth(); }

   // This is used to prevent crashes when passing through the ground/sea surface or the artificial "floor" of sea
   // level. It is usually set to true when testing.
   void SetTestingIgnoreAllCrashes(bool aValue) { mTestingIgnoreAllCrashes = aValue; }

   // This returns true when crashes with the ground will be ignored
   bool GetTestingIgnoreAllCrashes() const { return mTestingIgnoreAllCrashes; }

   // This is used to prevent the vehicle from going below sea level at any time. The effectively limits the altitude
   // to zero. It is sometimes set to true when testing.
   void SetTestingClampToSeaLevelMinAlt(bool aValue) { mTestingClampToSeaLevelMinAlt = aValue; }

   // This returns true when the vehicle will be prevented from going below sea level at any time. The altitude will
   // effectively be limited to zero.
   bool GetTestingClampToSeaLevelMinAlt() const { return mTestingClampToSeaLevelMinAlt; }

   // ====================================================================================================================

   UtCallbackListN<void(double)> CrashedIntoGround;
   // First Mover* is the Newly Jettisoned Object, and the second is the Parent object that released it
   UtCallbackListN<void(std::unique_ptr<Mover>&)> SubobjectJettisoned;

   //! @name Callback objects for route changes and stage transitions. Another object can 'connect' to these objects to
   //! receive notification when a stage transition occurs. The first argument in the callback is the current
   //! simulation time. The second argument is the actual time of the event. This may less than the current simulation
   //! time.
   //@{
   UtCallbackListN<void(double)>         RouteChanged;
   UtCallbackListN<void(double, double)> StageIgnition;
   UtCallbackListN<void(double, double)> StageBurnout;
   UtCallbackListN<void(double, double)> StageSeparation;
   //@}

protected:
   explicit Mover(const Mover& aSrc);

   bool ExecuteTriggersCB(unsigned int aPointIndex);
   bool ExecuteTriggers(double aSimTime, unsigned int aPointIndex) override;

   void CrashIntoGroundCB(double aSimTime);

   void CheckForNewlyJettisonedPlatforms(double aSimTime);

   // This is a callback to handle a jettison
   void HandleSubobjectJettison(std::unique_ptr<Mover>& aJettisonedObject);

   // This is used to perform common tasks required when a subobject is jettisoned. If it succeeds, it returns a
   // pointer to a newly created platform. If it fails, it returns null.
   WsfPlatform* CommonSubobjectJettisonTasks(std::unique_ptr<Mover> aJettisonedObject);

   Route* ConvertWsfRouteToSixDOF_Route(const WsfRoute* aWsfRoute);

   void InitializeSubobject(double              aSimTime,
                            std::string&        aRootName,
                            Mover*              aObject,
                            const UtQuaternion& aRotation,
                            const UtVec3dX&     aOffset);

   virtual void CopyDataFromParent();

   // This is used by all Initialize functions to complete "details"
   virtual bool InitializeDetails(double aSimTime_sec) { return true; };

   virtual void CalculateWashIn(double aSimTime_sec);

   virtual void UpdatePropulsionFuelBurn(double aSimTime_sec, double aDeltaTime_sec, const KinematicState& aState) = 0;

   // This removes a sequencer (with specified name), from the sequencer list
   bool RemoveSequencer(const std::string& aName);

   std::unique_ptr<Mover> SeparateSubObject(Mover* aSubObject);

   void SetSphericalEarthFlagOnSubobjects(bool aSetting);

   void CalculateSizeFactor(double dT_sec);

   void SetupStartingLatLon();
   void SetTheLastValues();

   virtual void HandleGroundContact(int64_t aSimTime_nanosec);

   virtual void PerformCrash(int64_t aSimTime_nanosec);
   void         ClampToSeaLevelMinAlt();

   void ComputeWashInConditions(double aSimTime_sec);

   // ====================================================================================================================

   const double cUNDEFINED_DOUBLE = 1.0E+30; // This must be a large POSITIVE value

   Mover* mParentVehicle = nullptr;

   bool mEnginesOffAtStart            = false;
   bool mThrottleIdleAtStart          = false;
   bool mThrottleFullAtStart          = false;
   bool mThrottleAfterburnerAtStart   = false;
   bool mAutopilotNoControlAtStart    = false;
   bool mEnableThrustVectoringAtStart = true;
   bool mEnableControlsAtStart        = true;

   //// Guidance data

   double  mMaxGees = 0.0;
   double  mSpeed   = 0.0;
   UtVec3d mManeuverPlane;

   WsfMoverGuidance* mGuidancePtr        = nullptr;
   double            mYGLoad             = 0.0; // Using mYGLoad instead of mYForce
   double            mZGLoad             = 0.0; // Using mZGLoad instead of mZForce
   double            mCommandedYawRate   = 0.0;
   double            mCommandedPitchRate = 0.0;
   double            mCommandedRollRate  = 0.0;
   double            mCommandedSpeed     = -1.0;

   //! The absolute simulation time when the engines are commanded explicitly commanded to stop.
   //! It can be set either from the launch computer or via explicit script command. If not defined,
   //! then the engines will burn until they run out of fuel.
   double mCommandedEngineStopTime = cUNDEFINED_DOUBLE;

   //! The minimum height above terrain required for terrain following.
   double mMinimumHeightAboveTerrain = 0.0;

   //! Information captured at time of 'first missile motion' (needed by trajectory output)
   //@{
   double mFMM_Time    = -1.0;
   double mFMM_Heading = 0.0;
   double mFMM_LocWCS[3]{0.0, 0.0, 0.0};
   //@}

   double mLastStickX                = 0.0;
   double mLastStickY                = 0.0;
   double mLastRudder                = 0.0;
   double mLastThrottle              = 0.0;
   double mLastSpdBrakeLeverPosition = 0.0;
   double mLastSpoilerLeverPosition  = 0.0;
   double mLastFlapsLeverPosition    = 0.0;

   //// Formation data

   double mInitialPosition_Latitude  = 0.0;
   double mInitialPosition_Longitude = 0.0;
   double mInitialAltitude_m         = 0.0;
   double mInitialNEDHeading         = 0.0;
   double mInitialNEDPitch           = 0.0;
   double mInitialNEDRoll            = 0.0;
   double mInitialSpeed_NED_X_fps    = 0.0;
   double mInitialSpeed_NED_Y_fps    = 0.0;
   double mInitialSpeed_NED_Z_fps    = 0.0;
   bool   mValidInitialPosition      = false;
   bool   mValidInitialAltitude_ft   = false;
   bool   mValidInitialNEDHeading    = false;
   bool   mValidInitialNEDPitch      = false;
   bool   mValidInitialNEDRoll       = false;
   bool   mValidInitialSpeed_NED     = false;

   UtCloneablePtr<wsf::six_dof::ManeuverSequence> mManeuverPtr{nullptr};

   unsigned int mStageIndex = 0; //!< The index of the current stage

   std::queue<WsfPlatform*> mNewlyJettisonedPlatforms;

   UtCloneablePtr<WsfRoute> mWsfRoutePtr{nullptr};
   UtCloneablePtr<WsfRoute> mDefaultWsfRoutePtr{nullptr};

   bool mFollowVerticalTrack = false;

   double mJokerFuel_lbs = 0.0;
   double mBingoFuel_lbs = 0.0;

   // Event Pipe Manager for SixDOF Data. Maps a message name to an update rate and number of updates.
   std::map<std::string, std::pair<double, int>>      mEventPipeManager;
   std::map<std::string, std::vector<WsfVisualPart*>> mVisualPartManager;
   UtCallbackHolder                                   mCallbacks;

   bool mIsInitialized = false;

   std::string mTypeName;        // This type
   std::string mBaseName;        // Base type
   std::string mDerivedFromName; // Immediate ancestor type
   std::string mIntegratorTypeName;

   int64_t mLastSimTime_nanosec  = 0;
   int64_t mStepSize_nanosec     = 0;
   int64_t mLifeTime_nanosec     = 0;
   int64_t mTimeOfFlight_nanosec = 0;

   bool mIsCaptive     = false;
   bool mLastIsCaptive = true;

   UtVec3dX mPosRelToParent_m;
   UtVec3dX mAngRelToParentYPR_rad;
   UtVec3dX mSeparationVel_fps;
   UtVec3dX mSeparationOmega_rps;

   std::list<std::unique_ptr<Mover>>        mSubObjectList;
   std::vector<ut::CloneablePtr<Sequencer>> mSequencerList;

   const Environment* mEnvironment;
   MassProperties     mMassProperties;
   KinematicState     mKinematicState;

   FreezeFlags mFreezeFlags;

   // The size factor allows effects (esp aerodynamic)
   SizeFactor mSizeFactor;

   double mStallAngle               = 0.0;
   double mStallDetectionDeltaAngle = 0.0; // This is a delta angle where stall is detected

   // These parameters are used to generate internal tabular data. Although the object can
   // operate outside of these values, the tabular data will be clamped to the highest value.
   // For this reason, accurate estimates of the operating envelope should be used.
   double mMaxMach      = 100.0;
   double mMaxAlpha_deg = 180.0;
   double mMinAlpha_deg = -180.0;
   double mMaxBeta_deg  = 90.0;

   // If set to true, the object will not move
   bool mFixedObject = false;

   double mPrepositionLat             = 0.0;
   double mPrepositionLon             = 0.0;
   double mInitialLat                 = 0.0;
   double mInitialLon                 = 0.0;
   double mInitialAlt_m               = 0.0;
   double mDownRangeDist_m            = 0.0;
   double mThrustTerminationTime_sec  = 0.0;
   bool   mThrustTerminationTimeValid = false;

   //// Appearance data

   bool mCrashed                   = false;
   bool mEngineOn                  = false;
   bool mAfterburnerOn             = false;
   bool mSmoking                   = false;
   bool mOnFire                    = false;
   bool mDamageSmokeTrailingEffect = false;
   bool mFlamesPresent             = false;
   bool mIsDestroyed               = false;

   bool   mProducesLaunchSmoke = false;
   double mLaunchSmokeTime_sec = 0.0;
   bool   mRisingSmokePlume    = false;
   bool   mLaunchFlashSmoke    = false;
   bool   mLightDamage         = false;
   bool   mHeavyDamage         = false;

   bool mInitSetupMode                = false;
   bool mUseWashInInitConditions      = false;
   bool mTestingIgnoreAllCrashes      = false;
   bool mTestingClampToSeaLevelMinAlt = false;

   int64_t mTestingSimTime_nanosec = 0;
};
} // namespace six_dof
} // namespace wsf

#endif
