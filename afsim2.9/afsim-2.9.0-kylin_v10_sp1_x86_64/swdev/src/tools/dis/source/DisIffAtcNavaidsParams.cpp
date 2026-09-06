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

#include "DisIffAtcNavaidsParams.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisIffAtcNavaidsParams::DisIffAtcNavaidsParams()
   : mEffectiveRadiatedPower(0.0F)
   , mFrequency(0.0F)
   , mPulseRepetitionFrequency(0.0F)
   , mPulseWidth(0.0F)
   , mBurstLength(0)
   , mApplicableModes(0)
{
}

DisIffAtcNavaidsParams::~DisIffAtcNavaidsParams() {}

void DisIffAtcNavaidsParams::Get(GenI& aGenI)
{
   aGenI >> mEffectiveRadiatedPower;
   aGenI >> mFrequency;
   aGenI >> mPulseRepetitionFrequency;
   aGenI >> mPulseWidth;
   aGenI >> mBurstLength;
   aGenI >> mApplicableModes;
}

void DisIffAtcNavaidsParams::Put(GenO& aGenO) const
{
   aGenO << mEffectiveRadiatedPower;
   aGenO << mFrequency;
   aGenO << mPulseRepetitionFrequency;
   aGenO << mPulseWidth;
   aGenO << mBurstLength;
   aGenO << mApplicableModes;
}

bool DisIffAtcNavaidsParams::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mEffectiveRadiatedPower))
      isValid = false;
   if (!ValidateScalar(mFrequency))
      isValid = false;
   if (!ValidateScalar(mPulseRepetitionFrequency))
      isValid = false;
   if (!ValidateScalar(mPulseWidth))
      isValid = false;
   return isValid;
}
