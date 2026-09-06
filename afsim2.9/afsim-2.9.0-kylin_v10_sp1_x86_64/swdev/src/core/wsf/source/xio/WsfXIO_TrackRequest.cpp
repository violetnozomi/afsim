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

#include "xio/WsfXIO_TrackRequest.hpp"

#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

WsfXIO_TrackRequestManager::WsfXIO_TrackRequestManager(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_RequestManager(aInterfacePtr)
{
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_TrackRequestManager::HandleRawTrack, this));
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_TrackRequestManager::HandleLocalTrack, this));
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_TrackRequestManager::HandleDropTrack, this));
}

// virtual
void WsfXIO_TrackRequestManager::RequestCancelled(WsfXIO_Request* aRequestPtr)
{
   WsfXIO_TrackRequest* requestPtr = static_cast<WsfXIO_TrackRequest*>(aRequestPtr);
   if (requestPtr != nullptr && requestPtr->IsGlobalRequest())
   {
      TargetId target(aRequestPtr->GetConnection()->GetApplicationId(), requestPtr->GetPlatformIndex());
      mRequests.erase(target);
   }
   WsfXIO_RequestManager::RequestCancelled(aRequestPtr);
}

void WsfXIO_TrackRequestManager::AddRequest(WsfXIO_TrackRequest* aRequestPtr)
{
   if (aRequestPtr->IsGlobalRequest())
   {
      TargetId target(aRequestPtr->GetConnection()->GetApplicationId(), aRequestPtr->GetPlatformIndex());

      if (mRequests.find(target) != mRequests.end())
      {
         delete aRequestPtr;
      }
      else
      {
         WsfXIO_RequestManager::AddRequest(aRequestPtr);
         mRequests[target] = aRequestPtr;
      }
   }
   else
   {
      WsfXIO_RequestManager::AddRequest(aRequestPtr);
   }
}

void WsfXIO_TrackRequestManager::RemoveRequest(WsfXIO_TrackRequest* aRequestPtr)
{
   WsfXIO_RequestManager::RemoveRequest(aRequestPtr);
}

void WsfXIO_TrackRequestManager::RemoveRequest(int aRequestId)
{
   WsfXIO_RequestManager::RemoveRequest(aRequestId);
}

void WsfXIO_TrackRequestManager::RemoveRequest(GenUniqueId& aAppId, int aPlatformIndex)
{
   WsfXIO_TrackRequest* request = FindRequest(aAppId, aPlatformIndex);
   if (request != nullptr)
   {
      RemoveRequest(request);
   }
}

WsfXIO_TrackRequest* WsfXIO_TrackRequestManager::FindRequest(GenUniqueId& aAppId, int aPlatformIndex)
{
   WsfXIO_TrackRequest* requestPtr = nullptr;
   RequestMap::iterator i          = mRequests.find(TargetId(aAppId, aPlatformIndex));
   if (i != mRequests.end())
   {
      requestPtr = i->second;
   }
   return requestPtr;
}

// Global packet handlers:
// If the requestId is zero, the packet was not addressed to a specific requester
// and needs to be sent to any request assigned to the track request manager.
void WsfXIO_TrackRequestManager::HandleRawTrack(WsfXIO_RawTrackPkt& aPkt)
{
   if (aPkt.mRequestId == 0)
   {
      ForwardPacket(aPkt);
   }
}
void WsfXIO_TrackRequestManager::HandleLocalTrack(WsfXIO_LocalTrackPkt& aPkt)
{
   if (aPkt.mRequestId == 0)
   {
      ForwardPacket(aPkt);
   }
}
void WsfXIO_TrackRequestManager::HandleDropTrack(WsfXIO_TrackDropPkt& aPkt)
{
   if (aPkt.mRequestId == 0)
   {
      ForwardPacket(aPkt);
   }
}


WsfXIO_TrackRequest::WsfXIO_TrackRequest(int aRemotePlatformIndex, WsfXIO_Connection* aConnectionPtr, bool aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
   , mRemotePlatformIndex(aRemotePlatformIndex)
   , mRequiredData(WsfXIO_RequestTracksPkt::cALL)
   , mSendInitialTracks(false)
   , mUpdateInterval(0.0)
   , mRealTime(false)
{
}

void WsfXIO_TrackRequest::Initialized()
{
   WsfXIO_RequestTracksPkt pkt;
   pkt.mPlatformIndex     = mRemotePlatformIndex;
   pkt.mSendInitialTracks = mSendInitialTracks;
   pkt.mRequiredData      = mRequiredData;
   pkt.mSensorNameId      = mSensorNameId;
   pkt.mUpdateInterval    = mUpdateInterval;
   pkt.mRealtimeInterval  = mRealTime;
   SendRequest(pkt);
}

void WsfXIO_TrackRequest::SetUpdateInterval(double aUpdateInterval, bool aRealTime)
{
   mUpdateInterval = aUpdateInterval;
   mRealTime       = aRealTime;
}

void WsfXIO_TrackRequest::HandleResponse(WsfXIO_ResponsePkt& aPkt)
{
   // Depending on the request type, the packet could either be sent as a real response,
   // or broadcasted to multiple receivers with a request-ID of zero.  This handles the first case.
   switch (aPkt.ID())
   {
   case WsfXIO_LocalTrackPkt::cPACKET_ID:
      Handle((WsfXIO_LocalTrackPkt&)aPkt);
      break;
   case WsfXIO_TrackDropPkt::cPACKET_ID:
      Handle((WsfXIO_TrackDropPkt&)aPkt);
      break;
   case WsfXIO_RawTrackPkt::cPACKET_ID:
      Handle((WsfXIO_RawTrackPkt&)aPkt);
      break;
   }
}
