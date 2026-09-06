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

#include "P6DofPilotObject.hpp"

#include <cstring>

#include "P6DofAtmosphere.hpp"
#include "P6DofFlightControlSystem.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTypeManager.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicle.hpp"
#include "UtException.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"

P6DofPilotObject::P6DofPilotObject(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mLastSimTime_nanosec(0)
   , mStdStickBackPtr(nullptr)
   , mStdStickRightPtr(nullptr)
   , mStdRudderRightPtr(nullptr)
   , mStdThrottleMilPtr(nullptr)
   , mStdThrottleABPtr(nullptr)
   , mStdThrustReverserPtr(nullptr)
   , mStdThrustVectoringYawPtr(nullptr)
   , mStdThrustVectoringPitchPtr(nullptr)
   , mStdThrustVectoringRollPtr(nullptr)
   , mStdSpeedBrakesOutPtr(nullptr)
   , mStdFlapsDownPtr(nullptr)
   , mStdSpoilersOutPtr(nullptr)
   , mStdLandingGearDownPtr(nullptr)
   , mStdNoseWheelSteeringPtr(nullptr)
   , mStdNWS_SteeringPtr(nullptr)
   , mStdNWS_EnabledPtr(nullptr)
   , mStdWheelBrakeLeftPtr(nullptr)
   , mStdWheelBrakeRightPtr(nullptr)
   , mStdStickBackHandle(size_t(0))
   , mStdStickRightHandle(size_t(0))
   , mStdRudderRightHandle(size_t(0))
   , mStdThrottleMilHandle(size_t(0))
   , mStdThrottleABHandle(size_t(0))
   , mStdThrustReverserHandle(size_t(0))
   , mStdThrustVectoringYawHandle(size_t(0))
   , mStdThrustVectoringPitchHandle(size_t(0))
   , mStdThrustVectoringRollHandle(size_t(0))
   , mStdSpeedBrakesOutHandle(size_t(0))
   , mStdFlapsDownHandle(size_t(0))
   , mStdSpoilersOutHandle(size_t(0))
   , mStdLandingGearDownHandle(size_t(0))
   , mStdNoseWheelSteeringHandle(size_t(0))
   , mStdNWS_SteeringHandle(size_t(0))
   , mStdNWS_EnabledHandle(size_t(0))
   , mStdWheelBrakeLeftHandle(size_t(0))
   , mStdWheelBrakeRightHandle(size_t(0))
   , mCommonController(nullptr)
   , mAutopilotActionPtr(nullptr)
   , mPlannedPrevWptDataPtr(nullptr)
   , mPlannedCurrWptDataPtr(nullptr)
   , mPlannedNextWptDataPtr(nullptr)
   , mPlannedRoutePtr(nullptr)
   , mCurrentPlannedWaypointIndex(size_t(-1))
   , mTempActionPtr(nullptr)
   , mTempPrevWptDataPtr(nullptr)
   , mTempCurrWptDataPtr(nullptr)
   , mTempNextWptDataPtr(nullptr)
   , mTempRoutePtr(nullptr)
   , mCurrentTempWaypointIndex(size_t(-1))
   , mControlAugmentationStickBack(0)
   , mControlAugmentationStickRight(0)
   , mControlAugmentationRudderRight(0)
   , mPitchControlAugmentationFactor_g(10.0)
   , mRollControlAugmentationFactor_dps(360.0)
   , mPitchTrimFactor(0.1)
   , mRollTrimFactor(0.1)
   , mYawTrimFactor(0.1)
   , mNoseUpTrimDeltaT_sec(0.0)
   , mRollRightTrimDeltaT_sec(0.0)
   , mYawRightTrimDeltaT_sec(0.0)
   , mTrimNoseUp(0.0)
   , mTrimRollRight(0.0)
   , mTrimYawRight(0.0)
   , mPitchControlMapping(nullptr)
   , mRollControlMapping(nullptr)
   , mYawControlMapping(nullptr)
   , mControlOverrideThrottle(false)
   , mControlOverrideSpeedBrakes(false)
   , mControlOverrideWheelBrakes(false)
   , mManualControl(false)
   , mExternalDirectControl(false)
   , mPitchStabilityAugmentationModeActive(false)
   , mYawStabilityAugmentationModeActive(false)
   , mRollStabilityAugmentationModeActive(false)
   , mControlAugmentationModeActive(false)
   , mAutopilotEnabled(true)
   , mControlsEnabled(true)
   , mTestControl(false)
   , mIsDestroyed(false)
{
   // Initialize autopilot controls
   mAutopilotControls.stickBack           = 0;
   mAutopilotControls.stickRight          = 0;
   mAutopilotControls.rudderRight         = 0;
   mAutopilotControls.throttleMilitary    = 0;
   mAutopilotControls.throttleAfterburner = 0;
   mAutopilotControls.thrustVectorYaw     = 0;
   mAutopilotControls.thrustVectorPitch   = 0;
   mAutopilotControls.thrustVectorRoll    = 0;
   mAutopilotControls.speedBrake          = 0;
   mAutopilotControls.noseWheelSteering   = 0;
   mAutopilotControls.nwsSteering         = 0;
   mAutopilotControls.wheelBrakeLeft      = 0;
   mAutopilotControls.wheelBrakeRight     = 0;
}

P6DofPilotObject::~P6DofPilotObject()
{
   mControlsList.clear();

   DeleteInputOutputPair(mStdStickBackPtr);
   DeleteInputOutputPair(mStdStickRightPtr);
   DeleteInputOutputPair(mStdRudderRightPtr);
   DeleteInputOutputPair(mStdThrottleMilPtr);
   DeleteInputOutputPair(mStdThrottleABPtr);
   DeleteInputOutputPair(mStdThrustReverserPtr);
   DeleteInputOutputPair(mStdThrustVectoringYawPtr);
   DeleteInputOutputPair(mStdThrustVectoringPitchPtr);
   DeleteInputOutputPair(mStdThrustVectoringRollPtr);
   DeleteInputOutputPair(mStdSpeedBrakesOutPtr);
   DeleteInputOutputPair(mStdFlapsDownPtr);
   DeleteInputOutputPair(mStdSpoilersOutPtr);
   DeleteInputOutputPair(mStdLandingGearDownPtr);
   DeleteInputOutputPair(mStdNoseWheelSteeringPtr);
   DeleteInputOutputPair(mStdNWS_SteeringPtr);
   DeleteInputOutputPair(mStdNWS_EnabledPtr);
   DeleteInputOutputPair(mStdWheelBrakeLeftPtr);
   DeleteInputOutputPair(mStdWheelBrakeRightPtr);

   if (mCommonController)
   {
      delete mCommonController;
   }
   if (mAutopilotActionPtr)
   {
      delete mAutopilotActionPtr;
   }
   if (mTempActionPtr)
   {
      delete mTempActionPtr;
   }
   if (mPlannedRoutePtr)
   {
      delete mPlannedRoutePtr;
   }
   if (mTempRoutePtr)
   {
      delete mTempRoutePtr;
   }
}

P6DofPilotObject* P6DofPilotObject::Clone() const
{
   return new P6DofPilotObject(*this);
}

P6DofPilotObject::P6DofPilotObject(const P6DofPilotObject& aSrc)
   : P6DofObject(aSrc)
   , WaypointAchieved()
   , mState(aSrc.mState)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mControlsList(aSrc.mControlsList)
   , mStdStickBackPtr(nullptr)
   , mStdStickRightPtr(nullptr)
   , mStdRudderRightPtr(nullptr)
   , mStdThrottleMilPtr(nullptr)
   , mStdThrottleABPtr(nullptr)
   , mStdThrustReverserPtr(nullptr)
   , mStdThrustVectoringYawPtr(nullptr)
   , mStdThrustVectoringPitchPtr(nullptr)
   , mStdThrustVectoringRollPtr(nullptr)
   , mStdSpeedBrakesOutPtr(nullptr)
   , mStdFlapsDownPtr(nullptr)
   , mStdSpoilersOutPtr(nullptr)
   , mStdLandingGearDownPtr(nullptr)
   , mStdNoseWheelSteeringPtr(nullptr)
   , mStdNWS_SteeringPtr(nullptr)
   , mStdNWS_EnabledPtr(nullptr)
   , mStdWheelBrakeLeftPtr(nullptr)
   , mStdWheelBrakeRightPtr(nullptr)
   , mStdStickBackHandle(size_t(0))
   , mStdStickRightHandle(size_t(0))
   , mStdRudderRightHandle(size_t(0))
   , mStdThrottleMilHandle(size_t(0))
   , mStdThrottleABHandle(size_t(0))
   , mStdThrustReverserHandle(size_t(0))
   , mStdThrustVectoringYawHandle(size_t(0))
   , mStdThrustVectoringPitchHandle(size_t(0))
   , mStdThrustVectoringRollHandle(size_t(0))
   , mStdSpeedBrakesOutHandle(size_t(0))
   , mStdFlapsDownHandle(size_t(0))
   , mStdSpoilersOutHandle(size_t(0))
   , mStdLandingGearDownHandle(size_t(0))
   , mStdNoseWheelSteeringHandle(size_t(0))
   , mStdNWS_SteeringHandle(size_t(0))
   , mStdNWS_EnabledHandle(size_t(0))
   , mStdWheelBrakeLeftHandle(size_t(0))
   , mStdWheelBrakeRightHandle(size_t(0))
   , mCommonController(nullptr)
   , mAutopilotActionPtr(nullptr)
   , mPlannedPrevWptDataPtr(nullptr)
   , mPlannedCurrWptDataPtr(nullptr)
   , mPlannedNextWptDataPtr(nullptr)
   , mPlannedRoutePtr(nullptr)
   , mCurrentPlannedWaypointIndex(aSrc.mCurrentPlannedWaypointIndex)
   , mTempActionPtr(nullptr)
   , mTempPrevWptDataPtr(nullptr)
   , mTempCurrWptDataPtr(nullptr)
   , mTempNextWptDataPtr(nullptr)
   , mTempRoutePtr(nullptr)
   , mCurrentTempWaypointIndex(aSrc.mCurrentTempWaypointIndex)
   , mControlAugmentationStickBack(aSrc.mControlAugmentationStickBack)
   , mControlAugmentationStickRight(aSrc.mControlAugmentationStickRight)
   , mControlAugmentationRudderRight(aSrc.mControlAugmentationRudderRight)
   , mPitchControlAugmentationFactor_g(aSrc.mPitchControlAugmentationFactor_g)
   , mRollControlAugmentationFactor_dps(aSrc.mRollControlAugmentationFactor_dps)
   , mPitchTrimFactor(aSrc.mPitchTrimFactor)
   , mRollTrimFactor(aSrc.mRollTrimFactor)
   , mYawTrimFactor(aSrc.mYawTrimFactor)
   , mNoseUpTrimDeltaT_sec(aSrc.mNoseUpTrimDeltaT_sec)
   , mRollRightTrimDeltaT_sec(aSrc.mRollRightTrimDeltaT_sec)
   , mYawRightTrimDeltaT_sec(aSrc.mYawRightTrimDeltaT_sec)
   , mTrimNoseUp(aSrc.mTrimNoseUp)
   , mTrimRollRight(aSrc.mTrimRollRight)
   , mTrimYawRight(aSrc.mTrimYawRight)
   , mPitchControlMapping(aSrc.mPitchControlMapping)
   , mRollControlMapping(aSrc.mRollControlMapping)
   , mYawControlMapping(aSrc.mYawControlMapping)
   , mControlOverrideThrottle(aSrc.mControlOverrideThrottle)
   , mControlOverrideSpeedBrakes(aSrc.mControlOverrideSpeedBrakes)
   , mControlOverrideWheelBrakes(aSrc.mControlOverrideWheelBrakes)
   , mManualControl(aSrc.mManualControl)
   , mExternalDirectControl(aSrc.mExternalDirectControl)
   , mPitchStabilityAugmentationModeActive(aSrc.mPitchStabilityAugmentationModeActive)
   , mYawStabilityAugmentationModeActive(aSrc.mYawStabilityAugmentationModeActive)
   , mRollStabilityAugmentationModeActive(aSrc.mRollStabilityAugmentationModeActive)
   , mControlAugmentationModeActive(aSrc.mControlAugmentationModeActive)
   , mAutopilotEnabled(aSrc.mAutopilotEnabled)
   , mControlsEnabled(aSrc.mControlsEnabled)
   , mTestControl(aSrc.mTestControl)
   , mIsDestroyed(aSrc.mIsDestroyed)
{
   mAutopilotControls = aSrc.mAutopilotControls;

   CopyStandardControlInput(aSrc.mStdStickBackPtr, mStdStickBackPtr);
   CopyStandardControlInput(aSrc.mStdStickRightPtr, mStdStickRightPtr);
   CopyStandardControlInput(aSrc.mStdRudderRightPtr, mStdRudderRightPtr);
   CopyStandardControlInput(aSrc.mStdThrottleMilPtr, mStdThrottleMilPtr);
   CopyStandardControlInput(aSrc.mStdThrottleABPtr, mStdThrottleABPtr);
   CopyStandardControlInput(aSrc.mStdThrustReverserPtr, mStdThrustReverserPtr);
   CopyStandardControlInput(aSrc.mStdThrustVectoringYawPtr, mStdThrustVectoringYawPtr);
   CopyStandardControlInput(aSrc.mStdThrustVectoringPitchPtr, mStdThrustVectoringPitchPtr);
   CopyStandardControlInput(aSrc.mStdThrustVectoringRollPtr, mStdThrustVectoringRollPtr);
   CopyStandardControlInput(aSrc.mStdSpeedBrakesOutPtr, mStdSpeedBrakesOutPtr);
   CopyStandardControlInput(aSrc.mStdFlapsDownPtr, mStdFlapsDownPtr);
   CopyStandardControlInput(aSrc.mStdSpoilersOutPtr, mStdSpoilersOutPtr);
   CopyStandardControlInput(aSrc.mStdLandingGearDownPtr, mStdLandingGearDownPtr);
   CopyStandardControlInput(aSrc.mStdNoseWheelSteeringPtr, mStdNoseWheelSteeringPtr);
   CopyStandardControlInput(aSrc.mStdNWS_SteeringPtr, mStdNWS_SteeringPtr);
   CopyStandardControlInput(aSrc.mStdNWS_EnabledPtr, mStdNWS_EnabledPtr);
   CopyStandardControlInput(aSrc.mStdWheelBrakeLeftPtr, mStdWheelBrakeLeftPtr);
   CopyStandardControlInput(aSrc.mStdWheelBrakeRightPtr, mStdWheelBrakeRightPtr);

   if (aSrc.mCommonController)
   {
      mCommonController = aSrc.mCommonController->Clone();

      // Set the CAS and SAS modes
      mCommonController->SetControlAugmentationModeActive(mControlAugmentationModeActive);
      mCommonController->SetPitchStabilityAugmentationModeActive(mPitchStabilityAugmentationModeActive);
      mCommonController->SetYawStabilityAugmentationModeActive(mYawStabilityAugmentationModeActive);
      mCommonController->SetRollStabilityAugmentationModeActive(mRollStabilityAugmentationModeActive);
   }

   if (aSrc.mAutopilotActionPtr != nullptr)
   {
      // Clone from source
      mAutopilotActionPtr = aSrc.mAutopilotActionPtr->Clone();
   }

   if (aSrc.mTempActionPtr != nullptr)
   {
      // Clone from source
      mTempActionPtr = aSrc.mTempActionPtr->Clone();
   }

   if (aSrc.mPlannedRoutePtr != nullptr)
   {
      mPlannedRoutePtr = aSrc.mPlannedRoutePtr->Clone();

      if (aSrc.mPlannedPrevWptDataPtr != nullptr)
      {
         size_t index           = aSrc.mPlannedRoutePtr->GetWaypointIndex(aSrc.mPlannedPrevWptDataPtr);
         mPlannedPrevWptDataPtr = mPlannedRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mPlannedCurrWptDataPtr != nullptr)
      {
         size_t index           = aSrc.mPlannedRoutePtr->GetWaypointIndex(aSrc.mPlannedCurrWptDataPtr);
         mPlannedCurrWptDataPtr = mPlannedRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mPlannedNextWptDataPtr != nullptr)
      {
         size_t index           = aSrc.mPlannedRoutePtr->GetWaypointIndex(aSrc.mPlannedNextWptDataPtr);
         mPlannedNextWptDataPtr = mPlannedRoutePtr->GetWaypointAtIndex(index);
      }
   }

   if (aSrc.mTempRoutePtr != nullptr)
   {
      mTempRoutePtr = aSrc.mTempRoutePtr->Clone();

      if (aSrc.mTempPrevWptDataPtr != nullptr)
      {
         size_t index        = aSrc.mTempRoutePtr->GetWaypointIndex(aSrc.mTempPrevWptDataPtr);
         mTempPrevWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mTempCurrWptDataPtr != nullptr)
      {
         size_t index        = aSrc.mTempRoutePtr->GetWaypointIndex(aSrc.mTempCurrWptDataPtr);
         mTempCurrWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mTempNextWptDataPtr != nullptr)
      {
         size_t index        = aSrc.mTempRoutePtr->GetWaypointIndex(aSrc.mTempNextWptDataPtr);
         mTempNextWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }
   }
}

void P6DofPilotObject::DeleteInputOutputPair(sInputOutputPair*& aPair)
{
   if (aPair != nullptr)
   {
      delete aPair;
   }
}

void P6DofPilotObject::CopyStandardControlInput(sInputOutputPair* aSourceData, sInputOutputPair*& aTargetData)
{
   if (aSourceData != nullptr)
   {
      // Create a new input/output pair
      aTargetData = new sInputOutputPair(aSourceData->outputName);

      // Does the source have a control input?
      if (aSourceData->controlInput != nullptr)
      {
         // We need to find our own control input with the same name as the source control input
         size_t handle = GetControlHandle(aSourceData->controlInput->controlInputName);
         if (handle > 0)
         {
            // We have a match, so set the input to point to our control input
            aTargetData->controlInput = &(mControlsList.at(handle - 1));

            // This is "copied" input
            aTargetData->copiedInput = true;
         }
         else
         {
            // No match -- This should not occur
            throw UtException("Unable to match control input");
         }
      }
   }
}

bool P6DofPilotObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_inputs")
   {
      UtInputBlock block(aInput, "end_control_inputs");

      // Clear any current items
      mControlsList.clear();

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_name")
         {
            std::string controlName;
            aInput.ReadValue(controlName);

            P6DOF::FlightControlInputValue controlValue;
            controlValue.controlInputName = controlName;
            controlValue.boolData         = false;
            controlValue.normalizedValue  = 0;
            controlValue.typeData         = P6DOF::DataType::Float;
            mControlsList.push_back(controlValue);
         }
         else if (localCommand == "std_stick_back")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdStickBackPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_stick_right")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdStickRightPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_rudder_right")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdRudderRightPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_throttle_mil")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrottleMilPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_throttle_ab")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrottleABPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_thrust_reverser")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustReverserPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_thrust_vectoring_yaw")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustVectoringYawPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_thrust_vectoring_pitch")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustVectoringPitchPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_thrust_vectoring_roll")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustVectoringRollPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_speed_brakes_out")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdSpeedBrakesOutPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_flaps_down")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdFlapsDownPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_spoilers_out")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdSpoilersOutPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_landing_gear_down")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdLandingGearDownPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_nose_wheel_steering")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdNoseWheelSteeringPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_nws_steering")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdNWS_SteeringPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_nws_enabled")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdNWS_EnabledPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_wheel_brake_left")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdWheelBrakeLeftPtr = new sInputOutputPair(controlName);
         }
         else if (localCommand == "std_wheel_brake_right")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdWheelBrakeRightPtr = new sInputOutputPair(controlName);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofPilotObject::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   // This will match the standard inputs/outputs with the control in mControlsList
   MatchStdInputsAndOutputs();

   return myCommand;
}

