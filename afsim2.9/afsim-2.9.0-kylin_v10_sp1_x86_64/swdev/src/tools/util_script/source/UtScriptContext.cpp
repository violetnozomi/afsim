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

#include "UtScriptContext.hpp"

#include <cassert>

#include "UtLog.hpp"
#include "UtScript.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptParser.hpp"
#include "UtScriptScope.hpp"

UtScriptContext::UtScriptContext()
   : mScopePtr(nullptr)
   , mParentInstancePtr(nullptr)
   , mVarReadFunctionMapPtr(nullptr)
   , mFlags(0)
{
}

UtScriptContext::UtScriptContext(UtScriptEnvironment* aEnvironmentPtr)
   : mScopePtr(new UtScriptScope(aEnvironmentPtr))
   , mParentInstancePtr(nullptr)
   , mVarReadFunctionMapPtr(nullptr)
   , mFlags(0)
{
   aEnvironmentPtr->ScriptContextCreated(this);
   mScopePtr->AddRef();
}

UtScriptContext::UtScriptContext(UtScriptScope* aScopePtr)
   : mScopePtr(aScopePtr)
   , mParentInstancePtr(nullptr)
   , mVarReadFunctionMapPtr(nullptr)
   , mFlags(0)
{
   GetEnvironment()->ScriptContextCreated(this);
   mScopePtr->AddRef();
}

//! Copy
UtScriptContext::UtScriptContext(const UtScriptContext& aSrc)
   : mScopePtr(aSrc.mScopePtr)
   , mParentInstancePtr(aSrc.mParentInstancePtr)
   , mInstanceVariables(aSrc.mInstanceVariables)
   , mVarReadFunctionMapPtr(aSrc.mVarReadFunctionMapPtr)
   , mFlags(aSrc.mFlags)
{
   GetEnvironment()->ScriptContextCreated(this);
   mScopePtr->AddRef();
   if (mVarReadFunctionMapPtr != nullptr)
   {
      mVarReadFunctionMapPtr = new VarReadFunctionMap(*mVarReadFunctionMapPtr);
   }
}

UtScriptContext& UtScriptContext::operator=(const UtScriptContext& aRhs)
{
   if (this == &aRhs)
      return *this;
   if (mScopePtr != aRhs.mScopePtr)
   {
      mScopePtr->Unref();
      mScopePtr = aRhs.mScopePtr;
      mScopePtr->AddRef();
   }
   DeleteVariables();
   ParentChanging(aRhs.mParentInstancePtr);
   mInstanceVariables = aRhs.mInstanceVariables;

   if (mVarReadFunctionMapPtr != nullptr)
   {
      delete mVarReadFunctionMapPtr;
   }
   mVarReadFunctionMapPtr = aRhs.mVarReadFunctionMapPtr;
   if (mVarReadFunctionMapPtr != nullptr)
   {
      mVarReadFunctionMapPtr = new VarReadFunctionMap(*mVarReadFunctionMapPtr);
   }

   mFlags = aRhs.mFlags;
   return *this;
}

UtScriptContext::~UtScriptContext()
{
   if (GetEnvironment())
      GetEnvironment()->ScriptContextDeleted(this);
   ParentChanging(nullptr);
   DeleteVariables();
   mScopePtr->Unref();
   delete mVarReadFunctionMapPtr;
}

void UtScriptContext::DeleteVariables()
{
   mInstanceVariables.clear();
}

//! Initialize the instance by resolving references.
bool UtScriptContext::Initialize()
{
   // make sure scopes are pointing to the latest version before
   // resolving references
   CheckScopeParent();
   bool ok = mScopePtr->ResolveReferences();
   if (ok && !IsInitialized())
   {
      mFlags |= cINITIALIZED_FLAG;
   }
   return ok;
}

//! Executes the specified script.  The caller should ensure the script belongs to this instance
//! before calling Execute()
void UtScriptContext::Execute(UtScriptExecutor*       aExecutorPtr,
                              UtScript*               aScriptPtr,
                              UtScriptData&           aReturnVal,
                              const UtScriptDataList& aArgs)
{
   assert(mScopePtr->mScripts.find(aScriptPtr->GetName()) != mScopePtr->mScripts.end());
   aExecutorPtr->Execute(aScriptPtr, aReturnVal, aArgs, *this);
}

