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

// Reference
// The effect of atmospheric optical turbulence on laser communication systems—Part 1, Theory
// Thomas C Farrell, Air Force Research Laboratory, Space Vehicles Directorate 3550 Aberdeen Ave., SE,
// Kirtland AFB, NM 87117-5776; Unpublished.

#include "WsfAtmosphericTurbulence.hpp"

#include <string>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfMil.hpp"
#include "WsfScenario.hpp"

namespace wsf
{

namespace
{
/**
 *  Calculate the height from platform to target above the surface
 *  of the earth.
 *
 *  @param aRangeFromPlatform Position along path [0, SlantRange] from platform to target (m)
 *  @param aTotalSlantRange   Total slant range from platform to target (m)
 *  @param aTargetHeight      Target height (m)
 *  @param aPlatformHeight    Platform height (m)
 */
double GetIntermediateHeight(double aRangeFromPlatform, double aTotalSlantRange, double aTargetHeight, double aPlatformHeight)
{
   static const double earthRadius = UtSphericalEarth::cEARTH_RADIUS;

   // assume spherical earth, compute positions relative to earth center
   double targetRadius = earthRadius + aTargetHeight;

   double platformRadius = earthRadius + aPlatformHeight;

   // use law of cosines to compute angle between platform position vector and
   // platform-to-target vector
   // CosBeta = (TotalSlantRange.^2 + platformRadius.^2 - targetRadius.^2)./
   //(2*TotalSlantRange.*platformRadius);
   double CosBeta = (pow(aTotalSlantRange, 2) + pow(platformRadius, 2) - pow(targetRadius, 2)) /
                    (2 * aTotalSlantRange * platformRadius);

   // use law of cosines to compute height above earth at given range from platform
   // LaserHeight = sqrt(RangeFromPlatform.^2 + platformRadius.^2 - 2*RangeFromPlatform.*platformRadius.*CosBeta) - earthRadius;
   return (sqrt(pow(aRangeFromPlatform, 2) + pow(platformRadius, 2) - 2 * aRangeFromPlatform * platformRadius * CosBeta) -
           earthRadius);
}
} // namespace

AtmosphericTurbulence::AtmosphericTurbulence(const WsfScenario& aScenario)
   : mScenario(aScenario)
   , mCN2_Ptr(nullptr)
{
}

/**
 *  Calculate CN^2 for a Hufnagel-Valley turbulence profile
 *
 *  @param aHeight [in]: height (m)
 *  @return Index of refraction structure constant (m^-2/3)
 */
double CN2_Types::hv57(double aHeight)
{
   // wind correlating factor; selected as 21 for the HV 5/7 model
   // W = 21;
   double W = 21.0;

   // CN2 = 0.00594*(W/27)^2*(height.*1E-5).^10 .* exp(-height./1000) + ...
   //     2.7E-16 * exp(-height./(1.5*1000)) + 1.7E-14*exp(-height./(0.1*1000));
   //  Note that in the documentation the (equivalent) alternate form of this equation is given as
   // CN2 = 8.2E-26*W^2*(height/1000)^10 * exp(-height./1000) + ...
   //     2.7E-16 * exp(-height./(1500)) + 1.7E-14*exp(-height/100));
   double CN2 = 0.00594 * pow(W / 27, 2.0) * pow(aHeight * 1e-5, 10.0) * exp(-aHeight / 1000) +
                2.7e-16 * exp(-aHeight / (1.5 * 1000)) + 1.7e-14 * exp(-aHeight / (0.1 * 1000));

   return CN2;
}

const std::string& AtmosphericTurbulence::GetCN2Form()
{
   return GetCN2().GetName();
}

void AtmosphericTurbulence::SetCN2Form(const std::string& aCN2form)
{
   mCN2_Ptr = CN2_Types::Get(mScenario).Find(aCN2form);
   if (mCN2_Ptr == nullptr)
   {
      std::string what = "AtmosphericTurbulence: unknown atmospheric_structure / cn2form: " + aCN2form;
      throw UtException(what);
   }
}

const CN2& AtmosphericTurbulence::GetCN2()
{
   if (mCN2_Ptr == nullptr)
   {
      // Default is to use hv/57
      SetCN2Form("hv57");
   }
   return *mCN2_Ptr;
}

//! Compute the average beam spread angle at a target, produced by atmospheric turbulence on a monochromatic beam of
//! given wavelength (e.g., from a laser).
//! @param aSlantRange The slant range to target.
//! @param aTargetHeight The target height.
//! @param aPlatformHeight The height of the platform transmitting the beam.
//! @param aWavelength The wavelenght of the transmitting beam (assumed monochromatic).
double wsf::AtmosphericTurbulence::BeamSpreadAngle(double aSlantRange,
                                                   double aTargetHeight,
                                                   double aPlatformHeight,
                                                   double aWavelength)
{
   TurbulentBeamSpreadIntegrand integrand(aSlantRange, aTargetHeight, aPlatformHeight, aWavelength, GetCN2());

   // compute the value of the integrand - Need low tolerance
   // Typical values of integrand can be on the order of 1e-17 or smaller
   double intRes = integrand.Integrate(0.0, 1.0, 5, 1e-18);
   double k      = UtMath::cTWO_PI / aWavelength;
   double rho_0  = pow(1.4572 * k * k * aSlantRange * intRes, -0.6);
   double angle  = 2.0 / (k * rho_0);
   return angle;
}

// virtual
bool AtmosphericTurbulence::ProcessInput(UtInput& aInput)
{
   bool        processed = false;
   std::string command   = aInput.GetCommand();
   if ((command == "atmospheric_structure") || (command == "CN2_form"))
   {
      std::string cn2form;
      aInput.ReadValueQuoted(cn2form); // allows optional use of quotes.
      SetCN2Form(cn2form);
      processed = true;
   }
   return processed;
}

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
CN2_Types& CN2_Types::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetCN2_Types();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const CN2_Types& CN2_Types::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetCN2_Types();
}

// =================================================================================================
CN2_Types::CN2_Types(WsfScenario& aScenario)
   : WsfObjectTypeList<CN2>(aScenario, cREDEFINITION_ALLOWED, "cn2")
{
   Add("hv57", ut::make_unique<CN2>(hv57));
}

AtmosphericTurbulence::TurbulentBeamSpreadIntegrand::TurbulentBeamSpreadIntegrand(double          aSlantRange,
                                                                                  double          aTargetHeight,
                                                                                  double          aPlatformHeight,
                                                                                  double          aWavelength,
                                                                                  const wsf::CN2& aCN2form)
   : mSlantRange(aSlantRange)
   , mTargetHeight(aTargetHeight)
   , mPlatformHeight(aPlatformHeight)
   , mWavelength(aWavelength)
   , mCN2form(aCN2form)
{
}

// virtual
//! Given a normalized range (aZ = z/L), calculate the integral in the expression for scale length rho_0.
double AtmosphericTurbulence::TurbulentBeamSpreadIntegrand::Function(double aZ)
{
   double height = GetIntermediateHeight(aZ * mSlantRange, mSlantRange, mTargetHeight, mPlatformHeight);
   double CN2    = mCN2form(height);

   // integrand in equation for spherical wave rho_0 except place the point source at the
   // target (beam projection) instead of at the transmitter - replace (z/L) with (1 - z/L)
   // from Eq. 116 page 209 in Andrews Phillips, Laser Beam Propagation through Random Media, 2005.
   return CN2 * pow(1.0 - aZ, 5.0 / 3.0);
}

} // namespace wsf
