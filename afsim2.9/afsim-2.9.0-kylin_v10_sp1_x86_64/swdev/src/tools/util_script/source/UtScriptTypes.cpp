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

#include "UtScriptTypes.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

#include "UtAtmosphere.hpp"
#include "UtDictionary.hpp"
#include "UtMemory.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptAtmosphere.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptCalendar.hpp"
#include "UtScriptCentralBody.hpp"
#include "UtScriptColor.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptCoordinateSystem.hpp"
#include "UtScriptCore.hpp"
#include "UtScriptCovarianceEllipsoid.hpp"
#include "UtScriptDCM.hpp"
#include "UtScriptEarth.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptFileIO.hpp"
#include "UtScriptFormat.hpp"
#include "UtScriptMath.hpp"
#include "UtScriptMatrix.hpp"
#include "UtScriptMethod.hpp"
#include "UtScriptMoon.hpp"
#include "UtScriptOrbitalElements.hpp"
#include "UtScriptOrbitalState.hpp"
#include "UtScriptParser.hpp"
#include "UtScriptPath.hpp"
#include "UtScriptQuadTree.hpp"
#include "UtScriptQuaternion.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptReferenceFrame.hpp"
#include "UtScriptScope.hpp"
#include "UtScriptSignal.hpp"
#include "UtScriptSolarSystem.hpp"
#include "UtScriptStruct.hpp"
#include "UtScriptSun.hpp"
#include "UtScriptSystem.hpp"
#include "UtScriptVec3.hpp"
#include "UtScriptVec3dX.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"

UtScriptTypes::UtScriptTypes(UtDictionary& aDictionary)
   : mMutex()
   , mIsInitialized(false)
   , mClassMap()
   , mDictionary(aDictionary)
{
   // This is the base class, which is referred to as Object.
   // All script objects can be stored as an Object.
   Register(ut::make_unique<UtScriptClass>("Object", this));

   Register(ut::make_unique<UtScriptAppPointerClass>(this));
   Register(ut::make_unique<UtScriptVoid>(this));
   Register(ut::make_unique<UtScriptNull>(this));
   Register(ut::make_unique<UtScriptVariant>(this));
   Register(ut::make_unique<UtScriptString>(this));
   Register(ut::make_unique<UtScriptInt>(this));
   Register(ut::make_unique<UtScriptDouble>(this));
   Register(ut::make_unique<UtScriptBool>(this));
   Register(ut::make_unique<UtScriptCore>(this));
   Register(ut::make_unique<UtScriptStruct>(this));

   // Containers.
   Register(ut::make_unique<UtScriptArray>("Array", this, UtScriptClass::TemplateArgTypes()));
   Register(ut::make_unique<UtScriptMap>("Map", this, UtScriptClass::TemplateArgTypes()));
   Register(ut::make_unique<UtScriptSet>("Set", this, UtScriptClass::TemplateArgTypes()));
   Register(ut::make_unique<UtScriptIterator>("Iterator", this));
   Register(ut::make_unique<UtScriptArrayIterator>("ArrayIterator", this));
   Register(ut::make_unique<UtScriptMapIterator>("MapIterator", this));
   Register(ut::make_unique<UtScriptSetIterator>("SetIterator", this));

   GetOrCreateContainer("Array<Object>");
   GetOrCreateContainer("Set<Object>");
   GetOrCreateContainer("Map<Object,Object>");

   Register(ut::make_unique<UtScriptAtmosphere>(this, UtAtmosphere::CreateNewAtmosphereTable()));
   Register(ut::make_unique<UtScriptCalendar>("Calendar", this));
   Register(ut::make_unique<UtScriptColor>("Color", this));
   Register(ut::make_unique<UtScriptEarth>(this));
   Register(ut::make_unique<UtScriptFileIO>(this));
   Register(ut::make_unique<UtScriptFormat>(this));
   Register(ut::make_unique<UtScriptMath>(this));
   Register(ut::make_unique<ut::script::Matrix>(this));
   Register(ut::make_unique<UtScriptPath>("Path", this));
   Register(ut::make_unique<UtScriptQuadTree>("QuadTree", this));
   Register(ut::make_unique<UtScriptMoon>(this));
   Register(ut::make_unique<UtScriptSolarSystem>(this));
   Register(ut::make_unique<UtScriptSun>(this));
   Register(ut::make_unique<UtScriptVec3>(this));
   Register(ut::make_unique<UtScriptVec3dX>(this));
   Register(ut::make_unique<UtScriptSystem>(this));
   Register(ut::make_unique<UtScriptQuaternion>(this));
   Register(ut::make_unique<UtScriptDCM>(this));
   Register(ut::make_unique<UtScriptEllipsoid>(this));

   Register(ut::make_unique<ut::script::CentralBody>("CentralBody", this));
   Register(ut::make_unique<ut::script::CoordinateSystem>("CoordinateSystem", this));
   Register(ut::make_unique<ut::script::ReferenceFrame>("ReferenceFrame", this));
   Register(ut::make_unique<ut::script::OrbitalElements>("OrbitalElements", this));
   Register(ut::make_unique<ut::script::OrbitalState>("OrbitalState", this));

   Register(UtScriptMethod::CreateScriptClass("Method", this));
   Register(UtScriptSignal::CreateScriptClass("Signal", this));
   // mGlobalInstancePrototypePtr->RegisterVariable("__builtin__", "__BUILTIN__");
}

