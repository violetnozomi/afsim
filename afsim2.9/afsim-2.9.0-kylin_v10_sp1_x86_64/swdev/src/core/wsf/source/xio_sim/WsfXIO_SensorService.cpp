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

#include "xio_sim/WsfXIO_SensorService.hpp"

#include "WsfDefaultSensorScheduler.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_SensorService::WsfXIO_SensorService(WsfXIO_Simulation& aXIO_Simulation)
   : BaseClassType(aXIO_Simulation.GetInterface())
   , mSimulation(aXIO_Simulation.GetSimulation())
   , mRequestCount(0)
{
   mCallbacks += GetInterface()->Connect(&WsfXIO_SensorService::HandleRequest, this);
   WsfXIO_SimService::Initialize(aXIO_Simulation.GetSimulation());
}

void WsfXIO_SensorService::RequireCallbacks(bool aRequired)
{
   if (mRequestCallbacks.Empty() != !aRequired)
   {
      if (aRequired)
      {
         mRequestCallbacks +=
            WsfObserver::SensorRequestCanceled(&mSimulation).Connect(&WsfXIO_SensorService::RequestCanceled, this);
         mRequestCallbacks +=
            WsfObserver::SensorRequestInitiated(&mSimulation).Connect(&WsfXIO_SensorService::RequestUpdated, this);
         mRequestCallbacks +=
            WsfObserver::SensorRequestUpdated(&mSimulation).Connect(&WsfXIO_SensorService::RequestUpdated, this);
         mRequestCallbacks +=
            WsfObserver::SensorTurnedOff(&mSimulation).Connect(&WsfXIO_SensorService::SensorTurnedOff, this);
      }
      else
      {
         mRequestCallbacks.Clear();
      }
   }
}

void WsfXIO_SensorService::RequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorToRequestMap::iterator i = mSensorToRequests.lower_bound(aSensorPtr);
   if (i == mSensorToRequests.end())
   {
      return;
   }
   for (; i != mSensorToRequests.end() && i->first == aSensorPtr; ++i)
   {
      WsfXIO_Subscription*               subscriptionPtr = i->second;
      const WsfXIO_RequestSensorDataPkt* requestPtr      = FindRequest(subscriptionPtr->GetId());
      if (requestPtr && (requestPtr->mRequiredData & WsfXIO_RequestSensorDataPkt::cTRACKING_REQUESTS))
      {
         WsfXIO_SensorTrackingRequestUpdatePkt pkt;
         pkt.mPlatformIndex = requestPtr->mPlatformIndex;
         pkt.mSensorNameId  = requestPtr->mSensorNameId;
         pkt.mClear         = false;
         pkt.mRemovedRequests.push_back(aTrackPtr->GetTrackId());
         subscriptionPtr->SendResponse(pkt);
      }
   }
}

void WsfXIO_SensorService::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
   SensorToRequestMap::iterator i = mSensorToRequests.lower_bound(aSensorPtr);
   if (i == mSensorToRequests.end())
   {
      return;
   }
   for (; i != mSensorToRequests.end() && i->first == aSensorPtr; ++i)
   {
      WsfXIO_Subscription*               subscriptionPtr = i->second;
      const WsfXIO_RequestSensorDataPkt* requestPtr      = FindRequest(subscriptionPtr->GetId());
      if (requestPtr && (requestPtr->mRequiredData & WsfXIO_RequestSensorDataPkt::cTRACKING_REQUESTS))
      {
         WsfXIO_SensorTrackingRequestUpdatePkt pkt;
         pkt.mPlatformIndex = requestPtr->mPlatformIndex;
         pkt.mSensorNameId  = requestPtr->mSensorNameId;
         pkt.mClear         = true;
         subscriptionPtr->SendResponse(pkt);
      }
   }
}

