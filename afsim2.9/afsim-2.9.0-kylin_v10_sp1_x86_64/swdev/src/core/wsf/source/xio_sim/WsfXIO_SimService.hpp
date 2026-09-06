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

#ifndef WSFXIO_SIMSERVICE_HPP
#define WSFXIO_SIMSERVICE_HPP

#include <utility>
#include <vector>

class WsfSimulation;
#include "xio/WsfXIO_Service.hpp"

class WSF_EXPORT WsfXIO_SimService : public WsfXIO_Service
{
public:
   WsfXIO_SimService(WsfXIO_Interface* aInterfacePtr);

   void Initialize(WsfSimulation& aSimulation);

   void AddRequiredPlatform(WsfPlatform& aPlatform, WsfXIO_Subscription* aSubscriptionPtr);

protected:
   void RemovingSubscription(WsfXIO_Subscription* aSubscriptionPtr) override;

private:
   void HandlePlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   typedef std::vector<std::pair<size_t, WsfXIO_Subscription*>> RequiredPlatformList;

   RequiredPlatformList mRequiredPlatforms;
};

#endif
