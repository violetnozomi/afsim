// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisFlareStateRecord.hpp"

#include "DisEntityType.hpp"
#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"

DisFlareStateRecord::DisFlareStateRecord(const DisFlareStateRecord& aSrc)
   : mTimeSinceCreation(aSrc.mTimeSinceCreation)
   , mMunitionSource(aSrc.mMunitionSource)
   , mNumberIntensity(aSrc.mNumberIntensity)
   , mNumberOfSources(aSrc.mNumberOfSources)
   , mGeometryIndex(aSrc.mGeometryIndex)
{
}

DisFlareStateRecord::DisFlareStateRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisFlareStateRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisFlareStateRecord::GetLength()
{
   return cBASE_LENGTH_OCTETS;
}

void DisFlareStateRecord::Put(GenO& aGenO) const
{
   aGenO << mTimeSinceCreation;
   aGenO << mMunitionSource;
   aGenO << mNumberIntensity;
   aGenO << mNumberOfSources;
   aGenO << mGeometryIndex;
   aGenO << mPadding;
}

DisUint32 DisFlareStateRecord::GetTimeSinceCreation() const
{
   return mTimeSinceCreation;
}

DisEntityType& DisFlareStateRecord::GetMunitionSource()
{
   return mMunitionSource;
}

DisUint32 DisFlareStateRecord::GetNumberIntensity() const
{
   return mNumberIntensity;
}

DisUint32 DisFlareStateRecord::GetNumberOfSources() const
{
   return mNumberOfSources;
}

DisUint16 DisFlareStateRecord::GetGeometryIndex() const
{
   return mGeometryIndex;
}

void DisFlareStateRecord::SetTimeSinceCreation(DisUint32 aTimeSinceCreation)
{
   mTimeSinceCreation = aTimeSinceCreation;
}

void DisFlareStateRecord::SetMunitionSource(const DisEntityType& aMunitionSource)
{
   mMunitionSource = aMunitionSource;
}

void DisFlareStateRecord::SetNumberIntensity(DisUint32 aNumberIntensity)
{
   mNumberIntensity = aNumberIntensity;
}

void DisFlareStateRecord::SetNumberOfSources(DisUint32 aNumberOfSources)
{
   mNumberOfSources = aNumberOfSources;
}

void DisFlareStateRecord::SetGeometryIndex(DisUint16 aGeometryIndex)
{
   mGeometryIndex = aGeometryIndex;
}

bool DisFlareStateRecord::IsValid() const
{
   return mMunitionSource.IsValid();
}

DisFlareStateRecord* DisFlareStateRecord::Clone() const
{
   return new DisFlareStateRecord(*this);
}

std::string DisFlareStateRecord::ToString() const
{
   std::string returnString = "Time since creation: " + std::to_string(mTimeSinceCreation) + "\n";
   returnString += mMunitionSource.ToString();
   returnString += "Number Intensity: " + std::to_string(mNumberIntensity) + "\n";
   returnString += "Number Of Sources: " + std::to_string(mNumberOfSources) + "\n";
   returnString += "Geometry Index: " + std::to_string(mGeometryIndex) + "\n";
   return returnString;
}

void DisFlareStateRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

void DisFlareStateRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mTimeSinceCreation;
   aGenI >> mMunitionSource;
   aGenI >> mNumberIntensity;
   aGenI >> mNumberOfSources;
   aGenI >> mGeometryIndex;
   aGenI >> mPadding;
}
