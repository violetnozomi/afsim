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

#ifndef SCENEGENGEOCOORD_HPP
#define SCENEGENGEOCOORD_HPP

/// @file SceneGenGeoCoord.hpp
/// @brief File contains Declaration of SceneGenGeoCoord, a structure that defines the geolocation
///    of a platform w.r.t earth. It also contains various functions used for path generation
///    and determining derived attributes between SceneGenGeoCoorda

#include <iomanip>
#include <sstream>
#include <string>

namespace sceneGen
{
/// @brief A struct that defines a geolocation in latitude
///    and longitude. Generates the Degrees-Minutes-Seconds format
///    used to define routes in AFSIM.
struct SceneGenGeoCoord
{
   /// @brief Basic constructor that can accept latitude and longitude values.
   /// @param aLat - latitude value.
   /// @param aLon - longitude value.
   explicit SceneGenGeoCoord(float aLat = 0.0f, float aLon = 0.0f);

   /// @brief Equality operator
   /// @param aRhs - right-hand side of expression.
   bool operator==(const SceneGenGeoCoord& aRhs) const;

   /// @brief inequality operator
   /// @param aRhs - right-hand side of expression.
   bool operator!=(const SceneGenGeoCoord& aRhs) const;

   /// @brief Generates the geolocation string format needed for use
   ///    in AFSIM scripts to specify waypoints.
   /// @returns geolocation in Degree-Minute-Seconds format
   std::string GetDegreesMinutesSeconds() const;

   //! Latitude value in degrees; mapped to -90 <= lat <= 90.
   float mLatitude;
   //! Longitude value in degrees; mapped to -180 <= lon <= 180.
   float mLongitude;
};

/// @brief Function converts a degree value to Degree-Minute-Seconds format.
/// @param ss - the stringstream where the result is stored.
/// @param aDegree - the value to convert.
void ConvertDegToDMS(std::stringstream& ss, float aDegree);

/// @brief Function that determines the heading between two SceneGenGeoCoords.
/// @param aStart - the start position
/// @param aEnd - the destination
float Heading(const SceneGenGeoCoord& aStart, const SceneGenGeoCoord& aEnd);

/// @brief Function that calculates the midpoint between two SceneGenGeoCoords.
/// @param aStart - the start position
/// @param aEnd - the destination
SceneGenGeoCoord MidPoint(const SceneGenGeoCoord& aStart, const SceneGenGeoCoord& aEnd);

/// @brief Calculates the approximate distance (in degrees) between two SceneGenGeoCoords
///    using Haversine approximation.
/// @param A - the start position
/// @param B - the destination
float DistanceInDegress(const SceneGenGeoCoord& A, const SceneGenGeoCoord& B);

/// @brief Helper function to wrap latitude to -90 <= lat <= 90.
/// @param aLatitude - latitude value
/// @returns modified latitude value within valid range
float AdjustLat(float aLatitude);

/// @brief Helper function to wrap longitude to -180 <= lon <= 180.
/// @param aLongitude - longitude value
/// @returns modified longitude value within valid range
float AdjustLon(float aLongitude);

/// @brief converts degrees to radians
/// @param aDegree - the value to convert
float ToRadians(float aDegree);

} // namespace sceneGen

#endif // SCENEGENGEOCOORD_HPP