bool UtScriptTypes::Register(std::unique_ptr<UtScriptClass> aScriptClassPtr)
{
   auto valid = mClassMap.emplace(aScriptClassPtr->GetClassName(), std::move(aScriptClassPtr));
   if (valid.second)
   {
      if (mIsInitialized)
      {
         return valid.first->second->Initialize();
      }
      return true;
   }

   return false;
}

//! Add a non-static class method to a class.
bool UtScriptTypes::AddClassMethod(UtStringId aClassName, std::unique_ptr<UtScriptClass::InterfaceMethod> aMethodPtr)
{
   UtScriptClass* classPtr = GetClass(aClassName);
   if (classPtr != nullptr)
   {
      return classPtr->AddMethod(std::move(aMethodPtr));
   }
   return false;
}

//! Add a static class method to a class.
bool UtScriptTypes::AddStaticClassMethod(UtStringId aClassName, std::unique_ptr<UtScriptClass::InterfaceMethod> aMethodPtr)
{
   UtScriptClass* classPtr = GetClass(aClassName);
   if (classPtr != nullptr)
   {
      return classPtr->AddStaticMethod(std::move(aMethodPtr));
   }
   return false;
}

//--------------------------------------------------------------
//! Returns a pointer to the UtScriptClass that corresponds
//! to the provided class name id.
//! Returns null if unsuccessful.
//--------------------------------------------------------------

UtScriptClass* UtScriptTypes::GetClass(UtStringId aClassNameId) const
{
   UtScriptClass* ptr = nullptr;
   auto           itr = mClassMap.find(aClassNameId);
   if (itr != mClassMap.end())
   {
      ptr = itr->second.get();
   }
   return ptr;
}

//--------------------------------------------------------------
//! Returns a pointer to the UtScriptClass that corresponds to the provided template class data
//! Returns null if unsuccessful.
//--------------------------------------------------------------

UtScriptClass* UtScriptTypes::GetClass(const TemplateType& aTemplateType)
{
   auto i = mTemplateTypes.find(aTemplateType);
   if (i != mTemplateTypes.end())
   {
      return i->second;
   }
   UtScriptClass* basePtr = GetClass(aTemplateType.first);
   if (basePtr != nullptr)
   {
      UtScriptClass* firstArgPtr = GetClass(aTemplateType.second.first);
      if (firstArgPtr != nullptr)
      {
         std::string containerName = basePtr->GetClassName().GetString() + '<' + firstArgPtr->GetClassName();
         std::vector<UtStringId> tempArgs;
         tempArgs.emplace_back(firstArgPtr->GetClassName());
         UtScriptClass* secondArgPtr = GetClass(aTemplateType.second.second);
         if (secondArgPtr != nullptr)
         {
            tempArgs.emplace_back(secondArgPtr->GetClassName());
            containerName += ',' + secondArgPtr->GetClassName().GetString() + '>';
            UtScriptClass* containerPtr = GetOrCreateContainer(containerName);
            // UtScriptClass* containerPtr = CreateContainer(basePtr->GetName(), containerName, tempArgs);
            // if (containerPtr != 0)
            //{
            //    Register(containerPtr);
            // }
            return containerPtr;
         }
         else
         {
            containerName += '>';
            UtScriptClass* containerPtr = GetOrCreateContainer(containerName);
            // UtScriptClass* containerPtr = CreateContainer(basePtr->GetName(), containerName, tempArgs);
            // if (containerPtr != 0)
            //{
            //    Register(containerPtr);
            // }
            return containerPtr;
         }
      }
   }
   return nullptr;
}

