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

#ifndef SCENEGENGEOCOORDGEN_HPP
#define SCENEGENGEOCOORDGEN_HPP

/// @file SceneGenGeoCoordGenerator.hpp
/// @brief Declaration of SceneGenGeoCoordGen, a class designed to generated GeoCoord distributions.
///    Any additional patterns would be implemented here to generate custom "formations" of
///    platforms or platform groups.

#include <random>
#include <vector>

#include "SceneGenGeoCoord.hpp"

namespace sceneGen
{
/// @brief The SceneGenGeoCoordGen class is used to generate distribution patters for the
///    AFSIM platforms. By supplying a group location and a radius of distribution,
///    it can create non-trivial group arrangements that can serve as a basis or
///    starting point to create more complex and specific configurations
///    within an AFSIM scenario.
class SceneGenGeoCoordGen
{
public:
   /// @brief Basic constructor that initializes members and seeds the randomizer engine.
   SceneGenGeoCoordGen() = default;

   /// @brief Sets the groups geospatial epicenter.
   /// @param aLatitude - latitude in degrees.
   /// @param aLongitude - longitude in degrees.
   void SetCenter(float aLatitude, float aLongitude);

   /// @brief Getter to retrieve group epicenter.
   /// @returns a GeoCoord struct with the latitude and longitude of the group epicenter.
   SceneGenGeoCoord GetCenter() const;

   /// @brief Distribution function that creates a random cloud of GeoCoord points centered
   ///    on the group epicenter.
   /// @param aRangeDegrees - Max range away from epicenter.
   /// @param aCount - Number of GeoCoords to generate.
   std::vector<SceneGenGeoCoord> RandomCircularDistribAroundCenter(float aRangeDegrees, int aCount) const;

   /// @brief Distribution function that creates a uniform circular pattern of GeoCoords centered
   ///    on the group epicenter.
   /// @param aRangeDegrees - Uniform distance, in degrees, away from epicenter.
   /// @param aCount - Number of GeoCoords to generate.
   std::vector<SceneGenGeoCoord> CircularDistribAroundCenter(float aRangeDegrees, int aCount) const;

private:
   //! Epicenter latitude
   float mLatitude{};
   //! Epicenter longitude
   float mLongitude{};
};

/// @brief Utility function to get a pseudo-random number within specified range.
/// @param aRange - Maximum half-distance from origin.
/// @returns float value x, s.t. ABS(x) <= ABS(aRange)
float RandFloat(float aRange);

} // namespace sceneGen

#endif // SCENEGENGEOCOORDGEN_HPP
