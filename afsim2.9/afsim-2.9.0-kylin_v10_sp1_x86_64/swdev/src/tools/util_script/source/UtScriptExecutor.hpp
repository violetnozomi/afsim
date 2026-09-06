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

#ifndef UTSCRIPTEXECUTOR_HPP
#define UTSCRIPTEXECUTOR_HPP

#include "util_script_export.h"

#include <iosfwd>
#include <vector>

#include "UtCompilerAttributes.hpp"
#include "UtSpan.hpp"
#include "UtStringId.hpp"
namespace ut
{
namespace log
{
class MessageStream;
}
namespace script
{
class Data;
using DataList   = std::vector<Data>;
using MethodArgs = ut::span<const Data>;
} // namespace script
} // namespace ut
using UtScriptData       = ut::script::Data;
using UtScriptDataList   = ut::script::DataList;
using UtScriptMethodArgs = ut::script::MethodArgs;
class UtScript;
class UtScriptContext;
class UtScriptClass;
class UtScriptDebugger;
struct UtScriptDebuggerControlState;
class UtScriptEnvironment;
struct UtScriptExecutorProgramCounter;
class UtScriptTypes;

//! Executes scripts.
//! Only a single UtScriptExecutor is usually required, but more than one could conceivably be used
//! to implement coroutines or multiple script threads.
class UTIL_SCRIPT_EXPORT UtScriptExecutor
{
public:
   UtScriptExecutor(UtScriptEnvironment* aEnvironmentPtr);
   ~UtScriptExecutor();

   using ExecuteFn = void (UtScriptExecutor::*)(UtScript*, UtScriptData&, size_t, UtScriptContext&);

   //--------------------------------------------------------------
   //! Executes the given script.
   //!
   //! @param aScriptPtr:    A pointer to the script to execute.
   //! @param aScriptRetVal: Filled by reference with the script's return
   //!                       value.
   //! @param aScriptArgs:   A list of arguments to pass to the script.
   //! @param aInstance:     The script instance which owns the script
   //--------------------------------------------------------------
   void Execute(UtScript*               aScriptPtr,
                UtScriptData&           aScriptRetVal,
                const UtScriptDataList& aScriptArgs,
                UtScriptContext&        aInstance);

   //! This should only be called internally by ut_script, as it assumes the invariants
   //! on the stack-pointer for the script calling convention have already been set up.
   void ExecuteInternal(UtScript* aScriptPtr, UtScriptData& aScriptRetVal, size_t aNumScriptArgs, UtScriptContext& aInstance)
   {
      // Dispatch either Execute_Debug() or Execute_NoDebug()
      (this->*mExecuteFunc)(aScriptPtr, aScriptRetVal, aNumScriptArgs, aInstance);
   }

   void Attach(UtScriptDebugger* aDebuggerPtr);

   void DetatchDebugger();

   UtScriptDebugger* GetAttachedDebugger() { return mDebuggerPtr; }

   std::vector<UtScriptExecutorProgramCounter*> GetCallstack() { return mCallStack; }
   size_t                                       GetCallstackSize() { return mCallStackSize; }

   UtScriptExecutorProgramCounter* GetCallstackTop()
   {
      return (mCallStackSize > 0) ? mCallStack[mCallStackSize - 1] : nullptr;
   }
   UtScriptContext* GetTopContext();

   std::vector<UtScriptData>& GetStack() { return mStack; }

   static UtScriptExecutor* GetMainExecutor();
   static UtScriptExecutor* GetCurrentExecutor();

   UtScriptEnvironment* GetScriptEnvironment() const { return mEnvironmentPtr; }
   UtScriptTypes*       GetTypes() { return mTypesPtr; }

   //! @name exception handling and call stack printing.
   //@{
   void        PrintCallStack(ut::log::MessageStream& aStream, bool aOutputFullStack);
   static void PrintAllCallStacks(ut::log::MessageStream& aStream, bool aOutputFullStack);

   void ScriptException(bool aFullStack, const std::string& aExceptionDescription);

   //! Indicate exceptions are to be ignored.
   void SetIgnoreExceptions() { mIgnoreExceptions = true; }
   //@}

protected:
   void VariableNotFoundError(const std::string& aVariableName);
   void AttributeDoesNotExist(const std::string& aName);

   void DoBreak(int aBreakCode, UtScriptDebuggerControlState& aControlState);

   // UT_FLATTEN_ATTR -- improves performance on g++
   void Execute_NoDebug(UtScript*        aScriptPtr,
                        UtScriptData&    aScriptRetVal,
                        size_t           aNumScriptArgs,
                        UtScriptContext& aInstance) UT_FLATTEN_ATTR;

   void Execute_Debug(UtScript* aScriptPtr, UtScriptData& aScriptRetVal, size_t aNumScriptArgs, UtScriptContext& aInstance);

   void PrintCurrentInstruction(std::ostream&          aOut,
                                const UtScript&        aScript,
                                const UtScriptContext& aContext,
                                const UtScriptData*    aStackPtr,
                                const UtScriptData*    aDataPtr,
                                size_t                 aCodeIndex);

   UtScriptExecutorProgramCounter& PushCall();

   UtScriptTypes*                               mTypesPtr;
   UtScriptEnvironment*                         mEnvironmentPtr;
   size_t                                       mCallStackSize;
   std::vector<UtScriptExecutorProgramCounter*> mCallStack;
   std::vector<UtScriptData>                    mStack; // data stack
   ptrdiff_t                                    mStackSize;
   UtScriptDebugger*                            mDebuggerPtr;
   ExecuteFn                                    mExecuteFunc;
   bool                                         mIgnoreExceptions;

   // Cache some type information
   const UtStringId     iId;
   const UtStringId     dId;
   const UtStringId     bId;
   const UtStringId     sId;
   UtScriptClass* const mNullClassPtr;
   UtScriptClass* const mStringClassPtr;
};

#endif
