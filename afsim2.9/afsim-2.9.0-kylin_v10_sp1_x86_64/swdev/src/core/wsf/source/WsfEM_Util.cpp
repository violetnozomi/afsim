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

#include "WsfEM_Util.hpp"

#include <algorithm>
#include <iostream>

#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
//! A pass-through to the other form, not given the altitude.
// static
bool WsfEM_Util::ComputeApparentPosition(double       aEarthRadiusScale,
                                         const double aSrcLocWCS[3],
                                         const double aTgtLocWCS[3],
                                         double       aApparentSrcLocWCS[3],
                                         double       aApparentTgtLocWCS[3])
{
   double lat;
   double lon;
   double srcAlt;
   double tgtAlt;
   UtEntity::ConvertWCSToLLA(aSrcLocWCS, lat, lon, srcAlt);
   UtEntity::ConvertWCSToLLA(aTgtLocWCS, lat, lon, tgtAlt);
   return ComputeApparentPosition(aEarthRadiusScale, aSrcLocWCS, aTgtLocWCS, srcAlt, tgtAlt, aApparentSrcLocWCS, aApparentTgtLocWCS);
}

// =================================================================================================
//! Compute the apparent position of a target, accounting for refraction.
//!
//! This routine computes the apparent position of a target using the well-known 'scaled-earth' model for refraction.
//!
//! @param aEarthRadiusScale  [input]  The scale factor for the earth radius (typically 4/3).
//! @param aSrcLocWCS         [input]  The source location.
//! @param aTgtLocWCS         [input]  The target location.
//! @param aSrcAlt            [input]  The altitude of the source, corresponding to aSrcLocWCS.
//! @param aTgtAlt            [input]  The altitude of the target, corresponding to aTgtLocWCS.
//! @param aApparentSrcLocWCS [output] The apparent location of the source when viewed from the target location.
//!                                    This may refer to the same memory as aSrcLocWCS.
//! @param aApparentTgtLocWCS [output] The apparent location of the target when viewed from the source location.
//!                                    This may refer to the same memory as aTgtLocWCS.
//! @returns 'true' if an adjustment was necessary, 'false' if not. The output values are valid in both cases.
//!
//! @note This method does NOT preserve slant range. Its' major purpose is to provide a location from with the caller
//! can compute a unit vector that points to the apparent location.
// static
bool WsfEM_Util::ComputeApparentPosition(double       aEarthRadiusScale,
                                         const double aSrcLocWCS[3],
                                         const double aTgtLocWCS[3],
                                         double       aSrcAlt,
                                         double       aTgtAlt,
                                         double       aApparentSrcLocWCS[3],
                                         double       aApparentTgtLocWCS[3])
{
   // Assume no correction necessary
   UtVec3d::Set(aApparentSrcLocWCS, aSrcLocWCS);
   UtVec3d::Set(aApparentTgtLocWCS, aTgtLocWCS);

   // Using a scale factor of the Earth's radius to account for the effects of atmospheric refraction
   // is a well known approximation, but it is only valid 'near' the Earth's surface. If both objects
   // are 'outside' the atmosphere then we bypass these calculations.

   if ((aSrcAlt > 100000.0) && (aTgtAlt > 100000.0))
   {
      return false; // Both objects are 'outside' the atmosphere
   }

   double srcToTgtLocWCS[3];
   UtVec3d::Subtract(srcToTgtLocWCS, aTgtLocWCS, aSrcLocWCS);
   double srcToTgtRange = UtVec3d::Magnitude(srcToTgtLocWCS);
   if (srcToTgtRange < 1.0)
   {
      return false; // Objects are co-located
   }

   // Use the law of cosines to compute the elevation angles in the scaled Earth frame of the target with
   // respect to the source and the source with respect to the target (Note that we are really computing
   // the elevation angle + 90 degrees). This angle is the elevation angle of the 'apparent' object in the
   // non-scaled frame.
   //
   // sideA  is the side from the Earth center to the source location.
   // sideB  is the side from the Earth center to the target location.
   // sideC  is the side from the source location to the target location.
   // angleA is the included angle between sides B and C.
   // angleB is the included angle between sides A and C.

   double earthRadius = UtSphericalEarth::cEARTH_RADIUS * aEarthRadiusScale;
   double sideA       = earthRadius + aSrcAlt;
   double sideB       = earthRadius + aTgtAlt;
   double sideC       = srcToTgtRange;
   double sideA2      = sideA * sideA;
   double sideB2      = sideB * sideB;
   double sideC2      = sideC * sideC;
   double cosAngleA   = (sideB2 + sideC2 - sideA2) / (2.0 * sideB * sideC);
   cosAngleA          = UtMath::Limit(cosAngleA, 1.0);
   double cosAngleB   = (sideA2 + sideC2 - sideB2) / (2.0 * sideA * sideC);
   cosAngleB          = UtMath::Limit(cosAngleB, 1.0);

   // Using local versions of the WCS position vectors, modify the target WCS position vector so the
   // resulting elevation matches the apparent elevation.
   //
   // NOTE: This computation does NOT preserve the length of the slant range, but that is not important.
   // What we're after is unit pointing vector. The slant range is carried along separately as a scalar.

   // Compute the separation angle
   double apparentSrcLocWCS[3];
   double apparentTgtLocWCS[3];
   double srcLocMag = UtVec3d::Normalize(apparentSrcLocWCS, aSrcLocWCS);
   double tgtLocMag = UtVec3d::Normalize(apparentTgtLocWCS, aTgtLocWCS);

   double cosAngleC = UtVec3d::DotProduct(apparentSrcLocWCS, apparentTgtLocWCS);
   if (fabs(cosAngleC) > 0.99999999) // ~acos(0.01 deg)
   {
      return false; // Objects are directly above/below each other
   }

   // Get the approximate source and target positions in an unscaled round earth.
   earthRadius      = std::min(srcLocMag - aSrcAlt, tgtLocMag - aTgtAlt);
   double srcRadius = earthRadius + aSrcAlt;
   double tgtRadius = earthRadius + aTgtAlt;

   // Compute the height adjustment for the source-to-target view
   sideA                      = srcRadius;
   sideB2                     = (sideA * sideA) + (sideC * sideC) - (2.0 * sideA * sideC * cosAngleB);
   sideB                      = sqrt(std::max(sideB2, tgtRadius));
   double tgtHeightAdjustment = sideB - tgtRadius;
   UtVec3d::Multiply(aApparentTgtLocWCS, apparentTgtLocWCS, tgtLocMag + tgtHeightAdjustment);

   // Compute the height adjustment for the target-to-source view
   sideB                      = tgtRadius;
   sideA2                     = (sideB * sideB) + (sideC * sideC) - (2.0 * sideB * sideC * cosAngleA);
   sideA                      = sqrt(std::max(sideA2, srcRadius));
   double srcHeightAdjustment = sideA - srcRadius;
   UtVec3d::Multiply(aApparentSrcLocWCS, apparentSrcLocWCS, srcLocMag + srcHeightAdjustment);

   return true;
}

