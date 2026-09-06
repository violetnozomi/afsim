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

#ifndef WSFRIGIDBODYSIXDOFPILOTOBJECT_HPP
#define WSFRIGIDBODYSIXDOFPILOTOBJECT_HPP

#include "WsfRigidBodySixDOF_CommonController.hpp"
#include "WsfRigidBodySixDOF_VehicleData.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_PilotObject.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RigidBodyPilotObject : public PilotObject
{
public:
   RigidBodyPilotObject()  = default;
   ~RigidBodyPilotObject() = default;

   RigidBodyPilotObject& operator=(const RigidBodyPilotObject& other) = delete;

   // In most instances, PilotObject children should override this function
   virtual bool ProcessInput(UtInput& aInput);

   // In most instances, PilotObject children should override this function
   virtual bool Initialize(int64_t aSimTime_nanosec);

   void ManageFrozenControls() override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "RigidBodySixDOF_Object"; }

   // Flight control system functions -----------------------------------------------------

   // FlightControlSystem Functions -- These functions (which are inherited,
   // and not overridable) are used by the FlightControlSystem to get values
   // of control inputs.

   // This returns a 'handle' to a control. If there is no control with the
   // specified name, the function returns 0;
   size_t GetControlHandle(const std::string& aControlName) const;

   // These functions get handles to "standard" control inputs
   size_t GetStdStickBackHandle() const { return mStdStickBackHandle; }
   size_t GetStdStickRightHandle() const { return mStdStickRightHandle; }
   size_t GetStdRudderRightHandle() const { return mStdRudderRightHandle; }
   size_t GetStdThrottleMilHandle() const { return mStdThrottleMilHandle; }
   size_t GetStdThrottleABHandle() const { return mStdThrottleABHandle; }
   size_t GetStdThrustReverserHandle() const { return mStdThrustReverserHandle; }
   size_t GetStdThrustVectoringYawHandle() const { return mStdThrustVectoringYawHandle; }
   size_t GetStdThrustVectoringPitchHandle() const { return mStdThrustVectoringPitchHandle; }
   size_t GetStdThrustVectoringRollHandle() const { return mStdThrustVectoringRollHandle; }
   size_t GetStdSpeedBrakesOutHandle() const { return mStdSpeedBrakesOutHandle; }
   size_t GetStdFlapsDownHandle() const { return mStdFlapsDownHandle; }
   size_t GetStdSpoilersOutHandle() const { return mStdSpoilersOutHandle; }
   size_t GetStdLandingGearDownHandle() const { return mStdLandingGearDownHandle; }
   size_t GetStdNoseWheelSteeringHandle() const { return mStdNoseWheelSteeringHandle; }
   size_t GetStdNWS_SteeringHandle() const { return mStdNWS_SteeringHandle; }
   size_t GetStdNWS_EnabledHandle() const { return mStdNWS_EnabledHandle; }
   size_t GetStdWheelBrakeLeftHandle() const { return mStdWheelBrakeLeftHandle; }
   size_t GetStdWheelBrakeRightHandle() const { return mStdWheelBrakeRightHandle; }

   // This returns the normalized value of the control input, given its handle.
   // If the input is a boolean input, it returns 0.0 for false and 1.0 for true.
   double ControlInputValue(size_t aControlInputHandle) const;

   // This returns the boolean value of the control input, given its handle.
   // If the input is not a boolean input, it returns true if the value is
   // greater than 0.5.
   bool ControlInputValueBool(size_t aControlInputHandle) const;

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
   double GetLandingGearControllerPosition() const override;

   // This returns the current spoilers control input position
   double GetSpoilersControllerPosition() const override;

   // This returns the current thrust reverser control input position
   double GetThrustReverserControllerPosition() const override;

   // Manual pilot functions --------------------------------------------------------------
   // TODO -- These will be moved to the manual pilot classes, as appropriate

   // These functions provide an interface for a ManualPilot controlled via
   // input from a crew station.

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // This allows external controls to be "injected" into the RigidBodyPilotObject.
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

   // -------------------------------------------------------------------------------------

   // The CommonController (autopilot) typically controls the throttle and speed brakes
   // as part of its speed control. However, these can be overridden with direct commands.

   RigidBodyCommonController* GetRigidBodyCommonController() const { return mCommonController.get(); }
   CommonController*          GetCommonController() const override { return mCommonController.get(); }

   CommonController::sAutopilotControls* GetCurrentControlLimits() { return &mAutopilotControls; }

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

   // Wheel brakes are normally controlled by the autopilot but can be overridden
   // using these functions that provide external control
   void EnableDirectBraking();
   void SetParkingBrake();
   void ReleaseParkingBrake();
   void ApplyLeftGearBrake(double aValue);
   void ApplyRightGearBrake(double aValue);
   void ReleaseDirectBraking();

   // -------------------------------------------------------------------------------------

   // These functions provide control inputs for landing gear, flaps, spoilers,
   // thrust reversers, and nose wheel steering (NWS)

   void SetLandingGearControlPosition(double aPosition) override;
   void SetFlapsControlPosition(double aPosition) override;
   void SetSpoilersControlPosition(double aPosition) override;
   void SetThrustReverserControlPosition(double aPosition) override;
   void SetEnableNWS(bool aNWS_Enabled);

