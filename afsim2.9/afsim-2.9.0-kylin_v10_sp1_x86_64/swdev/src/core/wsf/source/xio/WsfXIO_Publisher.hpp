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

#ifndef WSFXIO_PUBLISHER_HPP
#define WSFXIO_PUBLISHER_HPP

#include "wsf_export.h"

#include <list>
#include <map>
#include <string>
#include <vector>

#include "GenMemI.hpp"
#include "GenMemIO.hpp"
#include "GenUniqueId.hpp"
#include "UtBinder.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
class WsfXIO_Connection;
class WsfXIO_Interface;
#include "xio/WsfXIO_PublishKey.hpp"
class WsfXIO_PublicationPkt;
class WsfXIO_SubscriptionPkt;

// Includes for serialization
#include "xio/WsfXIO_Defines.hpp"
#include "xio/WsfXIO_Destination.hpp"
#include "xio/WsfXIO_SerializeTypes.hpp"


/** WsfXIO_Publisher Provides a basic Publish/Subscribe mechanism.
Example:

   WsfTrack someTrack;
   WsfXIO_Publisher& publisher = xioInterface->GetPublisher();

   // publish someTrack using this data as a key
   // Keys can have up to 4 fields -- each being a string or an integer.
   WsfXIO_PublishKey key("my_data", "track-data", 54);

   // Any datatype may be published as long as it can be serialized.
   //   Some examples of valid data are: int, double, WsfTrack, or std::vector<std::pair<int, WsfTrackId> >
   // Publish() may be called subsequent times to update the data
   publisher.Publish(key, someTrack);

   ...

   // subscribe to publications with keys containing this data, "*" is a wildcard:
   // Publications can be received from any connected application
   WsfXIO_PublishFilter keyFilter("my_data", "track-data", "*");
   // A Callback is invoked when a publication is received
   publisher.Subscribe(keyFilter, RecvTrackSubscription);

   ...

   // This is called when a publication is received
   void RecvTrackSubscription(const WsfXIO_PublishKey& aKey)
   {
      // Parameter to GetData() must be the same as Publish(), otherwise GetData() results in undefined behavior
      WsfTrack trackData;
      publisher.GetData(aKey, trackData);
   }

*/
class WSF_EXPORT WsfXIO_Publisher
{
   WsfXIO_Publisher& operator=(const WsfXIO_Publisher&) = delete; // Not implemented
   template<typename C>
   using MemberFnPtr    = void (C::*)(const WsfXIO_PublishKey&);
   using NonMemberFnPtr = void (*)(const WsfXIO_PublishKey&);

public:
   WsfXIO_Publisher(WsfXIO_Interface* aInterfacePtr);

   using SubscriptionId        = GenUniqueId;
   using SubscribeCallbackList = UtCallbackListN<void(const WsfXIO_PublishKey&)>;
   using SubscribeCallback     = SubscribeCallbackList::CallbackType;

   //! Remove a previously published publication
   void Unpublish(const WsfXIO_PublishKey& aKey);

   //! Publish some data using a key.
   template<typename T>
   void Publish(const WsfXIO_PublishKey& aKey, const T& aData)
   {
      GenBuffer buf;
      buf.SetBigEndian();
      PakO ar(&buf);
      ar& const_cast<T&>(aData);
      auto bytes = static_cast<int>(buf.GetPutPos());
      Publish(aKey, (char*)buf.GetBuffer(), bytes);
   }

   bool Unsubscribe(const SubscriptionId& aSubscriptionId);

   //! Subscribe to data being published in another application.
   //! Data published with keys that match aFilter will trigger
   //! the returned callback.  GetData() is used to get the published data.
   SubscribeCallback* Subscribe(const WsfXIO_PublishFilter& aFilter, SubscribeCallback* aCallbackPtr);

   SubscribeCallback* Subscribe(SubscriptionId&             aSubscriptionId,
                                const WsfXIO_PublishFilter& aFilter,
                                SubscribeCallback*          aCallbackPtr);

   template<typename C>
   std::unique_ptr<SubscribeCallback> Subscribe(const WsfXIO_PublishFilter& aFilter, MemberFnPtr<C> aMethodPtr, C* aThisPtr)
   {
      SubscriptionId subscriptionId;
      return Subscribe(subscriptionId, aFilter, aMethodPtr, aThisPtr);
   }

   template<typename C>
   std::unique_ptr<SubscribeCallback>
   Subscribe(SubscriptionId& aSubscriptionId, const WsfXIO_PublishFilter& aFilter, MemberFnPtr<C> aMethodPtr, C* aThisPtr)
   {
      auto callbackPtr = ut::make_unique<SubscribeCallback>(UtStd::Bind(aMethodPtr, aThisPtr));
      Subscribe(aSubscriptionId, aFilter, callbackPtr.get());
      return callbackPtr;
   }

