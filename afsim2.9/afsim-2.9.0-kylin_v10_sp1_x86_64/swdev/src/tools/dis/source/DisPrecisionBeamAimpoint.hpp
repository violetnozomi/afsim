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

#ifndef DISPRECISIONBEAMAIMPOINT_HPP
#define DISPRECISIONBEAMAIMPOINT_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisTypes.hpp"

class DIS_EXPORT DisPrecisionBeamAimpoint
{
public:
   DisPrecisionBeamAimpoint();
   virtual ~DisPrecisionBeamAimpoint();

   // Accessors
   void GetTargetSpotWorldLocation(DisFloat64& aTargetSpotWorldLocationX,
                                   DisFloat64& aTargetSpotWorldLocationY,
                                   DisFloat64& aTargetSpotWorldLocationZ) const;
   void GetTargetSpotEntityLocation(DisFloat32& aTargetSpotEntityLocationX,
                                    DisFloat32& aTargetSpotEntityLocationY,
                                    DisFloat32& aTargetSpotEntityLocationZ) const;
   void GetTargetSpotVelocity(DisFloat32& aTargetSpotVelocityX,
                              DisFloat32& aTargetSpotVelocityY,
                              DisFloat32& aTargetSpotVelocityZ) const;
   void GetTargetSpotAcceleration(DisFloat32& aTargetSpotAccX, DisFloat32& aTargetSpotAccY, DisFloat32& aTargetSpotAccZ) const;
   const DisEntityId& GetTargetEntity() const;
   DisEnum8           GetTargetSpotType() const;
   DisEnum8           GetBeamSpotShape() const;
   DisFloat32         GetBeamSpotSemiMajorAxis() const;
   DisFloat32         GetBeamSpotSemiMinorAxis() const;
   DisFloat32         GetBeamSpotOrientationAngle() const;
   DisFloat32         GetPeakIrradiance() const;

   // Mutators
   void SetTargetSpotWorldLocation(DisFloat64 aTargetSpotWorldLocationX,
                                   DisFloat64 aTargetSpotWorldLocationY,
                                   DisFloat64 aTargetSpotWorldLocationZ);
   void SetTargetSpotEntityLocation(DisFloat32 aTargetSpotEntityLocationX,
                                    DisFloat32 aTargetSpotEntityLocationY,
                                    DisFloat32 aTargetSpotEntityLocationZ);
   void SetTargetSpotVelocity(DisFloat32 aTargetSpotVelocityX,
                              DisFloat32 aTargetSpotVelocityY,
                              DisFloat32 aTargetSpotVelocityZ);
   void SetTargetSpotAcceleration(DisFloat32 aTargetSpotAccelerationX,
                                  DisFloat32 aTargetSpotAccelerationY,
                                  DisFloat32 aTargetSpotAccelerationZ);
   void SetTargetEntity(const DisEntityId& aEntityId);
   void SetTargetSpotType(DisEnum8 aTargetSpotType);
   void SetBeamSpotShape(DisEnum8 aBeamSpotShape);
   void SetBeamSpotSemiMajorAxis(DisFloat32 aSemiMajorAxis);
   void SetBeamSpotSemiMinorAxis(DisFloat32 aSemiMinorAxis);
   void SetBeamSpotOrientationAngle(DisFloat32 aOrientationAngle);
   void SetPeakIrradiance(DisFloat32 aPeakIrradiance);

   // Input/Output

   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength();
   virtual void      Put(GenO& aGenO);

private:
   // Disallow copy and assignment
   DisPrecisionBeamAimpoint(const DisPrecisionBeamAimpoint& aSrc);
   DisPrecisionBeamAimpoint& operator=(const DisPrecisionBeamAimpoint& aRhs) = delete;

   DisFloat64  mTargetSpotWorldLocation[3];
   DisFloat32  mTargetSpotEntityLocation[3];
   DisFloat32  mTargetSpotVelocity[3];
   DisFloat32  mTargetSpotAcceleration[3];
   DisEntityId mTargetEntity;
   DisEnum8    mTargetSpotType;
   DisEnum8    mBeamSpotShape;
   DisFloat32  mBeamSpotSemiMajorAxis;
   DisFloat32  mBeamSpotSemiMinorAxis;
   DisFloat32  mBeamSpotOrientationAngle;
   DisFloat32  mPeakIrradiance;
   DisFloat32  mPadding1;
};

inline void DisPrecisionBeamAimpoint::GetTargetSpotWorldLocation(DisFloat64& aTargetSpotWorldLocationX,
                                                                 DisFloat64& aTargetSpotWorldLocationY,
                                                                 DisFloat64& aTargetSpotWorldLocationZ) const
{
   aTargetSpotWorldLocationX = mTargetSpotWorldLocation[0];
   aTargetSpotWorldLocationY = mTargetSpotWorldLocation[1];
   aTargetSpotWorldLocationZ = mTargetSpotWorldLocation[2];
}

inline void DisPrecisionBeamAimpoint::GetTargetSpotEntityLocation(DisFloat32& aTargetSpotEntityLocationX,
                                                                  DisFloat32& aTargetSpotEntityLocationY,
                                                                  DisFloat32& aTargetSpotEntityLocationZ) const
{
   aTargetSpotEntityLocationX = mTargetSpotEntityLocation[0];
   aTargetSpotEntityLocationY = mTargetSpotEntityLocation[1];
   aTargetSpotEntityLocationZ = mTargetSpotEntityLocation[2];
}

