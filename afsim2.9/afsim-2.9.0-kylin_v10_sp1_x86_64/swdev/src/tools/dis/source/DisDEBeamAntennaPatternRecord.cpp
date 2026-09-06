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

#include "DisDEBeamAntennaPatternRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDEBeamAntennaPatternRecord::DisDEBeamAntennaPatternRecord()
   : mAzimuthBeamwidth(0.0)
   , mElevationBeamwidth(0.0)
   , mReferenceSystem(0)
   , mPadding1(0)
   , mPadding2(0)
   , mEz(0.0)
   , mEx(0.0)
   , mPhase(0.0)
   , mPadding3(0)
{
   mDirection[0] = 0.0;
   mDirection[1] = 0.0;
   mDirection[2] = 0.0;
}

DisDEBeamAntennaPatternRecord::DisDEBeamAntennaPatternRecord(GenI& aGenI)
   : mAzimuthBeamwidth(0.0)
   , mElevationBeamwidth(0.0)
   , mReferenceSystem(0)
   , mPadding1(0)
   , mPadding2(0)
   , mEz(0.0)
   , mEx(0.0)
   , mPhase(0.0)
   , mPadding3(0)
{
   mDirection[0] = 0.0;
   mDirection[1] = 0.0;
   mDirection[2] = 0.0;

   Get(aGenI);
}

DisDEBeamAntennaPatternRecord::~DisDEBeamAntennaPatternRecord() {}

void DisDEBeamAntennaPatternRecord::Put(GenO& aGenO) const
{
   aGenO << mDirection[0] << mDirection[1] << mDirection[2];
   aGenO << mAzimuthBeamwidth;
   aGenO << mElevationBeamwidth;
   aGenO << mReferenceSystem;
   aGenO << mPadding1;
   aGenO << mPadding2;
   aGenO << mEz;
   aGenO << mEx;
   aGenO << mPhase;
   aGenO << mPadding3;
}

void DisDEBeamAntennaPatternRecord::Get(GenI& aGenI)
{
   aGenI >> mDirection[0] >> mDirection[1] >> mDirection[2];
   aGenI >> mAzimuthBeamwidth;
   aGenI >> mElevationBeamwidth;
   aGenI >> mReferenceSystem;
   aGenI >> mPadding1;
   aGenI >> mPadding2;
   aGenI >> mEz;
   aGenI >> mEx;
   aGenI >> mPhase;
   aGenI >> mPadding3;
}

bool DisDEBeamAntennaPatternRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mDirection[0]))
      isValid = false;
   if (!ValidateScalar(mDirection[1]))
      isValid = false;
   if (!ValidateScalar(mDirection[2]))
      isValid = false;
   if (!ValidateScalar(mAzimuthBeamwidth))
      isValid = false;
   if (!ValidateScalar(mElevationBeamwidth))
      isValid = false;
   if (!ValidateScalar(mEz))
      isValid = false;
   if (!ValidateScalar(mEx))
      isValid = false;
   if (!ValidateScalar(mPhase))
      isValid = false;
   return isValid;
}
