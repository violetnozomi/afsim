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

#ifndef SCENEGENINPUTDATA_HPP
#define SCENEGENINPUTDATA_HPP

/// @file SceneGenInputData.hpp
/// @brief Declaration of the helper data structure to store input parameters
///    parsed from an input file

#include <string>
#include <vector>

#include "UtLog.hpp"

namespace sceneGen
{
constexpr float cMAX_LAT = 90.0f;
constexpr float cMAX_LON = 180.0f;

/// @brief The SceneGenInputData struct is used as a POD to store the input values
///    for each group of platforms to generate. It contains a few helper
///    functions for data validation and restoring default values
struct SceneGenInputData
{
   SceneGenInputData() = default;

   //! The name of the platform object which will be displayed in the platforms
   //! list in the AFSIM editor (required)
   std::string mName{};
   //! The user defined object type of the platform. (required)
   //! @warning The user is responsible for having the object type defined and
   //! included before the script that contains the generated platforms script
   std::string mType{};
   //! The color group the platforms are assigned to, (required)
   std::string mSide{};
   //! Parameter to determine the distribution pattern of the generated
   //! platforms (required). The default is a random distribution contained within
   //! a circle
   std::string mDistribution{"random"};
   //! The maximum distribution length of the generated group. If set to Zero, all
   //! the platforms are set to the latitude and longitude defined for the group
   float mRadius{};
   //! The number of platforms to generate for a group in degrees (required)
   int mCount{};
   //! The starting latitude position (required)
   float mStartlat{cMAX_LAT + 1.0f};
   //! The starting longitude position (required)
   float mStartlon{cMAX_LON + 1.0f};
   //! The final latitude destination of each platform in the group
   float mEndlat{cMAX_LAT + 1.0f};
   //! The final longitude destination of each platform in the group
   float mEndlon{cMAX_LON + 1.0f};
   //! The speed of each platform in meters per second
   float mSpeed{};
   //! The altitude of each platform in meters
   float mAltitude{};
   //! The list of platform types the group is tracking
   std::vector<std::string> mTrackTypes;
   //! The names of the platforms the group is tracking
   std::vector<std::string> mTrackSide;
   //! Flag that enables route generation for each platform in the group
   //! to move to the tracked platforms
   bool mMoveToTrack{false};
};

/// @brief Used to check whether the user has defined the required
///     parameters to create a group of platforms. Not all members need
///     to be defined. Throws std::logic_error if data is missing
/// @param aData - data structure containing parsed information
void ValidateInput(const SceneGenInputData& aData);

} // namespace sceneGen

#endif // SCENEGENINPUTDATA_HPP
