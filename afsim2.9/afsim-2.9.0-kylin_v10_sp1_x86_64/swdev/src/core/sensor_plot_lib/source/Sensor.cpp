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

#include "Sensor.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "Target.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "Utility.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
Sensor::Sensor()
   : mPlatformType("SENSOR_PLATFORM_TYPE")
   , mModeName()
   , mAutomaticTargetCueing(true)
   , mPlatformPtr(nullptr)
   , mSensorPtr(nullptr)
   , mHeight(0.0)
   , mMaximumRange(0.0)
   , mEarthRadiusMultiplier(0.0)
   , mSettings()
   , mSimTime(0.0)
{
   mSettings.mRequiredPd = 0.5;
}

// =================================================================================================
Sensor::~Sensor()
{
   // The platform is not deleted here because it is owned by the simulation
}

// =================================================================================================
bool Sensor::AttemptToDetect(Target& aTarget, WsfSensorResult& aResult)
{
   return AttemptToDetect(aTarget.GetPlatform(), aResult);
}

// =================================================================================================
bool Sensor::AttemptToDetect(WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   // Ensure that anything that is event driven gets dispatched.
   while (mPlatformPtr->GetSimulation()->AdvanceTime(mSimTime) < mSimTime)
   {
   }

   bool detected = mSensorPtr->AttemptToDetect(mSimTime, aTargetPtr, mSettings, aResult);
   mSimTime += std::max(0.1, mSensorPtr->GetUpdateInterval());
   return detected;
}

// =================================================================================================
//! Given the ground range, bearing relative to the sensor and altitude, return the WCS location.
//! @note Bearing is radians clockwise from north!.
void Sensor::ConvertRBA_ToWCS(double aGroundRange, double aBearing, double aAltitude, double aLocationWCS[3])
{
   double sensorLat;
   double sensorLon;
   double sensorAlt;
   mPlatformPtr->GetLocationLLA(sensorLat, sensorLon, sensorAlt);

   // Process the trivial case.
   if (aGroundRange < 1.0)
   {
      WsfPlatform::ConvertLLAToWCS(sensorLat, sensorLon, aAltitude, aLocationWCS);
      return;
   }

   // The following method is more difficult than simply assuming a spherical Earth, but gives
   // more exact results when comparing against codes with spherical/flat earth models. The overall
   // errors of the simpler methods aren't significant, but this produces a better match.

   // For the purposes of matching detections in various models, what we REALLY care about is the
   // slant range and elevation angle.

   // Given the ground range and altitude, calculate the slant range on the scaled earth.

   double earthRadius = UtSphericalEarth::cEARTH_RADIUS;
   double re          = earthRadius * mEarthRadiusMultiplier;
   double sideA       = re + sensorAlt;
   double sideB       = re + aAltitude;
   double angleC      = aGroundRange / re;
   double sideC       = sqrt((sideA * sideA) + (sideB * sideB) - (2.0 * sideA * sideB * cos(angleC)));
   double slantRange  = sideC;

   // Given the slant range and altitude, compute the elevation angle on the unscaled earth.

   re               = earthRadius;
   sideA            = re + sensorAlt;
   sideB            = re + aAltitude;
   double cosAngleB = ((sideA * sideA) + (sideC * sideC) - (sideB * sideB)) / (2.0 * sideA * sideC);
   double angleB    = acos(std::min(std::max(cosAngleB, -1.0), 1.0));
   double elevation = angleB - UtMath::cPI_OVER_2;

   double locationNED[3];
   double sinEl   = sin(elevation);
   double cosEl   = cos(elevation);
   locationNED[0] = slantRange * cosEl * cos(aBearing);
   locationNED[1] = slantRange * cosEl * sin(aBearing);
   locationNED[2] = -slantRange * sinEl;
   mPlatformPtr->ConvertNEDToWCS(locationNED, aLocationWCS);
}

