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

#include "DisPrecisionBeamAimpoint.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisPrecisionBeamAimpoint::DisPrecisionBeamAimpoint()
   : mTargetEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mTargetSpotType(0)
   , mBeamSpotShape(0)
   , mBeamSpotSemiMajorAxis(0.0)
   , mBeamSpotSemiMinorAxis(0.0)
   , mBeamSpotOrientationAngle(0.0)
   , mPeakIrradiance(0.0)
   , mPadding1(0.0)
{
   mTargetSpotWorldLocation[0]  = 0.0;
   mTargetSpotWorldLocation[1]  = 0.0;
   mTargetSpotWorldLocation[2]  = 0.0;
   mTargetSpotEntityLocation[0] = 0.0;
   mTargetSpotEntityLocation[1] = 0.0;
   mTargetSpotEntityLocation[2] = 0.0;
   mTargetSpotVelocity[0]       = 0.0;
   mTargetSpotVelocity[1]       = 0.0;
   mTargetSpotVelocity[2]       = 0.0;
   mTargetSpotAcceleration[0]   = 0.0;
   mTargetSpotAcceleration[1]   = 0.0;
   mTargetSpotAcceleration[2]   = 0.0;
}

// virtual
DisPrecisionBeamAimpoint::~DisPrecisionBeamAimpoint() {}

// virtual
void DisPrecisionBeamAimpoint::Get(GenI& aGenI)
{
   aGenI >> mTargetSpotWorldLocation[0] >> mTargetSpotWorldLocation[1] >> mTargetSpotWorldLocation[2];
   aGenI >> mTargetSpotEntityLocation[0] >> mTargetSpotEntityLocation[1] >> mTargetSpotEntityLocation[2];
   aGenI >> mTargetSpotVelocity[0] >> mTargetSpotVelocity[1] >> mTargetSpotVelocity[2];
   aGenI >> mTargetSpotAcceleration[0] >> mTargetSpotAcceleration[1] >> mTargetSpotAcceleration[2];
   aGenI >> mTargetEntity;
   aGenI >> mTargetSpotType;
   aGenI >> mBeamSpotShape;
   aGenI >> mBeamSpotSemiMajorAxis;
   aGenI >> mBeamSpotSemiMinorAxis;
   aGenI >> mBeamSpotOrientationAngle;
   aGenI >> mPeakIrradiance;
   aGenI >> mPadding1;
}

// virtual
void DisPrecisionBeamAimpoint::Put(GenO& aGenO)
{
   aGenO << mTargetSpotWorldLocation[0] << mTargetSpotWorldLocation[1] << mTargetSpotWorldLocation[2];
   aGenO << mTargetSpotEntityLocation[0] << mTargetSpotEntityLocation[1] << mTargetSpotEntityLocation[2];
   aGenO << mTargetSpotVelocity[0] << mTargetSpotVelocity[1] << mTargetSpotVelocity[2];
   aGenO << mTargetSpotAcceleration[0] << mTargetSpotAcceleration[1] << mTargetSpotAcceleration[2];
   aGenO << mTargetEntity;
   aGenO << mTargetSpotType;
   aGenO << mBeamSpotShape;
   aGenO << mBeamSpotSemiMajorAxis;
   aGenO << mBeamSpotSemiMinorAxis;
   aGenO << mBeamSpotOrientationAngle;
   aGenO << mPeakIrradiance;
   aGenO << mPadding1;
}

// virtual
DisUint16 DisPrecisionBeamAimpoint::GetLength()
{
   static DisUint16 sLength = 80;
   return sLength;
}
