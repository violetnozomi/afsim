// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfMTT_Correlation.hpp"

#include <sstream>
#include <string>

#include "MTT.hpp"
#include "MTT_PerceivedCluster.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMTT_Interface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfUtil.hpp"

WsfMTT_Correlation::WsfMTT_Correlation()
   : WsfCorrelationStrategy()
   , mMTT_InterfacePtr()
   , mMTT_Ptr(std::make_shared<MTT>())
   , mNLT_IdToAT_IdMap()
   , mLT_IdToAT_IdMap()
   , mPerceivedClusterMap()
{
}

WsfMTT_Correlation::WsfMTT_Correlation(const WsfMTT_Correlation& aSrc)
   : WsfCorrelationStrategy(aSrc)
   , mMTT_InterfacePtr(aSrc.mMTT_InterfacePtr)
   , mMTT_Ptr(std::make_shared<MTT>(*aSrc.mMTT_Ptr))
   , mNLT_IdToAT_IdMap()
   , mLT_IdToAT_IdMap()
   , mPerceivedClusterMap()
{
}

// virtual
bool WsfMTT_Correlation::ProcessInput(UtInput& aInput)
{
   return mMTT_Ptr->ProcessInput(aInput);
}

// virtual
bool WsfMTT_Correlation::Initialize(WsfTrackManager* aTrackManagerPtr)
{
   bool initialized = WsfCorrelationStrategy::Initialize(aTrackManagerPtr);

   // Subscribe to notifications from track manager:
   mCallbacks.Add(aTrackManagerPtr->LocalTrackCorrelation.Connect(&WsfMTT_Correlation::LocalTrackCorrelated, this));
   mCallbacks.Add(aTrackManagerPtr->LocalTrackDecorrelation.Connect(&WsfMTT_Correlation::LocalTrackDecorrelated, this));
   mCallbacks.Add(aTrackManagerPtr->LocalTrackDropped.Connect(&WsfMTT_Correlation::LocalTrackDropped, this));
   mCallbacks.Add(aTrackManagerPtr->RawTrackDropped.Connect(&WsfMTT_Correlation::RawTrackDropped, this));

   // Set the track manager to not initiate local tracks (they must be initiated from the correlator).
   GetTrackManager().SetInitiateLocalTracks(false);

   // Needed for debug and event output.
   mMTT_Ptr->SetTrackManager(aTrackManagerPtr);

   mMTT_InterfacePtr = static_cast<WsfMTT_Interface*>(aTrackManagerPtr->GetSimulation()->FindExtension("wsf_mtt"));
   mMTT_InterfacePtr->Register(mMTT_Ptr, GetTrackManager().GetPlatform());

   return initialized;
}

//! Return the correlated active track id for the given local track id.
//! If no correlation exists, -1 is returned.
int WsfMTT_Correlation::GetActiveTrackId(const WsfTrackId& aLocalTrackId) const
{
   auto iter = mLT_IdToAT_IdMap.find(aLocalTrackId);
   if (iter != mLT_IdToAT_IdMap.end())
   {
      return iter->second;
   }
   return -1;
}