//--------------------------------------------------------------
//! Returns a pointer to the UtScriptClass that corresponds to the provided class name.
//! Returns null if unsuccessful.
//--------------------------------------------------------------

UtScriptClass* UtScriptTypes::GetClass(const std::string& aClassName) const
{
   if (UtStringId::Exists(aClassName))
   {
      return GetClass(UtStringId(aClassName));
   }
   return nullptr;
}

UtScriptClass* UtScriptTypes::GetClass(UtScriptAccessible* aAccessiblePtr) const
{
   const char*                           classNamePtr = aAccessiblePtr->GetScriptClassName();
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   UtScriptClass*&                       classPtr = mClassByCompiledNameMap[classNamePtr];
   if (classPtr == nullptr)
   {
      classPtr = GetClass(UtStringId(classNamePtr));
   }
   return classPtr;
}

//! Returns a pointer to the script class with the given name.
//! Because the result is cached, this should only be called with a C-string compiled into the code.
//! Use GetClass() otherwise.
UtScriptClass* UtScriptTypes::GetClassFromCharArray(const char* aCharArray) const
{
   if (aCharArray != nullptr)
   {
      std::lock_guard<std::recursive_mutex> lock(mMutex);
      UtScriptClass*&                       classPtr = mClassByCompiledNameMap[aCharArray];
      if (classPtr == nullptr)
      {
         classPtr = GetClass(UtStringId(aCharArray));
      }
      return classPtr;
   }
   return nullptr;
}

//--------------------------------------------------------------
//! Returns the type id given a type name.
//! If the type doesn't exist a empty StringId is returned.
//--------------------------------------------------------------

UtStringId UtScriptTypes::GetTypeId(const std::string& aTypeName) const
{
   if (UtStringId::Exists(aTypeName))
   {
      UtStringId typeId(aTypeName);
      if (mClassMap.find(typeId) != mClassMap.end())
      {
         return typeId;
      }
   }
   return UtStringId();
}

//--------------------------------------------------------------
//! Returns the type name, given the type id.
//! If the type id is invalid, an empty string is returned.
//--------------------------------------------------------------

std::string UtScriptTypes::GetTypeName(UtStringId aTypeId) const
{
   std::string str = aTypeId;
   if (str == "<unknown>")
   {
      return "";
   }
   return str;
}

//--------------------------------------------------------------
//! Creates the specified container type. Returns a pointer to the
//! UtScriptClass object.
//! Note: The type in not registered with the types manager, (UtScriptTypes).
//!
//! aContainerType: The container type with template arguments in
//!                 angle brackets. For example, Map<int, string>
//--------------------------------------------------------------
std::unique_ptr<UtScriptClass> UtScriptTypes::CreateContainer(const std::string& aContainerType)
{
   // Parse the container spec into its base type and arguments.
   std::string                     containerType(aContainerType);
   std::string                     containerBaseType;
   UtScriptClass::TemplateArgTypes argTypes;
   if (ParseContainerType(containerType, containerBaseType, argTypes))
   {
      // Try to create the container.
      std::lock_guard<std::recursive_mutex> lock(mMutex);
      return CreateContainer(containerBaseType, containerType, argTypes);
   }
   return nullptr;
}

