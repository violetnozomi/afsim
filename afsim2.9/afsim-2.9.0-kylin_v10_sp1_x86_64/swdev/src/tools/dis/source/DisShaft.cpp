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

#include "DisShaft.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisShaft::DisShaft()
   : mDataLength(8)
   , // 64 bits / 8 bits = 8 bytes
   mCurrentRPM(0)
   , mOrderedRPM(0)
   , mRate(0.0)
{
}

DisShaft::~DisShaft() {}

DisUint8 DisShaft::GetDataLength()
{
   GetLength();
   return mDataLength;
}

DisUint16 DisShaft::GetLength()
{
   DisUint16 octetCount = 8; // 64 bits
   mDataLength          = 2; // length in 32 bit words
   return octetCount;
}

void DisShaft::Get(GenI& aGenI)
{
   aGenI >> mCurrentRPM;
   aGenI >> mOrderedRPM;
   aGenI >> mRate;
   mLengthRead = 8; // length of base

   // Read and ignore any 'extra' data...

   // int extraOctets = (mDataLength * 4) - mLengthRead;
   DisUint16 extraOctets = static_cast<DisUint16>(GetDataLength()) * 4 - mLengthRead;
   mLengthRead += extraOctets;
   while (extraOctets > 0)
   {
      DisUint8 tempOctet;
      aGenI >> tempOctet;
      --extraOctets;
   }
}

void DisShaft::Put(GenO& aGenO)
{
   aGenO << mCurrentRPM;
   aGenO << mOrderedRPM;
   aGenO << mRate;
}

bool DisShaft::IsValid() const
{
   bool isValid = true;
   if (!ValidateScalar(mRate))
      isValid = false;
   return isValid;
}
