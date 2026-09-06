//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "DisCombicStateRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisCombicStateRecord::DisCombicStateRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mDetonationVelocity[i] = 0;
   }
}

DisCombicStateRecord::DisCombicStateRecord(const DisCombicStateRecord& aSrc)
   : mTimeSinceCreation(aSrc.mTimeSinceCreation)
   , mMunitionSource(aSrc.mMunitionSource)
   , mNumberOfSources(aSrc.mNumberOfSources)
   , mGeometryIndex(aSrc.mGeometryIndex)
   , mPadding(0)
   , mSourceType(aSrc.mSourceType)
   , mBarrageRate(aSrc.mBarrageRate)
   , mBarrageDuration(aSrc.mBarrageDuration)
   , mBarrageCrosswindLength(aSrc.mBarrageCrosswindLength)
   , mBarrageDownwindLength(aSrc.mBarrageDownwindLength)
{
   for (int i = 0; i < 3; i++)
   {
      mDetonationVelocity[i] = aSrc.mDetonationVelocity[i];
   }
}

DisCombicStateRecord::DisCombicStateRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisCombicStateRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisCombicStateRecord::GetLength()
{
   return cMIN_LENGTH_OCTETS;
}

void DisCombicStateRecord::Put(GenO& aGenO) const
{
   aGenO << mTimeSinceCreation;
   aGenO << mMunitionSource;
   aGenO << mNumberOfSources;
   aGenO << mGeometryIndex;
   aGenO << mPadding;
   aGenO << mSourceType;
   aGenO << mBarrageRate;
   aGenO << mBarrageDuration;
   aGenO << mBarrageCrosswindLength;
   aGenO << mBarrageDownwindLength;
   aGenO << mDetonationVelocity[0] << mDetonationVelocity[1] << mDetonationVelocity[2];
   aGenO << mPadding2;
}

DisUint32 DisCombicStateRecord::GetTimeSinceCreation() const
{
   return mTimeSinceCreation;
}

DisEntityType& DisCombicStateRecord::GetMunitionSource()
{
   return mMunitionSource;
}

DisInt32 DisCombicStateRecord::GetNumberOfSources() const
{
   return mNumberOfSources;
}

DisUint16 DisCombicStateRecord::GetGeometryIndex() const
{
   return mGeometryIndex;
}

DisFloat32 DisCombicStateRecord::GetSourceType() const
{
   return mSourceType;
}

DisFloat32 DisCombicStateRecord::GetBarrageRate() const
{
   return mBarrageRate;
}

DisFloat32 DisCombicStateRecord::GetBarrageDuration() const
{
   return mBarrageDuration;
}

DisFloat32 DisCombicStateRecord::GetBarrageCrosswindLength() const
{
   return mBarrageCrosswindLength;
}

DisFloat32 DisCombicStateRecord::GetBarrageDownwindLength() const
{
   return mBarrageDownwindLength;
}

void DisCombicStateRecord::GetDetonationVelocity(DisFloat32& aDetonationVelocityX,
                                                 DisFloat32& aDetonationVelocityY,
                                                 DisFloat32& aDetonationVelocityZ)
{
   aDetonationVelocityX = mDetonationVelocity[0];
   aDetonationVelocityY = mDetonationVelocity[1];
   aDetonationVelocityZ = mDetonationVelocity[2];
}

void DisCombicStateRecord::SetTimeSinceCreation(DisUint32 aTimeSinceCreation)
{
   mTimeSinceCreation = aTimeSinceCreation;
}

void DisCombicStateRecord::SetMunitionSource(const DisEntityType& aMunitionSource)
{
   mMunitionSource = aMunitionSource;
}

void DisCombicStateRecord::SetNumberOfSources(const DisInt32 aNumberOfSources)
{
   mNumberOfSources = aNumberOfSources;
}

void DisCombicStateRecord::SetGeometryIndex(DisUint16 aGeometryIndex)
{
   mGeometryIndex = aGeometryIndex;
}

