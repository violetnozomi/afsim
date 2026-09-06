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

#include "PakUndefinedPacket.hpp"

#include "PakSerialize.hpp"

PakUndefinedPacket::~PakUndefinedPacket()
{
   delete[] mPacketBuffer;
}
//! Before this packet can be read, it needs to be initialized with the
//! Packet ID, and size.  PacketHandlers should take care of this.
void PakUndefinedPacket::Initialize(int aPacketID, int aPacketLength)
{
   mPacketID     = aPacketID;
   mPacketLength = aPacketLength;
   mPacketBuffer = new char[aPacketLength];
}
