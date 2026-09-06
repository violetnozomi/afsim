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

#include "UtAirspeed.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtUnitTypes.hpp"

namespace
{
const double cGAMMA   = 1.401;
const double exponent = (cGAMMA - 1) / cGAMMA;
// const double factor   = 2.0 / (cGAMMA - 1);
} // namespace

// ==========================================================================
UtAirspeed::UtAirspeed()
   : mAtmPtr(nullptr)
   , mMode(cNOT_SET)
   , mMach(0.5)
   , mKnots(200.0)
   , mSpeed(200.0)
{
}

// ==========================================================================
UtAirspeed::UtAirspeed(UtAtmosphere* aAtmPtr)
   : mAtmPtr(aAtmPtr)
   , mMode(cNOT_SET)
   , mMach(0.5)
   , mKnots(200.0)
   , mSpeed(200.0)
{
}

// ==========================================================================
void UtAirspeed::SetMach(double aMach)
{
   mMach = aMach;
   mMode = cMACH;
}

// ==========================================================================
void UtAirspeed::SetKEAS(double aKEAS)
{
   mKnots = aKEAS;
   mMode  = cKEAS;
}

// ==========================================================================
// TODO-fix This method does not use the parameter!
void UtAirspeed::SetKTAS(double /*aKTAS*/)
{
   mSpeed = mKnots * UtMath::cMPS_PER_NMPH;
   mMode  = cTRUE;
}

// ==========================================================================
void UtAirspeed::SetKCAS(double aKCAS)
{
   mKnots = aKCAS;
   mMode  = cKCAS;
}

// ==========================================================================
void UtAirspeed::SetTrue(double aSpeed)
{
   mSpeed = aSpeed;
   mMode  = cTRUE;
}

// ==========================================================================
void UtAirspeed::ProcessBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      UtInput& input = aInputBlock.GetInput();
      if (!ProcessInput(input))
      {
         throw UtInput::UnknownCommand(input);
      }
   }
}

