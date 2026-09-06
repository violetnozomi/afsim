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

#include "UtEllipsoidalEarth.hpp"

#include <sstream>
#include <string>

#include "UtCentralBodyEllipsoid.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalCentralBody.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"

// this method is based on the IBM java code at
// http://www.ibm.com/developerworks/java/library/j-coordconvert/index.html, in listings 8 and 9. I am pretty sure this
// just ignores a lot of the weirdness with the Norwegian and Svalbard Zones. static
void UtEllipsoidalEarth::ConvertUTMToLL(const unsigned int aZone,
                                        const double       aEastings,
                                        const double       aNorthings,
                                        const bool         aNorthernHemisphere,
                                        double&            aLat,
                                        double&            aLon)
{
   double northings = aNorthings;
   if (!aNorthernHemisphere)
   {
      northings = 10000000 - aNorthings;
   }
   double e    = 0.081819191;
   double e1sq = 0.006739497;
   double k0   = 0.9996;

   double arc = northings / k0;
   double mu  = arc / (UtEarth::cA * (1 - pow(e, 2) / 4.0 - 3 * pow(e, 4) / 64.0 - 5 * pow(e, 6) / 256.0));

   double ei = (1 - pow(1 - e * e, 0.5)) / (1 + pow(1 - e * e, 0.5));
   double ca = 3 * ei * 0.5 - 27 * pow(ei, 3) / 32.0;

   double cb   = 21 * pow(ei, 2) / 16.0 - 55 * pow(ei, 4) / 32.0;
   double cc   = 151 * pow(ei, 3) / 96.0;
   double cd   = 1097 * pow(ei, 4) / 512.0;
   double phi1 = mu + ca * sin(2 * mu) + cb * sin(4 * mu) + cc * sin(6 * mu) + cd * sin(8 * mu);

   double n0 = UtEarth::cA / pow((1 - pow(e * sin(phi1), 2)), 0.5);

   double r0    = UtEarth::cA * (1 - e * e) / pow((1 - pow(e * sin(phi1), 2)), 1.5);
   double fact1 = n0 * tan(phi1) / r0;

   double _a1   = 500000 - aEastings;
   double dd0   = _a1 / (n0 * k0);
   double fact2 = dd0 * dd0 * 0.5;

   double t0    = pow(tan(phi1), 2);
   double Q0    = e1sq * pow(cos(phi1), 2);
   double fact3 = (5 + 3 * t0 + 10 * Q0 - 4 * Q0 * Q0 - 9 * e1sq) * pow(dd0, 4) / 24.0;

   double fact4 = (61 + 90 * t0 + 298 * Q0 + 45 * t0 * t0 - 252 * e1sq - 3 * Q0 * Q0) * pow(dd0, 6) / 720.0;

   double lof1 = _a1 / (n0 * k0);
   double lof2 = (1 + 2 * t0 + Q0) * pow(dd0, 3) / 6.0;
   double lof3 = (5 - 2 * Q0 + 28 * t0 - 3 * pow(Q0, 2.0) + 8 * e1sq + 24 * pow(t0, 2)) * pow(dd0, 5) / 120.0;
   double _a2  = (lof1 - lof2 + lof3) / cos(phi1);
   double _a3  = _a2 * UtMath::cDEG_PER_RAD;
   aLat        = (phi1 - fact1 * (fact2 + fact3 + fact4)) * UtMath::cDEG_PER_RAD;
   if (!aNorthernHemisphere)
   {
      aLat = -aLat;
   }
   if (aZone != 0)
   {
      aLon = (6 * aZone - 183.0) - _a3;
   }
   else
   {
      aLon = 3 - _a3;
   }
}

