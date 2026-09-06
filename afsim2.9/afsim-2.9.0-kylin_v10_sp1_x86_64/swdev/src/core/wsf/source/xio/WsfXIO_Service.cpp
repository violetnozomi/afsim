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

#include "xio/WsfXIO_Service.hpp"

#include <iostream>

#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Request.hpp"

WsfXIO_Service::WsfXIO_Service(WsfXIO_Interface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
{
   mCallbacks.Add(mInterfacePtr->OnDisconnected.Connect(&WsfXIO_Service::HandleDisconnect, this));
   mCallbacks.Add(mInterfacePtr->Connect(&WsfXIO_Service::HandleCancellation, this));
}

WsfXIO_Service::~WsfXIO_Service()
{
   for (auto& mSubscription : mSubscriptions)
   {
      delete mSubscription.second;
   }
}


void WsfXIO_Service::AddSubscription(WsfXIO_Subscription* aSubscriptionPtr)
{
   mSubscriptions[aSubscriptionPtr->GetId()] = aSubscriptionPtr;
   aSubscriptionPtr->Initialize(this);
}

bool WsfXIO_Service::RemoveSubscription(WsfXIO_Subscription* aSubscriptionPtr)
{
   bool foundSubscription = false;
   auto i                 = mSubscriptions.find(aSubscriptionPtr->GetId());
   if (i != mSubscriptions.end())
   {
      foundSubscription = true;
      RemovingSubscription(aSubscriptionPtr);
      mSubscriptions.erase(i);
   }

   if (foundSubscription)
   {
      if (!aSubscriptionPtr->HasUserCancelled())
      {
         WsfXIO_SubscriptionCancelPkt pkt;
         pkt.mIsRequest      = false;
         pkt.mSubscriptionId = aSubscriptionPtr->GetRequestId();
         aSubscriptionPtr->GetConnection().Send(pkt);
      }
      delete aSubscriptionPtr;
   }

   return foundSubscription;
}


//! Returns a pointer to a subscription
//! @param aConnectionPtr A connection to the application that requested the subscription
//! @param aSubscriptionId The ID of the subscription.
WsfXIO_Subscription* WsfXIO_Service::FindSubscription(WsfXIO_Connection* aConnectionPtr, int aSubscriptionId)
{
   WsfXIO_Subscription* subscriptionPtr = nullptr;

   WsfXIO_SubscriptionId id(aConnectionPtr->GetConnectionId(), aSubscriptionId);
   auto                  i = mSubscriptions.find(id);
   if (i != mSubscriptions.end())
   {
      subscriptionPtr = i->second;
   }
   return subscriptionPtr;
}

void WsfXIO_Service::DenyRequest(WsfXIO_RequestDataPkt& aRequest)
{
   WsfXIO_Connection*           connectionPtr = (WsfXIO_Connection*)aRequest.GetSender();
   WsfXIO_SubscriptionCancelPkt pkt;
   pkt.mSubscriptionId = aRequest.mSubscriptionRequestId;
   pkt.mIsRequest      = false;
   connectionPtr->Send(pkt);
}

void WsfXIO_Service::HandleDisconnect(WsfXIO_Connection* aConnection)
{
   int connectionId = aConnection->GetConnectionId();
   for (auto i = mSubscriptions.begin(); i != mSubscriptions.end();)
   {
      if (i->first.first == connectionId)
      {
         WsfXIO_Subscription* subscriptionPtr = i->second;
         subscriptionPtr->SetUserCancelled(true);
         ++i;
         SubscriptionCancelled(subscriptionPtr);
      }
      else
      {
         ++i;
      }
   }
}

void WsfXIO_Service::HandleCancellation(WsfXIO_SubscriptionCancelPkt& aPkt)
{
   if (aPkt.mIsRequest)
   {
      WsfXIO_Subscription* subscriptionPtr = FindSubscription((WsfXIO_Connection*)aPkt.GetSender(), aPkt.mSubscriptionId);
      if (subscriptionPtr != nullptr)
      {
         subscriptionPtr->SetUserCancelled(true);
         SubscriptionCancelled(subscriptionPtr);
      }
   }
}

WsfXIO_Subscription* WsfXIO_Service::CreateSubscription(WsfXIO_RequestDataPkt& aRequest)
{
   WsfXIO_Subscription* subscriptionPtr = new WsfXIO_Subscription(aRequest);
   AddSubscription(subscriptionPtr);
   return subscriptionPtr;
}

void WsfXIO_Service::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   RemoveSubscription(aSubscriptionPtr);
}

WsfXIO_Subscription* WsfXIO_Service::FindSubscription(const WsfXIO_SubscriptionId& aSubscriptionId)
{
   WsfXIO_Subscription* subscriptionPtr = nullptr;
   auto                 i               = mSubscriptions.find(aSubscriptionId);
   if (i != mSubscriptions.end())
   {
      subscriptionPtr = i->second;
   }
   return subscriptionPtr;
}

WsfXIO_ServiceSession::WsfXIO_ServiceSession(WsfXIO_Subscription* aSubscriptionPtr)
   : mSubscriptionPtr(aSubscriptionPtr)
{
   mSubscriptionPtr->GetInterface().GetRequestManager().AddSession(this);
}

WsfXIO_ServiceSession::~WsfXIO_ServiceSession()
{
   mSubscriptionPtr->GetInterface().GetRequestManager().RemoveSession(this);
}