// virtual
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
WsfLocalTrack* WsfMTT_Correlation::CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList)
{
   // Need WsfTrack to MTT_PerceivedCluster MTT_PerceivedCluster(const WsfTrack&)
   WsfLocalTrack* localTrackPtr = nullptr;

   MTT_PerceivedCluster* nonLocalTrackPtr = nullptr;
   int                   MTT_TrackId      = WsfUtil::TrackIdToInt(aNonLocalTrack.GetTrackId());
   auto                  pcIter           = mPerceivedClusterMap.find(MTT_TrackId);
   if (pcIter == mPerceivedClusterMap.end())
   {
      auto inserted    = mPerceivedClusterMap.emplace(MTT_TrackId, ut::make_unique<MTT_PerceivedCluster>());
      nonLocalTrackPtr = inserted.first->second.get();
   }
   else
   {
      nonLocalTrackPtr = pcIter->second.get();
   }

   nonLocalTrackPtr->CopyFrom(aSimTime, mMTT_InterfacePtr, aNonLocalTrack);

   if (aNonLocalTrack.ElevationValid() && (!aNonLocalTrack.LocationValid()) && (!aNonLocalTrack.BearingValid()))
   {
      // Need to correlate this track for a height-finder measurement per suppressor.
      localTrackPtr = mMTT_Ptr->mHF_Correlator.Correlate(aSimTime, aNonLocalTrack, GetTrackManager().GetTrackList());
      if (localTrackPtr != nullptr)
      {
         int activeTrackId = GetActiveTrackId(localTrackPtr->GetTrackId());
         if (activeTrackId != -1)
         {
            nonLocalTrackPtr->mTrackNumber = activeTrackId;
         }
      }
   }

   MTT_ActiveTrack::Data* tempDataPtr = new MTT_ActiveTrack::Data(); // Used by the tracker correlator and fuser.

   if (GetTrackManager().DebugEnabled())
   {
      auto iter = mNLT_IdToAT_IdMap.find(aNonLocalTrack.GetTrackId());
      if (iter != mNLT_IdToAT_IdMap.end())
      {
         auto logger = ut::log::debug() << "Attempting to re-correlate non-local track with active track.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aNonLocalTrack.GetTrackId();
         logger.AddNote() << "AT: " << iter->second;
      }
      else
      {
         auto logger = ut::log::debug()
                       << "Attempting to correlate non-local track (not previously correlated with MTT active track).";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aNonLocalTrack.GetTrackId();
      }
   }

   MTT_ActiveTrack* activeTrackPtr = mMTT_Ptr->MTT_Correlate(aSimTime, *nonLocalTrackPtr, *tempDataPtr);
   if (activeTrackPtr != nullptr)
   {
      // Is there a local track already mapped to this active track?
      for (const auto& ids : mLT_IdToAT_IdMap)
      {
         if (ids.second == activeTrackPtr->GetId())
         {
            localTrackPtr = GetTrackManager().FindTrack(ids.first);
            assert(localTrackPtr != nullptr);
            break;
         }
      }

      if (localTrackPtr == nullptr) // no track initiated
      {
         localTrackPtr = GetTrackManager().InitiateTrackSilent(aSimTime, aNonLocalTrack);
         assert(localTrackPtr != nullptr);
         GetTrackManager().GetCorrelationStrategy().NewTrackCorrelation(aNonLocalTrack, *localTrackPtr);
         if (localTrackPtr->GetAuxDataConst().AttributeExists("IsNewActiveTrack"))
         {
            localTrackPtr->GetAuxData().Assign("IsNewActiveTrack", false);
            GetTrackManager().NotifyOfLocalTrackInitiated(aSimTime, localTrackPtr, &aNonLocalTrack);
         }
         else
         {
            // MJM Don't notify until after the correlation step;
            // (if the track is communicated the receiver might try to fuse, and this is not possible without the MTT
            // aux data). Mark as a new active track.
            localTrackPtr->GetAuxData().Assign("IsNewActiveTrack", true);
         }
      }

      // Store data in local maps.
      mNLT_IdToAT_IdMap[aNonLocalTrack.GetTrackId()] = activeTrackPtr->GetId();
      mLT_IdToAT_IdMap[localTrackPtr->GetTrackId()]  = activeTrackPtr->GetId();

      // The non local track is correlated with the active track
      if (GetTrackManager().DebugEnabled())
      {
         auto logger = ut::log::debug() << "Correlation of non-local track with MTT active track.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aNonLocalTrack.GetTrackId();
         logger.AddNote() << "MTT::AT: " << activeTrackPtr->mTrackId;
         logger.AddNote() << "TrackManager's correlation of NLT is with: " << localTrackPtr->GetTrackId();
         auto                                 note = logger.AddNote() << "Correlations:";
         const WsfLocalTrack::RawTrackIdList& list = localTrackPtr->GetRawTrackIds();
         for (unsigned i = 0; i < list.GetCount(); ++i)
         {
            note.AddNote() << *list.GetEntry(i);
         }
         if (list.GetCount() == 0)
         {
            note.AddNote() << "N/A.";
         }
      }

      // need to save activeTrackPtr, tempData, and nonLocalTrack in MTT object for call to Fusion.
      mMTT_Ptr->SaveData(nonLocalTrackPtr, tempDataPtr, activeTrackPtr);
   }
   else // no corresponding active track, or existing nlt no longer correlates with an active track
   {
      DecorrelateNonlocalTrack(aNonLocalTrack);

      // Is there an existing correlation from the track manager?
      localTrackPtr = GetTrackManager().FindCorrelatedTrack(aNonLocalTrack.GetTrackId());

      if (localTrackPtr != nullptr)
      {
         bool decorrelate = true;
         for (unsigned rt = 0; rt < localTrackPtr->GetRawTrackIds().GetCount(); ++rt)
         {
            WsfTrack* rtPtr = GetTrackManager().FindRawTrack(*localTrackPtr->GetRawTrackIds().GetEntry(rt));
            if ((rtPtr->GetTrackId() != aNonLocalTrack.GetTrackId()) &&
                (rtPtr->LocationValid() || (rtPtr->RangeValid() && rtPtr->BearingValid())))
            {
               decorrelate = false;
            }
         }
         if (decorrelate)
         {
            localTrackPtr->DecorrelateAll();
            for (unsigned rt = 0; rt < localTrackPtr->GetRawTrackIds().GetCount(); ++rt)
            {
               DecorrelateNonlocalTrack(*GetTrackManager().FindRawTrack(*localTrackPtr->GetRawTrackIds().GetEntry(rt)));
            }
         }
         localTrackPtr = nullptr;
      }
      delete tempDataPtr;
   }

   return localTrackPtr;
}

