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

#ifndef WSFXIO_PLATFORMSERVICE_HPP
#define WSFXIO_PLATFORMSERVICE_HPP

#include "wsf_export.h"

#include "GenUniqueId.hpp"
class WsfPlatform;
class WsfSimulation;
#include "xio/WsfXIO_Destination.hpp"
class WsfXIO_PlatformListUpdatePkt;
class WsfXIO_PlatformListRequest;
class WsfXIO_RequestPkt;
#include "xio/WsfXIO_Request.hpp"
#include "xio/WsfXIO_Service.hpp"
class WsfXIO_Simulation;
#include "xio/WsfXIO_Subscription.hpp"

//! Fulfills platform list subscription requests
class WSF_EXPORT WsfXIO_PlatformListService : public WsfXIO_Service
{
public:
   WsfXIO_PlatformListService(WsfXIO_Simulation& aXIO_Simulation);

   void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr) override;

private:
   void HandleRequest(WsfXIO_RequestPkt& aPkt);

   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   bool CanSendPlatform(WsfPlatform& aPlatform);

   WsfXIO_Simulation& mXIO_Simulation;
   WsfSimulation&     mSimulation;
   WsfXIO_Destination mDestination;
};

#endif
