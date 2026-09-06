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

#ifndef WSFXIO_SERVICE_HPP
#define WSFXIO_SERVICE_HPP

#include "wsf_export.h"

#include <map>
#include <utility>
#include <vector>

#include "UtCallbackHolder.hpp"
class WsfPlatform;
class WsfXIO_Connection;
class WsfXIO_Interface;
class WsfXIO_RequestDataPkt;
class WsfXIO_ResponsePkt;
class WsfXIO_Service;
#include "xio/WsfXIO_Subscription.hpp"
class WsfXIO_SubscriptionCancelPkt;

//! Provides basic functionality for handling WsfXIO_Requests
//! Generally, only a single service of each type is used.  See WsfXIO_ServiceSession
//! to create a new object for each active request.
class WSF_EXPORT WsfXIO_Service
{
public:
   WsfXIO_Service(WsfXIO_Interface* aInterfacePtr);
   virtual ~WsfXIO_Service();

   WsfXIO_Subscription* FindSubscription(WsfXIO_Connection* aConnectionPtr, int aSubscriptionId);

   WsfXIO_Interface* GetInterface() const { return mInterfacePtr; }

protected:
   using SubscriptionMap = std::map<WsfXIO_SubscriptionId, WsfXIO_Subscription*>;

   void DenyRequest(WsfXIO_RequestDataPkt& aRequest);

   void HandleDisconnect(WsfXIO_Connection* aConnection);

   void AddSubscription(WsfXIO_Subscription* aSubscriptionPtr);

   bool RemoveSubscription(WsfXIO_Subscription* aSubscriptionPtr);

   WsfXIO_Subscription* CreateSubscription(WsfXIO_RequestDataPkt& aRequest);

   void HandleCancellation(WsfXIO_SubscriptionCancelPkt& aPkt);

   virtual void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr);

   const SubscriptionMap GetSubscriptions() const { return mSubscriptions; }

   WsfXIO_Subscription* FindSubscription(const WsfXIO_SubscriptionId& aSubscriptionId);

   virtual void RemovingSubscription(WsfXIO_Subscription* aSubscriptionPtr) {}

   UtCallbackHolder  mCallbacks;
   WsfXIO_Interface* mInterfacePtr;

private:
   SubscriptionMap mSubscriptions;
};

//! Provides extended functionality for handling WsfXIO_Requests
//! Ties a subscription to the original request for that subscription.
//! Template parameter 'T' is the request packet used for this subscription provider
//! and must be derived from WsfXIO_SubscriptionRequestPkt.
template<typename T, typename SERVICE_TYPE = WsfXIO_Service>
class WsfXIO_ServiceT : public SERVICE_TYPE
{
public:
   using RequestType = T;

   WsfXIO_ServiceT(WsfXIO_Interface* aInterfacePtr)
      : SERVICE_TYPE(aInterfacePtr)
   {
   }

   //! Create a subscription from a request
   WsfXIO_Subscription* CreateSubscription(RequestType& aRequest)
   {
      WsfXIO_Subscription* subscriptionPtr = SERVICE_TYPE::CreateSubscription(aRequest);
      mRequests[subscriptionPtr->GetId()]  = aRequest;
      return subscriptionPtr;
   }

   //! Called when a subscription has been canceled
   //! May be called due to lost connection, destroyed simulation entity, or cancellation request
   virtual void SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, RequestType& aRequest)
   {
      SERVICE_TYPE::SubscriptionCancelled(aSubscriptionPtr);
   }

protected:
   using RequestMapT = std::map<WsfXIO_SubscriptionId, T>;
   void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr) override
   {
      typename RequestMapT::iterator i = mRequests.find(aSubscriptionPtr->GetId());
      if (i != mRequests.end())
      {
         SubscriptionCancelledT(aSubscriptionPtr, i->second);
         mRequests.erase(i);
      }
   }
   const T* FindRequest(WsfXIO_SubscriptionId aId) const
   {
      typename RequestMapT::const_iterator i = mRequests.find(aId);
      if (i != mRequests.end())
      {
         return &i->second;
      }
      return nullptr;
   }
   const RequestMapT& GetRequests() const { return mRequests; }

private:
   RequestMapT mRequests;
};

//! A session is a service-side object relating to a request.
//! The lifetime of the request should match that of the session.
class WSF_EXPORT WsfXIO_ServiceSession
{
public:
   WsfXIO_ServiceSession(WsfXIO_Subscription* aSubscriptionPtr);
   virtual ~WsfXIO_ServiceSession();
   //! Called when a WsfXIO_ResponsePkt is received from a the request object
   virtual void         HandleResponse(WsfXIO_ResponsePkt& aPkt) {}
   WsfXIO_Subscription& GetSubscription() { return *mSubscriptionPtr; }

private:
   WsfXIO_Subscription* mSubscriptionPtr;
};

//! A type of service where each request is allocated its own 'session' object.
//! SESSION_TYPE A class derived from WsfXIO_ServiceSession.  An instance of this session type
//!                is created along with the subscription. See CreateSubscription().
//! REQUEST_TYPE A request packet class used for this subscription provider
//!                must be derived from WsfXIO_SubscriptionRequestPkt.
template<typename SESSION_TYPE, typename REQUEST_TYPE, typename SERVICE_TYPE>
class WsfXIO_SessionServiceT : protected SERVICE_TYPE
{
public:
   using SessionType = SESSION_TYPE;
   using RequestType = REQUEST_TYPE;

   WsfXIO_SessionServiceT(WsfXIO_Interface* aInterfacePtr)
      : SERVICE_TYPE(aInterfacePtr)
   {
   }

   //! Create a subscription from a request
   WsfXIO_Subscription* CreateSubscription(RequestType& aRequest)
   {
      WsfXIO_Subscription* subscriptionPtr = SERVICE_TYPE::CreateSubscription(aRequest);
      mRequests[subscriptionPtr->GetId()]  = new SessionType(subscriptionPtr, aRequest);
      return subscriptionPtr;
   }

   //! Called when a subscription has been canceled
   //! May be called due to lost connection, destroyed simulation entity, or cancellation request
   virtual void SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, SessionType& aSession)
   {
      delete &aSession;
      SERVICE_TYPE::SubscriptionCancelled(aSubscriptionPtr);
   }

   WsfXIO_Subscription* FindSubscription(WsfXIO_Connection* aConnectionPtr, int aSubscriptionId)
   {
      return SERVICE_TYPE::FindSubscription(aConnectionPtr, aSubscriptionId);
   }

   void AddRequiredPlatform(WsfPlatform& aPlatform, WsfXIO_Subscription* aSubscriptionPtr)
   {
      SERVICE_TYPE::AddRequiredPlatform(aPlatform, aSubscriptionPtr);
   }

protected:
   using RequestMapT = std::map<WsfXIO_SubscriptionId, SessionType*>;

   virtual void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
   {
      typename RequestMapT::iterator i = mRequests.find(aSubscriptionPtr->GetId());
      if (mRequests.end() != i)
      {
         SubscriptionCancelledT(aSubscriptionPtr, *i->second);
         mRequests.erase(i);
      }
   }

   const RequestMapT& GetRequests() const { return mRequests; }

private:
   RequestMapT mRequests;
};

#endif