protected:
   RigidBodyPilotObject(const RigidBodyPilotObject& aSrc);

   // This is used to match-up control inputs and outputs
   struct InputOutputPair
   {
      explicit InputOutputPair(const std::string& aOutputName)
         : outputName(aOutputName)
      {
      }

      std::string              outputName;
      FlightControlInputValue* controlInput = nullptr;
      bool                     copiedInput  = false;
   };

   std::unique_ptr<InputOutputPair> CopyStandardControlInput(InputOutputPair* aSourceData);

   // Inherited PilotObject Functions -- These functions are not overridden;
   // child PilotObjects will simply inherit these internal functions.

   // This "zeros" control inputs if controls are disabled
   void ZeroDisabledControlData() override;

   // This updates the active pilot object with the commands from the parent vehicle
   void InheritParentControlData() override;

   // This is called during Initialize to match-up "standard" control inputs and outputs
   void MatchStdInputsAndOutputs();

   // The CommonController (autopilot) typically controls the throttle and speed brakes
   // as part of its speed control. However, these can be overridden with direct commands.

   // SetThrottleData assumes that 1 is full, unaugmented power (military power)
   // and 2.0 is full afterburner. This function is called (directly or indirectly)
   // by all variants of the SetControlData functions.
   void SetThrottleData(double aThrottleLeverPosition) override;

   // This sets the current military (non-afterburner) throttle control input position
   void SetThrottleMilitaryData(double aThrottleLeverPosition) override;

   // This sets the current afterburner throttle control input position
   void SetThrottleAfterburnerData(double aThrottleLeverPosition) override;

   virtual bool MatchInputOutput(InputOutputPair* aInputOutput);

   void EnforceControlLimits() override;

   // *** THIS SHOULD BE OVERRIDDEN BY EACH PILOT OBJECT ***
   // This uses the internal controls (mAutopilotControls of type
   // RigidBodySixDOF_AutopilotDataTypes::sAutopilotControls) to set the actual controls
   void LoadControlDataWithAutopilotControlData() override;

   // *** THIS SHOULD BE OVERRIDDEN BY EACH PILOT OBJECT ***
   // This uses the internal controls (mAutopilotControls of type
   // RigidBodySixDOF_AutopilotDataTypes::sAutopilotControls) to set the actual controls
   void LoadControlDataWithAutopilotStabilityData() override;

   // CommonController (autopilot) data ---------------------------------------------------

   UtCloneablePtr<RigidBodyCommonController> mCommonController{nullptr};

   // This is a list/vector containing all of the control inputs
   std::vector<FlightControlInputValue> mControlsList;

   // These are structures that provide "standard" (std) control inputs.

   std::unique_ptr<InputOutputPair> mStdStickBackPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdStickRightPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdRudderRightPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdThrottleMilPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdThrottleABPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdThrustReverserPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdThrustVectoringYawPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdThrustVectoringPitchPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdThrustVectoringRollPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdSpeedBrakesOutPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdFlapsDownPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdSpoilersOutPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdLandingGearDownPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdNoseWheelSteeringPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdNWS_SteeringPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdNWS_EnabledPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdWheelBrakeLeftPtr{nullptr};
   std::unique_ptr<InputOutputPair> mStdWheelBrakeRightPtr{nullptr};

   // These are "handles" to the standard control inputs

   size_t mStdStickBackHandle            = 0;
   size_t mStdStickRightHandle           = 0;
   size_t mStdRudderRightHandle          = 0;
   size_t mStdThrottleMilHandle          = 0;
   size_t mStdThrottleABHandle           = 0;
   size_t mStdThrustReverserHandle       = 0;
   size_t mStdThrustVectoringYawHandle   = 0;
   size_t mStdThrustVectoringPitchHandle = 0;
   size_t mStdThrustVectoringRollHandle  = 0;
   size_t mStdSpeedBrakesOutHandle       = 0;
   size_t mStdFlapsDownHandle            = 0;
   size_t mStdSpoilersOutHandle          = 0;
   size_t mStdLandingGearDownHandle      = 0;
   size_t mStdNoseWheelSteeringHandle    = 0;
   size_t mStdNWS_SteeringHandle         = 0;
   size_t mStdNWS_EnabledHandle          = 0;
   size_t mStdWheelBrakeLeftHandle       = 0;
   size_t mStdWheelBrakeRightHandle      = 0;

   // Manual control input data  ----------------------------------------------------------
   // Control override flags --------------------------------------------------------------
   bool mControlOverrideWheelBrakes = false;
};
} // namespace six_dof
} // namespace wsf

#endif
