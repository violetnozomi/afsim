// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOpticalEnvironment.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "UtBlackBody.hpp"
#include "UtCalendar.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "WsfDateTime.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfOpticalEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace
{
// The indices of our bands in the band description and the precomputed band data arrays.
// They are in ascending order of wavelength.
//
// NOTE: This includes more that just the bands that we ACTUALLY support. The 'EnumToIndex' method that follows
//       maps the supported bands from WsfEM_Types::OpticalBand to the indicies.
enum
{
   cBAND_UV_C,
   cBAND_UV_B,
   cBAND_UV_A,
   cBAND_VISUAL,
   cBAND_IR_SHORT,
   cBAND_IR_MEDIUM,
   cBAND_IR_LONG,
   cBAND_IR_VERY_LONG,
   cBAND_COUNT
};

// Map an optical band enumeration to one of our internal indices.
inline size_t BandEnumToIndex(WsfEM_Types::OpticalBand aBand)
{
   size_t index = cBAND_VISUAL;
   switch (aBand)
   {
   case WsfEM_Types::cOPT_DEFAULT:
      index = cBAND_IR_SHORT;
      break;
   case WsfEM_Types::cOPT_IR_SHORT:
      index = cBAND_IR_SHORT;
      break;
   case WsfEM_Types::cOPT_IR_MEDIUM:
      index = cBAND_IR_MEDIUM;
      break;
   case WsfEM_Types::cOPT_IR_LONG:
      index = cBAND_IR_LONG;
      break;
   case WsfEM_Types::cOPT_IR_VERY_LONG:
      index = cBAND_IR_VERY_LONG;
      break;
   case WsfEM_Types::cOPT_VISUAL:
      index = cBAND_VISUAL;
      break;
   default:
      index = cBAND_IR_SHORT;
      break;
   }
   return index;
}

// These are the descriptions of the wave bands.
struct BandDescriptor
{
   const char* mName;
   double      mLowerWavelength;
   double      mUpperWavelength;
};

//===================================================================================================================
// All of the following constants (up to the CUT-AND-PASTE) are used in generating the data that is
// computed by PrintComputedData. If you change these values:
//      1) Rebuild with the updated values
//      2) Run the command 'print_computed_data'
//      3) Cut and paste the output from print_computed_data, replacing the existing CUT-AND-PASTE block below.
//      4) Rebuild one more time.
//===================================================================================================================

// The arrays of the band descriptors and pre-computed data are in the order of the enumeration of the bands defined above
const BandDescriptor cBAND_DESC[] = {{"uv-c", 100.0E-9, 280.0E-9},
                                     {"uv-b", 280.0E-9, 315.0E-9},
                                     // UV A normally ends at 400nm, but we had already defined visual sto tart at 380nm
                                     {"uv-a", 315.0E-9, 380.0E-9},
                                     {"visual", 380.0E-9, 760.0E-9},
                                     {"ir-short", 1.0E-6, 3.0e-6},
                                     {"ir-medium", 3.0E-6, 5.0E-6},
                                     {"ir-long", 8.0E-6, 12.0E-6},
                                     {"ir-very_long", 15.0E-6, 30.0E-6}};

// The precomputed data for a band.
struct BandData
{
   double mSolarRadiance;         // Radiance (L), W/m^2/sr
   double mSolarRadiantIntensity; // Radiant Intensity (I), W/sr
   double mSolarFlux;             // Radiant Flux (Phi), W
};

// au - Astronomical unit - mean distance from Earth to Sun
const double cAU = 149597870700.0; // m

// Radius of the Sun.
// The solar radius that will be used throughout this routine is UtSun::cMEAN_RADIUS.
// This varies is slightly less than the IAU version (less than 0.1%)
const double cSUN_RADIUS = UtSun::cMEAN_RADIUS; // 6.963420E+8;
// const double cSUN_RADIUS = 695700.0E+3;                   // IAU, resolution B3, 2015

// Blackbody temperature of the Sun.
// The blackbody temperature only approximates the actual spectrum from the Sun. A range of values can be
// found (5772, 5776, 5780 and even as high as 5800 or 5900). The lower values seem more common.
const double cSUN_TEMPERATURE = 5772.0; // deg-K

// The following three variables are from the energy balance diagram in the design whitepaper. They are used in

// The approximate relected flux density from Sun
const double cAVG_EARTH_REFLECTED_FLUX_DENSITY = 102.0; // W/m^2

// The approximate emitted flux density of the Earth
const double cAVG_EARTH_EMITTED_FLUX_DENSITY = 239.0; // W/m^2

// The approximate total flux density of the Earth (reflected + emitted)
const double cAVG_EARTH_TOTAL_FLUX_DENSITY = 341.0; // W/m^2

// The following is produced by executing the 'show_computed_data' command and taking the results and cutting-and-pasting...

// BEGIN-CUT-AND-PASTE from 'show_computed_data' ====================================================================

const BandData cBAND_DATA[] = {
   {4.209093e+05, 6.411854e+23, 8.057373e+24}, // uv-c
   {4.109014e+05, 6.259400e+23, 7.865794e+24}, // uv-b
   {1.164846e+06, 1.774449e+24, 2.229838e+25}, // uv-a
   {8.955493e+06, 1.364221e+25, 1.714330e+26}, // visual
   {5.232400e+06, 7.970692e+24, 1.001627e+26}, // ir-short
   {3.211155e+05, 4.891661e+23, 6.147042e+24}, // ir-medium
   {1.910482e+04, 2.910302e+22, 3.657193e+23}, // ir-long
   {3.859787e+03, 5.879744e+21, 7.388704e+22}, // ir-very_long
   {0.000000e+00, 0.000000e+00, 0.000000e+00}, // dummy
};

// END-CUT-AND-PASTE from 'show_computed_data' ======================================================================
}; // namespace

