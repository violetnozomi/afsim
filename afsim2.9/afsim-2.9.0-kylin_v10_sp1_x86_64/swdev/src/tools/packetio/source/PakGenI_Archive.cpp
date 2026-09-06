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

#include "PakGenI_Archive.hpp"

#include "PakTypeDictionary.hpp"

PakGenI_Archive::PakGenI_Archive(GenI& aGenIPtr)
{
   mGenIPtr = &aGenIPtr;
}

// virtual
//! Serializes a buffer without byte-swapping.
void PakGenI_Archive::SerializeBuffer(char* aBuffer, int aSize)
{
   mGenIPtr->GetArray(aBuffer, aSize);
}

// virtual
//! Serializes a fixed-sized string.
//! @param aString The text to serialize
//! @param aBytes The number of bytes the string will fill.
void PakGenI_Archive::SerializeString(std::string& aString, int aBytes)
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
      mGenIPtr->GetArray(buffer, readLength);
      buffer[readLength] = 0;
      aString += buffer;
      length -= readLength;
   }
}

//! Ignores a number of bytes from the GenI stream
void PakGenI_Archive::IgnoreBytes(int aNumBytes)
{
   if (!aNumBytes)
      return;
   mGenIPtr->SetGetOffset(aNumBytes, GenBuf::FromCur);
}
