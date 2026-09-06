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

#ifndef UTSCRIPTTYPES_HPP
#define UTSCRIPTTYPES_HPP

#include "util_script_export.h"

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class UtDictionary;
class UtScriptAccessible;
class UtScriptContext;
#include "UtScriptClass.hpp"
#include "UtScriptExtension.hpp"
#include "UtStringId.hpp"

//--------------------------------------------------------------
//! UtScriptTypes is responsible for managing the collection of
//! UtScriptClass'. During script compilation the parser will use
//! the UtScriptTypes instance to type check class types that it
//! encounters. During runtime the UtScriptContext, which
//! executes the script will use the UtScriptTypes instance
//! to dynamically create objects.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT UtScriptTypes
{
public:
   UtScriptTypes(UtDictionary& aDictionary);
   ~UtScriptTypes() = default;

   void Initialize();
   bool IsInitialized() const { return mIsInitialized; }

   using ClassMap         = std::map<UtStringId, std::unique_ptr<UtScriptClass>>;
   using ClassMapItr      = ClassMap::iterator;
   using ConstClassMapItr = ClassMap::const_iterator;

   using TypeList         = std::vector<UtStringId>;
   using TypeListItr      = TypeList::iterator;
   using ConstTypeListItr = TypeList::const_iterator;

   using TemplateArgs    = std::pair<UtStringId, UtStringId>;
   using TemplateType    = std::pair<UtStringId, TemplateArgs>;
   using TemplateTypeMap = std::map<TemplateType, UtScriptClass*>;

   using ScriptExtensionList = std::vector<std::unique_ptr<UtScriptExtension>>;

   UtDictionary& GetDictionary() const { return mDictionary; }

   //--------------------------------------------------------------
   //! Register a script class.
   //!
   //! Users should register each of the UtScriptClass types that they define. Only one copy of each type
   //! should be registered and registration should occur before any scripts are parsed. The script class
   //! memory will be managed by this UtScriptTypes instance.
   //!
   //! @returns true if the class is properly registered.
   //! @note Do not assume that aScriptClassPtr is valid after the call.  It is read-only,
   //! so you *must* check the return value instead.
   //@{
   bool Register(std::unique_ptr<UtScriptClass> aScriptClassPtr);
   //@}

   bool AddClassMethod(UtStringId aClassName, std::unique_ptr<UtScriptClass::InterfaceMethod> aMethodPtr);
   bool AddStaticClassMethod(UtStringId aClassName, std::unique_ptr<UtScriptClass::InterfaceMethod> aMethodPtr);

   UtScriptClass* GetClass(UtStringId aClassId) const;
   UtScriptClass* GetClass(const TemplateType& aTemplateType);
   UtScriptClass* GetClass(const std::string& aClassName) const;
   UtScriptClass* GetClass(UtScriptAccessible* aAccessiblePtr) const;
   UtScriptClass* GetClass(const char* aClassName) const { return GetClass(std::string(aClassName)); }
   UtScriptClass* GetClassFromCharArray(const char* aCharArray) const;

   const ClassMap& GetScriptClasses() const { return mClassMap; }
   //! Iterator that points to the being of the list of class types.
   ConstClassMapItr Begin() const { return mClassMap.begin(); }
   //! Iterator that points to the end of the list of class types.
   ConstClassMapItr End() const { return mClassMap.end(); }

   UtStringId GetTypeId(const std::string& aTypeName) const;

   std::string GetTypeName(UtStringId aTypeId) const;

   UtScriptClass* GetOrCreateContainer(const std::string& aContainerType);

   std::unique_ptr<UtScriptClass> CreateContainer(const std::string& aContainerType);

   std::unique_ptr<UtScriptClass> CreateContainer(const std::string&                     aContainerBaseTypeName,
                                                  const std::string&                     aContainerTypeName,
                                                  const UtScriptClass::TemplateArgTypes& aTemplateArgTypes);

   static bool ParseContainerType(std::string&                     aContainerType,
                                  std::string&                     aContainerBaseType,
                                  UtScriptClass::TemplateArgTypes& aTemplateArgTypes);

   UtScriptRef* Create(UtStringId aClassNameId, const UtScriptContext& aContext, bool aMakeNullRef = false);

   static UtScriptRef* Create(UtScriptClass* aClassPtr, const UtScriptContext& aContext, bool aMakeNullRef = false);

   void RegisterExtension(std::unique_ptr<UtScriptExtension> aScriptExtPtr);

   ScriptExtensionList& GetScriptExtensions() { return mScriptExtensions; }

private:
   mutable std::recursive_mutex                            mMutex;
   bool                                                    mIsInitialized;
   ClassMap                                                mClassMap;
   mutable std::unordered_map<const char*, UtScriptClass*> mClassByCompiledNameMap;
   UtDictionary&                                           mDictionary;
   TemplateTypeMap                                         mTemplateTypes;
   ScriptExtensionList                                     mScriptExtensions;
};

#endif