void P6DofPilotObject::MatchStdInputsAndOutputs()
{
   if (mStdStickBackPtr)
   {
      if (!MatchInputOutput(mStdStickBackPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdStickBack";
         out.AddNote() << "Output: " << mStdStickBackPtr->outputName;
      }

      // Get the handle for the stick back control
      mStdStickBackHandle = GetControlHandle(mStdStickBackPtr->controlInput->controlInputName);
   }

   if (mStdStickRightPtr)
   {
      if (!MatchInputOutput(mStdStickRightPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdStickRight";
         out.AddNote() << "Output: " << mStdStickRightPtr->outputName;
      }

      // Get the handle for the stick right control
      mStdStickRightHandle = GetControlHandle(mStdStickRightPtr->controlInput->controlInputName);
   }

   if (mStdRudderRightPtr)
   {
      if (!MatchInputOutput(mStdRudderRightPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdRudderRight";
         out.AddNote() << "Output: " << mStdRudderRightPtr->outputName;
      }

      // Get the handle for the rudder right control
      mStdRudderRightHandle = GetControlHandle(mStdRudderRightPtr->controlInput->controlInputName);
   }

   if (mStdThrottleMilPtr)
   {
      if (!MatchInputOutput(mStdThrottleMilPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdThrottleMil";
         out.AddNote() << "Output: " << mStdThrottleMilPtr->outputName;
      }

      // Get the handle for the throttle MIL control
      mStdThrottleMilHandle = GetControlHandle(mStdThrottleMilPtr->controlInput->controlInputName);
   }

   if (mStdThrottleABPtr)
   {
      if (!MatchInputOutput(mStdThrottleABPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdThrottleAB";
         out.AddNote() << "Output: " << mStdThrottleABPtr->outputName;
      }

      // Get the handle for the throttle AB control
      mStdThrottleABHandle = GetControlHandle(mStdThrottleABPtr->controlInput->controlInputName);
   }

   if (mStdThrustReverserPtr)
   {
      if (!MatchInputOutput(mStdThrustReverserPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdThrustReverser";
         out.AddNote() << "Output: " << mStdThrustReverserPtr->outputName;
      }

      // Get the handle for the thrust reverser control
      mStdThrustReverserHandle = GetControlHandle(mStdThrustReverserPtr->controlInput->controlInputName);
   }

   if (mStdThrustVectoringYawPtr)
   {
      if (!MatchInputOutput(mStdThrustVectoringYawPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdThrustVectoringYaw";
         out.AddNote() << "Output: " << mStdThrustVectoringYawPtr->outputName;
      }

      // Get the handle for the thrust vectoring yaw control
      mStdThrustVectoringYawHandle = GetControlHandle(mStdThrustVectoringYawPtr->controlInput->controlInputName);
   }

   if (mStdThrustVectoringPitchPtr)
   {
      if (!MatchInputOutput(mStdThrustVectoringPitchPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdThrustVectoringPitch";
         out.AddNote() << "Output: " << mStdThrustVectoringPitchPtr->outputName;
      }

      // Get the handle for the thrust vectoring pitch control
      mStdThrustVectoringPitchHandle = GetControlHandle(mStdThrustVectoringPitchPtr->controlInput->controlInputName);
   }

   if (mStdThrustVectoringRollPtr)
   {
      if (!MatchInputOutput(mStdThrustVectoringRollPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdThrustVectoringRoll";
         out.AddNote() << "Output: " << mStdThrustVectoringRollPtr->outputName;
      }

      // Get the handle for the thrust vectoring roll control
      mStdThrustVectoringRollHandle = GetControlHandle(mStdThrustVectoringRollPtr->controlInput->controlInputName);
   }

   if (mStdSpeedBrakesOutPtr)
   {
      if (!MatchInputOutput(mStdSpeedBrakesOutPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdSpeedBrakesOut";
         out.AddNote() << "Output: " << mStdSpeedBrakesOutPtr->outputName;
      }
      // Get the handle for the speed brakes control
      mStdSpeedBrakesOutHandle = GetControlHandle(mStdSpeedBrakesOutPtr->controlInput->controlInputName);
   }

   if (mStdFlapsDownPtr)
   {
      if (!MatchInputOutput(mStdFlapsDownPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdFlapsDown";
         out.AddNote() << "Output: " << mStdFlapsDownPtr->outputName;
      }

      // Get the handle for the flaps control
      mStdFlapsDownHandle = GetControlHandle(mStdFlapsDownPtr->controlInput->controlInputName);
   }

   if (mStdSpoilersOutPtr)
   {
      if (!MatchInputOutput(mStdSpoilersOutPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdSpoilersOut";
         out.AddNote() << "Output: " << mStdSpoilersOutPtr->outputName;
      }

      // Get the handle for the spoilers control
      mStdSpoilersOutHandle = GetControlHandle(mStdSpoilersOutPtr->controlInput->controlInputName);
   }

   if (mStdLandingGearDownPtr)
   {
      if (!MatchInputOutput(mStdLandingGearDownPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdLandingGearDown";
         out.AddNote() << "Output: " << mStdLandingGearDownPtr->outputName;
      }

      // Get the handle for the landing gear control
      mStdLandingGearDownHandle = GetControlHandle(mStdLandingGearDownPtr->controlInput->controlInputName);
   }

   if (mStdNoseWheelSteeringPtr)
   {
      if (!MatchInputOutput(mStdNoseWheelSteeringPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdNoseWheelSteering";
         out.AddNote() << "Output: " << mStdNoseWheelSteeringPtr->outputName;
      }

      // Get the handle for the nose wheel steering control
      mStdNoseWheelSteeringHandle = GetControlHandle(mStdNoseWheelSteeringPtr->controlInput->controlInputName);
   }

   if (mStdNWS_SteeringPtr)
   {
      if (!MatchInputOutput(mStdNWS_SteeringPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdNWS_Steering";
         out.AddNote() << "Output: " << mStdNWS_SteeringPtr->outputName;
      }

      // Get the handle for the NWS control
      mStdNWS_SteeringHandle = GetControlHandle(mStdNWS_SteeringPtr->controlInput->controlInputName);
   }

   if (mStdNWS_EnabledPtr)
   {
      if (!MatchInputOutput(mStdNWS_EnabledPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdNWS_Enabled";
         out.AddNote() << "Output: " << mStdNWS_EnabledPtr->outputName;
      }

      // Get the handle for the NWS enabled control
      mStdNWS_EnabledHandle = GetControlHandle(mStdNWS_EnabledPtr->controlInput->controlInputName);
   }

   if (mStdWheelBrakeLeftPtr)
   {
      if (!MatchInputOutput(mStdWheelBrakeLeftPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdWheelBrakeLeft";
         out.AddNote() << "Output: " << mStdWheelBrakeLeftPtr->outputName;
      }

      // Get the handle for the left wheel brake control
      mStdWheelBrakeLeftHandle = GetControlHandle(mStdWheelBrakeLeftPtr->controlInput->controlInputName);
   }

   if (mStdWheelBrakeRightPtr)
   {
      if (!MatchInputOutput(mStdWheelBrakeRightPtr))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdWheelBrakeRight";
         out.AddNote() << "Output: " << mStdWheelBrakeRightPtr->outputName;
      }

      // Get the handle for the right wheel brake control
      mStdWheelBrakeRightHandle = GetControlHandle(mStdWheelBrakeRightPtr->controlInput->controlInputName);
   }
}

bool P6DofPilotObject::Initialize(int64_t) // aSimTime_nanosec)
{
   InitializeCommonController();

   // This will match the standard inputs/outputs with the control in mControlsList
   MatchStdInputsAndOutputs();

   return true;
}

void P6DofPilotObject::UpdateControlInputsUsingCommonControllerData(double aDeltaT_sec)
{
   if (mCommonController == nullptr)
   {
      return;
   }

   if (ControlModeAutopilotActive())
   {
      if (mTempRoutePtr != nullptr && mTempActionPtr != nullptr)
      {
         // Temp activity is being flown, check if its a route
         if (mCommonController->GetWaypointAchieved())
         {
            if (mTempRoutePtr->GetNumberOfWaypoints() >= 2)
            {
               mTempPrevWptDataPtr = mTempCurrWptDataPtr;
               mTempCurrWptDataPtr = mTempNextWptDataPtr;
               mTempNextWptDataPtr = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

               if (mTempPrevWptDataPtr != nullptr)
               {
                  mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                                  mTempCurrWptDataPtr,
                                                  mTempNextWptDataPtr,
                                                  mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr),
                                                  mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr));
                  mCommonController->SetCurrentActivity(mTempActionPtr);

                  mCurrentTempWaypointIndex = mTempRoutePtr->GetWaypointIndex(mTempCurrWptDataPtr);

                  int id = mTempPrevWptDataPtr->GetId();
                  if (id != -1)
                  {
                     WaypointAchieved(id);
                  }
               }
            }
         }
      }
      else
      {
         // Check if any active channel is in waypoint mode
         // TODO -- Remove this for now -- need to investigate
         // if( (mAutopilotControlData.autopilotHorizontalModeActive &&
         // (mAutopilotControlData.autopilotHorizontalMode == P6DofAutopilotDataTypes::ARCM_WPT)) ||
         // (mAutopilotControlData.autopilotVerticalModeActive &&
         // (mAutopilotControlData.autopilotVerticalMode == P6DofAutopilotDataTypes::APCM_WPT)) ||
         // (mAutopilotControlData.autopilotSpeedModeActive &&
         // (mAutopilotControlData.autopilotSpeedMode == P6DofAutopilotDataTypes::ASCM_WPT)) )

         // At least one active channel is in waypoint mode, so determine if we need to go to the next waypoint
         if (mCommonController->GetWaypointAchieved())
         {
            // if( (mCurrentPlannedWaypointIndex >= 1) && (mCurrentPlannedWaypointIndex <
            // (int)mPlannedWaypointList.size()-2) )
            // {
            // // Move to next waypoint
            // ++mCurrentPlannedWaypointIndex;
            //
            // mPlannedWaypointData.SetPreviousWaypoint(*(mPlannedWaypointList.at(mCurrentPlannedWaypointIndex-1)->waypoint));
            // mPlannedWaypointData.SetCurrentWaypoint(*(mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->waypoint));
            // mPlannedWaypointData.SetNextWaypoint(*(mPlannedWaypointList.at(mCurrentPlannedWaypointIndex+1)->waypoint));
            //
            // // We set the speed based on speed-type from the waypoint speed pairing -- only one of these speeds
            // should be > zero. if( mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_ktas > 0 )
            // {
            // mPlannedWaypointData.SetSpeed(P6DofAutopilotWaypoint::SPD_TYPE_TAS_KNOTS,
            // mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_ktas );
            // }
            // else if( mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_kias > 0 )
            // {
            // mPlannedWaypointData.SetSpeed(P6DofAutopilotWaypoint::SPD_TYPE_CAS_KNOTS,
            // mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_kias );
            // }
            // else if( mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_mach > 0 )
            // {
            // mPlannedWaypointData.SetSpeed(P6DofAutopilotWaypoint::SPD_TYPE_MACH,
            // mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_mach );
            // }
            //
            // mPlannedWaypointData.SetWaypointSwitchingType(P6DofAutopilotWaypoint::WP_ON_APPROACH);
            //
            // mAutopilotActivityPtr->SetNavWaypoint(mPlannedWaypointData);
            // mCommonController->SetCurrentActivity(mAutopilotActivityPtr);
            // }

            mPlannedPrevWptDataPtr = mPlannedCurrWptDataPtr;
            mPlannedCurrWptDataPtr = mPlannedNextWptDataPtr;

            if (mPlannedRoutePtr != nullptr)
            {
               mPlannedNextWptDataPtr = mPlannedRoutePtr->GetNextWaypoint(mPlannedCurrWptDataPtr);

               if (mPlannedPrevWptDataPtr != nullptr)
               {
                  mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                                       mPlannedCurrWptDataPtr,
                                                       mPlannedNextWptDataPtr,
                                                       mPlannedRoutePtr->GetRouteSegment(mPlannedPrevWptDataPtr),
                                                       mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr));
                  mCommonController->SetCurrentActivity(mAutopilotActionPtr);

                  mCurrentPlannedWaypointIndex = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);

                  int id = mPlannedPrevWptDataPtr->GetId();
                  if (id != -1)
                  {
                     WaypointAchieved(id);
                  }
               }
            }
            else
            {
               mPlannedNextWptDataPtr = nullptr;
            }
         }
      }

      // Update the autopilot
      mCommonController->Update(mParentVehicle, mState, mAutopilotControls, aDeltaT_sec);

      // Set the controls data
      SetControlDataWithAutopilotControls();
   }
   else if (ControlModeControlAugmentationActive())
   {
      // Create a new temp activity if one does not already exist
      if (mTempActionPtr == nullptr)
      {
         mTempActionPtr = new P6DofAutopilotAction();
      }

      P6DOF::Control::Method controlMethod = mCommonController->GetControlMethod();
      bool                   yawToTurnControl =
         (controlMethod == P6DOF::Control::YawToTurnNoRoll || controlMethod == P6DOF::Control::YawToTurnRollRate ||
          controlMethod == P6DOF::Control::YawToTurnZeroBank);
      bool bankToTurnControl =
         (controlMethod == P6DOF::Control::BankToTurnNoYaw || controlMethod == P6DOF::Control::BankToTurnWithYaw);

      if (ControlModeStabilityAugmentationActive())
      {
         // Autopilot won't fully take control, but will contribute stabilizing inputs

         // Set the modes
         if (mPitchStabilityAugmentationModeActive)
         {
            mTempActionPtr->SetPitchRate_dps(0);
            mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchRate);
         }

         if (mYawStabilityAugmentationModeActive)
         {
            if (yawToTurnControl)
            {
               mTempActionPtr->SetYawRate_dps(0);
               mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::YawRate);
            }
            if (bankToTurnControl)
            {
               // Uncomment the following to use yaw rate in SAS
               mTempActionPtr->SetYawRate_dps(0);
               mTempActionPtr->SetStabilizingChannelMode(P6DOF::Lateral::YawRate);

               // Uncomment the following to use beta in SAS
               // mTempActionPtr->SetBeta_deg(0);
               // mTempActionPtr->SetStabilizingChannelMode(P6DOF::Lateral::Beta);
            }
         }

         if (mRollStabilityAugmentationModeActive)
         {
            mTempActionPtr->SetRollRate_dps(0);

            if (yawToTurnControl)
            {
               mTempActionPtr->SetStabilizingChannelMode(P6DOF::Lateral::RollRate);
            }
            if (bankToTurnControl)
            {
               mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::RollRate);
            }
         }

         mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::Undefined);

         // TODO - Do we need to do this?
         mCommonController->SetCurrentActivity(mTempActionPtr);

         // Update the autopilot
         mCommonController->Update(mParentVehicle, mState, mAutopilotControls, aDeltaT_sec);

         // Set the controls data
         SetControlDataWithStabilityAugmentationControls();
      }
      else
      {
         // The AP will control using manual stick inputs

         // Utilize the augmentation stick inputs modified by the augmenetation factors to
         // get the pitch g-load and roll rate.

         float pitchGLoad = (float)(mControlAugmentationStickBack * mPitchControlAugmentationFactor_g);
         mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchGLoad);

         float rollRate_dps = (float)(mControlAugmentationStickRight * mRollControlAugmentationFactor_dps);

         // Set the roll channel using roll rate based on stick
         mTempActionPtr->SetRollRate_dps(rollRate_dps);

         // Yaw uses the same augmentation factor as pitch,
         // which may or may not be a good idea.
         if (yawToTurnControl)
         {
            // No g-bias for skid-to-turn vehicles
            mTempActionPtr->SetPitchGLoad_g(pitchGLoad);

            float yawGLoad = (float)(mControlAugmentationRudderRight * mPitchControlAugmentationFactor_g);
            mTempActionPtr->SetYawGLoad_g(yawGLoad);
            mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::YawGLoad);

            // By setting the roll channel to Undefined, we allow the appropriate roll-nulling control
            mTempActionPtr->SetStabilizingChannelMode(P6DOF::Lateral::Undefined);
         }
         else if (bankToTurnControl)
         {
            mTempActionPtr->SetPitchGLoad_g(pitchGLoad + 1.0f);

            float beta_deg = (float)(mControlAugmentationRudderRight * -mPitchControlAugmentationFactor_g);
            mTempActionPtr->SetBeta_deg(beta_deg);
            mTempActionPtr->SetStabilizingChannelMode(P6DOF::Lateral::Beta);

            mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::RollRate);
         }

         mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::Undefined);

         // TODO - Do we need to do this?
         mCommonController->SetCurrentActivity(mTempActionPtr);

         // Update the autopilot
         mCommonController->Update(mParentVehicle, mState, mAutopilotControls, aDeltaT_sec);

         // Set the controls data
         SetControlDataWithControlAugmentationControls();
      }
   }
}

void P6DofPilotObject::Update(int64_t aSimTime_nanosec, P6DofKinematicState& aState)
{
   // Set the state
   mState = aState;

   // If same time, simply return
   if (aSimTime_nanosec == mLastSimTime_nanosec)
   {
      return;
   }

   // Calculate the delta-T
   int64_t dT_nanosec = aSimTime_nanosec - mLastSimTime_nanosec;

   if (dT_nanosec <= 0)
   {
      // Do nothing
      return;
   }

   mLastSimTime_nanosec = aSimTime_nanosec;
   double deltaT_sec    = P6DofUtils::TimeToTime(dT_nanosec);

   UpdateControlInputsUsingCommonControllerData(deltaT_sec);

   // Handle disabled controls
   if (ControlModeDisabledActive())
   {
      ZeroDisabledControlData();
   }

   P6DofFreezeFlags* manager = mParentVehicle->GetScenario()->GetFreezeFlags();
   if (manager == nullptr)
   {
      ut::log::error() << "Null Freeze Flags in P6DofPilotObject::Update().";
      return;
   }

   if (manager->GetMasterFreezeSpeed())
   {
      // Speed is frozen, set the throttle at MIL power and retract brakes and spoilers
      if (mStdThrottleMilPtr)
      {
         if (mStdThrottleMilPtr->controlInput)
         {
            mStdThrottleMilPtr->controlInput->normalizedValue = 1.0f;
         }
      }

      if (mStdThrottleABPtr)
      {
         if (mStdThrottleABPtr->controlInput)
         {
            mStdThrottleABPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustReverserPtr)
      {
         if (mStdThrustReverserPtr->controlInput)
         {
            mStdThrustReverserPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdSpoilersOutPtr)
      {
         if (mStdSpoilersOutPtr->controlInput)
         {
            mStdSpoilersOutPtr->controlInput->normalizedValue = 0.0f;
         }
      }
   }

   if (manager->GetMasterFreezePitch())
   {
      // Pitch is frozen, zero stick-back and pitch TVC
      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            mStdStickBackPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustVectoringYawPtr)
      {
         if (mStdThrustVectoringYawPtr->controlInput)
         {
            mStdThrustVectoringYawPtr->controlInput->normalizedValue = 0.0f;
         }
      }
   }

   if (manager->GetMasterFreezeRoll())
   {
      // Roll is frozen, zero stick-back and roll TVC
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            mStdStickRightPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = 0.0f;
         }
      }
   }

   if (manager->GetMasterFreezeYaw())
   {
      // Roll is frozen, zero stick-back and roll TVC
      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            mStdRudderRightPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = 0.0f;
         }
      }
   }
}

