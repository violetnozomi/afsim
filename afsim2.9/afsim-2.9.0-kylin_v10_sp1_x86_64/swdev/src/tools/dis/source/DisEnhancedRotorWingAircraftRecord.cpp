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

#include "DisEnhancedRotorWingAircraftRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEnhancedRotorWingAircraftRecord::DisEnhancedRotorWingAircraftRecord(GenI& aGenI)
   : DisBasicRotorWingAircraftRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisEnhancedRotorWingAircraftRecord::DisEnhancedRotorWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                       GenI&                                    aGenI)
   : DisBasicRotorWingAircraftRecord(aRecord, aGenI)
{
   GetMemberData(aGenI);
}


DisEnhancedRotorWingAircraftRecord::DisEnhancedRotorWingAircraftRecord(const DisBasicRotorWingAircraftRecord& aRecord,
                                                                       GenI&                                  aGenI)
   : DisBasicRotorWingAircraftRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisEnhancedRotorWingAircraftRecord::DisEnhancedRotorWingAircraftRecord(const DisEnhancedRotorWingAircraftRecord& aSrc)
   : DisBasicRotorWingAircraftRecord(aSrc)
   , mSupplementalFuelStatus(aSrc.mSupplementalFuelStatus)
   , mAirMaintenanceStatus(aSrc.mAirMaintenanceStatus)
   , mPrimaryAmmunitionRemaining(aSrc.mPrimaryAmmunitionRemaining)
   , mSecondaryAmmunitionRemaining(aSrc.mSecondaryAmmunitionRemaining)
{
}

void DisEnhancedRotorWingAircraftRecord::Get(GenI& aGenI)
{
   DisBasicRotorWingAircraftRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEnhancedRotorWingAircraftRecord::GetLength()
{
   return DisBasicRotorWingAircraftRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisEnhancedRotorWingAircraftRecord::Put(GenO& aGenO) const
{
   DisBasicRotorWingAircraftRecord::Put(aGenO);
   aGenO << mSupplementalFuelStatus;
   aGenO << mAirMaintenanceStatus;
   aGenO << mPrimaryAmmunitionRemaining;
   aGenO << mSecondaryAmmunitionRemaining;
}

bool DisEnhancedRotorWingAircraftRecord::IsValid() const
{
   return DisBasicRotorWingAircraftRecord::IsValid();
}

DisEnhancedRotorWingAircraftRecord* DisEnhancedRotorWingAircraftRecord::Clone() const
{
   return new DisEnhancedRotorWingAircraftRecord(*this);
}

std::string DisEnhancedRotorWingAircraftRecord::ToString() const
{
   std::string returnString = DisBasicRotorWingAircraftRecord::ToString() + '\n';
   returnString += "Supplementatl Fuel Status:      " + std::to_string(mSupplementalFuelStatus) + '\n';
   returnString += "Air Maintenance Status:         " + std::to_string(mAirMaintenanceStatus) + '\n';
   returnString += "Primary Ammunition Remaining:   " + std::to_string(mPrimaryAmmunitionRemaining) + '\n';
   returnString += "Secondary Ammunition Remaining: " + std::to_string(mSecondaryAmmunitionRemaining) + '\n';
   return returnString;
}

DisUint8 DisEnhancedRotorWingAircraftRecord::GetSupplementalFuelStatus() const
{
   return mSupplementalFuelStatus;
}

DisUint8 DisEnhancedRotorWingAircraftRecord::GetAirMaintenanceStatus() const
{
   return mAirMaintenanceStatus;
}

DisUint8 DisEnhancedRotorWingAircraftRecord::GetPrimaryAmmunitionRemaining() const
{
   return mPrimaryAmmunitionRemaining;
}

DisUint8 DisEnhancedRotorWingAircraftRecord::GetSecondaryAmmunitionRemaining() const
{
   return mSecondaryAmmunitionRemaining;
}

void DisEnhancedRotorWingAircraftRecord::SetSupplementalFuelStatus(DisUint8 aSupplementalFuelStatus)
{
   mSupplementalFuelStatus = aSupplementalFuelStatus;
}

void DisEnhancedRotorWingAircraftRecord::SetAirMaintenanceStatus(DisUint8 aAirMaintenanceStatus)
{
   mAirMaintenanceStatus = aAirMaintenanceStatus;
}

void DisEnhancedRotorWingAircraftRecord::SetPrimaryAmmunitionRemaining(DisUint8 aPrimaryAmmunitionRemaining)
{
   mPrimaryAmmunitionRemaining = aPrimaryAmmunitionRemaining;
}

void DisEnhancedRotorWingAircraftRecord::SetSecondaryAmmunitionRemaining(DisUint8 aSecondaryAmmunitionRemaining)
{
   mSecondaryAmmunitionRemaining = aSecondaryAmmunitionRemaining;
}

void DisEnhancedRotorWingAircraftRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mSupplementalFuelStatus;
   aGenI >> mAirMaintenanceStatus;
   aGenI >> mPrimaryAmmunitionRemaining;
   aGenI >> mSecondaryAmmunitionRemaining;
}

// Operators
bool DisEnhancedRotorWingAircraftRecord::operator==(const DisEnhancedRotorWingAircraftRecord& aRhs) const
{
   return ((DisBasicRotorWingAircraftRecord::operator==(aRhs)) &&
           (mSupplementalFuelStatus == aRhs.mSupplementalFuelStatus) &&
           (mAirMaintenanceStatus == aRhs.mAirMaintenanceStatus) &&
           (mPrimaryAmmunitionRemaining == aRhs.mPrimaryAmmunitionRemaining) &&
           (mSecondaryAmmunitionRemaining == aRhs.mSecondaryAmmunitionRemaining));
}
