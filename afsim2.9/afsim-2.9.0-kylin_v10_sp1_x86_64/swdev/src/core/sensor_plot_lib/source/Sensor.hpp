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

#ifndef SENSOR_HPP
#define SENSOR_HPP

#include <string>

class Target;
class UtInput;
class WsfPlatform;
#include "WsfSensor.hpp"

//! An interface class for the sensor under test.
//!
//! This class was created because many of the available test functions were repeating code.
//!
//! @note This class assumes that the sensor does not move during the test!!! If this is ever
//! changed the ConvertRB_ToLL must be changed!.

class Sensor
{
public:
   Sensor();
   ~Sensor();

   bool AttemptToDetect(Target& aTarget, WsfSensorResult& aResult);

   bool AttemptToDetect(WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   void ConvertRBA_ToWCS(double aGroundRange, double aBearing, double aAltitude, double aLocationWCS[3]);

   void ConvertWCS_ToRBA(const double aLocationWCS[3], double& aGroundRange, double& aBearing, double& aAltitude);

   bool CreateAndInitialize(WsfSimulation& aSimulation);

   void CueToTarget(Target& aTarget);
   void CueToTarget(WsfPlatform* aTargetPtr);
   void SetAutomaticTargetCueing(bool aAutomaticTargetCueing) { mAutomaticTargetCueing = aAutomaticTargetCueing; }

   bool ProcessInput(UtInput& aInput);

   WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   WsfSensor*   GetSensor() const { return mSensorPtr; }
   double       GetSimTime() const { return mSimTime; }

   double               GetEarthRadiusMultiplier() const { return mEarthRadiusMultiplier; }
   double               GetHeight() const { return mHeight; }
   double               GetMaximumRange() const { return mMaximumRange; }
   WsfSensor::Settings& GetSettings() { return mSettings; }

private:
   std::string mPlatformType;
   std::string mModeName;
   bool        mAutomaticTargetCueing;

   WsfPlatform* mPlatformPtr;
   WsfSensor*   mSensorPtr;
   double       mHeight;
   double       mMaximumRange;
   double       mEarthRadiusMultiplier;

   WsfSensor::Settings mSettings;
   double              mSimTime;
};

#endif
