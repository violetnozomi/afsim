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

#include "VerticalMapFunction.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "Utility.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"

const double VerticalMapFunction::cUNDEFINED = 1.0E+30;

// =================================================================================================
VerticalMapFunction::VerticalMapFunction(const WsfScenario& aScenario)
   : MapPlotFunction(aScenario)
   , mSimulationPtr(nullptr)
   , mSensor()
   , mTarget(aScenario.GetAtmosphere())
   , mGroundRangeMin(cUNDEFINED)
   , mGroundRangeMax(cUNDEFINED)
   , mGroundRangeStep(1.0)
   , mGroundRangeUnits("nm")
   , mAltitudeMin(cUNDEFINED)
   , mAltitudeMax(cUNDEFINED)
   , mAltitudeStep(0.25)
   , mAltitudeUnits("nm")
   , mPdMapFile()
   , mHeaderLine1("")
   , mHeaderLine2("")
   , mHeaderLine3("")
   , mOutputColumnLimit(100)
   , mGnuPlotFile()
   , mGnuPlotPlayerFile()
   , mFixedSensorPosition(true)
{
   mGroundRangeStep = UtInput::ConvertValueFrom(mGroundRangeStep, mGroundRangeUnits, UtInput::cLENGTH);
   mAltitudeStep    = UtInput::ConvertValueFrom(mAltitudeStep, mAltitudeUnits, UtInput::cLENGTH);
}

