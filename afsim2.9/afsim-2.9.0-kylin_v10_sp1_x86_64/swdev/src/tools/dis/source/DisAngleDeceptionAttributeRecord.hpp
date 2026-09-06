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

// DIS Angle Deception Attribute Record

#ifndef DIS_ANGLE_DECEPTION_ATTRIBUTE_RECORD_HPP
#define DIS_ANGLE_DECEPTION_ATTRIBUTE_RECORD_HPP

#include "dis_export.h"

#include "DisAttributeRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisAngleDeceptionAttributeRecord : public DisAttributeRecord
{
public:
   DisAngleDeceptionAttributeRecord();
   DisAngleDeceptionAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI);
   ~DisAngleDeceptionAttributeRecord() override;

   // Allow compiler to build copy constructor and operator=
   // DisAngleDeceptionAttributeRecord(const DisAngleDeceptionAttributeRecord& aSrc);
   // DisAngleDeceptionAttributeRecord& operator=(const DisAngleDeceptionAttributeRecord& aRhs);

   // Input / Output
   void      GetDerivedClass(GenI& aGenI) override;
   void      PutDerivedClass(GenO& aGenO) const override;
   DisUint16 GetDerivedClassLength() override;

   //! Validate Data
   bool IsValid() const override;

   // Accessors

   DisUint8   GetEmitterNumber() const;
   DisUint8   GetBeamNumber() const;
   DisEnum8   GetStateIndicator() const;
   DisFloat32 GetAzimuthOffsetRadians() const;
   DisFloat32 GetAzimuthWidthRadians() const;
   DisFloat32 GetAzimuthPullRateRadiansPerSecond() const;
   DisFloat32 GetAzimuthPullAccelerationRadiansPerSecondSquared() const;
   DisFloat32 GetElevationOffsetRadians() const;
   DisFloat32 GetElevationWidthRadians() const;
   DisFloat32 GetElevationPullRateRadiansPerSecond() const;
   DisFloat32 GetElevationPullAccelerationRadiansPerSecondSquared() const;

   // Mutators

   void SetEmitterNumber(DisUint8 aEmitterNumber);
   void SetBeamNumber(DisUint8 aBeamNumber);
   void SetStateIndicator(DisEnum8 aStateIndicator);
   void SetAzimuthOffsetRadians(DisFloat32 aAzimuthOffsetRadians);
   void SetAzimuthWidthRadians(DisFloat32 aAzimuthWidthRadians);
   void SetAzimuthPullRateRadiansPerSecond(DisFloat32 aAzimuthPullRateRadiansPerSecond);
   void SetAzimuthPullAccelerationRadiansPerSecondSquared(DisFloat32 aAzimuthPullAccelerationRadiansPerSecondSquared);
   void SetElevationOffsetRadians(DisFloat32 aElevationOffsetRadians);
   void SetElevationWidthRadians(DisFloat32 aElevationWidthRadians);
   void SetElevationPullRateRadiansPerSecond(DisFloat32 aElevationPullRateRadiansPerSecond);
   void SetElevationPullAccelerationRadiansPerSecondSquared(DisFloat32 aElevationPullAccelerationRadiansPerSecondSquared);

private:
   DisUint8   mEmitterNumber;
   DisUint8   mBeamNumber;
   DisEnum8   mStateIndicator;
   DisUint8   mPadding1;
   DisFloat32 mAzimuthOffsetRadians;
   DisFloat32 mAzimuthWidthRadians;
   DisFloat32 mAzimuthPullRateRadiansPerSecond;
   DisFloat32 mAzimuthPullAccelerationRadiansPerSecondSquared;
   DisFloat32 mElevationOffsetRadians;
   DisFloat32 mElevationWidthRadians;
   DisFloat32 mElevationPullRateRadiansPerSecond;
   DisFloat32 mElevationPullAccelerationRadiansPerSecondSquared;
   DisUint32  mPadding2;
};

// Accessors

