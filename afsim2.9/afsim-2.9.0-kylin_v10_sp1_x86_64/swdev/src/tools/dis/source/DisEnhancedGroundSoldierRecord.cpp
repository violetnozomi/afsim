// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisEnhancedGroundSoldierRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEnhancedGroundSoldierRecord::DisEnhancedGroundSoldierRecord(GenI& aGenI)
   : DisBasicGroundSoldierRecord(aGenI)
{
   GetMemberData(aGenI);
}


DisEnhancedGroundSoldierRecord::DisEnhancedGroundSoldierRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI)
   : DisBasicGroundSoldierRecord(aRecord, aGenI)
{
   GetMemberData(aGenI);
}

DisEnhancedGroundSoldierRecord::DisEnhancedGroundSoldierRecord(const DisBasicGroundSoldierRecord& aRecord, GenI& aGenI)
   : DisBasicGroundSoldierRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisEnhancedGroundSoldierRecord::DisEnhancedGroundSoldierRecord(const DisEnhancedGroundSoldierRecord& aSrc)
   : DisBasicGroundSoldierRecord(aSrc)
   , mWaterStatus(aSrc.mWaterStatus)
   , mRestStatus(aSrc.mRestStatus)
   , mPrimaryAmmunition(aSrc.mPrimaryAmmunition)
   , mSecondaryAmmunition(aSrc.mSecondaryAmmunition)
{
}

void DisEnhancedGroundSoldierRecord::Get(GenI& aGenI)
{
   DisBasicGroundSoldierRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEnhancedGroundSoldierRecord::GetLength()
{
   return DisBasicGroundSoldierRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisEnhancedGroundSoldierRecord::Put(GenO& aGenO) const
{
   DisBasicGroundSoldierRecord::Put(aGenO);
   aGenO << mWaterStatus;
   aGenO << mRestStatus;
   aGenO << mPrimaryAmmunition;
   aGenO << mSecondaryAmmunition;
}

bool DisEnhancedGroundSoldierRecord::IsValid() const
{
   return DisBasicGroundSoldierRecord::IsValid();
}

DisEnhancedGroundSoldierRecord* DisEnhancedGroundSoldierRecord::Clone() const
{
   return new DisEnhancedGroundSoldierRecord(*this);
}

void DisEnhancedGroundSoldierRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mWaterStatus;
   aGenI >> mRestStatus;
   aGenI >> mPrimaryAmmunition;
   aGenI >> mSecondaryAmmunition;
}

DisUint8 DisEnhancedGroundSoldierRecord::GetWaterStatus() const
{
   return mWaterStatus;
}

DisEnum8 DisEnhancedGroundSoldierRecord::GetRestStatus() const
{
   return mRestStatus;
}

DisUint8 DisEnhancedGroundSoldierRecord::GetPrimaryAmmunition() const
{
   return mPrimaryAmmunition;
}

DisUint8 DisEnhancedGroundSoldierRecord::GetSecondaryAmmunition() const
{
   return mSecondaryAmmunition;
}

void DisEnhancedGroundSoldierRecord::SetWaterStatus(DisUint8 aWaterStatus)
{
   mWaterStatus = aWaterStatus;
}

void DisEnhancedGroundSoldierRecord::SetRestStatus(const DisEnum8& aRestStatus)
{
   mRestStatus = aRestStatus;
}

void DisEnhancedGroundSoldierRecord::SetPrimaryAmmunition(DisUint8 aPrimaryAmmunition)
{
   mPrimaryAmmunition = aPrimaryAmmunition;
}

void DisEnhancedGroundSoldierRecord::SetSecondaryAmmunition(DisUint8 aSecondaryAmmunition)
{
   mSecondaryAmmunition = aSecondaryAmmunition;
}

// Operators
bool DisEnhancedGroundSoldierRecord::operator==(const DisEnhancedGroundSoldierRecord& aRhs) const
{
   return ((DisBasicGroundSoldierRecord::operator==(aRhs)) && (mWaterStatus == aRhs.mWaterStatus) &&
           (mRestStatus == aRhs.mRestStatus) && (mPrimaryAmmunition == aRhs.mPrimaryAmmunition) &&
           (mSecondaryAmmunition == aRhs.mSecondaryAmmunition));
}