size_t P6DofPilotObject::GetControlHandle(const std::string& aControlName) const
{
   for (unsigned int i = 0; i < mControlsList.size(); ++i)
   {
      if (mControlsList.at(i).controlInputName == aControlName)
      {
         return i + 1;
      }
   }
   // We do not have a control input with the specified name
   return 0;
}

double P6DofPilotObject::ControlInputValue(size_t aControlInputHandle) const
{
   if ((aControlInputHandle > 0) && (aControlInputHandle <= mControlsList.size()))
   {
      size_t index = aControlInputHandle - 1;
      if (mControlsList.at(index).typeData == P6DOF::DataType::Float)
      {
         return mControlsList.at(index).normalizedValue;
      }
      else // bool
      {
         if (mControlsList.at(index).boolData)
         {
            return 1;
         }
         else
         {
            return 0;
         }
      }
   }
   return 0;
}

bool P6DofPilotObject::ControlInputValueBool(size_t aControlInputHandle) const
{
   if ((aControlInputHandle > 0) && (aControlInputHandle <= mControlsList.size()))
   {
      size_t index = aControlInputHandle - 1;
      if ((mControlsList.at(index).boolData) || ((mControlsList.at(index).typeData == P6DOF::DataType::Float) &&
                                                 (mControlsList.at(index).normalizedValue > 0.5)))
      {
         return true;
      }
   }
   return false;
}

void P6DofPilotObject::InputAngleDeltas(double aYaw_rad, double aPitch_rad, double aRoll_rad)
{
   if (mCommonController)
   {
      mCommonController->AngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }
}

bool P6DofPilotObject::InitializeCommonController()
{
   if (mCommonController)
   {
      mCommonController->SetControlAugmentationModeActive(mControlAugmentationModeActive);

      if (mAutopilotActionPtr == nullptr)
      {
         mAutopilotActionPtr = new P6DofAutopilotAction;
      }
      mCommonController->SetCurrentActivity(mAutopilotActionPtr);

      // TODO -- Do we want to initialize the mTempActivityPtr?
   }

   return true;
}

void P6DofPilotObject::SetPrePositionTAS(double aAltitude_ft, double aKTAS)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   mParentVehicle->ZeroKinematicsDuringTesting();
   mParentVehicle->SetPrepositionAlt_m(aAltitude_ft * UtMath::cM_PER_FT);
   mParentVehicle->SetVelocityNED(mParentVehicle->GetScenario()->GetAtmosphere()->CalcFpsFromKtas(aKTAS) * UtMath::cM_PER_FT,
                                  0.0,
                                  0.0);

   P6DofCommonController* commonController = GetCommonController();
   if (commonController)
   {
      commonController->ResetAccumulatedPidData();
   }
}

void P6DofPilotObject::SetPrePositionCAS(double aAltitude_ft, double aKCAS)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   mParentVehicle->ZeroKinematicsDuringTesting();
   mParentVehicle->SetPrepositionAlt_m(aAltitude_ft * UtMath::cM_PER_FT);
   mParentVehicle->SetVelocityNED(mParentVehicle->GetScenario()->GetAtmosphere()->CalcFpsFromKcas(aAltitude_ft, aKCAS) *
                                     UtMath::cM_PER_FT,
                                  0.0,
                                  0.0);

   P6DofCommonController* commonController = GetCommonController();
   if (commonController)
   {
      commonController->ResetAccumulatedPidData();
   }
}

void P6DofPilotObject::SetPrePositionMach(double aAltitude_ft, double aMach)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   mParentVehicle->ZeroKinematicsDuringTesting();
   mParentVehicle->SetPrepositionAlt_m(aAltitude_ft * UtMath::cM_PER_FT);
   mParentVehicle->SetVelocityNED(mParentVehicle->GetScenario()->GetAtmosphere()->CalcFpsFromMach(aAltitude_ft, aMach) *
                                     UtMath::cM_PER_FT,
                                  0.0,
                                  0.0);

   P6DofCommonController* commonController = GetCommonController();
   if (commonController)
   {
      commonController->ResetAccumulatedPidData();
   }
}

void P6DofPilotObject::SetPrePositionQ(double aAltitude_ft, double aDynamicPressure_lbft2)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   mParentVehicle->ZeroKinematicsDuringTesting();
   mParentVehicle->SetPrepositionAlt_m(aAltitude_ft * UtMath::cM_PER_FT);
   mParentVehicle->SetVelocityNED(
      mParentVehicle->GetScenario()->GetAtmosphere()->CalcFpsFromAltitudeDynamicPressure(aAltitude_ft,
                                                                                         aDynamicPressure_lbft2) *
         UtMath::cM_PER_FT,
      0.0,
      0.0);

   P6DofCommonController* commonController = GetCommonController();
   if (commonController)
   {
      commonController->ResetAccumulatedPidData();
   }
}

void P6DofPilotObject::SetObjectState(double aLat,
                                      double aLon,
                                      double aAlt_m,
                                      double aNEDVelN_mps,
                                      double aNEDVelE_mps,
                                      double aNEDVelD_mps,
                                      double aLocalHeading_rad,
                                      double aLocalPitch_rad,
                                      double aLocalRoll_rad)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   // Set conditions
   mParentVehicle->SetLocationLLA(aLat, aLon, aAlt_m);
   mParentVehicle->SetAttitudeNED(aLocalHeading_rad, aLocalPitch_rad, aLocalRoll_rad);
   mParentVehicle->SetVelocityNED(aNEDVelN_mps, aNEDVelE_mps, aNEDVelD_mps);

   P6DofCommonController* commonController = GetCommonController();
   if (commonController)
   {
      commonController->ResetAccumulatedPidData();
   }
}

void P6DofPilotObject::SetControlDataWithAutopilotControls()
{
   if (ControlModeAutopilotActive())
   {
      LoadControlDataWithAutopilotControlData();
   }
}

void P6DofPilotObject::SetControlDataWithControlAugmentationControls()
{
   if (ControlModeControlAugmentationActive())
   {
      LoadControlDataWithAutopilotControlData();
   }
}

void P6DofPilotObject::SetControlDataWithStabilityAugmentationControls()
{
   if (ControlModeStabilityAugmentationActive())
   {
      LoadControlDataWithAutopilotStabilityData();
   }
}

void P6DofPilotObject::LoadControlDataWithAutopilotControlData()
{
   // Be sure that controls are within limits
   EnforceControlLimits();

   // Now we go through the "standard" controls and set the control input values
   // using the common controller (autopilot) data, where appropriate.

   if (mStdStickRightPtr)
   {
      if (mStdStickRightPtr->controlInput)
      {
         mStdStickRightPtr->controlInput->normalizedValue = (float)mAutopilotControls.stickRight;
      }
   }

   if (mStdStickBackPtr)
   {
      if (mStdStickBackPtr->controlInput)
      {
         mStdStickBackPtr->controlInput->normalizedValue = (float)mAutopilotControls.stickBack;
      }
   }

   if (mStdRudderRightPtr)
   {
      if (mStdRudderRightPtr->controlInput)
      {
         mStdRudderRightPtr->controlInput->normalizedValue = (float)mAutopilotControls.rudderRight;
      }
   }

   if (ControlModeControlAugmentationActive())
   {
      // For CAS, we do not use the common controller data for throttle or speed brakes
   }
   else
   {
      if (mStdThrottleMilPtr)
      {
         // Only set data with common controller data if override is not active
         if (!mControlOverrideThrottle)
         {
            if (mStdThrottleMilPtr->controlInput)
            {
               mStdThrottleMilPtr->controlInput->normalizedValue = (float)mAutopilotControls.throttleMilitary;
            }
         }
      }

      if (mStdThrottleABPtr)
      {
         // Only set data with common controller data if override is not active
         if (!mControlOverrideThrottle)
         {
            if (mStdThrottleABPtr->controlInput)
            {
               mStdThrottleABPtr->controlInput->normalizedValue = (float)mAutopilotControls.throttleAfterburner;
            }
         }
      }

      if (mStdSpeedBrakesOutPtr)
      {
         // Only set data with common controller data if override is not active
         if (!mControlOverrideSpeedBrakes)
         {
            if (mStdSpeedBrakesOutPtr->controlInput)
            {
               mStdSpeedBrakesOutPtr->controlInput->normalizedValue = (float)mAutopilotControls.speedBrake;
            }
         }
      }
   }

   // mStdThrustReverserPtr -- Do not use common controller data

   if (mStdThrustVectoringYawPtr)
   {
      if (mStdThrustVectoringYawPtr->controlInput)
      {
         mStdThrustVectoringYawPtr->controlInput->normalizedValue = (float)mAutopilotControls.thrustVectorYaw;
      }
   }

   if (mStdThrustVectoringPitchPtr)
   {
      if (mStdThrustVectoringPitchPtr->controlInput)
      {
         mStdThrustVectoringPitchPtr->controlInput->normalizedValue = (float)mAutopilotControls.thrustVectorPitch;
      }
   }

   if (mStdThrustVectoringRollPtr)
   {
      if (mStdThrustVectoringRollPtr->controlInput)
      {
         mStdThrustVectoringRollPtr->controlInput->normalizedValue = (float)mAutopilotControls.thrustVectorRoll;
      }
   }

   // Do not use common controller data for mStdFlapsDownPtr, mStdSpoilersOutPtr,
   // mStdLandingGearDownPtr, and mStdNWS_EnabledPtr

   if (mStdNoseWheelSteeringPtr)
   {
      if (mStdNoseWheelSteeringPtr->controlInput)
      {
         mStdNoseWheelSteeringPtr->controlInput->normalizedValue = (float)mAutopilotControls.noseWheelSteering;
      }
   }

   if (mStdNWS_SteeringPtr)
   {
      if (mStdNWS_SteeringPtr->controlInput)
      {
         mStdNWS_SteeringPtr->controlInput->normalizedValue = (float)mAutopilotControls.nwsSteering;
      }
   }

   if (mStdWheelBrakeLeftPtr)
   {
      // Only set data with common controller data if override is not active
      if (!mControlOverrideWheelBrakes)
      {
         if (mStdWheelBrakeLeftPtr->controlInput)
         {
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = (float)mAutopilotControls.wheelBrakeLeft;
         }
      }
   }

   if (mStdWheelBrakeRightPtr)
   {
      // Only set data with common controller data if override is not active
      if (!mControlOverrideWheelBrakes)
      {
         if (mStdWheelBrakeRightPtr->controlInput)
         {
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = (float)mAutopilotControls.wheelBrakeRight;
         }
      }
   }
}

