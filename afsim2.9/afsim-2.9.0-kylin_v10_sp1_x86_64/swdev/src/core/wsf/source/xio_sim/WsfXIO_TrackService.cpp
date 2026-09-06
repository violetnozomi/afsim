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

#include "xio_sim/WsfXIO_TrackService.hpp"

#include "UtMemory.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackObserver.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_TrackService::WsfXIO_TrackService(WsfXIO_Simulation& aXIO_Simulation)
   : BaseClassType(aXIO_Simulation.GetInterface())
   , mSimulation(aXIO_Simulation.GetSimulation())
{
   mCallbacks.Add(GetInterface()->Connect(&WsfXIO_TrackService::HandleRequest, this));
   mCallbacks.Add(GetInterface()->Connect(&WsfXIO_TrackService::HandleExtendedTrackInfoRequest, this));
   WsfXIO_SimService::Initialize(aXIO_Simulation.GetSimulation());
}

void WsfXIO_TrackService::HandleRequest(WsfXIO_RequestTracksPkt& aPkt)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr == nullptr)
   {
      DenyRequest(aPkt);
      return;
   }
   WsfTrackManager& trackManager = platformPtr->GetTrackManager();

   if (!aPkt.mSensorNameId)
   {
      WsfXIO_Subscription* subPtr = CreateSubscription(aPkt);
      AddRequiredPlatform(*platformPtr, subPtr);

      if (aPkt.mUpdateInterval == 0.0)
      {
         PlatformObserver* observerPtr = FindPlatformObserver(aPkt.mPlatformIndex);
         observerPtr->Add(subPtr->GetDataConnection());
         observerPtr->mRequiredData |= aPkt.mRequiredData;
      }
      else
      {
         auto eventPtr             = ut::make_unique<UpdateSendEvent>(subPtr, platformPtr);
         eventPtr->mUpdateInterval = aPkt.mUpdateInterval;
         eventPtr->mRequiredData   = aPkt.mRequiredData;
         mUpdateSendEvents.push_back(eventPtr.get());
         if (aPkt.mRealtimeInterval)
         {
            eventPtr->SetTime(mSimulation.GetWallTime());
            mSimulation.AddWallEvent(std::move(eventPtr));
         }
         else
         {
            eventPtr->SetTime(mSimulation.GetSimTime());
            mSimulation.AddEvent(std::move(eventPtr));
         }
      }

      if (aPkt.mSendInitialTracks)
      {
         int32_t platformIndex = static_cast<int32_t>(platformPtr->GetIndex());
         if (aPkt.mRequiredData & WsfXIO_RequestTracksPkt::cLOCAL_TRACKS)
         {
            WsfLocalTrackList& localTracks = trackManager.GetTrackList();
            for (unsigned int i = 0; i < localTracks.GetTrackCount(); ++i)
            {
               WsfLocalTrack*       trackPtr = localTracks.GetTrackEntry(i);
               WsfXIO_LocalTrackPkt pkt;
               pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
               pkt.mPlatformIndex = platformIndex;
               pkt.mTrack         = trackPtr;
               subPtr->SendResponse(pkt);
            }
         }
         if (aPkt.mRequiredData & WsfXIO_RequestTracksPkt::cRAW_TRACKS)
         {
            WsfTrackList& rawTracks = trackManager.GetRawTrackList();
            for (unsigned int i = 0; i < rawTracks.GetTrackCount(); ++i)
            {
               WsfXIO_RawTrackPkt pkt;
               pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
               pkt.mPlatformIndex = platformIndex;
               pkt.mTrack         = rawTracks.GetTrackEntry(i);
               subPtr->SendResponse(pkt);
            }
         }
      }
   }
   else
   {
      WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(aPkt.mSensorNameId);
      if (sensorPtr != nullptr)
      {
         WsfXIO_Subscription* subPtr = CreateSubscription(aPkt);
         AddRequiredPlatform(*platformPtr, subPtr);
         SensorObserver*             observerPtr;
         SensorObserverMap::iterator i = mSensorObservers.find(sensorPtr);
         if (i == mSensorObservers.end())
         {
            observerPtr                 = new SensorObserver;
            observerPtr->mSensorPtr     = sensorPtr;
            observerPtr->mRequiredData  = aPkt.mRequiredData;
            mSensorObservers[sensorPtr] = observerPtr;
         }
         else
         {
            observerPtr = i->second;
            observerPtr->mRequiredData |= aPkt.mRequiredData;
         }
         observerPtr->Add(subPtr->GetDataConnection());
         UpdateSensorCallbacks();
      }
   }
}

