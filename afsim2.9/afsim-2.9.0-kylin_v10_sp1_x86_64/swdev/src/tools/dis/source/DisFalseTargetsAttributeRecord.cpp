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

#include "DisFalseTargetsAttributeRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisFalseTargetsAttributeRecord::DisFalseTargetsAttributeRecord()
   : DisAttributeRecord(FALSE_TARGETS_ATTRIBUTE_RECORD_TYPE)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mPadding1(0)
   , mPadding2(0)
   , mFalseTargetCount(0)
   , mWalkSpeedMetersPerSecond(0.0)
   , mWalkAccelerationMetersPerSecondSquared(0.0)
   , mMaximumWalkDistanceMeters(0.0)
   , mKeepTimeSeconds(0.0)
   , mEchoSpacingMeters(0.0)
   , mFirstTargetOffsetMeters(0.0)
{
}

DisFalseTargetsAttributeRecord::DisFalseTargetsAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI)
   : DisAttributeRecord(aAttributeRecord)
   , mEmitterNumber(0)
   , mBeamNumber(0)
   , mStateIndicator(0)
   , mPadding1(0)
   , mPadding2(0)
   , mFalseTargetCount(0)
   , mWalkSpeedMetersPerSecond(0.0)
   , mWalkAccelerationMetersPerSecondSquared(0.0)
   , mMaximumWalkDistanceMeters(0.0)
   , mKeepTimeSeconds(0.0)
   , mEchoSpacingMeters(0.0)
   , mFirstTargetOffsetMeters(0.0)
{
   GetDerivedClass(aGenI);
}

// virtual
DisUint16 DisFalseTargetsAttributeRecord::GetDerivedClassLength()
{
   return 32; // 256 bits = Total False Targets attribute record (320 bits) - Header size (64 bits)
}

void DisFalseTargetsAttributeRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisFalseTargetsAttributeRecord::ToString() const
{
   using namespace std;
   std::ostringstream ss("");
   ss << "***** False Targets Attribute Record Data   *****\n"
      << "Emitter Number:            " << static_cast<unsigned int>(GetEmitterNumber()) << '\n'
      << "Beam Number:               " << static_cast<unsigned int>(GetBeamNumber()) << '\n'
      << "State Indicator:           " << static_cast<unsigned int>(GetStateIndicator()) << " ("
      << std::to_string(static_cast<DisAttribute::StateIndicatorType>(GetStateIndicator())) << ")\n"
      << "False Target Count:        " << GetFalseTargetCount() << '\n'
      << "Walk Speed (m/s):          " << GetWalkSpeedMetersPerSecond() << '\n'
      << "Walk Acceleration (m/s^2): " << GetWalkAccelerationMetersPerSecondSquared() << '\n'
      << "Max Walk Distance (m):     " << GetMaximumWalkDistanceMeters() << '\n'
      << "Keep Time Seconds:         " << GetKeepTimeSeconds() << '\n'
      << "Echo Spacing (m):          " << GetEchoSpacingMeters() << '\n'
      << "First Target Offset (m):   " << GetFirstTargetOffsetMeters() << '\n'
      << "***** End of False Targets Attribute Record *****\n";
   return ss.str();
}

// virtual
void DisFalseTargetsAttributeRecord::GetDerivedClass(GenI& aGenI)
{
   aGenI >> mEmitterNumber;
   aGenI >> mBeamNumber;
   aGenI >> mStateIndicator;
   aGenI >> mPadding1;
   aGenI >> mPadding2;
   aGenI >> mFalseTargetCount;
   aGenI >> mWalkSpeedMetersPerSecond;
   aGenI >> mWalkAccelerationMetersPerSecondSquared;
   aGenI >> mMaximumWalkDistanceMeters;
   aGenI >> mKeepTimeSeconds;
   aGenI >> mEchoSpacingMeters;
   aGenI >> mFirstTargetOffsetMeters;
}

// virtual
void DisFalseTargetsAttributeRecord::PutDerivedClass(GenO& aGenO) const
{
   aGenO << mEmitterNumber;
   aGenO << mBeamNumber;
   aGenO << mStateIndicator;
   aGenO << mPadding1;
   aGenO << mPadding2;
   aGenO << mFalseTargetCount;
   aGenO << mWalkSpeedMetersPerSecond;
   aGenO << mWalkAccelerationMetersPerSecondSquared;
   aGenO << mMaximumWalkDistanceMeters;
   aGenO << mKeepTimeSeconds;
   aGenO << mEchoSpacingMeters;
   aGenO << mFirstTargetOffsetMeters;
}

bool DisFalseTargetsAttributeRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mWalkSpeedMetersPerSecond))
   {
      isValid = false;
   }
   if (!ValidateScalar(mWalkAccelerationMetersPerSecondSquared))
   {
      isValid = false;
   }
   if (!ValidateScalar(mMaximumWalkDistanceMeters))
   {
      isValid = false;
   }
   if (!ValidateScalar(mKeepTimeSeconds))
   {
      isValid = false;
   }
   if (!ValidateScalar(mEchoSpacingMeters))
   {
      isValid = false;
   }
   if (!ValidateScalar(mFirstTargetOffsetMeters))
   {
      isValid = false;
   }
   return isValid;
}

// Converts from a DisAttribute::StateIndicatorType to a string.
const char* DisFalseTargetsAttributeRecord::IndicatorTypeToString(DisAttribute::StateIndicatorType s) const
{
   switch (s)
   {
   case DisAttribute::StateUpdate:
   {
      return "state update";
   }
   case DisAttribute::ChangedDataUpdate:
   {
      return "changed data update";
   }
   case DisAttribute::Ceased:
   {
      return "ceased";
   }
   case DisAttribute::NotTransmitting:
   {
      return "not transmitting (non-standard extension)";
   }
   default:
   {
      break;
   }
   }
   return "unknown DisAttribute::StateIndicatorType";
}