// =================================================================================================
//! Determine the azimuth and elevation of a viewer with respect to the target entity.
//! This is a helper used by the signature script query methods that return a signature value.
//!
//! @param aTargetPtr         [input]  Pointer to the target entity.
//! @param aViewerLocWCS      [input]  Location of the viewer.
//! @param aEarthRadiusScale  [input]  The scale factor for the earth radius (typically 4/3).
//! @param aTargetToViewerAz  [output] The azimuth of the viewer with respect to the target.
//! @param aTargetToViewerEl  [output] The elevation of the viewer with respect to the target.
void WsfEM_Util::ComputeViewerAspect(UtEntity*    aTargetPtr,
                                     const double aViewerLocWCS[3],
                                     double       aEarthRadiusScale,
                                     double&      aTargetToViewerAz,
                                     double&      aTargetToViewerEl)
{
   double targetToViewerLocWCS[3];

   if (aEarthRadiusScale == 1.0)
   {
      aTargetPtr->GetRelativeLocationWCS(aViewerLocWCS, targetToViewerLocWCS);
   }
   else
   {
      double targetLocWCS[3];
      aTargetPtr->GetLocationWCS(targetLocWCS);
      double apparentViewerLocWCS[3];
      double apparentTargetLocWCS[3];

      ComputeApparentPosition(aEarthRadiusScale, aViewerLocWCS, targetLocWCS, apparentViewerLocWCS, apparentTargetLocWCS);
      aTargetPtr->GetRelativeLocationWCS(apparentViewerLocWCS, targetToViewerLocWCS);
   }
   aTargetPtr->ComputeAspect(targetToViewerLocWCS, aTargetToViewerAz, aTargetToViewerEl);
}