// =================================================================================================
// virtual
bool VerticalMapFunction::Execute(WsfSimulation& aSimulation)
{
   MapPlotFunction::Execute(aSimulation);
   mSimulationPtr = &aSimulation;
   if (mPdMapFile.empty() && mGnuPlotFile.empty())
   {
      ut::log::error() << "Either pd_map_file or gnuplot_file must be specified.";
      return false;
   }

   if ((mGroundRangeMax == cUNDEFINED) || (mAltitudeMax == cUNDEFINED))
   {
      ut::log::error() << "Target region not specified.";
      return false;
   }


   if ((!mSensor.CreateAndInitialize(aSimulation)) || (!mTarget.CreateAndInitialize(aSimulation)))
   {
      return false;
   }

   // Initialize the map plot variables
   if (!mPlotVariables.Initialize(aSimulation))
   {
      return false;
   }

   // Select Pd as the plot variable if none have selected
   if (mPlotVariables.Count() == 0)
   {
      mPlotVariables.AddVariable("pd");
   }

   // Initialize the default bearing, will be corrected for range based direction.
   double setBearing, heading, temp1, temp2;
   mSensor.GetPlatform()->GetOrientationNED(heading, temp1, temp2);
   mSensor.GetSensor()->GetOrientationWCS(setBearing, temp1, temp2);
   setBearing += heading;
   setBearing = UtMath::NormalizeAngleMinusPi_Pi(setBearing);

   WsfSensorResult result;

   // Don't let Pd stop the terrain check processing.
   mSensor.GetSettings().mRequiredPd = 1.0E-6;

   double rowMin   = mGroundRangeMin;
   double rowMax   = mGroundRangeMax;
   double rowStep  = mGroundRangeStep;
   double rowScale = 1.0 / UtInput::ConvertValueFrom(1.0, mGroundRangeUnits, UtInput::cLENGTH);
   double colMin   = mAltitudeMin;
   double colMax   = mAltitudeMax;
   double colStep  = mAltitudeStep;
   double colScale = 1.0 / UtInput::ConvertValueFrom(1.0, mAltitudeUnits, UtInput::cLENGTH);

   std::vector<double> rowValues;
   std::vector<double> colValues;
   std::vector<Point>  dataValues;
   std::vector<double> varValues;

   for (double rowValue = rowMin; rowValue <= (rowMax + 0.1 * rowStep); rowValue += rowStep)
   {
      rowValues.push_back(rowValue * rowScale);
      for (double colValue = colMin; colValue <= (colMax + 0.1 * colStep); colValue += colStep)
      {
         if (rowValue == rowMin)
         {
            colValues.push_back(colValue * colScale);
         }
         double altitude    = colValue;
         double groundRange = rowValue;
         double bearing     = setBearing;
         if (groundRange < 0.0)
         {
            groundRange = -groundRange;
            bearing += UtMath::cPI;
            bearing = UtMath::NormalizeAngleMinusPi_Pi(bearing);
         }

         if (mFixedSensorPosition)
         {
            // Set the location of the target.
            mTarget.SetLocationRBA(mSensor, groundRange, bearing, altitude);
            // double tgtLocWCS[3];
            // Utility::ConvertRBA_ToWCS(mSensor.GetPlatform(), groundRange, bearing, altitude,
            //                           mSensor.GetEarthRadiusMultiplier(),
            //                           tgtLocWCS);
            // mTarget.GetPlatform()->SetLocationWCS(tgtLocWCS);
         }
         else
         {
            // Set the location of the sensor.
            double snrLocWCS[3];
            Utility::ConvertRBA_ToWCS(mTarget.GetPlatform(),
                                      groundRange,
                                      bearing,
                                      altitude,
                                      mSensor.GetEarthRadiusMultiplier(),
                                      snrLocWCS);
            mSensor.GetPlatform()->SetLocationWCS(snrLocWCS);
         }
         mTarget.SetSpeedAndAttitude(mSensor);

         // Cue the sensor to the target just in case this is a tracker.
         mSensor.CueToTarget(mTarget);

         // Do the detection chance
         mSensor.AttemptToDetect(mTarget, result);

         // Extract the desired data
         mPlotVariables.SetDownRange(groundRange);
         mPlotVariables.SetCrossRange(0.0);
         mPlotVariables.InitializeValues(aSimulation, varValues, std::vector<double>());
         mPlotVariables.Evaluate(mSensor.GetSimTime(), mSensor.GetSensor(), result, varValues);
         dataValues.push_back(varValues);
      }
   }

   // Generate the output file.

   ut::log::info() << "Writing output.";
   if (!mGnuPlotFile.empty())
   {
      WriteGnuPlotFile(aSimulation, rowValues, colValues, dataValues);
   }
   if (!mPdMapFile.empty())
   {
      WritePdMapFile(rowValues, colValues, dataValues, 0);
   }
   if (!mGnuPlotPlayerFile.empty())
   {
      WriteGnuPlotPlayerFile(aSimulation);
   }

   return true;
}

