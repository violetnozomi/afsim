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

#include "UtAttributeContainer.hpp"

#include <cassert>
#include <map>

#include "UtException.hpp"
#include "UtMatrix.hpp"

// =================================================================================================
// static
//! Register atomic (simple) types.
void UtAttributeContainer::RegisterAtomicTypes()
{
   // Register standard types here.
   UtAttribute<int>::RegisterPrototype("int");
   UtAttribute<bool>::RegisterPrototype("bool");
   UtAttribute<double>::RegisterPrototype("double");
   UtAttribute<std::string>::RegisterPrototype("string");
}

// =================================================================================================
// static
void UtAttributeContainer::RegisterStandardTypes()
{
   RegisterAtomicTypes();
   UtAttribute<UtMatrixd>::RegisterPrototype("Matrix");
}

// =================================================================================================
void UtAttributeContainer::AddAttribute(std::unique_ptr<UtAttributeBase> aAttributePtr)
{
   auto result = mAttributeMap.emplace(aAttributePtr->GetName(), std::move(aAttributePtr));
   if (!result.second)
   {
      throw(UtException("Attribute: " + result.first->first + " already exists!"));
   }
}

// =================================================================================================
void UtAttributeContainer::SetAttributes(const AttributeMap& aSrc)
{
   for (const auto& srcAttrib : aSrc)
   {
      auto it = mAttributeMap.find(srcAttrib.first);
      if (it != mAttributeMap.end())
      {
         it->second->SetAttribute(*srcAttrib.second);
      }
      else
      {
         throw UtException("Attribute: " + srcAttrib.first + " does not exist in container");
      }
   }
}

// =================================================================================================
//! Return whether the container has any attributes.
bool UtAttributeContainer::HasAttributes() const
{
   return (!mAttributeMap.empty());
}

// =================================================================================================
void UtAttributeContainer::Merge(const UtAttributeContainer& aSrc)
{
   if (aSrc.HasAttributes())
   {
      for (const auto& srcAttribute : aSrc.mAttributeMap)
      {
         UtAttributeBase& attribute = *srcAttribute.second;
         assert(attribute.GetName() == srcAttribute.first);
         auto itersuccess = mAttributeMap.emplace(srcAttribute.first, UtCloneablePtr<UtAttributeBase>{});
         if (!itersuccess.second)
         {
            // attribute already existed in mAttributeMap with same name.
            itersuccess.first->second->SetAttribute(attribute);
         }
         else
         {
            // attribute with same name did not previously exist.
            itersuccess.first->second = ut::clone(attribute);
         }
      }
   }
}

// =================================================================================================
void UtAttributeContainer::Update(const UtAttributeContainer& aSrc)
{
   if (aSrc.HasAttributes())
   {
      for (const auto& srcAttribute : aSrc.mAttributeMap)
      {
         assert(srcAttribute.second->GetName() == srcAttribute.first);
         auto iter = mAttributeMap.find(srcAttribute.first);

         if (iter != mAttributeMap.end())
         {
            UtAttributeBase& attribute = *srcAttribute.second;
            iter->second->SetAttribute(attribute);
         }
      }
   }
}

// =================================================================================================
// virtual
void UtAttributeContainer::SetAttribute(const UtAttributeBase& aSrc)
{
   const UtAttributeContainer* derivedPtr = dynamic_cast<const UtAttributeContainer*>(&aSrc);
   if (derivedPtr != nullptr)
   {
      if (GetTypeId() == derivedPtr->GetTypeId())
      {
         SetAttributes(derivedPtr->mAttributeMap);
      }
      else
      {
         throw UtAttributeBase::NotConvertibleException(aSrc);
      }
   }
   else
   {
      throw UtAttributeBase::NotConvertibleException(aSrc);
   }
}

// =================================================================================================
//! Return the number of attributes in the container.
//! Each contained attribute container counts as one attribute.
unsigned UtAttributeContainer::GetNumberOfAttributes() const
{
   return static_cast<unsigned int>(mAttributeMap.size());
}

// =================================================================================================
void UtAttributeContainer::GetAttributeNames(std::vector<std::string>& aNames) const
{
   aNames.clear();
   for (const auto& iter : mAttributeMap)
   {
      aNames.push_back(iter.first);
   }
}

// =================================================================================================
//! Return the attribute object corresponding with with given name.
UtAttributeBase& UtAttributeContainer::GetAttribute(const std::string& aName) const
{
   auto iter = mAttributeMap.find(aName);

   if (iter != mAttributeMap.end())
   {
      return *(iter->second);
   }
   else
   {
      throw UtException("Attribute: " + aName + " does not exist!");
   }
}

// =================================================================================================
//! Return whether the given attribute exists within the container.
bool UtAttributeContainer::AttributeExists(const std::string& aName) const
{
   return (mAttributeMap.find(aName) != mAttributeMap.end());
}

// =================================================================================================
//! Get the boolean value corresponding with the attribute of the given name.
bool UtAttributeContainer::GetBool(const std::string& aName) const
{
   bool value;
   Get(aName, value);
   return value;
}

// =================================================================================================
//! Get the int value corresponding with the attribute of the given name.
int UtAttributeContainer::GetInt(const std::string& aName) const
{
   int value;
   Get(aName, value);
   return value;
}

// =================================================================================================
//! Get the double value corresponding with the attribute of the given name.
double UtAttributeContainer::GetDouble(const std::string& aName) const
{
   double value;
   Get(aName, value);
   return value;
}

