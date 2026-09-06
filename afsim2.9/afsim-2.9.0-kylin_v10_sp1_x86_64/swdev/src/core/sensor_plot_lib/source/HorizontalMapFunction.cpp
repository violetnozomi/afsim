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

#include "HorizontalMapFunction.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>

#include "ContourFilter2D.hpp"
#include "GeoShapeFile.hpp"
#include "UtColor.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtTiff.hpp"
#include "UtVec3.hpp"
#include "UtWallClock.hpp"
#include "Utility.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTerrain.hpp"

const double HorizontalMapFunction::cUNDEFINED = 1.0E+30;

// =================================================================================================
HorizontalMapFunction::HorizontalMapFunction(const WsfScenario& aScenario)
   : MapPlotFunction(aScenario)
   , mSimulationPtr(nullptr)
   , mSensorPlatformYaw(0.0)
   , mSensorPlatformPitch(0.0)
   , mSensorPlatformRoll(0.0)
   , mModeName()
   , mModeIndex(0)
   , mTarget(aScenario.GetAtmosphere())
   , mTargetAlt(0.0)
   , mTargetAltAGL(false)
   , mTargetAltMin(0.0)
   , mTargetAltMax(0.0)
   , mTargetAltStep(0.0)
   , mTargetAltRangeAGL(false)
   , mAutomaticTargetCueing(true)
   , mLineOfSightMasking(false)
   , mTargetPlatformId("TARGET")
   , mLatMin(cUNDEFINED)
   , mLatMax(cUNDEFINED)
   , mLatStep(0.01)
   , mLonMin(cUNDEFINED)
   , mLonMax(cUNDEFINED)
   , mLonStep(0.01)
   , mDownRangeMin(cUNDEFINED)
   , mDownRangeMax(cUNDEFINED)
   , mDownRangeStep(cUNDEFINED)
   , mDownRangeUnits("nm")
   , mCrossRangeMin(cUNDEFINED)
   , mCrossRangeMax(cUNDEFINED)
   , mCrossRangeStep(cUNDEFINED)
   , mCrossRangeUnits("nm")
   , mProjection()
   , mHeading(0.0)
   , mCenterLat(0.0)
   , mCenterLon(0.0)
   , mCenterEarthRadius(0.0)
   , mUseTM_Projection(false)
   , mGnuPlotFile()
   , mGnuPlotPlayerFile()
   , mPdMapFile()
   , mHeaderLine1("")
   , mHeaderLine2("")
   , mHeaderLine3("")
   , mOutputColumnLimit(100)
   , mKML_File()
   , mShapefile()
   , mSeditFile()
   , mWSF_File()
   , mContourLevels()
   , mDefendedAreaReportFile()
   , mAnalysisMapOptions()
   , mTiffFilename()
   , mExclusionList()
   , mInclusionList()
   , mSensors()
   , mExcludedSensors()
   , mFirstWrite(true)
{
   // Include all scenario platforms by default
   mPlatformAvailability.SetDefaultAvailability(1.0);

   // Set the default step size for X/Y plots to 1 grid unit (1 nm)
   mDownRangeStep  = UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
   mCrossRangeStep = UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);

   // Initialize sensor to ENU conversion.
   mProjection.SetCenter(0.0, 0.0);
   UtMat3d::Identity(mSensorToENU_Transform);
}

// =================================================================================================
// virtual
bool HorizontalMapFunction::Execute(WsfSimulation& aSimulation)
{
   MapPlotFunction::Execute(aSimulation);

   mPlatformAvailability.SetAvailabilityByName(mTargetPlatformId, 1.0);

   mSimulationPtr = &aSimulation;
   if (mPdMapFile.empty() && mGnuPlotFile.empty() && mShapefile.empty() && mKML_File.empty() && mSeditFile.empty() &&
       mWSF_File.empty() && mDefendedAreaReportFile.empty() && mAnalysisMapOptions.mFileName.empty() &&
       mTiffFilename.empty())
   {
      ut::log::error() << "At least one output format must be selected.";
      return false;
   }

   if ((GetCommand() == "multi_dimensional_horizontal_map") &&
       ((mWSF_File.empty()) ||
        (!mPdMapFile.empty() || !mGnuPlotFile.empty() || !mShapefile.empty() || !mKML_File.empty() ||
         !mSeditFile.empty() || !mDefendedAreaReportFile.empty() || !mAnalysisMapOptions.mFileName.empty())))
   {
      ut::log::error() << "multi_dimensional_horizontal_map may only output to wsf_file.";
      return false;
   }

   if ((mPlotVariables.Count() > 1) && (mGnuPlotFile.empty() && mTiffFilename.empty()))
   {
      ut::log::error() << "gnuplot_file or tiff_file must be specified if multiple variables specified.";
      return false;
   }

   // Make sure a target region has been specified (either X/Y or Lat/Lon) and that
   // it is not ambiguous (not a mixture of X/Y and Lat/Lon)

   if ((mDownRangeMax != cUNDEFINED) || (mCrossRangeMax != cUNDEFINED))
   {
      if (((mDownRangeMax != cUNDEFINED) && (mCrossRangeMax == cUNDEFINED)) ||
          ((mDownRangeMax == cUNDEFINED) && (mCrossRangeMax != cUNDEFINED)))
      {
         ut::log::error() << "Must specify BOTH a X and Y target region.";
         return false;
      }
      else if ((mLatMax != cUNDEFINED) || (mLonMax != cUNDEFINED))
      {
         ut::log::error() << "Cannot specify both X/Y and Lat/Lon target region.";
         return false;
      }
   }
   else if ((mLatMax != cUNDEFINED) || (mLonMax != cUNDEFINED))
   {
      if (((mLatMax != cUNDEFINED) && (mLonMax == cUNDEFINED)) || ((mLatMax == cUNDEFINED) && (mLonMax != cUNDEFINED)))
      {
         ut::log::error() << "Must specify BOTH a Lat and Lon target region.";
         return false;
      }
      else if ((mDownRangeMax != cUNDEFINED) || (mCrossRangeMax != cUNDEFINED))
      {
         ut::log::error() << "Cannot specify both X/Y and Lat/Lon target region.";
         return false;
      }
   }
   else
   {
      ut::log::error() << "Target region not specified.";
      return false;
   }

   // Create a clone of the target platform.

   if (!mTarget.CreateAndInitialize(aSimulation))
   {
      return false;
   }

   // Setup the platform to detect, normally the TARGET.

   if (aSimulation.GetPlatformByName(mTargetPlatformId) == nullptr)
   {
      auto out = ut::log::error() << "Target platform does not exist.";
      out.AddNote() << "Target: " << mTargetPlatformId;
      return false;
   }

   // Build the list of sensors. The include/exclude list is used to filter the list.

   CreateSensorList(aSimulation);

   if (mSensors.empty())
   {
      WsfPlatform* pltfrmPtr = WsfPlatformTypes::Get(aSimulation.GetScenario()).Clone("SENSOR_PLATFORM_TYPE");
      if (pltfrmPtr == nullptr)
      {
         ut::log::error() << "Unable to find sensor platform type SENSOR_PLATFORM_TYPE.";
         return false;
      }
      pltfrmPtr->SetName("SENSOR");
      aSimulation.AddPlatform(pltfrmPtr);
      CreateSensorList(aSimulation);

      if (mSensors.empty())
      {
         ut::log::error() << "No sensors defined.";
         return false;
      }
   }

   // Select the sensor mode if entered, else if single sensor then select default mode if one isn't specified.

   if (!mModeName.empty())
   {
      if (mSensors.size() > 1)
      {
         ut::log::error() << "mode_name can only be used with single sensor executions.";
         return false;
      }
      WsfSensor* sensorPtr = mSensors.front();
      mModeIndex           = sensorPtr->GetModeIndex(mModeName);
      if (mModeIndex >= sensorPtr->GetModeCount())
      {
         auto out = ut::log::error() << "mode_name does not exist on sensor.";
         out.AddNote() << "Platform: " << sensorPtr->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << sensorPtr->GetName();
         out.AddNote() << "Mode: " << mModeName;

         return false;
      }
   }
   else if (mSensors.size() == 1)
   {
      // Explicit mode not specified - use the initial/current mode of the sensor if it supports modes.
      WsfSensorMode* modePtr = mSensors.front()->GetCurrentMode();
      if (modePtr != nullptr)
      {
         mModeIndex = modePtr->GetModeIndex();
      }
   }

   // Initialize the map plot variables
   if (!mPlotVariables.Initialize(aSimulation))
   {
      return false;
   }

   size_t pdVarIndex = mPlotVariables.GetLabelPosition(mContourVariable);

   if (pdVarIndex >= mPlotVariables.Count())
   {
      if ((!mShapefile.empty()) || (!mKML_File.empty()) || (!mSeditFile.empty()) || (!mWSF_File.empty()) ||
          (!mDefendedAreaReportFile.empty()))
      {
         mPlotVariables.AddVariable(mContourVariable);
         pdVarIndex = mPlotVariables.GetLabelPosition(mContourVariable);
      }
   }

   if (GetCommand() == "multi_dimensional_horizontal_map")
   {
      return LoopOverTargetAltitude(pdVarIndex);
   }
   else
   {
      return BuildContours(pdVarIndex);
   }
}

// =================================================================================================
bool HorizontalMapFunction::LoopOverTargetAltitude(size_t aPdVarIndex)
{
   if (mTargetAltStep == 0.0)
   {
      return LoopOverTargetPitch(aPdVarIndex);
   }
   else
   {
      bool   ok = true;
      double altitude;
      for (altitude = mTargetAltMin; altitude <= mTargetAltMax; altitude += mTargetAltStep)
      {
         mTargetAlt    = altitude;
         mTargetAltAGL = mTargetAltRangeAGL;
         ok &= LoopOverTargetPitch(aPdVarIndex);
      }
      if (altitude + mTargetAltStep - mTargetAltMax > 0.5 * mTargetAltStep)
      {
         mTargetAlt    = mTargetAltMax;
         mTargetAltAGL = mTargetAltRangeAGL;
         ok &= LoopOverTargetPitch(aPdVarIndex);
      }
      return ok;
   }
}

