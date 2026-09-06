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

#include "GenUDP_IO.hpp"

#include <iostream>

#include "GenBufIManaged.hpp"
#include "GenBufOManaged.hpp"
#include "GenHostName.hpp"
#include "GenIP.hpp"
#include "GenInternetAddress.hpp"
#include "GenInternetSocketAddress.hpp"
#include "GenSocket.hpp"
#include "GenSocketIncludes.hpp"

GenUDP_IO::GenUDP_IO()
   : GenUDP_Connection()
   , GenNetIO("GenUDP_IO", "GenUDP_IO")
{
   // Maximum Transmission Unit(MTU) is usually 1400-1500.
   mRecvBuf = new GenBufIManaged(GenBuf::BigEndian, 10000);
   SetGenBufXferI(mRecvBuf);
   mSendBuf = new GenBufOManaged(GenBuf::BigEndian, 10000, 1);
   SetGenBufXferO(mSendBuf);
}

GenUDP_IO::~GenUDP_IO()
{
   delete mRecvBuf;
   delete mSendBuf;
}

// virtual
//! Fill buffer with an incoming message.  Existing data in buffer
//! is removed.
//! @param aWaitTimeInMicroSec The Time in microseconds to wait for incoming messages
int GenUDP_IO::Receive(int aWaitTimeInMicroSec)
{
   // could be erasing unread data...
   mRecvBuf->Reset();
   mRecvBuf->SetNumValidBytes(0);
   ClearInputErrorStatus();

   int bytesRecv = ReceiveBuffer(aWaitTimeInMicroSec,
                                 reinterpret_cast<char*>(mRecvBuf->GetBuffer()),
                                 static_cast<int>(mRecvBuf->GetTotalBytes()));
   if (bytesRecv > 0)
   {
      mRecvBuf->SetNumValidBytes(bytesRecv);
   }
   return bytesRecv;
}

// virtual
//! Send data in the send buffer as a single UDP message
int GenUDP_IO::Send()
{
   ClearOutputErrorStatus();
   int bytesToSend = static_cast<int>(mSendBuf->GetNumValidBytes());
   int bytesSent   = SendBuffer(reinterpret_cast<const char*>(mSendBuf->GetBuffer()), bytesToSend);
   mSendBuf->Reset();
   mSendBuf->SetNumValidBytes(0);
   return bytesSent;
}
