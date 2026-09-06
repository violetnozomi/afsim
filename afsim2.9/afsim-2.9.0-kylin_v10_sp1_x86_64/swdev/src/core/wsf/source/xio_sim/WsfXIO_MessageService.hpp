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

#ifndef WSFXIO_MESSAGEREQUEST_HPP
#define WSFXIO_MESSAGEREQUEST_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

#include "WsfComm.hpp"
class WsfMessage;
class WsfSimulation;
class WsfXIO_MessagePkt;
class WsfXIO_MessageRequestPkt;
#include "xio/WsfXIO_Request.hpp"
#include "xio/WsfXIO_Service.hpp"
class WsfXIO_Simulation;

//! Requests copies of messages sent/received by a platform
class WSF_EXPORT WsfXIO_MessageRequest : public WsfXIO_Request
{
public:
   WsfXIO_MessageRequest(WsfXIO_Connection* aConnectionPtr,
                         int                aPlatformIndex,
                         bool               aReceivedMessages,
                         bool               aTransmittedMessages,
                         bool               aIsReliable = true);

   //! Override this method to handle messages
   virtual void HandleMessage(WsfXIO_MessagePkt& aPkt) = 0;


private:
   void HandleMessagePrivate(WsfXIO_MessagePkt& aPkt);

   void Initialized() override;

   UtCallbackHolder mCallbacks;
   int              mPlatformIndex;
   bool             mGetReceived;
   bool             mGetTransmitted;
};

//! Provides WsfMessages sent and received by a platform as requested by WsfXIO_MessageRequest.
class WSF_EXPORT WsfXIO_MessageService : public WsfXIO_Service
{
public:
   WsfXIO_MessageService(WsfXIO_Simulation& aXIO_Simulation);

protected:
   void HandleRequest(WsfXIO_MessageRequestPkt& aPkt);

   void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr) override;

   void HandleReceivedMessage(double             aSimTime,
                              wsf::comm::Comm*   aXmtrCommPtr,
                              wsf::comm::Comm*   aReceiverCommPtr,
                              const WsfMessage&  aMessage,
                              wsf::comm::Result& aResult);

   void HandleTransmittedMessage(double aSimTime, wsf::comm::Comm* aXmtrCommPtr, const WsfMessage& aMessage);

   void HandleTransmitEndedMessage(double aSimTime, wsf::comm::Comm* aXmtrCommPtr, const WsfMessage& aMessage);

   void HandleTransmittedHeartbeatMessage(double aSimTime, wsf::comm::Comm* aXmtrCommPtr, const WsfMessage& aMessage);

   void HandleMessage(WsfPlatform* aPlatformPtr, const WsfMessage& aMessage, bool aIsReceived);

   void UpdateCallbacks();

   WsfSimulation&                             mSimulation;
   UtCallbackHolder                           mObserverCallbacks;
   typedef std::vector<WsfXIO_Subscription*>  SubscriptionList;
   typedef std::map<size_t, SubscriptionList> SubscriptionsMap;
   SubscriptionsMap                           mReceiveSubscribers;
   SubscriptionsMap                           mTransmitSubscribers;
};

#endif
