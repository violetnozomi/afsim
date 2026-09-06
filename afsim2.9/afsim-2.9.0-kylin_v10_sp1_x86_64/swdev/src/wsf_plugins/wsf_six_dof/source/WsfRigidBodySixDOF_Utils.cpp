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

#include "WsfRigidBodySixDOF_Utils.hpp"

#include <fstream>
#include <sstream>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfRigidBodySixDOF_AeroCoreObject.hpp"
#include "WsfRigidBodySixDOF_CommonController.hpp"
#include "WsfRigidBodySixDOF_FlightControlSystem.hpp"
#include "WsfRigidBodySixDOF_LandingGear.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_PilotManager.hpp"
#include "WsfRigidBodySixDOF_PilotObject.hpp"
#include "WsfRigidBodySixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_Terrain.hpp"
#include "WsfSixDOF_TunerGAManager.hpp"
#include "WsfSixDOF_Utils.hpp"

// -------------------------------------------------------------------------------

// Helper function for writing Autopilot config files (defined at bottom of file)
void WritePIDBlock(std::ofstream&                  aStream,
                   const std::string&              aBlockName,
                   const std::string&              aBlockWhiteSpace,
                   wsf::six_dof::Pid::Type         aPid,
                   wsf::six_dof::CommonController* aPilot);

// -------------------------------------------------------------------------------

wsf::six_dof::RigidBodyMover* wsf::six_dof::utils::CastToRigidBody(Mover* aVehicle)
{
   return dynamic_cast<RigidBodyMover*>(aVehicle);
}

// -------------------------------------------------------------------------------

void wsf::six_dof::utils::Autotune(RigidBodyMover* aVehicle)
{
   TunerGASpecimen parentSpecimen;
   TunerGAManager  GA_Manager;

   std::vector<Pid::Type> vehiclePids = utils::GetRelevantPids(aVehicle);

   std::vector<TunerGASpecimen::PidLogElement> pidLog;
   double                                      runTime      = 20.0;
   double                                      platformTime = 0.0;

   for (auto& pidType : vehiclePids)
   {
      // If you're tuning altitude you need to run longer
      if (pidType == Pid::Altitude)
      {
         runTime = 60.0;
      }

      GA_Manager.InitializePopulation(parentSpecimen);

      while (true)
      {
         // Set PID gains for next run
         CommonController* autopilot = aVehicle->GetActivePilotObject()->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetPidGainKP(pidType, 0, static_cast<float>(GA_Manager.GetCurrentSpecimen().GetKp()));
            autopilot->SetPidGainKI(pidType, 0, static_cast<float>(GA_Manager.GetCurrentSpecimen().GetKi()));
            autopilot->SetPidGainKD(pidType, 0, static_cast<float>(GA_Manager.GetCurrentSpecimen().GetKd()));
         }

         // determine end time of next run
         double runEndTime = platformTime + runTime;

         // Preposition before each run
         if (pidType == Pid::TaxiHeading || pidType == Pid::TaxiSpeed || pidType == Pid::TaxiYawRate)
         {
            RigidBodyLandingGear* gear = aVehicle->GetLandingGear();
            if (gear != nullptr)
            {
               aVehicle->ForceLandingGearDownInstantly("Landing_Gear_Extended");
               double hgt_nominal_m = gear->GetNominalHeightAboveGroundOnGear();

               double startLat, startLon;
               aVehicle->GetStartingLatLon(startLat, startLon);
               double hgt_terrain_m = aVehicle->GetEnvironment()->GetTerrainElevation(startLat, startLon);

               double hgt_m = hgt_terrain_m + hgt_nominal_m;
               aVehicle->GetActivePilotObject()->SetPrePositionTAS(hgt_m * UtMath::cFT_PER_M, 0);
            }
         }
         else
         {
            aVehicle->GetActivePilotObject()->SetPrePositionTAS(10000, 100);
         }

         // Switch on the PID to determine what command needs to be sent and send it
         switch (pidType)
         {
         case Pid::Alpha:
            aVehicle->GetActivePilotObject()->SetAutopilotAlpha(2);
            break;

         case Pid::VerticalSpeed:
            aVehicle->GetActivePilotObject()->SetAutopilotVerticalSpeed(2000);
            break;

         case Pid::PitchAngle:
            aVehicle->GetActivePilotObject()->SetAutopilotPitchAngle(10);
            break;

         case Pid::PitchRate:
            aVehicle->GetActivePilotObject()->SetAutopilotPitchRate(10);
            break;

         case Pid::FlightPathAngle:
            aVehicle->GetActivePilotObject()->SetAutopilotFlightPathAngle(10);
            break;

         case Pid::DeltaPitch:
            aVehicle->GetActivePilotObject()->SetAutopilotDeltaPitch(10);
            break;

         case Pid::Altitude:
            aVehicle->GetActivePilotObject()->SetAutopilotAltitude(12000);
            break;

         case Pid::Beta:
            aVehicle->GetActivePilotObject()->SetAutopilotBeta(2);
            break;

         case Pid::YawRate:
            aVehicle->GetActivePilotObject()->SetAutopilotYawRate(10);
            break;

         case Pid::YawHeading:
            aVehicle->GetActivePilotObject()->SetAutopilotYawHeading(30);
            break;

         case Pid::TaxiHeading:
            break;

         case Pid::RollRate:
            aVehicle->GetActivePilotObject()->SetAutopilotRollRate(30);
            break;

         case Pid::DeltaRoll:
            aVehicle->GetActivePilotObject()->SetAutopilotDeltaRoll(30);
            break;

         case Pid::BankAngle:
            aVehicle->GetActivePilotObject()->SetAutopilotRollAngle(30);
            break;

         case Pid::RollHeading:
            aVehicle->GetActivePilotObject()->SetAutopilotRollHeading(30);
            break;

         case Pid::Speed:
            aVehicle->GetActivePilotObject()->SetAutopilotSpeedKTAS(200);
            break;

         case Pid::TaxiSpeed:
            break;

         case Pid::TaxiYawRate:
            break;

         default:
            // TODO
            break;
         }

         // Update the vehicle through the specified time
         for (platformTime = 0.0; platformTime <= runEndTime; platformTime += 0.01)
         {
            aVehicle->Update(platformTime);
            AutopilotPidGroupValueData pidGroup;
            aVehicle->GetActivePilotObject()->GetCommonController()->GetAutopilotPidValues(&pidGroup);

            TunerGASpecimen::PidLogElement temp;
            temp.pidValues = GetSinglePidData(pidType, pidGroup);
            temp.timeStamp = platformTime;
            pidLog.push_back(temp);
         }

         // Pass data to GA
         GA_Manager.Advance(pidLog);
         pidLog.clear();

         if (GA_Manager.IsComplete())
         {
            autopilot->SetPidGainKP(pidType, 0, static_cast<float>(GA_Manager.GetOptimum().GetKp()));
            autopilot->SetPidGainKI(pidType, 0, static_cast<float>(GA_Manager.GetOptimum().GetKi()));
            autopilot->SetPidGainKD(pidType, 0, static_cast<float>(GA_Manager.GetOptimum().GetKd()));
            break;
         }
      }
   }
}