void P6DofPilotObject::LoadControlDataWithAutopilotStabilityData()
{
   // Be sure that controls are within limits
   EnforceControlLimits();

   // Go through the stick controls and blend the autopilot and manual responses.
   // Inspired by the A-10's SAS construction, SAS may contribute up to 25% stick
   // input in any axis. The remainder is provided by the stick inputs.

   const double autopilotMagnitudeLimit = 0.25;

   if (mStdStickRightPtr && mCommonController->RollStabilityAugmentationModeIsActive())
   {
      if (mStdStickRightPtr->controlInput)
      {
         double autopilotMagnitude = std::abs(mAutopilotControls.stickRight);
         autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
         double stickMagnitude     = 1.0 - autopilotMagnitude;

         mStdStickRightPtr->controlInput->normalizedValue =
            float(mControlAugmentationStickRight * stickMagnitude + mAutopilotControls.stickRight * autopilotMagnitude);
      }
   }

   if (mStdStickBackPtr && mCommonController->PitchStabilityAugmentationModeIsActive())
   {
      if (mStdStickBackPtr->controlInput)
      {
         double autopilotMagnitude = std::abs(mAutopilotControls.stickBack);
         autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
         double stickMagnitude     = 1.0 - autopilotMagnitude;

         mStdStickBackPtr->controlInput->normalizedValue =
            float(mControlAugmentationStickBack * stickMagnitude + mAutopilotControls.stickBack * autopilotMagnitude);
      }
   }

   if (mStdRudderRightPtr && mCommonController->YawStabilityAugmentationModeIsActive())
   {
      if (mStdRudderRightPtr->controlInput)
      {
         double autopilotMagnitude = std::abs(mAutopilotControls.rudderRight);
         autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
         double stickMagnitude     = 1.0 - autopilotMagnitude;

         mStdRudderRightPtr->controlInput->normalizedValue =
            float(mControlAugmentationRudderRight * stickMagnitude + mAutopilotControls.rudderRight * autopilotMagnitude);
      }
   }
}

void P6DofPilotObject::EnableAutopilot(bool aState)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mAutopilotEnabled = aState;
}

P6DofAutopilotAction* P6DofPilotObject::GetCurrentAction() const
{
   if (mTempActionPtr != nullptr)
   {
      return mTempActionPtr;
   }
   return mAutopilotActionPtr;
}

void P6DofPilotObject::GetAutopilotData(P6DOF::AutopilotData* aData)
{
   if ((mCommonController == nullptr) || !mAutopilotEnabled)
   {
      aData->autopilotEnabled = false;

      aData->navWaypointParameters.navWaypointDataValid     = false;
      aData->navWaypointParameters.navWaypointLegType       = P6DOF::Nav::LegUndefined;
      aData->navWaypointParameters.navWaypointSwitchingType = P6DOF::Nav::SwitchUndefined;

      aData->navWaypointParameters.navWaypointPrevLat = 0.0f;
      aData->navWaypointParameters.navWaypointPrevLon = 0.0f;
      aData->navWaypointParameters.navWaypointPrevAlt = 0.0f;

      aData->navWaypointParameters.navWaypointCurrLat = 0.0f;
      aData->navWaypointParameters.navWaypointCurrLon = 0.0f;
      aData->navWaypointParameters.navWaypointCurrAlt = 0.0f;

      aData->navWaypointParameters.navWaypointNextLat = 0.0f;
      aData->navWaypointParameters.navWaypointNextLon = 0.0f;
      aData->navWaypointParameters.navWaypointNextAlt = 0.0f;

      aData->navWaypointParameters.navWaypointAimHeading_rad   = 0.0f;
      aData->navWaypointParameters.navWaypointStartTurnHdg_rad = 0.0f;
      aData->navWaypointParameters.navWaypointTurnRadius_m     = 0.0f;
      aData->navWaypointParameters.navWaypointTurnAngle_rad    = 0.0f;
      aData->navWaypointParameters.navWaypointTurnRefPtLat     = 0.0f;
      aData->navWaypointParameters.navWaypointTurnRefPtLon     = 0.0f;
      aData->navWaypointParameters.navWaypointTurnCenterLat    = 0.0f;
      aData->navWaypointParameters.navWaypointTurnCenterLon    = 0.0f;

      return;
   }

   // Show AP as enabled
   aData->autopilotEnabled = true;

   double aimHeading_rad;
   double startTurnHdg_rad;
   double turnRadius_m;
   double turnAngle_rad;
   double turnRefPtLat;
   double turnRefPtLon;
   bool   turnCW;
   bool   turning;
   double turnCenterLat;
   double turnCenterLon;

   mCommonController->GetAutoPilotTurnData(aimHeading_rad,
                                           startTurnHdg_rad,
                                           turnRadius_m,
                                           turnAngle_rad,
                                           turnRefPtLat,
                                           turnRefPtLon,
                                           turnCW,
                                           turning,
                                           turnCenterLat,
                                           turnCenterLon);

   aData->navWaypointParameters.navWaypointAimHeading_rad   = (float)aimHeading_rad;
   aData->navWaypointParameters.navWaypointStartTurnHdg_rad = (float)startTurnHdg_rad;
   aData->navWaypointParameters.navWaypointTurnRadius_m     = (float)turnRadius_m;
   aData->navWaypointParameters.navWaypointTurnAngle_rad    = (float)turnAngle_rad;
   aData->navWaypointParameters.navWaypointTurnRefPtLat     = (float)turnRefPtLat;
   aData->navWaypointParameters.navWaypointTurnRefPtLon     = (float)turnRefPtLon;
   aData->navWaypointParameters.navWaypointTurnCenterLat    = (float)turnCenterLat;
   aData->navWaypointParameters.navWaypointTurnCenterLon    = (float)turnCenterLon;

   if (mTempActionPtr)
   {
      aData->navWaypointParameters.navWaypointDataValid = true;

      if (mTempPrevWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointPrevLat = (float)mTempPrevWptDataPtr->GetLLA().GetLat();
         aData->navWaypointParameters.navWaypointPrevLon = (float)mTempPrevWptDataPtr->GetLLA().GetLon();
         aData->navWaypointParameters.navWaypointPrevAlt = (float)mTempPrevWptDataPtr->GetLLA().GetAlt();
      }
      if (mTempCurrWptDataPtr != nullptr)
      {
         if (mTempCurrWptDataPtr->FollowHorizontalTrack())
         {
            aData->navWaypointParameters.navWaypointLegType = P6DOF::Nav::FollowTrack;
         }
         else
         {
            aData->navWaypointParameters.navWaypointLegType = P6DOF::Nav::DirectTo;
         }

         if (!mTempCurrWptDataPtr->WaypointOnPassing())
         {
            aData->navWaypointParameters.navWaypointSwitchingType = P6DOF::Nav::OnApproach;
         }
         else
         {
            aData->navWaypointParameters.navWaypointSwitchingType = P6DOF::Nav::OnPassing;
         }

         aData->navWaypointParameters.navWaypointCurrLat = (float)mTempCurrWptDataPtr->GetLLA().GetLat();
         aData->navWaypointParameters.navWaypointCurrLon = (float)mTempCurrWptDataPtr->GetLLA().GetLon();
         aData->navWaypointParameters.navWaypointCurrAlt = (float)mTempCurrWptDataPtr->GetLLA().GetAlt();
      }
      if (mTempNextWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointNextLat = (float)mTempNextWptDataPtr->GetLLA().GetLat();
         aData->navWaypointParameters.navWaypointNextLon = (float)mTempNextWptDataPtr->GetLLA().GetLon();
         aData->navWaypointParameters.navWaypointNextAlt = (float)mTempNextWptDataPtr->GetLLA().GetAlt();
      }
   }
   else
   {
      aData->navWaypointParameters.navWaypointDataValid = true;

      if (mPlannedPrevWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointPrevLat = (float)mPlannedPrevWptDataPtr->GetLLA().GetLat();
         aData->navWaypointParameters.navWaypointPrevLon = (float)mPlannedPrevWptDataPtr->GetLLA().GetLon();
         aData->navWaypointParameters.navWaypointPrevAlt = (float)mPlannedPrevWptDataPtr->GetLLA().GetAlt();
      }
      if (mPlannedCurrWptDataPtr != nullptr)
      {
         if (mPlannedCurrWptDataPtr->FollowHorizontalTrack())
         {
            aData->navWaypointParameters.navWaypointLegType = P6DOF::Nav::FollowTrack;
         }
         else
         {
            aData->navWaypointParameters.navWaypointLegType = P6DOF::Nav::DirectTo;
         }

         if (!mPlannedCurrWptDataPtr->WaypointOnPassing())
         {
            aData->navWaypointParameters.navWaypointSwitchingType = P6DOF::Nav::OnApproach;
         }
         else
         {
            aData->navWaypointParameters.navWaypointSwitchingType = P6DOF::Nav::OnPassing;
         }

         aData->navWaypointParameters.navWaypointCurrLat = (float)mPlannedCurrWptDataPtr->GetLLA().GetLat();
         aData->navWaypointParameters.navWaypointCurrLon = (float)mPlannedCurrWptDataPtr->GetLLA().GetLon();
         aData->navWaypointParameters.navWaypointCurrAlt = (float)mPlannedCurrWptDataPtr->GetLLA().GetAlt();
      }
      if (mPlannedNextWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointNextLat = (float)mPlannedNextWptDataPtr->GetLLA().GetLat();
         aData->navWaypointParameters.navWaypointNextLon = (float)mPlannedNextWptDataPtr->GetLLA().GetLon();
         aData->navWaypointParameters.navWaypointNextAlt = (float)mPlannedNextWptDataPtr->GetLLA().GetAlt();
      }
   }
}

bool P6DofPilotObject::GetAutopilotPidGainData(P6DOF::Pid::Type   aTableType,
                                               size_t&            aNumElements,
                                               P6DOF::PidGainData aPidGainData[],
                                               unsigned char&     aPidFlags)
{
   if (mCommonController == nullptr)
   {
      return false;
   }

   return mCommonController->GetAutopilotPidGainData(aTableType, aNumElements, aPidGainData, aPidFlags);
}

void P6DofPilotObject::GetAutopilotPidValueData(P6DOF::AutopilotPidGroupValueData* aData)
{
   if (mCommonController == nullptr)
   {
      return;
   }

   mCommonController->GetAutopilotPidValues(aData);
}

bool P6DofPilotObject::MatchInputOutput(sInputOutputPair* aInputOutput)
{
   if (aInputOutput == nullptr)
   {
      return false;
   }

   std::vector<P6DOF::FlightControlInputValue>::iterator iter = mControlsList.begin();
   while (iter != mControlsList.end())
   {
      std::string controlName((*iter).controlInputName);

      if (aInputOutput->outputName == controlName)
      {
         // Match found
         aInputOutput->controlInput = &(*iter);
         aInputOutput->copiedInput  = true;
         return true;
      }

      ++iter;
   }

   return false;
}

void P6DofPilotObject::EnforceControlLimits()
{
   EnforceSingleControlLimit(mAutopilotControls.stickBack, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.stickRight, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.rudderRight, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.throttleMilitary, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.throttleAfterburner, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.thrustVectorYaw, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.thrustVectorPitch, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.thrustVectorRoll, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.speedBrake, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.noseWheelSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.nwsSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.wheelBrakeLeft, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.wheelBrakeRight, 0.0, 1.0);
}

void P6DofPilotObject::EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue)
{
   if (aValue < aMinValue)
   {
      aValue = aMinValue;
   }
   if (aValue > aMaxValue)
   {
      aValue = aMaxValue;
   }
}

void P6DofPilotObject::SetThrottleData(double aThrottleLeverPosition)
{
   // The aThrottleLeverPosition uses 0.0=IDLE, 1.0=MIL, and 2.0=AB
   if (mStdThrottleMilPtr)
   {
      double tempThrottle = aThrottleLeverPosition;

      if (tempThrottle > 1.0)
      {
         // Using AB
         tempThrottle = tempThrottle - 1.0;
         if (mStdThrottleABPtr)
         {
            mStdThrottleMilPtr->controlInput->normalizedValue = 1.0f;
            mStdThrottleABPtr->controlInput->normalizedValue  = (float)tempThrottle;
         }
         else
         {
            // No AB present, set max power for MIL
            mStdThrottleMilPtr->controlInput->normalizedValue = 1.0f;
         }
      }
      else
      {
         // Not using AB
         if (mStdThrottleABPtr)
         {
            // AB is present but not current used
            mStdThrottleABPtr->controlInput->normalizedValue  = 0.0f;
            mStdThrottleMilPtr->controlInput->normalizedValue = (float)tempThrottle;
         }
         else
         {
            // No AB present, set power for MIL
            mStdThrottleMilPtr->controlInput->normalizedValue = (float)tempThrottle;
         }
      }
   }

   // Ensure values are normalized (0.0 to 1.0)

   if (mStdThrottleMilPtr)
   {
      if (mStdThrottleMilPtr->controlInput->normalizedValue < 0.0)
      {
         mStdThrottleMilPtr->controlInput->normalizedValue = 0.0;
      }
      else if (mStdThrottleMilPtr->controlInput->normalizedValue > 1.0)
      {
         mStdThrottleMilPtr->controlInput->normalizedValue = 1.0;
      }
   }

   if (mStdThrottleABPtr)
   {
      if (mStdThrottleABPtr->controlInput->normalizedValue < 0.0)
      {
         mStdThrottleABPtr->controlInput->normalizedValue = 0.0;
      }
      else if (mStdThrottleABPtr->controlInput->normalizedValue > 1.0)
      {
         mStdThrottleABPtr->controlInput->normalizedValue = 1.0;
      }
   }
}

void P6DofPilotObject::SetThrottleMilitaryData(double aThrottleLeverPosition)
{
   if (mStdThrottleMilPtr)
   {
      mStdThrottleMilPtr->controlInput->normalizedValue = (float)aThrottleLeverPosition;

      // Ensure values are normalized (0.0 to 1.0)
      if (mStdThrottleMilPtr->controlInput->normalizedValue < 0.0)
      {
         mStdThrottleMilPtr->controlInput->normalizedValue = 0.0;
      }
      else if (mStdThrottleMilPtr->controlInput->normalizedValue > 1.0)
      {
         mStdThrottleMilPtr->controlInput->normalizedValue = 1.0;
      }
   }
}

void P6DofPilotObject::SetThrottleAfterburnerData(double aThrottleLeverPosition)
{
   if (mStdThrottleABPtr)
   {
      mStdThrottleABPtr->controlInput->normalizedValue = (float)aThrottleLeverPosition;

      // Ensure values are normalized (0.0 to 1.0)
      if (mStdThrottleABPtr->controlInput->normalizedValue < 0.0)
      {
         mStdThrottleABPtr->controlInput->normalizedValue = 0.0;
      }
      else if (mStdThrottleABPtr->controlInput->normalizedValue > 1.0)
      {
         mStdThrottleABPtr->controlInput->normalizedValue = 1.0;
      }
   }
}

