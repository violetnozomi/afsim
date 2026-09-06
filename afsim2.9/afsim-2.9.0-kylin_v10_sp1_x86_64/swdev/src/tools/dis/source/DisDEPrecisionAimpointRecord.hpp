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

// DIS DE Precision Aimpoint Record

#ifndef DIS_DE_PRECISION_AIMPOINT_RECORD_HPP
#define DIS_DE_PRECISION_AIMPOINT_RECORD_HPP

#include "dis_export.h"

#include "DisDERecord.hpp"
#include "DisEntityId.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDEPrecisionAimpointRecord : public DisDERecord
{
public:
   DisDEPrecisionAimpointRecord();
   DisDEPrecisionAimpointRecord(DisDERecord& aRecord, GenI& aGenI);
   ~DisDEPrecisionAimpointRecord() override;

   // Input/output

   DisUint16 GetDerivedClassLength() override;
   void      GetDerivedClass(GenI& aGenI) override;
   void      PutDerivedClass(GenO& aGenO) const override;

   // Data Validation
   bool IsValid() const override;

   // Accessors

   void GetTargetSpotWorldLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ);
   void GetTargetSpotEntityLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ);
   void GetTargetSpotVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ);
   void GetTargetSpotAcceleration(DisFloat32& aAccelerationX, DisFloat32& aAccelerationY, DisFloat32& aAccelerationZ);
   const DisEntityId& GetTargetEntityId();
   DisEnum8           GetTargetCompoundIdentifier();
   DisEnum8           GetBeamSpotShape();
   DisFloat32         GetPeakIrradiance();
   DisFloat32         GetBeamSpotCrossSectionSemiMajorAxis();
   DisFloat32         GetBeamSpotCrossSectionSemiMinorAxis();
   DisFloat32         GetBeamSpotCrossSectionOrientationAngle();

   // Mutators

   void SetTargetSpotWorldLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);
   void SetTargetSpotEntityLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ);
   void SetTargetSpotVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetTargetSpotAcceleration(DisFloat32 aAccelerationX, DisFloat32 aAccelerationY, DisFloat32 aAccelerationZ);
   void SetTargetEntityId(const DisEntityId& aTargetEntityId);
   void SetTargetCompoundIdentifier(const DisEnum8 aTargetCompoundIdentifier);
   void SetBeamSpotShape(const DisEnum8 aBeamSpotShape);
   void SetPeakIrradiance(const DisFloat32 aPeakIrradiance);
   void SetBeamSpotCrossSectionSemiMajorAxis(const DisFloat32 aBeamSpotCrossSectionSemiMajorAxis);
   void SetBeamSpotCrossSectionSemiMinorAxis(const DisFloat32 aBeamSpotCrossSectionSemiMinorAxis);
   void SetBeamSpotCrossSectionOrientationAngle(const DisFloat32 aBeamSpotCrossSectionOrientationAngle);

private:
   DisFloat64  mTargetSpotWorldLocation[3];
   DisFloat32  mTargetSpotEntityLocation[3];
   DisFloat32  mTargetSpotVelocity[3];
   DisFloat32  mTargetSpotAcceleration[3];
   DisEntityId mTargetEntityId;
   DisEnum8    mTargetCompoundIdentifier;
   DisEnum8    mBeamSpotShape;
   DisFloat32  mPeakIrradiance;
   DisFloat32  mBeamSpotCrossSectionSemiMajorAxis;
   DisFloat32  mBeamSpotCrossSectionSemiMinorAxis;
   DisFloat32  mBeamSpotCrossSectionOrientationAngle;
   DisFloat32  mPadding1;
};

// Accessors

inline void DisDEPrecisionAimpointRecord::GetTargetSpotWorldLocation(DisFloat64& aLocationX,
                                                                     DisFloat64& aLocationY,
                                                                     DisFloat64& aLocationZ)
{
   aLocationX = mTargetSpotWorldLocation[0];
   aLocationY = mTargetSpotWorldLocation[1];
   aLocationZ = mTargetSpotWorldLocation[2];
}

inline void DisDEPrecisionAimpointRecord::GetTargetSpotEntityLocation(DisFloat32& aLocationX,
                                                                      DisFloat32& aLocationY,
                                                                      DisFloat32& aLocationZ)
{
   aLocationX = mTargetSpotEntityLocation[0];
   aLocationY = mTargetSpotEntityLocation[1];
   aLocationZ = mTargetSpotEntityLocation[2];
}

inline void DisDEPrecisionAimpointRecord::GetTargetSpotVelocity(DisFloat32& aVelocityX,
                                                                DisFloat32& aVelocityY,
                                                                DisFloat32& aVelocityZ)
{
   aVelocityX = mTargetSpotVelocity[0];
   aVelocityY = mTargetSpotVelocity[1];
   aVelocityZ = mTargetSpotVelocity[2];
}

