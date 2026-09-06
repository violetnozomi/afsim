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

#include "DisBlankingSectorAttributeRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisBlankingSectorAttributeRecord::DisBlankingSectorAttributeRecord()
   : DisAttributeRecord(BLANKING_SECTOR_ATTRIBUTE_RECORD_TYPE)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mPadding1(0)
   , mLeftAzimuthRadians(0.0)
   , mRightAzimuthRadians(0.0)
   , mLowerElevationRadians(0.0)
   , mUpperElevationRadians(0.0)
   , mResidualPowerDBm(0.0)
   , mPadding2(0)
{
}

DisBlankingSectorAttributeRecord::DisBlankingSectorAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI)
   : DisAttributeRecord(aAttributeRecord)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mPadding1(0)
   , mLeftAzimuthRadians(0.0)
   , mRightAzimuthRadians(0.0)
   , mLowerElevationRadians(0.0)
   , mUpperElevationRadians(0.0)
   , mResidualPowerDBm(0.0)
   , mPadding2(0)
{
   GetDerivedClass(aGenI);
}

// virtual
DisBlankingSectorAttributeRecord::~DisBlankingSectorAttributeRecord() {}

// virtual
DisUint16 DisBlankingSectorAttributeRecord::GetDerivedClassLength()
{
   return 32; // 256 bits = Total Blanking Sector attribute record (320 bits) - Header size (64 bits)
}

// virtual
void DisBlankingSectorAttributeRecord::GetDerivedClass(GenI& aGenI)
{
   aGenI >> mEmitterNumber;
   aGenI >> mBeamNumber;
   aGenI >> mStateIndicator;
   aGenI >> mPadding1;
   aGenI >> mLeftAzimuthRadians;
   aGenI >> mRightAzimuthRadians;
   aGenI >> mLowerElevationRadians;
   aGenI >> mUpperElevationRadians;
   aGenI >> mResidualPowerDBm;
   aGenI >> mPadding2;
}

// virtual
void DisBlankingSectorAttributeRecord::PutDerivedClass(GenO& aGenO) const
{
   aGenO << mEmitterNumber;
   aGenO << mBeamNumber;
   aGenO << mStateIndicator;
   aGenO << mPadding1;
   aGenO << mLeftAzimuthRadians;
   aGenO << mRightAzimuthRadians;
   aGenO << mLowerElevationRadians;
   aGenO << mUpperElevationRadians;
   aGenO << mResidualPowerDBm;
   aGenO << mPadding2;
}

bool DisBlankingSectorAttributeRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mLeftAzimuthRadians))
      isValid = false;
   if (!ValidateScalar(mRightAzimuthRadians))
      isValid = false;
   if (!ValidateScalar(mLowerElevationRadians))
      isValid = false;
   if (!ValidateScalar(mUpperElevationRadians))
      isValid = false;
   if (!ValidateScalar(mResidualPowerDBm))
      isValid = false;
   return isValid;
}
