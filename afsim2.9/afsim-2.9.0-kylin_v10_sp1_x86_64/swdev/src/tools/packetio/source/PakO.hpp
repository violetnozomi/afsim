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

#ifndef PAKBUFO_ARCHIVE_HPP
#define PAKBUFO_ARCHIVE_HPP

#include "packetio_export.h"

#include <string>

#include "GenBuffer.hpp"
class PakI;

//! Serializes data to a GenO
class PACKETIO_EXPORT PakO
{
public:
   static const bool cIS_OUTPUT = true;
   typedef PakO      OutputArchive;
   typedef PakI      InputArchive;

   PakO(GenBuffer* aBufferPtr);

   GenBuffer* GetBuffer() { return mBufferPtr; }

   void IgnoreBytes(int aNumBytes)
   {
      mBufferPtr->CheckPutSpace(aNumBytes);
      mBufferPtr->SetPutPos(mBufferPtr->GetPutPos() + aNumBytes);
   }

   template<typename T>
   void Serialize(const T& aVal)
   {
      mBufferPtr->Put(aVal);
   }

   void SerializeBuffer(char* aBuffer, int aSize) { mBufferPtr->PutRaw(aBuffer, aSize); }

   void SerializeString(std::string& aString, int aBytes);

protected:
   GenBuffer* mBufferPtr;
};

#endif
