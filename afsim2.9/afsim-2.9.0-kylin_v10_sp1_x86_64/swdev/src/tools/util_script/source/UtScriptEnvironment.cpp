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

#include "UtScriptEnvironment.hpp"

#include <algorithm>
#include <memory>
#include <sstream>

#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptParser.hpp"
#include "UtScriptRegistry.hpp"
#include "UtScriptStruct.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"

UtScriptEnvironment::UtScriptEnvironment(UtScriptTypes* aTypesPtr)
   : mScriptTypesPtr(aTypesPtr)
   , mGlobalInstancePtr(nullptr)
   , mEmptyStructInstancePtr(nullptr)
   , mClassMapMutex()
   , mStructTypesMutex()
   , mAliveScriptScopesMutex()
   , mAliveScriptsMutex()
   , mAliveScriptContextsMutex()

{
   mGlobalScopePrototypePtr    = nullptr;
   auto* globalScopePtr        = new UtScriptScope(this);
   mGlobalScopePrototypePtr    = globalScopePtr;
   mGlobalInstancePrototypePtr = new UtScriptContext(mGlobalScopePrototypePtr);

   mExecutorPtr       = nullptr;
   mScriptRegistryPtr = new UtScriptRegistry(this);
   mExecutorPtr       = new UtScriptExecutor(this);
   mParserPtr         = new UtScriptParser(this);

   mGlobalInstancePrototypePtr->RegisterVariable("MATH", UtStringIdLiteral("Math"))
      ->Set(UtScriptData(GetClass(UtStringIdLiteral("Math")), GetClass(UtStringIdLiteral("Math")), UtScriptRef::cDONT_MANAGE));
}

UtScriptEnvironment::~UtScriptEnvironment()
{
   Clear();
}

void UtScriptEnvironment::Clear()
{
   delete mParserPtr;
   mParserPtr = nullptr;
   delete mGlobalInstancePrototypePtr;
   mGlobalInstancePrototypePtr = nullptr;
   mGlobalScopePrototypePtr    = nullptr;

   delete mEmptyStructInstancePtr;
   mEmptyStructInstancePtr = nullptr;

   // Delete all variables in each struct's context before destroying the struct types themselves.
   // This prevents object lifetime issues in the case of nested structs (where one struct contains
   // an instance of another struct)
   for (auto& structType : mStructTypes)
   {
      structType.second->GetStructContext()->DeleteVariables();
   }
   mStructTypes.clear();

   delete mExecutorPtr;
   mExecutorPtr = nullptr;
   delete mScriptRegistryPtr;
   mScriptRegistryPtr = nullptr;
}

// This is a copy of UtScriptTypes::Register that is used for registering container types where
// the template argument(s) is(are) pseudo-classes.
// private
bool UtScriptEnvironment::Register(std::unique_ptr<UtScriptClass> aScriptClassPtr)
{
   aScriptClassPtr->SetEnvironment(this); // Set UtScriptClass::SetEnvironment why this is needed
   auto itr = mClassMap.find(aScriptClassPtr->GetClassName());
   if (itr == mClassMap.end())
   {
      auto valid = mClassMap.emplace(aScriptClassPtr->GetClassName(), std::move(aScriptClassPtr));
      if (valid.second)
      {
         if (mScriptTypesPtr->IsInitialized())
         {
            return valid.first->second->Initialize();
         }
         return true;
      }
   }
   return false;
}

UtScriptClass* UtScriptEnvironment::GetClass(UtStringId aClassNameId) const
{
   // First try the compiled-in classes and then the user-defined pseudo-classes (script_struct).
   UtScriptClass* classPtr = mScriptTypesPtr->GetClass(aClassNameId);
   if (classPtr == nullptr)
   {
      classPtr = FindStruct(aClassNameId);
   }
   if (classPtr == nullptr)
   {
      auto itr = mClassMap.find(aClassNameId);
      if (itr != mClassMap.end())
      {
         classPtr = itr->second.get();
      }
   }
   return classPtr;
}

