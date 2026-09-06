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

#include "P6DofUtils.hpp"

#include <fstream>
#include <sstream>

#include "P6DofAeroCoreObject.hpp"
#include "P6DofCommonController.hpp"
#include "P6DofFlightControlSystem.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofFuelTank.hpp"
#include "P6DofLandingGear.hpp"
#include "P6DofPilotManager.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTerrain.hpp"
#include "P6DofTunerGAManager.hpp"
#include "P6DofVehicle.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// -------------------------------------------------------------------------------

// This is used for the earth radius when mUseSphericalEarth is true
// Note: The AFSIM cEARTH_RADIUS = 6366707.0194937074958 (6366.7 km)
// Typical earth radius is 6371 km -- note the 4.3 km discrepancy
// Distances from points on the surface to the center range from
// 6,353 km to 6,384 km. Several different ways of modeling the
// Earth as a sphere each yield a mean radius of 6,371 km.
static const double gSphericalEarthRadius_m = 6371000.0;

// -------------------------------------------------------------------------------

// This outputs a double into a string.
// aWidth is the total width of the string and aPrecision is the number of
// digits to the right of the decimal.
std::string outputDoubleString(double aValue, int aWidth, int aPrecision)
{
   std::stringstream ss;

   ss.setf(std::ios::fixed);
   ss.width(aWidth);
   ss.precision(aPrecision);
   ss << aValue;

   return ss.str();
}

// Helper function for writing Autopilot config files (defined at bottom of file)
void WritePIDBlock(std::ofstream&         aStream,
                   const std::string&     aBlockName,
                   const std::string&     aBlockWhiteSpace,
                   P6DOF::Pid::Type       aPid,
                   P6DofCommonController* aPilot);

// -------------------------------------------------------------------------------

