// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PakLogServerSubscriber.hpp"

#include <iostream>

#include "GenTCP_Connection.hpp"
#include "PakLogPacket.hpp"
#include "PakProcessor.hpp"
#include "UtMemory.hpp"

log_server::LogServerMessageProcessor::LogServerMessageProcessor()
{
   mProcessor = ut::make_unique<PakProcessor>();
   mProcessor->RegisterPacket<log_server::LogPacket>("Log Packet", new log_server::LogPacket{});
}

void log_server::LogServerMessageProcessor::Process(const ut::log::Message& aMessage)
{
   LogPacket packet{};
   packet.message = aMessage;
   if (mTcpClient && mTcpClient->IsConnected())
   {
      mTcpClient->Send(packet);
   }
}

bool log_server::LogServerMessageProcessor::Init(const std::string& aLogServerHost, int aLogServerPort)
{
   mLogServerHost = aLogServerHost;
   mLogServerPort = aLogServerPort;

   // PakTCP_IO will take ownership of the connection.
   auto tcpConnection     = ut::make_unique<GenTCP_Connection>();
   bool isClientConnected = tcpConnection->Init(mLogServerHost, mLogServerPort);
   mTcpClient             = ut::make_unique<PakTCP_IO>(tcpConnection.release(), mProcessor.get());

   return isClientConnected;
}
