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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisSpecRecord.hpp"

#include <cassert>
#include <cstring> // for memcpy

#include "GenI.hpp"
#include "GenO.hpp"

DisUint32 DisSpecRecord::sSerialNumber = 0;

DisSpecRecord::DisSpecRecord()
   : mId(0)
   , mSerialNumber(0)
   , mLengthBits(0)
   , mCount(0)
   , mSizeBytes(0)
   , mValues(nullptr)
{
}

DisSpecRecord::DisSpecRecord(const DisSpecRecord& aSrc)
   : DisSpecRecord()
{
   Copy(aSrc);
}

DisSpecRecord::~DisSpecRecord()
{
   delete[] mValues;
}

void DisSpecRecord::GetValues(GenO& aGenO) const
{
   aGenO.PutArray(mValues, mSizeBytes);
}

void DisSpecRecord::SetValues(DisUint16 aRecLengthBytes, DisUint16 aRecCount, GenI& aGenI)
{
   mLengthBits = aRecLengthBytes * 8;
   mCount      = aRecCount;
   mSizeBytes  = aRecCount * aRecLengthBytes;

   // Get rid of the old values
   delete[] mValues;

   // Allocate space for the values
   unsigned int numDoubles = (mSizeBytes + 7) / 8;
   mValues                 = reinterpret_cast<unsigned char*>(new double[numDoubles]);
   assert(mValues);

   // Take the values from the user supplied buffer and store in this class
   aGenI.GetArray(mValues, mSizeBytes);
}

void DisSpecRecord::Get(GenI& aGenI)
{
   DisUint32 padding;

   aGenI >> mId;
   aGenI >> mSerialNumber;
   aGenI >> padding;
   aGenI >> mLengthBits;
   aGenI >> mCount;

   mSizeBytes = (mLengthBits + 7) / 8 * mCount;

   // Get rid of the old values
   delete[] mValues;

   // Allocate space for the values
   unsigned int numDoubles = (mSizeBytes + 7) / 8;
   mValues                 = reinterpret_cast<unsigned char*>(new double[numDoubles]);
   assert(mValues);

   // Get values, but don't convert to the right type here
   aGenI.GetArray(mValues, mSizeBytes);

   // Get any padding that may be in the buffer
   unsigned int numBytes = mSizeBytes & 0x7; // Logically the same as (mSize % 8)
   if (numBytes)
   {
      numBytes = 8 - numBytes;
      for (unsigned int j = 0; j < numBytes; ++j)
      {
         unsigned char dummy;
         aGenI.Get(dummy);
      }
   }
}

DisUint16 DisSpecRecord::GetLengthOfSet() const
{
   // total size 16: RecordId(4), Serial Number(4), Padding(4), RecordLength(2), RecordCount(2)
   DisUint16 numBytes = 16 + mSizeBytes;
   // Add padding to make record a multiple of 4 bytes
   numBytes = ((numBytes + 3) / 4) * 4;
   return numBytes;
}

void DisSpecRecord::Put(GenO& aGenO) const
{
   DisUint32 padding = 0;

   aGenO << mId;
   aGenO << mSerialNumber;
   aGenO << padding;
   aGenO << mLengthBits;
   aGenO << mCount;

   // Put values, but don't convert to the right type here
   aGenO.PutArray(mValues, mSizeBytes);

   // Write out any padding that may be needed to get on a 64-bit boundary
   unsigned int numBytes = mSizeBytes & 0x7; // Logically the same as (mSize % 8)
   if (numBytes)
   {
      numBytes = 8 - numBytes;
      for (unsigned int j = 0; j < numBytes; ++j)
      {
         unsigned char dummy = 0;
         aGenO.Put(dummy);
      }
   }
}

DisSpecRecord& DisSpecRecord::operator=(const DisSpecRecord& aRhs)
{
   if (this != &aRhs)
   {
      Copy(aRhs);
   }
   return *this;
}

bool DisSpecRecord::operator<(const DisSpecRecord& aRhs) const
{
   if (mId < aRhs.mId)
   {
      return true;
   }

   if (mId > aRhs.mId)
   {
      return false;
   }

   if (mSerialNumber < aRhs.mSerialNumber)
   {
      return true;
   }
   else
   {
      return false;
   }
}

//
// Private Functions
//

void DisSpecRecord::Copy(const DisSpecRecord& aSrc)
{
   mId           = aSrc.mId;
   mSerialNumber = aSrc.mSerialNumber;
   mLengthBits   = aSrc.mLengthBits;
   mCount        = aSrc.mCount;
   mSizeBytes    = aSrc.mSizeBytes;
   delete[] mValues;

   if (mSizeBytes)
   {
      mValues = new DisEnum8[mSizeBytes];
      memcpy(mValues, aSrc.mValues, mSizeBytes);
   }
   else
   {
      mValues = nullptr;
   }
}

void DisSpecRecord::Stream(std::ostream& aStream) const
{
   aStream << " Spec Record Id = " << mId << std::endl;
   aStream << "        Serial# = " << mSerialNumber << std::endl;
   aStream << "         Length = " << mLengthBits << " bits" << std::endl;
   aStream << "          Count = " << mCount << std::endl;
   aStream << "           Size = " << mSizeBytes << " bytes" << std::endl;
}

void DisSpecRecord::AssignNewSerialNumber()
{
   mSerialNumber = ++sSerialNumber;
}

bool DisSpecRecord::IsValid() const
{
   return true;
}
