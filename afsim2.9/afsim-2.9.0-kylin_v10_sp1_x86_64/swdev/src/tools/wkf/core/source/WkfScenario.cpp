// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfScenario.hpp"

#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfTrack.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::Scenario::Scenario()
   : vespa::VaScenario("wkf")
   , mRemovingAllTracks(false)
   , mMovementEnabled(true)
{
}

wkf::Platform* wkf::Scenario::FindPlatformByIndex(unsigned int aIndex) const
{
   auto it = mIndexedPlatformMap.find(aIndex);
   if (it != mIndexedPlatformMap.end())
   {
      return it->second;
   }
   return nullptr;
}

wkf::Platform* wkf::Scenario::FindPlatformByName(const std::string& aName) const
{
   vespa::VaEntity* entity = FindEntity(aName);
   auto*            plat   = dynamic_cast<wkf::Platform*>(entity);
   return plat;
}

wkf::Platform* wkf::Scenario::FindPlatformByName(const QString& aName) const
{
   return FindPlatformByName(aName.toStdString());
}

bool wkf::Scenario::AddEntity(vespa::VaEntity* aEntityPtr)
{
   if (VaScenario::AddEntity(aEntityPtr))
   {
      // If the Entity was successfully added to the scenario, check to see if the entity is a POI
      //  or a Bullseye and add it to the appropriate lists.
      auto* pointOfInterest = dynamic_cast<wkf::PointOfInterest*>(aEntityPtr);
      if (pointOfInterest)
      {
         mPOI_List.insert(pointOfInterest);

         auto* Bullseye = dynamic_cast<wkf::Bullseye*>(pointOfInterest);
         if (Bullseye)
         {
            mBullseyeList.insert(Bullseye);
         }
      }
      return true;
   }
   return false;
}

bool wkf::Scenario::RemoveEntity(vespa::VaEntity* aEntityPtr)
{
   // If the Entity was successfully removed from the scenario
   if (VaScenario::RemoveEntity(aEntityPtr))
   {
      if (!mRemovingAllTracks)
      {
         // If the entity is a track, remove it from the track map
         auto* trackPtr = dynamic_cast<Track*>(aEntityPtr);
         if (trackPtr)
         {
            mTrackMap.erase(trackPtr->GetTrackId());
         }
      }

      // If the entity is a Platform, remove it from the platform map
      auto* platformPtr = dynamic_cast<Platform*>(aEntityPtr);
      if (platformPtr)
      {
         mIndexedPlatformMap.erase(platformPtr->GetIndex());
      }

      // If the entity was a PointofInterest remove it from the list of POIs
      auto* pointOfInterest = dynamic_cast<wkf::PointOfInterest*>(aEntityPtr);
      if (pointOfInterest)
      {
         mPOI_List.erase(pointOfInterest);

         // If the entity was a Bullseye remove it from the list of bullseyes
         auto* bullseye = dynamic_cast<wkf::Bullseye*>(pointOfInterest);
         if (bullseye)
         {
            mBullseyeList.erase(bullseye);
         }
      }
      return true;
   }
   return false;
}

bool wkf::Scenario::DeletePlatformByIndex(unsigned int aIndex)
{
   Platform* platform = FindPlatformByIndex(aIndex);
   if (platform)
   {
      DeleteEntity(platform);
      return true;
   }
   return false;
}

bool wkf::Scenario::AddPlatformWithIndex(Platform* aPlatformPtr, unsigned int aIndex)
{
   mIndexedPlatformMap[aIndex] = aPlatformPtr;
   return AddEntity(aPlatformPtr);
}

wkf::Platform* wkf::Scenario::CreatePlaceholderPlatform(unsigned int aIndex)
{
   auto* entityPtr = VtkEnvironment::CreateEntity<Platform>("platform");
   if (entityPtr)
   {
      entityPtr->SetIndex(aIndex);
      AddPlatformWithIndex(entityPtr, aIndex);
      entityPtr->Initialize();
   }

   return entityPtr;
}

wkf::Track* wkf::Scenario::FindTrack(const TrackId& aTrackId) const
{
   if (mTrackMap.count(aTrackId) > 0)
   {
      return mTrackMap.at(aTrackId);
   }
   return nullptr;
}

void wkf::Scenario::AddTrack(Track* aTrackPtr)
{
   if (aTrackPtr != nullptr)
   {
      // Delete an existing track for this same track id, if it exists.
      DeleteTrack(aTrackPtr->GetTrackId());
      // Add the track pointer to the track map
      mTrackMap[aTrackPtr->GetTrackId()] = aTrackPtr;
      // Add the entity to the scenario.
      AddEntity(aTrackPtr);
   }
}

