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

#include "WsfTruthCorrelation.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
WsfTruthCorrelation::WsfTruthCorrelation()
   : WsfCorrelationStrategy()
   , mChangeTime(0.0)
   , // default
   mMaxDistanceForCorrelation(1000.0)
   , mIgnoreTrackTarget(false)
   , mIgnoreSameSide(false)
{
}

// =================================================================================================
WsfTruthCorrelation::~WsfTruthCorrelation() {}

// =================================================================================================
// virtual
bool WsfTruthCorrelation::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "evaluation_interval")
   {
      aInput.ReadValueOfType(mChangeTime, UtInput::cTIME);
   }
   else if (command == "ignore_track_target")
   {
      aInput.ReadValue(mIgnoreTrackTarget);
   }
   else if (command == "maximum_correlation_distance")
   {
      aInput.ReadValueOfType(mMaxDistanceForCorrelation, UtInput::cLENGTH);
   }
   else if (command == "ignore_same_side")
   {
      aInput.ReadValue(mIgnoreSameSide);
   }
   else
   {
      myCommand = WsfCorrelationStrategy::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
// virtual
bool WsfTruthCorrelation::Initialize(WsfTrackManager* aTrackManagerPtr)
{
   bool ok = WsfCorrelationStrategy::Initialize(aTrackManagerPtr);
   mCallbacks += GetTrackManager().LocalTrackCorrelation.Connect(&WsfTruthCorrelation::LocalTrackCorrelation, this);
   return ok;
}

// =================================================================================================
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
// virtual
WsfLocalTrack* WsfTruthCorrelation::CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList)
{
   WsfLocalTrack* localTrackPtr = WsfTrackManager::FindCorrelatedTrack(aNonLocalTrack.GetTrackId(), aTrackList);
   WsfTrackId     correlatedTrackId; // initializes to null
   bool           doCorrelate = true;

   if (localTrackPtr != nullptr)
   {
      correlatedTrackId = localTrackPtr->GetTrackId();

      // Don't bother to re-evaluate if there is an existing truth association in the nonlocal track
      if ((aNonLocalTrack.GetTargetIndex() == 0) || (mIgnoreTrackTarget))
      {
         std::map<WsfTrackId, double>::iterator iter = mChangeMap.find(aNonLocalTrack.GetTrackId());
         if (iter != mChangeMap.end())
         {
            if ((aSimTime - iter->second) > mChangeTime)
            {
               mChangeMap[aNonLocalTrack.GetTrackId()] = aSimTime + mChangeTime;
            }
            else
            {
               doCorrelate = false;
            }
         }
      }
      else
      {
         // We have a correlation based on truth; no need to evaluate further.
         doCorrelate = false;
      }
   }

   WsfPlatform* targetPtr = nullptr;

   if ((aNonLocalTrack.GetTargetIndex() != 0) && (!mIgnoreTrackTarget))
   {
      // Attempt to use the truth, if available.
      targetPtr = GetSimulation()->GetPlatformByIndex(aNonLocalTrack.GetTargetIndex());
   }
   else
   {
      std::map<WsfTrackId, size_t>::iterator iter = mTruthMap.find(aNonLocalTrack.GetTrackId());
      if (iter != mTruthMap.end())
      {
         targetPtr = GetSimulation()->GetPlatformByIndex(iter->second);
      }
      if (targetPtr == nullptr)
      {
         targetPtr = FindClosestPlatform(aSimTime, aNonLocalTrack);
      }
   }

   if (targetPtr != nullptr)
   {
      if (aNonLocalTrack.GetTargetIndex() == 0)
      {
         // Always fill this in, because a replacement update (usually performed in default fusion)
         // will overwrite any local track target information with the non-local track information.
         // Even if there is a correlation, the truth target information in the local track is erased.
         const_cast<WsfTrack&>(aNonLocalTrack).SetTargetIndex(targetPtr->GetIndex());
         const_cast<WsfTrack&>(aNonLocalTrack).SetTargetName(targetPtr->GetNameId());
         const_cast<WsfTrack&>(aNonLocalTrack).SetTargetType(targetPtr->GetTypeId());
      }

      if (doCorrelate)
      {
         localTrackPtr                          = nullptr; // re-correlate.
         bool localTrackExists                  = false;
         mTruthMap[aNonLocalTrack.GetTrackId()] = targetPtr->GetIndex();

         for (unsigned int trackNum = 0; trackNum < aTrackList.GetTrackCount(); ++trackNum)
         {
            localTrackPtr = aTrackList.GetTrackEntry(trackNum);

            // Use perfect correlation once we establish the truth association.
            localTrackExists = (targetPtr->GetNameId() == localTrackPtr->GetTargetName()) && (localTrackPtr->IsFusible());

            if (localTrackExists)
            {
               break;
            }
         }

         if (localTrackExists)
         {
            if (correlatedTrackId != localTrackPtr->GetTrackId())
            {
               localTrackPtr->Correlate(aNonLocalTrack.GetTrackId());

               // Set the truth id
               localTrackPtr->SetTargetIndex(targetPtr->GetIndex());
               localTrackPtr->SetTargetName(targetPtr->GetNameId());
               localTrackPtr->SetTargetType(targetPtr->GetTypeId());

               // Notify observers
               NotifyOfLocalTrackCorrelation(aSimTime, *localTrackPtr, aNonLocalTrack.GetTrackId());
               assert(localTrackPtr->IsCorrelatedWith(aNonLocalTrack.GetTrackId()));
            }

            // Store update time in change map.
            mChangeMap[aNonLocalTrack.GetTrackId()] = aSimTime;
         }
         else
         {
            localTrackPtr = nullptr;
         }
      }
   }

   if (localTrackPtr == nullptr)
   {
      if (!aNonLocalTrack.LocationValid() &&
          (!(aNonLocalTrack.RangeValid() && aNonLocalTrack.BearingValid() && aNonLocalTrack.ElevationValid())))
      {
         // If this is a mirror of a local track, return the mirror.
         // This will prevent blow-up in the track list.
         for (unsigned int trackNum = 0; trackNum < aTrackList.GetTrackCount(); ++trackNum)
         {
            WsfLocalTrack* candidatePtr = aTrackList.GetTrackEntry(trackNum);
            if (candidatePtr->IsMirrorTrack(aNonLocalTrack))
            {
               localTrackPtr = candidatePtr;
               break;
            }
         }
      }
   }

   return localTrackPtr;
}

