// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisEntityType.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

unsigned int DisEntityType::sEnumDocSort = 1;

DisEntityType::DisEntityType(DisEnum8  aEntityKind,
                             DisEnum8  aDomain,
                             DisEnum16 aCountry,
                             DisEnum8  aCategory,
                             DisEnum8  aSubcategory,
                             DisEnum8  aSpecific,
                             DisEnum8  aExtra)
   : mEntityKind(aEntityKind)
   , mDomain(aDomain)
   , mCountry(aCountry)
   , mCategory(aCategory)
   , mSubcategory(aSubcategory)
   , mSpecific(aSpecific)
   , mExtra(aExtra)
{
}

DisEntityType::DisEntityType(const DisEntityType& aSrc)
   : mEntityKind(aSrc.mEntityKind)
   , mDomain(aSrc.mDomain)
   , mCountry(aSrc.mCountry)
   , mCategory(aSrc.mCategory)
   , mSubcategory(aSrc.mSubcategory)
   , mSpecific(aSrc.mSpecific)
   , mExtra(aSrc.mExtra)
{
}

// Static Function
void DisEntityType::SetEnumDocSortOption()
{
   sEnumDocSort = 1;
}


// Static Function
void DisEntityType::SetLeftToRightSortOption()
{
   sEnumDocSort = 0;
}

void DisEntityType::Get(GenI& aGenI)
{
   aGenI >> mEntityKind;
   aGenI >> mDomain;
   aGenI >> mCountry;
   aGenI >> mCategory;
   aGenI >> mSubcategory;
   aGenI >> mSpecific;
   aGenI >> mExtra;
}

void DisEntityType::Put(GenO& aGenO) const
{
   aGenO << mEntityKind;
   aGenO << mDomain;
   aGenO << mCountry;
   aGenO << mCategory;
   aGenO << mSubcategory;
   aGenO << mSpecific;
   aGenO << mExtra;
}

void DisEntityType::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisEntityType::ToString() const
{
   return std::to_string(mEntityKind) + ':' + std::to_string(mDomain) + ':' + std::to_string(mCountry) + ':' +
          std::to_string(mCategory) + ':' + std::to_string(mSubcategory) + ':' + std::to_string(mSpecific) + ':' +
          std::to_string(mExtra);
}

DisEntityType& DisEntityType::operator=(const DisEntityType& aRhs)
{
   if (this != &aRhs)
   {
      mEntityKind  = aRhs.mEntityKind;
      mDomain      = aRhs.mDomain;
      mCountry     = aRhs.mCountry;
      mCategory    = aRhs.mCategory;
      mSubcategory = aRhs.mSubcategory;
      mSpecific    = aRhs.mSpecific;
      mExtra       = aRhs.mExtra;
   }
   return *this;
}

bool DisEntityType::operator<(const DisEntityType& aRhs) const
{
   if (mEntityKind > aRhs.mEntityKind)
   {
      return false;
   }
   if (mEntityKind < aRhs.mEntityKind)
   {
      return true;
   }

   if (sEnumDocSort)
   {
      if (mCountry > aRhs.mCountry)
      {
         return false;
      }
      if (mCountry < aRhs.mCountry)
      {
         return true;
      }

      if (mDomain > aRhs.mDomain)
      {
         return false;
      }
      if (mDomain < aRhs.mDomain)
      {
         return true;
      }
   }
   else
   {
      if (mDomain > aRhs.mDomain)
      {
         return false;
      }
      if (mDomain < aRhs.mDomain)
      {
         return true;
      }

      if (mCountry > aRhs.mCountry)
      {
         return false;
      }
      if (mCountry < aRhs.mCountry)
      {
         return true;
      }
   }

   if (mCategory > aRhs.mCategory)
   {
      return false;
   }
   if (mCategory < aRhs.mCategory)
   {
      return true;
   }

   if (mSubcategory > aRhs.mSubcategory)
   {
      return false;
   }
   if (mSubcategory < aRhs.mSubcategory)
   {
      return true;
   }

   if (mSpecific > aRhs.mSpecific)
   {
      return false;
   }
   if (mSpecific < aRhs.mSpecific)
   {
      return true;
   }

   if (mExtra >= aRhs.mExtra)
   {
      return false;
   }
   else
   {
      return true;
   }
}

bool DisEntityType::IsValid() const
{
   return true;
}