//! Executes the script with the specified name
bool UtScriptContext::Execute(UtScriptExecutor*       aExecutorPtr,
                              const std::string&      aName,
                              UtScriptData&           aReturnVal,
                              const UtScriptDataList& aArgs)
{
   UtScript* scriptPtr = mScopePtr->GetScript(aName);
   if (scriptPtr != nullptr)
   {
      aExecutorPtr->Execute(scriptPtr, aReturnVal, aArgs, *this);
      return true;
   }
   else if (mParentInstancePtr != nullptr)
   {
      return mParentInstancePtr->Execute(aExecutorPtr, aName, aReturnVal, aArgs);
   }
   return false;
}

//! Executes the script with the specified name
bool UtScriptContext::ExecuteInternal(UtScriptExecutor* aExecutorPtr, UtStringId aName, UtScriptData& aReturnVal, size_t aNumArgs)
{
   UtScript* scriptPtr = mScopePtr->GetScript(aName);
   if (scriptPtr != nullptr)
   {
      aExecutorPtr->ExecuteInternal(scriptPtr, aReturnVal, aNumArgs, *this);
      return true;
   }
   else if (mParentInstancePtr != nullptr)
   {
      return mParentInstancePtr->ExecuteInternal(aExecutorPtr, aName, aReturnVal, aNumArgs);
   }
   return false;
}

//! Registers a new script variable on this instance.
//! The new variable will be available to scripts compiled with this instance.
//! Returns a pointer to the variable's value which will remain valid as long as
//! this context exists and the variable isn't unregistered.
UtScriptData* UtScriptContext::RegisterVariable(const std::string& aName, UtScriptClass* aTypePtr)
{
   UtScriptData*                    dataPtr   = nullptr;
   UtScriptScope::InstanceVariable* varDefPtr = mScopePtr->GetInstanceVariableDef(aName);
   if (varDefPtr != nullptr)
   {
      dataPtr = &Var(varDefPtr->mName);
   }
   else
   {
      ModifyScope();
      varDefPtr = mScopePtr->RegisterVariable(aName, aTypePtr);
      dataPtr   = &Var(varDefPtr->mName);
   }
   *dataPtr = UtScriptData(aTypePtr);
   return dataPtr;
}

//! Registers a new script variable on this instance.
//! The new variable will be available to scripts compiled with this instance.
//! Returns a pointer to the variable's value which will remain valid as long as
//! this context exists and the variable isn't unregistered.
UtScriptData* UtScriptContext::RegisterVariable(const std::string& aName, UtStringId aType)
{
   UtScriptClass* classPtr = GetTypes()->GetClass(aType);
   if (classPtr != nullptr)
   {
      return RegisterVariable(aName, classPtr);
   }
   return nullptr;
}

//! Registers a pseudo script variable on this context.
//! The variable may only be read by scripts, and results in a function call to allow
//! computation of the value just in time.
void UtScriptContext::RegisterReadOnlyVariableFunction(const std::string& aName, UtStringId aType)
{
   UtScriptClass* classPtr = GetTypes()->GetClass(aType);
   if (classPtr != nullptr)
   {
      ModifyScope();
      mScopePtr->RegisterReadOnlyVariableFunction(aName, classPtr);
   }
}

//! Registers a 'this' application variable with the specified
//! type.  The 'this' pointer determines which type's methods are
//! available in the local scope.  This allows
//! any scripts which are defined inside a Platform to call its
//! methods via the 'this' pointer (i.e. this.Name() or Name()).
//!
//! @param aType: The 'this' pointers type.
void UtScriptContext::RegisterThisPointer(UtStringId aType)
{
   RegisterVariable("this", aType);
}

