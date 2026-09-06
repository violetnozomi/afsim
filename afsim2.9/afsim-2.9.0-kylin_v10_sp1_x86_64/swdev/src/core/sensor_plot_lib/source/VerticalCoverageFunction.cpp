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

#include "VerticalCoverageFunction.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
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

const double VerticalCoverageFunction::cUNDEFINED = 1.0E+30;

VerticalCoverageFunction::VerticalCoverageFunction(const WsfScenario& aScenario)
   : Function(aScenario)
   , mSimulationPtr(nullptr)
   , mOutputMaxHeight(false)
   , mOutputMaxRange(false)
   , mSensor()
   , mTarget(aScenario.GetAtmosphere())
   , mOutputBase()
   , mOutputFile()
   , mMaximumRange(0.0)
   , mLargeRangeStep(5000.0)
   , mSmallRangeStep(100.0)
   , mMinElevation(-5.0 * UtMath::cRAD_PER_DEG)
   , mMaxElevation(89.9 * UtMath::cRAD_PER_DEG)
   , mElevationStep(0.1 * UtMath::cRAD_PER_DEG)
   , mAltUnitsStr("m")
   , mRangeUnitsStr("m")
   , mX_UnitsStr("m")
   , mY_UnitsStr("m")
{
}

// virtual
bool VerticalCoverageFunction::Execute(WsfSimulation& aSimulation)
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

   double earthRadiusMultiplier = mSensor.GetEarthRadiusMultiplier();
   double earthRadius           = UtSphericalEarth::cEARTH_RADIUS;
   double scaledEarthRadius     = earthRadius * earthRadiusMultiplier;

   // Determine the depression angle, slant range and ground range to the horizon.

   double groundAngle        = acos(scaledEarthRadius / (scaledEarthRadius + sensorHeight));
   double horizonElevation   = -groundAngle;
   double horizonGroundRange = groundAngle * scaledEarthRadius;

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
      plotFileName = baseName + ".vcd";
   }

   std::ofstream plotOfs(plotFileName.c_str());

   double maxDetectSlantRange = 0.0;
   double maxDetectAltitude   = 0.0;

   WsfSensorResult result;

   double altUnitScale   = UtInput::ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH);
   double rangeUnitScale = UtInput::ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH);
   double xUnitScale     = UtInput::ConvertValueTo(1.0, mX_UnitsStr, UtInput::cLENGTH);
   double yUnitScale     = UtInput::ConvertValueTo(1.0, mY_UnitsStr, UtInput::cLENGTH);

   int intElevLastMsg = -9999;
   for (double elevation = mMinElevation; elevation <= (mMaxElevation + (0.1 * mElevationStep)); elevation += mElevationStep)
   {
      if (fabs(elevation) < 1.0E-10)
      {
         elevation = 0.0;
      }

      int intElev = static_cast<int>(floor(elevation * UtMath::cDEG_PER_RAD));
      if ((intElev - intElevLastMsg) >= 5)
      {
         ut::log::info() << "Processing elevation " << intElev;
         intElevLastMsg = intElev;
      }

      // The search is done in two steps.  We start at the maximum range and step in with
      // coarse increments until we get a hit.  Once we get a hit we back up 5 big
      // increments and search again using a fine increment.

      double range     = mMaximumRange;
      double rangeStep = mLargeRangeStep;
      if (elevation < horizonElevation)
      {
         range = static_cast<int>(2.0 * horizonGroundRange);
      }

      double detectedSlantRange  = 0.0;
      double detectedScaledX     = 0.0;
      double detectedScaledY     = sensorHeight;
      double detectedGroundRange = 0.0;
      double detectedAltitude    = sensorHeight;
      bool   done                = false;
      while (!done)
      {
         // Using the slant range and elevation angle in spherical Earth RADAR coordinates (e.g. 4/3 scaled)
         // compute the target altitude in unscaled spherical Earth coordinates that would result in the
         // apparent elevation angle.

         double slantRange = range;
         double antennaX   = 0.0;
         double antennaZ   = sensorHeight + scaledEarthRadius;
         double targetX    = antennaX + slantRange * cos(elevation);
         double targetZ    = antennaZ + slantRange * sin(elevation);

         double targetHeight = sqrt(targetX * targetX + targetZ * targetZ) - scaledEarthRadius;
         double altitude     = targetHeight;

         // Now having the slant range and target altitude, use the law of cosines to compute the
         // elevation angle in non-scaled earth coordinates.

         double sideA     = sensorHeight + earthRadius;
         double sideB     = targetHeight + earthRadius;
         double sideC     = slantRange;
         double cosAngleB = ((sideA * sideA) + (sideC * sideC) - (sideB * sideB)) / (2.0 * sideA * sideC);
         double angleB    = acos(std::min(std::max(cosAngleB, -1.0), 1.0));
         double tempElev  = angleB - UtMath::cPI_OVER_2;

         double newTargetX      = slantRange * cos(tempElev);
         double newTargetZ      = slantRange * sin(tempElev);
         double targetLocNED[3] = {newTargetX, 0.0, -(newTargetZ + sensorHeight - sensorAlt)};
         double targetLocWCS[3];
         mSensor.GetPlatform()->ConvertNEDToWCS(targetLocNED, targetLocWCS);
         mTarget.GetPlatform()->SetLocationWCS(targetLocWCS);

         // Get the approximate ground range for the output file.

         double mag1        = UtVec3d::Magnitude(sensorLocWCS);
         double mag2        = UtVec3d::Magnitude(targetLocWCS);
         double cosTheta    = UtVec3d::DotProduct(sensorLocWCS, targetLocWCS) / (mag1 * mag2);
         double theta       = acos(std::min(std::max(cosTheta, -1.0), 1.0));
         double groundRange = earthRadius * theta;

         // Set the speed and attitude of the target.
         mTarget.SetSpeedAndAttitude(mSensor);

         // Attempt to cue the sensor to the target, just in case the sensor is a tracker.
         mSensor.GetSensor()->SetCuedOrientation(0.0, elevation);

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
            detectedSlantRange  = slantRange;
            detectedScaledX     = targetX;
            detectedScaledY     = targetZ - scaledEarthRadius;
            detectedGroundRange = groundRange;
            detectedAltitude    = altitude;

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
      plotOfs << elevation * UtMath::cDEG_PER_RAD
              << '\t' << detectedSlantRange  * rangeUnitScale
              << '\t' << detectedScaledX     * xUnitScale
              << '\t' << detectedScaledY     * yUnitScale
              << '\t' << detectedGroundRange * rangeUnitScale
              << '\t' << detectedAltitude    * altUnitScale
              << std::endl;
      // clang-format on
      maxDetectSlantRange = std::max(maxDetectSlantRange, detectedSlantRange);
      maxDetectAltitude   = std::max(maxDetectAltitude, detectedAltitude);
   }

   mSimulationPtr->GetSystemLog().WriteOutputLogEntry("Vertical Coverage", plotFileName);

   if (mOutputMaxHeight)
   {
      ut::log::info() << "Maximum Detected Altitude: " << maxDetectAltitude;
   }

   if (mOutputMaxRange)
   {
      ut::log::info() << "Maximum Detected Slant Range: " << maxDetectSlantRange;
   }

   return true;
}

bool VerticalCoverageFunction::ProcessInput(UtInput& aInput)
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
   else if (command == "output_max_height")
   {
      mOutputMaxHeight = true;
   }
   else if (command == "output_max_range")
   {
      mOutputMaxRange = true;
   }
   else if (command == "altitude_units")
   {
      aInput.ReadValue(mAltUnitsStr);
      aInput.ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH); // Validates the string
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
   else if (command == "elevation_limits")
   {
      aInput.ReadValueOfType(mMinElevation, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxElevation, UtInput::cANGLE);
      aInput.ValueGreater(mMinElevation, -UtMath::cPI_OVER_2);
      aInput.ValueLess(mMaxElevation, UtMath::cPI_OVER_2);
      aInput.ValueGreaterOrEqual(mMaxElevation, mMinElevation);
   }
   else if (command == "elevation_step")
   {
      aInput.ReadValueOfType(mElevationStep, UtInput::cANGLE);
      aInput.ValueGreater(mElevationStep, 0.0);
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