// Note: This method was commented out earlier because of "instability" on UNIX systems (see Jon Marjamaa).
// this method is based on the IBM java code at
// http://www.ibm.com/developerworks/java/library/j-coordconvert/index.html, in listings 2, 3, 4, 5 and 6. I am pretty
// sure this just ignores a lot of the weirdness with the Norwegian and Svalbard Zones. static
void UtEllipsoidalEarth::ConvertLLToUTM(const double  aLat,
                                        const double  aLon,
                                        unsigned int& aLonZone,
                                        bool&         aNorthernHemisphere,
                                        double&       aEastings,
                                        double&       aNorthings)
{
   aNorthernHemisphere = (aLat >= 0) ? true : false;

   double A0 = 6367449.146;
   double B0 = 16038.42955;
   double C0 = 16.83261333;
   double D0 = 0.021984404;
   double E0 = 0.000312705;

   double sin1 = 4.84814E-06;

   double e    = 0.081819191;
   double e1sq = 0.006739497;
   double k0   = 0.9996;

   double latRad = aLat * UtMath::cRAD_PER_DEG;

   double nu = UtEarth::cA / pow(1 - pow(e * sin(latRad), 2), 0.5);

   double var1;
   if (aLon < 0.0)
   {
      var1 = ((int)((180 + aLon) / 6.0)) + 1;
   }
   else
   {
      var1 = ((int)(aLon / 6.0)) + 31;
   }
   double var2 = (6 * var1) - 183;
   double var3 = aLon - var2;
   double p    = var3 * 3600 / 10000.0;
   double S  = A0 * latRad - B0 * sin(2 * latRad) + C0 * sin(4 * latRad) - D0 * sin(6 * latRad) + E0 * sin(8 * latRad);
   double K1 = S * k0;
   double K2 = nu * sin(latRad) * cos(latRad) * pow(sin1, 2) * k0 * (100000000) * 0.5;
   double K3 = ((pow(sin1, 4) * nu * sin(latRad) * pow(cos(latRad), 3)) / 24.0 *
                (5 - pow(tan(latRad), 2) + 9 * e1sq * pow(cos(latRad), 3)) / 24.0 * pow(e1sq, 2) * pow(cos(latRad), 4)) *
               k0 * 10000000000000000.0;

   double K4 = nu * cos(latRad) * sin1 * k0 * 10000;

   double K5 = pow(sin1 * cos(latRad), 3) * (nu / 6.0) * (1 - pow(tan(latRad), 2) + e1sq * pow(cos(latRad), 2.0)) * k0 *
               1000000000000.0;

   //   double A6 = pow(p * sin1, 6.0) * nu * sin(latRad) * pow(cos(latRad), 5) / 720.0) *
   //               (61 - 58 * pow(tan(latRad), 2) + pow(tan(latRad), 4) + 270.0 *
   //               e1sq * pow(cos(latRad), 2) - 330 * e1sq *
   //               pow(sin(latRad), 2)) * k0 * (1E+24);

   // aLonZone = getLonZone(aLon);
   aLonZone = 0;

   if (aLon < 0.0)
   {
      aLonZone = (unsigned int)(((180.0 + aLon) / 6.0) + 1);
   }
   else
   {
      aLonZone = (unsigned int)((aLon / 6.0) + 31);
   }

   if (aLonZone > 60)
   {
      aLonZone = 1; // Quickly handles the special case of lon == 180.0;
   }

   // aEastings = getEasting();
   aEastings = 500000 + (K4 * p + K5 * pow(p, 3));

   // aNorthings = getNorthing(aLat);
   aNorthings = K1 + K2 * p * p + K3 * pow(p, 4);
   if (!aNorthernHemisphere)
   {
      aNorthings += 10000000;
   }
}

// Lat zones are 8 degrees high (TODO handle polar regions)
namespace
{
const char sGridZones[20]      = {'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M',  // Southern Hemisphere
                             'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'}; // Northern Hemisphere
const char sAllowedLetters[24] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M',
                                  'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const int  sLatLetterOffset[6] = {5, 0, 5, 0, 5, 0};
const int  sLonLetterOffset[6] = {16, 0, 8, 16, 0, 8};
} // namespace

