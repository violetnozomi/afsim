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

#include "xio_sim/WsfXIO_SimTimeService.hpp"

#include <cmath>
#include <iostream>

#include "WsfClockSource.hpp"
#include "WsfRealTimeClockSource.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_SimTimeService::WsfXIO_SimTimeService(WsfXIO_Simulation& aXIO_Simulation)
   : BaseType(aXIO_Simulation.GetInterface())
   , mSimulation(aXIO_Simulation.GetSimulation())
{
   mCallbacks += GetInterface()->Connect(&WsfXIO_SimTimeService::HandleRequest, this);
   mCallbacks += WsfObserver::SimulationPausing(&mSimulation).Connect(&WsfXIO_SimTimeService::HandlePause, this);
   mCallbacks += WsfObserver::SimulationResuming(&mSimulation).Connect(&WsfXIO_SimTimeService::HandleResume, this);
   mCallbacks +=
      WsfObserver::SimulationClockRateChange(&mSimulation).Connect(&WsfXIO_SimTimeService::HandleClockRateChange, this);
}

WsfXIO_SimTimeService::~WsfXIO_SimTimeService() {}

void WsfXIO_SimTimeService::HandleRequest(WsfXIO_RequestSimTimePkt& aPkt)
{
   WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
   if (aPkt.mTimerType == WsfXIO_RequestSimTimePkt::cSIM_TIME_INTERVAL ||
       aPkt.mTimerType == WsfXIO_RequestSimTimePkt::cADJUSTED_SIM_TIME_INTERVAL ||
       aPkt.mTimerType == WsfXIO_RequestSimTimePkt::cREAL_TIME_INTERVAL)
   {
      bool             adjusted  = aPkt.mTimerType == WsfXIO_RequestSimTimePkt::cADJUSTED_SIM_TIME_INTERVAL;
      bool             wallEvent = aPkt.mTimerType == WsfXIO_RequestSimTimePkt::cREAL_TIME_INTERVAL;
      SubscriptionData subData;
      auto             updateEventPtr =
         ut::make_unique<UpdateEvent>(this, subscriptionPtr->GetId(), aPkt.mTimerInterval, adjusted, wallEvent);
      subData.mEventPtr       = updateEventPtr.get();
      subData.mSubscriptionId = subscriptionPtr->GetId();
      mSubscriptionData.push_back(subData);

      if (aPkt.mTimerType == WsfXIO_RequestSimTimePkt::cREAL_TIME_INTERVAL)
      {
         subData.mEventPtr->SetTime(mSimulation.GetWallTime());
         mSimulation.AddWallEvent(std::move(updateEventPtr));
      }
      else
      {
         subData.mEventPtr->SetTime(mSimulation.GetSimTime());
         mSimulation.AddEvent(std::move(updateEventPtr));
      }
   }
   UpdateRequestInterval(mSimulation.GetSimTime(), subscriptionPtr->GetId());
}

bool WsfXIO_SimTimeService::UpdateRequestInterval(double aSimTime, WsfXIO_SubscriptionId aSubscriptionId)
{
   WsfXIO_Subscription* subscriptionPtr = FindSubscription(aSubscriptionId);
   if (subscriptionPtr != nullptr)
   {
      WsfXIO_SimTimePkt pkt;
      SetTimeData(pkt);
      pkt.mState = mSimulation.IsRealTime() ? WsfXIO_SimTimePkt::cREALTIME : WsfXIO_SimTimePkt::cNON_REALTIME;
      const WsfClockSource* clockPtr = mSimulation.GetClockSource();
      if (clockPtr != nullptr && clockPtr->IsStopped())
      {
         pkt.mState = WsfXIO_SimTimePkt::cPAUSED;
      }
      subscriptionPtr->SendResponse(pkt);
   }
   return subscriptionPtr != nullptr;
}

void WsfXIO_SimTimeService::SetTimeData(WsfXIO_SimTimePkt& aPkt)
{
   aPkt.mSimTime          = mSimulation.GetSimTime();
   aPkt.mClockRate        = mSimulation.GetClockSource()->GetClockRate();
   aPkt.mEndTime          = mSimulation.GetEndTime();
   aPkt.mCurrentRunNumber = mSimulation.GetRunNumber();
   aPkt.mFinalRunNumber   = mSimulation.GetFinalRunNumber();
}