inline DisUint8 DisAngleDeceptionAttributeRecord::GetEmitterNumber() const
{
   return mEmitterNumber;
}

inline DisUint8 DisAngleDeceptionAttributeRecord::GetBeamNumber() const
{
   return mBeamNumber;
}

inline DisEnum8 DisAngleDeceptionAttributeRecord::GetStateIndicator() const
{
   return mStateIndicator;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetAzimuthOffsetRadians() const
{
   return mAzimuthOffsetRadians;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetAzimuthWidthRadians() const
{
   return mAzimuthWidthRadians;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetAzimuthPullRateRadiansPerSecond() const
{
   return mAzimuthPullRateRadiansPerSecond;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetAzimuthPullAccelerationRadiansPerSecondSquared() const
{
   return mAzimuthPullAccelerationRadiansPerSecondSquared;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetElevationOffsetRadians() const
{
   return mElevationOffsetRadians;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetElevationWidthRadians() const
{
   return mElevationWidthRadians;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetElevationPullRateRadiansPerSecond() const
{
   return mElevationPullRateRadiansPerSecond;
}

inline DisFloat32 DisAngleDeceptionAttributeRecord::GetElevationPullAccelerationRadiansPerSecondSquared() const
{
   return mElevationPullAccelerationRadiansPerSecondSquared;
}

// Mutators

inline void DisAngleDeceptionAttributeRecord::SetEmitterNumber(DisUint8 aEmitterNumber)
{
   mEmitterNumber = aEmitterNumber;
}

inline void DisAngleDeceptionAttributeRecord::SetBeamNumber(DisUint8 aBeamNumber)
{
   mBeamNumber = aBeamNumber;
}

inline void DisAngleDeceptionAttributeRecord::SetStateIndicator(DisEnum8 aStateIndicator)
{
   mStateIndicator = aStateIndicator;
}

inline void DisAngleDeceptionAttributeRecord::SetAzimuthOffsetRadians(DisFloat32 aAzimuthOffsetRadians)
{
   mAzimuthOffsetRadians = aAzimuthOffsetRadians;
}

inline void DisAngleDeceptionAttributeRecord::SetAzimuthWidthRadians(DisFloat32 aAzimuthWidthRadians)
{
   mAzimuthWidthRadians = aAzimuthWidthRadians;
}

inline void DisAngleDeceptionAttributeRecord::SetAzimuthPullRateRadiansPerSecond(DisFloat32 aAzimuthPullRateRadiansPerSecond)
{
   mAzimuthPullRateRadiansPerSecond = aAzimuthPullRateRadiansPerSecond;
}

inline void DisAngleDeceptionAttributeRecord::SetAzimuthPullAccelerationRadiansPerSecondSquared(
   DisFloat32 aAzimuthPullAccelerationRadiansPerSecondSquared)
{
   mAzimuthPullAccelerationRadiansPerSecondSquared = aAzimuthPullAccelerationRadiansPerSecondSquared;
}

inline void DisAngleDeceptionAttributeRecord::SetElevationOffsetRadians(DisFloat32 aElevationOffsetRadians)
{
   mElevationOffsetRadians = aElevationOffsetRadians;
}

inline void DisAngleDeceptionAttributeRecord::SetElevationWidthRadians(DisFloat32 aElevationWidthRadians)
{
   mElevationWidthRadians = aElevationWidthRadians;
}

inline void DisAngleDeceptionAttributeRecord::SetElevationPullRateRadiansPerSecond(DisFloat32 aElevationPullRateRadiansPerSecond)
{
   mElevationPullRateRadiansPerSecond = aElevationPullRateRadiansPerSecond;
}

inline void DisAngleDeceptionAttributeRecord::SetElevationPullAccelerationRadiansPerSecondSquared(
   DisFloat32 aElevationPullAccelerationRadiansPerSecondSquared)
{
   mElevationPullAccelerationRadiansPerSecondSquared = aElevationPullAccelerationRadiansPerSecondSquared;
}

#endif
