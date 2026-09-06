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

#include "DisSilentEntitySystemRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisSilentEntitySystemRecord::DisSilentEntitySystemRecord()
   : mEntityType()
   , mAppearanceRecords()
{
}

DisSilentEntitySystemRecord::DisSilentEntitySystemRecord(GenI& aGenI)
   : mEntityType()
   , mAppearanceRecords()
{
   GetMemberData(aGenI);
}

DisSilentEntitySystemRecord::DisSilentEntitySystemRecord(const DisSilentEntitySystemRecord& aSrc)
   : mNumberOfEntities(aSrc.mNumberOfEntities)
   , mNumberOfAppearanceRecords(aSrc.mNumberOfAppearanceRecords)
   , mEntityType(aSrc.mEntityType)
   , mAppearanceRecords(aSrc.mAppearanceRecords)
{
}

void DisSilentEntitySystemRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisSilentEntitySystemRecord::GetLength()
{
   // unsigned type (U) specified to address Fortify false positive
   return cRECORD_BASE_LENGTH_IN_OCTETS + (mNumberOfAppearanceRecords * 4U);
}

void DisSilentEntitySystemRecord::Put(GenO& aGenO) const
{
   aGenO << mNumberOfEntities;
   aGenO << mNumberOfAppearanceRecords;
   mEntityType.Put(aGenO);
   for (int i = 0; i < mNumberOfAppearanceRecords; i++)
   {
      aGenO << mAppearanceRecords.at(i);
   }
}

bool DisSilentEntitySystemRecord::IsValid() const
{
   return mEntityType.IsValid();
}

DisSilentEntitySystemRecord* DisSilentEntitySystemRecord::Clone() const
{
   return new DisSilentEntitySystemRecord(*this);
}

void DisSilentEntitySystemRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << '\n';
}

std::string DisSilentEntitySystemRecord::ToString()
{
   std::string returnString = "-------Silent Entity System Record-------\n";
   returnString += "Number of entities:           " + std::to_string(mNumberOfEntities) + "\n";
   returnString += "Number of appearance records: " + std::to_string(mNumberOfAppearanceRecords) + "\n";
   returnString += mEntityType.ToString();
   returnString += "-------Appearance Record Contents-------\n";
   for (int i = 0; i < mNumberOfAppearanceRecords; i++)
   {
      returnString += "Record number " + std::to_string(i) + ": " + std::to_string(mAppearanceRecords.at(i)) + "\n";
   }
   returnString += "-----End Appearance Record Contents-----\n";
   returnString += "-----End Silent Entity System Record-----\n";
   return returnString;
}

DisUint16 DisSilentEntitySystemRecord::GetNumberOfEntities() const
{
   return mNumberOfEntities;
}

DisUint16 DisSilentEntitySystemRecord::GetNumberOfAppearanceRecords() const
{
   return mNumberOfAppearanceRecords;
}

const DisEntityType& DisSilentEntitySystemRecord::GetEntityType() const
{
   return mEntityType;
}

std::vector<DisUint32> DisSilentEntitySystemRecord::GetAppearanceRecords() const
{
   return mAppearanceRecords;
}

const bool DisSilentEntitySystemRecord::GetAppearanceRecordAtIndex(DisUint16 aIndex, DisUint32& aReturnValue) const
{
   if (aIndex >= 0 && aIndex < static_cast<int>(mAppearanceRecords.size()))
   {
      aReturnValue = mAppearanceRecords[aIndex];
      return true;
   }

   return false;
}

void DisSilentEntitySystemRecord::SetNumberOfEntities(DisUint16 aNumberOfEntities)
{
   mNumberOfEntities = aNumberOfEntities;
}

void DisSilentEntitySystemRecord::SetNumberOfAppearanceRecords(DisUint16 aNumberOfAppearanceRecords)
{
   mNumberOfAppearanceRecords = aNumberOfAppearanceRecords;
}

void DisSilentEntitySystemRecord::SetEntityType(const DisEntityType& aEntityType)
{
   mEntityType = aEntityType;
}

void DisSilentEntitySystemRecord::SetAppearanceRecords(const std::vector<DisUint32>& aAppearanceRecords)
{
   mAppearanceRecords = aAppearanceRecords;
}

void DisSilentEntitySystemRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mNumberOfEntities;
   aGenI >> mNumberOfAppearanceRecords;
   mEntityType.Get(aGenI);
   for (int i = 0; i < mNumberOfAppearanceRecords; i++)
   {
      DisUint32 temp = 0;
      aGenI >> temp;
      mAppearanceRecords.push_back(temp);
   }
}

// Operators
bool DisSilentEntitySystemRecord::operator==(const DisSilentEntitySystemRecord& aRhs) const
{
   return ((mNumberOfEntities == aRhs.mNumberOfEntities) &&
           (mNumberOfAppearanceRecords == aRhs.mNumberOfAppearanceRecords) && (mEntityType == aRhs.mEntityType));
}
