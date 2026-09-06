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

#include "BrawlerConverter.hpp"

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

BrawlerConverter::BrawlerConverter(const QString& aFileName)
{
   QFile source(aFileName);
   if (!source.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      auto error = ut::log::error() << "Unable to open file for conversion.";
      error.AddNote() << "File: " << aFileName.toStdString();

      if (!source.exists())
      {
         error.AddNote() << "File was not found.";
      }

      return;
   }

   if (!mBrawlerMover.LoadAeroConfig(aFileName.toStdString()))
   {
      auto error = ut::log::error() << "Brawler vehicle data could not be loaded.";
      error.AddNote() << "File: " << aFileName.toStdString();
   }

   mTableVec.push_back(&mLiftTable);
   mTableVec.push_back(&mDragTable);
   mTableVec.push_back(&mRollAccelerationTable);
   mTableVec.push_back(&mPitchAccelerationTable);
   mTableVec.push_back(&mYawAccelerationTable);
   mTableVec.push_back(&mRollFrequencyTable);
   mTableVec.push_back(&mPitchFrequencyTable);
   mTableVec.push_back(&mYawFrequencyTable);
   mTableVec.push_back(&mSpeedbrakeDragTable);
}

bool BrawlerConverter::OutputSixDOF_Vehicle()
{
   // Default controls to CAS, with an active yaw damper
   auto& controls = mVehiclePtr->GetControls();
   controls.SetBankToTurnControlSettings(VehicleControls::cAILERONS_ONLY,
                                         VehicleControls::cYAW_DAMPER,
                                         1.0);
   controls.SetManualPilotDirectControlSettings(true,
                                                true,
                                                true,
                                                0.0,
                                                0.0,
                                                0.0,
                                                VehicleControls::cCONTROL_AUGMENTATION);
   controls.SetManualPilotCAS_Settings(9.0,
                                       360.0);

   // Populate aero tables
   mVehiclePtr->GenerateAeroDataFromGeometry();

   auto aeroData = mVehiclePtr->GetAero();
   aeroData->SetVehicleRefArea_ft2(mBrawlerMover.rarea);

   PopulateCD_Table();
   PopulateCL_Table();

   // Debug checks to ensure we're entering our data correctly
   std::vector<double> testMach = { 0.5, 0.8, 1.0, 1.2, 2.0 };
   std::vector<double> testAlpha = { -5, -2.5, 0, 2.5, 5};

   for (double mach : testMach)
   {
      auto log = ut::log::debug() << "Mach " << mach;
      for (double alpha : testAlpha)
      {
         double CL = 0.0;
         mLiftTable.GetInterpolatedDependentValueBase(alpha, 0.0, mach, CL);

         log.AddNote() << "Alpha " << alpha << ": CL " << CL;
      }
   }

   double temperature_R = 0.0;
   double pressure_slug = 0.0;
   double density_slug = 0.0;
   double speedOfSound_fps = 0.0;
   double pressureRatio = 0.0;
   double densityRatio = 0.0;
   mBrawlerMover.atmos(0.0,
                       temperature_R,
                       pressure_slug,
                       density_slug,
                       speedOfSound_fps,
                       pressureRatio,
                       densityRatio);

   int numMach = mBrawlerMover.nfm2;
   std::vector<double> machs;
   for (int machIdx = 0; machIdx < numMach; machIdx++)
   {
      machs.push_back(mBrawlerMover.fmvec2[machIdx]);
   }

   // These are effectively placeholder tables and should be replaced
   // by data informed by outputs from the Brawler mover
   mVehiclePtr->GetAero()->Populate2D_Table(mRollAccelerationTable, 0.8, 180.0, machs);
   mVehiclePtr->GetAero()->Populate2D_Table(mPitchAccelerationTable, 0.8, 80.0, machs);
   mVehiclePtr->GetAero()->Populate2D_Table(mYawAccelerationTable, 0.8, 80.0, machs);

   mVehiclePtr->GetAero()->Populate2D_Table(mRollFrequencyTable, 0.8, 1.0, machs);
   mVehiclePtr->GetAero()->Populate2D_Table(mPitchFrequencyTable, 0.8, 0.05, machs);
   mVehiclePtr->GetAero()->Populate2D_Table(mYawFrequencyTable, 0.8, 10.0, machs);

   // Autogen the entire vehicle and folder structure
   GenerateDataFiles();

   // Update the autopilot support tables
   AutopilotSupportFileWork supportProcess(mVehiclePtr, mTestDir);
   supportProcess.BeginWork();

   return true;
}

