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

#ifndef PakDefaultHeader_HPP
#define PakDefaultHeader_HPP

#include "packetio_export.h"

#include "GenBuffer.hpp"
#include "PakHeader.hpp"
#include "PakIntTypes.hpp"
class GenO;
class GenI;
/** The default packet header.
   Contains a 32 bit length field containing the length of the packet.
   Contains a 32 bit id field containing a unique packet ID.
*/
class PACKETIO_EXPORT PakDefaultHeader : public PakHeader
{
public:
   ~PakDefaultHeader() override;

   PakHeader* Clone() const override;

   void WriteHeader(GenBuffer& aIO, int aPacketID, int aPacketLength) override;

   //! Returns true if read header successfully
   bool ReadHeader(GenBuffer& aIO, int& aPacketID, int& aPacketLength) override;

   int GetHeaderSize() override;

protected:
   struct header
   {
      int32_t length;
      int32_t id;
   };
};
#endif
