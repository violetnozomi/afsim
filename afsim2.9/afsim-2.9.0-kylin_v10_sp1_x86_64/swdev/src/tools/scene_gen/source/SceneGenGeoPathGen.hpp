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

#ifndef SCENEGENGEOPATHGEN_HPP
#define SCENEGENGEOPATHGEN_HPP

/// @file SceneGenGeoPathGen.hpp
/// @brief Declaration and functionality of the program's path generation system.
///    SceneGenGeoPathGen adds waypoints between to SceneGenGeoCoords as a way to
///    provide an afsim user additional waypoint in scripts to directly manipulate.

#include <queue>

#include "SceneGenGeoCoord.hpp"

namespace sceneGen
{
/// @brief The SceneGenGeoPathGen is utility class designed to insert waypoints
///    between two SceneGenGeoCoords. This gives the AFSIM user more route points
///    to directly manipulate and handcraft complex paths.
class SceneGenGeoPathGen
{
public:
   /// @brief Default constructor
   SceneGenGeoPathGen() = default;

   /// @brief Default destructor
   ~SceneGenGeoPathGen() = default;

   /// @brief Function that inserts waypoints between two SceneGenGeoCoords
   /// @param aStartPosition - starting latitude and longitude
   /// @param aDestination - ending latitude and longitude
   /// @param aHeight - height of the recursive callstack inserting mid points
   /// @param aMinSeperationDeg - minimum distance, in degrees, between two SceneGenGeoCoords to terminate recursion
   /// @returns list of SceneGenGeoCoord defining a straight-line path
   std::deque<SceneGenGeoCoord>
   PathMidPoint(SceneGenGeoCoord& aStartPosition, SceneGenGeoCoord& aDestination, int aHeight, float aMinSeperationDeg);

private:
   /// @brief Helper function to PathMidPoint
   /// @param aStartPosition - starting latitude and longitude
   /// @param aDestination - ending latitude and longitude
   /// @param aHeight - height of the recursive callstack inserting mid points
   /// @param aMinSeperationDeg - minimum distance, in degrees, between two SceneGenGeoCoords to terminate recursion
   /// @param aPath - resulting list of SceneGenGeoCoords
   void PathMidPointHelper(SceneGenGeoCoord&             aStartPosition,
                           SceneGenGeoCoord&             aDestination,
                           int                           aHeight,
                           const float&                  aMinSeperationDeg,
                           std::deque<SceneGenGeoCoord>& aPath);
};

} // namespace sceneGen

#endif // SCENEGENGEOPATHGEN_HPP