inline void DisDEPrecisionAimpointRecord::GetTargetSpotAcceleration(DisFloat32& aAccelerationX,
                                                                    DisFloat32& aAccelerationY,
                                                                    DisFloat32& aAccelerationZ)
{
   aAccelerationX = mTargetSpotAcceleration[0];
   aAccelerationY = mTargetSpotAcceleration[1];
   aAccelerationZ = mTargetSpotAcceleration[2];
}

inline const DisEntityId& DisDEPrecisionAimpointRecord::GetTargetEntityId()
{
   return mTargetEntityId;
}

inline DisEnum8 DisDEPrecisionAimpointRecord::GetTargetCompoundIdentifier()
{
   return mTargetCompoundIdentifier;
}

inline DisEnum8 DisDEPrecisionAimpointRecord::GetBeamSpotShape()
{
   return mBeamSpotShape;
}

inline DisFloat32 DisDEPrecisionAimpointRecord::GetPeakIrradiance()
{
   return mPeakIrradiance;
}

inline DisFloat32 DisDEPrecisionAimpointRecord::GetBeamSpotCrossSectionSemiMajorAxis()
{
   return mBeamSpotCrossSectionSemiMajorAxis;
}

inline DisFloat32 DisDEPrecisionAimpointRecord::GetBeamSpotCrossSectionSemiMinorAxis()
{
   return mBeamSpotCrossSectionSemiMinorAxis;
}

inline DisFloat32 DisDEPrecisionAimpointRecord::GetBeamSpotCrossSectionOrientationAngle()
{
   return mBeamSpotCrossSectionOrientationAngle;
}

inline void DisDEPrecisionAimpointRecord::SetPeakIrradiance(DisFloat32 aPeakIrradiance)
{
   mPeakIrradiance = aPeakIrradiance;
}

// Mutators

inline void DisDEPrecisionAimpointRecord::SetTargetSpotWorldLocation(DisFloat64 aLocationX,
                                                                     DisFloat64 aLocationY,
                                                                     DisFloat64 aLocationZ)
{
   mTargetSpotWorldLocation[0] = aLocationX;
   mTargetSpotWorldLocation[1] = aLocationY;
   mTargetSpotWorldLocation[2] = aLocationZ;
}

inline void DisDEPrecisionAimpointRecord::SetTargetSpotEntityLocation(DisFloat32 aLocationX,
                                                                      DisFloat32 aLocationY,
                                                                      DisFloat32 aLocationZ)
{
   mTargetSpotEntityLocation[0] = aLocationX;
   mTargetSpotEntityLocation[1] = aLocationY;
   mTargetSpotEntityLocation[2] = aLocationZ;
}

inline void DisDEPrecisionAimpointRecord::SetTargetSpotVelocity(DisFloat32 aVelocityX,
                                                                DisFloat32 aVelocityY,
                                                                DisFloat32 aVelocityZ)
{
   mTargetSpotVelocity[0] = aVelocityX;
   mTargetSpotVelocity[1] = aVelocityY;
   mTargetSpotVelocity[2] = aVelocityZ;
}

inline void DisDEPrecisionAimpointRecord::SetTargetSpotAcceleration(DisFloat32 aAccelerationX,
                                                                    DisFloat32 aAccelerationY,
                                                                    DisFloat32 aAccelerationZ)
{
   mTargetSpotAcceleration[0] = aAccelerationX;
   mTargetSpotAcceleration[1] = aAccelerationY;
   mTargetSpotAcceleration[2] = aAccelerationZ;
}

inline void DisDEPrecisionAimpointRecord::SetTargetEntityId(const DisEntityId& aTargetEntityId)
{
   mTargetEntityId = aTargetEntityId;
}

inline void DisDEPrecisionAimpointRecord::SetTargetCompoundIdentifier(DisEnum8 aTargetCompoundIdentifier)
{
   mTargetCompoundIdentifier = aTargetCompoundIdentifier;
}

inline void DisDEPrecisionAimpointRecord::SetBeamSpotShape(DisEnum8 aBeamSpotShape)
{
   mBeamSpotShape = aBeamSpotShape;
}

inline void DisDEPrecisionAimpointRecord::SetBeamSpotCrossSectionSemiMajorAxis(DisFloat32 aBeamSpotCrossSectionSemiMajorAxis)
{
   mBeamSpotCrossSectionSemiMajorAxis = aBeamSpotCrossSectionSemiMajorAxis;
}

inline void DisDEPrecisionAimpointRecord::SetBeamSpotCrossSectionSemiMinorAxis(DisFloat32 aBeamSpotCrossSectionSemiMinorAxis)
{
   mBeamSpotCrossSectionSemiMinorAxis = aBeamSpotCrossSectionSemiMinorAxis;
}

inline void DisDEPrecisionAimpointRecord::SetBeamSpotCrossSectionOrientationAngle(DisFloat32 aBeamSpotCrossSectionOrientationAngle)
{
   mBeamSpotCrossSectionOrientationAngle = aBeamSpotCrossSectionOrientationAngle;
}

#endif
