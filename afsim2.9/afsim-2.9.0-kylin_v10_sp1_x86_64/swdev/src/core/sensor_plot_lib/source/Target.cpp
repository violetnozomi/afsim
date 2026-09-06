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

#include "Target.hpp"

#include <cmath>
#include <iostream>
#include <string>

#include "Sensor.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"

const double Target::cUNDEFINED = 1.0E+30;

// =================================================================================================
Target::Target(const UtAtmosphere& aAtmosphere)
   : mPlatformType("TARGET_PLATFORM_TYPE")
   , mTargetHeading(cUNDEFINED)
   , mTargetYaw(0.0)
   , mTargetPitch(0.0)
   , mTargetRoll(0.0)
   , mTargetSpeed(250.0)
   , mTargetPitchMin(0.0)
   , mTargetPitchMax(0.0)
   , mTargetPitchStep(0.0)
   , mTargetRollMin(0.0)
   , mTargetRollMax(0.0)
   , mTargetRollStep(0.0)
   , mPlatformPtr(nullptr)
   , mSensorReferencePtr(nullptr)
   , mSensorRelativeOrientation(false)
   , mAtmosphere(aAtmosphere)
{
}

// =================================================================================================
Target::~Target()
{
   // The platform is not deleted here because it is owned by the simulation
   delete mSensorReferencePtr;
}

// =================================================================================================
bool Target::Create(const WsfScenario& aScenario)
{
   mPlatformPtr = WsfPlatformTypes::Get(aScenario).Clone(mPlatformType);
   if (mPlatformPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find target platform type.";
      out.AddNote() << "Type: " << mPlatformType;
      return false;
   }
   mPlatformPtr->SetName("TARGET");
   mSensorReferencePtr = new UtEntity();
   return true;
}

// =================================================================================================
//! Create and initialize a local platform.
bool Target::CreateAndInitialize(WsfSimulation& aSim)
{
   if (!Create(aSim.GetScenario()))
   {
      return false;
   }

   // For now the mover is deleted to prevent issues failed initialization due to missing routes.
   // This may need to be addressed if the path needs to be created.
   mPlatformPtr->SetMover(nullptr);

   // Add the platform to the simulation.
   // Note that many sensors do not actually require that the target be part of the simulation,
   // but some do (the 10-Table IR sensor, in particular).

   if (!aSim.AddPlatform(0.0, mPlatformPtr))
   {
      { // RAII block
         auto out = ut::log::error() << "Unable to add target platform to the simulation.";
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
      }
      delete mPlatformPtr;
      mPlatformPtr = nullptr;
      return false;
   }

   return true;
}

// =================================================================================================
bool Target::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "target_platform_type")
   {
      aInput.ReadValue(mPlatformType);
   }
   else if (command == "target_heading")
   {
      aInput.ReadValueOfType(mTargetHeading, UtInput::cANGLE);
      while (mTargetHeading < 0.0)
      {
         mTargetHeading += UtMath::cTWO_PI;
      }
      aInput.ValueInClosedRange(mTargetHeading, 0.0, UtMath::cTWO_PI);
   }
   else if (command == "sensor_relative_orientation")
   {
      aInput.ReadValue(mSensorRelativeOrientation);
   }
   else if (command == "target_yaw")
   {
      aInput.ReadValueOfType(mTargetYaw, UtInput::cANGLE);
      aInput.ValueInClosedRange(mTargetYaw, -UtMath::cPI, UtMath::cPI);
   }
   else if (command == "target_pitch")
   {
      aInput.ReadValueOfType(mTargetPitch, UtInput::cANGLE);
      aInput.ValueInClosedRange(mTargetPitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "target_roll")
   {
      aInput.ReadValueOfType(mTargetRoll, UtInput::cANGLE);
      aInput.ValueInClosedRange(mTargetRoll, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "target_speed")
   {
      aInput.ReadValueOfType(mTargetSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mTargetSpeed, 0.0);
   }
   else if (command == "target_mach")
   {
      aInput.ReadValue(mTargetSpeed);
      aInput.ValueGreaterOrEqual(mTargetSpeed, 0.0);
      if (mTargetSpeed > 0.0)
      {
         mTargetSpeed = -mTargetSpeed; // Negative speed indicates to use Mach.
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Set the target location using the ground range, bearing and altitude from a sensor.
//! @param aSensor      The reference sensor.
//! @param aGroundRange The ground range to the target from the sensor. (meters)
//! @param aBearing     The bearing of the target (radians, clockwise from north)
//! @param aAltitude    The altitude of the target (meters MSL).
void Target::SetLocationRBA(Sensor& aSensor, double aGroundRange, double aBearing, double aAltitude)
{
   double locationWCS[3];
   aSensor.ConvertRBA_ToWCS(aGroundRange, aBearing, aAltitude, locationWCS);
   mPlatformPtr->SetLocationWCS(locationWCS);
}

// =================================================================================================
void Target::SetSpeedAndAttitude(Sensor& aSensor)
{
   SetSpeedAndAttitude(aSensor.GetSensor());
}

// =================================================================================================
void Target::SetSpeedAndAttitude(WsfSensor* aSensorPtr)
{
   double targetVelNED[3];

   // If a heading was specified then use it.  Otherwise point it right at the target
   // so as to avoid Doppler calculations from coming in to play.
   //
   // Also set the pitch and roll to the requested value.

   double heading = mTargetHeading;
   if (mTargetHeading == cUNDEFINED)
   {
      // Use heading to sensor.
      mPlatformPtr->GetRelativeLocationNED(aSensorPtr->GetPlatform(), targetVelNED);
      targetVelNED[2] = 0.0;
      if (UtVec3d::Normalize(targetVelNED) == 0.0)
      {
         targetVelNED[0] = 1.0;
      }
      heading = atan2(targetVelNED[1], targetVelNED[0]);
   }
   else
   {
      // Use specified heading.
      targetVelNED[1] = sin(heading);
      targetVelNED[0] = cos(heading);
      targetVelNED[2] = 0.0;
   }

   heading = UtMath::NormalizeAngleMinusPi_Pi(heading + mTargetYaw);

   if (mSensorRelativeOrientation)
   {
      double sensorLocWCS[3];
      aSensorPtr->GetPlatform()->GetLocationWCS(sensorLocWCS);
      mSensorReferencePtr->SetLocationWCS(sensorLocWCS);
      mSensorReferencePtr->SetOrientationNED(heading, mTargetPitch, mTargetRoll);
      double psi, theta, phi;
      mSensorReferencePtr->GetOrientationWCS(psi, theta, phi);
      mPlatformPtr->SetOrientationWCS(psi, theta, phi);
   }
   else
   {
      mPlatformPtr->SetOrientationNED(heading, mTargetPitch, mTargetRoll);
   }

   UtVec3d::Multiply(targetVelNED, GetSpeed());
   mPlatformPtr->SetVelocityNED(targetVelNED);
}

// =================================================================================================
void Target::SetPitchRange(double aMin, double aMax, double aStep)
{
   mTargetPitchMin  = aMin;
   mTargetPitchMax  = aMax;
   mTargetPitchStep = aStep;
}

// =================================================================================================
void Target::SetRollRange(double aMin, double aMax, double aStep)
{
   mTargetRollMin  = aMin;
   mTargetRollMax  = aMax;
   mTargetRollStep = aStep;
}

// =================================================================================================
double Target::GetSpeed()
{
   double speed = mTargetSpeed;
   if (mTargetSpeed < 0.0)
   {
      speed = (-mTargetSpeed) * mAtmosphere.SonicVelocity(mPlatformPtr->GetAltitude());
   }
   return speed;
}
