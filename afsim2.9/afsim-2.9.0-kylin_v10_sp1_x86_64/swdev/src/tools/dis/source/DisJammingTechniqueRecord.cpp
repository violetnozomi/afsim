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

#include "DisJammingTechniqueRecord.hpp"

#include <iostream>

#include "DisJammingTechniqueEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisJammingTechniqueRecord::DisJammingTechniqueRecord()
   : mKind(0)
   , mCategory(0)
   , mSubcategory(0)
   , mSpecific(0)
{
}

DisJammingTechniqueRecord::DisJammingTechniqueRecord(DisEnum8 aKind, DisEnum8 aCategory, DisEnum8 aSubcategory, DisEnum8 aSpecific)
   : mKind(aKind)
   , mCategory(aCategory)
   , mSubcategory(aSubcategory)
   , mSpecific(aSpecific)
{
}

DisJammingTechniqueRecord::DisJammingTechniqueRecord(DisUint32 aValue)
   : mKind(static_cast<DisEnum8>((aValue >> 24) & 0xFF))
   , mCategory(static_cast<DisEnum8>((aValue >> 16) & 0xFF))
   , mSubcategory(static_cast<DisEnum8>((aValue >> 8) & 0xFF))
   , mSpecific(static_cast<DisEnum8>(aValue & 0xFF))
{
}

DisJammingTechniqueRecord::DisJammingTechniqueRecord(const DisJammingTechniqueRecord& aSrc)
   : mKind(aSrc.mKind)
   , mCategory(aSrc.mCategory)
   , mSubcategory(aSrc.mSubcategory)
   , mSpecific(aSrc.mSpecific)
{
}

void DisJammingTechniqueRecord::Get(GenI& aGenI)
{
   aGenI >> mKind;
   aGenI >> mCategory;
   aGenI >> mSubcategory;
   aGenI >> mSpecific;
}

void DisJammingTechniqueRecord::Put(GenO& aGenO) const
{
   aGenO << mKind;
   aGenO << mCategory;
   aGenO << mSubcategory;
   aGenO << mSpecific;
}

void DisJammingTechniqueRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}

std::string DisJammingTechniqueRecord::ToString() const
{
   std::string returnString;
   returnString += "Kind:        " + std::to_string(mKind) + "\n";
   returnString += "Category:    " + std::to_string(mCategory) + "\n";
   returnString += "SubCategory: " + std::to_string(mSubcategory) + "\n";
   returnString += "Specific:    " + std::to_string(mSpecific) + "\n";
   return returnString;
}

DisJammingTechniqueRecord& DisJammingTechniqueRecord::operator=(const DisJammingTechniqueRecord& aRhs)
{
   if (this != &aRhs)
   {
      mKind        = aRhs.mKind;
      mCategory    = aRhs.mCategory;
      mSubcategory = aRhs.mSubcategory;
      mSpecific    = aRhs.mSpecific;
   }
   return *this;
}

DisJammingTechniqueRecord& DisJammingTechniqueRecord::operator=(const DisUint32& aRhs)
{
   mKind        = static_cast<DisEnum8>((aRhs >> 24) & 0xFF);
   mCategory    = static_cast<DisEnum8>((aRhs >> 16) & 0xFF);
   mSubcategory = static_cast<DisEnum8>((aRhs >> 8) & 0xFF);
   mSpecific    = static_cast<DisEnum8>(aRhs & 0xFF);

   return *this;
}

bool DisJammingTechniqueRecord::IsValid() const
{
   return DisEnum::jamming_technique::IsValid(*this);
}
