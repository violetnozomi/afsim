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

#include "PakTCP_Connector.hpp"

#include "GenInternetSocketAddress.hpp"
#include "GenSocket.hpp"
#include "GenTCP_IO.hpp"
#include "GenTCP_Server.hpp"
#include "PakTCP_IO.hpp"
#include "UtMemory.hpp"
#include "UtWallClock.hpp"

namespace PakTCP_ConnectorDetail
{
//! Contains connection request information
class ConnectionInfo
{
public:
   std::unique_ptr<GenSockets::GenSocket> mSocketPtr;
   GenSockets::GenInternetSocketAddress   mAddress;
   double                                 mTimeoutTime;
};
} // namespace PakTCP_ConnectorDetail

PakTCP_Connector::PakTCP_Connector(PakProcessor* aProcessorPtr, PakHeader* aHeaderPtr)
   : mProcessorPtr(aProcessorPtr)
   , mTCP_ServerPtr(nullptr)
   , mHeaderPtr(aHeaderPtr)
{
}

PakTCP_Connector::~PakTCP_Connector()
{
   delete mTCP_ServerPtr;
   delete mHeaderPtr;
}

//! Begin listening for connections
bool PakTCP_Connector::Listen(int aPort /*= 0*/)
{
   delete mTCP_ServerPtr;
   mTCP_ServerPtr = new GenTCP_Server;
   mTCP_ServerPtr->SetOwnsConnections(false);
   return mTCP_ServerPtr->Init(aPort);
}

//! Returns the port PakTCP_Connector is listening on
int PakTCP_Connector::GetBoundPort()
{
   int port = 0;
   if (mTCP_ServerPtr != nullptr)
   {
      port = mTCP_ServerPtr->GetSocket()->GetBoundPort();
   }
   return port;
}

//! Poll for connections.
//! @param aWaitTime The duration of time to wait before returning
//!        if no connection is made.
//! @return A connected PakTCP_IO if successful,
//!  or a null pointer if no connection is ready.
PakTCP_IO* PakTCP_Connector::Accept(float aWaitTime)
{
   PakTCP_IO* pakIO(nullptr);
   GenTCP_IO* ioPtr = mTCP_ServerPtr->Accept(int(aWaitTime * 1.0E-6f));
   if (ioPtr != nullptr)
   {
      pakIO = new PakTCP_IO(ioPtr, mProcessorPtr, mHeaderPtr->Clone());
   }
   return pakIO;
}


//! Begins to connect to a TCP endpoint
//! @param aConnectionAddress The address to connect to
//! @param aTimeoutTime The maximum time to wait for connection
void PakTCP_Connector::BeginConnect(const GenSockets::GenInternetSocketAddress& aConnectionAddress, float aTimeoutTime)
{
   auto sockPtr = ut::make_unique<GenSockets::GenSocket>(GenSockets::GenSocket::cTCP_SOCKET);
   if (GenSockets::GenSocket::cWOULD_BLOCK == sockPtr->Connect(aConnectionAddress))
   {
      auto infoPtr        = ut::make_unique<PakTCP_ConnectorDetail::ConnectionInfo>();
      infoPtr->mAddress   = aConnectionAddress;
      infoPtr->mSocketPtr = std::move(sockPtr);
      UtWallClock wallClock;
      infoPtr->mTimeoutTime = wallClock.GetRawClock() + aTimeoutTime;
      mPendingConnections.push_back(std::move(infoPtr));
   }
}

//! Attempts to complete previous calls to BeginConnect()
//! @return 'true' if a connection was made
//! @param aConnectionAddress If a connection was made, aConnectionAddress is set
//!        to the address given to BeginConnect()
//! @param aIO_Ptr If a connection was made, aIO_Ptr is set to the an instance of a connected PakTCP_IO
bool PakTCP_Connector::CompleteConnect(GenSockets::GenInternetSocketAddress& aConnectionAddress, PakTCP_IO*& aIO_Ptr)
{
   bool ok = false;
   for (auto iter = std::begin(mPendingConnections); iter != std::end(mPendingConnections) && !ok;)
   {
      auto&                                infoPtr = *iter;
      GenSockets::GenSocket::ConnectStatus status  = infoPtr->mSocketPtr->WaitForConnection(0.0);
      bool                                 remove  = false;
      if (status == GenSockets::GenSocket::cCONNECT_IN_PROGRESS)
      {
         UtWallClock wallClock;
         remove = wallClock.GetRawClock() > infoPtr->mTimeoutTime;
      }
      else if (status == GenSockets::GenSocket::cCONNECT_SUCCESS)
      {
         aConnectionAddress  = infoPtr->mAddress;
         GenTCP_IO* genIOPtr = new GenTCP_IO();
         genIOPtr->Init(std::move(infoPtr->mSocketPtr));
         aIO_Ptr = new PakTCP_IO(genIOPtr, mProcessorPtr, mHeaderPtr->Clone());
         remove  = true;
         ok      = true;
      }
      else
      {
         remove = true;
      }

      if (remove)
      {
         iter = mPendingConnections.erase(iter);
      }
      else
      {
         ++iter;
      }
   }
   return ok;
}
