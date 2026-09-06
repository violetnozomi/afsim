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

#include "WsfPointMassSixDOF_CommonController.hpp"

#include <math.h>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"
#include "WsfPointMassSixDOF_PilotObject.hpp"
#include "WsfSixDOF_AutopilotAction.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_PID.hpp"

wsf::six_dof::PointMassCommonController::PointMassCommonController(UtInput& aInput, const std::string& aCommand)
   : CommonController()
{
   ProcessInputCommand(aInput, aCommand);
}

wsf::six_dof::PointMassCommonController::PointMassCommonController(const PointMassCommonController& aSrc)
   : CommonController(aSrc)
{
}

wsf::six_dof::PointMassCommonController* wsf::six_dof::PointMassCommonController::Clone() const
{
   return new PointMassCommonController(*this);
}

wsf::six_dof::Mover* wsf::six_dof::PointMassCommonController::GetParentVehicle()
{
   return mParentVehiclePtr;
}

void wsf::six_dof::PointMassCommonController::ProcessInputCommand(UtInput& aInput, const std::string& aCommand)
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
         else
         {
            auto out = ut::log::error() << "Unrecognized command within PointMassCommonController::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
}

void wsf::six_dof::PointMassCommonController::Update(sAutopilotControls& aControls, int64_t aSimTime_nanosec)
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

   // Enforce control limits
   EnforceControlLimits();

   // Return the current controls
   aControls = mControlOutputs;

   mLastUpdateTime_nanosec = aSimTime_nanosec;
}

void wsf::six_dof::PointMassCommonController::ProcessLaternalNavChannelsBankToTurn(double aSimTime)
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

void wsf::six_dof::PointMassCommonController::ProcessVerticalNavChannelBankToTurn(double aSimTime)
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

void wsf::six_dof::PointMassCommonController::ProcessSpeedChannelBankToTurn(double aSimTime)
{
   double speedCommand_g = 0.0;
   bool   controlled     = true;

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

void wsf::six_dof::PointMassCommonController::ProcessLaternalNavChannelsYawToTurn(double aSimTime)
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

void wsf::six_dof::PointMassCommonController::ProcessVerticalNavChannelYawToTurn(double aSimTime)
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

void wsf::six_dof::PointMassCommonController::ProcessSpeedChannelYawToTurn(double aSimTime)
{
   double speedCommand_g = 0.0;
   bool   controlled     = true;

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

void wsf::six_dof::PointMassCommonController::ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg,
                                                                                   double aSimTime)
{
   // Get current states
   double currentAlpha_deg = GetParentVehicle()->GetKinematicState()->GetAlpha_deg();

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

   double lastUpdateTime_sec = utils::TimeToTime(mLastUpdateTime_nanosec);

   // Execute Inner Loop
   // Point-mass inner loop should not need its own PID

   // Non-tunable kinematic projection for alpha command
   double alphaError_rad       = (commandedAlpha_deg - currentAlpha_deg) * UtMath::cRAD_PER_DEG;
   double pitchRateCommand_rps = alphaError_rad / (aSimTime - lastUpdateTime_sec);

   PointMassFlightControlSystem* fcs = mParentVehiclePtr->GetFlightControls();
   mControlOutputs.stickBack         = fcs->FindStickBackForPitchCommand(pitchRateCommand_rps);

   // Store last command for future
   mLastCommandedAlpha_deg = aCommandedAlpha_deg;
}

void wsf::six_dof::PointMassCommonController::ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aSimTime)
{
   // Get current states
   double currentBeta_deg = GetParentVehicle()->GetKinematicState()->GetBeta_deg();

   // Get the desired beta
   double commandedBeta_deg = aCommandedBeta_deg;

   // Limit the command
   if (commandedBeta_deg < -mLimitedBeta_deg)
   {
      commandedBeta_deg = -mLimitedBeta_deg;
   }
   if (commandedBeta_deg > mLimitedBeta_deg)
   {
      commandedBeta_deg = mLimitedBeta_deg;
   }

   double lastUpdateTime_sec = utils::TimeToTime(mLastUpdateTime_nanosec);

   // Execute Inner Loop
   // Point-mass inner loop should not need its own PID

   // Non-tunable kinematic projection for beta command
   double betaError_rad      = (commandedBeta_deg - currentBeta_deg) * UtMath::cRAD_PER_DEG;
   double yawRateCommand_rps = -betaError_rad / (aSimTime - lastUpdateTime_sec);

   PointMassFlightControlSystem* fcs = mParentVehiclePtr->GetFlightControls();
   mControlOutputs.rudderRight       = fcs->FindRudderRightForYawCommand(yawRateCommand_rps);

   // Store last command for future
   mLastCommandedBeta_deg = commandedBeta_deg;
}