void P6DofPilotObject::SetManualControlData(double aStickRightPosition,
                                            double aStickAftPosition,
                                            double aThrottleLeverPosition,
                                            double aSpdBrakeLeverPosition,
                                            double aSpoilerLeverPosition,
                                            double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            mTrimRollRight = mRollTrimFactor * mRollRightTrimDeltaT_sec;
            if (mTrimRollRight < -1.0)
            {
               mTrimRollRight = -1.0;
            }
            if (mTrimRollRight > 1.0)
            {
               mTrimRollRight = 1.0;
            }

            aStickRightPosition += mTrimRollRight;
            if (aStickRightPosition < -1.0)
            {
               aStickRightPosition = -1.0;
            }
            if (aStickRightPosition > 1.0)
            {
               aStickRightPosition = 1.0;
            }

            if (mRollControlMapping != nullptr)
            {
               aStickRightPosition = mRollControlMapping->Lookup(aStickRightPosition);
            }

            mStdStickRightPtr->controlInput->normalizedValue = (float)aStickRightPosition;

            // Set augmented control input value
            mControlAugmentationStickRight = aStickRightPosition;
         }
      }

      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            mTrimNoseUp = mPitchTrimFactor * mNoseUpTrimDeltaT_sec;
            if (mTrimNoseUp < -1.0)
            {
               mTrimNoseUp = -1.0;
            }
            if (mTrimNoseUp > 1.0)
            {
               mTrimNoseUp = 1.0;
            }

            aStickAftPosition += mTrimNoseUp;
            if (aStickAftPosition < -1.0)
            {
               aStickAftPosition = -1.0;
            }
            if (aStickAftPosition > 1.0)
            {
               aStickAftPosition = 1.0;
            }

            if (mPitchControlMapping != nullptr)
            {
               aStickAftPosition = mPitchControlMapping->Lookup(aStickAftPosition);
            }

            mStdStickBackPtr->controlInput->normalizedValue = (float)aStickAftPosition;

            // Set augmented control input value
            mControlAugmentationStickBack = aStickAftPosition;
         }
      }

      SetThrottleData(aThrottleLeverPosition);

      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = (float)aSpdBrakeLeverPosition;
         }
      }

      if (mStdSpoilersOutPtr)
      {
         if (mStdSpoilersOutPtr->controlInput)
         {
            mStdSpoilersOutPtr->controlInput->normalizedValue = (float)aSpoilerLeverPosition;
         }
      }

      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            mStdFlapsDownPtr->controlInput->normalizedValue = (float)aFlapsLeverPosition;
         }
      }
   }
}

void P6DofPilotObject::SetManualControlData(double aStickRightPosition,
                                            double aStickAftPosition,
                                            double aRudderRightPosition,
                                            double aThrottleLeverPosition,
                                            double aSpdBrakeLeverPosition,
                                            double aSpoilerLeverPosition,
                                            double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      SetManualControlData(aStickRightPosition,
                           aStickAftPosition,
                           aThrottleLeverPosition,
                           aSpdBrakeLeverPosition,
                           aSpoilerLeverPosition,
                           aFlapsLeverPosition);

      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            mTrimYawRight = mYawTrimFactor * mYawRightTrimDeltaT_sec;
            if (mTrimYawRight < -1.0)
            {
               mTrimYawRight = -1.0;
            }
            if (mTrimYawRight > 1.0)
            {
               mTrimYawRight = 1.0;
            }

            aRudderRightPosition += mTrimYawRight;
            if (aRudderRightPosition < -1.0)
            {
               aRudderRightPosition = -1.0;
            }
            if (aRudderRightPosition > 1.0)
            {
               aRudderRightPosition = 1.0;
            }

            if (mYawControlMapping != nullptr)
            {
               aRudderRightPosition = mYawControlMapping->Lookup(aRudderRightPosition);
            }

            mStdRudderRightPtr->controlInput->normalizedValue = (float)aRudderRightPosition;

            // Set augmented control input value
            mControlAugmentationRudderRight = aRudderRightPosition;
         }
      }
   }
}

void P6DofPilotObject::SetManualControlData(double aStickRightPosition,
                                            double aStickAftPosition,
                                            double aRudderRightPosition,
                                            double aThrottleLeverPosition,
                                            double aSpdBrakeLeverPosition,
                                            double aSpoilerLeverPosition,
                                            double aFlapsLeverPosition,
                                            double aLandingGearLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      SetManualControlData(aStickRightPosition,
                           aStickAftPosition,
                           aRudderRightPosition,
                           aThrottleLeverPosition,
                           aSpdBrakeLeverPosition,
                           aSpoilerLeverPosition,
                           aFlapsLeverPosition);

      if (mStdLandingGearDownPtr)
      {
         if (mStdLandingGearDownPtr->controlInput)
         {
            mStdLandingGearDownPtr->controlInput->normalizedValue = (float)aLandingGearLeverPosition;
         }
      }
   }
}

void P6DofPilotObject::SetManualControlData(double aStickRightPosition,
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
                                            bool   aNWS_Enabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      SetManualControlData(aStickRightPosition,
                           aStickAftPosition,
                           aRudderRightPosition,
                           aThrottleLeverPosition,
                           aSpdBrakeLeverPosition,
                           aSpoilerLeverPosition,
                           aFlapsLeverPosition,
                           aLandingGearLeverPosition);

      if (mStdThrustReverserPtr)
      {
         if (mStdThrustReverserPtr->controlInput)
         {
            mStdThrustReverserPtr->controlInput->normalizedValue = (float)aThrustReverserLeverPosition;
         }
      }

      if (mStdThrustVectoringYawPtr)
      {
         if (mStdThrustVectoringYawPtr->controlInput)
         {
            mStdThrustVectoringYawPtr->controlInput->normalizedValue = (float)aThrustVectorYawRightPosition;
         }
      }

      if (mStdThrustVectoringPitchPtr)
      {
         if (mStdThrustVectoringPitchPtr->controlInput)
         {
            mStdThrustVectoringPitchPtr->controlInput->normalizedValue = (float)aThrustVectorPitchUpPosition;
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = (float)aThrustVectorRollRightPosition;
         }
      }

      if (mStdNoseWheelSteeringPtr)
      {
         if (mStdNoseWheelSteeringPtr->controlInput)
         {
            mStdNoseWheelSteeringPtr->controlInput->normalizedValue = (float)aNoseWheelSteeringRightPosition;
         }
      }

      if (mStdNWS_SteeringPtr)
      {
         if (mStdNWS_SteeringPtr->controlInput)
         {
            // Note that mStdNWS_SteeringPtr uses the same input as for mStdNoseWheelSteeringPtr
            mStdNWS_SteeringPtr->controlInput->normalizedValue = (float)aNoseWheelSteeringRightPosition;
         }
      }

      if (mStdNWS_EnabledPtr)
      {
         if (mStdNWS_EnabledPtr->controlInput)
         {
            mStdNWS_EnabledPtr->controlInput->boolData = aNWS_Enabled;
         }
      }

      if (mStdWheelBrakeLeftPtr)
      {
         if (mStdWheelBrakeLeftPtr->controlInput)
         {
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = (float)aWheelBrakeLeftPosition;
         }
      }

      if (mStdWheelBrakeRightPtr)
      {
         if (mStdWheelBrakeRightPtr->controlInput)
         {
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = (float)aWheelBrakeRightPosition;
         }
      }
   }
}

void P6DofPilotObject::SetTrimManualControlData(double aNoseUpTrimDeltaT_sec,
                                                double aRollRightTrimDeltaT_sec,
                                                double aYawRightTrimDeltaT_sec)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mNoseUpTrimDeltaT_sec += aNoseUpTrimDeltaT_sec;
   mRollRightTrimDeltaT_sec += aRollRightTrimDeltaT_sec;
   mYawRightTrimDeltaT_sec += aYawRightTrimDeltaT_sec;
}

void P6DofPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                    double aStickAftPosition,
                                                    double aThrottleLeverPosition,
                                                    double aSpdBrakeLeverPosition,
                                                    double aSpoilerLeverPosition,
                                                    double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            mTrimRollRight = 0.1 * mRollRightTrimDeltaT_sec;
            if (mTrimRollRight < -1.0)
            {
               mTrimRollRight = -1.0;
            }
            if (mTrimRollRight > 1.0)
            {
               mTrimRollRight = 1.0;
            }

            aStickRightPosition += mTrimRollRight;
            if (aStickRightPosition < -1.0)
            {
               aStickRightPosition = -1.0;
            }
            if (aStickRightPosition > 1.0)
            {
               aStickRightPosition = 1.0;
            }

            if (mRollControlMapping != nullptr)
            {
               aStickRightPosition = mRollControlMapping->Lookup(aStickRightPosition);
            }

            mStdStickRightPtr->controlInput->normalizedValue = (float)aStickRightPosition;

            // Set augmented control input value
            mControlAugmentationStickRight = aStickRightPosition;
         }
      }

      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            mTrimNoseUp = 0.1 * mNoseUpTrimDeltaT_sec;
            if (mTrimNoseUp < -1.0)
            {
               mTrimNoseUp = -1.0;
            }
            if (mTrimNoseUp > 1.0)
            {
               mTrimNoseUp = 1.0;
            }

            aStickAftPosition += mTrimNoseUp;
            if (aStickAftPosition < -1.0)
            {
               aStickAftPosition = -1.0;
            }
            if (aStickAftPosition > 1.0)
            {
               aStickAftPosition = 1.0;
            }

            if (mPitchControlMapping != nullptr)
            {
               aStickAftPosition = mPitchControlMapping->Lookup(aStickAftPosition);
            }

            mStdStickBackPtr->controlInput->normalizedValue = (float)aStickAftPosition;

            // Set augmented control input value
            mControlAugmentationStickBack = aStickAftPosition;
         }
      }

      SetThrottleData(aThrottleLeverPosition);

      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = (float)aSpdBrakeLeverPosition;
         }
      }

      if (mStdSpoilersOutPtr)
      {
         if (mStdSpoilersOutPtr->controlInput)
         {
            mStdSpoilersOutPtr->controlInput->normalizedValue = (float)aSpoilerLeverPosition;
         }
      }

      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            mStdFlapsDownPtr->controlInput->normalizedValue = (float)aFlapsLeverPosition;
         }
      }
   }
}

void P6DofPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                    double aStickAftPosition,
                                                    double aRudderRightPosition,
                                                    double aThrottleLeverPosition,
                                                    double aSpdBrakeLeverPosition,
                                                    double aSpoilerLeverPosition,
                                                    double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      SetExternalDirectControlData(aStickRightPosition,
                                   aStickAftPosition,
                                   aThrottleLeverPosition,
                                   aSpdBrakeLeverPosition,
                                   aSpoilerLeverPosition,
                                   aFlapsLeverPosition);

      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            mTrimYawRight = 0.1 * mYawRightTrimDeltaT_sec;
            if (mTrimYawRight < -1.0)
            {
               mTrimYawRight = -1.0;
            }
            if (mTrimYawRight > 1.0)
            {
               mTrimYawRight = 1.0;
            }

            aRudderRightPosition += mTrimYawRight;
            if (aRudderRightPosition < -1.0)
            {
               aRudderRightPosition = -1.0;
            }
            if (aRudderRightPosition > 1.0)
            {
               aRudderRightPosition = 1.0;
            }

            mStdRudderRightPtr->controlInput->normalizedValue = (float)aRudderRightPosition;

            // TODO -- Do we need/want augmented rudder?
         }
      }
   }
}

void P6DofPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                    double aStickAftPosition,
                                                    double aRudderRightPosition,
                                                    double aThrottleLeverPosition,
                                                    double aSpdBrakeLeverPosition,
                                                    double aSpoilerLeverPosition,
                                                    double aFlapsLeverPosition,
                                                    double aLandingGearLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      SetExternalDirectControlData(aStickRightPosition,
                                   aStickAftPosition,
                                   aRudderRightPosition,
                                   aThrottleLeverPosition,
                                   aSpdBrakeLeverPosition,
                                   aSpoilerLeverPosition,
                                   aFlapsLeverPosition);

      if (mStdLandingGearDownPtr)
      {
         if (mStdLandingGearDownPtr->controlInput)
         {
            mStdLandingGearDownPtr->controlInput->normalizedValue = (float)aLandingGearLeverPosition;
         }
      }
   }
}

void P6DofPilotObject::SetExternalDirectControlData(double aStickRightPosition,
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
                                                    bool   aNWS_Enabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      SetExternalDirectControlData(aStickRightPosition,
                                   aStickAftPosition,
                                   aRudderRightPosition,
                                   aThrottleLeverPosition,
                                   aSpdBrakeLeverPosition,
                                   aSpoilerLeverPosition,
                                   aFlapsLeverPosition,
                                   aLandingGearLeverPosition);

      if (mStdThrustReverserPtr)
      {
         if (mStdThrustReverserPtr->controlInput)
         {
            mStdThrustReverserPtr->controlInput->normalizedValue = (float)aThrustReverserLeverPosition;
         }
      }

      if (mStdThrustVectoringYawPtr)
      {
         if (mStdThrustVectoringYawPtr->controlInput)
         {
            mStdThrustVectoringYawPtr->controlInput->normalizedValue = (float)aThrustVectorYawRightPosition;
         }
      }

      if (mStdThrustVectoringPitchPtr)
      {
         if (mStdThrustVectoringPitchPtr->controlInput)
         {
            mStdThrustVectoringPitchPtr->controlInput->normalizedValue = (float)aThrustVectorPitchUpPosition;
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = (float)aThrustVectorRollRightPosition;
         }
      }

      if (mStdNoseWheelSteeringPtr)
      {
         if (mStdNoseWheelSteeringPtr->controlInput)
         {
            mStdNoseWheelSteeringPtr->controlInput->normalizedValue = (float)aNoseWheelSteeringRightPosition;
         }
      }

      if (mStdNWS_SteeringPtr)
      {
         if (mStdNWS_SteeringPtr->controlInput)
         {
            // Note that mStdNWS_SteeringPtr uses the same input as for mStdNoseWheelSteeringPtr
            mStdNWS_SteeringPtr->controlInput->normalizedValue = (float)aNoseWheelSteeringRightPosition;
         }
      }

      if (mStdNWS_EnabledPtr)
      {
         if (mStdNWS_EnabledPtr->controlInput)
         {
            mStdNWS_EnabledPtr->controlInput->boolData = aNWS_Enabled;
         }
      }

      if (mStdWheelBrakeLeftPtr)
      {
         if (mStdWheelBrakeLeftPtr->controlInput)
         {
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = (float)aWheelBrakeLeftPosition;
         }
      }

      if (mStdWheelBrakeRightPtr)
      {
         if (mStdWheelBrakeRightPtr->controlInput)
         {
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = (float)aWheelBrakeRightPosition;
         }
      }
   }
}

void P6DofPilotObject::SetExternalDirectControlTrimManualControlData(double aNoseUpTrimDeltaT_sec,
                                                                     double aRollRightTrimDeltaT_sec,
                                                                     double aYawRightTrimDeltaT_sec)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mNoseUpTrimDeltaT_sec += aNoseUpTrimDeltaT_sec;
   mRollRightTrimDeltaT_sec += aRollRightTrimDeltaT_sec;
   mYawRightTrimDeltaT_sec += aYawRightTrimDeltaT_sec;
}


void P6DofPilotObject::TakeTestControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed
   mTestControl = true;
}

void P6DofPilotObject::ReleaseTestControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed
   mTestControl = false;
}

