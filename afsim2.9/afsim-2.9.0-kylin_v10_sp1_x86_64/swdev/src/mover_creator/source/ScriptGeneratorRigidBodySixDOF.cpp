// ****************************************************************************
// UNCLASSIFIED//FOUO
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// Distribution authorized to Department of Defense and U.S. DoD contractors only.
// You may not use this file except in compliance with the
// terms and conditions of 48 C.F.R. 252.204-7000 (Disclosure of Information),
// 48 C.F.R. 252.227-7025 (Limitations on the Use or Disclosure of Government-
// Furnished Information Marked with Restrictive Legends), and the AFSIM
// Memorandum of Understanding or Information Transfer Agreement as applicable.
// All requests for this software must be referred to the Air Force Research
// Laboratory Aerospace Systems Directorate, 2130 8th St., Wright-Patterson AFB,
// OH 45433. This software is provided "as is" without warranties of any kind.
//
// WARNING - EXPORT CONTROLLED
// This document contains technical data whose export is restricted by the
// Arms Export Control Act (Title 22, U.S.C. Sec 2751 et seq.) or the Export
// Administration Act of 1979, as amended, Title 50 U.S.C., App. 2401 et seq.
// Violations of these export laws are subject to severe criminal penalties.
// Disseminate in accordance with provisions of DoD Directive 5230.25.
// ****************************************************************************

#include "ScriptGeneratorRigidBodySixDOF.hpp"

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

ScriptGeneratorRigidBodySixDOF::ScriptGeneratorRigidBodySixDOF()
{
   mPlatformName = mVehicleName.toLower() + "_rb6dof";
   mPlatformType = mVehicleName.toUpper() + "_RB6DOF";

   mTestDir.setPath(mVehicleDir.absolutePath() + "/tests/rb6dof/");
   mOutputDir.setPath(mVehicleDir.absolutePath() + "/output/rb6dof/");
}

void ScriptGeneratorRigidBodySixDOF::CreateMoverFile()
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

void ScriptGeneratorRigidBodySixDOF::CreatePropulsionFile()
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

      curEngine->Export(engineFilename.toStdString(), AeroEngine::ExportMoverType::cRIGID_BODY);
   }
}

void ScriptGeneratorRigidBodySixDOF::CreateTestFiles()
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

void ScriptGeneratorRigidBodySixDOF::CreatePerformanceTestFiles(QString& aTestDir)
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
      case Designer::ScriptGenerator::PerformanceTestType::cNO_CONTROL_NO_ENGINE:
      {
         // Do nothing special, for now.
         break;
      }
      // Case for vehicles with no control with an engine
      // Example: boost stage on a missile
      case Designer::ScriptGenerator::PerformanceTestType::cNO_CONTROL_ENGINE:
      {
         CreateBallisticRangeTestFile(aTestDir);
         break;
      }
      // Case for a bank-to-turn vehicle.
      // Examples: aircraft, guided drone missiles
      case Designer::ScriptGenerator::PerformanceTestType::cBANK_TO_TURN:
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
      case Designer::ScriptGenerator::PerformanceTestType::cSKID_TO_TURN:
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

void ScriptGeneratorRigidBodySixDOF::CreateAeroFile(QString& aAeroDir)
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

void ScriptGeneratorRigidBodySixDOF::CreateAeroTables(QString& aAeroDir)
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

void ScriptGeneratorRigidBodySixDOF::CreateAeroComponentTables(QString& aAeroDir, VehicleAeroMovable aComponent)
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

void ScriptGeneratorRigidBodySixDOF::CreateFlightControlsFile(QString& aControlsDir)
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

void ScriptGeneratorRigidBodySixDOF::OutputControlDataForMovableComponents(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigStabsVerticalTail(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigStabsVentralTail(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigStabsUpperLowerTails(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigVTail(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigCanardsVerticalTail(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigCanardsStabsVerticalTail(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigFlyingWing(QTextStream& aOutputStream)
{
   VehicleGeometry& geometry = mVehiclePtr->GetGeometry();

   // Get controls
   VehicleControls& controls = mVehiclePtr->GetControls();

   OutputWingControl(aOutputStream, geometry, controls);
}

void ScriptGeneratorRigidBodySixDOF::OutputConfigStabsFlyingWing(QTextStream& aOutputStream)
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

bool ScriptGeneratorRigidBodySixDOF::OutputAileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputSpoileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputSpoilerControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputDrageronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputElevonControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputWingControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputHorizontalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputVerticalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputVentralTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputVTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

bool ScriptGeneratorRigidBodySixDOF::OutputCanardControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls)
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

void ScriptGeneratorRigidBodySixDOF::OutputControlSurface(QTextStream&                aOutputStream,
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

void ScriptGeneratorRigidBodySixDOF::OutputControlSurface(QTextStream&                aOutputStream,
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

void ScriptGeneratorRigidBodySixDOF::OutputActuatorData(QTextStream& aOutputStream,
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

void ScriptGeneratorRigidBodySixDOF::OutputControlInputBlock(QTextStream&                aOutputStream,
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

void ScriptGeneratorRigidBodySixDOF::OutputAngleMappingTable(QTextStream& aOutputStream,
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

void ScriptGeneratorRigidBodySixDOF::OutputPairsInAngleMappingTable(QTextStream&                                  aOutputStream,
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigTailFins(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigMidFins(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputConfigCanardFins(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputWingControlSurfaces(QTextStream& aOutputStream)
{
   // TODO: For each control surface that is present, output in a control_surface block
}

void ScriptGeneratorRigidBodySixDOF::OutputSpeedBrakes(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::OutputLandingGear(QTextStream& aOutputStream)
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

void ScriptGeneratorRigidBodySixDOF::CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream)
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

QString ScriptGeneratorRigidBodySixDOF::GenerateControlMethodString()
{
   QString controlMethodStr = "BANK_TO_TURN_NO_YAW";

   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorRigidBodySixDOF::GenerateControlMethodString().";
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

void ScriptGeneratorRigidBodySixDOF::CreateControlsInputFile(QString& aControlsDir)
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

void ScriptGeneratorRigidBodySixDOF::CreateAutopilotConfigFile(QString& aControlsDir)
{
   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in ScriptGeneratorRigidBodySixDOF::CreateAutopilotConfigFile().";
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

////////////////////////////////////////////////////////////////////
//          Performance tests file creation
////////////////////////////////////////////////////////////////////

void ScriptGeneratorRigidBodySixDOF::CreatePitchingMomentTest(QString& aTestDir, double aAltitude_ft, double aMach)
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

} // namespace Designer
