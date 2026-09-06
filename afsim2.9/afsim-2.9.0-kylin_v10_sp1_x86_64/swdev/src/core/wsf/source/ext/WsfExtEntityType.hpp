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

#ifndef WSFEXTENTITYTYPE_HPP
#define WSFEXTENTITYTYPE_HPP

#include "wsf_export.h"

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;

//#include "DisEntityType.hpp"

class WSF_EXPORT WsfExtEntityType
{
public:
   WsfExtEntityType();
   WsfExtEntityType(uint8_t  aEntityKind,
                    uint8_t  aDomain,
                    uint16_t aCountry,
                    uint8_t  aCategory,
                    uint8_t  aSubcategory,
                    uint8_t  aSpecific,
                    uint8_t  aExtra);

   bool operator==(const WsfExtEntityType& aRhs) const;
   bool operator<(const WsfExtEntityType& aRhs) const;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   uint8_t  mEntityKind;
   uint8_t  mDomain;
   uint16_t mCountry;
   uint8_t  mCategory;
   uint8_t  mSubcategory;
   uint8_t  mSpecific;
   uint8_t  mExtra;
};

WSF_EXPORT std::ostream& operator<<(std::ostream& aOut, const WsfExtEntityType& aType);

#endif
