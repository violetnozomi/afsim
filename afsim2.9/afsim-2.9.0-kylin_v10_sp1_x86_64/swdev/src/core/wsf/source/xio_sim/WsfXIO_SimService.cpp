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

#include "xio_sim/WsfXIO_SimService.hpp"

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "observer/WsfPlatformObserver.hpp"

WsfXIO_SimService::WsfXIO_SimService(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_Service(aInterfacePtr)
{
}

void WsfXIO_SimService::Initialize(WsfSimulation& aSimulation)
{
   mCallbacks += WsfObserver::PlatformDeleted(&aSimulation).Connect(&WsfXIO_SimService::HandlePlatformDeleted, this);
}

//! Adds a platform dependency on a subscription.  If the platform is destroyed, the subscription is canceled.
void WsfXIO_SimService::AddRequiredPlatform(WsfPlatform& aPlatform, WsfXIO_Subscription* aSubscriptionPtr)
{
   mRequiredPlatforms.push_back(std::make_pair(aPlatform.GetIndex(), aSubscriptionPtr));
}

void WsfXIO_SimService::HandlePlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   std::set<WsfXIO_Subscription*> cancelledSubscriptions;
   size_t                         platformIndex = aPlatformPtr->GetIndex();
   for (size_t i = 0; i < mRequiredPlatforms.size(); ++i)
   {
      if (mRequiredPlatforms[i].first == platformIndex)
      {
         cancelledSubscriptions.insert(mRequiredPlatforms[i].second);
      }
   }
   for (std::set<WsfXIO_Subscription*>::iterator i = cancelledSubscriptions.begin(); i != cancelledSubscriptions.end(); ++i)
   {
      SubscriptionCancelled(*i);
   }
}

void WsfXIO_SimService::RemovingSubscription(WsfXIO_Subscription* aSubscriptionPtr)
{
   for (size_t j = 0; j < mRequiredPlatforms.size();)
   {
      if (mRequiredPlatforms[j].second == aSubscriptionPtr)
      {
         mRequiredPlatforms.erase(mRequiredPlatforms.begin() + j);
      }
      else
      {
         ++j;
      }
   }
}