//! Convert a location in lat, lon format to MGRS.
//! @param aLat The latitude of the location to convert.
//! @param aLon The longitude of the location to convert.
//! @param aLocationMGRS The location expressed in MGRS format.
void UtEllipsoidalEarth::ConvertLLToMGRS(const double aLat, const double aLon, std::string& aLocationMGRS)
{
   // MGRS is an extension of UTM, providing character designations for zones
   unsigned lonZone;
   bool     isNorthernHemisphere;
   double   easting;
   double   northing;

   ConvertLLToUTM(aLat, aLon, lonZone, isNorthernHemisphere, easting, northing);

   char gridLatZone = 0;

   if (fabs(aLat) < 75.0)
   {
      int latZone = (int)(aLat / 8.0);
      if (aLat >= 0.0)
      {
         gridLatZone = sGridZones[latZone + 10];
      }
      else
      {
         gridLatZone = sGridZones[latZone + 9];
      }
   }
   else
   {
      // TODO handle polar regions.
   }

   int letterSet = lonZone % 6;
   int lonStart  = sLonLetterOffset[letterSet];
   int latStart  = sLatLetterOffset[letterSet];
   int lonOffset = (int)(easting / 100000.0) - 1; // start at 100000
   int latOffset = (int)(northing / 100000.0) % 20;

   int latLetterIndex = (latStart + latOffset); // A-V, omitting I and O, sometimes starting at F
   if (latLetterIndex > 19)                     // "V"
   {
      latLetterIndex -= 20; // Start over at "A"
   }
   int lonLetterIndex = (lonStart + lonOffset); // A-Z, omitting I and O, sometimes starting at J or S
   if (lonLetterIndex > 24)                     // "Z"
   {
      lonLetterIndex -= 25; // Start over at "A"
   }
   char lonChar = sAllowedLetters[lonLetterIndex];
   char latChar = sAllowedLetters[latLetterIndex];

   double eastOffsetD  = fmod(easting, 100000.0);
   double northOffsetD = fmod(northing, 100000.0);
   int    eastOffset   = (int)eastOffsetD;
   int    northOffset  = (int)northOffsetD;

   // Leading zeros, if any.
   int eastZeroes  = 4;
   int northZeroes = 4;
   if (eastOffsetD > 1)
   {
      eastZeroes = 4 - (int)log10(eastOffsetD);
   }
   if (northOffsetD > 1)
   {
      northZeroes = 4 - (int)log10(northOffsetD);
   }

   std::ostringstream oss;
   oss << lonZone << gridLatZone << lonChar << latChar;
   for (int i = 0; i < eastZeroes; ++i)
   {
      oss << '0';
   }
   oss << eastOffset;
   for (int i = 0; i < northZeroes; ++i)
   {
      oss << '0';
   }
   oss << northOffset;
   aLocationMGRS = oss.str();
}