void wkf::Scenario::DeleteTrack(const TrackId& aTrackId)
{
   Track* track = FindTrack(aTrackId);
   if (track)
   {
      DeleteEntity(track);
   }
}

void wkf::Scenario::DeleteAllTracks()
{
   // Prevent the handle for RemoveEntity from corrupting the iterator on mTrackMap
   mRemovingAllTracks = true;
   for (auto& track : mTrackMap)
   {
      DeleteEntity(track.second);
   }
   mRemovingAllTracks = false;
   mTrackMap.clear();
}

void wkf::Scenario::MoveSelection(vespa::VaViewer*          aViewerPtr,
                                  const vespa::VaEntityMap& aEntityMap,
                                  vespa::VaAttachmentList&  aAttachmentList,
                                  UtEntity&                 aRefBefore,
                                  UtEntity&                 aRefAfter,
                                  int                       aFeedbackFormat,
                                  double                    aFeedbackPrecision,
                                  bool                      aOverrideModifiable)
{
   vespa::VaEntityMap entities = aEntityMap;
   for (auto it = entities.cbegin(); it != entities.cend();)
   {
      auto* platformPtr = dynamic_cast<wkf::Platform*>(it->second);
      if (platformPtr)
      {
         if (mMovementEnabled)
         {
            platformPtr->SetBeingDragged(true);
            ++it;
         }
         else
         {
            it = entities.erase(it);
         }
      }
      else
      {
         ++it;
      }
   }
   // Call base class
   // when no movement is enabled, there are no platforms, and the remaining entities/attachments are allowed to be moved
   vespa::VaScenario::MoveSelection(aViewerPtr,
                                    entities,
                                    aAttachmentList,
                                    aRefBefore,
                                    aRefAfter,
                                    aFeedbackFormat,
                                    aFeedbackPrecision,
                                    !mMovementEnabled);
}

void wkf::Scenario::MoveSelectionComplete(vespa::VaViewer*         aViewerPtr,
                                          vespa::VaEntityMap&      aEntityMap,
                                          vespa::VaAttachmentList& aAttachmentList)
{
   PlatformList pList;
   EntityList   nList;
   for (auto& iter : aEntityMap)
   {
      auto* platformPtr = dynamic_cast<wkf::Platform*>(iter.second);
      if (platformPtr)
      {
         if (mMovementEnabled)
         {
            pList.push_back(platformPtr);
            platformPtr->SetBeingDragged(false);
         }
      }
      else
      {
         wkf::Entity* entityPtr = dynamic_cast<wkf::Entity*>(iter.second);
         if (entityPtr)
         {
            nList.push_back(entityPtr);
         }
      }
   }
   if (mMovementEnabled)
   {
      // Call base class
      vespa::VaScenario::MoveSelectionComplete(aViewerPtr, aEntityMap, aAttachmentList);

      // check if plist is empty if not, emit it.
      if (!pList.isEmpty())
      {
         emit wkfEnv.MoveSelectionComplete(pList);
      }
   }
   emit wkfEnv.MoveSelectionAndAttachmentComplete(pList, nList, aAttachmentList);
}

bool wkf::Scenario::GetRangeBearingFromNearestBullseye(const vespa::VaPosition& aPosition,
                                                       double&                  aRange_m,
                                                       double&                  aBearing_deg,
                                                       std::string&             aBullseyeName) const
{
   static constexpr double cMAX_BULLSEYE_RANGE_M = 10000000; // 10,000 km

   if (vaEnv.GetStandardScenario())
   {
      aRange_m           = cMAX_BULLSEYE_RANGE_M;
      double heading_deg = 0;
      double distance_m  = 0;

      // If there are bullseyes, find the closest one
      for (const auto& bullseye : vaEnv.GetStandardScenario()->GetBullseyes())
      {
         bullseye->GetPosition().CalculateGreatCircleHeadingAndDistance(aPosition, heading_deg, distance_m);
         if (distance_m < aRange_m)
         {
            aRange_m      = distance_m;
            aBearing_deg  = heading_deg;
            aBullseyeName = bullseye->GetName();
         }
      }
      // If a bullseye exist and is within max range, then aRange_m will be less than the max_range
      return (aRange_m < cMAX_BULLSEYE_RANGE_M);
   }
   return false;
}
