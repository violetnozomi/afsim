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

#include "DisDETargetEnergyDepositionRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDETargetEnergyDepositionRecord::DisDETargetEnergyDepositionRecord()
   : mTargetEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mPadding1(0)
   , mPeakIrradiance(0.0)
{
}

DisDETargetEnergyDepositionRecord::DisDETargetEnergyDepositionRecord(GenI& aGenI)
   : mTargetEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mPadding1(0)
   , mPeakIrradiance(0.0)
{
   Get(aGenI);
}

DisDETargetEnergyDepositionRecord::~DisDETargetEnergyDepositionRecord() {}

void DisDETargetEnergyDepositionRecord::Put(GenO& aGenO) const
{
   aGenO << mTargetEntityId;
   aGenO << mPadding1;
   aGenO << mPeakIrradiance;
}

void DisDETargetEnergyDepositionRecord::Get(GenI& aGenI)
{
   aGenI >> mTargetEntityId;
   aGenI >> mPadding1;
   aGenI >> mPeakIrradiance;
}

bool DisDETargetEnergyDepositionRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!mTargetEntityId.IsValid())
      isValid = false;
   if (!ValidateScalar(mPeakIrradiance))
      isValid = false;
   return isValid;
}