// =================================================================================================
WsfPlatform* WsfTruthCorrelation::FindClosestPlatform(double aSimTime, const WsfTrack& aNonLocalTrack)
{
   WsfPlatform* bestTargetPtr = nullptr;

   if (aNonLocalTrack.LocationValid())
   {
      double bestTargetPriority = -1.0E+30;

      double trackLocWCS[3];
      aNonLocalTrack.GetExtrapolatedLocationWCS(aSimTime, trackLocWCS);

      // only handle this case for correlation
      WsfSimulation& sim = *GetSimulation();
      for (unsigned int entry = 0; entry < sim.GetPlatformCount(); ++entry)
      {
         WsfPlatform* targetPtr = sim.GetPlatformEntry(entry);
         if (targetPtr == nullptr)
         {
            continue;
         }

         // Don't consider the originating sensor's platform.
         if (targetPtr->GetNameId() == aNonLocalTrack.GetOriginatorNameId())
         {
            continue;
         }

         // Only choose targets that are not on the same team.
         if (mIgnoreSameSide && (aNonLocalTrack.SideIdValid() && (targetPtr->GetSideId() != aNonLocalTrack.GetSideId())))
         {
            continue;
         }

         double targetLocWCS[3];
         targetPtr->GetLocationWCS(targetLocWCS);
         double missOffsetVecWCS[3];
         UtVec3d::Subtract(missOffsetVecWCS, targetLocWCS, trackLocWCS);
         double missDistance = UtVec3d::Magnitude(missOffsetVecWCS);
         // The default priority is '-distance' (closer targets will have higher priority)
         double targetPriority = -missDistance;

         if ((targetPriority >= bestTargetPriority) && (missDistance <= mMaxDistanceForCorrelation))
         {
            if (GetTrackManager().DebugEnabled())
            {
               auto out = ut::log::debug() << "Truth Correlation: Now selected:";
               out.AddNote() << "Target: " << targetPtr->GetName();
               out.AddNote() << "Target Type: " << targetPtr->GetType();
               out.AddNote() << "Miss Distance: " << missDistance;
               out.AddNote() << "Priority: " << targetPriority;
            }
            bestTargetPtr      = targetPtr;
            bestTargetPriority = targetPriority;
         }
      }
   }

   return bestTargetPtr;
}

// =================================================================================================
void WsfTruthCorrelation::LocalTrackCorrelation(double               aSimTime,
                                                const WsfLocalTrack* aLocalTrackPtr,
                                                const WsfTrackId&    aNonLocalTrackId)
{
   // If no truth id in the local track, check our map for the truth correlation
   if ((aLocalTrackPtr->GetTargetIndex() == 0) || (mIgnoreTrackTarget))
   {
      std::map<WsfTrackId, size_t>::iterator iter = mTruthMap.find(aNonLocalTrackId);
      if (iter != mTruthMap.end())
      {
         WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(iter->second);
         if (targetPtr != nullptr)
         {
            const_cast<WsfLocalTrack*>(aLocalTrackPtr)->SetTargetIndex(iter->second);
            const_cast<WsfLocalTrack*>(aLocalTrackPtr)->SetTargetName(targetPtr->GetNameId());
            const_cast<WsfLocalTrack*>(aLocalTrackPtr)->SetTargetType(targetPtr->GetTypeId());
         }
      }
   }
}
