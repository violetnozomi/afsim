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

#include "UtScriptDebugger.hpp"

#include <cassert>
#include <sstream>

#include "UtInput.hpp"
#include "UtScript.hpp"
#include "UtScriptCore.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptParser.hpp"
#include "UtScriptRegistry.hpp"
#include "UtScriptStream.hpp"
#include "UtSleep.hpp"

UtScriptDebugger::UtScriptDebugger(UtScriptEnvironment* aEnvironmentPtr)
   : mEnvironmentPtr(aEnvironmentPtr)
   , mDetatching(false)
   , mResuming(false)
{
   mDebugExecutorPtr = new UtScriptExecutor(aEnvironmentPtr);
   mDebugExecutorPtr->SetIgnoreExceptions();
   mExecutorPtr = nullptr;
   mDoStepOver = mDoStepInto = mDoStepOut = false;
   mStepOutFrameIndex                     = -1;
   // Don't abort on asserts; the debugger will break instead
   UtScriptCore::sAbortOnAssert = false;
}
UtScriptDebugger::~UtScriptDebugger()
{
   for (std::map<int, EvaluationFunction*>::iterator i = mCompiledBreakpointConditions.begin();
        i != mCompiledBreakpointConditions.end();
        ++i)
   {
      delete i->second;
   }
   delete mDebugExecutorPtr;
   UtScriptCore::sAbortOnAssert = true;
}

void UtScriptDebugger::BreakForException(UtScriptExecutor* aExecutorPtr, const std::string& aExceptionDescription)
{
   UtScriptDebuggerControlState garbage;
   mExceptionDescription = aExceptionDescription;
   Break(aExecutorPtr, garbage, cEXCEPTION_BREAK);
}

void UtScriptDebugger::Break(UtScriptExecutor* aExecutorPtr, UtScriptDebuggerControlState& aControlState, BreakType aBreakCode)
{
   // A step-into can be accomplished by either breaking in the current script
   // or breaking at the start of another script
   // Make sure we don't break a second time here.
   if (aControlState.mDoStepInto && mDoStepInto == false && aBreakCode == 2)
   {
      aControlState.mDoStepInto = false;
   }

   assert(mExecutorPtr == nullptr);
   mExecutorPtr = aExecutorPtr;

   mAtBreak    = true;
   mResuming   = false;
   mDoStepOver = mDoStepInto = mDoStepOut = false;

   OnBreak(aBreakCode);
   while (!mDetatching && !mResuming)
   {
      UpdateWhileAtBreak();
      UtSleep::Sleep(.01);
   }
   mExceptionDescription.clear();
   mAtBreak = false;
   OnContinue();
   if (!mDetatching)
   {
      Update(aExecutorPtr, aControlState);
      aControlState.mDoStepInto = mDoStepInto;
      aControlState.mDoStepOver = mDoStepOver;
      aControlState.mDoStepOut  = mDoStepOut;
      if (aControlState.mDoStepInto)
      {
         aControlState.mStepFromFrameIndex = aExecutorPtr->GetCallstackTop()->mFrameIndex;
      }
      if (aControlState.mDoStepOver || aControlState.mDoStepInto)
      {
         UtScriptExecutorProgramCounter* pc = aExecutorPtr->GetCallstackTop();
         aControlState.mCurrentLine         = pc->mScriptPtr->GetLineFromInstructionIndex(pc->mCodeIndex);
      }
   }
   mExecutorPtr = nullptr;
}

UtScriptDebuggerBreakpoint UtScriptDebugger::MakeBreakpointFromLine(const std::string& aFilePath, size_t aLineNumber)
{
   UtScriptDebuggerBreakpoint brk;
   brk.mScriptPtr = mEnvironmentPtr->GetRegistry()->FindScriptAtLine(aFilePath, aLineNumber);
   if (brk.mScriptPtr != nullptr)
   {
      do
      {
         brk.mLineNumber       = aLineNumber;
         brk.mInstructionIndex = brk.mScriptPtr->GetInstructionIndexFromLineNumber(brk.mLineNumber);
         if (brk.mInstructionIndex != static_cast<size_t>(-1))
         {
            return brk;
         }
         aLineNumber++;
      } while (aLineNumber < aFilePath.size());
   }
   return UtScriptDebuggerBreakpoint();
}