void wsf::six_dof::utils::CreateAutopilotConfigFile(const RigidBodyMover* aVehicle, const std::string& aFilename)
{
   std::ofstream file(aFilename);
   if (file.is_open())
   {
      file << "# **************************************************************************************************"
           << std::endl;
      file << "# This file defines the configuration of the autopilot, including its PIDs. A PID is a proportional,"
           << std::endl;
      file << "# integral, derivative feedback control technique. Multiple PIDs are used to 'define' the autopilot."
           << std::endl;
      file << "# Each control output may be influenced by one or more PIDs." << std::endl;
      file << "# **************************************************************************************************"
           << std::endl;
      file << "# This file was generated by the WsfRigidBodySixDOF_Utils::CreateAutopilotConfigFile() function"
           << std::endl;
      file << "# **************************************************************************************************"
           << std::endl;
      file << std::endl;
      file << "autopilot_config" << std::endl;
      file << std::endl;
      file << "  vertical_middle_loop_rate_factor  4" << std::endl;
      file << "  vertical_outer_loop_rate_factor   4" << std::endl;
      file << "  lateral_middle_loop_rate_factor   4" << std::endl;
      file << "  lateral_outer_loop_rate_factor    4" << std::endl;
      file << "  speed_middle_loop_rate_factor     200" << std::endl;
      file << "  speed_outer_loop_rate_factor      4" << std::endl;
      file << std::endl;

      PilotObject* pilotObject = aVehicle->GetActivePilotObject();
      if (pilotObject != nullptr)
      {
         CommonController* controller = pilotObject->GetCommonController();
         if (controller != nullptr)
         {
            Control::Method controlMethod = controller->GetControlMethod();
            switch (controlMethod)
            {
            case Control::BankToTurnNoYaw:
               file << "  control_method                    BANK_TO_TURN_NO_YAW" << std::endl;
               break;
            case Control::BankToTurnWithYaw:
               file << "  control_method                    BANK_TO_TURN_WITH_YAW" << std::endl;
               break;
            case Control::YawToTurnNoRoll:
               file << "  control_method                    YAW_TO_TURN_NO_ROLL" << std::endl;
               break;
            case Control::YawToTurnRollRate:
               file << "  control_method                    YAW_TO_TURN_ROLL_RATE" << std::endl;
               break;
            case Control::YawToTurnZeroBank:
               file << "  control_method                    YAW_TO_TURN_ZERO_BANK" << std::endl;
               break;
            default:
               file << "  control_method                    YAW_TO_TURN_NO_ROLL" << std::endl;
            }
         }
         else
         {
            file << "  control_method                    YAW_TO_TURN_NO_ROLL" << std::endl;
         }
      }
      else
      {
         file << "  control_method                    YAW_TO_TURN_NO_ROLL" << std::endl;
      }

      file << std::endl;
      file << "  use_legacy_beta                   false" << std::endl;
      file << std::endl;
      file << "  min_taxi_turn_radius              50.0 ft   # This can be adjusted after testing" << std::endl;
      file << std::endl;
      file << "  use_simple_yaw_damper             false     # This can be set to true to damp yaw oscillations"
           << std::endl;
      file << std::endl;

      file << "  # ------------------------------------------------------------" << std::endl;
      file << std::endl;
      file << "  pid_group" << std::endl;
      file << std::endl;

      std::string whitespace = "    ";
      ;
      WritePIDBlock(file, "pid_alpha", whitespace, Pid::Alpha, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_altitude", whitespace, Pid::Altitude, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_bank_angle", whitespace, Pid::BankAngle, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_beta", whitespace, Pid::Beta, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_delta_pitch",
                    whitespace,
                    Pid::DeltaPitch,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_delta_roll", whitespace, Pid::DeltaRoll, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_flightpath_angle",
                    whitespace,
                    Pid::FlightPathAngle,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_forward_accel",
                    whitespace,
                    Pid::ForwardAccel,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_pitch_angle",
                    whitespace,
                    Pid::PitchAngle,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_pitch_rate", whitespace, Pid::PitchRate, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_roll_heading",
                    whitespace,
                    Pid::RollHeading,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_roll_rate", whitespace, Pid::RollRate, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_speed", whitespace, Pid::Speed, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_forward_accel",
                    whitespace,
                    Pid::TaxiForwardAccel,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_heading",
                    whitespace,
                    Pid::TaxiHeading,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_taxi_speed", whitespace, Pid::TaxiSpeed, aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_yaw_rate",
                    whitespace,
                    Pid::TaxiYawRate,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_vert_speed",
                    whitespace,
                    Pid::VerticalSpeed,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_yaw_heading",
                    whitespace,
                    Pid::YawHeading,
                    aVehicle->GetActivePilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_yaw_rate", whitespace, Pid::YawRate, aVehicle->GetActivePilotObject()->GetCommonController());

      AutopilotLimitsAndSettings limits =
         aVehicle->GetActivePilotObject()->GetCommonController()->GetCurrentLimitsAndSettings();
      file << std::endl;
      file << "    limits_and_settings" << std::endl;
      file << "      afterburner_threshold       " << limits.afterburnerThreshold << std::endl;
      file << "      speedbrake_threshold        " << limits.speedBrakeThreshold << std::endl;
      file << "      pitch_gload_min             " << limits.pitchGLoad_Min << std::endl;
      file << "      pitch_gload_max             " << limits.pitchGLoad_Max << std::endl;
      file << "      alpha_min                   " << limits.alpha_Min << std::endl;
      file << "      alpha_max                   " << limits.alpha_Max << std::endl;
      file << "      pitch_rate_min              " << limits.pitchRate_Min << std::endl;
      file << "      pitch_rate_max              " << limits.pitchRate_Max << std::endl;
      file << "      vert_speed_min              " << limits.vertSpd_Min << std::endl;
      file << "      vert_speed_max              " << limits.vertSpd_Max << std::endl;
      file << "      yaw_gload_max               " << limits.yawGLoad_Max << std::endl;
      file << "      beta_max                    " << limits.beta_Max << std::endl;
      file << "      yaw_rate_max                " << limits.yawRate_Max << std::endl;
      file << "      roll_rate_max               " << limits.rollRate_Max << std::endl;
      file << "      bank_angle_max              " << limits.bankAngle_Max << std::endl;
      file << "      forward_accel_min           " << limits.forwardAccel_Min << std::endl;
      file << "      forward_accel_max           " << limits.forwardAccel_Max << std::endl;
      file << "      taxi_speed_max_fps          " << limits.taxiSpeed_Max << std::endl;
      file << "      taxi_yaw_rate_max           " << limits.taxiYawRate_Max << std::endl;
      file << "      turn_roll_in_multiplier     " << limits.turnRollInMultiplier
           << "      # This can be manually 'tuned' for improved performance" << std::endl;
      file << "      route_allowable_angle_error " << limits.routeAllowableAngleError_rad * UtMath::cDEG_PER_RAD
           << " deg     # This can be manually 'tuned' for improved performance" << std::endl;
      file << "    end_limits_and_settings      " << std::endl;
      file << std::endl;
      file << "  end_pid_group" << std::endl;
      file << std::endl;
      file << "end_autopilot_config" << std::endl;
      file << std::endl;

      file.close();
   }
}

std::vector<wsf::six_dof::Pid::Type> wsf::six_dof::utils::GetRelevantPids(const RigidBodyMover* aVehicle)
{
   std::vector<Pid::Type> pidVector;

   // These pids are only relevant if the vehicle can actually land
   if (aVehicle->HasLandingGear())
   {
      pidVector.push_back(Pid::Type::TaxiYawRate);
      pidVector.push_back(Pid::Type::TaxiHeading);
      pidVector.push_back(Pid::Type::TaxiSpeed);
   }

   return pidVector;
}

wsf::six_dof::utils::CanFlyAtReturnValue wsf::six_dof::utils::CanFlyAt(RigidBodyMover* aObject,
                                                                       double          aAltitude_ft,
                                                                       double          aVelocity_fps,
                                                                       double          aThrottleForward,
                                                                       double          aFlapsDown)
{
   if (aAltitude_ft < 0)
   {
      auto out = ut::log::error() << "Negative altitude in CanFlyAt().";
      out.AddNote() << "Altitude: " << aAltitude_ft << " ft";
   }

   CanFlyAtReturnValue retVal = CAN_NOT_FLY;

   double minPitch  = -5;
   double maxPitch  = 20;
   double pitchStep = 0.02;

   for (double pitch = minPitch; pitch <= maxPitch; pitch += pitchStep)
   {
      aObject->TestingUpdate(aAltitude_ft, aVelocity_fps, pitch, 0.0, aThrottleForward, aFlapsDown);

      // Vertical Climb Rate is positive
      if (aObject->GetVertSpeed_fpm() >= 0)
      {
         // Check to see if Horizontal Acceleration is positive bool horizAccel =
         // IsHorizontalAccelerationPositive(aObject); For now, just assume it is heading north.
         if (aObject->GetAccelNED_mps2().X() >= 0)
         {
            // Check to see if we have Control Authority
            double stickBack = -1.0;
            aObject->TestingUpdate(aAltitude_ft, aVelocity_fps, pitch, stickBack, aThrottleForward, aFlapsDown);
            UtVec3dX omegaDotStickForward = aObject->GetKinematicState()->GetOmegaBodyDot();

            stickBack = 1.0;
            aObject->TestingUpdate(aAltitude_ft, aVelocity_fps, pitch, stickBack, aThrottleForward, aFlapsDown);
            UtVec3dX omegaDotStickBack = aObject->GetKinematicState()->GetOmegaBodyDot();

            if (omegaDotStickForward.Y() < 0.0 && omegaDotStickBack.Y() > 0.0)
            {
               return CAN_FLY;
            }

            retVal = CONTROL_FAIL;
         }
      }
   }
   return retVal;
}

// -------------------------------------------------------------------------------

void wsf::six_dof::utils::CreateAutopilotSupportFile(RigidBodyMover* aObject, const std::string& aFilename)
{
   RigidBodyTableTool tool(aObject);

   // Note that we do not call tool.InitializeTableMachValues() directly. Instead, it
   // is called within the RigidBodyTableTool constructor using default values.

   CommonCreateAutopilotSupportFile(tool, aFilename);
}

void wsf::six_dof::utils::CreateAutopilotSupportFileWithMaxMach(RigidBodyMover*    aObject,
                                                                const std::string& aFilename,
                                                                const double&      aMaxMach)
{
   RigidBodyTableTool tool(aObject);

   tool.InitializeTableMachValues(aMaxMach);

   CommonCreateAutopilotSupportFile(tool, aFilename);
}

void wsf::six_dof::utils::CommonCreateAutopilotSupportFile(RigidBodyTableTool& aTool, const std::string& aFilename)
{
   bool createdOk = aTool.CreateAutopilotTables();
   if (!createdOk)
   {
      ut::log::error()
         << "Unable to create autopilot tables in WsfRigidBodySixDOF_Utils::CommonCreateAutopilotSupportFile().";
      return;
   }

   // Note: Do not remove this PROGRESS flag, since it is used in Mover Creator
   // to parse console data to detect progress.
   ut::log::info() << "<<PROGRESS '1000'>>";

   // All output data is routed through this string
   std::string outputString;

   // Output the CLMax and CLmin tables to the output string
   aTool.OutputCLMaxMachTableToInputString(outputString);
   aTool.OutputCLMinMachTableToInputString(outputString);

   // Output the AlphaMax and AlphaMin tables to the output string
   aTool.OutputAlphaMaxMachTableToInputString(outputString);
   aTool.OutputAlphaMinMachTableToInputString(outputString);

   // Output the AlphaMachCL table to the output string
   aTool.OutputAlphaMachCLTableToInputString(outputString);

   // Output the StickMachAlpha table to the output string
   aTool.OutputStickMachAlphaTablesToInputString(outputString);

   // Output the CLMachAlpha table to the output string
   aTool.OutputCLMachAlphaTableToInputString(outputString);

   // Output the data (string) to file
   std::ofstream file(aFilename.c_str());
   if (file.is_open())
   {
      file << "autopilot_support_tables\n\n";
      file << outputString;
      file << "end_autopilot_support_tables\n\n";
      file.close();
   }
   else
   {
      ut::log::error() << "Unable to open autopilot_support_tables file in "
                          "WsfRigidBodySixDOF_Utils::CommonCreateAutopilotSupportFile().";
   }
}

// -------------------------------------------------------------------------------

wsf::six_dof::RigidBodyMachParamDataTable::RigidBodyMachParamDataTable()
   : mCurrentMachParamDataPairPtr(nullptr)
{
}

wsf::six_dof::RigidBodyMachParamDataTable::~RigidBodyMachParamDataTable()
{
   for (auto& machIter : mMachParamDataList)
   {
      std::vector<paramDataPair*>& pairList = machIter->paramDataList;

      for (auto& alphaIter : pairList)
      {
         delete alphaIter;
      }
      pairList.clear();

      delete machIter;
   }
   mMachParamDataList.clear();
}

wsf::six_dof::RigidBodyMachParamDataTable::RigidBodyMachParamDataTable(const RigidBodyMachParamDataTable& aSrc)
{
   for (auto& machIter : aSrc.mMachParamDataList)
   {
      CreateNewMach(machIter->mach);
      for (auto& pair : machIter->paramDataList)
      {
         AddNewParamDataPair(pair->parameter, pair->value);
      }
   }
}

wsf::six_dof::RigidBodyMachParamDataTable* wsf::six_dof::RigidBodyMachParamDataTable::Clone() const
{
   return new RigidBodyMachParamDataTable(*this);
}

void wsf::six_dof::RigidBodyMachParamDataTable::CreateNewMach(double aMach)
{
   machParamDataListPair* machPair = new machParamDataListPair;
   machPair->mach                  = aMach;
   mMachParamDataList.push_back(machPair);

   mCurrentMachParamDataPairPtr = machPair;
}

void wsf::six_dof::RigidBodyMachParamDataTable::AddNewParamDataPair(double aParameter, double aData)
{
   paramDataPair* alphaValue = new paramDataPair;
   alphaValue->parameter     = aParameter;
   alphaValue->value         = aData;

   mCurrentMachParamDataPairPtr->paramDataList.push_back(alphaValue);
}

// Note: this assumes mMachParamDataList, is sorted in ascended order
double wsf::six_dof::RigidBodyMachParamDataTable::GetValueAtMachParam(double aMach, double aParameter) const
{
   if (!mMachParamDataList.empty())
   {
      // if aMach is less than any pair in machParamDataListPair, return the first Mach's value
      if (aMach <= (*mMachParamDataList.begin())->mach)
      {
         return GetValueAtParam((*mMachParamDataList.begin())->paramDataList, aParameter);
      }

      machParamDataListPair* previousPair = *mMachParamDataList.begin();
      for (auto& iter : mMachParamDataList)
      {
         if (aMach <= iter->mach)
         {
            // Linearly extrapolate between previous value and current value;
            double previousValue = GetValueAtParam(previousPair->paramDataList, aParameter);
            double currentValue  = GetValueAtParam(iter->paramDataList, aParameter);

            double factor = (aMach - previousPair->mach) / (iter->mach - previousPair->mach);
            return (currentValue - previousValue) * factor + previousValue;
         }
         previousPair = iter;
      }

      // aMach is greater than any pair in machParamDataListPair, return last Mach's value
      return GetValueAtParam((*mMachParamDataList.rbegin())->paramDataList, aParameter);
   }
   return 0.0;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMinParameterAtMach(double aMach) const
{
   return GetParameterAtMach(aMach, true);
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMaxParameterAtMach(double aMach) const
{
   return GetParameterAtMach(aMach, false);
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMinDataAtMach(double aMach) const
{
   return GetDataAtMach(aMach, true);
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMaxDataAtMach(double aMach) const
{
   return GetDataAtMach(aMach, false);
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetParameterAtMach(double aMach, bool aGetMin) const
{
   if (mMachParamDataList.size() == 0)
   {
      return 0.0;
   }
   else if (mMachParamDataList.size() == 1)
   {
      return GetMinParameter(mMachParamDataList.at(0));
   }
   else if (mMachParamDataList.size() == 2)
   {
      double machLo = mMachParamDataList.at(0)->mach;
      double machHi = mMachParamDataList.at(1)->mach;

      double paramLo = 0;
      double paramHi = 0;

      if (aGetMin)
      {
         paramLo = GetMinParameter(mMachParamDataList.at(0));
         paramHi = GetMinParameter(mMachParamDataList.at(1));
      }
      else
      {
         paramLo = GetMaxParameter(mMachParamDataList.at(0));
         paramHi = GetMaxParameter(mMachParamDataList.at(1));
      }

      if (aMach <= machLo)
      {
         return paramLo;
      }
      else if (aMach >= machHi)
      {
         return paramHi;
      }
      else
      {
         double machDelta = machHi - machLo;
         double result    = 0;
         if (machDelta != 0)
         {
            double fraction = (aMach - machLo) / machDelta;
            result          = paramLo + fraction * (paramHi - paramLo);
         }
         else
         {
            // Use mid-point
            result = paramLo + 0.5 * (paramHi - paramLo);
         }

         return result;
      }
   }
   else
   {
      size_t num    = mMachParamDataList.size();
      double machLo = mMachParamDataList.at(0)->mach;
      double machHi = mMachParamDataList.at(num - 1)->mach;

      double paramLo = 0;
      double paramHi = 0;

      if (aGetMin)
      {
         paramLo = GetMinParameter(mMachParamDataList.at(0));
         paramHi = GetMinParameter(mMachParamDataList.at(num - 1));
      }
      else
      {
         paramLo = GetMaxParameter(mMachParamDataList.at(0));
         paramHi = GetMaxParameter(mMachParamDataList.at(num - 1));
      }

      if (aMach <= machLo)
      {
         return paramLo;
      }
      else if (aMach >= machHi)
      {
         return paramHi;
      }
      else
      {
         // Start with the first entry
         std::vector<machParamDataListPair*>::const_iterator iter = mMachParamDataList.begin();

         machLo = (*iter)->mach;

         if (aGetMin)
         {
            paramLo = GetMinParameter((*iter));
         }
         else
         {
            paramLo = GetMaxParameter((*iter));
         }

         // Increment once to have two bounded points
         ++iter;

         machHi = (*iter)->mach;

         if (aGetMin)
         {
            paramHi = GetMinParameter((*iter));
         }
         else
         {
            paramHi = GetMaxParameter((*iter));
         }

         // Now, loop until we get a "bracket" or hit the end of the list
         while (iter != mMachParamDataList.end())
         {
            if ((aMach >= machLo) && (aMach <= machHi))
            {
               // We have a bracket

               double machDelta = machHi - machLo;
               double result    = 0;
               if (machDelta != 0)
               {
                  double fraction = (aMach - machLo) / machDelta;
                  result          = paramLo + fraction * (paramHi - paramLo);
               }
               else
               {
                  // Use mid-point
                  result = paramLo + 0.5 * (paramHi - paramLo);
               }

               return result;
            }

            // Increment

            machLo = (*iter)->mach;

            if (aGetMin)
            {
               paramLo = GetMinParameter((*iter));
            }
            else
            {
               paramLo = GetMaxParameter((*iter));
            }

            ++iter;

            if (iter != mMachParamDataList.end())
            {
               machHi = (*iter)->mach;
               if (aGetMin)
               {
                  paramHi = GetMinParameter((*iter));
               }
               else
               {
                  paramHi = GetMaxParameter((*iter));
               }
            }
            else
            {
               // Done
            }
         }

         // We're at the end of the list and did not get result!!!
         ut::log::error() << "Unable to get a valid result in RigidBodyMachParamDataTable::GetMinParameterAtMach().";
      }
   }

   return 0.0;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMinParameter(const machParamDataListPair* aMachParamDataListPair) const
{
   if (aMachParamDataListPair->paramDataList.size() == 0)
   {
      return 0.0;
   }

   std::vector<paramDataPair*>::const_iterator iter = aMachParamDataListPair->paramDataList.begin();

   double minParameter = (*iter)->parameter;

   while (iter != aMachParamDataListPair->paramDataList.end())
   {
      if ((*iter)->parameter < minParameter)
      {
         minParameter = (*iter)->parameter;
      }

      ++iter;
   }

   return minParameter;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMaxParameter(const machParamDataListPair* aMachParamDataListPair) const
{
   if (aMachParamDataListPair->paramDataList.size() == 0)
   {
      return 0.0;
   }

   std::vector<paramDataPair*>::const_iterator iter = aMachParamDataListPair->paramDataList.begin();

   double maxParameter = (*iter)->parameter;

   while (iter != aMachParamDataListPair->paramDataList.end())
   {
      if ((*iter)->parameter > maxParameter)
      {
         maxParameter = (*iter)->parameter;
      }

      ++iter;
   }

   return maxParameter;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetDataAtMach(double aMach, bool aGetMin) const
{
   if (mMachParamDataList.size() == 0)
   {
      return 0.0;
   }
   else if (mMachParamDataList.size() == 1)
   {
      return GetMinData(mMachParamDataList.at(0));
   }
   else if (mMachParamDataList.size() == 2)
   {
      double machLo = mMachParamDataList.at(0)->mach;
      double machHi = mMachParamDataList.at(1)->mach;

      double dataLo = 0;
      double dataHi = 0;

      if (aGetMin)
      {
         dataLo = GetMinData(mMachParamDataList.at(0));
         dataHi = GetMinData(mMachParamDataList.at(1));
      }
      else
      {
         dataLo = GetMaxData(mMachParamDataList.at(0));
         dataHi = GetMaxData(mMachParamDataList.at(1));
      }

      if (aMach <= machLo)
      {
         return dataLo;
      }
      else if (aMach >= machHi)
      {
         return dataHi;
      }
      else
      {
         double machDelta = machHi - machLo;
         double result    = 0;
         if (machDelta != 0)
         {
            double fraction = (aMach - machLo) / machDelta;
            result          = dataLo + fraction * (dataHi - dataLo);
         }
         else
         {
            // Use mid-point
            result = dataLo + 0.5 * (dataHi - dataLo);
         }

         return result;
      }
   }
   else
   {
      size_t num    = mMachParamDataList.size();
      double machLo = mMachParamDataList.at(0)->mach;
      double machHi = mMachParamDataList.at(num - 1)->mach;

      double dataLo = 0;
      double dataHi = 0;

      if (aGetMin)
      {
         dataLo = GetMinData(mMachParamDataList.at(0));
         dataHi = GetMinData(mMachParamDataList.at(num - 1));
      }
      else
      {
         dataLo = GetMaxData(mMachParamDataList.at(0));
         dataHi = GetMaxData(mMachParamDataList.at(num - 1));
      }

      if (aMach <= machLo)
      {
         return dataLo;
      }
      else if (aMach >= machHi)
      {
         return dataHi;
      }
      else
      {
         // Start with the first entry
         std::vector<machParamDataListPair*>::const_iterator iter = mMachParamDataList.begin();

         machLo = (*iter)->mach;

         if (aGetMin)
         {
            dataLo = GetMinData((*iter));
         }
         else
         {
            dataLo = GetMaxData((*iter));
         }

         // Increment once to have two bounded points
         ++iter;

         machHi = (*iter)->mach;

         if (aGetMin)
         {
            dataHi = GetMinData((*iter));
         }
         else
         {
            dataHi = GetMaxData((*iter));
         }

         // Now, loop until we get a "bracket" or hit the end of the list
         while (iter != mMachParamDataList.end())
         {
            if ((aMach >= machLo) && (aMach <= machHi))
            {
               // We have a bracket

               double machDelta = machHi - machLo;
               double result    = 0;
               if (machDelta != 0)
               {
                  double fraction = (aMach - machLo) / machDelta;
                  result          = dataLo + fraction * (dataHi - dataLo);
               }
               else
               {
                  // Use mid-point
                  result = dataLo + 0.5 * (dataHi - dataLo);
               }

               return result;
            }

            // Increment

            machLo = (*iter)->mach;

            if (aGetMin)
            {
               dataLo = GetMinData((*iter));
            }
            else
            {
               dataLo = GetMaxData((*iter));
            }

            ++iter;

            if (iter != mMachParamDataList.end())
            {
               machHi = (*iter)->mach;
               if (aGetMin)
               {
                  dataHi = GetMinData((*iter));
               }
               else
               {
                  dataHi = GetMaxData((*iter));
               }
            }
            else
            {
               // Done
            }
         }

         // We're at the end of the list and did not get result!!!
         ut::log::error() << "Unable to get a valid result in RigidBodyMachParamDataTable::GetMinParameterAtMach().";
      }
   }

   return 0.0;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMinData(const machParamDataListPair* aMachParamDataListPair) const
{
   if (aMachParamDataListPair->paramDataList.size() == 0)
   {
      return 0.0;
   }

   std::vector<paramDataPair*>::const_iterator iter = aMachParamDataListPair->paramDataList.begin();

   double minData = (*iter)->value;

   while (iter != aMachParamDataListPair->paramDataList.end())
   {
      if ((*iter)->value < minData)
      {
         minData = (*iter)->value;
      }

      ++iter;
   }

   return minData;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetMaxData(const machParamDataListPair* aMachParamDataListPair) const
{
   if (aMachParamDataListPair->paramDataList.size() == 0)
   {
      return 0.0;
   }

   std::vector<paramDataPair*>::const_iterator iter = aMachParamDataListPair->paramDataList.begin();

   double maxData = (*iter)->value;

   while (iter != aMachParamDataListPair->paramDataList.end())
   {
      if ((*iter)->value > maxData)
      {
         maxData = (*iter)->value;
      }

      ++iter;
   }

   return maxData;
}

double wsf::six_dof::RigidBodyMachParamDataTable::GetValueAtParam(std::vector<paramDataPair*>& aList, double aParameter) const
{
   if (!aList.empty())
   {
      // if aParameter is less than any pair in aList, return the first parameter's value
      if (aParameter <= (*aList.begin())->parameter)
      {
         return (*aList.begin())->value;
      }

      paramDataPair* previousPair = *aList.begin();
      for (std::vector<paramDataPair*>::const_iterator iter = aList.begin(); iter != aList.end(); ++iter)
      {
         if (aParameter <= (*iter)->parameter)
         {
            double factor = (aParameter - previousPair->parameter) / ((*iter)->parameter - previousPair->parameter);
            return ((*iter)->value - previousPair->value) * factor + previousPair->value;
         }
         previousPair = *iter;
      }

      // aParameter is greater than any pair in aList, return last parameter's value
      return (*aList.rbegin())->value;
   }
   return 0.0;
}

// aParameterType typically is "alpha" or "cl"
// aParameterUnitsString typically is "units deg" or "precision float"
void wsf::six_dof::RigidBodyMachParamDataTable::OutputToString(std::string&       aInput,
                                                               const std::string& aTableName,
                                                               const std::string& aParameterType,
                                                               const std::string& aParameterUnitsString) const
{
   aInput = aInput + "   " + aTableName + "\n";
   aInput = aInput + "      irregular_table\n";
   aInput = aInput + "         independent_variable mach precision float\n";
   aInput = aInput + "         independent_variable " + aParameterType + " " + aParameterUnitsString + "\n";
   aInput = aInput + "         dependent_variable precision float\n";

   for (auto& iter : mMachParamDataList)
   {
      double Mach = iter->mach;
      aInput      = aInput + "         mach  " + utils::OutputDoubleString(Mach, 5, 2) + "\n";

      std::stringstream parameterNameStr;
      parameterNameStr.width(8);
      parameterNameStr.setf(std::ios::left);
      parameterNameStr << aParameterType;

      //    aInput = aInput + "            alpha  ";
      //    aInput = aInput + "            " + aParameterType + "  ";
      aInput = aInput + "            " + parameterNameStr.str() + "  ";

      for (auto& paramData : iter->paramDataList)
      {
         aInput = aInput + "   " + utils::OutputDoubleString(paramData->parameter, 8, 3);
      }
      aInput = aInput + "\n";

      aInput = aInput + "            values    ";
      for (auto& paramData : iter->paramDataList)
      {
         aInput = aInput + "   " + utils::OutputDoubleString(paramData->value, 8, 3);
      }
      aInput = aInput + "\n";
   }

   aInput = aInput + "      end_irregular_table\n";
   aInput = aInput + "   end_" + aTableName + "\n\n";
}

// -------------------------------------------------------------------------------

wsf::six_dof::RigidBodyMachDataTable::RigidBodyMachDataTable() {}

wsf::six_dof::RigidBodyMachDataTable::~RigidBodyMachDataTable()
{
   for (auto& machIter : mMachDataList)
   {
      delete machIter;
   }
   mMachDataList.clear();
}

wsf::six_dof::RigidBodyMachDataTable::RigidBodyMachDataTable(const RigidBodyMachDataTable& aSrc)
{
   for (auto& machIter : aSrc.mMachDataList)
   {
      CreateNewMachValuePair(machIter->mach, machIter->value);
   }
}

wsf::six_dof::RigidBodyMachDataTable* wsf::six_dof::RigidBodyMachDataTable::Clone() const
{
   return new RigidBodyMachDataTable(*this);
}

void wsf::six_dof::RigidBodyMachDataTable::CreateNewMachValuePair(double aMach, double aValue)
{
   machDataPair* machPair = new machDataPair;
   machPair->mach         = aMach;
   machPair->value        = aValue;
   mMachDataList.push_back(machPair);
}

double wsf::six_dof::RigidBodyMachDataTable::GetValueAtMach(double aMach) const
{
   if (!mMachDataList.empty())
   {
      // if aParameter is less than any pair in aList, return the first parameter's value
      if (aMach <= (*mMachDataList.begin())->mach)
      {
         return (*mMachDataList.begin())->value;
      }

      machDataPair* previousPair = *mMachDataList.begin();
      for (auto& iter : mMachDataList)
      {
         if (aMach <= iter->mach)
         {
            double factor = (aMach - previousPair->mach) / (iter->mach - previousPair->mach);
            return (iter->value - previousPair->value) * factor + previousPair->value;
         }
         previousPair = iter;
      }

      // aParameter is greater than any pair in aList, return last parameter's value
      return (*mMachDataList.rbegin())->value;
   }
   return 0.0;
}

void wsf::six_dof::RigidBodyMachDataTable::OutputToString(std::string& aInput, const std::string& aTableName) const
{
   aInput = aInput + "   " + aTableName + "\n";

   for (auto& iter : mMachDataList)
   {
      aInput = aInput + "      " + utils::OutputDoubleString(iter->mach, 7, 3) + "      " +
               utils::OutputDoubleString(iter->value, 8, 3) + "\n";
   }

   aInput = aInput + "   end_" + aTableName + "\n\n";
}

// -------------------------------------------------------------------------------

wsf::six_dof::RigidBodyTableTool::RigidBodyTableTool(RigidBodyMover* aObject)
   : mObject(aObject)
   , mCLMaxVsMachTable(nullptr)
   , mCLMinVsMachTable(nullptr)
   , mAlphaMax_degVsMachTable(nullptr)
   , mAlphaMin_degVsMachTable(nullptr)
   , mStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mCLVsMachAlphaTable(nullptr)
   , mAlphaVsMachCLTable(nullptr)
   , mTableListsInitialized(false)
{
   InitializeTableMachValues(aObject->GetNominalEstimateMaxMach());
}

bool wsf::six_dof::RigidBodyTableTool::CalculateStickForZeroMoment(double aMach, double aAlpha_rad, double& aStickBack)
{
   double tempThrustOut_lbs = 0.0;
   return CalculateStickForZeroMoment(aMach, aAlpha_rad, false, tempThrustOut_lbs, aStickBack);
}

bool wsf::six_dof::RigidBodyTableTool::CalculateStickForZeroMoment(double  aMach,
                                                                   double  aAlpha_rad,
                                                                   bool    aIsPowerOn,
                                                                   double& aThrustProduced_lbs,
                                                                   double& aStickBack)
{
   if ((mObject->GetFlightControls() == nullptr) || (mObject->GetPilotManager() == nullptr) ||
       (mObject->GetPilotManager()->GetActivePilot() == nullptr))
   {
      aStickBack = 0;
      return false;
   }

   double coreCm        = 0;
   double controlsCm    = 0;
   double Cm            = 0;
   double lastStickBack = 0;
   double lastCm        = 0;

   std::deque<double> preservedFuelLevels;
   double             preservedThrottleInput             = 0.0;
   bool               preservedTestControlSetting        = false;
   bool               preservedMasterNoLagTestingSetting = false;

   RigidBodyPropulsionSystem* propulsion = mObject->GetRigidBodyPropulsionSystem();
   PilotObject*               pilot      = mObject->GetActivePilotObject();

   if (propulsion)
   {
      FreezeFlags* freezeFlags = mObject->GetFreezeFlags();
      if (freezeFlags)
      {
         preservedMasterNoLagTestingSetting = freezeFlags->testingNoLag;
         freezeFlags->testingNoLag          = true;
      }

      preservedFuelLevels.resize(propulsion->GetNumFuelTanks());
      for (auto& tank : propulsion->GetFuelTanks())
      {
         preservedFuelLevels.push_back(tank.second->GetCurrentFuelQuantity_lbs());
      }

      if (pilot)
      {
         preservedThrottleInput      = pilot->GetThrottleControllerPosition();
         preservedTestControlSetting = pilot->UsingTestControl();

         pilot->TakeTestControl();

         if (aIsPowerOn)
         {
            pilot->SetTestThrottleControllerPosition(1.0);
         }
         else
         {
            pilot->SetTestThrottleControllerPosition(0.0);
         }
      }

      // Don't empty the tanks entirely -- if we do,
      // no thrust will be available and the output
      // for the power-on case will be incorrect
      propulsion->FillAllTanks(5.0);
   }

   mObject->CalculateCurrentMassProperties();

   // Start with neutral stick
   aStickBack = 0;

   mObject->CalculateCmAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, aIsPowerOn, aThrustProduced_lbs, coreCm, controlsCm);
   Cm = coreCm + controlsCm;

   double stickMostForward = 0;
   double stickMostAft     = 0;
   bool   needNoseUp       = false;

   lastStickBack = aStickBack;
   lastCm        = Cm;

   if (Cm >= 0)
   {
      // Vehicle is pitching nose up, we need nose down, so push stick forward
      stickMostForward = aStickBack - 0.01;
      stickMostAft     = aStickBack;
      needNoseUp       = false;
      aStickBack       = stickMostForward;
   }
   else
   {
      // Vehicle is pitching nose down, we need nose up, so pull stick back
      stickMostForward = aStickBack;
      stickMostAft     = aStickBack + 0.01;
      needNoseUp       = true;
      aStickBack       = stickMostAft;
   }

   bool done    = false;
   bool success = false;
   while (!done)
   {
      mObject->CalculateCmAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, aIsPowerOn, aThrustProduced_lbs, coreCm, controlsCm);
      Cm = coreCm + controlsCm;

      if (Cm >= 0)
      {
         if (needNoseUp)
         {
            // We have our other point
            done    = true;
            success = true;
         }
         else
         {
            // We have nose up, but need nose down, so push stick forward more

            if (Cm > lastCm)
            {
               // Nose up tendency is increasing -- stop, since its getting worse
               done = true;
            }

            lastStickBack    = aStickBack;
            lastCm           = Cm;
            stickMostForward = aStickBack - 0.01;
            stickMostAft     = aStickBack;
            aStickBack       = stickMostForward;
         }
      }
      else
      {
         if (!needNoseUp)
         {
            // We have our other point
            done    = true;
            success = true;
         }
         else
         {
            // We have nose down, but need nose up, so pull stick back more
            if (Cm < lastCm)
            {
               // Nose down tendency is increasing -- stop, since its getting worse
               done = true;
            }

            lastStickBack    = aStickBack;
            lastCm           = Cm;
            stickMostForward = aStickBack;
            stickMostAft     = aStickBack + 0.01;
            aStickBack       = stickMostAft;
         }
      }

      if (aStickBack > 1)
      {
         aStickBack = 1;
         done       = true;
      }
      else if (aStickBack < -1)
      {
         aStickBack = -1;
         done       = true;
      }
   }

   if (success)
   {
      double lowCm     = 0;
      double highCm    = 0;
      double lowStick  = 0;
      double highStick = 0;

      if (lastCm < Cm)
      {
         lowCm     = lastCm;
         highCm    = Cm;
         lowStick  = lastStickBack;
         highStick = aStickBack;
      }
      else
      {
         lowCm     = Cm;
         highCm    = lastCm;
         lowStick  = aStickBack;
         highStick = lastStickBack;
      }

      double deltaCm    = highCm - lowCm;
      double deltaStick = highStick - lowStick;

      if (deltaCm != 0.0)
      {
         double fraction = -lowCm / deltaCm;
         aStickBack      = lowStick + deltaStick * fraction;
      }
      else
      {
         // Use mid-point
         aStickBack = lowStick + deltaStick * 0.5;
      }
   }

   // Limit stick value
   if (aStickBack < -1.0)
   {
      aStickBack = -1.0;
   }
   else if (aStickBack > 1.0)
   {
      aStickBack = 1.0;
   }

   // Restore fuel levels and pilot settings
   if (propulsion)
   {
      FreezeFlags* freezeFlags = mObject->GetFreezeFlags();
      if (freezeFlags)
      {
         freezeFlags->testingNoLag = preservedMasterNoLagTestingSetting;
      }

      if (pilot)
      {
         pilot->SetTestThrottleControllerPosition(preservedThrottleInput);

         if (!preservedTestControlSetting)
         {
            pilot->ReleaseTestControl();
         }
      }

      for (auto& tank : propulsion->GetFuelTanks())
      {
         tank.second->SetCurrentFuelQuantity(preservedFuelLevels.front());
         preservedFuelLevels.pop_front();
      }
   }

   return success;
}

double wsf::six_dof::RigidBodyTableTool::CalcCLFromCLArea(double aCLArea)
{
   double area_sqft = 0.0;
   if (mObject->GetActiveAeroObject()->UsesRefArea())
   {
      area_sqft = mObject->GetActiveAeroObject()->GetRefArea_sqft();
   }
   else
   {
      area_sqft = mObject->GetActiveAeroObject()->GetWingArea_sqft();
   }

   if (area_sqft > 0.0)
   {
      return (aCLArea / area_sqft);
   }

   return 0.0;
}


bool wsf::six_dof::RigidBodyTableTool::CreateAutopilotTables()
{
   // Note: Do not remove this BEGIN flag, since it is used in Mover Creator
   // to parse console data to detect progress.
   ut::log::info() << "<<BEGIN>>";

   Initialize();

   // ........................................................................................................

   bool controllable = true;

   // First group: CLMaxVsMach and AlphaMaxVsMach
   controllable &= CreateCLMaxAndAlphaMaxTables();

   // Next group: CLmin and AlphaMin tables
   controllable &= CreateCLMinAndAlphaMinTables();

   // Next group: StickForZeroMomentVsMach and CLVsMachAlpha
   controllable &= CreateStickForZeroMomentTable();

   // Next group: AlphaVsMachCL
   controllable &= CreateAlphaVsMachCLTable();

   // Note: At some point, we may want to add support for flaps. If the aircraft has flaps,
   // we should generate/repeat the above process for multiple flaps settings. A flag
   // will indicate if the additional flaps table should be used. If so, we will merely
   // interpolate using the flaps setting.

   return controllable;
}

void wsf::six_dof::RigidBodyTableTool::AddAlphaVsMachCL(double aMach,
                                                        double aCL,
                                                        double aMinAlpha_deg,
                                                        double aMaxAlpha_deg,
                                                        bool&  aMachAdded)
{
   // Loop through CL while using a binary-search varying alpha with CL-Mach-Alpha
   // to match the desired CL. Once this is achieved, use the "searched" alpha and
   // current CL and mach to create Alpha-Mach-CL.

   double alpha_deg     = 0.0;
   double highAlpha_deg = 0.0;
   double lowAlpha_deg  = 0.0;

   double tableCL = mCLVsMachAlphaTable->GetValueAtMachParam(aMach, alpha_deg);

   // LoopToFindAlphaForTargetCL();

   // First time
   if (tableCL > aCL)
   {
      // Need to reduce alpha
      lowAlpha_deg  = aMinAlpha_deg;
      highAlpha_deg = alpha_deg;
      alpha_deg     = (lowAlpha_deg + highAlpha_deg) * 0.5;
   }
   else
   {
      // Need to increase alpha
      highAlpha_deg = aMaxAlpha_deg;
      lowAlpha_deg  = alpha_deg;
      alpha_deg     = (lowAlpha_deg + highAlpha_deg) * 0.5;
   }

   // This is the number of iterations to use to converge on a solution. It is a compromise
   // between accuracy and processing time. Based on experimentation, 20 iterations works
   // well for most cases. An improved technique could be used in the future.
   const int numIterations = 20;

   // Loop for a specified number of iterations to determine alpha_deg
   int count = 0;
   while (count < numIterations)
   {
      ++count;

      tableCL = mCLVsMachAlphaTable->GetValueAtMachParam(aMach, alpha_deg);

      if (tableCL > aCL)
      {
         // Need to reduce alpha
         // lowAlpha_deg = minAlpha_deg;
         highAlpha_deg = alpha_deg;
         alpha_deg     = (lowAlpha_deg + highAlpha_deg) * 0.5;
      }
      else
      {
         // Need to increase alpha
         // highAlpha_deg = maxAlpha_deg;
         lowAlpha_deg = alpha_deg;
         alpha_deg    = (lowAlpha_deg + highAlpha_deg) * 0.5;
      }
   }

   {
      // RAII block
      auto out = ut::log::info() << "Added Alpha-Mach CL.";
      out.AddNote() << "Target CL: " << aCL;
      out.AddNote() << "Table CL: " << tableCL;
   }

   // We need to get tableCL close enough to CL -- once we do we're done with this part

   if (!aMachAdded)
   {
      mAlphaVsMachCLTable->CreateNewMach(aMach);
      aMachAdded = true;
   }

   mAlphaVsMachCLTable->AddNewParamDataPair(aCL, alpha_deg);
}

bool wsf::six_dof::RigidBodyTableTool::CalcStickAndCL(double  aMach,
                                                      double  aAlpha_rad,
                                                      double& aStickBackBaseline,
                                                      double& aStickBackThrustShift,
                                                      double& aStickBackXCmShift,
                                                      double& aStickBackZCmShift)
{
   // Setup pitch using alpha...
   mObject->SetAttitudeNED_rad(0.0, aAlpha_rad, 0.0);

   // We may want to consider the use of "flags" to include whether subobjects and/or
   // fuel should be used in mass and aero calculations

   // Vary stick trying to zero pitching moment
   double thrustProduced_lbs = 0.0;

   bool valid = CalculateStickForZeroMoment(aMach, aAlpha_rad, aStickBackBaseline);
   valid |= CalculateStickForZeroMoment(aMach, aAlpha_rad, true, thrustProduced_lbs, aStickBackThrustShift);

   UtVec3dX xCmShift_ft(0.001, 0.0, 0.0);
   UtVec3dX zCmShift_ft(0.0, 0.0, 0.001);

   mObject->ShiftCurrentCg_ft(xCmShift_ft);
   valid |= CalculateStickForZeroMoment(aMach, aAlpha_rad, aStickBackXCmShift);
   mObject->ShiftCurrentCg_ft(-xCmShift_ft);

   mObject->ShiftCurrentCg_ft(zCmShift_ft);
   valid |= CalculateStickForZeroMoment(aMach, aAlpha_rad, aStickBackZCmShift);
   mObject->ShiftCurrentCg_ft(-zCmShift_ft);

   RigidBodyMover* vehicle = mObject;
   if (vehicle && thrustProduced_lbs > 1.0)
   {
      MassProperties massProperties    = vehicle->GetMassProperties();
      double         referenceMass_lbs = massProperties.GetBaseMass_lbs();

      aStickBackThrustShift = (aStickBackThrustShift - aStickBackBaseline) * referenceMass_lbs / thrustProduced_lbs;
   }
   else
   {
      aStickBackThrustShift = 0.0;
   }
   aStickBackXCmShift = (aStickBackXCmShift - aStickBackBaseline) / xCmShift_ft.Magnitude();
   aStickBackZCmShift = (aStickBackZCmShift - aStickBackBaseline) / zCmShift_ft.Magnitude();

   return valid;
}

int wsf::six_dof::RigidBodyTableTool::CountValidStickAndCL(double aMach, std::vector<double> aAlphaVector_deg)
{
   double stickBackBaseline    = 0.0;
   double stickBackThrustShift = 0.0;
   double stickBackXcgShift    = 0.0;
   double stickBackZcgShift    = 0.0;

   int validAlphaPoints = 0;

   for (auto& alpha_deg : aAlphaVector_deg)
   {
      double alpha_rad = alpha_deg * UtMath::cRAD_PER_DEG;

      if (CalcStickAndCL(aMach, alpha_rad, stickBackBaseline, stickBackThrustShift, stickBackXcgShift, stickBackZcgShift))
      {
         validAlphaPoints++;
      }
   }

   return validAlphaPoints;
}

void wsf::six_dof::RigidBodyTableTool::AddStickAndCL(double  aMach,
                                                     double  aAlpha_rad,
                                                     double& aStickBack,
                                                     bool&   aMachAdded,
                                                     bool /*aLowPoint*/,
                                                     bool /*aHighPoint*/)
{
   // We may want to consider the use of "flags" to include whether subobjects and/or
   // fuel should be used in mass and aero calculations

   // Vary stick trying to zero pitching moment
   double stickBackBaseline    = 0.0;
   double stickBackThrustShift = 0.0;
   double stickBackXCmShift    = 0.0;
   double stickBackZCmShift    = 0.0;

   // FUTURE_IMPROVEMENT_NOTE -- Some testing showed that the approach below might be
   // beneficial. Although at this point, we cannot control at this alpha, there appeared
   // to be some situations where there was a small error in the alpha value, so this
   // technique appeared to work. However, at the present time, we have decided against
   // using it.
   // if (!valid)
   // {
   //    int count = 0;
   //    double deltaAlpha_rad = 0.001 * UtMath::cRAD_PER_DEG;
   //    double tempAlpha_rad = aAlpha_rad;
   //
   //    if (aLowPoint)
   //    {
   //       bool done = false;
   //       while (!done)
   //       {
   //          tempAlpha_rad += deltaAlpha_rad;
   //
   //          valid = CalculateStickForZeroMoment(aMach, tempAlpha_rad, aStickBack);
   //          if (valid)
   //          {
   //             aAlpha_rad = tempAlpha_rad;
   //             done = true;
   //          }
   //
   //          ++count;
   //          if (count > 100)
   //          {
   //             done = true;
   //          }
   //       }
   //    }
   //    else if (aHighPoint)
   //    {
   //       bool done = false;
   //       while (!done)
   //       {
   //          tempAlpha_rad -= deltaAlpha_rad;
   //
   //          valid = CalculateStickForZeroMoment(aMach, tempAlpha_rad, aStickBack);
   //          if (valid)
   //          {
   //             aAlpha_rad = tempAlpha_rad;
   //             done = true;
   //          }
   //
   //          ++count;
   //          if (count > 100)
   //          {
   //             done = true;
   //          }
   //       }
   //    }
   // }

   double alpha_deg = aAlpha_rad * UtMath::cDEG_PER_RAD;

   if (CalcStickAndCL(aMach, aAlpha_rad, stickBackBaseline, stickBackThrustShift, stickBackXCmShift, stickBackZCmShift))
   {
      // We may be able to control at this alpha, so add to lists

      double coreCLArea     = 0;
      double controlsCLArea = 0;
      double CL_Area        = 0;

      // Get the current CL
      mObject->CalculateCLAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, coreCLArea, controlsCLArea);
      CL_Area = coreCLArea + controlsCLArea;

      // Divide by ref area
      double CL = CalcCLFromCLArea(CL_Area);

      // Use the current stick, CL, Mach, and alpha to create entries
      // in the mStickForZeroMomentVsMachTable and mCLVsMachAlphaTable tables
      // Add mach entry, if needed

      if (!aMachAdded)
      {
         mStickForZeroMomentVsMachAlphaTablePtr->CreateNewMach(aMach);
         mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr->CreateNewMach(aMach);
         mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr->CreateNewMach(aMach);
         mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr->CreateNewMach(aMach);
         mCLVsMachAlphaTable->CreateNewMach(aMach);
         aMachAdded = true;
      }

      // Add param and data
      mStickForZeroMomentVsMachAlphaTablePtr->AddNewParamDataPair(alpha_deg, stickBackBaseline);
      mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr->AddNewParamDataPair(alpha_deg, stickBackThrustShift);
      mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr->AddNewParamDataPair(alpha_deg, stickBackXCmShift);
      mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr->AddNewParamDataPair(alpha_deg, stickBackZCmShift);
      mCLVsMachAlphaTable->AddNewParamDataPair(alpha_deg, CL);

      auto out = ut::log::info() << "Added to tables:";
      out.AddNote() << "Mach: " << aMach;
      out.AddNote() << "Alpha: " << alpha_deg << " deg";
      out.AddNote() << "Produced CL: " << CL; // units?
      out.AddNote() << "Stick Back: " << aStickBack;
   }
   else
   {
      // We cannot control at this alpha
      // double alpha_deg = aAlpha_rad * UtMath::cDEG_PER_RAD;

      auto out = ut::log::info() << "Cannot control at given mach and alpha.";
      out.AddNote() << "Mach: " << aMach;
      out.AddNote() << "Alpha: " << alpha_deg << " deg";

      aStickBack = 0.0;
   }
}

void wsf::six_dof::RigidBodyTableTool::OutputCLMaxMachTableToInputString(std::string& aInput) const
{
   if (mCLMaxVsMachTable == nullptr)
   {
      return;
   }

   mCLMaxVsMachTable->OutputToString(aInput, "cl_max_mach_table");
}

void wsf::six_dof::RigidBodyTableTool::OutputCLMinMachTableToInputString(std::string& aInput) const
{
   if (mCLMinVsMachTable == nullptr)
   {
      return;
   }

   mCLMinVsMachTable->OutputToString(aInput, "cl_min_mach_table");
}

void wsf::six_dof::RigidBodyTableTool::OutputAlphaMaxMachTableToInputString(std::string& aInput) const
{
   if (mAlphaMax_degVsMachTable == nullptr)
   {
      return;
   }

   mAlphaMax_degVsMachTable->OutputToString(aInput, "alpha_max_mach_table");
}

void wsf::six_dof::RigidBodyTableTool::OutputAlphaMinMachTableToInputString(std::string& aInput) const
{
   if (mAlphaMin_degVsMachTable == nullptr)
   {
      return;
   }

   mAlphaMin_degVsMachTable->OutputToString(aInput, "alpha_min_mach_table");
}

void wsf::six_dof::RigidBodyTableTool::OutputAlphaMachCLTableToInputString(std::string& aInput) const
{
   if (mAlphaVsMachCLTable == nullptr)
   {
      return;
   }

   // aParameterType typically is "alpha" or "cl"
   // aParameterUnitsString typically is "units deg" or "precision float"
   mAlphaVsMachCLTable->OutputToString(aInput, "alpha_versus_mach_cl_table", "cl", "precision float");
}

void wsf::six_dof::RigidBodyTableTool::OutputStickMachAlphaTablesToInputString(std::string& aInput) const
{
   if (mStickForZeroMomentVsMachAlphaTablePtr)
   {
      mStickForZeroMomentVsMachAlphaTablePtr->OutputToString(aInput, "stick_zero_moment_mach_alpha_table", "alpha", "units deg");
   }

   if (mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr)
   {
      mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr
         ->OutputToString(aInput, "stick_zero_moment_delta_thrust_mach_alpha_table", "alpha", "units deg");
   }

   if (mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
   {
      mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr->OutputToString(aInput,
                                                                     "stick_zero_moment_delta_xcg_mach_alpha_table",
                                                                     "alpha",
                                                                     "units deg");
   }

   if (mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
   {
      mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr->OutputToString(aInput,
                                                                     "stick_zero_moment_delta_zcg_mach_alpha_table",
                                                                     "alpha",
                                                                     "units deg");
   }
}

void wsf::six_dof::RigidBodyTableTool::OutputCLMachAlphaTableToInputString(std::string& aInput) const
{
   if (mCLVsMachAlphaTable == nullptr)
   {
      return;
   }

   // aParameterType typically is "alpha" or "cl"
   // aParameterUnitsString typically is "units deg" or "precision float"
   mCLVsMachAlphaTable->OutputToString(aInput, "effective_CL_versus_mach_alpha_table", "alpha", "units deg");
}

void wsf::six_dof::RigidBodyTableTool::InitializeTableMachValues(double aMaxMach)
{
   CreateMachTestList(aMaxMach);
}

void wsf::six_dof::RigidBodyTableTool::CreateMachTestList(double aMaxMach)
{
   // Clear the list
   mMachTestList.clear();

   // Add entries to the list, based on max mach expected

   mMachTestList.push_back(0.300);

   // Special case for less than Mach 0.5
   if (aMaxMach < 0.5)
   {
      mMachTestList.push_back(0.500);
      return;
   }

   mMachTestList.push_back(0.600);
   mMachTestList.push_back(0.700);
   mMachTestList.push_back(0.750);
   mMachTestList.push_back(0.800);

   if (aMaxMach < 0.8)
   {
      return;
   }

   mMachTestList.push_back(0.820);
   mMachTestList.push_back(0.850);
   mMachTestList.push_back(0.870);
   mMachTestList.push_back(0.900);
   mMachTestList.push_back(0.925);
   mMachTestList.push_back(0.950);
   mMachTestList.push_back(0.975);
   mMachTestList.push_back(1.000);

   if (aMaxMach < 1.0)
   {
      return;
   }

   mMachTestList.push_back(1.025);
   mMachTestList.push_back(1.050);
   mMachTestList.push_back(1.075);
   mMachTestList.push_back(1.100);
   mMachTestList.push_back(1.150);
   mMachTestList.push_back(1.200);
   mMachTestList.push_back(1.250);
   mMachTestList.push_back(1.300);
   mMachTestList.push_back(1.400);
   mMachTestList.push_back(1.500);
   mMachTestList.push_back(1.600);
   mMachTestList.push_back(1.800);
   mMachTestList.push_back(2.000);

   if (aMaxMach < 2.0)
   {
      return;
   }

   mMachTestList.push_back(2.200);
   mMachTestList.push_back(2.500);
   mMachTestList.push_back(3.000);

   if (aMaxMach < 3.0)
   {
      return;
   }

   mMachTestList.push_back(4.000);
   mMachTestList.push_back(5.000);
   mMachTestList.push_back(6.000);

   if (aMaxMach < 6.0)
   {
      return;
   }

   mMachTestList.push_back(7.000);
   mMachTestList.push_back(8.000);
   mMachTestList.push_back(9.000);

   if (aMaxMach < 9.0)
   {
      return;
   }

   mMachTestList.push_back(10.00);
   mMachTestList.push_back(15.000);
   mMachTestList.push_back(20.000);

   if (aMaxMach < 20.0)
   {
      return;
   }

   mMachTestList.push_back(25.000);
   mMachTestList.push_back(30.000);
   mMachTestList.push_back(50.000);
}

// Helper function for writing Autopilot config files
void WritePIDBlock(std::ofstream&                  aStream,
                   const std::string&              aBlockName,
                   const std::string&              aBlockWhiteSpace,
                   wsf::six_dof::Pid::Type         aPid,
                   wsf::six_dof::CommonController* aPilot)
{
   std::vector<wsf::six_dof::PidGainData>* data = aPilot->GetAutopilotPidGainData(aPid);

   if (data != nullptr)
   {
      aStream << aBlockWhiteSpace << aBlockName << std::endl;
      std::string localWhiteSpace = aBlockWhiteSpace + "  ";

      bool useTable = (data->size() != 1);
      if (useTable)
      {
         aStream << aBlockWhiteSpace << "  gain_table" << std::endl;
         localWhiteSpace = localWhiteSpace + "  ";
      }

      for (const auto& d : *data)
      {
         if (useTable)
         {
            aStream << localWhiteSpace << "control_value " << d.ControllingValue << std::endl;
         }
         aStream << localWhiteSpace << "kp " << d.KpGain << std::endl;
         aStream << localWhiteSpace << "ki " << d.KiGain << std::endl;
         aStream << localWhiteSpace << "kd " << d.KdGain << std::endl;

         constexpr float maxFloat = std::numeric_limits<float>::max();
         constexpr float minFloat = std::numeric_limits<float>::min();
         if (d.MaxAccum < maxFloat)
         {
            aStream << localWhiteSpace << "max_error_accum " << d.MaxAccum << std::endl;
         }
         if (!UtMath::NearlyEqual(d.LowpassAlpha, 1.0f))
         {
            aStream << localWhiteSpace << "low_pass_alpha " << d.LowpassAlpha << std::endl;
         }
         if (d.MaxErrorZero < maxFloat)
         {
            aStream << localWhiteSpace << "ignore_large_error_accum " << d.MaxErrorZero << std::endl;
         }
         if (d.MinErrorZero > minFloat)
         {
            aStream << localWhiteSpace << "ignore_small_error_accum " << d.MinErrorZero << std::endl;
         }
         if (!UtMath::NearlyZero(d.KtAntiWindup))
         {
            aStream << localWhiteSpace << "kt_anti_windup_gain " << d.KtAntiWindup << std::endl;
         }
      }

      if (useTable)
      {
         aStream << aBlockWhiteSpace << "  end_gain_table" << std::endl;
      }

      aStream << aBlockWhiteSpace << "end_" << aBlockName << std::endl << std::endl;
   }
}

void wsf::six_dof::RigidBodyTableTool::Initialize()
{
   // Create CLMax table
   mCLMaxVsMachTable = ut::make_unique<RigidBodyMachDataTable>();

   // Create CLMin tables
   mCLMinVsMachTable = ut::make_unique<RigidBodyMachDataTable>();

   // Create AlphaMax table
   mAlphaMax_degVsMachTable = ut::make_unique<RigidBodyMachDataTable>();

   // Create AlphaMin table
   mAlphaMin_degVsMachTable = ut::make_unique<RigidBodyMachDataTable>();

   // Create StickForZeroMoment table
   mStickForZeroMomentVsMachAlphaTablePtr            = ut::make_unique<RigidBodyMachParamDataTable>();
   mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr = ut::make_unique<RigidBodyMachParamDataTable>();
   mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr    = ut::make_unique<RigidBodyMachParamDataTable>();
   mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr    = ut::make_unique<RigidBodyMachParamDataTable>();

   // Create CLVsMachAlpha table
   mCLVsMachAlphaTable = ut::make_unique<RigidBodyMachParamDataTable>();

   // Create AlphaVsMachCL table
   mAlphaVsMachCLTable = ut::make_unique<RigidBodyMachParamDataTable>();

   // We need to setup test conditions...
   mObject->SetLocationLLA(0.0, 0.0, 100.0);
   mObject->SetVelocityNED_mps(100.0, 0.0, 0.0);
   mObject->SetAttitudeNED_rad(0.0, 0.0, 0.0);
}

bool wsf::six_dof::RigidBodyTableTool::CreateCLMaxAndAlphaMaxTables()
{
   bool   controllable = false;
   size_t numTests     = mMachTestList.size();
   size_t count        = 0;

   // Note: Do not remove these TEST_NAME and PROGRESS flags, since they are used in Mover Creator
   // to parse console data to detect progress.
   ut::log::info() << "<<TEST_NAME 'CLMaxVsMach and AlphaMaxVsMach'>>";
   ut::log::info() << "<<PROGRESS '0'>>";

   // ........................................................................................................

   // First, we create the CLmax and AlphaMax tables -- loop through Mach
   for (double mach : mMachTestList)
   {
      // Loop and increment alpha until CL peaks
      bool   alphaLoopDone  = false;
      double alpha_rad      = 0;
      double stickBack      = 0;
      double coreCLArea     = 0;
      double controlsCLArea = 0;
      double CL_Area        = 0;
      double CL             = 0;

      bool increasingCL = false;

      bool alphaTestSucessful = false;

      mObject->CalculateCLAreaForCoreAndControls(mach, alpha_rad, stickBack, coreCLArea, controlsCLArea);
      CL_Area = coreCLArea + controlsCLArea;

      // Divide by ref area
      CL = CalcCLFromCLArea(CL_Area);

      double lastCL        = CL;
      double lastAlpha_rad = alpha_rad;

      const double deltaAlpha_rad = 0.0001745329; // This is 0.01 deg

      // We are looking for the highest CL we can achieve while still under control.
      // Keep incrementing alpha until we can no longer control (no success with
      // CalculateStickForZeroMoment) or until CL peaks.
      while (!alphaLoopDone)
      {
         alpha_rad += deltaAlpha_rad;

         // Setup pitch using alpha...
         mObject->SetAttitudeNED_rad(0.0, alpha_rad, 0.0);

         // We may want to consider the use of "flags" to include whether subobjects and/or
         // fuel should be used in mass and aero calculations

         // Adjust the stick to nullify any pitching moment
         stickBack               = 0;
         double thrustOutput_lbs = 0.0;
         bool   success          = CalculateStickForZeroMoment(mach, alpha_rad, true, thrustOutput_lbs, stickBack);

         if (success)
         {
            // We can control at this alpha, so determine if we've reached the max CL yet

            mObject->CalculateCLAreaForCoreAndControls(mach, alpha_rad, stickBack, coreCLArea, controlsCLArea);
            CL_Area = coreCLArea + controlsCLArea;

            // Divide by ref area
            CL = CalcCLFromCLArea(CL_Area);

            if (CL > lastCL)
            {
               // CL is getting higher, so keep going
               lastCL             = CL;
               lastAlpha_rad      = alpha_rad;
               alphaTestSucessful = true;
               increasingCL       = true;
            }
            else
            {
               if (increasingCL)
               {
                  // Past the peak, so use the previous CL and exit loop
                  CL                 = lastCL;
                  alpha_rad          = lastAlpha_rad;
                  alphaTestSucessful = true;
                  alphaLoopDone      = true;
               }
            }
         }
         else
         {
            // Since we cannot control at this alpha, we're done.
            // Use the previous CL and exit loop.
            CL            = lastCL;
            alphaLoopDone = true;
         }
      }

      if (alphaTestSucessful)
      {
         double alpha_deg = alpha_rad * UtMath::cDEG_PER_RAD;
         controllable     = true;

         {
            // RAII block
            auto out = ut::log::info() << "Alpha test successful.";
            out.AddNote() << "Mach: " << mach;
            out.AddNote() << "Alpha: " << alpha_deg << " deg";
            out.AddNote() << "Produced CL: " << CL; // units?
            out.AddNote() << "Stick Back: " << stickBack;
         }

         mCLMaxVsMachTable->CreateNewMachValuePair(mach, CL);
         mAlphaMax_degVsMachTable->CreateNewMachValuePair(mach, alpha_deg);
      }
      else
      {
         auto out = ut::log::error() << "Unable to work.";
         out.AddNote() << "Mach: " << mach;
         out.AddNote() << "Alpha: " << alpha_rad * UtMath::cDEG_PER_RAD << " deg";
      }
      ++count;

      // Note: Do not remove this PROGRESS flag, since it is used in Mover Creator
      // to parse console data to detect progress.
      ut::log::info() << "<<PROGRESS '" << 1000 * count / numTests << "'>>";
   }
   return controllable;
}

bool wsf::six_dof::RigidBodyTableTool::CreateCLMinAndAlphaMinTables()
{
   bool   controllable = false;
   size_t numTests     = mMachTestList.size();
   size_t count        = 0;

   // Note: Do not remove these TEST_NAME and PROGRESS flags, since they are used in Mover Creator
   // to parse console data to detect progress.
   ut::log::info() << "<<TEST_NAME 'CLMinVsMach and AlphaMinVsMach'>>";
   ut::log::info() << "<<PROGRESS '0'>>";

   // ........................................................................................................

   // Next, we create the CLmin and AlphaMin tables -- loop through Mach
   for (double mach : mMachTestList)
   {
      // Loop and increment alpha until CL peaks negative
      bool   alphaLoopDone  = false;
      double alpha_rad      = 0;
      double stickBack      = 0;
      double coreCLArea     = 0;
      double controlsCLArea = 0;
      double CL_Area        = 0;
      double CL             = 0;

      bool decreasingCL = false;

      bool alphaTestSucessful = false;

      mObject->CalculateCLAreaForCoreAndControls(mach, alpha_rad, stickBack, coreCLArea, controlsCLArea);
      CL_Area = coreCLArea + controlsCLArea;

      // Divide by ref area
      CL = CalcCLFromCLArea(CL_Area);

      double lastCL        = CL;
      double lastAlpha_rad = alpha_rad;

      const double deltaAlpha_rad = -0.0001745329; // This is -0.01 deg

      // We are looking for the highest CL we can achieve while still under control.
      // Keep incrementing alpha until we can no longer control (no success with
      // CalculateStickForZeroMoment) or until CL peaks.
      while (!alphaLoopDone)
      {
         alpha_rad += deltaAlpha_rad;

         // Setup pitch using alpha...
         mObject->SetAttitudeNED_rad(0.0, alpha_rad, 0.0);

         // We may want to consider the use of "flags" to include whether subobjects and/or
         // fuel should be used in mass and aero calculations

         // Adjust the stick to nullify any pitching moment
         stickBack               = 0;
         double thrustOutput_lbs = 0.0;
         bool   success          = CalculateStickForZeroMoment(mach, alpha_rad, true, thrustOutput_lbs, stickBack);

         if (success)
         {
            // We can control at this alpha, so determine if we've reached the max CL yet

            mObject->CalculateCLAreaForCoreAndControls(mach, alpha_rad, stickBack, coreCLArea, controlsCLArea);
            CL_Area = coreCLArea + controlsCLArea;

            // Divide by ref area
            CL = CalcCLFromCLArea(CL_Area);

            if (CL < lastCL)
            {
               // CL is getting lower, so keep going
               lastCL             = CL;
               lastAlpha_rad      = alpha_rad;
               alphaTestSucessful = true;
               decreasingCL       = true;
            }
            else
            {
               if (decreasingCL)
               {
                  // Past the peak, so use the previous CL and exit loop
                  CL                 = lastCL;
                  alpha_rad          = lastAlpha_rad;
                  alphaTestSucessful = true;
                  alphaLoopDone      = true;
               }
            }
         }
         else
         {
            // Since we cannot control at this alpha, we're done.
            // Use the previous CL and exit loop.
            CL            = lastCL;
            alphaLoopDone = true;
         }
      }

      if (alphaTestSucessful)
      {
         double alpha_deg = alpha_rad * UtMath::cDEG_PER_RAD;
         controllable     = true;

         auto out = ut::log::info() << "Alpha test successful.";
         out.AddNote() << "Mach: " << mach;
         out.AddNote() << "Alpha: " << alpha_deg << " deg";
         out.AddNote() << "Produced CL: " << CL;
         out.AddNote() << "Stick Back: " << stickBack;

         mCLMinVsMachTable->CreateNewMachValuePair(mach, CL);
         mAlphaMin_degVsMachTable->CreateNewMachValuePair(mach, alpha_deg);
      }
      else
      {
         auto out = ut::log::error() << "Unable to work.";
         out.AddNote() << "Mach: " << mach;
         out.AddNote() << "Alpha: " << alpha_rad * UtMath::cDEG_PER_RAD << " deg";
      }
      ++count;

      // Note: Do not remove this PROGRESS flag, since it is used in Mover Creator
      // to parse console data to detect progress.
      ut::log::info() << "<<PROGRESS '" << 1000 * count / numTests << "'>>";
   }
   return controllable;
}

bool wsf::six_dof::RigidBodyTableTool::CreateStickForZeroMomentTable()
{
   // We create the mStickForZeroMomentVsMachTable and mCLVsMachAlpha by looping through
   // a Mach outer loop, and then looping through alpha (between the max/min alpha), while
   // adjusting stickBack to get a zero moment (if possible). If this data point is valid,
   // use the current stick, CL, Mach, and alpha to create entries in Stick-Mach-Alpha and
   // CL-Mach-Alpha tables.

   // Note: Do not remove these TEST_NAME and PROGRESS flags, since they are used in Mover Creator
   // to parse console data to detect progress.
   ut::log::info() << "<<TEST_NAME 'StickForZeroMomentVsMach and CLVsMachAlpha'>>";
   ut::log::info() << "<<PROGRESS '0'>>";

   bool success = false;

   size_t numTests = mMachTestList.size();
   size_t count    = 0;

   for (double mach : mMachTestList)
   {
      // Loop and increment alpha until CL peaks
      double minAlpha_deg = mAlphaMin_degVsMachTable->GetValueAtMach(mach);
      double maxAlpha_deg = mAlphaMax_degVsMachTable->GetValueAtMach(mach);

      // We will step at half degree increments

      int lowsideAlpha_halfdeg  = static_cast<int>((minAlpha_deg + 0.5) * 2.0);
      int highsideAlpha_halfdeg = static_cast<int>((maxAlpha_deg - 0.5) * 2.0);

      int deltaInt = highsideAlpha_halfdeg - lowsideAlpha_halfdeg;
      if (deltaInt < 3)
      {
         // If the available alpha range is too small, we cannot output data for this Mach
         ut::log::warning() << "Aero data problem. Insufficient available alpha span (minAlpha=" << minAlpha_deg
                            << "deg maxAlpha=" << maxAlpha_deg << "deg)";
         continue;
      }

      std::vector<double> testAlphaVector_deg;

      // Push minimum alpha
      testAlphaVector_deg.push_back(minAlpha_deg);

      // Push intermediate alphas
      for (int alpha_halfdeg = lowsideAlpha_halfdeg; alpha_halfdeg <= highsideAlpha_halfdeg; ++alpha_halfdeg)
      {
         double alpha_deg = 0.5 * static_cast<double>(alpha_halfdeg);
         testAlphaVector_deg.push_back(alpha_deg);
      }

      // Push maximum alpha
      testAlphaVector_deg.push_back(maxAlpha_deg);

      // Verify we have at least 3 controllable points for this Mach
      deltaInt = CountValidStickAndCL(mach, testAlphaVector_deg);
      if (deltaInt < 3)
      {
         // We have a problem -- the span of alphas is unrealistically too small
         auto out = ut::log::warning() << "Aero data problem. Insufficient span.";
         out.AddNote() << "Min Alpha: " << minAlpha_deg << " deg";
         out.AddNote() << "Max Alpha: " << maxAlpha_deg << " deg";
      }
      else
      {
         double stickBack = 0;
         bool   machAdded = false;

         for (auto& alpha_deg : testAlphaVector_deg)
         {
            double alpha_rad = alpha_deg * UtMath::cRAD_PER_DEG;
            AddStickAndCL(mach, alpha_rad, stickBack, machAdded, false, false);
         }

         if (machAdded)
         {
            success = true;
         }
      }
      ++count;

      // Note: Do not remove this PROGRESS flag, since it is used in Mover Creator
      // to parse console data to detect progress.
      ut::log::info() << "<<PROGRESS '" << 1000 * count / numTests << "'>>";
   }

   return success;
}

bool wsf::six_dof::RigidBodyTableTool::CreateAlphaVsMachCLTable()
{
   // We now create the mAlphaVsMachCLTable by looping through another Mach outer loop,
   // and looping through CL (between the max/min CL) while using a binary-search varying
   // alpha with CL-Mach-Alpha to match the desired CL. Once this is achieved, we'll use
   // the current alpha and CL to create Alpha-Mach-CL.

   // Note: Do not remove these TEST_NAME and PROGRESS flags, since they are used in Mover Creator
   // to parse console data to detect progress.
   ut::log::info() << "<<TEST_NAME 'AlphaVsMachCL'>>";
   ut::log::info() << "<<PROGRESS '0'>>";

   bool success = false;

   size_t numTests = mMachTestList.size();
   size_t count    = 0;

   for (double mach : mMachTestList)
   {
      // Loop and increment alpha until CL peaks
      bool machAdded = false;

      double minAlpha_deg = mCLVsMachAlphaTable->GetMinParameterAtMach(mach);
      double maxAlpha_deg = mCLVsMachAlphaTable->GetMaxParameterAtMach(mach);

      double minCL = mCLVsMachAlphaTable->GetMinDataAtMach(mach);
      double maxCL = mCLVsMachAlphaTable->GetMaxDataAtMach(mach);

      int lowsideCL_tenths  = static_cast<int>((minCL + 0.1) * 10.0);
      int highsideCL_tenths = static_cast<int>((maxCL - 0.1) * 10.0);

      int lowsideCL_twentieths  = static_cast<int>((minCL + 0.05) * 20.0);
      int highsideCL_twentieths = static_cast<int>((maxCL - 0.05) * 20.0);

      int lowsideCL_fiftieths  = static_cast<int>((minCL + 0.02) * 50.0);
      int highsideCL_fiftieths = static_cast<int>((maxCL - 0.02) * 50.0);

      int lowsideCL_hundredths  = static_cast<int>((minCL + 0.01) * 100.0);
      int highsideCL_hundredths = static_cast<int>((maxCL - 0.01) * 100.0);

      double deltaCL_tenths     = highsideCL_tenths - lowsideCL_tenths;
      double deltaCL_twentieths = highsideCL_twentieths - lowsideCL_twentieths;
      double deltaCL_fiftieths  = highsideCL_fiftieths - lowsideCL_fiftieths;
      double deltaCL_hundredths = highsideCL_hundredths - lowsideCL_hundredths;

      bool done = false;

      if (deltaCL_hundredths < 2)
      {
         // We have a problem -- the span of CLs is unrealistically too small
         auto out = ut::log::warning() << "Aero data problem. Insufficient CL span.";
         out.AddNote() << "Min CL: " << minCL;
         out.AddNote() << "Max CL: " << maxCL;

         continue;
      }

      // .......................................................................

      // Output the low point
      double CL = minCL;
      AddAlphaVsMachCL(mach, CL, minAlpha_deg, maxAlpha_deg, machAdded);

      // .......................................................................

      if (deltaCL_tenths > 20)
      {
         // Output the central region
         for (int CL_fraction = lowsideCL_tenths; CL_fraction <= highsideCL_tenths; ++CL_fraction)
         {
            CL = 0.1 * static_cast<double>(CL_fraction);
            AddAlphaVsMachCL(mach, CL, minAlpha_deg, maxAlpha_deg, machAdded);
         }

         done = true;
      }
      else if (!done && (deltaCL_twentieths > 20))
      {
         // Output the central region
         for (int CL_fraction = lowsideCL_twentieths; CL_fraction <= highsideCL_twentieths; ++CL_fraction)
         {
            CL = 0.05 * static_cast<double>(CL_fraction);
            AddAlphaVsMachCL(mach, CL, minAlpha_deg, maxAlpha_deg, machAdded);
         }

         done = true;
      }
      else if (!done && (deltaCL_fiftieths > 20))
      {
         // Output the central region
         for (int CL_fraction = lowsideCL_fiftieths; CL_fraction <= highsideCL_fiftieths; ++CL_fraction)
         {
            CL = 0.02 * static_cast<double>(CL_fraction);
            AddAlphaVsMachCL(mach, CL, minAlpha_deg, maxAlpha_deg, machAdded);
         }

         done = true;
      }
      else if (!done)
      {
         // Output the central region
         for (int CL_fraction = lowsideCL_hundredths; CL_fraction <= highsideCL_hundredths; ++CL_fraction)
         {
            CL = 0.01 * static_cast<double>(CL_fraction);
            AddAlphaVsMachCL(mach, CL, minAlpha_deg, maxAlpha_deg, machAdded);
         }

         done = true;
      }

      // .......................................................................

      // Output the high point
      CL = maxCL;
      AddAlphaVsMachCL(mach, CL, minAlpha_deg, maxAlpha_deg, machAdded);

      // .......................................................................
      success = true;

      ++count;

      // Note: Do not remove this PROGRESS flag, since it is used in Mover Creator
      // to parse console data to detect progress.
      ut::log::info() << "<<PROGRESS '" << 1000 * count / numTests << "'>>";
   }
   return success;
}
