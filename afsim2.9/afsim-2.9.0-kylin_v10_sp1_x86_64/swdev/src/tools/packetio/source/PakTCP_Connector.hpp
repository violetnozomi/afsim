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

#ifndef PAKTCP_CONNECTOR_HPP
#define PAKTCP_CONNECTOR_HPP

#include "packetio_export.h"

#include <memory>
#include <vector>

#include "PakDefaultHeader.hpp"

class GenTCP_Server;
class PakHeader;
class PakProcessor;
class PakTCP_IO;

namespace GenSockets
{
class GenSocket;
class GenInternetSocketAddress;
} // namespace GenSockets

namespace PakTCP_ConnectorDetail
{
class ConnectionInfo;
}

//! Handles listening for multiple client connections.
//! Handles asynchronous connect
class PACKETIO_EXPORT PakTCP_Connector
{
public:
   PakTCP_Connector(PakProcessor* aProcessorPtr, PakHeader* aHeaderPtr = new PakDefaultHeader);
   ~PakTCP_Connector();
   PakTCP_Connector(const PakTCP_Connector&) = delete;
   PakTCP_Connector& operator=(const PakTCP_Connector&) = delete;

   bool Listen(int aPort = 0);

   int GetBoundPort();

   PakTCP_IO* Accept(float aWaitTime);

   void BeginConnect(const GenSockets::GenInternetSocketAddress& aConnectionAddress, float aTimeoutTime);

   bool CompleteConnect(GenSockets::GenInternetSocketAddress& aConnectionAddress, PakTCP_IO*& aIO_Ptr);

private:
   using ConnectInfoList = std::vector<std::unique_ptr<PakTCP_ConnectorDetail::ConnectionInfo>>;
   PakProcessor*   mProcessorPtr;
   GenTCP_Server*  mTCP_ServerPtr;
   PakHeader*      mHeaderPtr;
   ConnectInfoList mPendingConnections;
};

#endif