void WsfXIO_SimTimeService::SendTimeStateToAll(WsfXIO_SimTimePkt::SimulationState aState)
{
   const SubscriptionMap& subscriptions = GetSubscriptions();
   for (SubscriptionMap::const_iterator i = subscriptions.begin(); i != subscriptions.end(); ++i)
   {
      WsfXIO_Subscription* subscriptionPtr = i->second;
      WsfXIO_SimTimePkt    pkt;
      SetTimeData(pkt);
      pkt.mState = aState;
      subscriptionPtr->SendResponse(pkt);
   }
}

void WsfXIO_SimTimeService::HandlePause()
{
   SendTimeStateToAll(WsfXIO_SimTimePkt::cPAUSED);
}

void WsfXIO_SimTimeService::HandleResume()
{
   SendTimeStateToAll(mSimulation.IsRealTime() ? WsfXIO_SimTimePkt::cREALTIME : WsfXIO_SimTimePkt::cNON_REALTIME);
}

void WsfXIO_SimTimeService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   for (size_t i = 0; i < mSubscriptionData.size(); ++i)
   {
      if (mSubscriptionData[i].mSubscriptionId == aSubscriptionPtr->GetId())
      {
         mSubscriptionData[i].mEventPtr->Disable();
         mSubscriptionData.erase(mSubscriptionData.begin() + i);
      }
   }
   WsfXIO_Service::SubscriptionCancelled(aSubscriptionPtr);
}

void WsfXIO_SimTimeService::HandleClockRateChange(double aClockRate)
{
   WsfXIO_SimTimePkt::SimulationState state = WsfXIO_SimTimePkt::cPAUSED;
   if (!mSimulation.GetClockSource()->IsStopped())
   {
      state = mSimulation.IsRealTime() ? WsfXIO_SimTimePkt::cREALTIME : WsfXIO_SimTimePkt::cNON_REALTIME;
   }
   SendTimeStateToAll(state);
   for (auto& subData : mSubscriptionData)
   {
      UpdateEvent*& eventPtr = subData.mEventPtr;
      if (eventPtr->mIsAdjusted)
      {
         double simTime         = mSimulation.GetSimTime();
         double intervalTime    = eventPtr->mInterval * eventPtr->mRate;
         double newInterval     = eventPtr->mInterval * aClockRate;
         double tStart          = eventPtr->GetTime() - intervalTime;
         double newEventTime    = (1.0 - ((simTime - tStart) / intervalTime)) * newInterval + simTime;
         auto   newEventPtr     = ut::make_unique<UpdateEvent>(*eventPtr);
         newEventPtr->mInterval = newInterval;
         newEventPtr->mRate     = aClockRate;
         newEventPtr->SetTime(newEventTime);
         mSimulation.AddEvent(std::move(newEventPtr));
         eventPtr->Disable();
      }
   }
}

WsfXIO_SimTimeService::UpdateEvent::UpdateEvent(WsfXIO_SimTimeService* aServicePtr,
                                                WsfXIO_SubscriptionId  aSubscriptionId,
                                                double                 aInterval,
                                                bool                   aIsAdjusted,
                                                bool                   aIsWallClockEvent)
   : mIsActive(true)
   , mIsAdjusted(aIsAdjusted)
   , mIsWallClockEvent(aIsWallClockEvent)
   , mInterval(aInterval)
   , mServicePtr(aServicePtr)
   , mSubscriptionId(aSubscriptionId)
{
   if (mIsAdjusted)
   {
      mRate = GetSimulation()->GetClockSource()->GetClockRate();
   }
   else
   {
      mRate = 1.0;
   }
}

WsfXIO_SimTimeService::UpdateEvent::UpdateEvent(const UpdateEvent& aSrc)
   : mIsActive(aSrc.mIsActive)
   , mIsAdjusted(aSrc.mIsAdjusted)
   , mIsWallClockEvent(aSrc.mIsWallClockEvent)
   , mInterval(aSrc.mInterval)
   , mServicePtr(aSrc.mServicePtr)
   , mSubscriptionId(aSrc.mSubscriptionId)
   , mRate(aSrc.mRate)
{
}

WsfEvent::EventDisposition WsfXIO_SimTimeService::UpdateEvent::Execute()
{
   if (!mIsActive)
   {
      return cDELETE;
   }
   EventDisposition disposition = cDELETE;
   if (mServicePtr->UpdateRequestInterval(GetTime(), mSubscriptionId))
   {
      double newTime;
      if (mIsWallClockEvent)
      {
         newTime = GetSimulation()->GetWallTime() + mInterval;
      }
      else
      {
         newTime = GetTime() + mInterval * mRate;
      }
      SetTime(newTime);
      disposition = cRESCHEDULE;
   }
   return disposition;
}
