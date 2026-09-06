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

#include "UtAttributeBase.hpp"

#include "UtAttribute.hpp"
#include "UtDictionary.hpp"
#include "UtLog.hpp"


UtAttributeBase::Prototypes* UtAttributeBase::mPrototypesInstancePtr = nullptr;
UtDictionary*                UtAttributeBase::mDictionaryInstancePtr = nullptr;

UtAttributeBase::UtAttributeBase(UtStringId aName)
   : mNameId(aName)
{
}

// static
//! Return the name dictionary (see UtDictionary) associated with UtAttribute names.
//! @return A reference to the UtAttribute name dictionary.
UtDictionary& UtAttributeBase::GetDictionary()
{
   if (mDictionaryInstancePtr == nullptr)
   {
      mDictionaryInstancePtr = new UtDictionary();
   }
   return *mDictionaryInstancePtr;
}

// static
const std::string& UtAttributeBase::GetDictionaryString(int aId)
{
   return GetDictionary().GetString(aId);
}

// static
int UtAttributeBase::GetDictionaryNumber(const std::string& aString)
{
   return GetDictionary().GetNumber(aString);
}
//! Get the prototype with the given name.
//!   @param aName The integer corresponding with the name of the prototype.
UtAttributeBase* UtAttributeBase::Prototypes::Get(int aType)
{
   auto             iter         = mPrototypeMap.find(aType);
   UtAttributeBase* attributePtr = nullptr;
   if (iter != mPrototypeMap.end())
   {
      attributePtr = iter->second.get();
   }
   return attributePtr;
}

//! Add the prototype with the given name.
//!   @param aTypeName The integer corresponding with the name of the prototype.
//!   @param aPrototypePtr The prototype object to add to the prototypes list.
//!      Ownership of the object is transferred in this call.
void UtAttributeBase::Prototypes::Add(const std::string& aTypeName, std::unique_ptr<UtAttributeBase> aPrototypePtr)
{
   aPrototypePtr->SetType(aTypeName);
   // Get typeId first to avoid UB when moving from aPrototypePtr
   int typeId = aPrototypePtr->GetTypeId();
   mPrototypeMap.emplace(typeId, std::move(aPrototypePtr));
}

// static
//! Register an attribute "prototype."
//! Ownership of the object is transferred in this call.
//! Once registered an attribute can be referenced by name in the input and
//! placed in the container (see the User Input Guide for more details).
//! @param aTypeName The type name of the prototype object.
//! @param aPrototypePtr The object to be used as the prototype in the prototypes "list."
void UtAttributeBase::RegisterPrototype(const std::string& aTypeName, std::unique_ptr<UtAttributeBase> aPrototypePtr)
{
   aPrototypePtr->SetType(aTypeName); // Used for derived types.
   GetPrototypes().Add(aTypeName, std::move(aPrototypePtr));
}

//! Get the pointer to the prototype attribute object with the given type name.
UtAttributeBase* UtAttributeBase::GetPrototype(const std::string& aTypeName)
{
   return GetPrototypes().Get(GetDictionary().GetNumber(aTypeName));
}

// virtual
UtAttributeBase* UtAttributeBase::Clone() const
{
   throw(UtException("UtAttributeBase: Unexpected call of Clone()"));
}

// virtual
const std::string& UtAttributeBase::GetType() const
{
   static const std::string undefined("Undefined");
   return undefined;
}

UtAttributeBase::NotConvertibleException::NotConvertibleException(const UtAttributeBase& aAttribute)
   : UtException("UtAttributeBase: Attribute not convertible between types.")
{
   auto out = ut::log::error() << "UtAttributeBase: Attribute not convertible between types.";
   out.AddNote() << "Attribute: " << aAttribute.GetName();
}


void UtAttributeBase::ClearStatic()
{
   ClearPrototypes();
   delete mDictionaryInstancePtr;
   mDictionaryInstancePtr = nullptr;
}