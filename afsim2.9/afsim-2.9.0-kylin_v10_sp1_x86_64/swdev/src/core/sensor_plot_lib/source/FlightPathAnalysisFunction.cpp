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

#include "FlightPathAnalysisFunction.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "Utility.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTSPI_Mover.hpp"
#include "WsfTerrain.hpp"

// =================================================================================================
FlightPathAnalysisFunction::FlightPathAnalysisFunction(const WsfScenario& aScenario)
   : MapPlotFunction(aScenario)
   , mSimulationPtr(nullptr)
   , mVarList()
   , mVarLabels()
   , mVarMinValues()
   , mPathPoints()
   , mALARM_FPA_FileName()
   , mTSPI_FileName()
   , mTSPI_Point()
   , mSampleInterval(1.0)
   , mSensorPlatformYaw(0.0)
   , mSensorPlatformPitch(0.0)
   , mSensorPlatformRoll(0.0)
   , mModeName()
   , mModeIndex(0)
   , mAutomaticTargetCueing(true)
   , mReverseEvaluations(false)
   , mTargetPlatformId(nullptr)
   , mRangeUnits("km")
   , mProjection()
   , mGnuPlotFile()
   , mHeaderLine1("")
   , mHeaderLine2("")
   , mHeaderLine3("")
   , mExclusionList()
   , mInclusionList()
   , mSensors()
{
   // Initialize sensor to ENU conversion.
   mProjection.SetCenter(0.0, 0.0);
   UtMat3d::Identity(mOriginToENU_Transform);
}