// =================================================================================================
WsfOpticalEnvironment::WsfOpticalEnvironment()
   : mPlatformPtr(nullptr)
   , mBackgroundRadianceAboveHorizon(0.0)
   , mBackgroundRadianceBelowHorizon(0.0)
   , mLowerTransitionAngle(0.0)
   , mUpperTransitionAngle(0.0)
   , mPathRadiance(0.0)
   , mUseDynamicModel(false)
   , mSunMoonUpdateMutex()
   , mLastSunMoonUpdateTime(-1.0)
   , mCurrentTime()
   , mSunLoc()
   , mMoonLoc()
{
}

// =================================================================================================
WsfOpticalEnvironment::WsfOpticalEnvironment(const WsfOpticalEnvironment& aSrc)
   : mPlatformPtr(nullptr)
   , mBackgroundRadianceAboveHorizon(aSrc.mBackgroundRadianceAboveHorizon)
   , mBackgroundRadianceBelowHorizon(aSrc.mBackgroundRadianceBelowHorizon)
   , mLowerTransitionAngle(aSrc.mLowerTransitionAngle)
   , mUpperTransitionAngle(aSrc.mUpperTransitionAngle)
   , mPathRadiance(aSrc.mPathRadiance)
   , mUseDynamicModel(aSrc.mUseDynamicModel)
   , mSunMoonUpdateMutex()
   , mLastSunMoonUpdateTime(-1.0)
   , mCurrentTime()
   , mSunLoc()
   , mMoonLoc()
{
}

// =================================================================================================
bool WsfOpticalEnvironment::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool ok      = true;
   mPlatformPtr = aPlatformPtr;
   mPlatformPtr->GetSimulation()->GetDateTime().GetCurrentTime(aSimTime, mCurrentTime);
   return ok;
}

