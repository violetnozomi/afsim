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
#ifndef L16_NUMERICCONVERSION_HPP
#define L16_NUMERICCONVERSION_HPP

namespace WsfL16
{
namespace NumericConversion
{
//! Meters to Feet
inline double MetersToFeet(double aMeters)
{
   return aMeters * 3.2808399;
}
//! Feet to Meters
inline double FeetToMeters(double aFeet)
{
   return aFeet / 3.2808399;
}
//! Radians to Degrees
inline double RadiansToDeg(double aRadians)
{
   return aRadians * 57.29577951308232087721;
} //(180.0 / 3.1415926); }
//! Degrees to Radians
inline double DegToRadians(double aDegrees)
{
   return aDegrees * 0.01745329251994329576923;
} //(3.1415926 / 180.0); }
//! Meters to Miles
inline double MetersToMiles(double aMeters)
{
   return aMeters / 1609.344;
}
//! Meters to Data miles
inline double MetersToDataMiles(double aMeters)
{
   return aMeters / 1828.8;
}
//! Meters to nautical miles
inline double MetersToNauticalMiles(double aMeters)
{
   return aMeters / 1852.0;
}
//! Miles to Meters
inline double MilesToMeters(double aMiles)
{
   return aMiles * 1609.344;
}
//! Data miles to Meters
inline double DataMilesToMeters(double aDataMiles)
{
   return aDataMiles * 1828.8;
}
//! Nautical miles to Meters
inline double NauticalMilesToMeters(double aNauticalMiles)
{
   return aNauticalMiles * 1852.0;
}
//! Meters per second to Miles per hour
inline double MetersPerSecToMPH(double aMPS)
{
   return MetersToMiles(aMPS) * 3600;
}
//! Meters per second to Data miles per hour
inline double MetersPerSecToDMPH(double aMPS)
{
   return MetersToDataMiles(aMPS) * 3600;
}
//! Meters per second to nautical miles per hour (knots)
inline double MetersPerSecToKnots(double aMPS)
{
   return MetersToNauticalMiles(aMPS) * 3600;
}
//! Miles per hour to Meters per second
inline double MPHToMetersPerSec(double aMPH)
{
   return MilesToMeters(aMPH) / 3600;
}
//! Data miles per hour to Meters per second
inline double DMPHToMetersPerSec(double aMPH)
{
   return DataMilesToMeters(aMPH) / 3600;
}
//! Nautical miles per hour (Knots) to Meters per second
inline double KnotsToMetersPerSec(double aMPH)
{
   return NauticalMilesToMeters(aMPH) / 3600;
}
//! Kilograms to Pounds
inline double KilogramsToPounds(double aKg)
{
   return aKg * 2.20462262;
}
//! Pounds to Kilograms
inline double PoundsToKilograms(double aPounds)
{
   return aPounds / 2.20462262;
}

inline double NormalizeAngle0_360(double aAngle)
{
   while (aAngle >= 360.0)
   {
      aAngle -= 360.0;
   }
   while (aAngle < 0.0)
   {
      aAngle += 360.0;
   }
   return aAngle;
}
} // namespace NumericConversion
} // namespace WsfL16

#endif
