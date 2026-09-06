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

#include "SceneGenPathCost.hpp"

#include <algorithm>

#include "SceneGenGeoCoordGen.hpp"

namespace sceneGen
{
SceneGenPathCost CheapestRoute(const std::vector<SceneGenGeoCoord>& aRoute, bool aHasDestination)
{
   SceneGenPathCost result;
   result.mPath = aRoute;
   result.mCost = result.CalculateCost();

   int   size        = static_cast<int>(aRoute.size());
   float best_cost   = result.mCost;
   bool  improvement = true;
   int   offset      = 0;

   if (aHasDestination)
   {
      offset = 1;
   }

   while (improvement)
   {
      improvement = false;
      int i_best = -1, j_best = -1;

      for (int i = 1; i < size - 1 - offset; ++i)
      {
         for (int j = i + 1; j < size - offset; ++j)
         {
            result.Swap(i, j);
            float new_cost = result.CalculateCost();

            if (new_cost < best_cost)
            {
               best_cost   = new_cost;
               i_best      = i;
               j_best      = j;
               improvement = true;
            }
            result.Swap(i, j);
         }
      }

      if (improvement)
      {
         result.Swap(i_best, j_best);
         result.mCost = best_cost;
      }
   }

   return result;
}

bool SceneGenPathCost::Insert(const SceneGenGeoCoord& aLocation)
{
   auto it = std::find(mPath.begin(), mPath.end(), aLocation);

   // only insert unique entries
   if (it == mPath.end())
   {
      size_t size = mPath.size();

      // adjust the path cost of adding the new entry
      mCost += DistanceInDegress(mPath[size - 1], aLocation);

      mPath.push_back(aLocation);
      return true;
   }
   return false;
}

bool SceneGenPathCost::operator<(const SceneGenPathCost& aOther) const
{
   return mCost < aOther.mCost;
}

void SceneGenPathCost::Swap(int i, int j)
{
   std::swap(mPath[i], mPath[j]);
}

float SceneGenPathCost::CalculateCost() const
{
   float  cost = 0.0f;
   size_t size = mPath.size();

   for (size_t i = 0; i < size - 1; ++i)
   {
      cost += DistanceInDegress(mPath[i], mPath[i + 1]);
   }

   return cost;
}

} // namespace sceneGen
