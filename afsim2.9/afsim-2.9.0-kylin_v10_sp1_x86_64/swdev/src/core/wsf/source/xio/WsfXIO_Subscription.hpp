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

#ifndef WSFXIO_SUBSCRIPTION_HPP
#define WSFXIO_SUBSCRIPTION_HPP

#include "wsf_export.h"

#include "UtCallbackHolder.hpp"
class WsfXIO_Connection;
class WsfXIO_Interface;
class WsfXIO_RequestDataPkt;
class WsfXIO_ResponsePkt;
class WsfXIO_Service;

//! Uniquely identifies a subscription
//! (Connection ID, Request ID)
typedef std::pair<int, int> WsfXIO_SubscriptionId;

//! A WsfXIO_Subscription wraps information about a remote request for information.
class WSF_EXPORT WsfXIO_Subscription
{
public:
   WsfXIO_Subscription(WsfXIO_Connection* aConnectionPtr);
   WsfXIO_Subscription(WsfXIO_RequestDataPkt& aRequest);

   ~WsfXIO_Subscription();

   void Initialize(WsfXIO_Service* aProviderPtr);

   //! Returns a unique value identifying this subscription
   WsfXIO_SubscriptionId GetId() const { return mId; }

   //! Returns the subscription ID.  This ID is unique for a given application, but
   //! not necessarily unique across all applications.
   int GetRequestId() const { return mId.second; }

   //! Returns the unique ID for the connection controlling the subscription
   //! This value combined with the SubscriptionId creates a unique identifier
   int GetConnectionId() const { return mId.first; }

   //! Returns the service providing this subscription
   WsfXIO_Service* GetService() const { return mProviderPtr; }

   //! Returns true if the user has canceled this connection remotely
   void SetUserCancelled(bool aHasCancelled) { mUserCancelled = aHasCancelled; }

   //! Sets the 'User Canceled' flag value
   bool HasUserCancelled() const { return mUserCancelled; }

   //! Returns the connection used to send data to the requesting application
   WsfXIO_Connection* GetDataConnection() const { return mDataConnectionPtr; }

   //! Returns the reliable connection used for control messages for this subscription
   WsfXIO_Connection& GetConnection() const { return *mConnectionPtr; }

   //! Sends a response packet to the requestor of this subscription
   void SendResponse(WsfXIO_ResponsePkt& aPkt);

   WsfXIO_Interface& GetInterface() const;

private:
   WsfXIO_SubscriptionId mId;
   WsfXIO_Service*       mProviderPtr;
   int                   mSubscriptionId;
   WsfXIO_Connection*    mConnectionPtr;
   WsfXIO_Connection*    mDataConnectionPtr;
   bool                  mUserCancelled;
};

#endif
