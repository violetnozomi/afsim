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

#include "DisEnhancedGroundCombatVehicleRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEnhancedGroundCombatVehicleRecord::DisEnhancedGroundCombatVehicleRecord(GenI& aGenI)
   : DisBasicGroundCombatVehicleRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisEnhancedGroundCombatVehicleRecord::DisEnhancedGroundCombatVehicleRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                           GenI& aGenI)
   : DisBasicGroundCombatVehicleRecord(aRecord, aGenI)
{
   GetMemberData(aGenI);
}

DisEnhancedGroundCombatVehicleRecord::DisEnhancedGroundCombatVehicleRecord(const DisBasicGroundCombatVehicleRecord& aRecord,
                                                                           GenI& aGenI)
   : DisBasicGroundCombatVehicleRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisEnhancedGroundCombatVehicleRecord::DisEnhancedGroundCombatVehicleRecord(const DisEnhancedGroundCombatVehicleRecord& aSrc)
   : DisBasicGroundCombatVehicleRecord(aSrc)
   , mFuelStatus(aSrc.mFuelStatus)
   , mGroundMaintenanceStatus(aSrc.mGroundMaintenanceStatus)
   , mPrimaryAmmunition(aSrc.mPrimaryAmmunition)
   , mSecondaryAmmunition(aSrc.mSecondaryAmmunition)
{
}

void DisEnhancedGroundCombatVehicleRecord::Get(GenI& aGenI)
{
   DisBasicGroundCombatVehicleRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEnhancedGroundCombatVehicleRecord::GetLength()
{
   return DisBasicGroundCombatVehicleRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisEnhancedGroundCombatVehicleRecord::Put(GenO& aGenO) const
{
   DisBasicGroundCombatVehicleRecord::Put(aGenO);
   aGenO << mFuelStatus;
   aGenO << mGroundMaintenanceStatus;
   aGenO << mPrimaryAmmunition;
   aGenO << mSecondaryAmmunition;
}

bool DisEnhancedGroundCombatVehicleRecord::IsValid() const
{
   return DisBasicGroundCombatVehicleRecord::IsValid();
}

DisEnhancedGroundCombatVehicleRecord* DisEnhancedGroundCombatVehicleRecord::Clone() const
{
   return new DisEnhancedGroundCombatVehicleRecord(*this);
}

DisUint8 DisEnhancedGroundCombatVehicleRecord::GetFuelStatus() const
{
   return mFuelStatus;
}

DisUint8 DisEnhancedGroundCombatVehicleRecord::GetGroundMaintenanceStatus() const
{
   return mGroundMaintenanceStatus;
}

DisUint8 DisEnhancedGroundCombatVehicleRecord::GetPrimaryAmmunition() const
{
   return mPrimaryAmmunition;
}

DisUint8 DisEnhancedGroundCombatVehicleRecord::GetSecondaryAmmunition() const
{
   return mSecondaryAmmunition;
}

void DisEnhancedGroundCombatVehicleRecord::SetFuelStatus(DisUint8 aFuelStatus)
{
   mFuelStatus = aFuelStatus;
}

void DisEnhancedGroundCombatVehicleRecord::SetGroundMaintenanceStatus(DisUint8 aGroundMaintenanceStatus)
{
   mGroundMaintenanceStatus = aGroundMaintenanceStatus;
}

void DisEnhancedGroundCombatVehicleRecord::SetPrimaryAmmunition(DisUint8 aPrimaryAmmunition)
{
   mPrimaryAmmunition = aPrimaryAmmunition;
}

void DisEnhancedGroundCombatVehicleRecord::SetSecondaryAmmunition(DisUint8 aSecondaryAmmunition)
{
   mSecondaryAmmunition = aSecondaryAmmunition;
}

void DisEnhancedGroundCombatVehicleRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mFuelStatus;
   aGenI >> mGroundMaintenanceStatus;
   aGenI >> mPrimaryAmmunition;
   aGenI >> mSecondaryAmmunition;
}

// Operators
bool DisEnhancedGroundCombatVehicleRecord::operator==(const DisEnhancedGroundCombatVehicleRecord& aRhs) const
{
   return ((DisBasicGroundCombatVehicleRecord::operator==(aRhs)) && (mFuelStatus == aRhs.mFuelStatus) &&
           (mGroundMaintenanceStatus == aRhs.mGroundMaintenanceStatus) &&
           (mPrimaryAmmunition == aRhs.mPrimaryAmmunition) && (mSecondaryAmmunition == aRhs.mSecondaryAmmunition));
}
