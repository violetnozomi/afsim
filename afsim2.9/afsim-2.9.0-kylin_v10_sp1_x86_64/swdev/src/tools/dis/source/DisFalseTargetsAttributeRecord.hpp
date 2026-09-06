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

#ifndef DIS_FALSE_TARGETS_ATTRIBUTE_RECORD_HPP
#define DIS_FALSE_TARGETS_ATTRIBUTE_RECORD_HPP

#include "dis_export.h"

#include "DisAttribute.hpp"
#include "DisAttributeRecord.hpp"

class GenI;
class GenO;

/*!
 * Encapsulates information for communicating descrete values that are associated
 * with false targets jamming that cannot be referenced to an emitter node. The values
 * provided in this record should only be considered valid for the victim radar beams listed
 * in the jamming beam's Track/Jam data record(s).
 */
class DIS_EXPORT DisFalseTargetsAttributeRecord : public DisAttributeRecord
{
public:
   DisFalseTargetsAttributeRecord();
   DisFalseTargetsAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI);
   ~DisFalseTargetsAttributeRecord() override = default;

   // Allow compiler to build copy constructor and operator=
   // DisFalseTargetsAttributeRecord(const DisFalseTargetsAttributeRecord& aSrc);
   // DisFalseTargetsAttributeRecord& operator=(const DisFalseTargetsAttributeRecord& aRhs);

   // Input / Output
   void        GetDerivedClass(GenI& aGenI) override;
   void        PutDerivedClass(GenO& aGenO) const override;
   DisUint16   GetDerivedClassLength() override;
   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;
   const char* IndicatorTypeToString(DisAttribute::StateIndicatorType s) const;

   // Data Validation
   bool IsValid() const override;

   // Accessors

   DisUint8   GetEmitterNumber() const;
   DisUint8   GetBeamNumber() const;
   DisEnum8   GetStateIndicator() const;
   DisUint16  GetFalseTargetCount() const;
   DisFloat32 GetWalkSpeedMetersPerSecond() const;
   DisFloat32 GetWalkAccelerationMetersPerSecondSquared() const;
   DisFloat32 GetMaximumWalkDistanceMeters() const;
   DisFloat32 GetKeepTimeSeconds() const;
   DisFloat32 GetEchoSpacingMeters() const;
   DisFloat32 GetFirstTargetOffsetMeters() const;

   // Mutators

   void SetEmitterNumber(DisUint8 aEmitterNumber);
   void SetBeamNumber(DisUint8 aBeamNumber);
   void SetStateIndicator(DisEnum8 aStateIndicator);
   void SetFalseTargetCount(DisUint16 aFalseTargetCount);
   void SetWalkSpeedMetersPerSecond(DisFloat32 aWalkSpeedMetersPerSecond);
   void SetWalkAccelerationMetersPerSecondSquared(DisFloat32 aWalkAccelerationMetersPerSecondSquared);
   void SetMaximumWalkDistanceMeters(DisFloat32 aMaximumWalkDistanceMeters);
   void SetKeepTimeSeconds(DisFloat32 aKeepTimeSeconds);
   void SetEchoSpacingMeters(DisFloat32 aEchoSpacingMeters);
   void SetFirstTargetOffsetMeters(DisFloat32 aFirstTargetOffsetMeters);

private:
   /*!
    * Indicates the emitter system generating false targets
    */
   DisUint8 mEmitterNumber;
   /*!
    * Indicates the jamming beam generating false targets
    */
   DisUint8 mBeamNumber;
   /*!
    * Indicates:
    * 1. Whether false target data has changed since the issuance of the
    *    last false targets attribute record for this beam.
    * 2. Whether this record is part of the heartbeat update to meet periodic
    *    update requirements
    * 3. Whether false target data for the beam has ceased.
    */
   DisEnum8 mStateIndicator;
   /*!
    * Padding
    */
   DisUint8 mPadding1;
   /*!
    * Padding
    */
   DisUint16 mPadding2;
   /*!
    * Specifies the number of false targets presented to the victim radar
    */
   DisUint16 mFalseTargetCount;
   /*!
    * Specifies the speed (m/s) at which the false targets move toward the victim radar
    */
   DisFloat32 mWalkSpeedMetersPerSecond;
   /*!
    * Specifies the acceleration (m/s^2) at which the false targets accelerate toward the victim radar
    */
   DisFloat32 mWalkAccelerationMetersPerSecondSquared;
   /*!
    * Specifies the maximum distance (meters) that a false target is to walk before it pauses in range
    */
   DisFloat32 mMaximumWalkDistanceMeters;
   /*!
    * Specifies the simulation time (seconds) that a false target is to be held at the maximum walk distance
    * before resetting to its initial position
    */
   DisFloat32 mKeepTimeSeconds;
   /*!
    * Specifies the distance between false targets in meters
    */
   DisFloat32 mEchoSpacingMeters;
   /*!
    * Specifies the position of the first target relative to the jamming entity in meters.
    */
   DisFloat32 mFirstTargetOffsetMeters;
};

