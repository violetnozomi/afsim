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

#include "HorizontalCoverageFunction.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"

const double HorizontalCoverageFunction::cUNDEFINED = 1.0E+30;

HorizontalCoverageFunction::HorizontalCoverageFunction(WsfScenario& aScenario)
   : Function(aScenario)
   , mSimulationPtr(nullptr)
   , mOutputMinRange(false)
   , mOutputMaxRange(false)
   , mSensor()
   , mTarget(aScenario.GetAtmosphere())
   , mOutputBase()
   , mOutputFile()
   , mMinimumRange(0.0)
   , mMaximumRange(0.0)
   , mLargeRangeStep(5000.0)
   , mSmallRangeStep(100.0)
   , mMinAzimuth(-180.0 * UtMath::cRAD_PER_DEG)
   , mMaxAzimuth(180.0 * UtMath::cRAD_PER_DEG)
   , mAzimuthStep(0.1 * UtMath::cRAD_PER_DEG)
   , mAltValues()
   , mAltUnitsStr("m")
   , mRangeUnitsStr("m")
   , mX_UnitsStr("m")
   , mY_UnitsStr("m")
{
}

// virtual
bool HorizontalCoverageFunction::Execute(WsfSimulation& aSimulation)
{
   Function::Execute(aSimulation);
   mSimulationPtr = &aSimulation;
   if ((!mSensor.CreateAndInitialize(aSimulation)) || (!mTarget.CreateAndInitialize(aSimulation)))
   {
      return false;
   }

   // Set the range limit to the user specified value, the sensor value or 2000 km.
   if (mMaximumRange <= 0.0)
   {
      mMaximumRange = 2.0E+6;
      if (mSensor.GetMaximumRange() < 1.0E+9)
      {
         mMaximumRange = 1.5 * mSensor.GetMaximumRange();
      }
   }

   double sensorLat = 0.0;
   double sensorLon = 0.0;
   double sensorAlt = 0.0;
   mSensor.GetPlatform()->GetLocationLLA(sensorLat, sensorLon, sensorAlt);
   double sensorLocWCS[3];
   mSensor.GetPlatform()->GetLocationWCS(sensorLocWCS);

   // From here on 'sensorHeight' is the height of the sensor origin above the Earth's surface.
   double sensorHeight = mSensor.GetHeight() + sensorAlt;

   // Determine the 'scaled' Earth radius.
   double scaledEarthRadius = UtSphericalEarth::cEARTH_RADIUS * mSensor.GetEarthRadiusMultiplier();

   // Generate the base name for output files.  If one hasn't been defined then we'll use the
   // sensor type name.
   std::string plotFileName(mOutputFile);
   if (plotFileName.empty())
   {
      std::string baseName(mOutputBase);
      if (baseName.empty())
      {
         baseName = mSensor.GetSensor()->GetType();
      }
      plotFileName = baseName + ".hcd";
   }

   std::ofstream plotOfs(plotFileName.c_str());

   double minDetectSlantRange = 0.0;
   double maxDetectSlantRange = 0.0;

   WsfSensorResult result;

   double altUnitScale   = UtInput::ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH);
   double rangeUnitScale = UtInput::ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH);
   double xUnitScale     = UtInput::ConvertValueTo(1.0, mX_UnitsStr, UtInput::cLENGTH);
   double yUnitScale     = UtInput::ConvertValueTo(1.0, mY_UnitsStr, UtInput::cLENGTH);

   for (size_t altIndex = 0; altIndex < mAltValues.size(); ++altIndex)
   {
      double altitude = mAltValues[altIndex];

      ut::log::info() << "Processing altitude: " << altitude;
      // plotOfs << "    altitude " << altitude * altUnitScale << " " << mAltUnitsStr << endl;

      // Determine the maximum scaled ground range to the target based on horizon masking.
      double horizonGroundRange = sqrt(2.0 * scaledEarthRadius * sensorHeight) + sqrt(2.0 * scaledEarthRadius * altitude);

      int intAzimuthLastMsg = -9999;
      for (double azimuth = mMinAzimuth; azimuth <= (mMaxAzimuth + (0.1 * mAzimuthStep)); azimuth += mAzimuthStep)
      {
         if (fabs(azimuth) < 1.0E-10)
         {
            azimuth = 0.0;
         }

         int intAzimuth = static_cast<int>(floor(azimuth * UtMath::cDEG_PER_RAD));
         if ((intAzimuth - intAzimuthLastMsg) >= 5)
         {
            ut::log::info() << "Processing azimuth: " << intAzimuth;
            intAzimuthLastMsg = intAzimuth;
         }

         // The search is done in two steps.  We start at the maximum range and step in with
         // coarse increments until we get a hit.  Once we get a hit we back up 5 big
         // increments and search again using a fine increment.

         double range     = mMaximumRange;
         double rangeStep = mLargeRangeStep;
         if (range < horizonGroundRange)
         {
            range = static_cast<int>(2.0 * horizonGroundRange);
         }

         double detectedSlantRange  = 0.0;
         double detectedScaledX     = 0.0;
         double detectedScaledY     = 0.0;
         double detectedX           = 0.0;
         double detectedY           = 0.0;
         double detectedGroundRange = 0.0;
         bool   done                = false;
         while (!done)
         {
            mTarget.SetLocationRBA(mSensor, range, azimuth, altitude);

            // Set the speed and attitude of the target.
            mTarget.SetSpeedAndAttitude(mSensor);

            // Attempt to cue the sensor to the target, just in case the sensor is a tracker.
            mSensor.CueToTarget(mTarget);

            // Perform the detection attempt.
            bool detected = mSensor.AttemptToDetect(mTarget, result);
            if (!detected)
            {
               range -= rangeStep;
               if (range <= 0)
               {
                  if (rangeStep == mSmallRangeStep)
                  {
                     done = true;
                  }
                  else
                  {
                     range     = 5 * mLargeRangeStep;
                     rangeStep = mSmallRangeStep;
                  }
               }
            }
            else
            {
               // We have a detection.  If doing the coarse step then switch to fine step.
               // If doing the fine step then we're done.

               detected            = true;
               detectedSlantRange  = result.mRcvrToTgt.mRange;
               detectedScaledX     = detectedSlantRange * cos(azimuth);
               detectedScaledY     = detectedSlantRange * sin(azimuth);
               detectedGroundRange = range;
               detectedX           = detectedGroundRange * cos(azimuth);
               detectedY           = detectedGroundRange * sin(azimuth);

               if (rangeStep == mLargeRangeStep)
               {
                  // We're on the big step, so back up a little and switch to the small step.
                  range     = range + (5 * mLargeRangeStep);
                  rangeStep = mSmallRangeStep;
               }
               else
               {
                  // We're on the small step so we are done.
                  done = true;
               }
            }
         }

         // clang-format off
         plotOfs << azimuth * UtMath::cDEG_PER_RAD
                 << '\t' << detectedSlantRange * rangeUnitScale
                 << '\t' << detectedScaledX * xUnitScale
                 << '\t' << detectedScaledY * yUnitScale
                 << '\t' << detectedX * xUnitScale
                 << '\t' << detectedY * yUnitScale
                 << '\t' << detectedGroundRange * rangeUnitScale
                 << '\t' << altitude * altUnitScale
                 << std::endl;
         // clang-format on

         minDetectSlantRange = std::min(minDetectSlantRange, detectedSlantRange);
         maxDetectSlantRange = std::max(maxDetectSlantRange, detectedSlantRange);
      }
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Horizontal Coverage", plotFileName);

   if (mOutputMinRange)
   {
      ut::log::info() << " Minimum Detected Slant Range: " << minDetectSlantRange;
   }

   if (mOutputMaxRange)
   {
      ut::log::info() << " Maximum Detected Slant Range: " << maxDetectSlantRange;
   }

   return true;
}