void BrawlerConverter::PopulateCD_Table()
{
   // Populate drag table
   mDragTable.Clear();

   int numAlpha = mBrawlerMover.nalph2;
   int numMach = mBrawlerMover.nfm3;

   double alphaZeroLift = mBrawlerMover.alpha0;

   for (int machIdx = 0; machIdx < numMach; machIdx++)
   {
      double mach = mBrawlerMover.fmvec3[machIdx];

      mDragTable.AddSecondaryTable(mach);

      std::vector<double> betas = { -0.01, 0.0, 0.01 };
      int numBeta = ut::cast_to_int(betas.size());
      for (int betaIdx = 0; betaIdx < numBeta; betaIdx++)
      {
         mDragTable.AddPrimaryTable(betas.at(betaIdx) * UtMath::cDEG_PER_RAD);

         // Bottom half of the drag table (if applicable)
         // Use n - 1 here to avoid duplicating the center point later
         if (mBrawlerMover.aoa_tab_type == reflective)
         {
            for (int alphaIdx = numAlpha - 1; alphaIdx > 0; alphaIdx--)
            {
               double alpha = -(mBrawlerMover.alfv2[alphaIdx] + alphaZeroLift) * UtMath::cDEG_PER_RAD;

               double dragCoefficient = mBrawlerMover.cdrag[machIdx + alphaIdx * numMach];
               mDragTable.AddData(alpha, dragCoefficient);
            }
         }

         // Top half of the drag table
         for (int alphaIdx = 0; alphaIdx < numAlpha; alphaIdx++)
         {
            double alpha = (mBrawlerMover.alfv2[alphaIdx] + alphaZeroLift) * UtMath::cDEG_PER_RAD;

            double dragCoefficient = mBrawlerMover.cdrag[machIdx + alphaIdx * numMach];
            mDragTable.AddData(alpha, dragCoefficient);
         }
      }
   }

}

void BrawlerConverter::PopulateCL_Table()
{
   // Populate lift table
   mLiftTable.Clear();

   int numAlpha = mBrawlerMover.nalph1;
   int numMach = mBrawlerMover.nfm2;

   double alphaZeroLift = mBrawlerMover.alpha0;

   for (int machIdx = 0; machIdx < numMach; machIdx++)
   {
      double mach = mBrawlerMover.fmvec2[machIdx];

      mLiftTable.AddSecondaryTable(mach);

      std::vector<double> betas = { -0.01, 0.0, 0.01 };
      int numBeta = ut::cast_to_int(betas.size());
      for (int betaIdx = 0; betaIdx < numBeta; betaIdx++)
      {
         mLiftTable.AddPrimaryTable(betas.at(betaIdx) * UtMath::cDEG_PER_RAD);

         // Bottom half of the lift table (if applicable)
         // Use n - 1 here to avoid duplicating the center point later
         if (mBrawlerMover.aoa_tab_type == reflective)
         {
            for (int alphaIdx = numAlpha - 1; alphaIdx > 0; alphaIdx--)
            {
               // Negate lift to reflect data about the origin
               double alpha = -(mBrawlerMover.alfv2[alphaIdx] + alphaZeroLift) * UtMath::cDEG_PER_RAD;

               double coefficient = mBrawlerMover.clift[machIdx + alphaIdx * numMach];
               mLiftTable.AddData(alpha, -coefficient);
            }
         }

         // Top half of the lift table
         for (int alphaIdx = 0; alphaIdx < numAlpha; alphaIdx++)
         {
            double alpha = (mBrawlerMover.alfv2[alphaIdx] + alphaZeroLift) * UtMath::cDEG_PER_RAD;

            // Check this indexing
            double coefficient = mBrawlerMover.clift[machIdx + alphaIdx * numMach];

            mLiftTable.AddData(alpha, coefficient);
         }
      }
   }
}

