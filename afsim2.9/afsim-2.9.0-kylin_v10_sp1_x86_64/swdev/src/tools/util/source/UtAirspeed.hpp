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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTAIRSPEED_HPP
#define UTAIRSPEED_HPP

#include "ut_export.h"

class UtInput;
class UtInputBlock;

#include "UtAtmosphere.hpp"

//! Utility to Set and Get Airspeed values in different ways.  Desired airspeed
//! may be Set in Knots sea-level Equivalent (KEAS), Knots True (KTAS), Knots
//! Calibrated (KCAS), or Mach, and then fetched in m/s at any altitude.  Class
//! also provides the acceleration value required to hold the speed setting
//! while at a specified rate-of-climb.  Involves use of conversion routines
//! previously placed in UtAtmosphere.  Pitot-Static tubes fitted on aircraft
//! measure dynamic pressure -> q = 1/2 rho V^2.  Then q -> CAS -> EAS -> TAS
//! -> GroundSpeed.
//! a) Indicated (IAS) corrected for instrument and position errors is CAS.
//!    (This correction is small, is aircraft specific, and is ignored here.)
//! b) Calibrated Airspeed (CAS) corrected for compressibility errors is EAS.
//! c) Equivalent Airspeed (EAS) corrected for density errors is TAS.
//! d) True Airspeed (TAS) corrected for (average) winds is mean Ground Speed.

class UT_EXPORT UtAirspeed
{
public:
   enum Mode
   {
      cNOT_SET,
      cTRUE,
      cKEAS,
      cKCAS,
      cMACH
   };

   UtAirspeed();
   UtAirspeed(UtAtmosphere* aAtmPtr);

   bool ProcessInput(UtInput& aInput);
   void ProcessBlock(UtInputBlock& aInputBlock);

   void          SetAtmosphere(UtAtmosphere* aAtmPtr) { mAtmPtr = aAtmPtr; }
   UtAtmosphere* GetAtmospherePtr() { return mAtmPtr; }

   void SetKEAS(double aKEAS);  // Knots sea-level equivalent
   void SetKCAS(double aKCAS);  // Knots calibrated airspeed
   void SetKTAS(double aKTAS);  // Knots true airspeed
   void SetMach(double aMach);  // Speed in Mach
   void SetTrue(double aSpeed); // Set speed in true m/s

   double Get(double aCurrentAltitude) const; // m/s

   void Get(double  aCurrentAltitude,
            double  aRateOfClimb,
            double& aTrueSpeed,        // m/s
            double& aAccelRate) const; // m/s2

   double GetSpeedIn(Mode aMode, double aAltitude);

   // Determine speed of sound in feet/second
   // Input altitude must be in units of feet.
   static double GetSpeedOfSoundAtAltitude_fps(double aAltitude_ft);

   // Calculate mach based on altitude and speed
   // Input altitude must be in units of feet.
   // Input speed must be in units of feet/second.
   static double CalculateMachAtAltitude_ft(double aAltitude_ft, double aSpeed_fps);

   // Calculate mach based on altitude and speed
   // Input altitude must be in units of meters.
   // Input speed must be in units of meters/second.
   static double CalculateMachAtAltitude_m(double aAltitude_m, double aSpeed_mps);

   // Fetch speed in meters per second
   double TAS_FromKCAS(double aAltitude, double aKCAS) const;

   // Fetch speed in meters per second
   double TAS_FromKEAS(double aAltitude, double aKEAS) const;

   bool IsSet() const { return mMode != cNOT_SET; }

   static bool Test(bool aPrintResults = false);

private:
   UtAtmosphere* mAtmPtr;
   Mode          mMode;
   double        mMach;
   double        mKnots; // knots (either true, equivalent, or calibrated)
   double        mSpeed; // m/s
};

#endif
