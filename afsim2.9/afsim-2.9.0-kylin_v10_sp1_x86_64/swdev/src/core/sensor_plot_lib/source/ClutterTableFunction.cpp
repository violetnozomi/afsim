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

#include "ClutterTableFunction.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTerrain.hpp"
#include "WsfUtil.hpp"

ClutterTableFunction::ClutterTableFunction(WsfScenario& aScenario)
   : Function(aScenario)
   , mSimulationPtr(nullptr)
   , mEnvelope()
   , mSensor()
   , mTarget(aScenario.GetAtmosphere())
   , mSensorPlatformYaw(0.0)
   , mSensorPlatformPitch(0.0)
   , mSensorPlatformRoll(0.0)
   , mSensorPlatformLat(0.0)
   , mSensorPlatformLon(0.0)
   , mSensorPlatformAlt(0.0)
   , mSensorPlatformAltSet(false)
   , mOutputObjectName()
   , mOutputFileName()
   , mAltUnitsStr("m")
   , mBearingUnitsStr("degrees")
   , mAltUnitsScale(1.0)
   , mRangeUnitsStr("m")
   , mRangeUnitsScale(1.0)
   , mBearingUnitsScale(UtMath::cDEG_PER_RAD)
{
}

// virtual
bool ClutterTableFunction::Execute(WsfSimulation& aSimulation)
{
   Function::Execute(aSimulation);
   mSimulationPtr = &aSimulation;
   if ((!mSensor.CreateAndInitialize(aSimulation)) || (!mTarget.CreateAndInitialize(aSimulation)))
   {
      return false;
   }

   mSensor.GetPlatform()->SetOrientationNED(mSensorPlatformYaw, mSensorPlatformPitch, mSensorPlatformRoll);
   if (mSensorPlatformAltSet)
   {
      wsf::Terrain terrain(aSimulation.GetTerrainInterface());
      if (terrain.IsEnabled() && (mSensor.GetPlatform()->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND))
      {
         float elev;
         terrain.GetElevInterp(mSensorPlatformLat, mSensorPlatformLon, elev);
         mSensorPlatformAlt += elev;
      }
   }
   mSensor.GetPlatform()->SetLocationLLA(mSensorPlatformLat, mSensorPlatformLon, mSensorPlatformAlt);

   // We still need to set the location of the sensor.

   // Generate the base name for output files.  If one hasn't been defined then we'll use the
   // sensor type name.
   std::string baseName(mOutputFileName);
   if (baseName.empty())
   {
      baseName = mSensor.GetSensor()->GetType();
   }

   std::ofstream plotOfs(baseName.c_str());

   std::string objectName(mOutputObjectName);
   if (objectName.empty())
   {
      objectName = mSensor.GetSensor()->GetType();
   }

   plotOfs << "clutter_model " << objectName << " WSF_SURFACE_CLUTTER_TABLE" << std::endl;
   plotOfs << "  clutters" << std::endl;

   WsfSensorResult result;
   for (size_t altIndex = 0; altIndex < mEnvelope.size(); ++altIndex)
   {
      double altitude = Altitude(mEnvelope, altIndex);

      ut::log::info() << "Processing altitude: " << altitude;

      plotOfs << "    altitude " << altitude * mAltUnitsScale << " " << mAltUnitsStr << std::endl;

      size_t numBearings = mBearings.size();

      for (size_t bearingIndex = 0; bearingIndex < numBearings; ++bearingIndex)
      {
         double bearing = mBearings[bearingIndex];
         if (numBearings > 1)
         {
            // This becomes a site-specific clutter map.
            plotOfs << "     bearing " << bearing * mBearingUnitsScale << " " << mBearingUnitsStr << std::endl;
         }

         for (size_t rangeIndex = 0; rangeIndex < mEnvelope[altIndex].data.size(); ++rangeIndex)
         {
            double groundRange = GroundRange(mEnvelope, altIndex, rangeIndex);

            // Set the location, speed and attitude of the target.
            mTarget.SetLocationRBA(mSensor, groundRange, bearing, altitude);

            mTarget.SetSpeedAndAttitude(mSensor.GetSensor());

            // Attempt to cue the sensor to the target, just in case the sensor is a tracker.
            mSensor.CueToTarget(mTarget);

            // Perform the detection attempt.
            /* bool detected = */ mSensor.AttemptToDetect(mTarget, result);

            // get the clutter power
            double clutterPowerW = result.mClutterPower;
            double clutterDBW    = UtMath::SafeLinearToDB(clutterPowerW);

            // output to file
            plotOfs << "      range " << groundRange * mRangeUnitsScale << " " << mRangeUnitsStr;
            const static double sMIN_CLUTTER = -360.0;
            plotOfs << "  clutter " << std::max(clutterDBW, sMIN_CLUTTER) << " dbw" << std::endl;
         }
      } // end of range loop
   }    // end of altitude loop

   plotOfs << "  end_clutters" << std::endl;
   plotOfs << "end_clutter_model" << std::endl;
   plotOfs.close();

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Clutter Table", mOutputFileName);

   return true;
}

