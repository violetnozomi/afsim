// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfZoneAttenuation.hpp"

#include "WsfEnvironment.hpp"
#include "WsfNoiseCloud.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfZone.hpp"
#include "WsfZoneSet.hpp"
#include "WsfZoneTypes.hpp"

// private
WsfZoneAttenuation::WsfZoneAttenuation(WsfSimulation* aSimulationPtr)
   : mSimulationPtr(aSimulationPtr)
{
}

//! Destructor.
WsfZoneAttenuation::~WsfZoneAttenuation()
{
   for (auto noiseCloud : mNoiseClouds)
   {
      delete noiseCloud;
   }
}

bool WsfZoneAttenuation::Initialize(const WsfScenario& aScenario)
{
   // std::vector<WsfZone*> typeList;
   mCategoryToValueMap.clear();
   mCategoryToZoneMap.clear();

   WsfObjectTypeListBase::TypeIdList typeIds;
   WsfZoneTypes::Get(aScenario).GetTypeIds(typeIds);
   for (const auto& typeId : typeIds)
   {
      WsfZone* zonePtr = WsfZoneTypes::Get(aScenario).Find(typeId);
      if (zonePtr == nullptr)
      {
         continue;
      }

      for (auto& zsIdModMap : zonePtr->GetModifierList())
      {
         WsfStringId categoryVal;
         double      modVal;

         categoryVal = zsIdModMap.first;
         modVal      = zsIdModMap.second;

         mCategoryToValueMap[categoryVal] = modVal;

         // if the key exists in the mCategoryToZoneMap see if the zone ptr also exists in the mCategoryToZoneMap
         // zoneset list, if not, add it
         std::map<WsfStringId, std::vector<WsfZone*>>::iterator catToZoneItr;
         catToZoneItr = mCategoryToZoneMap.find(categoryVal);
         if (catToZoneItr != mCategoryToZoneMap.end())
         {
            // the category already exists...
            std::vector<WsfZone*> listOfSets = catToZoneItr->second;
            // now search for a duplicate in the list
            std::vector<WsfZone*>::iterator zoneSetItr;
            zoneSetItr = listOfSets.begin();
            while (zoneSetItr != listOfSets.end())
            {
               WsfZone* tmpZone = *zoneSetItr;
               if (tmpZone == zonePtr)
               {
                  break;
               }
               ++zoneSetItr;
            }
            if (zoneSetItr == listOfSets.end())
            {
               // the set was not found in the list so add it
               mCategoryToZoneMap[categoryVal].push_back(zonePtr);
            }
         }
         else
         {
            catToZoneItr = mCategoryToZoneMap.begin();
            mCategoryToZoneMap[categoryVal].push_back(zonePtr);
         }
      }
   }

   const WsfNoiseCloudTypes& noiseCloudTypes = WsfNoiseCloudTypes::Get(aScenario);
   assert(mNoiseClouds.empty());
   noiseCloudTypes.GetCurrentUserTypes(mNoiseClouds);
   // Now that we have all the types, loop through and find any containing the given category....
   for (auto& noiseCloud : mNoiseClouds)
   {
      // Clone a simulation-specific copy of the noise cloud
      noiseCloud = noiseCloud->Clone();
      noiseCloud->Initialize(*mSimulationPtr);
      if (!noiseCloud->GetModifierList().empty())
      {
         for (auto& ncIdModMap : noiseCloud->GetModifierList())
         {
            WsfStringId categoryVal;
            double      modVal;

            categoryVal = ncIdModMap.first;
            modVal      = ncIdModMap.second;

            mCategoryToValueMap[categoryVal] = modVal;

            // if the key exists in the mCategoryToZoneMap see if the zone ptr also exists in the mCategoryToZoneMap
            // zoneset list, if not, add it
            std::map<WsfStringId, std::vector<WsfNoiseCloud*>>::iterator catToNCItr;
            catToNCItr = mCategoryToNoiseCloudMap.find(categoryVal);
            if (catToNCItr != mCategoryToNoiseCloudMap.end())
            {
               // the category already exists...
               std::vector<WsfNoiseCloud*> listOfSets = catToNCItr->second;
               // now search for a duplicate in the list
               std::vector<WsfNoiseCloud*>::iterator noiseCloudItr;
               noiseCloudItr = listOfSets.begin();
               while (noiseCloudItr != listOfSets.end())
               {
                  WsfNoiseCloud* tmpCloud = *noiseCloudItr;
                  if (tmpCloud == noiseCloud)
                  {
                     break;
                  }
                  ++noiseCloudItr;
               }
               if (noiseCloudItr == listOfSets.end())
               {
                  // the set was not found in the list so add it
                  mCategoryToNoiseCloudMap[categoryVal].push_back(noiseCloud);
               }
            }
            else
            {
               catToNCItr = mCategoryToNoiseCloudMap.begin();
               mCategoryToNoiseCloudMap[categoryVal].push_back(noiseCloud);
            }
         }
      }
   }
   return true;
}

double WsfZoneAttenuation::ComputeAttenuation(WsfStringId aCategoryId,
                                              double      aLocationFromLLA[3],
                                              double      aLocationToLLA[3],
                                              bool        aIsTwoWay)
{
   if (aLocationFromLLA[2] == 0.0)
   {
      // bump the platform up a bit to avoid edge cases
      aLocationFromLLA[2] = 1;
   }
   if (aLocationToLLA[2] == 0.0)
   {
      // bump the platform up a bit to avoid edge cases
      aLocationToLLA[2] = 1;
   }

   // loop through all zone sets that have the matching category id
   double attenuationValue = 0.0;
   for (auto& zonePtr : mCategoryToZoneMap[aCategoryId])
   {
      if (zonePtr != nullptr)
      {
         double penetration = zonePtr->CheckIntersections(aLocationToLLA, aLocationFromLLA);
         penetration        = aIsTwoWay ? penetration * 2 : penetration;
         if (penetration >= 0.0)
         {
            for (auto& zoneIdModMap : zonePtr->GetModifierList())
            {
               if (aCategoryId == zoneIdModMap.first)
               {
                  double modifierValue = zoneIdModMap.second;
                  // the penetration distance will be in meters, for each meter of penetration apply the modifier
                  attenuationValue += modifierValue * penetration;
               }
            }
         }
      }
   }

   // now loop through all noise-clouds that have the matching category id
   for (auto noiseCloudPtr : mCategoryToNoiseCloudMap[aCategoryId])
   {
      if (noiseCloudPtr != nullptr)
      {
         double penetration =
            noiseCloudPtr->CheckIntersections(mSimulationPtr->GetSimTime(), aLocationFromLLA, aLocationToLLA);
         penetration = aIsTwoWay ? penetration * 2 : penetration;
         if (penetration >= 0.0)
         {
            for (auto& ncIdModMap : noiseCloudPtr->GetModifierList())
            {
               if (aCategoryId == ncIdModMap.first)
               {
                  double modifierValue = ncIdModMap.second;
                  // the penetration distance will be in meters, for each meter of penetration apply the modifier
                  attenuationValue += modifierValue * penetration;
               }
            }
         }
      }
   }

   return attenuationValue;
}