UtScriptClass* UtScriptEnvironment::GetClass(const std::string& aClassName) const
{
   if (UtStringId::Exists(aClassName))
   {
      return GetClass(UtStringId(aClassName));
   }
   return nullptr;
}

UtScriptClass* UtScriptEnvironment::GetClass(const char* aClassNamePtr) const
{
   // First try the compiled-in classes and then the user-defined pseudo-classes (script_struct).
   UtScriptClass* classPtr = mScriptTypesPtr->GetClass(aClassNamePtr);
   if (classPtr == nullptr)
   {
      classPtr = FindStruct(aClassNamePtr);
   }
   return classPtr;
}

UtScriptClass* UtScriptEnvironment::GetClass(const UtScriptTypes::TemplateType& aTemplateType)
{
   UtScriptClass* classPtr = mScriptTypesPtr->GetClass(aTemplateType);
   if (classPtr == nullptr)
   {
      // The class is not defined. This could be because one (or both of the template arguments) are
      // pseudo-classes, which UtScriptTypes doesn't (and can't) know about. We maintain our own registry
      // in such cases. It's a bit more work, but oh well...

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
            UtScriptClass::TemplateArgTypes tempArgs;
            tempArgs.emplace_back(firstArgPtr->GetClassName());
            UtScriptClass* secondArgPtr = GetClass(aTemplateType.second.second);
            if (secondArgPtr != nullptr)
            {
               tempArgs.emplace_back(secondArgPtr->GetClassName());
               containerName += ',' + secondArgPtr->GetClassName().GetString() + '>';
               auto containerPtr = CreateContainer(basePtr->GetClassName(), containerName, tempArgs);
               classPtr          = containerPtr.get();
               if (containerPtr != nullptr)
               {
                  Register(std::move(containerPtr));
               }
               return classPtr;
            }
            else
            {
               containerName += '>';
               auto containerPtr = CreateContainer(basePtr->GetClassName(), containerName, tempArgs);
               classPtr          = containerPtr.get();
               if (containerPtr != nullptr)
               {
                  Register(std::move(containerPtr));
               }
               return classPtr;
            }
         }
      }
   }
   return classPtr;
}

// Gets or creates a container, but allows our locally maintained types as well as the built-in types.
// See UtScriptTypes::GetOrCreateContainer.
UtScriptClass* UtScriptEnvironment::GetOrCreateContainer(const std::string& aContainerType)
{
   // remove any extra spaces
   std::string typeName(aContainerType);
   typeName.resize(std::remove(typeName.begin(), typeName.end(), ' ') - typeName.begin());
   std::lock_guard<std::recursive_mutex> lock(mClassMapMutex);
   auto                                  containerPtr = GetClass(typeName);
   if (containerPtr == nullptr)
   {
      auto newContainerPtr = CreateContainer(typeName);
      if (newContainerPtr != nullptr)
      {
         containerPtr = newContainerPtr.get();
         if (!Register(std::move(newContainerPtr)))
         {
            containerPtr = nullptr;
         }
      }
   }
   return containerPtr;
}

//--------------------------------------------------------------
//! Creates the specified container type. Returns a pointer to the
//! UtScriptClass object.
//! Note: The type in not registered with the types manager,
//! (UtScriptTypes).
//!
//! aContainerType: The container type with template arguments in
//!                 angle brackets. For example, Map<int, string>
//--------------------------------------------------------------
std::unique_ptr<UtScriptClass> UtScriptEnvironment::CreateContainer(const std::string& aContainerType)
{
   // Parse the container spec into its base type and arguments.
   std::string                     containerType(aContainerType);
   std::string                     containerBaseType;
   UtScriptClass::TemplateArgTypes argTypes;
   if (UtScriptTypes::ParseContainerType(containerType, containerBaseType, argTypes))
   {
      // Try to create the container.
      std::lock_guard<std::recursive_mutex> lock(mClassMapMutex);
      return CreateContainer(containerBaseType, containerType, argTypes);
   }
   return nullptr;
}

