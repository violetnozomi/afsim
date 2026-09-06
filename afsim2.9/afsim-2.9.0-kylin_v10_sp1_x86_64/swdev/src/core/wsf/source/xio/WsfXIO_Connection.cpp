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

#include "xio/WsfXIO_Connection.hpp"

#include <iostream>

#include "PakTCP_IO.hpp"
#include "PakUDP_IO.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"


WsfXIO_Connection::WsfXIO_Connection(WsfXIO_Interface* aInterfacePtr, PakSocketIO* aIOPtr)
   : mInterfacePtr(aInterfacePtr)
   , mLinkedConnectionPtr(nullptr)
   , mIOPtr(aIOPtr)
   , mIsServer(false)
   , mIsInitialized(false)
   , mDisconnecting(false)
   , mHasClockTranslation(false)
   , mRecvClockOffset(0.0)
   , mLastTimeStamp(0.0)
{
   static int sUniqueConnectionId = 1;
   mConnectionId                  = sUniqueConnectionId++;

   mTCP_IO_Ptr = dynamic_cast<PakTCP_IO*>(mIOPtr);
   mUDP_IO_Ptr = dynamic_cast<PakUDP_IO*>(mIOPtr);
}

WsfXIO_Connection::~WsfXIO_Connection()
{
   delete mIOPtr;
}

void WsfXIO_Connection::Send(WsfXIO_Packet& aPkt)
{
   mInterfacePtr->Send(aPkt, this);
}

//! Sets the offset between local and remote simulation time
void WsfXIO_Connection::SetClockOffset(double aRecvOffset)
{
   mRecvClockOffset = aRecvOffset;
}

//! Returns the offset between local and remote simulation time
double WsfXIO_Connection::GetClockOffset() const
{
   return mRecvClockOffset;
}

UtScriptClass* WsfXIO_Connection::mScriptClassPtr = nullptr;
