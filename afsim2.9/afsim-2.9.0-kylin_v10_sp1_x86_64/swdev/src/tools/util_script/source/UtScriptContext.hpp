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

#ifndef UTSCRIPTCONTEXT_HPP
#define UTSCRIPTCONTEXT_HPP

#include "util_script_export.h"

#include <unordered_map>

#include "UtCloneablePtr.hpp"
namespace ut
{
namespace log
{
class MessageStream;
}
} // namespace ut
#include "UtScript.hpp"
class UtScriptExecutor;
#include "UtScriptRef.hpp"
#include "UtScriptScope.hpp"
class UtScriptStream;

//! A script instance is an object accessible to script which may contain scripts and variables.
//! script instances may have a parent, from which variables and scripts are visible.
//! Each script instance has a UtScriptScope which can be shared by multiple instances.
class UTIL_SCRIPT_EXPORT UtScriptContext
{
public:
   class UTIL_SCRIPT_EXPORT VariableReadFunction
   {
   public:
      virtual void Read(UtScriptData& aData) = 0;
      virtual ~VariableReadFunction()        = default;
   };

   using VarMap             = std::unordered_map<UtStringId, ut::CloneablePtr<UtScriptData>>;
   using VarReadFunctionMap = std::unordered_map<UtStringId, VariableReadFunction*>;

   enum Flags
   {
      //! If this flag is set, scripts executed in this context (or child context) will print debug writes
      cDEBUG_WRITE_FLAG = 1,
      //! If this flag is set, scripts executed in this context (or child context) will not print debug writes
      cDEBUG_WRITE_OFF_FLAG = 2,
      cINITIALIZED_FLAG     = 0x80000000
   };

   UtScriptContext();
   UtScriptContext(UtScriptEnvironment* aEnvironmentPtr);
   UtScriptContext(UtScriptScope* aScopePtr);
   UtScriptContext(const UtScriptContext& aSrc);
   UtScriptContext& operator=(const UtScriptContext& aRhs);
   ~UtScriptContext();

   bool Initialize();
   bool IsInitialized() const { return 0 != (mFlags & cINITIALIZED_FLAG); }

   //! @name Methods associates with variables.
   //@{
   bool          IsVariableDefined(UtStringId aName);
   UtScriptData& Var(UtStringId aName);
   UtScriptData* FindVar(UtStringId aName) const;
   UtScriptData* FindGlobalVar(UtStringId aName) const;

   //! Gets all local variables in the instance
   //! This map should not be modified directly, but the values may.
   VarMap& GetVariables() { return mInstanceVariables; }

   //! Deletes all local variables in the context
   void DeleteVariables();

   VariableReadFunction* FindReadOnlyVariableFunction(UtStringId aName) const;

   void SetReadOnlyVariableFunction(UtStringId aName, VariableReadFunction* aFunctionPtr);

   UtScriptData* RegisterVariable(const std::string& aName, UtScriptClass* aTypePtr);

   UtScriptData* RegisterVariable(const std::string& aName, UtStringId aType);

   void RegisterReadOnlyVariableFunction(const std::string& aName, UtStringId aType);

   void RegisterThisPointer(UtStringId aType);
   bool SetThisPointer(void* aThisPtr);
   //@}

   //! @name Methods associated with scripts
   //@{
   UtScript* FindScript(UtStringId aScriptName);

   UtScript* GetScript(UtStringId aName, UtScriptContext*& aInstance);

   void Execute(UtScriptExecutor*       aExecutorPtr,
                UtScript*               aScriptPtr,
                UtScriptData&           aReturnVal,
                const UtScriptDataList& aArgs = UtScriptDataList());

   bool Execute(UtScriptExecutor*       aExecutorPtr,
                const std::string&      aName,
                UtScriptData&           aReturnVal,
                const UtScriptDataList& aArgs = UtScriptDataList());

   // This should only be called internally by the script engine
   bool ExecuteInternal(UtScriptExecutor* aExecutorPtr, UtStringId aName, UtScriptData& aReturnVal, size_t aNumArgs);

   void ExecuteInitScripts(UtScriptExecutor* aExecutorPtr);

   //! @name Parsing methods
   //@{
   UtScript* Parse(UtScriptStream* aStreamPtr, const std::string& aNamespace = std::string());

   UtScript* Parse(const std::string& aScriptName,
                   UtStringId         aScriptReturnType,
                   UtScriptStream*    aStreamPtr,
                   const std::string& aNamespace       = std::string(),
                   bool               aIsVariableBlock = false);
   //@}

   //! @name Context methods
   void               SetContextName(const std::string& aContextName);
   const std::string& GetContextName() const { return mContextName; }

   void             SetParent(UtScriptContext* aParentPtr);
   UtScriptContext* GetParent() const { return mParentInstancePtr; }

   UtScriptContext* GetRoot();
   //@}

   //! @name Scope methods
   //@{
   const UtScriptScope* GetScope() const { return mScopePtr; }
   UtScriptScope*       GetScope() { return mScopePtr; }
   UtScriptScope*       ModifyScope();
   void                 UpdateScope();
   //@}

   //! @name Miscellaneous methods.
   //@{
   UtScriptTypes*       GetTypes() const;
   UtScriptEnvironment* GetEnvironment() const;

   bool WritesEnabled() const;
   void SetWritesEnabled(bool aWritesEnabled);

   void Print(ut::log::MessageStream& aStream);
   //@}

protected:
   void             CheckScopeParent();
   void             ParentChanging(UtScriptContext* aNewParent);
   UtScriptContext* GetInstanceForScope(UtScriptScope* aScopePtr);

   // A friendly name for the script context.
   // Only named contexts will be maintained in the parent's child list
   std::string    mContextName;
   UtScriptScope* mScopePtr;

private:
   UtScriptContext* mParentInstancePtr;

protected:
   VarMap              mInstanceVariables;
   VarReadFunctionMap* mVarReadFunctionMapPtr;

public:
   //! Miscellaneous flags used by the instance
   int mFlags;
};
#endif
