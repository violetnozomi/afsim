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

#ifndef WSFXIO_PLATFORMLISTREQUEST_HPP
#define WSFXIO_PLATFORMLISTREQUEST_HPP

#include "wsf_export.h"

#include "UtCallbackHolder.hpp"
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_PlatformListUpdatePkt;

//! Requests for an application to send the local platform list information
//! Must be subclassed to implement the handling of platform list updates.
class WSF_EXPORT WsfXIO_PlatformListRequest : public WsfXIO_Request
{
public:
   WsfXIO_PlatformListRequest(WsfXIO_Connection* aConnectionPtr);

   void         Initialized() override;
   virtual void HandlePlatformList(WsfXIO_PlatformListUpdatePkt& aPkt) = 0;

private:
   void             HandleUpdate(WsfXIO_PlatformListUpdatePkt& aPkt);
   UtCallbackHolder mCallbacks;
};

#endif
