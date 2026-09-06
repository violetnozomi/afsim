// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "Platform.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "InputConfig.hpp"
#include "RunConfig.hpp"
#include "UtVec3.hpp"
#include "Utilities.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace engage
{

// =================================================================================================
// Constructor to clone a platform from a platform type.
Platform::Platform(const RunConfig& aRunConfig, WsfSimulation& aSimulation, const std::string& aType, const std::string& aName)
   : mRunConfig(aRunConfig)
   , mSimulation(aSimulation)
   , mWsfPlatformPtr(nullptr)
   , mWsfPlatformIndex(0)
   , mFlags(0)
   , mLastUpdateTime(-1.0)
   , mFlightPath()
   , mFlightPathIndex(0)
{
   const WsfScenario& scenario = aSimulation.GetScenario();
   mWsfPlatformPtr             = WsfPlatformTypes::Get(scenario).Clone(aType);
   if (mWsfPlatformPtr != nullptr)
   {
      mWsfPlatformPtr->SetName(aName);
   }
   else
   {
      std::cout << "***** ERROR: Unable to create platform of type " << aType << std::endl;
   }
}

// =================================================================================================
// Constructor to create a platform given a standard WSF platform.
Platform::Platform(const RunConfig& aRunConfig, WsfPlatform* aWsfPlatformPtr)
   : mRunConfig(aRunConfig)
   , mSimulation(*(aWsfPlatformPtr->GetSimulation()))
   , mWsfPlatformPtr(aWsfPlatformPtr)
   , mWsfPlatformIndex(aWsfPlatformPtr->GetIndex())
   , mFlags(0)
   , mLastUpdateTime(aWsfPlatformPtr->GetLastUpdateTime())
{
}

// =================================================================================================
bool Platform::AddToSimulation(double aSimTime, WsfSimulation& aSimulation)
{
   bool ok = false;
   if (aSimulation.AddPlatform(0.0, mWsfPlatformPtr))
   {
      mWsfPlatformIndex = mWsfPlatformPtr->GetIndex();
      ok                = true;
   }
   else if (mWsfPlatformPtr != nullptr)
   {
      std::cout << "***** ERROR: Unable to add platform " << mWsfPlatformPtr->GetName() << " to simulation" << std::endl;
      delete mWsfPlatformPtr;
      mWsfPlatformPtr = nullptr;
   }
   return ok;
}

void Platform::Update(double aSimTime)
{
   if (mFlightPath.empty())
   {
      return;
   }

   if (mFlightPathIndex < (mFlightPath.size() - 1))
   {
      double nextTime = mFlightPath[mFlightPathIndex + 1].mTime;
      if (aSimTime >= nextTime)
      {
         mFlightPathIndex = std::min(mFlightPathIndex + 1, (unsigned int)mFlightPath.size() - 1);
      }
   }

   double vX    = mFlightPath[mFlightPathIndex].mVelXYZ[0];
   double vY    = mFlightPath[mFlightPathIndex].mVelXYZ[1];
   double vZ    = mFlightPath[mFlightPathIndex].mVelXYZ[2];
   double speed = UtVec3d::Magnitude(mFlightPath[mFlightPathIndex].mVelXYZ);

   double deltaT = aSimTime - mFlightPath[mFlightPathIndex].mTime;
   double x      = mFlightPath[mFlightPathIndex].mPosXYZ[0] + deltaT * vX;
   double y      = mFlightPath[mFlightPathIndex].mPosXYZ[1] + deltaT * vY;
   double z      = mFlightPath[mFlightPathIndex].mPosXYZ[2] + deltaT * vZ;

   double heading = mFlightPath[mFlightPathIndex].mYaw;
   double pitch   = mFlightPath[mFlightPathIndex].mPitch;
   double roll    = mFlightPath[mFlightPathIndex].mRoll;

   SetLocationXYZ(x, y, z);
   SetSpeedAndOrientationXYZ(speed, heading, pitch, roll);
}

void Platform::SetFlightPath(TargetConfig::FlightPathVec aFlightPath)
{
   mFlightPath = aFlightPath;
}

// =================================================================================================
bool Platform::IsValid() const
{
   bool isValid = true;
   if (mWsfPlatformIndex != 0) // Before addition to simulation
   {
      isValid = mSimulation.PlatformExists(mWsfPlatformIndex);
   }
   else
   {
      isValid = (mWsfPlatformPtr != nullptr);
   }
   return isValid;
}

// =================================================================================================
WsfPlatform* Platform::GetWsfPlatform() const
{
   return (mSimulation.PlatformExists(mWsfPlatformIndex) ? mWsfPlatformPtr : nullptr);
}

// =================================================================================================
void Platform::SetLocationXYZ(double aX, double aY, double aZ)
{
   if (mWsfPlatformPtr != nullptr)
   {
      double           lat;
      double           lon;
      double           alt(aZ);
      const Utilities& utilities(mRunConfig.GetUtilities());
      utilities.ConvertXY_ToLL(aX, aY, lat, lon);
      mWsfPlatformPtr->SetLocationLLA(lat, lon, alt);
   }
}

// =================================================================================================
void Platform::SetSpeedAndOrientationXYZ(double aSpeed, double aHeading, double aPitch, double aRoll)
{
   if (mWsfPlatformPtr != nullptr)
   {
      double heading(aHeading);
      double pitch(aPitch);
      double roll(aRoll);
      double velocityNED[3] = {aSpeed * cos(heading), aSpeed * sin(heading), 0.0};
      mWsfPlatformPtr->SetVelocityNED(velocityNED);
      mWsfPlatformPtr->SetOrientationNED(heading, pitch, roll);
      double accelerationNED[3] = {0.0, 0.0, 0.0};
      mWsfPlatformPtr->SetAccelerationNED(accelerationNED);
   }
}

// =================================================================================================
void Platform::UpdateValidityFlags()
{
   if (mSimulation.PlatformExists(mWsfPlatformIndex))
   {
      if (mLastUpdateTime != mWsfPlatformPtr->GetLastUpdateTime())
      {
         mFlags          = 0;
         mLastUpdateTime = mWsfPlatformPtr->GetLastUpdateTime();
      }
   }
   else
   {
      // If the platform is no longer valid then force the validity flags to true so the last value
      // computed is returned.
      mFlags = 0xFFFF;
   }
}

// =================================================================================================
void Platform::UpdateLocationXYZ()
{
   UpdateValidityFlags();
   if (!mLocationXYZ_Valid)
   {
      double lat;
      double lon;
      mWsfPlatformPtr->GetLocationLLA(lat, lon, mLocationXYZ[2]);
      const Utilities& utilities(mRunConfig.GetUtilities());
      utilities.ConvertLL_ToXY(lat, lon, mLocationXYZ[0], mLocationXYZ[1]);
      mLocationXYZ_Valid = true;
   }
}

// =================================================================================================
void Platform::UpdateVelocityXYZ()
{
   UpdateValidityFlags();
   if (!mVelocityXYZ_Valid)
   {
      double velocityNED[3];
      mWsfPlatformPtr->GetVelocityNED(velocityNED);
      mVelocityXYZ[0]    = velocityNED[1];
      mVelocityXYZ[1]    = -velocityNED[0];
      mVelocityXYZ[2]    = -velocityNED[2];
      mVelocityXYZ_Valid = true;
   }
}

// =================================================================================================
void Platform::UpdateAccelerationXYZ()
{
   UpdateValidityFlags();
   if (!mAccelerationXYZ_Valid)
   {
      double accelerationNED[3];
      mWsfPlatformPtr->GetAccelerationNED(accelerationNED);
      mAccelerationXYZ[0]    = accelerationNED[1];
      mAccelerationXYZ[1]    = -accelerationNED[0];
      mAccelerationXYZ[2]    = -accelerationNED[2];
      mAccelerationXYZ_Valid = true;
   }
}

} // namespace engage
