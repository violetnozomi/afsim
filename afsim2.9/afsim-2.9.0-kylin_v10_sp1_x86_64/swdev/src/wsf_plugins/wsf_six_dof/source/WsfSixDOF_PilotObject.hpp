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

#ifndef WSFSIXDOFPILOTOBJECT_HPP
#define WSFSIXDOFPILOTOBJECT_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtCloneablePtr.hpp"
#include "UtInput.hpp"
#include "WsfSixDOF_AutopilotAction.hpp"
#include "WsfSixDOF_CommonController.hpp"
#include "WsfSixDOF_Object.hpp"
#include "WsfSixDOF_Route.hpp"
#include "WsfSixDOF_VehicleData.hpp"
#include "WsfSixDOF_Waypoint.hpp"

namespace wsf
{
namespace six_dof
{
class KinematicState;

class WSF_SIX_DOF_EXPORT PilotObject : public Object
{
public:
   PilotObject() = default;

   PilotObject& operator=(const PilotObject& other) = delete;

   // In most instances, PilotObject children should override this function
   virtual bool ProcessInput(UtInput& aInput);

   // In many instances, PilotObject children should override this function
   void Update(int64_t aSimTime_nanosec);

   virtual void ManageFrozenControls() = 0;

   void SetLastSimTime(int64_t aLastSimTime_nanosec) { mLastSimTime_nanosec = aLastSimTime_nanosec; }

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   virtual std::string GetPilotType() const = 0;

   // Callback function for achieving waypoint. The callback registration
   // should be performed in Mover.
   UtCallbackListN<bool(unsigned int)> WaypointAchieved;

   // Pilot manager functions -------------------------------------------------------------

   // PilotManager Functions -- These functions (which are inherited,
   // and not overridable) are called by the PilotManager as it conveys
   // information to each PilotObject that it manages.

   // This function should be called every update by PilotManager,
   // which will be called from Mover. Each PilotObject passes these
   // values to its CommonController (autopilot), if one exists, where
   // values can be differentiated to determine deltaRoll, for example.
   void InputAngleDeltas(double aYaw_rad, double aPitch_rad, double aRoll_rad);

   // This enables/disables controls. When controls are disabled,
   // all control inputs are set to zero.
   void EnableControlInputs(bool aEnabled) { mControlsEnabled = aEnabled; }

   // This returns the controls enabled/disabled state.
   bool ControlsAreEnabled() const { return mControlsEnabled; }

   // This function is called by PilotManager when the Mover is destroyed in flight.
   virtual void SetDestroyed();

   // Control input positions -------------------------------------------------------------

   // These functions return the current positions of specified control inputs

   // This returns the current stick back control input position
   virtual double GetStickBackControllerPosition() const = 0;

   // This returns the current stick right control input position
   virtual double GetStickRightControllerPosition() const = 0;

   // This returns the current rudder right control input position
   virtual double GetRudderRightControllerPosition() const = 0;

   // This returns the current throttle control input position,
   // using IDLE=0, MIL=1.0, and AB=2.0
   virtual double GetThrottleControllerPosition() const = 0;

   // This returns the current military (non-afterburner) throttle control
   // input position. For a combined military/afterburner throttle value,
   // use GetThrottleControllerPosition() instead.
   virtual double GetThrottleMilitaryControllerPosition() const = 0;

   // This returns the current afterburner throttle control input
   // position. For a combined military/afterburner throttle value,
   // use GetThrottleControllerPosition() instead.
   virtual double GetThrottleAfterburnerControllerPosition() const = 0;

   // This returns the current speed brakes control input position
   virtual double GetSpeedBrakesControllerPosition() const = 0;

   // This returns the current flaps control input position
   virtual double GetFlapsControllerPosition() const = 0;

   // This returns the current landing gear control input position
   virtual double GetLandingGearControllerPosition() const = 0;

   // This returns the current spoilers control input position
   virtual double GetSpoilersControllerPosition() const = 0;

   // This returns the current thrust reverser control input position
   virtual double GetThrustReverserControllerPosition() const = 0;

   // Manual pilot functions --------------------------------------------------------------
   // TODO -- These will be moved to the manual pilot classes, as appropriate

   // These functions provide an interface for a ManualPilot controlled via
   // input from a crew station.

   // This returns true if "external manual control" is active
   virtual bool UsingManualControl() const;

   // When called, controls will be set through SetManualControlData()
   virtual void TakeManualControl();

   // When called, the autopilot will gain control
   virtual void ReleaseManualControl();

   // This allows external controls to be "injected" into the PilotObject.
   // This variant allows a reduced set of inputs, omitting rudder.
   // The aStickRightPosition and aStickAftPosition values are +/-1. The
   // aSpdBrakeLeverPosition, aSpoilerLeverPosition, and aFlapsLeverPosition
   // are 0-1. The aThrottleLeverPosition assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power
   // with full augmentation/afterburner (full afterburner).
   virtual void SetManualControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aThrottleLeverPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition) = 0;

   // This allows external controls to be "injected" into the PilotObject.
   // This variant includes rudder control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, and aFlapsLeverPosition are 0-1. The
   // aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner).
   virtual void SetManualControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aRudderRightPosition,
                                     double aThrottleLeverPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition) = 0;

