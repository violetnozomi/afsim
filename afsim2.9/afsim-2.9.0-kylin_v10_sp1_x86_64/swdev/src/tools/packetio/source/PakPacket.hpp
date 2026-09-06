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

#ifndef PAKPACKET_HPP
#define PAKPACKET_HPP

#include "packetio_export.h"

#include "PakIntTypes.hpp"

class PakSerialize;
class PakConnection;

/** The base class for any packet type.

*/
class PACKETIO_EXPORT PakPacket
{
public:
   typedef bool BaseType; // No base type

   PakPacket(int aPacketID);
   virtual ~PakPacket() {}
   //! Called by the PakProcessor after processing a received packet
   //! Gives an opportunity to delete pointers stored in packet
   virtual void ReceiveCleanup() {}
   //! Sets the sender of the packet.
   void SetSender(PakConnection* aConnection) { mConnectionPtr = aConnection; }

   //! Returns a PakConnection representing the sender of the packet.
   PakConnection* GetSender() const { return mConnectionPtr; }

   //! Returns the ID of the packet.
   int ID() const { return mPacketID; }

   int            GetOriginatorAddress() const { return mOriginatorAddress; }
   void           SetOriginatorAddress(unsigned int aAddr) { mOriginatorAddress = aAddr; }
   unsigned short GetOriginatorPort() const { return mOriginatorPort; }
   void           SetOriginatorPort(unsigned short aAddr) { mOriginatorPort = aAddr; }


protected:
   int            mPacketID;
   PakConnection* mConnectionPtr;
   unsigned int   mOriginatorAddress;
   unsigned short mOriginatorPort;
};

#endif