// =================================================================================================
bool HorizontalMapFunction::LoopOverTargetPitch(size_t aPdVarIndex)
{
   double pmin  = mTarget.GetTargetPitchMin();
   double pmax  = mTarget.GetTargetPitchMax();
   double pstep = mTarget.GetTargetPitchStep();
   if (pstep == 0.0)
   {
      return LoopOverTargetRoll(aPdVarIndex);
   }
   else
   {
      bool   ok = true;
      double pitch;
      for (pitch = pmin; pitch <= pmax; pitch += pstep)
      {
         mTarget.SetPitch(pitch);
         ok &= LoopOverTargetRoll(aPdVarIndex);
      }
      if (pitch + pstep - pmax > 0.5 * pstep)
      {
         mTarget.SetPitch(pmax);
         ok &= LoopOverTargetRoll(aPdVarIndex);
      }
      return ok;
   }
}

// =================================================================================================
bool HorizontalMapFunction::LoopOverTargetRoll(size_t aPdVarIndex)
{
   double rmin  = mTarget.GetTargetRollMin();
   double rmax  = mTarget.GetTargetRollMax();
   double rstep = mTarget.GetTargetRollStep();
   if (rstep == 0.0)
   {
      return BuildContours(aPdVarIndex);
   }
   else
   {
      bool   ok = true;
      double roll;
      for (roll = rmin; roll <= rmax; roll += rstep)
      {
         mTarget.SetRoll(roll);
         ok &= BuildContours(aPdVarIndex);
      }
      if (roll + rstep - rmax > 0.5 * rstep)
      {
         mTarget.SetRoll(rmax);
         ok &= BuildContours(aPdVarIndex);
      }
      return ok;
   }
}

// =================================================================================================
bool HorizontalMapFunction::BuildContours(size_t aPdVarIndex)
{
   // Set the looping variables to step in either X/Y or Lat/Lon.

   double rowMin   = mDownRangeMin;
   double rowMax   = mDownRangeMax;
   double rowStep  = mDownRangeStep;
   double colMin   = mCrossRangeMin;
   double colMax   = mCrossRangeMax;
   double colStep  = mCrossRangeStep;
   double cellArea = mDownRangeStep * mCrossRangeStep;
   if (mLatMax != cUNDEFINED)
   {
      rowMin  = mLatMin;
      rowMax  = mLatMax;
      rowStep = mLatStep;
      if (rowMin > rowMax)
      {
         rowMin = mLatMax;
         rowMax = mLatMin;
      }
      colMin  = mLonMin;
      colMax  = mLonMax;
      colStep = mLonStep;
      if (colMin > colMax)
      {
         colMin = mLonMax;
         colMax = mLonMin;
      }

      double targetLat          = 0.5 * (rowMin + rowMax);
      double metersPerDegreeLat = (UtMath::cTWO_PI * UtSphericalEarth::cEARTH_RADIUS) / 360.0;
      double metersPerDegreeLon = metersPerDegreeLat * cos(targetLat * UtMath::cRAD_PER_DEG);
      double latCellSize        = metersPerDegreeLon * mLonStep;
      double lonCellSize        = metersPerDegreeLat * mLatStep;
      cellArea                  = latCellSize * lonCellSize;

      auto out = ut::log::info() << "Cell size:";
      out.AddNote() << "Lat Size: " << lonCellSize * 0.001 << " km";
      out.AddNote() << "Lon Size: " << latCellSize * 0.001 << " km";
      out.AddNote() << "Area: " << cellArea * 1E-6 << "km^2";
   }
   else
   {
      // Cartesian down range/cross range format. Establish the coordinate conversion.

      double notUsed;
      mSensors[0]->GetPlatform()->GetLocationLLA(mCenterLat, mCenterLon, notUsed);
      double locECEF[3];
      UtEllipsoidalEarth::ConvertLLAToECEF(mCenterLat, mCenterLon, 0.0, locECEF);
      mCenterEarthRadius = UtVec3d::Magnitude(locECEF);

      if (mUseTM_Projection)
      {
         mProjection.SetCenter(mCenterLat, mCenterLon);
      }
      mSensors[0]->GetPlatform()->GetOrientationNED(mHeading, notUsed, notUsed);

      // Convert heading from degrees clockwise from north to degrees counter clockwise from east.
      double downRangeAxisAngle = UtMath::NormalizeAngleMinusPi_Pi((UtMath::cTWO_PI - mHeading) + UtMath::cPI_OVER_2);
      double sinAngle           = sin(downRangeAxisAngle);
      double cosAngle           = cos(downRangeAxisAngle);
      UtMat3d::Identity(mSensorToENU_Transform);
      mSensorToENU_Transform[0][0] = cosAngle;
      mSensorToENU_Transform[0][1] = -sinAngle;
      mSensorToENU_Transform[1][0] = sinAngle;
      mSensorToENU_Transform[1][1] = cosAngle;

      double rowScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
      double colScale = 1.0 / UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);

      auto out = ut::log::info() << "Cell size:";
      out.AddNote() << "Lat Size: " << rowStep * rowScale << ' ' << mDownRangeUnits;
      out.AddNote() << "Lon Size: " << colStep * colScale << ' ' << mCrossRangeUnits;
      out.AddNote() << "Area: " << cellArea * rowScale * rowScale << ' ' << mDownRangeUnits << "^2";
   }

   // NOTE: The concept of a simulation time must be maintained in order to force the invalidation
   // of sensor subsystem location data on the target platform (if it has any).  The target may have
   // active systems that are attempting to be detected by the system under test...
   //
   // In addition, there may be 'execute at_time' scripts that need to execute sensor or jammer cueing.
   //
   // The sim time starts at 1 second. This is a bit of a hack right now because WsfAirMover doesn't really
   // update unless the time changes, and it doesn't set the pitch on initialization.

   double simTime = 1.0;
   InitializeSensorPlatforms(simTime);

   wsf::Terrain terrain(wsf::TerrainInterface::Get(GetScenario()));
   float        elev;

   std::vector<double> areaByPd(10, 0.0);
   std::vector<double> rowValues;
   std::vector<double> colValues;
   std::vector<Point>  dataValues;
   std::vector<double> varValues;
   int                 lastComplete = 0;
   UtWallClock         statusClock;
   for (double rowValue = rowMin; rowValue <= (rowMax + 0.1 * rowStep); rowValue += rowStep)
   {
      rowValues.push_back(rowValue);
      int complete =
         static_cast<int>(static_cast<double>(rowValue - rowMin) * 100.0 / static_cast<double>(rowMax - rowMin));
      // if % complete changes and elapsed time > 2 seconds, print message
      if (complete != lastComplete && statusClock.GetClock() > 2.0)
      {
         ut::log::info() << "Build Contours: " << complete << "% Complete.";
         lastComplete = complete;
         statusClock.ResetClock();
      }
      for (double colValue = colMin; colValue <= (colMax + 0.1 * colStep); colValue += colStep)
      {
         if (rowValue == rowMin)
         {
            colValues.push_back(colValue);
         }

         double targetLat = rowValue;
         double targetLon = colValue;
         if (mLatMax == cUNDEFINED)
         {
            ConvertCartesianToSpherical(rowValue, colValue, targetLat, targetLon);
         }

         // Set the target position at the current grid point.  If the altitude was specified as agl then
         // adjust the altitude to account for the terrain elevation.

         terrain.GetElevInterp(targetLat, targetLon, elev);
         double targetAlt = mTargetAlt;
         if (mTargetAltAGL)
         {
            targetAlt += elev;
         }
         mTarget.GetPlatform()->SetLocationLLA(targetLat, targetLon, targetAlt);

         // Force event dispatching in case anything is event driven.

         while (mSimulationPtr->AdvanceTime(simTime) < simTime)
         {
         }

         // Perform the required sensor detections.

         EvaluatePoint(simTime, rowValue, colValue, varValues);
         dataValues.push_back(varValues);

         // Increment the defended area statistics (only valid if 'Pd' has been selected).

         if (aPdVarIndex < mPlotVariables.Count())
         {
            int cellIndex = static_cast<int>(varValues[aPdVarIndex] * 10.0);
            cellIndex     = std::min(cellIndex, 9);
            areaByPd[cellIndex] += cellArea;
         }
         simTime += 0.1;
      }
   }

   ut::log::info() << "Writing output.";

   // If 'Pd' has been selected, write the defended area report and contours files if
   // they have been selected.

   if (aPdVarIndex < mPlotVariables.Count())
   {
      if (!mKML_File.empty())
      {
         WriteKML_File(rowValues, colValues, dataValues, aPdVarIndex);
      }
      if (!mShapefile.empty())
      {
         WriteShapefile(rowValues, colValues, dataValues, aPdVarIndex);
      }
      if (!mSeditFile.empty())
      {
         WriteSeditContours(rowValues, colValues, dataValues, aPdVarIndex);
      }
      if (!mWSF_File.empty())
      {
         WriteWSFContours(rowValues, colValues, dataValues, aPdVarIndex);
      }
      if (!mDefendedAreaReportFile.empty())
      {
         WriteDefendedAreaReport(areaByPd);
      }
   }

   // Write plots for selected variables.

   if (!mPdMapFile.empty())
   {
      WritePdMapFile(rowValues, colValues, dataValues, 0);
   }
   if (!mGnuPlotFile.empty())
   {
      WriteGnuPlotFile(rowValues, colValues, dataValues);
   }
   if (!mGnuPlotPlayerFile.empty())
   {
      WriteGnuPlotPlayerFile();
   }
   if (!mAnalysisMapOptions.mFileName.empty())
   {
      WriteAnalysisMapFile(rowValues, colValues, dataValues, 0);
   }
   if (!mTiffFilename.empty())
   {
      WriteTiffFile(rowValues, colValues, dataValues);
   }

   WriteCustomOutputFormats(rowValues, colValues, dataValues, 0);

   return true;
}