//--------------------------------------------------------------
//! Returns pointer to the specified container class.
//! If necessary, the container class will be created.
//! Returns null if the container class could not be created.
//! aContainerType: The container type with template arguments in
//!                 angle brackets. For example, Map<int,string>
//--------------------------------------------------------------
UtScriptClass* UtScriptTypes::GetOrCreateContainer(const std::string& aContainerType)
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   // remove any extra spaces
   std::string typeName(aContainerType);
   typeName.resize(std::remove(typeName.begin(), typeName.end(), ' ') - typeName.begin());
   auto containerPtr = GetClass(typeName);
   if (containerPtr == nullptr)
   {
      auto newContainerPtr = CreateContainer(typeName);
      if (newContainerPtr != nullptr)
      {
         auto* tmpPtr = newContainerPtr.get();
         if (!Register(std::move(newContainerPtr)))
         {
            return nullptr;
         }
         return tmpPtr;
      }
   }
   return containerPtr;
}

//--------------------------------------------------------------
//! Creates the specified container type with the specified
//! template type arguments. Returns a pointer to the
//! UtScriptClass object. Note: The type in not registered with
//! the types manager (UtScriptTypes).
//! aContainerBaseTypeName: The containers non-templated name
//!                         without template arguments (e.g. Array)
//! aContainerTypeName:     The containers name with template
//!                         arguments (e.g. Array<int>)
//! aTemplateArgTypes:      The dictionary type ids of the template
//!                         arguments.
//--------------------------------------------------------------
std::unique_ptr<UtScriptClass> UtScriptTypes::CreateContainer(const std::string& aContainerBaseTypeName,
                                                              const std::string& aContainerTypeName,
                                                              const UtScriptClass::TemplateArgTypes& aTemplateArgTypes)
{
   if (aContainerBaseTypeName == "Array")
   {
      if (aTemplateArgTypes.size() == 1)
      {
         UtScriptClass::TemplateArgTypes argTypes;
         argTypes.emplace_back(UtStringIdLiteral("int")); // The array uses an int as the key value.
         argTypes.emplace_back(aTemplateArgTypes[0].mTypeId);

         std::unique_ptr<UtScriptClass> rType  = ut::make_unique<UtScriptArray>(aContainerTypeName, this, argTypes);
         UtStringId                     first  = GetClass(UtStringIdLiteral("Array"))->GetClassName();
         TemplateArgs                   second = TemplateArgs(aTemplateArgTypes[0].mTypeId, UtStringId());
         TemplateType                   tt     = TemplateType(first, second);
         mTemplateTypes[tt]                    = rType.get();
         return rType;
      }
   }
   else if (aContainerBaseTypeName == "Map")
   {
      if (aTemplateArgTypes.size() == 2)
      {
         UtScriptClass::TemplateArgTypes argTypes;
         argTypes.insert(argTypes.end(), aTemplateArgTypes.begin(), aTemplateArgTypes.end());
         std::unique_ptr<UtScriptClass> rtnTypePtr = ut::make_unique<UtScriptMap>(aContainerTypeName, this, argTypes);
         UtStringId                     first      = GetClass(UtStringIdLiteral("Map"))->GetClassName();
         TemplateArgs                   second     = TemplateArgs(aTemplateArgTypes[0].mTypeId, UtStringId());
         TemplateType                   tt         = TemplateType(first, second);
         mTemplateTypes[tt]                        = rtnTypePtr.get();
         return rtnTypePtr;
      }
   }
   else if (aContainerBaseTypeName == "Set")
   {
      if (aTemplateArgTypes.size() == 1)
      {
         UtScriptClass::TemplateArgTypes argTypes;
         argTypes.insert(argTypes.end(), aTemplateArgTypes.begin(), aTemplateArgTypes.end());
         if (!argTypes.empty())
         {
            argTypes.push_back(argTypes[0]);
         }

         std::unique_ptr<UtScriptClass> rtnTypePtr = ut::make_unique<UtScriptSet>(aContainerTypeName, this, argTypes);
         mTemplateTypes[TemplateType(GetClass(UtStringIdLiteral("Set"))->GetClassName(),
                                     TemplateArgs(aTemplateArgTypes[0].mTypeId, UtStringId()))] = rtnTypePtr.get();
         return rtnTypePtr;
      }
   }
   return nullptr;
}

