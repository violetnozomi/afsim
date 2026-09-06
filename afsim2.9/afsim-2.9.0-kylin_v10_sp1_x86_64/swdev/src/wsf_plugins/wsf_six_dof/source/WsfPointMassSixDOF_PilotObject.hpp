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

#ifndef WSFPOINTMASSSIXDOFPILOTOBJECT_HPP
#define WSFPOINTMASSSIXDOFPILOTOBJECT_HPP

#include "WsfPointMassSixDOF_CommonController.hpp"
#include "WsfPointMassSixDOF_VehicleData.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_PilotObject.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PointMassPilotObject : public PilotObject
{
public:
   PointMassPilotObject()  = default;
   ~PointMassPilotObject() = default;

   PointMassPilotObject& operator=(const PointMassPilotObject& other) = delete;

   // In most instances, PilotObject children should override this function
   virtual bool ProcessInput(UtInput& aInput);

   // In most instances, PilotObject children should override this function
   virtual bool Initialize(int64_t aSimTime_nanosec);

   void ManageFrozenControls() override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "PointMassPilotObject"; }

   // Control input positions -------------------------------------------------------------

   // These functions return the current positions of specified control inputs

   // This returns the current stick back control input position
   double GetStickBackControllerPosition() const override;

   // This returns the current stick right control input position
   double GetStickRightControllerPosition() const override;

   // This returns the current rudder right control input position
   double GetRudderRightControllerPosition() const override;

   // This returns the current throttle control input position,
   // using IDLE=0, MIL=1.0, and AB=2.0
   double GetThrottleControllerPosition() const override;

   // This returns the current military (non-afterburner) throttle control
   // input position. For a combined military/afterburner throttle value,
   // use GetThrottleControllerPosition() instead.
   double GetThrottleMilitaryControllerPosition() const override;

   // This returns the current afterburner throttle control input
   // position. For a combined military/afterburner throttle value,
   // use GetThrottleControllerPosition() instead.
   double GetThrottleAfterburnerControllerPosition() const override;

   // This returns the current speed brakes control input position
   double GetSpeedBrakesControllerPosition() const override;

   // This returns the current flaps control input position
   double GetFlapsControllerPosition() const override;

   // This returns the current landing gear control input position
   virtual double GetLandingGearControllerPosition() const override;

   // This returns the current spoilers control input position
   double GetSpoilersControllerPosition() const override;

   // This returns the current thrust reverser control input position
   double GetThrustReverserControllerPosition() const override { return 0.0; }

   // Manual pilot functions --------------------------------------------------------------
   // TODO -- These will be moved to the manual pilot classes, as appropriate

   // These functions provide an interface for a ManualPilot controlled via
   // input from a crew station.

   // This allows external controls to be "injected" into the PointMassPilotObject.
   // This variant allows a reduced set of inputs, omitting rudder.
   // The aStickRightPosition and aStickAftPosition values are +/-1. The
   // aSpdBrakeLeverPosition, aSpoilerLeverPosition, and aFlapsLeverPosition
   // are 0-1. The aThrottleLeverPosition assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power
   // with full augmentation/afterburner (full afterburner).
   void SetManualControlData(double aStickRightPosition,
                             double aStickAftPosition,
                             double aThrottleLeverPosition,
                             double aSpdBrakeLeverPosition,
                             double aSpoilerLeverPosition,
                             double aFlapsLeverPosition) override;

   // This allows external controls to be "injected" into the PointMassPilotObject.
   // This variant includes rudder control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, and aFlapsLeverPosition are 0-1. The
   // aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner).
   void SetManualControlData(double aStickRightPosition,
                             double aStickAftPosition,
                             double aRudderRightPosition,
                             double aThrottleLeverPosition,
                             double aSpdBrakeLeverPosition,
                             double aSpoilerLeverPosition,
                             double aFlapsLeverPosition) override;

   // This allows external controls to be "injected" into the PointMassPilotObject.
   // This variant adds landing gear control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, aFlapsLeverPosition, and aLandingGearLeverPosition are 0-1.
   // The aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner). The aLandingGearLeverPosition assumes
   // that 0 is gear up and 1 is gear full down and locked.
   void SetManualControlData(double aStickRightPosition,
                             double aStickAftPosition,
                             double aRudderRightPosition,
                             double aThrottleLeverPosition,
                             double aSpdBrakeLeverPosition,
                             double aSpoilerLeverPosition,
                             double aFlapsLeverPosition,
                             double aLandingGearLeverPosition) override;

   // This allows external controls to be "injected" into the PointMassPilotObject.
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
   void SetManualControlData(double aStickRightPosition,
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
                             bool   aNWS_Enabled) override;

   // Synthetic pilot functions -----------------------------------------------------------
   // TODO -- These will be moved to the synthetic pilot class, as appropriate

   // These functions provide an interface for a SyntheticPilot controlled via
   // script commands

   // This allows external controls to be "injected" into the PointMassPilotObject.
   // This variant allows a reduced set of inputs, omitting rudder.
   // The aStickRightPosition and aStickAftPosition values are +/-1. The
   // aSpdBrakeLeverPosition, aSpoilerLeverPosition, and aFlapsLeverPosition
   // are 0-1. The aThrottleLeverPosition assumes that 0 is idle, 1 is full power
   // without augmentation/afterburner (military power), and 2 is full power
   // with full augmentation/afterburner (full afterburner).
   void SetExternalDirectControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aThrottleLeverPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition) override;

   // This allows external controls to be "injected" into the PointMassPilotObject.
   // This variant includes rudder control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, and aFlapsLeverPosition are 0-1. The
   // aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner).
   void SetExternalDirectControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aRudderRightPosition,
                                     double aThrottleLeverPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition) override;

   // This allows external controls to be "injected" into the PointMassPilotObject.
   // This variant adds landing gear control. The aStickRightPosition, aStickAftPosition
   // and aRudderRightPosition values are +/-1. The aSpdBrakeLeverPosition,
   // aSpoilerLeverPosition, aFlapsLeverPosition, and aLandingGearLeverPosition are 0-1.
   // The aThrottleLeverPosition assumes that 0 is idle, 1 is full power without
   // augmentation/afterburner (military power), and 2 is full power with full
   // augmentation/afterburner (full afterburner). The aLandingGearLeverPosition assumes
   // that 0 is gear up and 1 is gear full down and locked.
   void SetExternalDirectControlData(double aStickRightPosition,
                                     double aStickAftPosition,
                                     double aRudderRightPosition,
                                     double aThrottleLeverPosition,
                                     double aSpdBrakeLeverPosition,
                                     double aSpoilerLeverPosition,
                                     double aFlapsLeverPosition,
                                     double aLandingGearLeverPosition) override;

   // This allows external controls to be "injected" into the PointMassPilotObject.
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
   void SetExternalDirectControlData(double aStickRightPosition,
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
                                     bool   aNWS_Enabled) override;

   // Testing support functions -----------------------------------------------------------

   // Testing Support -- These functions are used when testing. Note that they are
   // inherited, and are not overridable, since all PilotObjects support "testing".

   // This next group of functions are typically used to set controls for testing
   // and "calculation" functions.

   // Sets the stick right control:  -1=Full Forward (nose down), 0=Neutral,
   // 1=Full Aft (nose up)
   void SetTestStickBackControllerPosition(double aStickAftPosition) override;

   // Sets the stick right control:  -1=Full Left, 0=Neutral, 1=Full Right
   void SetTestStickRightControllerPosition(double aStickRightPosition) override;

   // Sets the rudder right control:  -1=Full Left, 0=Neutral, 1=Full Right
   void SetTestRudderRightControllerPosition(double aRudderRightPosition) override;

   // Sets the speed brake control:  0=Closed, 1=Open
   void SetTestSpeedBrakesControllerPosition(double aSpeedBrakesPosition) override;

   // Sets the flaps control:  0=Retracted, 1=Fully Extended
   void SetTestFlapsControllerPosition(double aFlapsPosition) override;

   // Sets the spoilers control:  0=Retracted, 1=Fully Extended
   void SetTestSpoilersControllerPosition(double aSpoilersPosition) override;

   // Sets the landing gear control:  0=Retracted, 1=Fully Extended
   void SetTestLandingGearControllerPosition(double aLandingGearPosition) override;

   // These functions provide the ability to "preposition" the vehicle to a
   // specified altitude and speed during testing. Note that these should only be
   // used in testing, since they directly modify the speed/altitude.

   // -------------------------------------------------------------------------------------

   // The CommonController (autopilot) typically controls the throttle and speed brakes
   // as part of its speed control. However, these can be overridden with direct commands.

   PointMassCommonController* GetPointMassCommonController() const { return mCommonController.get(); }
   CommonController*          GetCommonController() const override { return mCommonController.get(); }

   // Stick and rudder position are normally controlled by the autopilot, but can be
   // overridden using these direct input functions.

   void SetDirectStickBackInput(double aStickBackPosition) override;
   void SetDirectStickRightInput(double aStickBackPosition) override;
   void SetDirectRudderRightInput(double aStickBackPosition) override;

   // Speed brakes are normally controlled by the autopilot, but they can be overridden
   // using these direct input functions.

   void OpenSpeedBrake() override;
   void CloseSpeedBrake() override;
   void SetDirectSpeedBrakesInput(double aValue) override;

   // -------------------------------------------------------------------------------------

   // These functions provide control inputs for landing gear, flaps, spoilers,
   // thrust reversers, and nose wheel steering (NWS)

   void SetLandingGearControlPosition(double aPosition) override;
   void SetFlapsControlPosition(double aPosition) override;
   void SetSpoilersControlPosition(double aPosition) override;
   void SetThrustReverserControlPosition(double aPosition) override;

   // -------------------------------------------------------------------------------------