//! Sets the 'this' pointer.
//!
//! @param aDataPtr: A pointer to the data.
//! @return Returns true if the 'this' variable was set.
bool UtScriptContext::SetThisPointer(void* aThisPtr)
{
   UtScriptData* varPtr = FindVar("this");
   if (varPtr != nullptr)
   {
      UtScriptClass* classPtr = varPtr->GetPointer()->GetScriptClass();
      varPtr->SetPointer(new UtScriptRef(aThisPtr, classPtr, UtScriptRef::cDONT_MANAGE));
   }
   return varPtr != nullptr;
}

// Find the parent instance that matches this scope.
UtScriptContext* UtScriptContext::GetInstanceForScope(UtScriptScope* aScopePtr)
{
   UtScriptContext* instancePtr = this;
   while (instancePtr != nullptr)
   {
      if (instancePtr->mScopePtr == aScopePtr)
         return instancePtr;
      instancePtr = instancePtr->mParentInstancePtr;
   }
   return nullptr;
}

//! Returns the UtScriptTypes object
UtScriptTypes* UtScriptContext::GetTypes() const
{
   return (mScopePtr != nullptr) ? mScopePtr->GetTypes() : nullptr;
}

UtScriptEnvironment* UtScriptContext::GetEnvironment() const
{
   return (mScopePtr != nullptr) ? mScopePtr->GetEnvironment() : nullptr;
}

//! Finds a script on this instance or any parent.  Stores the instance
//! that has the script in aInstance.
UtScript* UtScriptContext::GetScript(UtStringId aName, UtScriptContext*& aInstancePtr)
{
   UtScript* scriptPtr = nullptr;
   aInstancePtr        = this;
   while (aInstancePtr != nullptr)
   {
      scriptPtr = aInstancePtr->FindScript(aName);
      if (scriptPtr != nullptr)
      {
         break;
      }
      aInstancePtr = aInstancePtr->mParentInstancePtr;
   }
   return scriptPtr;
}

//! Update the scope parent if necessary
void UtScriptContext::CheckScopeParent()
{
   // If we have a parent, the parent may have modified its scope and therefore have another copy
   // Update the parent to match context's parent
   // We can get away with this because ModifyScope() is called before compiling any new methods
   if (GetParent())
   {
      GetParent()->CheckScopeParent();
      if (GetParent()->GetScope() != mScopePtr->GetParent())
      {
         mScopePtr = mScopePtr->BeginModification();
         mScopePtr->SetParent(mParentInstancePtr->GetScope());
      }
   }
}

//! Because script scopes may be shared, a check is done before changing the scope.  If the scope is used
//! by other instances, then it must be copied.
UtScriptScope* UtScriptContext::ModifyScope()
{
   mScopePtr = mScopePtr->BeginModification();
   CheckScopeParent();
   return mScopePtr;
}

//! Sets the parent instance.
void UtScriptContext::SetParent(UtScriptContext* aParentPtr)
{
   UtScriptScope* parentScopePtr    = mScopePtr->GetParent();
   UtScriptScope* newParentScopePtr = aParentPtr->GetScope();
   if (newParentScopePtr != parentScopePtr)
   {
      ModifyScope();
      mScopePtr->SetParent(aParentPtr->GetScope());
   }
   ParentChanging(aParentPtr);
}

//! Returns a variable with the specified name.  Creates a variable if none exists.
UtScriptData& UtScriptContext::Var(UtStringId aName)
{
   auto& sd = mInstanceVariables[aName];
   if (sd == nullptr)
   {
      sd = ut::make_unique<UtScriptData>();
   }
   return *sd;
}

//! Returns a variable if it exists.
UtScriptData* UtScriptContext::FindVar(UtStringId aName) const
{
   auto i = mInstanceVariables.find(aName);
   if (i != mInstanceVariables.end())
   {
      return i->second.get();
   }
   return nullptr;
}

//! Returns a variable if it exists on this instance, or any parent.
UtScriptData* UtScriptContext::FindGlobalVar(UtStringId aName) const
{
   auto i = mInstanceVariables.find(aName);
   if (i != mInstanceVariables.end())
   {
      return i->second.get();
   }
   if (mParentInstancePtr == nullptr)
   {
      return nullptr;
   }
   return mParentInstancePtr->FindGlobalVar(aName);
}

