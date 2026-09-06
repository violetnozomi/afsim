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

#include "xio/WsfXIO_TaskRequest.hpp"

#include "xio/WsfXIO_PacketRegistry.hpp"

WsfXIO_TaskRequest::WsfXIO_TaskRequest(WsfXIO_Connection* aConnectionPtr,
                                       unsigned int       aPlatformIndex,
                                       WsfStringId        aProcessorName,
                                       bool               aTakeControl,
                                       bool               aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
   , mTakeControl(aTakeControl)
   , mPlatformIndex(aPlatformIndex)
   , mProcessorName(aProcessorName)
{
}

// virtual
void WsfXIO_TaskRequest::HandleResponse(WsfXIO_ResponsePkt& aPkt)
{
   if (WsfXIO_TaskInitPkt::cPACKET_ID == aPkt.ID())
   {
      HandleInitialize((WsfXIO_TaskInitPkt&)aPkt);
   }
   else if (WsfXIO_TaskUpdatePkt::cPACKET_ID == aPkt.ID())
   {
      HandleUpdate((WsfXIO_TaskUpdatePkt&)aPkt);
   }
}

// virtual
void WsfXIO_TaskRequest::Initialized()
{
   WsfXIO_TaskRequestPkt pkt;
   pkt.mPlatformIndex     = mPlatformIndex;
   pkt.mTaskProcessorName = mProcessorName;
   pkt.mTakeControl       = mTakeControl;
   SendRequest(pkt);
}