   // This allows external controls to be "injected" into the PilotObject.
   // This variant adds landing gear control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, aFlapsLeverPosition, and aLandingGearLeverPosition are 0-1.
   // The aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner). The aLandingGearLeverPosition assumes
   // that 0 is gear up and 1 is gear full down and locked.
   virtual void SetManualControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aRudderRightPosition,
                                     double aThrottleLeverPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition,
                                     double aLandingGearLeverPosition) = 0;

   // This allows external controls to be "injected" into the PilotObject.
   // This variant adds thrust vectoring and enhanced landing gear and braking
   // controls. The aStickRightPosition, aStickAftPosition, aRudderRightPosition,
   // aThrustVectorYawRightPosition, aThrustVectorPitchUpPosition,
   // aThrustVectorRollRightPosition, and aNoseWheelSteeringRightPosition values
   // are +/-1. The aWheelBrakeLeftPosition and aWheelBrakeRightPosition assume
   // that 0 is brake off and 1 is full brake. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, aFlapsLeverPosition, and aLandingGearLeverPosition
   // are 0-1. The aThrottleLeverPosition assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power with
   // full augmentation/afterburner (full afterburner). The
   // aThrustReverserLeverPosition assumes that 0 is no reverser and 1 is full
   // reverser. The aLandingGearLeverPosition assumes that 0 is gear up and 1 is
   // gear full down and locked. The aNWS_Enabled is a flag to enable/disable
   // Nose Wheel Steering (NWS) and assumes false is normal mode and true is NWS
   // mode. NWS mode provides a greater range of motion for the nose wheel, often
   // used when taxiing.
   virtual void SetManualControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aRudderRightPosition,
                                     double aThrottleLeverPosition,
                                     double aThrustReverserLeverPosition,
                                     double aThrustVectorYawRightPosition,
                                     double aThrustVectorPitchUpPosition,
                                     double aThrustVectorRollRightPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition,
                                     double aLandingGearLeverPosition,
                                     double aNoseWheelSteeringRightPosition,
                                     double aWheelBrakeLeftPosition,
                                     double aWheelBrakeRightPosition,
                                     bool   aNWS_Enabled) = 0;

   // These functions provide control for CAS

   virtual void SetControlAugmentationModeActive(bool aCASIsActive);
   virtual bool ControlAugmentationModeIsActive() const;

   // This allows external trim inputs to be "injected" into the PilotObject.
   // The values represent how long the trim switch has been pressed multiplied
   // by the direction. Thus, a negative direction will result in a negative value.
   virtual void SetTrimManualControlData(double aNoseUpTrimDeltaT_sec,
                                         double aRollRightTrimDeltaT_sec,
                                         double aYawRightTrimDeltaT_sec);

   // Synthetic pilot functions -----------------------------------------------------------
   // TODO -- These will be moved to the synthetic pilot class, as appropriate

   // These functions provide an interface for a SyntheticPilot controlled via
   // script commands

   // When called, controls will be set through SetControlData()
   virtual void TakeExternalDirectControl();

   // When called, the autopilot will gain control
   virtual void ReleaseExternalDirectControl();

   // This allows external controls to be "injected" into the PilotObject.
   // This variant allows a reduced set of inputs, omitting rudder.
   // The aStickRightPosition and aStickAftPosition values are +/-1. The
   // aSpdBrakeLeverPosition, aSpoilerLeverPosition, and aFlapsLeverPosition
   // are 0-1. The aThrottleLeverPosition assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power
   // with full augmentation/afterburner (full afterburner).
   virtual void SetExternalDirectControlData(double aStickRightPosition,
                                             double aStickAftPosition,
                                             double aThrottleLeverPosition,
                                             double aSpdBrakeLeverPosition,
                                             double aSpoilerLeverPosition,
                                             double aFlapsLeverPosition) = 0;

   // This allows external controls to be "injected" into the PilotObject.
   // This variant includes rudder control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, and aFlapsLeverPosition are 0-1. The
   // aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner).
   virtual void SetExternalDirectControlData(double aStickRightPosition,
                                             double aStickAftPosition,
                                             double aRudderRightPosition,
                                             double aThrottleLeverPosition,
                                             double aSpdBrakeLeverPosition,
                                             double aSpoilerLeverPosition,
                                             double aFlapsLeverPosition) = 0;

   // This allows external controls to be "injected" into the PilotObject.
   // This variant adds landing gear control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, aFlapsLeverPosition, and aLandingGearLeverPosition are 0-1.
   // The aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner). The aLandingGearLeverPosition assumes
   // that 0 is gear up and 1 is gear full down and locked.
   virtual void SetExternalDirectControlData(double aStickRightPosition,
                                             double aStickAftPosition,
                                             double aRudderRightPosition,
                                             double aThrottleLeverPosition,
                                             double aSpdBrakeLeverPosition,
                                             double aSpoilerLeverPosition,
                                             double aFlapsLeverPosition,
                                             double aLandingGearLeverPosition) = 0;

   // This allows external controls to be "injected" into the PilotObject.
   // This variant adds thrust vectoring and enhanced landing gear and braking
   // controls. The aStickRightPosition, aStickAftPosition, aRudderRightPosition,
   // aThrustVectorYawRightPosition, aThrustVectorPitchUpPosition,
   // aThrustVectorRollRightPosition, and aNoseWheelSteeringRightPosition values
   // are +/-1. The aWheelBrakeLeftPosition and aWheelBrakeRightPosition assume
   // that 0 is brake off and 1 is full brake. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, aFlapsLeverPosition, and aLandingGearLeverPosition
   // are 0-1. The aThrottleLeverPosition assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power with
   // full augmentation/afterburner (full afterburner). The
   // aThrustReverserLeverPosition assumes that 0 is no reverser and 1 is full
   // reverser. The aLandingGearLeverPosition assumes that 0 is gear up and 1 is
   // gear full down and locked. The aNWS_Enabled is a flag to enable/disable
   // Nose Wheel Steering (NWS) and assumes false is normal mode and true is NWS
   // mode. NWS mode provides a greater range of motion for the nose wheel, often
   // used when taxiing.
   virtual void SetExternalDirectControlData(double aStickRightPosition,
                                             double aStickAftPosition,
                                             double aRudderRightPosition,
                                             double aThrottleLeverPosition,
                                             double aThrustReverserLeverPosition,
                                             double aThrustVectorYawRightPosition,
                                             double aThrustVectorPitchUpPosition,
                                             double aThrustVectorRollRightPosition,
                                             double aSpdBrakeLeverPosition,
                                             double aSpoilerLeverPosition,
                                             double aFlapsLeverPosition,
                                             double aLandingGearLeverPosition,
                                             double aNoseWheelSteeringRightPosition,
                                             double aWheelBrakeLeftPosition,
                                             double aWheelBrakeRightPosition,
                                             bool   aNWS_Enabled) = 0;

   // This allows external trim inputs to be "injected" into the PilotObject.
   // The values represent how long the trim switch has been pressed multiplied
   // by the direction. Thus, a negative direction will result in a negative value.
   virtual void SetExternalDirectControlTrimManualControlData(double aNoseUpTrimDeltaT_sec,
                                                              double aRollRightTrimDeltaT_sec,
                                                              double aYawRightTrimDeltaT_sec);

   // Testing support functions -----------------------------------------------------------

   // Testing Support -- These functions are used when testing. Note that they are
   // inherited, and are not overridable, since all PilotObjects support "testing".

   // This returns true if "testing control" is active
   bool UsingTestControl() const { return mTestControl; }

   // When called, controls will only respond to SetTest<type>ControllerPosition()
   // functions.
   void TakeTestControl();

   // When called, normal (non-test) control inputs will be used.
   void ReleaseTestControl();

   // This next group of functions are typically used to set controls for testing
   // and "calculation" functions.

   // Sets the stick right control:  -1=Full Forward (nose down), 0=Neutral,
   // 1=Full Aft (nose up)
   virtual void SetTestStickBackControllerPosition(double aStickAftPosition) = 0;

   // Sets the stick right control:  -1=Full Left, 0=Neutral, 1=Full Right
   virtual void SetTestStickRightControllerPosition(double aStickRightPosition) = 0;

   // Sets the rudder right control:  -1=Full Left, 0=Neutral, 1=Full Right
   virtual void SetTestRudderRightControllerPosition(double aRudderRightPosition) = 0;

   // Sets the speed brake control:  0=Closed, 1=Open
   virtual void SetTestSpeedBrakesControllerPosition(double aSpeedBrakesPosition) = 0;

   // Sets the flaps control:  0=Retracted, 1=Fully Extended
   virtual void SetTestFlapsControllerPosition(double aFlapsPosition) = 0;

   // Sets the spoilers control:  0=Retracted, 1=Fully Extended
   virtual void SetTestSpoilersControllerPosition(double aSpoilersPosition) = 0;

   // Sets the landing gear control:  0=Retracted, 1=Fully Extended
   virtual void SetTestLandingGearControllerPosition(double aLandingGearPosition) = 0;

   // This sets the throttle control (combined afterburner/military setting).
   // The throttle controller position assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power
   // with full augmentation/afterburner (full afterburner)
   virtual void SetTestThrottleControllerPosition(double aThrottlePosition);

   // This sets the current military (non-afterburner) throttle control input position
   virtual void SetTestThrottleMilitaryControllerPosition(double aThrottleLeverPosition);

   // This sets the current afterburner throttle control input position
   virtual void SetTestThrottleAfterburnerControllerPosition(double aThrottleLeverPosition);

   // These functions provide the ability to "preposition" the vehicle to a
   // specified altitude and speed during testing. Note that these should only be
   // used in testing, since they directly modify the speed/altitude.

   // Prepositions the vehicle to the specified altitude (ft) and KTAS.
   // Note: This should only be used in testing mode.
   virtual void SetPrePositionTAS(double aAltitude_ft, double aKTAS);

   // Prepositions the vehicle to the specified altitude (ft) and KCAS.
   // Note: This should only be used in testing mode.
   virtual void SetPrePositionCAS(double aAltitude_ft, double aKCAS);

   // Prepositions the vehicle to the specified altitude (ft) and Mach.
   // Note: This should only be used in testing mode.
   virtual void SetPrePositionMach(double aAltitude_ft, double aMach);

   // Prepositions the vehicle to the specified altitude (ft) and Dynamic Pressure.
   // Note: This should only be used in testing mode.
   virtual void SetPrePositionQ(double aAltitude_ft, double aDynamicPressure_lbft2);

   // -------------------------------------------------------------------------------------

   // These functions provide an interface for autopilot commands
   // TODO -- These will be moved to the other derived pilot classes, as appropriate

   AutopilotAction* GetCurrentAction() const;

   // This loads a AutopilotData with data
   virtual void GetAutopilotData(AutopilotData* aData);

   // This loads a sAutopilotPidGroupGainData with data and returns true if data is valid
   virtual bool GetAutopilotPidGainData(Pid::Type aTableType, size_t& aNumElements, PidGainData aPidGainData[]);

   // This loads a AutopilotPidGroupValueData with data and returns true if data is valid
   virtual void GetAutopilotPidValueData(AutopilotPidGroupValueData* aData);

   // Set the planned route
   // Note this function takes ownership of aRoute and will delete it!
   virtual void SetPlannedRoute(Route* aRoute);

   // Set the temporary route
   // Note this function takes ownership of aRoute and will delete it!
   virtual void SetTempRoute(Route* aRoute);

   // -------------------------------------------------------------------------------------

   // Theses functions provide additional controls for afterburner and speed brakes and
   // define how the CommonController (autopilot) will utilize them.

   // These functions provide special "override" commands

   // Afterburner use is enabled by default, but can be enabled/disabled

   virtual bool GetAfterburnerEnabled() const;
   virtual void SetAfterburnerEnabled(bool aEnabled);

   // Afterburner will be used (if enabled) if command exceeds the threshold value

   virtual double GetAfterburnerThreshold() const;
   virtual void   SetAfterburnerThreshold(double aValue = 1.0);

   // Speed brake use is enabled by default, but can be enabled/disabled

   virtual bool GetSpeedBrakeEnabled() const;
   virtual void SetSpeedBrakeEnabled(bool aEnabled);

   // Speed brake will be used (if enabled) if command is less than the threshold value
   virtual double GetSpeedBrakeThreshold() const;
   virtual void   SetSpeedBrakeThreshold(double aValue = 0.0);

   // Returns the current turn roll-in multiplier for autopilot turns when following waypoints
   double GetTurnRollInMultiplier();

   // Sets the current turn roll-in multiplier for autopilot turns when following waypoints
   void SetTurnRollInMultiplier(double aValue = 1.0);

   // -------------------------------------------------------------------------------------

   // The CommonController (autopilot) typically controls the throttle and speed brakes
   // as part of its speed control. However, these can be overridden with direct commands.

   virtual CommonController* GetCommonController() const = 0;

   CommonController::sAutopilotControls* GetCurrentControlLimits() { return &mAutopilotControls; }

   // Throttle is normally controlled by the autopilot, but it can be overridden
   // using these direct input functions.

   virtual void EnableDirectThrottleInput();
   virtual void ReleaseDirectThrottleInput();

   virtual void MoveThrottleToIdle();
   virtual void MoveThrottleToFull();
   virtual void MoveThrottleToAfterburner();
   virtual void SetDirectThrottleInput(double aValue);

   // Stick and rudder are normally controlled by the autopilot, but can be overridden
   // using these direct input functions.

   virtual void EnableDirectStickBackInput();
   virtual void ReleaseDirectStickBackInput();

   virtual void EnableDirectStickRightInput();
   virtual void ReleaseDirectStickRightInput();

   virtual void EnableDirectRudderRightInput();
   virtual void ReleaseDirectRudderRightInput();

   virtual void SetDirectStickBackInput(double aValue)   = 0;
   virtual void SetDirectStickRightInput(double aValue)  = 0;
   virtual void SetDirectRudderRightInput(double aValue) = 0;

   // Speed brakes are normally controlled by the autopilot, but they can be overridden
   // using these direct input functions.

   virtual void EnableDirectSpeedBrakeInput();
   virtual void ReleaseDirectSpeedBrakeInput();

   virtual void OpenSpeedBrake()                         = 0;
   virtual void CloseSpeedBrake()                        = 0;
   virtual void SetDirectSpeedBrakesInput(double aValue) = 0;

   // -------------------------------------------------------------------------------------

   // These functions provide control inputs for landing gear, flaps, spoilers,
   // thrust reversers, and nose wheel steering (NWS)

   virtual void SetLandingGearControlPosition(double aPosition)    = 0;
   virtual void SetFlapsControlPosition(double aPosition)          = 0;
   virtual void SetSpoilersControlPosition(double aPosition)       = 0;
   virtual void SetThrustReverserControlPosition(double aPosition) = 0;

   // -------------------------------------------------------------------------------------

   // These functions provide various waypoint-related support.
   // TODO -- These will be moved to the other derived pilot classes, as appropriate

   // This will replace the planned route with temporary waypoints
   // This may be called during a simulation [from script] because of
   // some special stimulus
   // Note this function takes ownership of aRoute and will delete it!
   virtual void FlyTempWaypoints(Route* aRoute);

   // Return the index of the current waypoint on the planned route
   virtual size_t GetPlannedWaypointIndex() const { return mCurrentPlannedWaypointIndex; };

   // Set the planned waypoint index, causing the SixDOF object to fly to this waypoint
   virtual bool SetPlannedWaypointIndex(size_t aIndex);

   // Sets the position of the SixDOF Object to a planned waypoint
   virtual bool SetPositionToPlannedWaypoint(size_t aIndex);

   // Return the index of the current waypoint on the temporary route
   virtual size_t GetTempWaypointIndex() const { return mCurrentTempWaypointIndex; };

   // Set the temp waypoint index, causing the SixDOF object to fly to this waypoint
   virtual bool SetTempWaypointIndex(size_t aIndex);

   // Sets the position of the SixDOF Mover to a temp waypoint
   virtual bool SetPositionToTempWaypoint(size_t aIndex);

   // Return whether or not the platform is flying the planned route. Otherwise
   // it is flying the temporary route
   virtual bool IsOnPlannedRoute() const { return mTempActionPtr == nullptr; }

   // This will cause the vehicle to ignore waypoints and fly to the specified point
   virtual void FlyAtPoint(const UtLLAPos& aPt);

   // This will return to the initial "planned" waypoints/route
   virtual void ReturnToPlannedWaypoints();

   //! radial accel and turn direction ignored for now
   virtual void TurnToHeading(double                              aHeading_rad, // radians
                              double                              aRadialAccel, // meters/sec^2
                              CommonController::eAutopilotTurnDir aTurn);

   virtual void GoToSpeed(double aSpeed,        // knots true air speed
                          double aLinearAccel); // meters/sec^2

   virtual void GoToAltitude(double aAltitude,   // meters
                             double aClimbRate); // meters/sec

   virtual bool FlyRates(double aRollRate,     // deg/sec
                         double aPitchRate,    // deg/sec
                         double aAcceleration, // feet/sec^2
                         double aSpeed);       // knots (true air speed)

   virtual bool FlyHeadingSpeedAltitude(double aHeading_rad, // rad
                                        double aSpeed,       // knots (true air speed)
                                        double aAltitude,    // feet
                                        double aMaxGees,     // Gs
                                        double aMaxClimb);   // feet/minute

   virtual bool FlySpecificTurn(double aRollError, // degrees
                                double aGees,      // Gs
                                double aSpeed);    // knots (true air speed)

   virtual bool FlySpecificVector(double aRollError,  // degrees
                                  double aPitchError, // degrees
                                  double aMaxGees,    // Gs
                                  double aSpeed);     // knots (true air speed)

   virtual bool FlyYawPitchAccel(double aYawAccel_g,    // accel in g's
                                 double aPitchAccel_g); // accel in g's

   virtual bool AutopilotIsEnabled() const { return mAutopilotEnabled; }

   // Gets the "planned" waypoint list - if it doesn't exist, an empty vector
   // will be returned
   virtual void GetPlannedWaypoints(AutopilotWaypointDataList* aWaypointDataList) const;

   // Gets the "temp" waypoint list - if it doesn't exist, an empty vector
   // will be returned
   virtual void GetTempWaypoints(AutopilotWaypointDataList* aWaypointDataList) const;

   // Gets the current waypoint list (either temp or planned) - if neither exists,
   // an empty vector will be returned
   virtual void GetCurrentWaypoints(AutopilotWaypointDataList* aWaypointDataList) const;

   // -------------------------------------------------------------------------------------

   // These functions provide autopilot "controls"
   // TODO -- These will be moved to the other derived pilot classes, as appropriate

   // This enables/disables the autopilot
   virtual void EnableAutopilot(bool aState);

   // This sets the autopilot to use pitch angle mode with the specified pitch angle (in degrees)
   virtual void SetAutopilotPitchAngle(double aPitchAngle_deg);

   // This sets the autopilot to use pitch rate mode with the specified pitch rate (in deg/sec)
   virtual void SetAutopilotPitchRate(double aPitchRate_dps);

   // This sets the autopilot to use flight path angle mode with the specified flight path angle (in degrees)
   virtual void SetAutopilotFlightPathAngle(double aFlightPathAngle_deg);

   // This sets the autopilot to use vertical speed mode with the specified vertical speed (in ft/min)
   virtual void SetAutopilotVerticalSpeed(double aVerticalSpeed_fpm);

   // This sets the autopilot to use altitude mode with the specified altitude (in feet)
   virtual void SetAutopilotAltitude(double aAltitude_ft);

   // This sets the autopilot to use pitch g-load mode with the specified g-load
   virtual void SetPitchGLoad(double aGLoad);

   // This sets the autopilot to use alpha (angle of attack) mode with the specified alpha (in degrees)
   virtual void SetAutopilotAlpha(double aAlpha_deg);

   // This sets the autopilot to use delta pitch mode with the specified delta pitch (in degrees)
   virtual void SetAutopilotDeltaPitch(double aDeltaPitchAngle_deg);

   // This sets the autopilot to use roll/bank angle mode with the specified roll/bank angle (in degrees)
   virtual void SetAutopilotRollAngle(double aRollAngle_deg);

   // This sets the autopilot to use roll rate mode with the specified roll rate (in deg/sec)
   virtual void SetAutopilotRollRate(double aRollRate_dps);

   // This sets the autopilot to use delta roll mode with the specified delta roll (in degrees)
   virtual void SetAutopilotDeltaRoll(double aDeltaRollAngle_deg);

   // This sets the autopilot to use heading mode with the specified heading (in degrees)
   virtual void SetAutopilotRollHeading(double aRollHeading_deg);

   // This sets the autopilot to use heading mode with the specified heading (in degrees)
   virtual void SetAutopilotYawHeading(double aYawHeading_deg);

   // This sets the autopilot to use yaw rate mode with the specified yaw rate (in deg/sec)
   virtual void SetAutopilotYawRate(double aYawRate_dps);

   // This sets the autopilot to use beta (angle of sideslip) mode with the specified beta (in degrees)
   virtual void SetAutopilotBeta(double aBeta_deg);

   // This sets the autopilot to use yaw g-load mode with the specified g-load
   virtual void SetAutopilotYawGLoad(double aBeta_deg);

   // This sets the autopilot to maintain the specified speed (in KTAS, knots true air speed)
   virtual void SetAutopilotSpeedKTAS(double aSpeed_KTAS);

   // This sets the autopilot to maintain the specified speed (in KCAS, knots calibrated air speed)
   virtual void SetAutopilotSpeedKCAS(double aSpeed_KCAS);

   // This sets the autopilot to maintain the specified speed (in Mach)
   virtual void SetAutopilotSpeedMach(double aSpeed_Mach);

   // This sets the autopilot to hold the specified throttle position
   virtual void SetAutopilotThrottle(double aSpeed_Mach);

   // This sets the autopilot to use waypoint mode in the lateral channel
   virtual void SetAutopilotLateralWaypointMode();

   // This sets the autopilot to use waypoint mode in the vertical channel
   virtual void SetAutopilotVerticalWaypointMode();

   // This sets the autopilot to use waypoint mode in the speed channel
   virtual void SetAutopilotSpeedWaypointMode();

   // This sets the autopilot to use waypoint mode
   virtual void SetAutopilotWaypointMode();

   // This sets the autopilot to hold all controls in a centered/neutralized position
   virtual void SetAutopilotNoControl();

   // -------------------------------------------------------------------------------------

   // Virtual functions for autopilot commands - default to no action
   virtual void HoldAltitude(){};
   virtual void HoldVerticalSpeed(){};
   virtual void HoldPitchAngle(){};
   virtual void HoldBankAngle(){};
   virtual void HoldHeading(){};
   virtual void HoldSpeedKCAS(){};
   virtual void HoldSpeedKTAS(){};
   virtual void HoldSpeedMach(){};

