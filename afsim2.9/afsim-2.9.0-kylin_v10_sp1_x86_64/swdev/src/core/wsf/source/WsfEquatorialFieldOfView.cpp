// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEquatorialFieldOfView.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfPlatform.hpp"

WsfEquatorialFieldOfView::WsfEquatorialFieldOfView()
{
   SetType("equatorial");
}

// =================================================================================================
bool WsfEquatorialFieldOfView::operator==(const WsfEquatorialFieldOfView& aRhs) const
{
   return ((mMinEqFOV == aRhs.mMinEqFOV) && (mMaxEqFOV == aRhs.mMaxEqFOV) && (mMinPolarFOV == aRhs.mMinPolarFOV) &&
           (mMaxPolarFOV == aRhs.mMaxPolarFOV));
}

bool WsfEquatorialFieldOfView::operator!=(const WsfEquatorialFieldOfView& aRhs) const
{
   return !(*this == aRhs);
}

// =================================================================================================
// virtual
bool WsfEquatorialFieldOfView::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "equatorial_field_of_view")
   {
      aInput.ReadValueOfType(mMinEqFOV, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxEqFOV, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMinEqFOV, -UtMath::cPI, mMaxEqFOV);
      aInput.ValueLessOrEqual(mMaxEqFOV, UtMath::cPI);
   }
   else if (command == "polar_field_of_view")
   {
      aInput.ReadValueOfType(mMinPolarFOV, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxPolarFOV, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMinPolarFOV, -UtMath::cPI_OVER_2, mMaxPolarFOV);
      aInput.ValueLessOrEqual(mMaxPolarFOV, UtMath::cPI_OVER_2);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
// virtual
bool WsfEquatorialFieldOfView::WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl)
{
   double cuedAz;
   double cuedEl;
   double latitude;

   const bool azElSuccess = GetCuedAzElPlatformLat(cuedAz, cuedEl, latitude);

   if (azElSuccess)
   {
      return WithinFieldOfView(aThisToTgtAz, aThisToTgtEl, cuedAz, cuedEl, latitude);
   }

   return false;
}

//! Is the target within the field of view?
//!
//! @param aThisToTgtAz [input] Azimuth of the target with respect to the device (radians).
//! @param aThisToTgtEl [input] Elevation of the target with respect to the device (radians).
//! @param aCuedAz [input] Cued azimuth of the device (radians).
//! @param aCuedEl [input] Cued elevation of the device (radians).
//! @param aLatitude [input] Latitude of the device's location (degrees).
//! @returns 'true' if within the field of view or 'false' if not.
bool WsfEquatorialFieldOfView::WithinFieldOfView(double aThisToTgtAz,
                                                 double aThisToTgtEl,
                                                 double aCuedAz,
                                                 double aCuedEl,
                                                 double aLatitude) const
{
   const double parallacticAngle = UtSphericalEarth::ConvertAzElToParallacticAngle(aCuedAz, aCuedEl, aLatitude);

   const double thisToTgtX = cos(aThisToTgtEl) * cos(aThisToTgtAz); // points out from sensor
   const double thisToTgtY = cos(aThisToTgtEl) * sin(aThisToTgtAz); // point out from right side of sensor
   const double thisToTgtZ = -sin(aThisToTgtEl);                    // points down

   const double thisToTgtEquatorialX = thisToTgtX;
   const double thisToTgtEquatorialY = cos(parallacticAngle) * thisToTgtY + sin(parallacticAngle) * thisToTgtZ;
   const double thisToTgtEquatorialZ = -sin(parallacticAngle) * thisToTgtY + cos(parallacticAngle) * thisToTgtZ;

   const double thisToTgtEquatorialAz = atan2(thisToTgtEquatorialY, thisToTgtEquatorialX);
   double       thisToTgtEquatorialPolar;
   const double xy = sqrt(thisToTgtEquatorialX * thisToTgtEquatorialX + thisToTgtEquatorialY * thisToTgtEquatorialY);
   if (xy != 0.0)
   {
      thisToTgtEquatorialPolar = -atan(thisToTgtEquatorialZ / xy);
   }
   else
   {
      thisToTgtEquatorialPolar = UtMath::cPI_OVER_2;
      if (thisToTgtEquatorialZ > 0.0)
      {
         thisToTgtEquatorialPolar = -UtMath::cPI_OVER_2;
      }
   }

   return ((thisToTgtEquatorialAz >= mMinEqFOV) && (thisToTgtEquatorialAz <= mMaxEqFOV) &&
           (thisToTgtEquatorialPolar >= mMinPolarFOV) && (thisToTgtEquatorialPolar <= mMaxPolarFOV));
}

//! Obtain the cued azimuth/elevation and the location latitude of the associated platform.
//!
//! @param aCuedAz [output] Cued azimuth (radians).
//! @param aCuedEl [output] Cued elevation (radians).
//! @param aPlatformLatDeg [output] Latitude of the platform's location (degrees).
//! @returns 'true' if successful in obtaining the values or 'false' if not.
bool WsfEquatorialFieldOfView::GetCuedAzElPlatformLat(double& aCuedAz, double& aCuedEl, double& aPlatformLatDeg) const
{
   if (!GetArticulatedPart())
   {
      throw UtException("Equatorial field of view has no valid WsfArticulatedPart associated with it.");
   }

   if (!GetArticulatedPart()->GetActualCuedOrientation(aCuedAz, aCuedEl))
   {
      ut::log::warning()
         << "WsfArticulatedPart not able to give current cue for articulated part using equatorial field of view.";
      return false;
   }

   double longitudeDeg;
   double altitude;
   GetArticulatedPart()->GetPlatform()->GetLocationLLA(aPlatformLatDeg, longitudeDeg, altitude);

   return true;
}

//! Obtain the field of view range along the equatorial axis.
//!
//! @param aMinEqFOV [output] Minimum of the range. (radians).
//! @param aMaxEqFOV [output] Maximum of the range (radians).
void WsfEquatorialFieldOfView::GetEquatorialFieldOfView(double& aMinEqFOV, double& aMaxEqFOV) const
{
   aMinEqFOV = mMinEqFOV;
   aMaxEqFOV = mMaxEqFOV;
}

//! Obtain the field of view range along the polar axis.
//!
//! @param aMinPolarFOV [output] Minimum of the range. (radians).
//! @param aMaxPolarFOV [output] Maximum of the range (radians).
void WsfEquatorialFieldOfView::GetPolarFieldOfView(double& aMinPolarFOV, double& aMaxPolarFOV) const
{
   aMinPolarFOV = mMinPolarFOV;
   aMaxPolarFOV = mMaxPolarFOV;
}

//! Set the field of view range along the equatorial axis.
//!
//! @param aMinEqFOV [input] Minimum of the range. (radians).
//! @param aMaxEqFOV [input] Maximum of the range (radians).
void WsfEquatorialFieldOfView::SetEquatorialFieldOfView(double aMinEqFOV, double aMaxEqFOV)
{
   mMinEqFOV = aMinEqFOV;
   mMaxEqFOV = aMaxEqFOV;
}

//! Set the field of view range along the polar axis.
//!
//! @param aMinPolarFOV [input] Minimum of the range. (radians).
//! @param aMaxPolarFOV [input] Maximum of the range (radians).
void WsfEquatorialFieldOfView::SetPolarFieldOfView(double aMinPolarFOV, double aMaxPolarFOV)
{
   mMinPolarFOV = aMinPolarFOV;
   mMaxPolarFOV = aMaxPolarFOV;
}