std::unique_ptr<UtScriptClass> UtScriptEnvironment::CreateContainer(const std::string& aContainerBaseTypeName,
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
         auto rtnTypePtr = ut::make_unique<UtScriptArray>(aContainerTypeName, mScriptTypesPtr, argTypes);
         rtnTypePtr->SetEnvironment(this);

         mTemplateTypes[TemplateType(GetClass(UtStringIdLiteral("Array"))->GetClassName(),
                                     TemplateArgs(aTemplateArgTypes[0].mTypeId, UtStringId()))] = rtnTypePtr.get();
         return std::move(rtnTypePtr);
      }
   }
   else if (aContainerBaseTypeName == "Map")
   {
      if (aTemplateArgTypes.size() == 2)
      {
         UtScriptClass::TemplateArgTypes argTypes;
         argTypes.insert(argTypes.end(), aTemplateArgTypes.begin(), aTemplateArgTypes.end());
         auto rtnTypePtr = ut::make_unique<UtScriptMap>(aContainerTypeName, mScriptTypesPtr, argTypes);
         rtnTypePtr->SetEnvironment(this);
         mTemplateTypes[TemplateType(GetClass(UtStringIdLiteral("Map"))->GetClassName(),
                                     TemplateArgs(aTemplateArgTypes[0].mTypeId, aTemplateArgTypes[1].mTypeId))] =
            rtnTypePtr.get();
         return std::move(rtnTypePtr);
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

         auto rtnTypePtr = ut::make_unique<UtScriptSet>(aContainerTypeName, mScriptTypesPtr, argTypes);
         rtnTypePtr->SetEnvironment(this);
         mTemplateTypes[TemplateType(GetClass(UtStringIdLiteral("Set"))->GetClassName(),
                                     TemplateArgs(aTemplateArgTypes[0].mTypeId, UtStringId()))] = rtnTypePtr.get();
         return std::move(rtnTypePtr);
      }
   }
   return nullptr;
}

//! Adds a script prototype and returns a unique ID for that prototype
int UtScriptEnvironment::AddPrototype(const UtScriptFunction& aPrototype)
{
   auto iter = mPrototypeMap.find(aPrototype);
   if (iter == mPrototypeMap.end())
   {
      int              protoId                       = (int)mPrototypes.size() + cPROTOTYPE_ID_START;
      UtScriptFunction proto                         = aPrototype;
      proto.mReturnClassPtr                          = GetClass(aPrototype.mReturnType);
      std::pair<PrototypeMap::iterator, bool> result = mPrototypeMap.insert(std::make_pair(proto, protoId));
      mPrototypes.push_back(&result.first->first);
      return protoId;
   }
   else
   {
      return iter->second;
   }
}

const UtScriptFunction* UtScriptEnvironment::GetPrototype(UtStringId                      aReturnTypeName,
                                                          const std::vector<std::string>& aArgTypes)
{
   UtScriptFunction fn;
   fn.mReturnClassPtr = GetClass(aReturnTypeName);
   if (fn.mReturnClassPtr != nullptr)
   {
      fn.mReturnType = fn.mReturnClassPtr->GetClassName();
      for (const auto& argType : aArgTypes)
      {
         UtScriptClass* argTypePtr = GetClass(argType);
         if (argTypePtr != nullptr)
         {
            fn.mArgs.push_back(argTypePtr->GetClassName());
         }
         else
         {
            return nullptr;
         }
      }
   }
   else
   {
      return nullptr;
   }
   return GetPrototype(AddPrototype(fn));
}

const UtScriptFunction* UtScriptEnvironment::GetPrototype(UtStringId         aReturnTypeName,
                                                          const std::string& aCommaSeparatedArgTypes)
{
   std::vector<std::string> args;
   if (!aCommaSeparatedArgTypes.empty())
   {
      UtStringUtil::Parse(aCommaSeparatedArgTypes, args, ",");
      for (auto& arg : args)
      {
         UtStringUtil::TrimWhiteSpace(arg);
      }
   }
   return GetPrototype(aReturnTypeName, args);
}

