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

#ifndef PAKUNDEFINEDPACKET_HPP
#define PAKUNDEFINEDPACKET_HPP

#include "packetio_export.h"

#include "PakPacket.hpp"

/** This packet is used for receiving packets that do not have a PakPacket implementation.
This class simply reads and stores a fixed number of bytes.
*/
class PACKETIO_EXPORT PakUndefinedPacket : public PakPacket
{
public:
   PakUndefinedPacket()
      : PakPacket(cPACKET_ID)
      , mPacketBuffer(nullptr)
   {
   }
   ~PakUndefinedPacket() override;
   template<class T>
   void Serialize(T& aBuff)
   {
      aBuff.SerializeBuffer(mPacketBuffer + 8, mPacketLength - 8);
   }
   void Initialize(int aPacketID, int aPacketLength);

   virtual PakPacket* New() { return new PakUndefinedPacket; }

   // This packet has no constant ID, the ID could be anything.
   enum
   {
      cPACKET_ID = 0
   };

   int   mPacketLength;
   char* mPacketBuffer;
};

#endif