// =================================================================================================
//! Determine if the line of sight between two objects is masked by the horizon.
//!
//! @param aLat1             The latitude of the first object.
//! @param aLon1             The longitude of the first object.
//! @param aAlt1             The MSL altitude of the first object.
//! @param aTrnHgt1          The MSL height of the terrain under the first object.
//! @param aLat2             The latitude of the second object.
//! @param aLon2             The longitude of the second object.
//! @param aAlt2             The MSL altitude of the second object.
//! @param aTrnHgt2          The MSL height of the terrain under the second object.
//! @param aEarthRadiusScale The scale factor for the earth radius (typically 4/3).
//!
//! @returns true if the line of sight between the two objects is masked by the horizon.
// static
bool WsfEM_Util::MaskedByHorizon(double aLat1,
                                 double aLon1,
                                 double aAlt1,
                                 double aTrnHgt1,
                                 double aLat2,
                                 double aLon2,
                                 double aAlt2,
                                 double aTrnHgt2,
                                 double aEarthRadiusScale)
{
   bool maskedByHorizon = true;       // Assume at least one object is below ground
   if (((aAlt1 - aTrnHgt1) > -1.0) && // Both objects are above ground (with a little slop for safety)
       ((aAlt2 - aTrnHgt2) > -1.0))
   {
      // The simple horizon masking check assumes a smooth bald Earth and assumes that any object below 0 MSL
      // cannot see anything, but an object can be above ground but still below sea level! If such a situation
      // occurs the altitudes will be shifted up (preserving the altitude difference) to prevent problems.
      //
      // Just remember this whole horizon checking stuff is just one big approximation. We prefer to err on the
      // side of not indicating that masking has occurred.

      if ((aAlt1 < 0.0) || (aAlt2 < 0.0))
      {
         // At least one object is below MSL... shift up by the one that is most below MSL.
         double altShift = aTrnHgt1; // Assume first object is most below MSL
         if (aAlt2 < aAlt1)
         {
            altShift = aTrnHgt2; // Second object is most below MSL
         }
         // The terrain height we are shifting by is a negative number, so it must be subtracted to shift upwards!
         // (i.e. if the MSL altitude is negative and the height above terrain is positive, then the terrain height
         // must be more negative then altitude!)
         aAlt1 -= altShift;
         aAlt2 -= altShift;
      }
      maskedByHorizon = UtSphericalEarth::MaskedByHorizon(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aEarthRadiusScale);
   }
   return maskedByHorizon;
}

