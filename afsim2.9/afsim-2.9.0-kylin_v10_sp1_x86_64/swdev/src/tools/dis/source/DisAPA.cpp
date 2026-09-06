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

#include "DisAPA.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisAPA::DisAPA()
   : mDataLength(4)
   , // 32 bits / 8 bits = 4 bytes
   mIndex(0)
   , mValue(0)
{
}

DisAPA::~DisAPA() {}

DisUint8 DisAPA::GetDataLength()
{
   GetLength();
   return mDataLength;
}

DisUint16 DisAPA::GetLength()
{
   static const DisUint16 octetCount = 4; // 32 bits
   mDataLength                       = 1; // length in 32 bit words
   return octetCount;
}

void DisAPA::Get(GenI& aGenI)
{
   aGenI >> mIndex;
   aGenI >> mValue;
   mLengthRead = 4; // length of base

   // Read and ignore any 'extra' data...

   // int extraOctets = (mDataLength * 4) - mLengthRead;
   DisUint16 extraOctets = (GetDataLength() * 4) - mLengthRead;
   mLengthRead += extraOctets;
   while (extraOctets > 0)
   {
      DisUint8 tempOctet;
      aGenI >> tempOctet;
      --extraOctets;
   }
}

void DisAPA::Put(GenO& aGenO)
{
   aGenO << mIndex;
   aGenO << mValue;
}

bool DisAPA::IsValid() const
{
   return true;
}
