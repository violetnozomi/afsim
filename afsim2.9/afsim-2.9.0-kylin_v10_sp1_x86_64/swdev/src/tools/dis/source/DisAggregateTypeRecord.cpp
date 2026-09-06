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

#include "DisAggregateTypeRecord.hpp"

#include "DisValidationUtils.hpp"


DisAggregateTypeRecord::DisAggregateTypeRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisAggregateTypeRecord::DisAggregateTypeRecord(const DisAggregateTypeRecord& aSrc)
   : mAggregateKind(aSrc.mAggregateKind)
   , mDomain(aSrc.mDomain)
   , mCountry(aSrc.mCountry)
   , mCategory(aSrc.mCategory)
   , mSubCategory(aSrc.mSubCategory)
   , mSpecific(aSrc.mSpecific)
   , mExtra(aSrc.mExtra)
{
}

void DisAggregateTypeRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisAggregateTypeRecord::GetLength()
{
   return cRECORD_BASE_LENGTH_IN_OCTETS;
}

void DisAggregateTypeRecord::Put(GenO& aGenO) const
{
   aGenO << mAggregateKind;
   aGenO << mDomain;
   aGenO << mCountry;
   aGenO << mCategory;
   aGenO << mSubCategory;
   aGenO << mSpecific;
   aGenO << mExtra;
}

bool DisAggregateTypeRecord::IsValid() const
{
   return true;
}

DisAggregateTypeRecord* DisAggregateTypeRecord::Clone() const
{
   return new DisAggregateTypeRecord(*this);
}

void DisAggregateTypeRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << '\n';
}

std::string DisAggregateTypeRecord::ToString() const
{
   std::string returnString = "-------Dis Aggregate State-------\n";
   returnString += "Aggregate Kind: " + std::to_string(mAggregateKind) + '\n';
   returnString += "Domain:         " + std::to_string(mDomain) + '\n';
   returnString += "Country:        " + std::to_string(mCountry) + '\n';
   returnString += "Category:       " + std::to_string(mCategory) + '\n';
   returnString += "Sub Category:   " + std::to_string(mSubCategory) + '\n';
   returnString += "Specific:       " + std::to_string(mSpecific) + '\n';
   returnString += "Extra:          " + std::to_string(mExtra) + '\n';
   returnString += "-----End Dis Aggregate State-----\n";
   return returnString;
}

DisEnum8 DisAggregateTypeRecord::GetAggregateKind() const
{
   return mAggregateKind;
}

DisEnum8 DisAggregateTypeRecord::GetDomain() const
{
   return mDomain;
}

DisEnum16 DisAggregateTypeRecord::GetCountry() const
{
   return mCountry;
}

DisEnum8 DisAggregateTypeRecord::GetCategory() const
{
   return mCategory;
}

DisEnum8 DisAggregateTypeRecord::GetSubCategory() const
{
   return mSubCategory;
}

DisEnum8 DisAggregateTypeRecord::GetSpecific() const
{
   return mSpecific;
}

DisEnum8 DisAggregateTypeRecord::GetExtra() const
{
   return mExtra;
}

void DisAggregateTypeRecord::SetAggregateKind(const DisEnum8 aAggregateKind)
{
   mAggregateKind = mAggregateKind;
}

void DisAggregateTypeRecord::SetDomain(const DisEnum8 aDomain)
{
   mDomain = aDomain;
}

void DisAggregateTypeRecord::SetCountry(const DisEnum16 aCountry)
{
   mCountry = aCountry;
}

void DisAggregateTypeRecord::SetCategory(const DisEnum8 aCategory)
{
   mCategory = aCategory;
}

void DisAggregateTypeRecord::SetSubCategory(const DisEnum8 aSubCategory)
{
   mSubCategory = aSubCategory;
}

void DisAggregateTypeRecord::SetSpecific(const DisEnum8 aSpecific)
{
   mSpecific = aSpecific;
}

void DisAggregateTypeRecord::SetExtra(const DisEnum8 aExtra)
{
   mExtra = aExtra;
}

void DisAggregateTypeRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mAggregateKind;
   aGenI >> mDomain;
   aGenI >> mCountry;
   aGenI >> mCategory;
   aGenI >> mSubCategory;
   aGenI >> mSpecific;
   aGenI >> mExtra;
}

// Operators
bool DisAggregateTypeRecord::operator==(const DisAggregateTypeRecord& aRhs) const
{
   return ((mAggregateKind == aRhs.mAggregateKind) && (mDomain == aRhs.mDomain) && (mCountry == aRhs.mCountry) &&
           (mCategory == aRhs.mCategory) && (mSubCategory == aRhs.mSubCategory) && (mSpecific == aRhs.mSpecific) &&
           (mExtra == aRhs.mExtra));
}
