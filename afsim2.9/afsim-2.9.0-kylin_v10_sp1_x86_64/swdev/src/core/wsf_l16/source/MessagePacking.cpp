// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "MessagePacking.hpp"

#include <algorithm>
#include <cstring>

#include "GenIODefs.hpp"

#ifdef GENIO_LIT_ENDIAN
const bool IsChipLittleEndian = true;
#else
const bool IsChipLittleEndian = false;
#endif

namespace
{
void ByteSwap(unsigned int& aSwap)
{
   std::swap(reinterpret_cast<char*>(&aSwap)[0], reinterpret_cast<char*>(&aSwap)[3]);
   std::swap(reinterpret_cast<char*>(&aSwap)[1], reinterpret_cast<char*>(&aSwap)[2]);
}
} // namespace

// pack a maximum of 32 bits (one integer) into a buffer of shorts
void WsfL16::Messages::Pack(char* aBuf, unsigned int aData, int start, int stop)
{
   typedef unsigned int uint;
   uint                 lDataCopy[2];

   int startIndex = start / 32;
   int endIndex   = stop / 32;
   int startBit   = start % 32;
   int endBit     = stop % 32;

   // Copy shorts to 4 byte boundry so this will work on SGI!
   uint lBuf[2];
   memcpy(lBuf, ((int*)aBuf) + startIndex, sizeof(int) * 2);
   if (!IsChipLittleEndian)
   {
      ByteSwap(lBuf[0]);
      ByteSwap(lBuf[1]);
   }
   // uint  lData=(uint)aData;
   uint lFullBits = 0xffffffff;

   // first allign the input data
   if (startBit)
   {
      // need to shift data
      lDataCopy[0] = (aData << startBit);
      lDataCopy[1] = (aData) >> (32 - startBit);
   }
   else
   {
      // dont need to shift
      lDataCopy[0] = aData;
      lDataCopy[1] = 0; // Quiet the compiler.  This isn't used
   }
   // now copy it into the buffer

   if (startIndex == endIndex)
   {
      // note: unfortunately, you cannot shift by 32, so I have to shift to what I don't
      //       want and then (~) to avoid using more if statements
      uint mask = 0;
      mask      = (lFullBits << (startBit)) & (lFullBits >> (31 - endBit));
      lBuf[0]   = (mask & (lDataCopy[0])) | ((~mask) & lBuf[0]);
   }
   else
   {
      // do first int
      lBuf[0] = (lBuf[0] & (~(lFullBits << startBit))) | lDataCopy[0];
      // do last int
      lBuf[1] = (lBuf[1] & (~(lFullBits >> (32 - 1 - endBit)))) | lDataCopy[1];
   }
   if (!IsChipLittleEndian)
   {
      ByteSwap(lBuf[0]);
      ByteSwap(lBuf[1]);
   }
   memcpy(((int*)aBuf) + startIndex, lBuf, sizeof(int) * 2);
}

// unpacks an integer from an array.  If the number of bits unpacked is less
// than 32, the remaining bits are set to zero
void WsfL16::Messages::Unpack(const char* aBuf, unsigned int& aData, int start, int stop)
{
   typedef unsigned int uint;
   int                  startIndex = start / 32;
   int                  startBit   = start % 32;
   int                  bitLen     = stop - start + 1;
   // Copy shorts to 4 byte boundry so this will work on SGI!
   uint lBuf[2];
   memcpy(lBuf, ((int*)aBuf) + startIndex, sizeof(int) * 2);
   if (!IsChipLittleEndian)
   {
      ByteSwap(lBuf[0]);
      ByteSwap(lBuf[1]);
   }
   if (startBit)
   {
      // need to shift data
      aData = (lBuf[0] >> startBit) | (lBuf[1] << (32 - startBit));
   }
   else
   {
      // dont need to shift
      aData = lBuf[0]; // copy directly from buf
   }
   aData &= ~(0xffffffff << bitLen); // select only the bits we want
}