void WsfXIO_SensorService::RequestUpdated(double          aSimTime,
                                          WsfSensor*      aSensorPtr,
                                          WsfSensorMode*  aModePtr,
                                          const WsfTrack* aTrackPtr)
{
   SensorToRequestMap::iterator i = mSensorToRequests.lower_bound(aSensorPtr);
   if (i == mSensorToRequests.end())
   {
      return;
   }
   for (; i != mSensorToRequests.end() && i->first == aSensorPtr; ++i)
   {
      WsfXIO_Subscription*               subscriptionPtr = i->second;
      const WsfXIO_RequestSensorDataPkt* requestPtr      = FindRequest(subscriptionPtr->GetId());
      if (requestPtr && (requestPtr->mRequiredData & WsfXIO_RequestSensorDataPkt::cTRACKING_REQUESTS))
      {
         WsfXIO_SensorTrackingRequestUpdatePkt pkt;
         pkt.mPlatformIndex = requestPtr->mPlatformIndex;
         pkt.mSensorNameId  = requestPtr->mSensorNameId;
         pkt.mClear         = false;
         WsfXIO_SensorTrackingRequestUpdatePkt::TrackingRequest rData;
         rData.mModeIndex = static_cast<int>(aModePtr->GetModeIndex());
         rData.mTrackId   = aTrackPtr->GetTrackId();
         pkt.mAddedRequests.push_back(rData);
         subscriptionPtr->SendResponse(pkt);
      }
   }
}

void WsfXIO_SensorService::HandleRequest(WsfXIO_RequestSensorDataPkt& aPkt)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
   WsfSensor*   sensorPtr   = nullptr;
   if (platformPtr != nullptr)
   {
      sensorPtr = platformPtr->GetComponent<WsfSensor>(aPkt.mSensorNameId);
   }
   if (sensorPtr == nullptr)
   {
      DenyRequest(aPkt);
   }
   WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
   AddRequiredPlatform(*platformPtr, subscriptionPtr);
   mSensorToRequests.insert(std::make_pair(sensorPtr, subscriptionPtr));

   if (aPkt.mRequiredData & WsfXIO_RequestSensorDataPkt::cTRACKING_REQUESTS)
   {
      RequireCallbacks(true);
      ++mRequestCount;

      WsfXIO_SensorTrackingRequestUpdatePkt pkt;
      pkt.mPlatformIndex                             = static_cast<int32_t>(platformPtr->GetIndex());
      pkt.mSensorNameId                              = sensorPtr->GetNameId();
      pkt.mClear                                     = false;
      WsfSensorScheduler*        schedulerPtr        = sensorPtr->GetScheduler();
      WsfDefaultSensorScheduler* defaultSchedulerPtr = dynamic_cast<WsfDefaultSensorScheduler*>(schedulerPtr);
      if (defaultSchedulerPtr != nullptr)
      {
         const WsfDefaultSensorScheduler::RequestList& requests = defaultSchedulerPtr->GetRequestList();
         for (size_t i = 0; i < requests.size(); ++i)
         {
            WsfXIO_SensorTrackingRequestUpdatePkt::TrackingRequest rData;
            rData.mModeIndex = static_cast<int>(requests[i].mModeIndex);
            rData.mTrackId   = requests[i].mRequestId;
            pkt.mAddedRequests.push_back(rData);
         }
      }
      subscriptionPtr->SendResponse(pkt);
   }
}

void WsfXIO_SensorService::SubscriptionCancelledT(WsfXIO_Subscription*         aSubscriptionPtr,
                                                  WsfXIO_RequestSensorDataPkt& aRequestPkt)
{
   if (aRequestPkt.mRequiredData & WsfXIO_RequestSensorDataPkt::cTRACKING_REQUESTS)
   {
      if (--mRequestCount == 0)
      {
         RequireCallbacks(false);
      }
   }
   for (SensorToRequestMap::iterator i = mSensorToRequests.begin(); i != mSensorToRequests.end(); ++i)
   {
      if (i->second == aSubscriptionPtr)
      {
         mSensorToRequests.erase(i);
         break;
      }
   }
}
