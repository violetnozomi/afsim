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

#include "xio/WsfXIO_PublisherHelper.hpp"

#include "PakSerialize.hpp"
#include "PakSerializeImpl.hpp"
#include "WsfEvent.hpp"
#include "WsfSimulation.hpp"
#include "xio/WsfXIO.hpp"
#include "xio/WsfXIO_Interface.hpp"

class PublishEvent : public WsfEvent
{
public:
   PublishEvent(double aSimTime, WsfXIO_PublisherHelper* aHelperPtr)
      : WsfEvent(aSimTime)
      , mHelperPtr(aHelperPtr)
   {
   }

   EventDisposition Execute() override
   {
      if (mHelperPtr != nullptr)
      {
         mHelperPtr->mStateDirty = false;
         mHelperPtr->PublishState(GetTime());
      }
      return WsfEvent::cDELETE;
   };

private:
   WsfXIO_PublisherHelper* mHelperPtr;
};

WsfXIO_PublisherHelper::WsfXIO_PublisherHelper(WsfXIO_Interface* aInterfacePtr, WsfSimulation& aSimulation)
   : mInterfacePtr(aInterfacePtr)
   , mSimulation(aSimulation)
   , mSubscriptions()
   , mStateDirty(false)
{
}

WsfXIO_PublisherHelper::~WsfXIO_PublisherHelper()
{
   Subscriptions tempSubscriptions = mSubscriptions;
   for (Subscriptions::iterator iter = tempSubscriptions.begin(); iter != tempSubscriptions.end(); ++iter)
   {
      Unsubscribe(iter->first);
   }
}

void WsfXIO_PublisherHelper::SetStateDirty(double aSimTime)
{
   if (!mStateDirty)
   {
      double timeNow = mSimulation.GetSimTime();
      double simTime = (aSimTime < 0.0) ? timeNow : aSimTime;
      if (simTime == timeNow)
      {
         PublishState(simTime);
      }
      else
      {
         mStateDirty = true;
         mSimulation.AddEvent(ut::make_unique<PublishEvent>(simTime, this));
      }
   }
}

//! Publish a message with the specified key.
//! @param aKey The key to associate the message with.
//! @param aData The contents of the message.
void WsfXIO_PublisherHelper::Publish(const PublisherKey& aKey, const std::string& aData)
{
   WsfXIO_Publisher& publisher = mInterfacePtr->GetPublisher();
   publisher.Publish(aKey, aData);
}

//! Retrieves the published data given a key.
//! @param aKey The key to retrieve the data with.
//! @param aData The data that is returned by reference.
//! @return Returns 'true' if data was retrieved.
bool WsfXIO_PublisherHelper::GetPublishedData(const PublisherKey& aKey, std::string& aData) const
{
   WsfXIO_Publisher& publisher = mInterfacePtr->GetPublisher();
   return publisher.GetData(aKey, aData);
}

//! Check to see if a subscription already exists.
//! @param aSubscriptionId The subscription id to check.
//! @return Returns 'true' if the subscription exists.
bool WsfXIO_PublisherHelper::HasSubscription(const SubscriptionId& aSubscriptionId) const
{
   return (mSubscriptions.find(aSubscriptionId) != mSubscriptions.end());
}

//! Create a new subscription with the specified filter.
//! @param aSubscriptionFilter The filter to match against any published messages.
//! @param aSubscriptionCallbackPtr The callback to trigger when a publication that matches the filter is received.
//! @return Returns the subscription id.
//! @note You can use new UtCallbackN<R(Args...)> to simplify the creation of the callback.
//! @note This class manages the memory of the SubscriptionCallback.
WsfXIO_PublisherHelper::SubscriptionId WsfXIO_PublisherHelper::Subscribe(const SubscriptionFilter& aSubscriptionFilter,
                                                                         SubscriptionCallback* aSubscriptionCallbackPtr)
{
   WsfXIO_Publisher& publisher = mInterfacePtr->GetPublisher();
   SubscriptionId    subscriptionId;
   mSubscriptions[subscriptionId] = publisher.Subscribe(subscriptionId, aSubscriptionFilter, aSubscriptionCallbackPtr);
   return subscriptionId;
}

//! Cancel the specified subscription.
//! @param aSubscriptionId The subscription id to cancel.
//! @return Returns 'true' if the subscription was canceled.
bool WsfXIO_PublisherHelper::Unsubscribe(const SubscriptionId& aSubscriptionId)
{
   bool unsubscribed = false;

   WsfXIO_Publisher&       publisher = mInterfacePtr->GetPublisher();
   Subscriptions::iterator iter      = mSubscriptions.find(aSubscriptionId);
   if (iter != mSubscriptions.end())
   {
      unsubscribed = publisher.Unsubscribe(iter->first);
      delete iter->second; // Clean up the callback's memory.
      mSubscriptions.erase(iter);
   }
   return unsubscribed;
}
