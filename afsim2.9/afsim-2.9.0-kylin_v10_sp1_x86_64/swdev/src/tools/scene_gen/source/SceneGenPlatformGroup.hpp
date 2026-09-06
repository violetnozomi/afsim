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

#ifndef SCENEGENPLATFORMGROUP_HPP
#define SCENEGENPLATFORMGROUP_HPP

/// @file SceneGenPlatformGroup.hpp
/// @brief Declaration of the helper data structure to store input parameters
///    parsed from an input file. Similar to InputData but the data members
///    are modified to output unique identifiers in the generated scripts

#include <string>
#include <vector>

#include "SceneGenGeoCoord.hpp"

namespace sceneGen
{
/// @brief The SceneGenPlatformGroup struct is used as a POD to store the input values
///    for each platform to generate. The values are modified from the original input,
///    such as by mangling platform names and added to path positions
///    (route generation) as a final step before script generation.
struct SceneGenPlatformGroup
{
   SceneGenPlatformGroup() = default;

   /// @brief Function builds a route from any GeoCoords added to mTargets. It only
   ///    generates routes if the platform is flagged to move to tracked platforms
   ///    and contains the required path information.
   void CreateRoute();

   //! Mangled name of the platform
   std::string mName{};
   //! The user defined object type of the platform.
   //! @warning The user is responsible for having the object type defined and
   //! included before the script that contains the generated platforms script
   std::string mType{};
   //! Route of the platform in latitude and longitude
   std::vector<SceneGenGeoCoord> mPositions{};
   //! Speed of the platform in meters per second
   float mSpeed{};
   //! Altitude of the platform in meters
   float mAltitude{};
   //! Flag if tracking other platforms
   bool mTracking{false};
   //! List of platform types to track
   std::vector<std::string> mTrackTypes{};
   //! The color/side of the tracked platforms
   std::vector<std::string> mTrackSides{};
   //! Flag if moving to tracked platforms
   bool mMoveToTrack{false};
   //! Helper member, storing positions of tracked platforms
   std::vector<SceneGenGeoCoord> mTargets{};
   //! Helper member, storing names of tracked platforms
   std::vector<std::string> mTargetNames{};
};

} // namespace sceneGen

#endif // SCENEGENPLATFORMGROUP_HPP
