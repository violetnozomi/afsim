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

#include "xio_sim/WsfXIO_PlatformListService.hpp"

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "observer/WsfPlatformObserver.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_PlatformListService::WsfXIO_PlatformListService(WsfXIO_Simulation& aXIO_Simulation)
   : WsfXIO_Service(aXIO_Simulation.GetInterface())
   , mXIO_Simulation(aXIO_Simulation)
   , mSimulation(aXIO_Simulation.GetSimulation())
{
   mCallbacks += GetInterface()->Connect(&WsfXIO_PlatformListService::HandleRequest, this);
   mCallbacks +=
      WsfObserver::PlatformInitialized(&mSimulation).Connect(&WsfXIO_PlatformListService::PlatformInitialized, this);
   mCallbacks += WsfObserver::PlatformDeleted(&mSimulation).Connect(&WsfXIO_PlatformListService::PlatformDeleted, this);
}

void WsfXIO_PlatformListService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   mDestination.Remove(&aSubscriptionPtr->GetConnection());
}

namespace
{
WsfXIO_PlatformListUpdatePkt::PlatformData GetPlatformData(WsfPlatform* aPlatformPtr, WsfDisInterface* aDisInterfacePtr)
{
   WsfXIO_PlatformListUpdatePkt::PlatformData data;
   data.mIndex                  = static_cast<int>(aPlatformPtr->GetIndex());
   data.mName                   = aPlatformPtr->GetName();
   data.mIcon                   = aPlatformPtr->GetIconId();
   data.mSide                   = aPlatformPtr->GetSideId();
   data.mIsExternallyControlled = aPlatformPtr->IsExternallyControlled();
   if (aDisInterfacePtr != nullptr)
   {
      WsfDisPlatform* disPlatformPtr = aDisInterfacePtr->GetDisPlatform(data.mIndex);
      if (disPlatformPtr != nullptr)
      {
         data.mEntityId = disPlatformPtr->GetEntityId();
      }
   }
   return data;
}
} // namespace

void WsfXIO_PlatformListService::HandleRequest(WsfXIO_RequestPkt& aPkt)
{
   if (aPkt.mSubscriptionType == WsfXIO_RequestPkt::cPLATFORM_LIST)
   {
      WsfDisInterface*     disInterfacePtr = WsfDisInterface::Find(mSimulation);
      WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
      mDestination.Add(&subscriptionPtr->GetConnection());

      WsfXIO_PlatformListUpdatePkt pkt;
      size_t                       count = mSimulation.GetPlatformCount();
      for (size_t i = 0; i < count; ++i)
      {
         WsfPlatform* platformPtr = mSimulation.GetPlatformEntry(i);
         if (platformPtr != nullptr && CanSendPlatform(*platformPtr))
         {
            pkt.mPlatformsAdded.push_back(GetPlatformData(mSimulation.GetPlatformEntry(i), disInterfacePtr));
         }
      }
      subscriptionPtr->GetConnection().Send(pkt);
   }
}

void WsfXIO_PlatformListService::PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (CanSendPlatform(*aPlatformPtr))
   {
      WsfDisInterface*             disInterfacePtr = mXIO_Simulation.GetDisInterface();
      WsfXIO_PlatformListUpdatePkt pkt;
      pkt.mPlatformsAdded.push_back(GetPlatformData(aPlatformPtr, disInterfacePtr));
      pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
      mDestination.Send(pkt);
   }
}

void WsfXIO_PlatformListService::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (CanSendPlatform(*aPlatformPtr))
   {
      WsfXIO_PlatformListUpdatePkt pkt;
      pkt.mPlatformsDeleted.push_back(static_cast<int32_t>(aPlatformPtr->GetIndex()));
      pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
      mDestination.Send(pkt);
   }
}
bool WsfXIO_PlatformListService::CanSendPlatform(WsfPlatform& aPlatform)
{
   return !aPlatform.IsExternallyControlled();
}
