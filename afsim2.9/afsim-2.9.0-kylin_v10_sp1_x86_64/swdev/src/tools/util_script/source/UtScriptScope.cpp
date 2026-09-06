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

#include "UtScriptScope.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "UtLog.hpp"
#include "UtScript.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptRegistry.hpp"

UtScriptScope::UtScriptScope()
   : mEnvironmentPtr(nullptr)
   , mParentPtr(nullptr)
   , mReferenceCount(0)
   , mChildReferenceCount(0)
   , mIsResolved(false)
{
}

UtScriptScope::UtScriptScope(UtScriptEnvironment* aEnvironmentPtr)
   : mEnvironmentPtr(aEnvironmentPtr)
   , mParentPtr(nullptr)
   , mReferenceCount(0)
   , mChildReferenceCount(0)
   , mIsResolved(false)
{
   mEnvironmentPtr->ScopeCreated(this);
}

UtScriptScope::UtScriptScope(const UtScriptScope& aSrc)
   : mEnvironmentPtr(aSrc.mEnvironmentPtr)
   , mParentPtr(aSrc.mParentPtr)
   , mReferenceCount(0)
   , mChildReferenceCount(0)
   , mIsResolved(false)
   , mScripts(aSrc.mScripts)
   , mVarPrototypes(aSrc.mVarPrototypes)
   , mInitScripts(aSrc.mInitScripts)
{
   mEnvironmentPtr->ScopeCreated(this);
   if (mParentPtr != nullptr)
   {
      mParentPtr->ChildAddRef();
   }
   for (auto i = mScripts.begin(); i != mScripts.end(); ++i)
   {
      if (i->second.mScriptPtr)
      {
         i->second.mScriptPtr->AddRef();
      }
   }
}

UtScriptScope::~UtScriptScope()
{
   mEnvironmentPtr->ScopeDeleted(this);
   for (auto i = mScripts.begin(); i != mScripts.end(); ++i)
   {
      if (i->second.mScriptPtr)
      {
         i->second.mScriptPtr->Unref();
      }
   }

   if (mParentPtr != nullptr)
   {
      mParentPtr->ChildUnref();
      mParentPtr = nullptr;
   }
}

//! Resolves references to external variables.
//! Returns 'false' if a reference could not be resolved.
bool UtScriptScope::ResolveReferences()
{
   if (mIsResolved)
   {
      return true;
   }
   // Note:
   //    This resolution process is strictly for user benefit via the errors printed.
   //    Externals do not need to be resolved in order to execute a script.
   if (mParentPtr != nullptr)
   {
      mParentPtr->ResolveReferences();
   }
   bool ok = true;
   for (auto i = mScripts.begin(); i != mScripts.end(); ++i)
   {
      if (!i->second.mIsLocal)
      {
         if (nullptr == FindGlobalScript(i->first))
         {
            auto out = ut::log::error() << "Could not resolve reference to extern script.";
            out.AddNote() << "Script: " << i->first;
            ok = false;
         }
      }
   }
   for (auto i = mVarPrototypes.begin(); i != mVarPrototypes.end(); ++i)
   {
      if (!i->second.mIsLocal)
      {
         bool           found    = false;
         UtScriptScope* scopePtr = mParentPtr;
         while (scopePtr)
         {
            InstanceVariable* var = scopePtr->GetInstanceVariableDef(i->first);
            if (var && var->mIsLocal)
            {
               found = true;
               break;
            }
            scopePtr = scopePtr->mParentPtr;
         }
         if (!found)
         {
            auto out = ut::log::error() << "Could not resolve reference to extern variable.";
            out.AddNote() << "Variable: " << i->first;
            ok = false;
         }
      }
   }
   mIsResolved = ok;
   return ok;
}

void UtScriptScope::SetParent(UtScriptScope* aParentPtr)
{
   if (mParentPtr == aParentPtr)
   {
      return;
   }
   if (mParentPtr != nullptr)
   {
      mParentPtr->ChildUnref();
   }
   mParentPtr = aParentPtr;
   if (mParentPtr != nullptr)
   {
      mParentPtr->ChildAddRef();
   }

   MarkChanged();
}

UtScriptScope::InstanceVariable* UtScriptScope::RegisterVariable(const std::string& aName, UtScriptClass* aTypePtr)
{
   InstanceVariable& iv = mVarPrototypes[aName];
   iv.mName             = aName;
   iv.mType             = aTypePtr;
   iv.mIsLocal          = true;
   return &iv;
}

