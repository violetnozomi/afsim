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

#ifndef WSFXIO_REQUEST_HPP
#define WSFXIO_REQUEST_HPP

#include "wsf_export.h"

#include <map>

#include "UtCallbackHolder.hpp"
class WsfXIO_Connection;
class WsfXIO_Interface;
class WsfXIO_Request;
class WsfXIO_RequestDataPkt;
class WsfXIO_ResponsePkt;
#include "WsfXIO_Subscription.hpp"
class WsfXIO_SubscriptionCancelPkt;
class WsfXIO_ServiceSession;

//! Maintains a group of WsfXIO_Requests.
class WSF_EXPORT WsfXIO_RequestManager
{
public:
   WsfXIO_RequestManager(WsfXIO_Interface* aInterfacePtr);
   virtual ~WsfXIO_RequestManager();

   //! Begins maintaining a WsfXIO_Request.
   //! The WsfXIO_RequestManager gains ownership of the request.
   void AddRequest(WsfXIO_Request* aRequestPtr);

   //! Cancels a request currently owned by this request manager.
   //! The request will be deleted by this class.
   void RemoveRequest(WsfXIO_Request* aRequestPtr);

   //! Cancels a request currently owned by this request manager.
   //! The request will be deleted by this class.  Returns 'true' if the request was found.
   bool RemoveRequest(int aRequestId);

   WsfXIO_Request* FindRequest(int aRequestId);

   void AddSession(WsfXIO_ServiceSession* aSessionPtr);

   void RemoveSession(WsfXIO_ServiceSession* aSessionPtr);

   using RequestMap = std::map<int, WsfXIO_Request*>;
   using SessionMap = std::map<WsfXIO_SubscriptionId, WsfXIO_ServiceSession*>;

   const SessionMap& GetSessions() const { return mSessions; }
   const RequestMap& GetRequests() const { return mRequests; }

protected:
   virtual void RequestCancelled(WsfXIO_Request* aRequestPtr);

   void HandleRequestResponse(WsfXIO_ResponsePkt& aPkt);

   void HandleCancel(WsfXIO_SubscriptionCancelPkt& aPkt);

   void HandleDisconnect(WsfXIO_Connection* aConnectionPtr);

   UtCallbackHolder  mCallbacks;
   WsfXIO_Interface* mInterfacePtr{nullptr};

private:
   SessionMap          mSessions;
   RequestMap          mRequests;
   static unsigned int mNextRequestId;
};

//! WsfXIO_Request maintains the lifetime of a subscription request.
//! WsfXIO_Request is destroyed when the subscription is no longer available
class WSF_EXPORT WsfXIO_Request
{
public:
   friend class WsfXIO_RequestManager;

   WsfXIO_Request(WsfXIO_Connection* aConnectionPtr, bool aIsReliable = true);

   virtual void HandleResponse(WsfXIO_ResponsePkt& aPkt) {}

   void SendRequest(WsfXIO_RequestDataPkt& aPkt);

   //! Returns the connection to the application from which data is requested
   WsfXIO_Connection* GetConnection() const { return mConnectionPtr; }

   //! Returns 'true' if data for this request is to be sent reliably
   bool IsReliable() const { return mIsReliable; }

   //! Returns the request's locally unique ID.
   int GetRequestId() const { return mRequestId; }

   //! Sets the owner of this request
   void SetRequestManager(WsfXIO_RequestManager* aRequestManagerPtr) { mRequestManagerPtr = aRequestManagerPtr; }

   void SendResponse(WsfXIO_ResponsePkt& aPkt);

protected:
   // Destructor should only be called by WsfXIO_RequestManager
   virtual ~WsfXIO_Request();

   //! Called from the WsfXIO_RequestManager indicating that the request message is ready to be sent
   //! SendRequest() should be done from Initialized()
   virtual void Initialized() = 0;

   void SetRequestId(unsigned int aRequestId) { mRequestId = aRequestId; }

   void SetRemotelyDestroyed() { mRemotelyDestroyed = true; }

   bool IsRemotelyDestroyed() const { return mRemotelyDestroyed; }

   void SetIsReliable(bool aIsReliable) { mIsReliable = aIsReliable; }

private:
   WsfXIO_RequestManager* mRequestManagerPtr;
   WsfXIO_Connection*     mConnectionPtr;
   bool                   mIsReliable;
   bool                   mRemotelyDestroyed;
   unsigned int           mRequestId;
};

#endif
