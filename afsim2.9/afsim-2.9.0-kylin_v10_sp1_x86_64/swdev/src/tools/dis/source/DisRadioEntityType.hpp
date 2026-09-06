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

#ifndef DISRADIOENTITYTYPE_HPP
#define DISRADIOENTITYTYPE_HPP

#include "dis_export.h"

#include <iostream>

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisRadioEntityType
{
public:
   DisRadioEntityType();
   ~DisRadioEntityType();
   // Using default copy constructor

   // Accessors

   DisEnum8  GetEntityKind() const;
   DisEnum8  GetDomain() const;
   DisEnum16 GetCountry() const;
   DisEnum8  GetCategory() const;
   DisEnum8  GetNomenclatureVersion() const;
   DisEnum16 GetNomenclature() const;

   // Mutators

   void SetEntityKind(DisEnum8 aEntityKind);
   void SetDomain(DisEnum8 aDomain);
   void SetCountry(DisEnum16 aCountry);
   void SetCategory(DisEnum8 aCategory);
   void SetNomenclatureVersion(DisEnum8 aNomenclatureVersion);
   void SetNomenclature(DisEnum16 aNomenclature);

   // Input/output

   void        Get(GenI& aGenI);
   void        Put(GenO& aGenO) const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   bool IsValid() const;
   // Operators

   // Using default operator=
   bool operator==(const DisRadioEntityType& aRhs) const;
   bool operator!=(const DisRadioEntityType& aRhs) const;

private:
   DisEnum8  mEntityKind{0};
   DisEnum8  mDomain{0};
   DisEnum16 mCountry{0};
   DisEnum8  mCategory{0};
   DisEnum8  mNomenclatureVersion{0};
   DisEnum16 mNomenclature{0};
};

// Accessors

inline DisEnum8 DisRadioEntityType::GetEntityKind() const
{
   return mEntityKind;
}
inline DisEnum8 DisRadioEntityType::GetDomain() const
{
   return mDomain;
}
inline DisEnum16 DisRadioEntityType::GetCountry() const
{
   return mCountry;
}
inline DisEnum8 DisRadioEntityType::GetCategory() const
{
   return mCategory;
}
inline DisEnum8 DisRadioEntityType::GetNomenclatureVersion() const
{
   return mNomenclatureVersion;
}
inline DisEnum16 DisRadioEntityType::GetNomenclature() const
{
   return mNomenclature;
}

// Mutators

inline void DisRadioEntityType::SetEntityKind(DisEnum8 aEntityKind)
{
   mEntityKind = aEntityKind;
}
inline void DisRadioEntityType::SetDomain(DisEnum8 aDomain)
{
   mDomain = aDomain;
}
inline void DisRadioEntityType::SetCountry(DisEnum16 aCountry)
{
   mCountry = aCountry;
}
inline void DisRadioEntityType::SetCategory(DisEnum8 aCategory)
{
   mCategory = aCategory;
}
inline void DisRadioEntityType::SetNomenclatureVersion(DisEnum8 aNomenclatureVersion)
{
   mNomenclatureVersion = aNomenclatureVersion;
}
inline void DisRadioEntityType::SetNomenclature(DisEnum16 aNomenclature)
{
   mNomenclature = aNomenclature;
}

// Inline Operators

inline bool DisRadioEntityType::operator==(const DisRadioEntityType& aRhs) const
{
   // The goofy order is attempt to optimize the code.
   // There should be the most variation in "Category".
   return ((mCategory == aRhs.mCategory) && (mCountry == aRhs.mCountry) && (mNomenclature == aRhs.mNomenclature) &&
           (mNomenclatureVersion == aRhs.mNomenclatureVersion) && (mEntityKind == aRhs.mEntityKind) &&
           (mDomain == aRhs.mDomain));
}

inline bool DisRadioEntityType::operator!=(const DisRadioEntityType& aRhs) const
{
   // The goofy order is attempt to optimize the code.
   // There should be the most variation in "Category".
   return ((mCategory != aRhs.mCategory) || (mCountry != aRhs.mCountry) || (mNomenclature != aRhs.mNomenclature) ||
           (mNomenclatureVersion != aRhs.mNomenclatureVersion) || (mEntityKind != aRhs.mEntityKind) ||
           (mDomain != aRhs.mDomain));
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisRadioEntityType& aRadioEntityType)
{
   aRadioEntityType.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisRadioEntityType& aRadioEntityType)
{
   aRadioEntityType.Get(aGenI);
   return aGenI;
}

#endif
