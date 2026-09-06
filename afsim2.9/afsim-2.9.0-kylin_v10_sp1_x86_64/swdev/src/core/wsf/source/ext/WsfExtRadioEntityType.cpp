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

#include "ext/WsfExtRadioEntityType.hpp"

#include <iostream>

WsfExtRadioEntityType::WsfExtRadioEntityType()
   : mEntityKind()
   , mDomain()
   , mCountry()
   , mCategory()
   , mNomenclatureVersion()
   , mNomenclature()
{
}

WsfExtRadioEntityType::WsfExtRadioEntityType(uint8_t  aEntityKind,
                                             uint8_t  aDomain,
                                             uint16_t aCountry,
                                             uint8_t  aCategory,
                                             uint8_t  aNomenclatureVersion,
                                             uint16_t aNomenclature)
   : mEntityKind(aEntityKind)
   , mDomain(aDomain)
   , mCountry(aCountry)
   , mCategory(aCategory)
   , mNomenclatureVersion(aNomenclatureVersion)
   , mNomenclature(aNomenclature)
{
}

bool WsfExtRadioEntityType::operator<(const WsfExtRadioEntityType& aRhs) const
{
   if (mEntityKind > aRhs.mEntityKind)
   {
      return false;
   }
   if (mEntityKind < aRhs.mEntityKind)
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

   if (mDomain > aRhs.mDomain)
   {
      return false;
   }
   if (mDomain < aRhs.mDomain)
   {
      return true;
   }

   if (mCategory > aRhs.mCategory)
   {
      return false;
   }
   if (mCategory < aRhs.mCategory)
   {
      return true;
   }

   if (mNomenclatureVersion > aRhs.mNomenclatureVersion)
   {
      return false;
   }
   if (mNomenclatureVersion < aRhs.mNomenclatureVersion)
   {
      return true;
   }

   if (mNomenclature > aRhs.mNomenclature)
   {
      return false;
   }
   if (mNomenclature < aRhs.mNomenclature)
   {
      return true;
   }
   return false;
}

bool WsfExtRadioEntityType::operator==(const WsfExtRadioEntityType& aRhs) const
{
   return mEntityKind == aRhs.mEntityKind && mDomain == aRhs.mDomain && mCountry == aRhs.mCountry &&
          mCategory == aRhs.mCategory && mNomenclatureVersion == aRhs.mNomenclatureVersion &&
          mNomenclature == aRhs.mNomenclature;
}

std::ostream& operator<<(std::ostream& aOut, const WsfExtRadioEntityType& aType)
{
   // clang-format off
   aOut << static_cast<unsigned int>(aType.mEntityKind)  << ':'
        << static_cast<unsigned int>(aType.mDomain)      << ':'
        << static_cast<unsigned int>(aType.mCountry)     << ':'
        << static_cast<unsigned int>(aType.mCategory)    << ':'
        << static_cast<unsigned int>(aType.mNomenclatureVersion) << ':'
        << static_cast<unsigned int>(aType.mNomenclature);
   // clang-format on
   return aOut;
}
