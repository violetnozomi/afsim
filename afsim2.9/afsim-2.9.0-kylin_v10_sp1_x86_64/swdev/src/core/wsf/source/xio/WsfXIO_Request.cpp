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

#include "xio/WsfXIO_Request.hpp"

#include <cassert>

#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Service.hpp"

unsigned int WsfXIO_RequestManager::mNextRequestId = 1;

WsfXIO_RequestManager::WsfXIO_RequestManager(WsfXIO_Interface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
{
   mCallbacks.Add(aInterfacePtr->OnDisconnected.Connect(&WsfXIO_RequestManager::HandleDisconnect, this));
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_RequestManager::HandleCancel, this));
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_RequestManager::HandleRequestResponse, this));
}

WsfXIO_RequestManager::~WsfXIO_RequestManager() {}

void WsfXIO_RequestManager::HandleRequestResponse(WsfXIO_ResponsePkt& aPkt)
{
   if (aPkt.mFromProvider)
   {
      RequestMap::iterator i = mRequests.find(aPkt.mRequestId);
      if (i != mRequests.end())
      {
         i->second->HandleResponse(aPkt);
         if (aPkt.mCompletesRequest)
         {
            i->second->SetRemotelyDestroyed();
            RequestCancelled(i->second);
         }
      }
   }
   else
   {
      WsfXIO_Connection*    connectionPtr = (WsfXIO_Connection*)aPkt.GetSender();
      WsfXIO_SubscriptionId id(connectionPtr->GetConnectionId(), aPkt.mRequestId);
      SessionMap::iterator  i = mSessions.find(id);
      if (i != mSessions.end())
      {
         i->second->HandleResponse(aPkt);
      }
   }
}

//! WsfXIO_RequestManager takes control of the request.  The request is initialized and sent
//! to the remote application.  WsfXIO_RequestManager will delete the request upon cancellation.
void WsfXIO_RequestManager::AddRequest(WsfXIO_Request* aRequestPtr)
{
   mRequests[mNextRequestId] = aRequestPtr;
   aRequestPtr->SetRequestId(mNextRequestId++);
   aRequestPtr->SetRequestManager(this);
   aRequestPtr->Initialized();
}

void WsfXIO_RequestManager::RemoveRequest(WsfXIO_Request* aRequestPtr)
{
   RequestCancelled(aRequestPtr);
}

bool WsfXIO_RequestManager::RemoveRequest(int aRequestId)
{
   bool                 removed = false;
   RequestMap::iterator i       = mRequests.find(aRequestId);
   if (i != mRequests.end())
   {
      RemoveRequest(i->second);
      removed = true;
   }
   return removed;
}

//! Adds a session to the request manager for the primary goal of routing WsfXIO_ResponsePkt to the session
void WsfXIO_RequestManager::AddSession(WsfXIO_ServiceSession* aSessionPtr)
{
   mSessions[aSessionPtr->GetSubscription().GetId()] = aSessionPtr;
}

//! Removes the session from the request manager
void WsfXIO_RequestManager::RemoveSession(WsfXIO_ServiceSession* aSessionPtr)
{
   mSessions.erase(aSessionPtr->GetSubscription().GetId());
}

//! Handles subscription cancellation notifications
void WsfXIO_RequestManager::HandleCancel(WsfXIO_SubscriptionCancelPkt& aPkt)
{
   if (!aPkt.mIsRequest)
   {
      RequestMap::iterator i = mRequests.find(aPkt.mSubscriptionId);
      if (i != mRequests.end())
      {
         WsfXIO_Request* requestPtr = i->second;
         mRequests.erase(i);
         requestPtr->SetRemotelyDestroyed();
         RequestCancelled(requestPtr);
      }
   }
}

//! Removes any requests on a lost connection
void WsfXIO_RequestManager::HandleDisconnect(WsfXIO_Connection* aConnectionPtr)
{
   for (RequestMap::iterator i = mRequests.begin(); i != mRequests.end();)
   {
      if (i->second->GetConnection() == aConnectionPtr)
      {
         WsfXIO_Request* requestPtr = i->second;
         mRequests.erase(i++);
         requestPtr->SetRemotelyDestroyed();
         RequestCancelled(requestPtr);
      }
      else
      {
         ++i;
      }
   }
}

//! Returns the request with the given request ID if it exists.
WsfXIO_Request* WsfXIO_RequestManager::FindRequest(int aRequestId)
{
   WsfXIO_Request*      requestPtr = nullptr;
   RequestMap::iterator i          = mRequests.find(aRequestId);
   if (i != mRequests.end())
   {
      requestPtr = i->second;
   }
   return requestPtr;
}

void WsfXIO_RequestManager::RequestCancelled(WsfXIO_Request* aRequestPtr)
{
   if (!aRequestPtr->IsRemotelyDestroyed())
   {
      // inform the provider application the subscription is no longer needed
      WsfXIO_SubscriptionCancelPkt pkt;
      pkt.mSubscriptionId = aRequestPtr->GetRequestId();
      pkt.mIsRequest      = true;
      aRequestPtr->GetConnection()->Send(pkt);
   }
   mRequests.erase(aRequestPtr->GetRequestId());
   delete aRequestPtr;
}

WsfXIO_Request::WsfXIO_Request(WsfXIO_Connection* aConnectionPtr, bool aIsReliable)
   : mRequestManagerPtr(nullptr)
   , mConnectionPtr(aConnectionPtr)
   , mIsReliable(aIsReliable)
   , mRemotelyDestroyed(false)
   , mRequestId(0)
{
}

// virtual
WsfXIO_Request::~WsfXIO_Request() {}

//! Sends the request to the remote application
void WsfXIO_Request::SendRequest(WsfXIO_RequestDataPkt& aPkt)
{
   aPkt.mSubscriptionRequestId = mRequestId;
   aPkt.mReliable              = mIsReliable;
   mConnectionPtr->Send(aPkt);
}

//! Sends a 'response' message to the service.
//! Can only be used after the request has been initialized
void WsfXIO_Request::SendResponse(WsfXIO_ResponsePkt& aPkt)
{
   aPkt.mFromProvider = false;
   aPkt.mRequestId    = mRequestId;
   mConnectionPtr->Send(aPkt);
}