void P6DofUtils::Autotune(P6DofVehicle* aVehicle)
{
   P6DofTunerGASpecimen parentSpecimen;
   P6dofTunerGAManager  GA_Manager;

   std::vector<P6DOF::Pid::Type> vehiclePids = P6DofUtils::GetRelevantPids(aVehicle);

   std::vector<P6DofTunerGASpecimen::PidLogElement> pidLog;
   double                                           runTime      = 20.0;
   double                                           platformTime = 0.0;

   for (auto& pidType : vehiclePids)
   {
      // If you're tuning altitude you need to run longer
      if (pidType == P6DOF::Pid::Altitude)
      {
         runTime = 60.0;
      }
      else
      {
         runTime = 20.0;
      }

      GA_Manager.InitializePopulation(parentSpecimen);

      while (true)
      {
         // Set PID gains for next run
         P6DofCommonController* autopilot = aVehicle->GetPilotObject()->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetPidGainKP(pidType, 0, static_cast<float>(GA_Manager.GetCurrentSpecimen().GetKp()));
            autopilot->SetPidGainKI(pidType, 0, static_cast<float>(GA_Manager.GetCurrentSpecimen().GetKi()));
            autopilot->SetPidGainKD(pidType, 0, static_cast<float>(GA_Manager.GetCurrentSpecimen().GetKd()));
         }

         // determine end time of next run
         double runEndTime = platformTime + runTime;

         // Preposition before each run
         if (pidType == P6DOF::Pid::TaxiHeading || pidType == P6DOF::Pid::TaxiSpeed || pidType == P6DOF::Pid::TaxiYawRate)
         {
            P6DofLandingGear* gear = aVehicle->GetLandingGear();
            if (gear != nullptr)
            {
               aVehicle->ForceLandingGearDownInstantly("Landing_Gear_Extended");
               double        hgt_nominal_m = gear->GetNominalHeightAboveGroundOnGear();
               double        hgt_terrain_m = 0.0;
               P6DofTerrain* terrain       = aVehicle->GetScenario()->GetTerrain();
               if (terrain != nullptr)
               {
                  double startLat, startLon;
                  aVehicle->GetStartingLatLon(startLat, startLon);
                  hgt_terrain_m = terrain->HeightOfTerrain_m(startLat, startLon);
               }
               double hgt_m = hgt_terrain_m + hgt_nominal_m;
               aVehicle->GetPilotObject()->SetPrePositionTAS(hgt_m * UtMath::cFT_PER_M, 0);
            }
         }
         else
         {
            aVehicle->GetPilotObject()->SetPrePositionTAS(10000, 100);
         }

         // Switch on the PID to determine what command needs to be sent and send it
         switch (pidType)
         {
         case P6DOF::Pid::Alpha:
            aVehicle->GetPilotObject()->SetAutopilotAlpha(2);
            break;

         case P6DOF::Pid::VertSpeed:
            aVehicle->GetPilotObject()->SetAutopilotVerticalSpeed(2000);
            break;

         case P6DOF::Pid::PitchAngle:
            aVehicle->GetPilotObject()->SetAutopilotPitchAngle(10);
            break;

         case P6DOF::Pid::PitchRate:
            aVehicle->GetPilotObject()->SetAutopilotPitchRate(10);
            break;

         case P6DOF::Pid::FltPathAngle:
            aVehicle->GetPilotObject()->SetAutopilotFlightPathAngle(10);
            break;

         case P6DOF::Pid::DeltaPitch:
            aVehicle->GetPilotObject()->SetAutopilotDeltaPitch(10);
            break;

         case P6DOF::Pid::Altitude:
            aVehicle->GetPilotObject()->SetAutopilotAltitude(12000);
            break;

         case P6DOF::Pid::Beta:
            aVehicle->GetPilotObject()->SetAutopilotBeta(2);
            break;

         case P6DOF::Pid::YawRate:
            aVehicle->GetPilotObject()->SetAutopilotYawRate(10);
            break;

         case P6DOF::Pid::YawHeading:
            aVehicle->GetPilotObject()->SetAutopilotYawHeading(30);
            break;

         case P6DOF::Pid::TaxiHeading:
            break;

         case P6DOF::Pid::RollRate:
            aVehicle->GetPilotObject()->SetAutopilotRollRate(30);
            break;

         case P6DOF::Pid::DeltaRoll:
            aVehicle->GetPilotObject()->SetAutopilotDeltaRoll(30);
            break;

         case P6DOF::Pid::BankAngle:
            aVehicle->GetPilotObject()->SetAutopilotRollAngle(30);
            break;

         case P6DOF::Pid::RollHeading:
            aVehicle->GetPilotObject()->SetAutopilotRollHeading(30);
            break;

         case P6DOF::Pid::Speed:
            aVehicle->GetPilotObject()->SetAutopilotSpeedKTAS(200);
            break;

         case P6DOF::Pid::TaxiSpeed:
            break;

         case P6DOF::Pid::TaxiYawRate:
            break;

         default:
            // TODO
            break;
         }

         // Update the vehicle through the specified time
         for (platformTime = 0.0; platformTime <= runEndTime; platformTime += 0.01)
         {
            aVehicle->Update(platformTime);
            P6DOF::AutopilotPidGroupValueData pidGroup;
            aVehicle->GetPilotObject()->GetCommonController()->GetAutopilotPidValues(&pidGroup);

            P6DofTunerGASpecimen::PidLogElement temp;
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

void P6DofUtils::CreateAutopilotConfigFile(P6DofVehicle* aVehicle, const std::string& aFilename)
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
      file << "# This file was generated by the P6DofUtils::CreateAutopilotConfigFile() function" << std::endl;
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

      P6DofPilotObject*      pilotObject = aVehicle->GetPilotObject();
      P6DofCommonController* controller  = nullptr;
      if (pilotObject != nullptr)
      {
         controller = pilotObject->GetCommonController();
         if (controller != nullptr)
         {
            P6DOF::Control::Method controlMethod = controller->GetControlMethod();
            switch (controlMethod)
            {
            case P6DOF::Control::BankToTurnNoYaw:
               file << "  control_method                    BANK_TO_TURN_NO_YAW" << std::endl;
               break;
            case P6DOF::Control::BankToTurnWithYaw:
               file << "  control_method                    BANK_TO_TURN_WITH_YAW" << std::endl;
               break;
            case P6DOF::Control::YawToTurnNoRoll:
               file << "  control_method                    YAW_TO_TURN_NO_ROLL" << std::endl;
               break;
            case P6DOF::Control::YawToTurnRollRate:
               file << "  control_method                    YAW_TO_TURN_ROLL_RATE" << std::endl;
               break;
            case P6DOF::Control::YawToTurnZeroBank:
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
      WritePIDBlock(file, "pid_alpha", whitespace, P6DOF::Pid::Alpha, aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_altitude", whitespace, P6DOF::Pid::Altitude, aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_bank_angle",
                    whitespace,
                    P6DOF::Pid::BankAngle,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_beta", whitespace, P6DOF::Pid::Beta, aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_delta_pitch",
                    whitespace,
                    P6DOF::Pid::DeltaPitch,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_delta_roll",
                    whitespace,
                    P6DOF::Pid::DeltaRoll,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_flightpath_angle",
                    whitespace,
                    P6DOF::Pid::FltPathAngle,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_forward_accel",
                    whitespace,
                    P6DOF::Pid::ForwardAccel,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_pitch_angle",
                    whitespace,
                    P6DOF::Pid::PitchAngle,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_pitch_rate",
                    whitespace,
                    P6DOF::Pid::PitchRate,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_roll_heading",
                    whitespace,
                    P6DOF::Pid::RollHeading,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_roll_rate", whitespace, P6DOF::Pid::RollRate, aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_speed", whitespace, P6DOF::Pid::Speed, aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_forward_accel",
                    whitespace,
                    P6DOF::Pid::TaxiForwardAccel,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_heading",
                    whitespace,
                    P6DOF::Pid::TaxiHeading,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_speed",
                    whitespace,
                    P6DOF::Pid::TaxiSpeed,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_taxi_yaw_rate",
                    whitespace,
                    P6DOF::Pid::TaxiYawRate,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_vert_speed",
                    whitespace,
                    P6DOF::Pid::VertSpeed,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file,
                    "pid_yaw_heading",
                    whitespace,
                    P6DOF::Pid::YawHeading,
                    aVehicle->GetPilotObject()->GetCommonController());

      WritePIDBlock(file, "pid_yaw_rate", whitespace, P6DOF::Pid::YawRate, aVehicle->GetPilotObject()->GetCommonController());

      P6DOF::AutopilotLimitsAndSettings limits =
         aVehicle->GetPilotObject()->GetCommonController()->GetCurrentLimitsAndSettings();
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

std::vector<P6DOF::Pid::Type> P6DofUtils::GetRelevantPids(P6DofVehicle* aVehicle)
{
   std::vector<P6DOF::Pid::Type> pidVector;

   // These pids are only relevant if the vehicle can actually land
   if (aVehicle->HasLandingGear())
   {
      pidVector.push_back(P6DOF::Pid::Type::TaxiYawRate);
      pidVector.push_back(P6DOF::Pid::Type::TaxiHeading);
      pidVector.push_back(P6DOF::Pid::Type::TaxiSpeed);
   }

   return pidVector;
}

const P6DOF::SinglePidValueData& P6DofUtils::GetSinglePidData(P6DOF::Pid::Type                         aPid,
                                                              const P6DOF::AutopilotPidGroupValueData& aPID_Values)
{
   switch (aPid)
   {
   case P6DOF::Pid::Alpha:
      return aPID_Values.alphaPID;
   case P6DOF::Pid::VertSpeed:
      return aPID_Values.vertSpeedPID;
   case P6DOF::Pid::PitchAngle:
      return aPID_Values.pitchAnglePID;
   case P6DOF::Pid::PitchRate:
      return aPID_Values.pitchRatePID;
   case P6DOF::Pid::FltPathAngle:
      return aPID_Values.fltpathAnglePID;
   case P6DOF::Pid::DeltaPitch:
      return aPID_Values.deltaPitchPID;
   case P6DOF::Pid::Altitude:
      return aPID_Values.altitudePID;
   case P6DOF::Pid::Beta:
      return aPID_Values.betaPID;
   case P6DOF::Pid::YawRate:
      return aPID_Values.yawRatePID;
   case P6DOF::Pid::YawHeading:
      return aPID_Values.yawheadingPID;
   case P6DOF::Pid::TaxiHeading:
      return aPID_Values.taxiHeadingPID;
   case P6DOF::Pid::RollRate:
      return aPID_Values.rollRatePID;
   case P6DOF::Pid::DeltaRoll:
      return aPID_Values.deltaRollPID;
   case P6DOF::Pid::BankAngle:
      return aPID_Values.bankAnglePID;
   case P6DOF::Pid::RollHeading:
      return aPID_Values.rollHeadingPID;
   case P6DOF::Pid::ForwardAccel:
      return aPID_Values.forwardAccelPID;
   case P6DOF::Pid::Speed:
      return aPID_Values.speedPID;
   case P6DOF::Pid::TaxiSpeed:
      return aPID_Values.taxiSpeedPID;
   case P6DOF::Pid::TaxiYawRate:
      return aPID_Values.taxiYawRatePID;
   default:
      // Use alpha PID as default
      return aPID_Values.alphaPID;
   }
}

P6DofUtils::CanFlyAtReturnValue P6DofUtils::CanFlyAt(P6DofVehicle* aObject,
                                                     double        aAltitude_ft,
                                                     double        aVelocity_fps,
                                                     double        aThrottleForward,
                                                     double        aFlapsDown)
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
         if (aObject->GetAccelNED_mps2().Get(0) >= 0)
         {
            // Check to see if we have Control Authority
            double stickBack = -1.0;
            aObject->TestingUpdate(aAltitude_ft, aVelocity_fps, pitch, stickBack, aThrottleForward, aFlapsDown);

            double angAccel1_X, angAccel1_Y, angAccel1_Z;
            aObject->GetBodyAngularAccel_rps2(angAccel1_X, angAccel1_Y, angAccel1_Z);

            stickBack = 1.0;
            aObject->TestingUpdate(aAltitude_ft, aVelocity_fps, pitch, stickBack, aThrottleForward, aFlapsDown);

            double angAccel2_X, angAccel2_Y, angAccel2_Z;
            aObject->GetBodyAngularAccel_rps2(angAccel2_X, angAccel2_Y, angAccel2_Z);

            if ((angAccel1_Y != angAccel2_Y) &&
                ((angAccel1_Y <= 0 && angAccel2_Y >= 0) || (angAccel1_Y >= 0 && angAccel2_Y <= 0)))
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

void P6DofUtils::CreateAutopilotSupportFile(P6DofVehicle* aObject, const std::string& aFilename)
{
   P6DofTableTool tool(aObject);

   // Note that we do not call tool.InitializeTableMachValues() directly. Instead, it
   // is called within the P6DofTableTool constructor using default values.

   CommonCreateAutopilotSupportFile(tool, aFilename);
}

void P6DofUtils::CreateAutopilotSupportFileWithMaxMach(P6DofVehicle*      aObject,
                                                       const std::string& aFilename,
                                                       const double&      aMaxMach)
{
   P6DofTableTool tool(aObject);

   tool.InitializeTableMachValues(aMaxMach);

   CommonCreateAutopilotSupportFile(tool, aFilename);
}

void P6DofUtils::CommonCreateAutopilotSupportFile(P6DofTableTool& aTool, const std::string& aFilename)
{
   bool createdOk = aTool.CreateAutopilotTables();
   if (!createdOk)
   {
      ut::log::error() << "Unable to create autopilot tables in P6DofUtils::CommonCreateAutopilotSupportFile().";
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
      ut::log::error()
         << "Unable to open autopilot_support_tables file in P6DofUtils::CommonCreateAutopilotSupportFile().";
   }
}

int64_t P6DofUtils::TimeToTime(double aSimTime_sec)
{
   double  localTime  = aSimTime_sec * 1000000000.0;
   int64_t local_nano = static_cast<int64_t>(localTime);

   return local_nano;
}

double P6DofUtils::TimeToTime(int64_t aSimTime_nanosec)
{
   return (0.000000001 * static_cast<double>(aSimTime_nanosec));
}

void P6DofUtils::CalcSphericalLatLonAlt(const UtVec3dX& aPos_m, double& aLat_deg, double& aLon_deg, double& aAlt_m)
{
   double pos_m[3];
   aPos_m.Get(pos_m);

   aLon_deg = UtMath::cDEG_PER_RAD * atan2(pos_m[1], pos_m[0]);
   if (aLon_deg > 180.0)
   {
      aLon_deg -= 360.0;
   }
   if (aLon_deg < -180.0)
   {
      aLon_deg += 360.0;
   }

   double dist = sqrt(pos_m[0] * pos_m[0] + pos_m[1] * pos_m[1]);

   if (dist < 0.0001)
   {
      if (pos_m[2] > (gSphericalEarthRadius_m - 1000.0))
      {
         aLat_deg = 90.0;
      }
      else if (pos_m[2] < -(gSphericalEarthRadius_m - 1000.0))
      {
         aLat_deg = -90.0;
      }
      else
      {
         aLat_deg = 0.0;
      }
   }
   else
   {
      aLat_deg = UtMath::cDEG_PER_RAD * atan2(pos_m[2], dist);
   }

   aAlt_m = aPos_m.Magnitude() - gSphericalEarthRadius_m;
}

void P6DofUtils::CalcSphericalVecToNED(const UtVec3dX& aPos, const UtVec3dX& aSphericalVec, UtVec3dX& aNEDVec)
{
   UtVec3dX localNorth;
   UtVec3dX localEast;
   UtVec3dX localDown;
   CalcLocalNEDVectors(aPos, localNorth, localEast, localDown);

   double velNED[3];
   velNED[0] = localNorth.Dot(aSphericalVec);
   velNED[1] = localEast.Dot(aSphericalVec);
   velNED[2] = localDown.Dot(aSphericalVec);

   aNEDVec.Set(velNED);
}

void P6DofUtils::CalcUnitVectors(const UtDCM& aDCM, UtVec3dX& aUnitVecX, UtVec3dX& aUnitVecY, UtVec3dX& aUnitVecZ)
{
   aUnitVecX.Set(1.0, 0.0, 0.0);
   aUnitVecY.Set(0.0, 1.0, 0.0);
   aUnitVecZ.Set(0.0, 0.0, 1.0);

   aUnitVecX = aDCM.InverseTransform(aUnitVecX);
   aUnitVecY = aDCM.InverseTransform(aUnitVecY);
   aUnitVecZ = aDCM.InverseTransform(aUnitVecZ);

   aUnitVecX.Normalize();
   aUnitVecY.Normalize();
   aUnitVecZ.Normalize();
}

void P6DofUtils::CalcLocalNEDVectors(const UtVec3dX& aPos, UtVec3dX& aLocalNorth, UtVec3dX& aLocalEast, UtVec3dX& aLocalDown)
{
   aLocalDown.Set(aPos);
   aLocalDown = aLocalDown * -1.0;
   aLocalDown.Normalize();

   UtVec3dX tempVec(0.0, 0.0, 10.0); // z is north
   aLocalEast = aLocalDown.Cross(tempVec);
   aLocalEast.Normalize();

   aLocalNorth = aLocalEast.Cross(aLocalDown);
   aLocalNorth.Normalize();
}

void P6DofUtils::CalcLocalAngles(const UtVec3dX& aLocalNorth,
                                 const UtVec3dX& aLocalEast,
                                 const UtVec3dX& aLocalDown,
                                 const UtVec3dX& aUnitVecX,
                                 const UtVec3dX& aUnitVecZ,
                                 double&         aLocalYaw_rad,
                                 double&         aLocalPitch_rad,
                                 double&         aLocalRoll_rad)
{
   // Determine if near vertical
   UtVec3dX localUp = -1.0 * aLocalDown;

   double dotProduct = aUnitVecX.Dot(localUp);
   if (dotProduct > 1.0)
   {
      dotProduct = 1.0;
   }
   if (dotProduct < -1.0)
   {
      dotProduct = -1.0;
   }
   double angleFromUp_rad = acos(dotProduct);

   if (fabs(angleFromUp_rad) < 1.75E-6) // 0.01 deg (0.0001/57.3 = 1.75e-6)
   {
      // A vertical or near-vertical situation
      aLocalPitch_rad = UtMath::cPI_OVER_2;
      aLocalRoll_rad  = 0.0;

      double northComp = aLocalNorth.Dot(aUnitVecZ);
      double eastComp  = aLocalEast.Dot(aUnitVecZ);

      aLocalYaw_rad = atan2(eastComp, northComp);

      return;
   }

   UtVec3dX tempRight = aLocalDown.Cross(aUnitVecX);
   tempRight.Normalize();
   UtVec3dX tempForward = tempRight.Cross(aLocalDown);
   tempForward.Normalize();

   double dot = tempForward.Dot(aUnitVecX);
   if (dot > 1.0)
   {
      dot = 1.0;
   }
   if (dot < -1.0)
   {
      dot = -1.0;
   }
   aLocalPitch_rad     = acos(dot);
   double downFraction = aLocalDown.Dot(aUnitVecX);
   if (downFraction > 0.0)
   {
      aLocalPitch_rad *= -1.0;
   }

   double northFraction = aLocalNorth.Dot(aUnitVecX);
   double eastFraction  = aLocalEast.Dot(aUnitVecX);
   aLocalYaw_rad        = atan2(eastFraction, northFraction);

   UtVec3dX tempDown = aUnitVecX.Cross(tempRight);
   double   tan_x    = tempRight.Dot(aUnitVecZ);
   double   tan_y    = tempDown.Dot(aUnitVecZ);
   aLocalRoll_rad    = atan2(tan_y, tan_x) - UtMath::cPI_OVER_2;
}

void P6DofUtils::CalcSphericalWCSAngles(const UtDCM& aDCM, double& aYaw_rad, double& aPitch_rad, double& aRoll_rad)
{
   double mat[3][3];
   aDCM.Get(mat);

   // static void ExtractEulerAngles(double  aTransform[3][3],
   //                                double& aPsi,       // [-pi, pi]
   //                                double& aTheta,     // [-pi/2, pi/2]
   //                                double& aPhi)       // [-pi/2, pi/2]

   // This algorithm is taken directly from:
   //
   // IEEE Standard for Distributed Interactive Simulation - Application Protocols
   // IEEE Std 1278.1-1995, Annex B, section B.1.6.1.5
   //
   // if aTransform[0][2] is exactly 1 or -1, the resulting value of theta will be
   // be -pi/2 or pi/2 respectively. Taking the cos(theta) would then result in a
   // value of zero and eventually a divide by zero. A more insidious problem arises,
   // however. Taking cos(asin(pi/2)) mathematically equals zero, but both GNU g++
   // Microsoft Visual C++ both return something that is not zero (about 1E-17).
   // When this is subsequently used in the recovery of the angles, one can get some
   // pretty violent fluctuations.
   //
   // If theta is +/- pi/2, we really can't solve for anything because psi and phi
   // cannot be uniquely determined. Take the matrix definition in B.1.6.1.4, and
   // set cos(theta)=0. Look at what's left, and you'll see there is no what to solve
   // for phi and psi unless you assume one of them. In this case we'll assume phi
   // (roll) is zero and solve for psi (yaw).

   if ((mat[0][2] >= 1.0) || (mat[0][2] <= -1.0))
   {
      aYaw_rad = acos(std::min(std::max(mat[1][1], -1.0), 1.0)); // [0, PI]
      if (mat[1][0] > 0.0)
      {
         aYaw_rad = -aYaw_rad;
      }
      aPitch_rad = UtMath::cPI_OVER_2; // theta has the opposite sign of mat[0][2]
      if (mat[0][2] >= 1.0)
      {
         aPitch_rad = -UtMath::cPI_OVER_2;
      }
      aRoll_rad = 0.0;
   }
   else
   {
      aPitch_rad      = asin(-mat[0][2]);
      double cosTheta = cos(aPitch_rad);
      aYaw_rad        = acos(std::min(std::max(mat[0][0] / cosTheta, -1.0), 1.0));
      if (mat[0][1] < 0.0)
      {
         aYaw_rad = -aYaw_rad;
      }
      aRoll_rad = acos(std::min(std::max(mat[2][2] / cosTheta, -1.0), 1.0));
      if (mat[1][2] < 0.0)
      {
         aRoll_rad = -aRoll_rad;
      }
   }
}

void P6DofUtils::SphericalGreatCircleHeadingAndDistance(const double aLat,
                                                        const double aLon,
                                                        const double aTargetLat,
                                                        const double aTargetLon,
                                                        double&      aTargetHeading_r,
                                                        double&      aTargetDistance_m)
{
   UtVec3dX startPos_m;
   CalcSphericalPositionFromLatLonAlt(aLat, aLon, 0.0, startPos_m);

   UtVec3dX tgtPos_m;
   CalcSphericalPositionFromLatLonAlt(aTargetLat, aTargetLon, 0.0, tgtPos_m);

   UtVec3dX vectorToTarget = tgtPos_m - startPos_m;
   vectorToTarget.Normalize();

   UtVec3dX northVec, eastVec, downVec;
   CalcLocalNEDVectors(startPos_m, northVec, eastVec, downVec);

   double northing  = vectorToTarget.Dot(northVec);
   double easting   = vectorToTarget.Dot(eastVec);
   aTargetHeading_r = atan2(easting, northing);

   startPos_m.Normalize();
   tgtPos_m.Normalize();
   double dotProduct = startPos_m.DotProduct(tgtPos_m);
   if (dotProduct > 1.0)
   {
      dotProduct = 1.0;
   }
   if (dotProduct < -1.0)
   {
      dotProduct = -1.0;
   }
   double angle_rad  = acos(dotProduct);
   aTargetDistance_m = gSphericalEarthRadius_m * angle_rad;
}

void P6DofUtils::CalcSphericalPositionFromLatLonAlt(const double aLat_deg,
                                                    const double aLon_deg,
                                                    const double aAlt_m,
                                                    UtVec3dX&    aPos_m)
{
   double r     = gSphericalEarthRadius_m + aAlt_m;
   double lat_r = aLat_deg * UtMath::cRAD_PER_DEG;
   double lon_r = aLon_deg * UtMath::cRAD_PER_DEG;
   double x     = r * cos(lat_r) * cos(lon_r);
   double y     = r * cos(lat_r) * sin(lon_r);
   double z     = r * sin(lat_r);
   aPos_m.Set(x, y, z);
}

void P6DofUtils::InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel)
{
   UtVec3dX vec;
   UtVec3dX result;
   UtVec3dX body;

   vec.Set(1.0, 0.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyX = result.DotProduct(aInertial);

   vec.Set(0.0, 1.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyY = result.DotProduct(aInertial);

   vec.Set(0.0, 0.0, 1.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyZ = result.DotProduct(aInertial);

   aBodyRel.Set(bodyX, bodyY, bodyZ);
}

double P6DofUtils::GetSphericalEarthRadius_m()
{
   return gSphericalEarthRadius_m;
}

void P6DofUtils::DCM_InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel)
{
   UtVec3dX vec;
   UtVec3dX result;
   UtVec3dX body;

   vec.Set(1.0, 0.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyX = result.DotProduct(aInertial);

   vec.Set(0.0, 1.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyY = result.DotProduct(aInertial);

   vec.Set(0.0, 0.0, 1.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyZ = result.DotProduct(aInertial);

   aBodyRel.Set(bodyX, bodyY, bodyZ);
}

void P6DofUtils::SetSphericalDCM(const double aLat, const double aLon, UtDCM& aDCM)
{
   UtDCM startDCM(0.0, -UtMath::cPI_OVER_2, 0.0);        // Pitch -90 deg
   UtDCM lonDCM(0.0, 0.0, aLon * UtMath::cRAD_PER_DEG);  // Roll according to lon angle
   UtDCM latDCM(0.0, -aLat * UtMath::cRAD_PER_DEG, 0.0); // Pitch according to lat angle

   aDCM = latDCM * lonDCM * startDCM;
}

void P6DofUtils::SetSphericalDCM(const double aLat,
                                 const double aLon,
                                 const double aYaw_rad,
                                 const double aPitch_rad,
                                 const double aRoll_rad,
                                 UtDCM&       aDCM)
{
   SetSphericalDCM(aLat, aLon, aDCM);

   // Now handle YPR
   UtDCM yawDCM(aYaw_rad, 0.0, 0.0);
   UtDCM pitchDCM(0.0, aPitch_rad, 0.0);
   UtDCM rollDCM(0.0, 0.0, aRoll_rad);
   aDCM = rollDCM * pitchDCM * yawDCM * aDCM;
}

void P6DofUtils::SetSphericalDCM(const UtVec3dX& aPos_m,
                                 const double    aHdg_rad,
                                 const double    aPitch_rad,
                                 const double    aRoll_rad,
                                 UtDCM&          aDCM)
{
   // Use pos to determine lat, lon, alt
   double lat_deg = 0.0;
   double lon_deg = 0.0;
   double alt_m   = 0.0;
   P6DofUtils::CalcSphericalLatLonAlt(aPos_m, lat_deg, lon_deg, alt_m);

   SetSphericalDCM(lat_deg, lon_deg, aHdg_rad, aPitch_rad, aRoll_rad, aDCM);
}

void P6DofUtils::GetSphericalNEDAngles(const UtDCM&    aDCM,
                                       const UtVec3dX& aPos,
                                       double&         aHdg_rad,
                                       double&         aPitch_rad,
                                       double&         aRoll_rad)
{
   // Calc local and inertial angles
   UtVec3dX unitX, unitY, unitZ;
   P6DofUtils::CalcUnitVectors(aDCM, unitX, unitY, unitZ);

   UtVec3dX localNorth, localEast, localDown;
   P6DofUtils::CalcLocalNEDVectors(aPos, localNorth, localEast, localDown);

   P6DofUtils::CalcLocalAngles(localNorth, localEast, localDown, unitX, unitZ, aHdg_rad, aPitch_rad, aRoll_rad);
}

UtVec3dX P6DofUtils::CalcWGS84VecToNED(const UtVec3dX& aWGS84Pos_m, const UtVec3dX& aVecWGS84_m)
{
   UtVec3dX vecNED(0., 0., 0.);

   UtEntity tempEntity;
   double   posVec[3];
   aWGS84Pos_m.Get(posVec);
   tempEntity.SetLocationWCS(posVec);
   double inertialVec[3];
   aVecWGS84_m.Get(inertialVec);
   tempEntity.SetVelocityWCS(inertialVec);
   double NEDVec[3];
   tempEntity.GetVelocityNED(NEDVec);
   vecNED.Set(NEDVec);

   return vecNED;
}

void P6DofUtils::CalcSphericalNEDVelToSphericalVel(const UtVec3dX& aPos, const UtVec3dX& aNEDVec, UtVec3dX& aSphericalVec)
{
   UtVec3dX localNorthVec;
   UtVec3dX localEastVec;
   UtVec3dX localDownVec;
   P6DofUtils::CalcLocalNEDVectors(aPos, localNorthVec, localEastVec, localDownVec);

   double nedVec[3];
   aNEDVec.Get(nedVec);
   localNorthVec = localNorthVec * nedVec[0];
   localEastVec  = localEastVec * nedVec[1];
   localDownVec  = localDownVec * nedVec[2];

   aSphericalVec = localNorthVec + localEastVec + localDownVec;
}

double P6DofUtils::CalcSphericalRangeToPoint_m(const UtVec3dX& aPos, const UtVec3dX& aPoint)
{
   // Find new pos on the surface
   UtVec3dX newPos_m(aPos);
   newPos_m.Normalize();

   // Find new point on the surface
   UtVec3dX newPoint_m(aPoint);
   newPoint_m.Normalize();

   double dotProduct = newPos_m.DotProduct(newPoint_m);
   if (dotProduct > 1.0)
   {
      dotProduct = 1.0;
   }
   if (dotProduct < -1.0)
   {
      dotProduct = -1.0;
   }
   double angle_rad = acos(dotProduct);
   double range_m   = P6DofUtils::GetSphericalEarthRadius_m() * angle_rad;

   return range_m;
}

double P6DofUtils::CalcSphericalHeadingToPoint_r(const UtVec3dX& aPos, const UtVec3dX& aPoint)
{
   // Find new pos on the surface
   UtVec3dX newPos_m(aPos);
   newPos_m.Normalize();
   newPos_m = newPos_m * P6DofUtils::GetSphericalEarthRadius_m();

   // Find new point on the surface
   UtVec3dX newPoint_m(aPoint);
   newPoint_m.Normalize();
   newPoint_m = newPoint_m * P6DofUtils::GetSphericalEarthRadius_m();

   UtVec3dX vectorToTarget = newPoint_m - newPos_m;
   vectorToTarget.Normalize();

   UtVec3dX northVec, eastVec, downVec;
   P6DofUtils::CalcLocalNEDVectors(newPos_m, northVec, eastVec, downVec);

   double northing  = vectorToTarget.Dot(northVec);
   double easting   = vectorToTarget.Dot(eastVec);
   double heading_r = atan2(easting, northing);

   return heading_r;
}

void P6DofUtils::AlphaBetaFromInertialVel(const UtDCM&    aMatrix,
                                          const UtVec3dX& aInertialVel_mps,
                                          double&         aSpeed_fps,
                                          double&         aAlpha_deg,
                                          double&         aBeta_deg)
{
   double vx, vy, vz;
   aInertialVel_mps.Get(vx, vy, vz);
   UtVec3dX inertial_mps(vx, vy, vz);
   UtVec3dX bodyRel_mps;
   DCM_InertialToBodyRel(aMatrix, inertial_mps, bodyRel_mps);

   UtVec3dX bodyRel_fps = bodyRel_mps * UtMath::cFT_PER_M;
   AlphaBetaFromBodyRelVel(bodyRel_fps, aAlpha_deg, aBeta_deg, aSpeed_fps);

   // Limit alpha/beta when velocity is small, to reduce "noise" when
   // stationary or nearly stationary
   if (aSpeed_fps < 0.1)
   {
      aAlpha_deg = 0.0;
      aBeta_deg  = 0.0;
   }
}

void P6DofUtils::AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg, double& aSpeed_fps)
{
   AlphaBetaFromBodyRelVel(aVel_fps, aAlpha_deg, aBeta_deg);
   aSpeed_fps = aVel_fps.Magnitude();
}

void P6DofUtils::AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg)
{
   double alpha_rad = 0.0;
   double beta_rad  = 0.0;

   double vx = 0.0;
   double vy = 0.0;
   double vz = 0.0;

   aVel_fps.Get(vx, vy, vz);

   if (aVel_fps.Magnitude() < std::numeric_limits<double>::epsilon())
   {
      aAlpha_deg = 0.0;
      aBeta_deg  = 0.0;
      return;
   }

   beta_rad = std::asin(vy / aVel_fps.Magnitude());
   // Special case: Velocity is only in Y direction. We don't have enough information
   // to determine alpha, so we set to -pi/2 or pi/2
   if (std::fabs(vx) < std::numeric_limits<double>::epsilon() && std::fabs(vz) < std::numeric_limits<double>::epsilon())
   {
      if (vy > 0)
      {
         alpha_rad = UtMath::cPI_OVER_2;
      }
      else
      {
         alpha_rad = -UtMath::cPI_OVER_2;
      }
   }
   // Special case: Velocity is only in Z direction. We don't have enough information
   // to determine alpha, so we set to -pi/2 or pi/2
   else if (std::fabs(vx) < std::numeric_limits<double>::epsilon() &&
            std::fabs(vy) < std::numeric_limits<double>::epsilon())
   {
      if (vz < 0)
      {
         alpha_rad = -UtMath::cPI_OVER_2;
      }
      else
      {
         alpha_rad = UtMath::cPI_OVER_2;
      }
   }
   else
   {
      alpha_rad = std::atan2(vz, vx);
   }

   aAlpha_deg = UtMath::cDEG_PER_RAD * alpha_rad;
   aBeta_deg  = UtMath::cDEG_PER_RAD * beta_rad;
}

void P6DofUtils::GetPositionFromLLA(const double aLat,
                                    const double aLon,
                                    const double aAlt_m,
                                    const bool   aUseSpericalCoords,
                                    UtVec3dX&    aPosition_m)
{
   if (aUseSpericalCoords)
   {
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(aLat, aLon, aAlt_m, aPosition_m);
   }
   else
   {
      UtEntity entity;
      entity.SetLocationLLA(aLat, aLon, aAlt_m);
      double posWCS_m[3];
      entity.GetLocationWCS(posWCS_m);
      aPosition_m.Set(posWCS_m);
   }
}

void P6DofUtils::GetVelocityFromVelNED(const double aNorthVel_mps,
                                       const double aEastVel_mps,
                                       const double aDownVel_mps,
                                       const bool   aUseSpericalCoords,
                                       UtVec3dX&    aPosition_m,
                                       UtVec3dX&    aVelocity_mps)
{
   if (aUseSpericalCoords)
   {
      UtVec3dX NEDVec(aNorthVel_mps, aEastVel_mps, aDownVel_mps);
      CalcSphericalNEDVelToSphericalVel(aPosition_m, NEDVec, aVelocity_mps);
   }
   else
   {
      UtEntity entity;
      double   wcsPos_m[3] = {aPosition_m.X(), aPosition_m.Y(), aPosition_m.Z()};
      entity.SetLocationWCS(wcsPos_m);
      double velNED_mps[3] = {aNorthVel_mps, aEastVel_mps, aDownVel_mps};
      entity.SetVelocityNED(velNED_mps);
      double velWCS_mps[3];
      entity.GetVelocityWCS(velWCS_mps);
      aVelocity_mps.Set(velWCS_mps);
   }
}

void P6DofUtils::GetLLAFromPosition(const UtVec3dX& aPosition_m,
                                    const bool      aUseSpericalCoords,
                                    double&         aLat,
                                    double&         aLon,
                                    double&         aAlt_m)
{
   if (aUseSpericalCoords)
   {
      P6DofUtils::CalcSphericalLatLonAlt(aPosition_m, aLat, aLon, aAlt_m);
   }
   else
   {
      UtEntity entity;
      double   wcsPos_m[3] = {aPosition_m.X(), aPosition_m.Y(), aPosition_m.Z()};
      entity.SetLocationWCS(wcsPos_m);
      entity.GetLocationLLA(aLat, aLon, aAlt_m);
   }
}

void P6DofUtils::LimitThrottleNormalized(double& aThrottle)
{
   aThrottle = UtMath::Limit(aThrottle, 0.0, 1.0);
}

void P6DofUtils::LimitThrottleAfterburner(double& aThrottle)
{
   aThrottle = UtMath::Limit(aThrottle, 0.0, 2.0);
}

// -------------------------------------------------------------------------------

P6DofMachParamDataTable::P6DofMachParamDataTable()
   : mCurrentMachParamDataPairPtr(nullptr)
{
}

P6DofMachParamDataTable::~P6DofMachParamDataTable()
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

P6DofMachParamDataTable::P6DofMachParamDataTable(const P6DofMachParamDataTable& aSrc)
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

P6DofMachParamDataTable* P6DofMachParamDataTable::Clone() const
{
   return new P6DofMachParamDataTable(*this);
}

void P6DofMachParamDataTable::CreateNewMach(double aMach)
{
   machParamDataListPair* machPair = new machParamDataListPair;
   machPair->mach                  = aMach;
   mMachParamDataList.push_back(machPair);

   mCurrentMachParamDataPairPtr = machPair;
}

void P6DofMachParamDataTable::AddNewParamDataPair(double aParameter, double aData)
{
   paramDataPair* alphaValue = new paramDataPair;
   alphaValue->parameter     = aParameter;
   alphaValue->value         = aData;

   mCurrentMachParamDataPairPtr->paramDataList.push_back(alphaValue);
}

// Note: this assumes mMachParamDataList, is sorted in ascended order
double P6DofMachParamDataTable::GetValueAtMachParam(double aMach, double aParameter) const
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

double P6DofMachParamDataTable::GetMinParameterAtMach(double aMach) const
{
   return GetParameterAtMach(aMach, true);
}

double P6DofMachParamDataTable::GetMaxParameterAtMach(double aMach) const
{
   return GetParameterAtMach(aMach, false);
}

double P6DofMachParamDataTable::GetMinDataAtMach(double aMach) const
{
   return GetDataAtMach(aMach, true);
}

double P6DofMachParamDataTable::GetMaxDataAtMach(double aMach) const
{
   return GetDataAtMach(aMach, false);
}

double P6DofMachParamDataTable::GetParameterAtMach(double aMach, bool aGetMin) const
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
         ut::log::error() << "Unable to get a valid result in P6DofMachParamDataTable::GetMinParameterAtMach().";
      }
   }

   return 0.0;
}

double P6DofMachParamDataTable::GetMinParameter(const machParamDataListPair* aMachParamDataListPair) const
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

double P6DofMachParamDataTable::GetMaxParameter(const machParamDataListPair* aMachParamDataListPair) const
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

double P6DofMachParamDataTable::GetDataAtMach(double aMach, bool aGetMin) const
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
         ut::log::error() << "Unable to get a valid result in P6DofMachParamDataTable::GetMinParameterAtMach().";
      }
   }

   return 0.0;
}

double P6DofMachParamDataTable::GetMinData(const machParamDataListPair* aMachParamDataListPair) const
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

double P6DofMachParamDataTable::GetMaxData(const machParamDataListPair* aMachParamDataListPair) const
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

double P6DofMachParamDataTable::GetValueAtParam(std::vector<paramDataPair*>& aList, double aParameter) const
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
void P6DofMachParamDataTable::OutputToString(std::string&       aInput,
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
      aInput      = aInput + "         mach  " + outputDoubleString(Mach, 5, 2) + "\n";

      std::stringstream parameterNameStr;
      parameterNameStr.width(8);
      parameterNameStr.setf(std::ios::left);
      parameterNameStr << aParameterType;

      //    aInput = aInput + "            alpha  ";
      //    aInput = aInput + "            " + aParameterType + "  ";
      aInput = aInput + "            " + parameterNameStr.str() + "  ";

      for (auto& paramData : iter->paramDataList)
      {
         aInput = aInput + "   " + outputDoubleString(paramData->parameter, 8, 3);
      }
      aInput = aInput + "\n";

      aInput = aInput + "            values    ";
      for (auto& paramData : iter->paramDataList)
      {
         aInput = aInput + "   " + outputDoubleString(paramData->value, 8, 3);
      }
      aInput = aInput + "\n";
   }

   aInput = aInput + "      end_irregular_table\n";
   aInput = aInput + "   end_" + aTableName + "\n\n";
}

// -------------------------------------------------------------------------------

P6DofMachDataTable::P6DofMachDataTable() {}

P6DofMachDataTable::~P6DofMachDataTable()
{
   for (auto& machIter : mMachDataList)
   {
      delete machIter;
   }
   mMachDataList.clear();
}

P6DofMachDataTable::P6DofMachDataTable(const P6DofMachDataTable& aSrc)
{
   for (auto& machIter : aSrc.mMachDataList)
   {
      CreateNewMachValuePair(machIter->mach, machIter->value);
   }
}

P6DofMachDataTable* P6DofMachDataTable::Clone() const
{
   return new P6DofMachDataTable(*this);
}

void P6DofMachDataTable::CreateNewMachValuePair(double aMach, double aValue)
{
   machDataPair* machPair = new machDataPair;
   machPair->mach         = aMach;
   machPair->value        = aValue;
   mMachDataList.push_back(machPair);
}

double P6DofMachDataTable::GetValueAtMach(double aMach) const
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

void P6DofMachDataTable::OutputToString(std::string& aInput, const std::string& aTableName) const
{
   aInput = aInput + "   " + aTableName + "\n";

   for (auto& iter : mMachDataList)
   {
      aInput = aInput + "      " + outputDoubleString(iter->mach, 7, 3) + "      " +
               outputDoubleString(iter->value, 8, 3) + "\n";
   }

   aInput = aInput + "   end_" + aTableName + "\n\n";
}

// -------------------------------------------------------------------------------

P6DofTableTool::P6DofTableTool(P6DofVehicle* aObject)
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

bool P6DofTableTool::CalculateStickForZeroMoment(double aMach, double aAlpha_rad, double& aStickBack)
{
   double tempThrustOut_lbs = 0.0;
   return CalculateStickForZeroMoment(aMach, aAlpha_rad, false, tempThrustOut_lbs, aStickBack);
}

bool P6DofTableTool::CalculateStickForZeroMoment(double  aMach,
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

   P6DofPropulsionSystem* propulsion = mObject->GetPropulsionSystem();
   P6DofPilotObject*      pilot      = mObject->GetPilotObject();

   if (propulsion)
   {
      P6DofScenario* scenario = mObject->GetScenario();
      if (scenario)
      {
         P6DofFreezeFlags* freezeFlags = scenario->GetFreezeFlags();
         if (freezeFlags)
         {
            preservedMasterNoLagTestingSetting = freezeFlags->GetMasterNoLagTesting();
            freezeFlags->SetMasterNoLagTesting(true);
         }
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
      P6DofScenario* scenario = mObject->GetScenario();
      if (scenario)
      {
         P6DofFreezeFlags* freezeFlags = scenario->GetFreezeFlags();
         if (freezeFlags)
         {
            freezeFlags->SetMasterNoLagTesting(preservedMasterNoLagTestingSetting);
         }
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

double P6DofTableTool::CalcCLFromCLArea(double aCLArea)
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


bool P6DofTableTool::CreateAutopilotTables()
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

void P6DofTableTool::AddAlphaVsMachCL(double aMach, double aCL, double aMinAlpha_deg, double aMaxAlpha_deg, bool& aMachAdded)
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

   { // RAII block
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

bool P6DofTableTool::CalcStickAndCL(double  aMach,
                                    double  aAlpha_rad,
                                    double& aStickBackBaseline,
                                    double& aStickBackThrustShift,
                                    double& aStickBackXCmShift,
                                    double& aStickBackZCmShift)
{
   // Setup pitch using alpha...
   mObject->SetAttitudeNED(0.0, aAlpha_rad, 0.0);

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

   P6DofVehicle* vehicle = mObject;
   if (vehicle && thrustProduced_lbs > 1.0)
   {
      P6DofMassProperties massProperties    = vehicle->GetMassProperties();
      double              referenceMass_lbs = massProperties.GetBaseMass_lbs();

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

int P6DofTableTool::CountValidStickAndCL(double aMach, std::vector<double> aAlphaVector_deg)
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

void P6DofTableTool::AddStickAndCL(double  aMach,
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

void P6DofTableTool::OutputCLMaxMachTableToInputString(std::string& aInput) const
{
   if (mCLMaxVsMachTable == nullptr)
   {
      return;
   }

   mCLMaxVsMachTable->OutputToString(aInput, "cl_max_mach_table");
}

void P6DofTableTool::OutputCLMinMachTableToInputString(std::string& aInput) const
{
   if (mCLMinVsMachTable == nullptr)
   {
      return;
   }

   mCLMinVsMachTable->OutputToString(aInput, "cl_min_mach_table");
}

void P6DofTableTool::OutputAlphaMaxMachTableToInputString(std::string& aInput) const
{
   if (mAlphaMax_degVsMachTable == nullptr)
   {
      return;
   }

   mAlphaMax_degVsMachTable->OutputToString(aInput, "alpha_max_mach_table");
}

void P6DofTableTool::OutputAlphaMinMachTableToInputString(std::string& aInput) const
{
   if (mAlphaMin_degVsMachTable == nullptr)
   {
      return;
   }

   mAlphaMin_degVsMachTable->OutputToString(aInput, "alpha_min_mach_table");
}

void P6DofTableTool::OutputAlphaMachCLTableToInputString(std::string& aInput) const
{
   if (mAlphaVsMachCLTable == nullptr)
   {
      return;
   }

   // aParameterType typically is "alpha" or "cl"
   // aParameterUnitsString typically is "units deg" or "precision float"
   mAlphaVsMachCLTable->OutputToString(aInput, "alpha_versus_mach_cl_table", "cl", "precision float");
}

void P6DofTableTool::OutputStickMachAlphaTablesToInputString(std::string& aInput) const
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

void P6DofTableTool::OutputCLMachAlphaTableToInputString(std::string& aInput) const
{
   if (mCLVsMachAlphaTable == nullptr)
   {
      return;
   }

   // aParameterType typically is "alpha" or "cl"
   // aParameterUnitsString typically is "units deg" or "precision float"
   mCLVsMachAlphaTable->OutputToString(aInput, "effective_CL_versus_mach_alpha_table", "alpha", "units deg");
}

void P6DofTableTool::InitializeTableMachValues(double aMaxMach)
{
   CreateMachTestList(aMaxMach);
}

void P6DofTableTool::CreateMachTestList(double aMaxMach)
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
void WritePIDBlock(std::ofstream&         aStream,
                   const std::string&     aBlockName,
                   const std::string&     aBlockWhiteSpace,
                   P6DOF::Pid::Type       aPid,
                   P6DofCommonController* aPilot)
{
   unsigned char                    flags;
   std::vector<P6DOF::PidGainData>* data = aPilot->GetAutopilotPidGainData(aPid, flags);

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

         if (flags & P6DOF::Pid::LimitMax)
         {
            aStream << localWhiteSpace << "max_error_accum " << d.MaxAccum << std::endl;
         }
         if (flags & P6DOF::Pid::UseAlpha)
         {
            aStream << localWhiteSpace << "low_pass_alpha " << d.LowpassAlpha << std::endl;
         }
         if (flags & P6DOF::Pid::ZeroGtMax)
         {
            aStream << localWhiteSpace << "ignore_large_error_accum " << d.MaxErrorZero << std::endl;
         }
         if (flags & P6DOF::Pid::ZeroLtMin)
         {
            aStream << localWhiteSpace << "ignore_small_error_accum " << d.MinErrorZero << std::endl;
         }
         if (flags & P6DOF::Pid::UseKt)
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

void P6DofTableTool::Initialize()
{
   // Create CLMax table
   mCLMaxVsMachTable = ut::make_unique<P6DofMachDataTable>();

   // Create CLMin tables
   mCLMinVsMachTable = ut::make_unique<P6DofMachDataTable>();

   // Create AlphaMax table
   mAlphaMax_degVsMachTable = ut::make_unique<P6DofMachDataTable>();

   // Create AlphaMin table
   mAlphaMin_degVsMachTable = ut::make_unique<P6DofMachDataTable>();

   // Create StickForZeroMoment table
   mStickForZeroMomentVsMachAlphaTablePtr            = ut::make_unique<P6DofMachParamDataTable>();
   mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr = ut::make_unique<P6DofMachParamDataTable>();
   mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr    = ut::make_unique<P6DofMachParamDataTable>();
   mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr    = ut::make_unique<P6DofMachParamDataTable>();

   // Create CLVsMachAlpha table
   mCLVsMachAlphaTable = ut::make_unique<P6DofMachParamDataTable>();

   // Create AlphaVsMachCL table
   mAlphaVsMachCLTable = ut::make_unique<P6DofMachParamDataTable>();

   // We need to setup test conditions...
   mObject->SetLocationLLA(0.0, 0.0, 100.0);
   mObject->SetVelocityNED(100.0, 0.0, 0.0);
   mObject->SetAttitudeNED(0.0, 0.0, 0.0);
}

bool P6DofTableTool::CreateCLMaxAndAlphaMaxTables()
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
         mObject->SetAttitudeNED(0.0, alpha_rad, 0.0);

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

         { // RAII block
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

bool P6DofTableTool::CreateCLMinAndAlphaMinTables()
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
         mObject->SetAttitudeNED(0.0, alpha_rad, 0.0);

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

bool P6DofTableTool::CreateStickForZeroMomentTable()
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

bool P6DofTableTool::CreateAlphaVsMachCLTable()
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
