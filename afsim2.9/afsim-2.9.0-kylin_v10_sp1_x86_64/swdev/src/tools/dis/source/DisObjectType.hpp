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

#ifndef DISOBJECTTYPE_HPP
#define DISOBJECTTYPE_HPP

#include "dis_export.h"

#include <cstddef>
#include <iostream>

#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"

class GenI;
class GenO;

/*!
 * The type of synthetic environment point, linear object, and areal object in a DIS exercise shall be specified
 * by an Object Type record. This record shall specify the domain of the object, the kind of object, and the
 * specific identification of the entity. Fields not used shall contain the value zero.
 */
class DIS_EXPORT DisObjectType
{
public:
   DisObjectType() = default;
   DisObjectType(DisEnum8 aDomain, DisEnum8 aObjectKind, DisEnum8 aCategory, DisEnum8 aSubcategory);
   DisObjectType(const DisObjectType& aSrc);
   ~DisObjectType() = default;

   // Accessors

   DisEnum8 GetDomain() const;
   DisEnum8 GetObjectKind() const;
   DisEnum8 GetCategory() const;
   DisEnum8 GetSubcategory() const;

   // Mutators

   void SetDomain(DisEnum8 aDomain);
   void SetObjectKind(DisEnum8 aObjectKind);
   void SetCategory(DisEnum8 aCategory);
   void SetSubcategory(DisEnum8 aSubcategory);

   // Input/output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Data Validation
   bool IsValid() const;

   // Operators
   DisObjectType& operator=(const DisObjectType& aRhs);
   bool           operator==(const DisObjectType& aRhs) const;

private:
   /*!
    * Specifies the operation domain for the entity (subsurface, surface, and land) excluding munition entities.
    * For munition entities: This field specifies the domain of the target (surface to air --> anti air etc).
    */
   DisEnum8 mDomain = 0;

   /*!
    * Specifies the kind of entity described by this record.
    */
   DisEnum8 mObjectKind = 0;

   /*!
    * Specifies the main category of the entity
    */
   DisEnum8 mCategory = 0;

   /*!
    * Specifies the sub category of the entity
    */
   DisEnum8 mSubcategory = 0;
};

// Accessors

inline DisEnum8 DisObjectType::GetObjectKind() const
{
   return mObjectKind;
}

inline DisEnum8 DisObjectType::GetDomain() const
{
   return mDomain;
}

inline DisEnum8 DisObjectType::GetCategory() const
{
   return mCategory;
}

inline DisEnum8 DisObjectType::GetSubcategory() const
{
   return mSubcategory;
}

inline void DisObjectType::SetDomain(DisEnum8 aDomain)
{
   mDomain = aDomain;
}

// Mutators
inline void DisObjectType::SetObjectKind(DisEnum8 aObjectKind)
{
   mObjectKind = aObjectKind;
}

inline void DisObjectType::SetCategory(DisEnum8 aCategory)
{
   mCategory = aCategory;
}

inline void DisObjectType::SetSubcategory(DisEnum8 aSubcategory)
{
   mSubcategory = aSubcategory;
}

// Inline Operators
inline bool DisObjectType::operator==(const DisObjectType& aRhs) const
{
   // The goofy order is attempt to optimize the code.
   // There should be the most variation in "Category".
   return (mCategory == aRhs.mCategory) && (mSubcategory == aRhs.mSubcategory) && (mObjectKind == aRhs.mObjectKind) &&
          (mDomain == aRhs.mDomain);
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisObjectType& aEntityType)
{
   aEntityType.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisObjectType& aEntityType)
{
   aEntityType.Get(aGenI);
   return aGenI;
}

#endif
