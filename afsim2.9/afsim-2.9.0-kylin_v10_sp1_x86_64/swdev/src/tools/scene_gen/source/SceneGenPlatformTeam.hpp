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

#ifndef SCENEGENPLATFORMTEAM_HPP
#define SCENEGENPLATFORMTEAM_HPP

/// @file SceneGenPlatformTeam.hpp
/// @brief Declaration of the helper data structure to store the collection
///    of generated groups. The file also contains functions to modify group
///    data as a postprocessing step, once all groups have been parsed and
///    instantiated.

#include <map>

#include "SceneGenInputData.hpp"
#include "SceneGenPlatformGroup.hpp"

namespace sceneGen
{
/// @brief Declaration of the struct that stores the platforms groups that are created
///    from parsing the input file.

struct SceneGenPlatformTeam
{
   /// @brief Adds track information to a SceneGenPlatformTeam if it is flagged to track
   ///    platforms of another team.
   /// @param aTrackedTeam - Reference to the team the group is tracking.
   void AddTrackInformation(const SceneGenPlatformTeam& aTrackedTeam);

   //! The collection of platform groups. Each vector contain only one type of platform.
   std::map<std::string, std::vector<SceneGenPlatformGroup>> mPlatformData{};
   //! The color a team represents
   std::string mSide{};
};

/// @brief Adds a group of platforms to a team.
/// @param aTeam - Reference to the team the group will be added to.
/// @param aGroup - parsed group data from input file.
void AddGroup(SceneGenPlatformTeam& aTeam, const SceneGenInputData& aGroup);

/// @brief Takes a collection of SceneGenPlatformTeams, ordered by color alphabetically,
///    and generates Track data to the groups of a team by taking and evaluating all
///    the other teams that precedes it (only if that group is tracking them).
/// @param aTeams - All platform teams.
void AddTrackData(std::map<std::string, SceneGenPlatformTeam>& aTeams);

/// @brief Generates routes for platforms in a group by using the start and
///    end positions of a platform, and any additional GeoCoords added to mTargets.
/// @param aGroup - parsed group data from input file.
void CreateRoutes(SceneGenPlatformTeam& aGroup);

/// @brief Adds a group of platforms to a team.
/// @param os - out stream object.
/// @param aTeam - collection of platform groups.
std::ostream& operator<<(std::ostream& os, const SceneGenPlatformTeam& aTeam);

} // namespace sceneGen

#endif // SCENEGENPLATFORMTEAM_HPP