// =================================================================================================
//! Handle the WsfXIO_ExtendedTrackInfoRequestPkt.
//! This returns the list of all contributing sensors (direct and indirect) for a local track.
void WsfXIO_TrackService::HandleExtendedTrackInfoRequest(WsfXIO_ExtendedTrackInfoRequestPkt& aPkt)
{
   WsfXIO_Connection* senderPtr = dynamic_cast<WsfXIO_Connection*>(aPkt.GetSender());
   if (senderPtr == nullptr)
   {
      return;
   }

   // Initialize the response packet.
   WsfXIO_ExtendedTrackInfoPkt pkt;
   pkt.mQueryId    = aPkt.mQueryId;
   pkt.mTrackId    = aPkt.mTrackId;
   pkt.mLocalTrack = nullptr;

   // Populate the return packet with the contributing sensor tracks.
   std::set<WsfTrackId> selectedTrackIds; // The set of selected track IDs (to prevent duplication)
   std::set<WsfTrackId> searchedTrackIds; // The set of searched track IDs (to prevent runaway recursion)
   SelectSensorTracks(pkt, pkt.mTrackId, selectedTrackIds, searchedTrackIds);

   senderPtr->Send(pkt);
}

// =================================================================================================
//! Select all the contributing sensor tracks a given track ID.
//! @param aPkt The packet to be filled.
//! @param aOtherTrackId The track ID to be processed.
//! @param aSelectedTrackIds A set of track IDs that have already been selected (to prevent duplicates)
//! @param aSearchedTrackIds The set of tracks that have already been searched (to prevent infinite loops)
void WsfXIO_TrackService::SelectSensorTracks(WsfXIO_ExtendedTrackInfoPkt& aPkt,
                                             const WsfTrackId&            aLocalTrackId,
                                             std::set<WsfTrackId>&        aSelectedTrackIds,
                                             std::set<WsfTrackId>&        aSearchedTrackIds)
{
   // Locate the platform that owns the track.
   WsfPlatform* platformPtr = mSimulation.GetPlatformByName(aLocalTrackId.GetOwningPlatformId());
   if (platformPtr == nullptr)
   {
      return;
   }

   // Find the track in their local track list.
   WsfLocalTrack* localTrackPtr = platformPtr->GetMasterTrackList().FindTrack(aLocalTrackId);
   if (localTrackPtr == nullptr)
   {
      return;
   }

   if (aPkt.mLocalTrack == nullptr)
   {
      aPkt.mLocalTrack = localTrackPtr;
   }

   // A track in the local track list should not be a sensor track, but we will handle it if it is.
   if (localTrackPtr->GetSensorNameId() != 0)
   {
      if (aSelectedTrackIds.find(aLocalTrackId) == aSelectedTrackIds.end())
      {
         aSelectedTrackIds.insert(aLocalTrackId);
         aPkt.mTracks.push_back(localTrackPtr);
      }
   }

   // Recursively handle the contributing tracks.
   unsigned int rawTrackCount = static_cast<unsigned int>(localTrackPtr->GetRawTrackIds().GetCount());
   for (unsigned int rawTrackIndex = 0; rawTrackIndex < rawTrackCount; ++rawTrackIndex)
   {
      const WsfTrackId* rawTrackIdPtr = localTrackPtr->GetRawTrackIds().GetEntry(rawTrackIndex);
      WsfTrack*         rawTrackPtr   = platformPtr->GetMasterRawTrackList().FindTrack(*rawTrackIdPtr);
      if (rawTrackPtr != nullptr)
      {
         if (rawTrackPtr->GetSensorNameId() != 0)
         {
            if (aSelectedTrackIds.find(*rawTrackIdPtr) == aSelectedTrackIds.end())
            {
               aSelectedTrackIds.insert(*rawTrackIdPtr);
               aPkt.mTracks.push_back(rawTrackPtr);
            }
         }
         else if (aSearchedTrackIds.find(*rawTrackIdPtr) == aSearchedTrackIds.end())
         {
            aSearchedTrackIds.insert(*rawTrackIdPtr);
            SelectSensorTracks(aPkt, *rawTrackIdPtr, aSelectedTrackIds, aSearchedTrackIds);
         }
      }
   }
}