double P6DofPilotObject::GetStickBackControllerPosition() const
{
   if (mStdStickBackPtr)
   {
      if (mStdStickBackPtr->controlInput)
      {
         return (double)mStdStickBackPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

void P6DofPilotObject::SetTestStickBackControllerPosition(double aStickAftPosition)
{
   if (mTestControl)
   {
      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            mStdStickBackPtr->controlInput->normalizedValue = (float)aStickAftPosition;
         }
      }
   }
}

double P6DofPilotObject::GetStickRightControllerPosition() const
{
   if (mStdStickRightPtr)
   {
      if (mStdStickRightPtr->controlInput)
      {
         return (double)mStdStickRightPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

void P6DofPilotObject::SetTestStickRightControllerPosition(double aStickRightPosition)
{
   if (mTestControl)
   {
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            mStdStickRightPtr->controlInput->normalizedValue = (float)aStickRightPosition;
         }
      }
   }
}

double P6DofPilotObject::GetRudderRightControllerPosition() const
{
   if (mStdRudderRightPtr)
   {
      if (mStdRudderRightPtr->controlInput)
      {
         return (double)mStdRudderRightPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

void P6DofPilotObject::SetTestRudderRightControllerPosition(double aRudderRightPosition)
{
   if (mTestControl)
   {
      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            mStdRudderRightPtr->controlInput->normalizedValue = (float)aRudderRightPosition;
         }
      }
   }
}

double P6DofPilotObject::GetSpeedBrakesControllerPosition() const
{
   if (mStdSpeedBrakesOutPtr)
   {
      if (mStdSpeedBrakesOutPtr->controlInput)
      {
         return (double)mStdSpeedBrakesOutPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

void P6DofPilotObject::SetTestSpeedBrakesControllerPosition(double aSpeedBrakesPosition)
{
   if (mTestControl)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = (float)aSpeedBrakesPosition;
         }
      }
   }
}

double P6DofPilotObject::GetFlapsControllerPosition() const
{
   if (mStdFlapsDownPtr)
   {
      if (mStdFlapsDownPtr->controlInput)
      {
         return (double)mStdFlapsDownPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

void P6DofPilotObject::SetTestFlapsControllerPosition(double aFlapsPosition)
{
   if (mTestControl)
   {
      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            float value = (float)aFlapsPosition;
            if (value < 0.0)
            {
               value = 0.0;
            }
            else if (value > 1.0)
            {
               value = 1.0;
            }
            mStdFlapsDownPtr->controlInput->normalizedValue = value;
         }
      }
   }
}

void P6DofPilotObject::SetTestSpoilersControllerPosition(double aSpoilersPosition)
{
   if (mTestControl)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            float value = (float)aSpoilersPosition;
            if (value < 0.0)
            {
               value = 0.0;
            }
            else if (value > 1.0)
            {
               value = 1.0;
            }
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = (float)value;
         }
      }
   }
}

void P6DofPilotObject::SetTestLandingGearControllerPosition(double aLandingGearPosition)
{
   if (mTestControl)
   {
      if (mStdLandingGearDownPtr)
      {
         if (mStdLandingGearDownPtr->controlInput)
         {
            float value = (float)aLandingGearPosition;
            if (value < 0.0)
            {
               value = 0.0;
            }
            else if (value > 1.0)
            {
               value = 1.0;
            }
            mStdLandingGearDownPtr->controlInput->normalizedValue = (float)value;
         }
      }
   }
}

double P6DofPilotObject::GetThrottleMilitaryControllerPosition() const
{
   if (mStdThrottleMilPtr)
   {
      if (mStdThrottleMilPtr->controlInput)
      {
         return mStdThrottleMilPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

double P6DofPilotObject::GetThrottleAfterburnerControllerPosition() const
{
   if (mStdThrottleABPtr)
   {
      if (mStdThrottleABPtr->controlInput)
      {
         return mStdThrottleABPtr->controlInput->normalizedValue;
      }
   }

   return 0.0;
}

double P6DofPilotObject::GetThrottleControllerPosition() const
{
   double throttleMil = 0;
   double throttleAB  = 0;

   if (mStdThrottleMilPtr)
   {
      if (mStdThrottleMilPtr->controlInput)
      {
         throttleMil = mStdThrottleMilPtr->controlInput->normalizedValue;
      }
   }

   if (mStdThrottleABPtr)
   {
      if (mStdThrottleABPtr->controlInput)
      {
         throttleAB = mStdThrottleABPtr->controlInput->normalizedValue;
      }
   }

   // The throttle position has 1.0=MIL and 2.0=AB

   double throttle = 0;

   if (throttleAB > 0)
   {
      throttle = 1.0 + throttleAB;
   }
   else
   {
      throttle = throttleMil;
   }

   return throttle;
}

void P6DofPilotObject::SetTestThrottleControllerPosition(double aThrottlePosition)
{
   if (mTestControl)
   {
      SetThrottleData(aThrottlePosition);
   }
}

void P6DofPilotObject::SetTestThrottleMilitaryControllerPosition(double aThrottlePosition)
{
   if (mTestControl)
   {
      SetThrottleMilitaryData(aThrottlePosition);
   }
}

void P6DofPilotObject::SetTestThrottleAfterburnerControllerPosition(double aThrottlePosition)
{
   if (mTestControl)
   {
      SetThrottleAfterburnerData(aThrottlePosition);
   }
}

void P6DofPilotObject::SetPlannedRoute(P6DofRoute* aRoute)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mPlannedRoutePtr != nullptr)
   {
      delete mPlannedRoutePtr;
   }
   mPlannedRoutePtr = aRoute;

   if (mPlannedRoutePtr != nullptr)
   {
      if (mPlannedRoutePtr->GetNumberOfWaypoints() >= 1)
      {
         const P6DofWaypoint* first = mPlannedRoutePtr->GetFirstElement();

         mState.SetPositionLLA(first->GetLLA().GetLat(), first->GetLLA().GetLat(), first->GetLLA().GetAlt());

         // We set the speed based on speed-type from the waypoint speed pairing -- only one of these speeds should be > zero.
         if (first->GetSpeed().type == P6DofWaypoint::SPD_TYPE_TAS_KNOTS)
         {
            mState.SetSpeed_ktas(first->GetSpeed().val);
         }
         else if (first->GetSpeed().type == P6DofWaypoint::SPD_TYPE_CAS_KNOTS)
         {
            mState.SetSpeed_kias(first->GetSpeed().val);
         }
         else if (first->GetSpeed().type == P6DofWaypoint::SPD_TYPE_MACH)
         {
            mState.SetSpeed_Mach(first->GetSpeed().val);
         }
         else if (first->GetSpeed().type == P6DofWaypoint::SPD_TYPE_FPS)
         {
            mState.SetSpeed_fps(first->GetSpeed().val);
         }
         else if (first->GetSpeed().type == P6DofWaypoint::SPD_TYPE_MPH)
         {
            mState.SetSpeed_mph(first->GetSpeed().val);
         }
         else if (first->GetSpeed().type == P6DofWaypoint::SPD_TYPE_MPS)
         {
            mState.SetSpeed_mps(first->GetSpeed().val);
         }

         mPlannedPrevWptDataPtr = first;
      }

      if (mPlannedRoutePtr->GetNumberOfWaypoints() >= 2)
      {
         const P6DofWaypoint* first  = mPlannedRoutePtr->GetFirstElement();
         const P6DofWaypoint* second = mPlannedRoutePtr->GetNextWaypoint(first);

         double heading_deg, distance;

         UtSphericalEarth::GreatCircleHeadingAndDistance(first->GetLLA().GetLat(),
                                                         first->GetLLA().GetLon(),
                                                         second->GetLLA().GetLat(),
                                                         second->GetLLA().GetLon(),
                                                         heading_deg,
                                                         distance);
         double deltaAlt = second->GetLLA().GetAlt() - first->GetLLA().GetAlt();

         double pitchRad = atan2(deltaAlt, distance);
         double rollRad  = 0;

         mState.SetAttitude_rad(heading_deg * UtMath::cRAD_PER_DEG, pitchRad, rollRad);
         mState.SetSpeed_ktas(cos(pitchRad) * mState.GetSpeed_ktas());
         mState.SetVerticalSpeed_fpm((mState.GetSpeed_fps() * sin(pitchRad)) / 60.0);

         mPlannedPrevWptDataPtr = first;
         mPlannedCurrWptDataPtr = second;
      }

      if (mPlannedRoutePtr->GetNumberOfWaypoints() >= 3)
      {
         const P6DofWaypoint* first  = mPlannedRoutePtr->GetFirstElement();
         const P6DofWaypoint* second = mPlannedRoutePtr->GetNextWaypoint(first);
         const P6DofWaypoint* third  = mPlannedRoutePtr->GetNextWaypoint(second);

         mPlannedPrevWptDataPtr = first;
         mPlannedCurrWptDataPtr = second;
         mPlannedNextWptDataPtr = third;
      }

      if (mAutopilotActionPtr == nullptr)
      {
         mAutopilotActionPtr = new P6DofAutopilotAction;
      }
      mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                           mPlannedCurrWptDataPtr,
                                           mPlannedNextWptDataPtr,
                                           mPlannedRoutePtr->GetRouteSegment(mPlannedPrevWptDataPtr),
                                           mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr));

      if (mCommonController != nullptr)
      {
         mCommonController->SetCurrentActivity(mAutopilotActionPtr);
      }

      mCurrentPlannedWaypointIndex = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);
   }
}

void P6DofPilotObject::SetTempRoute(P6DofRoute* aRoute)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mTempRoutePtr != nullptr)
   {
      delete mTempRoutePtr;
   }
   mTempRoutePtr = aRoute;
}

void P6DofPilotObject::FlyTempWaypoints(P6DofRoute* aRoute)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mCommonController)
   {
      if (mTempRoutePtr != nullptr)
      {
         delete mTempRoutePtr;
         mTempRoutePtr = nullptr;
      }

      if (aRoute->GetNumberOfWaypoints() >= 2)
      {
         P6DofWaypoint* prev   = new P6DofWaypoint();
         UtLLAPos       prevPt = mState.GetCurrentPositionLLA();

         prev->SetLLA(prevPt);

         P6DofRoute* newRoute = aRoute;
         newRoute->AddWaypointToRouteStart(prev);
         newRoute->ComputeSegmentMap();

         mTempRoutePtr = newRoute;

         mTempPrevWptDataPtr = mTempRoutePtr->GetFirstElement();
         mTempCurrWptDataPtr = mTempRoutePtr->GetNextWaypoint(mTempPrevWptDataPtr);
         mTempNextWptDataPtr = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

         if (mTempActionPtr == nullptr)
         {
            mTempActionPtr = new P6DofAutopilotAction;
         }

         if (mTempPrevWptDataPtr != nullptr && mTempCurrWptDataPtr != nullptr)
         {
            mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                            mTempCurrWptDataPtr,
                                            mTempNextWptDataPtr,
                                            mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr),
                                            mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr));
            mCommonController->SetCurrentActivity(mTempActionPtr);

            mCurrentTempWaypointIndex = mTempRoutePtr->GetWaypointIndex(mTempCurrWptDataPtr);
         }
      }
      else // Store route, but should we do anything else?
      {
         mTempRoutePtr = aRoute;
      }
   }
}

bool P6DofPilotObject::SetPlannedWaypointIndex(size_t aIndex)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   if ((mPlannedRoutePtr != nullptr) && (aIndex < mPlannedRoutePtr->GetNumberOfWaypoints()))
   {
      mCurrentPlannedWaypointIndex = aIndex;
      mPlannedCurrWptDataPtr       = mPlannedRoutePtr->GetWaypointAtIndex(aIndex);
      mPlannedNextWptDataPtr       = mPlannedRoutePtr->GetNextWaypoint(mPlannedCurrWptDataPtr);

      // Use the current position to transition to a new current waypoint
      P6DofWaypoint* transitionWaypoint = new P6DofWaypoint(mState.GetLat(), mState.GetLon(), mState.GetAlt_m());
      transitionWaypoint->SetSpeed(mPlannedPrevWptDataPtr->GetSpeed());
      P6DofRoute::sRouteSegment* transitionSegment =
         P6DofRoute::CalcSegmentGeometry(transitionWaypoint, mPlannedCurrWptDataPtr);

      // Add the transition waypoint and segment into the transition route so that
      // they are properly destroyed
      mTransitionRoute.AddWaypointToRouteEnd(transitionWaypoint);
      mTransitionRoute.AddSegment(transitionWaypoint, transitionSegment);

      P6DofRoute::sRouteSegment* nextSegment =
         mPlannedRoutePtr->CalcSegmentGeometry(mPlannedCurrWptDataPtr, mPlannedNextWptDataPtr);

      mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                           mPlannedCurrWptDataPtr,
                                           mPlannedNextWptDataPtr,
                                           transitionSegment,
                                           nextSegment);

      mCommonController->SetCurrentActivity(mAutopilotActionPtr);

      return true;
   }

   return false;
}

bool P6DofPilotObject::SetPositionToPlannedWaypoint(size_t aIndex)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   if ((mPlannedRoutePtr != nullptr) && (aIndex < mPlannedRoutePtr->GetNumberOfWaypoints()))
   {
      // Set waypoint data
      mCurrentPlannedWaypointIndex = aIndex + 1;
      mPlannedPrevWptDataPtr       = mPlannedRoutePtr->GetWaypointAtIndex(aIndex);
      mPlannedCurrWptDataPtr       = mPlannedRoutePtr->GetNextWaypoint(mPlannedPrevWptDataPtr);
      mPlannedNextWptDataPtr       = mPlannedRoutePtr->GetNextWaypoint(mPlannedCurrWptDataPtr);

      P6DofRoute::sRouteSegment* currSegment = mPlannedRoutePtr->GetRouteSegment(mPlannedPrevWptDataPtr);
      P6DofRoute::sRouteSegment* nextSegment = mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr);

      // Set object state
      double lat   = mPlannedPrevWptDataPtr->GetLLA().GetLat();
      double lon   = mPlannedPrevWptDataPtr->GetLLA().GetLon();
      double alt_m = mPlannedPrevWptDataPtr->GetLLA().GetAlt();

      UtVec3d normDirectionVec;
      currSegment->trackVector.Normalize(normDirectionVec);

      double heading_rad = currSegment->trackStartHdg_rad;
      double pitch_rad   = currSegment->slope_rad;
      double speed_fps   = 0.0;
      double speed_mps   = 0.0;

      P6DofAtmosphere* atm = GetScenario()->GetAtmosphere();

      P6DofWaypoint::sSpeed speed = mPlannedCurrWptDataPtr->GetSpeed();

      if (speed.type == P6DofWaypoint::SPD_TYPE_CAS_KNOTS)
      {
         double speed_kias = speed.val;
         speed_fps         = atm->CalcFpsFromKcas(alt_m * UtMath::cFT_PER_M, speed_kias);
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == P6DofWaypoint::SPD_TYPE_TAS_KNOTS)
      {
         double speed_ktas = speed.val;
         speed_fps         = atm->CalcFpsFromKtas(speed_ktas);
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == P6DofWaypoint::SPD_TYPE_MACH)
      {
         double mach = speed.val;
         speed_fps   = atm->CalcFpsFromMach(alt_m * UtMath::cFT_PER_M, mach);
         speed_mps   = speed_fps * UtMath::cM_PER_FT;
      }
      else
      {
         // error -- no speed specified
         return false;
      }

      normDirectionVec = normDirectionVec * speed_mps * -1.0;

      SetObjectState(lat, lon, alt_m, normDirectionVec[0], normDirectionVec[1], normDirectionVec[2], heading_rad, pitch_rad, 0.0);

      // Set autopilot activity
      mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                           mPlannedCurrWptDataPtr,
                                           mPlannedNextWptDataPtr,
                                           currSegment,
                                           nextSegment);

      mCommonController->SetCurrentActivity(mAutopilotActionPtr);

      return true;
   }

   return false;
}

bool P6DofPilotObject::SetTempWaypointIndex(size_t aIndex)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   if ((mTempRoutePtr != nullptr) && (aIndex < mTempRoutePtr->GetNumberOfWaypoints()))
   {
      if (mTempActionPtr == nullptr)
      {
         mTempActionPtr = new P6DofAutopilotAction;
      }

      mCurrentTempWaypointIndex = aIndex;
      mTempCurrWptDataPtr       = mTempRoutePtr->GetWaypointAtIndex(aIndex);
      mTempNextWptDataPtr       = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

      // Use the current position to transition to a new current waypoint
      P6DofWaypoint* transitionWaypoint = new P6DofWaypoint(mState.GetLat(), mState.GetLon(), mState.GetAlt_m());
      transitionWaypoint->SetSpeed(mTempPrevWptDataPtr->GetSpeed());
      P6DofRoute::sRouteSegment* transitionSegment =
         P6DofRoute::CalcSegmentGeometry(transitionWaypoint, mTempCurrWptDataPtr);

      // Add the transition waypoint and segment into the transition route so that
      // they are properly destroyed
      mTransitionRoute.AddWaypointToRouteEnd(transitionWaypoint);
      mTransitionRoute.AddSegment(transitionWaypoint, transitionSegment);

      P6DofRoute::sRouteSegment* nextSegment =
         mTempRoutePtr->CalcSegmentGeometry(mTempCurrWptDataPtr, mTempNextWptDataPtr);

      mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                      mTempCurrWptDataPtr,
                                      mTempNextWptDataPtr,
                                      transitionSegment,
                                      nextSegment);

      mCommonController->SetCurrentActivity(mTempActionPtr);

      return true;
   }

   return false;
}

