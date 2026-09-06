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

#include "DisAngleDeceptionAttributeRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisAngleDeceptionAttributeRecord::DisAngleDeceptionAttributeRecord()
   : DisAttributeRecord(ANGLE_DECEPTION_ATTRIBUTE_RECORD_TYPE)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mPadding1(0)
   , mAzimuthOffsetRadians(0.0)
   , mAzimuthWidthRadians(0.0)
   , mAzimuthPullRateRadiansPerSecond(0.0)
   , mAzimuthPullAccelerationRadiansPerSecondSquared(0.0)
   , mElevationOffsetRadians(0.0)
   , mElevationWidthRadians(0.0)
   , mElevationPullRateRadiansPerSecond(0.0)
   , mElevationPullAccelerationRadiansPerSecondSquared(0.0)
   , mPadding2(0)
{
}

DisAngleDeceptionAttributeRecord::DisAngleDeceptionAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI)
   : DisAttributeRecord(aAttributeRecord)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mPadding1(0)
   , mAzimuthOffsetRadians(0.0)
   , mAzimuthWidthRadians(0.0)
   , mAzimuthPullRateRadiansPerSecond(0.0)
   , mAzimuthPullAccelerationRadiansPerSecondSquared(0.0)
   , mElevationOffsetRadians(0.0)
   , mElevationWidthRadians(0.0)
   , mElevationPullRateRadiansPerSecond(0.0)
   , mElevationPullAccelerationRadiansPerSecondSquared(0.0)
   , mPadding2(0)
{
   GetDerivedClass(aGenI);
}

// virtual
DisAngleDeceptionAttributeRecord::~DisAngleDeceptionAttributeRecord() {}

// virtual
DisUint16 DisAngleDeceptionAttributeRecord::GetDerivedClassLength()
{
   return 40; // 320 bits = Total Angle Deception attribute record (384 bits) - Header size (64 bits)
}

// virtual
void DisAngleDeceptionAttributeRecord::GetDerivedClass(GenI& aGenI)
{
   aGenI >> mEmitterNumber;
   aGenI >> mBeamNumber;
   aGenI >> mStateIndicator;
   aGenI >> mPadding1;
   aGenI >> mAzimuthOffsetRadians;
   aGenI >> mAzimuthWidthRadians;
   aGenI >> mAzimuthPullRateRadiansPerSecond;
   aGenI >> mAzimuthPullAccelerationRadiansPerSecondSquared;
   aGenI >> mElevationOffsetRadians;
   aGenI >> mElevationWidthRadians;
   aGenI >> mElevationPullRateRadiansPerSecond;
   aGenI >> mElevationPullAccelerationRadiansPerSecondSquared;
   aGenI >> mPadding2;
}

// virtual
void DisAngleDeceptionAttributeRecord::PutDerivedClass(GenO& aGenO) const
{
   aGenO << mEmitterNumber;
   aGenO << mBeamNumber;
   aGenO << mStateIndicator;
   aGenO << mPadding1;
   aGenO << mAzimuthOffsetRadians;
   aGenO << mAzimuthWidthRadians;
   aGenO << mAzimuthPullRateRadiansPerSecond;
   aGenO << mAzimuthPullAccelerationRadiansPerSecondSquared;
   aGenO << mElevationOffsetRadians;
   aGenO << mElevationWidthRadians;
   aGenO << mElevationPullRateRadiansPerSecond;
   aGenO << mElevationPullAccelerationRadiansPerSecondSquared;
   aGenO << mPadding2;
}
bool DisAngleDeceptionAttributeRecord::IsValid() const
{
   bool isValid = true;
   // Validate data
   if (!ValidateScalar(mAzimuthOffsetRadians))
      isValid = false;
   if (!ValidateScalar(mAzimuthWidthRadians))
      isValid = false;
   if (!ValidateScalar(mAzimuthPullRateRadiansPerSecond))
      isValid = false;
   if (!ValidateScalar(mAzimuthPullAccelerationRadiansPerSecondSquared))
      isValid = false;
   if (!ValidateScalar(mElevationOffsetRadians))
      isValid = false;
   if (!ValidateScalar(mElevationWidthRadians))
      isValid = false;
   if (!ValidateScalar(mElevationPullRateRadiansPerSecond))
      isValid = false;
   if (!ValidateScalar(mElevationPullAccelerationRadiansPerSecondSquared))
      isValid = false;
   return isValid;
}
