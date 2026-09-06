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

#include "xio/WsfXIO_DrawRequest.hpp"

#include "xio/WsfXIO_PacketRegistry.hpp"

WsfXIO_DrawRequest::WsfXIO_DrawRequest(WsfXIO_Connection* aConnectionPtr, bool aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
{
}

void WsfXIO_DrawRequest::Initialized()
{
   WsfXIO_RequestPkt pkt;
   pkt.mSubscriptionType = WsfXIO_RequestPkt::cDRAW;
   SendRequest(pkt);
}
