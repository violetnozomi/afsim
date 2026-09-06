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

#include "DisBasicRotorWingAircraftRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisBasicRotorWingAircraftRecord::DisBasicRotorWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                 GenI&                                    aGenI)
   : DisGroupedEntityDescriptionRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisBasicRotorWingAircraftRecord::DisBasicRotorWingAircraftRecord(GenI& aGenI)
   : DisGroupedEntityDescriptionRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisBasicRotorWingAircraftRecord::DisBasicRotorWingAircraftRecord(const DisBasicRotorWingAircraftRecord& aSrc)
   : DisGroupedEntityDescriptionRecord(aSrc)
   , mFuelStatus(aSrc.mFuelStatus)
   , mHorizontalDeviation(aSrc.mHorizontalDeviation)
   , mVerticalDeviation(aSrc.mVerticalDeviation)
   , mMovementSpeed(aSrc.mMovementSpeed)
   , mTurretAzimuth(aSrc.mTurretAzimuth)
   , mGunElevation(aSrc.mGunElevation)
   , mTurretSlewRate(aSrc.mTurretSlewRate)
   , mGunElevationRate(aSrc.mGunElevationRate)
{
}

void DisBasicRotorWingAircraftRecord::Get(GenI& aGenI)
{
   DisGroupedEntityDescriptionRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisBasicRotorWingAircraftRecord::GetLength()
{
   return DisGroupedEntityDescriptionRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisBasicRotorWingAircraftRecord::Put(GenO& aGenO) const
{
   DisGroupedEntityDescriptionRecord::Put(aGenO);
   aGenO << mFuelStatus;
   aGenO << mHorizontalDeviation;
   aGenO << mVerticalDeviation;
   aGenO << mMovementSpeed;
   aGenO << mTurretAzimuth;
   aGenO << mGunElevation;
   aGenO << mTurretSlewRate;
   aGenO << mGunElevationRate;
}

bool DisBasicRotorWingAircraftRecord::IsValid() const
{
   return DisGroupedEntityDescriptionRecord::IsValid();
}

DisBasicRotorWingAircraftRecord* DisBasicRotorWingAircraftRecord::Clone() const
{
   return new DisBasicRotorWingAircraftRecord(*this);
}

std::string DisBasicRotorWingAircraftRecord::ToString() const
{
   std::string returnString = DisGroupedEntityDescriptionRecord::ToString() + '\n';
   returnString += "Fuel Status:          " + std::to_string(mFuelStatus) + '\n';
   returnString += "Horizontal Deviation: " + std::to_string(mHorizontalDeviation) + '\n';
   returnString += "Vertical Deviation:   " + std::to_string(mVerticalDeviation) + '\n';
   returnString += "Movement Speed:       " + std::to_string(mMovementSpeed) + '\n';
   returnString += "Turret Azimuth:       " + std::to_string(mTurretAzimuth) + '\n';
   returnString += "Gun Elevation:        " + std::to_string(mGunElevation) + '\n';
   returnString += "Turret Slew Rate:     " + std::to_string(mTurretSlewRate) + '\n';
   returnString += "Gun Elevation Rate:   " + std::to_string(mGunElevationRate) + '\n';
   return returnString;
}

DisUint8 DisBasicRotorWingAircraftRecord::GetFuelStatus() const
{
   return mFuelStatus;
}

DisInt8 DisBasicRotorWingAircraftRecord::GetHorizontalMovementDeviation() const
{
   return mHorizontalDeviation;
}

DisInt8 DisBasicRotorWingAircraftRecord::GetVerticalMovementDeviation() const
{
   return mVerticalDeviation;
}

DisInt16 DisBasicRotorWingAircraftRecord::GetMovementSpeed() const
{
   return mMovementSpeed;
}

DisInt8 DisBasicRotorWingAircraftRecord::GetTurretAzimuth() const
{
   return mTurretAzimuth;
}

DisInt8 DisBasicRotorWingAircraftRecord::GetGunElevation() const
{
   return mGunElevation;
}

DisInt8 DisBasicRotorWingAircraftRecord::GetTurretSlewRate() const
{
   return mTurretSlewRate;
}

DisInt8 DisBasicRotorWingAircraftRecord::GetGunElevationRate() const
{
   return mGunElevationRate;
}

void DisBasicRotorWingAircraftRecord::SetFuelStatus(const DisUint8 aFuelStatus)
{
   mFuelStatus = aFuelStatus;
}

void DisBasicRotorWingAircraftRecord::SetHorizontalDeviation(const DisInt8 aHorizontalDeviation)
{
   mHorizontalDeviation = aHorizontalDeviation;
}

void DisBasicRotorWingAircraftRecord::SetVerticalDeviation(const DisInt8 aVerticalDeviation)
{
   mVerticalDeviation = aVerticalDeviation;
}

void DisBasicRotorWingAircraftRecord::SetMovementSpeed(const DisInt16 aMovementSpeed)
{
   mMovementSpeed = aMovementSpeed;
}

void DisBasicRotorWingAircraftRecord::SetTurretAzimuth(const DisInt8 aTurretAzimuth)
{
   mTurretAzimuth = aTurretAzimuth;
}

void DisBasicRotorWingAircraftRecord::SetGunElevation(const DisInt8 aGunElevation)
{
   mGunElevation = aGunElevation;
}

void DisBasicRotorWingAircraftRecord::SetTurretSlewRate(const DisInt8 aTurretSlewRate)
{
   mTurretSlewRate = aTurretSlewRate;
}

void DisBasicRotorWingAircraftRecord::SetGunElevationRate(const DisInt8 aGunElevationRate)
{
   mGunElevationRate = aGunElevationRate;
}

void DisBasicRotorWingAircraftRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mFuelStatus;
   aGenI >> mHorizontalDeviation;
   aGenI >> mVerticalDeviation;
   aGenI >> mMovementSpeed;
   aGenI >> mTurretAzimuth;
   aGenI >> mGunElevation;
   aGenI >> mTurretSlewRate;
   aGenI >> mGunElevationRate;
}

// Operators
bool DisBasicRotorWingAircraftRecord::operator==(const DisBasicRotorWingAircraftRecord& aRhs) const
{
   return ((DisGroupedEntityDescriptionRecord::operator==(aRhs)) && (mFuelStatus == aRhs.mFuelStatus) &&
           (mHorizontalDeviation == aRhs.mHorizontalDeviation) && (mVerticalDeviation == aRhs.mVerticalDeviation) &&
           (mMovementSpeed == aRhs.mMovementSpeed) && (mTurretAzimuth == aRhs.mTurretAzimuth) &&
           (mGunElevation == aRhs.mGunElevation) && (mTurretSlewRate == aRhs.mTurretSlewRate) &&
           (mGunElevationRate == aRhs.mGunElevationRate));
}