UtScriptScope::InstanceVariable* UtScriptScope::RegisterVariable(const std::string& aName, UtStringId aType)
{
   assert(!aName.empty());
   UtScriptClass* typePtr = mEnvironmentPtr->GetClass(aType);
   if (typePtr != nullptr)
   {
      return RegisterVariable(aName, typePtr);
   }
   else
   {
      assert(false);
   }
   return nullptr;
}

UtScriptScope::InstanceVariable* UtScriptScope::RegisterExternVariable(const std::string& aName, UtScriptClass* aTypePtr)
{
   assert(!aName.empty());
   InstanceVariable* varPtr = GetInstanceVariableDef(aName);
   if (!varPtr)
   {
      varPtr           = RegisterVariable(aName, aTypePtr);
      varPtr->mIsLocal = false;
   }
   return varPtr;
}

UtScriptScope::InstanceVariable* UtScriptScope::RegisterExternVariable(const std::string& aName, UtStringId aType)
{
   UtScriptClass* classPtr = mEnvironmentPtr->GetClass(aType);
   if (classPtr != nullptr)
   {
      return RegisterExternVariable(aName, classPtr);
   }
   return nullptr;
}

//! Returns the variable declaration if one is local to this script scope
UtScriptScope::InstanceVariable* UtScriptScope::GetLocalInstanceVariableDef(const std::string& aName)
{
   auto i = mVarPrototypes.find(aName);
   if (i == mVarPrototypes.end())
   {
      return nullptr;
   }
   return &i->second;
}

UtScriptScope::InstanceVariable* UtScriptScope::GetInstanceVariableDef(const std::string& aName)
{
   auto i = mVarPrototypes.find(aName);
   if (i == mVarPrototypes.end())
   {
      if (mParentPtr != nullptr)
      {
         return mParentPtr->GetInstanceVariableDef(aName);
      }
      else
      {
         return nullptr;
      }
   }
   return &i->second;
}

const UtScriptScope::ScriptRef* UtScriptScope::FindScript(UtStringId aScriptName, bool aFindExterns) const
{
   auto i = mScripts.find(aScriptName);
   if (i != mScripts.end() && (aFindExterns | i->second.mIsLocal))
   {
      return &i->second;
   }
   return nullptr;
}

const UtScriptScope::ScriptRef* UtScriptScope::FindGlobalScript(UtStringId aScriptName, bool aFindExterns) const
{
   const UtScriptScope::ScriptRef* scriptRef = FindScript(aScriptName, aFindExterns);
   if (!scriptRef && mParentPtr)
   {
      scriptRef = mParentPtr->FindGlobalScript(aScriptName, aFindExterns);
   }
   return scriptRef;
}

bool UtScriptScope::AddScript(UtStringId aName, const UtScriptFunction* aPrototypePtr)
{
   ScriptRef* refPtr = FindScript(aName, true);
   if (refPtr && !refPtr->mIsLocal)
   {
      // allow implementation
   }
   else if (!refPtr)
   {
      ScriptRef& ref    = mScripts[aName];
      ref.mIsLocal      = false;
      ref.mPrototypePtr = aPrototypePtr;
      ref.mScriptPtr    = nullptr;
   }
   else
   {
      return false;
   }
   return true;
}

void UtScriptScope::ImplementScript(UtScript* aScriptPtr)
{
   mIsResolved       = false;
   ScriptRef* refPtr = FindScript(aScriptPtr->mName, true);
   if (refPtr)
   {
      assert(!refPtr->mIsLocal && !refPtr->mScriptPtr);        // implement only once
      assert(aScriptPtr->mPrototype == refPtr->mPrototypePtr); // must have same prototype as call to AddScript()
      refPtr->mIsLocal   = true;
      refPtr->mScriptPtr = aScriptPtr;
      mEnvironmentPtr->GetRegistry()->RegisterScript(aScriptPtr);
      if (refPtr->mScriptPtr->mIsVarScript)
      {
         mInitScripts.push_back(refPtr->mScriptPtr);
      }
   }
}

bool UtScriptScope::RemoveScript(UtStringId aScriptName)
{
   auto i = mScripts.find(aScriptName);
   if (i != mScripts.end())
   {
      if (i->second.mScriptPtr)
      {
         i->second.mScriptPtr->Unref();
      }
      mScripts.erase(i);
      return true;
   }
   return false;
}