bool HorizontalCoverageFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "output_base")
   {
      aInput.ReadValueQuoted(mOutputBase);
      mOutputBase = aInput.SubstitutePathVariables(mOutputBase);
   }
   else if (command == "output_file")
   {
      aInput.ReadValueQuoted(mOutputFile);
      mOutputFile = aInput.SubstitutePathVariables(mOutputFile);
   }
   else if (command == "output_min_range")
   {
      mOutputMinRange = true;
   }
   else if (command == "output_max_range")
   {
      mOutputMaxRange = true;
   }
   else if (command == "altitude")
   {
      std::string word;
      aInput.ReadValue(word);
      if (word == "from")
      {
         double alt1;
         double alt2;
         double altStep;
         aInput.ReadValueOfType(alt1, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(alt1, 0.0);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "to");
         aInput.ReadValueOfType(alt2, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(alt2, alt1);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "by");
         aInput.ReadValueOfType(altStep, UtInput::cLENGTH);
         aInput.ValueGreater(altStep, 0.0);

         for (double alt = alt1; alt <= (alt2 + (0.1 * altStep)); alt += altStep)
         {
            mAltValues.push_back(alt);
         }
      }
      else
      {
         double alt;
         aInput.PushBack(word);
         aInput.ReadValueOfType(alt, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(alt, 0.0);
         mAltValues.push_back(alt);
      }
   }
   else if (command == "altitude_units")
   {
      aInput.ReadValue(mAltUnitsStr);
      UtInput::ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH); // Validate the specification
   }
   else if (command == "range_units")
   {
      aInput.ReadValue(mRangeUnitsStr);
      aInput.ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH); // Validates the string
   }
   else if (command == "x_units")
   {
      aInput.ReadValue(mX_UnitsStr);
      aInput.ConvertValueTo(1.0, mX_UnitsStr, UtInput::cLENGTH); // Validates the string
   }
   else if (command == "y_units")
   {
      aInput.ReadValue(mY_UnitsStr);
      aInput.ConvertValueTo(1.0, mY_UnitsStr, UtInput::cLENGTH); // Validates the string
   }
   else if (command == "maximum_range")
   {
      aInput.ReadValueOfType(mMaximumRange, UtInput::cLENGTH);
      aInput.ValueGreater(mMaximumRange, 0.0);
   }
   else if (command == "large_range_step")
   {
      aInput.ReadValueOfType(mLargeRangeStep, UtInput::cLENGTH);
      aInput.ValueGreater(mLargeRangeStep, 0.0);
   }
   else if (command == "small_range_step")
   {
      aInput.ReadValueOfType(mSmallRangeStep, UtInput::cLENGTH);
      aInput.ValueGreater(mSmallRangeStep, 0.0);
   }
   else if (command == "azimuth_limits")
   {
      aInput.ReadValueOfType(mMinAzimuth, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxAzimuth, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMaxAzimuth, mMinAzimuth);
   }
   else if (command == "azimuth_step")
   {
      aInput.ReadValueOfType(mAzimuthStep, UtInput::cANGLE);
      aInput.ValueGreater(mAzimuthStep, 0.0);
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