//! A helper method to parse a container specification and create a more useful internal representation.
//! A specification is of the form like Array<T>, Map<T1,T2>, Set<T>.
//! @param aContainerType     [input]  The container type name with template arguments (e.g.: "Array<int>").
//!                           [output] Updated with any spaces removed.
//! @param aContainerBaseType [output] The container base type name (e.g.: "Array").
//! @param aTemplateArgTypes  [output] The string IDs of the template arguments.
// static
bool UtScriptTypes::ParseContainerType(std::string&                     aContainerType,
                                       std::string&                     aContainerBaseType,
                                       UtScriptClass::TemplateArgTypes& aTemplateArgTypes)
{
   bool ok(false);

   // Remove all spaces
   std::string containerType;
   for (std::string::const_iterator iter = aContainerType.begin(); iter != aContainerType.end(); ++iter)
   {
      if (*iter != ' ')
      {
         containerType += *iter;
      }
   }
   aContainerType = containerType;

   // Find the opening and closing angle bracket positions.
   size_t openAnglePos  = containerType.find_first_of('<');
   size_t closeAnglePos = containerType.find_last_of('>');

   if ((openAnglePos != std::string::npos) && (closeAnglePos != std::string::npos))
   {
      aContainerBaseType = containerType.substr(0, openAnglePos);
      aTemplateArgTypes.clear();

      std::string argTypesString = containerType.substr(openAnglePos + 1, closeAnglePos - (openAnglePos + 1));

      int         skipCommasCount = 0;
      std::string type;
      for (char ch : argTypesString)
      {
         if (ch == '<')
         {
            ++skipCommasCount;
         }

         if (ch == '>')
         {
            --skipCommasCount;
         }

         if ((skipCommasCount > 0) || (!(skipCommasCount > 0) && (ch != ',')))
         {
            type += ch;
         }
         else
         {
            aTemplateArgTypes.emplace_back(type);
            type            = "";
            skipCommasCount = 0;
         }
      }

      if (!type.empty())
      {
         aTemplateArgTypes.emplace_back(type);
      }
      ok = true;
   }
   return ok;
}

//--------------------------------------------------------------
//! Creates and returns a UtScriptRef if the provided class
//! name is valid. Otherwise it returns 0.
//! @param aClassNameId The dictionary id of the type name.
//! @param aContext The script context that is creating the instance.
//! @param aMakeNullRef Creates a reference that points to null.
//! @return Returns a pointer to the newly created script reference.
//--------------------------------------------------------------

UtScriptRef* UtScriptTypes::Create(UtStringId aClassNameId, const UtScriptContext& aInstance, bool aMakeNullRef)
{
   UtScriptRef*   refPtr   = nullptr;
   UtScriptClass* classPtr = GetClass(aClassNameId);
   if (classPtr != nullptr)
   {
      void* ptr = aMakeNullRef ? nullptr : classPtr->Create(aInstance);
      if ((ptr != nullptr) || aMakeNullRef)
      {
         refPtr = new UtScriptRef(ptr, classPtr, UtScriptRef::cMANAGE);
      }
   }
   return refPtr;
}

UtScriptRef* UtScriptTypes::Create(UtScriptClass* aClassPtr, const UtScriptContext& aInstance, bool aMakeNullRef)
{
   UtScriptRef* refPtr = nullptr;

   void* ptr = aMakeNullRef ? nullptr : aClassPtr->Create(aInstance);
   if ((ptr != nullptr) || aMakeNullRef)
   {
      refPtr = new UtScriptRef(ptr, aClassPtr, UtScriptRef::cMANAGE);
   }
   return refPtr;
}

void UtScriptTypes::Initialize()
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (mIsInitialized)
   {
      return;
   }

   mIsInitialized = true;
   for (auto& c : mClassMap)
   {
      c.second->Initialize();
   }
}

void UtScriptTypes::RegisterExtension(std::unique_ptr<UtScriptExtension> aScriptExtPtr)
{
   mScriptExtensions.emplace_back(std::move(aScriptExtPtr));
}