bool UtScriptScope::ResolveScript(UtScript* aScriptPtr)
{
   bool ok = true;
   for (auto i = mVarPrototypes.begin(); i != mVarPrototypes.end(); ++i)
   {
      if (!i->second.mIsLocal)
      {
         InstanceVariable* var = GetInstanceVariableDef(i->first);
         if (!(var && var->mType == i->second.mType))
         {
            auto out = ut::log::error() << "Could not resolve script variable.";
            out.AddNote() << "Variable: " << i->first;
            out.AddNote() << "Type: " << i->second.mType->GetClassName();
            ok = false;
         }
      }
   }
   for (auto i = mScripts.begin(); i != mScripts.end(); ++i)
   {
      if (!i->second.mIsLocal)
      {
         UtScript* scriptPtr = GetScript(i->first);
         if (!scriptPtr)
         {
            auto out = ut::log::error() << "Could not resolve reference to script.";
            out.AddNote() << "Script: " << i->first;
            ok = false;
         }
         else if (scriptPtr->mPrototype != i->second.mPrototypePtr)
         {
            auto out = ut::log::error() << "Could not resolve reference to script.";
            out.AddNote() << "Script: " << i->first;
            out.AddNote() << "Definition does not match.";
            ok = false;
         }
      }
   }
   return ok;
}

UtScript* UtScriptScope::GetLocalScript(UtStringId aName) const
{
   auto i = mScripts.find(aName);
   if (i != mScripts.end())
   {
      if (i->second.mIsLocal)
      {
         return i->second.mScriptPtr;
      }
   }
   return nullptr;
}

UtScript* UtScriptScope::GetScript(UtStringId aName) const
{
   auto i = mScripts.find(aName);
   if (i != mScripts.end())
   {
      if (i->second.mIsLocal)
      {
         return i->second.mScriptPtr;
      }
   }
   if (mParentPtr)
   {
      return mParentPtr->GetScript(aName);
   }
   return nullptr;
}

const UtScriptFunction* UtScriptScope::GetScriptPrototype(UtStringId aName) const
{
   auto i = mScripts.find(aName);
   if (i != mScripts.end())
   {
      return i->second.mPrototypePtr;
   }
   if (mParentPtr)
   {
      return mParentPtr->GetScriptPrototype(aName);
   }
   return nullptr;
}

void UtScriptScope::MarkChanged()
{
   mIsResolved = false;
}

////////////////////////////////////////////////////////////////////////////////
// Reference Counting:
//    UtScriptScope keeps track of how many instances are using the scope.
//    If no instances remain, the scope is deleted.
//    If an instance wants to change a scope, it must first ensure it is the
//    sole instance using the scope.  Otherwise, the scope is duplicated.
////////////////////////////////////////////////////////////////////////////////

//! Prepares the scope for modification.
//! May return a new scope to modify if this scope is being used elsewhere.
UtScriptScope* UtScriptScope::BeginModification()
{
   auto refcount = mReferenceCount.load(std::memory_order_acquire);
   if (refcount == 1)
   {
      // This is the sole reference to the scope, so there is no need to create a new scope.
      return this;
   }
   else if (refcount > 1)
   {
      // There are other references to the scope, so a new scope must be created.

      // The new scope is created prior to releasing my reference. This is required for thread-safety.
      UtScriptScope* newScopePtr = new UtScriptScope(*this);

      newScopePtr->mReferenceCount.store(1, std::memory_order_relaxed);

      // This shouldn't cause deletion of the input scope because the count was greater than 1.
      mReferenceCount.fetch_sub(1, std::memory_order_acq_rel);

      return newScopePtr;
   }
   else
   {
      ut::log::error() << "UtScriptScope reference count error.";
      return nullptr;
   }
}

void UtScriptScope::AddRef()
{
   mReferenceCount.fetch_add(1, std::memory_order_relaxed);
}

void UtScriptScope::ChildAddRef()
{
   mChildReferenceCount.fetch_add(1, std::memory_order_relaxed);
}

//! Scopes are shared between instances.  Use Unref() instead of delete.
void UtScriptScope::Unref()
{
   auto refcount = mReferenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
   assert(refcount >= 0);
   if (refcount == 0 && 0 == mChildReferenceCount.load(std::memory_order_acquire))
   {
      delete this;
   }
}

void UtScriptScope::ChildUnref()
{
   auto childcount = mChildReferenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
   assert(childcount >= 0);
   if (childcount == 0 && 0 == mReferenceCount.load(std::memory_order_acquire))
   {
      delete this;
   }
}

UtScriptTypes* UtScriptScope::GetTypes()
{
   return mEnvironmentPtr ? &mEnvironmentPtr->GetTypes() : nullptr;
}

void UtScriptScope::RegisterReadOnlyVariableFunction(const std::string& aName, UtScriptClass* aClassPtr)
{
   RegisterVariable(aName, aClassPtr)->mIsReadOnlyFunction = true;
}