WsfXIO_TrackService::PlatformObserver::PlatformObserver(WsfPlatform* aPlatformPtr)
   : mRequiredData(0)
   , mPlatformPtr(aPlatformPtr)
{
   mTrackManagerPtr = &mPlatformPtr->GetTrackManager();

   auto localTracksCb = [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
   {
      if (mRequiredData & WsfXIO_RequestTracksPkt::cLOCAL_TRACKS)
      {
         WsfXIO_LocalTrackPkt pkt;
         pkt.mFromProvider = true;
         pkt.mRequestId    = 0;
         pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
         pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
         auto trackCopy     = ut::make_unique<WsfLocalTrack>(*aLocalTrackPtr);
         // pkt.mTrack = (WsfLocalTrack*)changedTrackPtr;
         trackCopy->KinematicUpdate(mPlatformPtr->GetSimulation()->GetSimTime());
         pkt.mTrack = trackCopy.get();
         Send(pkt);
      }
   };
   mCallbacks.Add(mTrackManagerPtr->LocalTrackUpdated.Connect(localTracksCb));
   mCallbacks.Add(mTrackManagerPtr->LocalTrackInitiated.Connect(localTracksCb));
   mCallbacks.Add(mTrackManagerPtr->LocalTrackDropped.Connect(
      [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
      {
         if (mRequiredData & WsfXIO_RequestTracksPkt::cLOCAL_DROPPED_TRACKS)
         {
            WsfXIO_TrackDropPkt pkt;
            pkt.mFromProvider = true;
            pkt.mRequestId    = 0;
            pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
            pkt.mIsRawTrack    = false;
            pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
            pkt.mTrackId       = aLocalTrackPtr->GetTrackId();
            Send(pkt);
         }
      }));
   mCallbacks.Add(mTrackManagerPtr->RawTrackReceived.Connect(
      [this](double aSimTime, const WsfTrack* aRawTrackPtr)
      {
         if (mRequiredData & WsfXIO_RequestTracksPkt::cRAW_TRACKS)
         {
            WsfXIO_RawTrackPkt pkt;
            pkt.mFromProvider = true;
            pkt.mRequestId    = 0;
            pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
            pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
            pkt.mTrack         = const_cast<WsfTrack*>(aRawTrackPtr);
            Send(pkt);
         }
      }));
   mCallbacks.Add(mTrackManagerPtr->RawTrackDropped.Connect(
      [this](double aSimTime, const WsfTrack* aRawTrackPtr)
      {
         if (mRequiredData & WsfXIO_RequestTracksPkt::cRAW_DROPPED_TRACKS)
         {
            WsfXIO_TrackDropPkt pkt;
            pkt.mFromProvider = true;
            pkt.mRequestId    = 0;
            pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
            pkt.mIsRawTrack    = true;
            pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
            pkt.mTrackId       = aRawTrackPtr->GetTrackId();
            Send(pkt);
         }
      }));
}

WsfXIO_TrackService::PlatformObserver* WsfXIO_TrackService::FindPlatformObserver(int aPlatformIndex)
{
   PlatformObserver*             observerPtr = nullptr;
   PlatformObserverMap::iterator i           = mPlatformObservers.find(aPlatformIndex);
   if (i != mPlatformObservers.end())
   {
      observerPtr = i->second;
   }
   else
   {
      WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPlatformIndex);
      if (platformPtr != nullptr)
      {
         observerPtr                        = new PlatformObserver(platformPtr);
         mPlatformObservers[aPlatformIndex] = observerPtr;
      }
   }
   return observerPtr;
}

void WsfXIO_TrackService::SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, WsfXIO_RequestTracksPkt& aRequest)
{
   if (!aRequest.mSensorNameId)
   {
      if (aRequest.mUpdateInterval == 0.0)
      {
         PlatformObserver* observerPtr = FindPlatformObserver(aRequest.mPlatformIndex);
         if (observerPtr != nullptr)
         {
            observerPtr->Remove(aSubscriptionPtr->GetDataConnection());
            if (observerPtr->IsEmpty())
            {
               mPlatformObservers.erase(aRequest.mPlatformIndex);
            }
         }
      }
      else
      {
         for (size_t i = 0; i < mUpdateSendEvents.size(); ++i)
         {
            UpdateSendEvent* eventPtr = mUpdateSendEvents[i];
            if (eventPtr->mSubscriptionPtr == aSubscriptionPtr)
            {
               eventPtr->Disable();
               mUpdateSendEvents.erase(mUpdateSendEvents.begin() + i);
               break;
            }
         }
      }
   }
   else
   {
      WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aRequest.mPlatformIndex);
      if (platformPtr != nullptr)
      {
         WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(aRequest.mSensorNameId);
         if (sensorPtr != nullptr)
         {
            mSensorObservers.erase(sensorPtr);
            UpdateSensorCallbacks();
         }
      }
   }
   BaseClassType::SubscriptionCancelledT(aSubscriptionPtr, aRequest);
}

