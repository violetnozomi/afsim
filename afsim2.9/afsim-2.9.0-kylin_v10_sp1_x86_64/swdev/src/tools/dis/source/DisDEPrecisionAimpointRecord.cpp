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

#include "DisDEPrecisionAimpointRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDEPrecisionAimpointRecord::DisDEPrecisionAimpointRecord()
   : DisDERecord(DisDERecord::PRECISION_AIMPOINT_DE_RECORD_TYPE)
   , mTargetEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mTargetCompoundIdentifier(0)
   , mBeamSpotShape(0)
   , mPeakIrradiance(0.0)
   , mBeamSpotCrossSectionSemiMajorAxis(0.0)
   , mBeamSpotCrossSectionSemiMinorAxis(0.0)
   , mBeamSpotCrossSectionOrientationAngle(0.0)
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

DisDEPrecisionAimpointRecord::DisDEPrecisionAimpointRecord(DisDERecord& aRecord, GenI& aGenI)
   : DisDERecord(aRecord)
   , mTargetEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mTargetCompoundIdentifier(0)
   , mBeamSpotShape(0)
   , mPeakIrradiance(0.0)
   , mBeamSpotCrossSectionSemiMajorAxis(0.0)
   , mBeamSpotCrossSectionSemiMinorAxis(0.0)
   , mBeamSpotCrossSectionOrientationAngle(0.0)
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

   GetDerivedClass(aGenI);
}

DisDEPrecisionAimpointRecord::~DisDEPrecisionAimpointRecord() {}

// virtual
DisUint16 DisDEPrecisionAimpointRecord::GetDerivedClassLength()
{
   return 88;
}

// virtual
void DisDEPrecisionAimpointRecord::PutDerivedClass(GenO& aGenO) const
{
   aGenO << mTargetSpotWorldLocation[0] << mTargetSpotWorldLocation[1] << mTargetSpotWorldLocation[2];
   aGenO << mTargetSpotEntityLocation[0] << mTargetSpotEntityLocation[1] << mTargetSpotEntityLocation[2];
   aGenO << mTargetSpotVelocity[0] << mTargetSpotVelocity[1] << mTargetSpotVelocity[2];
   aGenO << mTargetSpotAcceleration[0] << mTargetSpotAcceleration[1] << mTargetSpotAcceleration[2];
   aGenO << mTargetEntityId;
   aGenO << mTargetCompoundIdentifier;
   aGenO << mBeamSpotShape;
   aGenO << mBeamSpotCrossSectionSemiMajorAxis;
   aGenO << mBeamSpotCrossSectionSemiMinorAxis;
   aGenO << mBeamSpotCrossSectionOrientationAngle;
   aGenO << mPeakIrradiance;
   aGenO << mPadding1;
}

// virtual
void DisDEPrecisionAimpointRecord::GetDerivedClass(GenI& aGenI)
{
   aGenI >> mTargetSpotWorldLocation[0] >> mTargetSpotWorldLocation[1] >> mTargetSpotWorldLocation[2];
   aGenI >> mTargetSpotEntityLocation[0] >> mTargetSpotEntityLocation[1] >> mTargetSpotEntityLocation[2];
   aGenI >> mTargetSpotVelocity[0] >> mTargetSpotVelocity[1] >> mTargetSpotVelocity[2];
   aGenI >> mTargetSpotAcceleration[0] >> mTargetSpotAcceleration[1] >> mTargetSpotAcceleration[2];
   aGenI >> mTargetEntityId;
   aGenI >> mTargetCompoundIdentifier;
   aGenI >> mBeamSpotShape;
   aGenI >> mBeamSpotCrossSectionSemiMajorAxis;
   aGenI >> mBeamSpotCrossSectionSemiMinorAxis;
   aGenI >> mBeamSpotCrossSectionOrientationAngle;
   aGenI >> mPeakIrradiance;
   aGenI >> mPadding1;
}

bool DisDEPrecisionAimpointRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mTargetSpotWorldLocation[0]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotWorldLocation[1]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotWorldLocation[2]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotEntityLocation[0]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotEntityLocation[1]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotEntityLocation[2]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotVelocity[0]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotVelocity[1]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotVelocity[2]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotAcceleration[0]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotAcceleration[1]))
      isValid = false;
   if (!ValidateScalar(mTargetSpotAcceleration[2]))
      isValid = false;
   if (!ValidateScalar(mBeamSpotCrossSectionSemiMajorAxis))
      isValid = false;
   if (!ValidateScalar(mBeamSpotCrossSectionSemiMinorAxis))
      isValid = false;
   if (!ValidateScalar(mBeamSpotCrossSectionOrientationAngle))
      isValid = false;
   if (!ValidateScalar(mPeakIrradiance))
      isValid = false;
   if (!mTargetEntityId.IsValid())
      isValid = false;
   return isValid;
}
