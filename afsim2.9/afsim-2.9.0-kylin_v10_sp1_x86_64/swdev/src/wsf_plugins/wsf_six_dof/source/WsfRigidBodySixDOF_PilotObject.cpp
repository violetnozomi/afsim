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

#include "WsfRigidBodySixDOF_PilotObject.hpp"

#include <cstring>

#include "UtException.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfRigidBodySixDOF_FlightControlSystem.hpp"
#include "WsfRigidBodySixDOF_PropulsionSystem.hpp"
#include "WsfRigidBodySixDOF_Utils.hpp"
#include "WsfSixDOF_Environment.hpp"

wsf::six_dof::RigidBodyPilotObject::RigidBodyPilotObject(const RigidBodyPilotObject& aSrc)
   : PilotObject(aSrc)
   , mCommonController(aSrc.mCommonController)
   , mControlsList(aSrc.mControlsList)
   , mControlOverrideWheelBrakes(aSrc.mControlOverrideWheelBrakes)
{
   mStdStickBackPtr            = CopyStandardControlInput(aSrc.mStdStickBackPtr.get());
   mStdStickRightPtr           = CopyStandardControlInput(aSrc.mStdStickRightPtr.get());
   mStdRudderRightPtr          = CopyStandardControlInput(aSrc.mStdRudderRightPtr.get());
   mStdThrottleMilPtr          = CopyStandardControlInput(aSrc.mStdThrottleMilPtr.get());
   mStdThrottleABPtr           = CopyStandardControlInput(aSrc.mStdThrottleABPtr.get());
   mStdThrustReverserPtr       = CopyStandardControlInput(aSrc.mStdThrustReverserPtr.get());
   mStdThrustVectoringYawPtr   = CopyStandardControlInput(aSrc.mStdThrustVectoringYawPtr.get());
   mStdThrustVectoringPitchPtr = CopyStandardControlInput(aSrc.mStdThrustVectoringPitchPtr.get());
   mStdThrustVectoringRollPtr  = CopyStandardControlInput(aSrc.mStdThrustVectoringRollPtr.get());
   mStdSpeedBrakesOutPtr       = CopyStandardControlInput(aSrc.mStdSpeedBrakesOutPtr.get());
   mStdFlapsDownPtr            = CopyStandardControlInput(aSrc.mStdFlapsDownPtr.get());
   mStdSpoilersOutPtr          = CopyStandardControlInput(aSrc.mStdSpoilersOutPtr.get());
   mStdLandingGearDownPtr      = CopyStandardControlInput(aSrc.mStdLandingGearDownPtr.get());
   mStdNoseWheelSteeringPtr    = CopyStandardControlInput(aSrc.mStdNoseWheelSteeringPtr.get());
   mStdNWS_SteeringPtr         = CopyStandardControlInput(aSrc.mStdNWS_SteeringPtr.get());
   mStdNWS_EnabledPtr          = CopyStandardControlInput(aSrc.mStdNWS_EnabledPtr.get());
   mStdWheelBrakeLeftPtr       = CopyStandardControlInput(aSrc.mStdWheelBrakeLeftPtr.get());
   mStdWheelBrakeRightPtr      = CopyStandardControlInput(aSrc.mStdWheelBrakeRightPtr.get());

   if (mCommonController)
   {
      // Set the CAS and SAS modes
      mCommonController->SetControlAugmentationModeActive(mControlAugmentationModeActive);
      mCommonController->SetPitchStabilityAugmentationModeActive(mPitchStabilityAugmentationModeActive);
      mCommonController->SetYawStabilityAugmentationModeActive(mYawStabilityAugmentationModeActive);
      mCommonController->SetRollStabilityAugmentationModeActive(mRollStabilityAugmentationModeActive);
   }
}

std::unique_ptr<wsf::six_dof::RigidBodyPilotObject::InputOutputPair>
wsf::six_dof::RigidBodyPilotObject::CopyStandardControlInput(InputOutputPair* aSourceData)
{
   std::unique_ptr<InputOutputPair> targetData{nullptr};

   if (aSourceData != nullptr)
   {
      // Does the source have a control input?
      if (aSourceData->controlInput != nullptr)
      {
         // We need to find our own control input with the same name as the source control input
         size_t handle = GetControlHandle(aSourceData->controlInput->controlInputName);
         if (handle > 0)
         {
            // Create a new input/output pair
            targetData = ut::make_unique<InputOutputPair>(aSourceData->outputName);

            // We have a match, so set the input to point to our control input
            targetData->controlInput = &(mControlsList.at(handle - 1));

            // This is "copied" input
            targetData->copiedInput = true;
         }
         else
         {
            // No match -- This should not occur
            throw UtException("Unable to match control input");
         }
      }
   }

   return targetData;
}

