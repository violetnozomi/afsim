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

#ifndef UTSCRIPTDEBUGGER_HPP
#define UTSCRIPTDEBUGGER_HPP

#include "util_script_export.h"

#include <map>
#include <string>

#include "UtCallback.hpp"
class UtScript;
class UtScriptClass;
class UtScriptContext;
class UtScriptEnvironment;
class UtScriptExecutor;
#include "UtScriptExecutorProgramCounter.hpp"
class UtScriptScope;
class UtScriptTypes;

struct UTIL_SCRIPT_EXPORT UtScriptDebuggerControlState
{
   // Allow the debugger to update after the script has executed 5000 instructions
   static const int cDEBUG_UPDATE_INTERVAL = 5000;
   UtScriptDebuggerControlState()
      : mUpdateCounter(cDEBUG_UPDATE_INTERVAL)
      , mHasBreakpoints(false)
      , mDoStepOver(false)
      , mDoStepInto(false)
      , mDoStepOut(false)
   {
   }

   int  mUpdateCounter;
   bool mHasBreakpoints;
   bool mDoStepOver;
   bool mDoStepInto;
   bool mDoStepOut;
   // for DoStepOver or DoStepInto:
   size_t    mCurrentLine;
   ptrdiff_t mStepFromFrameIndex;
};

class UTIL_SCRIPT_EXPORT UtScriptDebuggerBreakpoint
{
public:
   UtScriptDebuggerBreakpoint()
      : mScriptPtr(nullptr)
      , mLineNumber(0)
      , mInstructionIndex(0)
   {
   }
   bool IsValid() const { return mScriptPtr != nullptr; }

   //! Location Details
   //@{
   //! The script containing the breakpoint
   UtScript* mScriptPtr;
   //! Line number of the script on which to break
   size_t mLineNumber;
   //! Instruction index on which to break
   size_t mInstructionIndex;
   //@}

   //! User-defined breakpoint identifier
   int mId;
   //! Only break when mCondition evaluates to 'true'.  The empty condition is always true
   std::string mCondition;
};

class UTIL_SCRIPT_EXPORT UtScriptDebugger
{
public:
   UtScriptDebugger(UtScriptEnvironment* aEnvironmentPtr);
   ~UtScriptDebugger();

   enum BreakType
   {
      cBREAKPOINT_BREAK,
      cSTEP_BREAK,
      cEXCEPTION_BREAK
   };

   //! @name Methods to be called by code external to UtScript
   //@{
   UtScriptDebuggerBreakpoint MakeBreakpointFromLine(const std::string& aFilePath, size_t aLineNumber);
   void                       AddBreakpoint(UtScriptDebuggerBreakpoint aBreakpoint);
   void                       RemoveBreakpoint(int aBreakpointId);
   bool                       IsBreaking() const { return mAtBreak; }
   //@}

   //! Called repeatedly while in break-mode.
   UtCallbackListN<void()> UpdateWhileAtBreak;
   //! Called when the script executor breaks (for breakpoint or a step command)
   UtCallbackListN<void(BreakType)> OnBreak;
   //! Called when the script executor returns from a break
   UtCallbackListN<void()> OnContinue;
   //! Called when the script has been executing for a while
   //! this is used to allow processing during infinite loops
   UtCallbackListN<void()> OnUpdate;

   //! @name Methods to be called while in Break()
   //@{
   void Resume() { mResuming = true; }
   void StepInto();
   void StepOver();
   void StepOut();
   //@}

   //! @ Methods to be called by the script executor
   //@{
   bool MaybeBreak(UtScript* aScriptPtr, size_t aCodeIndex)
   {
      // TODO: This is very slow!
      for (size_t i = 0; i < mBreakpoints.size(); ++i)
      {
         const UtScriptDebuggerBreakpoint& brk = mBreakpoints[i];
         if (brk.mInstructionIndex == aCodeIndex && brk.mScriptPtr == aScriptPtr)
         {
            return true;
         }
      }
      return false;
   }
   bool IsBreak(UtScriptExecutor*               aExecutorPtr,
                UtScript*                       aScriptPtr,
                UtScriptExecutorProgramCounter* aProgramCounterPtr,
                UtScriptContext&                aContext);
   void BreakForException(UtScriptExecutor* aExecutorPtr, const std::string& aExceptionDescription);
   void Break(UtScriptExecutor* aExecutorPtr, UtScriptDebuggerControlState& aControlState, BreakType aBreakCode);
   void Update(UtScriptExecutor* aExecutorPtr, UtScriptDebuggerControlState& aControlState);
   UtScriptDebuggerControlState BeginScript(UtScriptExecutor*);

   void CallComplete(UtScriptExecutor*               aExecutorPtr,
                     UtScriptExecutorProgramCounter* aPc,
                     UtScriptDebuggerControlState&   aControlState)
   {
      if ((mDoStepOver | mDoStepInto | mDoStepOut) && aPc->mFrameIndex < mStepOutFrameIndex)
      {
         Break(aExecutorPtr, aControlState, cSTEP_BREAK);
      }
   }
   void Detatching();
   bool IsDetatching() const { return mDetatching; }

   const std::string& GetExceptionDescription() const { return mExceptionDescription; }
   //@}

   struct UTIL_SCRIPT_EXPORT EvaluationFunction
   {
      EvaluationFunction()
      {
         mExpressionScriptPtr = nullptr;
         mScopePtr            = nullptr;
      }
      ~EvaluationFunction();

      struct Arg
      {
         ptrdiff_t      mVariableIndex;
         UtScriptClass* mTypePtr;
      };
      UtScriptScope*   mScopePtr;
      UtScript*        mExpressionScriptPtr;
      UtScript*        mCallerScriptPtr;
      std::vector<Arg> mLocalVariableArguments;
   };

   EvaluationFunction* CompileExpression(UtScriptExecutorProgramCounter* aProgramCounterPtr,
                                         const std::string&              aExpression);

   UtScriptExecutor* GetDebugExecutor() { return mDebugExecutorPtr; }

private:
   bool TestCondition(UtScriptExecutor*                 aExecutorPtr,
                      const UtScriptDebuggerBreakpoint& aBreak,
                      UtScriptExecutorProgramCounter*   aProgramCounterPtr);

   UtScriptEnvironment*                    mEnvironmentPtr;
   UtScriptTypes*                          mTypesPtr;
   std::vector<UtScriptDebuggerBreakpoint> mBreakpoints;

   UtScriptExecutor* mExecutorPtr;
   // This executor is used to execute scripts while the main executor is at a breakpoint.
   UtScriptExecutor* mDebugExecutorPtr;
   bool              mAtBreak;
   bool              mDetatching;
   bool              mResuming;

   bool        mDoStepOver;
   bool        mDoStepInto;
   bool        mDoStepOut;
   ptrdiff_t   mStepOutFrameIndex;
   std::string mExceptionDescription;

   std::map<int, EvaluationFunction*> mCompiledBreakpointConditions;
};

#endif
