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


#include "DisUABeam.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisUABeam::DisUABeam()
   : mDataLength(24)
   , // 192 bits / 8 bits = 24 bytes
   mBeamId(0)
   , mIndex(0)
   , mScanPattern(0)
   , mAzimuthCenter(0.0F)
   , mAzimuthSweep(0.0F)
   , mElevationCenter(0.0F)
   , mElevationSweep(0.0F)
   , mLengthRead(0)
{
}

DisUABeam::~DisUABeam()
{
   // The destructor for "mTrackJamList" will take of itself and the
   // pointers it owns.
}

DisUint8 DisUABeam::GetDataLength()
{
   GetLength(); // Force length re-calculation
   return mDataLength;
}

DisUint16 DisUABeam::GetLength()
{
   // 192 bits
   DisUint16 octetCount = 24;
   mDataLength          = 6; // in 32 bit words...
   return octetCount;
}

void DisUABeam::Get(GenI& aGenI)
{
   DisUint16 pad16;

   aGenI >> mDataLength;
   aGenI >> mBeamId;
   aGenI >> pad16;
   aGenI >> mIndex;
   aGenI >> mScanPattern;
   aGenI >> mAzimuthCenter;
   aGenI >> mAzimuthSweep;
   aGenI >> mElevationCenter;
   aGenI >> mElevationSweep;
   mLengthRead = 24;

   // Read and ignore any 'extra' data...

   int extraOctets = (mDataLength * 4) - mLengthRead;
   mLengthRead += extraOctets;
   while (extraOctets > 0)
   {
      DisUint8 tempOctet;
      aGenI >> tempOctet;
      --extraOctets;
   }
}

void DisUABeam::Put(GenO& aGenO)
{
   DisUint16 pad16(0);

   // mDataLength and mTrackJamCount *should* be accurate because GetLength()
   // *should* have been called to determine the length for outputting
   // the Emission PDU.

   aGenO << mDataLength;
   aGenO << mBeamId;
   aGenO << pad16;
   aGenO << mIndex;
   aGenO << mScanPattern;
   aGenO << mAzimuthCenter;
   aGenO << mAzimuthSweep;
   aGenO << mElevationCenter;
   aGenO << mElevationSweep;
}

bool DisUABeam::IsValid() const
{
   bool isValid = true;
   if (!ValidateScalar(mAzimuthCenter))
      isValid = false;
   if (!ValidateScalar(mAzimuthSweep))
      isValid = false;
   if (!ValidateScalar(mElevationCenter))
      isValid = false;
   if (!ValidateScalar(mElevationSweep))
      isValid = false;
   return isValid;
}
