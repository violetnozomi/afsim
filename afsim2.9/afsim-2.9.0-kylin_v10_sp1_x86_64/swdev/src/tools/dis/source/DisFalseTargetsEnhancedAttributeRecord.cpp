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

#include "DisFalseTargetsEnhancedAttributeRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisFalseTargetsEnhancedAttributeRecord::DisFalseTargetsEnhancedAttributeRecord()
   : DisAttributeRecord(FALSE_TARGETS_ATTRIBUTE_RECORD_TYPE)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mShowRealTargetReturn(0)
   , mFalseTargetCount(0.0)
   , mWalkSpeedMetersPerSecond(0.0)
   , mWalkAccelerationMetersPerSecondSquared(0.0)
   , mMaximumWalkDistanceMeters(0.0)
   , mKeepTimeSeconds(0.0)
   , mEchoSpacingMeters(0.0)
   , mInitialOffsetMeters(0)
{
}

DisFalseTargetsEnhancedAttributeRecord::DisFalseTargetsEnhancedAttributeRecord(const DisAttributeRecord& aAttributeRecord,
                                                                               GenI&                     aGenI)
   : DisAttributeRecord(aAttributeRecord)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mShowRealTargetReturn(0)
   , mFalseTargetCount(0.0)
   , mWalkSpeedMetersPerSecond(0.0)
   , mWalkAccelerationMetersPerSecondSquared(0.0)
   , mMaximumWalkDistanceMeters(0.0)
   , mKeepTimeSeconds(0.0)
   , mEchoSpacingMeters(0.0)
   , mInitialOffsetMeters(0.0)
{
   GetDerivedClass(aGenI);
}

// virtual
DisFalseTargetsEnhancedAttributeRecord::~DisFalseTargetsEnhancedAttributeRecord() {}

// virtual
DisUint16 DisFalseTargetsEnhancedAttributeRecord::GetDerivedClassLength()
{
   return 32; // 256 bits = Total False Targets attribute record (320 bits) - Header size (64 bits)
}

// virtual
void DisFalseTargetsEnhancedAttributeRecord::GetDerivedClass(GenI& aGenI)
{
   aGenI >> mEmitterNumber;
   aGenI >> mBeamNumber;
   aGenI >> mStateIndicator;
   aGenI >> mShowRealTargetReturn;
   aGenI >> mFalseTargetCount;
   aGenI >> mWalkSpeedMetersPerSecond;
   aGenI >> mWalkAccelerationMetersPerSecondSquared;
   aGenI >> mMaximumWalkDistanceMeters;
   aGenI >> mKeepTimeSeconds;
   aGenI >> mEchoSpacingMeters;
   aGenI >> mInitialOffsetMeters;
}

// virtual
void DisFalseTargetsEnhancedAttributeRecord::PutDerivedClass(GenO& aGenO) const
{
   aGenO << mEmitterNumber;
   aGenO << mBeamNumber;
   aGenO << mStateIndicator;
   aGenO << mShowRealTargetReturn;
   aGenO << mFalseTargetCount;
   aGenO << mWalkSpeedMetersPerSecond;
   aGenO << mWalkAccelerationMetersPerSecondSquared;
   aGenO << mMaximumWalkDistanceMeters;
   aGenO << mKeepTimeSeconds;
   aGenO << mEchoSpacingMeters;
   aGenO << mInitialOffsetMeters;
}

bool DisFalseTargetsEnhancedAttributeRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mFalseTargetCount))
      isValid = false;
   if (!ValidateScalar(mWalkSpeedMetersPerSecond))
      isValid = false;
   if (!ValidateScalar(mWalkAccelerationMetersPerSecondSquared))
      isValid = false;
   if (!ValidateScalar(mMaximumWalkDistanceMeters))
      isValid = false;
   if (!ValidateScalar(mKeepTimeSeconds))
      isValid = false;
   if (!ValidateScalar(mEchoSpacingMeters))
      isValid = false;
   return isValid;
}
