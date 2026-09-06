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

#include "DisVariableDatum.hpp"

#include <cstring> // for memcpy

#include "DisClockTime.hpp"
#include "DisDatumEnums.hpp"
#include "GenI.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenO.hpp"
#include "GenOConvertBigEndian.hpp"
#include "math.h"

DisVariableDatum::DisVariableDatum()
   : mId(0)
   , mDataLength(0)
   , mDataPtr(nullptr)
{
}

DisVariableDatum::DisVariableDatum(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisVariableDatum::DisVariableDatum(DisEnum32 aId, const unsigned char* aDataPtr, DisUint32 aDataLength)
   : mId(aId)
   , mDataPtr(nullptr)
{
   SetData(aDataPtr, aDataLength);
}

DisVariableDatum::DisVariableDatum(const DisVariableDatum& aRhs)
   : mDataPtr(nullptr)
{
   Copy(aRhs);
}

DisVariableDatum::~DisVariableDatum()
{
   DeleteMemory();
}

DisVariableDatum::DisVariableDatum(DisEnum32 aId, const DisClockTime& aDisClockTime)
   : mId(aId)
   , mDataLength(0)
   , mDataPtr(nullptr)
{
   const unsigned int   offsetBytes = sizeof(DisUint32);
   const unsigned int   totalBytes  = 2 * offsetBytes;
   const unsigned int   totalBits   = 8 * totalBytes;
   unsigned char*       ptr         = new unsigned char[totalBytes];
   GenOConvertBigEndian converter;
   DisUint32            a = aDisClockTime.GetHour();
   DisUint32            b = aDisClockTime.GetTimePastHour();
   converter.PutToBuf(&(ptr[0]), a);
   converter.PutToBuf(&(ptr[offsetBytes]), b);
   SetDataPtr(ptr, totalBits);
}

DisVariableDatum& DisVariableDatum::operator=(const DisVariableDatum& aRhs)
{
   if (this != &aRhs)
   {
      Copy(aRhs);
   }
   return *this;
}

void DisVariableDatum::Stream(std::ostream& aStream) const
{
   GenIConvertBigEndian converter;

   DisEnum32 id = GetId();
   // unsigned int byteLength = GetLength();
   const unsigned char* bufPtr;
   unsigned int         dataBits;
   GetData(bufPtr, dataBits);
   if (dataBits > 0)
   {
      std::string  dataAsString;
      unsigned int byteLength = (dataBits + 7) / 8;
      for (unsigned int i = 0; i != byteLength; ++i)
      {
         dataAsString.push_back(bufPtr[i]);
      }
      aStream << "            => DisUint32 = " << id << ", as string(?) = " << dataAsString << std::endl;
   }
   else
   {
      aStream << "            => DisUint32 = " << id << ", string is NULL" << std::endl;
   }
}

void DisVariableDatum::GetClockTimeFromBuffer(DisClockTime& aDisClockTime) const
{
   const unsigned int   offsetBytes = sizeof(DisUint32);
   GenIConvertBigEndian converter;
   DisUint32            value;
   converter.GetFromBuf(&(mDataPtr[0]), value);
   aDisClockTime.SetHour(value);
   converter.GetFromBuf(&(mDataPtr[offsetBytes]), value);
   aDisClockTime.SetTimePastHour(value);
}

void DisVariableDatum::SetData(const unsigned char* aDataPtr, DisUint32 aDataLengthInBits)
{
   unsigned int numBytes = PrepareMemory(aDataLengthInBits);
   if (numBytes)
   {
      memcpy(mDataPtr, aDataPtr, numBytes);
   }
}

void DisVariableDatum::SetDataPtr(unsigned char* aDataPtr, DisUint32 aDataLength)
{
   DeleteMemory();
   mDataLength = aDataLength;
   mDataPtr    = aDataLength ? aDataPtr : nullptr;
}

void DisVariableDatum::Get(GenI& aGenI)
{
   DeleteMemory();
   aGenI >> mId;
   aGenI >> mDataLength;

   if (mDataLength > 0)
   {
      unsigned int numBytes = static_cast<unsigned int>((mDataLength + 63) / 64) * 8;
      mDataPtr              = new unsigned char[numBytes];
      aGenI.GetArray(mDataPtr, numBytes);
   }
   else
   {
      mDataPtr = nullptr;
   }
}

void DisVariableDatum::Put(GenO& aGenO) const
{
   aGenO << mId;
   aGenO << mDataLength;

   if (mDataLength > 0)
   {
      // Write everything, but the last byte
      unsigned int numBytes = (mDataLength + 7) / 8 - 1;
      if (numBytes > 0)
      {
         aGenO.PutArray(mDataPtr, numBytes);
      }

      // Be sure the bits past the length are set to zero.
      // For big endian thinking
      // mDataPtr[numBytes] &= static_cast<unsigned char>(0xFF) << (mDataLength & 7 ? 8 - (mDataLength & 7) : 0);
      // For little endian thinking
      mDataPtr[numBytes] &= ~(static_cast<unsigned char>(0xFF) << ((mDataLength & 7) ? (mDataLength & 7) : 8));
      aGenO << mDataPtr[numBytes];

      // Write out the byte pads
      ++numBytes;
      while (numBytes & 0x7)
      {
         unsigned char pad = 0;
         aGenO << pad;
         ++numBytes;
      }
   }
}

//
// Private
//

void DisVariableDatum::Copy(const DisVariableDatum& aRhs)
{
   mId = aRhs.mId;

   unsigned int numBytes = PrepareMemory(aRhs.mDataLength);
   if (numBytes)
   {
      memcpy(mDataPtr, aRhs.mDataPtr, numBytes);
   }
}

unsigned int DisVariableDatum::PrepareMemory(DisUint32 aDataLength)
{
   DeleteMemory();
   mDataLength = aDataLength;

   unsigned int numBytes;
   if (aDataLength > 0)
   {
      numBytes = (aDataLength + 7) / 8;
      mDataPtr = new unsigned char[numBytes];
   }
   else
   {
      numBytes = 0;
      mDataPtr = nullptr;
   }
   return numBytes;
}

void DisVariableDatum::GetMemberData(GenI& aGenI)
{
   DeleteMemory();
   aGenI >> mId;
   aGenI >> mDataLength;

   if (mDataLength > 0)
   {
      unsigned int numBytes = static_cast<unsigned int>((mDataLength + 63) / 64) * 8;
      mDataPtr              = new unsigned char[numBytes];
      aGenI.GetArray(mDataPtr, numBytes);
   }
   else
   {
      mDataPtr = nullptr;
   }
}

bool DisVariableDatum::DeleteMemory()
{
   if (mDataLength > 0)
   {
      delete[] mDataPtr;
      mDataLength = 0;
      return true;
   }
   return false;
}

std::string DisVariableDatum::ToString() const
{
   std::string returnString = "-------Variable Datum Record-------\n";
   returnString += "Id:          " + std::to_string(mId) + "\n";
   returnString += "Data Length: " + std::to_string(mDataLength) + "\n";
   returnString += "-----End Variable Datum Record-----\n";
   return returnString;
}

bool DisVariableDatum::IsValid() const
{
   return true;
}

void DisVariableDatum::SetDatumString(const std::string& aString)
{
   SetData((unsigned char*)(aString.c_str()), static_cast<DisUint32>(aString.length() * 8));
}