bool ClutterTableFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "altitude")
   {
      PointArray pointArray;
      aInput.ReadValueOfType(pointArray.altitude, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(pointArray.altitude, 0.0);
      ProcessRangeBearingInput(aInput, "end_altitude", pointArray);
      mEnvelope.push_back(pointArray);
   }
   else if (command == "altitudes")
   {
      std::string word;
      double      minAlt;
      double      maxAlt;
      double      altStep;
      aInput.ReadValue(word);
      aInput.StringEqual(word, "from");
      aInput.ReadValueOfType(minAlt, UtInput::cLENGTH);
      aInput.ReadValue(word);
      aInput.StringEqual(word, "to");
      aInput.ReadValueOfType(maxAlt, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(minAlt, 0.0);
      aInput.ValueGreaterOrEqual(maxAlt, minAlt);
      aInput.ReadValue(word);
      aInput.StringEqual(word, "by");
      aInput.ReadValueOfType(altStep, UtInput::cLENGTH);
      aInput.ValueGreater(altStep, 0.0);

      PointArray pointArray;
      ProcessRangeBearingInput(aInput, "end_altitudes", pointArray);

      for (double alt = minAlt; alt <= (maxAlt + 1.0E-4); alt += altStep)
      {
         pointArray.altitude = alt;
         mEnvelope.push_back(pointArray);
      }
   }
   else if (command == "output_file_name")
   {
      aInput.ReadValueQuoted(mOutputFileName);
      mOutputFileName = aInput.SubstitutePathVariables(mOutputFileName);
   }
   else if (command == "output_object_name")
   {
      aInput.ReadValue(mOutputObjectName);
   }
   else if (command == "altitude_units")
   {
      aInput.ReadValue(mAltUnitsStr);
      mAltUnitsScale = aInput.ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH);
   }
   else if (command == "range_units")
   {
      aInput.ReadValue(mRangeUnitsStr);
      mRangeUnitsScale = aInput.ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH);
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
   else if (command == "sensor_platform_latitude")
   {
      aInput.ReadValueOfType(mSensorPlatformLat, UtInput::cLATITUDE);
   }
   else if (command == "sensor_platform_longitude")
   {
      aInput.ReadValueOfType(mSensorPlatformLon, UtInput::cLONGITUDE);
   }
   else if (command == "sensor_platform_altitude")
   {
      aInput.ReadValueOfType(mSensorPlatformAlt, UtInput::cLENGTH);
      mSensorPlatformAltSet = true;
   }
   else if (mSensor.ProcessInput(aInput))
   {
   }
   else if (mTarget.ProcessInput(aInput))
   {
   }
   else if (Function::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void ClutterTableFunction::ProcessRangeBearingInput(UtInput& aInput, const std::string& aBlockTerminator, PointArray& aPointArray)
{
   std::string  command;
   UtInputBlock inputBlock(aInput, aBlockTerminator);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "range")
      {
         Point point;
         aInput.ReadValueOfType(point.range, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(point.range, 0.0);
         aPointArray.data.push_back(point);
      }
      else if (command == "ranges")
      {
         Point       point;
         double      minRange;
         double      maxRange;
         double      rangeStep;
         std::string word;
         aInput.ReadValue(word);
         aInput.StringEqual(word, "from");
         aInput.ReadValueOfType(minRange, UtInput::cLENGTH);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "to");
         aInput.ReadValueOfType(maxRange, UtInput::cLENGTH);
         aInput.ValueGreater(maxRange, minRange);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "by");
         aInput.ReadValueOfType(rangeStep, UtInput::cLENGTH);
         aInput.ValueGreater(rangeStep, 0.0);
         for (point.range = minRange; point.range <= (maxRange + 1.0E-4); point.range += rangeStep)
         {
            aPointArray.data.push_back(point);
         }
      }
      else if (command == "bearings")
      {
         Point       point;
         double      minBearing;
         double      maxBearing;
         double      bearingStep;
         std::string word;
         aInput.ReadValue(word);
         aInput.StringEqual(word, "from");
         aInput.ReadValueOfType(minBearing, UtInput::cANGLE);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "to");
         aInput.ReadValueOfType(maxBearing, UtInput::cANGLE);
         aInput.ValueGreater(maxBearing, minBearing);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "by");
         aInput.ReadValueOfType(bearingStep, UtInput::cANGLE);
         aInput.ValueGreater(bearingStep, 0.0);
         for (double bearing = minBearing; bearing <= (maxBearing + 1.0E-4); bearing += bearingStep)
         {
            mBearings.push_back(bearing);
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // If no range values then attempt to inherit the values from the previous altitude.
   if (aPointArray.data.empty())
   {
      if (mEnvelope.empty())
      {
         throw UtInput::BadValue(aInput, "range values must be given for the first altitude");
      }
      aPointArray.data = mEnvelope.back().data;
   }

   // Place a dummy value for bearing if this is a generic table
   if (mBearings.empty())
   {
      mBearings.push_back(0.0);
   }
}