// =================================================================================================
//! Get the string value corresponding with the attribute of the given name.
std::string& UtAttributeContainer::GetString(const std::string& aName) const
{
   std::string* stringPtr;
   Get(aName, stringPtr);
   return *stringPtr;
}

// =================================================================================================
//! Get the basic-type value corresponding with the attribute of the given name.
UtVariant UtAttributeContainer::GetVar(const std::string& aName) const
{
   UtVariant        var;
   UtAttributeBase* baseAttributePtr = FindAttribute(aName);
   if (baseAttributePtr != nullptr)
   {
      baseAttributePtr->GetVariant(var);
   }
   return var;
}

// =================================================================================================
//! Add a boolean attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aInitialValue The initial value of the attribute.
void UtAttributeContainer::AddBool(UtStringId aName,
                                   bool       aInitialValue) // = false
{
   auto boolPtr = ut::make_unique<UtAttribute<bool>>();
   boolPtr->SetName(aName);
   boolPtr->Set(aInitialValue);
   AddAttribute(std::move(boolPtr));
}

// =================================================================================================
//! Add an int attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aInitialValue The initial value of the attribute.
void UtAttributeContainer::AddInt(UtStringId aName,
                                  int        aInitialValue) // = 0
{
   auto intPtr = ut::make_unique<UtAttribute<int>>();
   intPtr->SetName(aName);
   intPtr->Set(aInitialValue);
   AddAttribute(std::move(intPtr));
}

// =================================================================================================
//! Add a double attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aInitialValue The initial value of the attribute.
void UtAttributeContainer::AddDouble(UtStringId aName,
                                     double     aInitialValue) // = 0.0
{
   auto doublePtr = ut::make_unique<UtAttribute<double>>();
   doublePtr->SetName(aName);
   doublePtr->Set(aInitialValue);
   AddAttribute(std::move(doublePtr));
}

// =================================================================================================
//! Add a string attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aInitialValue The initial value of the attribute.
void UtAttributeContainer::AddString(UtStringId         aName,
                                     const std::string& aInitialValue) // = ""
{
   auto stringPtr = ut::make_unique<UtAttribute<std::string>>();
   stringPtr->SetName(aName);
   stringPtr->Set(aInitialValue);
   AddAttribute(std::move(stringPtr));
}

// =================================================================================================
//! Assign a boolean attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aValue The value of the attribute.
void UtAttributeContainer::AssignBool(const std::string& aName,
                                      bool               aValue) // = false
{
   Assign(aName, aValue);
}

// =================================================================================================
//! Assign an int attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aValue The value of the attribute.
void UtAttributeContainer::AssignInt(const std::string& aName,
                                     int                aValue) // = 0
{
   Assign(aName, aValue);
}

// =================================================================================================
//! Assign a double attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aValue The value of the attribute.
void UtAttributeContainer::AssignDouble(const std::string& aName,
                                        double             aValue) // = 0.0
{
   Assign(aName, aValue);
}

// =================================================================================================
//! Assign a string attribute of the given name.
//! @param aName The name corresponding with the new attribute.
//! @param aValue The value of the attribute.
void UtAttributeContainer::AssignString(const std::string& aName,
                                        const std::string& aValue) // = ""
{
   Assign(aName, aValue);
}

// =================================================================================================
//! Assign a basic-type attribute of the given name
//! @param aName The name corresponding with the new attribute.
//! @param aVariant The value of the attribute.
void UtAttributeContainer::AssignVar(const std::string& aName, const UtVariant& aVariant)
{
   switch (aVariant.GetType())
   {
   case UtVariant::Type::cBOOL:
      AssignBool(aName, aVariant.GetBool());
      break;
   case UtVariant::Type::cINT:
      AssignInt(aName, aVariant.GetInt());
      break;
   case UtVariant::Type::cDOUBLE:
      AssignDouble(aName, aVariant.GetDouble());
      break;
   case UtVariant::Type::cSTRING:
      AssignString(aName, aVariant.GetString());
      break;
   default:
      break;
   }
}

// =================================================================================================
//! Delete an entry from the container.
//! @param aName The name of the attribute to be deleted.
//! @returns true if the attribute exists and was deleted, or false if the attribute did not exist.
bool UtAttributeContainer::Delete(const std::string& aName)
{
   bool deleted = false;
   auto iter    = mAttributeMap.find(aName);
   if (iter != mAttributeMap.end())
   {
      mAttributeMap.erase(iter);
      deleted = true;
   }
   return deleted;
}

// =================================================================================================
// static
//! Register an attribute "prototype."
//! See UtAttributeBase::RegisterPrototype.
void UtAttributeContainer::RegisterPrototype(const std::string& aTypeName, std::unique_ptr<UtAttributeBase> aPrototypePtr)
{
   UtAttributeBase::RegisterPrototype(aTypeName, std::move(aPrototypePtr));
}

// =================================================================================================
UtAttributeBase* UtAttributeContainer::FindAttribute(const std::string& aName) const
{
   UtAttributeBase* attributePtr = nullptr;
   auto             iter         = mAttributeMap.find(aName);
   if (iter != mAttributeMap.end())
   {
      attributePtr = iter->second.get();
   }
   return attributePtr;
}

// =================================================================================================
void UtAttributeContainer::Clear()
{
   mAttributeMap.clear();
}