bool wsf::six_dof::RigidBodyPilotObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (PilotObject::ProcessInput(aInput))
   {
   }
   else if (command == "autopilot_config")
   {
      mCommonController = ut::make_unique<RigidBodyCommonController>(aInput, command);

      if (mCommonController == nullptr)
      {
         auto out = ut::log::error() << "Unable to create mCommonController in RigidBodyPilotObject::ProcessInput().";
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "control_inputs")
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

            FlightControlInputValue controlValue(controlName);
            controlValue.boolData        = false;
            controlValue.normalizedValue = 0;
            controlValue.typeData        = DataType::Float;
            mControlsList.push_back(controlValue);
         }
         else if (localCommand == "std_stick_back")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdStickBackPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_stick_right")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdStickRightPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_rudder_right")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdRudderRightPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_throttle_mil")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrottleMilPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_throttle_ab")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrottleABPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_thrust_reverser")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustReverserPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_thrust_vectoring_yaw")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustVectoringYawPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_thrust_vectoring_pitch")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustVectoringPitchPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_thrust_vectoring_roll")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdThrustVectoringRollPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_speed_brakes_out")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdSpeedBrakesOutPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_flaps_down")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdFlapsDownPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_spoilers_out")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdSpoilersOutPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_landing_gear_down")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdLandingGearDownPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_nose_wheel_steering")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdNoseWheelSteeringPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_nws_steering")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdNWS_SteeringPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_nws_enabled")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdNWS_EnabledPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_wheel_brake_left")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdWheelBrakeLeftPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else if (localCommand == "std_wheel_brake_right")
         {
            std::string controlName;
            aInput.ReadValue(controlName);
            mStdWheelBrakeRightPtr = ut::make_unique<InputOutputPair>(controlName);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RigidBodyPilotObject::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // This will match the standard inputs/outputs with the control in mControlsList
      MatchStdInputsAndOutputs();
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void wsf::six_dof::RigidBodyPilotObject::MatchStdInputsAndOutputs()
{
   if (mStdStickBackPtr)
   {
      if (!MatchInputOutput(mStdStickBackPtr.get()))
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
      if (!MatchInputOutput(mStdStickRightPtr.get()))
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
      if (!MatchInputOutput(mStdRudderRightPtr.get()))
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
      if (!MatchInputOutput(mStdThrottleMilPtr.get()))
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
      if (!MatchInputOutput(mStdThrottleABPtr.get()))
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
      if (!MatchInputOutput(mStdThrustReverserPtr.get()))
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
      if (!MatchInputOutput(mStdThrustVectoringYawPtr.get()))
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
      if (!MatchInputOutput(mStdThrustVectoringPitchPtr.get()))
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
      if (!MatchInputOutput(mStdThrustVectoringRollPtr.get()))
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
      if (!MatchInputOutput(mStdSpeedBrakesOutPtr.get()))
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
      if (!MatchInputOutput(mStdFlapsDownPtr.get()))
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
      if (!MatchInputOutput(mStdSpoilersOutPtr.get()))
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
      if (!MatchInputOutput(mStdLandingGearDownPtr.get()))
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
      if (!MatchInputOutput(mStdNoseWheelSteeringPtr.get()))
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
      if (!MatchInputOutput(mStdNWS_SteeringPtr.get()))
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
      if (!MatchInputOutput(mStdNWS_EnabledPtr.get()))
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
      if (!MatchInputOutput(mStdWheelBrakeLeftPtr.get()))
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
      if (!MatchInputOutput(mStdWheelBrakeRightPtr.get()))
      {
         auto out = ut::log::error() << "Unable to match input with output.";
         out.AddNote() << "Input: StdWheelBrakeRight";
         out.AddNote() << "Output: " << mStdWheelBrakeRightPtr->outputName;
      }

      // Get the handle for the right wheel brake control
      mStdWheelBrakeRightHandle = GetControlHandle(mStdWheelBrakeRightPtr->controlInput->controlInputName);
   }
}

bool wsf::six_dof::RigidBodyPilotObject::Initialize(int64_t) // aSimTime_nanosec)
{
   InitializeCommonController();

   // This will match the standard inputs/outputs with the control in mControlsList
   MatchStdInputsAndOutputs();

   return true;
}

void wsf::six_dof::RigidBodyPilotObject::ManageFrozenControls()
{
   FreezeFlags* freeze = mParentVehicle->GetFreezeFlags();
   if (freeze == nullptr)
   {
      ut::log::error() << "Null Freeze Flags in RigidBodyPilotObject::Update().";
      return;
   }

   if (freeze->speed)
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

   if (freeze->pitch)
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

   if (freeze->roll)
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

   if (freeze->yaw)
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

size_t wsf::six_dof::RigidBodyPilotObject::GetControlHandle(const std::string& aControlName) const
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

double wsf::six_dof::RigidBodyPilotObject::ControlInputValue(size_t aControlInputHandle) const
{
   if ((aControlInputHandle > 0) && (aControlInputHandle <= mControlsList.size()))
   {
      size_t index = aControlInputHandle - 1;
      if (mControlsList.at(index).typeData == DataType::Float)
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

bool wsf::six_dof::RigidBodyPilotObject::ControlInputValueBool(size_t aControlInputHandle) const
{
   if ((aControlInputHandle > 0) && (aControlInputHandle <= mControlsList.size()))
   {
      size_t index = aControlInputHandle - 1;
      if ((mControlsList.at(index).boolData) ||
          ((mControlsList.at(index).typeData == DataType::Float) && (mControlsList.at(index).normalizedValue > 0.5)))
      {
         return true;
      }
   }
   return false;
}

void wsf::six_dof::RigidBodyPilotObject::LoadControlDataWithAutopilotControlData()
{
   // Be sure that controls are within limits
   EnforceControlLimits();

   // Now we go through the "standard" controls and set the control input values
   // using the common controller (autopilot) data, where appropriate.

   if (mStdStickRightPtr)
   {
      if (mStdStickRightPtr->controlInput)
      {
         mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.stickRight);
      }
   }

   if (mStdStickBackPtr)
   {
      if (mStdStickBackPtr->controlInput)
      {
         mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.stickBack);
      }
   }

   if (mStdRudderRightPtr)
   {
      if (mStdRudderRightPtr->controlInput)
      {
         mStdRudderRightPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.rudderRight);
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
               mStdThrottleMilPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.throttleMilitary);
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
               mStdThrottleABPtr->controlInput->normalizedValue =
                  static_cast<float>(mAutopilotControls.throttleAfterburner);
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
               mStdSpeedBrakesOutPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.speedBrake);
            }
         }
      }
   }

   // mStdThrustReverserPtr -- Do not use common controller data

   if (mStdThrustVectoringYawPtr)
   {
      if (mStdThrustVectoringYawPtr->controlInput)
      {
         mStdThrustVectoringYawPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.thrustVectorYaw);
      }
   }

   if (mStdThrustVectoringPitchPtr)
   {
      if (mStdThrustVectoringPitchPtr->controlInput)
      {
         mStdThrustVectoringPitchPtr->controlInput->normalizedValue =
            static_cast<float>(mAutopilotControls.thrustVectorPitch);
      }
   }

   if (mStdThrustVectoringRollPtr)
   {
      if (mStdThrustVectoringRollPtr->controlInput)
      {
         mStdThrustVectoringRollPtr->controlInput->normalizedValue =
            static_cast<float>(mAutopilotControls.thrustVectorRoll);
      }
   }

   // Do not use common controller data for mStdFlapsDownPtr, mStdSpoilersOutPtr,
   // mStdLandingGearDownPtr, and mStdNWS_EnabledPtr

   if (mStdNoseWheelSteeringPtr)
   {
      if (mStdNoseWheelSteeringPtr->controlInput)
      {
         mStdNoseWheelSteeringPtr->controlInput->normalizedValue =
            static_cast<float>(mAutopilotControls.noseWheelSteering);
      }
   }

   if (mStdNWS_SteeringPtr)
   {
      if (mStdNWS_SteeringPtr->controlInput)
      {
         mStdNWS_SteeringPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.nwsSteering);
      }
   }

   if (mStdWheelBrakeLeftPtr)
   {
      // Only set data with common controller data if override is not active
      if (!mControlOverrideWheelBrakes)
      {
         if (mStdWheelBrakeLeftPtr->controlInput)
         {
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.wheelBrakeLeft);
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
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = static_cast<float>(mAutopilotControls.wheelBrakeRight);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::LoadControlDataWithAutopilotStabilityData()
{
   // Be sure that controls are within limits
   EnforceControlLimits();

   // Go through the stick controls and blend the autopilot and manual responses.
   // Inspired by the A-10's SAS construction, SAS may contribute up to 25% stick
   // input in any axis. The remainder is provided by the stick inputs.

   const double autopilotMagnitudeLimit = 0.25;

   if (mStdStickRightPtr && GetCommonController()->RollStabilityAugmentationModeIsActive())
   {
      if (mStdStickRightPtr->controlInput)
      {
         double autopilotMagnitude = std::abs(mAutopilotControls.stickRight);
         autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
         double stickMagnitude     = 1.0 - autopilotMagnitude;

         mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(
            mControlAugmentationStickRight * stickMagnitude + mAutopilotControls.stickRight * autopilotMagnitude);
      }
   }

   if (mStdStickBackPtr && GetCommonController()->PitchStabilityAugmentationModeIsActive())
   {
      if (mStdStickBackPtr->controlInput)
      {
         double autopilotMagnitude = std::abs(mAutopilotControls.stickBack);
         autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
         double stickMagnitude     = 1.0 - autopilotMagnitude;

         mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(
            mControlAugmentationStickBack * stickMagnitude + mAutopilotControls.stickBack * autopilotMagnitude);
      }
   }

   if (mStdRudderRightPtr && GetCommonController()->YawStabilityAugmentationModeIsActive())
   {
      if (mStdRudderRightPtr->controlInput)
      {
         double autopilotMagnitude = std::abs(mAutopilotControls.rudderRight);
         autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
         double stickMagnitude     = 1.0 - autopilotMagnitude;

         mStdRudderRightPtr->controlInput->normalizedValue = static_cast<float>(
            mControlAugmentationRudderRight * stickMagnitude + mAutopilotControls.rudderRight * autopilotMagnitude);
      }
   }
}

bool wsf::six_dof::RigidBodyPilotObject::MatchInputOutput(InputOutputPair* aInputOutput)
{
   if (aInputOutput == nullptr)
   {
      return false;
   }

   for (auto& control : mControlsList)
   {
      std::string controlName(control.controlInputName);

      if (aInputOutput->outputName == controlName)
      {
         // Match found
         aInputOutput->controlInput = &control;
         aInputOutput->copiedInput  = true;
         return true;
      }
   }

   return false;
}

void wsf::six_dof::RigidBodyPilotObject::EnforceControlLimits()
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

void wsf::six_dof::RigidBodyPilotObject::SetThrottleData(double aThrottleLeverPosition)
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
            mStdThrottleABPtr->controlInput->normalizedValue  = static_cast<float>(tempThrottle);
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
            mStdThrottleMilPtr->controlInput->normalizedValue = static_cast<float>(tempThrottle);
         }
         else
         {
            // No AB present, set power for MIL
            mStdThrottleMilPtr->controlInput->normalizedValue = static_cast<float>(tempThrottle);
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

void wsf::six_dof::RigidBodyPilotObject::SetThrottleMilitaryData(double aThrottleLeverPosition)
{
   if (mStdThrottleMilPtr)
   {
      mStdThrottleMilPtr->controlInput->normalizedValue = static_cast<float>(aThrottleLeverPosition);

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

void wsf::six_dof::RigidBodyPilotObject::SetThrottleAfterburnerData(double aThrottleLeverPosition)
{
   if (mStdThrottleABPtr)
   {
      mStdThrottleABPtr->controlInput->normalizedValue = static_cast<float>(aThrottleLeverPosition);

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

void wsf::six_dof::RigidBodyPilotObject::SetManualControlData(double aStickRightPosition,
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

            mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(aStickRightPosition);

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

            mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(aStickAftPosition);

            // Set augmented control input value
            mControlAugmentationStickBack = aStickAftPosition;
         }
      }

      SetThrottleData(aThrottleLeverPosition);

      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = static_cast<float>(aSpdBrakeLeverPosition);
         }
      }

      if (mStdSpoilersOutPtr)
      {
         if (mStdSpoilersOutPtr->controlInput)
         {
            mStdSpoilersOutPtr->controlInput->normalizedValue = static_cast<float>(aSpoilerLeverPosition);
         }
      }

      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            mStdFlapsDownPtr->controlInput->normalizedValue = static_cast<float>(aFlapsLeverPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetManualControlData(double aStickRightPosition,
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

            mStdRudderRightPtr->controlInput->normalizedValue = static_cast<float>(aRudderRightPosition);

            // Set augmented control input value
            mControlAugmentationRudderRight = aRudderRightPosition;
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetManualControlData(double aStickRightPosition,
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
            mStdLandingGearDownPtr->controlInput->normalizedValue = static_cast<float>(aLandingGearLeverPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetManualControlData(double aStickRightPosition,
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
            mStdThrustReverserPtr->controlInput->normalizedValue = static_cast<float>(aThrustReverserLeverPosition);
         }
      }

      if (mStdThrustVectoringYawPtr)
      {
         if (mStdThrustVectoringYawPtr->controlInput)
         {
            mStdThrustVectoringYawPtr->controlInput->normalizedValue = static_cast<float>(aThrustVectorYawRightPosition);
         }
      }

      if (mStdThrustVectoringPitchPtr)
      {
         if (mStdThrustVectoringPitchPtr->controlInput)
         {
            mStdThrustVectoringPitchPtr->controlInput->normalizedValue = static_cast<float>(aThrustVectorPitchUpPosition);
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = static_cast<float>(aThrustVectorRollRightPosition);
         }
      }

      if (mStdNoseWheelSteeringPtr)
      {
         if (mStdNoseWheelSteeringPtr->controlInput)
         {
            mStdNoseWheelSteeringPtr->controlInput->normalizedValue = static_cast<float>(aNoseWheelSteeringRightPosition);
         }
      }

      if (mStdNWS_SteeringPtr)
      {
         if (mStdNWS_SteeringPtr->controlInput)
         {
            // Note that mStdNWS_SteeringPtr uses the same input as for mStdNoseWheelSteeringPtr
            mStdNWS_SteeringPtr->controlInput->normalizedValue = static_cast<float>(aNoseWheelSteeringRightPosition);
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
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = static_cast<float>(aWheelBrakeLeftPosition);
         }
      }

      if (mStdWheelBrakeRightPtr)
      {
         if (mStdWheelBrakeRightPtr->controlInput)
         {
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = static_cast<float>(aWheelBrakeRightPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetExternalDirectControlData(double aStickRightPosition,
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

            mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(aStickRightPosition);

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

            mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(aStickAftPosition);

            // Set augmented control input value
            mControlAugmentationStickBack = aStickAftPosition;
         }
      }

      SetThrottleData(aThrottleLeverPosition);

      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = static_cast<float>(aSpdBrakeLeverPosition);
         }
      }

      if (mStdSpoilersOutPtr)
      {
         if (mStdSpoilersOutPtr->controlInput)
         {
            mStdSpoilersOutPtr->controlInput->normalizedValue = static_cast<float>(aSpoilerLeverPosition);
         }
      }

      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            mStdFlapsDownPtr->controlInput->normalizedValue = static_cast<float>(aFlapsLeverPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetExternalDirectControlData(double aStickRightPosition,
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

            mStdRudderRightPtr->controlInput->normalizedValue = static_cast<float>(aRudderRightPosition);

            // Set augmented control input value
            mControlAugmentationRudderRight = aRudderRightPosition;
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetExternalDirectControlData(double aStickRightPosition,
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
            mStdLandingGearDownPtr->controlInput->normalizedValue = static_cast<float>(aLandingGearLeverPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetExternalDirectControlData(double aStickRightPosition,
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
            mStdThrustReverserPtr->controlInput->normalizedValue = static_cast<float>(aThrustReverserLeverPosition);
         }
      }

      if (mStdThrustVectoringYawPtr)
      {
         if (mStdThrustVectoringYawPtr->controlInput)
         {
            mStdThrustVectoringYawPtr->controlInput->normalizedValue = static_cast<float>(aThrustVectorYawRightPosition);
         }
      }

      if (mStdThrustVectoringPitchPtr)
      {
         if (mStdThrustVectoringPitchPtr->controlInput)
         {
            mStdThrustVectoringPitchPtr->controlInput->normalizedValue = static_cast<float>(aThrustVectorPitchUpPosition);
         }
      }

      if (mStdThrustVectoringRollPtr)
      {
         if (mStdThrustVectoringRollPtr->controlInput)
         {
            mStdThrustVectoringRollPtr->controlInput->normalizedValue = static_cast<float>(aThrustVectorRollRightPosition);
         }
      }

      if (mStdNoseWheelSteeringPtr)
      {
         if (mStdNoseWheelSteeringPtr->controlInput)
         {
            mStdNoseWheelSteeringPtr->controlInput->normalizedValue = static_cast<float>(aNoseWheelSteeringRightPosition);
         }
      }

      if (mStdNWS_SteeringPtr)
      {
         if (mStdNWS_SteeringPtr->controlInput)
         {
            // Note that mStdNWS_SteeringPtr uses the same input as for mStdNoseWheelSteeringPtr
            mStdNWS_SteeringPtr->controlInput->normalizedValue = static_cast<float>(aNoseWheelSteeringRightPosition);
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
            mStdWheelBrakeLeftPtr->controlInput->normalizedValue = static_cast<float>(aWheelBrakeLeftPosition);
         }
      }

      if (mStdWheelBrakeRightPtr)
      {
         if (mStdWheelBrakeRightPtr->controlInput)
         {
            mStdWheelBrakeRightPtr->controlInput->normalizedValue = static_cast<float>(aWheelBrakeRightPosition);
         }
      }
   }
}

double wsf::six_dof::RigidBodyPilotObject::GetStickBackControllerPosition() const
{
   if (mStdStickBackPtr)
   {
      if (mStdStickBackPtr->controlInput)
      {
         return static_cast<double>(mStdStickBackPtr->controlInput->normalizedValue);
      }
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyPilotObject::SetTestStickBackControllerPosition(double aStickAftPosition)
{
   if (mTestControl)
   {
      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(aStickAftPosition);
         }
      }
   }
}

double wsf::six_dof::RigidBodyPilotObject::GetStickRightControllerPosition() const
{
   if (mStdStickRightPtr)
   {
      if (mStdStickRightPtr->controlInput)
      {
         return static_cast<double>(mStdStickRightPtr->controlInput->normalizedValue);
      }
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyPilotObject::SetTestStickRightControllerPosition(double aStickRightPosition)
{
   if (mTestControl)
   {
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(aStickRightPosition);
         }
      }
   }
}

double wsf::six_dof::RigidBodyPilotObject::GetRudderRightControllerPosition() const
{
   if (mStdRudderRightPtr)
   {
      if (mStdRudderRightPtr->controlInput)
      {
         return static_cast<double>(mStdRudderRightPtr->controlInput->normalizedValue);
      }
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyPilotObject::SetTestRudderRightControllerPosition(double aRudderRightPosition)
{
   if (mTestControl)
   {
      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            mStdRudderRightPtr->controlInput->normalizedValue = static_cast<float>(aRudderRightPosition);
         }
      }
   }
}

double wsf::six_dof::RigidBodyPilotObject::GetSpeedBrakesControllerPosition() const
{
   if (mStdSpeedBrakesOutPtr)
   {
      if (mStdSpeedBrakesOutPtr->controlInput)
      {
         return static_cast<double>(mStdSpeedBrakesOutPtr->controlInput->normalizedValue);
      }
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyPilotObject::SetTestSpeedBrakesControllerPosition(double aSpeedBrakesPosition)
{
   if (mTestControl)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            mStdSpeedBrakesOutPtr->controlInput->normalizedValue = static_cast<float>(aSpeedBrakesPosition);
         }
      }
   }
}

double wsf::six_dof::RigidBodyPilotObject::GetFlapsControllerPosition() const
{
   if (mStdFlapsDownPtr)
   {
      if (mStdFlapsDownPtr->controlInput)
      {
         return static_cast<double>(mStdFlapsDownPtr->controlInput->normalizedValue);
      }
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyPilotObject::SetTestFlapsControllerPosition(double aFlapsPosition)
{
   if (mTestControl)
   {
      if (mStdFlapsDownPtr)
      {
         if (mStdFlapsDownPtr->controlInput)
         {
            float value = static_cast<float>(aFlapsPosition);
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

void wsf::six_dof::RigidBodyPilotObject::SetTestSpoilersControllerPosition(double aSpoilersPosition)
{
   if (mTestControl)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         if (mStdSpeedBrakesOutPtr->controlInput)
         {
            float value = static_cast<float>(aSpoilersPosition);
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
}

void wsf::six_dof::RigidBodyPilotObject::SetTestLandingGearControllerPosition(double aLandingGearPosition)
{
   if (mTestControl)
   {
      if (mStdLandingGearDownPtr)
      {
         if (mStdLandingGearDownPtr->controlInput)
         {
            float value = static_cast<float>(aLandingGearPosition);
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
   }
}

double wsf::six_dof::RigidBodyPilotObject::GetThrottleMilitaryControllerPosition() const
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

double wsf::six_dof::RigidBodyPilotObject::GetThrottleAfterburnerControllerPosition() const
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

double wsf::six_dof::RigidBodyPilotObject::GetThrottleControllerPosition() const
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

void wsf::six_dof::RigidBodyPilotObject::SetDirectStickBackInput(double aStickBackPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideStickBack)
   {
      if (mStdStickBackPtr)
      {
         if (mStdStickBackPtr->controlInput)
         {
            aStickBackPosition                              = UtMath::Limit(aStickBackPosition, 1.0);
            mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(aStickBackPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetDirectStickRightInput(double aStickRightPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideStickRight)
   {
      if (mStdStickRightPtr)
      {
         if (mStdStickRightPtr->controlInput)
         {
            aStickRightPosition                              = UtMath::Limit(aStickRightPosition, 1.0);
            mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(aStickRightPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::SetDirectRudderRightInput(double aRudderRightPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideRudderRight)
   {
      if (mStdRudderRightPtr)
      {
         if (mStdRudderRightPtr->controlInput)
         {
            aRudderRightPosition                              = UtMath::Limit(aRudderRightPosition, 1.0);
            mStdRudderRightPtr->controlInput->normalizedValue = static_cast<float>(aRudderRightPosition);
         }
      }
   }
}

void wsf::six_dof::RigidBodyPilotObject::OpenSpeedBrake()
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

void wsf::six_dof::RigidBodyPilotObject::CloseSpeedBrake()
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

void wsf::six_dof::RigidBodyPilotObject::SetDirectSpeedBrakesInput(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      if (mStdSpeedBrakesOutPtr)
      {
         float value = static_cast<float>(aValue);
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

void wsf::six_dof::RigidBodyPilotObject::SetLandingGearControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdLandingGearDownPtr)
   {
      float value = static_cast<float>(aPosition);
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

void wsf::six_dof::RigidBodyPilotObject::SetFlapsControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdFlapsDownPtr)
   {
      float value = static_cast<float>(aPosition);
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

void wsf::six_dof::RigidBodyPilotObject::SetSpoilersControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mStdSpoilersOutPtr)
   {
      float value = static_cast<float>(aPosition);
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

void wsf::six_dof::RigidBodyPilotObject::SetThrustReverserControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (GetParentVehicle())
   {
      if (mStdThrustReverserPtr)
      {
         auto propulsion = dynamic_cast<RigidBodyPropulsionSystem*>(mParentVehicle->GetPropulsionSystem());

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

            float value = static_cast<float>(aPosition);
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

void wsf::six_dof::RigidBodyPilotObject::SetEnableNWS(bool aNWS_Enabled)
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

void wsf::six_dof::RigidBodyPilotObject::EnableDirectBraking()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideWheelBrakes = true;
}

void wsf::six_dof::RigidBodyPilotObject::SetParkingBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   ApplyLeftGearBrake(1.0);
   ApplyRightGearBrake(1.0);
}

void wsf::six_dof::RigidBodyPilotObject::ReleaseParkingBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   ApplyLeftGearBrake(0.0);
   ApplyRightGearBrake(0.0);
}

void wsf::six_dof::RigidBodyPilotObject::ApplyLeftGearBrake(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideWheelBrakes)
   {
      if (mStdWheelBrakeLeftPtr)
      {
         float value = static_cast<float>(aValue);
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

void wsf::six_dof::RigidBodyPilotObject::ApplyRightGearBrake(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideWheelBrakes)
   {
      if (mStdWheelBrakeRightPtr)
      {
         float value = static_cast<float>(aValue);
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

void wsf::six_dof::RigidBodyPilotObject::ReleaseDirectBraking()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   ApplyLeftGearBrake(0.0);
   ApplyRightGearBrake(0.0);

   mControlOverrideWheelBrakes = false;
}

double wsf::six_dof::RigidBodyPilotObject::GetLandingGearControllerPosition() const
{
   if (mStdLandingGearDownPtr)
   {
      return mStdLandingGearDownPtr->controlInput->normalizedValue;
   }

   return 0.0;
}

double wsf::six_dof::RigidBodyPilotObject::GetSpoilersControllerPosition() const
{
   if (mStdSpoilersOutPtr)
   {
      return mStdSpoilersOutPtr->controlInput->normalizedValue;
   }

   return 0.0;
}

double wsf::six_dof::RigidBodyPilotObject::GetThrustReverserControllerPosition() const
{
   if (mStdThrustReverserPtr)
   {
      return mStdThrustReverserPtr->controlInput->normalizedValue;
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyPilotObject::ZeroDisabledControlData()
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

void wsf::six_dof::RigidBodyPilotObject::InheritParentControlData()
{
   Mover* vehicle = GetParentVehicle();

   if (vehicle)
   {
      Mover* vehicleParent = vehicle->GetParentVehicle();

      if (vehicleParent)
      {
         if (mStdStickRightPtr)
         {
            if (mStdStickRightPtr->controlInput)
            {
               mStdStickRightPtr->controlInput->normalizedValue = static_cast<float>(vehicleParent->GetStickRightInput());
            }
         }

         if (mStdStickBackPtr)
         {
            if (mStdStickBackPtr->controlInput)
            {
               mStdStickBackPtr->controlInput->normalizedValue = static_cast<float>(vehicleParent->GetStickBackInput());
            }
         }

         if (mStdRudderRightPtr)
         {
            if (mStdRudderRightPtr->controlInput)
            {
               mStdRudderRightPtr->controlInput->normalizedValue =
                  static_cast<float>(vehicleParent->GetRudderRightInput());
            }
         }

         double throttleInput    = vehicleParent->GetThrottleInput();
         double throttleInputMil = UtMath::Limit(throttleInput, 0.0, 1.0);
         double throttleInputAB  = UtMath::Limit(throttleInput - 1.0, 0.0, 1.0);

         if (mStdThrottleMilPtr)
         {
            if (mStdThrottleMilPtr->controlInput)
            {
               mStdThrottleMilPtr->controlInput->normalizedValue = static_cast<float>(throttleInputMil);
            }
         }

         if (mStdThrottleABPtr)
         {
            if (mStdThrottleABPtr->controlInput)
            {
               mStdThrottleABPtr->controlInput->normalizedValue = static_cast<float>(throttleInputAB);
            }
         }

         if (mStdThrustReverserPtr)
         {
            if (mStdThrustReverserPtr->controlInput)
            {
               mStdThrustReverserPtr->controlInput->normalizedValue =
                  static_cast<float>(vehicleParent->GetThrustReverserControlPosition());
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
               mStdSpeedBrakesOutPtr->controlInput->normalizedValue =
                  static_cast<float>(vehicleParent->GetSpeedBrakeControlPosition());
            }
         }

         if (mStdSpoilersOutPtr)
         {
            if (mStdSpoilersOutPtr->controlInput)
            {
               mStdSpoilersOutPtr->controlInput->normalizedValue =
                  static_cast<float>(vehicleParent->GetSpoilersLeverPosition());
            }
         }

         if (mStdFlapsDownPtr)
         {
            if (mStdFlapsDownPtr->controlInput)
            {
               mStdFlapsDownPtr->controlInput->normalizedValue =
                  static_cast<float>(vehicleParent->GetFlapsLeverPosition());
            }
         }

         if (mStdLandingGearDownPtr)
         {
            if (mStdLandingGearDownPtr->controlInput)
            {
               mStdLandingGearDownPtr->controlInput->normalizedValue =
                  static_cast<float>(vehicleParent->GetLandingGearLeverPosition());
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
}