bool P6DofPilotObject::SetPositionToTempWaypoint(size_t aIndex)
{
   if ((mTempRoutePtr != nullptr) && (aIndex < mTempRoutePtr->GetNumberOfWaypoints()))
   {
      if (mTempActionPtr == nullptr)
      {
         mTempActionPtr = new P6DofAutopilotAction;
      }

      // Set waypoint data
      mCurrentTempWaypointIndex = aIndex + 1;
      mTempPrevWptDataPtr       = mTempRoutePtr->GetWaypointAtIndex(aIndex);
      mTempCurrWptDataPtr       = mTempRoutePtr->GetNextWaypoint(mTempPrevWptDataPtr);
      mTempNextWptDataPtr       = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

      P6DofRoute::sRouteSegment* currSegment = mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr);
      P6DofRoute::sRouteSegment* nextSegment = mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr);

      // Set object state
      double lat   = mTempPrevWptDataPtr->GetLLA().GetLat();
      double lon   = mTempPrevWptDataPtr->GetLLA().GetLon();
      double alt_m = mTempPrevWptDataPtr->GetLLA().GetAlt();

      UtVec3d normDirectionVec;
      currSegment->trackVector.Normalize(normDirectionVec);

      double heading_rad = currSegment->trackStartHdg_rad;
      double pitch_rad   = currSegment->slope_rad;
      double speed_fps   = 0.0;
      double speed_mps   = 0.0;

      P6DofAtmosphere* atm = GetScenario()->GetAtmosphere();

      P6DofWaypoint::sSpeed speed = mTempCurrWptDataPtr->GetSpeed();

      if (speed.type == P6DofWaypoint::SPD_TYPE_CAS_KNOTS)
      {
         double speed_kias = speed.val;
         speed_fps         = atm->CalcFpsFromKcas(alt_m * UtMath::cFT_PER_M, speed_kias);
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == P6DofWaypoint::SPD_TYPE_TAS_KNOTS)
      {
         double speed_ktas = speed.val;
         speed_fps         = atm->CalcFpsFromKtas(speed_ktas);
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == P6DofWaypoint::SPD_TYPE_MACH)
      {
         double mach = speed.val;
         speed_fps   = atm->CalcFpsFromMach(alt_m * UtMath::cFT_PER_M, mach);
         speed_mps   = speed_fps * UtMath::cM_PER_FT;
      }
      else
      {
         // error -- no speed specified
         return false;
      }

      normDirectionVec = normDirectionVec * speed_mps * -1.0;

      SetObjectState(lat, lon, alt_m, normDirectionVec[0], normDirectionVec[1], normDirectionVec[2], heading_rad, pitch_rad, 0.0);

      // Set autopilot activity
      mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr, mTempCurrWptDataPtr, mTempNextWptDataPtr, currSegment, nextSegment);

      mCommonController->SetCurrentActivity(mTempActionPtr);

      return true;
   }

   return false;
}

void P6DofPilotObject::FlyAtPoint(const UtLLAPos& aPt)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   P6DofWaypoint* prev = new P6DofWaypoint(mState.GetLat(), mState.GetLon(), mState.GetAlt_m());
   P6DofWaypoint* curr = new P6DofWaypoint(aPt.GetLat(), aPt.GetLon(), aPt.GetAlt());

   P6DofRoute* newRoute = new P6DofRoute();

   newRoute->AddWaypointToRouteEnd(prev);
   newRoute->AddWaypointToRouteEnd(curr);
   newRoute->ComputeSegmentMap();

   curr->SetSpeed(P6DofWaypoint::SPD_TYPE_TAS_KNOTS, mState.GetSpeed_ktas());
   curr->SetWaypointOnPassing(true);

   if (mTempRoutePtr != nullptr)
   {
      delete mTempRoutePtr;
   }
   mTempRoutePtr       = newRoute;
   mTempPrevWptDataPtr = prev;
   mTempCurrWptDataPtr = curr;
   mTempNextWptDataPtr = nullptr;

   mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                   mTempCurrWptDataPtr,
                                   mTempNextWptDataPtr,
                                   mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr),
                                   mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr));

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::ReturnToPlannedWaypoints()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mPlannedRoutePtr != nullptr && mPlannedRoutePtr->GetNumberOfWaypoints() >= 3)
   {
      if (mTempActionPtr != nullptr)
      {
         // Transition point is the current location
         P6DofWaypoint* transitionWaypoint = new P6DofWaypoint(mState.GetLat(), mState.GetLon(), mState.GetAlt_m());
         P6DofRoute::sRouteSegment* transitionSegment =
            P6DofRoute::CalcSegmentGeometry(transitionWaypoint, mPlannedCurrWptDataPtr);

         // Add the transition waypoint and segment into the transition route so
         // that they are properly destroyed
         mTransitionRoute.AddWaypointToRouteEnd(transitionWaypoint);
         mTransitionRoute.AddSegment(transitionWaypoint, transitionSegment);

         // Left the planned route while traveling to current waypoint, so set the
         // previous waypoint to the current position and return to the last planned
         // waypoint that was being followed
         mPlannedPrevWptDataPtr = transitionWaypoint;

         if (mPlannedPrevWptDataPtr != nullptr && mPlannedCurrWptDataPtr != nullptr)
         {
            mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                                 mPlannedCurrWptDataPtr,
                                                 mPlannedNextWptDataPtr,
                                                 transitionSegment,
                                                 mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr));

            mCommonController->SetCurrentActivity(mAutopilotActionPtr);
            mCurrentPlannedWaypointIndex = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);
         }

         // Delete the user action and set to null
         delete mTempActionPtr;
         mTempActionPtr = nullptr;
      }
      else
      {
         // already on way-points
      }
   }
   else
   {
      // can not fly way-points
   }
}

//! radial accel & turn direction are ignored for now
void P6DofPilotObject::TurnToHeading(double aHeading_rad,                      // radians
                                     double,                                   // aRadialAccel,   //meters/sec^2
                                     P6DofCommonController::eAutopilotTurnDir) // aTurn)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to heading
   mTempActionPtr->SetHeading_rad((float)aHeading_rad);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Heading);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::GoToSpeed(double aSpeed, // knots true air speed
                                 double)        // aLinearAccel) // meters/sec^2
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to KTAS
   mTempActionPtr->SetTrueAirSpeed_KTAS((float)aSpeed);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::GoToAltitude(double aAltitude, // meters
                                    double)           // aClimbRate)  // meters/sec
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to alt
   mTempActionPtr->SetAltitudeMSL_ft((float)(aAltitude * UtMath::cFT_PER_M));
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Altitude);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

bool P6DofPilotObject::FlyRates(double aRollRate,     // deg/sec
                                double aPitchRate,    // deg/sec
                                double aAcceleration, // feet/sec^2
                                double)               // aSpeed)     //knots (true air speed)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   mTempActionPtr->SetRollRate_dps((float)aRollRate);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::RollRate);

   mTempActionPtr->SetPitchRate_dps((float)aPitchRate); // TODO - pitch rate???
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchRate);

   mTempActionPtr->SetForwardAcceleration_g((float)aAcceleration);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::ForwardAccel);

   mCommonController->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool P6DofPilotObject::FlyHeadingSpeedAltitude(double aHeading_rad, // rad
                                               double aSpeed,       // knots (true air speed)
                                               double aAltitude,    // feet
                                               double,              // aMaxGees,   // Gs
                                               double)              // aMaxClimb)  // feet/minute
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   mTempActionPtr->SetHeading_rad((float)aHeading_rad);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Heading);

   mTempActionPtr->SetAltitudeMSL_ft((float)aAltitude);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Altitude);

   mTempActionPtr->SetTrueAirSpeed_KTAS((float)aSpeed);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);

   mCommonController->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool P6DofPilotObject::FlySpecificTurn(double aRollError, // degrees
                                       double aGees,      // Gs
                                       double aSpeed)     // knots (true air speed)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   // Do we need some limit here?
   // if (aGees < 0.0) { aGees = 0.0; }
   // if (aGees > 8.0) { aGees = 8.0; }

   mTempActionPtr->SetRollDelta_deg((float)aRollError);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::DeltaRoll);

   mTempActionPtr->SetPitchGLoad_g((float)aGees);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchGLoad);

   mTempActionPtr->SetTrueAirSpeed_KTAS((float)aSpeed);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);

   mCommonController->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool P6DofPilotObject::FlySpecificVector(double aRollError,  // degrees
                                         double aPitchError, // degrees
                                         double,             // aMaxGees,   // Gs
                                         double aSpeed)      // knots (true air speed)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   mTempActionPtr->SetRollDelta_deg((float)aRollError);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::DeltaRoll);

   mTempActionPtr->SetDeltaPitch_deg((float)aPitchError);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::DeltaPitch);

   mTempActionPtr->SetTrueAirSpeed_KTAS((float)aSpeed);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);

   // TODO -- Need a means to set a temp g-load to support aMaxGees

   // We need to reset the delta angles
   mCommonController->ResetLateralDeltaAngles();
   mCommonController->ResetDeltaPitchAngle();

   mCommonController->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool P6DofPilotObject::FlyYawPitchAccel(double aYawAccel_g, double aPitchAccel_g)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical and lateral modes

   mTempActionPtr->SetYawGLoad_g((float)aYawAccel_g);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::YawGLoad);

   mTempActionPtr->SetPitchGLoad_g((float)aPitchAccel_g);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchGLoad);

   mCommonController->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool P6DofPilotObject::GetAfterburnerEnabled() const
{
   if (this->mCommonController)
   {
      return mCommonController->GetAfterburnerEnabled();
   }

   return true;
}

void P6DofPilotObject::SetAfterburnerEnabled(bool aEnabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->mCommonController)
   {
      mCommonController->SetAfterburnerEnabled(aEnabled);
   }
}

double P6DofPilotObject::GetAfterburnerThreshold() const
{
   if (this->mCommonController)
   {
      return mCommonController->GetAfterburnerThreshold();
   }

   // No value avail, so return default of 1.0
   return 1.0;
}

void P6DofPilotObject::SetAfterburnerThreshold(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->mCommonController)
   {
      mCommonController->SetAfterburnerThreshold(aValue);
   }
}

bool P6DofPilotObject::GetSpeedBrakeEnabled() const
{
   if (this->mCommonController)
   {
      return mCommonController->GetSpeedBrakeEnabled();
   }

   return true;
}

void P6DofPilotObject::SetSpeedBrakeEnabled(bool aEnabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->mCommonController)
   {
      mCommonController->SetSpeedBrakeEnabled(aEnabled);
   }
}

double P6DofPilotObject::GetSpeedBrakeThreshold() const
{
   if (this->mCommonController)
   {
      return mCommonController->GetSpeedBrakeThreshold();
   }

   // No value avail, so return default of 1.0
   return 0.0;
}

void P6DofPilotObject::SetSpeedBrakeThreshold(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->mCommonController)
   {
      mCommonController->SetSpeedBrakeThreshold(aValue);
   }
}

double P6DofPilotObject::GetTurnRollInMultiplier()
{
   if (this->mCommonController)
   {
      return mCommonController->GetCurrentTurnRollInMultiplier();
   }

   // No value avail, so return default of 1.0
   return 0.0;
}

void P6DofPilotObject::SetTurnRollInMultiplier(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->mCommonController)
   {
      mCommonController->SetCurrentTurnRollInMultiplier(aValue);
   }
}

void P6DofPilotObject::EnableDirectThrottleInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideThrottle = true;
}

void P6DofPilotObject::MoveThrottleToIdle()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleMilitaryData(0.0);
      SetThrottleAfterburnerData(0.0);
   }
}

void P6DofPilotObject::MoveThrottleToFull()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleMilitaryData(1.0);
      SetThrottleAfterburnerData(0.0);
   }
}

void P6DofPilotObject::MoveThrottleToAfterburner()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleMilitaryData(1.0);
      SetThrottleAfterburnerData(1.0);
   }
}

void P6DofPilotObject::ReleaseDirectThrottleInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideThrottle = false;
}

void P6DofPilotObject::EnableDirectSpeedBrakeInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideSpeedBrakes = true;
}

void P6DofPilotObject::OpenSpeedBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = 1.0f;
         }
      }
   }
}

void P6DofPilotObject::CloseSpeedBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = 0.0f;
         }
      }
   }
}

void P6DofPilotObject::SetDirectSpeedBrakesInput(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         float value = (float)aValue;
         if (value < 0.0)
         {
            value = 0.0;
         }
         else if (value > 1.0)
         {
            value = 1.0;
         }
         mStdSpeedBrakesOutPtr->controlInput->normalizedValue = value;
      }
   }
}

void P6DofPilotObject::ReleaseDirectSpeedBrakeInput()
{
   mControlOverrideSpeedBrakes = false;
}

bool P6DofPilotObject::UsingManualControl() const
{
   return mManualControl;
}

void P6DofPilotObject::TakeManualControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mManualControl = true;
}

void P6DofPilotObject::ReleaseManualControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mManualControl = false;
}

bool P6DofPilotObject::UsingExternalDirectControl() const
{
   return mExternalDirectControl;
}

void P6DofPilotObject::TakeExternalDirectControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mExternalDirectControl = true;
}

void P6DofPilotObject::ReleaseExternalDirectControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mExternalDirectControl = false;
}

void P6DofPilotObject::GetPlannedWaypoints(P6DOF::AutopilotWaypointDataList* aWaypointDataList) const
{
   if (mAutopilotActionPtr == nullptr)
   {
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   int num = 0;
   if (mPlannedRoutePtr != nullptr)
   {
      for (std::vector<P6DofWaypoint*>::const_iterator iter = mPlannedRoutePtr->Begin(); iter != mPlannedRoutePtr->End();
           ++iter)
      {
         if (num >= 50)
         {
            break;
         }

         aWaypointDataList->waypointListData[num].lat   = (float)((*iter)->GetLLA().GetLat());
         aWaypointDataList->waypointListData[num].lon   = (float)((*iter)->GetLLA().GetLon());
         aWaypointDataList->waypointListData[num].alt_m = (float)((*iter)->GetLLA().GetAlt());

         if ((*iter)->GetSpeed().type == P6DofWaypoint::SPD_TYPE_CAS_KNOTS)
         {
            aWaypointDataList->waypointListData[num].speed_kias = (float)((*iter)->GetSpeed().val);
            aWaypointDataList->waypointListData[num].speed_ktas = 0;
            aWaypointDataList->waypointListData[num].speed_mach = 0;
         }
         else if ((*iter)->GetSpeed().type == P6DofWaypoint::SPD_TYPE_TAS_KNOTS)
         {
            aWaypointDataList->waypointListData[num].speed_kias = 0;
            aWaypointDataList->waypointListData[num].speed_ktas = (float)((*iter)->GetSpeed().val);
            aWaypointDataList->waypointListData[num].speed_mach = 0;
         }
         else if ((*iter)->GetSpeed().type == P6DofWaypoint::SPD_TYPE_MACH)
         {
            aWaypointDataList->waypointListData[num].speed_kias = 0;
            aWaypointDataList->waypointListData[num].speed_ktas = 0;
            aWaypointDataList->waypointListData[num].speed_mach = (float)((*iter)->GetSpeed().val);
         }

         ++num;
      }
   }
   else
   {
      // No planned route, so return zeros
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   aWaypointDataList->currentWaypointIndex = (unsigned char)mCurrentPlannedWaypointIndex;
   aWaypointDataList->waypointListNumItems = (unsigned char)num;
}

void P6DofPilotObject::GetTempWaypoints(P6DOF::AutopilotWaypointDataList* aWaypointDataList) const
{
   // If we lack an action, simply return
   if (mTempActionPtr == nullptr)
   {
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   // If we lack a route, simply return
   if (mTempRoutePtr == nullptr)
   {
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   int num = 0;
   for (std::vector<P6DofWaypoint*>::const_iterator iter = mTempRoutePtr->Begin(); iter != mTempRoutePtr->End(); ++iter)
   {
      if (num >= 50)
      {
         break;
      }

      aWaypointDataList->waypointListData[num].lat   = (float)((*iter)->GetLLA().GetLat());
      aWaypointDataList->waypointListData[num].lon   = (float)((*iter)->GetLLA().GetLon());
      aWaypointDataList->waypointListData[num].alt_m = (float)((*iter)->GetLLA().GetAlt());

      if ((*iter)->GetSpeed().type == P6DofWaypoint::SPD_TYPE_CAS_KNOTS)
      {
         aWaypointDataList->waypointListData[num].speed_kias = (float)((*iter)->GetSpeed().val);
         aWaypointDataList->waypointListData[num].speed_ktas = 0;
         aWaypointDataList->waypointListData[num].speed_mach = 0;
      }
      else if ((*iter)->GetSpeed().type == P6DofWaypoint::SPD_TYPE_TAS_KNOTS)
      {
         aWaypointDataList->waypointListData[num].speed_kias = 0;
         aWaypointDataList->waypointListData[num].speed_ktas = (float)((*iter)->GetSpeed().val);
         aWaypointDataList->waypointListData[num].speed_mach = 0;
      }
      else if ((*iter)->GetSpeed().type == P6DofWaypoint::SPD_TYPE_MACH)
      {
         aWaypointDataList->waypointListData[num].speed_kias = 0;
         aWaypointDataList->waypointListData[num].speed_ktas = 0;
         aWaypointDataList->waypointListData[num].speed_mach = (float)((*iter)->GetSpeed().val);
      }

      ++num;
   }
   aWaypointDataList->currentWaypointIndex = (unsigned char)mCurrentTempWaypointIndex;
   aWaypointDataList->waypointListNumItems = (unsigned char)num;
}

void P6DofPilotObject::GetCurrentWaypoints(P6DOF::AutopilotWaypointDataList* aWaypointDataList) const
{
   if (mTempActionPtr != nullptr)
   {
      GetTempWaypoints(aWaypointDataList);
      return;
   }
   else if (mAutopilotActionPtr != nullptr)
   {
      GetPlannedWaypoints(aWaypointDataList);
      return;
   }

   // If we're here, we must lack any waypoints
   aWaypointDataList->currentWaypointIndex = 0;
   aWaypointDataList->waypointListNumItems = 0;
}

void P6DofPilotObject::SetControlAugmentationModeActive(bool aCASIsActive)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlAugmentationModeActive = aCASIsActive;

   if (mCommonController)
   {
      mCommonController->SetControlAugmentationModeActive(mControlAugmentationModeActive);
   }
}

bool P6DofPilotObject::ControlAugmentationModeIsActive() const
{
   return mControlAugmentationModeActive;
}

void P6DofPilotObject::SetLandingGearControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdLandingGearDownPtr)
   {
      float value = (float)aPosition;
      if (value < 0.0)
      {
         value = 0.0;
      }
      else if (value > 1.0)
      {
         value = 1.0;
      }
      mStdLandingGearDownPtr->controlInput->normalizedValue = value;
   }
}

void P6DofPilotObject::SetFlapsControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdFlapsDownPtr)
   {
      float value = (float)aPosition;
      if (value < 0.0)
      {
         value = 0.0;
      }
      else if (value > 1.0)
      {
         value = 1.0;
      }
      mStdFlapsDownPtr->controlInput->normalizedValue = value;
   }
}

