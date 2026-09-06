// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ScriptGeneratorPointMassSixDOF.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>
#include <QDate>
#include <QFile>
#include <QString>

#include "AeroJetEngine.hpp"
#include "GeometryPointMass.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "AutopilotSupportFileWork.hpp"
#include "UtLog.hpp"
#include "UtInput.hpp"

namespace Designer
{

ScriptGeneratorPointMassSixDOF::ScriptGeneratorPointMassSixDOF()
{
   mPlatformName = mVehicleName.toLower() + "_pm6dof";
   mPlatformType = mVehicleName.toUpper() + "_PM6DOF";

   mTestDir.setPath(mVehicleDir.absolutePath() + "/tests/pm6dof/");
   mOutputDir.setPath(mVehicleDir.absolutePath() + "/output/pm6dof/");
}

void ScriptGeneratorPointMassSixDOF::CreateMoverFile()
{
    // TODO  Reduce commonality with ScriptGeneratorSixDOF::CreateMoverFile()

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
            [=](const QString& engineInclude) {return engineInclude == tempString; });

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
    outStream << "   point_mass_vehicle_type " << mVehicleName.toUpper() << " BASE_TYPE\n\n";
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
    outStream << "      end_mass_properties\n\n";

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

            if (!mEngineList.empty())
            {
                // Loop through and add each engine
                for (auto curEngine : mEngineList)
                {
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

    outStream << "   end_point_mass_vehicle_type\n";
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

void ScriptGeneratorPointMassSixDOF::CreateControlsInputFile(QString& aControlsDir)
{
    // TODO  Address commonality with ScriptGeneratorSixDOF::CreateControlsInputFile

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
   outStream << "   synthetic_pilot\n";
   outStream << "      # Define the autopilot configuration\n";
   outStream << "      include autopilot_config" + GetFilenameSuffix() + ".txt\n";
   outStream << "   end_synthetic_pilot\n";
   outStream << "\n";

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

   outStream << "   # Make the synthetic pilot active\n";
   outStream << "   active_pilot synthetic_pilot\n";
   outStream << "\n";
   outStream << "   # This autopilot support file is common to all pilots\n";
   outStream << "   # that use a SixDOF_CommonController (autopilot)\n";
   outStream << "   common_autopilot_support_file      autopilot_support_tables" + GetFilenameSuffix() +".txt\n";
   outStream << "\n";
   outStream << "end_pilot_manager\n";
   controlFile.close();

   //create autopilot_config.txt file
   CreateAutopilotConfigFile(aControlsDir);
   CreateAutopilotG_LimitConfigFile(aControlsDir);
}

void ScriptGeneratorPointMassSixDOF::CreateFlightControlsFile(QString& aControlsDir)
{
   // Starting-point rate commands, should work well for most applications

   QFile controlFile(aControlsDir + "/flight_controls" + GetFilenameSuffix() + ".txt");
   if (!controlFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&controlFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   double speedbrakeLag = 0.0;

   if (mVehiclePtr->IsAircraft())
   {
      outStream << "flight_controls\n\n";
      outStream << "   stick_back # Map of stick throw vs pitch rate command (deg/s)\n";
      outStream << "      -1.0  -6.0\n";
      outStream << "       0.0   0.0\n";
      outStream << "       1.0  10.0\n";
      outStream << "   end_stick_back\n\n";
      outStream << "   stick_right # Map of stick throw vs roll rate command (deg/s)\n";
      outStream << "      -1.0  -360.0\n";
      outStream << "       0.0     0.0\n";
      outStream << "       1.0   360.0\n";
      outStream << "   end_stick_right\n\n";
      outStream << "   rudder_right # Map of rudder throw vs yaw rate command (deg/s)\n";
      outStream << "      -1.0  -5.0\n";
      outStream << "       0.0   0.0\n";
      outStream << "       1.0   5.0\n";
      outStream << "   end_rudder_right\n\n";
      outStream << "   speedbrakes\n";
      outStream << "      lag_time_constant " << speedbrakeLag << " sec\n";
      outStream << "   end_speedbrakes\n\n";
      outStream << "end_flight_controls\n\n";
   }
   else if (mVehiclePtr->IsWeapon())
   {
      outStream << "flight_controls\n\n";
      outStream << "   stick_back # Map of stick throw vs pitch rate command (deg/s)\n";
      outStream << "      -1.0  -20.0\n";
      outStream << "       0.0    0.0\n";
      outStream << "       1.0   20.0\n";
      outStream << "   end_stick_back\n\n";
      outStream << "   stick_right # Map of stick throw vs roll rate command (deg/s)\n";
      outStream << "      -1.0  -540.0\n";
      outStream << "       0.0     0.0\n";
      outStream << "       1.0   540.0\n";
      outStream << "   end_stick_right\n\n";
      outStream << "   rudder_right # Map of rudder throw vs yaw rate command (deg/s)\n";
      outStream << "      -1.0  -20.0\n";
      outStream << "       0.0    0.0\n";
      outStream << "       1.0   20.0\n";
      outStream << "   end_rudder_right\n\n";
      outStream << "end_flight_controls\n\n";
   }

   controlFile.close();
}

void ScriptGeneratorPointMassSixDOF::CreateAeroFile(QString& aAeroDir)
{
   // Derived from ScriptGenerator::CreateMoverFile(), and simplified and
   // modified to account for peculiarities in the conversion process

   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   QFile aeroFile(aAeroDir + "/aero" + GetFilenameSuffix() + ".txt");
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   CreateAeroTables(aAeroDir);

   QTextStream outStream(&aeroFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "# This file defines the aerodynamics of the vehicle.\n";
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

   double refArea_sqft = vehicleAero->UseRefArea() ? vehicleAero->GetVehicleRefArea_ft2() : vehicleAero->GetVehicleWingArea_ft2();

   outStream << "   ref_area_sqft    ";
   OutputData(outStream, refArea_sqft, width, precision, alignment);
   outStream << "\n";

   const std::vector<std::string> enabledTablesList = mVehiclePtr->GetAero()->GetCoreAero().GetEnabledTables();
   for (const auto& curTable : enabledTablesList)
   {
       QStringList tableStrList = QString::fromStdString(curTable).split(" ");
       QString tableFileName = "";
       QString tableKeyWord = "";
       for (auto curStr : tableStrList)
       {
           if (curStr == "vs" || curStr == "(trimmed)")
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
       outStream << "      file data/" << tableFileName << "\n";
       outStream << "   end_" << tableKeyWord << "\n";
       outStream << "\n";
   }
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "end_aero_data\n";
   outStream << "\n";
   outStream << "# **************************************************************************************************\n";
   outStream << "\n";

   aeroFile.close();
}

void ScriptGeneratorPointMassSixDOF::CreateAeroTables(QString& aAeroDir)
{
   QString aeroTablesDir = aAeroDir + "/data";

   //Create core aero table files

   // Lift and Drag
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetTrimCL_AlphaBetaMachTablePtr());
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetTrimCd_AlphaBetaMachTablePtr());
   Create4dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetTrimCY_AlphaBetaMachTablePtr());

   // Remaining tables are 2D
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetRollAccelCapability_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetPitchAccelCapability_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetYawAccelCapability_MachTable());

   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetRollStabilityFrequency_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetPitchStabilityFrequency_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetYawStabilityFrequency_MachTable());

   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetSpeedbrakeDeltaCD_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetFlapsDeltaCL_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetFlapsDeltaCD_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetSpoilersDeltaCL_MachTable());
   Create2dTableFile(aeroTablesDir, mVehiclePtr->GetAero()->GetCoreAero().GetSpoilersDeltaCD_MachTable());
}

void ScriptGeneratorPointMassSixDOF::CreatePerformanceTestFiles(QString& aTestDir)
{
    if (mVehiclePtr == nullptr)
    {
        MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
        return;
    }

    // Performance tests that will be used for all cases:
    CreateCommonTestFile(aTestDir);
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

} // namespace Designer
