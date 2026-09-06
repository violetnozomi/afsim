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

#ifndef UTSCRIPTSCOPE_HPP
#define UTSCRIPTSCOPE_HPP

#include "util_script_export.h"

#include <atomic>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

class UtScript;
class UtScriptClass;
#include "UtScriptData.hpp"
class UtScriptEnvironment;
class UtScriptFunction;

//! Each script context has a 'scope'.  The scope manages scripts and variable declarations shared
//! by many script contexts.  Just like script contexts, the scope may have a parent scope
//! from which scripts and variables are visible.
//! Access to UtScriptScope directly is not usually necessary, instead, use UtScriptContext when possible.
class UTIL_SCRIPT_EXPORT UtScriptScope
{
public:
   friend class UtScriptContext;

   UtScriptScope();
   UtScriptScope(UtScriptEnvironment* aEnvironmentPtr);
   UtScriptScope(const UtScriptScope& aScope);
   ~UtScriptScope();

   //! Any global variable used by this scope, local or not, is stored here
   struct InstanceVariable
   {
      UtScriptClass* mType = nullptr;
      std::string    mName = "";
      //! True if this variable is owned by this scope.
      bool mIsLocal            = false;
      bool mIsReadOnlyFunction = false;
   };

   using VarProtoMap = std::unordered_map<std::string, InstanceVariable>;

   //! Any script used by this scope, local or not, is stored here
   struct ScriptRef
   {
      //! True if this script is owned by this scope
      bool                    mIsLocal      = false;
      const UtScriptFunction* mPrototypePtr = nullptr;
      UtScript*               mScriptPtr    = nullptr;
   };

   using ScriptMap = std::unordered_map<UtStringId, ScriptRef>;

   InstanceVariable* RegisterVariable(const std::string& aName, UtScriptClass* aTypePtr);
   InstanceVariable* RegisterVariable(const std::string& aName, UtStringId aType);
   InstanceVariable* RegisterExternVariable(const std::string& aName, UtScriptClass* aTypePtr);
   InstanceVariable* RegisterExternVariable(const std::string& aName, UtStringId aType);

   void RegisterReadOnlyVariableFunction(const std::string& aName, UtScriptClass* aClassPtr);

   InstanceVariable* GetInstanceVariableDef(const std::string& aName);
   InstanceVariable* GetLocalInstanceVariableDef(const std::string& aName);

   UtScript*               GetScript(UtStringId aName) const;
   UtScript*               GetLocalScript(UtStringId aName) const;
   const UtScriptFunction* GetScriptPrototype(UtStringId aName) const;

   bool AddScript(UtStringId aName, const UtScriptFunction* aPrototypePtr);

   void ImplementScript(UtScript* aScriptPtr);

   bool RemoveScript(UtStringId aScriptName);

   const ScriptRef* FindScript(UtStringId aScriptName, bool aFindExterns = false) const;
   ScriptRef*       FindScript(UtStringId aScriptName, bool aFindExterns = false)
   {
      return const_cast<ScriptRef*>(const_cast<const UtScriptScope*>(this)->FindScript(aScriptName, aFindExterns));
   }

   const ScriptRef* FindGlobalScript(UtStringId aScriptName, bool aFindExterns = false) const;

   UtScriptTypes*       GetTypes();
   UtScriptEnvironment* GetEnvironment() { return mEnvironmentPtr; }

   UtScriptScope* BeginModification();

   void           AddRef();
   void           Unref();
   int            RefCount() const { return mReferenceCount; }
   UtScriptScope* GetParent() { return mParentPtr; }

   int GetScriptCount() const { return static_cast<int>(mScripts.size()); }

   const ScriptMap& GetScripts() const { return mScripts; }
   //! Returns the list of 'script_variables' scripts in order of definition
   const std::vector<UtScript*>& GetInitScripts() const { return mInitScripts; }

private:
   void ChildAddRef();
   void ChildUnref();

   bool ResolveReferences();
   void SetParent(UtScriptScope* aParentPtr);

   void MarkChanged();

   bool ResolveScript(UtScript* aScriptPtr);

   UtScriptEnvironment* mEnvironmentPtr;
   UtScriptScope*       mParentPtr;

   //! Number of instances using this scope
   //! More than one reference means this scope must remain read-only
   //! for copy-on-write semantics
   mutable std::atomic<int> mReferenceCount;
   //! Number of child scopes.
   //! Independent of mReferenceCount as to not trigger copy-on-write.
   mutable std::atomic<int> mChildReferenceCount;
   //! True if externals have been resolved
   bool mIsResolved;

   ScriptMap              mScripts;
   VarProtoMap            mVarPrototypes;
   std::vector<UtScript*> mInitScripts;
};

#endif
