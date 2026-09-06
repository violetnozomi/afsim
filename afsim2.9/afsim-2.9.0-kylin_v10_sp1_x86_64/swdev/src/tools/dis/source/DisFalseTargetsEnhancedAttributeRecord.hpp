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

// DIS False Targets Attribute Record

#ifndef DIS_FALSE_TARGETS_ENHANCED_ATTRIBUTE_RECORD_HPP
#define DIS_FALSE_TARGETS_ENHANCED_ATTRIBUTE_RECORD_HPP

#include "dis_export.h"

#include "DisAttributeRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisFalseTargetsEnhancedAttributeRecord : public DisAttributeRecord
{
public:
   DisFalseTargetsEnhancedAttributeRecord();
   DisFalseTargetsEnhancedAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI);
   ~DisFalseTargetsEnhancedAttributeRecord() override;

   // Allow compiler to build copy constructor and operator=
   // DisFalseTargetsEnhancedAttributeRecord(const DisFalseTargetsEnhancedAttributeRecord& aSrc);
   // DisFalseTargetsEnhancedAttributeRecord& operator=(const DisFalseTargetsEnhancedAttributeRecord& aRhs);

   // Input / Output
   void      GetDerivedClass(GenI& aGenI) override;
   void      PutDerivedClass(GenO& aGenO) const override;
   DisUint16 GetDerivedClassLength() override;

   // Data Validation
   bool IsValid() const override;

   // Accessors

   DisUint8   GetEmitterNumber() const;
   DisUint8   GetBeamNumber() const;
   DisEnum8   GetStateIndicator() const;
   DisEnum8   GetShowRealTargetReturn() const;
   DisFloat32 GetFalseTargetCount() const;
   DisFloat32 GetWalkSpeedMetersPerSecond() const;
   DisFloat32 GetWalkAccelerationMetersPerSecondSquared() const;
   DisFloat32 GetMaximumWalkDistanceMeters() const;
   DisFloat32 GetKeepTimeSeconds() const;
   DisFloat32 GetEchoSpacingMeters() const;
   DisFloat32 GetInitialOffsetMeters() const;

   // Mutators

   void SetEmitterNumber(DisUint8 aEmitterNumber);
   void SetBeamNumber(DisUint8 aBeamNumber);
   void SetStateIndicator(DisEnum8 aStateIndicator);
   void SetShowRealTargetReturn(DisEnum8 aShowRealTargetReturn);
   void SetFalseTargetCount(DisFloat32 aFalseTargetCount);
   void SetWalkSpeedMetersPerSecond(DisFloat32 aWalkSpeedMetersPerSecond);
   void SetWalkAccelerationMetersPerSecondSquared(DisFloat32 aWalkAccelerationMetersPerSecondSquared);
   void SetMaximumWalkDistanceMeters(DisFloat32 aMaximumWalkDistanceMeters);
   void SetKeepTimeSeconds(DisFloat32 aKeepTimeSeconds);
   void SetEchoSpacingMeters(DisFloat32 aEchoSpacingMeters);
   void SetInitialOffsetMeters(DisFloat32 aInitialOffsetMeters);

private:
   DisUint8   mEmitterNumber;
   DisUint8   mBeamNumber;
   DisEnum8   mStateIndicator;
   DisUint8   mShowRealTargetReturn;
   DisFloat32 mFalseTargetCount;
   DisFloat32 mWalkSpeedMetersPerSecond;
   DisFloat32 mWalkAccelerationMetersPerSecondSquared;
   DisFloat32 mMaximumWalkDistanceMeters;
   DisFloat32 mKeepTimeSeconds;
   DisFloat32 mEchoSpacingMeters;
   DisFloat32 mInitialOffsetMeters;
};

// Accessors

inline DisUint8 DisFalseTargetsEnhancedAttributeRecord::GetEmitterNumber() const
{
   return mEmitterNumber;
}

inline DisUint8 DisFalseTargetsEnhancedAttributeRecord::GetBeamNumber() const
{
   return mBeamNumber;
}

inline DisEnum8 DisFalseTargetsEnhancedAttributeRecord::GetStateIndicator() const
{
   return mStateIndicator;
}

inline DisEnum8 DisFalseTargetsEnhancedAttributeRecord::GetShowRealTargetReturn() const
{
   return mShowRealTargetReturn;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetFalseTargetCount() const
{
   return mFalseTargetCount;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetWalkSpeedMetersPerSecond() const
{
   return mWalkSpeedMetersPerSecond;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetWalkAccelerationMetersPerSecondSquared() const
{
   return mWalkAccelerationMetersPerSecondSquared;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetMaximumWalkDistanceMeters() const
{
   return mMaximumWalkDistanceMeters;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetKeepTimeSeconds() const
{
   return mKeepTimeSeconds;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetEchoSpacingMeters() const
{
   return mEchoSpacingMeters;
}

inline DisFloat32 DisFalseTargetsEnhancedAttributeRecord::GetInitialOffsetMeters() const
{
   return mInitialOffsetMeters;
}

// Mutators

inline void DisFalseTargetsEnhancedAttributeRecord::SetEmitterNumber(DisUint8 aEmitterNumber)
{
   mEmitterNumber = aEmitterNumber;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetBeamNumber(DisUint8 aBeamNumber)
{
   mBeamNumber = aBeamNumber;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetStateIndicator(DisEnum8 aStateIndicator)
{
   mStateIndicator = aStateIndicator;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetShowRealTargetReturn(DisEnum8 aShowRealTargetReturn)
{
   mShowRealTargetReturn = aShowRealTargetReturn;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetFalseTargetCount(DisFloat32 aFalseTargetCount)
{
   mFalseTargetCount = aFalseTargetCount;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetWalkSpeedMetersPerSecond(DisFloat32 aWalkSpeedMetersPerSecond)
{
   mWalkSpeedMetersPerSecond = aWalkSpeedMetersPerSecond;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetWalkAccelerationMetersPerSecondSquared(
   DisFloat32 aWalkAccelerationMetersPerSecondSquared)
{
   mWalkAccelerationMetersPerSecondSquared = aWalkAccelerationMetersPerSecondSquared;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetMaximumWalkDistanceMeters(DisFloat32 aMaximumWalkDistanceMeters)
{
   mMaximumWalkDistanceMeters = aMaximumWalkDistanceMeters;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetKeepTimeSeconds(DisFloat32 aKeepTimeSeconds)
{
   mKeepTimeSeconds = aKeepTimeSeconds;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetEchoSpacingMeters(DisFloat32 aEchoSpacingMeters)
{
   mEchoSpacingMeters = aEchoSpacingMeters;
}

inline void DisFalseTargetsEnhancedAttributeRecord::SetInitialOffsetMeters(DisFloat32 aInitialOffsetMeters)
{
   mInitialOffsetMeters = aInitialOffsetMeters;
}

#endif
