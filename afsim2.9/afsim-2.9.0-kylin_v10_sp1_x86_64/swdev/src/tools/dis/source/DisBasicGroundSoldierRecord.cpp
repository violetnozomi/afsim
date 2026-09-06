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

#include "DisBasicGroundSoldierRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisBasicGroundSoldierRecord::DisBasicGroundSoldierRecord(GenI& aGenI)
   : DisGroupedEntityDescriptionRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisBasicGroundSoldierRecord::DisBasicGroundSoldierRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI)
   : DisGroupedEntityDescriptionRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisBasicGroundSoldierRecord::DisBasicGroundSoldierRecord(const DisBasicGroundSoldierRecord& aSrc)
   : DisGroupedEntityDescriptionRecord(aSrc)
   , mEntitySpeed(aSrc.mEntitySpeed)
   , mHeadAzimuth(aSrc.mHeadAzimuth)
   , mHeadElevation(aSrc.mHeadElevation)
   , mHeadScanRate(aSrc.mHeadScanRate)
   , mHeadElevationRate(aSrc.mHeadElevationRate)
{
}

void DisBasicGroundSoldierRecord::Get(GenI& aGenI)
{
   DisGroupedEntityDescriptionRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisBasicGroundSoldierRecord::GetLength()
{
   return DisGroupedEntityDescriptionRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisBasicGroundSoldierRecord::Put(GenO& aGenO) const
{
   DisGroupedEntityDescriptionRecord::Put(aGenO);
   aGenO << mEntitySpeed;
   aGenO << mHeadAzimuth;
   aGenO << mHeadElevation;
   aGenO << mHeadScanRate;
   aGenO << mHeadElevationRate;
}

bool DisBasicGroundSoldierRecord::IsValid() const
{
   return DisGroupedEntityDescriptionRecord::IsValid();
}

DisBasicGroundSoldierRecord* DisBasicGroundSoldierRecord::Clone() const
{
   return new DisBasicGroundSoldierRecord(*this);
}

void DisBasicGroundSoldierRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mEntitySpeed;
   aGenI >> mHeadAzimuth;
   aGenI >> mHeadElevation;
   aGenI >> mHeadScanRate;
   aGenI >> mHeadElevationRate;
}

DisInt8 DisBasicGroundSoldierRecord::GetSpeed() const
{
   return mEntitySpeed;
}

DisInt8 DisBasicGroundSoldierRecord::GetHeadAzimuth() const
{
   return mHeadAzimuth;
}

DisInt8 DisBasicGroundSoldierRecord::GetHeadElevation() const
{
   return mHeadElevation;
}

DisInt8 DisBasicGroundSoldierRecord::GetHeadScanRate() const
{
   return mHeadScanRate;
}

DisInt8 DisBasicGroundSoldierRecord::GetHeadElevationRate() const
{
   return mHeadElevationRate;
}

void DisBasicGroundSoldierRecord::SetSpeed(const DisInt8 aSpeed)
{
   mEntitySpeed = aSpeed;
}

void DisBasicGroundSoldierRecord::SetHeadAzimuth(const DisInt8 aHeadAzimuth)
{
   mHeadAzimuth = aHeadAzimuth;
}

void DisBasicGroundSoldierRecord::SetHeadElevation(const DisInt8 HeadElevation)
{
   mHeadElevation = HeadElevation;
}

void DisBasicGroundSoldierRecord::SetHeadScanRate(const DisInt8 aHeadScanRate)
{
   mHeadScanRate = aHeadScanRate;
}

void DisBasicGroundSoldierRecord::SetHeadElevationRate(const DisInt8 aHeadElevationRate)
{
   mHeadElevationRate = aHeadElevationRate;
}

std::string DisBasicGroundSoldierRecord::ToString() const
{
   std::string returnString = DisGroupedEntityDescriptionRecord::ToString() + '\n';
   returnString += "Entity Speed:              " + std::to_string(mEntitySpeed) + '\n';
   returnString += "Head Azimuth:              " + std::to_string(mHeadAzimuth) + '\n';
   returnString += "Head Elevation:            " + std::to_string(mHeadElevation) + '\n';
   returnString += "Head Scan Rate:            " + std::to_string(mHeadScanRate) + '\n';
   returnString += "Head Elevation Rate:       " + std::to_string(mHeadElevationRate) + '\n';
   return returnString;
}

// Operators
bool DisBasicGroundSoldierRecord::operator==(const DisBasicGroundSoldierRecord& aRhs) const
{
   return ((DisGroupedEntityDescriptionRecord::operator==(aRhs)) && (mEntitySpeed == aRhs.mEntitySpeed) &&
           (mHeadAzimuth == aRhs.mHeadAzimuth) && (mHeadElevation == aRhs.mHeadElevation) &&
           (mHeadScanRate == aRhs.mHeadScanRate) && (mHeadElevationRate == aRhs.mHeadElevationRate));
}
