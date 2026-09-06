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

#include "DisObjectType.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisObjectType::DisObjectType(DisEnum8 aDomain, DisEnum8 aObjectKind, DisEnum8 aCategory, DisEnum8 aSubcategory)
   : mDomain(aDomain)
   , mObjectKind(aObjectKind)
   , mCategory(aCategory)
   , mSubcategory(aSubcategory)
{
}

DisObjectType::DisObjectType(const DisObjectType& aSrc)
   : mDomain(aSrc.mDomain)
   , mObjectKind(aSrc.mObjectKind)
   , mCategory(aSrc.mCategory)
   , mSubcategory(aSrc.mSubcategory)
{
}

void DisObjectType::Get(GenI& aGenI)
{
   aGenI >> mDomain;
   aGenI >> mObjectKind;
   aGenI >> mCategory;
   aGenI >> mSubcategory;
}

void DisObjectType::Put(GenO& aGenO) const
{
   aGenO << mDomain;
   aGenO << mObjectKind;
   aGenO << mCategory;
   aGenO << mSubcategory;
}

void DisObjectType::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisObjectType::ToString() const
{
   std::string returnString = "------- Dis Object Type -------\n";
   returnString += "Domain:       " + std::to_string(mDomain) + '\n';
   returnString += "Object Kind:  " + std::to_string(mObjectKind) + '\n';
   returnString += "Category:     " + std::to_string(mCategory) + '\n';
   returnString += "Sub Category: " + std::to_string(mSubcategory) + '\n';
   returnString += "----- End Dis Object Type -----\n";
   return returnString;
}

bool DisObjectType::IsValid() const
{
   return true;
}

DisObjectType& DisObjectType::operator=(const DisObjectType& aRhs)
{
   if (this != &aRhs)
   {
      mObjectKind  = aRhs.mObjectKind;
      mDomain      = aRhs.mDomain;
      mCategory    = aRhs.mCategory;
      mSubcategory = aRhs.mSubcategory;
   }
   return *this;
}
