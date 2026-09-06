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

// This file is included twice to implement both
// UtScriptExecutor::Execute_NoDebug and UtScriptExecutor::Execute_Debug

#define EXECUTE_METHOD_NAME(DEBUG_MODE) Execute##DEBUG_MODE

#include "UtLog.hpp"

#if !(UT_SCRIPT_EXECUTOR_DEBUG_METHOD)

#define DEBUG_CHECK_BREAK()
#define DEBUG_DECLARATIONS()
#define DEBUG_WRAPUP()
#define CALL_WRAPUP() sCurrentExecutor = this;
#define RECURSE_EXECUTE Execute_NoDebug
#define SCRIPT_ASSERT(assertion, text) assert(assertion)

void UtScriptExecutor::Execute_NoDebug(
#else

#undef DEBUG_CHECK_BREAK
#define DEBUG_CHECK_BREAK()                                                                                                 \
   if (mDebuggerPtr != 0)                                                                                                   \
   {                                                                                                                        \
      if (--debugCS.mUpdateCounter == 0)                                                                                    \
      {                                                                                                                     \
         debugCS.mUpdateCounter = UtScriptDebuggerControlState::cDEBUG_UPDATE_INTERVAL;                                     \
         mDebuggerPtr->Update(this, debugCS);                                                                               \
      }                                                                                                                     \
      if (debugCS.mHasBreakpoints)                                                                                          \
      {                                                                                                                     \
         if (mDebuggerPtr->MaybeBreak(aScriptPtr, pc.mCodeIndex) && mDebuggerPtr->IsBreak(this, aScriptPtr, &pc, aContext)) \
         {                                                                                                                  \
            DoBreak(1, debugCS);                                                                                            \
         }                                                                                                                  \
      }                                                                                                                     \
      if (debugCS.mDoStepOver || debugCS.mDoStepInto)                                                                       \
      {                                                                                                                     \
         if (debugCS.mCurrentLine != aScriptPtr->GetLineFromInstructionIndex(CODE_INDEX()))                                 \
         {                                                                                                                  \
            DoBreak(UtScriptDebugger::cSTEP_BREAK, debugCS);                                                                \
         }                                                                                                                  \
      }                                                                                                                     \
   }

#undef DEBUG_WRAPUP
#define DEBUG_WRAPUP() /* empty for now */

#undef DEBUG_DECLARATIONS
#define DEBUG_DECLARATIONS()                     \
   UtScriptDebuggerControlState debugCS;         \
   if (mDebuggerPtr != 0)                        \
   {                                             \
      debugCS = mDebuggerPtr->BeginScript(this); \
   }
#undef CALL_WRAPUP
#define CALL_WRAPUP()                                 \
   sCurrentExecutor = this;                           \
   if (mDebuggerPtr != 0)                             \
   {                                                  \
      mDebuggerPtr->CallComplete(this, &pc, debugCS); \
   }

#undef RECURSE_EXECUTE
#define RECURSE_EXECUTE Execute_Debug

#undef SCRIPT_ASSERT
#define SCRIPT_ASSERT(assertion, text)                               \
   if (!(assertion))                                                 \
   {                                                                 \
      ScriptException(false, "ERROR: internal script error: " text); \
   }                                                                 \
   assert(assertion)

void UtScriptExecutor::Execute_Debug(
#endif
   UtScript*        aScriptPtr,
   UtScriptData&    aScriptRetVal,
   size_t           aNumScriptArgs,
   UtScriptContext& aContext)
{
   // Note: Execute() is configured to allow being called recursively.

   SCRIPT_ASSERT(aScriptPtr != nullptr, "script is null");
   sCurrentExecutor              = this;
   const UtScript& code          = *aScriptPtr;
   ptrdiff_t       prevStackSize = mStackSize;

   UtScriptExecutorProgramCounter& pc = PushCall();
   pc.mScriptPtr                      = aScriptPtr;
   pc.mNumArgs                        = aNumScriptArgs;
   pc.mInstancePtr                    = &aContext;
   const script_code_t* codePtr       = &code.mOps[0];

   pc.mFrameIndex -= aNumScriptArgs;
   SCRIPT_ASSERT(mStack.size() > pc.mFrameIndex + aNumScriptArgs + aScriptPtr->mStackSize + 1,
                 "not enough stack space. Commonly caused by deep/infinite recursion");

   // Push the incoming script arguments onto the stack.
   SCRIPT_ASSERT(aNumScriptArgs == aScriptPtr->mPrototype->mArgs.size(), "script called with wrong number of args");

   mStackSize             = pc.mFrameIndex + aScriptPtr->mStackSize;
   UtScriptData*       sp = &mStack[0] + pc.mFrameIndex; // Stack pointer
   const UtScriptData* dp = &code.mData[0];              // Data pointer

   script_code_t codeIndex = ut::script::npos;

   DEBUG_DECLARATIONS();

   auto SetupScriptMethodArgs = [&](script_code_t aArgCount)
   {
      SCRIPT_ASSERT(mStack.size() >= static_cast<size_t>(mStackSize + aArgCount),
                    "not enough stack space to push script arguements");

      UtScriptData* params = &mStack[static_cast<size_t>(mStackSize)];
      for (script_code_t i = 0; i < aArgCount; ++i)
      {
         script_code_t loadIndex = NEXT_OP();
         if (loadIndex < cMAX_STACK_INDEX)
         {
            params[i].ManagedElsewhereCopy(sp[loadIndex]);
         }
         else
         {
            params[i].LocalStackCopy(dp[loadIndex - cMAX_STACK_INDEX]);
         }
      }
      mStackSize += aArgCount;
      return UtScriptMethodArgs{params, aArgCount};
   };

   auto SetupAppMethodArgs = [&](script_code_t aArgCount)
   {
      SCRIPT_ASSERT(mStack.size() >= static_cast<size_t>(mStackSize + aArgCount),
                    "not enough stack space to push method arguements");

      UtScriptData* params = &mStack[static_cast<size_t>(mStackSize)];
      for (script_code_t i = 0; i < aArgCount; ++i)
      {
         script_code_t loadIndex = NEXT_OP();
         // all items can be marked managed elsewhere when calling a application method.
         params[i].ManagedElsewhereCopy(LOAD(loadIndex));
      }
      mStackSize += aArgCount;
      return UtScriptMethodArgs{params, aArgCount};
   };

   auto PopArgsFromStack = [this](script_code_t aNumArgs)
   {
      for (script_code_t i = 0; i < aNumArgs; ++i)
      {
         mStack[--mStackSize].Clear();
      }
   };

   for (;;)
   {
      UtScript::Opcode opcode = static_cast<UtScript::Opcode>(NEXT_OP());
      // Uncomment the next line to enable instruction execution tracing for debugging.
      // PrintCurrentInstruction(cout, code, aContext, sp, dp, codeIndex);
      SYNC_PC();
      DEBUG_CHECK_BREAK();
      switch (opcode)
      {
      case UtScript::Opcode::CAST:
      {
         OP3(s, castTypeIndex, from);
         UtScriptClass* castTypePtr  = aScriptPtr->GetScriptClass(castTypeIndex);
         sp[s]                       = sp[from];
         ut::script::Data::Type type = sp[s].GetType();
         if (type == ut::script::Data::Type::cPOINTER)
         {
            sp[s].CastToType(castTypePtr);
         }
         else
         {
            UtStringId castId = castTypePtr->GetClassName();
            if (castId == iId)
            {
               sp[s].CastToInt();
            }
            else if (castId == dId)
            {
               sp[s].CastToDouble();
            }
            else if (castId == bId)
            {
               sp[s].CastToBool();
            }
            else if (castId == sId)
            {
               sp[s].CastToString();
            }
         }
      }
      break;
      case UtScript::Opcode::LOAD_LOCAL_VAR:
      {
         OP2(s, di);
         sp[s].LocalStackCopy(dp[di]);
      }
      break;
      case UtScript::Opcode::LOAD_STATIC_VAR:
      {
         OP2(s, i);
         sp[s].LocalStackCopy(*aScriptPtr->GetStaticVariable(i));
      }
      break;
      case UtScript::Opcode::LOAD_GLOBAL_VAR:
      {
         OP2(s, nameId);
         UtStringId    varName = OpToStr(nameId);
         UtScriptData* varPtr  = aContext.FindGlobalVar(varName);
         if (varPtr != nullptr)
         {
            sp[s].LocalStackCopy(*varPtr);
         }
         else
         {
            VariableNotFoundError(varName);
         }
      }
      break;
      case UtScript::Opcode::LOAD_STRUCT_VAR:
      {
         OP3(s, l, nameId);
         bool                ok   = false;
         const UtScriptData& base = LOAD(l);
         sp[s].Clear();
         UtStringId structVarName = OpToStr(nameId);
         SCRIPT_ASSERT(base.GetType() == ut::script::Data::Type::cPOINTER,
                       "can't load struct var from a base that is not a struct (or is null)");
         UtScriptRef*   ptr      = base.GetPointer();
         UtScriptClass* classPtr = ptr->GetScriptClass();
         UtScriptData*  attrPtr  = classPtr->GetAttribute(ptr->GetAppObject(), structVarName);
         if (attrPtr != nullptr)
         {
            sp[s].LocalStackCopy(*attrPtr);
            ok = true;
         }
         if (!ok)
         {
            AttributeDoesNotExist(structVarName);
         }
      }
      break;
      case UtScript::Opcode::LOAD_READ_ONLY_FUNCTION_VAR:
      {
         OP2(s, nameId);
         UtStringId                             varName = OpToStr(nameId);
         UtScriptContext::VariableReadFunction* fnPtr   = aContext.FindReadOnlyVariableFunction(varName);
         if (fnPtr != nullptr)
         {
            fnPtr->Read(sp[s]);
         }
         else
         {
            VariableNotFoundError(varName);
         }
      }
      break;
      case UtScript::Opcode::STORE_LOCAL_VAR_SP:
      {
         OP2(s, l);
         sp[s].LocalStackCopy(LOAD_SP(l));
      }
      break;
      case UtScript::Opcode::STORE_STATIC_VAR:
      {
         OP2(s, i);
         *aScriptPtr->GetStaticVariable(s) = LOAD(i);
      }
      break;
      case UtScript::Opcode::STORE_GLOBAL_VAR:
      {
         OP2(nameId, s);
         *(aContext.FindGlobalVar(OpToStr(nameId))) = LOAD(s);
      }
      break;
      case UtScript::Opcode::STORE_RVALUE_GLOBAL_VAR:
      {
         OP2(nameId, s);
         aContext.FindGlobalVar(OpToStr(nameId))->ReturningCopy(LOAD_SP(s));
      }
      break;
      case UtScript::Opcode::STORE_STRUCT_VAR:
      {
         OP3(baseIndex, nameId, rhs);
         const UtScriptData& base = LOAD(baseIndex);
         SCRIPT_ASSERT(base.GetType() == ut::script::Data::Type::cPOINTER, "can't store var into null or non/struct type");

         UtScriptRef* ptr = base.GetPointer();
         SCRIPT_ASSERT(ptr != nullptr, "struct to store var into was null");

         UtScriptClass* classPtr = ptr->GetScriptClass();
         bool           ok       = classPtr->SetAttribute(ptr->GetAppObject(), OpToStr(nameId), LOAD(rhs));
         if (!ok)
         {
            ScriptException(false, "Failed to assign attribute");
         }
      }
      break;
      case UtScript::Opcode::CREATE_VAR:
      {
         OP2(s, classIndex);
         // Called when declaration of the form '<class> name = <class>();'.
         // OP_INIT_VAR called for '<class> name;'.
         UtScriptClass* classPtr = aScriptPtr->GetScriptClass(classIndex);
         UtScriptRef*   refPtr   = UtScriptTypes::Create(classPtr, aContext);
         if (refPtr != nullptr)
         {
            sp[s].SetPointer(refPtr);
            // If this an instance of a pseudo-class (script_struct) then initialize the script variables.
            if (classPtr->IsPseudoClass())
            {
               auto* contextPtr = refPtr->GetAppObject<UtScriptContext>();
               contextPtr->SetParent(aContext.GetRoot());
               contextPtr->ExecuteInitScripts(this);
            }
         }
         else
         {
            ScriptException(false, "Cannot create a variable with type");
            EXIT_SCRIPT();
         }
      }
      break;
      case UtScript::Opcode::CREATE_NULL_VAR:
      {
         OP2(s, classIndex);
         UtScriptClass* classPtr = aScriptPtr->GetScriptClass(classIndex);
         UtScriptRef*   refPtr   = UtScriptTypes::Create(classPtr, aContext, true);
         SCRIPT_ASSERT(refPtr != nullptr, "failed to create null variable");
         sp[s].SetPointer(refPtr);
      }
      break;
      case UtScript::Opcode::CLONE_VAR:
      {
         OP2(s, l);
         UtScriptRef* refPtr = LOAD(l).GetPointer();
         if ((refPtr != nullptr) && refPtr->IsValid())
         {
            sp[s].SetPointer(refPtr->Clone());
         }
         else
         {
            ScriptException(false, "Attempting to copy a null object");
         }
      }
      break;
      case UtScript::Opcode::INIT_VAR:
      {
         OP2(s, classIndex);
         // Called when declaration of the form '<class> name;' (no initializater).
         // OP_CREATE_VAR called for '<class> name = <class>();'
         UtScriptClass* classPtr = aScriptPtr->GetScriptClass(classIndex);
         sp[s]                   = UtScriptData(classPtr, nullptr);
      }
      break;
      case UtScript::Opcode::ADD:
      {
         OP3(s, a, b);
         sp[s].LocalStackCopy(LOAD(a)) += LOAD(b);
      }
      break;
      case UtScript::Opcode::SUBTRACT:
      {
         OP3(s, a, b);
         sp[s].LocalStackCopy(LOAD(a)) -= LOAD(b);
      }
      break;
      case UtScript::Opcode::MULTIPLY:
      {
         OP3(s, a, b);
         sp[s].LocalStackCopy(LOAD(a)) *= LOAD(b);
      }
      break;
      case UtScript::Opcode::DIVIDE:
      {
         OP3(s, a, b);
         sp[s].LocalStackCopy(LOAD(a)) /= LOAD(b);
      }
      break;
      case UtScript::Opcode::NOT:
      {
         OP2(s, l);
         sp[s] = !(LOAD(l).IsTrue());
      }
      break;
      case UtScript::Opcode::NEGATE:
      {
         OP2(s, l);
         sp[s].LocalStackCopy(sp[l]);
         -sp[s];
      }
      break;
      case UtScript::Opcode::IS_TRUE:
      {
         OP2(s, l);
         sp[s] = sp[l].IsTrue();
      }
      break;
      case UtScript::Opcode::EQUAL:
      {
         OP3(s, a, b);
         sp[s] = LOAD(a) == LOAD(b);
      }
      break;
      case UtScript::Opcode::NOT_EQUAL:
      {
         OP3(s, a, b);
         sp[s] = LOAD(a) != LOAD(b);
      }
      break;
      case UtScript::Opcode::LESS:
      {
         OP3(s, a, b);
         sp[s] = LOAD(a) < LOAD(b);
      }
      break;
      case UtScript::Opcode::LESS_EQUAL:
      {
         OP3(s, a, b);
         sp[s] = LOAD(a) <= LOAD(b);
      }
      break;
      case UtScript::Opcode::JUMP:
      {
         OP1(offset);
         CODE_INDEX() = offset - 1;
      }
      break;
      case UtScript::Opcode::JUMP_IF_TRUE:
      {
         OP2(offset, l);
         bool isTrue = sp[l].IsTrue();
         if (isTrue)
         {
            CODE_INDEX() = offset - 1;
         }
      }
      break;
      case UtScript::Opcode::JUMP_IF_FALSE:
      {
         OP2(offset, l);
         bool isTrue = sp[l].IsTrue();
         if (!isTrue)
         {
            CODE_INDEX() = offset - 1;
         }
      }
      break;
      case UtScript::Opcode::JUMP_IF_NOT_FIRST:
      {
         OP2(offset, isfirst);
         // If we haven't executed this instruction before, keep executing instructions in order.
         if (isfirst)
         {
            const_cast<script_code_t&>(code.mOps[CODE_INDEX()]) = 0;
         }
         // Otherwise jump to the specified instruction.
         else
         {
            if (offset != ut::script::npos)
            {
               CODE_INDEX() = offset - 1;
            }
         }
      }
      break;
      case UtScript::Opcode::RETURN_FUNCTION_SP:
      {
         OP1(loadIndex);
         SCRIPT_ASSERT(loadIndex != ut::script::npos, "attempting to return from illegal stack location");
         aScriptRetVal.ReturningCopy(sp[loadIndex]);
         EXIT_SCRIPT();
      }
      break;
      case UtScript::Opcode::RETURN_FUNCTION_DP:
      {
         OP1(loadIndex);
         SCRIPT_ASSERT(loadIndex != ut::script::npos, "attempting to return from illegal location");
         aScriptRetVal = dp[loadIndex];
         EXIT_SCRIPT();
      }
      break;
      case UtScript::Opcode::CALL_STATIC_APP_FUNCTION:
      {
         OP3(classTypeIndex, methodIndex, argCount);
         auto           args     = SetupAppMethodArgs(argCount);
         script_code_t  s        = NEXT_OP(); // save return here
         UtScriptClass* classPtr = aScriptPtr->GetScriptClass(classTypeIndex);

#if UT_SCRIPT_EXECUTOR_DEBUG_METHOD || !defined(NDEBUG)
         if (argCount == 0 && s != ut::script::npos)
         {
            // clear return location to gaurentee we can catch if the methods fails to populate the return value.
            // it's a developer mistake to make a returning-method without setting the return value, so this
            // is only done in debug mode.
            sp[s].Clear();
         }
#endif
         bool hasRet = false;
         SCRIPT_ASSERT(classPtr != nullptr, "Invalid reference, can't call static application method.");
         UtScriptRef nullref(nullptr, classPtr);
         classPtr->Call(this, aContext, nullref, methodIndex, args, hasRet, sp[s]);
         SCRIPT_ASSERT(s != ut::script::npos || (s == ut::script::npos && !hasRet),
                       "static app function returned something when it claimed it wouldn't");
         if (hasRet)
         {
            sp[s].PromoteManagedElsewhere();
            if (sp[s].GetType() == ut::script::Data::Type::cUNDEFINED)
            {
               ScriptException(false, "A method just returned an undefined value");
            }
         }
         PopArgsFromStack(argCount);
         CALL_WRAPUP()
      }
      break;
      case UtScript::Opcode::CALL_APP_FUNCTION:
      {
         OP3(refIndex, methodIndex, argCount);
         auto                args = SetupAppMethodArgs(argCount);
         script_code_t       s    = NEXT_OP(); // save return here
         const UtScriptData& ref  = LOAD(refIndex);

#if UT_SCRIPT_EXECUTOR_DEBUG_METHOD || !defined(NDEBUG)
         if (s != refIndex && argCount == 0 && s != ut::script::npos)
         {
            // clear return location to gaurentee we can catch if the methods fails to populate the return value.
            // it's a developer mistake to make a returning-method without setting the return value, so this
            // is only done in debug mode.
            sp[s].Clear();
         }
#endif
         bool hasRet = false;
         if (ref.GetType() == ut::script::Data::Type::cPOINTER)
         {
            ref.GetPointer()->GetScriptClass()->Call(this, aContext, *ref.GetPointer(), methodIndex, args, hasRet, sp[s]);
         }
         else if (ref.GetType() == ut::script::Data::Type::cSTRING)
         {
            // const_cast is safe because scripting treats string types as immutable.
            std::string& tempString = const_cast<std::string&>(ref.GetString());
            UtScriptRef  reference(&tempString, mStringClassPtr, UtScriptRef::cDONT_MANAGE);
            mStringClassPtr->Call(this, aContext, reference, methodIndex, args, hasRet, sp[s]);
         }
         else
         {
            ScriptException(false, "Invalid reference, can't call application method.");
            continue;
         }
         SCRIPT_ASSERT(s != ut::script::npos || (s == ut::script::npos && !hasRet),
                       "app function returned something when it claimed it wouldn't");

         if (hasRet)
         {
            sp[s].PromoteManagedElsewhere();
            if (sp[s].GetType() == ut::script::Data::Type::cUNDEFINED)
            {
               if (ref.GetType() == ut::script::Data::Type::cPOINTER)
               {
                  UtScriptRef* refPtr = ref.GetPointer();
                  if (refPtr != nullptr && refPtr->GetScriptClass() != nullptr)
                  {
                     UtScriptClass*                  classPtr  = refPtr->GetScriptClass();
                     UtScriptClass::InterfaceMethod* methodPtr = classPtr->GetMethodEntry(methodIndex);
                     auto                            out       = ut::log::warning() << "Returned an undefined value.";
                     out.AddNote() << "Class: " << classPtr->GetClassName();
                     out.AddNote() << "Method: " << methodPtr->GetName();
                     out.AddNote() << "Reference: " << ref;
                  }
                  else
                  {
                     ut::log::warning() << "A method just returned an undefined value.";
                  }
               }
               else
               {
                  ut::log::warning() << "A method just returned an undefined value.";
               }
            }
         }
         PopArgsFromStack(argCount);
         CALL_WRAPUP()
      }
      break;
      case UtScript::Opcode::CALL_SCRIPT:
      {
         OP2(scriptNameId, argCount);
         SetupScriptMethodArgs(argCount);
         script_code_t s = NEXT_OP(); // save return here

         UtScriptContext* contextPtr = nullptr;
         UtScript*        scriptPtr  = aContext.GetScript(OpToStr(scriptNameId), contextPtr);
         SCRIPT_ASSERT(scriptPtr != nullptr, "could not execute script '" + OpToStr(scriptNameId).GetString() + "'");
         // Execute the script
         SCRIPT_ASSERT(contextPtr->GetScope()->FindScript(scriptPtr->GetName()) != nullptr,
                       "the script is not part of the context scope");

         RECURSE_EXECUTE(scriptPtr, sp[s], argCount, *contextPtr);

         // script compiler will throw an error if a method fails to return a value, so this should never happen.
         SCRIPT_ASSERT(scriptPtr->mPrototype->mReturnClassPtr->IsVoid() ||
                          sp[s].GetType() != ut::script::Data::Type::cUNDEFINED,
                       "Script '" + scriptPtr->GetName() + "' just returned an undefined value");

         PopArgsFromStack(argCount);
         CALL_WRAPUP()
      }
      break;
      case UtScript::Opcode::CALL_STRUCT_SCRIPT:
      {
         OP3(baseIndex, scriptNameId, argCount);
         SetupScriptMethodArgs(argCount);
         const UtScriptData& base = sp[baseIndex];

         script_code_t s = NEXT_OP(); // save return here
         SCRIPT_ASSERT(base.GetType() == ut::script::Data::Type::cPOINTER,
                       "attempting to call struct script on invalid struct");
         UtScriptRef*     refPtr         = base.GetPointer();
         auto*            contextPtr     = refPtr->GetAppObject<UtScriptContext>();
         UtScriptContext* execContextPtr = nullptr;
         UtScript*        scriptPtr      = contextPtr->GetScript(OpToStr(scriptNameId), execContextPtr);
         SCRIPT_ASSERT(scriptPtr != nullptr, "could not execute struct script '" + OpToStr(scriptNameId).GetString() + "'");
         SCRIPT_ASSERT(execContextPtr->GetScope()->FindScript(scriptPtr->GetName()) != nullptr,
                       "the struct script is not part of the context");
         RECURSE_EXECUTE(scriptPtr, sp[s], argCount, *execContextPtr);

         // script compiler will throw an error if a method fails to return a value, so this should never happen.
         SCRIPT_ASSERT(scriptPtr->mPrototype->mReturnClassPtr->IsVoid() ||
                          sp[s].GetType() != ut::script::Data::Type::cUNDEFINED,
                       "Struct Script '" + scriptPtr->GetName() + "' just returned an undefined value");

         PopArgsFromStack(argCount);
         CALL_WRAPUP()
      }
      break;
      case UtScript::Opcode::CALL_DYN_STRUCT_SCRIPT:
      {
         OP3(base, scriptNameId, argCount);
         auto          args    = SetupScriptMethodArgs(argCount);
         script_code_t s       = NEXT_OP(); // save return here
         UtScriptData& baseVal = sp[base];
         if (s != base && argCount == 0)
         {
            sp[s].Clear();
         }
         SCRIPT_ASSERT(baseVal.GetType() == ut::script::Data::Type::cPOINTER,
                       "attempting to call dynamic struct script on invalid struct");
         UtScriptRef*   ptr      = baseVal.GetPointer();
         UtScriptClass* classPtr = ptr->GetScriptClass();
         if (!classPtr->CallDynamicInternal(ptr->GetAppObject(), this, OpToStr(scriptNameId), args, sp[s]))
         {
            std::stringstream ss;
            ss << "WARNING: Could not call script " << OpToStr(scriptNameId);
            ScriptException(false, ss.str());
         }
         PopArgsFromStack(argCount);
         CALL_WRAPUP()
      }
      break;
      case UtScript::Opcode::SET_DEBUG_FLAG:
      {
         UtScriptContext* contextPtr = &aContext;
         bool             debugFlag  = false;
         while (contextPtr != nullptr)
         {
            if (0 != (contextPtr->mFlags & UtScriptContext::cDEBUG_WRITE_OFF_FLAG))
            {
               break;
            }
            if (0 != (contextPtr->mFlags & UtScriptContext::cDEBUG_WRITE_FLAG))
            {
               debugFlag = true;
               break;
            }
            contextPtr = contextPtr->GetParent();
         }

         if (!debugFlag)
         {
            // skip over the CALL_STATIC_APP_FUNCTION
            opcode = static_cast<UtScript::Opcode>(NEXT_OP());
            SYNC_PC();
            DEBUG_CHECK_BREAK();
            SCRIPT_ASSERT(opcode == UtScript::Opcode::CALL_STATIC_APP_FUNCTION,
                          "expected SET_DEBUG_FLAG to only be used before static app functions");
            CODE_INDEX() += 2; // skip classTypeIndex and methodIndex
            OP1(argCount);
            CODE_INDEX() += argCount;
            /*[[maybe_unused]]*/ OP1(store);
            (void)store; // prevent unused variable warning.
            SCRIPT_ASSERT(store == ut::script::npos, "expected all functions using SET_DEBUG_FLAG to return void");
         }
      }
      break;
      case UtScript::Opcode::END:
         EXIT_SCRIPT();
         break;
      default:
      { // RAII block
         auto out = ut::log::error() << "Invalid opcode.";
         out.AddNote() << "Opcode: " << static_cast<script_code_t>(opcode);
      }
         SCRIPT_ASSERT(false, "invalid opcode encountered");
         assert(false);
         EXIT_SCRIPT();
      } // end of switch
   }    // end of for (;;)

exit_script:

   // Clear stack to remove references to pointers
   for (ptrdiff_t i = prevStackSize; i < mStackSize; ++i)
   {
      mStack[i].Clear();
   }
   mStackSize = prevStackSize;
   --mCallStackSize;
   sCurrentExecutor = nullptr;
   SCRIPT_ASSERT(aScriptPtr->mPrototype->mReturnClassPtr->IsVoid() ||
                    aScriptRetVal.GetType() != ut::script::Data::Type::cUNDEFINED,
                 "Script signature claims it will return something, but it didn't");
   DEBUG_WRAPUP();
}