protected:
   PointMassPilotObject(const PointMassPilotObject& aSrc);

   // Inherited PilotObject Functions -- These functions are not overridden;
   // child PilotObjects will simply inherit these internal functions.

   // This "zeros" control inputs if controls are disabled
   void ZeroDisabledControlData() override;

   // This updates the active pilot object with the commands from the parent vehicle
   void InheritParentControlData() override;

   // SetThrottleData assumes that 1 is full, unaugmented power (military power)
   // and 2.0 is full afterburner. This function is called (directly or indirectly)
   // by all variants of the SetControlData functions.
   void SetThrottleData(double aThrottleLeverPosition) override;

   // This sets the current military (non-afterburner) throttle control input position
   void SetThrottleMilitaryData(double aThrottleLeverPosition) override;

   // This sets the current afterburner throttle control input position
   void SetThrottleAfterburnerData(double aThrottleLeverPosition) override;

   void EnforceControlLimits() override;

   // *** THIS SHOULD BE OVERRIDDEN BY EACH PILOT OBJECT ***
   // This uses the internal controls (mAutopilotControls of type
   // PointMassSixDOF_AutopilotDataTypes::sAutopilotControls) to set the actual controls
   void LoadControlDataWithAutopilotControlData() override;

   // *** THIS SHOULD BE OVERRIDDEN BY EACH PILOT OBJECT ***
   // This uses the internal controls (mAutopilotControls of type
   // PointMassSixDOF_AutopilotDataTypes::sAutopilotControls) to set the actual controls
   void LoadControlDataWithAutopilotStabilityData() override;

   UtCloneablePtr<PointMassCommonController> mCommonController{nullptr};

   float mStdStickBack      = 0.0f;
   float mStdStickRight     = 0.0f;
   float mStdRudderRight    = 0.0f;
   float mStdThrottleMil    = 0.0f;
   float mStdThrottleAB     = 0.0f;
   float mStdSpeedBrakesOut = 0.0f;
   float mStdFlapsDown      = 0.0f;
   float mStdSpoilersOut    = 0.0f;
};
} // namespace six_dof
} // namespace wsf

#endif