// =================================================================================================
//! Process the horizontal_map input block.
// virtual
bool HorizontalMapFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "contour_level")
   {
      ContourLevel contourLevel;
      aInput.ReadValue(contourLevel.mLevel);
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "line_color")
         {
            aInput.ReadValue(contourLevel.mLineColor);
         }
         else if (command == "line_width")
         {
            aInput.ReadValue(contourLevel.mLineWidth);
            aInput.ValueGreater(contourLevel.mLineWidth, 0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mContourLevels.push_back(contourLevel);
   }
   else if (command == "contour_variable")
   {
      aInput.ReadValue(mContourVariable);
      try
      {
         mPlotVariables.AddVariable(mContourVariable);
      }
      catch (std::exception&)
      {
         throw UtInput::BadValue(aInput, "Unknown contour_variable type: " + mContourVariable);
      }
   }
   else if (command == "analysis_map")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "file")
         {
            aInput.ReadValueQuoted(mAnalysisMapOptions.mFileName);
            mAnalysisMapOptions.mFileName = aInput.SubstitutePathVariables(mAnalysisMapOptions.mFileName);
         }
         else if (command == "color_range")
         {
            AnalysisMapOptions::ColorRange colorRange;
            aInput.ReadValue(colorRange.mMinValue);
            std::string to;
            aInput.ReadValue(to);
            aInput.StringEqual(to, "to");
            aInput.ReadValue(colorRange.mMaxValue);
            aInput.ValueLessOrEqual(colorRange.mMinValue, colorRange.mMaxValue);
            aInput.ReadValue(colorRange.mColor);
            mAnalysisMapOptions.mColorRanges.push_back(colorRange);
         }
         else if (command == "data_title")
         {
            aInput.ReadValue(mAnalysisMapOptions.mDataTitleStr);
         }
         else if (command == "units")
         {
            aInput.ReadValue(mAnalysisMapOptions.mUnitsStr);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "tiff_file")
   {
      aInput.ReadValueQuoted(mTiffFilename);
      mTiffFilename = aInput.SubstitutePathVariables(mTiffFilename);
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
   else if (command == "pd_map_file")
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
   else if (command == "kml_file")
   {
      aInput.ReadValueQuoted(mKML_File);
      mKML_File = aInput.SubstitutePathVariables(mKML_File);
   }
   else if (command == "shapefile")
   {
      aInput.ReadValueQuoted(mShapefile);
      mShapefile = aInput.SubstitutePathVariables(mShapefile);
   }
   else if (command == "sedit_file")
   {
      aInput.ReadValueQuoted(mSeditFile);
      mSeditFile = aInput.SubstitutePathVariables(mSeditFile);
   }
   else if (command == "wsf_file")
   {
      aInput.ReadValueQuoted(mWSF_File);
      mWSF_File = aInput.SubstitutePathVariables(mWSF_File);
   }
   else if (command == "defended_area_report_file")
   {
      aInput.ReadValueQuoted(mDefendedAreaReportFile);
      mDefendedAreaReportFile = aInput.SubstitutePathVariables(mDefendedAreaReportFile);
   }
   else if (command == "mode_name")
   {
      aInput.ReadValue(mModeName);
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
   else if ((command == "target_platform_name") || (command == "platform_to_detect_name"))
   {
      std::string pltfrmName;
      aInput.ReadValue(pltfrmName);
      mTargetPlatformId = pltfrmName;
   }
   else if (command == "target_region")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (!ProcessTargetRegion(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (ProcessTargetRegion(aInput))
   {
   }
   else if (mTarget.ProcessInput(aInput))
   {
   }
   else if (command == "target_altitude")
   {
      aInput.ReadValueOfType(mTargetAlt, UtInput::cLENGTH);
      std::string altType;
      aInput.ReadValue(altType);
      if (altType == "msl")
      {
         mTargetAltAGL = false;
      }
      else if (altType == "agl")
      {
         mTargetAltAGL = true;
      }
      else
      {
         aInput.PushBack(altType);
      }
   }
   else if (command == "automatic_target_cueing")
   {
      aInput.ReadValue(mAutomaticTargetCueing);
   }
   else if (command == "line_of_sight_masking")
   {
      aInput.ReadValue(mLineOfSightMasking);
   }
   else if (command == "exclude")
   {
      ReadSelector(aInput, mExclusionList);
   }
   else if (command == "no_exclude")
   {
      // 'include' would have been a better name, but it is reserved...
      ReadSelector(aInput, mInclusionList);
   }
   else if (GetCommand() == "multi_dimensional_horizontal_map")
   {
      if (command == "target_roll_range")
      {
         double targetRollMin;
         double targetRollMax;
         double targetRollStep;
         aInput.ReadValueOfType(targetRollMin, UtInput::cANGLE);
         aInput.ValueInClosedRange(targetRollMin, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         aInput.ReadValueOfType(targetRollMax, UtInput::cANGLE);
         aInput.ValueInClosedRange(targetRollMax, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         aInput.ReadValueOfType(targetRollStep, UtInput::cANGLE);
         aInput.ValueInClosedRange(targetRollStep, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         mTarget.SetRollRange(targetRollMin, targetRollMax, targetRollStep);
      }
      else if (command == "target_altitude_range")
      {
         aInput.ReadValueOfType(mTargetAltMin, UtInput::cLENGTH);
         aInput.ReadValueOfType(mTargetAltMax, UtInput::cLENGTH);
         aInput.ReadValueOfType(mTargetAltStep, UtInput::cLENGTH);
         std::string altType;
         aInput.ReadValue(altType);
         if (altType == "msl")
         {
            mTargetAltRangeAGL = false;
         }
         else if (altType == "agl")
         {
            mTargetAltRangeAGL = true;
         }
         else
         {
            aInput.PushBack(altType);
         }
      }
      else if (command == "target_pitch_range")
      {
         double targetPitchMin;
         double targetPitchMax;
         double targetPitchStep;
         aInput.ReadValueOfType(targetPitchMin, UtInput::cANGLE);
         aInput.ValueInClosedRange(targetPitchMin, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         aInput.ReadValueOfType(targetPitchMax, UtInput::cANGLE);
         aInput.ValueInClosedRange(targetPitchMax, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         aInput.ReadValueOfType(targetPitchStep, UtInput::cANGLE);
         aInput.ValueInClosedRange(targetPitchStep, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
         mTarget.SetPitchRange(targetPitchMin, targetPitchMax, targetPitchStep);
      }
      // we should probably incorporate more dimensions here
   }
   else if (command == "use_tm_projection")
   {
      aInput.ReadValue(mUseTM_Projection);
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
//! Given a down range and cross range value from the sensor, compute the equivalent latitude and longitude.
void HorizontalMapFunction::ConvertCartesianToSpherical(double aDownRange, double aCrossRange, double& aLatitude, double& aLongitude)
{
   double sensorXYZ[3] = {aDownRange, aCrossRange, 0.0};
   double enu[3];
   UtMat3d::Transform(enu, mSensorToENU_Transform, sensorXYZ);

   if (mUseTM_Projection)
   {
      mProjection.ConvertXYToLatLon(enu[0], enu[1], aLatitude, aLongitude);
   }
   else
   {
      aLatitude  = enu[1] / mCenterEarthRadius * UtMath::cDEG_PER_RAD + mCenterLat;
      aLongitude = enu[0] / mCenterEarthRadius * UtMath::cDEG_PER_RAD + mCenterLon;
   }
}

// =================================================================================================
//! Given a latitude and longitude, compute the equivalent down range and cross range values.
void HorizontalMapFunction::ConvertSphericalToCartesian(double aLatitude, double aLongitude, double& aDownRange, double& aCrossRange)
{
   double enu[3] = {0.0, 0.0, 0.0};
   if (mUseTM_Projection)
   {
      mProjection.ConvertLatLonToXY(aLatitude, aLongitude, enu[0], enu[1]);
   }
   else
   {
      enu[0] = (aLatitude - mCenterLat) * mCenterEarthRadius * UtMath::cRAD_PER_DEG;
      enu[1] = (aLongitude - mCenterLon) * mCenterEarthRadius * UtMath::cRAD_PER_DEG;
   }
   double sensorXYZ[3];
   UtMat3d::InverseTransform(sensorXYZ, mSensorToENU_Transform, enu);
   aDownRange  = sensorXYZ[0];
   aCrossRange = sensorXYZ[1];
}

// =================================================================================================
//! Determine the function value for a point.
//!
//! This routine will perform a detection attempt from all sensors against the target.
//! The return value is the selected function for the sensor that had the best probability
//! of detection.
// private
void HorizontalMapFunction::EvaluatePoint(double aSimTime, double aRowValue, double aColValue, std::vector<double>& aVarValues)
{
   // The following only works for downrange/crossrange input... not lat/lon
   mPlotVariables.SetCrossRange(aColValue);
   mPlotVariables.SetDownRange(aRowValue);

   // Use down range units for any range scaling.
   double xScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
   mPlotVariables.SetRangeScale(xScale);

   mPlotVariables.InitializeValues(*mSimulationPtr, aVarValues, std::vector<double>());

   WsfSensorResult     result;
   WsfSensor::Settings settings;
   settings.mModeIndex  = mModeIndex;
   settings.mRequiredPd = 1.0E-6;
   for (WsfSensor* sensorPtr : mSensors)
   {
      // If there are multiple sensors then use the initial/current mode of the sensor if it supports modes
      if (mSensors.size() > 1)
      {
         WsfSensorMode* modePtr = sensorPtr->GetCurrentMode();
         if (modePtr != nullptr)
         {
            settings.mModeIndex = modePtr->GetModeIndex();
         }
      }

      // Set the speed and attitude of the target.
      mTarget.SetSpeedAndAttitude(sensorPtr);

      // Get the pointer to the target we are trying to detect.
      // Note that this may be different than the target we are moving on the grid.
      WsfPlatform* targetPtr = mSimulationPtr->GetPlatformByName(mTargetPlatformId);

      // If the current sensor  is an ESM sensor then it may be using the listener interface so
      // the other sensors that are part of the input, but not being included in the plotting process,
      // must be processed also. TODO - need to make sure that we don't have to process even the ones
      // that are included beforehand and watch for recursion (i.e. same sensor as tgtSensor below)
      if (sensorPtr->IsA_TypeOf("WSF_ESM_SENSOR") || sensorPtr->IsA_TypeOf("WSF_PASSIVE_SENSOR"))
      {
         for (WsfSensor* tgtSensorPtr : mExcludedSensors)
         {
            // Cue the sensor to the target just in case this is a tracker.
            if (mAutomaticTargetCueing)
            {
               Utility::CueSensorToTarget(tgtSensorPtr, sensorPtr->GetPlatform());
            }
            tgtSensorPtr->AttemptToDetect(aSimTime, sensorPtr->GetPlatform(), settings, result);
         }
      }

      // Cue the sensor to the target just in case this is a tracker.
      if (mAutomaticTargetCueing)
      {
         Utility::CueSensorToTarget(sensorPtr, targetPtr);
      }

      sensorPtr->AttemptToDetect(aSimTime, targetPtr, settings, result);

      // If requested, provide a terrain check if one has not already been performed.
      // Do not evaluate the variables if terrain is masked.
      if (mLineOfSightMasking)
      {
         bool doEvaluate = false;
         if (!((result.mCheckedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING) &&
               (result.mCheckedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING)))
         {
            doEvaluate = (!result.MaskedByTerrain());
         }
         else
         {
            doEvaluate = (!((result.mFailedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING) ||
                            (result.mFailedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING)));
         }
         if (doEvaluate)
         {
            mPlotVariables.Evaluate(aSimTime, sensorPtr, result, aVarValues);
         }
      }
      else
      {
         // Standard evaluation without los masking checks.
         mPlotVariables.Evaluate(aSimTime, sensorPtr, result, aVarValues);
      }
   }
}

// =================================================================================================
void HorizontalMapFunction::InitializeSensorPlatforms(double aSimTime)
{
   // One mover update must be performed to cause the mover to set its initial kinematic state.
   // After this has been done, the mover is deleted to prevent future position updates that would occur
   // because of the advancing of time. Except for the target platform (which is moved through the grid),
   // none of the platforms (especially the sensing platform) should move while advancing through time,
   // even though they have a defined velocity (which is required for things like the SAR sensor, but may
   // require velocity for Doppler computations).

   size_t platformCount = mSimulationPtr->GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);
      double       lat, lon, alt;
      platformPtr->GetLocationLLA(lat, lon, alt); // Capture initial location (may change during Update)
      platformPtr->Update(aSimTime);
      double yaw, pitch, roll;
      platformPtr->GetOrientationNED(yaw, pitch, roll); // Hack for WsfAirMover not setting pitch
      platformPtr->SetLocationLLA(lat, lon, alt);       // Set location back to the initial location
      platformPtr->GetOrientationNED(yaw, pitch, roll); // Hack for WsfAirMover not setting pitch
      platformPtr->SetMover(nullptr);                   // Prevent further position updates
   }

   // Orient all of the platforms which have a sensor being tested.
   // The specified pitch and roll angles are used 'as-is'. The yaw angle is added to the existing heading.

   std::set<WsfPlatform*> sensorPlatforms;
   for (std::vector<WsfSensor*>::const_iterator si = mSensors.begin(); si != mSensors.end(); ++si)
   {
      WsfSensor*   sensorPtr   = *si;
      WsfPlatform* platformPtr = sensorPtr->GetPlatform();
      if (sensorPlatforms.find(platformPtr) == sensorPlatforms.end())
      {
         sensorPlatforms.insert(platformPtr);
      }
   }

   for (WsfPlatform* platformPtr : sensorPlatforms)
   {
      double heading;
      double pitch;
      double roll;
      platformPtr->GetOrientationNED(heading, pitch, roll);
      heading = UtMath::NormalizeAngleMinusPi_Pi(heading + mSensorPlatformYaw);
      platformPtr->SetOrientationNED(heading, mSensorPlatformPitch, mSensorPlatformRoll);
   }
}

void HorizontalMapFunction::CreateSensorList(WsfSimulation& aSimulation)
{
   mSensors.clear();
   mExcludedSensors.clear();
   size_t platformCount = aSimulation.GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = aSimulation.GetPlatformEntry(platformEntry);
      for (WsfComponentList::RoleIterator<WsfSensor> iter(*platformPtr); !iter.AtEnd(); ++iter)
      {
         WsfSensor* sensorPtr = *iter;
         if (sensorPtr)
         {
            // A sensor is implicitly 'included'. If it isn't explicitly included then it will be included only
            // if it isn't in the exclusion list (i.e. the inclusion list takes priority).
            bool excluded = false;
            if (!InSelectorList(sensorPtr, mInclusionList))
            {
               excluded = InSelectorList(sensorPtr, mExclusionList);
            }
            if (!excluded)
            {
               mSensors.push_back(sensorPtr);
               if (!sensorPtr->IsTurnedOn()) // Not sure this is necessary, but just in case...
               {
                  aSimulation.TurnPartOn(0.0, sensorPtr);
               }
            }
            else
            {
               mExcludedSensors.push_back(sensorPtr);
               // Do not turn on excluded sensors as the user may be purposefully controlling their on/off state
               auto out = ut::log::info() << "Excluded sensor from output results.";
               out.AddNote() << "Platform: " << platformPtr->GetName();
               out.AddNote() << "Sensor: " << sensorPtr->GetName();
            }
         }
      }
   }
}

// =================================================================================================
bool HorizontalMapFunction::InSelectorList(WsfSensor* aSensorPtr, const SelectorList& aSelectorList)
{
   for (const Selector& selector : aSelectorList)
   {
      bool selected = false;
      switch (selector.mType)
      {
      case Selector::cSENSOR_TYPE:
         selected = (aSensorPtr->GetTypeId() == selector.mValue);
         break;
      case Selector::cSENSOR_CATEGORY:
         selected = (aSensorPtr->IsCategoryMember(selector.mValue));
         break;
      case Selector::cPLATFORM_TYPE:
         selected = (aSensorPtr->GetPlatform()->GetTypeId() == selector.mValue);
         break;
      case Selector::cPLATFORM_CATEGORY:
         selected = (aSensorPtr->GetPlatform()->IsCategoryMember(selector.mValue));
         break;
      default:
         break;
      }
      if (selected)
      {
         return true;
      }
   }
   return false;
}

// =================================================================================================
//! Process target region definition commands.
//! Formerly these could go only in a 'target_region' block, but now they can also appear outside
//! the block. This makes it consistent with the vertical_map command where the region commands
//! weren't bounded by a block. Requiring them to be in a block was kind of useless, so now we'll
//! accept it either way.
bool HorizontalMapFunction::ProcessTargetRegion(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "latitude_limits") || (command == "latitude_range"))
   {
      aInput.ReadValueOfType(mLatMin, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mLatMax, UtInput::cLATITUDE);
   }
   else if ((command == "longitude_limits") || (command == "longitude_range"))
   {
      aInput.ReadValueOfType(mLonMin, UtInput::cLONGITUDE);
      aInput.ReadValueOfType(mLonMax, UtInput::cLONGITUDE);
   }
   else if (command == "latitude_step")
   {
      aInput.ReadValue(mLatStep);
      aInput.ValueGreater(mLatStep, 0.0);
   }
   else if (command == "longitude_step")
   {
      aInput.ReadValue(mLonStep);
      aInput.ValueGreater(mLonStep, 0.0);
   }
   else if (command == "down_range_limits")
   {
      aInput.ReadValueOfType(mDownRangeMin, UtInput::cLENGTH);
      aInput.ReadValueOfType(mDownRangeMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mDownRangeMax, mDownRangeMin);
   }
   else if (command == "cross_range_limits")
   {
      aInput.ReadValueOfType(mCrossRangeMin, UtInput::cLENGTH);
      aInput.ReadValueOfType(mCrossRangeMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mCrossRangeMax, mCrossRangeMin);
   }
   else if (command == "down_range_step")
   {
      aInput.ReadValue(mDownRangeStep);
      aInput.ReadValue(mDownRangeUnits);
      mDownRangeStep = aInput.ConvertValue(mDownRangeStep, mDownRangeUnits, UtInput::cLENGTH);
      aInput.ValueGreater(mDownRangeStep, 0.0);
   }
   else if (command == "cross_range_step")
   {
      aInput.ReadValue(mCrossRangeStep);
      aInput.ReadValue(mCrossRangeUnits);
      mCrossRangeStep = aInput.ConvertValue(mCrossRangeStep, mCrossRangeUnits, UtInput::cLENGTH);
      aInput.ValueGreater(mCrossRangeStep, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void HorizontalMapFunction::ReadSelector(UtInput& aInput, SelectorList& aSelectorList)
{
   Selector    selector;
   std::string selectorType;
   std::string selectorValue;
   aInput.ReadValue(selectorType);

   if (selectorType == "sensor_type")
   {
      selector.mType = Selector::cSENSOR_TYPE;
   }
   else if (selectorType == "sensor_category")
   {
      selector.mType = Selector::cSENSOR_CATEGORY;
   }
   else if (selectorType == "platform_type")
   {
      selector.mType = Selector::cPLATFORM_TYPE;
   }
   else if (selectorType == "platform_category")
   {
      selector.mType = Selector::cPLATFORM_CATEGORY;
   }
   else
   {
      throw UtInput::BadValue(aInput);
   }
   aInput.ReadValue(selectorValue);
   selector.mValue = selectorValue;
   aSelectorList.push_back(selector);
}

// =================================================================================================
//! Write the defended area report if requested.
// private
void HorizontalMapFunction::WriteDefendedAreaReport(const std::vector<double>& aAreaByPd)
{
   std::ofstream ofs(mDefendedAreaReportFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open report file.";
      out.AddNote() << "File: " << mDefendedAreaReportFile;
      return;
   }

   ofs << "Defended Area Report\n";
   ofs << "--------------------\n";
   ofs << "\n";
   if (mLatMax == cUNDEFINED)
   {
      double rowScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
      double colScale = 1.0 / UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);
      ofs << " Down  Range Limits: " << mDownRangeMin * rowScale << ' ' << mDownRangeUnits << ' '
          << mDownRangeMax * rowScale << ' ' << mDownRangeUnits << "; Step " << mDownRangeStep * rowScale << ' '
          << mDownRangeUnits << std::endl;
      ofs << " Cross Range Limits: " << mCrossRangeMin * colScale << ' ' << mCrossRangeUnits << ' '
          << mCrossRangeMax * colScale << ' ' << mCrossRangeUnits << "; Step " << mCrossRangeStep * rowScale << ' '
          << mCrossRangeUnits << std::endl;
   }
   else
   {
      ofs << " Longitude Range: " << UtLonPos(mLonMin) << ' ' << UtLonPos(mLonMax) << '\n';
      ofs << " Latitude  Range: " << UtLatPos(mLatMin) << ' ' << UtLonPos(mLatMax) << '\n';
      double metersPerDegreeLat = (UtMath::cTWO_PI * UtSphericalEarth::cEARTH_RADIUS) / 360.0;
      double midLat             = 0.5 * (mLatMin + mLatMax);
      double metersPerDegreeLon = metersPerDegreeLat * cos(midLat * UtMath::cRAD_PER_DEG);
      double latCellSize        = metersPerDegreeLat * mLatStep;
      double lonCellSize        = metersPerDegreeLon * mLonStep;
      ofs << " Longitude Step : " << mLonStep << " deg (approx." << ' ' << lonCellSize * 1.0E-3 << " km"
          << " or " << lonCellSize / UtMath::cM_PER_NM << " nm)" << std::endl;
      ofs << " Latitude  Step : " << mLatStep << " deg (approx." << ' ' << latCellSize * 1.0E-3 << " km"
          << " or " << latCellSize / UtMath::cM_PER_NM << " nm)" << std::endl;
   }
   ofs << std::endl;
   ofs << " Target Altitude: " << mTargetAlt << " m ";
   if (mTargetAltAGL)
   {
      ofs << "agl" << std::endl;
   }
   else
   {
      ofs << "msl" << std::endl;
   }
   ofs << "    Target Speed: " << mTarget.GetSpeed() << " m/s" << std::endl;
   ofs << std::endl;
   ofs << "  ----Pd----                Cumulative\n";
   ofs << "  From   To   Area (km^2)  Area (km^2)\n";
   ofs << "  ----  ----  -----------  -----------\n";
   double areaSum = 0.0;
   for (int i = 9; i >= 0; --i)
   {
      double pd = i * 0.1;
      areaSum += aAreaByPd[i];

      ofs.setf(std::ios::showpoint);
      ofs.setf(std::ios::fixed);
      ofs << std::setfill(' ') << std::setprecision(1);
      ofs << "  " << std::setw(4) << pd;
      ofs << "  " << std::setw(4) << pd + 0.1;
      ofs.unsetf(std::ios::showpoint);
      ofs << std::setprecision(10);
      ofs << "  " << std::setw(11) << static_cast<long int>(aAreaByPd[i] * 1.0E-6);
      ofs << "  " << std::setw(11) << static_cast<long int>(areaSum * 1.0E-6) << '\n';
      ofs << std::setprecision(6);
      ofs << resetiosflags(std::ios::floatfield);
   }
   ofs << "  ----  ----  -----------  -----------\n";
   ofs << "              Total Area:  " << std::setprecision(10) << std::setw(11)
       << static_cast<long int>(areaSum * 1.0E-6) << " km^2\n"
       << std::setprecision(6);
   ofs.unsetf(std::ios::floatfield);

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Report", mDefendedAreaReportFile);
}

// =================================================================================================
//! Write output gnuplot 'splot' output.
void HorizontalMapFunction::WriteGnuPlotFile(const std::vector<double>& aRowValues,
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

   if (!mHeaderLine1.empty())
   {
      ofs << "# " << mHeaderLine1 << '\n';
   }
   if (!mHeaderLine2.empty())
   {
      ofs << "# " << mHeaderLine2 << '\n';
   }
   if (!mHeaderLine3.empty())
   {
      ofs << "# " << mHeaderLine3 << '\n';
   }

   ofs << "# gnuplot 'splot' data file\n";
   ofs << "#\n";
   if (mLatMax == cUNDEFINED)
   {
      ofs << "# Column 1: Down Range (" << mDownRangeUnits << ")\n";
      ofs << "# Column 2: Cross Range (" << mCrossRangeUnits << ")\n";
   }
   else
   {
      ofs << "# Column 1: Latitude (deg)\n";
      ofs << "# Column 2: Longitude (deg)\n";
   }
   int column = 3;
   for (size_t i = 0; i < mPlotVariables.Count(); ++i)
   {
      ofs << "# Column " << column << ": " << mPlotVariables.GetLabel(i) << '\n';
      ++column;
   }

   double xScale = 1.0;
   double yScale = 1.0;
   if (mLatMax == cUNDEFINED)
   {
      xScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
      yScale = 1.0 / UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);
   }

   size_t rowCount    = aRowValues.size();
   size_t colCount    = aColValues.size();
   size_t varCount    = aVarValues[0].size();
   size_t rowColIndex = 0;
   for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      for (size_t colIndex = 0; colIndex < colCount; ++colIndex)
      {
         ofs << std::setw(10) << aRowValues[rowIndex] * xScale << ' ' << std::setw(10) << aColValues[colIndex] * yScale;
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
//! Write output gnuplot player location output.
void HorizontalMapFunction::WriteGnuPlotPlayerFile()
{
   std::ofstream ofs(mGnuPlotPlayerFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mGnuPlotPlayerFile;
      return;
   }

   double xScale = 1.0;
   double yScale = 1.0;
   if (mLatMax == cUNDEFINED)
   {
      xScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
      yScale = 1.0 / UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);
   }

   ofs << "#\n";
   ofs << "# Column 1: Down Range (" << mDownRangeUnits << ")\n";
   ofs << "# Column 2: Cross Range (" << mCrossRangeUnits << ")\n";
   ofs << "# Column 3: Dummy\n";
   ofs << "#\n";

   size_t pltfrmCnt     = 0;
   size_t platformCount = mSimulationPtr->GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);

      // Skip the target platform.
      if (platformPtr == mTarget.GetPlatform())
      {
         continue;
      }

      ofs << "# Player " << ++pltfrmCnt << ": " << platformPtr->GetName();
      ofs << '\n';

      double lat;
      double lon;
      double alt;
      platformPtr->GetLocationLLA(lat, lon, alt);

      double x = lat;
      double y = lon;

      if (mLatMax == cUNDEFINED)
      {
         ConvertSphericalToCartesian(lat, lon, x, y);
      }

      ofs << std::setw(10) << x * xScale << ' ' << std::setw(10) << y * yScale << ' ' << std::setw(10) << '1';
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("GNU Plot Player", mGnuPlotPlayerFile);
}

// =================================================================================================
//! Write the KML output if requested.
// private
void HorizontalMapFunction::WriteKML_File(const std::vector<double>& aRowValues,
                                          const std::vector<double>& aColValues,
                                          const std::vector<Point>&  aVarValues,
                                          size_t                     aVarIndex)
{
   std::ofstream ofs(mKML_File);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output kml_file.";
      out.AddNote() << "File: " << mKML_File;
      return;
   }
   ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
   ofs << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
   ofs << "<Document>\n";
   ofs << " <name>" << mKML_File << "</name>\n";
   ofs << " <open>0</open>\n";
   if ((!mHeaderLine1.empty()) || (!mHeaderLine2.empty()) || (!mHeaderLine3.empty()))
   {
      ofs << " <description>\n";
      if (!mHeaderLine1.empty())
      {
         ofs << mHeaderLine1 << '\n';
      }
      if (!mHeaderLine2.empty())
      {
         ofs << mHeaderLine2 << '\n';
      }
      if (!mHeaderLine3.empty())
      {
         ofs << mHeaderLine3 << '\n';
      }
      ofs << " </description>\n";
   }

   std::vector<ContourLevel> levels(mContourLevels);
   if (levels.empty())
   {
      levels.emplace_back();
   }

   // Write out all the styles at the top so they are easy to find and edit.

   for (size_t levelIndex = 0; levelIndex < levels.size(); ++levelIndex)
   {
      // clang-format off
      ofs << " <Style id=\"contour_" << levelIndex + 1 << "\">\n"
          << "  <LineStyle>\n"
          << "   <color>ff" << std::hex << std::setfill('0')
          << std::setw(2) << static_cast<int>(levels[levelIndex].mLineColor[2] * 255.0)
          << std::setw(2) << static_cast<int>(levels[levelIndex].mLineColor[1] * 255.0)
          << std::setw(2) << static_cast<int>(levels[levelIndex].mLineColor[0] * 255.0)
          << "</color>\n" << std::dec << std::setfill(' ')
          << "   <width>" << levels[levelIndex].mLineWidth << "</width>\n"
          << "  </LineStyle>\n"
          << " </Style>\n";
      // clang-format on
   }

   // clang-format off
   ofs << " <Style id=\"platform\">\n"
       << "  <IconStyle>\n"
       << "   <scale>0.5</scale>\n"
       << "  </IconStyle>\n"
       << "  <LabelStyle>\n"
       << "   <scale>0.5</scale>\n"
       << "  </LabelStyle>\n"
       << "  <LineStyle>\n"
       << "   <color>ff0000ff</color>\n"
       << "   <width>2</width>\n"
       << "  </LineStyle>\n"
       << " </Style>\n";

   ofs << " <Style id=\"boundary\">\n"
       << "  <LineStyle>\n"
       << "   <color>ff00007f</color>\n"
       << "   <width>2</width>\n"
       << "  </LineStyle>\n"
       << " </Style>\n";
   // clang-format on

   ContourFilter2D           contourFilter;
   ContourFilter2D::LineList lineList;
   ContourFunction           contourFunction(aRowValues, aColValues, aVarValues, aVarIndex);

   ofs.precision(13);
   for (size_t levelIndex = 0; levelIndex < levels.size(); ++levelIndex)
   {
      double value = levels[levelIndex].mLevel;
      ofs << "\n";
      ofs << " <Placemark>\n";
      ofs << "  <name>Pd=" << value << "</name>\n";
      ofs << "  <styleUrl>#contour_" << levelIndex + 1 << "</styleUrl>\n";
      ofs << "  <MultiGeometry>\n";
      lineList.clear();
      contourFilter.Contour(&contourFunction, 0, aRowValues.size() - 1, 0, aColValues.size() - 1, value, lineList);

      ContourFilter2D::LineListIter lineListIter;
      for (lineListIter = lineList.begin(); lineListIter != lineList.end(); ++lineListIter)
      {
         ofs << "   <LineString>\n";
         ofs << "    <tessellate>1</tessellate>\n";
         ofs << "    <altitudeMode>clampToGround</altitudeMode>\n";
         ofs << "    <coordinates>\n";
         const ContourFilter2D::Line& line = *lineListIter;
         for (const auto& pt : line)
         {
            double lat = pt.mX;
            double lon = pt.mY;
            if (mLatMax == cUNDEFINED)
            {
               ConvertCartesianToSpherical(pt.mX, pt.mY, lat, lon);
            }
            ofs << "     " << lon << ',' << lat << ",0.0\n";
         }
         ofs << "    </coordinates>\n";
         ofs << "   </LineString>\n";
      }
      ofs << "  </MultiGeometry>\n";
      ofs << " </Placemark>\n";
   }

   // Write out the locations of the platforms.

   ofs << "\n";
   ofs << " <Folder>\n";
   ofs << "  <name>Sensor Sites</name>\n";

   size_t platformCount = mSimulationPtr->GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);
      if (platformPtr == mTarget.GetPlatform())
      {
         continue;
      }
      double lat;
      double lon;
      double alt;
      platformPtr->GetLocationLLA(lat, lon, alt);
      // clang-format off
      ofs << "  <Placemark>\n"
          << "   <name>" << platformPtr->GetName() << "</name>\n"
          << "   <styleUrl>#platform</styleUrl>\n"
          << "   <Point>\n"
          << "    <altitudeMode>clampToGround</altitudeMode>\n"
          << "    <coordinates>" << lon << ',' << lat << ',' << alt << "</coordinates>\n"
          << "   </Point>\n"
          << "  </Placemark>\n";
      // clang-format on
   }
   ofs << " </Folder>\n";

   // Write out the grid boundary.

   double latMin = mLatMin;
   double latMax = mLatMax;
   double lonMin = mLonMin;
   double lonMax = mLonMax;
   if (mLatMax == cUNDEFINED)
   {
      ConvertCartesianToSpherical(mDownRangeMin, mCrossRangeMin, latMin, lonMin);
      ConvertCartesianToSpherical(mDownRangeMax, mCrossRangeMax, latMax, lonMax);
   }
   // clang-format off
   ofs << "\n";
   ofs << " <Placemark>\n"
       << "  <name>Boundary</name>\n"
       << "  <styleUrl>#boundary</styleUrl>\n"
       << "  <visibility>0</visibility>\n"
       << "  <LineString>\n"
       << "   <altitudeMode>clampToGround</altitudeMode>\n"
       << "   <extrude>0</extrude>\n"
       << "   <tessellate>1</tessellate>\n"
       << "   <coordinates>\n"
       << "    " << lonMin << ',' << latMin << ",0.0\n"
       << "    " << lonMax << ',' << latMin << ",0.0\n"
       << "    " << lonMax << ',' << latMax << ",0.0\n"
       << "    " << lonMin << ',' << latMax << ",0.0\n"
       << "    " << lonMin << ',' << latMin << ",0.0\n"
       << "   </coordinates>\n"
       << "  </LineString>\n"
       << " </Placemark>\n";

   // End of the KML file.
   ofs << "</Document>\n"
       << "</kml>\n";
   // clang-format on
   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("KML", mKML_File);
}

// =================================================================================================
//! Write the PdMap output if requested.
void HorizontalMapFunction::WritePdMapFile(const std::vector<double>& aRowValues,
                                           const std::vector<double>& aColValues,
                                           const std::vector<Point>&  aVarValues,
                                           size_t                     aVarIndex)
{
   std::ofstream ofs(mPdMapFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output pd_map_file.";
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

   double xScale = 1.0;
   double yScale = 1.0;
   if (mLatMax == cUNDEFINED)
   {
      xScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
      yScale = 1.0 / UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);
   }

   ofs << "          ";
   itemsOnLine = 0;
   for (colIndex = 0; colIndex < colCount; ++colIndex)
   {
      if (itemsOnLine >= mOutputColumnLimit)
      {
         itemsOnLine = 0;
         ofs << "\n          ";
      }
      ofs << ' ' << std::setw(10) << aColValues[colIndex] * yScale;
      ++itemsOnLine;
   }
   ofs << '\n';

   size_t rowColIndex = 0;
   for (rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      ofs << std::setw(10) << aRowValues[rowIndex] * xScale;
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
//! Write the scenario editor output if requested.
// private
void HorizontalMapFunction::WriteSeditContours(const std::vector<double>& aRowValues,
                                               const std::vector<double>& aColValues,
                                               const std::vector<Point>&  aVarValues,
                                               size_t                     aVarIndex)
{
   std::ofstream ofs(mSeditFile);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output sedit file.";
      out.AddNote() << "File: " << mSeditFile;
      return;
   }

   std::vector<ContourLevel> levels(mContourLevels);
   if (levels.empty())
   {
      levels.emplace_back();
   }

   ContourFilter2D           contourFilter;
   ContourFilter2D::LineList lineList;
   ContourFunction           contourFunction(aRowValues, aColValues, aVarValues, aVarIndex);

   for (const auto& level : levels)
   {
      double value = level.mLevel;
      lineList.clear();
      contourFilter.Contour(&contourFunction, 0, aRowValues.size() - 1, 0, aColValues.size() - 1, value, lineList);

      size_t                        j = 0;
      ContourFilter2D::LineListIter lineListIter;
      for (lineListIter = lineList.begin(); lineListIter != lineList.end(); ++lineListIter)
      {
         ++j;
         const ContourFilter2D::Line& line = *lineListIter;
         ofs << "zone\n";
         ofs << "  name: pd_" << value << "<" << j << ">\n";
         ofs << "  line_width: " << level.mLineWidth << "\n";
         UtColor color{level.mLineColor};
         color.SetFormat(UtColor::FmtFloat); // Force the output into a 3 - component floating point format
         ofs << "  line_color: " << color << '\n';
         ofs << "  stationary\n";
         if (line.size() >= 2)
         {
            if ((line.front().mX != line.back().mX) || (line.front().mY != line.back().mY))
            {
               ofs << "  non_closed\n";
            }
         }
         for (const auto& pt : line)
         {
            double lat = pt.mX;
            double lon = pt.mY;
            if (mLatMax == cUNDEFINED)
            {
               ConvertCartesianToSpherical(pt.mX, pt.mY, lat, lon);
            }
            ofs << "  ll: " << UtLatPos(lat) << ' ' << UtLonPos(lon) << '\n';
         }
         ofs << "end_zone\n";
      }
   }

   // Write out the locations of the platforms.

   size_t platformCount = mSimulationPtr->GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);

      // Skip the target platform.
      if (platformPtr == mTarget.GetPlatform())
      {
         continue;
      }

      ofs << "player\n";
      ofs << "  name: " << platformPtr->GetName() << '\n';
      ofs << "  type: " << platformPtr->GetIcon() << '\n';
      if (!platformPtr->GetSide().empty())
      {
         ofs << "  team: " << platformPtr->GetSide() << '\n';
      }
      double lat;
      double lon;
      double alt;
      platformPtr->GetLocationLLA(lat, lon, alt);
      double heading;
      double pitch;
      double roll;
      platformPtr->GetOrientationNED(heading, pitch, roll);
      ofs << "  lla: " << UtLatPos(lat) << ' ' << UtLonPos(lon) << ' ' << alt << " m\n";
      ofs << "  heading: " << heading * UtMath::cDEG_PER_RAD << '\n';
      ofs << "end_player\n";
   }

   // Write out the grid boundary.

   double latMin = mLatMin;
   double latMax = mLatMax;
   double lonMin = mLonMin;
   double lonMax = mLonMax;
   if (mLatMax == cUNDEFINED)
   {
      ConvertCartesianToSpherical(mDownRangeMin, mCrossRangeMin, latMin, lonMin);
      ConvertCartesianToSpherical(mDownRangeMax, mCrossRangeMax, latMax, lonMax);
   }
   // clang-format off
   ofs << "zone\n"
       << "  name: grid_boundary\n"
       << "  line_width: 2\n"
       << "  line_color: 0.0 0.0 0.0\n"
       << "  stationary\n"
       << "  ll: " << UtLatPos(latMin) << ' ' << UtLonPos(lonMin) << '\n'
       << "  ll: " << UtLatPos(latMin) << ' ' << UtLonPos(lonMax) << '\n'
       << "  ll: " << UtLatPos(latMax) << ' ' << UtLonPos(lonMax) << '\n'
       << "  ll: " << UtLatPos(latMax) << ' ' << UtLonPos(lonMin) << '\n'
       << "end_zone\n";
   // clang-format on
   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Sedit", mSeditFile);
}

// =================================================================================================
//! Write the scenario editor output if requested.
// private
void HorizontalMapFunction::WriteWSFContours(const std::vector<double>& aRowValues,
                                             const std::vector<double>& aColValues,
                                             const std::vector<Point>&  aVarValues,
                                             size_t                     aVarIndex)
{
   bool          multidim = (GetCommand() == "multi_dimensional_horizontal_map") ? true : false;
   std::ofstream ofs;
   if (!multidim || mFirstWrite)
   {
      ofs.open(mWSF_File);
      if (!ofs)
      {
         auto out = ut::log::error() << "Unable to open output wsf file.";
         out.AddNote() << "File: " << mWSF_File;
         return;
      }
      mFirstWrite = false;
   }
   else // if (GetCommand() == "multi_dimensional_horizontal_map")
   {
      ofs.open(mWSF_File, std::ios::out | std::ios::app);
      if (!ofs)
      {
         auto out = ut::log::error() << "Unable to open output wsf file.";
         out.AddNote() << "File: " << mWSF_File;
         return;
      }
      // get the file, and get to the end
   }

   std::vector<ContourLevel> levels(mContourLevels);
   if (levels.empty())
   {
      levels.emplace_back();
   }

   ContourFilter2D           contourFilter;
   ContourFilter2D::LineList lineList;
   ContourFunction           contourFunction(aRowValues, aColValues, aVarValues, aVarIndex);

   for (const auto& level : levels)
   {
      double value = level.mLevel;
      lineList.clear();
      contourFilter.Contour(&contourFunction, 0, aRowValues.size() - 1, 0, aColValues.size() - 1, value, lineList);

      static size_t mdj = 1;
      size_t        j   = 0;
      if (multidim)
      {
         j = mdj - 1;
      }
      ContourFilter2D::LineListIter lineListIter;
      for (lineListIter = lineList.begin(); lineListIter != lineList.end(); ++lineListIter)
      {
         ++j;
         ++mdj;
         const ContourFilter2D::Line& line = *lineListIter;
         ofs << "zone ";
         ofs << "pd_" << value << "<" << j << ">\n";
         if (multidim)
         {
            ofs << "/*"
                << " altitude " << mTargetAlt << " */ \n";
            ofs << "/*"
                << " pitch " << mTarget.GetTargetPitch() << " */ \n";
            ofs << "/*"
                << " roll " << mTarget.GetTargetRoll() << " */ \n";
         }
         ofs << "/* vespa <line_width> " << level.mLineWidth
             << " </line_width> end_vespa */ \n"; // there is no native support for line-width in AFSIM zones
         // ofs << "  stationary\n";
         /*if (line.size() >= 2)
         {
            if ((line.front().mX != line.back().mX) ||
               (line.front().mY != line.back().mY))
            {
               ofs << "  non_closed\n";
            }
         }*/
         UtColor color(level.mLineColor);
         color.SetFormat(UtColor::FmtUChar);
         ofs << "  line_color " << color << std::endl;
         ofs << "  fill_color 0 0 0 0" << std::endl; // transparent
         ofs << "  polygonal\n";
         ofs << "  lat_lon\n";
         for (const auto& pt : line)
         {
            double lat = pt.mX;
            double lon = pt.mY;
            if (mLatMax == cUNDEFINED)
            {
               ConvertCartesianToSpherical(pt.mX, pt.mY, lat, lon);
            }
            ofs << "  point " << UtLatPos(lat) << ' ' << UtLonPos(lon) << '\n';
         }
         ofs << "end_zone\n";
      }
   }

   if (!multidim)
   {
      // Write out the locations of the platforms.

      size_t platformCount = mSimulationPtr->GetPlatformCount();
      for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
      {
         WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);

         // Skip the target platform.
         if (platformPtr == mTarget.GetPlatform())
         {
            continue;
         }

         ofs << "platform " << platformPtr->GetName() << " " << platformPtr->GetType() << '\n';
         ofs << "  icon " << platformPtr->GetIcon() << '\n';
         if (!platformPtr->GetSide().empty())
         {
            ofs << "  side " << platformPtr->GetSide() << '\n';
         }
         double lat;
         double lon;
         double alt;
         platformPtr->GetLocationLLA(lat, lon, alt);
         double heading;
         double pitch;
         double roll;
         platformPtr->GetOrientationNED(heading, pitch, roll);
         ofs << "  position " << UtLatPos(lat) << ' ' << UtLonPos(lon) << ' ' << alt << " m\n";
         ofs << "  heading " << heading * UtMath::cDEG_PER_RAD << " deg" << '\n';
         ofs << "end_platform\n";
      }

      // Write out the grid boundary.

      double latMin = mLatMin;
      double latMax = mLatMax;
      double lonMin = mLonMin;
      double lonMax = mLonMax;
      if (mLatMax == cUNDEFINED)
      {
         ConvertCartesianToSpherical(mDownRangeMin, mCrossRangeMin, latMin, lonMin);
         ConvertCartesianToSpherical(mDownRangeMax, mCrossRangeMax, latMax, lonMax);
      }
      // clang-format off
      ofs << "zone "
          << "  grid_boundary\n"
          << "  /* vespa <line_width> 2 </line_width> end_vespa */ \n"
          << "  /*vespa <line_color> 00000000 </line_color> end_vespa */ \n"
          << "  polygonal\n"
          << "  lat_lon\n"
          << "  point " << UtLatPos(latMin) << ' ' << UtLonPos(lonMin) << '\n'
          << "  point " << UtLatPos(latMin) << ' ' << UtLonPos(lonMax) << '\n'
          << "  point " << UtLatPos(latMax) << ' ' << UtLonPos(lonMax) << '\n'
          << "  point " << UtLatPos(latMax) << ' ' << UtLonPos(lonMin) << '\n'
          << "end_zone\n";
      // clang-format on
   }

   ofs.close();

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("WSF", mWSF_File);
}

// =================================================================================================
//! Write the shapefile output if requested.
// private
void HorizontalMapFunction::WriteShapefile(const std::vector<double>& aRowValues,
                                           const std::vector<double>& aColValues,
                                           const std::vector<Point>&  aVarValues,
                                           size_t                     aVarIndex)
{
   GeoShapeFile shapefile(GeoShapeFile::Shape::ShapeTypePolyLine, // Shape type
                          GeoShapeFile::ProjectionTypeGeodetic,
                          false); // AGL base altitude

   GeoShapeFile::Part polyLinePart;
   polyLinePart.mPartType = GeoShapeFile::Shape::ShapeTypePolyLine;
   polyLinePart.mWinding  = 0;

   std::vector<ContourLevel> levels(mContourLevels);
   if (levels.empty())
   {
      levels.emplace_back();
   }

   ContourFilter2D           contourFilter;
   ContourFilter2D::LineList lineList;
   ContourFunction           contourFunction(aRowValues, aColValues, aVarValues, aVarIndex);
   shapefile.GetParameterList().emplace_back("WSFCOLOR");
   shapefile.GetParameterList().emplace_back("WSFWIDTH");

   GeoShapeFile::Position point;

   for (auto& level : levels)
   {
      shapefile.GetShapeList().emplace_back();
      GeoShapeFile::Shape& shape = shapefile.GetShapeList().back();
      std::string          colorstr;
      level.mLineColor.Get(colorstr);
      shape.GetParameterValues().emplace_back(colorstr);
      shape.GetParameterValues().emplace_back(std::to_string(level.mLineWidth));
      shape.SetShapeType(GeoShapeFile::Shape::ShapeTypePolyLine);

      double value = level.mLevel;
      lineList.clear();
      contourFilter.Contour(&contourFunction, 0, aRowValues.size() - 1, 0, aColValues.size() - 1, value, lineList);

      ContourFilter2D::LineListIter lineListIter;
      for (lineListIter = lineList.begin(); lineListIter != lineList.end(); ++lineListIter)
      {
         shape.GetPartList().push_back(polyLinePart);
         GeoShapeFile::Part& shapePart = shape.GetPartList().back();

         const ContourFilter2D::Line& line = *lineListIter;
         for (const auto& pt : line)
         {
            point.x = pt.mX; // lat
            point.y = pt.mY; // lon
            if (mLatMax == cUNDEFINED)
            {
               ConvertCartesianToSpherical(pt.mX, pt.mY, point.x, point.y);
            }
            shapePart.mPointList.push_back(point);
         }
      }
   }

   // Write out the grid boundary.

   shapefile.GetShapeList().emplace_back();
   GeoShapeFile::Shape& shape = shapefile.GetShapeList().back();
   shape.SetShapeType(GeoShapeFile::Shape::ShapeTypePolyLine);
   // Lower edge
   {
      shape.GetParameterValues().emplace_back("FFFFFFFF");
      shape.GetParameterValues().emplace_back(std::to_string(1));
      shape.GetPartList().push_back(polyLinePart);
      GeoShapeFile::Part& shapePart = shape.GetPartList().back();

      size_t rowIndex = 0;
      for (double colVal : aColValues)
      {
         point.x = aRowValues[rowIndex];
         point.y = colVal;
         if (mLatMax == cUNDEFINED)
         {
            ConvertCartesianToSpherical(aRowValues[rowIndex], colVal, point.x, point.y);
         }
         shapePart.mPointList.push_back(point);
      }
   }

   // Upper edge
   {
      shape.GetPartList().push_back(polyLinePart);
      GeoShapeFile::Part& shapePart = shape.GetPartList().back();

      size_t rowIndex = aRowValues.size() - 1;
      for (double colVal : aColValues)
      {
         point.x = aRowValues[rowIndex];
         point.y = colVal;
         if (mLatMax == cUNDEFINED)
         {
            ConvertCartesianToSpherical(aRowValues[rowIndex], colVal, point.x, point.y);
         }
         shapePart.mPointList.push_back(point);
      }
   }

   // Left edge
   {
      shape.GetPartList().push_back(polyLinePart);
      GeoShapeFile::Part& shapePart = shape.GetPartList().back();

      size_t colIndex = 0;
      for (double rowVal : aRowValues)
      {
         point.x = rowVal;
         point.y = aColValues[colIndex];
         if (mLatMax == cUNDEFINED)
         {
            ConvertCartesianToSpherical(rowVal, aColValues[colIndex], point.x, point.y);
         }
         shapePart.mPointList.push_back(point);
      }
   }

   // Right edge
   {
      shape.GetPartList().push_back(polyLinePart);
      GeoShapeFile::Part& shapePart = shape.GetPartList().back();

      size_t colIndex = aColValues.size() - 1;
      for (double rowVal : aRowValues)
      {
         point.x = rowVal;
         point.y = aColValues[colIndex];
         if (mLatMax == cUNDEFINED)
         {
            ConvertCartesianToSpherical(rowVal, aColValues[colIndex], point.x, point.y);
         }
         shapePart.mPointList.push_back(point);
      }
   }

   if (shapefile.Write(mShapefile + ".shp") == 0)
   {
      auto out = ut::log::error() << "error writing ESRI shapefile.";
      out.AddNote() << "File: " << mShapefile << ".shp";
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Shape", mShapefile);
}

// =================================================================================================
void HorizontalMapFunction::WriteAnalysisMapFile(const std::vector<double>& aRowValues,
                                                 const std::vector<double>& aColValues,
                                                 const std::vector<Point>&  aVarValues,
                                                 size_t                     aVarIndex)
{
   std::ofstream ofs(mAnalysisMapOptions.mFileName);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output analysis_map_file.";
      out.AddNote() << "File: " << mAnalysisMapOptions.mFileName;
      return;
   }

   ofs << "Generated by sensor_plot." << std::endl;
   ofs << mHeaderLine1 << std::endl;
   ofs << mHeaderLine2 << std::endl;
   ofs << mHeaderLine3 << std::endl;
   ofs << std::endl;

   ofs << "SENSOR_PLOT" << std::endl;
   ofs << mAnalysisMapOptions.mDataTitleStr << std::endl;
   ofs << mAnalysisMapOptions.mUnitsStr << std::endl;
   ofs << std::endl;

   double latMin = std::min(mLatMin, mLatMax);
   double latMax = std::max(mLatMin, mLatMax);
   double lonMin = std::min(mLonMin, mLonMax);
   double lonMax = std::max(mLonMin, mLonMax);
   double hDist, vDist;
   if (mLatMax == cUNDEFINED)
   {
      ConvertCartesianToSpherical(mDownRangeMin, mCrossRangeMin, latMin, lonMin);
      ConvertCartesianToSpherical(mDownRangeMax, mCrossRangeMax, latMax, lonMax);
      hDist = mCrossRangeMax - mCrossRangeMin;
      vDist = mDownRangeMax - mDownRangeMin;
   }
   else
   {
      double notUsed;
      UtSphericalEarth::GreatCircleHeadingAndDistance((latMin + latMax) / 2.0,
                                                      lonMin,
                                                      (latMin + latMax) / 2.0,
                                                      lonMax,
                                                      notUsed,
                                                      hDist);
      UtSphericalEarth::GreatCircleHeadingAndDistance(latMin, lonMax, latMax, lonMax, notUsed, vDist);
   }
   ofs << std::fixed << std::setprecision(11) << latMin << ' ' << lonMin << ' ' << latMax << ' ' << lonMax << std::endl;
   ofs << UtInput::ConvertValueTo(hDist, "km", UtInput::cLENGTH) << " "
       << UtInput::ConvertValueTo(vDist, "km", UtInput::cLENGTH) << std::endl;
   ofs << std::endl;

   if (!mAnalysisMapOptions.mColorRanges.empty())
   {
      ofs << "ColorLookupData" << std::endl;
      std::sort(mAnalysisMapOptions.mColorRanges.begin(), mAnalysisMapOptions.mColorRanges.end());

      std::vector<AnalysisMapOptions::ColorRange>::const_iterator it        = mAnalysisMapOptions.mColorRanges.begin();
      int                                                         numColors = 0;
      std::ostringstream oss; // temporary stream, will write to file after numColors is counted
      oss << std::fixed << std::setprecision(6);
      while (it != mAnalysisMapOptions.mColorRanges.end())
      {
         // Ensure all 4 color components are written even if alpha not specified in input
         UtColor color{it->mColor};
         color.SetFormat(UtColor::FmtFloatAlpha);
         oss << it->mMinValue << ' ' << color << std::endl;
         numColors++;
         double maxValue = it->mMaxValue;
         if ((++it != mAnalysisMapOptions.mColorRanges.end()) && (maxValue < it->mMinValue))
         {
            // gap in range values, fill in with invisible color
            oss << maxValue << ' ' << 0. << ' ' << 0. << ' ' << 0. << ' ' << 0. << std::endl;
            numColors++;
         }
      }

      ofs << numColors << std::endl;
      ofs << oss.str();
   }
   else if (!mContourLevels.empty())
   {
      ofs << "ColorLookupData" << std::endl;
      ofs << mContourLevels.size() << std::endl;

      for (std::vector<ContourLevel>::const_iterator it = mContourLevels.begin(); it != mContourLevels.end(); ++it)
      {
         ofs << std::fixed << std::setprecision(6) << it->mLevel << ' ' << it->mLineColor << ' ' << 1. << std::endl;
      }
   }
   else
   {
      ofs << "DefaultColorLookupData" << std::endl;
   }
   ofs << std::endl;

   ofs << "NoOverlayGrid" << std::endl;
   ofs << std::endl;

   size_t numRows = aRowValues.size();
   size_t numCols = aColValues.size();
   ofs << aVarValues.size() << ' ' << numRows << ' ' << numCols << ' '
       << std::min_element(aVarValues.begin(), aVarValues.end(), PointCompare(aVarIndex))->at(aVarIndex) << ' '
       << std::max_element(aVarValues.begin(), aVarValues.end(), PointCompare(aVarIndex))->at(aVarIndex) << std::endl;
   ofs << std::endl;

   size_t rowColIndex = 0;
   for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
   {
      for (size_t colIndex = 0; colIndex < numCols; ++colIndex)
      {
         ofs << std::fixed << std::setprecision(11) << aVarValues[rowColIndex++][aVarIndex] << ' ';
      }
      ofs << std::endl;
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Analysis Map", mAnalysisMapOptions.mFileName);
}

void HorizontalMapFunction::WriteTiffFile(const std::vector<double>& aRowValues,
                                          const std::vector<double>& aColValues,
                                          const std::vector<Point>&  aVarValues) const
{
   double latMin = std::min(mLatMin, mLatMax);
   double latMax = std::max(mLatMin, mLatMax);
   double lonMin = std::min(mLonMin, mLonMax);
   double lonMax = std::max(mLonMin, mLonMax);

   UtTiff             tiffOut;
   uint32_t           w        = static_cast<uint32_t>(aColValues.size());
   uint32_t           h        = static_cast<uint32_t>(aRowValues.size());
   size_t             varCount = aVarValues[0].size();
   std::vector<float> data(w * h * varCount);
   for (size_t i = 0; i < varCount; ++i)
   {
      unsigned int j   = 0;
      int32_t      min = std::numeric_limits<int32_t>::max();
      int32_t      max = std::numeric_limits<int32_t>::min();
      for (auto&& it : aVarValues)
      {
         // this flips the image vertically to match what we want in geotiff
         unsigned int flipj = (h - 1 - j / w) * w + j % w;
         ++j;
         data[flipj + i * w * h] = static_cast<float>(it[i]);
         int32_t ival_lo         = static_cast<int32_t>(it[i]);
         int32_t ival_hi         = static_cast<int32_t>(ceil(it[i]));
         min                     = std::min(ival_lo, min);
         max                     = std::max(ival_hi, max);
      }
      UtTiff::Image& img = tiffOut.AddImage("AFSIM SensorPlot",
                                            w,
                                            h,
                                            UtTiff::cPixelFormatR,
                                            UtTiff::cComponentFormatFloat,
                                            min,
                                            max,
                                            &data[i * w * h]);
      img.SetLabel(mPlotVariables.GetLabel(i));
      if (mLatMax != cUNDEFINED)
      {
         img.SetGeodeticRectangle(latMin, lonMin, latMax, lonMax);
      }
   }
   tiffOut.WriteToFile(mTiffFilename);

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("tiff", mTiffFilename);
}

// =================================================================================================
//! Returns the JSON metadata to be associated with a PD map output file. The
//! metadata contains the name of the variable that the data pertains to as
//! well as the names of the data that make up the X and Y axes. This metadata
//! can be used to label the data in visualizations; e.g., in the IDE.
std::string HorizontalMapFunction::GetPdMapFileJsonMetadata() const
{
   const std::string variable("\"variable\": \"" + mPlotVariables.GetLabel(0) + "\"");
   const std::string xAxis("\"x\": \"Down range (" + mDownRangeUnits + ")\"");
   const std::string yAxis("\"y\": \"Cross range (" + mCrossRangeUnits + ")\"");
   const std::string json("{ " + variable + ", " + xAxis + ", " + yAxis + " }");
   return json;
}
