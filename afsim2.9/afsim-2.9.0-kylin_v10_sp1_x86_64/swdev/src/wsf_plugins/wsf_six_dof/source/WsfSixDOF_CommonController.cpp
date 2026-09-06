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

#include "WsfSixDOF_CommonController.hpp"

#include <math.h>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_AutopilotAction.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_Mover.hpp"

void wsf::six_dof::CommonController::ProcessInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();
   ProcessInputCommand(aInput, command);
}

bool wsf::six_dof::CommonController::ProcessCommonInputCommand(UtInput& aInput, const std::string& aCommand)
{
   if (aCommand == "vertical_middle_loop_rate_factor")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 1, 255);
      mVerticalControlLoop.middleLoopFactor = value;
   }
   else if (aCommand == "lateral_middle_loop_rate_factor")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 1, 255);
      mLateralControlLoop.middleLoopFactor = value;
   }
   else if (aCommand == "speed_middle_loop_rate_factor")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 1, 255);
      mSpeedControlLoop.middleLoopFactor = value;
   }
   else if (aCommand == "vertical_outer_loop_rate_factor")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 1, 255);
      mVerticalControlLoop.outerLoopFactor = value;
   }
   else if (aCommand == "lateral_outer_loop_rate_factor")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 1, 255);
      mLateralControlLoop.outerLoopFactor = value;
   }
   else if (aCommand == "speed_outer_loop_rate_factor")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 1, 255);
      mSpeedControlLoop.outerLoopFactor = value;
   }
   else if (aCommand == "control_method")
   {
      std::string value;
      aInput.ReadValue(value);

      if (value == "bank_to_turn_no_yaw")
      {
         mControlMethod = Control::BankToTurnNoYaw;
      }
      else if (value == "bank_to_turn_with_yaw")
      {
         mControlMethod = Control::BankToTurnWithYaw;
      }
      else if (value == "yaw_to_turn_no_roll")
      {
         mControlMethod = Control::YawToTurnNoRoll;
      }
      else if (value == "yaw_to_turn_roll_rate")
      {
         mControlMethod = Control::YawToTurnRollRate;
      }
      else if (value == "yaw_to_turn_zero_bank")
      {
         mControlMethod = Control::YawToTurnZeroBank;
      }
      else if (value == "BANK_TO_TURN_NO_YAW")
      {
         mControlMethod = Control::BankToTurnNoYaw;
      }
      else if (value == "BANK_TO_TURN_WITH_YAW")
      {
         mControlMethod = Control::BankToTurnWithYaw;
      }
      else if (value == "YAW_TO_TURN_NO_ROLL")
      {
         mControlMethod = Control::YawToTurnNoRoll;
      }
      else if (value == "YAW_TO_TURN_ROLL_RATE")
      {
         mControlMethod = Control::YawToTurnRollRate;
      }
      else if (value == "YAW_TO_TURN_ZERO_BANK")
      {
         mControlMethod = Control::YawToTurnZeroBank;
      }
   }
   else if (aCommand == "pid_group")
   {
      UtInputBlock subBlock(aInput, "end_pid_group");
      ProcessPidGroupInputBlock(subBlock, aInput);
   }
   else
   {
      return false;
   }

   return true;
}