// Accessors

inline DisUint8 DisFalseTargetsAttributeRecord::GetEmitterNumber() const
{
   return mEmitterNumber;
}

inline DisUint8 DisFalseTargetsAttributeRecord::GetBeamNumber() const
{
   return mBeamNumber;
}

inline DisEnum8 DisFalseTargetsAttributeRecord::GetStateIndicator() const
{
   return mStateIndicator;
}

inline DisUint16 DisFalseTargetsAttributeRecord::GetFalseTargetCount() const
{
   return mFalseTargetCount;
}

inline DisFloat32 DisFalseTargetsAttributeRecord::GetWalkSpeedMetersPerSecond() const
{
   return mWalkSpeedMetersPerSecond;
}

inline DisFloat32 DisFalseTargetsAttributeRecord::GetWalkAccelerationMetersPerSecondSquared() const
{
   return mWalkAccelerationMetersPerSecondSquared;
}

inline DisFloat32 DisFalseTargetsAttributeRecord::GetMaximumWalkDistanceMeters() const
{
   return mMaximumWalkDistanceMeters;
}

inline DisFloat32 DisFalseTargetsAttributeRecord::GetKeepTimeSeconds() const
{
   return mKeepTimeSeconds;
}

inline DisFloat32 DisFalseTargetsAttributeRecord::GetEchoSpacingMeters() const
{
   return mEchoSpacingMeters;
}

inline DisFloat32 DisFalseTargetsAttributeRecord::GetFirstTargetOffsetMeters() const
{
   return mFirstTargetOffsetMeters;
}

// Mutators
inline void DisFalseTargetsAttributeRecord::SetEmitterNumber(DisUint8 aEmitterNumber)
{
   mEmitterNumber = aEmitterNumber;
}

inline void DisFalseTargetsAttributeRecord::SetBeamNumber(DisUint8 aBeamNumber)
{
   mBeamNumber = aBeamNumber;
}

inline void DisFalseTargetsAttributeRecord::SetStateIndicator(DisEnum8 aStateIndicator)
{
   mStateIndicator = aStateIndicator;
}

inline void DisFalseTargetsAttributeRecord::SetFalseTargetCount(DisUint16 aFalseTargetCount)
{
   mFalseTargetCount = aFalseTargetCount;
}

inline void DisFalseTargetsAttributeRecord::SetWalkSpeedMetersPerSecond(DisFloat32 aWalkSpeedMetersPerSecond)
{
   mWalkSpeedMetersPerSecond = aWalkSpeedMetersPerSecond;
}

inline void DisFalseTargetsAttributeRecord::SetWalkAccelerationMetersPerSecondSquared(
   DisFloat32 aWalkAccelerationMetersPerSecondSquared)
{
   mWalkAccelerationMetersPerSecondSquared = aWalkAccelerationMetersPerSecondSquared;
}

inline void DisFalseTargetsAttributeRecord::SetMaximumWalkDistanceMeters(DisFloat32 aMaximumWalkDistanceMeters)
{
   mMaximumWalkDistanceMeters = aMaximumWalkDistanceMeters;
}

inline void DisFalseTargetsAttributeRecord::SetKeepTimeSeconds(DisFloat32 aKeepTimeSeconds)
{
   mKeepTimeSeconds = aKeepTimeSeconds;
}

inline void DisFalseTargetsAttributeRecord::SetEchoSpacingMeters(DisFloat32 aEchoSpacingMeters)
{
   mEchoSpacingMeters = aEchoSpacingMeters;
}

inline void DisFalseTargetsAttributeRecord::SetFirstTargetOffsetMeters(DisFloat32 aFirstTargetOffsetMeters)
{
   mFirstTargetOffsetMeters = aFirstTargetOffsetMeters;
}

#endif