// =================================================================================================
// virtual
bool VerticalMapFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "pd_map_file")
   {
      aInput.ReadValueQuoted(mPdMapFile);
      mPdMapFile = aInput.SubstitutePathVariables(mPdMapFile);
   }
   else if (command == "header_line_1")
   {
      aInput.ReadLine(mHeaderLine1, false);
      if (!mHeaderLine1.empty())
      {
         mHeaderLine1 = mHeaderLine1.substr(1);
      }
   }
   else if (command == "header_line_2")
   {
      aInput.ReadLine(mHeaderLine2, false);
      if (!mHeaderLine2.empty())
      {
         mHeaderLine2 = mHeaderLine2.substr(1);
      }
   }
   else if (command == "header_line_3")
   {
      aInput.ReadLine(mHeaderLine3, false);
      if (!mHeaderLine3.empty())
      {
         mHeaderLine3 = mHeaderLine3.substr(1);
      }
   }
   else if (command == "output_column_limit")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreater(value, 0);
      mOutputColumnLimit = static_cast<size_t>(value);
   }
   else if (command == "gnuplot_file")
   {
      aInput.ReadValueQuoted(mGnuPlotFile);
      mGnuPlotFile = aInput.SubstitutePathVariables(mGnuPlotFile);
   }
   else if (command == "gnuplot_player_file")
   {
      aInput.ReadValueQuoted(mGnuPlotPlayerFile);
      mGnuPlotPlayerFile = aInput.SubstitutePathVariables(mGnuPlotPlayerFile);
   }
   else if (command == "ground_range_limits")
   {
      aInput.ReadValueOfType(mGroundRangeMin, UtInput::cLENGTH);
      aInput.ReadValueOfType(mGroundRangeMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mGroundRangeMax, mGroundRangeMin);
   }
   else if (command == "altitude_limits")
   {
      aInput.ReadValueOfType(mAltitudeMin, UtInput::cLENGTH);
      aInput.ReadValueOfType(mAltitudeMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mAltitudeMax, mAltitudeMin);
   }
   else if (command == "ground_range_step")
   {
      aInput.ReadValue(mGroundRangeStep);
      aInput.ReadValue(mGroundRangeUnits);
      mGroundRangeStep = aInput.ConvertValue(mGroundRangeStep, mGroundRangeUnits, UtInput::cLENGTH);
      aInput.ValueGreater(mGroundRangeStep, 0.0);
   }
   else if (command == "altitude_step")
   {
      aInput.ReadValue(mAltitudeStep);
      aInput.ReadValue(mAltitudeUnits);
      mAltitudeStep = aInput.ConvertValue(mAltitudeStep, mAltitudeUnits, UtInput::cLENGTH);
      aInput.ValueGreater(mAltitudeStep, 0.0);
   }
   else if (command == "fixed_target_position")
   {
      mFixedSensorPosition = false;
   }
   else if (command == "fixed_sensor_position")
   {
      mFixedSensorPosition = true;
   }
   else if (mSensor.ProcessInput(aInput))
   {
   }
   else if (mTarget.ProcessInput(aInput))
   {
   }
   else if (MapPlotFunction::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Write output gnuplot 'splot' output.
void VerticalMapFunction::WriteGnuPlotFile(WsfSimulation&             aSimulation,
                                           const std::vector<double>& aRowValues,
                                           const std::vector<double>& aColValues,
                                           const std::vector<Point>&  aVarValues)
{
   std::ofstream ofs(mGnuPlotFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mGnuPlotFile;
      return;
   }

   ofs << "# gnuplot 'splot' data file\n";
   ofs << "#\n";
   ofs << "# Column 1: Ground Range (" << mGroundRangeUnits << ")\n";
   ofs << "# Column 2: Altitude (" << mAltitudeUnits << ")\n";
   for (size_t i = 0; i < mPlotVariables.Count(); ++i)
   {
      ofs << "# Column " << i + 3 << ": " << mPlotVariables.GetLabel(i) << '\n';
   }

   size_t rowCount    = aRowValues.size();
   size_t colCount    = aColValues.size();
   size_t varCount    = aVarValues[0].size();
   size_t rowColIndex = 0;
   for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      for (size_t colIndex = 0; colIndex < colCount; ++colIndex)
      {
         ofs << std::setw(10) << aRowValues[rowIndex] << ' ' << std::setw(10) << aColValues[colIndex];
         for (size_t varIndex = 0; varIndex < varCount; ++varIndex)
         {
            ofs << ' ' << std::setw(10) << aVarValues[rowColIndex][varIndex];
         }
         ofs << '\n';
         ++rowColIndex;
      }
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("GNU Plot", mGnuPlotFile);
}

// =================================================================================================
//! Write the PdMap output if requested.
void VerticalMapFunction::WritePdMapFile(const std::vector<double>& aRowValues,
                                         const std::vector<double>& aColValues,
                                         const std::vector<Point>&  aVarValues,
                                         size_t                     aVarIndex)
{
   std::ofstream ofs(mPdMapFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mPdMapFile;
      return;
   }

   size_t rowCount = aRowValues.size();
   size_t colCount = aColValues.size();

   const std::string& headerLine3 = !mHeaderLine3.empty() ? mHeaderLine3 : GetPdMapFileJsonMetadata();

   ofs << mHeaderLine1 << '\n';
   ofs << mHeaderLine2 << '\n';
   ofs << headerLine3 << '\n';
   ofs << "   " << rowCount << "  " << colCount << '\n';

   size_t rowIndex;
   size_t colIndex;
   size_t itemsOnLine;

   ofs << "          ";
   itemsOnLine = 0;
   for (colIndex = 0; colIndex < colCount; ++colIndex)
   {
      if (itemsOnLine >= mOutputColumnLimit)
      {
         itemsOnLine = 0;
         ofs << "\n          ";
      }
      ofs << ' ' << std::setw(10) << aColValues[colIndex];
      ++itemsOnLine;
   }
   ofs << '\n';

   size_t rowColIndex = 0;
   for (rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      ofs << std::setw(10) << aRowValues[rowIndex];
      itemsOnLine = 0;
      for (colIndex = 0; colIndex < colCount; ++colIndex)
      {
         if (itemsOnLine >= mOutputColumnLimit)
         {
            itemsOnLine = 0;
            ofs << "\n          ";
         }
         ofs << ' ' << std::setw(10) << aVarValues[rowColIndex][aVarIndex];
         ++itemsOnLine;
         ++rowColIndex;
      }
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("PD Map", mPdMapFile);
}

// =================================================================================================
//! Write output gnuplot player location output.
void VerticalMapFunction::WriteGnuPlotPlayerFile(WsfSimulation& aSimulation)
{
   std::ofstream ofs(mGnuPlotPlayerFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mGnuPlotPlayerFile;
      return;
   }

   double groundRangeScale = 1.0 / UtInput::ConvertValueFrom(1.0, mGroundRangeUnits, UtInput::cLENGTH);
   double altitudeScale    = 1.0 / UtInput::ConvertValueFrom(1.0, mAltitudeUnits, UtInput::cLENGTH);

   ofs << "#\n";
   ofs << "# Column 1: Ground Range (" << mGroundRangeUnits << ")\n";
   ofs << "# Column 2: Altitude (" << mAltitudeUnits << ")\n";
   ofs << "# Column 3: Dummy";
   ofs << "#\n";

   size_t pltfrmCnt     = 0;
   size_t platformCount = aSimulation.GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = aSimulation.GetPlatformEntry(platformEntry);

      // Skip the target platform.
      if (platformPtr == mTarget.GetPlatform())
      {
         continue;
      }

      ofs << "# Player " << ++pltfrmCnt << ": " << platformPtr->GetName();
      ofs << '\n';

      double locationWCS[3];
      platformPtr->GetLocationWCS(locationWCS);

      double groundRange;
      double bearing;
      double altitude;
      mSensor.ConvertWCS_ToRBA(locationWCS, groundRange, bearing, altitude);

      ofs << std::setw(10) << groundRange * groundRangeScale;
      ofs << ' ' << std::setw(10) << altitude * altitudeScale;
      ofs << ' ' << std::setw(10) << '1';
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("GNU Plot Player", mGnuPlotPlayerFile);
}

// =================================================================================================
//! Returns the JSON metadata to be associated with a PD map output file. The
//! metadata contains the name of the variable that the data pertains to as
//! well as the names of the data that make up the X and Y axes. This metadata
//! can be used to label the data in visualizations.
std::string VerticalMapFunction::GetPdMapFileJsonMetadata() const
{
   const std::string variable("\"variable\": \"" + mPlotVariables.GetLabel(0) + "\"");
   const std::string xAxis("\"x\": \"Ground range (" + mGroundRangeUnits + ")\"");
   const std::string yAxis("\"y\": \"Altitude (" + mAltitudeUnits + ")\"");
   const std::string json("{ " + variable + ", " + xAxis + ", " + yAxis + " }");
   return json;
}
