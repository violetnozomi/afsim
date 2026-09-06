// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SceneGenGeoCoord.hpp"

#include "UtMath.hpp"

namespace sceneGen
{
//! Radius of Earth used in Haversine calculation: 6,357 to 6,378 km
static constexpr float cRADIUS_OF_EARTH_IN_KM = 6357.0f; // using lower bound

/// @brief converts a distance in kilometers to an approximate distance in degrees
/// @param aKilometers -the value to convert
static float KmToDegrees(float aKilometers)
{
   return aKilometers / 111.0f;
}

/// @brief converts radians to degrees
/// @param aRadians - the value to convert
inline static float ToDegrees(float aRadians)
{
   return static_cast<float>(UtMath::cDEG_PER_RAD) * aRadians;
}

void ConvertDegToDMS(std::stringstream& ss, float aDegree)
{
   // gets the whole part of the value to assign to Degree
   float     val    = std::abs(aDegree);
   const int ValDeg = static_cast<int>(val);
   val -= ValDeg;
   val *= 60.0f;

   // gets the converted remainder to assign to Minute
   const int ValMin = static_cast<int>(val);
   val -= ValMin;
   val *= 60.0f;

   // gets the converted remainder to assign to Seconds
   const int ValSecW = static_cast<int>(val);
   val -= ValSecW;
   val *= 10.0f;
   const int ValSecR = static_cast<int>(val);

   // stores the Degree-Minute-Seconds format
   ss << std::setw(3) << std::setfill('0') << ValDeg << ":";
   ss << std::setw(2) << std::setfill('0') << ValMin << ":";
   ss << std::setw(2) << std::setfill('0') << ValSecW << ".";
   ss << std::setw(2) << std::setfill('0') << ValSecR;
}

// Algorithm - https://www.movable-type.co.uk/scripts/latlong.html
float Heading(const SceneGenGeoCoord& aStart, const SceneGenGeoCoord& aEnd)
{
   if (aStart == aEnd)
   {
      return 0.0f;
   }

   const float phi1   = ToRadians(aStart.mLatitude);
   const float phi2   = ToRadians(aEnd.mLatitude);
   const float deltaL = ToRadians(aStart.mLongitude - aEnd.mLongitude);

   const float y = sin(deltaL) * cos(phi2);
   const float x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(deltaL);

   float bearing = ToDegrees(atan2f(y, x));

   if (y < 0)
   {
      bearing = copysign(bearing, y);
   }

   if (sin(ToRadians(aEnd.mLongitude - aStart.mLongitude)) < 0.0)
   {
      bearing = 360.0f - bearing;
   }

   return bearing;
}

// Algorithm - https://www.movable-type.co.uk/scripts/latlong.html
SceneGenGeoCoord MidPoint(const SceneGenGeoCoord& aStart, const SceneGenGeoCoord& aEnd)
{
   const float phi1   = ToRadians(aStart.mLatitude);
   const float phi2   = ToRadians(aEnd.mLatitude);
   const float lambda = ToRadians(aEnd.mLongitude - aStart.mLongitude);

   const float Bx = cosf(phi2) * cosf(lambda);
   const float By = cosf(phi2) * sinf(lambda);

   const float X      = cosf(phi1) + Bx;
   float       midLat = atan2f(sinf(phi1) + sinf(phi2), sqrt(X * X + By * By));
   float       midLon = ToRadians(aStart.mLongitude) + atan2f(By, cosf(phi1) + Bx);
   midLat             = ToDegrees(midLat);
   midLon             = ToDegrees(midLon);
   midLon             = fmodf(midLon + 540.0f, 360.0f) - 180.0f;

   return SceneGenGeoCoord(midLat, midLon);
}

// Algorithm - https://www.movable-type.co.uk/scripts/latlong.html
float DistanceInDegress(const SceneGenGeoCoord& A, const SceneGenGeoCoord& B)
{
   const float lat_a = ToRadians(A.mLatitude);
   const float lon_a = ToRadians(A.mLongitude);
   const float lat_b = ToRadians(B.mLatitude);
   const float lon_b = ToRadians(B.mLongitude);

   const float dlat = lat_b - lat_a;
   const float dlon = lon_b - lon_a;

   const float sin_dlat  = sinf(dlat / 2.0f);
   const float sin_dlon  = sinf(dlon / 2.0f);
   const float cos_lat_a = cosf(lat_a);
   const float cos_lat_b = cosf(lat_b);

   float result = asin(sqrt(sin_dlat * sin_dlat + cos_lat_a * cos_lat_b * sin_dlon * sin_dlon));

   return KmToDegrees(2.0f * cRADIUS_OF_EARTH_IN_KM * result);
}

SceneGenGeoCoord::SceneGenGeoCoord(float aLat, float aLon)
   : mLatitude(AdjustLat(aLat))
   , mLongitude(AdjustLon(aLon))
{
}

bool SceneGenGeoCoord::operator==(const SceneGenGeoCoord& aRhs) const
{
   return (mLatitude == aRhs.mLatitude) && (mLongitude == aRhs.mLongitude);
}
bool SceneGenGeoCoord::operator!=(const SceneGenGeoCoord& aRhs) const
{
   return !(*this == aRhs);
}

// AFSIM uses characters to denote direction. Negative values are allowed
// but using characters makes the scripts more human-readable
std::string SceneGenGeoCoord::GetDegreesMinutesSeconds() const
{
   std::stringstream ss;
   ss << "position ";

   std::string LatBearing("n"); // degrees North
   if (mLatitude < 0)
   {
      LatBearing[0] = 's'; // degrees South
   }

   ConvertDegToDMS(ss, mLatitude);
   ss << LatBearing << " ";

   std::string LonBearing("e"); // degrees East
   if (mLongitude < 0)
   {
      LonBearing[0] = 'w'; // degrees West
   }

   ConvertDegToDMS(ss, mLongitude);
   ss << LonBearing;

   return ss.str();
}

float AdjustLat(float aLatitude)
{
   return fmodf(aLatitude, 90.0f);
}

float AdjustLon(float aLongitude)
{
   return fmodf(aLongitude, 180.0f);
}

float ToRadians(float aDegree)
{
   return static_cast<float>(UtMath::cRAD_PER_DEG) * aDegree;
}

} // namespace sceneGen
