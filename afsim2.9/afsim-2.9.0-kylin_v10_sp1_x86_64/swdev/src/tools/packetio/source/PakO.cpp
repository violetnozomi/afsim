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

#include "PakO.hpp"

PakO::PakO(GenBuffer* aBufferPtr)
   : mBufferPtr(aBufferPtr)
{
}

// virtual
//! Serializes a fixed-sized string.
//! @param aString The text to serialize
//! @param aBytes The number of bytes the string will fill.
void PakO::SerializeString(std::string& aString, int aBytes)
{
   //_byteswap_ulong()
   int len = static_cast<int>(aString.length());
   if (len < aBytes)
   {
      mBufferPtr->PutRaw(aString.c_str(), len);
      while (len < aBytes)
      {
         mBufferPtr->Put('\0');
         ++len;
      }
   }
   else
   {
      mBufferPtr->PutRaw(aString.c_str(), aBytes - 1);
      mBufferPtr->Put('\0');
   }
}
