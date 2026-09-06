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

#ifndef WSFXIO_UDP_HEADER_HPP
#define WSFXIO_UDP_HEADER_HPP

#include "wsf_export.h"

#include "PakHeader.hpp"
#include "PakIntTypes.hpp"

//! Defines the header used for UDP packets.
//! The purpose of this is to ignore packets that are
//! either not XIO packets, or not the correct version
class WSF_EXPORT WsfXIO_UDP_Header : public PakHeader
{
public:
   WsfXIO_UDP_Header(int aVersionID);
   WsfXIO_UDP_Header(const WsfXIO_UDP_Header& aSrc);
   ~WsfXIO_UDP_Header() override {}

   PakHeader* Clone() const override;

   void WriteHeader(GenBuffer& aIO, int aPacketID, int aPacketLength) override;

   bool ReadHeader(GenBuffer& aIO, int& aPacketID, int& aPacketLength) override;

   int GetHeaderSize() override;

protected:
   struct header
   {
      uint32_t mXIO_Version;
      int32_t  mLength;
      int32_t  mID;
   };
   uint32_t mXIO_Version;
   bool     mReceivedInvalidPacket;
};

#endif