protected:
   PilotObject(const PilotObject& aSrc);

   // Inherited PilotObject Functions -- These functions are not overridden;
   // child PilotObjects will simply inherit these internal functions.

   // This "zeros" control inputs if controls are disabled
   virtual void ZeroDisabledControlData() = 0;

   // This updates the active pilot object with the commands from the parent vehicle
   virtual void InheritParentControlData() = 0;

   // This is called during Initialize to initial the common controller (autopilot)
   bool InitializeCommonController();

   // This drives control inputs to those provided by the CommonController (autopilot)
   virtual void UpdateControlInputsUsingCommonControllerData(int64_t aSimTime_nanosec);

   // SetThrottleData assumes that 1 is full, unaugmented power (military power)
   // and 2.0 is full afterburner. This function is called (directly or indirectly)
   // by all variants of the SetControlData functions.
   virtual void SetThrottleData(double aThrottleLeverPosition) = 0;

   // This sets the current military (non-afterburner) throttle control input position
   virtual void SetThrottleMilitaryData(double aThrottleLeverPosition) = 0;

   // This sets the current afterburner throttle control input position
   virtual void SetThrottleAfterburnerData(double aThrottleLeverPosition) = 0;

   virtual void EnforceControlLimits();

   void EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue);

   // *** THIS SHOULD BE OVERRIDDEN BY EACH PILOT OBJECT ***
   // This uses the internal controls (mAutopilotControls of type
   // AutopilotDataTypes::sAutopilotControls) to set the actual controls
   virtual void LoadControlDataWithAutopilotControlData() = 0;

   // *** THIS SHOULD BE OVERRIDDEN BY EACH PILOT OBJECT ***
   // This uses the internal controls (mAutopilotControls of type
   // AutopilotDataTypes::sAutopilotControls) to set the actual controls
   virtual void LoadControlDataWithAutopilotStabilityData() = 0;

   // This uses the internal controls (mAutopilotControls of type
   // AutopilotDataTypes::sAutopilotControls) to set the actual controls.
   // Uses LoadControlDataWithAutopilotControlData internally.
   virtual void SetControlDataWithAutopilotControls();

   // This uses the internal controls (mAutopilotControls of
   // type AutopilotDataTypes::sAutopilotControls) to set
   // the actual controls. Uses LoadControlDataWithAutopilotControlData
   // internally.
   virtual void SetControlDataWithControlAugmentationControls();

   // This uses the internal controls (mAutopilotControls of
   // type AutopilotDataTypes::sAutopilotControls) to modify
   // the actual controls. Uses LoadControlDataWithAutopilotControlData
   // internally.
   virtual void SetControlDataWithStabilityAugmentationControls();

   // Waypoint Functions
   // TODO -- These may be moved to the other derived pilot classes, as appropriate

   // This inherited function sets the location, orientation, and speed
   // of the Mover. It is called from SetPositionToPlannedWaypoint
   // and SetPositionToTempWaypoint.
   virtual void SetObjectState(double aLat,
                               double aLon,
                               double aAlt,
                               double aNEDVelN_mps,
                               double aNEDVelE_mps,
                               double aNEDVelD_mps,
                               double aLocalHeading_rad,
                               double aLocalPitch_rad,
                               double aLocalRoll_rad);

   // This copies the planned action, waypoints, and route data into the
   // temp action and waypoints. This includes mAutopilotActionPtr,
   // mPlannedPrevWptData, mPlannedCurrWptData, mPlannedNextWptData,
   // mPlannedRoute, and mCurrentPlannedWaypointIndex. It returns true
   // if the data was copied successfully.
   virtual bool ClonePlannedActionToTempAction();

   // This sets the temp action to hold the current altitude, heading,
   // and speed (ktas). If the temp action is null, it will also create it.
   virtual void SetTempActionToHoldCurrentConditions();

   // This creates a temp action (if null) and then sets the action to
   // hold the current altitude, heading, and speed (ktas). If the
   // temp action was not null, it will be left untouched.
   virtual void CreateTempActionIfNeeded();

   // This will leave the temp action untouched, if it already exists.
   // If the temp action is null, it uses the CopyPlannedActionToTempAction
   // to copy planned data to the temp data (if planned data exists) or
   // it uses SetTempActionToHoldCurrentConditions to set the new
   // temp action data to use the current conditions (alt, heading, speed).
   virtual void SetTempActionToPlannedDataOrCurrentConditions();

   // Control flags -----------------------------------------------------------------------
   // TODO -- As derived pilot classes as modified, the need for these control modes
   // may be eliminated

   // PilotObject uses the following "control flags" that denote the source of
   // control inputs. These include (listed in order of priority):
   //
   //    * mTestControl -- This flag is used for any type of testing or predictive
   //                      code. If true, this flag takes priority over all other flag
   //                      types. When active, control inputs are set through the
   //                      SetTestControlInputs() functions.
   //
   //    * EnableControls -- If false, this flag will cause all control inputs to go to
   //                        their default/neutral state.
   //
   //    * mAutopilotEnabled -- This flag indicates that the autopilot is controlling
   //                           the object. It is the second highest priority flag.
   //
   //    * mManualControl -- This flag indicates that control inputs are being
   //                        provided by a manual source, fed through the
   //                        SetManualControlData() functions.
   //
   //    * mExternalDirectControl -- This flag indicates that control inputs are being
   //                                provided by a script interface, providing direct
   //                                input to the manual pilot, fed through the
   //                                SetExternalDirectControlData() functions.
   //
   //    * mControlAugmentationModeActive -- If true, this indicates that the control
   //                                        inputs from mExternalManualControl are
   //                                        routed through the control augmentation
   //                                        inputs, rather than feeding the flight
   //                                        control system directly.
   //
   //    * mStabilityAugmentation[Yaw/Pitch/Roll]ModeActive -- Where true, this
   //                                 indicates that the control inputs from either
   //                                 manual or autopilot control are routed through
   //                                 stability augmentation inputs, rather than feeding
   //                                 the flight control system directly.

   // This returns true if the "testing" control mode is active.
   virtual bool ControlModeTestingActive() const;

   // This returns true if the "disabled controls" control mode is active. Note that, due
   // to control priorities, this may become false due to higher priority control modes
   // being active.
   virtual bool ControlModeDisabledActive() const;

   // This returns true if the autopilot is enabled and active. Note that, due to control
   // priorities, this may become false due to higher priority control modes being active.
   virtual bool ControlModeAutopilotActive() const;

   // This returns true if the "manual controls" control mode is active. Note that,
   // due to control priorities, this may become false due to higher priority control modes
   // being active.
   virtual bool ControlModeManualActive() const;

   // This returns true if the "external direct controls" control mode is active. Note that,
   // due to control priorities, this may become false due to higher priority control modes
   // being active.
   virtual bool ControlModeExternalDirectActive() const;

   // This returns true if the "Control Augmentation System" (CAS) is active. When active,
   // control inputs from mExternalManualControl are routed through the CAS inputs, rather
   // than feeding the flight control system directly.
   virtual bool ControlModeControlAugmentationActive() const;

   // This returns true if the "Stability Augmentation System" (SAS) is active. When active,
   // control inputs from mExternalManualControl are modified by the SAS inputs
   virtual bool ControlModeStabilityAugmentationActive() const;

   // Attributes ==========================================================================

   // This is essentially a copy of the kinematic state of the vehicle. It is passed to
   // the pilot object through the Update() function.

   int64_t mLastSimTime_nanosec = 0;

   // CommonController (autopilot) data ---------------------------------------------------

   CommonController::sAutopilotControls mAutopilotControls;

   UtCloneablePtr<AutopilotAction> mAutopilotActionPtr{nullptr};
   const Waypoint*                 mPlannedPrevWptDataPtr = nullptr;
   const Waypoint*                 mPlannedCurrWptDataPtr = nullptr;
   const Waypoint*                 mPlannedNextWptDataPtr = nullptr;
   UtCloneablePtr<const Route>     mPlannedRoutePtr{nullptr};
   size_t                          mCurrentPlannedWaypointIndex = -1;

   UtCloneablePtr<AutopilotAction> mTempActionPtr{nullptr};
   const Waypoint*                 mTempPrevWptDataPtr = nullptr;
   const Waypoint*                 mTempCurrWptDataPtr = nullptr;
   const Waypoint*                 mTempNextWptDataPtr = nullptr;
   UtCloneablePtr<Route>           mTempRoutePtr{nullptr};
   size_t                          mCurrentTempWaypointIndex = -1;

   Route mTransitionRoute;

   // Manual control input data  ----------------------------------------------------------

   // These are normalized control inputs, after having non-linear mapping effects applied,
   // that are used by the Control Augmentation System (CAS).
   double mControlAugmentationStickBack   = 0.0;
   double mControlAugmentationStickRight  = 0.0;
   double mControlAugmentationRudderRight = 0.0;

   // These provide a "factor" to convert normalized inputs in g-load or roll rate.
   double mPitchControlAugmentationFactor_g  = 10.0;
   double mYawControlAugmentationFactor_g    = 10.0;
   double mYawControlAugmentationFactor_deg  = 30.0;
   double mRollControlAugmentationFactor_dps = 180.0;

   // These provide a "factor" to convert scale trim commands based on vehicle response.
   // Fast-responding aircraft should have lower factor values than slow-responding
   // vehicles.
   double mPitchTrimFactor = 0.1;
   double mRollTrimFactor  = 0.1;
   double mYawTrimFactor   = 0.1;

   // These are trim control inputs and indicate how long a trim switch has been
   // actuated. This is an "integrated" level, so if a nose up trim was actuated for
   // 1 second, then a nose down trim was actuated for 1.5 seconds, and then nose up
   // trim was actuated for 0.3 seconds, the value would be -0.2 seconds, indicating
   // a nose down trim.
   double mNoseUpTrimDeltaT_sec    = 0.0;
   double mRollRightTrimDeltaT_sec = 0.0;
   double mYawRightTrimDeltaT_sec  = 0.0;

   // These are the current trim values
   double mTrimNoseUp    = 0.0;
   double mTrimRollRight = 0.0;
   double mTrimYawRight  = 0.0;

   // These curves modify the control input, providing non-liner mapping and making
   // manual control easier than liner control mapping.
   UtCloneablePtr<UtTable::Curve> mPitchControlMapping{nullptr};
   UtCloneablePtr<UtTable::Curve> mRollControlMapping{nullptr};
   UtCloneablePtr<UtTable::Curve> mYawControlMapping{nullptr};

   // Control override flags --------------------------------------------------------------
   bool mControlOverrideStickBack   = false;
   bool mControlOverrideStickRight  = false;
   bool mControlOverrideRudderRight = false;
   bool mControlOverrideThrottle    = false;
   bool mControlOverrideSpeedBrakes = false;

   // Mode control flags ------------------------------------------------------------------
   // TODO -- As derived pilot classes as modified, the need for these control modes
   // *may* be eliminated

   // This indicates that the CAS system is active. Note that, for full operation, the CAS
   // also requires that mAutopilotEnabled also be true.
   bool mControlAugmentationModeActive = false;

   // This indicates that the SAS system is active.
   bool mPitchStabilityAugmentationModeActive = false;
   bool mRollStabilityAugmentationModeActive  = false;
   bool mYawStabilityAugmentationModeActive   = false;

   // Mode control flags ------------------------------------------------------------------
   // TODO -- As derived pilot classes as modified, the need for these control modes
   // *may* be eliminated

   // This indicates that control is being performed through a link to a parent vehicle.
   bool mParentControl = false;

   // This indicates that manual/direct control is being performed through a manual pilot.
   bool mManualControl = false;

   // This indicates that manual/direct control is being performed through a synthetic
   // pilot.
   bool mExternalDirectControl = false;

   // This indicates if the common controller (autopilot) is active/enabled. Note that CAS
   // uses the "autopilot", so this must be true for the CAS to operate.
   bool mAutopilotEnabled = true;

   // This is a top-level flag that indicates whether the controls are enabled/disabled.
   // When false, all controls will use the nominal condition (typically zero).
   bool mControlsEnabled = true;

   // The test control flag supersedes and has priority over all other mode flags.
   bool mTestControl = false;

   // If this is true, control inputs are disabled
   bool mIsDestroyed = false;
};
} // namespace six_dof
} // namespace wsf

#endif
