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

#ifndef SCENEGENPATHCOST_HPP
#define SCENEGENPATHCOST_HPP

/// @file SceneGenPathCost.hpp
/// @brief Declaration and functionality of the program's path generation system.
///    SceneGenPathCost attempts find the shortest route given a collection of GeoCoords.

#include <vector>

namespace sceneGen
{
struct SceneGenGeoCoord;

/// @file PlatformGroup.hpp
/// @brief Declaration of the helper data structure to store input parameters
///    parsed from an input file. Similar to InputData but the data members
///    are modified to output unique identifiers in the generated scripts

/// @brief The SceneGenPathCost struct is a helper data structure used to calculate
///    the cost of a configuration of geocoordinates (route), and alter the path
///    IOT find the shortest path from the starting position to the final destination
struct SceneGenPathCost
{
   /// @brief Adds a geocoordinate to the path
   /// @param aLocation - position in latitude and longitude
   /// @returns true if it was able to add the new position to the path
   bool Insert(const SceneGenGeoCoord& aLocation);

   /// @brief Less than comparison operator comparing the cost of two routes
   /// @param aOther - another path configuration
   /// @returns true if self-cost is strictly less than other
   bool operator<(const SceneGenPathCost& aOther) const;

   /// @brief Helper function that swaps two positions in the route
   /// @param i - the ith position in the route
   /// @param j - the jth position in the route
   void Swap(int i, int j);

   /// @brief Calculates the cost of the current path configuration by summing
   ///    all the Haversine distances between consecutive pairs of GeoCoords
   /// @returns sum of haversine distance of entire path
   float CalculateCost() const;

   //! List of all positions in a path
   std::vector<SceneGenGeoCoord> mPath{};
   //! Cost of the route configuration
   float mCost{0.0f};
};

/// @brief Function that tries to find the cheapest path (least distance to travel)
///    by swapping pair-wise points in the path and keeping track of the cost.
/// @param aRoute - the path to evaluate and modify
/// @param aHasDestination - flag telling the function if the path has a static
///    destination. If true, the last entry will not be swapped.
/// @returns the path of cheapest cost.
SceneGenPathCost CheapestRoute(const std::vector<SceneGenGeoCoord>& aRoute, bool aHasDestination);

} // namespace sceneGen

#endif // SCENEGENPATHCOST_HPP