void WsfXIO_TrackService::SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorObserverMap::iterator i = mSensorObservers.find(aSensorPtr);
   if (i != mSensorObservers.end())
   {
      if ((i->second->mRequiredData & WsfXIO_RequestTracksPkt::cRAW_COASTED_TRACKS) != 0)
      {
         WsfXIO_TrackCoastPkt pkt;
         pkt.mPlatformIndex = static_cast<int32_t>(aSensorPtr->GetPlatform()->GetIndex());
         pkt.mTrackId       = aTrackPtr->GetTrackId();
         pkt.mSensorNameId  = aSensorPtr->GetNameId();
         i->second->Send(pkt);
      }
   }
}

void WsfXIO_TrackService::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorObserverMap::iterator i = mSensorObservers.find(aSensorPtr);
   if (i != mSensorObservers.end())
   {
      if ((i->second->mRequiredData & WsfXIO_RequestTracksPkt::cRAW_DROPPED_TRACKS) != 0)
      {
         WsfXIO_TrackDropPkt pkt;
         pkt.mPlatformIndex = static_cast<int32_t>(aSensorPtr->GetPlatform()->GetIndex());
         pkt.mTrackId       = aTrackPtr->GetTrackId();
         pkt.mSensorNameId  = aSensorPtr->GetNameId();
         i->second->Send(pkt);
      }
   }
}

void WsfXIO_TrackService::SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorObserverMap::iterator i = mSensorObservers.find(aSensorPtr);
   if (i != mSensorObservers.end())
   {
      if ((i->second->mRequiredData & WsfXIO_RequestTracksPkt::cRAW_TRACKS) != 0)
      {
         WsfXIO_RawTrackPkt pkt;
         pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
         pkt.mPlatformIndex = static_cast<int32_t>(aSensorPtr->GetPlatform()->GetIndex());
         pkt.mTrack         = aTrackPtr;
         pkt.mSensorNameId  = aSensorPtr->GetNameId();
         i->second->Send(pkt);
      }
   }
}

void WsfXIO_TrackService::UpdateSensorCallbacks()
{
   if (mSensorObservers.empty() != mSensorCallbacks.Empty())
   {
      if (mSensorObservers.empty())
      {
         mSensorCallbacks.Clear();
      }
      else
      {
         mSensorCallbacks +=
            WsfObserver::SensorTrackCoasted(&mSimulation).Connect(&WsfXIO_TrackService::SensorTrackCoasted, this);
         mSensorCallbacks +=
            WsfObserver::SensorTrackDropped(&mSimulation).Connect(&WsfXIO_TrackService::SensorTrackDropped, this);
         mSensorCallbacks +=
            WsfObserver::SensorTrackUpdated(&mSimulation).Connect(&WsfXIO_TrackService::SensorTrackUpdated, this);
         mSensorCallbacks +=
            WsfObserver::SensorTrackInitiated(&mSimulation).Connect(&WsfXIO_TrackService::SensorTrackUpdated, this);
      }
   }
}