// =================================================================================================
// virtual
bool FlightPathAnalysisFunction::Execute(WsfSimulation& aSimulation)
{
   Function::Execute(aSimulation);
   mSimulationPtr = &aSimulation;
   if (mGnuPlotFile.empty())
   {
      ut::log::error() << "gnuplot_file must be specified.";
      return false;
   }

   // The TSPI file is read here rather than in ProcessInput so there is no order dependence in
   // the command input stream (not requiring TSPI unit modifiers to appear before the file name).
   if (!mTSPI_FileName.empty())
   {
      bool ok = false;
      try
      {
         // Throws if file could not be opened.
         ReadTSPI_File(mTSPI_FileName);
         ok = true;
      }
      catch (UtException&)
      {
         // Catch and rely on !ok to log error.
      }
      if (!ok)
      {
         auto out = ut::log::error() << "Unable to process TSPI file.";
         out.AddNote() << "File: " << mTSPI_FileName;
         return false;
      }
   }

   // Setup the platform to detect, normally the TARGET.

   if (mTargetPlatformId == nullptr)
   {
      mTargetPlatformId = "TARGET";
   }

   WsfPlatform* targetPtr = aSimulation.GetPlatformByName(mTargetPlatformId);
   if (targetPtr == nullptr)
   {
      auto out = ut::log::error() << "Target platform does not exist.";
      out.AddNote() << "Platform: " << mTargetPlatformId;
      return false;
   }
   size_t targetIndex = targetPtr->GetIndex();

   // Ensure there are sample points. They may come from a file or from the route of the target.
   // Note that tests like IsStopped, IsPaused, IsExtrapolating or speed tests do not work here
   // because the mover hasn't moved yet. In addition, the mover 'start_time' may be used to sync
   // the start time with the simulation time of the initial detection chance defined later.

   bool      moverHasPath = false;
   WsfMover* moverPtr     = targetPtr->GetMover();
   if (moverPtr != nullptr)
   {
      if (((moverPtr->GetRoute() != nullptr) && (moverPtr->GetRoute()->GetSize() >= 2)) ||
          (dynamic_cast<WsfTSPI_Mover*>(moverPtr) != nullptr))
      {
         moverHasPath = true;
      }
   }

   if (mPathPoints.empty() && (!moverHasPath))
   {
      ut::log::error() << "Sample points or 'route' have not been provided.";
      return false;
   }

   // If path points were specified, do away with the mover.
   if (!mPathPoints.empty())
   {
      targetPtr->SetMover(nullptr);
   }

   mSensors.clear();
   WsfPlatform* originPlatformPtr = nullptr;
   if (mReverseEvaluations)
   {
      if (targetPtr->GetComponentCount<WsfSensor>() == 0)
      {
         ut::log::error() << "No sensors defined.";
         return false;
      }

      // The origin of the X/Y system is the first platform that is NOT the 'target'.

      size_t platformCount = mSimulationPtr->GetPlatformCount();
      for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
      {
         WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);
         if ((platformPtr != nullptr) && (platformPtr->GetIndex() != targetPtr->GetIndex()))
         {
            originPlatformPtr = platformPtr;
            break;
         }
      }
      if (originPlatformPtr == nullptr)
      {
         ut::log::error() << "No objects for the sensors to detect.";
         return false;
      }
   }
   else
   {
      // Build the list of sensors. The include/exclude list is used to filter the list.

      size_t platformCount = mSimulationPtr->GetPlatformCount();
      for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
      {
         WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(platformEntry);
         if (platformPtr == targetPtr)
         {
            continue;
         }

         for (WsfComponentList::RoleIterator<WsfSensor> iter(*platformPtr); !iter.AtEnd(); ++iter)
         {
            WsfSensor* sensorPtr = *iter;
            if (sensorPtr)
            {
               // A sensor is implicitly 'included'. If it isn't explicitly included then it will be included only
               // if it isn't in the exclusion list (i.e.: The inclusion list takes priority).

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
                     mSimulationPtr->TurnPartOn(0.0, sensorPtr);
                  }
               }
               else
               {
                  auto out = ut::log::info() << "Excluded sensor from output.";
                  out.AddNote() << "Platform: " << platformPtr->GetName();
                  out.AddNote() << "Sensor: " << sensorPtr->GetName();
               }
            }
         }
      }
      if (mSensors.empty())
      {
         ut::log::error() << "No sensors defined.";
         return false;
      }

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

      // Set the origin of the X/Y system to that of the first sensor.
      originPlatformPtr = mSensors[0]->GetPlatform();
   }

   // Establish the transform for the X/Y coordinate system.

   if (originPlatformPtr != nullptr)
   {
      double latOrigin;
      double lonOrigin;
      double notUsed;
      originPlatformPtr->GetLocationLLA(latOrigin, lonOrigin, notUsed);
      mProjection.SetCenter(latOrigin, lonOrigin);
      double heading;
      originPlatformPtr->GetOrientationNED(heading, notUsed, notUsed);
      // Convert heading from degrees clockwise from north to degrees counter clockwise from east.
      double downRangeAxisAngle = UtMath::NormalizeAngleMinusPi_Pi((UtMath::cTWO_PI - heading) + UtMath::cPI_OVER_2);
      double sinAngle           = sin(downRangeAxisAngle);
      double cosAngle           = cos(downRangeAxisAngle);
      UtMat3d::Identity(mOriginToENU_Transform);
      mOriginToENU_Transform[0][0] = cosAngle;
      mOriginToENU_Transform[0][1] = -sinAngle;
      mOriginToENU_Transform[1][0] = sinAngle;
      mOriginToENU_Transform[1][1] = cosAngle;
   }

   // NOTE: The concept of a simulation time must be maintained in order to force the invalidation
   // of sensor subsystem location data on the target platform (if it has any).  The target may have
   // active systems that are attempting to be detected by the system under test...
   //
   // In addition, there may be 'execute at_time' scripts that need to execute sensor or jammer cueing.
   // Such scripts should use a time < 1 second so they execute before the first detection chance.

   double simTime = 1.0; // Time of first detection chance (DO NOT CHANGE THIS!)
   InitializeSensorPlatforms();

   // Initialize the map plot variables
   if (mPlotVariables.Count() < 2)
   {
      auto out = ut::log::error() << "At least two variables must be selected.";
      out.AddNote() << "Defined: " << mPlotVariables.Count();
      return false;
   }

   if (!mPlotVariables.Initialize(aSimulation))
   {
      return false;
   }

   std::vector<Point>  dataValues;
   std::vector<double> varValues;
   size_t              pointIndex = 0;
   bool                done       = false;
   while (!done)
   {
      // Force event dispatching in case anything is event driven.

      while (mSimulationPtr->AdvanceTime(simTime) < simTime)
      {
      }

      // Perform the required sensor detections.

      done      = true; // Assume done if the platform died
      targetPtr = mSimulationPtr->GetPlatformByIndex(targetIndex);
      if (targetPtr != nullptr)
      {
         varValues.clear();
         if (mPathPoints.empty())
         {
            targetPtr->Update(simTime);
            done = targetPtr->IsExtrapolating();
            if (!done)
            {
               EvaluatePoint(simTime, targetPtr, varValues);
               dataValues.push_back(varValues);
            }
         }
         else
         {
            // Push the data from the point into the platform

            const PathPoint& pathPoint = mPathPoints[pointIndex];
            wsf::Terrain     terrain(mSimulationPtr->GetTerrainInterface());
            float            height;
            double           alt = pathPoint.mAlt;
            if (pathPoint.mAltIsAGL)
            {
               terrain.GetElevInterp(pathPoint.mLat, pathPoint.mLon, height);
               alt += height;
            }
            targetPtr->SetLocationLLA(pathPoint.mLat, pathPoint.mLon, alt);
            targetPtr->SetOrientationNED(pathPoint.mHeading, pathPoint.mPitch, pathPoint.mRoll);

            // Set the velocity in a form that is compatible with WsfTSPI_Mover.
            if (mPathPoints.size() == 1)
            {
               double velocityNED[3] = {pathPoint.mSpeed * cos(pathPoint.mHeading),
                                        pathPoint.mSpeed * sin(pathPoint.mHeading),
                                        0.0};
               targetPtr->SetVelocityNED(velocityNED);
            }
            else
            {
               size_t p1 = 0;
               size_t p2 = 1;
               if (pointIndex > 0)
               {
                  p1 = pointIndex - 1;
                  p2 = pointIndex;
               }
               double alt1 = mPathPoints[p1].mAlt;
               if (mPathPoints[p1].mAltIsAGL)
               {
                  terrain.GetElevInterp(mPathPoints[p1].mLat, mPathPoints[p1].mLon, height);
                  alt1 += height;
               }
               double alt2 = mPathPoints[p2].mAlt;
               if (mPathPoints[p2].mAltIsAGL)
               {
                  terrain.GetElevInterp(mPathPoints[p2].mLat, mPathPoints[p2].mLon, height);
                  alt2 += height;
               }
               double r1[3];
               double r2[3];
               UtEntity::ConvertLLAToWCS(mPathPoints[p1].mLat, mPathPoints[p1].mLon, alt1, r1);
               UtEntity::ConvertLLAToWCS(mPathPoints[p2].mLat, mPathPoints[p2].mLon, alt2, r2);
               double dr[3];
               UtVec3d::Subtract(dr, r2, r1);
               UtVec3d::Normalize(dr);
               double velWCS[3];
               UtVec3d::Multiply(velWCS, dr, pathPoint.mSpeed);
               targetPtr->SetVelocityWCS(velWCS);
            }

            EvaluatePoint(simTime, targetPtr, varValues);
            dataValues.push_back(varValues);
            ++pointIndex;
            done = (pointIndex >= mPathPoints.size());
         }
         simTime += mSampleInterval;
      }
   }

   // Write plots for selected variables.

   if (!mGnuPlotFile.empty())
   {
      WriteGnuPlotFile(dataValues);
   }

   return true;
}

