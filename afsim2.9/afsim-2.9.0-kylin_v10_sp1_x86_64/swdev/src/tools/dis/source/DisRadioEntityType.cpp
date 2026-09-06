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

#include "DisRadioEntityType.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisRadioEntityType::DisRadioEntityType()
   : mEntityKind(0)
   , mDomain(0)
   , mCountry(0)
   , mCategory(0)
   , mNomenclatureVersion(0)
   , mNomenclature(0)
{
}

DisRadioEntityType::~DisRadioEntityType() {}

void DisRadioEntityType::Get(GenI& aGenI)
{
   aGenI >> mEntityKind;
   aGenI >> mDomain;
   aGenI >> mCountry;
   aGenI >> mCategory;
   aGenI >> mNomenclatureVersion;
   aGenI >> mNomenclature;
}

void DisRadioEntityType::Put(GenO& aGenO) const
{
   aGenO << mEntityKind;
   aGenO << mDomain;
   aGenO << mCountry;
   aGenO << mCategory;
   aGenO << mNomenclatureVersion;
   aGenO << mNomenclature;
}

void DisRadioEntityType::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

std::string DisRadioEntityType::ToString() const
{
   std::string returnString;
   returnString += "Entity Kind:           " + std::to_string(mEntityKind) + "\n";
   returnString += "Domain:                " + std::to_string(mDomain) + "\n";
   returnString += "Country:               " + std::to_string(mCountry) + "\n";
   returnString += "Category:              " + std::to_string(mCategory) + "\n";
   returnString += "Nomen-Clature Version: " + std::to_string(mCategory) + "\n";
   returnString += "Nomen-Clature:         " + std::to_string(mNomenclature) + "\n";
   return returnString;
}

bool DisRadioEntityType::IsValid() const
{
   return true;
}
