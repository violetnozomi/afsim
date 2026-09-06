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

#ifndef PAKBUFI_ARCHIVE_HPP
#define PAKBUFI_ARCHIVE_HPP

#include "packetio_export.h"

#include <string>

#include "GenBuffer.hpp"

class PakBufO_Archive;

//! Serializes data from a GenI
class PACKETIO_EXPORT PakI
{
public:
   static const bool       cIS_OUTPUT = false;
   typedef PakBufO_Archive OutputArchive;
   typedef PakI            InputArchive;

   PakI(GenBuffer* aBufferPtr);

   GenBuffer* GetBuffer() { return mBufferPtr; }

   void IgnoreBytes(int aNumBytes) { mBufferPtr->SetGetPos(mBufferPtr->GetGetPos() + aNumBytes); }

   template<typename T>
   void Serialize(T& aVal)
   {
      mBufferPtr->Get(aVal);
   }

   void SerializeBuffer(char* aBuffer, int aSize) { mBufferPtr->GetRaw(aBuffer, aSize); }

   void SerializeString(std::string& aString, int aBytes);

protected:
   GenBuffer* mBufferPtr;
};

#endif