//! Returns the script with the specified name if it exists on this instance.
UtScript* UtScriptContext::FindScript(UtStringId aScriptName)
{
   UtScriptScope::ScriptRef* refPtr = mScopePtr->FindScript(aScriptName);
   if (refPtr != nullptr)
   {
      return refPtr->mScriptPtr;
   }
   return nullptr;
}

//! Executes scripts designated as 'initial' scripts, usually defined in 'script_variables' blocks.
void UtScriptContext::ExecuteInitScripts(UtScriptExecutor* aExecutorPtr)
{
   const std::vector<UtScript*>& scripts = mScopePtr->GetInitScripts();
   for (auto script : scripts)
   {
      UtScriptData     rval;
      UtScriptDataList args;
      Execute(aExecutorPtr, script, rval, args);
   }
}

//! Prints the contents of the instance to a stream
void UtScriptContext::Print(ut::log::MessageStream& aStream)
{
   for (const auto& var : mInstanceVariables)
   {
      aStream.AddNote() << var.first << ": " << *var.second;
   }
}

UtScript* UtScriptContext::Parse(UtScriptStream* aStreamPtr, const std::string& aNamespace)
{
   return GetEnvironment()->GetParser().Parse(aStreamPtr, this, aNamespace);
}

UtScript* UtScriptContext::Parse(const std::string& aScriptName,
                                 UtStringId         aScriptReturnType,
                                 UtScriptStream*    aStreamPtr,
                                 const std::string& aNamespace,
                                 bool               aIsVariableBlock)
{
   return GetEnvironment()->GetParser().Parse(aScriptName, aScriptReturnType, aStreamPtr, this, aNamespace, aIsVariableBlock);
}

bool UtScriptContext::WritesEnabled() const
{
   return 0 != (mFlags & UtScriptContext::cDEBUG_WRITE_FLAG);
}

void UtScriptContext::SetWritesEnabled(bool aWritesEnabled)
{
   if (aWritesEnabled)
   {
      mFlags |= UtScriptContext::cDEBUG_WRITE_FLAG;
      mFlags &= ~UtScriptContext::cDEBUG_WRITE_OFF_FLAG;
   }
   else
   {
      mFlags &= ~UtScriptContext::cDEBUG_WRITE_FLAG;
      mFlags |= UtScriptContext::cDEBUG_WRITE_OFF_FLAG;
   }
}

bool UtScriptContext::IsVariableDefined(UtStringId aName)
{
   return GetScope()->GetInstanceVariableDef(aName) != nullptr;
}

void UtScriptContext::ParentChanging(UtScriptContext* aNewParent)
{
   if (aNewParent == mParentInstancePtr)
      return;
   mParentInstancePtr = aNewParent;
}

void UtScriptContext::SetContextName(const std::string& aContextName)
{
   assert(!aContextName.empty());
   mContextName = aContextName;
}

UtScriptContext* UtScriptContext::GetRoot()
{
   UtScriptContext* rootPtr = this;
   while (rootPtr->mParentInstancePtr != nullptr)
   {
      rootPtr = rootPtr->mParentInstancePtr;
   }
   return rootPtr;
}

void UtScriptContext::SetReadOnlyVariableFunction(UtStringId aName, VariableReadFunction* aFunctionPtr)
{
   if (mVarReadFunctionMapPtr == nullptr)
   {
      mVarReadFunctionMapPtr = new VarReadFunctionMap;
   }
   (*mVarReadFunctionMapPtr)[aName] = aFunctionPtr;
}

UtScriptContext::VariableReadFunction* UtScriptContext::FindReadOnlyVariableFunction(UtStringId aName) const
{
   if (mVarReadFunctionMapPtr != nullptr)
   {
      auto i = mVarReadFunctionMapPtr->find(aName);
      if (i != mVarReadFunctionMapPtr->end())
      {
         return i->second;
      }
   }
   if (mParentInstancePtr != nullptr)
   {
      return mParentInstancePtr->FindReadOnlyVariableFunction(aName);
   }
   return nullptr;
}
