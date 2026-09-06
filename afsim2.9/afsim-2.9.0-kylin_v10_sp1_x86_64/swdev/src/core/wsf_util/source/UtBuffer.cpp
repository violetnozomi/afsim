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

#include "UtBuffer.hpp"

//! Enables translation of calls to Put() and Get() to/from big endian
void UtBuffer::SetBigEndian()
{
   int  lsbSet         = 1;
   bool isLittleEndian = ((bool*)&lsbSet)[0];
   EnableByteSwap(isLittleEndian);
}

void UtBuffer::SetLittleEndian()
{
   int  lsbSet         = 1;
   bool isLittleEndian = ((bool*)&lsbSet)[0];
   EnableByteSwap(!isLittleEndian);
}

//! Configures Put() and Get() to use native byte ordering
void UtBuffer::SetNativeByteOrder()
{
   EnableByteSwap(false);
}

//! Enlarges the buffer to a new size
void UtBuffer::GrowBy(size_t aBytes)
{
   size_t newSize = aBytes + mBytes + 1;
   // Always grow at minimum by 50%
   if ((aBytes + 1) < (mBytes / 2))
   {
      newSize = mBytes + mBytes / 2;
   }
   char* newData = new char[newSize];
   std::copy(mBuffer, mBuffer + mBytes, newData);
   delete[] mBuffer;
   mBuffer = newData;
   mBytes  = newSize;
}

//! Move a set of bytes within the buffer
void UtBuffer::Move(size_t aBeginOffset, size_t aEndOffset, size_t aNewBegin)
{
   memmove(mBuffer + aNewBegin, mBuffer + aBeginOffset, aEndOffset - aBeginOffset);
}
