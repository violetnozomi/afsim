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

#include "SceneGenPlatformGroup.hpp"

#include <algorithm>
#include <limits>
#include <list>
#include <stack>

#include "SceneGenGeoCoordGen.hpp"
#include "SceneGenPathCost.hpp"

namespace sceneGen
{
/// Maximum separation in degrees when grouping neighboring platforms in
/// CreateRoute. Larger values creates relaxed paths; a value of zero forces
/// every target position to be included in the generated path.
constexpr float cMAX_DIST_DEG = 1.0f;

void SceneGenPlatformGroup::CreateRoute()
{
   // if this platform is moving to tracked targets
   if (!mTargets.empty())
   {
      std::vector<SceneGenGeoCoord> route;

      // load its starting position
      route.push_back(mPositions[0]);

      // get all the target location
      std::list<SceneGenGeoCoord> locations(mTargets.begin(), mTargets.end());

      // and incrementally add the closest target position, removing
      // the added target positions from the target list
      while (!locations.empty())
      {
         // find closest position to tail of route
         std::list<SceneGenGeoCoord>::iterator loc = locations.begin();
         std::list<SceneGenGeoCoord>::iterator closest;
         float                                 dist = std::numeric_limits<float>::max();
         while (loc != locations.end())
         {
            float test_dist = DistanceInDegress(route.back(), *loc);

            if (test_dist < dist)
            {
               dist    = test_dist;
               closest = loc;
            }
            ++loc;
         }

         std::stack<SceneGenGeoCoord> group; // group of close neighbors
         SceneGenGeoCoord             next_pos = *closest;
         locations.erase(closest);

         // now find all near points to the closest target position and add it to the group.
         // This uses MAX_DIST_DEG to determine neighbors to group. If MAX_DIST_DEG > 0,
         // this section will try to find a centroid position of closest neighbors and use that
         // position value to add to the path; otherwise, the path generated will strictly move to
         // all target position.
         while (true)
         {
            closest = std::find_if(locations.begin(),
                                   locations.end(),
                                   [&next_pos](const SceneGenGeoCoord& x)
                                   {
                                      float dist_check = DistanceInDegress(next_pos, x);

                                      return dist_check < cMAX_DIST_DEG;
                                   });

            if (closest != locations.end())
            {
               group.push(*closest);
               locations.erase(closest);
            }
            else
            {
               break;
            }
         }

         // Calculate the approximate centroid of the non-empty cluster of nearest neighbors
         while (!group.empty())
         {
            next_pos = MidPoint(next_pos, group.top());
            group.pop();
         }

         route.push_back(next_pos);
      }

      bool hasDestination = false;

      // Add the destination to the route
      if (mPositions.size() > 1)
      {
         route.push_back(mPositions.back());
         hasDestination = true;
      }

      // find the cheapest route configuration
      SceneGenPathCost path = CheapestRoute(route, hasDestination);

      mPositions = path.mPath;
   }
}

} // namespace sceneGen
