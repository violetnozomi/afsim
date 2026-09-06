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

#include "DisBasicFixedWingAircraftRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisBasicFixedWingAircraftRecord::DisBasicFixedWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                 GenI&                                    aGenI)
   : DisGroupedEntityDescriptionRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisBasicFixedWingAircraftRecord::DisBasicFixedWingAircraftRecord(GenI& aGenI)
   : DisGroupedEntityDescriptionRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisBasicFixedWingAircraftRecord::DisBasicFixedWingAircraftRecord(const DisBasicFixedWingAircraftRecord& aSrc)
   : DisGroupedEntityDescriptionRecord(aSrc)
   , mFuelStatus(aSrc.mFuelStatus)
   , mHorizontalDeviation(aSrc.mHorizontalDeviation)
   , mVerticalDeviation(aSrc.mVerticalDeviation)
   , mMovementSpeed(aSrc.mMovementSpeed)
{
}

void DisBasicFixedWingAircraftRecord::Get(GenI& aGenI)
{
   DisGroupedEntityDescriptionRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisBasicFixedWingAircraftRecord::GetLength()
{
   return DisGroupedEntityDescriptionRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisBasicFixedWingAircraftRecord::Put(GenO& aGenO) const
{
   DisGroupedEntityDescriptionRecord::Put(aGenO);
   aGenO << mFuelStatus;
   aGenO << mHorizontalDeviation;
   aGenO << mVerticalDeviation;
   aGenO << mMovementSpeed;
}

bool DisBasicFixedWingAircraftRecord::IsValid() const
{
   return DisGroupedEntityDescriptionRecord::IsValid();
}

DisBasicFixedWingAircraftRecord* DisBasicFixedWingAircraftRecord::Clone() const
{
   return new DisBasicFixedWingAircraftRecord(*this);
}

std::string DisBasicFixedWingAircraftRecord::ToString() const
{
   std::string returnString = DisGroupedEntityDescriptionRecord::ToString() + '\n';
   returnString += "Fuel Status:          " + std::to_string(mFuelStatus) + '\n';
   returnString += "Horizontal Deviation: " + std::to_string(mHorizontalDeviation) + '\n';
   returnString += "Vertical Deviation:   " + std::to_string(mVerticalDeviation) + '\n';
   returnString += "Movement Speed:       " + std::to_string(mMovementSpeed) + '\n';
   return returnString;
}

DisUint8 DisBasicFixedWingAircraftRecord::GetFuelStatus() const
{
   return mFuelStatus;
}

DisInt8 DisBasicFixedWingAircraftRecord::GetHorizontalMovementDeviation() const
{
   return mHorizontalDeviation;
}

DisInt8 DisBasicFixedWingAircraftRecord::GetVerticalMovementDeviation() const
{
   return mVerticalDeviation;
}

DisInt16 DisBasicFixedWingAircraftRecord::GetMovementSpeed() const
{
   return mMovementSpeed;
}

void DisBasicFixedWingAircraftRecord::SetFuelStatus(const DisUint8 aFuelStatus)
{
   mFuelStatus = aFuelStatus;
}

void DisBasicFixedWingAircraftRecord::SetHorizontalDeviation(const DisInt8 aHorizontalDeviation)
{
   mHorizontalDeviation = aHorizontalDeviation;
}

void DisBasicFixedWingAircraftRecord::SetVerticalDeviation(const DisInt8 aVerticalDeviation)
{
   mVerticalDeviation = aVerticalDeviation;
}

void DisBasicFixedWingAircraftRecord::SetMovementSpeed(const DisInt16 aMovementSpeed)
{
   mMovementSpeed = aMovementSpeed;
}


void DisBasicFixedWingAircraftRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mFuelStatus;
   aGenI >> mHorizontalDeviation;
   aGenI >> mVerticalDeviation;
   aGenI >> mMovementSpeed;
}

// Operators
bool DisBasicFixedWingAircraftRecord::operator==(const DisBasicFixedWingAircraftRecord& aRhs) const
{
   return ((DisGroupedEntityDescriptionRecord::operator==(aRhs)) && (mFuelStatus == aRhs.mFuelStatus) &&
           (mHorizontalDeviation == aRhs.mHorizontalDeviation) && (mVerticalDeviation == aRhs.mVerticalDeviation) &&
           (mMovementSpeed == aRhs.mMovementSpeed));
}
