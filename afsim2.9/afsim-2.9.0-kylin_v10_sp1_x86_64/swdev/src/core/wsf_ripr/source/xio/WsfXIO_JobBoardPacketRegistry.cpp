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
#include "xio/WsfXIO_JobBoardPacketRegistry.hpp"

#include "PakProcessor.hpp"
#include "PakSerializeImpl.hpp"
#include "xio/WsfXIO_SerializeTypes.hpp"

// Helper macro to ease the pain of registering packets
// Copied from the hidden WsfXIO_PacketRegistry definition.
#define VALID_ID_RANGE(ID) ((ID >= cXIO_FIRST_PACKET_ID_RIPR) && (ID <= cXIO_LAST_PACKET_ID_RIPR))
#define REGISTER_PACKET(Z, N)            \
   aProcessor.RegisterPacket(#Z, new Z); \
   assert(N + cXIO_FIRST_PACKET_ID_RIPR == Z::cPACKET_ID && VALID_ID_RANGE(Z::cPACKET_ID));

WsfXIO_JobBoardPacketRegistry::WsfXIO_JobBoardPacketRegistry() {}

WsfXIO_JobBoardPacketRegistry::~WsfXIO_JobBoardPacketRegistry() {}

//! Registers all packets used by XIO.  Unregistered packets cannot be sent or received.
void WsfXIO_JobBoardPacketRegistry::Initialize(PakProcessor& aProcessor)
{
   REGISTER_PACKET(WsfXIO_JobBoardRequestPkt, 0);
   REGISTER_PACKET(WsfXIO_JobBoardInitPkt, 1);
   REGISTER_PACKET(WsfXIO_JobBoardUpdatePkt, 2);
   REGISTER_PACKET(WsfXIO_JobBoardCommandPkt, 3);
   REGISTER_PACKET(WsfXIO_ChannelIdUpdatePkt, 4);
   REGISTER_PACKET(WsfXIO_RIPRManagerRequestPkt, 5);
   REGISTER_PACKET(WsfXIO_RIPRManagerInitPkt, 6);
   REGISTER_PACKET(WsfXIO_RIPRManagerUpdatePkt, 7);
   REGISTER_PACKET(WsfXIO_BidControlUpdatePkt, 8);
   REGISTER_PACKET(WsfXIO_JobBoardWinnersUpdatePkt, 9);
}
