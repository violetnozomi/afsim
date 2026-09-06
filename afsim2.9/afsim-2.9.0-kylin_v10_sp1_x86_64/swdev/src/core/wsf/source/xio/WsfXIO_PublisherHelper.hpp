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

#ifndef WSFXIO_PUBLISHERHELPER_HPP
#define WSFXIO_PUBLISHERHELPER_HPP

#include "wsf_export.h"

class WsfSimulation;
class WsfXIO_Interface;
#include "xio/WsfXIO_Publisher.hpp"

//! This is a helper class that simplifies the interface to the WsfXIO_Publisher class.
//! It uses an std::string as the message type.
//! It can be used as a base class for dynamic plugins that would like to use XIO as a communication mechanism.
//! Simply derive from WsfXIO_PublisherHelper or create an instance of it. You can receive
//! remote messages by subscribing with a filter and callback using 'Subscribe'. You can publish messages by calling
//! 'Publish'. If this class doesn't meet your needs, you can use WsfXIO_Publisher directly.

class WSF_EXPORT WsfXIO_PublisherHelper
{
public:
   friend class PublishEvent;

   typedef WsfXIO_PublishKey                                              PublisherKey;
   typedef WsfXIO_Publisher::SubscriptionId                               SubscriptionId;
   typedef WsfXIO_PublishFilter                                           SubscriptionFilter;
   typedef WsfXIO_Publisher::SubscribeCallback                            SubscriptionCallback;
   typedef std::map<SubscriptionId, WsfXIO_Publisher::SubscribeCallback*> Subscriptions;

   WsfXIO_PublisherHelper(WsfXIO_Interface* aInterfacePtr, WsfSimulation& aSimulation);

   virtual ~WsfXIO_PublisherHelper();

   void SetStateDirty(double aSimTime = -1.0);

   void Publish(const PublisherKey& aKey, const std::string& aData);

   bool GetPublishedData(const PublisherKey& aKey, std::string& aData) const;

   bool HasSubscription(const SubscriptionId& aSubscriptionId) const;

   SubscriptionId Subscribe(const SubscriptionFilter& aSubscriptionFilter, SubscriptionCallback* aSubscriptionCallbackPtr);

   bool Unsubscribe(const SubscriptionId& aSubscriptionId);

protected:
   virtual void PublishState(double aSimTime) {}

private:
   WsfXIO_Interface* mInterfacePtr;
   WsfSimulation&    mSimulation;
   Subscriptions     mSubscriptions;
   bool              mStateDirty;
};

#endif
