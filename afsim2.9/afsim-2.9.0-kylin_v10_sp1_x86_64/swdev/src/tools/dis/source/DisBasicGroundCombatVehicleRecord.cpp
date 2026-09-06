//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisBasicGroundCombatVehicleRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisBasicGroundCombatVehicleRecord::DisBasicGroundCombatVehicleRecord(GenI& aGenI)
   : DisGroupedEntityDescriptionRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisBasicGroundCombatVehicleRecord::DisBasicGroundCombatVehicleRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                     GenI&                                    aGenI)
   : DisGroupedEntityDescriptionRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisBasicGroundCombatVehicleRecord::DisBasicGroundCombatVehicleRecord(const DisBasicGroundCombatVehicleRecord& aSrc)
   : DisGroupedEntityDescriptionRecord(aSrc)
   , mEntitySpeed(aSrc.mEntitySpeed)
   , mTurretAzimuth(aSrc.mTurretAzimuth)
   , mGunElevation(aSrc.mGunElevation)
   , mTurretSlewRate(aSrc.mTurretSlewRate)
   , mGunElevationRate(aSrc.mGunElevationRate)
{
}

void DisBasicGroundCombatVehicleRecord::Get(GenI& aGenI)
{
   DisGroupedEntityDescriptionRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisBasicGroundCombatVehicleRecord::GetLength()
{
   return DisGroupedEntityDescriptionRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisBasicGroundCombatVehicleRecord::Put(GenO& aGenO) const
{
   DisGroupedEntityDescriptionRecord::Put(aGenO);
   aGenO << mEntitySpeed;
   aGenO << mTurretAzimuth;
   aGenO << mGunElevation;
   aGenO << mTurretSlewRate;
   aGenO << mGunElevationRate;
}

bool DisBasicGroundCombatVehicleRecord::IsValid() const
{
   return DisGroupedEntityDescriptionRecord::IsValid();
}

DisBasicGroundCombatVehicleRecord* DisBasicGroundCombatVehicleRecord::Clone() const
{
   return new DisBasicGroundCombatVehicleRecord(*this);
}

std::string DisBasicGroundCombatVehicleRecord::ToString() const
{
   std::string returnString = DisGroupedEntityDescriptionRecord::ToString() + '\n';
   returnString += "Entity Speed:       " + std::to_string(mEntitySpeed) + '\n';
   returnString += "Turret Azimuth:     " + std::to_string(mTurretAzimuth) + '\n';
   returnString += "Gun Elevation:      " + std::to_string(mGunElevation) + '\n';
   returnString += "Turret Slew Rate:   " + std::to_string(mTurretSlewRate) + '\n';
   returnString += "Gun Elevation Rate: " + std::to_string(mGunElevationRate) + '\n';
   return returnString;
}

DisInt8 DisBasicGroundCombatVehicleRecord::GetSpeed() const
{
   return mEntitySpeed;
}

DisInt8 DisBasicGroundCombatVehicleRecord::GetTurretAzimuth() const
{
   return mTurretAzimuth;
}

DisInt8 DisBasicGroundCombatVehicleRecord::GetGunElevation() const
{
   return mGunElevation;
}

DisInt8 DisBasicGroundCombatVehicleRecord::GetTurretSlewRate() const
{
   return mTurretSlewRate;
}

DisInt8 DisBasicGroundCombatVehicleRecord::GetGunElevationRate() const
{
   return mGunElevationRate;
}

void DisBasicGroundCombatVehicleRecord::SetSpeed(const DisInt8 aSpeed)
{
   mEntitySpeed = aSpeed;
}

void DisBasicGroundCombatVehicleRecord::SetTurretAzimuth(const DisInt8 aTurretAzimuth)
{
   mTurretAzimuth = aTurretAzimuth;
}

void DisBasicGroundCombatVehicleRecord::SetGunElevation(const DisInt8 aGunElevation)
{
   mGunElevation = aGunElevation;
}

void DisBasicGroundCombatVehicleRecord::SetTurretSlewRate(const DisInt8 aTurretSlewRate)
{
   mTurretSlewRate = aTurretSlewRate;
}

void DisBasicGroundCombatVehicleRecord::SetGunElevationRate(const DisInt8 aGunElevationRate)
{
   mGunElevationRate = aGunElevationRate;
}

void DisBasicGroundCombatVehicleRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mEntitySpeed;
   aGenI >> mTurretAzimuth;
   aGenI >> mGunElevation;
   aGenI >> mTurretSlewRate;
   aGenI >> mGunElevationRate;
}

// Operators
bool DisBasicGroundCombatVehicleRecord::operator==(const DisBasicGroundCombatVehicleRecord& aRhs) const
{
   return ((DisGroupedEntityDescriptionRecord::operator==(aRhs)) && (mEntitySpeed == aRhs.mEntitySpeed) &&
           (mTurretAzimuth == aRhs.mTurretAzimuth) && (mGunElevation == aRhs.mGunElevation) &&
           (mTurretSlewRate == aRhs.mTurretSlewRate) && (mGunElevationRate == aRhs.mGunElevationRate));
}