void wsf::six_dof::PointMassCommonController::ProcessStandardLateralNavMode_RollRate(double aCommandedRollRate_dps,
                                                                                     double aSimTime)
{
   // Get the desired roll rate
   double commandedRollRate_dps = aCommandedRollRate_dps;

   // Limit the command
   if (commandedRollRate_dps < -mCurrentLimitsAndSettings.rollRate_Max)
   {
      commandedRollRate_dps = -mCurrentLimitsAndSettings.rollRate_Max;
   }
   else if (commandedRollRate_dps > mCurrentLimitsAndSettings.rollRate_Max)
   {
      commandedRollRate_dps = mCurrentLimitsAndSettings.rollRate_Max;
   }

   // Execute Inner Loop
   // Point-mass inner loop should not need its own PID

   double rollRateCommand_rps = commandedRollRate_dps * UtMath::cRAD_PER_DEG;

   PointMassFlightControlSystem* fcs = mParentVehiclePtr->GetFlightControls();
   mControlOutputs.stickRight        = fcs->FindStickRightForRollCommand(rollRateCommand_rps);

   // Store last command for future
   mLastCommandedRollRate_dps = commandedRollRate_dps;
}

void wsf::six_dof::PointMassCommonController::ProcessLateralNavMode_DeltaRoll(double aSimTime)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedRollRate_dps = mLastCommandedRollRate_dps;

   // Get the desired angle
   double commandedDeltaRollAngle_deg = mCurrentActivityPtr->GetRollDelta_deg();

   {
      // Get current states
      double currentRollDelta_deg = mIntegratedDeltaRoll_deg;

      // Get the desired g-load with g-limits
      commandedRollRate_dps = mBankAnglePID.CalcOutputFromTargetWithLimits(commandedDeltaRollAngle_deg,
                                                                           currentRollDelta_deg,
                                                                           aSimTime,
                                                                           -mCurrentLimitsAndSettings.rollRate_Max,
                                                                           mCurrentLimitsAndSettings.rollRate_Max);
   }

   // Execute Inner Loop
   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aSimTime);
}

void wsf::six_dof::PointMassCommonController::ProcessVerticalNavMode_DeltaPitch(double aSimTime)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   {
      // Get current states
      double currentPitchDelta_deg = mIntegratedDeltaPitch_deg;

      // Get the desired angle
      double commandedDeltaPitchAngle_deg = mCurrentActivityPtr->GetDeltaPitch_deg();

      // Set bias
      mPitchAnglePID.SetBias(mCurrentGBiasAlpha_deg);

      // Get the desired pitch rate within limits
      commandedAlpha_deg = mPitchAnglePID.CalcOutputFromTargetWithLimits(commandedDeltaPitchAngle_deg,
                                                                         currentPitchDelta_deg,
                                                                         aSimTime,
                                                                         mLimitedMinAlpha_deg,
                                                                         mLimitedMaxAlpha_deg);
   }

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::PointMassCommonController::LoadConfigFile(const std::string& aFilename)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFilename);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open input file " + aFilename + " in PointMassCommonController::LoadConfigFile()");
   }
   UtInput input;
   input.PushInput(std::move(inputPtr));

   ProcessInput(input);
}

void wsf::six_dof::PointMassCommonController::ReadSupportFile(const std::string& aFilename)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFilename);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open input file " + aFilename + " in PointMassCommonController::ReadSupportFile()");
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

wsf::six_dof::PID* wsf::six_dof::PointMassCommonController::GetPID_ByType(Pid::Type aPidType)
{
   switch (aPidType)
   {
   case Pid::Alpha:
      return &mAlphaPID;
   case Pid::VerticalSpeed:
      return &mVerticalSpeedPID;
   case Pid::DeltaPitch:
   // Fallthrough to Pitch Angle
   case Pid::PitchAngle:
      return &mPitchAnglePID;
   case Pid::PitchRate:
      return &mPitchRatePID;
   case Pid::FlightPathAngle:
      return &mFlightPathAnglePID;
   case Pid::Altitude:
      return &mAltitudePID;
   case Pid::Beta:
      return &mBetaPID;
   case Pid::YawRate:
      return &mYawRatePID;
   case Pid::YawHeading:
      return &mYawHeadingPID;
   case Pid::RollRate:
      return &mRollRatePID;
   case Pid::DeltaRoll:
   // Fallthrough to Bank Angle
   case Pid::BankAngle:
      return &mBankAnglePID;
   case Pid::RollHeading:
      return &mRollHeadingPID;
   case Pid::ForwardAccel:
      return &mForwardAccelPID;
   case Pid::Speed:
      return &mSpeedPID;
   default:
      return nullptr;
   }
}

void wsf::six_dof::PointMassCommonController::SetControllingValueForAllPIDs(double aControllingValue)
{
   mAlphaPID.SetControllingValue(aControllingValue);
   mVerticalSpeedPID.SetControllingValue(aControllingValue);
   mPitchAnglePID.SetControllingValue(aControllingValue);
   mPitchRatePID.SetControllingValue(aControllingValue);
   mFlightPathAnglePID.SetControllingValue(aControllingValue);
   mAltitudePID.SetControllingValue(aControllingValue);
   mBetaPID.SetControllingValue(aControllingValue);
   mYawRatePID.SetControllingValue(aControllingValue);
   mYawHeadingPID.SetControllingValue(aControllingValue);
   mRollRatePID.SetControllingValue(aControllingValue);
   mBankAnglePID.SetControllingValue(aControllingValue);
   mRollHeadingPID.SetControllingValue(aControllingValue);
   mForwardAccelPID.SetControllingValue(aControllingValue);
   mSpeedPID.SetControllingValue(aControllingValue);
}