const UtScriptFunction* UtScriptEnvironment::GetPrototype(int aId)
{
   int id = aId - cPROTOTYPE_ID_START;
   if ((id >= 0) && (id < (int)mPrototypes.size()))
   {
      return mPrototypes[id];
   }
   return nullptr;
}

std::string UtScriptEnvironment::GetPrototypeDescription(const UtScriptFunction* aPrototypePtr,
                                                         const std::string&      aFunctionName) const
{
   std::stringstream ss;
   if (aPrototypePtr->mReturnClassPtr != nullptr)
   {
      ss << aPrototypePtr->mReturnClassPtr->GetClassName();
   }
   else
   {
      ss << "?";
   }
   ss << " " << aFunctionName << "(";
   for (size_t i = 0; i < aPrototypePtr->mArgs.size(); ++i)
   {
      UtScriptClass* classPtr = GetClass(aPrototypePtr->mArgs[i]);
      if (i > 0)
      {
         ss << ", ";
      }
      if (classPtr != nullptr)
      {
         ss << classPtr->GetClassName();
      }
      else
      {
         ss << "?";
      }
   }
   ss << ")";
   return ss.str();
}

bool UtScriptEnvironment::AddStruct(const std::string& aName, std::unique_ptr<UtScriptStruct> aStructPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mStructTypesMutex);
   auto                                  result = mStructTypes.emplace(aName, std::move(aStructPtr));
   return result.second;
}

UtScriptStruct* UtScriptEnvironment::FindStruct(const std::string& aName) const
{
   std::lock_guard<std::recursive_mutex> lock(mStructTypesMutex);
   auto                                  i = mStructTypes.find(aName);
   if (i != mStructTypes.end())
   {
      return i->second.get();
   }
   return nullptr;
}

void UtScriptEnvironment::SetGlobalInstance(UtScriptContext* aInstancePtr)
{
   mGlobalInstancePtr = aInstancePtr;
   delete mEmptyStructInstancePtr;

   mEmptyStructInstancePtr = new UtScriptContext(new UtScriptScope(this));
   mEmptyStructInstancePtr->SetParent(mGlobalInstancePtr);

   std::lock_guard<std::recursive_mutex> lock(mStructTypesMutex);
   for (auto& structType : mStructTypes)
   {
      structType.second->GetStructContext()->SetParent(mGlobalInstancePtr);
   }
}

bool UtScriptEnvironment::ScopeIsValid(UtScriptScope* aScopePtr) const
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptScopesMutex);
   return mAliveScriptScopes.find(aScopePtr) != mAliveScriptScopes.end();
}

void UtScriptEnvironment::ScopeCreated(UtScriptScope* aScopePtr)
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptScopesMutex);
   mAliveScriptScopes[aScopePtr];
}

void UtScriptEnvironment::ScopeDeleted(UtScriptScope* aScopePtr)
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptScopesMutex);
   mAliveScriptScopes.erase(aScopePtr);
}

void UtScriptEnvironment::ScriptCreated(UtScript* aScriptPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptsMutex);
   mAliveScripts[aScriptPtr];
}

void UtScriptEnvironment::ScriptDeleted(UtScript* aScriptPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptsMutex);
   mAliveScripts.erase(aScriptPtr);
}

bool UtScriptEnvironment::ScriptIsValid(UtScript* aScriptPtr) const
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptsMutex);
   return mAliveScripts.find(aScriptPtr) != mAliveScripts.end();
}

bool UtScriptEnvironment::ScriptContextIsValid(UtScriptContext* aContextPtr) const
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptContextsMutex);
   return mAliveScriptContexts.find(aContextPtr) != mAliveScriptContexts.end();
}

void UtScriptEnvironment::ScriptContextCreated(UtScriptContext* aContextPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptContextsMutex);
   mAliveScriptContexts[aContextPtr];
}

void UtScriptEnvironment::ScriptContextDeleted(UtScriptContext* aContextPtr)
{
   std::lock_guard<std::recursive_mutex> lock(mAliveScriptContextsMutex);
   mAliveScriptContexts.erase(aContextPtr);
}
