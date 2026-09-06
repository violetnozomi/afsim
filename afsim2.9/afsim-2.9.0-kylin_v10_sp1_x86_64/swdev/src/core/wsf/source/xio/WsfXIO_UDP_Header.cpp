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

#include "xio/WsfXIO_UDP_Header.hpp"

#include "GenI.hpp"
#include "GenO.hpp"
#include "UtLog.hpp"

WsfXIO_UDP_Header::WsfXIO_UDP_Header(int aVersionID)
   : mXIO_Version(0xfeedbeef ^ aVersionID)
{
   mReceivedInvalidPacket = false;
}

WsfXIO_UDP_Header::WsfXIO_UDP_Header(const WsfXIO_UDP_Header& aSrc)
   : PakHeader(aSrc)
   , mXIO_Version(aSrc.mXIO_Version)
   , mReceivedInvalidPacket(aSrc.mReceivedInvalidPacket)
{
}

PakHeader* WsfXIO_UDP_Header::Clone() const
{
   return new WsfXIO_UDP_Header(*this);
}

void WsfXIO_UDP_Header::WriteHeader(GenBuffer& aIO, int aPacketID, int aPacketLength)
{
   aIO.Put(mXIO_Version);
   aIO.Put(static_cast<int32_t>(aPacketLength - sizeof(header)));
   aIO.Put(static_cast<int32_t>(aPacketID));
}

bool WsfXIO_UDP_Header::ReadHeader(GenBuffer& aIO, int& aPacketID, int& aPacketLength)
{
   uint32_t version;
   int32_t  length, id;
   aIO.Get(version);
   aIO.Get(length);
   aPacketLength = length + sizeof(header);
   aIO.Get(id);
   aPacketID  = id;
   bool valid = (version == mXIO_Version);
   if (!valid && !mReceivedInvalidPacket)
   {
      mReceivedInvalidPacket = true;
      auto out               = ut::log::warning() << "xio_interface: Received PDU with invalid version.";
      out.AddNote() << "XIO versions may not match.";
   }
   return valid;
}

int WsfXIO_UDP_Header::GetHeaderSize()
{
   return sizeof(header);
}