// =================================================================================================
//! Given a WCS location, return the equivalent ground range, bearing relative to the sensor and altitude.
//! @note Bearing is radians clockwise from north!.
void Sensor::ConvertWCS_ToRBA(const double aLocationWCS[3], double& aGroundRange, double& aBearing, double& aAltitude)
{
   double sensorLat;
   double sensorLon;
   double sensorAlt;
   mPlatformPtr->GetLocationLLA(sensorLat, sensorLon, sensorAlt);

   double locationLLA[3];
   mPlatformPtr->ConvertWCSToLLA(aLocationWCS, locationLLA[0], locationLLA[1], locationLLA[2]);

   double deltaLocationWCS[3];
   mPlatformPtr->GetRelativeLocationWCS(aLocationWCS, deltaLocationWCS);

   double earthRadius = UtSphericalEarth::cEARTH_RADIUS;
   double re          = earthRadius;
   double sideA       = re + sensorAlt;
   double sideB       = re + locationLLA[2];
   double sideC       = UtVec3d::Magnitude(deltaLocationWCS);

   double cosAngleB = ((sideA * sideA) + (sideC * sideC) - (sideB * sideB)) / (2.0 * sideA * sideC);
   double angleB    = acos(std::min(std::max(cosAngleB, -1.0), 1.0));
   double elevation = angleB - UtMath::cPI_OVER_2;

   double cosAngleC = ((sideA * sideA) + (sideB * sideB) - (sideC * sideC)) / (2.0 * sideA * sideB);
   double angleC    = acos(std::min(std::max(cosAngleC, -1.0), 1.0));

   aAltitude    = locationLLA[2];
   aGroundRange = re * mEarthRadiusMultiplier * angleC;

   double orientationNED[3];
   mPlatformPtr->GetOrientationNED(orientationNED[0], orientationNED[1], orientationNED[2]);

   double locationNED[3];
   mPlatformPtr->ConvertWCSToNED(aLocationWCS, locationNED);
   double cosAbsBearing = locationNED[0] / (sideC * cos(elevation));
   aBearing             = acos(std::min(std::max(cosAbsBearing, -1.0), 1.0)) - orientationNED[0];
}

