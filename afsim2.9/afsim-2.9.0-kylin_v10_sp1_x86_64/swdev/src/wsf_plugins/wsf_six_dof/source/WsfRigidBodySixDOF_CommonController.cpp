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

#include "WsfRigidBodySixDOF_CommonController.hpp"

#include <math.h>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfRigidBodySixDOF_GroundReactionPoint.hpp"
#include "WsfRigidBodySixDOF_LandingGear.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_PilotObject.hpp"
#include "WsfSixDOF_AutopilotAction.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_PID.hpp"

wsf::six_dof::RigidBodyCommonController::RigidBodyCommonController(UtInput& aInput, const std::string& aCommand)
   : CommonController()
{
   // Note: mCurrentActivityPtr should be null until set from external source

   ProcessInputCommand(aInput, aCommand);
}

wsf::six_dof::RigidBodyCommonController::RigidBodyCommonController(const RigidBodyCommonController& aSrc)
   : CommonController(aSrc)
   , mCombinedWheelBraking(aSrc.mCombinedWheelBraking)
   , mWheelBrakingLeft(aSrc.mWheelBrakingLeft)
   , mWheelBrakingRight(aSrc.mWheelBrakingRight)
   , mExternalGearControlValue(aSrc.mExternalGearControlValue)
   , mExternalLftWheelBrakeControlValue(aSrc.mExternalLftWheelBrakeControlValue)
   , mExternalRgtWheelBrakeControlValue(aSrc.mExternalRgtWheelBrakeControlValue)
   , mTaxiModeActive(aSrc.mTaxiModeActive)
   , mMinimumTaxiTurnRadius_ft(aSrc.mMinimumTaxiTurnRadius_ft)
   , mDesiredTaxiTurnRadius_ft(aSrc.mDesiredTaxiTurnRadius_ft)
   , mStickForZeroMomentVsMachAlphaTablePtr(aSrc.mStickForZeroMomentVsMachAlphaTablePtr)
   , mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr(aSrc.mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr)
   , mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr(aSrc.mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
   , mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr(aSrc.mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
   , mUseSimpleYawDamper(aSrc.mUseSimpleYawDamper)
{
}

wsf::six_dof::RigidBodyCommonController* wsf::six_dof::RigidBodyCommonController::Clone() const
{
   return new RigidBodyCommonController(*this);
}

wsf::six_dof::Mover* wsf::six_dof::RigidBodyCommonController::GetParentVehicle()
{
   return mParentVehiclePtr;
}

void wsf::six_dof::RigidBodyCommonController::Update(sAutopilotControls& aControls, int64_t aSimTime_nanosec)
{
   // Set the atm pointer, if not yet initialized
   if (mEnvironment == nullptr)
   {
      mEnvironment = mParentVehiclePtr->GetEnvironment();
   }

   CalcAlphaBetaGLimits();

   // Set the controlling value for the pids
   double dynPressure_psf = mParentVehiclePtr->GetDynamicPressure_psf();
   SetControllingValueForAllPIDs(dynPressure_psf);

   // Reset the temp braking values
   mCombinedWheelBraking = 0.0f;
   mWheelBrakingLeft     = 0.0f;
   mWheelBrakingRight    = 0.0f;

   // To determine our primary control action, we determine what to do based on
   // mControlMethod (namely, bank-to-turn or yaw-to-turn) along with the
   // lateral and vertical channel commands. Previous autopilot versions processed
   // the lateral and vertical channels separately -- that approach had many shortcomings.
   // By considering the possible relationship between lateral and vertical commands,
   // a proper control response is possible. In particular, a split-s maneuver is only
   // possible with consideration of both lateral and vertical commands.

   double simTime_sec = utils::TimeToTime(aSimTime_nanosec);

   if (Control::IsBankToTurn((mControlMethod)))
   {
      UpdateBankToTurn(aControls, simTime_sec);
   }
   else if (Control::IsYawToTurn((mControlMethod)))
   {
      UpdateYawToTurn(aControls, simTime_sec);
   }
   else
   {
      // Control::Undefined (no control)
   }

   // Set wheel brakes based on the temp braking values

   // Check for any significant differential braking
   if ((mWheelBrakingLeft > 0.01) || (mWheelBrakingRight > 0.01))
   {
      // Differential braking is present
      mControlOutputs.wheelBrakeLeft  = mWheelBrakingLeft + 0.8f * mCombinedWheelBraking;
      mControlOutputs.wheelBrakeRight = mWheelBrakingRight + 0.8f * mCombinedWheelBraking;
   }
   else
   {
      // No differential braking is present
      mControlOutputs.wheelBrakeLeft  = mCombinedWheelBraking;
      mControlOutputs.wheelBrakeRight = mCombinedWheelBraking;
   }

   // Enforce control limits
   EnforceControlLimits();

   // Return the current controls
   aControls = mControlOutputs;

   mLastUpdateTime_nanosec = aSimTime_nanosec;
}

void wsf::six_dof::RigidBodyCommonController::SetTaxiMode(bool aTaxiModeActive)
{
   mTaxiModeActive = aTaxiModeActive;
}

bool wsf::six_dof::RigidBodyCommonController::GetTaxiMode() const
{
   return mTaxiModeActive;
}

void wsf::six_dof::RigidBodyCommonController::ProcessLaternalNavChannelsBankToTurn(double aSimTime)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
      switch (mCurrentActivityPtr->GetLateralChannelMode())
      {
      case Lateral::Waypoint:
         ProcessLateralNavMode_Taxi_Waypoint(aSimTime);
         break;

         // case Lateral::Taxi_Waypoint:
         //    ProcessLateralNavMode_Taxi_Waypoint(aSimTime);
         //    break;

      case Lateral::Heading:
         ProcessLateralNavMode_Taxi_Heading(aSimTime);
         break;

         // case Lateral::Taxi_Heading:
         //    ProcessLateralNavMode_Taxi_Heading(aSimTime);
         //    break;

      case Lateral::YawRate:
         ProcessLateralNavMode_Taxi_YawRate(aSimTime);
         break;

         // case Lateral::Taxi_YawRate:
         //    ProcessLateralNavMode_Taxi_YawRate(aSimTime);
         //    break;

      default:
         ProcessLateralNavMode_NoControl();
         break;
      }
   }
   else
   {
      bool stabilizingChannelIsControlled = false;

      // Process the navigational channel
      switch (mCurrentActivityPtr->GetLateralChannelMode())
      {
      case Lateral::Waypoint:
         ProcessLateralNavMode_RollWaypoint(aSimTime);
         break;
      case Lateral::Heading:
         ProcessLateralNavMode_RollHeading(aSimTime);
         break;
      case Lateral::Point:
         ProcessLateralNavMode_RollPoint(aSimTime);
         break;
      case Lateral::RollRate:
         ProcessLateralNavMode_RollRate(aSimTime);
         break;
      case Lateral::Bank:
         ProcessLateralNavMode_Bank(aSimTime);
         break;
      case Lateral::DeltaRoll:
         ProcessLateralNavMode_DeltaRoll(aSimTime);
         break;
      // Yaw cases here are mostly vestigial, but are
      // still needed to tune stabilizing channels
      case Lateral::YawGLoad:
         ProcessLateralNavMode_YawGLoad(aSimTime);
         stabilizingChannelIsControlled = true;
         break;
      case Lateral::YawRate:
         ProcessLateralNavMode_YawRate(aSimTime);
         stabilizingChannelIsControlled = true;
         break;
      case Lateral::Beta:
         ProcessLateralNavMode_Beta(aSimTime);
         stabilizingChannelIsControlled = true;
         break;
      default:
         ProcessLateralNavMode_NoRollControl();
         break;
      }

      // Process the stabilizing channel, if requested
      if (!stabilizingChannelIsControlled)
      {
         switch (mCurrentActivityPtr->GetStabilizingChannelMode())
         {
         case Lateral::YawGLoad:
            ProcessLateralNavMode_YawGLoad(aSimTime);
            break;
         case Lateral::YawRate:
            ProcessLateralNavMode_YawRate(aSimTime);
            break;
         case Lateral::Beta:
            ProcessLateralNavMode_Beta(aSimTime);
            break;
         case Lateral::Undefined:
            // In lieu of an explicit stabilizing control, use the control type
            switch (mControlMethod)
            {
            case Control::BankToTurnWithYaw:
               ProcessStandardLateralNavMode_Beta(0.0, aSimTime);
               break;
            case Control::BankToTurnNoYaw:
            default:
               ProcessLateralNavMode_NoYawControl();
               break;
            }
            break;
         default:
            ProcessLateralNavMode_NoYawControl();
            break;
         }
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::ProcessVerticalNavChannelBankToTurn(double aSimTime)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
      switch (mCurrentActivityPtr->GetVerticalChannelMode())
      {
      case Vertical::Waypoint:
         ProcessVerticalNavMode_Waypoint(aSimTime);
         break;
      case Vertical::Altitude:
         ProcessVerticalNavMode_Altitude(aSimTime);
         break;
      case Vertical::VertSpeed:
         ProcessVerticalNavMode_VertSpeed(aSimTime);
         break;
      case Vertical::Point:
         ProcessVerticalNavMode_Point(aSimTime);
         break;
      case Vertical::PitchGLoad:
         ProcessVerticalNavMode_PitchGLoad(aSimTime);
         break;
      case Vertical::PitchAng:
         ProcessVerticalNavMode_PitchAng(aSimTime);
         break;
      case Vertical::PitchRate:
         ProcessVerticalNavMode_PitchRate(aSimTime);
         break;
      case Vertical::FltPathAng:
         ProcessVerticalNavMode_FltPathAng(aSimTime);
         break;
      case Vertical::DeltaPitch:
         ProcessVerticalNavMode_DeltaPitch(aSimTime);
         break;
      case Vertical::Alpha:
         ProcessVerticalNavMode_Alpha(aSimTime);
         break;
      default:
         ProcessVerticalNavMode_NoControl(aSimTime);
         break;
      }
   }
   else
   {
      switch (mCurrentActivityPtr->GetVerticalChannelMode())
      {
      case Vertical::Waypoint:
         ProcessVerticalNavMode_Waypoint(aSimTime);
         break;
      case Vertical::Altitude:
         ProcessVerticalNavMode_Altitude(aSimTime);
         break;
      case Vertical::VertSpeed:
         ProcessVerticalNavMode_VertSpeed(aSimTime);
         break;
      case Vertical::Point:
         ProcessVerticalNavMode_Point(aSimTime);
         break;
      case Vertical::PitchGLoad:
         ProcessVerticalNavMode_PitchGLoad(aSimTime);
         break;
      case Vertical::PitchAng:
         ProcessVerticalNavMode_PitchAng(aSimTime);
         break;
      case Vertical::PitchRate:
         ProcessVerticalNavMode_PitchRate(aSimTime);
         break;
      case Vertical::FltPathAng:
         ProcessVerticalNavMode_FltPathAng(aSimTime);
         break;
      case Vertical::DeltaPitch:
         ProcessVerticalNavMode_DeltaPitch(aSimTime);
         break;
      case Vertical::Alpha:
         ProcessVerticalNavMode_Alpha(aSimTime);
         break;
      default:
         ProcessVerticalNavMode_NoControl(aSimTime);
         break;
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::ProcessSpeedChannelBankToTurn(double aSimTime)
{
   double speedCommand_g = 0.0;
   bool   controlled     = true;

   if (mTaxiModeActive)
   {
      // TODO-Taxi

      switch (mCurrentActivityPtr->GetSpeedChannelMode())
      {
         // case Speed::Taxi_Waypoint:
         //    speedCommand_g = ProcessSpeedMode_Taxi_Waypoint(aSimTime);
         //    break;
         // case Speed::Taxi_ForwardAccel:
         //    speedCommand_g = ProcessSpeedMode_Taxi_ForwardAccel(aSimTime);
         //    break;
         // case Speed::Taxi_KIAS:
         //    speedCommand_g = ProcessSpeedMode_Taxi_KIAS(aSimTime);
         //    break;
         // case Speed::Taxi_KTAS:
         //    speedCommand_g = ProcessSpeedMode_Taxi_KTAS(aSimTime);
         //    break;

      case Speed::Waypoint:
         // case Speed::Taxi_Waypoint:
         speedCommand_g = ProcessSpeedMode_Taxi_Waypoint(aSimTime);
         break;

      case Speed::ForwardAccel:
         // case Speed::Taxi_ForwardAccel:
         speedCommand_g = ProcessSpeedMode_ForwardAccel(aSimTime);
         break;

      case Speed::KIAS:
         // case Speed::Taxi_KIAS:
         speedCommand_g = ProcessSpeedMode_KIAS(aSimTime);
         break;

      case Speed::KTAS:
         // case Speed::Taxi_KTAS:
         speedCommand_g = ProcessSpeedMode_KTAS(aSimTime);
         break;

      case Speed::Mach:
         speedCommand_g = ProcessSpeedMode_Mach(aSimTime);
         break;

      case Speed::FPS:
         speedCommand_g = ProcessSpeedMode_FPS(aSimTime);
         break;

      case Speed::Throttle:
         speedCommand_g = ProcessSpeedMode_Throttle(aSimTime);
         break;

      default:
         speedCommand_g = ProcessSpeedMode_NoControl();
         controlled     = false;
         break;
      }

      // TODO-Taxi -- need to add the threshold for wheel brake use

      // Set wheel brakes
      if (speedCommand_g < 0.0)
      {
         mCombinedWheelBraking = static_cast<double>(mCurrentLimitsAndSettings.speedBrakeThreshold) - speedCommand_g;
      }
   }
   else
   {
      switch (mCurrentActivityPtr->GetSpeedChannelMode())
      {
      case Speed::Waypoint:
         speedCommand_g = ProcessSpeedMode_Waypoint(aSimTime);
         break;
      case Speed::ForwardAccel:
         speedCommand_g = ProcessSpeedMode_ForwardAccel(aSimTime);
         break;
      case Speed::KIAS:
         speedCommand_g = ProcessSpeedMode_KIAS(aSimTime);
         break;
      case Speed::KTAS:
         speedCommand_g = ProcessSpeedMode_KTAS(aSimTime);
         break;
      case Speed::Mach:
         speedCommand_g = ProcessSpeedMode_Mach(aSimTime);
         break;
      case Speed::FPS:
         speedCommand_g = ProcessSpeedMode_FPS(aSimTime);
         break;
      case Speed::Throttle:
         speedCommand_g = ProcessSpeedMode_Throttle(aSimTime);
         break;
      default:
         speedCommand_g = ProcessSpeedMode_NoControl();
         controlled     = false;
         break;
      }
   }

   if (controlled)
   {
      // Set throttles and speed brakes
      if (mCurrentLimitsAndSettings.enableAfterburnerAutoControl &&
          (speedCommand_g > mCurrentLimitsAndSettings.afterburnerThreshold))
      {
         // Use afterburner
         mControlOutputs.throttleAfterburner =
            speedCommand_g - static_cast<double>(mCurrentLimitsAndSettings.afterburnerThreshold);
         mControlOutputs.throttleMilitary = 1;
         mControlOutputs.speedBrake       = 0;
      }
      else if (mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl &&
               (speedCommand_g < mCurrentLimitsAndSettings.speedBrakeThreshold))
      {
         // Use speed brake
         mControlOutputs.throttleAfterburner = 0;
         mControlOutputs.throttleMilitary    = 0;
         mControlOutputs.speedBrake = static_cast<double>(mCurrentLimitsAndSettings.speedBrakeThreshold) - speedCommand_g;
      }
      else
      {
         // normal range
         mControlOutputs.throttleAfterburner = 0;
         mControlOutputs.throttleMilitary    = speedCommand_g;
         mControlOutputs.speedBrake          = 0;
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::ProcessLaternalNavChannelsYawToTurn(double aSimTime)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
   }
   else
   {
      bool stabilizingChannelIsControlled = false;

      switch (mCurrentActivityPtr->GetLateralChannelMode())
      {
      case Lateral::Waypoint:
         ProcessLateralNavMode_YawWaypoint(aSimTime);
         break;
      case Lateral::Heading:
         ProcessLateralNavMode_YawHeading(aSimTime);
         break;
      case Lateral::Point:
         ProcessLateralNavMode_YawPoint(aSimTime);
         break;
      case Lateral::YawGLoad:
         ProcessLateralNavMode_YawGLoad(aSimTime);
         break;
      case Lateral::YawRate:
         ProcessLateralNavMode_YawRate(aSimTime);
         break;
      case Lateral::Beta:
         ProcessLateralNavMode_Beta(aSimTime);
         break;
      // Roll channels here are mostly vestigial, but are
      // still needed to tune stabilizing channels
      case Lateral::RollRate:
         ProcessLateralNavMode_RollRate(aSimTime);
         stabilizingChannelIsControlled = true;
         break;
      case Lateral::Bank:
         ProcessLateralNavMode_Bank(aSimTime);
         stabilizingChannelIsControlled = true;
         break;
      case Lateral::DeltaRoll:
         ProcessLateralNavMode_DeltaRoll(aSimTime);
         stabilizingChannelIsControlled = true;
         break;
      default:
         ProcessLateralNavMode_NoYawControl();
         break;
      }

      // Process the stabilizing channel, if requested
      if (!stabilizingChannelIsControlled)
      {
         switch (mCurrentActivityPtr->GetStabilizingChannelMode())
         {
         case Lateral::RollRate:
            ProcessLateralNavMode_RollRate(aSimTime);
            break;
         case Lateral::Bank:
            ProcessLateralNavMode_Bank(aSimTime);
            break;
         case Lateral::DeltaRoll:
            ProcessLateralNavMode_DeltaRoll(aSimTime);
            break;
         case Lateral::Undefined:
            // In lieu of an explicit stabilizing control, use the control type
            switch (mControlMethod)
            {
            case Control::YawToTurnRollRate:
               ProcessStandardLateralNavMode_RollRate(0.0, aSimTime);
               break;
            case Control::YawToTurnZeroBank:
               ProcessStandardLateralNavMode_Bank(0.0, aSimTime);
               break;
            case Control::YawToTurnNoRoll:
            default:
               ProcessLateralNavMode_NoRollControl();
               break;
            }
            break;
         default:
            ProcessLateralNavMode_NoRollControl();
            break;
         }
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::ProcessVerticalNavChannelYawToTurn(double aSimTime)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
   }

   switch (mCurrentActivityPtr->GetVerticalChannelMode())
   {
   case Vertical::Waypoint:
      ProcessVerticalNavMode_Waypoint(aSimTime);
      break;
   case Vertical::Altitude:
      ProcessVerticalNavMode_Altitude(aSimTime);
      break;
   case Vertical::VertSpeed:
      ProcessVerticalNavMode_VertSpeed(aSimTime);
      break;
   case Vertical::Point:
      ProcessVerticalNavMode_Point(aSimTime);
      break;
   case Vertical::PitchGLoad:
      ProcessVerticalNavMode_PitchGLoad(aSimTime);
      break;
   case Vertical::PitchAng:
      ProcessVerticalNavMode_PitchAng(aSimTime);
      break;
   case Vertical::PitchRate:
      ProcessVerticalNavMode_PitchRate(aSimTime);
      break;
   case Vertical::FltPathAng:
      ProcessVerticalNavMode_FltPathAng(aSimTime);
      break;
   case Vertical::DeltaPitch:
      ProcessVerticalNavMode_DeltaPitch(aSimTime);
      break;
   case Vertical::Alpha:
      ProcessVerticalNavMode_Alpha(aSimTime);
      break;
   default:
      ProcessVerticalNavMode_NoControl(aSimTime);
      break;
   }
}

void wsf::six_dof::RigidBodyCommonController::ProcessSpeedChannelYawToTurn(double aSimTime)
{
   double speedCommand_g = 0.0;
   bool   controlled     = true;

   if (mTaxiModeActive)
   {
      // TODO-Taxi
   }

   switch (mCurrentActivityPtr->GetSpeedChannelMode())
   {
   case Speed::Waypoint:
      speedCommand_g = ProcessSpeedMode_Waypoint(aSimTime);
      break;
   case Speed::ForwardAccel:
      speedCommand_g = ProcessSpeedMode_ForwardAccel(aSimTime);
      break;
   case Speed::KIAS:
      speedCommand_g = ProcessSpeedMode_KIAS(aSimTime);
      break;
   case Speed::KTAS:
      speedCommand_g = ProcessSpeedMode_KTAS(aSimTime);
      break;
   case Speed::Mach:
      speedCommand_g = ProcessSpeedMode_Mach(aSimTime);
      break;
   case Speed::FPS:
      speedCommand_g = ProcessSpeedMode_FPS(aSimTime);
      break;
   case Speed::Throttle:
      speedCommand_g = ProcessSpeedMode_Throttle(aSimTime);
      break;

      // case Speed::Taxi_Waypoint:
      //    speedCommand_g = ProcessSpeedMode_Taxi_Waypoint(aSimTime);
      //    break;
      // case Speed::Taxi_ForwardAccel:
      //    speedCommand_g = ProcessSpeedMode_Taxi_ForwardAccel(aSimTime);
      //    break;
      // case Speed::Taxi_KIAS:
      //    speedCommand_g = ProcessSpeedMode_Taxi_KIAS(aSimTime);
      //    break;
      // case Speed::Taxi_KTAS:
      //    speedCommand_g = ProcessSpeedMode_Taxi_KTAS(aSimTime);
      //    break;

   default:
      speedCommand_g = ProcessSpeedMode_NoControl();
      controlled     = false;
      break;
   }

   if (controlled)
   {
      // Set throttles and speed brakes
      if (mCurrentLimitsAndSettings.enableAfterburnerAutoControl &&
          (speedCommand_g > static_cast<double>(mCurrentLimitsAndSettings.afterburnerThreshold)))
      {
         // Use afterburner
         mControlOutputs.throttleAfterburner =
            speedCommand_g - static_cast<double>(mCurrentLimitsAndSettings.afterburnerThreshold);
         mControlOutputs.throttleMilitary = 1.0f;
         mControlOutputs.speedBrake       = 0.0f;
      }
      else if (mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl &&
               (speedCommand_g < static_cast<double>(mCurrentLimitsAndSettings.speedBrakeThreshold)))
      {
         // Use speed brake
         mControlOutputs.throttleAfterburner = 0;
         mControlOutputs.throttleMilitary    = 0;
         mControlOutputs.speedBrake = static_cast<double>(mCurrentLimitsAndSettings.speedBrakeThreshold) - speedCommand_g;
      }
      else
      {
         // normal range
         mControlOutputs.throttleAfterburner = 0;
         mControlOutputs.throttleMilitary    = speedCommand_g;
         mControlOutputs.speedBrake          = 0;
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::LoadConfigFile(const std::string& aFilename)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFilename);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open input file " + aFilename + " in RigidBodyCommonController::LoadConfigFile()");
   }
   UtInput input;
   input.PushInput(std::move(inputPtr));

   ProcessInput(input);
}

void wsf::six_dof::RigidBodyCommonController::ProcessInputCommand(UtInput& aInput, const std::string& aCommand)
{
   if (aCommand == "autopilot_config")
   {
      UtInputBlock block(aInput, "end_autopilot_config");
      while (block.ReadCommand())
      {
         std::string blockCommand = block.GetCommand();

         if (CommonController::ProcessCommonInputCommand(aInput, blockCommand))
         {
         }
         else if (blockCommand == "min_taxi_turn_radius")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            mMinimumTaxiTurnRadius_ft = value * UtMath::cFT_PER_M;
            mDesiredTaxiTurnRadius_ft = mMinimumTaxiTurnRadius_ft;
         }
         else if (blockCommand == "use_simple_yaw_damper")
         {
            bool value;
            aInput.ReadValue(value);

            mUseSimpleYawDamper = value;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RigidBodyCommonController::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::ReadSupportFile(const std::string& aFilename)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFilename);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open input file " + aFilename + " in RigidBodyCommonController::ReadSupportFile()");
   }

   UtInput input;
   input.PushInput(std::move(inputPtr));

   const std::string& command = input.GetCommand();
   if (command == "autopilot_support_tables")
   {
      UtInputBlock block(input, "end_autopilot_support_tables");
      while (block.ReadCommand())
      {
         std::string blockCommand = block.GetCommand();

         if (blockCommand == "cl_max_mach_table")
         {
            mCLMaxMachTablePtr = ut::make_unique<UtTable::Curve>();
            mCLMaxMachTablePtr->ProcessInput(input,
                                             UtInput::cNON_DIMENSIONAL,
                                             "mach",
                                             UtTable::ValueGE(0.0),
                                             UtInput::cNON_DIMENSIONAL,
                                             "clmax",
                                             UtTable::NoCheck());
         }
         else if (blockCommand == "cl_min_mach_table")
         {
            mCLMinMachTablePtr = ut::make_unique<UtTable::Curve>();
            mCLMinMachTablePtr->ProcessInput(input,
                                             UtInput::cNON_DIMENSIONAL,
                                             "mach",
                                             UtTable::ValueGE(0.0),
                                             UtInput::cNON_DIMENSIONAL,
                                             "clmin",
                                             UtTable::NoCheck());
         }
         else if (blockCommand == "alpha_max_mach_table")
         {
            mAlphaMaxMachTablePtr = ut::make_unique<UtTable::Curve>();
            mAlphaMaxMachTablePtr->ProcessInput(input,
                                                UtInput::cNON_DIMENSIONAL,
                                                "mach",
                                                UtTable::ValueGE(0.0),
                                                UtInput::cNON_DIMENSIONAL,
                                                "alpha_deg",
                                                UtTable::NoCheck());
         }
         else if (blockCommand == "alpha_min_mach_table")
         {
            mAlphaMinMachTablePtr = ut::make_unique<UtTable::Curve>();
            mAlphaMinMachTablePtr->ProcessInput(input,
                                                UtInput::cNON_DIMENSIONAL,
                                                "mach",
                                                UtTable::ValueGE(0.0),
                                                UtInput::cNON_DIMENSIONAL,
                                                "alpha_deg",
                                                UtTable::NoCheck());
         }
         else if (blockCommand == "stick_zero_moment_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "stick_zero_moment_delta_thrust_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_delta_thrust_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "stick_zero_moment_delta_xcg_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_delta_xcg_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "stick_zero_moment_delta_zcg_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_delta_zcg_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "effective_CL_versus_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_effective_CL_versus_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mEffectiveCLVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "alpha_versus_mach_cl_table")
         {
            UtInputBlock subBlock(input, "end_alpha_versus_mach_cl_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("cl", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 1);

               mAlphaVsMachCLTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
      }
   }
}

void wsf::six_dof::RigidBodyCommonController::GetAimHeadingAndTurnRateForTaxiWaypointNav_deg(double  aSimTime,
                                                                                             double& aAimHeading_deg,
                                                                                             double& aTurnRate_dps)
{
   // No waypoint or ATM, maintain current heading and return
   if (mCurrentActivityPtr->GetCurrWaypoint() == nullptr)
   {
      mNavData.executeTurn = false;

      aAimHeading_deg = mNavData.aimHeading_rad * UtMath::cDEG_PER_RAD;
      aTurnRate_dps   = 0;

      return;
   }

   KinematicState* state = mParentVehiclePtr->GetKinematicState();

   double aimHdg_rad = 0.0;

   UtLLAPos        currentPosition = state->GetCurrentPositionLLA();
   const UtLLAPos& currPos         = currentPosition;

   double currentHdg_rad = state->GetLocalHeading_rad();

   UtVec2d currVel(state->GetSpeed_mps() * cos(currentHdg_rad), state->GetSpeed_mps() * sin(currentHdg_rad));

   double desiredTaxiRadius_ft = mDesiredTaxiTurnRadius_ft;

   // We have the desired radius, but we need to be sure that it is within the taxi yaw rate limit
   double rateYawLimitedTaxiRadius_ft =
      CalcTurnRadiusBasedOnTurnRateAndCurrentSpeed_ft(mCurrentLimitsAndSettings.taxiYawRate_Max);
   if (desiredTaxiRadius_ft < rateYawLimitedTaxiRadius_ft)
   {
      desiredTaxiRadius_ft = rateYawLimitedTaxiRadius_ft;
   }

   double dt_sec = aSimTime - utils::TimeToTime(mLastUpdateTime_nanosec);

   Route::CalcTaxiAimHeadingAngle(mCurrentActivityPtr->GetPrevWaypoint(),
                                  mCurrentActivityPtr->GetCurrWaypoint(),
                                  mCurrentActivityPtr->GetNextWaypoint(),
                                  mCurrentActivityPtr->GetCurrSegment(),
                                  mCurrentActivityPtr->GetNextSegment(),
                                  currPos,
                                  currVel,
                                  mNavData,
                                  static_cast<float>(currentHdg_rad),
                                  desiredTaxiRadius_ft,
                                  dt_sec,
                                  mAchievedWaypoint);

   if (mCurrentActivityPtr->GetCurrWaypoint()->FollowHorizontalTrack())
   {
      aimHdg_rad = mNavData.aimHeading_rad;
   }
   else
   {
      aimHdg_rad = Route::GetInitialHeading_rad(currPos, mCurrentActivityPtr->GetCurrWaypoint()->GetLLA());
   }

   // Return the aim heading in deg
   aAimHeading_deg = aimHdg_rad * UtMath::cDEG_PER_RAD;

   aTurnRate_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(desiredTaxiRadius_ft);
}

void wsf::six_dof::RigidBodyCommonController::ProcessLateralNavMode_Taxi_Waypoint(double aSimTime)
{
   double aimHeading_deg = 0;
   double turnRate_dps   = 0;

   GetAimHeadingAndTurnRateForTaxiWaypointNav_deg(aSimTime, aimHeading_deg, turnRate_dps);

   // Process PIDs
   ProcessStandardLateralNavMode_TaxiHeading(aimHeading_deg, turnRate_dps, aSimTime);
}

void wsf::six_dof::RigidBodyCommonController::ProcessLateralNavMode_Taxi_Heading(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = static_cast<double>(mCurrentActivityPtr->GetHeading_deg());

   double turnRate_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(mDesiredTaxiTurnRadius_ft);
   ;

   // Process PIDs
   ProcessStandardLateralNavMode_TaxiHeading(commandedHeading_deg, turnRate_dps, aSimTime);
}

void wsf::six_dof::RigidBodyCommonController::ProcessLateralNavMode_Taxi_YawRate(double aSimTime)
{
   // Get commanded rate
   double commandedYawRate_dps = mCurrentActivityPtr->GetYawRate_dps();

   // Process PIDs
   ProcessStandardLateralNavMode_TaxiYawRate(commandedYawRate_dps, aSimTime);
}

void wsf::six_dof::RigidBodyCommonController::ProcessStandardLateralNavMode_TaxiHeading(double aHeading_deg,
                                                                                        double aTurnRate_dps,
                                                                                        double aSimTime)
{
   // Get current states
   double currentHeading_deg = mParentVehiclePtr->GetKinematicState()->GetLocalHeading_deg();

   // Set the aim heading
   mAimHeading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;

   // Set outer loop value (in case it is not executed this frame)
   double commandedYawRate_dps = mLastCommandedYawRate_dps;

   // Calc the heading error
   double hdgError_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - currentHeading_deg);

   // Get autopilot-based max turn rate based on max turn radius
   // Note that this uses mDesiredTaxiTurnRadius_ft which is >= mMinimumTaxiTurnRadius_ft
   double maxTurnRateBasedOnAutopilot_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(mDesiredTaxiTurnRadius_ft);

   // Also limit the turn rate to AP yaw-rate limit
   double autopilotLimitedYawRate_dps = mCurrentLimitsAndSettings.taxiYawRate_Max;
   if (maxTurnRateBasedOnAutopilot_dps > autopilotLimitedYawRate_dps)
   {
      maxTurnRateBasedOnAutopilot_dps = autopilotLimitedYawRate_dps;
   }

   // Set min/max values
   double maxTurnRate_dps = std::abs(aTurnRate_dps);
   if (maxTurnRate_dps > maxTurnRateBasedOnAutopilot_dps)
   {
      maxTurnRate_dps = maxTurnRateBasedOnAutopilot_dps;
   }

   double minValue = -maxTurnRate_dps;
   double maxValue = maxTurnRate_dps;

   // Get the commanded yaw rate
   commandedYawRate_dps =
      mTaxiHeadingPID.CalcOutputFromErrorWithLimits(hdgError_deg, currentHeading_deg, aSimTime, minValue, maxValue);

   ProcessStandardLateralNavMode_TaxiYawRate(commandedYawRate_dps, aSimTime);
}

void wsf::six_dof::RigidBodyCommonController::ProcessStandardLateralNavMode_TaxiYawRate(double aCommandedYawRate_dps,
                                                                                        double aSimTime)
{
   RigidBodyLandingGear* gear = mParentVehiclePtr->GetLandingGear();
   if (gear == nullptr)
   {
      return;
   }

   // Get the desired yaw rate
   double commandedYawRate_dps = aCommandedYawRate_dps;

   // Calc rate limit - note that this uses mDesiredTaxiTurnRadius_ft which is >= mMinimumTaxiTurnRadius_ft
   double maxTurnRate_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(mDesiredTaxiTurnRadius_ft);

   // Also limit the turn rate to AP yaw-rate limit
   double autopilotLimitedYawRate_dps = mCurrentLimitsAndSettings.taxiYawRate_Max;
   if (maxTurnRate_dps > autopilotLimitedYawRate_dps)
   {
      maxTurnRate_dps = autopilotLimitedYawRate_dps;
   }

   // Limit the command
   if (commandedYawRate_dps > maxTurnRate_dps)
   {
      commandedYawRate_dps = maxTurnRate_dps;
   }
   else if (commandedYawRate_dps < -maxTurnRate_dps)
   {
      commandedYawRate_dps = -maxTurnRate_dps;
   }

   KinematicState* state = mParentVehiclePtr->GetKinematicState();

   double currentYawRate_dps = state->GetYawRate_dps();

   // Execute Inner Loop

   double speed_fps = state->GetSpeed_fps();

   // Note -- a negative turn radius implies a turn to the left
   double turnRadius_ft = 0.0;

   if (std::abs(commandedYawRate_dps) < 0.0000001)
   {
      // Essentially a zero rate -- this results in an infinite radius
      if (commandedYawRate_dps > 0)
      {
         turnRadius_ft = 1.0E20;
      }
      else
      {
         turnRadius_ft = -1.0E20;
      }
   }
   else
   {
      turnRadius_ft = speed_fps / (commandedYawRate_dps * UtMath::cRAD_PER_DEG);
   }

   // We need to limit how much the angle can be at very low speeds
   double spd_fps = state->GetSpeed_fps(); // should be ground_speed_fps

   double speedGain = spd_fps / 1.0;
   if (speedGain < 1)
   {
      speedGain = 0;
   }
   if (speedGain > 1)
   {
      speedGain = 1;
   }

   // Get the nose gear
   RigidBodyGroundReactionPoint* noseGear = gear->GetNoseGear();
   if (noseGear == nullptr)
   {
      // We lack a nose gear, so return "zero"
      mControlOutputs.rudderRight       = 0.0;
      mControlOutputs.nwsSteering       = 0.0;
      mControlOutputs.noseWheelSteering = 0.0;

      // Store last command for future
      mLastCommandedYawRate_dps = aCommandedYawRate_dps;

      return;
   }

   double currentSteeringAngle_deg = 0;
   double maxSteeringAngle_deg     = 0;
   noseGear->GetSteeringData(currentSteeringAngle_deg, maxSteeringAngle_deg);

   // Limit the turn radius (avoid infinite radius)
   if (std::abs(turnRadius_ft) > 1E10)
   {
      turnRadius_ft = (std::abs(turnRadius_ft) / turnRadius_ft) * 1E10;
   }

   // Get the steering angle
   double calculatedSteeringAngle_deg = 0;
   gear->SteeringAngleForDesiredRadiusFt_deg(turnRadius_ft, calculatedSteeringAngle_deg, maxSteeringAngle_deg);

   // Set the feed-forward (bias)
   double bias = calculatedSteeringAngle_deg / maxSteeringAngle_deg;
   mTaxiYawRatePID.SetBias(bias);

   double command =
      mTaxiYawRatePID.CalcOutputFromTargetWithLimits(commandedYawRate_dps, currentYawRate_dps, aSimTime, -1.0, 1.0);

   // At some point, we may want to add differential braking such as this:
   // if (command >  0.75) { mWheelBrakingRight = (command - 0.75) / 0.25; }
   // if (command < -0.75) { mWheelBrakingLeft  = (std::abs(command) - 0.75) / 0.25; }

   // Apply the speed gain to steering commands
   command *= speedGain;

   mControlOutputs.rudderRight       = command;
   mControlOutputs.nwsSteering       = command;
   mControlOutputs.noseWheelSteering = command;

   // Store last command for future
   mLastCommandedYawRate_dps = aCommandedYawRate_dps;
}

void wsf::six_dof::RigidBodyCommonController::ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aSimTime)
{
   // Get current states
   double currentBeta_deg = mParentVehiclePtr->GetKinematicState()->GetBeta_deg();

   // Get the desired beta
   double commandedBeta_deg = aCommandedBeta_deg;

   // Limit the command
   if (commandedBeta_deg < -mLimitedBeta_deg)
   {
      commandedBeta_deg = -mLimitedBeta_deg;
   }
   else if (commandedBeta_deg > mLimitedBeta_deg)
   {
      commandedBeta_deg = mLimitedBeta_deg;
   }

   // Calculate the feed-forward for beta
   if (Control::IsYawToTurn(mControlMethod))
   {
      double rudderRight = 0;
      mParentVehiclePtr->CalculateStickBackForZeroMoment(commandedBeta_deg, rudderRight);
      mBetaPID.SetFeedForward(rudderRight);
   }

   // Execute Inner Loop -- Notice the negation of the PID output (rudder command is opposite to beta)
   mControlOutputs.rudderRight = -mBetaPID.CalcOutputFromTarget(commandedBeta_deg, currentBeta_deg, aSimTime);

   // Store last command for future
   mLastCommandedBeta_deg = commandedBeta_deg;
}

void wsf::six_dof::RigidBodyCommonController::ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg,
                                                                                   double aSimTime)
{
   // Get current states
   double currentAlpha_deg = mParentVehiclePtr->GetKinematicState()->GetAlpha_deg();

   // Get the desired alpha
   double commandedAlpha_deg = aCommandedAlpha_deg;

   // Limit the command using the g-limits calculated in the Update function
   if (commandedAlpha_deg < mLimitedMinAlpha_deg)
   {
      commandedAlpha_deg = mLimitedMinAlpha_deg;
   }
   if (commandedAlpha_deg > mLimitedMaxAlpha_deg)
   {
      commandedAlpha_deg = mLimitedMaxAlpha_deg;
   }

   double stickBack = 0;
   mParentVehiclePtr->CalculateStickBackForZeroMoment(commandedAlpha_deg, stickBack);
   mAlphaPID.SetFeedForward(stickBack);

   // Execute Inner Loop
   mControlOutputs.stickBack = mAlphaPID.CalcOutputFromTarget(commandedAlpha_deg, currentAlpha_deg, aSimTime);
   // Store last command for future
   mLastCommandedAlpha_deg = aCommandedAlpha_deg;
}

void wsf::six_dof::RigidBodyCommonController::ProcessVerticalNavMode_Taxi(double) // aSimTime)
{
}

double wsf::six_dof::RigidBodyCommonController::ProcessStandardSpeedModeTaxi_FPS(double aSpeed_fps, double aSimTime)
{
   if (mParentVehiclePtr == nullptr)
   {
      return 0.0;
   }

   double currentSpeed_fps = mParentVehiclePtr->GetKinematicState()->GetSpeed_fps();

   double commandedSpeed_fps = aSpeed_fps;
   double biasThrottle       = 0.0;
   double drag               = mParentVehiclePtr->GetDrag_lbs();
   double alpha_rad          = mParentVehiclePtr->GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double cosAngle           = cos(alpha_rad);
   double maxThrust          = mParentVehiclePtr->GetMaximumPotentialThrust_lbs() * cosAngle;
   double minThrust          = mParentVehiclePtr->GetMinimumPotentialThrust_lbs() * cosAngle;
   double deltaThrust        = maxThrust - minThrust;

   if (drag > maxThrust)
   {
      biasThrottle = 1.0;
   }
   else if (drag < minThrust)
   {
      biasThrottle = -1.0;
   }
   else
   {
      if (deltaThrust == 0.0)
      {
         biasThrottle = 0.0;
      }
      else
      {
         biasThrottle = (drag - minThrust) / deltaThrust;
      }
   }

   mSpeedPID.SetBias(biasThrottle);

   double minOutput = -1.0;
   double maxOutput = 2.0;

   double commandedThrottle =
      mTaxiSpeedPID.CalcOutputFromTargetWithLimits(commandedSpeed_fps, currentSpeed_fps, aSimTime, minOutput, maxOutput);
   return commandedThrottle;
}

double wsf::six_dof::RigidBodyCommonController::ProcessSpeedMode_Taxi_Waypoint(double aSimTime)
{
   if (mParentVehiclePtr == nullptr)
   {
      return 0.0;
   } // Return if we lack a parent

   if (mEnvironment == nullptr)
   {
      return 0.0;
   } // Return if we lack an atm

   // Get current states
   double tgtSpeed_fps = mParentVehiclePtr->GetKinematicState()->GetSpeed_fps();

   Waypoint::sSpeed speed;

   // Either still executing turn or done with route, so use last waypoint speed
   if (mNavData.executeTurn || mCurrentActivityPtr->GetCurrWaypoint() == nullptr)
   {
      if (mCurrentActivityPtr->GetPrevWaypoint() != nullptr)
      {
         speed = mCurrentActivityPtr->GetPrevWaypoint()->GetSpeed();
      }
      else
      {
         speed.type = Waypoint::SPD_TYPE_FPS;
         speed.val  = 0.0;
      }
   }
   else
   {
      // Turn complete, so align speed with next waypoint speed
      speed = mCurrentActivityPtr->GetCurrWaypoint()->GetSpeed();
   }

   double currentAlt_ft = mParentVehiclePtr->GetKinematicState()->GetAlt_ft();
   double currentAlt_m  = UtMath::cM_PER_FT * currentAlt_ft;

   switch (speed.type)
   {
   case Waypoint::SPD_TYPE_MACH:
      tgtSpeed_fps = mEnvironment->CalcFpsFromMach(currentAlt_m, speed.val);
      break;
   case Waypoint::SPD_TYPE_TAS_KNOTS:
      tgtSpeed_fps = UtMath::cFPS_PER_NMPH * speed.val;
      break;
   case Waypoint::SPD_TYPE_CAS_KNOTS:
      tgtSpeed_fps = mEnvironment->CalcFpsFromKcas(currentAlt_m, speed.val);
      break;
   case Waypoint::SPD_TYPE_FPS:
      tgtSpeed_fps = speed.val;
      break;
   default:
      break;
   }

   return ProcessStandardSpeedModeTaxi_FPS(tgtSpeed_fps, aSimTime);
}

double wsf::six_dof::RigidBodyCommonController::ProcessSpeedMode_Taxi_ForwardAccel(double) // aSimTime)
{
   return 0.0;
}

double wsf::six_dof::RigidBodyCommonController::ProcessSpeedMode_Taxi_KIAS(double) // aSimTime)
{
   return 0.0;
}

double wsf::six_dof::RigidBodyCommonController::ProcessSpeedMode_Taxi_KTAS(double) // aSimTime)
{
   return 0.0;
}

void wsf::six_dof::RigidBodyCommonController::SetDesiredTaxiRadius(double aTaxiRadius_ft)
{
   if (aTaxiRadius_ft <= mMinimumTaxiTurnRadius_ft)
   {
      // Limit to the minimum
      mDesiredTaxiTurnRadius_ft = mMinimumTaxiTurnRadius_ft;
      auto out                  = ut::log::warning() << "Requested taxi radius too low. Using minimum radius instead.";
      out.AddNote() << "Requested: " << aTaxiRadius_ft << " ft";
      out.AddNote() << "Minimum: " << mMinimumTaxiTurnRadius_ft << " ft";
   }
   else
   {
      mDesiredTaxiTurnRadius_ft = aTaxiRadius_ft;
   }
}

wsf::six_dof::PID* wsf::six_dof::RigidBodyCommonController::GetPID_ByType(Pid::Type aPidType)
{
   switch (aPidType)
   {
   case Pid::Alpha:
      return &mAlphaPID;
   case Pid::VerticalSpeed:
      return &mVerticalSpeedPID;
   case Pid::PitchAngle:
      return &mPitchAnglePID;
   case Pid::PitchRate:
      return &mPitchRatePID;
   case Pid::FlightPathAngle:
      return &mFlightPathAnglePID;
   case Pid::DeltaPitch:
      return &mDeltaPitchPID;
   case Pid::Altitude:
      return &mAltitudePID;
   case Pid::Beta:
      return &mBetaPID;
   case Pid::YawRate:
      return &mYawRatePID;
   case Pid::YawHeading:
      return &mYawHeadingPID;
   case Pid::TaxiHeading:
      return &mTaxiHeadingPID;
   case Pid::RollRate:
      return &mRollRatePID;
   case Pid::DeltaRoll:
      return &mDeltaRollPID;
   case Pid::BankAngle:
      return &mBankAnglePID;
   case Pid::RollHeading:
      return &mRollHeadingPID;
   case Pid::ForwardAccel:
      return &mForwardAccelPID;
   case Pid::Speed:
      return &mSpeedPID;
   case Pid::TaxiForwardAccel:
      return &mTaxiForwardAccelPID;
   case Pid::TaxiSpeed:
      return &mTaxiSpeedPID;
   case Pid::TaxiYawRate:
      return &mTaxiYawRatePID;
   default:
      return nullptr;
   }
}

void wsf::six_dof::RigidBodyCommonController::SetControllingValueForAllPIDs(double aControllingValue)
{
   mAlphaPID.SetControllingValue(aControllingValue);
   mVerticalSpeedPID.SetControllingValue(aControllingValue);
   mPitchAnglePID.SetControllingValue(aControllingValue);
   mPitchRatePID.SetControllingValue(aControllingValue);
   mFlightPathAnglePID.SetControllingValue(aControllingValue);
   mDeltaPitchPID.SetControllingValue(aControllingValue);
   mAltitudePID.SetControllingValue(aControllingValue);
   mBetaPID.SetControllingValue(aControllingValue);
   mYawRatePID.SetControllingValue(aControllingValue);
   mYawHeadingPID.SetControllingValue(aControllingValue);
   mTaxiHeadingPID.SetControllingValue(aControllingValue);
   mRollRatePID.SetControllingValue(aControllingValue);
   mDeltaRollPID.SetControllingValue(aControllingValue);
   mBankAnglePID.SetControllingValue(aControllingValue);
   mRollHeadingPID.SetControllingValue(aControllingValue);
   mForwardAccelPID.SetControllingValue(aControllingValue);
   mSpeedPID.SetControllingValue(aControllingValue);
   mTaxiForwardAccelPID.SetControllingValue(aControllingValue);
   mTaxiSpeedPID.SetControllingValue(aControllingValue);
   mTaxiYawRatePID.SetControllingValue(aControllingValue);
}

bool wsf::six_dof::RigidBodyCommonController::GetStickForZeroMomentVsMachAlpha(double  aMach,
                                                                               double  aAlpha_rad,
                                                                               double  aThrust_lbs,
                                                                               double& aStickBack) const
{
   if (mStickForZeroMomentVsMachAlphaTablePtr == nullptr)
   {
      return false;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;

   aStickBack = mStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);

   if (aStickBack == 1.0 || aStickBack == -1.0)
   {
      // Stick command was limited to +/-1.0, so leave it alone
      return true;
   }

   if (mParentVehiclePtr)
   {
      MassProperties massProperties = mParentVehiclePtr->GetMassProperties();

      if (mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr)
      {
         // Account for the change in moment caused by thrust and thrust vectoring
         double referenceMass_lbs = massProperties.GetBaseMass_lbs();

         double deltaStickBack = mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);
         aStickBack += deltaStickBack * aThrust_lbs / referenceMass_lbs;
      }

      UtVec3dX changeInCenterOfGravity_ft =
         massProperties.GetCmPosRelToRef_ft() - massProperties.GetBaseCmPosRelToRef_ft();

      if (mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
      {
         double deltaStickBack = mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);
         aStickBack += deltaStickBack * changeInCenterOfGravity_ft.X();
      }

      if (mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
      {
         double deltaStickBack = mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);
         aStickBack += deltaStickBack * changeInCenterOfGravity_ft.Z();
      }
   }

   return true;
}