WsfEvent::EventDisposition WsfXIO_TrackService::UpdateSendEvent::Execute()
{
   if (!mEnabled)
   {
      return cDELETE;
   }
   for (const auto& trackId : mLocalUpdateSet)
   {
      WsfLocalTrack* trackPtr = mTrackManagerPtr->FindTrack(trackId);
      if (trackPtr != nullptr)
      {
         WsfXIO_LocalTrackPkt pkt;
         pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
         pkt.mTrack         = trackPtr;
         mSubscriptionPtr->SendResponse(pkt);
      }
   }

   for (const auto& trackId : mRawUpdateSet)
   {
      WsfTrack* trackPtr = mTrackManagerPtr->FindRawTrack(trackId);
      if (trackPtr != nullptr)
      {
         WsfXIO_RawTrackPkt pkt;
         pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
         pkt.mTrack         = trackPtr;
         mSubscriptionPtr->SendResponse(pkt);
      }
   }

   for (const auto& trackId : mLocalDropSet)
   {
      WsfXIO_TrackDropPkt pkt;
      pkt.mIsRawTrack    = false;
      pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
      pkt.mTrackId       = trackId;
      mSubscriptionPtr->SendResponse(pkt);
   }

   for (const auto& trackId : mRawDropSet)
   {
      WsfXIO_TrackDropPkt pkt;
      pkt.mIsRawTrack    = true;
      pkt.mPlatformIndex = static_cast<int32_t>(mPlatformPtr->GetIndex());
      pkt.mTrackId       = trackId;
      mSubscriptionPtr->SendResponse(pkt);
   }

   mLocalUpdateSet.clear();
   mRawUpdateSet.clear();
   mLocalDropSet.clear();
   mRawDropSet.clear();

   SetTime(GetTime() + mUpdateInterval);
   return cRESCHEDULE;
}

WsfXIO_TrackService::UpdateSendEvent::UpdateSendEvent(WsfXIO_Subscription* aSubscriptionPtr, WsfPlatform* aPlatformPtr)
   : mSubscriptionPtr(aSubscriptionPtr)
   , mPlatformPtr(aPlatformPtr)
{
   mTrackManagerPtr = &mPlatformPtr->GetTrackManager();

   auto localTracksCb = [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
   {
      if (mRequiredData & WsfXIO_RequestTracksPkt::cLOCAL_TRACKS)
      {
         mLocalUpdateSet.insert(aLocalTrackPtr->GetTrackId());
      }
   };
   mCallbacks.Add(mTrackManagerPtr->LocalTrackUpdated.Connect(localTracksCb));
   mCallbacks.Add(mTrackManagerPtr->LocalTrackInitiated.Connect(localTracksCb));
   mCallbacks.Add(mTrackManagerPtr->LocalTrackDropped.Connect(
      [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
      {
         if (mRequiredData & WsfXIO_RequestTracksPkt::cLOCAL_DROPPED_TRACKS)
         {
            mLocalDropSet.insert(aLocalTrackPtr->GetTrackId());
            mLocalUpdateSet.erase(aLocalTrackPtr->GetTrackId());
         }
      }));
   mCallbacks.Add(mTrackManagerPtr->RawTrackReceived.Connect(
      [this](double aSimTime, const WsfTrack* aRawTrackPtr)
      {
         if (mRequiredData & WsfXIO_RequestTracksPkt::cRAW_TRACKS)
         {
            mRawUpdateSet.insert(aRawTrackPtr->GetTrackId());
         }
      }));
   mCallbacks.Add(mTrackManagerPtr->RawTrackDropped.Connect(
      [this](double aSimTime, const WsfTrack* aRawTrackPtr)
      {
         if (mRequiredData & WsfXIO_RequestTracksPkt::cRAW_DROPPED_TRACKS)
         {
            mRawDropSet.insert(aRawTrackPtr->GetTrackId());
            mRawUpdateSet.erase(aRawTrackPtr->GetTrackId());
         }
      }));
}

void WsfXIO_TrackService::UpdateSendEvent::Disable()
{
   mEnabled = false;
   mCallbacks.Clear();
   mTrackManagerPtr = nullptr;
}