void wsf::six_dof::CommonController::ProcessPidGroupInputBlock(UtInputBlock& aInputBlock, UtInput& aInput)
{
   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();

      if (command == "pid_alpha")
      {
         UtInputBlock subBlock(aInput, "end_pid_alpha");
         mAlphaPID.ProcessInput(subBlock);
      }
      else if (command == "pid_vert_speed")
      {
         UtInputBlock subBlock(aInput, "end_pid_vert_speed");
         mVerticalSpeedPID.ProcessInput(subBlock);

         mVerticalSpeedPID.TrySetUpdateInterval_sec(mVerticalControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_pitch_angle")
      {
         UtInputBlock subBlock(aInput, "end_pid_pitch_angle");
         mPitchAnglePID.ProcessInput(subBlock);

         mPitchAnglePID.TrySetUpdateInterval_sec(mVerticalControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_pitch_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_pitch_rate");
         mPitchRatePID.ProcessInput(subBlock);

         mPitchRatePID.TrySetUpdateInterval_sec(mVerticalControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_flightpath_angle")
      {
         UtInputBlock subBlock(aInput, "end_pid_flightpath_angle");
         mFlightPathAnglePID.ProcessInput(subBlock);

         mFlightPathAnglePID.TrySetUpdateInterval_sec(mVerticalControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_delta_pitch")
      {
         UtInputBlock subBlock(aInput, "end_pid_delta_pitch");
         mDeltaPitchPID.ProcessInput(subBlock);

         mDeltaPitchPID.TrySetUpdateInterval_sec(mVerticalControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_altitude")
      {
         UtInputBlock subBlock(aInput, "end_pid_altitude");
         mAltitudePID.ProcessInput(subBlock);

         mAltitudePID.TrySetUpdateInterval_sec(mVerticalControlLoop.GetOuterLoopInterval_sec());
      }
      else if (command == "pid_beta")
      {
         UtInputBlock subBlock(aInput, "end_pid_beta");
         mBetaPID.ProcessInput(subBlock);
      }
      else if (command == "pid_yaw_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_yaw_rate");
         mYawRatePID.ProcessInput(subBlock);

         mYawRatePID.TrySetUpdateInterval_sec(mLateralControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_yaw_heading")
      {
         UtInputBlock subBlock(aInput, "end_pid_yaw_heading");
         mYawHeadingPID.ProcessInput(subBlock);

         mYawHeadingPID.TrySetUpdateInterval_sec(mLateralControlLoop.GetOuterLoopInterval_sec());
      }
      else if (command == "pid_taxi_heading")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_heading");
         mTaxiHeadingPID.ProcessInput(subBlock);

         mTaxiHeadingPID.TrySetUpdateInterval_sec(mLateralControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_roll_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_roll_rate");
         mRollRatePID.ProcessInput(subBlock);
      }
      else if (command == "pid_delta_roll")
      {
         UtInputBlock subBlock(aInput, "end_pid_delta_roll");
         mDeltaRollPID.ProcessInput(subBlock);

         mDeltaRollPID.TrySetUpdateInterval_sec(mLateralControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_bank_angle")
      {
         UtInputBlock subBlock(aInput, "end_pid_bank_angle");
         mBankAnglePID.ProcessInput(subBlock);

         mBankAnglePID.TrySetUpdateInterval_sec(mLateralControlLoop.GetMiddleLoopInterval_sec());
      }
      else if (command == "pid_roll_heading")
      {
         UtInputBlock subBlock(aInput, "end_pid_roll_heading");
         mRollHeadingPID.ProcessInput(subBlock);

         mRollHeadingPID.TrySetUpdateInterval_sec(mLateralControlLoop.GetOuterLoopInterval_sec());
      }
      else if (command == "pid_forward_accel")
      {
         UtInputBlock subBlock(aInput, "end_pid_forward_accel");
         mForwardAccelPID.ProcessInput(subBlock);
      }
      else if (command == "pid_speed")
      {
         UtInputBlock subBlock(aInput, "end_pid_speed");
         mSpeedPID.ProcessInput(subBlock);
      }
      else if (command == "pid_taxi_forward_accel")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_forward_accel");
         mTaxiForwardAccelPID.ProcessInput(subBlock);
      }
      else if (command == "pid_taxi_speed")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_speed");
         mTaxiSpeedPID.ProcessInput(subBlock);
      }
      else if (command == "pid_taxi_yaw_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_yaw_rate");
         mTaxiYawRatePID.ProcessInput(subBlock);
      }
      else if (command == "limits_and_settings")
      {
         UtInputBlock subBlock(aInput, "end_limits_and_settings");
         ProcessLimitsAndSettingsInputBlock(subBlock);
      }
      else
      {
         auto out = ut::log::warning() << "Unrecognized data command in CommonController::ProcessPidGroupInputBlock().";
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
}

void wsf::six_dof::CommonController::ProcessLimitsAndSettingsInputBlock(UtInputBlock& aInputBlock)
{
   mDefaultLimitsAndSettings = AutopilotLimitsAndSettings();

   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();

      if (command == "afterburner_threshold")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.afterburnerThreshold         = static_cast<float>(value);
         mDefaultLimitsAndSettings.enableAfterburnerAutoControl = true;
      }
      else if (command == "speedbrake_threshold")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.speedBrakeThreshold         = static_cast<float>(value);
         mDefaultLimitsAndSettings.enableSpeedBrakeAutoControl = true;
      }
      else if (command == "turn_roll_in_multiplier")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.turnRollInMultiplier = static_cast<float>(value);
      }
      else if (command == "route_allowable_angle_error")
      {
         double value = 0.0;
         controllerInput.ReadValueOfType(value, UtInput::cANGLE);
         mDefaultLimitsAndSettings.routeAllowableAngleError_rad = static_cast<float>(value);
      }
      else if (command == "pitch_gload_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchGLoad_Min = static_cast<float>(value);
      }
      else if (command == "pitch_gload_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchGLoad_Max = static_cast<float>(value);
      }
      else if (command == "alpha_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.alpha_Min = static_cast<float>(value);
      }
      else if (command == "alpha_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.alpha_Max = static_cast<float>(value);
      }
      else if (command == "pitch_rate_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchRate_Min = static_cast<float>(value);
      }
      else if (command == "pitch_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchRate_Max = static_cast<float>(value);
      }
      else if (command == "vert_speed_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.vertSpd_Min = static_cast<float>(value);
      }
      else if (command == "vert_speed_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.vertSpd_Max = static_cast<float>(value);
      }
      else if (command == "yaw_gload_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.yawGLoad_Max = static_cast<float>(value);
      }
      else if (command == "taxi_speed_max_fps")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.taxiSpeed_Max = static_cast<float>(value);
      }
      else if (command == "taxi_yaw_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.taxiYawRate_Max = static_cast<float>(value);
      }
      else if (command == "beta_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.beta_Max = static_cast<float>(value);
      }
      else if (command == "yaw_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.yawRate_Max = static_cast<float>(value);
      }

      else if (command == "roll_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.rollRate_Max = static_cast<float>(value);
      }
      else if (command == "bank_angle_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.bankAngle_Max = static_cast<float>(value);
      }
      else if (command == "forward_accel_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.forwardAccel_Min = static_cast<float>(value);
      }
      else if (command == "forward_accel_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.forwardAccel_Max = static_cast<float>(value);
      }
   }

   // Once the block has finished parsing, mDefaultLimitsAndSettings should be complete.
   // Set the current limits to be equal to the default limits.
   mCurrentLimitsAndSettings = mDefaultLimitsAndSettings;
}

void wsf::six_dof::CommonController::UpdateBankToTurn(sAutopilotControls& aControls, double aSimTime)
{
   // The bank-to-turn control method is typically more complex than the yaw-to-turn method.
   // In some cases, it must consider both the lateral and vertical channels together in
   // order to produce the best control response.

   // Process the horizontal/heading channel,
   // plus any requested yaw stabilization
   ProcessLaternalNavChannelsBankToTurn(aSimTime);

   // Process the vertical/altitude channel
   ProcessVerticalNavChannelBankToTurn(aSimTime);

   // Process the speed channel
   ProcessSpeedChannelBankToTurn(aSimTime);

   // Return the current controls
   aControls = mControlOutputs;
}

void wsf::six_dof::CommonController::UpdateYawToTurn(sAutopilotControls& aControls, double aSimTime)
{
   // The yaw-to-turn control method is simpler in many respects than the bank-to-turn.
   // Specifically, it can generally handle the lateral and vertical channels separately.

   // Process the horizontal/heading channel,
   // plus any requested roll stabilization
   ProcessLaternalNavChannelsYawToTurn(aSimTime);

   // Process the vertical/altitude channel
   ProcessVerticalNavChannelYawToTurn(aSimTime);

   // Process the speed channel
   ProcessSpeedChannelYawToTurn(aSimTime);

   // Return the current controls
   aControls = mControlOutputs;
}

double wsf::six_dof::CommonController::GetAimHeadingForWaypointNav_deg(double aSimTime)
{
   // No waypoint to travel to, so maintain last computed heading
   if (mCurrentActivityPtr->GetCurrWaypoint() == nullptr)
   {
      mNavData.executeTurn = false;
      return mNavData.aimHeading_rad * UtMath::cDEG_PER_RAD;
   }

   double aimHdg_rad = 0.0;

   KinematicState* state = GetParentVehicle()->GetKinematicState();

   UtLLAPos        currentPosition = state->GetCurrentPositionLLA();
   const UtLLAPos& currPos         = currentPosition;

   double currHdg_rad   = state->GetLocalHeading_rad();
   double currSpeed_fps = state->GetSpeed_fps();

   // Get speed at waypoint in m/sec
   double waypointSpeed_fps = currSpeed_fps;
   double currentAlt_m      = mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt();

   Waypoint::sSpeed speed = mCurrentActivityPtr->GetCurrWaypoint()->GetSpeed();
   switch (speed.type)
   {
   case Waypoint::SPD_TYPE_MACH:
      waypointSpeed_fps = mEnvironment->CalcFpsFromMach(currentAlt_m, speed.val);
      break;
   case Waypoint::SPD_TYPE_TAS_KNOTS:
      waypointSpeed_fps = UtMath::cFPS_PER_NMPH * speed.val;
      break;
   case Waypoint::SPD_TYPE_CAS_KNOTS:
      waypointSpeed_fps = mEnvironment->CalcFpsFromKcas(currentAlt_m, speed.val);
      break;
   case Waypoint::SPD_TYPE_FPS:
      waypointSpeed_fps = speed.val;
      break;
   default:
      break;
   }

   double  waypointSpeed_mps = waypointSpeed_fps * UtMath::cM_PER_FT;
   double  currSpeed_mps     = currSpeed_fps * UtMath::cM_PER_FT;
   UtVec2d currVel(state->GetSpeed_mps() * cos(currHdg_rad), state->GetSpeed_mps() * sin(currHdg_rad));

   // If roll is the control method, then call CalcAimHeadingAndBankAngle
   double dt_sec = aSimTime - utils::TimeToTime(mLastUpdateTime_nanosec);

   if (Control::IsBankToTurn((mControlMethod)))
   {
      Route::CalcAimHeadingAndBankAngle(mCurrentActivityPtr->GetPrevWaypoint(),
                                        mCurrentActivityPtr->GetCurrWaypoint(),
                                        mCurrentActivityPtr->GetNextWaypoint(),
                                        mCurrentActivityPtr->GetCurrSegment(),
                                        mCurrentActivityPtr->GetNextSegment(),
                                        currPos,
                                        currVel,
                                        mNavData,
                                        static_cast<double>(mCurrentLimitsAndSettings.turnRollInMultiplier),
                                        static_cast<double>(mDefaultLimitsAndSettings.routeAllowableAngleError_rad),
                                        currHdg_rad,
                                        waypointSpeed_mps,
                                        static_cast<double>(mCurrentLimitsAndSettings.bankAngle_Max) * UtMath::cRAD_PER_DEG,
                                        static_cast<double>(mCurrentLimitsAndSettings.rollRate_Max) * UtMath::cRAD_PER_DEG,
                                        static_cast<double>(mCurrentLimitsAndSettings.pitchGLoad_Max),
                                        dt_sec,
                                        mAchievedWaypoint);
   }
   // If yaw is the control method, then call CalcYawAimHeadingAngle
   else if (Control::IsYawToTurn((mControlMethod)))
   {
      Route::CalcYawAimHeadingAngle(mCurrentActivityPtr->GetPrevWaypoint(),
                                    mCurrentActivityPtr->GetCurrWaypoint(),
                                    mCurrentActivityPtr->GetNextWaypoint(),
                                    mCurrentActivityPtr->GetCurrSegment(),
                                    mCurrentActivityPtr->GetNextSegment(),
                                    currPos,
                                    currVel,
                                    mNavData,
                                    (double)mDefaultLimitsAndSettings.routeAllowableAngleError_rad,
                                    currHdg_rad,
                                    currSpeed_mps,
                                    (double)mCurrentLimitsAndSettings.pitchGLoad_Max,
                                    dt_sec,
                                    mAchievedWaypoint);
   }

   if (mCurrentActivityPtr->GetCurrWaypoint()->FollowHorizontalTrack())
   {
      aimHdg_rad = mNavData.aimHeading_rad;
   }
   else
   {
      aimHdg_rad = Route::GetInitialHeading_rad(currPos, mCurrentActivityPtr->GetCurrWaypoint()->GetLLA());
   }

   // Return the aim heading
   return aimHdg_rad * UtMath::cDEG_PER_RAD;
}

double wsf::six_dof::CommonController::GetAimHeadingForPoint_deg()
{
   UtLLAPos currentPosition = GetParentVehicle()->GetKinematicState()->GetCurrentPositionLLA();

   const UtLLAPos& currPos = currentPosition;

   double aimHdg_rad = Route::GetInitialHeading_rad(currPos, mCurrentActivityPtr->GetCurrWaypoint()->GetLLA());

   // Return the aim heading (in deg)
   return aimHdg_rad * UtMath::cDEG_PER_RAD;
}

double wsf::six_dof::CommonController::CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(double aTurnRadius_ft)
{
   // Get current speed
   double speed_fps = GetParentVehicle()->GetKinematicState()->GetSpeed_fps(); // TODO - We should use ground_speed_fps

   if (std::abs(aTurnRadius_ft) < 0.01) // We test to ensure we have a reasonable radius
   {
      aTurnRadius_ft = 0.01; // Set a limit (we do not want zero, for example)
   }

   double turnRate_rps = speed_fps / aTurnRadius_ft;

   return (turnRate_rps * UtMath::cDEG_PER_RAD);
}

double wsf::six_dof::CommonController::CalcTurnRadiusBasedOnTurnRateAndCurrentSpeed_ft(double aTurnRate_dps)
{
   // Get current speed
   double speed_fps = GetParentVehicle()->GetKinematicState()->GetSpeed_fps(); // TODO - We should use ground_speed_fps

   if (std::abs(aTurnRate_dps) < 1.0E-8) // Avoid a near zero rate
   {
      return 1.0E20; // Return a very large turn radius
   }

   double aTurnRate_rps = aTurnRate_dps * UtMath::cRAD_PER_DEG;

   double turnRadius_ft = speed_fps / aTurnRate_rps;

   return turnRadius_ft;
}

void wsf::six_dof::CommonController::GetAutoPilotTurnData(double& aAimHeading_rad,
                                                          double& aStartTurnHdg_rad,
                                                          double& aTurnRadius_m,
                                                          double& aTurnAngle_rad,
                                                          double& aTurnRefPtLat,
                                                          double& aTurnRefPtLon,
                                                          bool&   aTurnCW,
                                                          bool&   aTurning,
                                                          double& aTurnCenterLat,
                                                          double& aTurnCenterLon)
{
   aAimHeading_rad   = mAimHeading_rad;
   aStartTurnHdg_rad = mStartTurnHdg_rad;
   aTurnRadius_m     = mTurnRadius_m;
   aTurnAngle_rad    = mTurnAngle_rad;
   aTurnRefPtLat     = mTurnRefPoint.GetLat();
   aTurnRefPtLon     = mTurnRefPoint.GetLon();
   aTurnCW           = mTurnCW;
   aTurning          = mTurning;
   aTurnCenterLat    = mTurnCenterLat;
   aTurnCenterLon    = mTurnCenterLon;
}

void wsf::six_dof::CommonController::EnforceControlLimits()
{
   EnforceSingleControlLimit(mControlOutputs.stickBack, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.stickRight, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.rudderRight, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.throttleMilitary, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.throttleAfterburner, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.speedBrake, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.noseWheelSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.nwsSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.wheelBrakeLeft, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputs.wheelBrakeRight, 0.0, 1.0);
}

void wsf::six_dof::CommonController::EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue)
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

bool wsf::six_dof::CommonController::GetAutopilotPidGainData(Pid::Type   aTableType,
                                                             size_t&     aNumElements,
                                                             PidGainData aPidGainData[])
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* dataTables = pid->GetPidGainTableData();

      size_t numPidElements = dataTables->size();
      if (aNumElements >= numPidElements)
      {
         for (size_t i = 0; i < numPidElements; ++i)
         {
            aPidGainData[i] = dataTables->at(i);
         }
         aNumElements = numPidElements;
      }
      return true;
   }
   return false;
}

std::vector<wsf::six_dof::PidGainData>* wsf::six_dof::CommonController::GetAutopilotPidGainData(Pid::Type aTableType)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      return pid->GetPidGainTableData();
   }
   return nullptr;
}

