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


#ifndef WSFEXTRADIOENTITYTYPE_HPP
#define WSFEXTRADIOENTITYTYPE_HPP

#include "wsf_export.h"

#include <cstdint>
#include <iosfwd>

class WSF_EXPORT WsfExtRadioEntityType
{
public:
   WsfExtRadioEntityType();
   WsfExtRadioEntityType(uint8_t  aEntityKind,
                         uint8_t  aDomain,
                         uint16_t aCountry,
                         uint8_t  aCategory,
                         uint8_t  aNomenclatureVersion,
                         uint16_t aNomenclature);
   uint8_t  GetEntityKind();
   uint8_t  GetDomain();
   uint16_t GetCountry();
   uint8_t  GetCategory();
   uint8_t  GetNomenclatureVersion();
   uint16_t GetNomenclature();

   bool     operator==(const WsfExtRadioEntityType& aRhs) const;
   bool     operator<(const WsfExtRadioEntityType& aRhs) const;
   uint8_t  mEntityKind;
   uint8_t  mDomain;
   uint16_t mCountry;
   uint8_t  mCategory;
   uint8_t  mNomenclatureVersion;
   uint16_t mNomenclature;
};

std::ostream& operator<<(std::ostream& aOut, const WsfExtRadioEntityType& aType);
// Accessors

inline uint8_t WsfExtRadioEntityType::GetEntityKind()
{
   return mEntityKind;
}
inline uint8_t WsfExtRadioEntityType::GetDomain()
{
   return mDomain;
}
inline uint16_t WsfExtRadioEntityType::GetCountry()
{
   return mCountry;
}
inline uint8_t WsfExtRadioEntityType::GetCategory()
{
   return mCategory;
}
inline uint8_t WsfExtRadioEntityType::GetNomenclatureVersion()
{
   return mNomenclatureVersion;
}
inline uint16_t WsfExtRadioEntityType::GetNomenclature()
{
   return mNomenclature;
}

#endif
