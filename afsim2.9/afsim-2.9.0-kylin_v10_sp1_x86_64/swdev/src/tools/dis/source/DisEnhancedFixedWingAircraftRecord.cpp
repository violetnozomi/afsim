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

#include "DisEnhancedFixedWingAircraftRecord.hpp"

#include "DisBasicFixedWingAircraftRecord.hpp"
#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEnhancedFixedWingAircraftRecord::DisEnhancedFixedWingAircraftRecord(GenI& aGenI)
   : DisBasicFixedWingAircraftRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisEnhancedFixedWingAircraftRecord::DisEnhancedFixedWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                       GenI&                                    aGenI)
   : DisBasicFixedWingAircraftRecord(aRecord, aGenI)
{
   GetMemberData(aGenI);
}

DisEnhancedFixedWingAircraftRecord::DisEnhancedFixedWingAircraftRecord(const DisBasicFixedWingAircraftRecord& aRecord,
                                                                       GenI&                                  aGenI)
   : DisBasicFixedWingAircraftRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisEnhancedFixedWingAircraftRecord::DisEnhancedFixedWingAircraftRecord(const DisEnhancedFixedWingAircraftRecord& aSrc)
   : DisBasicFixedWingAircraftRecord(aSrc)
   , mSupplementalFuelStatus(aSrc.mSupplementalFuelStatus)
   , mAirMaintenanceStatus(aSrc.mAirMaintenanceStatus)
   , mPrimaryAmmunition(aSrc.mPrimaryAmmunition)
   , mSecondaryAmmunition(aSrc.mSecondaryAmmunition)
{
}

void DisEnhancedFixedWingAircraftRecord::Get(GenI& aGenI)
{
   DisBasicFixedWingAircraftRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEnhancedFixedWingAircraftRecord::GetLength()
{
   return DisBasicFixedWingAircraftRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisEnhancedFixedWingAircraftRecord::Put(GenO& aGenO) const
{
   DisBasicFixedWingAircraftRecord::Put(aGenO);
   aGenO << mSupplementalFuelStatus;
   aGenO << mAirMaintenanceStatus;
   aGenO << mPrimaryAmmunition;
   aGenO << mSecondaryAmmunition;
}

bool DisEnhancedFixedWingAircraftRecord::IsValid() const
{
   return DisBasicFixedWingAircraftRecord::IsValid();
}

DisEnhancedFixedWingAircraftRecord* DisEnhancedFixedWingAircraftRecord::Clone() const
{
   return new DisEnhancedFixedWingAircraftRecord(*this);
}

DisUint8 DisEnhancedFixedWingAircraftRecord::GetSupplementalFuelStatus() const
{
   return mSupplementalFuelStatus;
}

DisUint8 DisEnhancedFixedWingAircraftRecord::GetAirMaintenanceStatus() const
{
   return mAirMaintenanceStatus;
}

DisUint8 DisEnhancedFixedWingAircraftRecord::GetPrimaryAmmunition() const
{
   return mPrimaryAmmunition;
}

DisUint8 DisEnhancedFixedWingAircraftRecord::GetSecondaryAmmunition() const
{
   return mSecondaryAmmunition;
}

void DisEnhancedFixedWingAircraftRecord::SetSupplementalFuelStatus(const DisUint8 aFuelStatus)
{
   mSupplementalFuelStatus = aFuelStatus;
}

void DisEnhancedFixedWingAircraftRecord::SetAirMaintenanceStatus(const DisUint8 aAirMaintenanceStatus)
{
   mAirMaintenanceStatus = aAirMaintenanceStatus;
}

void DisEnhancedFixedWingAircraftRecord::SetPrimaryAmmunition(const DisUint8 aPrimaryAmmunition)
{
   mPrimaryAmmunition = aPrimaryAmmunition;
}

void DisEnhancedFixedWingAircraftRecord::SetSecondaryAmmunition(const DisUint8 aSecondaryAmmunition)
{
   mSecondaryAmmunition = aSecondaryAmmunition;
}

void DisEnhancedFixedWingAircraftRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mSupplementalFuelStatus;
   aGenI >> mAirMaintenanceStatus;
   aGenI >> mPrimaryAmmunition;
   aGenI >> mSecondaryAmmunition;
}

// Operators
bool DisEnhancedFixedWingAircraftRecord::operator==(const DisEnhancedFixedWingAircraftRecord& aRhs) const
{
   return ((DisBasicFixedWingAircraftRecord::operator==(aRhs)) &&
           (mSupplementalFuelStatus == aRhs.mSupplementalFuelStatus) &&
           (mAirMaintenanceStatus == aRhs.mAirMaintenanceStatus) && (mPrimaryAmmunition == aRhs.mPrimaryAmmunition) &&
           (mSecondaryAmmunition == aRhs.mSecondaryAmmunition));
}