void P6DofPilotObject::SetSpoilersControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdSpoilersOutPtr)
   {
      float value = (float)aPosition;
      if (value < 0.0)
      {
         value = 0.0;
      }
      else if (value > 1.0)
      {
         value = 1.0;
      }
      mStdSpoilersOutPtr->controlInput->normalizedValue = value;
   }
}

void P6DofPilotObject::SetThrustReverserControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mParentVehicle)
   {
      if (mStdThrustReverserPtr)
      {
         P6DofPropulsionSystem* propulsion = mParentVehicle->GetPropulsionSystem();

         if (propulsion)
         {
            if (aPosition <= 0.001)
            {
               propulsion->EnableThrustReverser(false);
            }
            else
            {
               propulsion->EnableThrustReverser(true);
            }

            float value = (float)aPosition;
            if (value < 0.0)
            {
               value = 0.0;
            }
            else if (value > 1.0)
            {
               value = 1.0;
            }

            mStdThrustReverserPtr->controlInput->normalizedValue = value;
         }
      }
   }
}

void P6DofPilotObject::SetEnableNWS(bool aNWS_Enabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdNWS_EnabledPtr)
   {
      // Set float data
      if (aNWS_Enabled)
      {
         mStdNWS_EnabledPtr->controlInput->normalizedValue = 1.0f;
      }
      else
      {
         mStdNWS_EnabledPtr->controlInput->normalizedValue = 0.0f;
      }

      // Set bool data
      mStdNWS_EnabledPtr->controlInput->boolData = aNWS_Enabled;
   }
}

void P6DofPilotObject::EnableDirectBraking()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideWheelBrakes = true;
}

void P6DofPilotObject::SetParkingBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   ApplyLeftGearBrake(1.0);
   ApplyRightGearBrake(1.0);
}

void P6DofPilotObject::ReleaseParkingBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   ApplyLeftGearBrake(0.0);
   ApplyRightGearBrake(0.0);
}

void P6DofPilotObject::ApplyLeftGearBrake(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideWheelBrakes)
   {
      if (mStdWheelBrakeLeftPtr)
      {
         float value = (float)aValue;
         if (value < 0.0)
         {
            value = 0.0;
         }
         else if (value > 1.0)
         {
            value = 1.0;
         }
         mStdWheelBrakeLeftPtr->controlInput->normalizedValue = value;
      }
   }
}

void P6DofPilotObject::ApplyRightGearBrake(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideWheelBrakes)
   {
      if (mStdWheelBrakeRightPtr)
      {
         float value = (float)aValue;
         if (value < 0.0)
         {
            value = 0.0;
         }
         else if (value > 1.0)
         {
            value = 1.0;
         }
         mStdWheelBrakeRightPtr->controlInput->normalizedValue = value;
      }
   }
}

void P6DofPilotObject::ReleaseDirectBraking()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   ApplyLeftGearBrake(0.0);
   ApplyRightGearBrake(0.0);

   mControlOverrideWheelBrakes = false;
}

double P6DofPilotObject::GetLandingGearControllerPosition() const
{
   if (mStdLandingGearDownPtr)
   {
      return mStdLandingGearDownPtr->controlInput->normalizedValue;
   }

   return 0.0;
}

double P6DofPilotObject::GetSpoilersControllerPosition() const
{
   if (mStdSpoilersOutPtr)
   {
      return mStdSpoilersOutPtr->controlInput->normalizedValue;
   }

   return 0.0;
}

double P6DofPilotObject::GetThrustReverserControllerPosition() const
{
   if (mStdThrustReverserPtr)
   {
      return mStdThrustReverserPtr->controlInput->normalizedValue;
   }

   return 0.0;
}

void P6DofPilotObject::SetAutopilotPitchAngle(double aPitchAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetPitchAngle_deg((float)aPitchAngle_deg);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchAng);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotPitchRate(double aPitchRate_dps)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch rate
   mTempActionPtr->SetPitchRate_dps((float)aPitchRate_dps);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchRate);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotFlightPathAngle(double aFlightPathAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to gamma
   mTempActionPtr->SetFlightPathAngle_deg((float)aFlightPathAngle_deg);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::FltPathAng);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotVerticalSpeed(double aVerticalSpeed_fpm)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to vertical speed
   mTempActionPtr->SetVerticalRate_fpm((float)aVerticalSpeed_fpm);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::VertSpeed);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotAltitude(double aAltitude_ft)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to alt
   mTempActionPtr->SetAltitudeMSL_ft((float)aAltitude_ft);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Altitude);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotRollAngle(double aRollAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to roll angle
   mTempActionPtr->SetBank_deg((float)aRollAngle_deg);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Bank);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotRollRate(double aRollRate_dps)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to pitch angle
   mTempActionPtr->SetRollRate_dps((float)aRollRate_dps);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::RollRate);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetPitchGLoad(double aGLoad)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetPitchGLoad_g((float)aGLoad);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::PitchGLoad);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotAlpha(double aAlpha_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetAlpha_deg((float)aAlpha_deg);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Alpha);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotDeltaPitch(double aDeltaPitchAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetDeltaPitch_deg((float)aDeltaPitchAngle_deg);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::DeltaPitch);

   mCommonController->ResetDeltaPitchAngle();
   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotDeltaRoll(double aDeltaRollAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetRollDelta_deg((float)aDeltaRollAngle_deg);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::DeltaRoll);

   mCommonController->ResetDeltaRollAngle();
   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotRollHeading(double aRollHeading_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetHeading_deg((float)aRollHeading_deg);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Heading);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotYawHeading(double aYawHeading_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetHeading_deg((float)aYawHeading_deg);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Heading);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotYawRate(double aYawRate_dps)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetYawRate_dps((float)aYawRate_dps);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::YawRate);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotBeta(double aBeta_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetBeta_deg((float)aBeta_deg);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Beta);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotYawGLoad(double aGLoad)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetYawGLoad_g((float)aGLoad);
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::YawGLoad);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotSpeedKTAS(double aSpeed_KTAS)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to KTAS
   mTempActionPtr->SetTrueAirSpeed_KTAS((float)aSpeed_KTAS);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotSpeedKCAS(double aSpeed_KCAS)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to KCAS
   mTempActionPtr->SetCalibratedAirSpeed_KCAS((float)aSpeed_KCAS);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KIAS);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotSpeedMach(double aSpeed_Mach)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to mach
   mTempActionPtr->SetMach((float)aSpeed_Mach);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::Mach);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotLateralWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to waypoint
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Waypoint);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotVerticalWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to waypoint
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Waypoint);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotSpeedWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to waypoint
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::Waypoint);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

void P6DofPilotObject::SetAutopilotWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   SetAutopilotLateralWaypointMode();
   SetAutopilotVerticalWaypointMode();
   SetAutopilotSpeedWaypointMode();
}

void P6DofPilotObject::SetAutopilotNoControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set all modes to undefined/no-control
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Undefined);
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Undefined);
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::Undefined);

   mCommonController->SetCurrentActivity(mTempActionPtr);
}

bool P6DofPilotObject::ClonePlannedActionToTempAction()
{
   if (mCommonController == nullptr)
   {
      return false;
   }

   // Return if we do not have valid "planned" data
   if (mAutopilotActionPtr == nullptr)
   {
      return false;
   }

   // Delete the temp action, if it currently exists, since
   // we are going to clone the planned data to the temp data
   if (mTempActionPtr != nullptr)
   {
      delete mTempActionPtr;
   }

   mTempActionPtr = mAutopilotActionPtr->Clone();


   if (mPlannedRoutePtr != nullptr)
   {
      if (mTempRoutePtr != nullptr)
      {
         delete mTempRoutePtr;
      }
      mTempRoutePtr = mPlannedRoutePtr->Clone();

      if (mPlannedPrevWptDataPtr != nullptr)
      {
         size_t index        = mPlannedRoutePtr->GetWaypointIndex(mPlannedPrevWptDataPtr);
         mTempPrevWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (mPlannedCurrWptDataPtr != nullptr)
      {
         size_t index        = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);
         mTempCurrWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (mPlannedNextWptDataPtr != nullptr)
      {
         size_t index        = mPlannedRoutePtr->GetWaypointIndex(mPlannedNextWptDataPtr);
         mTempNextWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      mCurrentTempWaypointIndex = mCurrentPlannedWaypointIndex;
   }

   return true;
}

void P6DofPilotObject::SetTempActionToHoldCurrentConditions()
{
   if (mCommonController == nullptr)
   {
      return;
   }

   if (mTempActionPtr == nullptr)
   {
      mTempActionPtr = new P6DofAutopilotAction();
   }

   // Use current heading, altitude, and speed
   mTempActionPtr->SetHeading_rad((float)mState.GetLocalHeading_rad());
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Heading);

   mTempActionPtr->SetAltitudeMSL_ft((float)mState.GetAlt_ft());
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Altitude);

   mTempActionPtr->SetTrueAirSpeed_KTAS((float)mState.GetSpeed_ktas());
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);
}

void P6DofPilotObject::CreateTempActionIfNeeded()
{
   if (mCommonController == nullptr)
   {
      return;
   }

   if (mTempActionPtr != nullptr)
   {
      return;
   }

   mTempActionPtr = new P6DofAutopilotAction();

   // Use current heading, altitude, and speed
   mTempActionPtr->SetHeading_rad((float)mState.GetLocalHeading_rad());
   mTempActionPtr->SetLateralChannelMode(P6DOF::Lateral::Heading);

   mTempActionPtr->SetAltitudeMSL_ft((float)mState.GetAlt_ft());
   mTempActionPtr->SetVerticalChannelMode(P6DOF::Vertical::Altitude);

   mTempActionPtr->SetTrueAirSpeed_KTAS((float)mState.GetSpeed_ktas());
   mTempActionPtr->SetSpeedChannelMode(P6DOF::Speed::KTAS);
}

void P6DofPilotObject::SetTempActionToPlannedDataOrCurrentConditions()
{
   if (mCommonController == nullptr)
   {
      return;
   }

   if (mTempActionPtr != nullptr)
   {
      return;
   }

   if (ClonePlannedActionToTempAction())
   {
      return;
   }

   SetTempActionToHoldCurrentConditions();
}

bool P6DofPilotObject::ControlModeTestingActive() const
{
   return mTestControl;
}

bool P6DofPilotObject::ControlModeDisabledActive() const
{
   if (mTestControl)
   {
      return false;
   }

   return !mControlsEnabled;
}

bool P6DofPilotObject::ControlModeAutopilotActive() const
{
   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   return mAutopilotEnabled;
}

bool P6DofPilotObject::ControlModeManualActive() const
{
   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   if (!mControlAugmentationModeActive)
   {
      if (mAutopilotEnabled)
      {
         return false;
      }
   }

   return mManualControl;
}

bool P6DofPilotObject::ControlModeExternalDirectActive() const
{
   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   if (mManualControl)
   {
      return false;
   }

   if (!mControlAugmentationModeActive)
   {
      if (mAutopilotEnabled)
      {
         return false;
      }
   }

   return mExternalDirectControl;
}

bool P6DofPilotObject::ControlModeControlAugmentationActive() const
{
   if (mCommonController == nullptr)
   {
      return false;
   }

   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   if (mAutopilotEnabled)
   {
      return false;
   }

   if (!mManualControl)
   {
      return false;
   }

   return mControlAugmentationModeActive;
}

bool P6DofPilotObject::ControlModeStabilityAugmentationActive() const
{
   bool anyStabilityAugmentationActive = mPitchStabilityAugmentationModeActive || mYawStabilityAugmentationModeActive ||
                                         mRollStabilityAugmentationModeActive;
   return ControlModeControlAugmentationActive() && anyStabilityAugmentationActive;
}

void P6DofPilotObject::ZeroDisabledControlData()
{
   if (ControlModeDisabledActive())
   {
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            mStdStickRightPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            mStdStickBackPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            mStdRudderRightPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrottleMilPtr)
      {
         if (mStdThrottleMilPtr->controlInput)
         {
            mStdThrottleMilPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrottleABPtr)
      {
         if (mStdThrottleABPtr->controlInput)
         {
            mStdThrottleABPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustReverserPtr)
      {
         if (mStdThrustReverserPtr->controlInput)
         {
            mStdThrustReverserPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustVectoringYawPtr)
      {
         if (mStdThrustVectoringYawPtr->controlInput)
         {
            mStdThrustVectoringYawPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustVectoringPitchPtr)
      {
         if (mStdThrustVectoringPitchPtr->controlInput)
         {
            mStdThrustVectoringPitchPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdSpoilersOutPtr)
      {
         if (mStdSpoilersOutPtr->controlInput)
         {
            mStdSpoilersOutPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            mStdFlapsDownPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdLandingGearDownPtr)
      {
         if (mStdLandingGearDownPtr->controlInput)
         {
            mStdLandingGearDownPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdNoseWheelSteeringPtr)
      {
         if (mStdNoseWheelSteeringPtr->controlInput)
         {
            mStdNoseWheelSteeringPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdNWS_SteeringPtr)
      {
         if (mStdNWS_SteeringPtr->controlInput)
         {
            mStdNWS_SteeringPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdNWS_EnabledPtr)
      {
         if (mStdNWS_EnabledPtr->controlInput)
         {
            mStdNWS_EnabledPtr->controlInput->boolData = false;
         }
      }

      if (mStdWheelBrakeLeftPtr)
      {
         if (mStdWheelBrakeLeftPtr->controlInput)
         {
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = 0.0f;
         }
      }

      if (mStdWheelBrakeRightPtr)
      {
         if (mStdWheelBrakeRightPtr->controlInput)
         {
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = 0.0f;
         }
      }
   }
}

void P6DofPilotObject::SetDestroyed()
{
   mIsDestroyed = true;
}