//! Convert a location MGRS format to lat, lon.
//! @param aLocationMGRS The location expressed in MGRS format.
//! @param aLat The latitude of the location to convert.
//! @param aLon The longitude of the location to convert.
//! @return Whether the location can be converted.  This may either be because
//! the form of the MGRS location is invalid, or because the latitude is too high / low
//! and the conversion is not supported (Universal Polar Stereographic, above lat 84N
//! and below lat -80).
//! @note We assume at least 4-5 characters in the location (100 km resolution).
bool UtEllipsoidalEarth::ConvertMGRSToLL(const std::string& aLocationMGRS, double& aLat, double& aLon)
{
   int               lonZone;
   int               next = 2;
   std::stringstream ss;

   std::string mgrsLoc = aLocationMGRS; // copy to a local temp to lose the const qualifier
   UtStringUtil::ToUpper(mgrsLoc);      // allow lower case letters

   if ((mgrsLoc.size() < 4) || (isdigit(mgrsLoc[0]) == 0)) // first character must be a digit, and the location must
                                                           // have at least four characters
   {
      return false;
   }

   if (isalpha(mgrsLoc[1]) != 0)
   {
      ss.str(mgrsLoc.substr(0, 1).c_str());
      next = 1;
   }
   else if (isdigit(mgrsLoc[1]) != 0)
   {
      ss.str(mgrsLoc.substr(0, 2).c_str());
      next = 2;
   }
   else // Second character must be alphanumeric.
   {
      return false;
   }

   ss >> lonZone;
   if ((lonZone > 60) || (lonZone == 0)) // zone out of bounds
   {
      return false;
   }

   ss.str("");
   ss.clear();

   char gridZoneLat = mgrsLoc[next++];

   // find the numeric equivalent
   int i = 0;
   while (i < 20)
   {
      if (gridZoneLat == sGridZones[i])
      {
         break;
      }
      ++i;
   }
   if (i == 20) // gridzone is not valid
   {
      return false;
   }

   int gridZoneIndex = (i - 10);

   // Next is 100000 square
   char lonLetter = mgrsLoc[next++];
   char latLetter = mgrsLoc[next++];
   int  letterSet = lonZone % 6;
   int  lonStart  = sLonLetterOffset[letterSet];
   int  latStart  = sLatLetterOffset[letterSet];

   int lonGridBound = 0;
   while (lonGridBound < 24) // A-Z, except I and O
   {
      if (lonLetter == sAllowedLetters[lonGridBound])
      {
         break;
      }
      ++lonGridBound;
   }
   if (lonGridBound == 24) // longitude grid bound is not valid
   {
      return false;
   }

   int latGridBound = 0;
   while (latGridBound < 20) // A-V, except I and O
   {
      if (latLetter == sAllowedLetters[latGridBound])
      {
         break;
      }
      ++latGridBound;
   }
   if (latGridBound == 20) // latitude grid bound is not valid
   {
      return false;
   }

   if (latGridBound >= latStart)
   {
      latGridBound -= latStart;
   }
   else
   {
      latGridBound += (19 - latStart + 1);
   }

   if (lonGridBound >= lonStart)
   {
      lonGridBound -= lonStart;
   }
   else
   {
      lonGridBound += (24 - lonStart + 1);
   }

   // What are offset eastings and northings?
   int digits = static_cast<int>(mgrsLoc.size()) - next;

   // digits must be between 0 and 10
   if ((digits > 10) || ((digits % 2) != 0))
   {
      return false;
   }

   double easting  = 0.0;
   double northing = 0.0;
   int    numSize  = digits / 2;

   if (numSize > 0)
   {
      std::string eastingStr = mgrsLoc.substr(next, numSize);
      // All characters must be digits.
      for (char c : eastingStr)
      {
         if (isdigit(c) == 0)
         {
            return false;
         }
      }
      next += numSize;
      std::string northingStr = mgrsLoc.substr(next, numSize);
      // All characters must be digits.
      for (char c : northingStr)
      {
         if (isdigit(c) == 0)
         {
            return false;
         }
      }
      ss.str(eastingStr.c_str());
      ss >> easting;
      ss.str("");
      ss.clear();
      ss.str(northingStr.c_str());
      ss >> northing;

      double multiple = pow(10.0, 5 - numSize);
      easting *= multiple;
      northing *= multiple;
   }

   northing += latGridBound * 100000.0;
   easting += (lonGridBound + 1) * 100000.0;

   bool northernHemisphere = gridZoneLat >= 'N';

   // Add "Major Northing"; Each Lat Zone Adds 2000000.
   double majorNorthingOffset = 0.0;

   if (northernHemisphere)
   {
      // The most straightforward way to find the major northing
      // is with if-then-else statements.
      if (gridZoneIndex == 9) // 'X'
      {
         if (latGridBound <= 11) // 'P'
         {
            majorNorthingOffset = 8000000.0;
         }
         else
         {
            majorNorthingOffset = 6000000.0;
         }
      }
      else if (gridZoneIndex >= 6) // 'U'
      {
         if ((gridZoneIndex > 6) || (latGridBound < 2)) //'C'
         {
            majorNorthingOffset = 6000000.0;
         }
         else
         {
            majorNorthingOffset = 4000000.0;
         }
      }
      else if (gridZoneIndex >= 4) // 'S'
      {
         if ((gridZoneIndex > 4) || (latGridBound < 4)) //'E'
         {
            majorNorthingOffset = 4000000.0;
         }
         else
         {
            majorNorthingOffset = 2000000.0;
         }
      }
      else if (gridZoneIndex >= 2) // 'Q'
      {
         if ((gridZoneIndex > 2) || (latGridBound < 6)) //'G'
         {
            majorNorthingOffset = 2000000.0;
         }
         else
         {
            majorNorthingOffset = 0000000.0;
         }
      }
   }
   else
   {
      if (gridZoneIndex >= -3) // 'K'
      {
         if ((gridZoneIndex > -3) || (latGridBound < 2)) //'C'
         {
            majorNorthingOffset = 8000000.0;
         }
         else
         {
            majorNorthingOffset = 6000000.0;
         }
      }
      else if (gridZoneIndex >= -5) // 'U'
      {
         if ((gridZoneIndex > -5) || (latGridBound < 4)) //'E'
         {
            majorNorthingOffset = 6000000.0;
         }
         else
         {
            majorNorthingOffset = 4000000.0;
         }
      }
      else if (gridZoneIndex >= -7) // 'S'
      {
         if ((gridZoneIndex > -7) || (latGridBound < 6)) //'G'
         {
            majorNorthingOffset = 4000000.0;
         }
         else
         {
            majorNorthingOffset = 2000000.0;
         }
      }
      else if (gridZoneIndex >= -9) // 'Q'
      {
         if ((gridZoneIndex > -9) || (latGridBound < 8)) //'J'
         {
            majorNorthingOffset = 2000000.0;
         }
         else
         {
            majorNorthingOffset = 0000000.0;
         }
      }
   }
   northing += majorNorthingOffset;

   UtEllipsoidalEarth::ConvertUTMToLL(lonZone, easting, northing, northernHemisphere, aLat, aLon);
   return true;
}

