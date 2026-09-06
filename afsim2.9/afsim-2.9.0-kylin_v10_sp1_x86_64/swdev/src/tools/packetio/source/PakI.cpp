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

#include "PakI.hpp"

PakI::PakI(GenBuffer* aBufferPtr)
   : mBufferPtr(aBufferPtr)
{
}

// virtual
//! Serializes a fixed-sized string.
//! @param aString The text to serialize
//! @param aBytes The number of bytes the string will fill.
void PakI::SerializeString(std::string& aString, int aBytes)
{
   aString.clear();
   static const int cBUFF_SIZE = 1025;
   char             buffer[cBUFF_SIZE];
   int              length = aBytes;
   while (length > 0)
   {
      int readLength = cBUFF_SIZE - 1;
      if (length < readLength)
      {
         readLength = length;
      }
      mBufferPtr->GetRaw(buffer, readLength);
      buffer[readLength] = 0;
      aString += buffer;
      length -= readLength;
   }
}