void WsfMTT_Correlation::DecorrelateNonlocalTrack(const WsfTrack& aNonLocalTrack)
{
   // Make sure all active track correlations are dropped, for the NLT.
   auto iter = mNLT_IdToAT_IdMap.find(aNonLocalTrack.GetTrackId());
   if (iter != mNLT_IdToAT_IdMap.end())
   {
      if (GetTrackManager().DebugEnabled())
      {
         auto logger = ut::log::debug() << "Non-Local Track DeCorrelation:";
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aNonLocalTrack.GetTrackId();
         logger.AddNote() << "MTT::AT: " << iter->second;
      }
      mNLT_IdToAT_IdMap.erase(iter);
   }
}

void WsfMTT_Correlation::LocalTrackCorrelated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrackId& aRawTrackId)
{
   // Do we have the source track id mapped?
   auto iter = mNLT_IdToAT_IdMap.find(aRawTrackId);
   if (iter != mNLT_IdToAT_IdMap.end())
   {
      int atId = iter->second;
      // mMTT_Ptr->MapCorrelation(atId, trackPtr->GetTrackId());
      mLT_IdToAT_IdMap[aLocalTrackPtr->GetTrackId()] = atId;
      if (GetTrackManager().DebugEnabled())
      {
         auto logger = ut::log::debug() << "Local Track Correlation:";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aRawTrackId;
         logger.AddNote() << "MTT::AT: " << iter->second;
         logger.AddNote() << "LT: " << aLocalTrackPtr->GetTrackId();
      }
   }
}

void WsfMTT_Correlation::LocalTrackDecorrelated(double               aSimTime,
                                                const WsfLocalTrack* aLocalTrackPtr,
                                                const WsfTrackId&    aRawTrackId)
{
   // Do we have the source track id mapped?
   auto iter = mNLT_IdToAT_IdMap.find(aRawTrackId);
   if (iter != mNLT_IdToAT_IdMap.end())
   {
      if (GetTrackManager().DebugEnabled())
      {
         auto logger = ut::log::debug() << "Local Track Decorrelation:";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aRawTrackId;
         logger.AddNote() << "MTT::AT: " << iter->second;
         logger.AddNote() << "LT: " << aLocalTrackPtr->GetTrackId();
      }
      mNLT_IdToAT_IdMap.erase(iter);
   }
}

void WsfMTT_Correlation::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   auto iter = mLT_IdToAT_IdMap.find(aLocalTrackPtr->GetTrackId());
   if (iter != mLT_IdToAT_IdMap.end())
   {
      if (GetTrackManager().DebugEnabled())
      {
         auto logger = ut::log::debug() << "Local track dropped.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "LT: " << aLocalTrackPtr->GetTrackId();
         logger.AddNote() << "MTT::AT: " << iter->second;
      }
      mLT_IdToAT_IdMap.erase(iter);
   }
}

void WsfMTT_Correlation::RawTrackDropped(double aSimTime, const WsfTrack* aRawTrackPtr)
{
   // Erase any NLT associations with active tracks.
   auto iter = mNLT_IdToAT_IdMap.find(aRawTrackPtr->GetTrackId());
   if (iter != mNLT_IdToAT_IdMap.end())
   {
      if (GetTrackManager().DebugEnabled())
      {
         auto logger = ut::log::debug() << "Raw track dropped.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
         logger.AddNote() << "NLT: " << aRawTrackPtr->GetTrackId();
         logger.AddNote() << "MTT::AT: " << iter->second;
      }
      mNLT_IdToAT_IdMap.erase(iter);
   }

   // Check whether we have this track mapped to a perceived cluster.
   // If so, there is no reason to keep it mapped.
   int MTT_TrackId = WsfUtil::TrackIdToInt(aRawTrackPtr->GetTrackId());
   mPerceivedClusterMap.erase(MTT_TrackId);

   // Get the local track; if there are only HF tracks left, decorrelate them from the local track.
   WsfLocalTrack* ltPtr = GetTrackManager().FindCorrelatedTrack(aRawTrackPtr->GetTrackId());
   if (ltPtr != nullptr)
   {
      bool decorrelate = true;
      for (unsigned rt = 0; rt < ltPtr->GetRawTrackIds().GetCount(); ++rt)
      {
         WsfTrack* rtPtr = GetTrackManager().FindRawTrack(*ltPtr->GetRawTrackIds().GetEntry(rt));
         if (rtPtr->LocationValid() || (rtPtr->RangeValid() && rtPtr->BearingValid()))
         {
            decorrelate = false;
         }
      }
      if (decorrelate)
      {
         ltPtr->DecorrelateAll();
      }
   }
}