void UtScriptDebugger::AddBreakpoint(UtScriptDebuggerBreakpoint aBreakpoint)
{
   // Replace or add breakpoint
   for (size_t i = 0; i < mBreakpoints.size(); ++i)
   {
      if (mBreakpoints[i].mId == aBreakpoint.mId)
      {
         delete mCompiledBreakpointConditions[aBreakpoint.mId];
         mCompiledBreakpointConditions.erase(aBreakpoint.mId);
         mBreakpoints[i] = aBreakpoint;
         return;
      }
   }
   mBreakpoints.push_back(aBreakpoint);
}

bool UtScriptDebugger::IsBreak(UtScriptExecutor*               aExecutorPtr,
                               UtScript*                       aScriptPtr,
                               UtScriptExecutorProgramCounter* aProgramCounter,
                               UtScriptContext&                aContext)
{
   for (size_t i = 0; i < mBreakpoints.size(); ++i)
   {
      const UtScriptDebuggerBreakpoint& brk = mBreakpoints[i];
      if (brk.mInstructionIndex == aProgramCounter->mCodeIndex && brk.mScriptPtr == aScriptPtr)
      {
         if (!brk.mCondition.empty())
         {
            return TestCondition(aExecutorPtr, brk, aProgramCounter);
         }
         return true;
      }
   }
   return false;
}

void UtScriptDebugger::Update(UtScriptExecutor* aExecutorPtr, UtScriptDebuggerControlState& aControlState)
{
   OnUpdate();
   aControlState.mHasBreakpoints         = false;
   UtScriptExecutorProgramCounter* pcPtr = aExecutorPtr->GetCallstackTop();
   if (pcPtr != nullptr)
   {
      for (size_t i = 0; i < mBreakpoints.size(); ++i)
      {
         if (mBreakpoints[i].mScriptPtr == pcPtr->mScriptPtr)
         {
            aControlState.mHasBreakpoints = true;
         }
      }
   }
}
UtScriptDebuggerControlState UtScriptDebugger::BeginScript(UtScriptExecutor* aExecutorPtr)
{
   UtScriptDebuggerControlState cs;
   Update(aExecutorPtr, cs);
   if (mDoStepInto)
   {
      Break(aExecutorPtr, cs, cSTEP_BREAK);
   }
   return cs;
}


void UtScriptDebugger::Detatching()
{
   mDetatching = true;
   if (mAtBreak)
   {
      // Can't detach now, debugger will be detatched fully later
   }
}

void UtScriptDebugger::StepInto()
{
   if (!mAtBreak)
      return;
   mDoStepInto = true;
   if (mExecutorPtr->GetCallstackTop())
   {
      mStepOutFrameIndex = mExecutorPtr->GetCallstackTop()->mFrameIndex;
   }
   mResuming = true;
}

void UtScriptDebugger::StepOver()
{
   if (!mAtBreak)
      return;
   mDoStepOver = true;
   if (mExecutorPtr->GetCallstackTop())
   {
      mStepOutFrameIndex = mExecutorPtr->GetCallstackTop()->mFrameIndex;
   }
   mResuming = true;
}

void UtScriptDebugger::StepOut()
{
   if (!mAtBreak)
      return;

   if (mExecutorPtr != nullptr)
   {
      if (mExecutorPtr->GetCallstackTop())
      {
         mDoStepOut         = true;
         mStepOutFrameIndex = mExecutorPtr->GetCallstackTop()->mFrameIndex;
      }
   }
   mResuming = true;
}

void UtScriptDebugger::RemoveBreakpoint(int aBreakpointId)
{
   for (size_t i = 0; i < mBreakpoints.size(); ++i)
   {
      if (mBreakpoints[i].mId == aBreakpointId)
      {
         mBreakpoints.erase(mBreakpoints.begin() + i);
      }
   }
}

