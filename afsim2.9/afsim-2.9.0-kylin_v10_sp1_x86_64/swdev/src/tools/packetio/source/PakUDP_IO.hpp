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

#ifndef PakUDP_IO_HPP
#define PakUDP_IO_HPP

#include "packetio_export.h"

#include "PakDefaultHeader.hpp"
#include "PakSocketIO.hpp"
class GenUDP_Connection;
class PakPacket;
class PakProcessor;
class GenUDP_IO;
class PakO;
class PakI;
//! Sends and receives packets via UDP.
//! Each PakPacket must fit inside a UDP packet.
class PACKETIO_EXPORT PakUDP_IO : public PakSocketIO
{
public:
   PakUDP_IO(GenUDP_Connection* aConnection, PakProcessor* aProcessorPtr, PakHeader* aHeaderType = new PakDefaultHeader);
   ~PakUDP_IO() override;

   bool Send(const PakPacket& aPkt) override;

   bool ReceiveHeader(int& aPacketId, int& aPacketLength, int aWaitTimeMicroSeconds) override;

   bool Receive(PakPacket& aPkt) override;

   void IgnorePacket() override;

   GenUDP_Connection& GetConnection() { return *mConnectionPtr; }

   GenSockets::GenSocket* GetRecvSocket() const override;

   GenSockets::GenSocket* GetSendSocket() const override;

   PakPacket* ReceiveNew();

   PakProcessor* GetPakProcessor() const { return mProcessorPtr; }

protected:
   void               ReadUDP();
   bool               ReadMoreUDP();
   bool               ReadToBoundaryUDP();
   GenUDP_Connection* mConnectionPtr;
   PakProcessor*      mProcessorPtr;
   GenBuffer          mBufO;
   GenBuffer          mBufI;
   PakO*              mSerializeWriter;
   PakI*              mSerializeReader;
   bool               mIsHeaderRead;
   int                mLastId;
   bool               mHasReadHeader;
   int                mHeaderPacketId;
   int                mHeaderPacketLength;
   char*              mEmptyJunk;
   int                mHeaderSize;

private:
   void operator=(const PakUDP_IO&); // Not allowed
};

#endif