void DisCombicStateRecord::SetSourceType(DisFloat32 aSourceType)
{
   mSourceType = aSourceType;
}

void DisCombicStateRecord::SetBarrageRate(DisFloat32 aBarrageRate)
{
   mBarrageRate = aBarrageRate;
}

void DisCombicStateRecord::SetBarrageDuration(DisFloat32 aBarrageDuration)
{
   mBarrageDuration = aBarrageDuration;
}

void DisCombicStateRecord::SetBarrageCrosswindLength(DisFloat32 aBarrageCrosswindLength)
{
   mBarrageCrosswindLength = aBarrageCrosswindLength;
}

void DisCombicStateRecord::SetBarrageDownwindLength(DisFloat32 aBarrageDownwindLength)
{
   mBarrageDownwindLength = aBarrageDownwindLength;
}

void DisCombicStateRecord::SetDetonationVelocity(DisFloat32 aDetonationVelocityX,
                                                 DisFloat32 aDetonationVelocityY,
                                                 DisFloat32 aDetonationVelocityZ)
{
   mDetonationVelocity[0] = aDetonationVelocityX;
   mDetonationVelocity[1] = aDetonationVelocityY;
   mDetonationVelocity[2] = aDetonationVelocityZ;
}

bool DisCombicStateRecord::IsValid() const
{
   return mMunitionSource.IsValid() && ValidateScalar(mSourceType) && ValidateScalar(mBarrageRate) &&
          ValidateScalar(mBarrageDuration) && ValidateScalar(mBarrageCrosswindLength) &&
          ValidateScalar(mBarrageDownwindLength) && ValidateScalarArray(mDetonationVelocity, 3);
}

DisCombicStateRecord* DisCombicStateRecord::Clone() const
{
   return new DisCombicStateRecord(*this);
}

void DisCombicStateRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisCombicStateRecord::ToString() const
{
   std::string returnString = "";
   returnString += "-------Combic State Record-------\n";
   returnString += "Time Since Creation: " + std::to_string(mTimeSinceCreation) + '\n';
   returnString += mMunitionSource.ToString() + '\n';
   returnString += "Munition Source: " + std::to_string(mNumberOfSources) + '\n';
   returnString += "Geometry Index: " + std::to_string(mGeometryIndex) + '\n';
   returnString += "Source Type: " + std::to_string(mSourceType) + '\n';
   returnString += "Barrage Rate: " + std::to_string(mBarrageRate) + '\n';
   returnString += "Barrage Duration: " + std::to_string(mBarrageDuration) + '\n';
   returnString += "Barrage Crosswind Length: " + std::to_string(mBarrageCrosswindLength) + '\n';
   returnString += "Barrage Downwind Length: " + std::to_string(mBarrageDownwindLength) + '\n';
   returnString += "-------Detonation Velocity-------\n";
   returnString += "Detonation Psi: " + std::to_string(mDetonationVelocity[0]) + '\n';
   returnString += "Detonation Theta: " + std::to_string(mDetonationVelocity[1]) + '\n';
   returnString += "Detonation Phi: " + std::to_string(mDetonationVelocity[2]) + '\n';
   returnString += "-----End Detonation Velocity-----\n";
   returnString += "-----End Combic State Record-----\n";
   return returnString;
}

void DisCombicStateRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mTimeSinceCreation;
   aGenI >> mMunitionSource;
   aGenI >> mNumberOfSources;
   aGenI >> mGeometryIndex;
   aGenI >> mPadding;
   aGenI >> mSourceType;
   aGenI >> mBarrageRate;
   aGenI >> mBarrageDuration;
   aGenI >> mBarrageCrosswindLength;
   aGenI >> mBarrageDownwindLength;
   aGenI >> mDetonationVelocity[0] >> mDetonationVelocity[1] >> mDetonationVelocity[2];
   aGenI >> mPadding2;
}
