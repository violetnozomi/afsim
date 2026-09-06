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

#include "ext/WsfExtEntityType.hpp"

#include <iostream>
#include <sstream>

#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"

WsfExtEntityType::WsfExtEntityType()
   : mEntityKind()
   , mDomain()
   , mCountry()
   , mCategory()
   , mSubcategory()
   , mSpecific()
   , mExtra()
{
}

WsfExtEntityType::WsfExtEntityType(uint8_t  aEntityKind,
                                   uint8_t  aDomain,
                                   uint16_t aCountry,
                                   uint8_t  aCategory,
                                   uint8_t  aSubcategory,
                                   uint8_t  aSpecific,
                                   uint8_t  aExtra)
   : mEntityKind(aEntityKind)
   , mDomain(aDomain)
   , mCountry(aCountry)
   , mCategory(aCategory)
   , mSubcategory(aSubcategory)
   , mSpecific(aSpecific)
   , mExtra(aExtra)
{
}

bool WsfExtEntityType::operator<(const WsfExtEntityType& aRhs) const
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

bool WsfExtEntityType::operator==(const WsfExtEntityType& aRhs) const
{
   return mEntityKind == aRhs.mEntityKind && mDomain == aRhs.mDomain && mCountry == aRhs.mCountry &&
          mCategory == aRhs.mCategory && mSubcategory == aRhs.mSubcategory && mSpecific == aRhs.mSpecific &&
          mExtra == aRhs.mExtra;
}

std::ostream& operator<<(std::ostream& aOut, const WsfExtEntityType& aType)
{
   // clang-format off
   aOut << static_cast<unsigned int>(aType.mEntityKind)  << ':'
        << static_cast<unsigned int>(aType.mDomain)      << ':'
        << static_cast<unsigned int>(aType.mCountry)     << ':'
        << static_cast<unsigned int>(aType.mCategory)    << ':'
        << static_cast<unsigned int>(aType.mSubcategory) << ':'
        << static_cast<unsigned int>(aType.mSpecific)    << ':'
        << static_cast<unsigned int>(aType.mExtra);
   // clang-format on
   return aOut;
}


// =================================================================================================
// Embedded EntityType scripting class
// =================================================================================================

// =================================================================================================
//! Define script methods for WsfExtEntityType.,
class WSF_EXPORT WsfScriptExtEntityTypeClass : public UtScriptClass
{
public:
   WsfScriptExtEntityTypeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptExtEntityTypeClass() override = default;

   void Destroy(void* aObjectPtr) override;

   // Entity Type methods
   UT_DECLARE_SCRIPT_METHOD(String);
   UT_DECLARE_SCRIPT_METHOD(EntityKind);
   UT_DECLARE_SCRIPT_METHOD(Domain);
   UT_DECLARE_SCRIPT_METHOD(Country);
   UT_DECLARE_SCRIPT_METHOD(Category);
   UT_DECLARE_SCRIPT_METHOD(SubCategory);
   UT_DECLARE_SCRIPT_METHOD(Specific);
   UT_DECLARE_SCRIPT_METHOD(Extra);
};

// =================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfExtEntityType::CreateScriptClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptExtEntityTypeClass>(aClassName, aScriptTypesPtr);
}

// =================================================================================================
WsfScriptExtEntityTypeClass::WsfScriptExtEntityTypeClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("EntityType");

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<String>());
   AddMethod(ut::make_unique<EntityKind>());
   AddMethod(ut::make_unique<Domain>());
   AddMethod(ut::make_unique<Country>());
   AddMethod(ut::make_unique<Category>());
   AddMethod(ut::make_unique<SubCategory>());
   AddMethod(ut::make_unique<Specific>());
   AddMethod(ut::make_unique<Extra>());
}

void WsfScriptExtEntityTypeClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfExtEntityType*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, String, 0, "string", "")
{
   std::stringstream sstr;
   sstr << *static_cast<WsfExtEntityType*>(aObjectPtr);
   aReturnVal.SetString(sstr.str());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, EntityKind, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mEntityKind);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, Domain, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mDomain);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, Country, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mCountry);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, Category, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mCategory);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, SubCategory, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mSubcategory);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, Specific, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mSpecific);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExtEntityTypeClass, WsfExtEntityType, Extra, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mExtra);
}
