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

#include "PakDefaultHeader.hpp"

#include "GenIO.hpp"

// virtual
PakDefaultHeader::~PakDefaultHeader() {}

// virtual
PakDefaultHeader::PakHeader* PakDefaultHeader::Clone() const
{
   return new PakDefaultHeader;
}

// virtual
void PakDefaultHeader::WriteHeader(GenBuffer& aIO, int aPacketID, int aPacketLength)
{
   aIO.Put(static_cast<int32_t>(aPacketLength - 8));
   aIO.Put(static_cast<int32_t>(aPacketID));
}

// virtual
bool PakDefaultHeader::ReadHeader(GenBuffer& aIO, int& aPacketID, int& aPacketLength)
{
   int32_t id, length;
   aIO.Get(length);
   aPacketLength = length + 8;
   aIO.Get(id);
   aPacketID = id;
   return true;
}

// virtual
int PakDefaultHeader::GetHeaderSize()
{
   return sizeof(header);
}