inline void DisPrecisionBeamAimpoint::GetTargetSpotVelocity(DisFloat32& aTargetSpotVelocityX,
                                                            DisFloat32& aTargetSpotVelocityY,
                                                            DisFloat32& aTargetSpotVelocityZ) const
{
   aTargetSpotVelocityX = mTargetSpotVelocity[0];
   aTargetSpotVelocityY = mTargetSpotVelocity[1];
   aTargetSpotVelocityZ = mTargetSpotVelocity[2];
}

inline void DisPrecisionBeamAimpoint::GetTargetSpotAcceleration(DisFloat32& aTargetSpotAccX,
                                                                DisFloat32& aTargetSpotAccY,
                                                                DisFloat32& aTargetSpotAccZ) const
{
   aTargetSpotAccX = mTargetSpotAcceleration[0];
   aTargetSpotAccY = mTargetSpotAcceleration[1];
   aTargetSpotAccZ = mTargetSpotAcceleration[2];
}

inline const DisEntityId& DisPrecisionBeamAimpoint::GetTargetEntity() const
{
   return mTargetEntity;
}

inline DisEnum8 DisPrecisionBeamAimpoint::GetTargetSpotType() const
{
   return mTargetSpotType;
}

inline DisEnum8 DisPrecisionBeamAimpoint::GetBeamSpotShape() const
{
   return mBeamSpotShape;
}

inline DisFloat32 DisPrecisionBeamAimpoint::GetPeakIrradiance() const
{
   return mPeakIrradiance;
}

inline void DisPrecisionBeamAimpoint::SetPeakIrradiance(DisFloat32 aPeakIrradiance)
{
   mPeakIrradiance = aPeakIrradiance;
}

// Mutators
inline void DisPrecisionBeamAimpoint::SetTargetSpotWorldLocation(DisFloat64 aTargetSpotWorldLocationX,
                                                                 DisFloat64 aTargetSpotWorldLocationY,
                                                                 DisFloat64 aTargetSpotWorldLocationZ)
{
   mTargetSpotWorldLocation[0] = aTargetSpotWorldLocationX;
   mTargetSpotWorldLocation[1] = aTargetSpotWorldLocationY;
   mTargetSpotWorldLocation[2] = aTargetSpotWorldLocationZ;
}

inline void DisPrecisionBeamAimpoint::SetTargetSpotEntityLocation(DisFloat32 aTargetSpotEntityLocationX,
                                                                  DisFloat32 aTargetSpotEntityLocationY,
                                                                  DisFloat32 aTargetSpotEntityLocationZ)
{
   mTargetSpotEntityLocation[0] = aTargetSpotEntityLocationX;
   mTargetSpotEntityLocation[1] = aTargetSpotEntityLocationY;
   mTargetSpotEntityLocation[2] = aTargetSpotEntityLocationZ;
}

inline void DisPrecisionBeamAimpoint::SetTargetSpotVelocity(DisFloat32 aTargetSpotVelocityX,
                                                            DisFloat32 aTargetSpotVelocityY,
                                                            DisFloat32 aTargetSpotVelocityZ)
{
   mTargetSpotVelocity[0] = aTargetSpotVelocityX;
   mTargetSpotVelocity[1] = aTargetSpotVelocityY;
   mTargetSpotVelocity[2] = aTargetSpotVelocityZ;
}

inline void DisPrecisionBeamAimpoint::SetTargetSpotAcceleration(DisFloat32 aTargetSpotAccelerationX,
                                                                DisFloat32 aTargetSpotAccelerationY,
                                                                DisFloat32 aTargetSpotAccelerationZ)
{
   mTargetSpotAcceleration[0] = aTargetSpotAccelerationX;
   mTargetSpotAcceleration[1] = aTargetSpotAccelerationY;
   mTargetSpotAcceleration[2] = aTargetSpotAccelerationZ;
}

inline void DisPrecisionBeamAimpoint::SetTargetEntity(const DisEntityId& aEntityId)
{
   mTargetEntity = aEntityId;
}

inline void DisPrecisionBeamAimpoint::SetTargetSpotType(DisEnum8 aTargetSpotType)
{
   mTargetSpotType = aTargetSpotType;
}

inline void DisPrecisionBeamAimpoint::SetBeamSpotShape(DisEnum8 aBeamSpotShape)
{
   mBeamSpotShape = aBeamSpotShape;
}

inline void DisPrecisionBeamAimpoint::SetBeamSpotSemiMajorAxis(DisFloat32 aSemiMajorAxis)
{
   mBeamSpotSemiMajorAxis = aSemiMajorAxis;
}

inline void DisPrecisionBeamAimpoint::SetBeamSpotSemiMinorAxis(DisFloat32 aSemiMinorAxis)
{
   mBeamSpotSemiMinorAxis = aSemiMinorAxis;
}

inline void DisPrecisionBeamAimpoint::SetBeamSpotOrientationAngle(DisFloat32 aOrientationAngle)
{
   this->mBeamSpotOrientationAngle = aOrientationAngle;
}

inline DisFloat32 DisPrecisionBeamAimpoint::GetBeamSpotSemiMajorAxis() const
{
   return mBeamSpotSemiMajorAxis;
}

inline DisFloat32 DisPrecisionBeamAimpoint::GetBeamSpotSemiMinorAxis() const
{
   return mBeamSpotSemiMinorAxis;
}

inline DisFloat32 DisPrecisionBeamAimpoint::GetBeamSpotOrientationAngle() const
{
   return mBeamSpotOrientationAngle;
}

#endif
