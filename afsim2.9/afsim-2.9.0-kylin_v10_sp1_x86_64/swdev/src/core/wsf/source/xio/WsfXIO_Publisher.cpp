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

#include "xio/WsfXIO_Publisher.hpp"

#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

std::ostream& operator<<(std::ostream& ostr, const WsfXIO_PublishKey& aKey)
{
   std::string key1 = aKey[0].GetString(), key2 = aKey[1].GetString(), key3 = aKey[2].GetString(),
               key4 = aKey[3].GetString();
   ostr << key1 << '-' << key2 << '-' << key3 << '-' << key4;
   return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const WsfXIO_Publisher::SubscriptionId& aId)
{
   ostr << aId.GetData(0) << ":" << aId.GetData(1) << ":" << aId.GetData(2);
   return ostr;
}

WsfXIO_Publisher::WsfXIO_Publisher(WsfXIO_Interface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
   , mDebugLevel(0)
{
   mCallbacks += mInterfacePtr->OnConnected.Connect(&WsfXIO_Publisher::HandleConnect, this);
   mCallbacks += mInterfacePtr->OnDisconnected.Connect(&WsfXIO_Publisher::HandleDisconnect, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Publisher::ProcessSubscription, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Publisher::ProcessUnsubscription, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Publisher::ProcessPublication, this);
}

void WsfXIO_Publisher::ProcessSubscription(WsfXIO_SubscriptionPkt& aPkt)
{
   if (aPkt.mCancel) // Only handle subscription addition.
   {
      return;
   }

   for (size_t i = 0; i < aPkt.mKeyFilters.size(); ++i)
   {
      ReceivedSubscription sub;
      sub.mConnectionPtr  = (WsfXIO_Connection*)aPkt.GetSender();
      sub.mFilter         = aPkt.mKeyFilters[i];
      sub.mSubscriptionId = aPkt.mSubscriptionIds[i];

      mReceivedSubscriptions.push_back(sub);
      ReceivedSubscription* curSubPtr = &mReceivedSubscriptions.back();

      for (std::map<WsfXIO_PublishKey, Publication>::iterator iter = mPublished.begin(); iter != mPublished.end(); ++iter)
      {
         if (sub.mFilter == iter->first)
         {
            iter->second.mRcvSubscriptions.push_back(curSubPtr);
            iter->second.mDestination.Add(curSubPtr->mConnectionPtr);

            if (mDebugLevel >= 1)
            {
               auto out = ut::log::debug() << "xio_interface: Subscription fulfilled.";
               out.AddNote() << "Subscription: " << sub.mFilter;
               out.AddNote() << "Fulfilled by: " << iter->first;
            }

            WsfXIO_PublicationPkt pkt;
            pkt.mSubscriberIds.push_back(
               aPkt.mSubscriptionIds[i]); // Mark that only this subscriber should process the message.
            pkt.mData = iter->second.mData;
            pkt.mKey  = iter->second.mKey;
            iter->second.mDestination.Send(pkt);
         }
      }
   }
}


void WsfXIO_Publisher::ProcessUnsubscription(WsfXIO_SubscriptionPkt& aPkt)
{
   if (!aPkt.mCancel) // Only handle subscription removal.
   {
      return;
   }

   for (size_t i = 0; i < aPkt.mKeyFilters.size(); ++i)
   {
      // Find this subscription.
      const SubscriptionId                      subId = aPkt.mSubscriptionIds[i];
      std::list<ReceivedSubscription>::iterator rcvSubIter =
         std::find(mReceivedSubscriptions.begin(), mReceivedSubscriptions.end(), subId);
      if (rcvSubIter != mReceivedSubscriptions.end())
      {
         // Found it.
         ReceivedSubscription& recvSub = *rcvSubIter;

         // Before we erase the subscription, we need to remove any references to it from any publishers.
         for (std::map<WsfXIO_PublishKey, Publication>::iterator pubIter = mPublished.begin(); pubIter != mPublished.end();
              ++pubIter)
         {
            std::vector<ReceivedSubscription*>&          pubSubscriptions = pubIter->second.mRcvSubscriptions;
            std::vector<ReceivedSubscription*>::iterator pubSubIter =
               std::find(pubSubscriptions.begin(), pubSubscriptions.end(), &(*rcvSubIter));
            if (pubSubIter != pubSubscriptions.end())
            {
               if (mDebugLevel >= 1)
               {
                  auto out = ut::log::debug() << "xio_interface: Subscription removed.";
                  out.AddNote() << "Subscription: " << recvSub.mFilter;
                  out.AddNote() << "Removed from: " << pubIter->first;
               }
               pubIter->second.mDestination.Remove((*pubSubIter)->mConnectionPtr); // Remove its connection.
               pubSubscriptions.erase(pubSubIter);
            }
         }

         // Now remove the subscription.
         mReceivedSubscriptions.erase(rcvSubIter);
      }
   }
}

void WsfXIO_Publisher::ProcessPublication(WsfXIO_PublicationPkt& aPkt)
{
   ReceivedPublication*                                       publicationPtr = nullptr;
   std::map<WsfXIO_PublishKey, ReceivedPublication>::iterator rcvIter        = mReceivedPublications.find(aPkt.mKey);
   if (rcvIter == mReceivedPublications.end())
   {
      publicationPtr       = &mReceivedPublications[aPkt.mKey];
      publicationPtr->mKey = aPkt.mKey;
      for (std::list<Subscription>::iterator iter = mSubscriptions.begin(); iter != mSubscriptions.end(); ++iter)
      {
         if (iter->mFilter == aPkt.mKey)
         {
            if (mDebugLevel >= 1)
            {
               auto out = ut::log::debug() << "xio_interface: Publication received.";
               out.AddNote() << "Publication: " << aPkt.mKey;
               out.AddNote() << "Received by: " << iter->mFilter;
            }
            publicationPtr->mSubscriptions.push_back(&*iter);
         }
      }
   }
   else
   {
      publicationPtr = &rcvIter->second;
   }
   std::swap(aPkt.mData, publicationPtr->mData);

   // Dispatch the received publication to each subscriber.
   for (size_t i = 0; i < publicationPtr->mSubscriptions.size(); ++i)
   {
      Subscription* subscriptionPtr = publicationPtr->mSubscriptions[i];
      if (DispatchToSubscriber(aPkt.mSubscriberIds, *subscriptionPtr))
      {
         if (mDebugLevel >= 2)
         {
            auto out = ut::log::debug() << "xio_interface: Publication processed.";
            out.AddNote() << "Publication: " << aPkt.mKey;
            out.AddNote() << "Processed by: " << subscriptionPtr->mFilter;
         }
         subscriptionPtr->mCallback(aPkt.mKey);
      }
   }
}

void WsfXIO_Publisher::Unpublish(const WsfXIO_PublishKey& aKey)
{
   if (aKey.HasWildcards())
   {
      WsfXIO_PublishFilter filter(aKey);
      for (std::map<WsfXIO_PublishKey, Publication>::iterator i = mPublished.begin(); i != mPublished.end();)
      {
         if (filter == i->first)
         {
            mPublished.erase(i++);
         }
         else
         {
            ++i;
         }
      }
   }
   else
   {
      mPublished.erase(aKey);
   }
}

void WsfXIO_Publisher::HandleConnect(WsfXIO_Connection* aConnection)
{
   if (aConnection->IsReliable() && !mSubscriptions.empty())
   {
      if (!mSubscriptions.empty())
      {
         WsfXIO_SubscriptionPkt pkt;
         pkt.mCancel = false;
         for (std::list<Subscription>::iterator subIter = mSubscriptions.begin(); subIter != mSubscriptions.end(); ++subIter)
         {
            Subscription& sub = (*subIter);
            pkt.mSubscriptionIds.push_back(sub.mSubscriptionId);
            pkt.mKeyFilters.push_back(sub.mFilter);
         }
         aConnection->Send(pkt);
      }
   }
}

void WsfXIO_Publisher::HandleDisconnect(WsfXIO_Connection* aConnection)
{
   if (aConnection->IsReliable())
   {
      // Remove any subscriptions received by this connection
      for (std::list<ReceivedSubscription>::iterator rcvIter = mReceivedSubscriptions.begin();
           rcvIter != mReceivedSubscriptions.end();)
      {
         ReceivedSubscription& sub = (*rcvIter);
         if (sub.mConnectionPtr == aConnection)
         {
            for (std::map<WsfXIO_PublishKey, Publication>::iterator pubIter = mPublished.begin();
                 pubIter != mPublished.end();
                 ++pubIter)
            {
               std::vector<ReceivedSubscription*>&          pubSubscriptions = pubIter->second.mRcvSubscriptions;
               std::vector<ReceivedSubscription*>::iterator iter =
                  std::find(pubSubscriptions.begin(), pubSubscriptions.end(), &sub);
               if (iter != pubSubscriptions.end())
               {
                  pubSubscriptions.erase(iter);
               }
            }
            mReceivedSubscriptions.erase(rcvIter++);
         }
         else
         {
            ++rcvIter;
         }
      }
      // Remove any references to this connection on local publications
      for (std::map<WsfXIO_PublishKey, Publication>::iterator i = mPublished.begin(); i != mPublished.end(); ++i)
      {
         Publication& pub = i->second;
         for (size_t j = 0; j < pub.mRcvSubscriptions.size();)
         {
            ReceivedSubscription& sub = *pub.mRcvSubscriptions[j];
            if (sub.mConnectionPtr == aConnection)
            {
               pub.mRcvSubscriptions.erase(pub.mRcvSubscriptions.begin() + j);
            }
            else
            {
               ++j;
            }
         }
         pub.mDestination.RemoveAll(aConnection);
      }
   }
}

void WsfXIO_Publisher::Publish(const WsfXIO_PublishKey& aKey, char* aData, int aBytes)
{
   Publication*                                       pubPtr  = nullptr;
   std::map<WsfXIO_PublishKey, Publication>::iterator pubIter = mPublished.find(aKey);
   if (pubIter == mPublished.end())
   {
      if (mDebugLevel >= 1)
      {
         auto out = ut::log::debug() << "xio_interface: New publication.";
         out.AddNote() << "Publication: " << aKey;
      }
      pubPtr       = &mPublished[aKey];
      pubPtr->mKey = aKey;
      pubPtr->mData.assign(aData, aData + aBytes);
      for (std::list<ReceivedSubscription>::iterator rcvIter = mReceivedSubscriptions.begin();
           rcvIter != mReceivedSubscriptions.end();
           ++rcvIter)
      {
         if (rcvIter->mFilter == aKey)
         {
            pubPtr->mRcvSubscriptions.push_back(&*rcvIter);
            pubPtr->mDestination.Add(rcvIter->mConnectionPtr);
         }
      }
   }
   else
   {
      pubPtr = &pubIter->second;
      pubPtr->mData.assign(aData, aData + aBytes);
   }

   if (!pubPtr->mRcvSubscriptions.empty())
   {
      WsfXIO_PublicationPkt pkt;
      pkt.mData = pubPtr->mData;
      pkt.mKey  = aKey;
      pubPtr->mDestination.Send(pkt);
   }
}

//! Remove the specified subscription.
//! @note It is the callers responsibility to delete the associated 'SubscribeCallback'.
//! @param aSubscriptionId The subscription unique provided by the 'Subscribe' method.
//! @return Returns 'true' if the subscription exists and was successfully removed.
bool WsfXIO_Publisher::Unsubscribe(const SubscriptionId& aSubscriptionId)
{
   bool success = false;

   std::list<Subscription>::iterator iter = std::find(mSubscriptions.begin(), mSubscriptions.end(), aSubscriptionId);
   if (iter != mSubscriptions.end())
   {
      Subscription& sub = *iter;
      success           = true;

      if (mDebugLevel >= 1)
      {
         auto out = ut::log::debug() << "xio_interface: Removed subscription.";
         out.AddNote() << "Subscription: " << sub.mFilter;
      }

      // Remove any references to this subscription from the received publication list.
      for (std::map<WsfXIO_PublishKey, ReceivedPublication>::iterator rcvIter = mReceivedPublications.begin();
           rcvIter != mReceivedPublications.end();
           ++rcvIter)
      {
         ReceivedPublication& pub = rcvIter->second;

         std::vector<Subscription*>::iterator rcvPubIter =
            std::find(pub.mSubscriptions.begin(), pub.mSubscriptions.end(), &sub);
         if (rcvPubIter != pub.mSubscriptions.end())
         {
            pub.mSubscriptions.erase(rcvPubIter);
         }
      }

      // Send a packet to the publisher to remove this subscription.
      WsfXIO_SubscriptionPkt pkt;
      pkt.mCancel = true;
      pkt.mSubscriptionIds.push_back(sub.mSubscriptionId);
      pkt.mKeyFilters.push_back(sub.mFilter);
      const WsfXIO_Interface::ConnectionList& connections = mInterfacePtr->GetReliableConnections();
      for (size_t i = 0; i < connections.size(); ++i)
      {
         connections[i]->Send(pkt);
      }

      mSubscriptions.erase(iter);
   }

   if (success)
   {
      // Remove any received publications that don't have any subscribers.
      for (std::map<WsfXIO_PublishKey, ReceivedPublication>::iterator rcvPubIter = mReceivedPublications.begin();
           rcvPubIter != mReceivedPublications.end();)
      {
         if (rcvPubIter->second.mSubscriptions.empty())
         {
            mReceivedPublications.erase(rcvPubIter++);
         }
         else
         {
            ++rcvPubIter;
         }
      }
   }
   return success;
}

WsfXIO_Publisher::SubscribeCallback* WsfXIO_Publisher::Subscribe(const WsfXIO_PublishFilter& aFilter,
                                                                 SubscribeCallback*          aCallbackPtr)
{
   SubscriptionId subscriptionId;
   return Subscribe(subscriptionId, aFilter, aCallbackPtr);
}

WsfXIO_Publisher::SubscribeCallback* WsfXIO_Publisher::Subscribe(SubscriptionId&             aSubscriptionId,
                                                                 const WsfXIO_PublishFilter& aFilter,
                                                                 SubscribeCallback*          aCallbackPtr)
{
   mSubscriptions.push_back(Subscription());
   Subscription& sub = mSubscriptions.back();
   aSubscriptionId   = sub.mSubscriptionId;
   sub.mFilter       = aFilter;
   sub.mCallback.Connect(aCallbackPtr);
   if (mDebugLevel >= 1)
   {
      auto out = ut::log::debug() << "xio_interface: New subscription.";
      out.AddNote() << "Subscription: " << sub.mFilter;
   }

   for (std::map<WsfXIO_PublishKey, ReceivedPublication>::iterator rcvIter = mReceivedPublications.begin();
        rcvIter != mReceivedPublications.end();
        ++rcvIter)
   {
      if (aFilter == rcvIter->first)
      {
         rcvIter->second.mSubscriptions.push_back(&sub);
         (*aCallbackPtr)(rcvIter->first);
      }
   }

   WsfXIO_SubscriptionPkt pkt;
   pkt.mCancel = false;
   pkt.mSubscriptionIds.push_back(sub.mSubscriptionId);
   pkt.mKeyFilters.push_back(sub.mFilter);
   const WsfXIO_Interface::ConnectionList& connections = mInterfacePtr->GetReliableConnections();
   for (size_t i = 0; i < connections.size(); ++i)
   {
      connections[i]->Send(pkt);
   }
   return aCallbackPtr;
}

//! Returns 'true' if aSubscriberIds is empty (i.e. dispatch to all).
//! Returns 'true' if the aSubscription.mSubscriptionId is in aSubriberIds.
bool WsfXIO_Publisher::DispatchToSubscriber(const std::vector<SubscriptionId>& aSubscriberIds,
                                            const Subscription&                aSubscription) const
{
   bool dispatch = aSubscriberIds.empty();
   if (!dispatch)
   {
      dispatch =
         (find(aSubscriberIds.begin(), aSubscriberIds.end(), aSubscription.mSubscriptionId) != aSubscriberIds.end());
   }
   return dispatch;
}
