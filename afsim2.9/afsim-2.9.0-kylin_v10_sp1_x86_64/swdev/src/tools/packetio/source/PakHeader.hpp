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

#ifndef PakHeader_HPP
#define PakHeader_HPP

#include "packetio_export.h"

#include "GenBuffer.hpp"

class GenI;
class GenO;

/** An interface for a packet header.  Allows for a customized header format.*/
class PACKETIO_EXPORT PakHeader
{
public:
   virtual PakHeader* Clone() const = 0;
   virtual ~PakHeader() {}

   //! Writes the header to a GenIO
   //! @param aIO The GenIO to Write to
   //! @param aPacketID The ID of the packet.
   //! @param aPacketLength The length of the packet, including this header.
   virtual void WriteHeader(GenBuffer& aIO, int aPacketID, int aPacketLength) = 0;

   //! Reads the header from a GenIO
   //! @param aIO The GenIO to Read from
   //! @param[out] aPacketID The ID of the packet
   //! @param[out] aPacketLength The Length of the packet, including this header
   virtual bool ReadHeader(GenBuffer& aIO, int& aPacketID, int& aPacketLength) = 0;

   //! Returns the number of bytes this header occupies.
   virtual int GetHeaderSize() = 0;
};


#endif
