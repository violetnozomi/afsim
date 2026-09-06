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

#include "xio_sim/WsfXIO_MessageService.hpp"

#include "WsfAssociationMessage.hpp"
#include "WsfComm.hpp"
#include "WsfCommObserver.hpp"
#include "WsfControlMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTaskCancelMessage.hpp"
#include "WsfTaskControlMessage.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackMessage.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Destination.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

//! Create request for messages
//! @param aConnectionPtr Pointer to an XIO connection
//! @param aPlatformIndex The index of the platform sending or receiving the messages
//! @param aReceivedMessages 'true' if received messages are requested.
//! @param aTransmittedMessages 'true' if transmitted messages are requested.
//! @param aIsReliable 'true' a reliable protocol is to be used.
//! @note This request must be added to a WsfXIO_RequestManager
WsfXIO_MessageRequest::WsfXIO_MessageRequest(WsfXIO_Connection* aConnectionPtr,
                                             int                aPlatformIndex,
                                             bool               aReceivedMessages,
                                             bool               aTransmittedMessages,
                                             bool               aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
   , mPlatformIndex(aPlatformIndex)
   , mGetReceived(aReceivedMessages)
   , mGetTransmitted(aTransmittedMessages)
{
}

void WsfXIO_MessageRequest::HandleMessagePrivate(WsfXIO_MessagePkt& aPkt)
{
   // Filter out unrelated messages.
   if ((aPkt.mPlatformIndex == mPlatformIndex) && ((mGetReceived && aPkt.mMessageType == WsfXIO_MessagePkt::cRECEIVED) ||
                                                   (mGetTransmitted && aPkt.mMessageType == WsfXIO_MessagePkt::cSENT)))
   {
      HandleMessage(aPkt);
   }
}

void WsfXIO_MessageRequest::Initialized()
{
   WsfXIO_MessageRequestPkt pkt;
   pkt.mPlatformIndex     = mPlatformIndex;
   pkt.mRequestedMessages = mGetReceived ? WsfXIO_MessageRequestPkt::cRECEIVED_MESSAGES : 0u;
   if (mGetTransmitted)
   {
      pkt.mRequestedMessages |= WsfXIO_MessageRequestPkt::cSENT_MESSAGES;
   }
   SendRequest(pkt);
   mCallbacks += GetConnection()->GetInterface().Connect(&WsfXIO_MessageRequest::HandleMessage, this);
}

WsfXIO_MessageService::WsfXIO_MessageService(WsfXIO_Simulation& aXIO_Simulation)
   : WsfXIO_Service(aXIO_Simulation.GetInterface())
   , mSimulation(aXIO_Simulation.GetSimulation())
{
   mCallbacks += GetInterface()->Connect(&WsfXIO_MessageService::HandleRequest, this);
}

void WsfXIO_MessageService::HandleRequest(WsfXIO_MessageRequestPkt& aPkt)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
      if (aPkt.mRequestedMessages & WsfXIO_MessageRequestPkt::cRECEIVED_MESSAGES)
      {
         mReceiveSubscribers[aPkt.mPlatformIndex].push_back(subscriptionPtr);
      }
      if (aPkt.mRequestedMessages & WsfXIO_MessageRequestPkt::cSENT_MESSAGES)
      {
         mTransmitSubscribers[aPkt.mPlatformIndex].push_back(subscriptionPtr);
      }
      UpdateCallbacks();
   }
}

void WsfXIO_MessageService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   SubscriptionsMap* maps[] = {&mReceiveSubscribers, &mTransmitSubscribers};

   for (int m = 0; m < 2; ++m)
   {
      SubscriptionsMap& subMap = *maps[m];
      for (SubscriptionsMap::iterator i = subMap.begin(); i != subMap.end();)
      {
         SubscriptionsMap::iterator nextI = i;
         ++nextI;
         SubscriptionList&          subscriptionList = i->second;
         SubscriptionList::iterator iter = std::find(subscriptionList.begin(), subscriptionList.end(), aSubscriptionPtr);
         if (iter != subscriptionList.end())
         {
            subscriptionList.erase(iter);
            if (subscriptionList.empty())
            {
               subMap.erase(i);
            }
         }
         i = nextI;
      }
   }
   UpdateCallbacks();
}

