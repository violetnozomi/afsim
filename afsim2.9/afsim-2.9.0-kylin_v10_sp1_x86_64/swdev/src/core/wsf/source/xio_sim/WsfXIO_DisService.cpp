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

#include "xio_sim/WsfXIO_DisService.hpp"

#include <limits>

#include "WsfCommandChain.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "observer/WsfPlatformObserver.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Subscription.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

// =================================================================================================
WsfXIO_DisRequest::WsfXIO_DisRequest(WsfXIO_Connection* aConnectionPtr)
   : WsfXIO_Request(aConnectionPtr)
{
}

// =================================================================================================
// virtual
void WsfXIO_DisRequest::Initialized()
{
   WsfXIO_RequestDisDataPkt pkt;
   SendRequest(pkt);
}

// =================================================================================================
WsfXIO_DisRequestManager::WsfXIO_DisRequestManager(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_RequestManager(aInterfacePtr)
{
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_DisRequestManager::HandlePlatformInfo, this));
}

// =================================================================================================
// TODO_XIO Why is this needed. It just forwards to the base class.
void WsfXIO_DisRequestManager::AddRequest(WsfXIO_DisRequest* aRequestPtr)
{
   WsfXIO_RequestManager::AddRequest(aRequestPtr);
}

// =================================================================================================
// TODO_XIO Why is this needed. It just forwards to the base class.
void WsfXIO_DisRequestManager::RemoveRequest(WsfXIO_DisRequest* aRequestPtr)
{
   WsfXIO_RequestManager::RemoveRequest(aRequestPtr);
}

// =================================================================================================
// virtual
void WsfXIO_DisRequestManager::RequestCancelled(WsfXIO_Request* aRequestPtr)
{
   WsfXIO_DisRequest* requestPtr = static_cast<WsfXIO_DisRequest*>(aRequestPtr);
   if (requestPtr != nullptr)
   {
      // TODO_XIO what is this doing
   }
   WsfXIO_RequestManager::RequestCancelled(aRequestPtr);
}

// =================================================================================================
void WsfXIO_DisRequestManager::HandlePlatformInfo(WsfXIO_DisPlatformInfoPkt& aPkt)
{
   WsfXIO_DisRequest* requestPtr = (WsfXIO_DisRequest*)FindRequest(aPkt.mRequestId);
   if (requestPtr != nullptr)
   {
      requestPtr->HandlePlatformInfo(aPkt);
   }
}

// =================================================================================================
WsfXIO_DisService::WsfXIO_DisService(WsfXIO_Simulation& aXIO_Simulation)
   : WsfXIO_Service(aXIO_Simulation.GetInterface())
   , mSimulation(aXIO_Simulation.GetSimulation())
   , mDisInterfacePtr(aXIO_Simulation.GetDisInterface())
{
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_DisService::HandleRequest, this);
   mCallbacks += WsfObserver::PlatformInitialized(&mSimulation).Connect(&WsfXIO_DisService::PlatformInitialized, this);
   mCallbacks += WsfObserver::PlatformDeleted(&mSimulation).Connect(&WsfXIO_DisService::PlatformDeleted, this);
}

// =================================================================================================
void WsfXIO_DisService::HandleRequest(WsfXIO_RequestDisDataPkt& aRequest)
{
   auto* subscriptionPtr = CreateSubscription(aRequest);
   for (unsigned int i = 0; i < mSimulation.GetPlatformCount(); ++i)
   {
      SendPlatformInfo(subscriptionPtr, *mSimulation.GetPlatformEntry(i), true);
   }
}

// =================================================================================================
//! Callback from the simulation when a platform is initialized and just about to be added to the simulation.
void WsfXIO_DisService::PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr)
{
   const SubscriptionMap& subscriptions = GetSubscriptions();
   for (const auto& subscriptionPair : subscriptions)
   {
      SendPlatformInfo(subscriptionPair.second, *aPlatformPtr, true);
   }
}

// =================================================================================================
//! Callback from the simulation when a platform is deleted.
void WsfXIO_DisService::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   const SubscriptionMap& subscriptions = GetSubscriptions();
   for (const auto& subscriptionPair : subscriptions)
   {
      SendPlatformInfo(subscriptionPair.second, *aPlatformPtr, false);
   }
}

void WsfXIO_DisService::SendPlatformInfo(WsfXIO_Subscription* aSubscription, const WsfPlatform& aPlatform, bool aIsAlive) const
{
   WsfXIO_DisPlatformInfoPkt pkt;
   PackPlatformInfo(aPlatform, pkt, aIsAlive);
   pkt.mRequestId = aSubscription->GetRequestId();
   aSubscription->GetDataConnection()->Send(pkt);
}

// =================================================================================================
void WsfXIO_DisService::PackPlatformInfo(const WsfPlatform& aPlatform, WsfXIO_DisPlatformInfoPkt& aPkt, bool aIsAlive) const
{
   WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(aPlatform.GetIndex());
   if (disPlatformPtr != nullptr)
   {
      DisEntityId entityId = disPlatformPtr->GetEntityId();
      aPkt.mEntityId       = entityId;
   }

   aPkt.mName                   = aPlatform.GetNameId();
   aPkt.mType                   = aPlatform.GetTypeId();
   aPkt.mSide                   = aPlatform.GetSideId();
   aPkt.mPlatformIndex          = static_cast<int32_t>(aPlatform.GetIndex());
   aPkt.mIsAlive                = aIsAlive;
   aPkt.mIsExternallyControlled = aPlatform.IsExternallyControlled();
   for (WsfComponentList::RoleIterator<WsfCommandChain> ci(aPlatform); !ci.AtEnd(); ++ci)
   {
      WsfStringId chainName(ci->GetNameId());
      WsfStringId commanderName(ci->GetCommanderNameId());
      aPkt.mCommanders.emplace_back(chainName, commanderName);
   }
}
