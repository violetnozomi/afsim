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

#ifndef UTSCRIPTENVIRONMENT_HPP
#define UTSCRIPTENVIRONMENT_HPP

#include "util_script_export.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class UtScript;
class UtScriptContext;
class UtScriptExecutor;
#include "UtScriptFunction.hpp"
class UtScriptParser;
class UtScriptRegistry;
class UtScriptScope;
class UtScriptStruct;
#include "UtScriptTypes.hpp"

//! A script environment which can compile new scripts, define new structs, while using types in UtScriptTypes.
//! Each script environment operates independently of other script environments
class UTIL_SCRIPT_EXPORT UtScriptEnvironment
{
public:
   UtScriptEnvironment(UtScriptTypes* aTypesPtr);
   ~UtScriptEnvironment();

   using ClassMap         = UtScriptTypes::ClassMap;
   using ClassMapItr      = UtScriptTypes::ClassMapItr;
   using ConstClassMapItr = UtScriptTypes::ConstClassMapItr;

   using TemplateArgs    = UtScriptTypes::TemplateArgs;
   using TemplateType    = UtScriptTypes::TemplateType;
   using TemplateTypeMap = UtScriptTypes::TemplateTypeMap;

   UtScriptParser&   GetParser() { return *mParserPtr; }
   UtScriptExecutor& GetExecutor() { return *mExecutorPtr; }
   UtScriptRegistry* GetRegistry() { return mScriptRegistryPtr; }
   UtScriptTypes&    GetTypes() const { return *mScriptTypesPtr; }
   void              SetGlobalInstance(UtScriptContext* aInstancePtr);
   UtScriptContext*  GetGlobalInstancePrototype() { return mGlobalInstancePrototypePtr; }

   //! @name Class access methods.
   //! These are parallels to methods in UtScriptTypes, but also allow pseudo-classes (script_struct's).
   //! Note than only those methods needed to support pseudo-classes are provided.
   //@{
   bool Register(std::unique_ptr<UtScriptClass> aScriptClassPtr);

   UtScriptClass* GetClass(UtStringId aClassNameId) const;
   UtScriptClass* GetClass(const std::string& aClassName) const;
   UtScriptClass* GetClass(const char* aClassNamePtr) const;
   UtScriptClass* GetClass(const UtScriptTypes::TemplateType& aTemplateType);

   UtScriptClass* GetOrCreateContainer(const std::string& aContainerType);

   std::unique_ptr<UtScriptClass> CreateContainer(const std::string& aContainerType);

   std::unique_ptr<UtScriptClass> CreateContainer(const std::string&                     aContainerBaseTypeName,
                                                  const std::string&                     aContainerTypeName,
                                                  const UtScriptClass::TemplateArgTypes& aTemplateArgTypes);
   //@}

   //! @name 'script_struct' methods.
   //@{
   bool             AddStruct(const std::string& aName, std::unique_ptr<UtScriptStruct> aStructPtr);
   UtScriptStruct*  FindStruct(const std::string& aName) const;
   UtScriptContext* GetEmptyStruct() { return mEmptyStructInstancePtr; }
   //@}

   //! @name Script prototype methods.
   //@{
   static const int cPROTOTYPE_ID_START = 10000;

   int AddPrototype(const UtScriptFunction& aPrototype);

   const UtScriptFunction* GetPrototype(UtStringId aReturnTypeName, const std::vector<std::string>& aArgTypes);

   const UtScriptFunction* GetPrototype(UtStringId aReturnTypeName, const std::string& aCommaSeparatedArgTypes);

   const UtScriptFunction* GetPrototype(int aId);

   std::string GetPrototypeDescription(const UtScriptFunction* aPrototypePtr,
                                       const std::string&      aFunctionName = std::string()) const;
   //@}

   bool ScopeIsValid(UtScriptScope* aScopePtr) const;
   void ScopeCreated(UtScriptScope* aScopePtr);
   void ScopeDeleted(UtScriptScope* aScopePtr);

   bool ScriptIsValid(UtScript* aScriptPtr) const;
   void ScriptDeleted(UtScript* aScriptPtr);
   void ScriptCreated(UtScript* aScriptPtr);

   bool ScriptContextIsValid(UtScriptContext* aContextPtr) const;
   void ScriptContextCreated(UtScriptContext* aContextPtr);
   void ScriptContextDeleted(UtScriptContext* aContextPtr);

private:
   void Clear();

   UtScriptTypes*    mScriptTypesPtr;
   UtScriptExecutor* mExecutorPtr;
   UtScriptRegistry* mScriptRegistryPtr;
   UtScriptParser*   mParserPtr;
   UtScriptContext*  mGlobalInstancePtr;
   UtScriptContext*  mEmptyStructInstancePtr;
   UtScriptScope*    mGlobalScopePrototypePtr;
   UtScriptContext*  mGlobalInstancePrototypePtr;

   //! Local class registry
   ClassMap mClassMap;

   //! Local template registry
   TemplateTypeMap mTemplateTypes;

   // The list of 'script_struct' types.
   using StructTypeMap = std::map<std::string, std::unique_ptr<UtScriptStruct>>;
   StructTypeMap mStructTypes;

   // The list of script prototypes.
   std::vector<const UtScriptFunction*> mPrototypes;

   // Returns the prototype ID given a prototype.
   using PrototypeMap = std::map<UtScriptFunction, int>;
   PrototypeMap mPrototypeMap;

   // Only Key is used here:
   using UtScriptScopePointerMap = std::unordered_map<UtScriptScope*, bool>;
   UtScriptScopePointerMap mAliveScriptScopes;

   // Only Key is used here:
   using UtScriptPointerMap = std::unordered_map<UtScript*, bool>;
   UtScriptPointerMap mAliveScripts;

   // Only Key is used here:
   using UtScriptContextPointerMap = std::unordered_map<UtScriptContext*, bool>;
   UtScriptContextPointerMap mAliveScriptContexts;

   //! Mutex for updating STL objects.
   mutable std::recursive_mutex mClassMapMutex;
   mutable std::recursive_mutex mStructTypesMutex;
   mutable std::recursive_mutex mAliveScriptScopesMutex;
   mutable std::recursive_mutex mAliveScriptsMutex;
   mutable std::recursive_mutex mAliveScriptContextsMutex;
};

#endif