void wsf::six_dof::CommonController::GetAutopilotPidValues(AutopilotPidGroupValueData* aData)
{
   mAlphaPID.GetPidValueData(aData->alphaPID);
   mVerticalSpeedPID.GetPidValueData(aData->verticalSpeedPID);
   mPitchAnglePID.GetPidValueData(aData->pitchAnglePID);
   mPitchRatePID.GetPidValueData(aData->pitchRatePID);
   mFlightPathAnglePID.GetPidValueData(aData->flightPathAnglePID);
   mDeltaPitchPID.GetPidValueData(aData->deltaPitchPID);
   mAltitudePID.GetPidValueData(aData->altitudePID);
   mBetaPID.GetPidValueData(aData->betaPID);
   mYawRatePID.GetPidValueData(aData->yawRatePID);
   mYawHeadingPID.GetPidValueData(aData->yawheadingPID);
   mTaxiHeadingPID.GetPidValueData(aData->taxiHeadingPID);
   mRollRatePID.GetPidValueData(aData->rollRatePID);
   mDeltaRollPID.GetPidValueData(aData->deltaRollPID);
   mBankAnglePID.GetPidValueData(aData->bankAnglePID);
   mRollHeadingPID.GetPidValueData(aData->rollHeadingPID);
   mForwardAccelPID.GetPidValueData(aData->forwardAccelPID);
   mSpeedPID.GetPidValueData(aData->speedPID);
   mTaxiForwardAccelPID.GetPidValueData(aData->taxiForwardAccelPID);
   mTaxiSpeedPID.GetPidValueData(aData->taxiSpeedPID);
   mTaxiYawRatePID.GetPidValueData(aData->taxiYawRatePID);
}

void wsf::six_dof::CommonController::SetPidGainData(Pid::Type aPidType, int aNumElements, PidGainData aPidGainData[])
{
   std::vector<PidGainData> tempDataTables;
   for (int i = 0; i < aNumElements; ++i)
   {
      PidGainData data = aPidGainData[i];
      tempDataTables.push_back(data);
   }

   PID* pid = GetPID_ByType(aPidType);
   if (pid != nullptr)
   {
      pid->SetPidGainTableData(tempDataTables);
   }
}

