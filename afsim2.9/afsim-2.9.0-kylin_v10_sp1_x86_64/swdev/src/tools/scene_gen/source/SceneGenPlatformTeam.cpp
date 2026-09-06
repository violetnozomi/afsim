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

#include "SceneGenPlatformTeam.hpp"

#include "SceneGenGeoCoordGen.hpp"

namespace sceneGen
{
void SceneGenPlatformTeam::AddTrackInformation(const SceneGenPlatformTeam& aTrackedTeam)
{
   auto group_type = mPlatformData.begin();

   while (group_type != mPlatformData.end())
   {
      // get the list of platform groups of the current group type
      std::vector<SceneGenPlatformGroup>& platforms = group_type->second;

      // for each platform group
      for (SceneGenPlatformGroup& group : platforms)
      {
         // if the group is tracking platforms
         if (group.mTracking)
         {
            // fetch the color this group is tracking
            const std::vector<std::string>& tracking_sides = group.mTrackSides;

            // Determine if aTrackedTeam is being tracked by this group
            bool tracking_this_color = false;
            for (const std::string& color : tracking_sides)
            {
               if (color == aTrackedTeam.mSide)
               {
                  tracking_this_color = true;
                  break;
               }
            }

            if (tracking_this_color)
            {
               // fetch the type of platforms this group is tracking
               const std::vector<std::string>& tracking_types = group.mTrackTypes;

               // for each type of platform this group is tracking
               for (const std::string& platform_type : tracking_types)
               {
                  // get the corresponding type list from aTrackedTeam
                  auto type = aTrackedTeam.mPlatformData.find(platform_type);

                  if (type != aTrackedTeam.mPlatformData.end())
                  {
                     const std::vector<SceneGenPlatformGroup>& tracked_groups = type->second;

                     // for each enemy group that is being tracked
                     for (const SceneGenPlatformGroup& enemy_group : tracked_groups)
                     {
                        // get the enemy names and add it to the mTargetNames list
                        group.mTargetNames.push_back(enemy_group.mName);

                        // if this group is moving to the enemy group
                        if (group.mMoveToTrack)
                        {
                           // add the enemy positions to the mTargets list
                           group.mTargets.push_back(enemy_group.mPositions[0]);
                        }
                     }
                  }
               }
            }
         }
      }

      ++group_type;
   }
}

void AddGroup(SceneGenPlatformTeam& aTeam, const SceneGenInputData& aGroup)
{
   SceneGenGeoCoordGen cg;
   cg.SetCenter(aGroup.mStartlat, aGroup.mStartlon);
   std::vector<SceneGenGeoCoord> list;

   // Additional distributions can be added here to generate specific configuration
   if (aGroup.mDistribution == "circular")
   {
      list = cg.CircularDistribAroundCenter(aGroup.mRadius, aGroup.mCount);
   }
   else
   {
      list = cg.RandomCircularDistribAroundCenter(aGroup.mRadius, aGroup.mCount);
   }

   std::string base_name(aGroup.mSide);
   base_name.append("_");
   base_name.append(aGroup.mName);
   base_name.append("_");

   for (const SceneGenGeoCoord& location : list)
   {
      std::string name(base_name);

      SceneGenPlatformGroup data;
      data.mName = name;
      data.mPositions.push_back(location);
      data.mSpeed    = aGroup.mSpeed;
      data.mAltitude = aGroup.mAltitude;

      // Assigns track data if it exists
      if (aGroup.mTrackSide.size())
      {
         for (const std::string& tside : aGroup.mTrackSide)
         {
            // AFSIM scripts require that tracked objects be defined before a platform
            // can add tracks of them. For simplicity, in this program, the order of the
            // colors determines which color can track other colors.
            // Ex:
            // Given a black, red, and yellow side
            // red can track black, yellow can track black and red, and
            // black can't track any
            if (tside < aGroup.mSide)
            {
               data.mTrackSides.push_back(tside);
            }
         }

         // if this platform can track the specified color(s) and the input
         // file specified types to track
         if (data.mTrackSides.size() && aGroup.mTrackTypes.size())
         {
            data.mTracking    = true;
            data.mTrackTypes  = aGroup.mTrackTypes;
            data.mMoveToTrack = aGroup.mMoveToTrack;
         }
      }

      // End position is an optional entry. this checks if the end position
      // is valid or the default value before adding to route
      if (std::abs(aGroup.mEndlat) <= cMAX_LAT && std::abs(aGroup.mEndlon) <= cMAX_LON)
      {
         data.mPositions.emplace_back(aGroup.mEndlat, aGroup.mEndlon);
      }

      data.mType = aGroup.mType;

      auto it = aTeam.mPlatformData.find(data.mType);

      // add platform to currently existing list of same type
      if (it != aTeam.mPlatformData.end())
      {
         data.mName.append(std::to_string(it->second.size() + 1));
         it->second.push_back(data);
      }
      // else create a new list
      else
      {
         data.mName.append(std::to_string(1));
         std::vector<SceneGenPlatformGroup> plist = {data};
         aTeam.mPlatformData[data.mType]          = plist;
      }
   }
}

void AddTrackData(std::map<std::string, SceneGenPlatformTeam>& aTeams)
{
   if (aTeams.empty())
   {
      return;
   }

   // AFSIM scripts require that tracked objects be defined before a platform
   // can add tracks of them. For simplicity, in this program, the order of the
   // colors determines which color can track other colors.
   // Ex:
   // Given a black, red, and yellow side
   // red can track black, yellow can track black and red, and
   // black can't track any

   auto back  = aTeams.rbegin();
   auto front = aTeams.rend();

   // starting from the end of the PlatformGenerator map and moving backwards
   // (sides can only track groups that come before it)
   while (back != --front)
   {
      // determine if any group that precedes it is set to be tracked and modify
      // the appropriate track data of the platforms in the end iterator.
      // (don't evaluate self)
      while (front != back)
      {
         back->second.AddTrackInformation(front->second);
         --front;
      }

      CreateRoutes(back->second);

      // reset the front iterator
      front = aTeams.rend();

      // and move the back iterator up
      ++back;
   }
}

void CreateRoutes(SceneGenPlatformTeam& aTeam)
{
   for (auto it = aTeam.mPlatformData.begin(); it != aTeam.mPlatformData.end(); ++it)
   {
      for (auto& group : it->second)
      {
         group.CreateRoute();
      }
   }
}

std::ostream& operator<<(std::ostream& os, const SceneGenPlatformTeam& aTeam)
{
   // Generates AFSIM script entries
   for (auto it = aTeam.mPlatformData.cbegin(); it != aTeam.mPlatformData.cend(); ++it)
   {
      const std::vector<SceneGenPlatformGroup>& list = it->second;

      for (const auto& group : list)
      {
         os << "platform " << group.mName << " " << group.mType << std::endl;
         os << std::endl;

         if (group.mTracking)
         {
            for (const std::string& track_name : group.mTargetNames)
            {
               os << "   track platform " << track_name << " end_track" << std::endl;
            }
            os << std::endl;
         }

         os << "   side " << aTeam.mSide << std::endl;

         const std::vector<SceneGenGeoCoord>& positions = group.mPositions;
         size_t                               pos_count = positions.size();

         if (pos_count > 1)
         {
            float heading = Heading(positions[0], positions[1]);

            os << "   heading " << heading << " degrees" << std::endl;
            os << "   altitude " << group.mAltitude << " meters" << std::endl;
            os << "   route" << std::endl;

            for (size_t j = 0; j < pos_count; ++j)
            {
               os << "      " << positions[j].GetDegreesMinutesSeconds();

               if (j == 0)
               {
                  os << " altitude " << list[j].mAltitude << " meters speed " << list[j].mSpeed << " m/s";
               }
               os << std::endl;
            }

            os << "   end_route" << std::endl;
            os << std::endl;
         }
         else if (pos_count == 1)
         {
            os << "   " << positions[0].GetDegreesMinutesSeconds() << std::endl;
            os << "   heading " << RandFloat(180.0f) + 180.0f << " degrees" << std::endl;
            os << "   altitude " << list[0].mAltitude << " meters" << std::endl;
            os << std::endl;
         }

         os << "end_platform" << std::endl << std::endl;
      }
   }

   return os;
}

} // namespace sceneGen
