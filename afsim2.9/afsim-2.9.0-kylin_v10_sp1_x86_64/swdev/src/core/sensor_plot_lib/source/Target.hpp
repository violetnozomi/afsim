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

#ifndef TARGET_HPP
#define TARGET_HPP

#include <string>

class Sensor;
#include "UtAtmosphere.hpp"
#include "UtEntity.hpp"
class UtInput;
class WsfPlatform;
class WsfSensor;
class WsfScenario;
class WsfSimulation;

class Target
{
public:
   explicit Target(const UtAtmosphere& aAtmosphere);
   ~Target();

   bool Create(const WsfScenario& aScenario);

   bool CreateAndInitialize(WsfSimulation& aSim);

   bool ProcessInput(UtInput& aInput);

   void SetLocationRBA(Sensor& aSensor, double aGroundRange, double aBearing, double aAltitude);

   void SetPitchRange(double aMin, double aMax, double aStep);
   void SetRollRange(double aMin, double aMax, double aStep);
   void SetSpeedAndAttitude(Sensor& aSensor);
   void SetSpeedAndAttitude(WsfSensor* aSensorPtr);

   WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   double       GetTargetPitch() const { return mTargetPitch; }
   double       GetTargetPitchMin() const { return mTargetPitchMin; }
   double       GetTargetPitchMax() const { return mTargetPitchMax; }
   double       GetTargetPitchStep() const { return mTargetPitchStep; }
   double       GetTargetRoll() const { return mTargetRoll; }
   double       GetTargetRollMin() const { return mTargetRollMin; }
   double       GetTargetRollMax() const { return mTargetRollMax; }
   double       GetTargetRollStep() const { return mTargetRollStep; }
   double       GetSpeed();

   void SetPitch(double aPitch) { mTargetPitch = aPitch; }
   void SetRoll(double aRoll) { mTargetRoll = aRoll; }

private:
   static const double cUNDEFINED;

   std::string mPlatformType;
   double      mTargetHeading;
   double      mTargetYaw;
   double      mTargetPitch;
   double      mTargetRoll;
   double      mTargetSpeed; //! A negative value indicates this is a Mach number.

   double mTargetPitchMin;
   double mTargetPitchMax;
   double mTargetPitchStep; //! Defaults to zero, indicating the range is invalid
   double mTargetRollMin;
   double mTargetRollMax;
   double mTargetRollStep; //! Defaults to zero, indicating the range is invalid

   WsfPlatform* mPlatformPtr;
   UtEntity*    mSensorReferencePtr;
   bool         mSensorRelativeOrientation;

   UtAtmosphere mAtmosphere;
};

#endif
