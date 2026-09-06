// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ScriptGeneratorSixDOF.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>
#include <QDate>
#include <QFile>
#include <QString>

#include "AeroEngine.hpp"
#include "AeroLandingGear.hpp"
#include "AeroSolidRocketEngine.hpp"
#include "AeroSpeedBrake.hpp"
#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"
#include "ControlsWidget.hpp"
#include "EngineDesignerWidget.hpp"
#include "EngineWidget.hpp"
#include "GeometryEngine.hpp"
#include "G_LimitSettings.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "UtLog.hpp"
#include "Vehicle.hpp"
#include "VehicleAero.hpp"
#include "VehicleAeroCore.hpp"
#include "VehicleAeroMovable.hpp"
#include "VehiclePropulsion.hpp"

namespace Designer
{

void ScriptGeneratorSixDOF::CreateFolderStructure()
{
   QString fullPath = mVehicleDir.absolutePath();

   // Create main folder
   if (!QDir(fullPath).exists())
   {
      QDir().mkdir(fullPath);
   }

   CreateFolderIfDoesntExist(fullPath + "/platforms");
   CreateFolderIfDoesntExist(fullPath + "/model_3d");
   CreateFolderIfDoesntExist(fullPath + "/six_dof_types");
   CreateFolderIfDoesntExist(fullPath + "/six_dof_types/environment");
   CreateFolderIfDoesntExist(fullPath + "/six_dof_types/scripts");

   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString newFullPath;

   if (mVehiclePtr->IsAircraft())
   {
      CreateFolderIfDoesntExist(fullPath + "/six_dof_types/aircraft");
      newFullPath = mVehicleDir.absolutePath() + "/six_dof_types/aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      CreateFolderIfDoesntExist(fullPath + "/six_dof_types/weapons");
      newFullPath = mVehicleDir.absolutePath() + "/six_dof_types/weapons";
   }

   QString lowercaseVehicleName = mVehicleName.toLower();

   newFullPath = newFullPath + "/" + lowercaseVehicleName;
   CreateFolderIfDoesntExist(newFullPath);

   CreateFolderIfDoesntExist(newFullPath + "/aero");
   CreateFolderIfDoesntExist(newFullPath + "/aero/data");
   CreateFolderIfDoesntExist(newFullPath + "/prop");
   if (mVehiclePtr->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL)
   {
      CreateFolderIfDoesntExist(newFullPath + "/controls");
   }
   CreateFolderIfDoesntExist(fullPath + "/tests");
   CreateFolderIfDoesntExist(mTestDir.absolutePath());
   CreateFolderIfDoesntExist(fullPath + "/output");
   CreateFolderIfDoesntExist(mOutputDir.absolutePath());

}

void ScriptGeneratorSixDOF::CreateBaseScenarioFile()
{
   QFile baseFile(mTestDir.absolutePath() + "/" + mVehicleName.toLower() + "_scenario.txt");

   if (!baseFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&baseFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This scenario file is used for testing the vehicle within AFSIM. It can be used for autopilot\n";
   outStream << "# tuning and flight testing within the Mover Creator tool development environment.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "# Set a start time that has a daytime sun position for our test in the Oahu region\n";
   outStream << "start_epoch 2018354.75   # Southern Solstice 2018\n";
   outStream << "\n";
   outStream << "# Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "# Provide plenty of runtime, to prevent the scenario from ending before testing is complete.\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";

   outStream << "# Setup the test vehicle\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "platform  TestAicraft  " << mPlatformType << "\n";
      outStream << "\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      outStream << "platform  TestWeapon  " << mPlatformType << "\n";
      outStream << "\n";
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   std::string side = "green";
   std::string icon = mVehiclePtr->GetVehicleName();

   outStream << "   side  ";
   OutputData(outStream, QString(side.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Neutral force color\n";
   outStream << "   icon  ";
   OutputData(outStream, QString(icon.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Icon based on geometry\n";
   outStream << "\n";

   double mach        = mVehiclePtr->GetTestingMach();
   double altitude_ft = mVehiclePtr->GetVehicleAltitude_ft();
   double altitude_m  = altitude_ft * UtMath::cM_PER_FT;

   UtAtmosphere atm = mVehiclePtr->GetAtmosphere();
   double ktas = atm.CalcKtasFromMach(altitude_m, mach);


   outStream << "   # Setup a northbound test route roughly 10nm south of Pearl Harbor in the Pacific\n";
   outStream << "   route\n";

   outStream << "      position   21.1888n 157.9534w   altitude ";
   OutputData(outStream, altitude_ft, 9, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " ft   speed ";
   OutputData(outStream, ktas, 7, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " kts\n";

   outStream << "      position   21.3618n 157.9534w   altitude ";
   OutputData(outStream, altitude_ft, 9, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " ft   speed ";
   OutputData(outStream, ktas, 7, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " kts\n";

   outStream << "      position   21.5348n 157.9534w   altitude ";
   OutputData(outStream, altitude_ft, 9, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " ft   speed ";
   OutputData(outStream, ktas, 7, 1, QTextStream::FieldAlignment::AlignRight);
   outStream << " kts\n";

   outStream << "   end_route\n";
   outStream << "\n";
   outStream << "   # Climb/dive uses on slope between points\n";
   outStream << "   follow_vertical_track\n";
   outStream << "\n";

   outStream << "end_platform\n";
   baseFile.close();
}

void ScriptGeneratorSixDOF::CreatePlatformFile()
{
   QString platformDir = mVehicleDir.absolutePath() + "/platforms";
   QFile platformFile(platformDir + "/" + mPlatformName + ".txt");
   if (!platformFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&platformFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "include_once ../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "include_once ../six_dof_types/aircraft/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      outStream << "include_once ../six_dof_types/weapons/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   outStream << "\n";
   outStream << "platform_type  " << mPlatformType << "  WSF_PLATFORM\n";
   outStream << "\n";

   std::string domain = "air";
   outStream << "   spatial_domain  ";
   OutputData(outStream, QString(domain.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # This vehicle operates in the air domain\n";

   std::string side = "green";
   std::string icon = mVehiclePtr->GetVehicleName();

   outStream << "   side            ";
   OutputData(outStream, QString(side.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Neutral force color\n";
   outStream << "   icon            ";
   OutputData(outStream, QString(icon.c_str()), 32, QTextStream::FieldAlignment::AlignLeft);
   outStream << "   # Icon based on geometry\n";
   outStream << "\n";
   outStream << "   # Setup the mover\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "\n";

      // TODO: This section below can be restored to provide an SA Processor, if needed
      // outStream << "   # processor situational_awareness WSF_SA_PROCESSOR \n";
      // outStream << "   #   on\n";
      // outStream << "   #   update_interval      1.0 sec\n";
      // outStream << "   #\n";
      // outStream << "   #   # Data collection rates\n";
      // outStream << "   #   report_interval      5.0 sec\n";
      // outStream << "   # end_processor\n";
      // outStream << "\n";

   outStream << "end_platform_type\n";
   platformFile.close();
}

void ScriptGeneratorSixDOF::CreateAdditionalFiles()
{
   CreateBaseScenarioFile();
   CreateEnvironmentFiles();
   CreateSignaturesFile();
   CreateMoverFile();
   CreatePropulsionFile();
}

void ScriptGeneratorSixDOF::CreateMoverFile()
{
   if (!mVehiclePtr->IsAircraft() && !mVehiclePtr->IsWeapon())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   QString moverFolderPath = mVehicleDir.absolutePath() + "/six_dof_types/" + mVehicleType + "/" + mVehicleName.toLower();
   QString moverPath = moverFolderPath + "/" + mVehicleName.toLower() + GetFilenameSuffix() + ".txt";
   QFile moverFile(moverPath);
   if (!moverFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream(&moverFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();

   CreateAutogenHeader(mVehiclePtr, outStream);

   std::vector<QString> engineIncludeList;

   // Include all of the engines, but list each type only once
   for (auto curEngine : mEngineList)
   {
      QString engineModel = QString::fromStdString(curEngine->GetModelName()).toLower();
      QString engineType = QString();
      switch (curEngine->GetEngineType())
      {
         case GeometryEngine::EngineType::cJET:
            engineType = "jet_engines";
            break;
         case GeometryEngine::EngineType::cRAMJET:
            engineType = "ramjets";
            break;
         case GeometryEngine::EngineType::cLIQUIDROCKET:
            engineType = "liquid_propellant_rockets";
            break;
         case GeometryEngine::EngineType::cSOLIDROCKET:
            engineType = "solid_propellant_rockets";
            break;
      }

      // Set the temporary string for this engine
      QString tempString = "include_once prop/" + engineType + "/" + engineModel + "/" + engineModel + GetFilenameSuffix() + ".txt";

      // Test to see if the current string is already in the list
      bool matchFound = std::any_of(engineIncludeList.begin(), engineIncludeList.end(),
      [ = ](const QString & engineInclude) {return engineInclude == tempString; });

      // If not found in the list, add it to the list
      if (!matchFound)
      {
         engineIncludeList.emplace_back(tempString);
      }
   }

   // Output the unique engine include strings
   for (auto& tmp : engineIncludeList)
   {
      outStream << tmp << "\n";
   }

   outStream << "\n";
   outStream << "six_dof_object_types\n";
   outStream << "   rigid_body_vehicle_type " << mVehicleName.toUpper() << " BASE_TYPE\n\n";
   outStream << "      include aero/aero" + GetFilenameSuffix() + ".txt                 # Defines the vehicle aerodynamics\n";

   if (mVehiclePtr->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL)
   {
      outStream << "      include controls/control_inputs" + GetFilenameSuffix() + ".txt   # Defines the control inputs (stick, rudder, throttle, etc)\n";
      outStream << "      include controls/flight_controls" + GetFilenameSuffix() + ".txt  # Defines how the control inputs move the control surfaces\n";
   }
   outStream << "\n";

   int width = 16;
   int precision = 5;
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   outStream << "      mass_properties\n";
   outStream << "         mass                       ";
   OutputData(outStream, massProperties->GetEmptyMass_lbs(), width, precision, alignment);
   outStream << " lbs\n";
   outStream << "         moment_of_inertia_ixx      ";
   OutputData(outStream, massProperties->GetIxx(), width, precision, alignment);
   outStream << " slug*ft^2\n";
   outStream << "         moment_of_inertia_iyy      ";
   OutputData(outStream, massProperties->GetIyy(), width, precision, alignment);
   outStream << " slug*ft^2\n";
   outStream << "         moment_of_inertia_izz      ";
   OutputData(outStream, massProperties->GetIzz(), width, precision, alignment);
   outStream << " slug*ft^2\n";
   outStream << "         center_of_mass_x           ";
   OutputData(outStream, massProperties->GetCgX(), width, precision, alignment);
   outStream << " ft\n";
   outStream << "         center_of_mass_y           ";
   OutputData(outStream, massProperties->GetCgY(), width, precision, alignment);
   outStream << " ft\n";
   outStream << "         center_of_mass_z           ";
   OutputData(outStream, massProperties->GetCgZ(), width, precision, alignment);
   outStream << " ft\n";
   outStream << "      end_mass_properties\n\n";

   const UtVec3dX originalCgLocation_ft = massProperties->GetOriginalCg();
   if (originalCgLocation_ft.Magnitude() > 0.0)
   {
      outStream << "         # Original center of mass, for load and balance reference only.\n";
      outStream << "         # Leave these lines commented. Use of this c.m. within P6DOF is incorrect.\n";
      outStream << "         # center_of_mass_x           ";
      OutputData(outStream, originalCgLocation_ft.X(), width, precision, alignment);
      outStream << " ft\n";
      outStream << "         # center_of_mass_y           ";
      OutputData(outStream, originalCgLocation_ft.Y(), width, precision, alignment);
      outStream << " ft\n";
      outStream << "         # center_of_mass_z           ";
      OutputData(outStream, originalCgLocation_ft.Z(), width, precision, alignment);
      outStream << " ft\n";
   }

   // Vehicles should include a propulsion system definition, even if no engines/fuel are present

   VehiclePropulsion* propulsion = mVehiclePtr->GetPropulsion();

   if (propulsion == nullptr)
   {
      outStream << "      # Note: This vehicle lacks any propulsion system\n";
      outStream << "\n";
   }
   else
   {
      GeometryPropulsionData* propData = vehicleGeometry.GetPropulsionData();
      bool fuelTankPresent = false;
      bool solidRocketPropellantPresent = false;

      if (propData != nullptr)
      {
         if (propData->FuelTankPresent())
         {
            fuelTankPresent = true;
         }
      }

      // If we lack a fuel tank and any engines, we lack propulsion
      if (mEngineList.empty() && !fuelTankPresent)
      {
         outStream << "      # Note: This vehicle lacks any propulsion system\n";
         outStream << "\n";
      }
      else
      {
         // We have a propulsion system
         outStream << "      propulsion_data\n";
         outStream << "\n";

         bool hasJetEngine = false;
         if (!mEngineList.empty())
         {
            // Loop through and add each engine
            for (auto curEngine : mEngineList)
            {
               if (curEngine->GetEngineType() == GeometryEngine::EngineType::cJET) { hasJetEngine = true; }

               QString curEngineName = QString::fromStdString(curEngine->GetName()).replace(" ", "_");

               outStream << "         engine " << curEngineName << "  " << QString::fromStdString(curEngine->GetModelName()).toUpper() << "\n";
               outStream << "            rel_pos_x               ";
               OutputData(outStream, curEngine->GetRefPoint_ft().X(), width, precision, alignment);
               outStream << " ft\n";
               outStream << "            rel_pos_y               ";
               OutputData(outStream, curEngine->GetRefPoint_ft().Y(), width, precision, alignment);
               outStream << " ft\n";
               outStream << "            rel_pos_z               ";
               OutputData(outStream, curEngine->GetRefPoint_ft().Z(), width, precision, alignment);
               outStream << " ft\n";
               outStream << "            rel_yaw                 ";
               OutputData(outStream, curEngine->GetYaw_deg(), width, precision, alignment);
               outStream << " deg\n";
               outStream << "            rel_pitch               ";
               OutputData(outStream, curEngine->GetPitch_deg(), width, precision, alignment);
               outStream << " deg\n";

               outStream << "            fuel_feed               ";

               // SixDOF handles each solid rocket engine and its propellant as an engine and fuel tank
               if (curEngine->GetEngineType() == GeometryEngine::EngineType::cSOLIDROCKET) 
               { 
                  solidRocketPropellantPresent = true;
                  OutputData(outStream, QString(curEngineName + "_Propellant"), width, alignment);
               }
               else
               {
                  OutputData(outStream, QString("MainFuelTank"), width, alignment);
               }
               outStream << "\n";

               outStream << "         end_engine\n";
               outStream << "\n";
            }
         }

         // Fuel tank(s)
         if (fuelTankPresent)
         {
            outStream << "         # Current fuel tank support is limited to a single tank.\n";
            outStream << "         # Future Mover Creator upgrades may support more complex, multi-tank fuel systems.\n";
            outStream << "\n";

            double fuel_max = propData->GetFuelQtyMax();
            double fuel_current = propData->GetFuelQtyCurrent();

            outStream << "         fuel_tank  MainFuelTank\n";
            outStream << "            max_fuel_quantity       ";
            OutputData(outStream, fuel_max, width, precision, alignment);
            outStream << " lbs\n";
            outStream << "            current_fuel_quantity   ";
            OutputData(outStream, fuel_current, width, precision, alignment);
            outStream << " lbs\n";
            outStream << "            max_flow_rate           ";
            OutputData(outStream, 1000000.0, width, precision, alignment);
            outStream << " lbs/sec\n";
            outStream << "            max_fill_rate           ";
            OutputData(outStream, 1000000.0, width, precision, alignment);
            outStream << " lbs/sec\n";
            outStream << "            max_xfer_rate           ";
            OutputData(outStream, 1000000.0, width, precision, alignment);
            outStream << " lbs/sec\n";

            UtVec3dX fuelCg_ft(0.0, 0.0, 0.0);
            propData->GetCG_ft(fuelCg_ft);
            double cgX = fuelCg_ft.X();
            double cgY = fuelCg_ft.Y();
            double cgZ = fuelCg_ft.Z();

            outStream << "            cg_full_x               ";
            OutputData(outStream, cgX, width, precision, alignment);
            outStream << " ft\n";
            outStream << "            cg_full_y               ";
            OutputData(outStream, cgY, width, precision, alignment);
            outStream << " ft\n";
            outStream << "            cg_full_z               ";
            OutputData(outStream, cgZ, width, precision, alignment);
            outStream << " ft\n";
            outStream << "            cg_empty_x              ";
            OutputData(outStream, cgX, width, precision, alignment);
            outStream << " ft\n";
            outStream << "            cg_empty_y              ";
            OutputData(outStream, cgY, width, precision, alignment);
            outStream << " ft\n";
            outStream << "            cg_empty_z              ";
            OutputData(outStream, cgZ, width, precision, alignment);
            outStream << " ft\n";

            outStream << "         end_fuel_tank\n";
            outStream << "\n";
         }
         else if (solidRocketPropellantPresent)
         {
            for (auto& object : vehicleGeometry.GetGeometryObjectMap())
            {
               GeometryEngine* engine = dynamic_cast<GeometryEngine*>(object.second);
               if (engine != nullptr)
               {
                  if (engine->GetEngineType() == GeometryEngine::EngineType::cSOLIDROCKET)
                  {
                     outStream << "         # Current fuel tank support is limited to a single tank.\n";
                     outStream << "         # Future Mover Creator upgrades may support more complex, multi-tank fuel systems.\n";
                     outStream << "\n";

                     UtVec3dX propellantCG_ft;
                     UtVec3dX propellantInertia;
                     double propellantMass_lbs = 0.0;
                     engine->GetCalculatedPropellantMassProperties(propellantCG_ft, propellantMass_lbs, propellantInertia);

                     double fuel_max = engine->GetPropellantMass_lbs();
                     double fuel_current = engine->GetPropellantMass_lbs();

                     outStream << "         fuel_tank  " << QString::fromStdString(engine->GetName()) << "_Propellant" << endl;
                     outStream << "            max_fuel_quantity       ";
                     OutputData(outStream, fuel_max, width, precision, alignment);
                     outStream << " lbs" << endl;
                     outStream << "            current_fuel_quantity   ";
                     OutputData(outStream, fuel_current, width, precision, alignment);
                     outStream << " lbs" << endl;
                     outStream << "            max_flow_rate           ";
                     OutputData(outStream, 1000000.0, width, precision, alignment);
                     outStream << " lbs/sec" << endl;
                     outStream << "            max_fill_rate           ";
                     OutputData(outStream, 1000000.0, width, precision, alignment);
                     outStream << " lbs/sec" << endl;
                     outStream << "            max_xfer_rate           ";
                     OutputData(outStream, 1000000.0, width, precision, alignment);
                     outStream << " lbs/sec" << endl;

                     double cgX = propellantCG_ft.X();
                     double cgY = propellantCG_ft.Y();
                     double cgZ = propellantCG_ft.Z();

                     outStream << "            cg_full_x               ";
                     OutputData(outStream, cgX, width, precision, alignment);
                     outStream << " ft" << endl;
                     outStream << "            cg_full_y               ";
                     OutputData(outStream, cgY, width, precision, alignment);
                     outStream << " ft" << endl;
                     outStream << "            cg_full_z               ";
                     OutputData(outStream, cgZ, width, precision, alignment);
                     outStream << " ft" << endl;
                     outStream << "            cg_empty_x              ";
                     OutputData(outStream, cgX, width, precision, alignment);
                     outStream << " ft" << endl;
                     outStream << "            cg_empty_y              ";
                     OutputData(outStream, cgY, width, precision, alignment);
                     outStream << " ft" << endl;
                     outStream << "            cg_empty_z              ";
                     OutputData(outStream, cgZ, width, precision, alignment);
                     outStream << " ft" << endl;

                     outStream << "         end_fuel_tank" << endl << endl;
                  }
               }
            }
         }
         else
         {
            outStream << "         # This vehicle lacks a fuel tank\n";
            outStream << "\n";
         }

         if (!mEngineList.empty())
         {
            outStream << "         # Throttle controls:\n";
            outStream << "         throttle_setting_mil         ThrottleMilitary\n";
            if (hasJetEngine)
            {
               outStream << "         throttle_setting_ab          ThrottleAfterburner\n";
               outStream << "         throttle_setting_reverser    ThrottleThrustReverser\n";
            }
            outStream << "\n";

            QString enableThrustVectoringString;
            if (propData->GetEnableThrustVectoringString(enableThrustVectoringString))
            {
               outStream << "         # Thrust vectoring controls:\n";

               if (enableThrustVectoringString.contains("Yaw"))
               {
                  outStream << "         throttle_setting_yaw         ThrustVectoringYaw\n";
               }
               else
               {
                  outStream << "         #throttle_setting_yaw        ThrustVectoringYaw\n";
               }

               if (enableThrustVectoringString.contains("Pitch"))
               {
                  outStream << "         throttle_setting_pitch       ThrustVectoringPitch\n";
               }
               else
               {
                  outStream << "         #throttle_setting_pitch      ThrustVectoringPitch\n";
               }
               outStream << "\n";
            }
         }

         outStream << "      end_propulsion_data\n\n";
      }
   }

   // If the vehicle is a weapon and has an engine, include a sequencer to "ignite" the engine(s)
   if (mVehiclePtr->IsWeapon())
   {
      if (!mEngineList.empty())
      {
         outStream << "      # This sequencer will ignite/start the engine(s)\n";
         outStream << "      sequencer ReleaseFromVehicle\n";
         outStream << "         event_released_from_parent\n";
         for (const auto& engine : mEngineList)
         {
            QString engineName = QString::fromStdString(engine->GetName());
            double ignitionDelay = engine->GetIgnitionDelay_sec();
            if (ignitionDelay != 0)
            {
               outStream << "         sequencer Ignite_" << engineName << "\n";
               outStream << "            event_timer " << ignitionDelay << " sec\n";
               outStream << "            action_ignite_engine " << engineName << "\n";
               outStream << "         end_sequencer\n";
            }
            else
            {
               outStream << "         action_ignite_engine " << engineName << "\n";
            }

         }
         outStream << "      end_sequencer\n";
      }
   }

   outStream << "   end_rigid_body_vehicle_type\n";
   outStream << "end_six_dof_object_types\n";
   moverFile.close();

   QString aeroFolderPath = moverFolderPath + "/aero";
   CreateAeroFile(aeroFolderPath);
   if (mVehiclePtr->GetVehicleControlConfiguration() != Vehicle::cNO_CONTROL)
   {
      QString controlsFolderPath = moverFolderPath + "/controls";
      CreateControlsInputFile(controlsFolderPath);
      CreateFlightControlsFile(controlsFolderPath);
      CreateStubAutopilotSupportFile(controlsFolderPath);
   }
}

void ScriptGeneratorSixDOF::CreatePropulsionFile()
{
   QString propulsionFolderPath = mVehicleDir.absolutePath() + "/six_dof_types/" + mVehicleType + "/" + mVehicleName.toLower() + "/prop";
   QString enginePath = propulsionFolderPath + "/" + mVehicleName.toLower() + ".txt";

   // For each engine, create a directory and engine file
   for (auto curEngine : mEngineList)
   {
      QString engineType = QString();
      switch (curEngine->GetEngineType())
      {
         case GeometryEngine::EngineType::cJET:
         {
            engineType = "jet_engines";
            break;
         }
         case GeometryEngine::EngineType::cRAMJET:
         {
            engineType = "ramjets";
            break;
         }
         case GeometryEngine::EngineType::cLIQUIDROCKET:
         {
            engineType = "liquid_propellant_rockets";
            break;
         }
         case GeometryEngine::EngineType::cSOLIDROCKET:
         {
            engineType = "solid_propellant_rockets";
            break;
         }
      }

      // Create engine folder and related parent folders
      CreateFolderIfDoesntExist(propulsionFolderPath);
      QString engineTypePath = propulsionFolderPath + "/" + engineType;
      CreateFolderIfDoesntExist(engineTypePath);
      QString engineName = QString::fromStdString(curEngine->GetModelName());
      QString engineFolderPath = propulsionFolderPath + "/" + engineType + "/" + engineName.toLower();
      CreateFolderIfDoesntExist(engineFolderPath);

      // Output the engine file
      QString engineFilename = engineFolderPath + "/" + engineName.toLower() + GetFilenameSuffix() + ".txt";

      curEngine->Export(engineFilename.toStdString(), GetMoverType());
   }
}

void ScriptGeneratorSixDOF::CreateSignaturesFile()
{
   QString signaturesDir = mVehicleDir.absolutePath() + "/signatures";
   QFile signatureFile(signaturesDir + "/simple_signatures.txt");
   if (!signatureFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&signatureFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# This file includes various \"simple\" signatures (radar, ir, and optical)\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_RCS which is a 5 square meter target, typical for a fighter-sized aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_IR_SIG which is a 200 watts/steradian infrared target, typical for\n";
   outStream << "#      non-afterburning aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_IR_SIG_AFTERBURNER which is a 1000 watts/steradian infrared target, typical for\n";
   outStream << "#      afterburning aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_FIGHTER_OPTICAL_SIG which is a 3 square meter optical target, typical for a fighter-sized\n";
   outStream << "#      aircraft\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_RCS which is a 1 square meter target, typical for weapons\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_IR_SIG which is a 100 watts/steradian infrared target, typical for\n";
   outStream << "#      a weapon that is not under propulsion (the rocket is not burning)\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_IR_SIG_THRUSTING which is a 1000 watts/steradian infrared target, typical for\n";
   outStream << "#       a weapon that is under propulsion (the rocket is burning)\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_WEAPON_OPTICAL_SIG which is a 1 square meter optical target, typical for a weapon\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_1M_RCS which is a 1 square meter target\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_100WSR_IR_SIG which is a 100 watts/steradian infrared target\n";
   outStream << "#\n";
   outStream << "#   SIMPLE_1M_OPTICAL_SIG which is a 1 square meter optical target\n";
   outStream << "#\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# Radar cross sections\n";
   outStream << "\n";
   outStream << "   radar_signature SIMPLE_FIGHTER_RCS\n";
   outStream << "      constant 5 m^2\n";
   outStream << "   end_radar_signature\n";
   outStream << "\n";
   outStream << "   radar_signature SIMPLE_WEAPON_RCS\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_radar_signature\n";
   outStream << "\n";
   outStream << "   radar_signature SIMPLE_1M_RCS\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_radar_signature\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# Infrared signatures\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_FIGHTER_IR_SIG\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 200 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_FIGHTER_IR_SIG_AFTERBURNER\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 1000 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_WEAPON_IR_SIG\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 100 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_WEAPON_IR_SIG_THRUSTING\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 1000 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "   infrared_signature SIMPLE_100WSR_IR_SIG\n";
   outStream << "      state default\n";
   outStream << "      band default\n";
   outStream << "      constant 100 w/sr\n";
   outStream << "   end_infrared_signature\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "# Optical signatures\n";
   outStream << "\n";
   outStream << "   optical_signature SIMPLE_FIGHTER_OPTICAL_SIG\n";
   outStream << "      constant 3 m^2\n";
   outStream << "   end_optical_signature\n";
   outStream << "\n";
   outStream << "   optical_signature SIMPLE_WEAPON_OPTICAL_SIG\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_optical_signature\n";
   outStream << "\n";
   outStream << "   optical_signature SIMPLE_1M_OPTICAL_SIG\n";
   outStream << "      constant 1 m^2\n";
   outStream << "   end_optical_signature\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";

   signatureFile.close();
}

void ScriptGeneratorSixDOF::CreateTestFiles()
{
   PerformanceTestType performanceTestType = GetPerformanceTestType();
   QString testDir = mTestDir.absolutePath();
   if (performanceTestType == PerformanceTestType::cBANK_TO_TURN ||
       performanceTestType == PerformanceTestType::cSKID_TO_TURN)
   {
      CreateAutopilotSupportGeneratorFile(testDir);
   }

   CreatePerformanceTestFiles(testDir);
}

const QString ScriptGeneratorSixDOF::CreateAutopilotTestFile(double  aLatitude,
                                                             double  aLongitude,
                                                             double  aAltitude,
                                                             double  aHeading,
                                                             double  aSpeed)
{
   QString testDir = mTestDir.absolutePath();
   QString testFile = testDir + "/autopilot_tuning_scenario.txt";
   QFile genFile(testFile);
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return "";
   }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This file is used for tuning the P6DOF autopilot using Warlock for a vehicle that was created\n";
   outStream << "# by the AFSIM Mover Creator tool.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "end_time 365 days           # Use a long time to provide sufficient time for any amount of tuning/testing\n";
   outStream << "minimum_mover_timestep 1 ms # Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "\n";
   outStream << "platform  six_dof_test_platform  " << mPlatformType << "\n";
   outStream << "\n";

   double heading_rad = aHeading * UtMath::cRAD_PER_DEG;
   double vel_fps = aSpeed * UtMath::cFT_PER_M;
   double north_fps = cos(heading_rad) * vel_fps;
   double east_fps = sin(heading_rad) * vel_fps;

   double heading_deg = aHeading;
   while (heading_deg < 0.0)
   {
      heading_deg += 360.0;
   }
   while (heading_deg > 360.0)
   {
      heading_deg -= 360.0;
   }

   outStream << "   six_dof_set_velocity_ned_fps  " << north_fps << "  " << east_fps << "  0\n";
   outStream << "   six_dof_position              " << aLatitude << "  " << aLongitude << "\n";
   outStream << "   six_dof_alt                   " << aAltitude* UtMath::cFT_PER_M << " ft\n";
   outStream << "   six_dof_ned_heading           " << heading_deg << " deg\n";
   outStream << "\n";

   outStream << "end_platform\n";
   outStream << "\n";

   genFile.close();

   return testFile;
}

void ScriptGeneratorSixDOF::CreateEnvironmentFiles()
{
   QString pathAddition = "/six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt";
   QFile baseFile1(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile1.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream1(&baseFile1);
   outStream1.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream1);
   outStream1 << "# --------------------------------------------------------------------------------------------------\n";
   outStream1 << "# This provides a definition of various environmental components used by the P6DOF model.\n";
   outStream1 << "# It includes atmosphere, wind, terrain, and gravity.\n";
   outStream1 << "#\n";
   outStream1 << "# Atmosphere provides data pertaining to air density, pressure, temperature, etc.\n";
   outStream1 << "#\n";
   outStream1 << "# Wind provides a means for location-dependent wind, providing a means for wind to vary as\n";
   outStream1 << "# a function of lat/lon and/or altitude.\n";
   outStream1 << "#\n";
   outStream1 << "# Terrain provides the altitude of terrain and/or other objects on which aircraft may land\n";
   outStream1 << "# or with which an aircraft may collide. It also provides a surface normal and friction\n";
   outStream1 << "# modifier. Moving objects(such as the deck of an aircraft carrier) will also provide\n";
   outStream1 << "# their velocity.\n";
   outStream1 << "#\n";
   outStream1 << "# Gravity provides a means for detailed gravity models as a function of lat/lon and altitude.\n";
   outStream1 << "# --------------------------------------------------------------------------------------------------\n";
   outStream1 << "\n";
   outStream1 << "six_dof_object_types\n";
   outStream1 << "   integrators  integrators_config" + GetFilenameSuffix() + ".txt\n";
   outStream1 << "end_six_dof_object_types\n";
   outStream1 << "\n";
   outStream1 << "six_dof_environment\n";
   outStream1 << "   terrain            terrain_config.txt\n";
   outStream1 << "end_six_dof_environment\n";

   outStream1 << "atmosphere_table\n";
   outStream1 << "   0.0       288.15        101325.5333   1.225143063   340.29396 \n";
   outStream1 << "   304.8     286.1666667   97716.80197   1.189695308   339.123528\n";
   outStream1 << "   609.6     284.1888889   94213.40713   1.155036082   337.947   \n";
   outStream1 << "   914.4     282.2055556   90811.99713   1.121149925   336.767424\n";
   outStream1 << "   1219.2    280.2277778   87511.13557   1.088036836   335.581752\n";
   outStream1 << "   1524.0    278.2444444   84307.47083   1.055671046   334.393032\n";
   outStream1 << "   1828.8    276.2611111   81200.04531   1.024047402   333.201264\n";
   outStream1 << "   2133.6    274.2833333   78185.50739   0.993155596   332.0034  \n";
   outStream1 << "   2438.4    272.3         75262.89947   0.96298532    330.802488\n";
   outStream1 << "   2743.2    270.3222222   72428.86994   0.933515959   329.598528\n";
   outStream1 << "   3048.0    268.3388889   69681.98239   0.90474236    328.388472\n";
   outStream1 << "   3352.8    266.3555556   67019.84281   0.876654214   327.17232 \n";
   outStream1 << "   3657.6    264.3777778   64441.01479   0.849236061   325.95312 \n";
   outStream1 << "   3962.4    262.3944444   61943.10432   0.822482747   324.730872\n";
   outStream1 << "   4267.2    260.4111111   59524.19619   0.796373656   323.502528\n";
   outStream1 << "   4572.0    258.4333333   57182.37520   0.770908788   322.268088\n";
   outStream1 << "   4876.8    256.45        54915.24734   0.746067529   321.0306  \n";
   outStream1 << "   5181.6    254.4722222   52721.855	   0.721844725   319.790064\n";
   outStream1 << "   5486.4    252.4888889   50600.28297   0.698230068   318.540384\n";
   outStream1 << "   5791.2    250.5055556   48547.65843   0.675208096   317.287656\n";
   outStream1 << "   6096.0    248.5277778   46563.45472   0.652773656   316.03188 \n";
   outStream1 << "   6400.8    246.5444444   44645.32569   0.630911287   314.770008\n";
   outStream1 << "   6705.6    244.5611111   42791.69131   0.609615834   313.50204 \n";
   outStream1 << "   7010.4    242.5833333   41000.82788   0.588871836   312.231024\n";
   outStream1 << "   7315.2    240.6         39271.15535   0.568674141   310.953912\n";
   outStream1 << "   7620.0    238.6222222   37601.09368   0.549012439   309.670704\n";
   outStream1 << "   7924.8    236.6388889   35988.96707   0.529871269   308.3814  \n";
   outStream1 << "   8229.6    234.6555556   34433.29122   0.51124754    307.089048\n";
   outStream1 << "   8534.4    232.6777778   32932.48609   0.493128367   305.787552\n";
   outStream1 << "   8839.2    230.6944444   31485.16315   0.475504473   304.483008\n";
   outStream1 << "   9144.0    228.7166667   30089.74236   0.458366582   303.172368\n";
   outStream1 << "   9448.8    226.7333333   28744.78731   0.441705415   301.85868 \n";
   outStream1 << "   9753.6    224.75        27449.00523   0.425511182   300.535848\n";
   outStream1 << "   10058.4   222.7722222   26200.86396   0.409775636   299.209968\n";
   outStream1 << "   10363.2   220.7888889   24999.11861   0.394488469   297.874944\n";
   outStream1 << "   10668.0   218.8055556   23842.38066   0.379641952   296.536872\n";
   outStream1 << "   10972.8   216.8277778   22729.40522   0.365226806   295.189656\n";
   outStream1 << "   11277.6   216.65        21662.82570   0.348372373   295.070784\n";
   outStream1 << "   11582.4   216.65        20646.28099   0.332024557   295.070784\n";
   outStream1 << "   11887.2   216.65        19677.42498   0.316443624   295.070784\n";
   outStream1 << "   12192.0   216.65        18754.00728   0.301594015   295.070784\n";
   outStream1 << "   12496.8   216.65        17873.92117   0.287441197   295.070784\n";
   outStream1 << "   12801.6   216.65        17035.15568   0.273952702   295.070784\n";
   outStream1 << "   13106.4   216.65        16235.79560   0.261097093   295.070784\n";
   outStream1 << "   13411.2   216.65        15473.87785   0.248844477   295.070784\n";
   outStream1 << "   13716.0   216.65        14747.72661   0.237167024   295.070784\n";
   outStream1 << "   14020.8   216.65        14055.66608   0.226037934   295.070784\n";
   outStream1 << "   14325.6   216.65        13396.11621   0.215430407   295.070784\n";
   outStream1 << "   14630.4   216.65        12767.44907   0.205321252   295.070784\n";
   outStream1 << "   14935.2   216.65        12168.32403   0.195686245   295.070784\n";
   outStream1 << "   15240.0   216.65        11597.30466   0.186503225   295.070784\n";
   outStream1 << "   15544.8   216.65        11053.09821   0.177751062   295.070784\n";
   outStream1 << "   15849.6   216.65        10534.41192   0.169409656   295.070784\n";
   outStream1 << "   16154.4   216.65        10040.04876   0.161459937   295.070784\n";
   outStream1 << "   16459.2   216.65        9568.907515   0.153883353   295.070784\n";
   outStream1 << "   16764.0   216.65        9119.886921   0.146661865   295.070784\n";
   outStream1 << "   17068.8   216.65        8691.885739   0.139779497   295.070784\n";
   outStream1 << "   17373.6   216.65        8284.042124   0.13322027    295.070784\n";
   outStream1 << "   17678.4   216.65        7895.302713   0.126968725   295.070784\n";
   outStream1 << "   17983.2   216.65        7524.805661   0.121010431   295.070784\n";
   outStream1 << "   18288.0   216.65        7171.689125   0.115331987   295.070784\n";
   outStream1 << "   18592.8   216.65        6835.139140   0.109919479   295.070784\n";
   outStream1 << "   18897.6   216.65        6514.389625   0.104761568   295.070784\n";
   outStream1 << "   19202.4   216.65        6208.674494   0.099845369   295.070784\n";
   outStream1 << "   19507.2   216.65        5917.323427   0.09516006    295.070784\n";
   outStream1 << "   19812.0   216.65        5639.666099   0.090694303   295.070784\n";
   outStream1 << "   20116.8   216.7666667   5375.032187   0.086392436   295.150032\n";
   outStream1 << "   20421.6   217.0722222   5123.086531   0.08222766    295.357296\n";
   outStream1 << "   20726.4   217.3777778   4883.302448   0.078269035   295.56456 \n";
   outStream1 << "   21031.2   217.6833333   4655.057494   0.074506254   295.771824\n";
   outStream1 << "   21336.0   217.9888889   4437.767533   0.07092901    295.979088\n";
   outStream1 << "   21640.8   218.2888889   4230.905881   0.067528025   296.186352\n";
   outStream1 << "   21945.6   218.5944444   4033.955432   0.064295054   296.390568\n";
   outStream1 << "   22250.4   218.9         3846.427808   0.061220819   296.597832\n";
   outStream1 << "   22555.2   219.2055556   3667.858571   0.05829759    296.805096\n";
   outStream1 << "   22860.0   219.5111111   3497.812011   0.055517637   297.01236 \n";
   outStream1 << "   23164.8   219.8166667   3335.871571   0.052873744   297.216576\n";
   outStream1 << "   23469.6   220.1222222   3181.635056   0.05035921    297.42384 \n";
   outStream1 << "   23774.4   220.4222222   3034.729      0.047967853   297.628056\n";
   outStream1 << "   24079.2   220.7277778   2894.794303   0.045692455   297.83532 \n";
   outStream1 << "   24384.0   221.0333333   2761.495803   0.04352838    298.039536\n";
   outStream1 << "   24688.8   221.3388889   2634.503127   0.041469441   298.2468  \n";
   outStream1 << "   24993.6   221.6444444   2513.514628   0.039510486   298.451016\n";
   outStream1 << "   25298.4   221.95        2398.233451   0.037646876   298.655232\n";
   outStream1 << "   25603.2   222.2555556   2288.391465   0.035872943   298.862496\n";
   outStream1 << "   25908.0   222.5555556   2183.720542   0.034185592   299.066712\n";
   outStream1 << "   26212.8   222.8611111   2083.966915   0.032579157   299.270928\n";
   outStream1 << "   26517.6   223.1666667   1988.900762   0.031050543   299.475144\n";
   outStream1 << "   26822.4   223.4722222   1898.292256   0.029595629   299.67936 \n";
   outStream1 << "   27127.2   223.7777778   1811.925936   0.028210806   299.883576\n";
   outStream1 << "   27432.0   224.0833333   1729.595918   0.026891951   300.087792\n";
   outStream1 << "   27736.8   224.3888889   1651.115468   0.025637004   300.292008\n";
   outStream1 << "   28041.6   224.6944444   1576.293067   0.02444184    300.496224\n";
   outStream1 << "   28346.4   224.9944444   1504.956344   0.023304399   300.70044 \n";
   outStream1 << "   28651.2   225.3         1436.937720   0.022221073   300.904656\n";
   outStream1 << "   28956.0   225.6055556   1372.079190   0.021189285   301.105824\n";
   outStream1 << "   29260.8   225.9111111   1310.232325   0.020206973   301.31004 \n";
   outStream1 << "   29565.6   226.2166667   1251.248696   0.019271045   301.514256\n";
   outStream1 << "   29870.4   226.5222222   1194.994240   0.018379955   301.715424\n";
   outStream1 << "   30175.2   226.8277778   1141.339678   0.017531126   301.91964 \n";
   outStream1 << "   30480.0   227.1277778   1090.165311   0.016722496   302.120808\n";
   outStream1 << "   30784.8   227.4333333   1041.346651   0.015952521   302.325024\n";
   outStream1 << "   31089.6   227.7388889   994.7735723   0.015218621   302.526192\n";
   outStream1 << "   31394.4   228.0444444   950.3407390   0.014519252   302.730408\n";
   outStream1 << "   31699.2   228.35        907.9523906   0.013853383   302.931576\n";
   outStream1 << "   32004.0   228.6611111   867.5031906   0.013217921   303.13884 \n";
   outStream1 << "   32308.8   229.5166667   828.9548348   0.012583489   303.70272 \n";
   outStream1 << "   32613.6   230.3666667   792.2546552   0.011982042   304.2666  \n";
   outStream1 << "   32918.4   231.2222222   757.3021031   0.011411002   304.83048 \n";
   outStream1 << "   33223.2   232.0777778   724.0157822   0.010869339   305.39436 \n";
   outStream1 << "   33528.0   232.9277778   692.3047203   0.010355507   305.955192\n";
   outStream1 << "   33832.8   233.7833333   662.0923088   0.009867443   306.512976\n";
   outStream1 << "   34137.6   234.6333333   633.2971515   0.009403602   307.073808\n";
   outStream1 << "   34442.4   235.4888889   605.8570041   0.008963468   307.631592\n";
   outStream1 << "   34747.2   236.3444444   579.6952583   0.008545496   308.189376\n";
   outStream1 << "   35052.0   237.1944444   554.7544578   0.008148654   308.744112\n";
   outStream1 << "   35356.8   238.05        530.9675703   0.007771397   309.298848\n";
   outStream1 << "   35661.6   238.9         508.2819276   0.007412694   309.853584\n";
   outStream1 << "   35966.4   239.7555556   486.6448614   0.007072028   310.405272\n";
   outStream1 << "   36271.2   240.6111111   465.9941276   0.006747855   310.95696 \n";
   outStream1 << "   36576.0   241.4611111   446.2914218   0.006439658   311.508648\n";
   outStream1 << "   36880.8   242.3166667   427.4888640   0.006146408   312.057288\n";
   outStream1 << "   37185.6   243.1722222   409.5385739   0.005867588   312.608976\n";
   outStream1 << "   37490.4   244.0222222   392.4022474   0.005602683   313.154568\n";
   outStream1 << "   37795.2   244.8777778   376.0367922   0.005350148   313.703208\n";
   outStream1 << "   38100.0   245.7277778   360.4086922   0.005109981   314.2488  \n";
   outStream1 << "   38404.8   246.5833333   345.4796432   0.004881668   314.794392\n";
   outStream1 << "   38709.6   247.4388889   331.2209172   0.004663663   315.339984\n";
   outStream1 << "   39014.4   248.2888889   317.5942099   0.004456481   315.882528\n";
   outStream1 << "   39319.2   249.1444444   304.5707932   0.004259091   316.425072\n";
   outStream1 << "   39624.0   250           292.1219390   0.004070977   316.964568\n";
   outStream1 << "   39928.8   250.85        280.2237073   0.003892141   317.507112\n";
   outStream1 << "   40233.6   251.7055556   268.8521578   0.00372155    318.046608\n";
   outStream1 << "   40538.4   252.5555556   257.9737745   0.003558691   318.586104\n";
   outStream1 << "   40843.2   253.4111111   247.5694052   0.003403562   319.122552\n";
   outStream1 << "   41148.0   254.2666667   237.6198979   0.003256163   319.659   \n";
   outStream1 << "   41452.8   255.1166667   228.1013125   0.00311495    320.195448\n";
   outStream1 << "   41757.6   255.9722222   218.9944969   0.002980951   320.731896\n";
   outStream1 << "   42062.4   256.8222222   210.2755109   0.002852622   321.265296\n";
   outStream1 << "   42367.2   257.6777778   201.9347786   0.002730477   321.798696\n";
   outStream1 << "   42672.0   258.5333333   193.9531479   0.002614001   322.332096\n";
   outStream1 << "   42976.8   259.3833333   186.3066786   0.00250268    322.862448\n";
   outStream1 << "   43281.6   260.2388889   178.9857948   0.002396511   323.3928  \n";
   outStream1 << "   43586.4   261.0944444   171.9809203   0.002294982   323.923152\n";
   outStream1 << "   43891.2   261.9444444   165.2633271   0.002198091   324.450456\n";
   outStream1 << "   44196.0   262.8         158.8330151   0.002105838   324.980808\n";
   outStream1 << "   44500.8   263.65        152.6756203   0.002017708   325.508112\n";
   outStream1 << "   44805.6   264.5055556   146.7719906   0.001933186   326.032368\n";
   outStream1 << "   45110.4   265.3611111   141.11255     0.001852787   326.559672\n";
   outStream1 << "   45415.2   266.2111111   135.6925104   0.001775995   327.083928\n";
   outStream1 << "   45720.0   267.0666667   130.4975078   0.001702296   327.608184\n";
   outStream1 << "   46024.8   267.9222222   125.5131781   0.001632205   328.13244 \n";
   outStream1 << "   46329.6   268.7722222   120.7347333   0.001565205   328.653648\n";
   outStream1 << "   46634.4   269.6277778   116.1573854   0.001500783   329.174856\n";
   outStream1 << "   46939.2   270.4777778   111.7619823   0.001439453   329.696064\n";
   outStream1 << "   47244.0   270.65        107.5437359   0.001384308   329.799696\n";
   outStream1 << "   47548.8   270.65        103.4834943   0.001332254   329.799696\n";
   outStream1 << "   47853.6   270.65        99.57646926   0.001281747   329.799696\n";
   outStream1 << "   48158.4   270.65        95.81787291   0.001233302   329.799696\n";
   outStream1 << "   48463.2   270.65        92.20291718   0.001186917   329.799696\n";
   outStream1 << "   48768.0   270.65        88.72202604   0.001142079   329.799696\n";
   outStream1 << "   49072.8   270.65        85.37519947   0.001098788   329.799696\n";
   outStream1 << "   49377.6   270.65        82.15286145   0.001057557   329.799696\n";
   outStream1 << "   49682.4   270.65        79.05022395   0.001017873   329.799696\n";
   outStream1 << "   49987.2   270.65        76.06728697   0.00097922    329.799696\n";
   outStream1 << "   50292.0   270.65        73.19926249   0.000942112   329.799696\n";
   outStream1 << "   50596.8   270.65        70.43657447   0.000906551   329.799696\n";
   outStream1 << "   50901.6   270.65        67.77443489   0.000872536   329.799696\n";
   outStream1 << "   51206.4   270.0722222   65.21763177   0.000841098   329.446128\n";
   outStream1 << "   51511.2   269.2166667   62.74701302   0.000812237   328.92492 \n";
   outStream1 << "   51816.0   268.3666667   60.36257864   0.000783891   328.403712\n";
   outStream1 << "   52120.8   267.5111111   58.05954062   0.000756061   327.882504\n";
   outStream1 << "   52425.6   266.6611111   55.84268698   0.000729776   327.358248\n";
   outStream1 << "   52730.4   265.8055556   53.69765364   0.000704007   326.833992\n";
   outStream1 << "   53035.2   264.95        51.63401666   0.000678754   326.309736\n";
   outStream1 << "   53340.0   264.1         49.63741198   0.000655046   325.782432\n";
   outStream1 << "   53644.8   263.2444444   47.71741562   0.000631339   325.255128\n";
   outStream1 << "   53949.6   262.3888889   45.86445156   0.000609178   324.727824\n";
   outStream1 << "   54254.4   261.5388889   44.07851979   0.000587016   324.20052 \n";
   outStream1 << "   54559.2   260.6833333   42.35483229   0.000565886   323.670168\n";
   outStream1 << "   54864.0   259.8333333   40.69338906   0.000545786   323.139816\n";
   outStream1 << "   55168.8   258.9777778   39.08940208   0.000525686   322.609464\n";
   outStream1 << "   55473.6   258.1222222   37.54765937   0.000506617   322.076064\n";
   outStream1 << "   55778.4   257.2722222   36.05858489   0.000488579   321.542664\n";
   outStream1 << "   56083.2   256.4166667   34.62696666   0.000470541   321.009264\n";
   outStream1 << "   56388.0   255.5611111   33.24801666   0.000453018   320.475864\n";
   outStream1 << "   56692.8   254.7111111   31.91694687   0.000436526   319.939416\n";
   outStream1 << "   56997.6   253.8555556   30.63854531   0.000420549   319.402968\n";
   outStream1 << "   57302.4   253.0055556   29.40323594   0.000405088   318.86652 \n";
   outStream1 << "   57607.2   252.15        28.21580677   0.000389626   318.327024\n";
   outStream1 << "   57912.0   251.2944444   27.07146979   0.000375196   317.787528\n";
   outStream1 << "   58216.8   250.4444444   25.970225     0.000361281   317.248032\n";
   outStream1 << "   58521.6   249.5888889   24.91207239   0.000347881   316.708536\n";
   outStream1 << "   58826.4   248.7388889   23.89222396   0.000334481   316.165992\n";
   outStream1 << "   59131.2   247.8833333   22.91067969   0.000322112   315.623448\n";
   outStream1 << "   59436.0   247.0277778   21.96743958   0.000309743   315.077856\n";
   outStream1 << "   59740.8   246.1777778   21.05771562   0.000297889   314.535312\n";
   outStream1 << "   60045.6   245.3222222   20.18629583   0.000286551   313.98972 \n";
   outStream1 << "   60350.4   244.4666667   19.34360417   0.000275728   313.44108 \n";
   outStream1 << "   60655.2   243.6166667   18.53921667   0.000264905   312.895488\n";
   outStream1 << "   60960.0   242.7611111   17.75876927   0.000255113   312.346848\n";
   outStream1 << "   61264.8   241.9111111   17.01183802   0.000244805   311.79516 \n";
   outStream1 << "   61569.6   241.0555556   16.29363489   0.000235528   311.24652 \n";
   outStream1 << "   61874.4   240.2         15.60415989   0.000226251   310.694832\n";
   outStream1 << "   62179.2   239.35        14.94341302   0.00021749    310.143144\n";
   outStream1 << "   62484.0   238.4944444   14.30660625   0.000208728   309.588408\n";
   outStream1 << "   62788.8   237.6388889   13.69373958   0.000200998   309.033672\n";
   outStream1 << "   63093.6   236.7888889   13.10481302   0.000192752   308.478936\n";
   outStream1 << "   63398.4   235.9333333   12.53982656   0.000185021   307.921152\n";
   outStream1 << "   63703.2   235.0833333   11.99878021   0.000177806   307.363368\n";
   outStream1 << "   64008.0   234.2277778   11.47688594   0.00017059    306.805584\n";
   outStream1 << "   64312.8   233.3722222   10.97893177   0.00016389    306.2478  \n";
   outStream1 << "   64617.6   232.5222222   10.49534167   0.000157191   305.686968\n";
   outStream1 << "   64922.4   231.6666667   10.03569167   0.000151006   305.126136\n";
   outStream1 << "   65227.2   230.8166667   9.595193749   0.000144821   304.562256\n";
   outStream1 << "   65532.0   229.9611111   9.169059895   0.000139152   303.998376\n";
   outStream1 << "   65836.8   229.1055556   8.762078124   0.000133483   303.434496\n";
   outStream1 << "   66141.6   228.2555556   8.374248437   0.000127814   302.867568\n";
   outStream1 << "   66446.4   227.4         8.000782812   0.00012266    302.30064 \n";
   outStream1 << "   66751.2   226.5444444   7.641681249   0.000117506   301.733712\n";
   outStream1 << "   67056.0   225.6944444   7.296943749   0.000112868   301.163736\n";
   outStream1 << "   67360.8   224.8388889   6.966570312   0.000107714   300.59376 \n";
   outStream1 << "   67665.6   223.9888889   6.650560937   0.000103591   300.023784\n";
   outStream1 << "   67970.4   223.1333333   6.348915625   9.89527E-05   299.45076 \n";
   outStream1 << "   68275.2   222.2777778   6.056846354   9.48297E-05   298.877736\n";
   outStream1 << "   68580.0   221.4277778   5.779141145   9.07067E-05   298.304712\n";
   outStream1 << "   68884.8   220.5722222   5.5158        8.7099E-05    297.72864 \n";
   outStream1 << "   69189.6   219.7166667   5.262034895   8.34914E-05   297.152568\n";
   outStream1 << "   69494.4   218.8666667   5.017845833   7.98837E-05   296.573448\n";
   outStream1 << "   69799.2   218.0111111   4.783232812   7.62761E-05   295.997376\n";
   outStream1 << "   70104.0   217.1611111   4.558195833   7.31838E-05   295.415208\n";
   outStream1 << "   70408.8   216.3055556   4.347522916   7.00915E-05   294.836088\n";
   outStream1 << "   70713.6   215.45        4.141638021   6.69992E-05   294.25392 \n";
   outStream1 << "   71018.4   214.6111111   3.945329166   6.3907E-05    293.680896\n";
   outStream1 << "   71323.2   214.0055556   3.758596354   6.13301E-05   293.26332 \n";
   outStream1 << "   71628.0   213.3944444   3.581439583   5.82378E-05   292.842696\n";
   outStream1 << "   71932.8   212.7833333   3.409070833   5.56609E-05   292.42512 \n";
   outStream1 << "   72237.6   212.1777778   3.246278125   5.3084E-05    292.007544\n";
   outStream1 << "   72542.4   211.5666667   3.088273437   5.10225E-05   291.58692 \n";
   outStream1 << "   72847.2   210.9555556   2.939844791   4.84456E-05   291.166296\n";
   outStream1 << "   73152.0   210.3444444   2.800992187   4.63841E-05   290.745672\n";
   outStream1 << "   73456.8   209.7388889   2.662139583   4.43226E-05   290.322   \n";
   outStream1 << "   73761.6   209.1277778   2.532863021   4.22611E-05   289.901376\n";
   outStream1 << "   74066.4   208.5166667   2.4131625     4.01995E-05   289.477704\n";
   outStream1 << "   74371.2   207.9055556   2.293461979   3.86534E-05   289.054032\n";
   outStream1 << "   74676.0   207.3         2.1833375     3.65919E-05   288.63036 \n";
   outStream1 << "   74980.8   206.6888889   2.073213021   3.50458E-05   288.206688\n";
   outStream1 << "   75285.6   206.0777778   1.972664583   3.34996E-05   287.779968\n";
   outStream1 << "   75590.4   205.4722222   1.876904167   3.19535E-05   287.356296\n";
   outStream1 << "   75895.2   204.8611111   1.781143753   3.04074E-05   286.929576\n";
   outStream1 << "   76200.0   204.25        1.694959375   2.88612E-05   286.499808\n";
   outStream1 << "   76504.8   203.6388889   1.608775      2.73151E-05   286.073088\n";
   outStream1 << "   76809.6   203.0333333   1.527378646   2.62843E-05   285.64332 \n";
   outStream1 << "   77114.4   202.4222222   1.450770312   2.52536E-05   285.2166  \n";
   outStream1 << "   77419.2   201.8111111   1.37895       2.37074E-05   284.786832\n";
   outStream1 << "   77724.0   201.2         1.311917708   2.26767E-05   284.354016\n";
   outStream1 << "   78028.8   200.5944444   1.244885417   2.16459E-05   283.924248\n";
   outStream1 << "   78333.6   199.9833333   1.182641146   2.06152E-05   283.491432\n";
   outStream1 << "   78638.4   199.3722222   1.120396875   1.95844E-05   283.058616\n";
   outStream1 << "   78943.2   198.7611111   1.062940625   1.85536E-05   282.6258  \n";
   outStream1 << "   79248.0   198.1555556   1.010272396   1.75229E-05   282.192984\n";
   outStream1 << "   79552.8   197.5444444   0.957604167   1.70075E-05   281.760168\n";
   outStream1 << "   79857.6   196.9333333   0.909723958   1.59767E-05   281.324304\n";
   outStream1 << "   80162.4   196.3277778   0.86184375    1.54614E-05   280.88844 \n";
   outStream1 << "   80467.2   195.7166667   0.818751562   1.44306E-05   280.452576\n";
   outStream1 << "   80772.0   195.1055556   0.775659375   1.39152E-05   280.013664\n";
   outStream1 << "   81076.8   194.4944444   0.732567187   1.33998E-05   279.5778  \n";
   outStream1 << "   81381.6   193.8888889   0.694263021   1.23691E-05   279.138888\n";
   outStream1 << "   81686.4   193.2777778   0.660746875   1.18537E-05   278.699976\n";
   outStream1 << "   81991.2   192.6666667   0.627230729   1.13383E-05   278.258016\n";
   outStream1 << "   82296.0   192.0555556   0.593714583   1.0823E-05    277.819104\n";
   outStream1 << "   82600.8   191.45        0.560198437   1.03076E-05   277.377144\n";
   outStream1 << "   82905.6   190.8388889   0.531470312   9.7922E-06    276.935184\n";
   outStream1 << "   83210.4   190.2277778   0.502742187   9.27682E-06   276.493224\n";
   outStream1 << "   83515.2   189.6222222   0.474014062   8.76144E-06   276.048216\n";
   outStream1 << "   83820.0   189.0111111   0.450073958   8.24606E-06   275.606256\n";
   outStream1 << "   84124.8   188.4         0.426133854   7.73068E-06   275.161248\n";
   outStream1 << "   84429.6   187.7888889   0.40219375    7.73068E-06   274.71624 \n";
   outStream1 << "   84734.4   187.1833333   0.383041667   7.2153E-06    274.268184\n";
   outStream1 << "   85039.2   186.9444444   0.359101562   6.69992E-06   274.097496\n";
   outStream1 << "   85344.0   186.9444444   0.339949479   6.18455E-06   274.097496\n";
   outStream1 << "   85648.8   186.9444444   0.320797396   6.18455E-06   274.097496\n";
   outStream1 << "   85953.6   186.9444444   0.306433333   5.66917E-06   274.097496\n";
   outStream1 << "   121920.0  186.9444444   0.0           0.0           274.097496\n";
   outStream1 << "end_atmosphere_table\n";

   baseFile1.close();

   // ...................................

   pathAddition = "/six_dof_types/environment/integrators_config" + GetFilenameSuffix() + ".txt";
   QFile baseFile2(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile2.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream2(&baseFile2);
   outStream2.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream2);
   outStream2 << "integrators\n";
   outStream2 << "\n";
   outStream2 << "   create_integrator  " + GetDefaultIntegratorName() + "\n";
   outStream2 << "\n";
   outStream2 << "end_integrators\n";
   outStream2 << "\n";

   baseFile2.close();

   // ...................................

   pathAddition = "/six_dof_types/environment/terrain_config.txt";
   QFile baseFile3(mVehicleDir.absolutePath() + pathAddition);
   if (!baseFile3.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream3(&baseFile3);
   outStream3.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream3);
   outStream3 << "terrain\n";
   outStream3 << "\n";
   outStream3 << "   region\n";
   outStream3 << "      # Oahu, HI\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Honolulu Intl, PHNL\n";
   outStream3 << "         lat    21.3178275\n";
   outStream3 << "         lon  -157.92026310\n";
   outStream3 << "         alt_ft  5.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Kalaeloa, PHJR\n";
   outStream3 << "         lat    21.3073539\n";
   outStream3 << "         lon  -158.0703017\n";
   outStream3 << "         alt_ft 30.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Kaneohe Bay MCAS, PHNG\n";
   outStream3 << "         lat    21.4504556\n";
   outStream3 << "         lon  -157.7679486\n";
   outStream3 << "         alt_ft 24.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Wheeler AFB, HHI/PHHI\n";
   outStream3 << "         lat     21.4814475\n";
   outStream3 << "         lon   -158.0378379\n";
   outStream3 << "         alt_ft 843.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "      location\n";
   outStream3 << "         # Dillingham, PHDH\n";
   outStream3 << "         lat    21.5794736\n";
   outStream3 << "         lon  -158.1972814\n";
   outStream3 << "         alt_ft 14.0\n";
   outStream3 << "      end_location\n";
   outStream3 << "\n";
   outStream3 << "   end_region\n";
   outStream3 << "\n";
   outStream3 << "end_terrain\n";
   outStream3 << "\n";

   baseFile3.close();
}

void ScriptGeneratorSixDOF::CreatePerformanceTestFiles(QString& aTestDir)
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   // Performance tests that will be used for all cases:
   CreateCommonTestFile(aTestDir);
   CreatePitchingMomentTest(aTestDir, 20000.0, 0.25);
   CreatePitchingMomentTest(aTestDir, 20000.0, 0.50);
   CreatePitchingMomentTest(aTestDir, 20000.0, 0.85);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.25);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.50);
   CreatePitchingMomentTest(aTestDir, 20000.0, 1.75);
   CreatePitchingMomentTest(aTestDir, 20000.0, 2.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 3.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 4.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 5.00);
   CreatePitchingMomentTest(aTestDir, 20000.0, 6.00);
   CreateFlightTestFile(21.12, -157.95, 20000.0, 0, 750);

   switch (GetPerformanceTestType())
   {
      // Case for vehicles with no control and no engine.
      // Examples: spent stage of a missile, unguided munitions
      case Designer::ScriptGeneratorSixDOF::PerformanceTestType::cNO_CONTROL_NO_ENGINE:
      {
         // Do nothing special, for now.
         break;
      }
      // Case for vehicles with no control with an engine
      // Example: boost stage on a missile
      case Designer::ScriptGeneratorSixDOF::PerformanceTestType::cNO_CONTROL_ENGINE:
      {
         CreateBallisticRangeTestFile(aTestDir);
         break;
      }
      // Case for a bank-to-turn vehicle.
      // Examples: aircraft, guided drone missiles
      case Designer::ScriptGeneratorSixDOF::PerformanceTestType::cBANK_TO_TURN:
      {
         CreateEnvelopeCommonTestFile(aTestDir);
         CreateFlightEnvelopeSimpleTestFile(aTestDir);

         if (mVehicleType == "aircraft")
         {
            CreateFlightEnvelopeAbTestFile(aTestDir);
            CreateFlightEnvelopeMilTestFile(aTestDir);
            CreateTurnPerformanceTestFile(aTestDir, 5000, true);
            CreateTurnPerformanceTestFile(aTestDir, 5000, false);
            CreateTurnPerformanceTestFile(aTestDir, 15000, true);
            CreateTurnPerformanceTestFile(aTestDir, 15000, false);
            CreateTurnPerformanceTestFile(aTestDir, 25000, true);
            CreateTurnPerformanceTestFile(aTestDir, 25000, false);
            CreateTurnPerformanceTestFile(aTestDir, 35000, true);
            CreateTurnPerformanceTestFile(aTestDir, 35000, false);
            CreateTurnPerformanceTestFile(aTestDir, 45000, true);
            CreateTurnPerformanceTestFile(aTestDir, 45000, false);
         }
         CreateRangeTestFile(aTestDir);
         CreateEnduranceTestFile(aTestDir);
         CreateThrustDragTest(aTestDir, 0.0);
         CreateThrustDragTest(aTestDir, 5000.0);
         CreateThrustDragTest(aTestDir, 15000.0);
         CreateThrustDragTest(aTestDir, 30000.0);
         CreateThrustDragTest(aTestDir, 45000.0);
         CreateThrustDragTest(aTestDir, 60000.0);
         CreateBaseScenarioFile();
         break;
      }
      // Cases for a skid-to-turn vehicle
      // Example: intercept air missiles
      case Designer::ScriptGeneratorSixDOF::PerformanceTestType::cSKID_TO_TURN:
      {
         CreateBallisticRangeTestFile(aTestDir);
         CreateGCapabilityTestFile(aTestDir);
         CreateLevelFlightRangeTestFile(aTestDir);
         CreateMaximumSpeedTestFile(aTestDir);
         CreateBaseScenarioFile();
         break;
      }
      default:
         break;
   }
}

const QString ScriptGeneratorSixDOF::CreateFlightTestFile(double aLatitude,
                                                          double aLongitude,
                                                          double aAltitude_ft,
                                                          double aHeading_deg,
                                                          double aSpeed_fps)
{
   QString testFile = mTestDir.absolutePath() + "/flight_test_scenario.txt";
   QFile genFile(testFile);
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return "";
   }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This file is used for flight testing a vehicle that was created by the AFSIM Mover Creator tool.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "# Setup a replay file for post-run viewing\n";
   outStream << "event_pipe\n";
   outStream << "   file ../../output/flight_test_scenario_replay.aer\n";
   outStream << "   enable SixDOF\n";
   outStream << "end_event_pipe\n";
   outStream << "\n";
   outStream << "# Set a start time that has a daytime sun position in the Oahu region (default test area)\n";
   outStream << "start_epoch 2018354.75   # Southern Solstice 2018\n";
   outStream << "\n";
   outStream << "# Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "# Provide plenty of runtime, to prevent the scenario from ending before testing is complete.\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "# Setup the test vehicle\n";
   outStream << "platform  six_dof_test_platform  " << mPlatformType << "\n";
   outStream << "\n";
   outStream << "   # Output six_dof Event Pipe data\n";
   outStream << "   edit mover\n";
   outStream << "      event_core_data             30 Hz\n";
   outStream << "      event_kinematic_data        30 Hz\n";
   outStream << "      event_engine_fuel_data      10 Hz\n";
   outStream << "      event_autopilot_data        10 Hz\n";
   outStream << "      event_autopilot_limits_data  1 Hz\n";
   outStream << "      event_control_inputs_data   20 Hz\n";
   outStream << "      event_force_moment_data     20 Hz\n";
   outStream << "      event_core_data             30 Hz\n";
   outStream << "   end_mover\n";
   outStream << "\n";

   double heading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;
   double vel_fps     = aSpeed_fps;
   double north_fps   = cos(heading_rad) * vel_fps;
   double east_fps    = sin(heading_rad) * vel_fps;

   double heading_deg = aHeading_deg;
   while (heading_deg < 0.0)
   {
      heading_deg += 360.0;
   }
   while (heading_deg > 360.0)
   {
      heading_deg -= 360.0;
   }

   outStream << "   six_dof_set_velocity_ned_fps  " << north_fps << "  " << east_fps << "  0\n";
   outStream << "   six_dof_position              " << aLatitude << "  " << aLongitude << "\n";
   outStream << "   six_dof_alt                   " << aAltitude_ft << " ft\n";
   outStream << "   six_dof_ned_heading           " << heading_deg << " deg\n";
   outStream << "\n";

   outStream << "end_platform\n";
   outStream << "\n";

   genFile.close();

   return testFile;
}

QString ScriptGeneratorSixDOF::ControlsFolderPath() const
{
   return mVehicleDir.absolutePath() + "/six_dof_types/" + mVehicleType + "/" + mVehicleName.toLower() + "/controls";
}

void ScriptGeneratorSixDOF::CreateAutopilotSupportGeneratorFile(QString& aTestDir)
{
   QFile genFile(aTestDir + "/generate_autopilot_support_file.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "# ==================================================================================================\n";
   outStream << "# This file is used for creating the autopilot support file that is used by the P6DOF autopilot.\n";
   outStream << "# ==================================================================================================\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n";
   outStream << "\n";
   outStream << "# Allow Warlock to update more frequently and miss fewer updates\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "# Provide plenty of runtime, in order to support multiple autopilot tests\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "platform  test_platform  " << mPlatformType << "\n";
   outStream << "\n";
   outStream << "   script void CreateAutopilotSupportFileWithMaxMach(WsfPlatform aPlatform, string aFilename, double aMaxMach)\n";
   outStream << "      WsfMover mover = aPlatform.Mover();\n";
   outStream << "      if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "      {\n";
   outStream << "         WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "         sixdofMover.__getTestObject().CreateAutopilotSupportFileWithMaxMach(aFilename,aMaxMach);\n";
   outStream << "      }\n";
   outStream << "      else\n";
   outStream << "      {\n";
   outStream << "         writeln(\"--ERROR-- Not a SixDOF Mover.\");\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "\n";
   outStream << "   six_dof_set_velocity_ned_fps  100 0 0\n";
   outStream << "   six_dof_position              0 0\n";
   outStream << "   six_dof_alt                   1000 ft\n";
   outStream << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      writeln("");\n";
   outStream << "      writeln(\"Generating autopilot support file for " << mVehicleName.toUpper() << " ...\");\n";
   outStream << "      writeln("");\n";

   QString machString = QString::number(mVehiclePtr->GetAeroDataMaxMach());

   QString outputFilePath = ControlsFolderPath() + "/autopilot_support_tables" + GetFilenameSuffix() + ".txt";
   outStream << "      CreateAutopilotSupportFileWithMaxMach(PLATFORM, \"" << outputFilePath << "\", " << machString << ");\n";

   outStream << "      writeln(\"Autopilot support file is complete.\");\n";
   outStream << "   end_on_initialize2\n";
   outStream << "\n";
   outStream << "end_platform\n";
   outStream << "\n";

   genFile.close();
}

void ScriptGeneratorSixDOF::CreateAeroFile(QString& aAeroDir)
{
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   QFile aeroFile(aAeroDir + "/aero" + GetFilenameSuffix() + ".txt");
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   CreateAeroTables(aAeroDir);

   QStringList TwoDTablesList = {"cmq_mach_table",
                                 "cmp_mach_table",
                                 "cm_alphadot_mach_table",
                                 "cn_betadot_mach_table",
                                 "cnr_mach_table",
                                 "cnp_mach_table",
                                 "clp_mach_table",
                                 "cl_alphadot_mach_table",
                                 "cl_betadot_mach_table",
                                 "clr_mach_table",
                                 "clq_mach_table"
                                };

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# This file defines the aerodynamics of the vehicle.\n";
   outStream << "# The aero is composed of two types --primary aero (defined within the aero_data block) and\n";
   outStream << "# component aero (defined within aero_component blocks).\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# This is the 'primary aero' data block. It defines the aerodynamics that are not produced by\n";
   outStream << "# a control surface or other movable component on the vehicle. It also specifies reference areas\n";
   outStream << "# and other reference values.\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n\n";
   outStream << "# **************************************************************************************************\n\n";
   outStream << "aero_data\n\n";

   int width = 15;
   int precision = 4;
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   double wingChord_ft  = vehicleAero->GetVehicleWingChord_ft();
   double wingSpan_ft   = vehicleAero->GetVehicleWingSpan_ft();
   double wingArea_sqft = vehicleAero->GetVehicleWingArea_ft2();
   double refArea_sqft  = vehicleAero->GetVehicleRefArea_ft2();

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "   wing_chord_ft    ";
      OutputData(outStream, wingChord_ft, width, precision, alignment);
      outStream << "\n";
      outStream << "   wing_span_ft     ";
      OutputData(outStream, wingSpan_ft, width, precision, alignment);
      outStream << "\n";
      outStream << "   wing_area_sqft   ";
      OutputData(outStream, wingArea_sqft, width, precision, alignment);
      outStream << "\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      if (mVehiclePtr->GetGeometry().GetWing() != nullptr)
      {
         outStream << "   wing_chord_ft    ";
         OutputData(outStream, wingChord_ft, width, precision, alignment);
         outStream << "\n";
         outStream << "   wing_span_ft     ";
         OutputData(outStream, wingSpan_ft, width, precision, alignment);
         outStream << "\n";
         outStream << "   wing_area_sqft   ";
         OutputData(outStream, wingArea_sqft, width, precision, alignment);
         outStream << "\n";
      }
      else
      {
         outStream << "   ref_area_sqft    ";
         OutputData(outStream, refArea_sqft, width, precision, alignment);
         outStream << "\n";
      }
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle needs to be either an aircraft or a weapon!");
      return;
   }

   const std::vector<std::string> enabledTablesList = mVehiclePtr->GetAero()->GetCoreAero().GetEnabledTables();
   for (const auto& curTable : enabledTablesList)
   {
      QStringList tableStrList = QString::fromStdString(curTable).split(" ");
      QString tableFileName = "";
      QString tableKeyWord = "";
      for (auto curStr : tableStrList)
      {
         if (curStr == "vs")
         {
            continue;
         }
         else
         {
            if (tableFileName != "")
            {
               tableFileName.append("_");
               tableKeyWord.append("_");
            }

            if (curStr == "CL")
            {
               curStr = "cL";
               tableKeyWord.append(curStr);

               curStr = "cLift";
               tableFileName.append(curStr);
            }
            else if (curStr == "CLq")
            {
               curStr = "cLq";
               tableKeyWord.append(curStr);

               curStr = "cLiftq";
               tableFileName.append(curStr);
            }
            else if (curStr == "CL_alphadot")
            {
               curStr = "cL_alphadot";
               tableKeyWord.append(curStr);

               curStr = "cLift_alphadot";
               tableFileName.append(curStr);
            }
            else
            {
               curStr = curStr.toLower();
               tableKeyWord.append(curStr);
               tableFileName.append(curStr);
            }
         }
      }
      tableKeyWord.append("_table");
      tableFileName.append("_table" + GetFilenameSuffix() + ".txt");
      outStream << "# --------------------------------------------------------------------------------------------------\n";
      outStream << "\n";
      outStream << "   " << tableKeyWord << "\n";

      bool isTwoD = TwoDTablesList.contains(tableKeyWord);
      if (!isTwoD)
      {
         outStream << "      file data/" << tableFileName << "\n";
      }
      else
      {
         if (tableKeyWord == "cmq_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCmq_MachTablePtr());
         }
         else if (tableKeyWord == "cmp_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCmp_MachTablePtr());
         }
         else if (tableKeyWord == "cm_alphadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCm_AlphaDotMachTablePtr());
         }
         else if (tableKeyWord == "cn_betadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCn_BetaDotMachTablePtr());
         }
         else if (tableKeyWord == "cnr_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCnr_MachTablePtr());
         }
         else if (tableKeyWord == "cnp_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCnp_MachTablePtr());
         }
         else if (tableKeyWord == "clp_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetClp_MachTablePtr());
         }
         else if (tableKeyWord == "cl_alphadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCl_AlphaDotMachTablePtr());
         }
         else if (tableKeyWord == "cl_betadot_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetCl_BetaDotMachTablePtr());
         }
         else if (tableKeyWord == "clr_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetClr_MachTablePtr());
         }
         else if (tableKeyWord == "clq_mach_table")
         {
            Create2dTableInline(&outStream, mVehiclePtr->GetAero()->GetCoreAero().GetClq_MachTablePtr());
         }
      }
      outStream << "   end_" << tableKeyWord << "\n";
      outStream << "\n";
   }
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "end_aero_data\n";
   outStream << "\n";
   outStream << "# **************************************************************************************************\n";
   outStream << "\n";
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# Listed below are the 'component aero' data blocks. These define the aerodynamics that are produced\n";
   outStream << "# by control surfaces or other movable components on the vehicle.\n";
   outStream << "\n";

   std::vector<VehicleAeroMovable> moveableComponentList = mVehiclePtr->GetAero()->GetMovableAeroList();
   for (auto curComponent : moveableComponentList)
   {
      QString curPart = QString::fromStdString(curComponent.GetName()).replace(" ", "_");
      const std::vector<std::string> enabledMovableTablesList = curComponent.GetEnabledTables();
      if (enabledMovableTablesList.size() == 0)
      {
         continue;
      }
      outStream << "# --------------------------------------------------------------------------------------------------\n\n";
      outStream << "aero_component\n";
      outStream << "   type                    " << curPart << "\n";
      outStream << "   #ref_area_sqft          12.0\n\n";
      for (const auto& curTable : enabledMovableTablesList)
      {
         QStringList tableStrList = QString::fromStdString(curTable).split(" ");
         QString tableFileName = "";
         QString tableKeyWord = "";
         for (auto curStr : tableStrList)
         {
            if (curStr == "vs")
            {
               continue;
            }
            else
            {
               if (tableFileName != "")
               {
                  tableFileName.append("_");
                  tableKeyWord.append("_");
               }

               if (curStr == "CL")
               {
                  curStr = "cL";
                  tableKeyWord.append(curStr);

                  curStr = "cLift";
                  tableFileName.append(curStr);
               }
               else if (curStr == "CLq")
               {
                  curStr = "cLq";
                  tableKeyWord.append(curStr);

                  curStr = "cLiftq";
                  tableFileName.append(curStr);
               }
               else if (curStr == "CL_alphadot")
               {
                  curStr = "cL_alphadot";
                  tableKeyWord.append(curStr);

                  curStr = "cLift_alphadot";
                  tableFileName.append(curStr);
               }
               else
               {
                  curStr = curStr.toLower();
                  tableKeyWord.append(curStr);
                  tableFileName.append(curStr);
               }
            }
         }
         tableKeyWord.append("_table");
         tableFileName.append("_table" + GetFilenameSuffix() + ".txt");

         std::string tableName = tableKeyWord.toStdString();
         if ((tableName == "cd_angle_alpha_mach_table") || (tableName == "cd_angle_beta_mach_table"))
         {
            // Skip these tables

         }
         else
         {
            outStream << "   " << tableKeyWord << "\n";
            outStream << "      file data/" << curPart << "/" << tableFileName << "\n";
            outStream << "   end_" << tableKeyWord << "\n\n";
         }
      }
      outStream << "end_aero_component\n";
   }
   aeroFile.close();
}

void ScriptGeneratorSixDOF::CreateAeroTables(QString& aAeroDir)
{
   QString aeroTablesDir = aAeroDir + "/data";

   //Create core aero table files

   // Lift
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCL_AlphaBetaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCLq_AlphaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCL_AlphaDotAlphaMachTablePtr());

   // Drag
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCd_AlphaBetaMachTablePtr());

   // Side force
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCY_AlphaBetaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCYr_BetaMachTablePtr());
   Create3dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCY_BetaDotBetaMachTablePtr());

   // Pitching moments -- Note that the 2D tables for Cmq, Cmp, and Cm-AlphaDot are not created here.
   // They are output directly in the aero file
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCm_AlphaBetaMachTablePtr());

   // Yawing moments -- Note that the 2D tables for Cn-BetaDot, Cnr, and Cnp are not created here.
   // They are output directly in the aero file
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCn_AlphaBetaMachTablePtr());

   // Rolling moments -- Note that the 2D tables for Clp, Cl-AlphaDot, Cl-BetaDot, Clr, and Clq) are not created here.
   // They are output directly in the aero file
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetCl_AlphaBetaMachTablePtr());

   //Get the list of components and generate tables for each.
   std::vector<VehicleAeroMovable> moveableComponentList = mVehiclePtr->GetAero()->GetMovableAeroList();
   for (const auto& curComponent : moveableComponentList)
   {
      CreateAeroComponentTables(aeroTablesDir, curComponent);
   }
}

void ScriptGeneratorSixDOF::Create4dTableFile(const QString& aAeroDir, AeroTable4d* aTablePtr)
{
   if (!aTablePtr)
   {
      return;
   }

   QString primaryDependentVarName(aTablePtr->GetDependentVarName().c_str());
   QStringList primaryDependentVarPieces = primaryDependentVarName.split(" ");
   primaryDependentVarName = primaryDependentVarPieces.first();

   QString primaryIndependentVarName(aTablePtr->GetPrimaryIndependentVarName().c_str());
   QString secondaryIndependentVarName(aTablePtr->GetSecondaryIndependentVarName().c_str());
   QString tertiaryIndependentVarName(aTablePtr->GetTertiaryIndependentVarName().c_str());

   if (primaryDependentVarName == "CL")
   {
      primaryDependentVarName = "cLift";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CLq")
   {
      primaryDependentVarName = "cLiftq";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CL_alphadot")
   {
      primaryDependentVarName = "cLift_alphadot";
   }
   else
   {
      primaryDependentVarName = primaryDependentVarName.toLower();
   }
   primaryIndependentVarName = primaryIndependentVarName.toLower();
   secondaryIndependentVarName = secondaryIndependentVarName.toLower();
   tertiaryIndependentVarName = tertiaryIndependentVarName.toLower();

   QString fileName = "/" + primaryDependentVarName + "_" + primaryIndependentVarName + "_" + secondaryIndependentVarName + "_" + tertiaryIndependentVarName + "_table" + GetFilenameSuffix() + ".txt";

   QFile aeroFile(aAeroDir + fileName);
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   if (tertiaryIndependentVarName == "angle")
   {
      tertiaryIndependentVarName = "surface_angle";
   }
   else if (secondaryIndependentVarName == "angle")
   {
      secondaryIndependentVarName = "surface_angle";
   }
   else if (primaryIndependentVarName == "angle")
   {
      primaryIndependentVarName = "surface_angle";
   }

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream.setRealNumberPrecision(17);
   outStream << "regular_table\n";
   outStream << "\n";
   if (tertiaryIndependentVarName != "mach")
   {
      outStream << "independent_variable " << tertiaryIndependentVarName << " units rad\n";
   }
   else
   {
      outStream << "independent_variable " << tertiaryIndependentVarName << " precision double\n";
   }
   const std::vector<double> tertiaryValues = aTablePtr->GetTertiaryIndepenentValues();
   for (auto curTertiary : tertiaryValues)
   {
      if (tertiaryIndependentVarName == "alpha" || tertiaryIndependentVarName == "beta")
      {
         outStream << curTertiary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curTertiary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   if (secondaryIndependentVarName == "surface_angle")
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " units deg\n";
   }
   else
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " units rad\n";
   }
   const std::vector<double> secondaryValues = aTablePtr->GetSecondaryIndependentValues();
   for (auto curSecondary : secondaryValues)
   {
      if (secondaryIndependentVarName == "alpha" || secondaryIndependentVarName == "beta")
      {
         outStream << curSecondary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curSecondary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   if (primaryIndependentVarName == "surface_angle")
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units deg\n";
   }
   else
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units rad\n";
   }
   const std::vector<double> primaryValues = aTablePtr->GetPrimaryIndependentValues();
   for (auto curPrimary : primaryValues)
   {
      if (primaryIndependentVarName == "alpha" || primaryIndependentVarName == "beta")
      {
         outStream << curPrimary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curPrimary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   outStream << "dependent_variable precision double\n";
   for (auto curTertiary : tertiaryValues)
   {
      for (auto curSecondary : secondaryValues)
      {
         std::vector<std::pair<double, double>> curData;
         aTablePtr->GetPrimaryModifiedData(curSecondary, curTertiary, curData);
         for (auto curPair : curData)
         {
            outStream << curPair.second << " ";
         }
      }
   }
   outStream << "\n";
   outStream << "end_dependent_variable\n";
   outStream << "\n";
   outStream << "end_regular_table\n";
}

void ScriptGeneratorSixDOF::Create3dTableFile(const QString& aAeroDir, AeroTable3d* aTablePtr)
{
   if (!aTablePtr) { return; }

   QString primaryDependentVarName(aTablePtr->GetDependentVarName().c_str());
   QString primaryIndependentVarName(aTablePtr->GetPrimaryIndependentVarName().c_str());
   QString secondaryIndependentVarName(aTablePtr->GetSecondaryIndependentVarName().c_str());

   if (primaryDependentVarName == "CL")
   {
      primaryDependentVarName = "cLift";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CLq")
   {
      primaryDependentVarName = "cLiftq";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CL_alphadot")
   {
      primaryDependentVarName = "cLift_alphadot";
   }
   else
   {
      primaryDependentVarName = primaryDependentVarName.toLower();
   }

   primaryIndependentVarName = primaryIndependentVarName.toLower();
   secondaryIndependentVarName = secondaryIndependentVarName.toLower();

   QString fileName = "/" + primaryDependentVarName + "_" + primaryIndependentVarName + "_" + secondaryIndependentVarName + "_table" + GetFilenameSuffix() + ".txt";

   QFile aeroFile(aAeroDir + fileName);
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   if (secondaryIndependentVarName == "angle")
   {
      secondaryIndependentVarName = "surface_angle";
   }
   else if (primaryIndependentVarName == "angle")
   {
      primaryIndependentVarName = "surface_angle";
   }

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream.setRealNumberPrecision(17);
   outStream << "regular_table\n";
   outStream << "\n";

   if (secondaryIndependentVarName != "mach")
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " units rad\n";
   }
   else
   {
      outStream << "independent_variable " << secondaryIndependentVarName << " precision double\n";
   }
   const std::vector<double> secondaryValues = aTablePtr->GetSecondaryIndependentValues();
   for (auto curSecondary : secondaryValues)
   {
      if (secondaryIndependentVarName == "alpha" || secondaryIndependentVarName == "beta")
      {
         outStream << curSecondary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curSecondary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   if (primaryIndependentVarName == "surface_angle")
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units deg\n";
   }
   else
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units rad\n";
   }
   const std::vector<double> primaryValues = aTablePtr->GetPrimaryIndependentValues();
   for (auto curPrimary : primaryValues)
   {
      if (primaryIndependentVarName == "alpha" || primaryIndependentVarName == "beta")
      {
         outStream << curPrimary* UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << curPrimary << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   outStream << "dependent_variable precision double\n";
   for (auto curSecondary : secondaryValues)
   {
      std::vector<std::pair<double, double>> curData;
      aTablePtr->GetPrimaryModifiedData(curSecondary, curData);
      for (auto curPair : curData)
      {
         outStream << curPair.second << " ";
      }
   }
   outStream << "\n";
   outStream << "end_dependent_variable\n";
   outStream << "\n";
   outStream << "end_regular_table\n";
}

void ScriptGeneratorSixDOF::Create2dTableFile(const QString& aAeroDir, AeroTable2d* aTablePtr)
{
   if (!aTablePtr) { return; }

   QString primaryDependentVarName(aTablePtr->GetDependentVarName().c_str());
   QString primaryIndependentVarName(aTablePtr->GetIndependentVarName().c_str());

   if (primaryDependentVarName == "CL")
   {
      primaryDependentVarName = "cLift";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CLq")
   {
      primaryDependentVarName = "cLiftq";  //creating two files beginning with 'cL' or 'cl' is not allowed in Windows
   }
   else if (primaryDependentVarName == "CL_alphadot")
   {
      primaryDependentVarName = "cLift_alphadot";
   }
   else
   {
      primaryDependentVarName = primaryDependentVarName.toLower();
   }

   primaryIndependentVarName = primaryIndependentVarName.toLower();

   QString fileName = "/" + primaryDependentVarName + "_" + primaryIndependentVarName + "_table" + GetFilenameSuffix() + ".txt";

   QFile aeroFile(aAeroDir + fileName);
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   if (primaryIndependentVarName == "angle")
   {
      primaryIndependentVarName = "surface_angle";
   }

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream.setRealNumberPrecision(17);
   outStream << "regular_table\n";
   outStream << "\n";

   if (primaryIndependentVarName != "mach")
   {
      outStream << "independent_variable " << primaryIndependentVarName << " units rad\n";
   }
   else
   {
      outStream << "independent_variable " << primaryIndependentVarName << " precision double\n";
   }

   std::vector<std::pair<double, double>> dataPairs;
   aTablePtr->GetModifiedData(dataPairs);

   for (auto dataPair : dataPairs)
   {
      if (primaryIndependentVarName == "alpha" || primaryIndependentVarName == "beta")
      {
         outStream << dataPair.first * UtMath::cRAD_PER_DEG << " ";
      }
      else
      {
         outStream << dataPair.first << " ";
      }
   }
   outStream << "\n";
   outStream << "end_independent_variable\n";
   outStream << "\n";

   outStream << "dependent_variable precision double\n";
   for (auto dataPair : dataPairs)
   {
      outStream << dataPair.second << " ";
   }
   outStream << "\n";
   outStream << "end_dependent_variable\n";
   outStream << "\n";
   outStream << "end_regular_table\n";
}

void ScriptGeneratorSixDOF::Create2dTableInline(QTextStream* aStreamPtr, AeroTable2d* aTablePtr)
{
   aStreamPtr->setRealNumberPrecision(17);
   std::vector<std::pair<double, double>> curData;
   aTablePtr->GetModifiedData(curData);
   for (auto curPrimary : curData)
   {
      *aStreamPtr << "      ";
      OutputData(*aStreamPtr, curPrimary.first, 8, 4, QTextStream::FieldAlignment::AlignRight);
      *aStreamPtr << "  ";
      OutputData(*aStreamPtr, curPrimary.second, 24, 15, QTextStream::FieldAlignment::AlignRight);
      *aStreamPtr << "\n";
   }

   aStreamPtr->flush();
}

void ScriptGeneratorSixDOF::CreateAeroComponentTables(QString& aAeroDir, VehicleAeroMovable aComponent)
{
   // For each component write out their tables...
   const std::vector<std::string> enabledTablesList = aComponent.GetEnabledTables();

   if (enabledTablesList.size() == 0)
   {
      // No enabled tables, do nothing
      return;
   }

   QDir aeroTablesDir(aAeroDir);
   QString componentNameStr = QString::fromStdString(aComponent.GetName()).replace(" ", "_");
   aeroTablesDir.mkdir(componentNameStr);
   aeroTablesDir.cd(componentNameStr);

   for (const auto& curTableStr : enabledTablesList)
   {
      // Lift force
      if (curTableStr == "CL vs Angle Alpha Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCL_AngleAlphaMachTablePtr());
      }

      // Drag force
      else if (curTableStr == "Cd vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCd_AngleMachTablePtr()); // This should be used when drag is a function of alpha and beta
      }

      // Side force
      else if (curTableStr == "CY vs Angle Beta Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCY_AngleBetaMachTablePtr());
      }

      // Pitching moment
      else if (curTableStr == "Cm vs Angle Alpha Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCm_AngleAlphaMachTablePtr());
      }

      // Yawing moment
      else if (curTableStr == "Cn vs Angle Beta Mach")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCn_AngleBetaMachTablePtr());
      }

      // Rolling moment
      else if (curTableStr == "Cl vs Angle Alpha Beta")
      {
         Create4dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCl_AngleAlphaBetaTablePtr());
      }

      // Pitching moment due to pitch rate
      else if (curTableStr == "Cmq vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCmq_AngleMachTablePtr());
      }

      // Yaw moment due to yaw rate
      else if (curTableStr == "Cnr vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetCnr_AngleMachTablePtr());
      }

      // Roll moment due to roll rate
      else if (curTableStr == "Clp vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetClp_AngleMachTablePtr());
      }

      // Roll moment due to pitch rate
      else if (curTableStr == "Clq vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetClq_AngleMachTablePtr());
      }

      // Roll moment due to yaw rate
      else if (curTableStr == "Clr vs Angle Mach")
      {
         Create3dTableFile(aeroTablesDir.absolutePath(), aComponent.GetClr_AngleMachTablePtr());
      }

   }
}

void ScriptGeneratorSixDOF::CreateStubAutopilotSupportFile(QString& aControlsDir)
{
   QFile apSupportFile(aControlsDir + "/autopilot_support_tables" + GetFilenameSuffix() + ".txt");
   if (!apSupportFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&apSupportFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "autopilot_support_tables\n";
   outStream << "\n";
   outStream << "   cl_max_mach_table\n";
   outStream << "      0.000    0.934\n";
   outStream << "      1.000    0.931\n";
   outStream << "      50.000   0.909\n";
   outStream << "   end_cl_max_mach_table\n";
   outStream << "\n";
   outStream << "   cl_min_mach_table\n";
   outStream << "      0.000   -0.934\n";
   outStream << "      1.000   -0.931\n";
   outStream << "      50.000  -0.909\n";
   outStream << "   end_cl_min_mach_table\n";
   outStream << "\n";
   outStream << "   alpha_max_mach_table\n";
   outStream << "      0.000    26.010\n";
   outStream << "      1.000    26.010\n";
   outStream << "      50.000   26.010\n";
   outStream << "   end_alpha_max_mach_table\n";
   outStream << "\n";
   outStream << "   alpha_min_mach_table\n";
   outStream << "      0.000   -26.010\n";
   outStream << "      1.000   -26.010\n";
   outStream << "      50.000  -26.010\n";
   outStream << "   end_alpha_min_mach_table\n";
   outStream << "\n";
   outStream << "   alpha_versus_mach_cl_table\n";
   outStream << "      irregular_table\n";
   outStream << "      independent_variable mach precision float\n";
   outStream << "      independent_variable cl precision float\n";
   outStream << "      dependent_variable precision float\n";
   outStream << "      mach   0.00\n";
   outStream << "         cl     -0.9340  0.000  0.9340\n";
   outStream << "         values -26.010  0.000  26.010\n";
   outStream << "      mach   1.00\n";
   outStream << "         cl     -0.9340  0.000  0.9340\n";
   outStream << "         values -26.010  0.000  26.010\n";
   outStream << "      mach  50.00\n";
   outStream << "         cl     -0.9340  0.000  0.9340\n";
   outStream << "         values -26.010  0.000  26.010\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_alpha_versus_mach_cl_table\n";
   outStream << "\n";
   outStream << "   stick_zero_moment_mach_alpha_table\n";
   outStream << "      irregular_table\n";
   outStream << "      independent_variable mach precision float\n";
   outStream << "      independent_variable alpha units deg\n";
   outStream << "      dependent_variable precision float\n";
   outStream << "      mach   0.00\n";
   outStream << "         alpha  -26.00   0.000    26.00\n";
   outStream << "         values -0.828   0.000    0.828\n";
   outStream << "      mach   1.00\n";
   outStream << "         alpha  -26.00   0.000    26.00\n";
   outStream << "         values -0.828   0.000    0.828\n";
   outStream << "      mach  50.00\n";
   outStream << "         alpha  -26.00   0.000    26.00\n";
   outStream << "         values -0.828   0.000    0.828\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_stick_zero_moment_mach_alpha_table\n";
   outStream << "\n";
   outStream << "   effective_CL_versus_mach_alpha_table\n";
   outStream << "      irregular_table\n";
   outStream << "      independent_variable mach precision float\n";
   outStream << "      independent_variable alpha units deg\n";
   outStream << "      dependent_variable precision float\n";
   outStream << "      mach   0.00\n";
   outStream << "         alpha  -26.00  0.000   26.01\n";
   outStream << "         values -0.934  0.000   0.934\n";
   outStream << "      mach   1.00\n";
   outStream << "         alpha  -26.00  0.000   26.01\n";
   outStream << "         values -0.934  0.000   0.934\n";
   outStream << "      mach  50.00\n";
   outStream << "         alpha  -26.00  0.000   26.01\n";
   outStream << "         values -0.934  0.000   0.934\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_effective_CL_versus_mach_alpha_table\n";
   outStream << "\n";
   outStream << "end_autopilot_support_tables\n";
   outStream << "\n";

   apSupportFile.close();
}

void ScriptGeneratorSixDOF::CreateFlightControlsFile(QString& aControlsDir)
{
   QFile controlFile(aControlsDir + "/flight_controls" + GetFilenameSuffix() + ".txt");
   if (!controlFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&controlFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "flight_controls\n";
   outStream << "\n";
   outStream << "mapping_table SignalMappingTableName\n";
   outStream << "   type signal_mapping\n";
   outStream << "   table_data\n";
   outStream << "      irregular_table\n";
   outStream << "         independent_variable control_value precision float\n";
   outStream << "         independent_variable input         precision float\n";
   outStream << "         dependent_variable                 precision float\n";
   outStream << "         control_value 0.0\n";
   outStream << "            input -1.0 -0.5  0.0  0.5  1.0\n";
   outStream << "            values -0.8 -0.4  0.0  0.4  0.8\n";
   outStream << "         control_value 1.0\n";
   outStream << "            input -1.0 -0.5  0.0  0.5  1.0\n";
   outStream << "            values -0.8 -0.4  0.0  0.4  0.8\n";
   outStream << "         control_value 2.0\n";
   outStream << "            input -1.0 -0.5  0.0  0.5  1.0\n";
   outStream << "            values -0.8 -0.4  0.0  0.4  0.8\n";
   outStream << "      end_irregular_table\n";
   outStream << "   end_table_data\n";
   outStream << "end_mapping_table\n";
   outStream << "\n";
   outStream << "scalar_gain NegativeGain\n";
   outStream << "   gain -1.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain HalfGain\n";
   outStream << "   gain 0.5\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain DoubleGain\n";
   outStream << "   gain 2.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain60\n";
   outStream << "   gain 60.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain90\n";
   outStream << "   gain 90.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain UnityGain\n";
   outStream << "   gain 1.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain ZeroGain\n";
   outStream << "   gain 0.0\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain TenthGain\n";
   outStream << "   gain 0.1\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain90Percent\n";
   outStream << "   gain 0.9\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain80Percent\n";
   outStream << "   gain 0.8\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain70Percent\n";
   outStream << "   gain 0.7\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain60Percent\n";
   outStream << "   gain 0.6\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain40Percent\n";
   outStream << "   gain 0.4\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain30Percent\n";
   outStream << "   gain 0.3\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain20Percent\n";
   outStream << "   gain 0.2\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain10Percent\n";
   outStream << "   gain 0.1\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "scalar_gain Gain5Percent\n";
   outStream << "   gain 0.05\n";
   outStream << "end_scalar_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_PosNeg\n";
   outStream << "   min_clamp -1.0\n";
   outStream << "   max_clamp  1.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_ZeroOne\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  1.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_NegOneZero\n";
   outStream << "   min_clamp  -1.0\n";
   outStream << "   max_clamp  0.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_Zero90\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  90.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain Clamp_Zero60\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  60.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "clamp_gain TestItem\n";
   outStream << "   min_clamp  0.0\n";
   outStream << "   max_clamp  2.0\n";
   outStream << "end_clamp_gain\n";
   outStream << "\n";
   outStream << "gain_table MachGainTableName\n";
   outStream << "  type mach_gain\n";
   outStream << "  simple_table\n";
   outStream << "   0.00          1.0\n";
   outStream << "   1.00          0.5\n";
   outStream << "   2.00          0.1\n";
   outStream << "  end_simple_table\n";
   outStream << "end_gain_table\n";
   outStream << "\n";
   outStream << "gain_table AlphaGainTableName\n";
   outStream << "  type alpha_gain\n";
   outStream << "  simple_table\n";
   outStream << "   -12.0         0.0\n";
   outStream << "   -10.0         1.0\n";
   outStream << "   0.00          1.0\n";
   outStream << "   10.0          1.0\n";
   outStream << "   12.0          0.8\n";
   outStream << "   20.0          0.1\n";
   outStream << "   30.0          0.0\n";
   outStream << "  end_simple_table\n";
   outStream << "end_gain_table\n";
   outStream << "\n";
   outStream << "gain_table BetaGainTableName\n";
   outStream << "  type beta_gain\n";
   outStream << "  simple_table\n";
   outStream << "   0.00          1.0\n";
   outStream << "   1.00          0.5\n";
   outStream << "   2.00          0.1\n";
   outStream << "  end_simple_table\n";
   outStream << "end_gain_table\n";
   outStream << "\n";

   // Output the control data for each movable component
   OutputControlDataForMovableComponents(outStream);

   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_boolean NWS_EnableActive\n";
   outStream << "  current_value              0\n";
   outStream << "  threshold_value            0.5\n";
   outStream << "  inputs\n";
   outStream << "   control_input NWS_Enable\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << "end_control_boolean\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value Wheel_Brake_Rgt\n";
   outStream << " min_value                  0.0\n";
   outStream << " max_value                  1.0\n";
   outStream << " current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input RgtWheelBrake\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value LeftWheelBrake\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input LftWheelBrake\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ParkingBrake\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input SpeedBrakesOut\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrottleMilitary\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrottleMil\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrottleAfterburner\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrottleAB\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrottleThrustReverser\n";
   outStream << "  min_value                  0.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrustReverser\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrustVectoringYaw\n";

   VehiclePropulsion* propulsion = mVehiclePtr->GetPropulsion();

   if (propulsion)
   {
      Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
      GeometryPropulsionData* propData = vehicleGeometry.GetPropulsionData();

      if (propData)
      {
         double minimumYawThrustVectorAngle;
         double maximumYawThrustVectorAngle;

         if (propData->GetMinimumThrustVectoringYawAngle_deg(minimumYawThrustVectorAngle))
         {
            outStream << "  min_value                  " << minimumYawThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  min_value                  0.0\n";
         }

         if (propData->GetMaximumThrustVectoringYawAngle_deg(maximumYawThrustVectorAngle))
         {
            outStream << "  max_value                  " << maximumYawThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  max_value                  0.0\n";
         }
      }
   }

   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input RudderRight\n";
   outStream << "    modifier                 Gain60\n";
   outStream << "    modifier                 NegativeGain\n";
   outStream << "    modifier                 DoubleGain\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrustVectoringPitch\n";

   if (propulsion)
   {
      Designer::VehicleGeometry& vehicleGeometry = mVehiclePtr->GetGeometry();
      GeometryPropulsionData* propData = vehicleGeometry.GetPropulsionData();

      if (propData)
      {
         double minimumPitchThrustVectorAngle;
         double maximumPitchThrustVectorAngle;

         if (propData->GetMinimumThrustVectoringPitchAngle_deg(minimumPitchThrustVectorAngle))
         {
            outStream << "  min_value                  " << minimumPitchThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  min_value                  0.0\n";
         }

         if (propData->GetMaximumThrustVectoringPitchAngle_deg(maximumPitchThrustVectorAngle))
         {
            outStream << "  max_value                  " << maximumPitchThrustVectorAngle << "\n";
         }
         else
         {
            outStream << "  max_value                  0.0\n";
         }
      }
   }

   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input StickBack\n";
   outStream << "    modifier                 Gain60\n";
   outStream << "    modifier                 NegativeGain\n";
   outStream << "    modifier                 DoubleGain\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_value ThrustVectoringRoll\n";
   outStream << "  min_value                  -1.0\n";
   outStream << "  max_value                  1.0\n";
   outStream << "  current_value              0.0\n";
   outStream << "  inputs\n";
   outStream << "   control_input ThrustVectorRoll\n";
   outStream << "    modifier                 Clamp_PosNeg\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_value\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_boolean TriggerPulled\n";
   outStream << "  current_value              0\n";
   outStream << "  threshold_value            0.5\n";
   outStream << "  inputs\n";
   outStream << "   control_input TriggerPulled\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_boolean\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << " control_boolean ButtonPressed\n";
   outStream << "  current_value              0\n";
   outStream << "  threshold_value            0.5\n";
   outStream << "  inputs\n";
   outStream << "   control_input ButtonPressed\n";
   outStream << "    modifier                 Clamp_ZeroOne\n";
   outStream << "   end_control_input\n";
   outStream << "  end_inputs\n";
   outStream << " end_control_boolean\n";
   outStream << "\n";
   outStream << "#--------------------------------------------------------------------------------\n";
   outStream << "end_flight_controls\n";
   outStream << "\n";

   controlFile.close();
}

void ScriptGeneratorSixDOF::OutputControlDataForMovableComponents(QTextStream& aOutputStream)
{
   // The concept of "hooking up controls" is to first determine the type of control configuration
   Vehicle::VehicleControlConfig controlConfig = mVehiclePtr->GetVehicleControlConfiguration();

   switch (controlConfig)
   {
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_TWO_TAILS:
         OutputConfigStabsVerticalTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_VENTRAL_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_TWO_VENTRAL_TAILS:
         OutputConfigStabsVentralTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_UPPER_LOWER_TAILS:
         OutputConfigStabsUpperLowerTails(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_VTAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_INVERTED_VTAIL:
         OutputConfigVTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_ONE_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_TWO_TAILS:
         OutputConfigCanardsVerticalTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_STABS_ONE_TAIL:
      case Vehicle::VehicleControlConfig::cCONFIG_CANARDS_STABS_TWO_TAILS:
         OutputConfigCanardsStabsVerticalTail(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_TAIL_FINS:
         OutputConfigTailFins(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_MID_FINS:
         OutputConfigMidFins(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_CANARD_FINS:
         OutputConfigCanardFins(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_FLYING_WING:
         OutputConfigFlyingWing(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cCONFIG_STABS_FLYING_WING:
         OutputConfigStabsFlyingWing(aOutputStream);
         break;
      case Vehicle::VehicleControlConfig::cNO_CONTROL:
         break;
      default:
         ut::log::error() << "Unknown control config.";
   }

   // Output speed brakes (this function will check internally whether speed brakes are present)
   OutputSpeedBrakes(aOutputStream);

   // Output landing gear (this function will check internally whether landing gear are present)
   OutputLandingGear(aOutputStream);
}

void ScriptGeneratorSixDOF::OutputConfigStabsVerticalTail(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigStabsVentralTail(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVentralTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigStabsUpperLowerTails(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputVentralTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigVTail(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigCanardsVerticalTail(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputCanardControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigCanardsStabsVerticalTail(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   // Verify bank-to-turn
   if (!mVehiclePtr->UsesBankToTurnControl())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks bank-to-turn control!");
      return;
   }

   if (!OutputVerticalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }
   if (!OutputCanardControl(aOutputStream, geometry, controls))
   {
      return;
   }
   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigFlyingWing(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorSixDOF::OutputConfigStabsFlyingWing(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   if (!OutputHorizontalTailControl(aOutputStream, geometry, controls))
   {
      return;
   }

   OutputWingControl(aOutputStream, geometry, controls);
}

bool ScriptGeneratorSixDOF::OutputAileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetAileronsPresent())
   {
      return false;
   }

   // Setup aileron data
   double minAngle_deg                = wing->GetAileronsControlSurfaceMinAngle_deg();
   double maxAngle_deg                = wing->GetAileronsControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = wing->GetAileronsActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = wing->GetAileronsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetAileronsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetAileronsActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = wing->GetAileronsUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = false;
   bool useRudderRight = false;
   bool useStickRight = false;
   bool useSpeedBrakesOut = false;
   std::vector<QString> rightAileronStickBackModifierList;
   std::vector<QString>  leftAileronStickBackModifierList;
   std::vector<QString> rightAileronRudderRightModifierList;
   std::vector<QString>  leftAileronRudderRightModifierList;
   std::vector<QString> rightAileronStickRightModifierList;
   std::vector<QString>  leftAileronStickRightModifierList;
   std::vector<QString> rightAileronSpeedBrakesOutModifierList;
   std::vector<QString>  leftAileronSpeedBrakesOutModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         useStickRight = true;
         rightAileronStickRightModifierList.emplace_back(strClampPosNeg);
         rightAileronStickRightModifierList.emplace_back(strNegativeGain);
         leftAileronStickRightModifierList.emplace_back(strClampPosNeg);
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;
         rightAileronStickRightModifierList.emplace_back(strClampPosNeg);
         rightAileronStickRightModifierList.emplace_back(strNegativeGain);
         leftAileronStickRightModifierList.emplace_back(strClampPosNeg);
         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Aileron_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightAileronStickBackModifierList,
                        useStickRight,
                        rightAileronStickRightModifierList,
                        useRudderRight,
                        rightAileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightAileronSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Aileron_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftAileronStickBackModifierList,
                        useStickRight,
                        leftAileronStickRightModifierList,
                        useRudderRight,
                        leftAileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftAileronSpeedBrakesOutModifierList);

   return true;
}

bool ScriptGeneratorSixDOF::OutputSpoileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetSpoileronsPresent())
   {
      return false;
   }

   // Setup spoileron data
   double minAngle_deg                = wing->GetSpoileronsControlSurfaceMinAngle_deg();
   double maxAngle_deg                = wing->GetSpoileronsControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = wing->GetSpoileronsActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = wing->GetSpoileronsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetSpoileronsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetSpoileronsActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = wing->GetSpoileronsUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = false;
   bool useRudderRight = false;
   bool useStickRight = false;
   bool useSpeedBrakesOut = true;
   std::vector<QString> rightSpoileronStickBackModifierList;
   std::vector<QString>  leftSpoileronStickBackModifierList;
   std::vector<QString> rightSpoileronRudderRightModifierList;
   std::vector<QString>  leftSpoileronRudderRightModifierList;
   std::vector<QString> rightSpoileronStickRightModifierList;
   std::vector<QString>  leftSpoileronStickRightModifierList;
   std::vector<QString> rightSpoileronSpeedBrakesOutModifierList;
   std::vector<QString>  leftSpoileronSpeedBrakesOutModifierList;

   QString strZeroOne      = "modifier        Clamp_ZeroOne";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         useStickRight = true;
         rightSpoileronStickRightModifierList.emplace_back(strZeroOne);
         leftSpoileronStickRightModifierList.emplace_back(strNegativeGain);
         leftSpoileronStickRightModifierList.emplace_back(strZeroOne);
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;
         rightSpoileronStickRightModifierList.emplace_back(strZeroOne);
         leftSpoileronStickRightModifierList.emplace_back(strNegativeGain);
         leftSpoileronStickRightModifierList.emplace_back(strZeroOne);
         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   rightSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);
   leftSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoileron_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightSpoileronStickBackModifierList,
                        useStickRight,
                        rightSpoileronStickRightModifierList,
                        useRudderRight,
                        rightSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightSpoileronSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoileron_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftSpoileronStickBackModifierList,
                        useStickRight,
                        leftSpoileronStickRightModifierList,
                        useRudderRight,
                        leftSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftSpoileronSpeedBrakesOutModifierList);

   return true;
}

bool ScriptGeneratorSixDOF::OutputSpoilerControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetSpoilersPresent())
   {
      return false;
   }

   // Setup spoiler data
   double minAngle_deg                = wing->GetSpoilersControlSurfaceMinAngle_deg();
   double maxAngle_deg                = wing->GetSpoilersControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = wing->GetSpoilersActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = wing->GetSpoilersActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetSpoilersActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetSpoilersActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = wing->GetSpoilersUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = false;
   bool useRudderRight = false;
   bool useStickRight = false;
   bool useSpeedBrakesOut = true;
   std::vector<QString> rightSpoileronStickBackModifierList;
   std::vector<QString>  leftSpoileronStickBackModifierList;
   std::vector<QString> rightSpoileronRudderRightModifierList;
   std::vector<QString>  leftSpoileronRudderRightModifierList;
   std::vector<QString> rightSpoileronStickRightModifierList;
   std::vector<QString>  leftSpoileronStickRightModifierList;
   std::vector<QString> rightSpoileronSpeedBrakesOutModifierList;
   std::vector<QString>  leftSpoileronSpeedBrakesOutModifierList;

   QString strZeroOne      = "modifier        Clamp_ZeroOne";

   rightSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);
   leftSpoileronSpeedBrakesOutModifierList.emplace_back(strZeroOne);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoiler_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightSpoileronStickBackModifierList,
                        useStickRight,
                        rightSpoileronStickRightModifierList,
                        useRudderRight,
                        rightSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightSpoileronSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Spoiler_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftSpoileronStickBackModifierList,
                        useStickRight,
                        leftSpoileronStickRightModifierList,
                        useRudderRight,
                        leftSpoileronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftSpoileronSpeedBrakesOutModifierList);

   return true;
}

bool ScriptGeneratorSixDOF::OutputDrageronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetDrageronsPresent())
   {
      return false;
   }

   // Setup drageron data
   double minAngle_deg = wing->GetDrageronsControlSurfaceMinAngle_deg();
   double maxAngle_deg = wing->GetDrageronsControlSurfaceMaxAngle_deg();
   double currentAngle_deg = 0.0;
   double minActuatorAngle_deg = wing->GetDrageronsActuatorMinAngle_deg();
   double maxActuatorAngle_deg = wing->GetDrageronsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetDrageronsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetDrageronsActuatorMaxRate_dps();
   double currentActuatorAngle_deg = 0.0;
   bool   useExpo = wing->GetDrageronsUseExponentialAngleMapping();

   bool useStickBack = false;
   bool useRudderRight = true;
   bool useStickRight = true;
   bool useSpeedBrakesOut = true;

   // -------------------- Top surfaces -------------------- //

   std::vector<QString>  rightTopDrageronStickBackModifierList;
   std::vector<QString>   leftTopDrageronStickBackModifierList;
   std::vector<QString>  rightTopDrageronRudderRightModifierList;
   std::vector<QString>   leftTopDrageronRudderRightModifierList;
   std::vector<QString>  rightTopDrageronStickRightModifierList;
   std::vector<QString>   leftTopDrageronStickRightModifierList;
   std::vector<QString>  rightTopDrageronSpeedBrakesOutModifierList;
   std::vector<QString>   leftTopDrageronSpeedBrakesOutModifierList;

   QString strClampZeroOne = "modifier        Clamp_ZeroOne";
   QString strClampPosNeg = "modifier        Clamp_PosNeg";
   QString strClampNegOneZero = "modifier        Clamp_NegOneZero";
   QString strNegativeGain = "modifier        NegativeGain";

   // stick back
   rightTopDrageronStickBackModifierList.push_back(strClampPosNeg);
   rightTopDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   rightTopDrageronStickRightModifierList.push_back(strClampPosNeg);
   rightTopDrageronStickRightModifierList.push_back(strNegativeGain);

   // rudder right
   rightTopDrageronRudderRightModifierList.push_back(strClampNegOneZero);
   rightTopDrageronRudderRightModifierList.push_back(strNegativeGain);

   // speed brakes out
   rightTopDrageronSpeedBrakesOutModifierList.push_back(strClampNegOneZero);
   rightTopDrageronSpeedBrakesOutModifierList.push_back(strNegativeGain);

   // Output the top right drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Right_Top",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightTopDrageronStickBackModifierList,
                        useStickRight,
                        rightTopDrageronStickRightModifierList,
                        useRudderRight,
                        rightTopDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightTopDrageronSpeedBrakesOutModifierList);

   // stick back
   leftTopDrageronStickBackModifierList.push_back(strClampPosNeg);
   leftTopDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   leftTopDrageronStickRightModifierList.push_back(strClampPosNeg);

   // rudder right
   leftTopDrageronRudderRightModifierList.push_back(strClampZeroOne);

   // speed brakes out
   leftTopDrageronSpeedBrakesOutModifierList.push_back(strClampNegOneZero);
   leftTopDrageronSpeedBrakesOutModifierList.push_back(strNegativeGain);

   // Output the top left drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Left_Top",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftTopDrageronStickBackModifierList,
                        useStickRight,
                        leftTopDrageronStickRightModifierList,
                        useRudderRight,
                        leftTopDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftTopDrageronSpeedBrakesOutModifierList);

   // -------------------- Bottom surfaces -------------------- //

   std::vector<QString>  rightBottomDrageronStickBackModifierList;
   std::vector<QString>   leftBottomDrageronStickBackModifierList;
   std::vector<QString>  rightBottomDrageronRudderRightModifierList;
   std::vector<QString>   leftBottomDrageronRudderRightModifierList;
   std::vector<QString>  rightBottomDrageronStickRightModifierList;
   std::vector<QString>   leftBottomDrageronStickRightModifierList;
   std::vector<QString>  rightBottomDrageronSpeedBrakesOutModifierList;
   std::vector<QString>   leftBottomDrageronSpeedBrakesOutModifierList;

   // stick back
   rightBottomDrageronStickBackModifierList.push_back(strClampPosNeg);
   rightBottomDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   rightBottomDrageronStickRightModifierList.push_back(strClampPosNeg);
   rightBottomDrageronStickRightModifierList.push_back(strNegativeGain);

   // rudder right
   rightBottomDrageronRudderRightModifierList.push_back(strClampZeroOne);

   // speed brakes out
   rightBottomDrageronSpeedBrakesOutModifierList.push_back(strClampZeroOne);

   // Output the top right drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Right_Bottom",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightBottomDrageronStickBackModifierList,
                        useStickRight,
                        rightBottomDrageronStickRightModifierList,
                        useRudderRight,
                        rightBottomDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightBottomDrageronSpeedBrakesOutModifierList);

   // stick back
   leftBottomDrageronStickBackModifierList.push_back(strClampPosNeg);
   leftBottomDrageronStickBackModifierList.push_back(strNegativeGain);

   // stick right
   leftBottomDrageronStickRightModifierList.push_back(strClampPosNeg);

   // rudder right
   leftBottomDrageronRudderRightModifierList.push_back(strClampNegOneZero);
   leftBottomDrageronRudderRightModifierList.push_back(strNegativeGain);

   // speed brakes out
   leftBottomDrageronSpeedBrakesOutModifierList.push_back(strClampZeroOne);


   // Output the top left drageron
   OutputControlSurface(aOutputStream,
                        "Drageron_Left_Bottom",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftBottomDrageronStickBackModifierList,
                        useStickRight,
                        leftBottomDrageronStickRightModifierList,
                        useRudderRight,
                        leftBottomDrageronRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftBottomDrageronSpeedBrakesOutModifierList);

   return true;
}

bool ScriptGeneratorSixDOF::OutputElevonControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometryWing* wing = geometry.GetWing();
   // We check for a null wing pointer in OutputWingControl

   if (!wing->GetElevonsPresent())
   {
      return false;
   }

   // Setup elevon data
   double minAngle_deg = wing->GetElevonsControlSurfaceMinAngle_deg();
   double maxAngle_deg = wing->GetElevonsControlSurfaceMaxAngle_deg();
   double currentAngle_deg = 0.0;
   double minActuatorAngle_deg = wing->GetElevonsActuatorMinAngle_deg();
   double maxActuatorAngle_deg = wing->GetElevonsActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = wing->GetElevonsActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = wing->GetElevonsActuatorMaxRate_dps();
   double currentActuatorAngle_deg = 0.0;
   bool useExpo = wing->GetElevonsUseExponentialAngleMapping();

   bool useStickBack = true;
   bool useRudderRight = false;
   bool useStickRight = true;
   bool useSpeedBrakesOut = false;

   std::vector<QString> rightElevonStickBackModifierList;
   std::vector<QString>  leftElevonStickBackModifierList;
   std::vector<QString> rightElevonRudderRightModifierList;
   std::vector<QString>  leftElevonRudderRightModifierList;
   std::vector<QString> rightElevonStickRightModifierList;
   std::vector<QString>  leftElevonStickRightModifierList;
   std::vector<QString> rightElevonSpeedBrakesOutModifierList;
   std::vector<QString>  leftElevonSpeedBrakesOutModifierList;

   QString strNegativeGain = "modifier        NegativeGain";
   QString strClampPosNeg = "modifier        Clamp_PosNeg";

   rightElevonStickBackModifierList.push_back(strClampPosNeg);
   rightElevonStickBackModifierList.push_back(strNegativeGain);
   leftElevonStickBackModifierList.push_back(strClampPosNeg);
   leftElevonStickBackModifierList.push_back(strNegativeGain);


   rightElevonStickRightModifierList.push_back(strClampPosNeg);
   rightElevonStickRightModifierList.push_back(strNegativeGain);
   leftElevonStickRightModifierList.push_back(strClampPosNeg);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Elevon_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightElevonStickBackModifierList,
                        useStickRight,
                        rightElevonStickRightModifierList,
                        useRudderRight,
                        rightElevonRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightElevonSpeedBrakesOutModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Elevon_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftElevonStickBackModifierList,
                        useStickRight,
                        leftElevonStickRightModifierList,
                        useRudderRight,
                        leftElevonRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftElevonSpeedBrakesOutModifierList);

   return true;

}

bool ScriptGeneratorSixDOF::OutputWingControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometryWing* wing = geometry.GetWing();
   if (wing == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Wing!");
      return false;
   }

   OutputAileronControl(aOutputStream, geometry, controls);
   OutputElevonControl(aOutputStream, geometry, controls);
   OutputSpoileronControl(aOutputStream, geometry, controls);
   OutputSpoilerControl(aOutputStream, geometry, controls);
   OutputDrageronControl(aOutputStream, geometry, controls);

   return true;
}

bool ScriptGeneratorSixDOF::OutputHorizontalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometrySurface* horizontalStab = geometry.GetHorizontalStab();
   if (horizontalStab == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Horizontal Stab!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = horizontalStab->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = horizontalStab->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = horizontalStab->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = horizontalStab->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = horizontalStab->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = horizontalStab->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = horizontalStab->GetUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = true;
   bool useRudderRight = false;
   bool useStickRight = false;
   std::vector<QString> rightStabStickBackModifierList;
   std::vector<QString>  leftStabStickBackModifierList;
   std::vector<QString> rightStabRudderRightModifierList;
   std::vector<QString>  leftStabRudderRightModifierList;
   std::vector<QString> rightStabStickRightModifierList;
   std::vector<QString>  leftStabStickRightModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";
   QString strHalfGain     = "modifier        HalfGain";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);

         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);
         rightStabStickRightModifierList.emplace_back(strHalfGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);
         leftStabStickRightModifierList.emplace_back(strHalfGain);

         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   rightStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      rightStabStickBackModifierList.emplace_back(strAlphaGain);
   }
   rightStabStickBackModifierList.emplace_back(strNegativeGain);

   leftStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      leftStabStickBackModifierList.emplace_back(strAlphaGain);
   }
   leftStabStickBackModifierList.emplace_back(strNegativeGain);

   // Output the right horizontal stab
   OutputControlSurface(aOutputStream,
                        "Horizontal_Stab_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightStabStickBackModifierList,
                        useStickRight,
                        rightStabStickRightModifierList,
                        useRudderRight,
                        rightStabRudderRightModifierList);

   // Output the left horizontal stab
   OutputControlSurface(aOutputStream,
                        "Horizontal_Stab_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftStabStickBackModifierList,
                        useStickRight,
                        leftStabStickRightModifierList,
                        useRudderRight,
                        leftStabRudderRightModifierList);

   return true;
}

bool ScriptGeneratorSixDOF::OutputVerticalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometrySurface* verticalTail = geometry.GetVerticalTail();
   if (verticalTail == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Vertical Tail!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = verticalTail->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = verticalTail->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = verticalTail->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = verticalTail->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = verticalTail->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = verticalTail->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = verticalTail->GetUseExponentialAngleMapping();

   bool useStickBack = false;
   bool useRudderRight = true;
   bool useStickRight = false;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";

   std::string symmetryString = verticalTail->GetSymmetryString().toStdString();
   if (symmetryString == "Horizontal")
   {
      bool useSpeedBrakesOut = false;
      std::vector<QString> rightTailStickBackModifierList;
      std::vector<QString> rightTailRudderRightModifierList;
      std::vector<QString> rightTailStickRightModifierList;
      std::vector<QString> rightTailSpeedBrakesOutModifierList;

      std::vector<QString> leftTailStickBackModifierList;
      std::vector<QString> leftTailRudderRightModifierList;
      std::vector<QString> leftTailStickRightModifierList;
      std::vector<QString> leftTailSpeedBrakesOutModifierList;

      rightTailRudderRightModifierList.emplace_back(strClampPosNeg);
      leftTailRudderRightModifierList.emplace_back(strNegativeGain);
      leftTailRudderRightModifierList.emplace_back(strClampPosNeg);

      rightTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      leftTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      leftTailSpeedBrakesOutModifierList.emplace_back(strNegativeGain);

      // Output the right vertical tail
      OutputControlSurface(aOutputStream,
                           "Vertical_Tail_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           rightTailStickBackModifierList,
                           useStickRight,
                           rightTailStickRightModifierList,
                           useRudderRight,
                           rightTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           rightTailSpeedBrakesOutModifierList);

      // Output the left vertical tail
      OutputControlSurface(aOutputStream,
                           "Vertical_Tail_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           leftTailStickBackModifierList,
                           useStickRight,
                           leftTailStickRightModifierList,
                           useRudderRight,
                           leftTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           leftTailSpeedBrakesOutModifierList);
   }
   else
   {
      std::vector<QString> stickBackModifierList;
      std::vector<QString> rudderRightModifierList;
      std::vector<QString> stickRightModifierList;
      rudderRightModifierList.emplace_back(strClampPosNeg);

      // Output the vertical tail
      OutputControlSurface(aOutputStream,
                           "Vertical_Tail",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   return true;
}

bool ScriptGeneratorSixDOF::OutputVentralTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometrySurface* ventralTail = geometry.GetVentralTail();
   if (ventralTail == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Ventral Tail!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = ventralTail->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = ventralTail->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = ventralTail->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = ventralTail->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = ventralTail->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = ventralTail->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = ventralTail->GetUseExponentialAngleMapping();

   bool useStickBack = false;
   bool useRudderRight = true;
   bool useStickRight = false;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";

   std::string symmetryString = ventralTail->GetSymmetryString().toStdString();
   if (symmetryString == "Horizontal")
   {
      bool useSpeedBrakesOut = false;
      std::vector<QString> rightTailStickBackModifierList;
      std::vector<QString> rightTailRudderRightModifierList;
      std::vector<QString> rightTailStickRightModifierList;
      std::vector<QString> rightTailSpeedBrakesOutModifierList;

      std::vector<QString> leftTailStickBackModifierList;
      std::vector<QString> leftTailRudderRightModifierList;
      std::vector<QString> leftTailStickRightModifierList;
      std::vector<QString> leftTailSpeedBrakesOutModifierList;

      rightTailRudderRightModifierList.emplace_back(strClampPosNeg);
      rightTailRudderRightModifierList.emplace_back(strNegativeGain);

      leftTailRudderRightModifierList.emplace_back(strClampPosNeg);

      rightTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      leftTailSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
      rightTailSpeedBrakesOutModifierList.emplace_back(strNegativeGain);

      // Output the right vertical tail
      OutputControlSurface(aOutputStream,
                           "Ventral_Tail_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           rightTailStickBackModifierList,
                           useStickRight,
                           rightTailStickRightModifierList,
                           useRudderRight,
                           rightTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           rightTailSpeedBrakesOutModifierList);

      // Output the left vertical tail
      OutputControlSurface(aOutputStream,
                           "Ventral_Tail_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           leftTailStickBackModifierList,
                           useStickRight,
                           leftTailStickRightModifierList,
                           useRudderRight,
                           leftTailRudderRightModifierList,
                           useSpeedBrakesOut,
                           leftTailSpeedBrakesOutModifierList);
   }
   else
   {
      std::vector<QString> stickBackModifierList;
      std::vector<QString> rudderRightModifierList;
      std::vector<QString> stickRightModifierList;
      rudderRightModifierList.emplace_back(strClampPosNeg);
      rudderRightModifierList.emplace_back(strNegativeGain);

      // Output the vertical tail
      OutputControlSurface(aOutputStream,
                           "Ventral_Tail",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   return true;
}

bool ScriptGeneratorSixDOF::OutputVTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   bool inverted = false;
   GeometrySurface* veeTail = geometry.GetVTail();
   if (veeTail == nullptr)
   {
      veeTail = geometry.GetInvertedVTail();
      inverted = true;
      if (veeTail == nullptr)
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a V-Tail!");
         return false;
      }
   }

   // Setup horizontal stab data
   double minAngle_deg                = veeTail->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = veeTail->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = veeTail->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = veeTail->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = veeTail->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = veeTail->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = veeTail->GetUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = true;
   bool useRudderRight = true;
   bool useStickRight = false;
   bool useSpeedBrakesOut = false;
   std::vector<QString> rightStabStickBackModifierList;
   std::vector<QString>  leftStabStickBackModifierList;
   std::vector<QString> rightStabRudderRightModifierList;
   std::vector<QString>  leftStabRudderRightModifierList;
   std::vector<QString> rightStabStickRightModifierList;
   std::vector<QString>  leftStabStickRightModifierList;
   std::vector<QString> rightStabSpeedBrakesOutModifierList;
   std::vector<QString>  leftStabSpeedBrakesOutModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";
   QString strHalfGain     = "modifier        HalfGain";


   // Pitch control
   rightStabStickBackModifierList.emplace_back(strClampPosNeg);
   rightStabStickBackModifierList.emplace_back(strNegativeGain);
   if (useAlphaControlGain)
   {
      rightStabStickBackModifierList.emplace_back(strAlphaGain);
   }

   leftStabStickBackModifierList.emplace_back(strClampPosNeg);
   leftStabStickBackModifierList.emplace_back(strNegativeGain);
   if (useAlphaControlGain)
   {
      leftStabStickBackModifierList.emplace_back(strAlphaGain);
   }


   // Rudder control
   rightStabRudderRightModifierList.emplace_back(strClampPosNeg);
   leftStabRudderRightModifierList.emplace_back(strClampPosNeg);
   rightStabRudderRightModifierList.emplace_back(strHalfGain);
   leftStabRudderRightModifierList.emplace_back(strHalfGain);
   if (inverted)
   {
      rightStabRudderRightModifierList.emplace_back(strNegativeGain);
   }
   else
   {
      leftStabRudderRightModifierList.emplace_back(strNegativeGain);
   }

   // Brake/flare control
   rightStabSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
   rightStabSpeedBrakesOutModifierList.emplace_back(strHalfGain);
   leftStabSpeedBrakesOutModifierList.emplace_back(strClampPosNeg);
   leftStabSpeedBrakesOutModifierList.emplace_back(strHalfGain);
   if (inverted)
   {
      rightStabSpeedBrakesOutModifierList.emplace_back(strNegativeGain);
   }
   else
   {
      leftStabSpeedBrakesOutModifierList.emplace_back(strNegativeGain);
   }

   QString rightStabName;
   QString leftStabName;

   if (inverted)
   {
      rightStabName = "Inverted_V-Tail_Right";
      leftStabName = "Inverted_V-Tail_Left";
   }
   else
   {
      rightStabName = "V-Tail_Right";
      leftStabName = "V-Tail_Left";
   }

   // Output the right V-tail stab
   OutputControlSurface(aOutputStream,
                        rightStabName,
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightStabStickBackModifierList,
                        useStickRight,
                        rightStabStickRightModifierList,
                        useRudderRight,
                        rightStabRudderRightModifierList,
                        useSpeedBrakesOut,
                        rightStabSpeedBrakesOutModifierList);

   // Output the left V-tail stab
   OutputControlSurface(aOutputStream,
                        leftStabName,
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftStabStickBackModifierList,
                        useStickRight,
                        leftStabStickRightModifierList,
                        useRudderRight,
                        leftStabRudderRightModifierList,
                        useSpeedBrakesOut,
                        leftStabSpeedBrakesOutModifierList);

   return true;
}

bool ScriptGeneratorSixDOF::OutputCanardControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
{
   GeometrySurface* canard = geometry.GetCanard();
   if (canard == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Canard!");
      return false;
   }

   // Setup horizontal stab data
   double minAngle_deg                = canard->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = canard->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = canard->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = canard->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = canard->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = canard->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = canard->GetUseExponentialAngleMapping();

   // Set alpha control gain to false by default
   VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::VehicleControlBankToTurnRollControl::cAILERONS_ONLY;
   VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::VehicleControlBankToTurnYawControlType::cNO_YAW_CONTROL;
   bool useAlphaControlGain = false;

   controls.GetBankToTurnControlSettings(bankControl, yawControlType, useAlphaControlGain);

   bool useStickBack = true;
   bool useRudderRight = false;
   bool useStickRight = false;
   std::vector<QString> rightStabStickBackModifierList;
   std::vector<QString>  leftStabStickBackModifierList;
   std::vector<QString> rightStabRudderRightModifierList;
   std::vector<QString>  leftStabRudderRightModifierList;
   std::vector<QString> rightStabStickRightModifierList;
   std::vector<QString>  leftStabStickRightModifierList;

   QString strClampPosNeg  = "modifier        Clamp_PosNeg";
   QString strNegativeGain = "modifier        NegativeGain";
   QString strAlphaGain    = "modifier        AlphaGainTableName";
   QString strHalfGain     = "modifier        HalfGain";

   switch (bankControl)
   {
      case Designer::VehicleControls::cAILERONS_ONLY:
      {
         // No additional control
         break;
      }
      case Designer::VehicleControls::cSTABILIZERS_ONLY:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);

         break;
      }
      case Designer::VehicleControls::cAILERONS_AND_STABILIZERS:
      {
         useStickRight = true;

         rightStabStickRightModifierList.emplace_back(strClampPosNeg);
         rightStabStickRightModifierList.emplace_back(strNegativeGain);
         rightStabStickRightModifierList.emplace_back(strHalfGain);

         leftStabStickRightModifierList.emplace_back(strClampPosNeg);
         leftStabStickRightModifierList.emplace_back(strHalfGain);

         break;
      }
      default:
         // We can't find a valid control mode, so deliberately fail
         MoverCreatorMainWindow().CreateErrorBox("The bank-to-turn control type is undefined!");
         return false;
   }

   rightStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      rightStabStickBackModifierList.emplace_back(strAlphaGain);
   }

   leftStabStickBackModifierList.emplace_back(strClampPosNeg);
   if (useAlphaControlGain)
   {
      leftStabStickBackModifierList.emplace_back(strAlphaGain);
   }

   // Output the right canard
   OutputControlSurface(aOutputStream,
                        "Canard_Right",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        rightStabStickBackModifierList,
                        useStickRight,
                        rightStabStickRightModifierList,
                        useRudderRight,
                        rightStabRudderRightModifierList);

   // Output the left canard
   OutputControlSurface(aOutputStream,
                        "Canard_Left",
                        minAngle_deg,
                        maxAngle_deg,
                        currentAngle_deg,
                        minActuatorAngle_deg,
                        maxActuatorAngle_deg,
                        maxNegativeActuatorRate_dps,
                        maxPositiveActuatorRate_dps,
                        currentActuatorAngle_deg,
                        useExpo,
                        useStickBack,
                        leftStabStickBackModifierList,
                        useStickRight,
                        leftStabStickRightModifierList,
                        useRudderRight,
                        leftStabRudderRightModifierList);

   return true;
}

void ScriptGeneratorSixDOF::OutputControlSurface(QTextStream&                aOutputStream,
                                                 const QString&              aControlSurfaceName,
                                                 double                      aMinAngle_deg,
                                                 double                      aMaxAngle_deg,
                                                 double                      aCurrentAngle_deg,
                                                 double                      aMinActuatorAngle_deg,
                                                 double                      aMaxActuatorAngle_deg,
                                                 double                      aMaxNegativeActuatorRate_dps,
                                                 double                      aMaxPositiveActuatorRate_dps,
                                                 double                      aCurrentActuatorAngle_deg,
                                                 bool                        aUseExpo,
                                                 bool                        aUseStickBack,
                                                 const std::vector<QString>& aStickBackModifierList,
                                                 bool                        aUseStickRight,
                                                 const std::vector<QString>& aStickRightModifierList,
                                                 bool                        aUseRudderRight,
                                                 const std::vector<QString>& aRudderRightModifierList)
{
   OutputControlSurface(aOutputStream,
                        aControlSurfaceName,
                        aMinAngle_deg,
                        aMaxAngle_deg,
                        aCurrentAngle_deg,
                        aMinActuatorAngle_deg,
                        aMaxActuatorAngle_deg,
                        aMaxNegativeActuatorRate_dps,
                        aMaxPositiveActuatorRate_dps,
                        aCurrentActuatorAngle_deg,
                        aUseExpo,
                        aUseStickBack,
                        aStickBackModifierList,
                        aUseStickRight,
                        aStickRightModifierList,
                        aUseRudderRight,
                        aRudderRightModifierList,
                        false,
                        std::vector<QString>());
}

void ScriptGeneratorSixDOF::OutputControlSurface(QTextStream&                aOutputStream,
                                                 const QString&              aControlSurfaceName,
                                                 double                      aMinAngle_deg,
                                                 double                      aMaxAngle_deg,
                                                 double                      aCurrentAngle_deg,
                                                 double                      aMinActuatorAngle_deg,
                                                 double                      aMaxActuatorAngle_deg,
                                                 double                      aMaxNegativeActuatorRate_dps,
                                                 double                      aMaxPositiveActuatorRate_dps,
                                                 double                      aCurrentActuatorAngle_deg,
                                                 bool                        aUseExpo,
                                                 bool                        aUseStickBack,
                                                 const std::vector<QString>& aStickBackModifierList,
                                                 bool                        aUseStickRight,
                                                 const std::vector<QString>& aStickRightModifierList,
                                                 bool                        aUseRudderRight,
                                                 const std::vector<QString>& aRudderRightModifierList,
                                                 bool                        aUseSpeedBrakesOut,
                                                 const std::vector<QString>& aSpeedBrakesOutModifierList)
{
   // Output format
   aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;
   int width = 8;
   int precision = 2;

   aOutputStream << "#--------------------------------------------------------------------------------\n";
   aOutputStream << "control_surface  " << aControlSurfaceName << "\n";

   aOutputStream << "   min_angle      ";
   OutputData(aOutputStream, aMinAngle_deg, width, precision, alignment);
   aOutputStream << " deg\n";
   aOutputStream << "   max_angle      ";
   OutputData(aOutputStream, aMaxAngle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "   current_angle  ";
   OutputData(aOutputStream, 0.0, width, precision, alignment);
   aOutputStream << " deg\n";

   // Inputs section

   aOutputStream << "   inputs\n";

   if (aUseStickBack)
   {
      OutputControlInputBlock(aOutputStream, "StickBack", aStickBackModifierList);
   }

   if (aUseRudderRight)
   {
      OutputControlInputBlock(aOutputStream, "RudderRight", aRudderRightModifierList);
   }

   if (aUseStickRight)
   {
      OutputControlInputBlock(aOutputStream, "StickRight", aStickRightModifierList);
   }

   if (aUseSpeedBrakesOut)
   {
      OutputControlInputBlock(aOutputStream, "SpeedBrakesOut", aSpeedBrakesOutModifierList);
   }

   aOutputStream << "   end_inputs\n";

   // Angle mapping section

   OutputAngleMappingTable(aOutputStream, aUseExpo, aMinAngle_deg, aMaxAngle_deg);

   // Actuator section

   OutputActuatorData(aOutputStream,
                      aMaxPositiveActuatorRate_dps,
                      aMaxNegativeActuatorRate_dps,
                      aMaxActuatorAngle_deg,
                      aMinActuatorAngle_deg,
                      aCurrentActuatorAngle_deg);

   aOutputStream << "end_control_surface\n";
   aOutputStream << "\n";
}

void ScriptGeneratorSixDOF::OutputActuatorData(QTextStream& aOutputStream,
                                               double       aMax_positive_rate_dps,
                                               double       aMax_negative_rate_dps,
                                               double       aMax_angle_deg,
                                               double       aMin_angle_deg,
                                               double       aCurrent_angle_deg)
{
   // Output format
   aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;
   int width     = 8;
   int precision = 2;

   aOutputStream << "   actuator\n";

   aOutputStream << "      max_positive_rate  ";
   OutputData(aOutputStream, aMax_positive_rate_dps, width, precision, alignment);
   aOutputStream << " deg/sec\n";

   aOutputStream << "      max_negative_rate  ";
   OutputData(aOutputStream, aMax_negative_rate_dps, width, precision, alignment);
   aOutputStream << " deg/sec\n";

   aOutputStream << "      max_angle          ";
   OutputData(aOutputStream, aMax_angle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "      min_angle          ";
   OutputData(aOutputStream, aMin_angle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "      current_angle      ";
   OutputData(aOutputStream, aCurrent_angle_deg, width, precision, alignment);
   aOutputStream << " deg\n";

   aOutputStream << "   end_actuator\n";
}

void ScriptGeneratorSixDOF::OutputControlInputBlock(QTextStream&                aOutputStream,
                                                    const QString&              aControlInputName,
                                                    const std::vector<QString>& aModifierList)
{
   aOutputStream << "      control_input  " << aControlInputName << "\n";

   for (auto& tmpStr : aModifierList)
   {
      aOutputStream << "         " << tmpStr << "\n";
   }

   aOutputStream << "      end_control_input\n";
}

void ScriptGeneratorSixDOF::OutputAngleMappingTable(QTextStream& aOutputStream,
                                                    bool         aUseExponential,
                                                    double       aMinAngle_deg,
                                                    double       aMaxAngle_deg)
{
   // Note: Output format will be set within OutputPairsInAngleMappingTable

   double minAngle_deg = std::abs(aMinAngle_deg);
   double maxAngle_deg = std::abs(aMaxAngle_deg);

   if (aUseExponential)
   {
      aOutputStream << "   angle_mapping_table\n";

      // y = 0.9 * x^3 + 0.1 * x

      std::vector<std::pair<double, double>> tmpVectorPair;
      tmpVectorPair.emplace_back(-1.00, -1.000000 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.95, -0.866650 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.90, -0.746100 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.85, -0.637700 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.80, -0.540800 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.75, -0.454700 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.70, -0.378700 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.65, -0.312150 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.60, -0.254400 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.55, -0.204750 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.50, -0.162500 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.45, -0.127000 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.40, -0.097600 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.35, -0.073600 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.30, -0.054300 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.25, -0.039050 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.20, -0.027200 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.15, -0.018050 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.10, -0.010900 * minAngle_deg);
      tmpVectorPair.emplace_back(-0.05, -0.005100 * minAngle_deg);
      tmpVectorPair.emplace_back(0.00,   0.000000);
      tmpVectorPair.emplace_back(0.05,   0.005100 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.10,   0.010900 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.15,   0.018050 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.20,   0.027200 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.25,   0.039050 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.30,   0.054300 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.35,   0.073600 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.40,   0.097600 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.45,   0.127000 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.50,   0.162500 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.55,   0.204750 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.60,   0.254400 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.65,   0.312150 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.70,   0.378700 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.75,   0.454700 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.80,   0.540800 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.85,   0.637700 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.90,   0.746100 * maxAngle_deg);
      tmpVectorPair.emplace_back(0.95,   0.866650 * maxAngle_deg);
      tmpVectorPair.emplace_back(1.00,   1.000000 * maxAngle_deg);

      OutputPairsInAngleMappingTable(aOutputStream, tmpVectorPair);

      aOutputStream << "   end_angle_mapping_table\n";
   }
   else
   {
      aOutputStream << "   angle_mapping_table\n";

      std::vector<std::pair<double, double>> tmpVectorPair;
      tmpVectorPair.emplace_back(-1.0, -1.000000 * minAngle_deg);
      tmpVectorPair.emplace_back(0.0,  0.000000);
      tmpVectorPair.emplace_back(1.0,  1.000000 * maxAngle_deg);

      OutputPairsInAngleMappingTable(aOutputStream, tmpVectorPair);

      aOutputStream << "   end_angle_mapping_table\n";
   }
}

void ScriptGeneratorSixDOF::OutputPairsInAngleMappingTable(QTextStream&                                  aOutputStream,
                                                           const std::vector<std::pair<double, double>>& aVectorPair)
{
   // Output format
   aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;
   int width     = 12;
   int precision = 6;

   for (auto& pair : aVectorPair)
   {
      aOutputStream << "   ";
      OutputData(aOutputStream, pair.first, width, precision, alignment);
      aOutputStream << "    ";
      OutputData(aOutputStream, pair.second, width, precision, alignment);
      aOutputStream << "\n";
   }
}

void ScriptGeneratorSixDOF::OutputConfigTailFins(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();
   GeometrySurface* tailFin = geometry.GetTailControlFins();

   if (tailFin == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Tail Fin!");
      return;
   }

   // Setup data
   double minAngle_deg                = tailFin->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = tailFin->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = tailFin->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = tailFin->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = tailFin->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = tailFin->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = tailFin->GetUseExponentialAngleMapping();
   std::vector<QString> stickBackModifierList;
   std::vector<QString> rudderRightModifierList;
   std::vector<QString> stickRightModifierList;

   QString symmetryString = tailFin->GetSymmetryString();

   if (symmetryString == "X Pattern")
   {
      // Output the top right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      bool useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_TopRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_TopLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom right fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_BottomRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_BottomLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   else if (symmetryString == "+ Pattern")
   {
      // Output the right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      bool useRudderRight = false;

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Top",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the left fin

      useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      useRudderRight = false;

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Tail_Control_Fin_Bottom",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The specified Tail Fin needs to have a '+' or 'X' pattern!");
      return;
   }
}

void ScriptGeneratorSixDOF::OutputConfigMidFins(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();
   GeometrySurface* midFin = geometry.GetMidControlFins();

   if (midFin == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Mid Fin!");
      return;
   }

   // Setup data
   double minAngle_deg                = midFin->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = midFin->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = midFin->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = midFin->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = midFin->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = midFin->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = midFin->GetUseExponentialAngleMapping();
   std::vector<QString> stickBackModifierList;
   std::vector<QString> rudderRightModifierList;
   std::vector<QString> stickRightModifierList;

   QString symmetryString = midFin->GetSymmetryString();

   if (symmetryString == "X Pattern")
   {
      // Output the top right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_TopRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_TopLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom right fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_BottomRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_BottomLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   else if (symmetryString == "+ Pattern")
   {
      // Output the right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = false;

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Top",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the left fin

      useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      useRudderRight = false;

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Mid_Control_Fin_Bottom",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The specified Mid Fin needs to have a '+' or 'X' pattern!");
      return;
   }
}

void ScriptGeneratorSixDOF::OutputConfigCanardFins(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry  = mVehiclePtr->GetGeometry();
   GeometrySurface* canardFin = geometry.GetCanardControlFins();

   if (canardFin == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a Canard Fin!");
      return;
   }

   // Setup data
   double minAngle_deg                = canardFin->GetControlSurfaceMinAngle_deg();
   double maxAngle_deg                = canardFin->GetControlSurfaceMaxAngle_deg();
   double currentAngle_deg            = 0.0;
   double minActuatorAngle_deg        = canardFin->GetActuatorMinAngle_deg();
   double maxActuatorAngle_deg        = canardFin->GetActuatorMaxAngle_deg();
   double maxNegativeActuatorRate_dps = canardFin->GetActuatorMinRate_dps();
   double maxPositiveActuatorRate_dps = canardFin->GetActuatorMaxRate_dps();
   double currentActuatorAngle_deg    = 0.0;
   bool useExpo                       = canardFin->GetUseExponentialAngleMapping();
   std::vector<QString> stickBackModifierList;
   std::vector<QString> rudderRightModifierList;
   std::vector<QString> stickRightModifierList;

   QString symmetryString = canardFin->GetSymmetryString();

   if (symmetryString == "X Pattern")
   {
      // Output the top right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_TopRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_TopLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom right fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_BottomRight",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom left fin

      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_BottomLeft",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);
   }
   else if (symmetryString == "+ Pattern")
   {
      // Output the right fin

      bool useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");

      bool useRudderRight = false;

      bool useStickRight = true;
      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Right",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the top fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      rudderRightModifierList.emplace_back("modifier        NegativeGain");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Top",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the left fin

      useStickBack = true;
      stickBackModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickBackModifierList.emplace_back("modifier        NegativeGain");

      useRudderRight = false;

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Left",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();

      // Output the bottom fin

      useStickBack = false;

      useRudderRight = true;
      rudderRightModifierList.emplace_back("modifier        Clamp_PosNeg");

      stickRightModifierList.emplace_back("modifier        Clamp_PosNeg");
      stickRightModifierList.emplace_back("modifier        NegativeGain");
      stickRightModifierList.emplace_back("modifier        Gain5Percent");

      OutputControlSurface(aOutputStream,
                           "Canard_Control_Fin_Bottom",
                           minAngle_deg,
                           maxAngle_deg,
                           currentAngle_deg,
                           minActuatorAngle_deg,
                           maxActuatorAngle_deg,
                           maxNegativeActuatorRate_dps,
                           maxPositiveActuatorRate_dps,
                           currentActuatorAngle_deg,
                           useExpo,
                           useStickBack,
                           stickBackModifierList,
                           useStickRight,
                           stickRightModifierList,
                           useRudderRight,
                           rudderRightModifierList);

      // Clear the modifier lists
      stickBackModifierList.clear();
      rudderRightModifierList.clear();
      stickRightModifierList.clear();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The specified Canard Fin needs to have a '+' or 'X' pattern!");
      return;
   }
}

void ScriptGeneratorSixDOF::OutputWingControlSurfaces(QTextStream& aOutputStream)
{
   // TODO: For each control surface that is present, output in a control_surface block
}

void ScriptGeneratorSixDOF::OutputSpeedBrakes(QTextStream& aOutputStream)
{
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   auto aeroObjVector = vehicleAero->GetAeroObjectBasedOnType("AeroSpeedBrake");

   for (auto& aeroObj : aeroObjVector)
   {
      auto speedBrake = dynamic_cast<AeroSpeedBrake*>(aeroObj);

      aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);

      aOutputStream << "#--------------------------------------------------------------------------------\n";
      aOutputStream << "control_surface  " << QString(speedBrake->GetName().c_str()) << "\n";
      aOutputStream << "   min_angle      ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   max_angle      ";
      OutputData(aOutputStream, speedBrake->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   current_angle  ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   inputs\n";
      aOutputStream << "      control_input  SpeedBrakesOut\n";
      aOutputStream << "         modifier  Clamp_PosNeg\n";
      aOutputStream << "      end_control_input\n";
      aOutputStream << "   end_inputs\n";
      aOutputStream << "   angle_mapping_table\n";
      aOutputStream << "       0.000   ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "       1.000   ";
      OutputData(aOutputStream, speedBrake->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "   end_angle_mapping_table\n";
      aOutputStream << "   actuator\n";
      aOutputStream << "      max_positive_rate  ";
      OutputData(aOutputStream, speedBrake->GetMaxExtensionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_negative_rate  ";
      OutputData(aOutputStream, speedBrake->GetMaxRetractionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_angle          ";
      OutputData(aOutputStream, speedBrake->GetMaxAngle_deg(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      min_angle          ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      current_angle      ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   end_actuator\n";
      aOutputStream << "end_control_surface\n";
      aOutputStream << "\n";
   }
}

void ScriptGeneratorSixDOF::OutputLandingGear(QTextStream& aOutputStream)
{
   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   auto aeroObjVector = vehicleAero->GetAeroObjectBasedOnType("AeroLandingGear");

   for (auto& aeroObj : aeroObjVector)
   {
      auto landingGear = dynamic_cast<AeroLandingGear*>(aeroObj);

      aOutputStream.setRealNumberNotation(QTextStream::FixedNotation);

      aOutputStream << "#--------------------------------------------------------------------------------\n";
      aOutputStream << "control_surface  " << QString(landingGear->GetName().c_str()) << "\n";
      aOutputStream << "   min_angle      ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   max_angle      ";
      OutputData(aOutputStream, landingGear->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   current_angle  ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   inputs\n";
      aOutputStream << "      control_input  LandingGearDown\n";
      aOutputStream << "         modifier  Clamp_PosNeg\n";
      aOutputStream << "      end_control_input\n";
      aOutputStream << "   end_inputs\n";
      aOutputStream << "   angle_mapping_table\n";
      aOutputStream << "       0.000   ";
      OutputData(aOutputStream, 0.0, 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "       1.000   ";
      OutputData(aOutputStream, landingGear->GetMaxAngle_deg(), 5, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
      aOutputStream << "   end_angle_mapping_table\n";
      aOutputStream << "   actuator\n";
      aOutputStream << "      max_positive_rate  ";
      OutputData(aOutputStream, landingGear->GetMaxExtensionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_negative_rate  ";
      OutputData(aOutputStream, landingGear->GetMaxRetractionRate_dps(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg/sec\n";
      aOutputStream << "      max_angle          ";
      OutputData(aOutputStream, landingGear->GetMaxAngle_deg(), 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      min_angle          ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "      current_angle      ";
      OutputData(aOutputStream, 0.0, 7, 1, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " deg\n";
      aOutputStream << "   end_actuator\n";
      aOutputStream << "end_control_surface\n";
      aOutputStream << "\n";
   }
}

void ScriptGeneratorSixDOF::CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream)
{
   QDate Today = QDate::currentDate();
   aOutStream << "# ##################################################################################################\n";
   aOutStream << "# \n";
   VersionInfo::OutputVersionStringInOutputStream(aOutStream);
   aOutStream << "# \n";
   aOutStream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << "\n";
   aOutStream << "# \n";
   aOutStream << "# ##################################################################################################\n";
   aOutStream << "\n";
}

QString ScriptGeneratorSixDOF::GenerateControlMethodString()
{
   QString controlMethodStr = "BANK_TO_TURN_NO_YAW";

   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorSixDOF::GenerateControlMethodString().";
      return controlMethodStr;
   }

   // Generate the appropriate control type string
   if (mVehiclePtr->UsesBankToTurnControl())
   {
      VehicleControls::VehicleControlBankToTurnRollControl bankControl = VehicleControls::cAILERONS_ONLY;
      VehicleControls::VehicleControlBankToTurnYawControlType yawControlType = VehicleControls::cNO_YAW_CONTROL;
      bool alphaControlGain = false;

      mVehiclePtr->GetControls().GetBankToTurnControlSettings(bankControl, yawControlType, alphaControlGain);

      switch (yawControlType)
      {
         case (VehicleControls::cNO_YAW_CONTROL):
            controlMethodStr = "BANK_TO_TURN_NO_YAW";
            break;
         case (VehicleControls::cYAW_DAMPER):
            controlMethodStr = "BANK_TO_TURN_WITH_YAW";
            break;
         default:
            break;
      }
   }
   else
   {
      VehicleControls::VehicleControlSkidToTurnRollControlType rollControlType = VehicleControls::cNO_ROLL_CONTROL;
      bool alphaControlGain = false;

      mVehiclePtr->GetControls().GetSkidToTurnControlSettings(rollControlType, alphaControlGain);

      switch (rollControlType)
      {
         case (VehicleControls::cNO_ROLL_CONTROL):
            controlMethodStr = "YAW_TO_TURN_NO_ROLL";
            break;
         case (VehicleControls::cROLL_DAMPER):
            controlMethodStr = "YAW_TO_TURN_ROLL_RATE";
            break;
         case (VehicleControls::cFULL_ROLL_CONTROL):
            controlMethodStr = "YAW_TO_TURN_ZERO_BANK";
            break;
         default:
            break;
      }
   }

   return controlMethodStr;
}

void ScriptGeneratorSixDOF::CreateControlsInputFile(QString& aControlsDir)
{
   QFile controlFile(aControlsDir + "/control_inputs" + GetFilenameSuffix() + ".txt");
   if (!controlFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   VehicleControls& controls = mVehiclePtr->GetControls();

   QTextStream outStream(&controlFile);

   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "pilot_manager\n";
   outStream << "\n";
   outStream << "   control_inputs\n";
   outStream << "\n";
   outStream << "      control_name StickBack\n";
   outStream << "      control_name StickRight\n";
   outStream << "      control_name RudderRight\n";
   outStream << "      control_name ThrottleMil\n";
   outStream << "      control_name ThrottleAB\n";
   outStream << "      control_name ThrustReverser\n";
   outStream << "      control_name ThrustVectorYaw\n";
   outStream << "      control_name ThrustVectorPitch\n";
   outStream << "      control_name ThrustVectorRoll\n";
   outStream << "      control_name LandingGearDown\n";
   outStream << "      control_name FlapsDown\n";
   outStream << "      control_name SpeedBrakesOut\n";
   outStream << "      control_name SpoilersOut\n";
   outStream << "      control_name NoseWheelRight\n";
   outStream << "      control_name NWS_Right\n";
   outStream << "      control_name NWS_Enable\n";
   outStream << "      control_name RgtWheelBrake\n";
   outStream << "      control_name LftWheelBrake\n";
   outStream << "      control_name TrimPitchUp\n";
   outStream << "      control_name TrimPitchDown\n";
   outStream << "      control_name TrimRollRight\n";
   outStream << "      control_name TrimRollLeft\n";
   outStream << "      control_name TriggerPulled\n";
   outStream << "      control_name ButtonPressed\n";
   outStream << "\n";
   outStream << "      std_stick_back              StickBack\n";
   outStream << "      std_stick_right             StickRight\n";
   outStream << "      std_rudder_right            RudderRight\n";
   outStream << "      std_throttle_mil            ThrottleMil\n";
   outStream << "      std_throttle_ab             ThrottleAB\n";
   outStream << "      std_thrust_reverser         ThrustReverser\n";
   outStream << "      std_thrust_vectoring_yaw    ThrustVectorYaw\n";
   outStream << "      std_thrust_vectoring_pitch  ThrustVectorPitch\n";
   outStream << "      std_thrust_vectoring_roll   ThrustVectorRoll\n";
   outStream << "      std_speed_brakes_out        SpeedBrakesOut\n";
   outStream << "      std_spoilers_out            SpoilersOut\n";
   outStream << "      std_flaps_down              FlapsDown\n";
   outStream << "      std_landing_gear_down       LandingGearDown\n";
   outStream << "      std_nose_wheel_steering     NoseWheelRight\n";
   outStream << "      std_nws_steering            NWS_Right\n";
   outStream << "      std_nws_enabled             NWS_Enable\n";
   outStream << "      std_wheel_brake_left        LftWheelBrake\n";
   outStream << "      std_wheel_brake_right       RgtWheelBrake\n";
   outStream << "\n";
   outStream << "   end_control_inputs\n";
   outStream << "\n";

   bool syntheticPilotPresent    = false;
   bool manualPilotPresent       = false;
   bool hardwareAutopilotPresent = false;
   bool guidanceSystemPresent    = false;
   VehicleControls::VehicleControlPilotType activePilot;

   controls.GetPilotTypes(syntheticPilotPresent,
                          manualPilotPresent,
                          hardwareAutopilotPresent,
                          guidanceSystemPresent,
                          activePilot);

   if (syntheticPilotPresent)
   {
      outStream << "   synthetic_pilot\n";
      outStream << "      # Define the autopilot configuration\n";
      outStream << "      include autopilot_config" + GetFilenameSuffix() + ".txt\n";
      outStream << "   end_synthetic_pilot\n";
      outStream << "\n";
   }
   else
   {
      outStream << "/* synthetic_pilot\n";
      outStream << "      # Define the autopilot configuration\n";
      outStream << "      include autopilot_config" + GetFilenameSuffix() + ".txt\n";
      outStream << "   end_synthetic_pilot\n";
      outStream << "*/\n";
   }

   if (manualPilotPresent)
   {
      bool useExponentialYawControlMapping = false;
      bool useExponentialPitchControlMapping = false;
      bool useExponentialRollControlMapping = false;
      double trimFactorYaw = 0.0;
      double trimFactorPitch = 0.0;
      double trimFactorRoll = 0.0;
      VehicleControls::VehicleControlAugmentationType pilotAugmentation = VehicleControls::cDIRECT_CONTROL;
      bool augmentStabilityYawAxis = false;
      bool augmentStabilityPitchAxis = false;
      bool augmentStabilityRollAxis = false;
      double pitchControlAugmented_Gs = 0.0;
      double rollControlAugmented_dps = 0.0;

      controls.GetManualPilotControlSettings(useExponentialYawControlMapping,
                                             useExponentialPitchControlMapping,
                                             useExponentialRollControlMapping,
                                             trimFactorYaw,
                                             trimFactorPitch,
                                             trimFactorRoll,
                                             pilotAugmentation,
                                             augmentStabilityPitchAxis,
                                             augmentStabilityYawAxis,
                                             augmentStabilityRollAxis,
                                             pitchControlAugmented_Gs,
                                             rollControlAugmented_dps);

      if (pilotAugmentation == VehicleControls::cDIRECT_CONTROL)
      {
         outStream << "   manual_pilot_simple_controls\n";
         outStream << "      simple_yaw_damper   false\n";
      }
      else if (pilotAugmentation == VehicleControls::cCONTROL_AUGMENTATION)
      {
         outStream << "   manual_pilot_augmented_controls\n";
         outStream << "      # Define the CAS autopilot configuration\n";
         outStream << "      include autopilot_config" + GetFilenameSuffix() + ".txt\n";
         outStream << "\n";
      }
      else if (pilotAugmentation == VehicleControls::cSTABILITY_AUGMENTATION)
      {
         outStream << "   manual_pilot_augmented_stability\n";
         outStream << "      # Define the SAS autopilot configuration\n";
         outStream << "      include autopilot_config" + GetFilenameSuffix() + ".txt\n";
         outStream << "\n";
      }

      if (controls.UseExponentialYawControlMapping())
      {
         outStream << "      yaw_control_mapping_table\n";
         outStream << "         -1.00   -1.00\n";
         outStream << "         -0.90   -0.75\n";
         outStream << "         -0.75   -0.45\n";
         outStream << "         -0.50   -0.15\n";
         outStream << "         -0.25   -0.05\n";
         outStream << "          0.00    0.00\n";
         outStream << "          0.25    0.05\n";
         outStream << "          0.50    0.15\n";
         outStream << "          0.75    0.45\n";
         outStream << "          0.90    0.75\n";
         outStream << "          1.00    1.00\n";
         outStream << "      end_yaw_control_mapping_table\n";
         outStream << "\n";
      }

      if (controls.UseExponentialPitchControlMapping())
      {
         outStream << "      pitch_control_mapping_table\n";
         outStream << "         -1.00   -1.00\n";
         outStream << "         -0.90   -0.75\n";
         outStream << "         -0.75   -0.45\n";
         outStream << "         -0.50   -0.15\n";
         outStream << "         -0.25   -0.05\n";
         outStream << "          0.00    0.00\n";
         outStream << "          0.25    0.05\n";
         outStream << "          0.50    0.15\n";
         outStream << "          0.75    0.45\n";
         outStream << "          0.90    0.75\n";
         outStream << "          1.00    1.00\n";
         outStream << "      end_pitch_control_mapping_table\n";
         outStream << "\n";
      }

      if (controls.UseExponentialRollControlMapping())
      {
         outStream << "      roll_control_mapping_table\n";
         outStream << "         -1.00   -1.00\n";
         outStream << "         -0.90   -0.75\n";
         outStream << "         -0.75   -0.45\n";
         outStream << "         -0.50   -0.15\n";
         outStream << "         -0.25   -0.05\n";
         outStream << "          0.00    0.00\n";
         outStream << "          0.25    0.05\n";
         outStream << "          0.50    0.15\n";
         outStream << "          0.75    0.45\n";
         outStream << "          0.90    0.75\n";
         outStream << "          1.00    1.00\n";
         outStream << "      end_roll_control_mapping_table\n";
         outStream << "\n";
      }

      outStream << "\n";
      outStream << "      pitch_trim_factor                     " << trimFactorPitch << "\n";
      outStream << "      roll_trim_factor                      " << trimFactorRoll << "\n";
      outStream << "      yaw_trim_factor                       " << trimFactorYaw << "\n";
      outStream << "\n";

      if (pilotAugmentation == VehicleControls::cCONTROL_AUGMENTATION)
      {
         outStream << "      pitch_control_augmentation_factor_g   " << pitchControlAugmented_Gs << "\n";
         outStream << "      roll_control_augmentation_factor_dps  " << rollControlAugmented_dps << "\n";
      }

      if (pilotAugmentation == VehicleControls::cSTABILITY_AUGMENTATION)
      {
         if (augmentStabilityPitchAxis)
         {
            outStream << "      pitch_stability_augmentation          true\n";
         }
         else
         {
            outStream << "      pitch_stability_augmentation          false\n";
         }

         if (augmentStabilityYawAxis)
         {
            outStream << "      yaw_stability_augmentation            true\n";
         }
         else
         {
            outStream << "      yaw_stability_augmentation            false\n";
         }

         if (augmentStabilityRollAxis)
         {
            outStream << "      roll_stability_augmentation           true\n";
         }
         else
         {
            outStream << "      roll_stability_augmentation           false\n";
         }
      }

      if (pilotAugmentation == VehicleControls::cDIRECT_CONTROL)
      {
         outStream << "   end_manual_pilot_simple_controls\n";
      }
      else if (pilotAugmentation == VehicleControls::cCONTROL_AUGMENTATION)
      {
         outStream << "   end_manual_pilot_augmented_controls\n";
      }
      else if (pilotAugmentation == VehicleControls::cSTABILITY_AUGMENTATION)
      {
         outStream << "   end_manual_pilot_augmented_stability\n";
      }
      outStream << "\n";
   }
   else
   {
      outStream << "/* manual_pilot_simple_controls\n";
      outStream << "      simple_yaw_damper   false\n";
      outStream << "      pitch_trim_factor   0.1\n";
      outStream << "      roll_trim_factor    0.1\n";
      outStream << "      yaw_trim_factor     0.1\n";
      outStream << "\n";
      outStream << "   end_manual_pilot_simple_controls\n";
      outStream << "*/\n";
   }

   outStream << "   # Make the synthetic pilot active\n";
   outStream << "   active_pilot synthetic_pilot\n";
   outStream << "\n";
   outStream << "   # This autopilot support file is common to all pilots\n";
   outStream << "   # that use a P6DofCommonController (autopilot)\n";
   outStream << "   common_autopilot_support_file      autopilot_support_tables" + GetFilenameSuffix() + ".txt\n";
   outStream << "\n";
   outStream << "end_pilot_manager\n";
   controlFile.close();

   //create autopilot_config.txt file
   CreateAutopilotConfigFile(aControlsDir);
   CreateAutopilotG_LimitConfigFile(aControlsDir);
}

void ScriptGeneratorSixDOF::CreateAutopilotConfigFile(QString& aControlsDir)
{
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorSixDOF::CreateAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/autopilot_config" + GetFilenameSuffix() + ".txt");

   // If the file already exists, we want to avoid overwriting tuned PID values.
   // Update the control method, and return.
   if (genFile.exists())
   {
      // Retrieve the contents of the existing file
      genFile.open(QIODevice::ReadOnly);
      QByteArray fileData = genFile.readAll();
      QString text(fileData);

      QString controlMethodStr = GenerateControlMethodString();

      // Assume we're only using one control method, and replace any instance of a stale control method string.
      // If we wanted to be safer, we might instead search for the full control_method line.
      text.replace(QString("BANK_TO_TURN_NO_YAW"), controlMethodStr);
      text.replace(QString("BANK_TO_TURN_WITH_YAW"), controlMethodStr);
      text.replace(QString("YAW_TO_TURN_NO_ROLL"), controlMethodStr);
      text.replace(QString("YAW_TO_TURN_ROLL_RATE"), controlMethodStr);
      text.replace(QString("YAW_TO_TURN_ZERO_BANK"), controlMethodStr);
      genFile.close();

      // Re-open the file to post the edited version
      genFile.open(QIODevice::WriteOnly);
      genFile.write(text.toUtf8());
      genFile.close();
      return;
   }


   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# **************************************************************************************************\n";
   outStream << "# This file defines the configuration of the autopilot, including its PIDs. A PID is a proportional,\n";
   outStream << "# integral, derivative feedback control technique. Multiple PIDs are used to 'define' the autopilot.\n";
   outStream << "# Each control output may be influenced by one or more PIDs.\n";
   outStream << "# **************************************************************************************************\n";
   outStream << "\n";
   outStream << "autopilot_config\n\n";

   QString controlMethodStr = GenerateControlMethodString();

   outStream << "  vertical_middle_loop_rate_factor  4\n";
   outStream << "  vertical_outer_loop_rate_factor   4\n";
   outStream << "  lateral_middle_loop_rate_factor   4\n";
   outStream << "  lateral_outer_loop_rate_factor    4\n";
   outStream << "  speed_middle_loop_rate_factor     200\n";
   outStream << "  speed_outer_loop_rate_factor      4\n";
   outStream << "\n";
   outStream << "  control_method                    " << controlMethodStr << "\n";
   outStream << "\n";
   outStream << "  # min_taxi_turn_radius              50.0 ft # This can be manually 'tuned' for improved performance\n";
   outStream << "\n";
   outStream << "  # use_simple_yaw_damper             true    # This can be enabled to dampen yaw oscillations\n";
   outStream << "\n";
   outStream << "# ------------------------------------------------------------\n";
   outStream << "\n";

   outStream << "  pid_group\n";
   outStream << "\n";
   outStream << "    pid_alpha\n";
   outStream << "      kp 0.001\n";
   outStream << "      ki 0.1\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_alpha\n";
   outStream << "\n";
   outStream << "    pid_altitude\n";
   outStream << "      kp 10\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 100\n";
   outStream << "    end_pid_altitude\n";
   outStream << "\n";
   outStream << "    pid_bank_angle\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_bank_angle\n";
   outStream << "\n";
   outStream << "    pid_beta\n";
   outStream << "      kp 0.001\n";
   outStream << "      ki 0.1\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_beta\n";
   outStream << "\n";
   outStream << "    pid_delta_pitch\n";
   outStream << "      kp 0.6\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_delta_pitch\n";
   outStream << "\n";
   outStream << "    pid_delta_roll\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_delta_roll\n";
   outStream << "\n";
   outStream << "    pid_flightpath_angle\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0.0\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "    end_pid_flightpath_angle\n";
   outStream << "\n";
   outStream << "    pid_forward_accel\n";
   outStream << "      kp 20\n";
   outStream << "      ki 2\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_forward_accel\n";
   outStream << "\n";
   outStream << "    pid_pitch_angle\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0.0\n";
   outStream << "      max_error_accum 0.5\n";
   outStream << "    end_pid_pitch_angle\n";
   outStream << "\n";
   outStream << "    pid_pitch_rate\n";
   outStream << "      kp 0\n";
   outStream << "      ki 0.2\n";
   outStream << "      kd 0.0001\n";
   outStream << "      max_error_accum 0\n";
   outStream << "    end_pid_pitch_rate\n";
   outStream << "\n";
   outStream << "    pid_roll_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 2\n";
   outStream << "      low_pass_alpha 0.5\n";
   outStream << "    end_pid_roll_heading\n";
   outStream << "\n";
   outStream << "    pid_roll_rate\n";
   outStream << "      kp 0.005\n";
   outStream << "      ki 0.05\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 0\n";
   outStream << "      low_pass_alpha 0.7\n";
   outStream << "    end_pid_roll_rate\n";
   outStream << "\n";
   outStream << "    pid_speed\n";
   outStream << "      kp 0.5\n";
   outStream << "      ki 0.05\n";
   outStream << "      kd 0.5\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_speed\n";
   outStream << "\n";
   outStream << "    pid_taxi_forward_accel\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_taxi_forward_accel\n";
   outStream << "\n";
   outStream << "    pid_taxi_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0.01\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_taxi_heading\n";
   outStream << "\n";
   outStream << "    pid_taxi_speed\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_taxi_speed\n";
   outStream << "\n";
   outStream << "    pid_taxi_yaw_rate\n";
   outStream << "      kp 0.1\n";
   outStream << "      ki 0.2\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 10\n";
   outStream << "    end_pid_taxi_yaw_rate\n";
   outStream << "\n";
   outStream << "    pid_vert_speed\n";
   outStream << "      kp 0.0012\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 60\n";
   outStream << "      ignore_large_error_accum 500\n";
   outStream << "    end_pid_vert_speed\n";
   outStream << "\n";
   outStream << "    pid_yaw_heading\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 1\n";
   outStream << "    end_pid_yaw_heading\n";
   outStream << "\n";
   outStream << "    pid_yaw_rate\n";
   outStream << "      kp 1\n";
   outStream << "      ki 0\n";
   outStream << "      kd 0\n";
   outStream << "      max_error_accum 0\n";
   outStream << "    end_pid_yaw_rate\n";
   outStream << "\n";
   outStream << "\n";

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "    limits_and_settings\n";
      outStream << "     include autopilot_config_g_limits.txt\n";
      outStream << "     afterburner_threshold       1\n";
      outStream << "     speedbrake_threshold        0\n";
      outStream << "     alpha_min                   -16\n";
      outStream << "     alpha_max                   20\n";
      outStream << "     pitch_rate_min              -10\n";
      outStream << "     pitch_rate_max              40\n";
      outStream << "     vert_speed_min              -15000\n";
      outStream << "     vert_speed_max              15000\n";
      outStream << "     beta_max                    10\n";
      outStream << "     yaw_rate_max                5\n";
      outStream << "     roll_rate_max               180\n";
      outStream << "     bank_angle_max              60\n";
      outStream << "     forward_accel_min           -1\n";
      outStream << "     forward_accel_max           2\n";
      outStream << "     taxi_speed_max_fps          10\n";
      outStream << "     taxi_yaw_rate_max           10\n";
      outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
      outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
      outStream << "    end_limits_and_settings\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      if (mVehiclePtr->UsesBankToTurnControl())
      {
         outStream << "    limits_and_settings\n";
         outStream << "     include autopilot_config_g_limits.txt\n";
         outStream << "     afterburner_threshold       1\n";
         outStream << "     speedbrake_threshold        0\n";
         outStream << "     alpha_min                   -16\n";
         outStream << "     alpha_max                   20\n";
         outStream << "     pitch_rate_min              -10\n";
         outStream << "     pitch_rate_max              40\n";
         outStream << "     vert_speed_min              -15000\n";
         outStream << "     vert_speed_max              15000\n";
         outStream << "     beta_max                    10\n";
         outStream << "     yaw_rate_max                5\n";
         outStream << "     roll_rate_max               180\n";
         outStream << "     bank_angle_max              60\n";
         outStream << "     forward_accel_min           -1\n";
         outStream << "     forward_accel_max           2\n";
         outStream << "     taxi_speed_max_fps          10\n";
         outStream << "     taxi_yaw_rate_max           10\n";
         outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
         outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
         outStream << "    end_limits_and_settings\n";
      }
      else
      {
         outStream << "    limits_and_settings\n";
         outStream << "     include autopilot_config_g_limits.txt\n";
         outStream << "     afterburner_threshold       1\n";
         outStream << "     speedbrake_threshold        0\n";
         outStream << "     alpha_min                   -20\n";
         outStream << "     alpha_max                   20\n";
         outStream << "     pitch_rate_min              -60\n";
         outStream << "     pitch_rate_max              60\n";
         outStream << "     vert_speed_min              -60000\n";
         outStream << "     vert_speed_max              60000\n";
         outStream << "     beta_max                    20.0\n";
         outStream << "     yaw_rate_max                60.0\n";
         outStream << "     roll_rate_max               360.0\n";
         outStream << "     bank_angle_max              89.0\n";
         outStream << "     forward_accel_min           -1\n";
         outStream << "     forward_accel_max           2\n";
         outStream << "     taxi_speed_max_fps          10\n";
         outStream << "     taxi_yaw_rate_max           10\n";
         outStream << "     turn_roll_in_multiplier     4.0       # This can be manually 'tuned' for improved performance\n";
         outStream << "     route_allowable_angle_error 5.0 deg   # This can be manually 'tuned' for improved performance\n";
         outStream << "    end_limits_and_settings\n";
      }
   }

   outStream << "\n";
   outStream << "  end_pid_group\n";
   outStream << "\n";
   outStream << "end_autopilot_config\n";
   outStream << "\n";

   genFile.close();
}

void ScriptGeneratorSixDOF::CreateAutopilotG_LimitConfigFile(QString& aControlsDir)
{
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorSixDOF::CreateAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/autopilot_config_g_limits.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObject = curDoc->object();
   G_LimitSettings settings{G_LimitSettings::PilotsGuidanceName()};
   settings.LoadFromFile(mainObject);

   outStream << "     pitch_gload_min             " << settings.GetPitchG_LoadMin() << "\n";
   outStream << "     pitch_gload_max             " << settings.GetPitchG_LoadMax() << "\n";
   outStream << "     yaw_gload_max               " << settings.GetYawG_LoadMax() << "\n";

   genFile.close();
}

////////////////////////////////////////////////////////////////////
//          Performance tests file creation
////////////////////////////////////////////////////////////////////

void ScriptGeneratorSixDOF::CreateCommonTestFile(QString& aTestDir)
{
   QFile testFile(aTestDir + "/common.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << GetEnableControlsScript();
   outStream << GetSetMasterNoLagTestingScript();
   outStream << GetSetMasterFreezeAltitudeScript();
   outStream << GetSetMasterFreezeVerticalSpeedScript();
   outStream << GetSetMasterFreezePitchScript();
   outStream << GetSetMasterFreezeRollScript();
   outStream << GetSetMasterFreezeYawScript();
   outStream << GetSetMasterFreezeFuelBurnScript();
   outStream << GetSetMasterNoAlphaTestingScript();
   outStream << GetRangeSinceStart_kmScript();
   outStream << GetFlyStraightLevelFullThrottleScript();
   outStream << GetFuelRemainingScript();

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateEnvelopeCommonTestFile(QString& aTestDir)
{
   QFile testFile(aTestDir + "/envelope_common.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << GetFlightEnvelopeScripts();

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateBallisticRangeTestFile(QString& aTestDir)
{
   QFile testFile(aTestDir + "/ballistic_range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Ballistic Range Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test launches the weapon on ballistic trajectories using multiple launch angles to determine ballistic range\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test launches the weapon on ballistic trajectories using multiple launch angles to determine its ballistic range. No weapon guidance/control is used. The maximum range is plotted as a function of launch angle.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "script_variables\n";
   outStream << "   // Note that a non-zero launch altitude is used to avoid\n";
   outStream << "   // collision with the ground during initial acceleration\n";
   outStream << "   double INITIAL_ALTITUDE_FT    = 1000;\n";
   outStream << "   double MIN_LAUNCH_ANGLE_DEG   = 10;\n";
   outStream << "   double MAX_LAUNCH_ANGLE_DEG   = 80;\n";
   outStream << "   double LAUNCH_ANGLE_INCREMENT = 1;\n";
   outStream << "end_script_variables\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "\n";
   outStream << "# Set a start time that has a daytime sun position for our test in the Oahu region\n";
   outStream << "start_epoch 2018354.75   # Southern Solstice 2018\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_struct DataStruct\n";
   outStream << "   script_variables\n";
   outStream << "      double maxAltitude_m       = 0;\n";
   outStream << "      double timeMaxAltitude_sec = 0;\n";
   outStream << "      double maxSpeed_mps        = 0;\n";
   outStream << "      double timeMaxSpeed_sec    = 0;\n";
   outStream << "      double maxRange_km         = 0;\n";
   outStream << "      double totalFlightTime_sec = -1;\n";
   outStream << "      double launchAngle_deg     = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Map<int, struct> mData = Map<int, struct>();\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_ballistic_range_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << GetRangeSinceStart_kmScript();
   outStream << "\n";
   outStream << "platform_type SAM_TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   \n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover   \n";
   outStream << "         \n";
   outStream << "   six_dof_ned_heading           0 deg\n";
   outStream << "   six_dof_position              21.4  -158.0\n";
   outStream << "   six_dof_set_velocity_ned_fps  0 0 0 \n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   enable_controls false\n";
   outStream << "      \n";
   outStream << "   on_update  \n";
   outStream << "      foreach (int key : struct value in mData)\n";
   outStream << "      {\n";
   outStream << "         WsfPlatform platform = WsfSimulation.FindPlatform(key);\n";
   outStream << "         if(!platform.IsNull())\n";
   outStream << "         {\n";
   outStream << "            double time = TIME_NOW;\n";
   outStream << "            \n";
   outStream << "            double altitude = platform.Altitude();\n";
   outStream << "            if(altitude > value->maxAltitude_m)\n";
   outStream << "            {\n";
   outStream << "               value->maxAltitude_m = altitude;\n";
   outStream << "               value->timeMaxAltitude_sec = time;\n";
   outStream << "            }\n";
   outStream << "            double speed = platform.Speed();\n";
   outStream << "            if(speed > value->maxSpeed_mps)\n";
   outStream << "            {\n";
   outStream << "               value->maxSpeed_mps = speed;\n";
   outStream << "               value->timeMaxSpeed_sec = time;\n";
   outStream << "            }\n";
   outStream << "            value->maxRange_km = GetRangeSinceStart_km(platform);\n";
   outStream << "         } \n";
   outStream << "      }\n";
   outStream << "   end_on_update\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "//Create the test platforms\n";
   outStream << "execute at_time .01 sec absolute\n";
   outStream << "   \n";
   outStream << "   double iterations = (MAX_LAUNCH_ANGLE_DEG - MIN_LAUNCH_ANGLE_DEG) / LAUNCH_ANGLE_INCREMENT;  \n";
   outStream << "   for(int i = 0; i <= iterations; i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      double launchAngle = LAUNCH_ANGLE_INCREMENT * i + MIN_LAUNCH_ANGLE_DEG;\n";
   outStream << "      WsfPlatform platform1 = WsfSimulation.CreatePlatform(\"SAM_TEST_PLATFORM\");\n";
   outStream << "      string initString = write_str(\"six_dof_ned_pitch \", launchAngle, \" deg six_dof_alt \", INITIAL_ALTITUDE_FT, \" ft\");\n";
   outStream << "      platform1.ProcessInput(initString);\n";
   outStream << "      WsfPlatform platform2 = WsfSimulation.AddPlatform(platform1, \"\");\n";
   outStream << "      struct temp = struct.New(\"DataStruct\");\n";
   outStream << "      temp->launchAngle_deg = launchAngle;  \n";
   outStream << "      mData.Set(platform2.Index(), temp);\n";
   outStream << "   }  \n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   double kmToNm = 0.5399568;\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Ballistic Range (1000 ft launch altitude),y_label=Range (nm)\");\n";
   outStream << "      output.Writeln(\"Launch Angle (deg), Weapon Range\");\n";
   outStream << "      \n";
   outStream << "      foreach (int key : struct value in mData)\n";
   outStream << "      {\n";
   outStream << "         output.Writeln(write_str(value->launchAngle_deg, \", \", value->maxRange_km * kmToNm));\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void SimulationComplete()\n";
   outStream << "   foreach (int key : struct value in mData)\n";
   outStream << "   {\n";
   outStream << "      //If object flew the entire time.\n";
   outStream << "      if(value->totalFlightTime_sec == -1)\n";
   outStream << "      {\n";
   outStream << "         value->totalFlightTime_sec = TIME_NOW;\n";
   outStream << "         writeln(\"Warning: Platform Id : \", key, \" did not terminate, range data may be inaccurate.\");\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   WriteOutput();\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CrashedIntoGround(WsfPlatform aPlatform)\n";
   outStream << "   if(mData.Exists(aPlatform.Index()))\n";
   outStream << "   {\n";
   outStream << "      mData[aPlatform.Index()]->totalFlightTime_sec = TIME_NOW;\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable SIMULATION_COMPLETE SimulationComplete\n";
   outStream << "   enable CRASHED_INTO_GROUND CrashedIntoGround\n";
   outStream << "end_observer\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateEnduranceTestFile(QString& aTestDir)
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/endurance_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Endurance Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test determines that maximum flight time (endurance) of an aircraft at different altitudes and speeds\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test determines that maximum flight time (endurance) of an aircraft at different altitudes and speeds. A series of plots are provided that present the maximum flight time (endurance) as a function of altitude for a specified speed.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   // Altitude and Velocity settings  \n";
   outStream << "   double initialAltitude_ft   = 0;\n";
   outStream << "   double finalAltitude_ft     = 50000;\n";
   outStream << "   double altitudeStep_ft      = 1000;\n";
   outStream << "   double initialVelocity_ktas = 100;\n";
   outStream << "   double finalVelocity_ktas   = 700;\n";
   outStream << "   double velocityStep_ktas    = 50;\n";
   outStream << "   \n";
   outStream << "   // Fuel at which the aircraft is considered out of fuel, and it has reach maximum range.\n";
   outStream << "   double finalFuelQuantity_lbs = 50; \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_endurance_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft  = 0;\n";
   outStream << "      double endurance_hr = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double>        mSpeeds = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData   = Array<Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double IntegrateEndurance(WsfPlatform aPlatform, double aSpeed_ktas, double aAltitude_ft)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfSixDOF_Mover mover = (WsfSixDOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfSixDOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "   \n";
   outStream << "   double emptyMass_kg = mover.GetEmptyWeight();\n";
   outStream << "   double fuelMass_kg = mover.GetTotalFuelCapacity();\n";
   outStream << "   \n";
   outStream << "   double initialMass_kg = emptyMass_kg + fuelMass_kg;\n";
   outStream << "   double finalMass_kg = initialMass_kg - (fuelMass_kg - Math.KG_PER_LB() * finalFuelQuantity_lbs);\n";
   outStream << "   double stepMass_kg = (finalMass_kg - initialMass_kg) / 100.0;\n";
   outStream << "   double endurance_hr = 0.0;\n";
   outStream << "   \n";
   outStream << "   double mach = mover.CalcMachFromKtas(MATH.M_PER_FT() * aAltitude_ft, aSpeed_ktas);\n";
   outStream << "   \n";
   outStream << "   for (double currentMass_kg = initialMass_kg; currentMass_kg > finalMass_kg; currentMass_kg = currentMass_kg + stepMass_kg)\n";
   outStream << "   {\n";
   outStream << "      double fuelRate_kgps = testObj.GetLevelFlightFuelBurnRate(MATH.M_PER_FT() * aAltitude_ft, mach, currentMass_kg);\n";
   outStream << "      double fuelRate_kgph = 3600.0 * fuelRate_kgps;\n";
   outStream << "      if (fuelRate_kgph > 0.0)\n";
   outStream << "      {\n";
   outStream << "         double integrand = -stepMass_kg / fuelRate_kgph;\n";
   outStream << "         endurance_hr = endurance_hr + integrand;\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   \n";
   outStream << "   return endurance_hr;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalVelocity_ktas - initialVelocity_ktas) / velocityStep_ktas) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int speedIdx = 0; speedIdx < nSpeedSteps; speedIdx = speedIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double speed_ktas = initialVelocity_ktas + velocityStep_ktas * speedIdx;\n";
   outStream << "      mSpeeds.PushBack(speed_ktas);\n";
   outStream << "      \n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;   \n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->endurance_hr = IntegrateEndurance(aPlatform, speed_ktas, alt_ft);\n";
   outStream << "         \n";
   outStream << "         loopData.PushBack(data);         \n";
   outStream << "      }\n";
   outStream << "      mData.PushBack(loopData);\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void GenerateFileHeaders(FileIO aFile)\n";
   outStream << "   aFile.Writeln(\"Meta,title=Endurance Test,y_label=Altitude (ft)\");\n";
   outStream << "   string line = \"\";\n";
   outStream << "   for (int i = 0; i < mSpeeds.Size(); i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      line = line + \"Endurance (hr),\" + (string)mSpeeds[i] + \" KTAS\";\n";
   outStream << "      if (i != mSpeeds.Size() - 1)\n";
   outStream << "      {\n";
   outStream << "         line = line + \",,\";\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   aFile.Writeln(line);\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      GenerateFileHeaders(output);\n";
   outStream << "      \n";
   outStream << "      int dataSize = mData[0].Size();\n";
   outStream << "      for (int i = 0; i < dataSize; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string line = \"\";\n";
   outStream << "         for (int j = 0; j < mData.Size(); j = j + 1)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(mData[j][i]->endurance_hr, \",\", mData[j][i]->altitude_ft);\n";
   outStream << "            if (j != mData.Size() - 1)\n";
   outStream << "            {\n";
   outStream << "               line = line + \",,\";\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line); \n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateFlightEnvelopeSimpleTestFile(QString& aTestDir)
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/flight_envelope_(simple)_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Flight Envelope (Simple) Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a flight envelope (dog house plot) for a vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a flight envelope (dog house plot) for a vehicle (Flight envelopes are often referred to by using the slang term, \"dog house plot\".)\n";
   outStream << "//\n";
   outStream << "// A flight envelope shows the minimum and maximum speed that the aircraft can fly under sustained conditions as a function of altitude.";
   outStream << " Flight envelopes may be generated under different conditions, such as maximum (military) power, augmented (afterburner) power, or other";
   outStream << " conditions such as flap settings or customized weapons/fuel load-outs (external fuel tanks and weapons will create drag and all weapons";
   outStream << " and fuel will effect mass properties, such as weight and rotational inertia as well as center-of-gravity). P6DOF models consider all of";
   outStream << " these effects in their kinematic calculations.\n";
   outStream << "//\n";
   outStream << "// This test will generate plots for different conditions, including with afterburner and with only military power (non-AB).\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once envelope_common.txt\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_flight_envelope_(simple)_test.csv\";\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   Array<struct> mMilEnvelope = Array<struct>();\n";
   outStream << "   Array<struct> mAB_Envelope = Array<struct>();\n";
   outStream << "   Map<int, Array<struct>> mGEnvelopes = Map<int, Array<struct>>();\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  1000 ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   \n";
   outStream << "   script void GenerateMil_Envelope()\n";
   outStream << "      mMilEnvelope = ComputeFlightEnvelope(PLATFORM, 1.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void GenerateAB_Envelope()\n";
   outStream << "      mAB_Envelope = ComputeFlightEnvelope(PLATFORM, 2.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      // Generate the Envelope curves\n";
   outStream << "      GenerateMil_Envelope();\n";
   outStream << "      GenerateAB_Envelope();\n";
   outStream << "      \n";
   outStream << "      //Print to File\n";
   outStream << "      WriteOutput(PLATFORM);\n";
   outStream << "    \n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "script double CalcMach(WsfPlatform aPlatform, double aAlt_ft, double aSpeed_fps)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "      return sixdofMover.CalcMachFromFps(MATH.M_PER_FT() * aAlt_ft, aSpeed_fps);\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return 0.0;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput(WsfPlatform aPlatform)\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Flight Envelope (AB and MIL),y_label=Alt (ft)\");\n";
   outStream << "      \n";
   outStream << "      string plotLabels = \"Mach, Afterburner,, Mach, Military Power\";\n";
   outStream << "      \n";
   outStream << "      output.Writeln(plotLabels);\n";
   outStream << "      \n";
   outStream << "      for(int i = 0; i < mAB_Envelope.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = CalcMach(aPlatform, mAB_Envelope[i]->altitude_ft, mAB_Envelope[i]->velocity_fps);\n";
   outStream << "         string lineOut = write_str(mach, \",\", mAB_Envelope[i]->altitude_ft);\n";
   outStream << "         \n";
   outStream << "         if(i < mMilEnvelope.Size())\n";
   outStream << "         {\n";
   outStream << "            double mach = CalcMach(aPlatform, mMilEnvelope[i]->altitude_ft, mMilEnvelope[i]->velocity_fps);\n";
   outStream << "            lineOut = lineOut + write_str(\",,\", mach, \",\", mMilEnvelope[i]->altitude_ft);\n";
   outStream << "         }\n";
   outStream << "         else\n";
   outStream << "         {\n";
   outStream << "            lineOut = lineOut + \",,,\";\n";
   outStream << "         }\n";
   outStream << "         \n";
   outStream << "         output.Writeln(lineOut);\n";
   outStream << "         \n";
   outStream << "      }  \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateFlightEnvelopeAbTestFile(QString& aTestDir)
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/flight_envelope_(ab_with_g-load)_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Flight Envelope (AB With G-Load) Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a flight envelope (dog house plot) for a vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a flight envelope (dog house plot) for a vehicle (Flight envelopes are often referred to by using the slang term, \"dog house plot\".)\n";
   outStream << "//\n";
   outStream << "// A flight envelope shows the minimum and maximum speed that the aircraft can fly under sustained conditions as a function of altitude.";
   outStream << " Flight envelopes may be generated under different conditions, such as maximum (military) power, augmented (afterburner) power, or other";
   outStream << " conditions such as flap settings or customized weapons/fuel load-outs (external fuel tanks and weapons will create drag and all weapons";
   outStream << " and fuel will effect mass properties, such as weight and rotational inertia as well as center-of-gravity). P6DOF models consider all of";
   outStream << " these effects in their kinematic calculations.\n";
   outStream << "//\n";
   outStream << "// This test will generate plots for different conditions, including afterburner performance";
   outStream << " and multiple g-load conditions.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once envelope_common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_flight_envelope_(ab_with_g-load)_test.csv\";\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   Array<struct> mMilEnvelope = Array<struct>();\n";
   outStream << "   Array<struct> mAB_Envelope = Array<struct>();\n";
   outStream << "   Map<int, Array<struct>> mGEnvelopes = Map<int, Array<struct>>();\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  1000 ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "   \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   \n";
   outStream << "   script void GenerateAB_Envelope()\n";
   outStream << "      mAB_Envelope = ComputeFlightEnvelope(PLATFORM, 2.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void GenerateG_Envelopes()\n";
   outStream << "      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)PLATFORM.Mover();\n";
   outStream << "      double baseWeight = sixdofMover.GetTotalWeight();\n";
   outStream << "      writeln(write_str(\"Current Weight: \", baseWeight));\n";
   outStream << "      for(int i = 2; i < 10; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double weight = baseWeight * i;\n";
   outStream << "         writeln(write_str(\"Calculated Weight: \", weight));\n";
   outStream << "         sixdofMover.__getTestObject().SetVehicleMass(weight);\n";
   outStream << "         writeln(write_str(\"Current Weight: \", sixdofMover.GetTotalWeight()));\n";
   outStream << "         \n";
   outStream << "         mGEnvelopes[i] = ComputeFlightEnvelope(PLATFORM, 2.0, 0.0, 1);\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      // Generate the Envelope curves\n";
   outStream << "      GenerateAB_Envelope();\n";
   outStream << "      GenerateG_Envelopes();\n";
   outStream << "      \n";
   outStream << "      //Print to File\n";
   outStream << "      WriteOutput(PLATFORM);\n";
   outStream << "    \n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "script double CalcMach(WsfPlatform aPlatform, double aAlt_ft, double aSpeed_fps)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "      return sixdofMover.CalcMachFromFps(MATH.M_PER_FT() * aAlt_ft, aSpeed_fps);\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return 0.0;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput(WsfPlatform aPlatform)\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Flight Envelope (AB with G-Load),y_label=Alt (ft)\");\n";
   outStream << "      \n";
   outStream << "      string plotLabels = \"Mach, Afterburner\";\n";
   outStream << "      \n";
   outStream << "      foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "      {\n";
   outStream << "         if(!data.Empty())\n";
   outStream << "         {\n";
   outStream << "            plotLabels = plotLabels + write_str(\",, Mach, Max G = \", key);\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Writeln(plotLabels);\n";
   outStream << "      \n";
   outStream << "      for(int i = 0; i < mAB_Envelope.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = CalcMach(aPlatform, mAB_Envelope[i]->altitude_ft, mAB_Envelope[i]->velocity_fps);\n";
   outStream << "         string lineOut = write_str(mach, \",\", mAB_Envelope[i]->altitude_ft);\n";
   outStream << "         \n";
   outStream << "         foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "         {\n";
   outStream << "            if(i < data.Size())\n";
   outStream << "            {\n";
   outStream << "               double mach = CalcMach(aPlatform, data[i]->altitude_ft, data[i]->velocity_fps);\n";
   outStream << "               lineOut = lineOut + write_str(\",,\", mach, \",\", data[i]->altitude_ft);\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         \n";
   outStream << "         output.Writeln(lineOut);\n";
   outStream << "         \n";
   outStream << "      }  \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateFlightEnvelopeMilTestFile(QString& aTestDir)
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/flight_envelope_(mil_with_g-load)_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Flight Envelope (Mil With G-Load) Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a flight envelope (dog house plot) for a vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a flight envelope (dog house plot) for a vehicle (Flight envelopes are often referred to by using the slang term, \"dog house plot\".)\n";
   outStream << "//\n";
   outStream << "// A flight envelope shows the minimum and maximum speed that the aircraft can fly under sustained conditions as a function of altitude.";
   outStream << " Flight envelopes may be generated under different conditions, such as maximum (military) power, augmented (afterburner) power, or other";
   outStream << " conditions such as flap settings or customized weapons/fuel load-outs (external fuel tanks and weapons will create drag and all weapons";
   outStream << " and fuel will effect mass properties, such as weight and rotational inertia as well as center-of-gravity). P6DOF models consider all of";
   outStream << " these effects in their kinematic calculations.\n";
   outStream << "//\n";
   outStream << "// This test will generate plots for different conditions, including military power (non-AB) performance";
   outStream << " and multiple g-load conditions.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once envelope_common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_flight_envelope_(mil_with_g-load)_test.csv\";\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   Array<struct> mMilEnvelope = Array<struct>();\n";
   outStream << "   Array<struct> mAB_Envelope = Array<struct>();\n";
   outStream << "   Map<int, Array<struct>> mGEnvelopes = Map<int, Array<struct>>();\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  1000 ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   \n";
   outStream << "   script void GenerateMilEnvelope()\n";
   outStream << "      mMilEnvelope = ComputeFlightEnvelope(PLATFORM, 1.0, 0.0, 1);\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void GenerateG_Envelopes()\n";
   outStream << "      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)PLATFORM.Mover();\n";
   outStream << "      double baseWeight = sixdofMover.GetTotalWeight();\n";
   outStream << "      writeln(write_str(\"Current Weight: \", baseWeight));\n";
   outStream << "      for(int i = 2; i < 10; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double weight = baseWeight * i;\n";
   outStream << "         writeln(write_str(\"Calculated Weight: \", weight));\n";
   outStream << "         sixdofMover.__getTestObject().SetVehicleMass(weight);\n";
   outStream << "         writeln(write_str(\"Current Weight: \", sixdofMover.GetTotalWeight()));\n";
   outStream << "         \n";
   outStream << "         mGEnvelopes[i] = ComputeFlightEnvelope(PLATFORM, 1.0, 0.0, 1);\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      // Generate the Envelope curves\n";
   outStream << "      GenerateMilEnvelope();\n";
   outStream << "      GenerateG_Envelopes();\n";
   outStream << "      \n";
   outStream << "      //Print to File\n";
   outStream << "      WriteOutput(PLATFORM);\n";
   outStream << "    \n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "script double CalcMach(WsfPlatform aPlatform, double aAlt_ft, double aSpeed_fps)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "      return sixdofMover.CalcMachFromFps(MATH.M_PER_FT() * aAlt_ft, aSpeed_fps);\n";
   outStream << "   }\n";
   outStream << "   else\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   return 0.0;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput(WsfPlatform aPlatform)\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if(output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=Flight Envelope (MIL with G-Load),y_label=Alt (ft)\");\n";
   outStream << "      \n";
   outStream << "      string plotLabels = \"Mach, Military Power\";\n";
   outStream << "      \n";
   outStream << "      foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "      {\n";
   outStream << "         if(!data.Empty())\n";
   outStream << "         {\n";
   outStream << "            plotLabels = plotLabels + write_str(\",, Mach, Max G = \", key);\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Writeln(plotLabels);\n";
   outStream << "      \n";
   outStream << "      for(int i = 0; i < mMilEnvelope.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string lineOut;\n";
   outStream << "          \n";
   outStream << "#         if(i < mMilEnvelope.Size())\n";
   outStream << "#         {\n";
   outStream << "#            lineOut = lineOut + write_str(\",,\", mMilEnvelope[i]->velocity_fps, \",\", mMilEnvelope[i]->altitude_ft);\n";
   outStream << "#         }\n";
   outStream << "#         else\n";
   outStream << "#         {\n";
   outStream << "#            lineOut = lineOut + \",,,\";\n";
   outStream << "#         }\n";
   outStream << "         double mach = CalcMach(aPlatform, mMilEnvelope[i]->altitude_ft, mMilEnvelope[i]->velocity_fps);\n";
   outStream << "         lineOut = lineOut + write_str(mach, \",\", mMilEnvelope[i]->altitude_ft);\n";
   outStream << "         \n";
   outStream << "         foreach(int key : Array<struct> data in mGEnvelopes)\n";
   outStream << "         {\n";
   outStream << "            if(i < data.Size())\n";
   outStream << "            {\n";
   outStream << "               double mach = CalcMach(aPlatform, data[i]->altitude_ft, data[i]->velocity_fps);\n";
   outStream << "               lineOut = lineOut + write_str(\",,\", mach, \",\", data[i]->altitude_ft);\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "      \n";
   outStream << "         output.Writeln(lineOut);\n";
   outStream << "\n";
   outStream << "      }  \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateGCapabilityTestFile(QString& aTestDir)
{
   if (mVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString vehicleType = "";
   if (mVehiclePtr->IsAircraft())
   {
      vehicleType = "aircraft";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      vehicleType = "weapons";
   }

   QFile testFile(aTestDir + "/g_capability_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Instantaneous G Capability Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test displays the maximum instantaneous G's that the vehicle can pull under a variety of flight conditions\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test displays the maximum instantaneous G's that the vehicle can pull under a variety of flight conditions. It plots a series of data based on speed, that present the maximum g as a function of altitude. This shows the maneuver capability possible if the vehicle operated at the specified speed, although not all speeds shown may be attainable in normal conditions.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << vehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   \n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_instantaneous_g_capability_test.csv\";\n";
   outStream << "   \n";
   outStream << "   // Altitude and Velocity settings\n";
   outStream << "   double initialAltitude_ft = 2500;\n";
   outStream << "   double finalAltitude_ft   = 100000;\n";
   outStream << "   double altitudeStep_ft    = 2500;\n";
   outStream << "   double initialMach        = 0.75;\n";
   outStream << "   double finalMach          = 4.0;\n";
   outStream << "   double machStep           = 0.25;\n";
   outStream << "   \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "\n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  2500 m\n";
   outStream << "\n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "\n";
   outStream << "   on_initialize\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft = 0;\n";
   outStream << "      double gLoad       = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double> mMachs = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData = Array<Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double MaxInstantaneousGLoad(WsfPlatform aPlatform, double aAltitude_ft, double aMach, double aCurrentMass_lbs)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfSixDOF_Mover mover = (WsfSixDOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfSixDOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "\n";
   outStream << "   double gLoad = testObj.MaxInstantaneousGLoad(Math.M_PER_FT() * aAltitude_ft, aMach, Math.KG_PER_LB() * aCurrentMass_lbs);\n";
   outStream << "\n";
   outStream << "   return gLoad;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return;\n";
   outStream << "   }\n";
   outStream << "   WsfSixDOF_Mover mover = (WsfSixDOF_Mover)aPlatform.Mover();\n";
   outStream << "\n";
   outStream << "   double currentMass_lbs = MATH.LB_PER_KG() * 0.5 * (mover.GetCurrentWeight() + mover.GetEmptyWeight());\n";
   outStream << "\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalMach - initialMach) / machStep) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int machIdx = 0; machIdx < nSpeedSteps; machIdx = machIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double mach = initialMach + machStep * machIdx;\n";
   outStream << "      mMachs.PushBack(mach);\n";
   outStream << "\n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->gLoad = MaxInstantaneousGLoad(aPlatform, alt_ft, mach, currentMass_lbs);\n";
   outStream << "\n";
   outStream << "         loopData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "      mData.PushBack(loopData);\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void GenerateFileHeaders(FileIO aFile)\n";
   outStream << "   aFile.Writeln(\"Meta,title=Instantaneous G-Load Test,y_label=Altitude (ft)\");\n";
   outStream << "   string line = \"\";\n";
   outStream << "   for (int i = 0; i < mMachs.Size(); i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      line = line + \"G-Load,\" + \" Mach \" + (string)mMachs[i];\n";
   outStream << "      if (i != mMachs.Size() - 1)\n";
   outStream << "      {\n";
   outStream << "         line = line + \",,\";\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   aFile.Writeln(line);\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      GenerateFileHeaders(output);\n";
   outStream << "\n";
   outStream << "      int dataSize = mData[0].Size();\n";
   outStream << "      for (int i = 0; i < dataSize; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string line = \"\";\n";
   outStream << "         for (int j = 0; j < mData.Size(); j = j + 1)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(mData[j][i]->gLoad, \",\", mData[j][i]->altitude_ft);\n";
   outStream << "            if (j != mData.Size() - 1)\n";
   outStream << "            {\n";
   outStream << "               line = line + \",,\";\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line);\n";
   outStream << "      }\n";
   outStream << "\n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateLevelFlightRangeTestFile(QString& aTestDir)
{
   QFile testFile(aTestDir + "/level_flight_range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Level Flight Range Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test displays that maximum level flight range of a weapon at different altitudes\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "//This test displays that maximum level flight range of a weapon at different altitudes for a launch speed of Mach 0.85. The maximum range is determined when the minimum speed (Mach 0.7) is reached. A plot of the altitude versus maximum range is provided.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "script_variables\n";
   outStream << "   double MINIMUM_LAUNCH_ALTITUDE_FT = 1000;\n";
   outStream << "   double MAXIMUM_LAUNCH_ALTITUDE_FT = 45000;\n";
   outStream << "   double ALTITUDE_STEP_FT           = 1000;\n";
   outStream << "   double LAUNCH_SPEED_MACH          = 0.85;\n";
   outStream << "   double MIN_TERMINATION_SPEED_MACH = 0.7;\n";
   outStream << "   double ACTIVATION_SPEED_MACH      = MIN_TERMINATION_SPEED_MACH * 1.2;\n";
   outStream << "   double INITIAL_LATITUDE = 21.4;\n";
   outStream << "   double INITIAL_LONGITUDE = -158.0;\n";
   outStream << "   int totalTests = 1 + (MAXIMUM_LAUNCH_ALTITUDE_FT - MINIMUM_LAUNCH_ALTITUDE_FT) / ALTITUDE_STEP_FT;\n";
   outStream << "   int completedCount = 0;\n";
   outStream << "   Map<double, double> rangeAltMap = Map<double, double>();\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_level_flight_range_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 10 min\n";
   outStream << "\n";
   outStream << "script void RegisterCompleted()\n";
   outStream << "   completedCount = completedCount + 1;\n";
   outStream << "   \n";
   outStream << "   if(completedCount == totalTests)\n";
   outStream << "   {\n";
   outStream << "      WsfSimulation.Terminate();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM " << mPlatformType << "\n";
   outStream << "   \n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n";
   outStream << "   \n";
   outStream << "   aux_data\n";
   outStream << "      double commandedAltitude_m = 0 // meters\n";
   outStream << "   end_aux_data\n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading           0 deg\n";
   outStream << "   six_dof_position              21.4  -158.0\n";
   outStream << "   six_dof_set_velocity_ned_fps  0 0 0 \n";
   outStream << "   \n";
   outStream << "   enable_controls false  \n";
   outStream << "   \n";
   outStream << "   script_variables\n";
   outStream << "      bool   completed       = false;\n";
   outStream << "      bool   activated       = false;\n";
   outStream << "      double maxSpeed_mach   = 0.0;\n";
   outStream << "   end_script_variables\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeVerticalSpeed(PLATFORM, true);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "   on_update\n";
   outStream << "      if (!completed)\n";
   outStream << "      {\n";
   outStream << "         WsfMover mover = PLATFORM.Mover();\n";
   outStream << "         if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "         {\n";
   outStream << "            WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "            \n";
   outStream << "            double mach = sixdofMover.GetMach();\n";
   outStream << "            \n";
   outStream << "            if (mach > maxSpeed_mach)\n";
   outStream << "            {\n";
   outStream << "               maxSpeed_mach = mach;\n";
   outStream << "            }\n";
   outStream << "            if (!activated && mach > ACTIVATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               activated = true;\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            if (activated && mach < MIN_TERMINATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               completed = true;\n";
   outStream << "               double altitude_m = PLATFORM.AuxDataDouble(\"commandedAltitude_m\");\n";
   outStream << "               double altitude_ft = altitude_m * MATH.FT_PER_M();\n";
   outStream << "               double maxRange_nm = MATH.NM_PER_M() * PLATFORM.GroundRangeTo(INITIAL_LATITUDE, INITIAL_LONGITUDE, altitude_m);\n";
   outStream << "               rangeAltMap.Set(altitude_ft, maxRange_nm);\n";
   outStream << "               writeln(write_str(\"TestCompleted: Alt \", altitude_ft, \" ft Range \", maxRange_nm, \" nm at t=\", TIME_NOW, \" sec\"));\n";
   outStream << "               RegisterCompleted();\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            FlyStraightLevelFullThrottle(PLATFORM);\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "   end_on_update\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "weapon " << mPlatformType << " WSF_EXPLICIT_WEAPON\n";
   outStream << "   launched_platform_type TEST_PLATFORM\n";
   outStream << "end_weapon\n";
   outStream << "\n";
   outStream << "platform_type LAUNCHER WSF_PLATFORM\n";
   outStream << "   position 21.4n 158.0w\n";
   outStream << "   heading 0 deg\n";
   outStream << "   side blue\n";
   outStream << "   weapon weapon " << mPlatformType << "\n";
   outStream << "      quantity 1\n";
   outStream << "   end_weapon\n";
   outStream << "   mover WSF_AIR_MOVER\n";
   outStream << "   end_mover\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "execute at_time 0.01 s absolute\n";
   outStream << "   for (double alt = MINIMUM_LAUNCH_ALTITUDE_FT; alt <= MAXIMUM_LAUNCH_ALTITUDE_FT; alt = alt + ALTITUDE_STEP_FT)\n";
   outStream << "   {\n";
   outStream << "      double commandedAltitude_m = alt * MATH.M_PER_FT();\n";
   outStream << "      \n";
   outStream << "      // Create a platform instance of type \"LAUNCHER\"\n";
   outStream << "      WsfPlatform tmpPlatform = WsfSimulation.CreatePlatform(\"LAUNCHER\");\n";
   outStream << "      \n";
   outStream << "      // Add the platform to the simulation\n";
   outStream << "      WsfPlatform p = WsfSimulation.AddPlatform(tmpPlatform, \"\");\n";
   outStream << "      \n";
   outStream << "      // Set the altitude to the desired commanded altitude\n";
   outStream << "      p.SetLocation(tmpPlatform.Latitude(), tmpPlatform.Longitude(), commandedAltitude_m);\n";
   outStream << "      \n";
   outStream << "      // Give it a commanded mach number\n";
   outStream << "      p.GoToMachNumber(LAUNCH_SPEED_MACH);\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "execute at_time 30 sec absolute\n";
   outStream << "   for (int i = 0; i < WsfSimulation.PlatformCount(); i += 1)\n";
   outStream << "   {\n";
   outStream << "      WsfPlatform p = WsfSimulation.PlatformEntry(i);\n";
   outStream << "      if (p.IsA_TypeOf(\"LAUNCHER\"))\n";
   outStream << "      {\n";
   outStream << "         p.Weapon(\"weapon\").Fire();\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "script void WeaponFired(WsfWeaponEngagement aWeaponEngagement, WsfTrack aTrack)\n";
   outStream << "   double commandedAltitude_m = aWeaponEngagement.FiringPlatform().Altitude();\n";
   outStream << "   aWeaponEngagement.WeaponPlatform().SetAuxData(\"commandedAltitude_m\", commandedAltitude_m);\n";
   outStream << "   FlyStraightLevelFullThrottle(aWeaponEngagement.WeaponPlatform());\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void SimulationComplete()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "   output.Writeln(\"Meta,title=Level Flight Range (Launch Mach 0.85),y_label=Altitude (ft)\");\n";
   outStream << "   output.Writeln(\"Range (nm), Weapon Range\");\n";
   outStream << "   \n";
   outStream << "   foreach(double key : double value in rangeAltMap)\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(write_str(value, \", \", key));\n";
   outStream << "   }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable SIMULATION_COMPLETE  SimulationComplete\n";
   outStream << "   enable WEAPON_FIRED         WeaponFired\n";
   outStream << "end_observer\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateMaximumSpeedTestFile(QString& aTestDir)
{
   QFile testFile(aTestDir + "/maximum_speed_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);
   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Maximum Speed Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Weapon Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test is used to determine the maximum level speed of the vehicle\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test is used to determine the maximum level speed of the vehicle. A plot of the altitude versus maximum level speed is provided.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "script_variables\n";
   outStream << "   double MINIMUM_LAUNCH_ALTITUDE_FT = 1000;\n";
   outStream << "   double MAXIMUM_LAUNCH_ALTITUDE_FT = 45000;\n";
   outStream << "   double ALTITUDE_STEP_FT           = 1000;\n";
   outStream << "   double LAUNCH_SPEED_MACH          = 0.85;\n";
   outStream << "   double MIN_TERMINATION_SPEED_MACH = 0.7;\n";
   outStream << "   double ACTIVATION_SPEED_MACH      = MIN_TERMINATION_SPEED_MACH * 1.2;\n";
   outStream << "   double INITIAL_LATITUDE = 21.4;\n";
   outStream << "   double INITIAL_LONGITUDE = -158.0;\n";
   outStream << "   int totalTests = 1 + (MAXIMUM_LAUNCH_ALTITUDE_FT - MINIMUM_LAUNCH_ALTITUDE_FT) / ALTITUDE_STEP_FT;\n";
   outStream << "   int completedCount = 0;\n";
   outStream << "   Map<double, double> speedAltMap = Map<double, double>();\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_maximum_speed_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once common.txt\n";
   outStream << "include_once ../../platforms/" << mPlatformName << ".txt\n\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 10 min\n";
   outStream << "\n";
   outStream << "script void RegisterCompleted()\n";
   outStream << "   completedCount = completedCount + 1;\n";
   outStream << "   \n";
   outStream << "   if(completedCount == totalTests)\n";
   outStream << "   {\n";
   outStream << "      WsfSimulation.Terminate();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM " << mPlatformType << "\n";
   outStream << "   \n";
   outStream << "   weapon_effects WSF_GRADUATED_LETHALITY\n";
   outStream << "   \n";
   outStream << "   aux_data\n";
   outStream << "      double commandedAltitude_m = 0 // meters\n";
   outStream << "   end_aux_data\n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading           0 deg\n";
   outStream << "   six_dof_position              21.4  -158.0\n";
   outStream << "   six_dof_set_velocity_ned_fps  0 0 0 \n";
   outStream << "   \n";
   outStream << "   enable_controls false  \n";
   outStream << "   \n";
   outStream << "   script_variables\n";
   outStream << "      bool   completed       = false;\n";
   outStream << "      bool   activated       = false;\n";
   outStream << "      double maxSpeed_mach   = 0.0;\n";
   outStream << "   end_script_variables\n";
   outStream << "   \n";
   outStream << "   on_initialize2\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeVerticalSpeed(PLATFORM, true);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "   on_update\n";
   outStream << "      if (!completed)\n";
   outStream << "      {\n";
   outStream << "         WsfMover mover = PLATFORM.Mover();\n";
   outStream << "         if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "         {\n";
   outStream << "            WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "            \n";
   outStream << "            double mach = sixdofMover.GetMach();\n";
   outStream << "            \n";
   outStream << "            if (mach > maxSpeed_mach)\n";
   outStream << "            {\n";
   outStream << "               maxSpeed_mach = mach;\n";
   outStream << "            }\n";
   outStream << "            if (!activated && mach > ACTIVATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               activated = true;\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            if (activated && mach < MIN_TERMINATION_SPEED_MACH)\n";
   outStream << "            {\n";
   outStream << "               completed = true;\n";
   outStream << "               double altitude_m = PLATFORM.AuxDataDouble(\"commandedAltitude_m\");\n";
   outStream << "               double altitude_ft = altitude_m * MATH.FT_PER_M();\n";
   outStream << "               double maxRange_nm = MATH.NM_PER_M() * PLATFORM.GroundRangeTo(INITIAL_LATITUDE, INITIAL_LONGITUDE, altitude_m);\n";
   outStream << "               speedAltMap.Set(altitude_ft, maxSpeed_mach);\n";
   outStream << "               writeln(write_str(\"TestCompleted: Alt \", altitude_ft, \" ft Max Mach \", maxSpeed_mach, \" at t=\", TIME_NOW, \" sec\"));\n";
   outStream << "               RegisterCompleted();\n";
   outStream << "            }\n";
   outStream << "            \n";
   outStream << "            FlyStraightLevelFullThrottle(PLATFORM);\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "   end_on_update\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "weapon " << mPlatformType << " WSF_EXPLICIT_WEAPON\n";
   outStream << "   launched_platform_type TEST_PLATFORM\n";
   outStream << "end_weapon\n";
   outStream << "\n";
   outStream << "platform_type LAUNCHER WSF_PLATFORM\n";
   outStream << "   position 21.4n 158.0w\n";
   outStream << "   heading 0 deg\n";
   outStream << "   side blue\n";
   outStream << "   weapon weapon " << mPlatformType << "\n";
   outStream << "      quantity 1\n";
   outStream << "   end_weapon\n";
   outStream << "   mover WSF_AIR_MOVER\n";
   outStream << "   end_mover\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "execute at_time 0.01 s absolute\n";
   outStream << "   for (double alt = MINIMUM_LAUNCH_ALTITUDE_FT; alt <= MAXIMUM_LAUNCH_ALTITUDE_FT; alt = alt + ALTITUDE_STEP_FT)\n";
   outStream << "   {\n";
   outStream << "      double commandedAltitude_m = alt * MATH.M_PER_FT();\n";
   outStream << "      \n";
   outStream << "      // Create a platform instance of type \"LAUNCHER\"\n";
   outStream << "      WsfPlatform tmpPlatform = WsfSimulation.CreatePlatform(\"LAUNCHER\");\n";
   outStream << "      \n";
   outStream << "      // Add the platform to the simulation\n";
   outStream << "      WsfPlatform p = WsfSimulation.AddPlatform(tmpPlatform, \"\");\n";
   outStream << "      \n";
   outStream << "      // Set the altitude to the desired commanded altitude\n";
   outStream << "      p.SetLocation(tmpPlatform.Latitude(), tmpPlatform.Longitude(), commandedAltitude_m);\n";
   outStream << "      \n";
   outStream << "      // Give it a commanded mach number\n";
   outStream << "      p.GoToMachNumber(LAUNCH_SPEED_MACH);\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "execute at_time 30 sec absolute\n";
   outStream << "   for (int i = 0; i < WsfSimulation.PlatformCount(); i += 1)\n";
   outStream << "   {\n";
   outStream << "      WsfPlatform p = WsfSimulation.PlatformEntry(i);\n";
   outStream << "      if (p.IsA_TypeOf(\"LAUNCHER\"))\n";
   outStream << "      {\n";
   outStream << "         p.Weapon(\"weapon\").Fire();\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_execute\n";
   outStream << "\n";
   outStream << "script void WeaponFired(WsfWeaponEngagement aWeaponEngagement, WsfTrack aTrack)\n";
   outStream << "   double commandedAltitude_m = aWeaponEngagement.FiringPlatform().Altitude();\n";
   outStream << "   aWeaponEngagement.WeaponPlatform().SetAuxData(\"commandedAltitude_m\", commandedAltitude_m);\n";
   outStream << "   FlyStraightLevelFullThrottle(aWeaponEngagement.WeaponPlatform());\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void SimulationComplete()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "   output.Writeln(\"Meta,title=Level Flight Maximum Speed (Launch Mach 0.85),y_label=Altitude (ft)\");\n";
   outStream << "   output.Writeln(\"Speed (Mach), Maximum Speed\");\n";
   outStream << "   \n";
   outStream << "   foreach(double key : double value in speedAltMap)\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(write_str(value, \", \", key));\n";
   outStream << "   }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "observer\n";
   outStream << "   enable SIMULATION_COMPLETE  SimulationComplete\n";
   outStream << "   enable WEAPON_FIRED         WeaponFired\n";
   outStream << "end_observer\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateRangeTestFile(QString& aTestDir)
{
   QFile testFile(aTestDir + "/range_test.txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE Range Test\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test determines the maximum range of an aircraft at different altitudes and speeds.\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test determines the maximum range of an aircraft at different altitudes and speeds. A series of plots are provided that present the maximum range as a function of altitude for a specified speed.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "// Use these variables to change the test parameters\n";
   outStream << "script_variables\n";
   outStream << "   // Altitude and Velocity settings  \n";
   outStream << "   double initialAltitude_ft   = 0;\n";
   outStream << "   double finalAltitude_ft     = 50000;\n";
   outStream << "   double altitudeStep_ft      = 1000;\n";
   outStream << "   double initialVelocity_ktas = 100;\n";
   outStream << "   double finalVelocity_ktas   = 700;\n";
   outStream << "   double velocityStep_ktas    = 50;\n";
   outStream << "   \n";
   outStream << "   // Fuel at which the aircraft is considered out of fuel, and it has reach maximum range.\n";
   outStream << "   double finalFuelQuantity_lbs = 100; \n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_range_test.csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double altitude_ft = 0;\n";
   outStream << "      double range_nm    = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double>        mSpeeds = Array<double>();\n";
   outStream << "   Array<Array<struct>> mData   = Array<Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script double IntegrateRange(WsfPlatform aPlatform, double aSpeed_ktas, double aAltitude_ft)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      writeln(\"--ERROR-- platform '\", aPlatform.Name(), \"' does not have a P6DOF mover\");\n";
   outStream << "      return 0.0;\n";
   outStream << "   }\n";
   outStream << "   WsfSixDOF_Mover mover = (WsfSixDOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfSixDOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "   \n";
   outStream << "   double emptyMass_kg = mover.GetEmptyWeight();\n";
   outStream << "   double fuelMass_kg = mover.GetTotalFuelCapacity();\n";
   outStream << "   \n";
   outStream << "   double initialMass_kg = emptyMass_kg + fuelMass_kg;\n";
   outStream << "   double finalMass_kg = initialMass_kg - (fuelMass_kg - Math.KG_PER_LB() * finalFuelQuantity_lbs);\n";
   outStream << "   double stepMass_kg = (finalMass_kg - initialMass_kg) / 100.0;\n";
   outStream << "   double range_nm = 0.0;\n";
   outStream << "   \n";
   outStream << "   double mach = mover.CalcMachFromKtas(MATH.M_PER_FT() * aAltitude_ft, aSpeed_ktas);\n";
   outStream << "   \n";
   outStream << "   for (double currentMass_kg = initialMass_kg; currentMass_kg > finalMass_kg; currentMass_kg = currentMass_kg + stepMass_kg)\n";
   outStream << "   {\n";
   outStream << "      double fuelRate_kgps = testObj.GetLevelFlightFuelBurnRate(MATH.M_PER_FT() * aAltitude_ft, mach, currentMass_kg);\n";
   outStream << "      double fuelRate_kgph = 3600.0 * fuelRate_kgps;\n";
   outStream << "      if (fuelRate_kgph > 0.0)\n";
   outStream << "      {\n";
   outStream << "         double integrand_dt = -stepMass_kg / fuelRate_kgph;\n";
   outStream << "         double integrand_nm = integrand_dt * aSpeed_ktas;\n";
   outStream << "         range_nm = range_nm + integrand_nm;\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   \n";
   outStream << "   return range_nm;\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   int nSpeedSteps = MATH.Floor((finalVelocity_ktas - initialVelocity_ktas) / velocityStep_ktas) + 1;\n";
   outStream << "   int nAltSteps = MATH.Floor((finalAltitude_ft - initialAltitude_ft) / altitudeStep_ft) + 1;\n";
   outStream << "   for (int speedIdx = 0; speedIdx < nSpeedSteps; speedIdx = speedIdx + 1)\n";
   outStream << "   {\n";
   outStream << "      double speed_ktas = initialVelocity_ktas + velocityStep_ktas * speedIdx;\n";
   outStream << "      mSpeeds.PushBack(speed_ktas);\n";
   outStream << "      \n";
   outStream << "      Array<struct> loopData = Array<struct>();\n";
   outStream << "      for (int altIdx = 0; altIdx < nAltSteps; altIdx = altIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double alt_ft = initialAltitude_ft + altitudeStep_ft * altIdx;   \n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->altitude_ft = alt_ft;\n";
   outStream << "         data->range_nm = IntegrateRange(aPlatform, speed_ktas, alt_ft);\n";
   outStream << "         \n";
   outStream << "         loopData.PushBack(data);         \n";
   outStream << "      }\n";
   outStream << "      mData.PushBack(loopData);\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void GenerateFileHeaders(FileIO aFile)\n";
   outStream << "   aFile.Writeln(\"Meta,title=Range Test,y_label=Altitude (ft)\");\n";
   outStream << "   string line = \"\";\n";
   outStream << "   for (int i = 0; i < mSpeeds.Size(); i = i + 1)\n";
   outStream << "   {\n";
   outStream << "      line = line + \"Range (nm),\" + (string)mSpeeds[i] + \" KTAS\";\n";
   outStream << "      if (i != mSpeeds.Size() - 1)\n";
   outStream << "      {\n";
   outStream << "         line = line + \",,\";\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "   aFile.Writeln(line);\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      GenerateFileHeaders(output);\n";
   outStream << "      \n";
   outStream << "      int dataSize = mData[0].Size();\n";
   outStream << "      for (int i = 0; i < dataSize; i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         string line = \"\";\n";
   outStream << "         for (int j = 0; j < mData.Size(); j = j + 1)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(mData[j][i]->range_nm, \",\", mData[j][i]->altitude_ft);\n";
   outStream << "            if (j != mData.Size() - 1)\n";
   outStream << "            {\n";
   outStream << "               line = line + \",,\";\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line); \n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateTurnPerformanceTestFile(QString& aTestDir, int aAltitudeFt, bool aUseAB)
{
   std::stringstream testNameBuilder{};
   if (aUseAB)
   {
      testNameBuilder << "Turn Performance (AB Power at " << aAltitudeFt << "ft)";
   }
   else
   {
      testNameBuilder << "Turn Performance (MIL Power at " << aAltitudeFt << "ft)";
   }
   testNameBuilder << " Test";
   QString testName = QString::fromStdString(testNameBuilder.str());
   QFile testFile(aTestDir + "/" + testName.toLower().replace(" ", "_") + ".txt");

   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   { return; }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE " << testName << "\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test generates a turn performance plot (also called an Energy-Management plot, or specific energy plot) for the aircraft\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTION\n";
   outStream << "// This test generates a turn performance plot (also called an Energy-Management plot, or specific energy plot) for the aircraft. It shows the ability of the aircraft to perform at various specific energy (Ps) values.\n";
   outStream << "//\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "\n";
   outStream << "//Changes the altitude that the Turn Performance Plot is generated at\n";
   outStream << "$define TEST_ALT_FT " << aAltitudeFt << "\n";
   outStream << "\n";
   outStream << "//Variables to control the output values\n";
   outStream << "script_variables\n";
   outStream << "\n";
   outStream << "   double MIN_VELOCITY_FPS  = 200;\n";
   outStream << "   double MAX_VELOCITY_FPS  = 2000;\n";
   outStream << "   double VELOCITY_STEP_FPS = 10;\n";
   outStream << "    \n";
   outStream << "   // Valid power settings are \"idle\", \"mil\", or \"ab\"\n";

   if (aUseAB)
   {
      outStream << "   string POWER_SETTING = \"ab\";\n";
   }
   else
   {
      outStream << "   string POWER_SETTING = \"mil\";\n";
   }

   outStream << "    \n";
   outStream << "   // Valid speed units are \"fps\", \"ktas\", \"kcas\", or \"mach\"\n";
   outStream << "   // NOTE: PlotIt will not show background curves for g \n";
   outStream << "   //       and turn radius when using mach.\n";
   outStream << "   string SPEED_UNITS = \"kcas\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "//**********************************************************\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";
   outStream << "script_struct Data\n";
   outStream << "   script_variables\n";
   outStream << "      double vel = 0;\n";
   outStream << "      double tr  = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "\n";
   outStream << "   Map<int, Array<struct>> mDataMap = Map<int, Array<struct>>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM_TYPE WSF_PLATFORM\n";
   outStream << "   spatial_domain        air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover   \n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  $<TEST_ALT_FT:15000>$ ft\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, true);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, false);   \n";
   outStream << "   end_on_initialize\n";
   outStream << "         \n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM_TYPE\n";
   outStream << "\n";
   outStream << "   script void GenerateTurnPerformanceData()\n";
   outStream << "      if(PLATFORM.Mover().IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "      {\n";
   outStream << "         WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)PLATFORM.Mover();\n";
   outStream << "         \n";
   outStream << "         // Valid power settings are \"idle\", \"mil\", or \"ab\"\n";
   outStream << "         double throttleSetting = 0.0;  \n";
   outStream << "         if(POWER_SETTING == \"mil\")       { throttleSetting = 1.0; }\n";
   outStream << "         else if(POWER_SETTING == \"ab\")   { throttleSetting = 2.0; }\n";
   outStream << "         else if(POWER_SETTING == \"idle\") { throttleSetting = 0.0; }\n";
   outStream << "         else \n";
   outStream << "         { \n";
   outStream << "            writeln(\"Error: Unknown POWER_SETTING of \", POWER_SETTING);\n";
   outStream << "         }\n";
   outStream << "\n";
   outStream << "         double currentMass_lbs = MATH.LB_PER_KG() * (sixdofMover.GetEmptyWeight() + 0.5 * sixdofMover.GetTotalFuelCapacity());\n";
   outStream << "         double maxG = sixdofMover.__getTestObject().GetMaxGLoad();\n";
   outStream << "\n";
   outStream << "         // Set-up specific excess power array\n";
   outStream << "         Array<double> SpecificExcessPower = Array<double>();\n";
   outStream << "         SpecificExcessPower.PushBack(-800);\n";
   outStream << "         SpecificExcessPower.PushBack(-400);\n";
   outStream << "         SpecificExcessPower.PushBack(0);\n";
   outStream << "         SpecificExcessPower.PushBack(200);\n";
   outStream << "         SpecificExcessPower.PushBack(400);\n";
   outStream << "         SpecificExcessPower.PushBack(600);\n";
   outStream << "         \n";
   outStream << "         ArrayIterator iter = SpecificExcessPower.GetIterator();\n";
   outStream << "         while (iter.HasNext())\n";
   outStream << "         {\n";
   outStream << "            iter.Next();\n";
   outStream << "            double specificExcessPower_fps = (double)iter.Data();       \n";
   outStream << "            mDataMap[specificExcessPower_fps] = Array<struct>();        \n";
   outStream << "            for(double velocity_fps = MIN_VELOCITY_FPS;\n";
   outStream << "                velocity_fps <= MAX_VELOCITY_FPS;\n";
   outStream << "                velocity_fps += VELOCITY_STEP_FPS)\n";
   outStream << "            {   \n";
   outStream << "               double turnRate = sixdofMover.__getTestObject().TurnRateWithConditions(Math.M_PER_FT() * specificExcessPower_fps, Math.KG_PER_LB() * currentMass_lbs, maxG, Math.M_PER_FT() * $<TEST_ALT_FT:15000>$, Math.M_PER_FT() * velocity_fps, throttleSetting);\n";
   outStream << "               struct temp = struct.New(\"Data\"); \n";
   outStream << "               temp->vel = velocity_fps; \n";
   outStream << "               temp->tr = turnRate;\n";
   outStream << "               mDataMap[specificExcessPower_fps].PushBack(temp);\n";
   outStream << "            }\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      else\n";
   outStream << "      {\n";
   outStream << "         writeln(\"Error: TurnPerformanceTest only works on platforms with P6DOF Movers on them.\");\n";
   outStream << "      }\n";
   outStream << "   end_script\n";
   outStream << "   \n";
   outStream << "   script void WriteOutData(double alt_ft)\n";
   outStream << "      FileIO output = FileIO();\n";
   outStream << "      output.Open(outputFileName, \"out\");\n";
   outStream << "   \n";
   outStream << "      //Output a nicer looking string than MIL or A/B for plot titles\n";
   outStream << "      string powerStr = \"\";\n";
   outStream << "      // Power settings are \"idle\", \"mil\", or \"ab\"\n";
   outStream << "      if(POWER_SETTING == \"mil\")      { powerStr = \"Military\"; }\n";
   outStream << "      else if(POWER_SETTING == \"ab\")  { powerStr = \"Afterburner\"; }\n";
   outStream << "      else                            { powerStr = \"Idle\"; }\n";
   outStream << "      \n";
   outStream << "      //No need to check this, as it was already checked above.\n";
   outStream << "      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)PLATFORM.Mover(); \n";
   outStream << "      \n";
   outStream << "      //ktas and kcas is not recognized by AFSIM so convert to knots which is recognized.\n";
   outStream << "      double speedMultiplier = 1.0;\n";
   outStream << "      if(SPEED_UNITS == \"fps\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = 1.0;\n";
   outStream << "      }\n";
   outStream << "      else if(SPEED_UNITS == \"ktas\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = sixdofMover.CalcKtasFromFps(1.0);\n";
   outStream << "      }\n";
   outStream << "      else if(SPEED_UNITS == \"kcas\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = sixdofMover.CalcKcasFromFps(MATH.M_PER_FT() * alt_ft, 1.0);\n";
   outStream << "      }\n";
   outStream << "      else if(SPEED_UNITS == \"mach\")\n";
   outStream << "      {\n";
   outStream << "         speedMultiplier = sixdofMover.CalcMachFromFps(MATH.M_PER_FT() * alt_ft, 1.0);\n";
   outStream << "      }\n";
   outStream << "      else\n";
   outStream << "      {\n";
   outStream << "         writeln(\"Error: Unknown SPEED_UNITS of \", SPEED_UNITS);\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      //This Meta data row outputs values used by PlotIt to format things correctly\n";
   outStream << "      output.Writeln(write_str(\"Meta,title=Turn Performance @ \", alt_ft, \"ft Power=\", powerStr,\",y_label=Turn Rate (deg/s),tp_multiplier=\", speedMultiplier));\n";
   outStream << "   \n";
   outStream << "      //Create header for the data\n";
   outStream << "      string header = write_str(\"Speed (\", SPEED_UNITS, \"),\");    \n";
   outStream << "      int maxSize = 0;\n";
   outStream << "      foreach (int key : Array<struct> data in mDataMap)\n";
   outStream << "      {\n";
   outStream << "         header += write_str(\"Ps=\", key,\",\");\n";
   outStream << "         if(data.Size() > maxSize)\n";
   outStream << "         {\n";
   outStream << "            maxSize = data.Size();\n";
   outStream << "         }\n";
   outStream << "      }\n";
   outStream << "      output.Writeln(header);\n";
   outStream << "    \n";
   outStream << "      for(int i = 0; i < maxSize; i += 1)\n";
   outStream << "      {\n";
   outStream << "         //This assumes the following lines use a valid index\n";
   outStream << "         int firstIndex = (int)mDataMap.ElementKeyAtIndex(0);\n";
   outStream << "         double vel_fps = mDataMap[firstIndex].Get(i)->vel;\n";
   outStream << "\n";
   outStream << "         string line = \"\";\n";
   outStream << "         if(SPEED_UNITS == \"ktas\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(sixdofMover.CalcKtasFromFps(vel_fps), \",\");\n";
   outStream << "         }\n";
   outStream << "         else if(SPEED_UNITS == \"kcas\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(sixdofMover.CalcKcasFromFps(MATH.M_PER_FT() * alt_ft,vel_fps), \",\");\n";
   outStream << "         }\n";
   outStream << "         else if(SPEED_UNITS == \"fps\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(vel_fps, \",\");\n";
   outStream << "         }\n";
   outStream << "         else if(SPEED_UNITS == \"mach\")\n";
   outStream << "         {\n";
   outStream << "            line = write_str(sixdofMover.CalcMachFromFps(MATH.M_PER_FT() * alt_ft,vel_fps), \",\");\n";
   outStream << "            writeln(\"Warning: Mach support is limited within PlotIt\");\n";
   outStream << "         }\n";
   outStream << "         else\n";
   outStream << "         {\n";
   outStream << "            writeln(\"Error: Unknown Unit String:\", SPEED_UNITS, \" Invalid Output File.\");\n";
   outStream << "         }\n";
   outStream << "      \n";
   outStream << "         foreach (int key : Array<struct> data in mDataMap)\n";
   outStream << "         {\n";
   outStream << "            line = line + write_str(data.Get(i)->tr, \",\");\n";
   outStream << "         }\n";
   outStream << "         output.Writeln(line);\n";
   outStream << "      }\n";
   outStream << "   \n";
   outStream << "      output.Close(); \n";
   outStream << "   end_script\n";
   outStream << "\n";
   outStream << "   on_initialize2\n";
   outStream << "      //Execute Turn Performance Test\n";
   outStream << "      writeln(\"Generating Turn Performance\");\n";
   outStream << "      GenerateTurnPerformanceData();\n";
   outStream << "      writeln(\"Writing out results\");\n";
   outStream << "      WriteOutData($<TEST_ALT_FT:15000>$);\n";
   outStream << "   end_on_initialize2\n";
   outStream << "   \n";
   outStream << "end_platform\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreatePitchingMomentTest(QString& aTestDir, double aAltitude_ft, double aMach)
{
   std::stringstream testNameBuilder{};
   testNameBuilder.precision(3);
   testNameBuilder << "Pitching Moment (" << "Mach " << aMach << ")";
   QString testHeading = QString::fromStdString(testNameBuilder.str());
   testNameBuilder << " Test";
   QString testName = QString::fromStdString(testNameBuilder.str());

   QFile testFile(aTestDir + "/" + testName.toLower().replace(" ", "_") + ".txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE " << testName << "\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test determines the pitching moment as a function of angle of attack\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTIONS\n";
   outStream << "// This test determines the pitching moment as a function of angle of attack. It generates three plot series -- zero control pitching moment, full aft control pitching moment, and full forward control pitching moment.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   double gTestAltitude_ft = " << aAltitude_ft << ";\n";
   outStream << "   double gTestMach        = " << aMach << ";\n";
   outStream << "   \n";
   outStream << "   double gInitialAlpha = -20.0;\n";
   outStream << "   double gFinalAlpha   = 75.0;\n";
   outStream << "   double gStepAlpha    = 1.0;\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<double> mAlpha        = Array<double>();\n";
   outStream << "   Array<double> mZeroStick    = Array<double>();\n";
   outStream << "   Array<double> mBackStick    = Array<double>();\n";
   outStream << "   Array<double> mForwardStick = Array<double>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   if (!aPlatform.Mover().IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      return;\n";
   outStream << "   }\n";
   outStream << "\n";
   outStream << "   WsfRigidBodySixDOF_Mover mover = (WsfRigidBodySixDOF_Mover)aPlatform.Mover();\n";
   outStream << "   WsfRigidBodySixDOF_MoverTestObject testObj = mover.__getTestObject();\n";
   outStream << "   \n";
   outStream << "   for (double alpha = gInitialAlpha; alpha <= gFinalAlpha; alpha = alpha + gStepAlpha)\n";
   outStream << "   {\n";
   outStream << "      mAlpha.PushBack(alpha);\n";
   outStream << "      mZeroStick.PushBack(Math.LB_PER_NT() * Math.FT_PER_M() * testObj.GetPitchingMoment(Math.M_PER_FT() * gTestAltitude_ft, gTestMach, alpha, 0.0));\n";
   outStream << "      mBackStick.PushBack(Math.LB_PER_NT() * Math.FT_PER_M() * testObj.GetPitchingMoment(Math.M_PER_FT() * gTestAltitude_ft, gTestMach, alpha, 1.0));\n";
   outStream << "      mForwardStick.PushBack(Math.LB_PER_NT() * Math.FT_PER_M() * testObj.GetPitchingMoment(Math.M_PER_FT() * gTestAltitude_ft, gTestMach, alpha, -1.0));\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=" << testHeading << ",y_label=Cm (ft-lbs)\");\n";
   outStream << "      output.Writeln(\"Alpha,Zero Stick,Stick Back,Stick Forward\");\n";
   outStream << "      for (int i = 0; i < mAlpha.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         output.Writeln(write_str(mAlpha[i], \",\", mZeroStick[i], \",\", mBackStick[i], \",\", mForwardStick[i]));\n";
   outStream << "      }\n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";

   testFile.close();
}

void ScriptGeneratorSixDOF::CreateThrustDragTest(QString& aTestDir, double aAltitude_ft)
{
   std::stringstream testNameBuilder{};
   testNameBuilder << "Thrust Drag (" << static_cast<int>(aAltitude_ft) << " ft)";
   QString testHeading = QString::fromStdString(testNameBuilder.str());
   testNameBuilder << " Test";
   QString testName = QString::fromStdString(testNameBuilder.str());

   QFile testFile(aTestDir + "/" + testName.toLower().replace(" ", "_") + ".txt");
   if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      return;
   }

   QTextStream outStream(&testFile);
   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "//TEST_HEADER=====================================================\n";
   outStream << "//\n";
   outStream << "//TITLE " << testName << "\n";
   outStream << "//\n";
   outStream << "//TEST_GROUP Aircraft Tests\n";
   outStream << "//\n";
   outStream << "//DESCRIPTION This test compares the drag and thrust curves at a specified altitude\n";
   outStream << "//\n";
   outStream << "//DETAILED_DESCRIPTIONS\n";
   outStream << "// This test compares the drag and thrust curves at a specified altitude. It plots, as a function of Mach, the drag, afterburner thrust, and military power thrust.\n";
   outStream << "//END_DETAILED_DESCRIPTION\n";
   outStream << "//END_TEST_HEADER=================================================\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   double gTestAltitude_ft = " << aAltitude_ft << ";\n";
   outStream << "   double gInitialMach = 0.0;\n";
   outStream << "   double gFinalMach   = 3.0;\n";
   outStream << "   double gStepMach    = 0.1;\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   string outputFileName = \"" << mOutputDir.absolutePath() << "/" << mVehicleName.toLower() << "_" << testName.toLower().replace(" ", "_") << ".csv\";\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "include_once ../../six_dof_types/environment/six_dof_environment" + GetFilenameSuffix() + ".txt\n";
   outStream << "include_once ../../six_dof_types/" << mVehicleType << "/" << mVehicleName.toLower() << "/" << mVehicleName.toLower() << GetFilenameSuffix() << ".txt\n";
   outStream << "include_once common.txt\n";
   outStream << "\n";
   outStream << "platform_type TEST_PLATFORM WSF_PLATFORM\n";
   outStream << "   spatial_domain air\n";
   outStream << "   mover " << GetScriptMoverType() << "\n";
   outStream << "      vehicle_type      " << mVehicleName.toUpper() << "\n";
   outStream << "      update_interval   0.01 sec\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   six_dof_ned_heading          0 deg\n";
   outStream << "   six_dof_ned_pitch            0 deg\n";
   outStream << "   six_dof_ned_roll             0 deg\n";
   outStream << "   six_dof_set_velocity_ned_fps 100 0 0\n";
   outStream << "   six_dof_position             0 0\n";
   outStream << "   six_dof_alt                  5000 m\n";
   outStream << "   \n";
   outStream << "   edit mover\n";
   outStream << "      engines_on true\n";
   outStream << "   end_mover\n";
   outStream << "   \n";
   outStream << "   on_initialize\n";
   outStream << "      EnableControls(PLATFORM, false);\n";
   outStream << "      SetMasterNoLagTesting(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeAltitude(PLATFORM, false);\n";
   outStream << "      SetMasterFreezePitch(PLATFORM, false);\n";
   outStream << "      SetMasterFreezeRoll(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeYaw(PLATFORM, true);\n";
   outStream << "      SetMasterFreezeFuelBurn(PLATFORM, false);\n";
   outStream << "      SetMasterNoAlphaTesting(PLATFORM, true);\n";
   outStream << "   end_on_initialize\n";
   outStream << "end_platform_type\n";
   outStream << "\n";
   outStream << "script_struct DataSet\n";
   outStream << "   script_variables\n";
   outStream << "      double mach          = 0;\n";
   outStream << "      double thrustMil_lbs = 0;\n";
   outStream << "      double thrustAB_lbs  = 0;\n";
   outStream << "      double drag_lbs      = 0;\n";
   outStream << "   end_script_variables\n";
   outStream << "end_script_struct\n";
   outStream << "\n";
   outStream << "script_variables\n";
   outStream << "   Array<struct> mData          = Array<struct>();\n";
   outStream << "end_script_variables\n";
   outStream << "\n";
   outStream << "script void CollectData(WsfPlatform aPlatform)\n";
   outStream << "   WsfMover mover = aPlatform.Mover();\n";
   outStream << "   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n";
   outStream << "   {\n";
   outStream << "      WsfSixDOF_Mover           sixdofMover = (WsfSixDOF_Mover)mover;\n";
   outStream << "      WsfSixDOF_MoverTestObject testObj    = sixdofMover.__getTestObject();\n";
   outStream << "      \n";
   outStream << "      double testWeight = sixdofMover.GetEmptyWeight() + 0.5 * sixdofMover.GetTotalFuelCapacity();\n";
   outStream << "      \n";
   outStream << "      int machSteps = MATH.Floor((gFinalMach - gInitialMach) / gStepMach) + 1;\n";
   outStream << "      for (int machIdx = 0; machIdx <= machSteps; machIdx = machIdx + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach = gInitialMach + machIdx * gStepMach;\n";
   outStream << "         struct data = struct.New(\"DataSet\");\n";
   outStream << "         data->mach = mach;\n";
   outStream << "         \n";
   outStream << "         data->thrustMil_lbs = Math.LB_PER_NT() * testObj.GetLevelFlightHorizontalThrust(Math.M_PER_FT() * gTestAltitude_ft, mach, 1, testWeight);   // 1 is military power\n";
   outStream << "         data->thrustAB_lbs  = Math.LB_PER_NT() * testObj.GetLevelFlightHorizontalThrust(Math.M_PER_FT() * gTestAltitude_ft, mach, 2, testWeight);   // 2 is afterburner\n";
   outStream << "         data->drag_lbs      = Math.LB_PER_NT() * testObj.GetLevelFlightDrag(Math.M_PER_FT() * gTestAltitude_ft, mach, testWeight);\n";
   outStream << "         \n";
   outStream << "         mData.PushBack(data);\n";
   outStream << "      }\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "script void WriteOutput()\n";
   outStream << "   FileIO output = FileIO();\n";
   outStream << "   if (output.Open(outputFileName, \"out\"))\n";
   outStream << "   {\n";
   outStream << "      output.Writeln(\"Meta,title=" << testHeading << ",y_label=Forces (lbs)\");\n";
   outStream << "      output.Writeln(\"Mach,Drag,,Mach,Thrust - Military,,Mach,Thrust - Afterburner\");\n";
   outStream << "      \n";
   outStream << "      for (int i = 0; i < mData.Size(); i = i + 1)\n";
   outStream << "      {\n";
   outStream << "         double mach   = mData[i]->mach;\n";
   outStream << "         double thrMIL = mData[i]->thrustMil_lbs;\n";
   outStream << "         double thrAB  = mData[i]->thrustAB_lbs; \n";
   outStream << "         double drag   = mData[i]->drag_lbs;\n";
   outStream << "         output.Writeln(write_str(mach, \",\", drag, \",,\", mach, \",\", thrMIL, \",,\", mach, \",\", thrAB));\n";
   outStream << "      }\n";
   outStream << "      \n";
   outStream << "      output.Close();\n";
   outStream << "   }\n";
   outStream << "end_script\n";
   outStream << "\n";
   outStream << "platform test_platform TEST_PLATFORM\n";
   outStream << "   on_initialize2\n";
   outStream << "      CollectData(PLATFORM);\n";
   outStream << "      WriteOutput();\n";
   outStream << "   end_on_initialize2\n";
   outStream << "end_platform\n";
   outStream << "\n";
   outStream << "minimum_mover_timestep 1 ms\n";
   outStream << "end_time 48 hrs\n";
   outStream << "\n";

   testFile.close();
}

QString ScriptGeneratorSixDOF::GetFlyStraightLevelFullThrottleScript()
{
   QString scriptStr = "\n";
   scriptStr.append("// This uses maximum throttle (value=2.0, afterburner). If the vehicle\n");
   scriptStr.append("// does not have an afterburner, it will use full throttle \n");
   scriptStr.append("// (value=1.0, military power) instead.\n");
   scriptStr.append("script void FlyStraightLevelFullThrottle(WsfPlatform aPlatform)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover();\n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover;\n");
   scriptStr.append("      sixdofMover.__getTestObject().FlyStraightLevelFullThrottle();\n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetRangeSinceStart_kmScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script double GetRangeSinceStart_km(WsfPlatform aPlatform)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      return sixdofMover.__getTestObject().GetRangeSinceStart_km(); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("      return false; \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterNoLagTestingScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterNoLagTesting(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterNoLagTesting(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterFreezeAltitudeScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeAltitude(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterFreezeAltitude(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterFreezePitchScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezePitch(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterFreezePitch(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterFreezeRollScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeRoll(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterFreezeRoll(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterFreezeVerticalSpeedScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeVerticalSpeed(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterFreezeVerticalSpeed(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterFreezeYawScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeYaw(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterFreezeYaw(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterFreezeFuelBurnScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterFreezeFuelBurn(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterFreezeFuelBurn(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetSetMasterNoAlphaTestingScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void SetMasterNoAlphaTesting(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if(mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.__getTestObject().SetMasterNoAlphaTesting(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetEnableControlsScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script void EnableControls(WsfPlatform aPlatform, bool aEnable)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      sixdofMover.EnableControls(aEnable); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetFuelRemainingScript()
{
   QString scriptStr = "\n";
   scriptStr.append("script double GetFuelRemaining(WsfPlatform aPlatform)\n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover(); \n");
   scriptStr.append("   if (mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover sixdofMover = (WsfSixDOF_Mover)mover; \n");
   scriptStr.append("      return sixdofMover.GetTotalFuelRemaining(); \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\"); \n");
   scriptStr.append("      return 0.0; \n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

QString ScriptGeneratorSixDOF::GetFlightEnvelopeScripts()
{
   QString scriptStr = "\n";

   scriptStr.append("script_struct DataPair\n");
   scriptStr.append("   script_variables\n");
   scriptStr.append("      double velocity_fps = 0;\n");
   scriptStr.append("      double altitude_ft  = 0;\n");
   scriptStr.append("   end_script_variables\n");
   scriptStr.append("end_script_struct\n");
   scriptStr.append("\n");
   scriptStr.append("script_variables\n");
   scriptStr.append("   //******************************************\n");
   scriptStr.append("   //These are input variables to control the generation\n");
   scriptStr.append("   // of AB and Mil flight envelopes\n");
   scriptStr.append("   double feInitialAltitude_ft  = 2000;\n");
   scriptStr.append("   double feFinalAltitude_ft    = 70000;\n");
   scriptStr.append("   double feAltitudeStep_ft     = 2000;\n");
   scriptStr.append("   double feAltitudePrecision   = 10.0;\n");
   scriptStr.append("   \n");
   scriptStr.append("   double feInitialVelocity_fps = 100;\n");
   scriptStr.append("   double feFinalVelocity_fps   = 3000;\n");
   scriptStr.append("   double feVelocityStep_fps    = 25;\n");
   scriptStr.append("   double feVelocityPrecision   = 1.0;\n");
   scriptStr.append("   \n");
   scriptStr.append("   //******************************************\n");
   scriptStr.append("   //Do not change the remaining variables here.\n");
   scriptStr.append("   //These are not input variables\n");
   scriptStr.append("   Array<struct> bottomSide = Array<struct>();\n");
   scriptStr.append("   Array<struct> topSide = Array<struct>();\n");
   scriptStr.append("   \n");
   scriptStr.append("   double throttleSetting; //Do not initialize here, as it is set prior to use, and it might cause confusion\n");
   scriptStr.append("   double flapSettings; //Do not initialize here, as it is set prior to use, and it might cause confusion\n");
   scriptStr.append("   int gLoad; //Do not initialize here, as it is set prior to use, and it might cause confusion\n");
   scriptStr.append("   \n");
   scriptStr.append("   double minCanFlyVelocity = 0.0;\n");
   scriptStr.append("   double minCanFlyAltitude = 0.0;\n");
   scriptStr.append("   //******************************************\n");
   scriptStr.append("end_script_variables\n");
   scriptStr.append("\n");
   scriptStr.append("script int CanFlyWithG_Load(WsfSixDOF_Mover aPlatform, double aAltitude, double aVelocity, double aThrottleSetting, double aFlapsSetting, int aGLoad)\n");
   scriptStr.append("   int output = aPlatform.__getTestObject().CanFlyAt(Math.M_PER_FT() * aAltitude, Math.M_PER_FT() * aVelocity, aThrottleSetting, aFlapsSetting);\n");
   scriptStr.append("\n");
   scriptStr.append("   if(aPlatform.MaxPotentialManeuverGLoad() >= aGLoad )\n");
   scriptStr.append("   {\n");
   scriptStr.append("      return output;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("   return 0;\n");
   scriptStr.append("   }\n");
   scriptStr.append("        \n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script bool ComputeMinFlightSpeed(WsfSixDOF_Mover aPlatform, double aAltitude, double aMinSpeed, double aMaxSpeed, double aSpeedStep)\n");
   scriptStr.append("   double velocity = aMinSpeed;\n");
   scriptStr.append("   while(velocity <= aMaxSpeed)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, aAltitude, velocity, throttleSetting, flapSettings, gLoad) == 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aSpeedStep <= feVelocityPrecision ||  // If step is small enough (accuracy is acheived, return result)\n");
   scriptStr.append("            aMinSpeed == velocity) // if Can fly at min velocity, return result\n");
   scriptStr.append("         {\n");
   scriptStr.append("            struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("            temp->altitude_ft = aAltitude;  \n");
   scriptStr.append("            temp->velocity_fps = velocity;  \n");
   scriptStr.append("            topSide.PushBack(temp);\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMinFlightSpeed(aPlatform, aAltitude, velocity - aSpeedStep, velocity, aSpeedStep / 2);\n");
   scriptStr.append("         }\n");
   scriptStr.append("      }\n");
   scriptStr.append("      velocity = velocity + aSpeedStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   return false;\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script bool ComputeMaxFlightSpeed(WsfSixDOF_Mover aPlatform, double aAltitude, double aMinSpeed, double aMaxSpeed, double aSpeedStep)\n");
   scriptStr.append("   double velocity = aMinSpeed;\n");
   scriptStr.append("   while(velocity <= aMaxSpeed)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, aAltitude, velocity, throttleSetting, flapSettings, gLoad) != 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aSpeedStep <= feVelocityPrecision)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("            temp->altitude_ft = aAltitude;  \n");
   scriptStr.append("            temp->velocity_fps = velocity - aSpeedStep;   \n");
   scriptStr.append("            bottomSide.PushBack(temp);\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMaxFlightSpeed(aPlatform, aAltitude, velocity - aSpeedStep, velocity, aSpeedStep / 2);\n");
   scriptStr.append("         }\n");
   scriptStr.append("      }\n");
   scriptStr.append("      velocity = velocity + aSpeedStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   return false;\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script void ComputeStep1(WsfSixDOF_Mover aPlatform)\n");
   scriptStr.append("   if(ComputeMinFlightSpeed(aPlatform, feInitialAltitude_ft, feInitialVelocity_fps, feFinalVelocity_fps, feVelocityStep_fps))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      minCanFlyVelocity = topSide[0]->velocity_fps;\n");
   scriptStr.append("      ComputeMaxFlightSpeed(aPlatform, feInitialAltitude_ft, minCanFlyVelocity, feFinalVelocity_fps, feVelocityStep_fps);\n");
   scriptStr.append("   }\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script double ComputeMinFlightAltitude(WsfSixDOF_Mover aPlatform, double aVelocity, double aStartAltitude, double aEndAltitude, double aAltitudeStep)\n");
   scriptStr.append("\n");
   scriptStr.append("   double alt_ft = aStartAltitude;\n");
   scriptStr.append("   while(alt_ft <= aEndAltitude)\n");
   scriptStr.append("   {     \n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, alt_ft, aVelocity, throttleSetting, flapSettings, gLoad) == 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aAltitudeStep <= feAltitudePrecision || alt_ft <= aStartAltitude)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            minCanFlyAltitude = alt_ft;\n");
   scriptStr.append("            \n");
   scriptStr.append("            if(alt_ft != feInitialAltitude_ft)\n");
   scriptStr.append("            {\n");
   scriptStr.append("               struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("               temp->altitude_ft = alt_ft;  \n");
   scriptStr.append("               temp->velocity_fps = aVelocity;  \n");
   scriptStr.append("               bottomSide.PushBack(temp);\n");
   scriptStr.append("            }\n");
   scriptStr.append("            \n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else if(alt_ft < aAltitudeStep)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            minCanFlyAltitude = feInitialAltitude_ft;\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMinFlightAltitude(aPlatform, aVelocity, alt_ft - aAltitudeStep, alt_ft, aAltitudeStep / 2);\n");
   scriptStr.append("         }\n");
   scriptStr.append("      }\n");
   scriptStr.append("      alt_ft += aAltitudeStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   minCanFlyAltitude = 0.0;\n");
   scriptStr.append("   return false;\n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script bool ComputeMaxFlightAltitude(WsfSixDOF_Mover aPlatform, double aVelocity, double aStartAltitude, double aEndAltitude, double aAltitudeStep)\n");
   scriptStr.append("   \n");
   scriptStr.append("   double alt_ft = aStartAltitude;\n");
   scriptStr.append("   while(alt_ft <= aEndAltitude)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      if(CanFlyWithG_Load(aPlatform, alt_ft, aVelocity, throttleSetting, flapSettings, gLoad) != 2) // 2 == CAN_FLY\n");
   scriptStr.append("      {\n");
   scriptStr.append("         if(aAltitudeStep <= feAltitudePrecision)\n");
   scriptStr.append("         {\n");
   scriptStr.append("            struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("            temp->altitude_ft = alt_ft;  \n");
   scriptStr.append("            temp->velocity_fps = aVelocity;  \n");
   scriptStr.append("            topSide.PushBack(temp);\n");
   scriptStr.append("            return true;\n");
   scriptStr.append("         }\n");
   scriptStr.append("         else\n");
   scriptStr.append("         {\n");
   scriptStr.append("            return ComputeMaxFlightAltitude(aPlatform, aVelocity, alt_ft - aAltitudeStep, alt_ft, aAltitudeStep / 2);\n");
   scriptStr.append("         }        \n");
   scriptStr.append("      }\n");
   scriptStr.append("     \n");
   scriptStr.append("      alt_ft = alt_ft + aAltitudeStep;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   if(CanFlyWithG_Load(aPlatform, alt_ft, aVelocity, throttleSetting, flapSettings, gLoad) == 2)  // 2 == CAN_FLY\n");
   scriptStr.append("   {\n");
   scriptStr.append("      struct temp = struct.New(\"DataPair\");\n");
   scriptStr.append("      temp->altitude_ft = aEndAltitude;  \n");
   scriptStr.append("      temp->velocity_fps = aVelocity;   \n");
   scriptStr.append("      topSide.PushBack(temp);\n");
   scriptStr.append("      return true;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   return false;\n");
   scriptStr.append("   \n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("script void ComputeStep2(WsfSixDOF_Mover aPlatform)\n");
   scriptStr.append("   double currentVelocity = (minCanFlyVelocity + feVelocityStep_fps) / feVelocityStep_fps;\n");
   scriptStr.append("   currentVelocity = Math.Floor(currentVelocity) * feVelocityStep_fps;\n");
   scriptStr.append("   \n");
   scriptStr.append("   bool completed = false;\n");
   scriptStr.append("   while(currentVelocity <= feFinalVelocity_fps && !completed)\n");
   scriptStr.append("   {\n");
   scriptStr.append("      write(\".\");\n");
   scriptStr.append("      if(ComputeMinFlightAltitude(aPlatform, currentVelocity, feInitialAltitude_ft, feFinalAltitude_ft, feAltitudeStep_ft))\n");
   scriptStr.append("      {\n");
   scriptStr.append("         ComputeMaxFlightAltitude(aPlatform, currentVelocity, minCanFlyAltitude, feFinalAltitude_ft, feAltitudeStep_ft);\n");
   scriptStr.append("      }\n");
   scriptStr.append("      else //There is no altitude at which the platform can fly for the given speed\n");
   scriptStr.append("      {\n");
   scriptStr.append("         completed = true;\n");
   scriptStr.append("      }\n");
   scriptStr.append("      \n");
   scriptStr.append("      currentVelocity = currentVelocity + feVelocityStep_fps;\n");
   scriptStr.append("   }\n");
   scriptStr.append("   writeln(\".\");\n");
   scriptStr.append("      \n");
   scriptStr.append("end_script\n");
   scriptStr.append("\n");
   scriptStr.append("// This computes the flight envelope for a platform at the specified throttle and flaps settings.\n");
   scriptStr.append("// The throttle input assumes that 0 is idle, 1 is full power without augmentation/afterburner\n");
   scriptStr.append("// (military power), and 2 is full power with full augmentation/afterburner (full afterburner).\n");
   scriptStr.append("script Array<struct> ComputeFlightEnvelope(WsfPlatform aPlatform, double aThrottle, double aFlaps, int aGLoad)\n");
   scriptStr.append("   Array<struct> returnVal = Array<struct>();\n");
   scriptStr.append("   \n");
   scriptStr.append("   throttleSetting = aThrottle;\n");
   scriptStr.append("   flapSettings = aFlaps;\n");
   scriptStr.append("   gLoad = aGLoad;\n");
   scriptStr.append("   \n");
   scriptStr.append("   bottomSide.Clear();\n");
   scriptStr.append("   topSide.Clear();\n");
   scriptStr.append("   \n");
   scriptStr.append("   WsfMover mover = aPlatform.Mover();\n");
   scriptStr.append("   if(mover.IsA_TypeOf(\"WSF_SIX_DOF_MOVER\"))\n");
   scriptStr.append("   {\n");
   scriptStr.append("      WsfSixDOF_Mover           sixdofMover = (WsfSixDOF_Mover)mover;\n");
   scriptStr.append("      WsfSixDOF_MoverTestObject testObj     = sixdofMover.__getTestObject();\n");
   scriptStr.append("      testObj.SetMasterNoLagTesting(true);\n");
   scriptStr.append("      testObj.SetMasterFreezeAltitude(true);\n");
   scriptStr.append("      testObj.SetMasterFreezeFuelBurn(true);\n");
   scriptStr.append("      testObj.SetMasterFreezeLocation(true);\n");
   scriptStr.append("      \n");
   scriptStr.append("      write(\".\");\n");
   scriptStr.append("      ComputeStep1(sixdofMover);\n");
   scriptStr.append("      ComputeStep2(sixdofMover);\n");
   scriptStr.append("      \n");
   scriptStr.append("   }\n");
   scriptStr.append("   else\n");
   scriptStr.append("   {\n");
   scriptStr.append("      writeln(\"--ERROR-- Not a P6DOF Mover.\");\n");
   scriptStr.append("   }\n");
   scriptStr.append("   \n");
   scriptStr.append("   for(int i = 0; i < topSide.Size(); i = i + 1)\n");
   scriptStr.append("   {     \n");
   scriptStr.append("      returnVal.PushBack(topSide[i]);\n");
   scriptStr.append("   } \n");
   scriptStr.append("   for(int i = bottomSide.Size() - 1; i >= 0; i = i - 1)\n");
   scriptStr.append("   {     \n");
   scriptStr.append("      returnVal.PushBack(bottomSide[i]);\n");
   scriptStr.append("   } \n");
   scriptStr.append("   \n");
   scriptStr.append("   return returnVal;\n");
   scriptStr.append("end_script\n");
   return scriptStr;
}

} // namespace Designer
