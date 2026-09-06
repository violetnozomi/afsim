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

#include "SphericalMapFunction.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"

namespace
{
// Make an angle (in degrees) 'printable'.
// If the result is very close to zero it will be forced to zero.
inline double MakePrintableAngle(double aAngleDeg)
{
   return (fabs(aAngleDeg) < 1.0E-12) ? 0.0 : aAngleDeg;
}
} // namespace

// =================================================================================================
SphericalMapFunction::SphericalMapFunction(const WsfScenario& aScenario)
   : MapPlotFunction(aScenario)
   , mSimulationPtr(nullptr)
   , mSensor()
   , mSensorPlatformYaw(0.0)
   , mSensorPlatformPitch(0.0)
   , mSensorPlatformRoll(0.0)
   , mSensorPlatformAltitude(0.0)
   , mTarget(aScenario.GetAtmosphere())
   , mAzMin(-UtMath::cPI)
   , mAzMax(UtMath::cPI)
   , mAzStep(1.0 * UtMath::cRAD_PER_DEG)
   , mElMin(-UtMath::cPI_OVER_2)
   , mElMax(UtMath::cPI_OVER_2)
   , mElStep(1.0 * UtMath::cRAD_PER_DEG)
   , mRange(0.0)
   , mAltitude(0.0)
   , mPdMapFile()
   , mHeaderLine1("")
   , mHeaderLine2("")
   , mHeaderLine3("")
   , mOutputColumnLimit(100)
   , mGnuPlotFile()
   , mFixedSensorPosition(true)
{
}

