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

#include "xio/WsfXIO_PlatformListRequest.hpp"

#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

WsfXIO_PlatformListRequest::WsfXIO_PlatformListRequest(WsfXIO_Connection* aConnectionPtr)
   : WsfXIO_Request(aConnectionPtr, true)
{
}

void WsfXIO_PlatformListRequest::Initialized()
{
   SetIsReliable(true);
   mCallbacks.Add(GetConnection()->GetInterface().Connect(&WsfXIO_PlatformListRequest::HandleUpdate, this));
   WsfXIO_RequestPkt pkt;
   pkt.mSubscriptionType = WsfXIO_RequestPkt::cPLATFORM_LIST;
   SendRequest(pkt);
}

void WsfXIO_PlatformListRequest::HandleUpdate(WsfXIO_PlatformListUpdatePkt& aPkt)
{
   if (aPkt.GetSender() == GetConnection())
   {
      HandlePlatformList(aPkt);
   }
}