   std::unique_ptr<SubscribeCallback> Subscribe(const WsfXIO_PublishFilter& aFilter, NonMemberFnPtr aFunctionPtr)
   {
      SubscriptionId subscriptionId;
      return Subscribe(subscriptionId, aFilter, aFunctionPtr);
   }

   std::unique_ptr<SubscribeCallback> Subscribe(SubscriptionId&             aSubscriptionId,
                                                const WsfXIO_PublishFilter& aFilter,
                                                NonMemberFnPtr              aFunctionPtr)
   {
      auto callbackPtr = ut::make_unique<SubscribeCallback>(UtStd::Bind(aFunctionPtr));
      Subscribe(aSubscriptionId, aFilter, callbackPtr.get());
      return callbackPtr;
   }

   std::unique_ptr<SubscribeCallback> Subscribe(const WsfXIO_PublishFilter&            aFilter,
                                                const SubscribeCallback::FunctionType& aFunc)
   {
      SubscriptionId subscriptionId;
      return Subscribe(subscriptionId, aFilter, aFunc);
   }

   std::unique_ptr<SubscribeCallback> Subscribe(SubscriptionId&                        aSubscriptionId,
                                                const WsfXIO_PublishFilter&            aFilter,
                                                const SubscribeCallback::FunctionType& aFunc)
   {
      auto callbackPtr = ut::make_unique<SubscribeCallback>(aFunc);
      Subscribe(aSubscriptionId, aFilter, callbackPtr.get());
      return callbackPtr;
   }

   //! Read data from a received publication.
   //! @param aKey The key of the publication
   //! @param aData The data being received from the publication.
   template<typename T>
   bool GetData(const WsfXIO_PublishKey aKey, T& aData)
   {
      bool ok = false;
      auto i  = mReceivedPublications.find(aKey);
      if (i != mReceivedPublications.end())
      {
         GenBuffer io(&i->second.mData[0], static_cast<int>(i->second.mData.size()));
         io.SetBigEndian();
         PakI ar(&io);
         ar&  aData;
         ok = true;
      }
      return ok;
   }

   void SetDebugLevel(int aDebugLevel) { mDebugLevel = aDebugLevel; }

private:
   void HandleConnect(WsfXIO_Connection* aConnection);
   void HandleDisconnect(WsfXIO_Connection* aConnection);

   void ProcessSubscription(WsfXIO_SubscriptionPkt& aPkt);
   void ProcessUnsubscription(WsfXIO_SubscriptionPkt& aPkt);
   void ProcessPublication(WsfXIO_PublicationPkt& aPkt);

   void Publish(const WsfXIO_PublishKey& aKey, char* aData, int aBytes);

   using DataBuffer = std::vector<char>;

   struct ReceivedSubscription;

   struct Publication
   {
      WsfXIO_PublishKey                  mKey;
      DataBuffer                         mData;
      WsfXIO_Destination                 mDestination;
      std::vector<ReceivedSubscription*> mRcvSubscriptions;
   };

   struct Subscription
   {
   private:
      Subscription& operator=(const Subscription& aRhs);

   public:
      SubscriptionId        mSubscriptionId;
      WsfXIO_PublishFilter  mFilter;
      SubscribeCallbackList mCallback;

      bool operator==(const SubscriptionId& aSubscriptionId) const { return mSubscriptionId == aSubscriptionId; }
      bool operator!=(const SubscriptionId& aSubscriptionId) const { return !(mSubscriptionId == aSubscriptionId); }
   };

   struct ReceivedPublication
   {
      WsfXIO_PublishKey          mKey;
      DataBuffer                 mData;
      std::vector<Subscription*> mSubscriptions;
   };

   struct ReceivedSubscription
   {
      WsfXIO_Connection*   mConnectionPtr;
      WsfXIO_PublishFilter mFilter;
      SubscriptionId       mSubscriptionId;

      bool operator==(const SubscriptionId& aSubscriptionId) const { return mSubscriptionId == aSubscriptionId; }
      bool operator!=(const SubscriptionId& aSubscriptionId) const { return !(mSubscriptionId == aSubscriptionId); }
   };

   bool DispatchToSubscriber(const std::vector<SubscriptionId>& aSubscriberIds, const Subscription& aSubscription) const;

   std::map<WsfXIO_PublishKey, Publication>         mPublished;
   std::list<Subscription>                          mSubscriptions;
   std::map<WsfXIO_PublishKey, ReceivedPublication> mReceivedPublications;
   std::list<ReceivedSubscription>                  mReceivedSubscriptions;

   UtCallbackHolder  mCallbacks;
   WsfXIO_Interface* mInterfacePtr;
   int               mDebugLevel;
};

#endif