UtScriptDebugger::EvaluationFunction* UtScriptDebugger::CompileExpression(UtScriptExecutorProgramCounter* aProgramCounterPtr,
                                                                          const std::string&              aExpression)
{
   EvaluationFunction* evalFnPtr = new EvaluationFunction;
   evalFnPtr->mCallerScriptPtr   = aProgramCounterPtr->mScriptPtr;

   // Create a new script and pass local variables as arguments to it
   // UtScriptDataList argData;
   UtScriptParser::ImplicitArgumentList argList;
   std::set<std::string>                usedNames;
   for (size_t i = 0; i < aProgramCounterPtr->mScriptPtr->mLocalVariables.size(); ++i)
   {
      // only add the variable if it's in scope
      UtScript::LocalVariable& var = aProgramCounterPtr->mScriptPtr->mLocalVariables[i];
      if (var.mValidAfter <= aProgramCounterPtr->mCodeIndex && var.mValidBefore >= aProgramCounterPtr->mCodeIndex)
      {
         if (usedNames.find(var.mName) != usedNames.end())
            continue;

         // Quick and dirty check that the expression uses the variable
         if (aExpression.find(var.mName) != std::string::npos)
         {
            EvaluationFunction::Arg arg = {var.mStackIndex, var.mTypePtr};
            evalFnPtr->mLocalVariableArguments.push_back(arg);
            UtScriptParser::ImplicitArgument implicitArg;
            implicitArg.mName    = var.mName;
            implicitArg.mTypePtr = var.mTypePtr;
            argList.push_back(implicitArg);
         }
      }
   }

   UtScriptContext* tmpContext = new UtScriptContext(aProgramCounterPtr->mInstancePtr->GetScope());
   tmpContext->SetParent(aProgramCounterPtr->mInstancePtr);
   UtScriptParser&   parser    = mEnvironmentPtr->GetParser();
   static int        evalCount = 0;
   std::stringstream nameSS;
   nameSS << "__debug_eval_" << evalCount;
   UtInput scriptInput;

   scriptInput.PushInputString("return " + aExpression + "; <END>");
   UtScriptStream stream(scriptInput, "<END>");
   UtScript*      scriptPtr = parser.Parse(nameSS.str(), "Object", &stream, tmpContext, "debug", false, argList);
   if (scriptPtr != nullptr)
   {
      evalFnPtr->mExpressionScriptPtr = scriptPtr;
      evalFnPtr->mExpressionScriptPtr->AddRef();
      evalFnPtr->mScopePtr = aProgramCounterPtr->mInstancePtr->GetScope();
      evalFnPtr->mScopePtr->AddRef();
   }
   else
   {
      delete evalFnPtr;
      evalFnPtr = nullptr;
   }
   delete tmpContext;
   return evalFnPtr;
}

bool UtScriptDebugger::TestCondition(UtScriptExecutor*                 aExecutorPtr,
                                     const UtScriptDebuggerBreakpoint& aBreak,
                                     UtScriptExecutorProgramCounter*   aProgramCounter)
{
   EvaluationFunction*& evalFnPtr = mCompiledBreakpointConditions[aBreak.mId];
   if (evalFnPtr == nullptr)
   {
      evalFnPtr = CompileExpression(aProgramCounter, aBreak.mCondition);
   }
   if (evalFnPtr == nullptr)
   {
      return false;
   }

   UtScriptDataList argData;
   for (size_t i = 0; i < evalFnPtr->mLocalVariableArguments.size(); ++i)
   {
      UtScriptDebugger::EvaluationFunction::Arg& arg = evalFnPtr->mLocalVariableArguments[i];
      if (UtScript::IsStaticVariableIndex(arg.mVariableIndex))
      {
         argData.push_back(
            aProgramCounter->mScriptPtr->mStaticVariables[UtScript::GetStaticVariableIndex(arg.mVariableIndex)]);
      }
      else
      {
         ptrdiff_t varIndex = arg.mVariableIndex + aProgramCounter->mFrameIndex;
         if (varIndex >= (int)aExecutorPtr->GetStack().size())
            continue;
         argData.push_back(aExecutorPtr->GetStack()[varIndex]);
      }
   }
   UtScriptContext* tmpContext = new UtScriptContext(aProgramCounter->mInstancePtr->GetScope());
   tmpContext->SetParent(aProgramCounter->mInstancePtr);
   UtScriptData result;
   result.SetBool(false);
   GetDebugExecutor()->Execute(evalFnPtr->mExpressionScriptPtr, result, argData, *tmpContext);
   delete tmpContext;
   return result.IsTrue();
}

UtScriptDebugger::EvaluationFunction::~EvaluationFunction()
{
   if (mScopePtr != nullptr)
   {
      mScopePtr->Unref();
   }
   if (mExpressionScriptPtr != nullptr)
   {
      mExpressionScriptPtr->Unref();
   }
}