// ==========================================================================
bool UtAirspeed::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "mach")
   {
      double mach;
      aInput.ReadValue(mach);
      aInput.ValueGreater(mach, 0.0);
      SetMach(mach);
   }
   else if (command == "true")
   {
      double Vtrue;
      aInput.ReadValueOfType(Vtrue, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(Vtrue, 0.0);
      SetTrue(Vtrue);
   }
   else if (command == "equivalent")
   {
      double Vequiv;
      aInput.ReadValueOfType(Vequiv, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(Vequiv, 0.0);
      double knotsEquiv = Vequiv * UtMath::cNMPH_PER_MPS;
      SetKEAS(knotsEquiv);
   }
   else if (command == "calibrated")
   {
      double Vcal;
      aInput.ReadValueOfType(Vcal, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(Vcal, 0.0);
      double knotsCal = Vcal * UtMath::cNMPH_PER_MPS;
      SetKCAS(knotsCal);
   }
   else if (command == "indicated")
   {
      // === TO-DO - This is suspect ===
      double Vind;
      aInput.ReadValueOfType(Vind, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(Vind, 0.0);
      double knotsInd = Vind * UtMath::cNMPH_PER_MPS;
      SetKCAS(knotsInd);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ==========================================================================
//! Obtain the set value for airspeed in true meters per second, at altitude.
//! Apart from a specified altitude, the KEAS or KCAS has no concrete meaning,
//! so to Get a value of (true) meters per second, supply the current altitude.
double UtAirspeed::Get(double aCurrentAltitude) const // meters per second
{
   assert(mAtmPtr != nullptr);

   if (mMode == cNOT_SET)
   {
      return 0.0;
   }
   else if (mMode == cTRUE)
   {
      return mSpeed;
   }
   else if (mMode == cKEAS)
   {
      return TAS_FromKEAS(aCurrentAltitude, mKnots);
   }
   else if (mMode == cKCAS)
   {
      return TAS_FromKCAS(aCurrentAltitude, mKnots);
   }
   else // if (mMode == cMACH)
   {
      return mAtmPtr->SonicVelocity(aCurrentAltitude) * mMach;
   }
}

// ==========================================================================
//! Obtain the set value for airspeed in true meters per second, at altitude.
//! Also provide the rate of acceleration required to hold this speed setting
//! at the current rate of climb.
//! Apart from a specified altitude, the KEAS or KCAS has no concrete meaning,
//! so to Get a value of (true) meters per second, supply the current altitude.
void UtAirspeed::Get(double  aCurrentAltitude,
                     double  aRateOfClimb,
                     double& aTrueSpeed,       // m/s
                     double& aAccelRate) const // m/s2
{
   assert(mAtmPtr != nullptr);

   if (mMode == cNOT_SET)
   {
      aTrueSpeed = 0.0;
      aAccelRate = 0.0;
   }
   else if (mMode == cTRUE)
   {
      aTrueSpeed = mSpeed;
      aAccelRate = 0.0;
   }
   else if (mMode == cKEAS)
   {
      aTrueSpeed = TAS_FromKEAS(aCurrentAltitude, mKnots);
      // TO-DO - Partial derivative.  There is probably a better way to do this.
      double higherAltSpeed = TAS_FromKEAS(aCurrentAltitude + 100.0, mKnots);
      aAccelRate            = (higherAltSpeed - aTrueSpeed) / 100.0 * aRateOfClimb;
   }
   else if (mMode == cKCAS)
   {
      aTrueSpeed = TAS_FromKCAS(aCurrentAltitude, mKnots);
      // TO-DO - Partial derivative.  There is probably a better way to do this.
      double higherAltSpeed = TAS_FromKCAS(aCurrentAltitude + 100.0, mKnots);
      aAccelRate            = (higherAltSpeed - aTrueSpeed) / 100.0 * aRateOfClimb;
   }
   else if (mMode == cMACH)
   {
      double sonicVel = mAtmPtr->SonicVelocity(aCurrentAltitude);
      aTrueSpeed      = sonicVel * mMach;
      aAccelRate      = mAtmPtr->dSpeed_dAltitudeAtMach(aCurrentAltitude) * aRateOfClimb;
   }
}

// ==========================================================================
double UtAirspeed::GetSpeedIn(Mode /*aMode*/, double /*aAltitude*/)
{
   // DRB TO-DO
   assert(false);
   return 0.0;
}

// ==========================================================================
double UtAirspeed::GetSpeedOfSoundAtAltitude_fps(double aAltitude_ft)
{
   double c = 0.0;

   // Data is given as a function of altitude in units of feet, so input is forced to that unit.
   // http://www.aerospaceweb.org/question/atmosphere/q0112.shtml
   if (aAltitude_ft < 40000.0)
   {
      c = 1116.4 + (968.1 - 1116.4) * ((aAltitude_ft - 0.0) / (40000.0 - 0.0));
   }
   else if (aAltitude_ft < 65000.0)
   {
      c = 968.1;
   }
   else if (aAltitude_ft < 160000.0)
   {
      c = 968.1 + (1082.0 - 968.1) * ((aAltitude_ft - 65000.0) / (160000.0 - 65000.0));
   }
   else if (aAltitude_ft < 165000.0)
   {
      c = 1082.0;
   }
   else if (aAltitude_ft < 285000.0)
   {
      c = 1082.0 + (899.3 - 1082.0) * ((aAltitude_ft - 165000.0) / (285000.0 - 165000.0));
   }
   else if (aAltitude_ft < 295000.0)
   {
      c = 899.3;
   }
   else if (aAltitude_ft < 400000.0)
   {
      c = 899.3 + (1010.0 - 899.3) * ((aAltitude_ft - 295000.0) / (400000.0 - 295000.0));
   }
   else
   {
      c = 0.0;
   }
   return c;
}

// ==========================================================================
double UtAirspeed::CalculateMachAtAltitude_ft(double aAltitude_ft, double aSpeed_fps)
{
   double c    = GetSpeedOfSoundAtAltitude_fps(aAltitude_ft);
   double mach = 0.0;

   if (c < 0.001)
   {
      mach = 0.0;
   }
   else
   {
      mach = aSpeed_fps / c;
   }

   return mach;
}

// ==========================================================================
double UtAirspeed::CalculateMachAtAltitude_m(double aAltitude_m, double aSpeed_mps)
{
   // Make inputs compatible with CalculateMachAtAltitude_ft()
   double alt_ft    = UtUnitLength::ConvertFromStandard(aAltitude_m, UtUnitLength::cFEET);
   double speed_fps = UtUnitSpeed::ConvertFromStandard(aSpeed_mps, UtUnitSpeed::cFEET_PER_SECOND);
   return CalculateMachAtAltitude_ft(alt_ft, speed_fps);
}

// ==========================================================================
//! Translate from Knots Calibrated Air Speed (KCAS) to True Air Speed (TAS),
//! given the input altitude (m).
double UtAirspeed::TAS_FromKCAS(double aAltitude, double aKCAS) const
{
   double delta = mAtmPtr->PressureRatio(aAltitude);
   double theta = mAtmPtr->TemperatureRatio(aAltitude);

   double ratio                 = aKCAS / 661.4786;
   double inSmallSquareBrackets = 1.0 + 0.2 * (ratio * ratio);
   double inCurlyBraces         = pow(inSmallSquareBrackets, 3.5) - 1.0;
   double inParens              = 1.0 / delta * inCurlyBraces + 1;
   double inLargeSquareBrackets = pow(inParens, 1.0 / 3.5) - 1.0;
   double KTAS                  = 1479.1 * sqrt(theta * inLargeSquareBrackets);

   return KTAS * UtMath::cMPS_PER_NMPH;
}

// ==========================================================================
double UtAirspeed::TAS_FromKEAS(double aAltitude, double aKEAS) const
{
   double sigma = mAtmPtr->DensityRatio(aAltitude);
   return aKEAS / sqrt(sigma) * UtMath::cMPS_PER_NMPH;
}

// ===========================================================================
// static
bool UtAirspeed::Test(bool aPrintResults)
{
   // This test will traverse a range of altitudes and speeds, and assure that
   // all speeds are calculated correctly, versus internally stored benchmarks.

   const size_t cNUM_ALTS       = 18;
   const double altitudesFeet[] = {0.0,
                                   10000.0,
                                   20000.0,
                                   30000.0,
                                   36089.0,
                                   40000.0,
                                   50000.0,
                                   60000.0,
                                   65617.0,
                                   70000.0,
                                   80000.0,
                                   90000.0,
                                   100000.0,
                                   120000.0,
                                   140000.0,
                                   160000.0,
                                   180000.0,
                                   200000.0};

   // const double cMAX_SPEED = 2000.0 * UtMath::cMPS_PER_NMPH;

   const size_t cNUM_KEAS    = 11;
   const double speedsKEAS[] = {50.0, 100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 700.0, 800.0, 900.0, 1000.0};

   const size_t cNUM_KCAS    = 9;
   const double speedsKCAS[] = {50.0, 100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 800.0, 1000.0};

   const size_t cNUM_MACHS   = 25;
   const double speedsMach[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3,
                                1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5};

   // Algorithm:  Sweep altitude (IV) from zero to maximum, and at each altitude,
   // calculate a true speed (m/s) for each desired:
   // A) Mach trace, B) KEAS trace, C) KCAS trace

   // If requested plot them.  For test purposes, pull off certain test points
   // which are to have a known value.

   // For test purposes, dump the resulting route to file:
   std::ofstream streamX;
   if (aPrintResults)
   {
      streamX.open("UtAirspeed_Test.csv");
      if (streamX)
      {
         streamX << "altFt, ";

         streamX << "M1, M2, M3, M4, M5, M6, M7, M8, M9, M10, M11, M12, M13, M14, M15, ";
         streamX << "M16, M17, M18, M19, M20, M21, M22, M23, M24, M25, ";

         streamX << "KE50, KE100, KE200, KE300, KE400, KE500, KE600, KE700, KE800, KE900, KE1000, ";

         streamX << "KC50, KC100, KC200, KC300, KC400, KC500, KC600, KC800, KC1000" << std::endl;
      }
      else
      {
         aPrintResults = false;
      }
   }


   UtAtmosphere atm(UtAtmosphere::CreateNewAtmosphereTable());
   UtAirspeed   airspeed(&atm);

   const size_t cNUM_ALT_INTERVALS = 100;
   double       deltaAlt           = altitudesFeet[cNUM_ALTS - 1] / cNUM_ALT_INTERVALS * UtMath::cM_PER_FT;

   double theSpeedMPS;

   for (size_t iAlt = 0; iAlt != cNUM_ALT_INTERVALS; ++iAlt)
   {
      // Now for this altitude...
      double alt   = iAlt * deltaAlt;
      double altFt = alt * UtMath::cFT_PER_M;
      if (aPrintResults)
      {
         streamX << altFt << ", ";
      }

      // Compute the true speeds (m/s) corresponding to all the Mach points of interest:
      for (size_t iMach = 0; iMach != cNUM_MACHS; ++iMach)
      {
         double mach = speedsMach[iMach];
         airspeed.SetMach(mach);
         theSpeedMPS = airspeed.Get(alt);

         if (aPrintResults)
         {
            streamX << theSpeedMPS * UtMath::cFT_PER_M << ", ";
         }
      }

      // Compute the true speeds (m/s) corresponding to all the KEAS points of interest:
      for (size_t iKEAS = 0; iKEAS != cNUM_KEAS; ++iKEAS)
      {
         double keas = speedsKEAS[iKEAS];
         airspeed.SetKEAS(keas);
         theSpeedMPS = airspeed.Get(alt);

         if (aPrintResults)
         {
            streamX << theSpeedMPS * UtMath::cFT_PER_M << ", ";
         }
      }

      // Compute the true speeds (m/s) corresponding to all the KCAS points of interest:
      bool lastOne = false;
      for (size_t iKCAS = 0; iKCAS != cNUM_KCAS; ++iKCAS)
      {
         double kcas = speedsKCAS[iKCAS];
         airspeed.SetKCAS(kcas);
         theSpeedMPS = airspeed.Get(alt);

         if (iKCAS + 1 == cNUM_KCAS)
         {
            lastOne = true;
         }

         if (aPrintResults)
         {
            if (lastOne)
            {
               streamX << theSpeedMPS * UtMath::cFT_PER_M << std::endl;
            }
            else
            {
               streamX << theSpeedMPS * UtMath::cFT_PER_M << ", ";
            }
         }
      }
   }

   streamX.close();

   // ==============================================================================
   // The above was just a tabulation.  Here we compare against two expected values:
   // ==============================================================================

   bool success = true;

   UtAirspeed as;
   as.SetAtmosphere(&atm);

   as.SetKCAS(300);
   double speedMPS       = as.Get(50000 * UtMath::cM_PER_FT);
   double speedKnotsTrue = speedMPS * UtMath::cNMPH_PER_MPS;

   if (fabs(speedKnotsTrue - 668.87) > 0.02)
   {
      ut::log::error() << "UtAirspeed Failed Calibrated Airspeed Conversion.";
      success = false;
   }

   as.SetKEAS(200);
   speedMPS       = as.Get(65617.0 * UtMath::cM_PER_FT);
   speedKnotsTrue = speedMPS * UtMath::cNMPH_PER_MPS;

   if (fabs(speedKnotsTrue - 746.06) > 0.02)
   {
      ut::log::error() << "UtAirspeed Failed Equivalent Airspeed Conversion.";
      success = false;
   }

   return success;
}