// =================================================================================================
//! Convert from TOD to ECEF coordinates.
//! @note The earth angle parameter defines the rotation; it is typically
//! computed from the current absolute simulation time (e.g., UtCalendar::GetEarthAngle()).
// static
void UtEllipsoidalEarth::ConvertTODToECEF(double aEarthAngle, const double aTOD[3], double aECEF[3])
{
   double sinPhi = sin(aEarthAngle);
   double cosPhi = cos(aEarthAngle);
   aECEF[0]      = cosPhi * aTOD[0] + sinPhi * aTOD[1];
   aECEF[1]      = cosPhi * aTOD[1] - sinPhi * aTOD[0];
   aECEF[2]      = aTOD[2];
}

// =================================================================================================
//! Convert from ECEF to TOD coordinates.
// static
void UtEllipsoidalEarth::ConvertECEFToTOD(double aEarthAngle, const double aECEF[3], double aTOD[3])
{
   double sinPhi = sin(aEarthAngle);
   double cosPhi = cos(aEarthAngle);
   aTOD[0]       = cosPhi * aECEF[0] - sinPhi * aECEF[1];
   aTOD[1]       = sinPhi * aECEF[0] + cosPhi * aECEF[1];
   aTOD[2]       = aECEF[2];
}

void UtEllipsoidalEarth::ComputeENUTransform(double aRefLat,
                                             double aRefLon,
                                             double aRefAlt,
                                             double aTransECEF[3][3],
                                             double aRefECEF[3])
{
   ut::EllipsoidalCentralBody::ComputeENUTransform(aRefLat, aRefLon, aRefAlt, ut::EarthEllipsoidWGS84(), aTransECEF, aRefECEF);
}

ut::coords::WCS UtEllipsoidalEarth::ComputeENUTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3])
{
   ut::coords::WCS refECEF;
   ComputeENUTransform(aRef.mLat, aRef.mLon, aRef.mAlt, aTransECEF, refECEF.GetData());
   return refECEF;
}

void UtEllipsoidalEarth::ComputeNEDTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3])
{
   ut::EllipsoidalCentralBody::ComputeNEDTransform(aRefLat, aRefLon, aRefAlt, aTransECEF);
}

void UtEllipsoidalEarth::ComputeNEDTransform(double aRefLat,
                                             double aRefLon,
                                             double aRefAlt,
                                             double aTransECEF[3][3],
                                             double aRefECEF[3])
{
   ut::EllipsoidalCentralBody::ComputeNEDTransform(aRefLat, aRefLon, aRefAlt, ut::EarthEllipsoidWGS84(), aTransECEF, aRefECEF);
}

ut::coords::WCS UtEllipsoidalEarth::ComputeNEDTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3])
{
   ut::coords::WCS refECEF;
   ComputeNEDTransform(aRef.mLat, aRef.mLon, aRef.mAlt, aTransECEF, refECEF.GetData());
   return refECEF;
}

void UtEllipsoidalEarth::ConvertLocalToLLA(const double aRefECEF[3],
                                           const double aTransECEF[3][3],
                                           const double aLocal[3],
                                           double&      aLat,
                                           double&      aLon,
                                           double&      aAlt)
{
   ut::EllipsoidalCentralBody::ConvertLocalToLLA(aRefECEF, aTransECEF, aLocal, ut::EarthEllipsoidWGS84(), aLat, aLon, aAlt);
}

ut::coords::LLA UtEllipsoidalEarth::ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                                      const double           aTransECEF[3][3],
                                                      const ut::coords::NED& aLocal)
{
   ut::coords::LLA out;
   ConvertLocalToLLA(aRefECEF.GetData(), aTransECEF, aLocal.GetData(), out.mLat, out.mLon, out.mAlt);
   return out;
}

