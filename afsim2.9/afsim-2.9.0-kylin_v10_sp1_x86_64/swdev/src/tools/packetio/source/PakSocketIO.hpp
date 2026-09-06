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

#ifndef PakIO_HPP
#define PakIO_HPP

#include "packetio_export.h"

#include "GenBuffer.hpp"

class GenIO;
// class PakSerializeReader;
// class PakSerializeWriter;
class PakHeader;
class PakPacket;
namespace GenSockets
{
class GenSocket;
}
/** A class that reads and writes packets
    see class PakTCP_IO and class PakUDP_IO
*/
class PACKETIO_EXPORT PakSocketIO
{
public:
   //! Constructor
   //! @param aHeaderType A pointer to the type of header to use in communication
   //!                    Can be null if no header is desired.
   PakSocketIO(PakHeader* aHeaderType)
      : mPacketHeaderType(aHeaderType)
   {
   }

   virtual ~PakSocketIO();

   //! Send a packet
   virtual bool Send(const PakPacket& aPkt) = 0;

   //! Receive a packet, and output the ID and Length.
   //! @param[out] aPacketId The ID of the packet
   //! @param[out] aPacketLength The length in bytes of the packet (including header)
   //! @param aWaitTimeMicroSeconds The time in microseconds to wait for an incoming packet
   //! @return 'true' if a packet was read.  'false' if no packet is available to read
   virtual bool ReceiveHeader(int& aPacketId, int& aPacketLength, int aWaitTimeMicroSeconds) = 0;


   //! After ReceiveHeader has returned true, this reads in the packet into aPkt.
   //! @param aPkt A reference to a packet of the correct type to receive
   //! @return 'true' if the packet was received correctly.
   virtual bool Receive(PakPacket& aPkt) = 0;

   //! After ReceiveHeader has returned true, this ignores the packet.
   virtual void IgnorePacket() = 0;

   //! Returns a pointer to the socket used for Receive methods
   virtual GenSockets::GenSocket* GetRecvSocket() const { return nullptr; }

   //! Returns a pointer to the socket used for Send methods
   virtual GenSockets::GenSocket* GetSendSocket() const { return nullptr; }

   PakHeader* GetHeaderType() { return mPacketHeaderType; }

protected:
   void SetPacketHeader(GenBuffer& aIO, int aPacketID, int aPacketLength);

   bool GetPacketHeader(GenBuffer& aIO, int& aPacketID, int& aPacketLength, bool& aIsInvalid);

   int GetHeaderSize();

private:
   PakHeader* mPacketHeaderType;
};
#endif