// =================================================================================================
bool WsfOpticalEnvironment::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   if (command == "background_radiance")
   {
      std::string word;
      aInput.ReadValue(word);
      if (word == "dynamic")
      {
         mUseDynamicModel = true;
      }
      else
      {
         aInput.PushBack(word);
         mBackgroundRadianceAboveHorizon = ReadRadianceValue(aInput);
         mBackgroundRadianceBelowHorizon = mBackgroundRadianceAboveHorizon;
         mUseDynamicModel                = false;
      }
   }
   else if (command == "background_radiance_above_horizon")
   {
      mBackgroundRadianceAboveHorizon = ReadRadianceValue(aInput);
      mUseDynamicModel                = false;
   }
   else if (command == "background_radiance_below_horizon")
   {
      mBackgroundRadianceBelowHorizon = ReadRadianceValue(aInput);
      mUseDynamicModel                = false;
   }
   else if (command == "background_transition_region")
   {
      aInput.ReadValueOfType(mLowerTransitionAngle, UtInput::cANGLE);
      aInput.ReadValueOfType(mUpperTransitionAngle, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mLowerTransitionAngle, -UtMath::cPI_OVER_2);
      aInput.ValueInClosedRange(mUpperTransitionAngle, mLowerTransitionAngle, UtMath::cPI_OVER_2);
   }
   else if (command == "path_radiance")
   {
      mPathRadiance = ReadRadianceValue(aInput);
   }
   else if (command == "print_computed_data")
   {
      PrintComputedData();
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
double WsfOpticalEnvironment::ComputeBackgroundRadiance(double           aSimTime,
                                                        const BandList&  aBands,
                                                        WsfSensorResult& aResult,
                                                        double&          aSolarNoise)
{
   double backgroundRadiance = 0.0;
   aSolarNoise               = 0.0;
   if (mUseDynamicModel)
   {
      UpdateSunMoonData(aSimTime);
      UtVec3d viewerLoc(aResult.mRcvrLoc.mLocWCS);
      UtVec3d viewerDir(aResult.mRcvrToTgt.mUnitVecWCS);
      backgroundRadiance = ComputeDynamicBackgroundRadiance(aBands, viewerLoc, viewerDir, mSunLoc, mMoonLoc, aSolarNoise);
   }
   else if (mBackgroundRadianceAboveHorizon != mBackgroundRadianceBelowHorizon)
   {
      // A different background radiance has been specified for both above and below horizon.
      // Determine the depression angle of the target compared to the depression angle of the horizon.

      // NOTE: This should only be used for airborne sensors. Ground based sensors should specify both
      //       angles the same.

      // Determine the depression angle of the horizon.
      double earthRadius            = UtSphericalEarth::cEARTH_RADIUS;
      double cosTheta               = earthRadius / (earthRadius + aResult.mRcvrLoc.mAlt);
      double horizonDepressionAngle = -acos(cosTheta);

      // Determine the depression angle of the target.
      WsfEM_Antenna* antennaPtr = aResult.GetReceiver()->GetAntenna();
      double         tgtLocNED[3];
      antennaPtr->ConvertWCS_ToNED(aResult.mTgtLoc.mLocWCS, tgtLocNED);
      double rangeNE               = sqrt(tgtLocNED[0] * tgtLocNED[0] + tgtLocNED[1] * tgtLocNED[1]);
      double targetDepressionAngle = -atan2(tgtLocNED[2], rangeNE);

      double lowerTransitionAngle = horizonDepressionAngle + mLowerTransitionAngle;
      double upperTransitionAngle = horizonDepressionAngle + mUpperTransitionAngle;
      if (targetDepressionAngle <= lowerTransitionAngle)
      {
         backgroundRadiance = mBackgroundRadianceBelowHorizon;
      }
      else if (targetDepressionAngle >= upperTransitionAngle)
      {
         backgroundRadiance = mBackgroundRadianceAboveHorizon;
      }
      else
      {
         double fraction = (targetDepressionAngle - lowerTransitionAngle) / (upperTransitionAngle - lowerTransitionAngle);
         double deltaRadiance = (mBackgroundRadianceAboveHorizon - mBackgroundRadianceBelowHorizon);
         backgroundRadiance   = mBackgroundRadianceBelowHorizon + (fraction * deltaRadiance);
      }
   }
   else
   {
      // Same value for above and below horizon.
      backgroundRadiance = mBackgroundRadianceAboveHorizon;
   }
   return backgroundRadiance;
}

// =================================================================================================
double WsfOpticalEnvironment::ComputePathRadiance(double aSimTime, const BandList& aBands, WsfSensorResult& aResult)
{
   return mPathRadiance;
}

// =================================================================================================
double WsfOpticalEnvironment::ComputeDynamicBackgroundRadiance(const BandList& aBands,
                                                               const UtVec3d&  aViewerLoc,
                                                               const UtVec3d&  aViewerDir,
                                                               const UtVec3d&  aSunLoc,
                                                               const UtVec3d&  aMoonLoc,
                                                               double&         aSolarNoise)
{
   aSolarNoise = 0.0;

   UtVec3d sunUnitVec(aSunLoc);
   double  D_earth_sun = sunUnitVec.Normalize();
   UtVec3d viewerUnitVec(aViewerLoc);
   viewerUnitVec.Normalize();

   // Compute the incidence angle between the viewer and the Sun.
   double cos_incAngle = viewerUnitVec.DotProduct(sunUnitVec);
   cos_incAngle        = UtMath::Limit(cos_incAngle, 1.0);

   // If Lambert's cosine law is blindly applied using the angle between the TOA and Sun vectors, we miss the effects of
   // twilight. According to www.timeanddate.com:
   //
   // --------------------------------------------------- D A Y -------------------------------------------------------
   //           Sunrise - Center of Sun rises above   0 deg                  Sunset - Center of Sun drops below   0 deg
   //
   //                                           C I V I L   T W I L I G H T
   //
   //        Civil Dawn - Center of Sun rises above  -6 deg            Civil Sunset - Center of Sun drops below  -6 deg
   //
   //                                        N A U T I C A L   T W I L I G H T
   // Horizon is faintly visible and many of the brighter stars can be seen.
   //
   //     Nautical Dawn - Center of Sun rises above -12 deg         Nautical Sunset - Center of Sun drops below -12 deg
   //
   //                                    A S T R O N O M I C A L   T W I L I G H T
   //
   // Astronomical Dawn - Center of Sun rises above -18 deg     Astronomical Sunset - Center of Sun drops below -18 deg
   // ------------------------------------------------- N I G H T -----------------------------------------------------

   // Based on this, we treat incidence angles from 0 to 108 degrees (measured from zenith) as having some impact on the
   // background. (A trivial implementation would be 0 to 90 degrees). Starting at 84 degrees we compress the +6 to -18
   // into the final 6 degrees.

   double incAngleDeg = acos(cos_incAngle) * UtMath::cDEG_PER_RAD; // 90 degrees will be horizon
   if (incAngleDeg > 84.0)
   {
      if (incAngleDeg <= 108.0) // Above 18 degrees below horizon
      {
         incAngleDeg  = 84.0 + (incAngleDeg - 84.0) / 6.0;
         cos_incAngle = cos(incAngleDeg * UtMath::cDEG_PER_RAD);
      }
      else
      {
         cos_incAngle = 0.0; // Indicate it is night time...
      }
   }

   // The total solar radiant intensity over the band(s) of interest (W/sr)
   double I_sun = GetSolarRadiantIntensity(aBands);

   double I_src = 0.0;
   double d_src = 0.0;
   if (cos_incAngle > 0.0)
   {
      // It is considered day time, so the Sun is the source.
      I_src = I_sun;
      d_src = D_earth_sun;

      // Compute the solar noise term if the target is close to the Sun.
      UtVec3d viewerSunVec;
      viewerSunVec.Subtract(aSunLoc, aViewerLoc);
      viewerSunVec.Normalize();

      // Compute angle between line viewer line of sight and the vector from the viewer to the Sun.
      double cos_losSunAngle = UtVec3d::DotProduct(viewerSunVec, aViewerDir);
      cos_losSunAngle        = UtMath::Limit(cos_losSunAngle, 1.0);
      double losSunAngle     = acos(cos_losSunAngle);

      // Compute the half angle subtended by the Sun.
      double sin_sunHalfAngle = UtSun::cMEAN_RADIUS / D_earth_sun;
      double sunHalfAngle     = asin(sin_sunHalfAngle);

      if (losSunAngle <= sunHalfAngle)
      {
         // OK, this is just a hack, but it is better than nothing.
         //
         // Compute the in-band irradiance at the Earth, assuming no attenuation by the atmosphere. We will then assume
         // about We will assume that about 50% is absorbed/scattered by the atmosphere. Even with that this number is
         // HUGE compared to the typical reciever noise. All we are trying to accomplish is simulating that it is
         // impossible to detect something when looking right at the Sun!
         double E    = I_src / (D_earth_sun * D_earth_sun);
         aSolarNoise = E * 0.5; // This probably needs to be less towards the horizon - more atmosphere...
      }
   }
   else
   {
      static const double cMOON_ALBEDO = 0.1324;

      // It is considered night, so the Moon is the source.
      UtVec3d moonEarthVec(-aMoonLoc);
      double  D_moon_earth = moonEarthVec.Normalize();

      // Determine the illumination fraction of the moon.
      UtVec3d moonSunVec;
      moonSunVec.Subtract(aSunLoc, aMoonLoc);
      double D_moon_sun = moonSunVec.Normalize();
      double cosPhase   = UtVec3d::DotProduct(moonEarthVec, moonSunVec);
      double illum      = (1.0 + cosPhase) / 2.0;

      // Determine the radiance intensity of the solar radiation reflected by the Moon towards the Earth

      double E_moon = I_sun / (D_moon_sun * D_moon_sun); // Solar irradiance at location of the moon (W/m^2)
      double L_moon =
         E_moon * (cMOON_ALBEDO / UtMath::cPI) * illum; // Lunar radiance, reduced by fraction visible from Earth
      double R_moon = UtMoon::cMEAN_RADIUS;
      double I_moon = L_moon * (UtMath::cPI * R_moon * R_moon); // Lunar radiant intensity

      I_src = I_moon;
      d_src = D_moon_earth;
   }

   // GIven the radiant intensity of the source, the distance to the source and the incidence and of the light rays on
   // the Earth at the observers location, calculate the irradiance.
   double E = I_src / (d_src * d_src) * cos_incAngle;

   // The radiance is computed assuming the top of the atmosphere to be a Lambertian scatter.
   double L = E / UtMath::cPI;

   return L;
}

// =================================================================================================
//! Get the blackbody radiance for the specified bands of interest.
//! @param aBands The bands of interest.
//! @param aTemperature The temperature of the body (K)
//! @returns The total radiance over the bands of interest (W/m2/sr).
// static
double WsfOpticalEnvironment::GetBlackbodyRadiance(const BandList& aBands, double aTemperature)
{
   double L = 0.0;
   double lowerWavelength;
   double upperWavelength;
   for (const auto band : aBands)
   {
      GetWavelengthLimits(band, lowerWavelength, upperWavelength);
      L += ut::BlackBody::BandRadiance(aTemperature, lowerWavelength, upperWavelength);
   }
   return L;
}

// =================================================================================================
//! Get the solar radiant intensity for the specified bands of interest.
//! @param aBands The bands of interest.
//! @returns The total solar radiant intensity over the bands of interest.
// static
double WsfOpticalEnvironment::GetSolarRadiantIntensity(const BandList& aBands)
{
   double I_sun = 0.0;
   for (const auto band : aBands)
   {
      I_sun += GetSolarRadiantIntensity(band);
   }
   return I_sun;
}

// =================================================================================================
//! Get the solar radiant intensity for the selected band.
//! @param aBand The desired band.
//! @returns The solar radiant intensity (W/sr).
// static
double WsfOpticalEnvironment::GetSolarRadiantIntensity(WsfEM_Types::OpticalBand aBand)
{
   size_t index = BandEnumToIndex(aBand);
   return cBAND_DATA[index].mSolarRadiantIntensity;
}

// =================================================================================================
//! Get the lower and upper wavelengths that define the selected band.
//! @param aBand The desired band.
//! @param aLowerWavelength The lower wavelength of the band (m).
//! @param aUpperWavelength The upper wavelength of the band (m).
// static
void WsfOpticalEnvironment::GetWavelengthLimits(WsfEM_Types::OpticalBand aBand,
                                                double&                  aLowerWavelength,
                                                double&                  aUpperWavelength)
{
   size_t index     = BandEnumToIndex(aBand);
   aLowerWavelength = cBAND_DESC[index].mLowerWavelength;
   aUpperWavelength = cBAND_DESC[index].mUpperWavelength;
}

// =================================================================================================
// Update the member variables that contain the location of the Sun and Moon.
// This is called only when the simulation time is updated as these methods are a bit intensive.
// Note that we could compute and save the needed unit vectors, but at some point we will have cached
// versions of the positions and we can simply use those directly. So the caller will still be left
// with needing to compute the unit vectors...
void WsfOpticalEnvironment::UpdateSunMoonData(double aSimTime)
{
   if (aSimTime != mLastSunMoonUpdateTime)
   {
      std::lock_guard<std::recursive_mutex> lock(mSunMoonUpdateMutex);
      mPlatformPtr->GetSimulation()->GetDateTime().GetCurrentTime(aSimTime, mCurrentTime);
      UtSun::GetSunLocationWCS(mCurrentTime, mSunLoc);
      UtVec3d moonLocECI;
      UtMoon::GetLocationECI(mCurrentTime, moonLocECI);
      UtEllipsoidalEarth::ConvertTODToECEF(mCurrentTime.GetEarthAngle(), moonLocECI.GetData(), mMoonLoc.GetData());
      mLastSunMoonUpdateTime = aSimTime;
   }
}

// =================================================================================================
double WsfOpticalEnvironment::ReadRadianceValue(UtInput& aInput)
{
   std::string units;
   double      radianceValue;
   aInput.ReadValue(radianceValue);
   aInput.ValueGreater(radianceValue, 0.0);
   aInput.ReadValue(units);
   std::string::size_type slashPos1 = units.find('/');
   std::string::size_type slashPos2 = units.rfind('/');
   if ((slashPos1 != 0) && (slashPos1 != std::string::npos) && (slashPos2 != std::string::npos) &&
       ((slashPos2 - slashPos1) > 1) && ((units.size() - slashPos2) > 1))
   {
      std::string powerUnits(units.substr(0, slashPos1));
      std::string angleUnits(units.substr(slashPos1 + 1, slashPos2 - slashPos1 - 1));
      std::string areaUnits(units.substr(slashPos2 + 1));
      double      powerFactor = aInput.ConvertValue(1.0, powerUnits, UtInput::cPOWER);
      double      angleFactor = aInput.ConvertValue(1.0, angleUnits, UtInput::cAREA);
      double      areaFactor  = aInput.ConvertValue(1.0, areaUnits, UtInput::cSOLID_ANGLE);
      double      multiplier  = powerFactor / angleFactor / areaFactor;
      radianceValue *= multiplier;
   }
   else
   {
      throw UtInput::BadValue(aInput, "Unknown radiance units " + units);
   }
   return radianceValue;
}

// =================================================================================================
// This routine is called when the 'print_computed_data' is specified.
// It prints out some useful information about the data used by the model, AND it produces the code
// that defines the tables used by this class. The code should be should be cut-and-paste back in to the
// private namespace where indicated at the top of the file.
//
// This also acts as a sandbox for some testing some simple computations and verifying some fundamental computations.
void WsfOpticalEnvironment::PrintComputedData()
{
   auto out = ut::log::info() << "Integrated band information based on Sun temperature and radius.";
   out.AddNote() << "Temperature: " << cSUN_TEMPERATURE << " K";
   out.AddNote() << "Radius: " << cSUN_RADIUS * 0.001 << " km";

   BandData bandData[cBAND_COUNT];
   double   lastUpperWavelength = 0.01E-6;
   double   L;
   double   Phi;
   double   sumL = 0.0;
   { // RAII block
      auto table = out.AddNote() << "Bands:";
      for (size_t band = 0; band < cBAND_COUNT; ++band)
      {
         const char* name            = cBAND_DESC[band].mName;
         double      lowerWavelength = cBAND_DESC[band].mLowerWavelength;
         double      upperWavelength = cBAND_DESC[band].mUpperWavelength;
         // Compute data for the unnamed portion at the front of the table or for gaps between entries
         if (lowerWavelength != lastUpperWavelength)
         {
            auto line = table.AddNote() << "Band Gap:";
            ComputeAndPrintBandData("(none)", lastUpperWavelength, lowerWavelength, L, Phi, line);
            sumL += L;
         }
         auto line = table.AddNote() << "Band " << band + 1 << ":";
         ComputeAndPrintBandData(name, lowerWavelength, upperWavelength, L, Phi, line);
         sumL += L;
         lastUpperWavelength = upperWavelength;

         bandData[band].mSolarRadiance         = L;
         bandData[band].mSolarRadiantIntensity = L * UtMath::cPI * cSUN_RADIUS * cSUN_RADIUS;
         bandData[band].mSolarFlux             = Phi;
      }
      { // RAII block
         // Compute the unnamed range from the last entry up to 1000 um
         auto line = table.AddNote() << "Band Gap:";
         ComputeAndPrintBandData("(none)", lastUpperWavelength, 1000.0E-6, L, Phi, line);
         sumL += L;
      }
   }

   double R_sun = cSUN_RADIUS;
   double a_0   = cAU;

   double L_sun   = sumL;                                     // Radiance, W/m2/sr
   double M_sun   = UtMath::cPI * L_sun;                      // W/m2
   double Phi_sun = M_sun * UtMath::cFOUR_PI * R_sun * R_sun; // Radiant flux, W
   double E_e     = Phi_sun / (UtMath::cFOUR_PI * a_0 * a_0); // Flux per unit area at 1 au, W/m^2

   out.AddNote() << "Total solar radiance: " << std::scientific << std::setprecision(6) << L_sun << " W/m^2/sr";
   out.AddNote() << "Total solar flux: " << std::scientific << std::setprecision(6) << Phi_sun << " W";
   out.AddNote() << "Peak solar irradiance at Earth: " << std::fixed << std::setprecision(2) << E_e << " W/m^2";
   out.AddNote() << "Average solar irradiance at Earth: " << std::fixed << std::setprecision(2) << 0.25 * E_e << " W/m^2";
   double albedo = cAVG_EARTH_REFLECTED_FLUX_DENSITY / cAVG_EARTH_TOTAL_FLUX_DENSITY;
   out.AddNote() << "Albedo (based on design paper): " << std::fixed << std::setprecision(6) << albedo;
   {
      auto note = out.AddNote() << "Earth's apparent black body temperatures when viewed from space:";
      note.AddNote() << "Due to peak reflection (Sun at zenith): " << std::fixed << std::setprecision(2)
                     << ut::BlackBody::Temperature(albedo * E_e / UtMath::cPI) << " K";
      note.AddNote() << "Due to emission: " << std::fixed << std::setprecision(2)
                     << ut::BlackBody::Temperature(cAVG_EARTH_EMITTED_FLUX_DENSITY / UtMath::cPI) << " K";
      note.AddNote() << "Due to emission plus peak reflection: " << std::fixed << std::setprecision(2)
                     << ut::BlackBody::Temperature((albedo * E_e + cAVG_EARTH_EMITTED_FLUX_DENSITY) / UtMath::cPI)
                     << " K";
   }

   // I have no idea if this is really how we want this section to be handled.
   auto copyNote = out.AddNote() << "CUT-AND-PASTE from 'show_computed_data'" << std::scientific << std::setprecision(6);
   auto declNote = copyNote.AddNote() << "const BandData cBAND_DATA{";
   for (size_t band = 0; band < cBAND_COUNT; ++band)
   {
      declNote.AddNote() << "{ " << bandData[band].mSolarRadiance << ", " << bandData[band].mSolarRadiantIntensity
                         << ", " << bandData[band].mSolarFlux << "}, // " << cBAND_DESC[band].mName;
   }
   declNote.AddNote() << "{ " << 0.0 << ", " << 0.0 << ", " << 0.0 << " }, // dummy";
   copyNote.AddNote() << "};";
}

// =================================================================================================
//! Helper routine for PrintComputedData
void WsfOpticalEnvironment::ComputeAndPrintBandData(const char*             aName,
                                                    double                  aLowerWavelength,
                                                    double                  aUpperWavelength,
                                                    double&                 aL,
                                                    double&                 aPhi,
                                                    ut::log::MessageStream& aStream)
{
   double L = ut::BlackBody::BandRadiance(cSUN_TEMPERATURE, aLowerWavelength, aUpperWavelength);
   // Radiant intensity, W/sr
   double I = L * UtMath::cPI * cSUN_RADIUS * cSUN_RADIUS;
   // Radiant exitance, W/m^2 (assumes blackbody)
   double M = UtMath::cPI * L;
   // Radiant flux, W
   double Phi = M * (UtMath::cFOUR_PI * cSUN_RADIUS * cSUN_RADIUS);

   double      unitScale = 1.0E+6;
   std::string unitName("um");
   if (aUpperWavelength < 1.0E-6)
   {
      unitScale = 1.0E+9;
      unitName  = "nm";
   }
   aStream.AddNote() << "Name: " << aName;
   aStream.AddNote() << "Lower Wavelength: " << std::fixed << std::setprecision(1) << aLowerWavelength * unitScale
                     << ' ' << unitName;
   aStream.AddNote() << "Upper Wavelength: " << std::fixed << std::setprecision(1) << aUpperWavelength * unitScale
                     << ' ' << unitName;
   aStream.AddNote() << "Radiance: " << std::scientific << std::setprecision(6) << L << "W/m^2/sr";
   aStream.AddNote() << "Intensity: " << std::scientific << std::setprecision(6) << I << "W/sr";
   aStream.AddNote() << "Flux: " << std::scientific << std::setprecision(6) << Phi << " W";

   aL   = L;
   aPhi = Phi;
}