void WsfXIO_MessageService::UpdateCallbacks()
{
   if (mReceiveSubscribers.empty() && mTransmitSubscribers.empty())
   {
      mObserverCallbacks.Clear();
   }
   else if (mObserverCallbacks.Empty())
   {
      mObserverCallbacks +=
         WsfObserver::MessageReceived(&mSimulation).Connect(&WsfXIO_MessageService::HandleReceivedMessage, this);
      mObserverCallbacks +=
         WsfObserver::MessageTransmitted(&mSimulation).Connect(&WsfXIO_MessageService::HandleTransmittedMessage, this);
      mObserverCallbacks +=
         WsfObserver::MessageTransmitEnded(&mSimulation).Connect(&WsfXIO_MessageService::HandleTransmitEndedMessage, this);
      mObserverCallbacks += WsfObserver::MessageTransmittedHeartbeat(&mSimulation)
                               .Connect(&WsfXIO_MessageService::HandleTransmittedHeartbeatMessage, this);
   }
}

void WsfXIO_MessageService::HandleReceivedMessage(double             aSimTime,
                                                  wsf::comm::Comm*   aXmtrCommPtr,
                                                  wsf::comm::Comm*   aReceiverCommPtr,
                                                  const WsfMessage&  aMessage,
                                                  wsf::comm::Result& aResult)
{
   HandleMessage(aReceiverCommPtr->GetPlatform(), aMessage, true);
}

void WsfXIO_MessageService::HandleTransmittedMessage(double aSimTime, wsf::comm::Comm* aXmtrCommPtr, const WsfMessage& aMessage)
{
   HandleMessage(aXmtrCommPtr->GetPlatform(), aMessage, false);
}

void WsfXIO_MessageService::HandleTransmitEndedMessage(double aSimTime, wsf::comm::Comm* aXmtrCommPtr, const WsfMessage& aMessage)
{
   HandleMessage(aXmtrCommPtr->GetPlatform(), aMessage, false);
}
void WsfXIO_MessageService::HandleTransmittedHeartbeatMessage(double            aSimTime,
                                                              wsf::comm::Comm*  aXmtrCommPtr,
                                                              const WsfMessage& aMessage)
{
   HandleMessage(aXmtrCommPtr->GetPlatform(), aMessage, false);
}

void WsfXIO_MessageService::HandleMessage(WsfPlatform* aPlatformPtr, const WsfMessage& aMessage, bool aIsReceived)
{
   // Currently, XIO supports only a subset of WSF messages.  Don't try to send unsupported messages.
   if (WsfAssociationMessage::GetTypeId() != aMessage.GetType() &&
       WsfControlMessage::GetTypeId() != aMessage.GetType() && WsfStatusMessage::GetTypeId() != aMessage.GetType() &&
       WsfTrackMessage::GetTypeId() != aMessage.GetType() && WsfTrackDropMessage::GetTypeId() != aMessage.GetType() &&
       WsfTaskAssignMessage::GetTypeId() != aMessage.GetType() && WsfTaskCancelMessage::GetTypeId() != aMessage.GetType() &&
       WsfTaskControlMessage::GetTypeId() != aMessage.GetType() && WsfTaskStatusMessage::GetTypeId() != aMessage.GetType())
   {
      return;
   }

   size_t                     platformIndex = aPlatformPtr->GetIndex();
   SubscriptionsMap&          subscriptions = aIsReceived ? mReceiveSubscribers : mTransmitSubscribers;
   WsfXIO_Destination         destination;
   SubscriptionsMap::iterator i = subscriptions.find(platformIndex);
   if (i != subscriptions.end())
   {
      for (size_t j = 0; j < i->second.size(); ++j)
      {
         WsfXIO_Subscription* subscriptionPtr = i->second[j];
         destination.Add(&subscriptionPtr->GetConnection());
      }
   }

   if (!destination.IsEmpty())
   {
      WsfXIO_MessagePkt pkt;
      pkt.mMessagePtr    = &aMessage;
      pkt.mPlatformIndex = static_cast<int>(platformIndex);
      pkt.mMessageType   = aIsReceived ? WsfXIO_MessagePkt::cRECEIVED : WsfXIO_MessagePkt::cSENT;
      destination.Send(pkt);
   }
}