// =================================================================================================
//! Convert a string representing a infrared band to its equivalent enumeration.
//! @param aBand   [output] The resulting enumeration.
//! @param aString [input]  The string to converted.
//! @returns 'true' if successful or 'false' if the input string did not represent a valid polarization.
// static
bool WsfEM_Util::StringToEnum(WsfEM_Types::InfraredBand& aBand, const std::string& aString)
{
   bool ok = true;
   if ((aString == "default") || aString.empty())
   {
      aBand = WsfEM_Types::cIRBAND_DEFAULT;
   }
   else if (aString == "short")
   {
      aBand = WsfEM_Types::cIRBAND_SHORT;
   }
   else if (aString == "medium")
   {
      aBand = WsfEM_Types::cIRBAND_MEDIUM;
   }
   else if (aString == "long")
   {
      aBand = WsfEM_Types::cIRBAND_LONG;
   }
   else if (aString == "very_long")
   {
      aBand = WsfEM_Types::cIRBAND_VERY_LONG;
   }
   else
   {
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Convert a string representing a infrared band to its equivalent enumeration.
//! @param aBand   [output] The resulting enumeration.
//! @param aString [input]  The string to converted.
//! @returns 'true' if successful or 'false' if the input string did not represent a valid polarization.
// static
bool WsfEM_Util::StringToEnum(WsfEM_Types::OpticalBand& aBand, const std::string& aString)
{
   bool ok = true;
   if ((aString == "default") || aString.empty())
   {
      aBand = WsfEM_Types::cOPT_DEFAULT;
   }
   else if (aString == "short")
   {
      aBand = WsfEM_Types::cOPT_IR_SHORT;
   }
   else if (aString == "medium")
   {
      aBand = WsfEM_Types::cOPT_IR_MEDIUM;
   }
   else if (aString == "long")
   {
      aBand = WsfEM_Types::cOPT_IR_LONG;
   }
   else if (aString == "very_long")
   {
      aBand = WsfEM_Types::cOPT_IR_VERY_LONG;
   }
   else if (aString == "visual")
   {
      aBand = WsfEM_Types::cOPT_VISUAL;
   }
   else
   {
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Convert a string representing a polarization to its equivalent enumeration.
//! @param aPolarization [output] The resulting enumeration.
//! @param aString       [input] The string to converted.
//! @returns 'true' if successful or 'false' if the input string did not represent a valid polarization.
// static
bool WsfEM_Util::StringToEnum(WsfEM_Types::Polarization& aPolarization, const std::string& aString)
{
   bool ok = true;
   if ((aString == "default") || aString.empty())
   {
      aPolarization = WsfEM_Types::cPOL_DEFAULT;
   }
   else if (aString == "horizontal")
   {
      aPolarization = WsfEM_Types::cPOL_HORIZONTAL;
   }
   else if (aString == "vertical")
   {
      aPolarization = WsfEM_Types::cPOL_VERTICAL;
   }
   else if (aString == "slant_45")
   {
      aPolarization = WsfEM_Types::cPOL_SLANT_45;
   }
   else if (aString == "slant_135")
   {
      aPolarization = WsfEM_Types::cPOL_SLANT_135;
   }
   else if (aString == "left_circular")
   {
      aPolarization = WsfEM_Types::cPOL_LEFT_CIRCULAR;
   }
   else if (aString == "right_circular")
   {
      aPolarization = WsfEM_Types::cPOL_RIGHT_CIRCULAR;
   }
   else
   {
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Convert an infrared band enumeration to an equivalent string.
//! @param aBand [input] The band whose string value is desired.
//! @returns The string value of the enumeration.
std::string WsfEM_Util::EnumToString(WsfEM_Types::InfraredBand aBand)
{
   std::string enumString("unknown");
   if (aBand == WsfEM_Types::cIRBAND_DEFAULT)
   {
      enumString = "default";
   }
   else if (aBand == WsfEM_Types::cIRBAND_SHORT)
   {
      enumString = "short";
   }
   else if (aBand == WsfEM_Types::cIRBAND_MEDIUM)
   {
      enumString = "medium";
   }
   else if (aBand == WsfEM_Types::cIRBAND_LONG)
   {
      enumString = "long";
   }
   else if (aBand == WsfEM_Types::cIRBAND_VERY_LONG)
   {
      enumString = "very_long";
   }
   return enumString;
}

// =================================================================================================
//! Convert an optical band enumeration to an equivalent string.
//! @param aBand [input] The band whose string value is desired.
//! @returns The string value of the enumeration.
std::string WsfEM_Util::EnumToString(WsfEM_Types::OpticalBand aBand)
{
   std::string enumString("unknown");
   if (aBand == WsfEM_Types::cOPT_DEFAULT)
   {
      enumString = "default";
   }
   else if (aBand == WsfEM_Types::cOPT_IR_SHORT)
   {
      enumString = "short";
   }
   else if (aBand == WsfEM_Types::cOPT_IR_MEDIUM)
   {
      enumString = "medium";
   }
   else if (aBand == WsfEM_Types::cOPT_IR_LONG)
   {
      enumString = "long";
   }
   else if (aBand == WsfEM_Types::cOPT_IR_VERY_LONG)
   {
      enumString = "very_long";
   }
   else if (aBand == WsfEM_Types::cOPT_VISUAL)
   {
      enumString = "visual";
   }
   return enumString;
}

// =================================================================================================
//! Convert a polarization enumeration to an equivalent string.
//! @param aPolarization [input] The polarization whose string value is desired.
//! @returns The string value of the enumeration.
// static
std::string WsfEM_Util::EnumToString(WsfEM_Types::Polarization aPolarization)
{
   std::string enumString("unknown");
   if (aPolarization == WsfEM_Types::cPOL_DEFAULT)
   {
      enumString = "default";
   }
   else if (aPolarization == WsfEM_Types::cPOL_HORIZONTAL)
   {
      enumString = "horizontal";
   }
   else if (aPolarization == WsfEM_Types::cPOL_VERTICAL)
   {
      enumString = "vertical";
   }
   else if (aPolarization == WsfEM_Types::cPOL_SLANT_45)
   {
      enumString = "slant_45";
   }
   else if (aPolarization == WsfEM_Types::cPOL_SLANT_135)
   {
      enumString = "slant_135";
   }
   else if (aPolarization == WsfEM_Types::cPOL_LEFT_CIRCULAR)
   {
      enumString = "left_circular";
   }
   else if (aPolarization == WsfEM_Types::cPOL_RIGHT_CIRCULAR)
   {
      enumString = "right_circular";
   }
   return enumString;
}
