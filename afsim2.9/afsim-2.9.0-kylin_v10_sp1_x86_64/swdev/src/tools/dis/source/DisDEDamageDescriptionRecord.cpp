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

#include "DisDEDamageDescriptionRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDEDamageDescriptionRecord::DisDEDamageDescriptionRecord()
   : DisDDRecord(DisDDRecord::DAMAGE_DESCRIPTION_DD_RECORD_TYPE)
   , mDamageDiameter(0)
   , mTemperature(0)
   , mComponentIdentification(0)
   , mComponentDamageStatus(0)
   , mComponentVisualDamageStatus(0)
   , mComponentVisualSmokeColor(0)
   , mFireEvent()
   , mPadding(0)
{
   mDamageLocation[0] = 0;
   mDamageLocation[1] = 0;
   mDamageLocation[2] = 0;
}

DisDEDamageDescriptionRecord::DisDEDamageDescriptionRecord(DisDDRecord& aRecord, GenI& aGenI)
   : DisDDRecord(aRecord)
   , mDamageDiameter(0)
   , mTemperature(0)
   , mComponentIdentification(0)
   , mComponentDamageStatus(0)
   , mComponentVisualDamageStatus(0)
   , mComponentVisualSmokeColor(0)
   , mFireEvent()
   , mPadding(0)
{
   mDamageLocation[0] = 0;
   mDamageLocation[1] = 0;
   mDamageLocation[2] = 0;

   GetDerivedClass(aGenI);
}

DisDEDamageDescriptionRecord::~DisDEDamageDescriptionRecord() {}

// virtual
DisUint16 DisDEDamageDescriptionRecord::GetDerivedClassLength()
{
   return 32;
}

// virtual
void DisDEDamageDescriptionRecord::PutDerivedClass(GenO& aGenO) const
{
   aGenO << mDamageLocation[0] << mDamageLocation[1] << mDamageLocation[2];
   aGenO << mDamageDiameter;
   aGenO << mTemperature;
   aGenO << mComponentIdentification;
   aGenO << mComponentDamageStatus;
   aGenO << mComponentVisualDamageStatus;
   aGenO << mComponentVisualSmokeColor;
   aGenO << mFireEvent;
   aGenO << mPadding;
}

// virtual
void DisDEDamageDescriptionRecord::GetDerivedClass(GenI& aGenI)
{
   aGenI >> mDamageLocation[0] >> mDamageLocation[1] >> mDamageLocation[2];
   aGenI >> mDamageDiameter;
   aGenI >> mTemperature;
   aGenI >> mComponentIdentification;
   aGenI >> mComponentDamageStatus;
   aGenI >> mComponentVisualDamageStatus;
   aGenI >> mComponentVisualSmokeColor;
   aGenI >> mFireEvent;
   aGenI >> mPadding;
}

bool DisDEDamageDescriptionRecord::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!ValidateScalar(mDamageLocation[0]))
      isValid = false;
   if (!ValidateScalar(mDamageLocation[1]))
      isValid = false;
   if (!ValidateScalar(mDamageLocation[2]))
      isValid = false;
   if (!ValidateScalar(mDamageDiameter))
      isValid = false;
   if (!ValidateScalar(mTemperature))
      isValid = false;
   if (!mFireEvent.IsValid())
      isValid = false;
   return isValid;
}