// =================================================================================================
// virtual
bool FlightPathAnalysisFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "gnuplot_file")
   {
      aInput.ReadValueQuoted(mGnuPlotFile);
      mGnuPlotFile = aInput.SubstitutePathVariables(mGnuPlotFile);
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
   else if (command == "target_platform_name")
   {
      std::string pltfrmName;
      aInput.ReadValue(pltfrmName);
      mTargetPlatformId = pltfrmName;
   }
   else if (command == "sample_interval")
   {
      aInput.ReadValueOfType(mSampleInterval, UtInput::cTIME);
      aInput.ValueGreater(mSampleInterval, 0.0);
   }
   else if (command == "automatic_target_cueing")
   {
      aInput.ReadValue(mAutomaticTargetCueing);
   }
   else if (command == "reverse_evaluations")
   {
      aInput.ReadValue(mReverseEvaluations);
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
   else if (command == "path")
   {
      ResetPathPoints();
      PathPoint    point;
      bool         havePosition = false;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "position")
         {
            if (havePosition)
            {
               mPathPoints.push_back(point);
            }
            havePosition = true;
            aInput.ReadValueOfType(point.mLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(point.mLon, UtInput::cLONGITUDE);
         }
         else if (havePosition)
         {
            if (command == "altitude")
            {
               std::string altRef;
               aInput.ReadValueOfType(point.mAlt, UtInput::cLENGTH);
               aInput.ReadValue(altRef);
               if (altRef == "agl")
               {
                  point.mAltIsAGL = true;
               }
               else if (altRef == "msl")
               {
                  point.mAltIsAGL = false;
               }
               else
               {
                  point.mAltIsAGL = false;
                  aInput.PushBack(altRef);
               }
            }
            else if (command == "heading")
            {
               aInput.ReadValueOfType(point.mHeading, UtInput::cANGLE);
               aInput.ValueInClosedRange(point.mHeading, -UtMath::cPI, UtMath::cTWO_PI);
               point.mHeading = UtMath::NormalizeAngleMinusPi_Pi(point.mHeading);
            }
            else if (command == "pitch")
            {
               aInput.ReadValueOfType(point.mPitch, UtInput::cANGLE);
               aInput.ValueInClosedRange(point.mRoll, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
            }
            else if (command == "roll")
            {
               aInput.ReadValueOfType(point.mRoll, UtInput::cANGLE);
               aInput.ValueInClosedRange(point.mRoll, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
            }
            else if (command == "speed")
            {
               aInput.ReadValueOfType(point.mSpeed, UtInput::cSPEED);
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (havePosition)
      {
         mPathPoints.push_back(point);
      }
   }
   else if (command == "alarm_fpa_file")
   {
      ResetPathPoints();
      aInput.ReadValue(mALARM_FPA_FileName);
      mALARM_FPA_FileName = aInput.LocateFile(mALARM_FPA_FileName);
      ReadALARM_FPA_File(mALARM_FPA_FileName);
   }
   else if ((command == "tspi_file") || (command == "TSPI_file") ||
            (command == "TSPI_filename")) // For compatibility with the mover
   {
      ResetPathPoints();
      aInput.ReadValue(mTSPI_FileName);
      mTSPI_FileName = aInput.LocateFile(mTSPI_FileName);
      // The file is read in Execute() so there is no order dependence between this command
      // and the TSPI unit modifier commands read below.
   }
   else if (mTSPI_Point.ProcessInput(aInput))
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
bool FlightPathAnalysisFunction::ConvertALARM_LatLon(const std::string& aString, bool aIsLat, double& aOutput)
{
   //-------------------------------------------------------------------
   // Determine hemisphere by checking for a minus sign at the beginning
   // or a NSEW letter prefix or suffix.
   //-------------------------------------------------------------------

   std::string::size_type strLen = aString.length();
   if (strLen < 6)
   {
      return false;
   }

   std::string::size_type lastChar = strLen - 1;

   char        dirChar = ' ';
   std::string str(aString);
   if (str[0] == '-')
   {
      if (aIsLat)
      {
         dirChar = 'S';
         str     = aString.substr(1);
      }
      else
      {
         dirChar = 'W';
         str     = aString.substr(1);
      }
   }
   else if ((str[0] == 's') || (str[0] == 'S') || (str[0] == 'w') || (str[0] == 'W') || (str[0] == 'n') ||
            (str[0] == 'N') || (str[0] == 'e') || (str[0] == 'E'))
   {
      dirChar = str[0];
      str     = aString.substr(1);
   }
   else if ((str[lastChar] == 's') || (str[lastChar] == 'S') || (str[lastChar] == 'w') || (str[lastChar] == 'W') ||
            (str[lastChar] == 'n') || (str[lastChar] == 'N') || (str[lastChar] == 'e') || (str[lastChar] == 'E'))
   {
      dirChar = str[lastChar];
      str     = aString.substr(0, lastChar);
   }

   std::string::size_type dmsEnd = str.find('.');
   if (dmsEnd == std::string::npos)
   {
      dmsEnd = str.length();
   }

   if (dmsEnd == 6)
   {
      str = str.substr(0, 2) + ':' + str.substr(2, 2) + ':' + str.substr(4) + dirChar;
   }
   else if (dmsEnd == 7)
   {
      str = str.substr(0, 3) + ':' + str.substr(3, 2) + ':' + str.substr(5) + dirChar;
   }
   else
   {
      return false;
   }

   std::istringstream iss(str);
   if (aIsLat)
   {
      UtLatPos lat;
      iss >> lat;
      aOutput = lat;
   }
   else
   {
      UtLonPos lon;
      iss >> lon;
      aOutput = lon;
   }

   bool ok = true;
   if (!iss)
   {
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Given a down range and cross range value from the sensor, compute the equivalent latitude and longitude.
void FlightPathAnalysisFunction::ConvertCartesianToSpherical(double  aDownRange,
                                                             double  aCrossRange,
                                                             double& aLatitude,
                                                             double& aLongitude)
{
   double relativeXYZ[3] = {aDownRange, aCrossRange, 0.0};
   double enu[3];
   UtMat3d::Transform(enu, mOriginToENU_Transform, relativeXYZ);
   mProjection.ConvertXYToLatLon(enu[0], enu[1], aLatitude, aLongitude);
}

// =================================================================================================
//! Given a latitude and longitude, compute the down range and cross range value from the origin.
void FlightPathAnalysisFunction::ConvertSphericalToCartesian(double  aLatitude,
                                                             double  aLongitude,
                                                             double& aDownRange,
                                                             double& aCrossRange)
{
   double enu[3] = {0.0, 0.0, 0.0};
   mProjection.ConvertLatLonToXY(aLatitude, aLongitude, enu[0], enu[1]);
   double relativeXYZ[3];
   UtMat3d::InverseTransform(relativeXYZ, mOriginToENU_Transform, enu);
   aDownRange  = relativeXYZ[0];
   aCrossRange = relativeXYZ[1];
}

// =================================================================================================
//! Determine the function value for a point.
//!
//! This routine will perform a detection attempt from all sensors against the target.
//! The return value is the selected function for the sensor that had the best probability
//! of detection.
// private
void FlightPathAnalysisFunction::EvaluatePoint(double aSimTime, WsfPlatform* aTargetPtr, std::vector<double>& aVarValues)
{
   mPlotVariables.InitializeValues(*aTargetPtr->GetSimulation(), aVarValues, mVarMinValues);

   double lat;
   double lon;
   double alt;
   aTargetPtr->GetLocationLLA(lat, lon, alt);
   double downRange;
   double crossRange;
   ConvertSphericalToCartesian(lat, lon, downRange, crossRange);
   mPlotVariables.SetCrossRange(crossRange);
   mPlotVariables.SetDownRange(downRange);

   // Set units for any scaling.
   double xScale = 1.0 / UtInput::ConvertValueFrom(1.0, mRangeUnits, UtInput::cLENGTH);
   mPlotVariables.SetRangeScale(xScale);

   if (mReverseEvaluations)
   {
      // When the evaluations are reversed, the sensors on the target platform attempt to detect all
      // the other platforms/sensors in the scenario.

      for (unsigned int entryIndex = 0; entryIndex < mSimulationPtr->GetPlatformCount(); ++entryIndex)
      {
         WsfPlatform* platformPtr = mSimulationPtr->GetPlatformEntry(entryIndex);
         if ((platformPtr != nullptr) && (platformPtr != aTargetPtr))
         {
            if (mAutomaticTargetCueing)
            {
               // Cue every sensor on the opposing platforms so they are pointed at me.
               for (WsfComponentList::RoleIterator<WsfSensor> iter(*platformPtr); !iter.AtEnd(); ++iter)
               {
                  WsfSensor* sensorPtr = *iter;
                  if (sensorPtr)
                  {
                     Utility::CueSensorToTarget(sensorPtr, aTargetPtr);
                  }
               }
            }
            for (WsfComponentList::RoleIterator<WsfSensor> iter(*platformPtr); !iter.AtEnd(); ++iter)
            {
               WsfSensor* sensorPtr = *iter;
               if (sensorPtr)
               {
                  EvaluateSensor(aSimTime, sensorPtr, platformPtr, aVarValues);
               }
            }
         }
      }
   }
   else
   {
      // When evaluations are NOT reversed, all of the sensors attempt to detect the target.

      for (WsfSensor* sensorPtr : mSensors)
      {
         EvaluateSensor(aSimTime, sensorPtr, aTargetPtr, aVarValues);
      }
   }
}

// =================================================================================================
void FlightPathAnalysisFunction::EvaluateSensor(double               aSimTime,
                                                WsfSensor*           aSensorPtr,
                                                WsfPlatform*         aTargetPtr,
                                                std::vector<double>& aVarValues)
{
   WsfSensorResult     result;
   WsfSensor::Settings settings;
   settings.mModeIndex = mModeIndex;
   // If there are multiple sensors then use the initial/current mode of the sensor if it supports modes
   if (mSensors.size() > 1)
   {
      WsfSensorMode* modePtr = aSensorPtr->GetCurrentMode();
      if (modePtr != nullptr)
      {
         settings.mModeIndex = modePtr->GetModeIndex();
      }
   }
   settings.mRequiredPd = 1.0E-6;

   // Cue the sensor to the target just in case this is a tracker.

   if (mAutomaticTargetCueing)
   {
      Utility::CueSensorToTarget(aSensorPtr, aTargetPtr);
   }

   aSensorPtr->AttemptToDetect(aSimTime, aTargetPtr, settings, result);

   mPlotVariables.Evaluate(aSimTime, aSensorPtr, result, aVarValues);
}

// =================================================================================================
void FlightPathAnalysisFunction::InitializeSensorPlatforms()
{
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

// =================================================================================================
bool FlightPathAnalysisFunction::InSelectorList(WsfSensor* aSensorPtr, const SelectorList& aSelectorList)
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
// Read an ALARM FPA file
void FlightPathAnalysisFunction::ReadALARM_FPA_File(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtException("Unable to open alarm_fpa_file " + aFileName);
   }

   std::string line;

   // Skip over comments
   while (getline(ifs, line) && (line[0] == '#'))
   {
   }

   // Read the point count and altitude reference
   int                pointCount;
   std::string        altitudeReference;
   std::istringstream iss(line);
   if (!(iss >> pointCount >> altitudeReference))
   {
      throw UtException("Error reading alarm_fpa_file " + aFileName + "\ndata='" + line + "'");
   }

   PathPoint point;

   if ((altitudeReference == "AGL") || (altitudeReference == "agl"))
   {
      point.mAltIsAGL = true;
   }
   else if ((altitudeReference == "MSL") || (altitudeReference == "msl"))
   {
      point.mAltIsAGL = false;
   }
   else
   {
      throw UtException("Invalid altitude reference in alarm_fpa_file " + aFileName);
   }

   // Skip over comments
   while (getline(ifs, line) && (line[0] == '#'))
   {
   }

   // Read the path points
   for (int i = 0; i < pointCount; ++i)
   {
      if (!ifs)
      {
         throw UtException("Unexpected end-of-file reading alarm_fpa_file " + aFileName);
      }
      std::string        lat;
      std::string        lon;
      std::istringstream iss2(line);
      if (!(iss2 >> lat >> lon >> point.mAlt >> point.mSpeed >> point.mHeading >> point.mPitch >> point.mRoll))
      {
         throw UtException("Error reading alarm_fpa_file " + aFileName + "\ndata='" + line + "'");
      }
      if ((!ConvertALARM_LatLon(lat, true, point.mLat)) || (!ConvertALARM_LatLon(lon, false, point.mLon)))
      {
         throw UtException("Error reading alarm_fpa_file " + aFileName + "\ndata='" + line + "'");
      }
      point.mHeading *= UtMath::cRAD_PER_DEG;
      point.mHeading *= UtMath::NormalizeAngleMinus180_180(point.mHeading);
      point.mPitch *= UtMath::cRAD_PER_DEG;
      point.mRoll *= UtMath::cRAD_PER_DEG;
      mPathPoints.push_back(point);

      getline(ifs, line);
   }
}

// =================================================================================================
void FlightPathAnalysisFunction::ReadSelector(UtInput& aInput, SelectorList& aSelectorList)
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
//! Read a TSPI file.
void FlightPathAnalysisFunction::ReadTSPI_File(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtException("Unable to open TSPI file " + aFileName);
   }

   PathPoint point;
   while ((ifs >> mTSPI_Point))
   {
      // point.mTime   = mTSPI_Point.mTime();
      point.mLat     = mTSPI_Point.mLat();
      point.mLon     = mTSPI_Point.mLon();
      point.mAlt     = mTSPI_Point.mAlt();
      point.mHeading = mTSPI_Point.mHeading();
      point.mHeading = UtMath::NormalizeAngleMinusPi_Pi(point.mHeading);
      point.mPitch   = mTSPI_Point.mPitch();
      point.mRoll    = mTSPI_Point.mRoll();
      point.mSpeed   = mTSPI_Point.mSpeed();
      mPathPoints.push_back(point);
   }
}

// =================================================================================================
//! Reset the path point source back to the default (no source).
void FlightPathAnalysisFunction::ResetPathPoints()
{
   mPathPoints.clear();
   mALARM_FPA_FileName = "";
   mTSPI_FileName      = "";
}

// =================================================================================================
//! Write output gnuplot 'splot' output.
void FlightPathAnalysisFunction::WriteGnuPlotFile(const std::vector<Point>& aVarValues)
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

   ofs << "# gnuplot 'plot' data file\n";
   ofs << "#\n";
   int column = 1;
   for (size_t i = 0; i < mPlotVariables.Count(); ++i)
   {
      ofs << "# Column " << column << ": " << mPlotVariables.GetLabel(i) << '\n';
      ++column;
   }
   for (size_t i = 0; i < mVarLabels.size(); ++i)
   {
      ofs << "# Column " << column << ": " << mVarLabels[i] << '\n';
      ++column;
   }

   // double xScale = 1.0 / UtInput::ConvertValueFrom(1.0, mDownRangeUnits, UtInput::cLENGTH);
   // double yScale = 1.0 / UtInput::ConvertValueFrom(1.0, mCrossRangeUnits, UtInput::cLENGTH);

   size_t pointCount = aVarValues.size();
   for (size_t pointIndex = 0; pointIndex < pointCount; ++pointIndex)
   {
      size_t varCount = aVarValues[0].size();
      for (size_t varIndex = 0; varIndex < varCount; ++varIndex)
      {
         if (varIndex != 0)
         {
            ofs << ' ';
         }
         ofs << std::setw(10) << aVarValues[pointIndex][varIndex];
      }
      ofs << '\n';
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("GNU Plot", mGnuPlotFile);
}
