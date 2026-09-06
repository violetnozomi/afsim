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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <string>

class WsfPlatform;
class WsfScenario;
class WsfSimulation;

#include "TargetConfig.hpp"

namespace engage
{

class RunConfig;

//! This class in the interface between WSF platforms and ENGAGE players.
class Platform
{
public:
   Platform(const RunConfig& aRunConfig, WsfSimulation& aSimulation, const std::string& aType, const std::string& aName);

   Platform(const RunConfig& aRunConfig, WsfPlatform* aPlatformPtr);

   bool AddToSimulation(double aSimTime, WsfSimulation& aSimulation);

   void Update(double aSimTime);

   void SetFlightPath(TargetConfig::FlightPathVec aFlightPath);

   bool         IsValid() const;
   WsfPlatform* GetWsfPlatform() const;

   double GetLocationX()
   {
      UpdateLocationXYZ();
      return mLocationXYZ[0];
   }
   double GetLocationY()
   {
      UpdateLocationXYZ();
      return mLocationXYZ[1];
   }
   double GetLocationZ()
   {
      UpdateLocationXYZ();
      return mLocationXYZ[2];
   }

   double GetVelocityX()
   {
      UpdateVelocityXYZ();
      return mVelocityXYZ[0];
   }
   double GetVelocityY()
   {
      UpdateVelocityXYZ();
      return mVelocityXYZ[1];
   }
   double GetVelocityZ()
   {
      UpdateVelocityXYZ();
      return mVelocityXYZ[2];
   }

   double GetAccelerationX()
   {
      UpdateAccelerationXYZ();
      return mAccelerationXYZ[0];
   }
   double GetAccelerationY()
   {
      UpdateAccelerationXYZ();
      return mAccelerationXYZ[1];
   }
   double GetAccelerationZ()
   {
      UpdateAccelerationXYZ();
      return mAccelerationXYZ[2];
   }

   void SetLocationXYZ(double aX, double aY, double aZ);

   void SetSpeedAndOrientationXYZ(double aSpeed, double aHeading, double aPitch, double aRoll);

private:
   void UpdateValidityFlags();
   void UpdateLocationXYZ();
   void UpdateVelocityXYZ();
   void UpdateAccelerationXYZ();

   const RunConfig& mRunConfig;
   WsfSimulation&   mSimulation;
   WsfPlatform*     mWsfPlatformPtr;
   size_t           mWsfPlatformIndex;

   //! Validity flags.
   union
   {
      struct
      {
         bool mLocationXYZ_Valid : 1;
         bool mVelocityXYZ_Valid : 1;
         bool mAccelerationXYZ_Valid : 1;
      };
      unsigned int mFlags;
   };

   double mLastUpdateTime;

   double mLocationXYZ[3];
   double mVelocityXYZ[3];
   double mAccelerationXYZ[3];

   TargetConfig::FlightPathVec mFlightPath;
   unsigned int                mFlightPathIndex;
};

} // namespace engage

#endif