// =================================================================================================
//! Create a local platform that has the sensor under test.
bool Sensor::CreateAndInitialize(WsfSimulation& aSimulation)
{
   mPlatformPtr = WsfPlatformTypes::Get(aSimulation.GetScenario()).Clone(mPlatformType);
   if (mPlatformPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find sensor platform type.";
      out.AddNote() << "Type: " << mPlatformType;
      return false;
   }
   mPlatformPtr->SetName("SENSOR");

   // Find the sensor on the source platform

   mSensorPtr = mPlatformPtr->GetComponent<WsfSensor>(WsfStringId("TEST_SENSOR"));
   if (mSensorPtr == nullptr)
   {
      unsigned int sensorCount = mPlatformPtr->GetComponentCount<WsfSensor>();
      if (sensorCount == 0)
      {
         auto out = ut::log::error() << "No sensors defined on platform type.";
         out.AddNote() << "Type: " << mPlatformType;
         return false;
      }
      mSensorPtr = mPlatformPtr->GetComponentEntry<WsfSensor>(0);
      if (sensorCount > 1)
      {
         auto out = ut::log::warning() << "More than one sensor on platform type, but TEST_SENSOR is not specified.";
         out.AddNote() << "Platform Type: " << mPlatformType;
         auto note = out.AddNote() << "Using Sensor: " << mSensorPtr->GetName();
         note.AddNote() << "Sensor Type: " << mSensorPtr->GetType();
      }
   }

   // Ensure that the sensor gets turned-on when it is created.
   mSensorPtr->SetInitiallyTurnedOn(true);

   // Add the platform to the simulation.
   // Many sensors do not actually require that it be part of the simulation, but some do

   if (!aSimulation.AddPlatform(0.0, mPlatformPtr))
   {
      { // RAII block
         auto out = ut::log::error() << "Unable to add target platform to the simulation.";
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
      }
      delete mPlatformPtr;
      mPlatformPtr = nullptr;
      return false;
   }

   // If a specific mode was selected then get the index of the mode.
   // If a specific mode was not selected and the sensor supports modes, use the
   // 'initial_mode' from the sensor. Otherwise just use the first mode.

   size_t modeIndex = 0;
   if (!mModeName.empty())
   {
      modeIndex = mSensorPtr->GetModeIndex(mModeName);
      if (modeIndex >= mSensorPtr->GetModeCount())
      {
         auto out = ut::log::error() << "Mode does not exist on sensor.";
         out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << mSensorPtr->GetName();
         out.AddNote() << "Mode: " << mModeName;

         return false;
      }

      // Also select the mode. Once again, some sensors actually require the mode to be selected.
      mSensorPtr->SelectMode(0.0, mModeName);
   }
   else
   {
      // Explicit mode not specified - use the initial/current mode of the sensor if it supports modes.
      WsfSensorMode* modePtr = mSensorPtr->GetCurrentMode();
      if (modePtr != nullptr)
      {
         modeIndex = modePtr->GetModeIndex();
      }
   }
   mSettings.mModeIndex = modeIndex;

   // Find the maximum height and range of any transmitter or receiver on the sensor.

   double sensorOffset[3];
   mSensorPtr->GetLocation(sensorOffset);
   mHeight       = -sensorOffset[2];
   mMaximumRange = 0.0;
   for (unsigned int i = 0; i < mSensorPtr->GetEM_XmtrCount(); ++i)
   {
      WsfEM_Antenna* antennaPtr = mSensorPtr->GetEM_Xmtr(i).GetAntenna();
      if (antennaPtr != nullptr)
      {
         mHeight       = std::max(mHeight, antennaPtr->GetHeight());
         mMaximumRange = std::max(mMaximumRange, std::min(antennaPtr->GetMaximumRange(), 1.0E+10));
      }
   }
   for (unsigned int i = 0; i < mSensorPtr->GetEM_RcvrCount(); ++i)
   {
      WsfEM_Antenna* antennaPtr = mSensorPtr->GetEM_Rcvr(i).GetAntenna();
      if (antennaPtr != nullptr)
      {
         mHeight       = std::max(mHeight, antennaPtr->GetHeight());
         mMaximumRange = std::max(mMaximumRange, std::min(antennaPtr->GetMaximumRange(), 1.0E+10));
      }
   }

   mEarthRadiusMultiplier = 1.0;
   if (mSensorPtr->GetEM_XmtrCount() > 0)
   {
      mEarthRadiusMultiplier = mSensorPtr->GetEM_Xmtr(0).GetEarthRadiusMultiplier();
   }
   else if (mSensorPtr->GetEM_RcvrCount() > 0)
   {
      mEarthRadiusMultiplier = mSensorPtr->GetEM_Rcvr(0).GetEarthRadiusMultiplier();
   }

   // Force one update of the platform to cause the mover to set its initial kinematic state and then delete the mover.
   // The sensors should not move will advancing through time, but may require velocity for Doppler computations.

   mPlatformPtr->Update(0.0);
   mPlatformPtr->SetMover(nullptr);
   return true;
}

// =================================================================================================
void Sensor::CueToTarget(Target& aTarget)
{
   CueToTarget(aTarget.GetPlatform());
}

// =================================================================================================
void Sensor::CueToTarget(WsfPlatform* aTargetPtr)
{
   if (mAutomaticTargetCueing)
   {
      Utility::CueSensorToTarget(mSensorPtr, aTargetPtr);
   }
}

// =================================================================================================
bool Sensor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "sensor_platform_type")
   {
      aInput.ReadValue(mPlatformType);
   }
   else if (command == "mode_name")
   {
      aInput.ReadValue(mModeName);
   }
   else if (command == "required_pd")
   {
      double requiredPd;
      aInput.ReadValue(requiredPd);
      aInput.ValueGreaterOrEqual(requiredPd, 0.0);
      aInput.ValueLess(requiredPd, 1.0);
      mSettings.mRequiredPd = requiredPd;
   }
   else if (command == "automatic_target_cueing")
   {
      aInput.ReadValue(mAutomaticTargetCueing);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}