void BrawlerConverter::CreateMoverFile()
{
   // Derived from ScriptGenerator::CreateMoverFile(), and simplified and
   // modified to account for peculiarities in the conversion process

   QString moverFolderPath = mVehicleDir.absolutePath() + "/six_dof_types/" + mVehicleType + "/" + mVehicleName.toLower();
   QString moverPath = moverFolderPath + "/" + mVehicleName.toLower() + GetFilenameSuffix() + ".txt";
   QFile moverFile(moverPath);
   if (!moverFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
   QTextStream outStream(&moverFile);

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   CreateAutogenHeader(mVehiclePtr, outStream);

   // Include all of the engines, but list each type only once
   QString engineModel(mVehicleName + "_Engine");

   // Set the temporary string for this engine
   QString engineInclude = "include_once prop/jet_engines/" + engineModel.toLower() + "/" + engineModel.toLower() + ".txt\n";

   outStream << engineInclude;
   outStream << "\n";
   outStream << "six_dof_object_types\n";
   outStream << "   point_mass_vehicle_type " << mVehicleName.toUpper() << " BASE_TYPE\n\n";
   outStream << "      include aero/aero" + GetFilenameSuffix() + ".txt                 # Defines the vehicle aerodynamics\n";

   outStream << "      include controls/control_inputs" + GetFilenameSuffix() + ".txt   # Defines the control inputs (stick, rudder, throttle, etc)\n";
   outStream << "      include controls/flight_controls" + GetFilenameSuffix() + ".txt  # Defines how the control inputs move the control surfaces\n";
   outStream << "\n";

   int width = 16;
   int precision = 5;
   QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

   outStream.setRealNumberNotation(QTextStream::FixedNotation);

   outStream << "      mass_properties\n";
   outStream << "         mass                       ";
   OutputData(outStream, mBrawlerMover.EmptyMass() * 32.174, width, precision, alignment);
   outStream << " lbs\n";
   outStream << "      end_mass_properties\n\n";

   // Vehicles should include a propulsion system definition, even if no engines/fuel are present

   outStream << "      propulsion_data\n";
   outStream << "\n";

   // Loop through and add each engine
   QString curEngineName = QString("Engine");

   outStream << "         engine " << curEngineName << "  " << engineModel.toUpper() << "\n";

   outStream << "            fuel_feed               ";
   OutputData(outStream, QString("MainFuelTank"), width, alignment);
   outStream << "\n";

   outStream << "         end_engine\n";
   outStream << "\n";

   // Fuel tank(s)
   outStream << "         # Current fuel tank support is limited to a single tank.\n";
   outStream << "         # Future Mover Creator upgrades may support more complex, multi-tank fuel systems.\n";
   outStream << "\n";

   double fuel_max     = mBrawlerMover.Gas() * 32.174;
   double fuel_current = fuel_max;

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

   outStream << "      end_propulsion_data\n\n";

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

template<class T> T GetPercentile(std::vector<T> aDataSet, double aPercentile)
{
   std::sort(aDataSet.begin(), aDataSet.end());

   double fraction = aPercentile / 100.0;

   double valuesCount = static_cast<double>(aDataSet.size());
   size_t index = static_cast<size_t>(valuesCount * fraction);

   if (index < 0) { index = 0; }
   if (index > aDataSet.size()) { index = aDataSet.size() - 1; }

   return aDataSet.at(index);
}

template<class T> T GetPercentile(T* aDataSet, size_t aDataSetSize, double aPercentile)
{
   std::vector<T> dataVector;
   for (size_t i = 0; i < aDataSetSize; i++)
   {
      dataVector.push_back(aDataSet[i]);
   }

   return GetPercentile(dataVector, aPercentile);
}

template<class T> std::vector<T> ElementwiseDivide(std::vector<T> aNumerator, std::vector<T> aDenominator)
{
   std::vector<T> quotient;
   if (aNumerator.size() != aDenominator.size())
   {
      auto error = ut::log::error() << "Numerator and denominator vector inputs to ElementwiseDivide() must be of the same size.";
      error.AddNote() << "Numerator size: " << aNumerator.size();
      error.AddNote() << "Denominator size: " << aDenominator.size();
      error.AddNote() << "Empty vector returned.";
      return quotient;
   }

   quotient.reserve(aNumerator.size());
   for (size_t i = 0; i < aNumerator.size(); i++)
   {
      quotient.push_back(aNumerator.at(i) / aDenominator.at(i));
   }

   return quotient;
}

template<class T> std::vector<T> ElementwiseDivide(T* aNumeratorData, size_t aNumeratorDataSize, T* aDenominatorData, size_t aDenominatorDataSize)
{
   std::vector<T> numeratorVector;
   numeratorVector.reserve(aNumeratorDataSize);
   for (size_t i = 0; i < aNumeratorDataSize; i++)
   {
      numeratorVector.push_back(aNumeratorData[i]);
   }

   std::vector<T> denominatorVector;
   denominatorVector.reserve(aDenominatorDataSize);
   for (size_t i = 0; i < aDenominatorDataSize; i++)
   {
      denominatorVector.push_back(aDenominatorData[i]);
   }

   return ElementwiseDivide(numeratorVector, denominatorVector);
}

void BrawlerConverter::CreatePropulsionFile()
{
   // Most of the functionality in this function is very duplicative and should
   // eventually be turned into a JSON export/import, and allow
   // ScriptGeneratorSixDOF::CreatePropulsionFile() to be made non-virtual

   QString propulsionFolderPath = mVehicleDir.absolutePath() + "/six_dof_types/" + mVehicleType + "/" + mVehicleName.toLower() + "/prop";
   QString enginePath = propulsionFolderPath + "/" + mVehicleName.toLower() + ".txt";

   // For each engine, create a directory and engine file
   QString engineType("jet_engines");
   QString engineModel(mVehicleName + "_Engine");

   // Create engine folder and related parent folders
   CreateFolderIfDoesntExist(propulsionFolderPath);
   QString engineTypePath = propulsionFolderPath + "/" + engineType;
   CreateFolderIfDoesntExist(engineTypePath);
   QString engineFolderPath = propulsionFolderPath + "/" + engineType + "/" + engineModel;
   CreateFolderIfDoesntExist(engineFolderPath);

   // Output the engine file
   QString engineFilename = engineFolderPath + "/" + engineModel.toLower() + ".txt";

   QDir directory(engineFilename);
   directory.cdUp();
   directory.mkdir(QString::fromStdString("data"));
   directory.setPath(directory.path() + QString::fromStdString("/data"));

   QFile file(engineFilename);
   file.remove();
   if (file.open(QIODevice::ReadWrite))
   {
      QTextStream stream(&file);

      stream.setRealNumberNotation(QTextStream::FixedNotation);
      QTextStream::FieldAlignment alignment = QTextStream::FieldAlignment::AlignRight;

      bool hasAfterburner = true;
      int numberThrustValues = mBrawlerMover.nhts * mBrawlerMover.nmach1;

      auto tsfcIdleVector = ElementwiseDivide(mBrawlerMover.floidl, numberThrustValues, mBrawlerMover.tidle, numberThrustValues);
      double tsfcIdle_slugps = GetPercentile(tsfcIdleVector, 50.0);
      double tsfcIdle = tsfcIdle_slugps * 3600.0 * 32.174;

      auto tsfcMilVector = ElementwiseDivide(mBrawlerMover.flomil, numberThrustValues, mBrawlerMover.tmil, numberThrustValues);
      double tsfcMil_slugps = GetPercentile(tsfcMilVector, 50.0);
      double tsfcMil = tsfcMil_slugps * 3600.0 * 32.174;

      QDate Today = QDate::currentDate();
      stream << "# ##################################################################################################" << endl;
      stream << "# " << endl;
      VersionInfo::OutputVersionStringInOutputStream(stream);
      stream << "# " << endl;
      stream << "# This file defines a PM6DOF model of " << engineModel.toUpper() << "." << endl;
      stream << "# " << endl;
      stream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << endl;
      stream << "# " << endl;
      stream << "# ##################################################################################################" << endl;
      stream << endl;
      stream << endl;
      stream << "six_dof_object_types" << endl;
      stream << endl;
      stream << "   point_mass_engine_type   " << engineModel.toUpper() << "   BASE_TYPE" << endl;
      stream << endl;
      stream << "      jet" << endl;
      stream << endl;
      stream << "      tsfc_idle_pph       ";
      OutputData(stream, tsfcIdle, 12, 4, alignment);
      stream << "  # lb/lb/hr" << endl;
      stream << "      tsfc_mil_pph        ";
      OutputData(stream, tsfcMil, 12, 4, alignment);
      stream << "  # lb/lb/hr" << endl;

      if (hasAfterburner)
      {
         auto tsfcAbVector = ElementwiseDivide(mBrawlerMover.flomax, numberThrustValues, mBrawlerMover.thrmx, numberThrustValues);
         double tsfcAb_slugps = GetPercentile(tsfcAbVector, 50.0);
         double tsfcAb = tsfcAb_slugps * 3600.0 * 32.174;

         stream << "      tsfc_ab_pph         ";
         OutputData(stream, tsfcAb, 12, 4, alignment);
         stream << "  # lb/lb/hr" << endl;
      }

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;

      double turbineSpinUp = 1.0 / mBrawlerMover.dtggup;
      {
         stream << "      spin_up_table_mil_per_sec" << endl;
         stream << "         // throttle_level  spin_up_per_sec" << endl;
         stream << "              ";
         OutputData(stream, 0.0, 12, 4, alignment);
         stream << "         ";
         OutputData(stream, turbineSpinUp, 12, 4, alignment);
         stream << endl;
         stream << "              ";
         OutputData(stream, 1.0, 12, 4, alignment);
         stream << "         ";
         OutputData(stream, turbineSpinUp, 12, 4, alignment);
         stream << endl;
         stream << "      end_spin_up_table_mil_per_sec" << endl;
         stream << endl;
      }

      double turbineSpinDown = 1.0 / mBrawlerMover.dtggdn;
      {
         stream << "      spin_down_table_mil_per_sec" << endl;
         stream << "         // throttle_level  spin_up_per_sec" << endl;
         stream << "              ";
         OutputData(stream, 0.0, 12, 4, alignment);
         stream << "         ";
         OutputData(stream, turbineSpinDown, 12, 4, alignment);
         stream << endl;
         stream << "              ";
         OutputData(stream, 1.0, 12, 4, alignment);
         stream << "         ";
         OutputData(stream, turbineSpinDown, 12, 4, alignment);
         stream << endl;
         stream << "      end_spin_down_table_mil_per_sec" << endl;
         stream << endl;
      }

      if (hasAfterburner)
      {
         double afterburnerRate = 1.0 / mBrawlerMover.abdt;
         {
            stream << "      spin_up_table_ab_per_sec" << endl;
            stream << "         // throttle_level  spin_up_per_sec" << endl;
            stream << "              ";
            OutputData(stream, 0.0, 12, 4, alignment);
            stream << "         ";
            OutputData(stream, afterburnerRate, 12, 4, alignment);
            stream << endl;
            stream << "              ";
            OutputData(stream, 1.0, 12, 4, alignment);
            stream << "         ";
            OutputData(stream, afterburnerRate, 12, 4, alignment);
            stream << endl;
            stream << "      end_spin_up_table_ab_per_sec" << endl;
            stream << endl;
         }

         {
            stream << "      spin_down_table_ab_per_sec" << endl;
            stream << "         // throttle_level  spin_down_per_sec" << endl;
            stream << "              ";
            OutputData(stream, 0.0, 12, 4, alignment);
            stream << "         ";
            OutputData(stream, afterburnerRate, 12, 4, alignment);
            stream << endl;
            stream << "              ";
            OutputData(stream, 1.0, 12, 4, alignment);
            stream << "         ";
            OutputData(stream, afterburnerRate, 12, 4, alignment);
            stream << endl;
            stream << "      end_spin_down_table_ab_per_sec" << endl;
            stream << endl;
         }
      }

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;
      stream << "      thrust_idle_alt_mach_table" << endl;
      stream << "         file " << "data/thrust_idle_alt_mach.txt" << endl;

      QFile idleFile(directory.path() + "/thrust_idle_alt_mach.txt");
      idleFile.remove();
      idleFile.open(QIODevice::ReadWrite);
      QTextStream idleStream(&idleFile);

      idleStream.setRealNumberNotation(QTextStream::FixedNotation);

      idleStream << "irregular_table" << endl;
      idleStream << "   independent_variable alt units ft" << endl;
      idleStream << "   independent_variable mach precision float" << endl;
      idleStream << "   dependent_variable precision float" << endl;

      double ratedThrustIdle = 0.0;

      for (int altIdx = 0; altIdx < mBrawlerMover.nhts; altIdx++)
      {
         idleStream << "   alt  ";
         OutputData(idleStream, mBrawlerMover.htvec[altIdx], 8, 1, alignment);
         idleStream << endl;

         idleStream << "      mach  ";
         for (int machIdx = 0; machIdx < mBrawlerMover.nmach1; machIdx++)
         {
            idleStream << "    ";
            OutputData(idleStream, mBrawlerMover.mchval[machIdx], 5, 2, alignment);
         }
         idleStream << endl;

         idleStream << "      values";

         for (int thrustIdx = 0; thrustIdx < mBrawlerMover.nmach1; thrustIdx++)
         {
            idleStream << "    ";
            double thrust = mBrawlerMover.tidle[thrustIdx + altIdx * mBrawlerMover.nmach1];
            OutputData(idleStream, thrust, 12, 4, alignment);

            if (thrust > ratedThrustIdle) { ratedThrustIdle = thrust; }
         }
         idleStream << endl;
      }

      idleStream << "end_irregular_table" << endl;
      idleFile.close();

      stream << "      end_thrust_idle_alt_mach_table" << endl;
      stream << endl;
      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;

      stream << "      rated_thrust_idle   ";
      OutputData(stream, ratedThrustIdle, 12, 4, alignment);
      stream << " lbf" << endl;

      stream << "      thrust_mil_alt_mach_table" << endl;
      stream << "         file " << "data/thrust_mil_alt_mach.txt" << endl;

      QFile milFile(directory.path() + "/thrust_mil_alt_mach.txt");
      milFile.remove();
      milFile.open(QIODevice::ReadWrite);
      QTextStream milStream(&milFile);

      milStream.setRealNumberNotation(QTextStream::FixedNotation);

      milStream << "irregular_table" << endl;
      milStream << "   independent_variable alt units ft" << endl;
      milStream << "   independent_variable mach precision float" << endl;
      milStream << "   dependent_variable precision float" << endl;

      double ratedThrustMil = 0.0;

      for (int altIdx = 0; altIdx < mBrawlerMover.nhts; altIdx++)
      {
         milStream << "   alt  ";
         OutputData(milStream, mBrawlerMover.htvec[altIdx], 8, 1, alignment);
         milStream << endl;

         milStream << "      mach  ";
         for (int machIdx = 0; machIdx < mBrawlerMover.nmach1; machIdx++)
         {
            milStream << "    ";
            OutputData(milStream, mBrawlerMover.mchval[machIdx], 5, 2, alignment);
         }
         milStream << endl;

         milStream << "      values";

         for (int thrustIdx = 0; thrustIdx < mBrawlerMover.nmach1; thrustIdx++)
         {
            milStream << "    ";
            double thrust = mBrawlerMover.tmil[thrustIdx + altIdx * mBrawlerMover.nmach1];
            OutputData(milStream, thrust, 12, 4, alignment);

            if (thrust > ratedThrustMil) { ratedThrustMil = thrust; }
         }
         milStream << endl;
      }

      milStream << "end_irregular_table" << endl;
      milFile.close();

      stream << "      end_thrust_mil_alt_mach_table" << endl;
      stream << endl;

      stream << "      rated_thrust_mil    ";
      OutputData(stream, ratedThrustMil, 12, 4, alignment);
      stream << " lbf" << endl;

      stream << "      //-------------------------------------------------------------------------" << endl;
      stream << endl;

      if (hasAfterburner)
      {
         stream << "      thrust_ab_alt_mach_table" << endl;
         stream << "         file " << "data/thrust_ab_alt_mach.txt" << endl;

         QFile abFile(directory.path() + "/thrust_ab_alt_mach.txt");
         abFile.remove();
         abFile.open(QIODevice::ReadWrite);
         QTextStream abStream(&abFile);

         abStream.setRealNumberNotation(QTextStream::FixedNotation);

         abStream << "irregular_table" << endl;
         abStream << "   independent_variable alt units ft" << endl;
         abStream << "   independent_variable mach precision float" << endl;
         abStream << "   dependent_variable precision float" << endl;

         double ratedThrustAb = 0.0;

         for (int altIdx = 0; altIdx < mBrawlerMover.nhts; altIdx++)
         {
            abStream << "   alt  ";
            OutputData(abStream, mBrawlerMover.htvec[altIdx], 8, 1, alignment);
            abStream << endl;

            abStream << "      mach  ";
            for (int machIdx = 0; machIdx < mBrawlerMover.nmach1; machIdx++)
            {
               abStream << "    ";
               OutputData(abStream, mBrawlerMover.mchval[machIdx], 5, 2, alignment);
            }
            abStream << endl;

            abStream << "      values";

            for (int thrustIdx = 0; thrustIdx < mBrawlerMover.nmach1; thrustIdx++)
            {
               abStream << "    ";
               double thrust = mBrawlerMover.thrmx[thrustIdx + altIdx * mBrawlerMover.nmach1];
               OutputData(abStream, thrust, 12, 4, alignment);

               if (thrust > ratedThrustAb) { ratedThrustAb = thrust; }
            }
            abStream << endl;
         }

         abStream << "end_irregular_table" << endl;
         abFile.close();

         stream << "      end_thrust_ab_alt_mach_table" << endl;
         stream << endl;

         stream << "      rated_thrust_ab     ";
         OutputData(stream, ratedThrustAb, 12, 4, alignment);
         stream << " lbf" << endl;
      }

      {
         stream << "      end_jet" << endl << endl;
         stream << "   end_point_mass_engine_type" << endl;
         stream << endl;
         stream << "end_six_dof_object_types" << endl;
      }

      stream << endl;

      file.close();
   }
}

void BrawlerConverter::CreateControlsInputFile(QString& aControlsDir)
{
   // Derived from ScriptGenerator::CreateMoverFile(), and simplified and
   // modified to account for peculiarities in the conversion process

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
   outStream << "   common_autopilot_support_file      autopilot_support_tables" + GetFilenameSuffix() + ".txt\n";
   outStream << "\n";
   outStream << "end_pilot_manager\n";
   controlFile.close();

   //create autopilot_config.txt file
   CreateAutopilotConfigFile(aControlsDir);
   CreateAutopilotG_LimitConfigFile(aControlsDir);
}

void BrawlerConverter::CreateAutopilotG_LimitConfigFile(QString& aControlsDir)
{
   // Derived from ScriptGenerator::CreateMoverFile(), and simplified and
   // modified to account for peculiarities in the conversion process

   if (mVehiclePtr == nullptr)
   {
      ut::log::error() << "Null vehicle in BrawlerConverter::CreateAutopilotConfigFile().";
      return;
   }

   QFile genFile(aControlsDir + "/autopilot_config_g_limits.txt");
   if (!genFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&genFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   double gLoadMax = mBrawlerMover.gcap[0][0];
   double gLoadMin = mBrawlerMover.gcap[1][0];

   outStream << "     pitch_gload_min             " << gLoadMin << "\n";
   outStream << "     pitch_gload_max             " << gLoadMax << "\n";
   outStream << "     yaw_gload_max               " << gLoadMax << "\n";

   genFile.close();
}

void BrawlerConverter::CreateFlightControlsFile(QString& aControlsDir)
{
   // Derived from ScriptGenerator::CreateMoverFile(), and simplified and
   // modified to account for peculiarities in the conversion process

   QFile controlFile(aControlsDir + "/flight_controls" + GetFilenameSuffix() + ".txt");
   if (!controlFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

   QTextStream outStream(&controlFile);

   CreateAutogenHeader(mVehiclePtr, outStream);

   double speedbrakeLag = 0.0;
   //if (mBrawlerMover.ddtd)
   //{
   //   speedbrakeLag = 1.0 / mBrawlerMover.ddtd;
   //}

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

   controlFile.close();
}

void BrawlerConverter::CreateAeroFile(QString& aAeroDir)
{
   // Derived from ScriptGenerator::CreateMoverFile(), and simplified and
   // modified to account for peculiarities in the conversion process

   VehicleAero* vehicleAero = mVehiclePtr->GetAero();
   if (vehicleAero == nullptr) { return; }

   QFile aeroFile(aAeroDir + "/aero" + GetFilenameSuffix() + ".txt");
   if (!aeroFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }

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

   double refArea_sqft = mBrawlerMover.rarea;

   outStream << "   ref_area_sqft    ";
   OutputData(outStream, refArea_sqft, width, precision, alignment);
   outStream << "\n";

   for (auto& curTable : mTableVec)
   {
      if (curTable->IsEmpty()) { continue; }

      curTable->UpdateModifiedData();

      QStringList tableStrList = QString::fromStdString(curTable->GetTableName()).split(" ");
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
      outStream << "      file data/" << tableFileName << "\n";
      outStream << "   end_" << tableKeyWord << "\n";
      outStream << "\n";
   }
   outStream << "# --------------------------------------------------------------------------------------------------\n";
   outStream << "\n";
   outStream << "end_aero_data\n";
   outStream << "\n";

   aeroFile.close();

   CreateAeroTables(aAeroDir);

}

void BrawlerConverter::CreateAeroTables(QString& aAeroDir)
{
   QString aeroTablesDir = aAeroDir + "/data";

   //Create core aero table files

   // Lift and Drag
   Create4dTableFile(aeroTablesDir, &mLiftTable);
   Create4dTableFile(aeroTablesDir, &mDragTable);

   // Remaining tables are 2D
   Create2dTableFile(aeroTablesDir, &mRollAccelerationTable);
   Create2dTableFile(aeroTablesDir, &mPitchAccelerationTable);
   Create2dTableFile(aeroTablesDir, &mYawAccelerationTable);

   Create2dTableFile(aeroTablesDir, &mRollFrequencyTable);
   Create2dTableFile(aeroTablesDir, &mPitchFrequencyTable);
   Create2dTableFile(aeroTablesDir, &mYawFrequencyTable);
}

} // namespace Designer