ut::coords::LLA UtEllipsoidalEarth::ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                                      const double           aTransECEF[3][3],
                                                      const ut::coords::ENU& aLocal)
{
   ut::coords::LLA out;
   ConvertLocalToLLA(aRefECEF.GetData(), aTransECEF, aLocal.GetData(), out.mLat, out.mLon, out.mAlt);
   return out;
}

ut::coords::NED UtEllipsoidalEarth::ConvertLLAToNED(const ut::coords::WCS& aRefECEF,
                                                    const double           aTransECEF[3][3],
                                                    const ut::coords::LLA& aLLA)
{
   ut::coords::NED out;
   ConvertLLAToLocal(aRefECEF.GetData(), aTransECEF, aLLA.mLat, aLLA.mLon, aLLA.mAlt, out.GetData());
   return out;
}

ut::coords::ENU UtEllipsoidalEarth::ConvertLLAToENU(const ut::coords::WCS& aRefECEF,
                                                    const double           aTransECEF[3][3],
                                                    const ut::coords::LLA& aLLA)
{
   ut::coords::ENU out;
   ConvertLLAToLocal(aRefECEF.GetData(), aTransECEF, aLLA.mLat, aLLA.mLon, aLLA.mAlt, out.GetData());
   return out;
}


void UtEllipsoidalEarth::ConvertLLAToLocal(const double aRefECEF[3],
                                           const double aTransECEF[3][3],
                                           double       aLat,
                                           double       aLon,
                                           double       aAlt,
                                           double       aLocal[3])
{
   ut::EllipsoidalCentralBody::ConvertLLAToLocal(aRefECEF, aTransECEF, aLat, aLon, aAlt, ut::EarthEllipsoidWGS84(), aLocal);
}

void UtEllipsoidalEarth::ConvertLocalToECEF(const double aRefECEF[3],
                                            const double aTransECEF[3][3],
                                            const double aLocal[3],
                                            double       aECEF[3])
{
   ut::EllipsoidalCentralBody::ConvertLocalToECEF(aRefECEF, aTransECEF, aLocal, aECEF);
}

void UtEllipsoidalEarth::ConvertECEFToLocal(const double aRefECEF[3],
                                            const double aTransECEF[3][3],
                                            const double aECEF[3],
                                            double       aLocal[3])
{
   ut::EllipsoidalCentralBody::ConvertECEFToLocal(aRefECEF, aTransECEF, aECEF, aLocal);
}

ut::coords::WCS UtEllipsoidalEarth::ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                                       const double           aTransECEF[3][3],
                                                       const ut::coords::NED& aLocal)
{
   ut::coords::WCS out;
   ConvertLocalToECEF(aTransECEF, aLocal.GetData(), out.GetData());
   return out;
}

ut::coords::WCS UtEllipsoidalEarth::ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                                       const double           aTransECEF[3][3],
                                                       const ut::coords::ENU& aLocal)
{
   ut::coords::WCS out;
   ConvertLocalToECEF(aTransECEF, aLocal.GetData(), out.GetData());
   return out;
}

//! Convert a POSITION in a local ENU or NED frame to the global ECEF frame.
ut::coords::NED UtEllipsoidalEarth::ConvertECEFToNED(const ut::coords::WCS& aRefECEF,
                                                     const double           aTransECEF[3][3],
                                                     const ut::coords::WCS& aECEF)
{
   ut::coords::NED out;
   ConvertECEFToLocal(aRefECEF.GetData(), aTransECEF, aECEF.GetData(), out.GetData());
   return out;
}

ut::coords::ENU UtEllipsoidalEarth::ConvertECEFToENU(const ut::coords::WCS& aRefECEF,
                                                     const double           aTransECEF[3][3],
                                                     const ut::coords::WCS& aECEF)
{
   ut::coords::ENU out;
   ConvertECEFToLocal(aRefECEF.GetData(), aTransECEF, aECEF.GetData(), out.GetData());
   return out;
}

void UtEllipsoidalEarth::ConvertLocalToECEF(const double aTransECEF[3][3], const double aLocal[3], double aECEF[3])
{
   ut::EllipsoidalCentralBody::ConvertLocalToECEF(aTransECEF, aLocal, aECEF);
}