// =================================================================================================
// virtual
bool SphericalMapFunction::Execute(WsfSimulation& aSimulation)
{
   MapPlotFunction::Execute(aSimulation);
   mSimulationPtr = &aSimulation;
   if (mPdMapFile.empty() && mGnuPlotFile.empty())
   {
      ut::log::error() << "Either 'pd_map_file' or 'gnuplot_file' must be specified.";
      return false;
   }

   if ((mRange <= 0.0) && (mAltitude <= 0))
   {
      ut::log::error() << "'range' or 'altitude' must be specified.";
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

   // Set the orientation of the sensor platform.
   double heading;
   double pitch;
   double roll;
   double refAlt;
   double objAlt;
   mSensor.GetPlatform()->GetOrientationNED(heading, pitch, roll);
   heading = UtMath::NormalizeAngleMinusPi_Pi(heading + mSensorPlatformYaw);
   mSensor.GetPlatform()->SetOrientationNED(heading, mSensorPlatformPitch, mSensorPlatformRoll);

   WsfSensorResult result;
   // Don't let Pd stop the terrain check processing.
   mSensor.GetSettings().mRequiredPd = 1.0E-6;

   // Set the looping variables

   double rowMin   = mAzMin;
   double rowMax   = mAzMax;
   double rowStep  = mAzStep;
   double rowScale = UtMath::cDEG_PER_RAD;
   double colMin   = mElMin;
   double colMax   = mElMax;
   double colStep  = mElStep;
   double colScale = UtMath::cDEG_PER_RAD;

   std::vector<double> rowValues;
   std::vector<double> colValues;
   std::vector<Point>  dataValues;
   std::vector<double> varValues;
   int                 lastComplete = 0;

   for (double rowValue = rowMin; rowValue <= (rowMax + 0.1 * rowStep); rowValue += rowStep)
   {
      rowValues.push_back(rowValue * rowScale);

      int complete = (int)((double)(rowValue - rowMin) * 100.0 / (double)(rowMax - rowMin));
      if (complete >= (lastComplete + 10))
      {
         ut::log::info() << "Executing: " << complete << "% Complete.";
         lastComplete = complete;
      }

      for (double colValue = colMin; colValue <= (colMax + 0.1 * colStep); colValue += colStep)
      {
         if (rowValue == rowMin)
         {
            colValues.push_back(colValue * colScale);
         }

         mPlotVariables.InitializeValues(aSimulation, varValues, std::vector<double>());

         WsfPlatform* refPtr = nullptr;
         WsfPlatform* objPtr = nullptr;
         if (mFixedSensorPosition)
         {
            // Reference is the sensor (setting the target location).
            refPtr = mSensor.GetPlatform();
            objPtr = mTarget.GetPlatform();
            refPtr->GetOrientationNED(heading, pitch, roll);
            refAlt = mSensorPlatformAltitude;
            objAlt = mAltitude;
         }
         else
         {
            // Reference is the target (setting the sensor location).
            refPtr = mTarget.GetPlatform();
            objPtr = mSensor.GetPlatform();
            // TODO-NOTE 'target_heading' and 'target_yaw' should not be used and
            //            automatic target cueing should not be used???
            heading = 0.0;
            refAlt  = mAltitude;
            objAlt  = mSensorPlatformAltitude;
            // mSensor.SetAutomaticTargetCueing(false);
         }

         double azimuth   = rowValue;
         double elevation = colValue;
         double objLocNE(0.0);
         double objLocNED[3] = {0.0, 0.0, refAlt - objAlt};
         ;
         double bearing   = UtMath::NormalizeAngleMinusPi_Pi(heading + azimuth);
         bool   dataValid = true;

         if (mRange <= 0.0)
         {
            dataValid = false;
            // Set reference altitude
            refPtr->SetLocationLLA(0.0, 0.0, refAlt);

            // sideA  is the side from the Earth center to the object.
            // sideB  is the side from the Earth center to the reference.
            // sideC  is the side from the reference to the object location.

            double earthRadius = UtSphericalEarth::cEARTH_RADIUS;
            double sideA       = earthRadius + objAlt;
            double sideB       = earthRadius + refAlt;

            double angleA = UtMath::cPI_OVER_2 + elevation; // ambiguous solution if >= pi/2

            // if sinAngleB greater than one then there is not a solution
            double sinAngleB = (sideB / sideA) * sin(angleA);
            if (sinAngleB <= 1.0)
            {
               double angleB = asin(sinAngleB);
               if ((sideA - sideB) < 0.0)
               {
                  // Use alternate solution in these cases
                  angleB = UtMath::cPI - asin(sinAngleB);
               }
               double angleC = UtMath::cPI - (angleA + angleB);

               // double sideC = sideA * (sin(angleC) / sin(angleA));  // another way
               double sideC = sqrt((sideA * sideA) + (sideB * sideB) - (2.0 * sideA * sideB * cos(angleC)));

               bool anglesValid = (angleA > 0.0) && (angleB > 0.0) && (angleC > 0.0);
               if (anglesValid)
               {
                  objLocNE = sideC * cos(elevation);
                  UtVec3d::Set(objLocNED, objLocNE * cos(bearing), objLocNE * sin(bearing), -sideC * sin(elevation));
                  dataValid = true;
               }
            }
         }
         else
         {
            objLocNE = mRange * cos(elevation);
            UtVec3d::Set(objLocNED, objLocNE * cos(bearing), objLocNE * sin(bearing), -mRange * sin(elevation));
         }

         if (dataValid)
         {
            double objLocWCS[3];
            refPtr->ConvertNEDToWCS(objLocNED, objLocWCS);
            objPtr->SetLocationWCS(objLocWCS);

            // Set speed and attitude of target
            mTarget.SetSpeedAndAttitude(mSensor);

            // Cue the sensor to the target just in case this is a tracker.
            mSensor.CueToTarget(mTarget);

            // Do the detection chance
            mSensor.AttemptToDetect(mTarget, result);

            // Extract the desired data
            mPlotVariables.SetDownRange(objLocNE * cos(azimuth));  // approximate
            mPlotVariables.SetCrossRange(objLocNE * sin(azimuth)); // approximate

            mPlotVariables.Evaluate(mSensor.GetSimTime(), mSensor.GetSensor(), result, varValues);
         }

         dataValues.push_back(varValues);
      }
   }

   // Generate the output file.

   ut::log::info() << "Writing output.";
   if (!mGnuPlotFile.empty())
   {
      WriteGnuPlotFile(rowValues, colValues, dataValues);
   }

   if (!mPdMapFile.empty())
   {
      WritePdMapFile(rowValues, colValues, dataValues, 0);
   }

   return true;
}

// =================================================================================================
// virtual
bool SphericalMapFunction::ProcessInput(UtInput& aInput)
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
   else if (command == "azimuth_limits")
   {
      aInput.ReadValueOfType(mAzMin, UtInput::cANGLE);
      aInput.ReadValueOfType(mAzMax, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mAzMax, mAzMin);
   }
   else if (command == "azimuth_step")
   {
      aInput.ReadValueOfType(mAzStep, UtInput::cANGLE);
      aInput.ValueGreater(mAzStep, 0.0);
   }
   else if (command == "elevation_limits")
   {
      aInput.ReadValueOfType(mElMin, UtInput::cANGLE);
      aInput.ReadValueOfType(mElMax, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mElMax, mElMin);
   }
   else if (command == "elevation_step")
   {
      aInput.ReadValueOfType(mElStep, UtInput::cANGLE);
      aInput.ValueGreater(mElStep, 0.0);
   }
   else if (command == "range")
   {
      aInput.ReadValueOfType(mRange, UtInput::cLENGTH);
      aInput.ValueGreater(mRange, 0.0);
      mAltitude = 0.0;
   }
   else if (command == "altitude")
   {
      aInput.ReadValueOfType(mAltitude, UtInput::cLENGTH);
      mRange = 0.0;
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
   else if (command == "sensor_platform_yaw")
   {
      aInput.ReadValueOfType(mSensorPlatformYaw, UtInput::cANGLE);
      aInput.ValueInClosedRange(mSensorPlatformYaw, -UtMath::cPI, UtMath::cPI);
   }
   else if (command == "sensor_platform_pitch")
   {
      aInput.ReadValueOfType(mSensorPlatformPitch, UtInput::cANGLE);
      aInput.ValueInClosedRange(mSensorPlatformPitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "sensor_platform_roll")
   {
      aInput.ReadValueOfType(mSensorPlatformRoll, UtInput::cANGLE);
      aInput.ValueInClosedRange(mSensorPlatformRoll, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "sensor_platform_altitude")
   {
      aInput.ReadValueOfType(mSensorPlatformAltitude, UtInput::cLENGTH);
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
void SphericalMapFunction::WriteGnuPlotFile(const std::vector<double>& aRowValues,
                                            const std::vector<double>& aColValues,
                                            const std::vector<Point>&  aVarValues)
{
   std::ofstream ofs(mGnuPlotFile.c_str());
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mGnuPlotFile;
      return;
   }

   ofs << "# gnuplot 'splot' data file\n";
   ofs << "#\n";
   ofs << "# Column 1: Azimuth\n";
   ofs << "# Column 2: Elevation\n";
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
         ofs << std::setw(10) << MakePrintableAngle(aRowValues[rowIndex]) << ' ';
         ofs << std::setw(10) << MakePrintableAngle(aColValues[colIndex]);
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
void SphericalMapFunction::WritePdMapFile(const std::vector<double>& aRowValues,
                                          const std::vector<double>& aColValues,
                                          const std::vector<Point>&  aVarValues,
                                          size_t                     aVarIndex)
{
   std::ofstream ofs(mPdMapFile.c_str());
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
      ofs << ' ' << std::setw(10) << MakePrintableAngle(aColValues[colIndex]);
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
         ofs << ' ' << std::setw(10) << MakePrintableAngle(aVarValues[rowColIndex][aVarIndex]);
         ++itemsOnLine;
         ++rowColIndex;
      }
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("PD Map", mPdMapFile);
}

// =================================================================================================
//! Returns the JSON metadata to be associated with a PD map output file. The
//! metadata contains the name of the variable that the data pertains to as
//! well as the names of the data that make up the X and Y axes. This metadata
//! can be used to label the data in visualizations; e.g., in the IDE.
std::string SphericalMapFunction::GetPdMapFileJsonMetadata() const
{
   const std::string variable("\"variable\": \"" + mPlotVariables.GetLabel(0) + "\"");
   const std::string xAxis("\"x\": \"Azimuth (deg)\"");
   const std::string yAxis("\"y\": \"Elevation (deg)\"");
   const std::string json("{ " + variable + ", " + xAxis + ", " + yAxis + " }");
   return json;
}