bool wsf::six_dof::CommonController::AddNewPidGainElement(Pid::Type aTableType, PidGainData& aPidGainData)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* data = pid->GetPidGainTableData();

      if (data != nullptr)
      {
         for (std::vector<PidGainData>::iterator iter = data->begin(); iter != data->end(); ++iter)
         {
            if (aPidGainData.ControllingValue < iter->ControllingValue)
            {
               data->insert(iter, aPidGainData);
               // Iterator is now invalid so just return
               return true;
            }
            if (aPidGainData.ControllingValue == iter->ControllingValue)
            {
               // Element with the same controlling value already exist. Vector must be monotonically increasing.
               return false;
            }
         }

         // If we reach here, then the controlling value but be greater than last element in the vector,
         //  thus push it onto the end of the vector;
         data->push_back(aPidGainData);
         return true;
      }
   }
   return false;
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_RollWaypoint(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForWaypointNav_deg(aSimTime);

   // Process PIDs
   ProcessStandardLateralNavMode_WaypointRollHeading(commandedHeading_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_RollPoint(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForPoint_deg();

   // Process PIDs
   ProcessStandardLateralNavMode_RollHeading(commandedHeading_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_RollHeading(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = static_cast<double>(mCurrentActivityPtr->GetHeading_deg());

   // Process PIDs
   ProcessStandardLateralNavMode_RollHeading(commandedHeading_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_RollHeading(double aHeading_deg, double aSimTime)
{
   double maxBankAngle_rad = mCurrentLimitsAndSettings.bankAngle_Max * UtMath::cRAD_PER_DEG;

   CalcLateralNavMode_RollHeadingCore(aHeading_deg, maxBankAngle_rad, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_WaypointRollHeading(double aHeading_deg, double aSimTime)
{
   double maxBankAngle_rad = std::min(mNavData.commandedBank_rad,
                                      static_cast<double>(mCurrentLimitsAndSettings.bankAngle_Max) * UtMath::cRAD_PER_DEG);

   CalcLateralNavMode_RollHeadingCore(aHeading_deg, maxBankAngle_rad, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_RollRate(double aCommandedRollRate_dps, double aSimTime)
{
   double currentRollRate_dps = GetParentVehicle()->GetKinematicState()->GetRollRate_dps();

   // Get the desired roll rate
   double commandedRollRate_dps = aCommandedRollRate_dps;

   // Store last command for future
   mLastCommandedRollRate_dps = aCommandedRollRate_dps;

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
   mControlOutputs.stickRight = mRollRatePID.CalcOutputFromTarget(commandedRollRate_dps, currentRollRate_dps, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_Beta(double aSimTime)
{
   double commandedBeta_deg = mCurrentActivityPtr->GetBeta_deg();
   ProcessStandardLateralNavMode_Beta(commandedBeta_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_YawGLoad(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return;
   }

   double commandedGLoad    = mCurrentActivityPtr->GetYawGLoad_g();
   double commandedBeta_deg = 0;

   GetParentVehicle()->CalculateBetaAtSpecifiedGLoad_deg(commandedGLoad, commandedBeta_deg);

   ProcessStandardLateralNavMode_Beta(commandedBeta_deg, aSimTime);
}

void wsf::six_dof::CommonController::CalcLateralNavMode_RollHeadingCore(double aHeading_deg,
                                                                        double aMaxBankAngle_rad,
                                                                        double aSimTime)
{
   // Get current states
   KinematicState* state              = GetParentVehicle()->GetKinematicState();
   double          currentHeading_deg = state->GetLocalHeading_deg();

   // Set the aim heading
   mAimHeading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;

   // Set outer and middle loop values (in case they are not executed this frame)
   double commandedBankAngle_deg = mLastComandedBankAngle_deg;

   // Set epsilon (used below)
   constexpr double epsilonVal = std::numeric_limits<double>::epsilon();

   // Check max bank angle
   if (aMaxBankAngle_rad < epsilonVal)
   {
      // Bank angle is so small we cannot maneuver
      mLastComandedBankAngle_deg = 0.0;
      ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aSimTime);
      return;
   }

   // Calc the heading error
   double hdgError_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - currentHeading_deg);

   // We previously explored a feed-forward for the mRollHeadingPID, but testing showed
   // that it was not needed and actually performed better without any feed-forward

   // Check max g
   double maxG = mCurrentLimitsAndSettings.pitchGLoad_Max;
   if (maxG < epsilonVal)
   {
      // Max g is so small we cannot maneuver
      mLastComandedBankAngle_deg = 0.0;
      ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aSimTime);
      return;
   }

   // Set the lateral g-load
   double lateral_g = 0.0;

   // If max bank angle is past 90 deg, limit max bank angle to PI/2
   if (aMaxBankAngle_rad > UtMath::cPI_OVER_2)
   {
      // We have a 90 deg bank, so lateral g-load is the maximum
      aMaxBankAngle_rad = UtMath::cPI_OVER_2;
      lateral_g         = maxG;
   }
   else
   {
      // Set lateral g based on a one g vertical component
      lateral_g = tan(aMaxBankAngle_rad);
   }

   // At this point, we will have a non-zero lateral g

   // Get the current pitch angle
   double pitchAngle_rad = state->GetLocalPitch_deg() * UtMath::cRAD_PER_DEG;

   // Use 89 degrees as the maximum effect - since it will have an effect of ~57x which is more than enough
   const double maxPitchFactorEffectAngle_rad = 89.0 * UtMath::cRAD_PER_DEG;

   // Set a pitch factor for pitch effects
   double pitchFactor = 1.0; // Default is 1 (no effect)

   // Limit the pitch angle
   if (pitchAngle_rad < -maxPitchFactorEffectAngle_rad)
   {
      pitchAngle_rad = -maxPitchFactorEffectAngle_rad;
   }
   else if (pitchAngle_rad > maxPitchFactorEffectAngle_rad)
   {
      pitchAngle_rad = maxPitchFactorEffectAngle_rad;
   }
   pitchFactor = 1.0 / cos(pitchAngle_rad);

   // Adjust lateral_g for pitch effects
   lateral_g *= pitchFactor;

   // Limit the lateral g to the maximum g
   if (lateral_g > maxG)
   {
      lateral_g = maxG;
   }

   // Get the current speed
   double currentSpeed_fps = state->GetSpeed_fps();

   // Set a minimum speed to consider - we'll use 0.001 ft/sec
   const double minSpeedToConsider_fps = 0.001; // This may need to be a vehicle-based value

   // Check for a low speed condition
   if (currentSpeed_fps < minSpeedToConsider_fps)
   {
      // The speed is so slow, we cannot maneuver
      mLastComandedBankAngle_deg = 0.0;
      ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aSimTime);
      return;
   }

   double radius_ft        = (currentSpeed_fps * currentSpeed_fps) / (32.174 * lateral_g);
   double circumference_ft = UtMath::cTWO_PI * radius_ft;
   double timeToCircle_sec = circumference_ft / currentSpeed_fps;

   // Check for a zero time to circle condition
   if (timeToCircle_sec < epsilonVal)
   {
      // We will simply turn at maximum bank angle

      if (hdgError_deg < 0)
      {
         commandedBankAngle_deg = -aMaxBankAngle_rad;
      }
      else
      {
         commandedBankAngle_deg = aMaxBankAngle_rad;
      }

      ProcessStandardLateralNavMode_Bank(commandedBankAngle_deg, aSimTime);
      return;
   }

   // Now, we need to determine the turn rate at the max bank angle (in a level turn)
   double maxTurnRate_dps = 360.0 / timeToCircle_sec;

   // Get the commanded turn rate from the roll heading PID
   double comandedTurnRate_dps = mRollHeadingPID.CalcOutputFromErrorWithLimits(hdgError_deg,
                                                                               currentHeading_deg,
                                                                               aSimTime,
                                                                               -maxTurnRate_dps,
                                                                               maxTurnRate_dps);

   // Check for a zero turn rate condition
   if (fabs(comandedTurnRate_dps) < epsilonVal)
   {
      // The turn rate is so slow, we assume level flight
      mLastComandedBankAngle_deg = 0.0;
      ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aSimTime);
      return;
   }

   // Now, we need to convert from turn rate into bank angle

   // First, get the time to circle, circumference, and radius
   timeToCircle_sec = 360.0 / fabs(comandedTurnRate_dps);
   circumference_ft = timeToCircle_sec * currentSpeed_fps;
   radius_ft        = circumference_ft / UtMath::cTWO_PI;

   // Next, calculate the the lateral g
   lateral_g = (currentSpeed_fps * currentSpeed_fps) / (radius_ft * 32.174);

   // Bank angle is a function of lateral g-load
   double bank_rad = atan2(lateral_g, pitchFactor);

   commandedBankAngle_deg = bank_rad * UtMath::cDEG_PER_RAD;

   // Adjust sign
   if (comandedTurnRate_dps < 0)
   {
      commandedBankAngle_deg *= -1.0;
   }

   if (commandedBankAngle_deg > mCurrentLimitsAndSettings.bankAngle_Max)
   {
      commandedBankAngle_deg = mCurrentLimitsAndSettings.bankAngle_Max;
   }
   else if (commandedBankAngle_deg < -mCurrentLimitsAndSettings.bankAngle_Max)
   {
      commandedBankAngle_deg = -mCurrentLimitsAndSettings.bankAngle_Max;
   }

   ProcessStandardLateralNavMode_Bank(commandedBankAngle_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_Bank(double aBankAngle_deg, double aSimTime)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedRollRate_dps = mLastCommandedRollRate_dps;

   // Get the desired bank angle
   double commandedBankAngle_deg = aBankAngle_deg;

   // Get current states
   double currentBank_deg = GetParentVehicle()->GetKinematicState()->GetLocalRoll_deg();

   // Limit the commanded vert speed
   if (commandedBankAngle_deg < -mCurrentLimitsAndSettings.bankAngle_Max)
   {
      commandedBankAngle_deg = -mCurrentLimitsAndSettings.bankAngle_Max;
   }
   else if (commandedBankAngle_deg > mCurrentLimitsAndSettings.bankAngle_Max)
   {
      commandedBankAngle_deg = mCurrentLimitsAndSettings.bankAngle_Max;
   }

   double bankError_deg = UtMath::NormalizeAngleMinus180_180(commandedBankAngle_deg - currentBank_deg);

   // Get the desired g-load with g-limits
   commandedRollRate_dps = mBankAnglePID.CalcOutputFromErrorWithLimits(bankError_deg,
                                                                       currentBank_deg,
                                                                       aSimTime,
                                                                       -mCurrentLimitsAndSettings.rollRate_Max,
                                                                       mCurrentLimitsAndSettings.rollRate_Max);

   // Execute Inner Loop
   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aSimTime);

   // Store last command for future
   mLastComandedBankAngle_deg = aBankAngle_deg;
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_YawHeading(double aHeading_deg, double aSimTime)
{
   // Get current states
   double currentHeading_deg = GetParentVehicle()->GetKinematicState()->GetLocalHeading_deg();

   // Set the aim heading
   mAimHeading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;

   // Set outer loop value (in case it is not executed this frame)
   double commandedYawRate_dps = mLastCommandedYawRate_dps;

   // Calc the heading error
   double hdgError_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - currentHeading_deg);

   // Get the commanded roll/bank angle
   commandedYawRate_dps = mYawHeadingPID.CalcOutputFromErrorWithLimits(hdgError_deg,
                                                                       currentHeading_deg,
                                                                       aSimTime,
                                                                       -mCurrentLimitsAndSettings.yawRate_Max,
                                                                       mCurrentLimitsAndSettings.yawRate_Max);

   ProcessStandardLateralNavMode_YawRate(commandedYawRate_dps, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_YawRate(double aCommandedYawRate_dps, double aSimTime)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedBeta_deg = mLastCommandedBeta_deg;

   // Get the desired yaw rate
   double commandedYawRate_dps = aCommandedYawRate_dps;

   // Get rate limits
   double minRate = -mCurrentLimitsAndSettings.yawRate_Max;
   double maxRate = mCurrentLimitsAndSettings.yawRate_Max;

   // Limit the command
   if (commandedYawRate_dps < minRate)
   {
      commandedYawRate_dps = minRate;
   }
   else if (commandedYawRate_dps > maxRate)
   {
      commandedYawRate_dps = maxRate;
   }

   // Get current states
   double currentYawRate_dps = GetParentVehicle()->GetKinematicState()->GetYawRate_dps();

   bool usingYawToTurn = (mControlMethod == Control::YawToTurnNoRoll) || (mControlMethod == Control::YawToTurnRollRate) ||
                         (mControlMethod == Control::YawToTurnZeroBank);

   if (usingYawToTurn)
   {
      double spd_fps                 = GetParentVehicle()->GetKinematicState()->GetSpeed_fps();
      double commandedYawRate_rps    = UtMath::cRAD_PER_DEG * commandedYawRate_dps;
      double targetAcceleration_fps2 = spd_fps * commandedYawRate_rps;
      double targetAcceleration_g    = targetAcceleration_fps2 / 32.174;

      // Convert g to beta
      double feedForwardBeta_deg = 0;
      GetParentVehicle()->CalculateBetaAtSpecifiedGLoad_deg(targetAcceleration_g, feedForwardBeta_deg);

      // Set bias
      mYawRatePID.SetBias(feedForwardBeta_deg);
   }

   // Beta is opposite in sign compared to yaw rate
   commandedBeta_deg = -mYawRatePID.CalcOutputFromTargetWithLimits(commandedYawRate_dps,
                                                                   currentYawRate_dps,
                                                                   aSimTime,
                                                                   -mLimitedBeta_deg,
                                                                   mLimitedBeta_deg);

   ProcessStandardLateralNavMode_Beta(commandedBeta_deg, aSimTime);

   // Store last command for future
   mLastCommandedYawRate_dps = aCommandedYawRate_dps;
}

void wsf::six_dof::CommonController::ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aSimTime)
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
   else if (commandedBeta_deg > mLimitedBeta_deg)
   {
      commandedBeta_deg = mLimitedBeta_deg;
   }

   // Execute Inner Loop -- Notice the negation of the PID output (rudder command is opposite to beta)
   mControlOutputs.rudderRight = -mBetaPID.CalcOutputFromTarget(commandedBeta_deg, currentBeta_deg, aSimTime);

   // Store last command for future
   mLastCommandedBeta_deg = commandedBeta_deg;
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_YawWaypoint(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForWaypointNav_deg(aSimTime);

   // Process PIDs
   ProcessStandardLateralNavMode_YawHeading(commandedHeading_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_YawPoint(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForPoint_deg();

   // Process PIDs
   ProcessStandardLateralNavMode_YawHeading(commandedHeading_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_YawHeading(double aSimTime)
{
   // Get commanded heading
   double commandedHeading_deg = static_cast<double>(mCurrentActivityPtr->GetHeading_deg());

   // Process PIDs
   ProcessStandardLateralNavMode_YawHeading(commandedHeading_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_YawRate(double aSimTime)
{
   double commandedYawRate_dps = mCurrentActivityPtr->GetYawRate_dps();

   ProcessStandardLateralNavMode_YawRate(commandedYawRate_dps, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_RollRate(double aSimTime)
{
   double commandedRollRate_dps = static_cast<double>(mCurrentActivityPtr->GetRollRate_dps());

   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_Bank(double aSimTime)
{
   double comandedBankAngle_deg = mCurrentActivityPtr->GetBank_deg();

   ProcessStandardLateralNavMode_Bank(comandedBankAngle_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_DeltaRoll(double aSimTime)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedRollRate_dps = mLastCommandedRollRate_dps;

   // Get the desired angle
   double commandedDeltaRollAngle_deg = mCurrentActivityPtr->GetRollDelta_deg();

   // Get current states
   double currentRollDelta_deg = mIntegratedDeltaRoll_deg;

   // Get the desired g-load with g-limits
   commandedRollRate_dps = mDeltaRollPID.CalcOutputFromTargetWithLimits(commandedDeltaRollAngle_deg,
                                                                        currentRollDelta_deg,
                                                                        aSimTime,
                                                                        -mCurrentLimitsAndSettings.rollRate_Max,
                                                                        mCurrentLimitsAndSettings.rollRate_Max);

   // Execute Inner Loop
   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aSimTime);
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_NoControl()
{
   ProcessLateralNavMode_NoRollControl();
   ProcessLateralNavMode_NoYawControl();
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_NoYawControl()
{
   mControlOutputs.rudderRight = 0.0;
}

void wsf::six_dof::CommonController::ProcessLateralNavMode_NoRollControl()
{
   mControlOutputs.stickRight = 0.0;
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_Waypoint(double aSimTime)
{
   double commandedAltMSL_ft = 0.0;

   // If there is a current waypoint
   if (mCurrentActivityPtr->GetCurrWaypoint() != nullptr)
   {
      commandedAltMSL_ft = mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt() * UtMath::cFT_PER_M;
   }
   else if (mCurrentActivityPtr->GetPrevWaypoint() != nullptr)
   {
      commandedAltMSL_ft = mCurrentActivityPtr->GetPrevWaypoint()->GetLLA().GetAlt() * UtMath::cFT_PER_M;
   }

   if (mCurrentActivityPtr->GetCurrWaypoint() != nullptr)
   {
      // Commanded to follow a vertical track
      if (mCurrentActivityPtr->GetCurrWaypoint()->FollowVerticalTrack())
      {
         Route::CalcVerticalSpeed(mNavData);

         double vertRate_fpm = (mNavData.vertSpeed_mps * UtMath::cFT_PER_M * 60.0);

         // If the current waypoint is at a different altitude than the previous waypoint, command the
         // vertical rate needed to follow the track
         if (mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt() !=
             mCurrentActivityPtr->GetPrevWaypoint()->GetLLA().GetAlt())
         {
            ProcessStandardVerticalNavMode_VertSpeed(vertRate_fpm, aSimTime);
         }
         else
         {
            // No change in altitude, so just maintain altitude
            ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aSimTime);
         }
      }
      else // Default to attaining target altitude as quickly as possible
      {
         // Follow normal approach to waypoint altitude
         //  commandedAltMSL_ft = mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt()*UtMath::cFT_PER_M;
         ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aSimTime);
      }
   }
   else
   {
      // No waypoint is set, so maintain current altitude
      ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aSimTime);
   }
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_Altitude(double aSimTime)
{
   double commandedAltMSL_ft = static_cast<double>(mCurrentActivityPtr->GetAltitudeMSL_ft());
   ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_VertSpeed(double aSimTime)
{
   double commandedVertRate_fpm = mCurrentActivityPtr->GetVerticalRate_fpm();
   ProcessStandardVerticalNavMode_VertSpeed(commandedVertRate_fpm, aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_PitchGLoad(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return;
   }

   double commandedGLoad     = mCurrentActivityPtr->GetPitchGLoad_g();
   double commandedAlpha_deg = 0;

   GetParentVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(commandedGLoad, commandedAlpha_deg);

   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardVerticalNavMode_Altitude(double aAltitude_ft, double aSimTime)
{
   // Set outer and middle loop values (in case they are not executed this frame)
   double commandedVertRate_fpm = mLastCommandedVertRate_fpm;

   // Get current states
   double currentAlt_ft = GetParentVehicle()->GetKinematicState()->GetAlt_ft();

   // Get the desired altitude
   double commandedAltMSL_ft = aAltitude_ft;

   // Get the commanded vertical speed with vert speed limits
   commandedVertRate_fpm = mAltitudePID.CalcOutputFromTargetWithLimits(commandedAltMSL_ft,
                                                                       currentAlt_ft,
                                                                       aSimTime,
                                                                       mCurrentLimitsAndSettings.vertSpd_Min,
                                                                       mCurrentLimitsAndSettings.vertSpd_Max);

   // Execute Middle and Inner Loops
   ProcessStandardVerticalNavMode_VertSpeed(commandedVertRate_fpm, aSimTime);
}

void wsf::six_dof::CommonController::CalcGBiasData(double& aGBias_g, double& aGBiasAlpha_deg)
{
   // We start by setting the g-bias (in terms of g-load) to unity
   aGBias_g = 1.0;

   // Get current states
   double pitchAngle_rad = GetParentVehicle()->GetKinematicState()->GetLocalPitch_rad();
   double rollAngle_rad  = GetParentVehicle()->GetKinematicState()->GetLocalRoll_rad();

   // Include the G-bias due to pitch angle
   double cosRoll = cos(rollAngle_rad);

   if (cosRoll == 0.0)
   {
      aGBias_g = mCurrentLimitsAndSettings.pitchGLoad_Max;
   }
   else
   {
      aGBias_g = 1.0 / cosRoll;
   }

   // Include the G-bias due to pitch angle
   aGBias_g *= cos(pitchAngle_rad);

   // Limit g
   if (aGBias_g > mCurrentLimitsAndSettings.pitchGLoad_Max)
   {
      aGBias_g = mCurrentLimitsAndSettings.pitchGLoad_Max;
   }

   if (aGBias_g < mCurrentLimitsAndSettings.pitchGLoad_Min)
   {
      aGBias_g = mCurrentLimitsAndSettings.pitchGLoad_Min;
   }

   // Get the alpha for the given g-load
   GetParentVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(aGBias_g, aGBiasAlpha_deg);
}

void wsf::six_dof::CommonController::ProcessStandardVerticalNavMode_VertSpeed(double aCommandedVertSpeed_fpm, double aSimTime)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   // Get the desired vertical speed
   double commandedVertRate_fpm = aCommandedVertSpeed_fpm;

   // Limit the commanded vert speed
   if (commandedVertRate_fpm < mCurrentLimitsAndSettings.vertSpd_Min)
   {
      commandedVertRate_fpm = mCurrentLimitsAndSettings.vertSpd_Min;
   }
   else if (commandedVertRate_fpm > mCurrentLimitsAndSettings.vertSpd_Max)
   {
      commandedVertRate_fpm = mCurrentLimitsAndSettings.vertSpd_Max;
   }

   // Get current states
   double currentVertSpeed_fpm = GetParentVehicle()->GetKinematicState()->GetVerticalSpeed_fpm();

   mVerticalSpeedPID.SetBias(mCurrentGBiasAlpha_deg);

   // Get the desired g-load with g-limits
   commandedAlpha_deg = mVerticalSpeedPID.CalcOutputFromTargetWithLimits(commandedVertRate_fpm,
                                                                         currentVertSpeed_fpm,
                                                                         aSimTime,
                                                                         mLimitedMinAlpha_deg,
                                                                         mLimitedMaxAlpha_deg);

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);

   // Store last command for future
   mLastCommandedVertRate_fpm = aCommandedVertSpeed_fpm;
}

void wsf::six_dof::CommonController::ProcessStandardVerticalNavMode_PitchRate(double aCommandedPitchRate_dps, double aSimTime)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   // Set outer and middle loop values (in case they are not executed this frame)
   double currentPitchRate_dps = GetParentVehicle()->GetKinematicState()->GetPitchRate_dps();

   // Get the desired rate
   double commandedPitchRate_dps = aCommandedPitchRate_dps;

   // Limit the command

   if (commandedPitchRate_dps < mCurrentLimitsAndSettings.pitchRate_Min)
   {
      commandedPitchRate_dps = mCurrentLimitsAndSettings.pitchRate_Min;
   }

   if (commandedPitchRate_dps > mCurrentLimitsAndSettings.pitchRate_Max)
   {
      commandedPitchRate_dps = mCurrentLimitsAndSettings.pitchRate_Max;
   }

   double spd_fps                 = GetParentVehicle()->GetKinematicState()->GetSpeed_fps();
   double commandedPitchRate_rps  = UtMath::cRAD_PER_DEG * commandedPitchRate_dps;
   double targetAcceleration_fps2 = spd_fps * commandedPitchRate_rps;
   double targetAcceleration_g    = targetAcceleration_fps2 / 32.174;

   if (Control::IsBankToTurn((mControlMethod)))
   {
      targetAcceleration_g += 1.0;
   }

   targetAcceleration_g *= mCurrentGBias_g;

   // Convert g to alpha
   double feedForwardAlpha_deg = 0;
   GetParentVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(targetAcceleration_g, feedForwardAlpha_deg);

   // Set bias
   mPitchRatePID.SetBias(feedForwardAlpha_deg);

   // Get the desired pitch rate within limits
   commandedAlpha_deg = mPitchRatePID.CalcOutputFromTargetWithLimits(commandedPitchRate_dps,
                                                                     currentPitchRate_dps,
                                                                     aSimTime,
                                                                     mLimitedMinAlpha_deg,
                                                                     mLimitedMaxAlpha_deg);

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg, double aSimTime)
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

   // Execute Inner Loop
   mControlOutputs.stickBack = mAlphaPID.CalcOutputFromTarget(commandedAlpha_deg, currentAlpha_deg, aSimTime);
   // Store last command for future
   mLastCommandedAlpha_deg = aCommandedAlpha_deg;
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_Point(double) // aSimTime)
{
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_PitchAng(double aSimTime)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   // Set outer and middle loop values (in case they are not executed this frame)
   double currentPitchAngle_deg = GetParentVehicle()->GetKinematicState()->GetLocalPitch_deg();

   // Get the desired angle
   double commandedPitchAngle_deg = mCurrentActivityPtr->GetPitchAngle_deg();

   // Limit the command using +/-90 deg
   if (commandedPitchAngle_deg < -90.0)
   {
      commandedPitchAngle_deg = -90.0;
   }
   if (commandedPitchAngle_deg > 90.0)
   {
      commandedPitchAngle_deg = 90.0;
   }

   // Set bias
   mPitchAnglePID.SetBias(mCurrentGBiasAlpha_deg);

   // Get the desired pitch rate within limits
   commandedAlpha_deg = mPitchAnglePID.CalcOutputFromTargetWithLimits(commandedPitchAngle_deg,
                                                                      currentPitchAngle_deg,
                                                                      aSimTime,
                                                                      mLimitedMinAlpha_deg,
                                                                      mLimitedMaxAlpha_deg);

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_PitchRate(double aSimTime)
{
   ProcessStandardVerticalNavMode_PitchRate(mCurrentActivityPtr->GetPitchRate_dps(), aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_FltPathAng(double aSimTime)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   // Set outer and middle loop values (in case they are not executed this frame)
   double currentFltPathAngle_deg = GetParentVehicle()->GetKinematicState()->GetFlightPathAngle_deg();

   // Get the desired angle
   double commandedFltPathAngle_deg = mCurrentActivityPtr->GetFlightPathAngle_deg();

   // Limit the command using +/-90 deg
   if (commandedFltPathAngle_deg < -90.0)
   {
      commandedFltPathAngle_deg = -90.0;
   }
   if (commandedFltPathAngle_deg > 90.0)
   {
      commandedFltPathAngle_deg = 90.0;
   }

   // Set bias
   mFlightPathAnglePID.SetBias(mCurrentGBiasAlpha_deg);

   // Get the desired pitch rate within limits
   commandedAlpha_deg = mFlightPathAnglePID.CalcOutputFromTargetWithLimits(commandedFltPathAngle_deg,
                                                                           currentFltPathAngle_deg,
                                                                           aSimTime,
                                                                           mLimitedMinAlpha_deg,
                                                                           mLimitedMaxAlpha_deg);

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_DeltaPitch(double aSimTime)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   // Get current states
   double currentPitchDelta_deg = mIntegratedDeltaPitch_deg;

   // Get the desired angle
   double commandedDeltaPitchAngle_deg = mCurrentActivityPtr->GetDeltaPitch_deg();

   // Set bias
   mDeltaPitchPID.SetBias(mCurrentGBiasAlpha_deg);

   // Get the desired pitch rate within limits
   commandedAlpha_deg = mDeltaPitchPID.CalcOutputFromTargetWithLimits(commandedDeltaPitchAngle_deg,
                                                                      currentPitchDelta_deg,
                                                                      aSimTime,
                                                                      mLimitedMinAlpha_deg,
                                                                      mLimitedMaxAlpha_deg);

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_Alpha(double aSimTime)
{
   double commandedAlpha_deg = mCurrentActivityPtr->GetAlpha_deg();
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aSimTime);
}

void wsf::six_dof::CommonController::ProcessVerticalNavMode_NoControl(double) // aSimTime)
{
   mControlOutputs.stickBack = 0.0;
}

double wsf::six_dof::CommonController::ProcessSpeedMode_Waypoint(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   } // Return if we lack a parent

   if (mEnvironment == nullptr)
   {
      return 0.0;
   } // Return if we lack an atm

   // Get current states
   double tgtSpeed_fps = GetParentVehicle()->GetKinematicState()->GetSpeed_fps();

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

   double currentAlt_ft = GetParentVehicle()->GetKinematicState()->GetAlt_ft();
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

   return ProcessStandardSpeedMode_FPS(tgtSpeed_fps, aSimTime);
}

double wsf::six_dof::CommonController::ProcessSpeedMode_ForwardAccel(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   double commandedAccel_g = mCurrentActivityPtr->GetForwardAcceleration_g();

   return ProcessStandardForwardAccel(commandedAccel_g, aSimTime);
}

double wsf::six_dof::CommonController::ProcessStandardSpeedMode_FPS(double aSpeed_fps, double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   double currentSpeed_fps = GetParentVehicle()->GetKinematicState()->GetSpeed_fps();

   double commandedSpeed_fps = aSpeed_fps;

   double biasThrottle = 0.0;
   double drag         = GetParentVehicle()->GetDrag_lbs();
   double alpha_rad    = GetParentVehicle()->GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double cosAngle     = cos(alpha_rad);
   double maxThrust    = GetParentVehicle()->GetMaximumPotentialThrust_lbs() * cosAngle;
   double minThrust    = GetParentVehicle()->GetMinimumPotentialThrust_lbs() * cosAngle;
   double deltaThrust  = maxThrust - minThrust;

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
      mSpeedPID.CalcOutputFromTargetWithLimits(commandedSpeed_fps, currentSpeed_fps, aSimTime, minOutput, maxOutput);
   return commandedThrottle;
}

double wsf::six_dof::CommonController::ProcessStandardForwardAccel(double aForwardAccel_g, double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   // TODO --- For now use Nx, but that is not accurate
   double currentAccel = GetParentVehicle()->GetKinematicState()->GetNx_g();
   double biasThrottle = 0.0;
   double drag         = GetParentVehicle()->GetDrag_lbs();
   double alpha_rad    = GetParentVehicle()->GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double cosAngle     = cos(alpha_rad);
   double maxThrust    = GetParentVehicle()->GetMaximumPotentialThrust_lbs() * cosAngle;
   double minThrust    = GetParentVehicle()->GetMinimumPotentialThrust_lbs() * cosAngle;
   double deltaThrust  = maxThrust - minThrust;

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

   mForwardAccelPID.SetBias(biasThrottle);

   // Calc limits
   double accelMax_g = static_cast<double>(mCurrentLimitsAndSettings.forwardAccel_Max);
   double accelMin_g = static_cast<double>(mCurrentLimitsAndSettings.forwardAccel_Min);

   double commandedAccel_g = aForwardAccel_g;
   if (aForwardAccel_g > accelMax_g)
   {
      commandedAccel_g = accelMax_g;
   }
   if (aForwardAccel_g < accelMin_g)
   {
      commandedAccel_g = accelMin_g;
   }

   // Execute Inner Loop
   double speedCommand_g = mForwardAccelPID.CalcOutputFromTarget(commandedAccel_g, currentAccel, aSimTime);
   return speedCommand_g;
}

double wsf::six_dof::CommonController::ProcessStandardSpeedMode_Throttle(double aThrottle, double /*aSimTime*/)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   // No PID input - straight to controller
   return aThrottle;
}

double wsf::six_dof::CommonController::ProcessSpeedMode_KIAS(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   double alt_m              = GetParentVehicle()->GetKinematicState()->GetAlt_m();
   double commmanded_kias    = mCurrentActivityPtr->GetCalibratedAirSpeed_KCAS();
   double commandedSpeed_fps = mEnvironment->CalcFpsFromKcas(alt_m, commmanded_kias);

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aSimTime);
}

double wsf::six_dof::CommonController::ProcessSpeedMode_KTAS(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   double commmanded_ktas    = mCurrentActivityPtr->GetTrueAirSpeed_KTAS();
   double commandedSpeed_fps = UtMath::cFPS_PER_NMPH * commmanded_ktas;

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aSimTime);
}

double wsf::six_dof::CommonController::ProcessSpeedMode_Mach(double aSimTime)
{
   if (GetParentVehicle() == nullptr)
   {
      return 0.0;
   }

   double alt_m              = GetParentVehicle()->GetKinematicState()->GetAlt_m();
   double commmanded_mach    = mCurrentActivityPtr->GetMach();
   double commandedSpeed_fps = mEnvironment->CalcFpsFromMach(alt_m, commmanded_mach);

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aSimTime);
}

double wsf::six_dof::CommonController::ProcessSpeedMode_FPS(double aSimTime)
{
   double commandedSpeed_fps = static_cast<double>(mCurrentActivityPtr->GetSpeed_fps());

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aSimTime);
}

double wsf::six_dof::CommonController::ProcessSpeedMode_Throttle(double aSimTime)
{
   double commandedThrottle = static_cast<double>(mCurrentActivityPtr->GetThrottle());

   return ProcessStandardSpeedMode_Throttle(commandedThrottle, aSimTime);
}

double wsf::six_dof::CommonController::ProcessSpeedMode_NoControl()
{
   // Previously, we kept the throttle set to full with no control
   // mControlOutputs.throttleAfterburner = 1.0f;
   // mControlOutputs.throttleMilitary    = 1.0f;
   // Now, we do not move the throttle at all -- we do nothing

   return 0.0;
}

void wsf::six_dof::CommonController::CalcAlphaBetaGLimits()
{
   // Calculate the g-bias (in g-load and alpha) to hold 1-g, given
   // the current pitch and roll
   CalcGBiasData(mCurrentGBias_g, mCurrentGBiasAlpha_deg);

   // Direct alpha limits
   double alphaMin_deg = static_cast<double>(mCurrentLimitsAndSettings.alpha_Min);
   double alphaMax_deg = static_cast<double>(mCurrentLimitsAndSettings.alpha_Max);

   // G-load-induced alpha limits
   double pitchGLoadMax   = static_cast<double>(mCurrentLimitsAndSettings.pitchGLoad_Max);
   double pitchGLoadMin   = static_cast<double>(mCurrentLimitsAndSettings.pitchGLoad_Min);
   double alphaAtMaxG_deg = 0.0;
   double alphaAtMinG_deg = 0.0;
   GetParentVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(pitchGLoadMax, alphaAtMaxG_deg);
   GetParentVehicle()->CalculateAlphaAtSpecifiedGLoad_deg(pitchGLoadMin, alphaAtMinG_deg);

   // We now need to determine the alpha limits, considering both direct
   // alpha limits and g-load induced alpha limits

   if (alphaAtMaxG_deg < alphaMax_deg)
   {
      // Use the restrained g-load alpha instead
      mLimitedMaxAlpha_deg = alphaAtMaxG_deg;

      if (alphaAtMinG_deg > alphaMin_deg)
      {
         // Use the restrained g-load alpha instead
         mLimitedMinAlpha_deg = alphaAtMinG_deg;
      }
      else
      {
         mLimitedMinAlpha_deg = alphaMin_deg;
      }
   }
   else
   {
      mLimitedMaxAlpha_deg = alphaMax_deg;

      if (alphaAtMinG_deg > alphaMin_deg)
      {
         // Use the restrained g-load alpha instead
         mLimitedMinAlpha_deg = alphaAtMinG_deg;
      }
      else
      {
         mLimitedMinAlpha_deg = alphaMin_deg;
      }
   }

   // Check for strange limits
   if (mLimitedMaxAlpha_deg < mLimitedMinAlpha_deg)
   {
      mLimitedMinAlpha_deg = mLimitedMaxAlpha_deg;
   }

   // Direct beta limits
   double betaMax_deg = static_cast<double>(mCurrentLimitsAndSettings.beta_Max);

   if (Control::IsYawToTurn(mControlMethod))
   {
      // G-load-induced beta limits
      double yawGLoadMax    = static_cast<double>(mCurrentLimitsAndSettings.yawGLoad_Max);
      double betaAtMaxG_deg = 0.0;
      GetParentVehicle()->CalculateBetaAtSpecifiedGLoad_deg(yawGLoadMax, betaAtMaxG_deg);
      betaAtMaxG_deg = std::abs(betaAtMaxG_deg); // Enforce positive

      // We now need to determine the beta limits, considering both
      // direct beta limits and g-load induced beta limits
      if (betaAtMaxG_deg < betaMax_deg)
      {
         mLimitedBeta_deg = betaAtMaxG_deg;
      }
      else
      {
         mLimitedBeta_deg = betaMax_deg;
      }
   }
   else
   {
      mLimitedBeta_deg = betaMax_deg;
   }
}

void wsf::six_dof::CommonController::AngleDeltas(double aYaw_rad, double aPitch_rad, double aRoll_rad)
{
   mIntegratedDeltaYaw_deg += (aYaw_rad * UtMath::cDEG_PER_RAD);
   mIntegratedDeltaPitch_deg += (aPitch_rad * UtMath::cDEG_PER_RAD);
   mIntegratedDeltaRoll_deg += (aRoll_rad * UtMath::cDEG_PER_RAD);
}

void wsf::six_dof::CommonController::ResetLateralDeltaAngles()
{
   mIntegratedDeltaYaw_deg  = 0;
   mIntegratedDeltaRoll_deg = 0;
}

void wsf::six_dof::CommonController::ResetAllDeltaAngles()
{
   mIntegratedDeltaYaw_deg   = 0;
   mIntegratedDeltaPitch_deg = 0;
   mIntegratedDeltaRoll_deg  = 0;
}

void wsf::six_dof::CommonController::ResetAccumulatedPidData()
{
   for (Pid::Type pidType = static_cast<Pid::Type>(1); pidType != Pid::LastPidType;
        pidType           = static_cast<Pid::Type>(pidType + 1))
   {
      PID* pid = GetPID_ByType(pidType);
      if (pid != nullptr)
      {
         pid->ResetPidState();
      }
   }
}

void wsf::six_dof::CommonController::ResetAllPidTimings()
{
   for (Pid::Type pidType = static_cast<Pid::Type>(1); pidType != Pid::LastPidType;
        pidType           = static_cast<Pid::Type>(pidType + 1))
   {
      PID* pid = GetPID_ByType(pidType);
      if (pid != nullptr)
      {
         pid->ResetPidTiming();
      }
   }
}

bool wsf::six_dof::CommonController::SetPidGainKP(Pid::Type aTableType, float aControllingValue, float aGainValue)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* data = pid->GetPidGainTableData();

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.KpGain = aGainValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool wsf::six_dof::CommonController::SetPidGainKI(Pid::Type aTableType, float aControllingValue, float aGainValue)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* data = pid->GetPidGainTableData();

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.KiGain = aGainValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool wsf::six_dof::CommonController::SetPidGainKD(Pid::Type aTableType, float aControllingValue, float aGainValue)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* data = pid->GetPidGainTableData();

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.KdGain = aGainValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool wsf::six_dof::CommonController::SetPidAlpha(Pid::Type aTableType, float aControllingValue, float aAlphaValue)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* data = pid->GetPidGainTableData();

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.LowpassAlpha = aAlphaValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool wsf::six_dof::CommonController::SetPidMaxAccum(Pid::Type aTableType, float aControllingValue, float aMaxAccumValue)
{
   PID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<PidGainData>* data = pid->GetPidGainTableData();

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.MaxAccum = aMaxAccumValue;
               return true;
            }
         }
      }
   }
   return false;
}

void wsf::six_dof::CommonController::SetCurrentActivity(AutopilotAction* aAPActivity)
{
   mCurrentActivityPtr = aAPActivity;

   // If we achieved the waypoint, we do not clear the turning flag
   if (mAchievedWaypoint)
   {
      // Clear the flag
      mAchievedWaypoint = false;
   }
   else
   {
      // Since we did not achieve the waypoint
      mTurning = false;
   }
}

void wsf::six_dof::CommonController::SetCurrentLimitsAndSettings(const AutopilotLimitsAndSettings& aData)
{
   mCurrentLimitsAndSettings = aData;
}

void wsf::six_dof::CommonController::SetCurrentMinPitchGLimit(double aMinPitchGLimit)
{
   mCurrentLimitsAndSettings.pitchGLoad_Min = static_cast<float>(aMinPitchGLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxPitchGLimit(double aMaxPitchGLimit)
{
   mCurrentLimitsAndSettings.pitchGLoad_Max = static_cast<float>(aMaxPitchGLimit);
}

void wsf::six_dof::CommonController::SetCurrentMinAlphaLimit(double aMinAlphaLimit)
{
   mCurrentLimitsAndSettings.alpha_Min = static_cast<float>(aMinAlphaLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxAlphaLimit(double aMaxAlphaLimit)
{
   mCurrentLimitsAndSettings.alpha_Max = static_cast<float>(aMaxAlphaLimit);
}

void wsf::six_dof::CommonController::SetCurrentMinPitchRateLimit(double aMinPitchRateLimit)
{
   mCurrentLimitsAndSettings.pitchRate_Min = static_cast<float>(aMinPitchRateLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxPitchRateLimit(double aMaxPitchRateLimit)
{
   mCurrentLimitsAndSettings.pitchRate_Max = static_cast<float>(aMaxPitchRateLimit);
}

void wsf::six_dof::CommonController::SetCurrentMinVertSpeedLimit(double aMinVertSpeedLimit)
{
   mCurrentLimitsAndSettings.vertSpd_Min = static_cast<float>(aMinVertSpeedLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxVertSpeedLimit(double aMaxVertSpeedLimit)
{
   mCurrentLimitsAndSettings.vertSpd_Max = static_cast<float>(aMaxVertSpeedLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxYawGLimit(double aMaxYawGLimit)
{
   mCurrentLimitsAndSettings.yawGLoad_Max = static_cast<float>(aMaxYawGLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxBetaLimit(double aMaxBetaLimit)
{
   mCurrentLimitsAndSettings.beta_Max = static_cast<float>(aMaxBetaLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxYawRateLimit(double aMaxYawRateLimit)
{
   mCurrentLimitsAndSettings.yawRate_Max = static_cast<float>(aMaxYawRateLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxRollRateLimit(double aMaxRollRateLimit)
{
   mCurrentLimitsAndSettings.rollRate_Max = static_cast<float>(aMaxRollRateLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxBankAngleLimit(double aMaxBankAngleLimit)
{
   mCurrentLimitsAndSettings.bankAngle_Max = static_cast<float>(aMaxBankAngleLimit);
}

void wsf::six_dof::CommonController::SetCurrentMinForwardAccelLimit(double aMinForwardAccelLimit)
{
   mCurrentLimitsAndSettings.forwardAccel_Min = static_cast<float>(aMinForwardAccelLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxForwardAccelLimit(double aMaxForwardAccelLimit)
{
   mCurrentLimitsAndSettings.forwardAccel_Max = static_cast<float>(aMaxForwardAccelLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxTaxiSpeedLimit(double aMaxTaxiSpeedLimit)
{
   mCurrentLimitsAndSettings.taxiSpeed_Max = static_cast<float>(aMaxTaxiSpeedLimit);
}

void wsf::six_dof::CommonController::SetCurrentMaxTaxiYawRateLimit(double aMaxTaxiYawRateLimit)
{
   mCurrentLimitsAndSettings.taxiYawRate_Max = static_cast<float>(aMaxTaxiYawRateLimit);
}

double wsf::six_dof::CommonController::GetDefaultTurnRollInMultiplier()
{
   return (double)mDefaultLimitsAndSettings.turnRollInMultiplier;
}

double wsf::six_dof::CommonController::GetDefaultRouteAllowableAngleError_rad()
{
   return (double)mDefaultLimitsAndSettings.routeAllowableAngleError_rad;
}

void wsf::six_dof::CommonController::SetAfterburnerEnabled(bool aEnabled)
{
   mCurrentLimitsAndSettings.enableAfterburnerAutoControl = aEnabled;
}

void wsf::six_dof::CommonController::SetAfterburnerThreshold(double aValue)
{
   mCurrentLimitsAndSettings.afterburnerThreshold = static_cast<float>(aValue);
}

void wsf::six_dof::CommonController::SetSpeedBrakeEnabled(bool aEnabled)
{
   mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl = aEnabled;
}

void wsf::six_dof::CommonController::SetSpeedBrakeThreshold(double aValue)
{
   mCurrentLimitsAndSettings.speedBrakeThreshold = static_cast<float>(aValue);
}

void wsf::six_dof::CommonController::SetCurrentTurnRollInMultiplier(double aValue)
{
   mCurrentLimitsAndSettings.turnRollInMultiplier = static_cast<float>(aValue);
}

void wsf::six_dof::CommonController::SetRouteAllowableAngleError_rad(double aValue_rad)
{
   mCurrentLimitsAndSettings.routeAllowableAngleError_rad = (float)aValue_rad;
}

bool wsf::six_dof::CommonController::GetEffectiveCLVsMachAlpha(double aMach, double aAlpha_rad, double& aCL) const
{
   if (mEffectiveCLVsMachAlphaTablePtr == nullptr)
   {
      return false;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   aCL           = mEffectiveCLVsMachAlphaTablePtr->Lookup(lookupArgs);
   return true;
}

bool wsf::six_dof::CommonController::GetAlphaVsMachCL(double aMach, double aCL, double& aAlpha_deg) const
{
   if (mAlphaVsMachCLTablePtr == nullptr)
   {
      return false;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aCL;
   aAlpha_deg    = mAlphaVsMachCLTablePtr->Lookup(lookupArgs);
   return true;
}

bool wsf::six_dof::CommonController::GetCLMaxMach(double aMach, double& aCLMax) const
{
   if (mCLMaxMachTablePtr == nullptr)
   {
      return false;
   }

   aCLMax = mCLMaxMachTablePtr->Lookup(aMach);
   return true;
}

bool wsf::six_dof::CommonController::GetCLMinMach(double aMach, double& aCLMin) const
{
   if (mCLMinMachTablePtr == nullptr)
   {
      return false;
   }

   aCLMin = mCLMinMachTablePtr->Lookup(aMach);
   return true;
}

bool wsf::six_dof::CommonController::GetAlphaMaxMach(double aMach, double& aAlphaMax_deg) const
{
   if (mAlphaMaxMachTablePtr == nullptr)
   {
      return false;
   }

   aAlphaMax_deg = mAlphaMaxMachTablePtr->Lookup(aMach);
   return true;
}

bool wsf::six_dof::CommonController::GetAlphaMinMach(double aMach, double& aAlphaMin_deg) const
{
   if (mAlphaMinMachTablePtr == nullptr)
   {
      return false;
   }

   aAlphaMin_deg = mAlphaMinMachTablePtr->Lookup(aMach);
   return true;
}