void UtEllipsoidalEarth::ConvertECEFToLocal(const double aTransECEF[3][3], const double aECEF[3], double aLocal[3])
{
   ut::EllipsoidalCentralBody::ConvertECEFToLocal(aTransECEF, aECEF, aLocal);
}

void UtEllipsoidalEarth::ConvertLLAToECEF(double aLat, double aLon, double aAlt, double aECEF[])
{
   ut::EllipsoidalCentralBody::ConvertLLAToECEF(aLat, aLon, aAlt, ut::EarthEllipsoidWGS84(), aECEF);
}

void UtEllipsoidalEarth::ConvertECEFToLLA(const double aECEF[], double& aLat, double& aLon, double& aAlt)
{
   ut::EllipsoidalCentralBody::ConvertECEFToLLA(aECEF, ut::EarthEllipsoidWGS84(), aLat, aLon, aAlt);
}

void UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(double  aBegLat,
                                                         double  aBegLon,
                                                         double  aHeading,
                                                         double  aDistance,
                                                         double& aEndLat,
                                                         double& aEndLon)
{
   ut::EllipsoidalCentralBody::ExtrapolateGreatEllipsePosition(aBegLat,
                                                               aBegLon,
                                                               aHeading,
                                                               aDistance,
                                                               ut::EarthEllipsoidWGS84(),
                                                               aEndLat,
                                                               aEndLon);
}

ut::coords::LL UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(ut::coords::LL aBeg, double aHeading, double aDistance)
{
   ut::coords::LL out;
   ExtrapolateGreatEllipsePosition(aBeg.mLat, aBeg.mLon, aHeading, aDistance, out.mLat, out.mLon);
   return out;
}

double UtEllipsoidalEarth::GetVincentyDistance(double  aBegLat,
                                               double  aBegLon,
                                               double  aEndLat,
                                               double  aEndLon,
                                               double& aHdgStart,
                                               double& aHdgEnd)
{
   return ut::EllipsoidalCentralBody::GetVincentyDistance(aBegLat,
                                                          aBegLon,
                                                          aEndLat,
                                                          aEndLon,
                                                          ut::EarthEllipsoidWGS84(),
                                                          aHdgStart,
                                                          aHdgEnd);
}

double UtEllipsoidalEarth::GetVincentyDistance(ut::coords::LL aBeg, ut::coords::LL aEnd, double& aHdgStart, double& aHdgEnd)
{
   return GetVincentyDistance(aBeg.mLat, aBeg.mLon, aEnd.mLat, aEnd.mLon, aHdgStart, aHdgEnd);
}

double UtEllipsoidalEarth::MetersPerDegreeLat(double aLatitude)
{
   return ut::EllipsoidalCentralBody::MetersPerDegreeLat(aLatitude, ut::EarthEllipsoidWGS84());
}

double UtEllipsoidalEarth::MetersPerDegreeLon(double aLongitude)
{
   return ut::EllipsoidalCentralBody::MetersPerDegreeLon(aLongitude, ut::EarthEllipsoidWGS84());
}

void UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(const double aEllipsoidalLocWCS[3], double aSphericalLocWCS[3])
{
   double lat;
   double lon;
   double alt;
   ConvertECEFToLLA(aEllipsoidalLocWCS, lat, lon, alt);
   UtSphericalEarth::ConvertLLAToECEF(lat, lon, alt, aSphericalLocWCS);
}

void UtEllipsoidalEarth::ConvertEllipsoidalToSphericalLLA(double  aEllipsoidalLat,
                                                          double  aEllipsoidalLon,
                                                          double  aEllipsoidalAlt,
                                                          double& aOutLat,
                                                          double& aOutLon,
                                                          double& aOutAlt)
{
   double ecef[3];
   UtEllipsoidalEarth::ConvertLLAToECEF(aEllipsoidalLat, aEllipsoidalLon, aEllipsoidalAlt, ecef);
   UtSphericalEarth::ConvertECEFToLLA(ecef, aOutLat, aOutLon, aOutAlt);
}

ut::coords::WCS UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(const ut::coords::WCS& aEllipsoidalLocWCS)
{
   ut::coords::WCS out;
   ConvertEllipsoidalToSpherical(aEllipsoidalLocWCS.GetData(), out.GetData());
   return out;
}
