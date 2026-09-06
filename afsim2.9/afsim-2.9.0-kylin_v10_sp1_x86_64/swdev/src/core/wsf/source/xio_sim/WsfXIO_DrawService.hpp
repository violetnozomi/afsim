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

#ifndef WSFXIO_DRAWSERVICE_HPP
#define WSFXIO_DRAWSERVICE_HPP

#include "wsf_export.h"

#include "WsfDraw.hpp"
class WsfDrawManager;
#include "xio/WsfXIO_Destination.hpp"
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_RequestPkt;
#include "xio/WsfXIO_Service.hpp"
class WsfXIO_Simulation;

//! Services WsfXIO_DrawRequests
class WSF_EXPORT WsfXIO_DrawService : public WsfXIO_Service
{
public:
   WsfXIO_DrawService(WsfXIO_Simulation& aXIO_Simulation);
   ~WsfXIO_DrawService() override = default;

protected:
   void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr) override;
   void HandleRequest(WsfXIO_RequestPkt& aPkt);
   void DrawUpdate(const std::string& aTarget, const std::string& aLayer, const WsfDraw::DrawEventList& aEvents);

   WsfXIO_Destination          mDestination;
   std::unique_ptr<UtCallback> mDrawCallbackPtr;
   WsfDrawManager*             mDrawManagerPtr{nullptr};
};
#endif
