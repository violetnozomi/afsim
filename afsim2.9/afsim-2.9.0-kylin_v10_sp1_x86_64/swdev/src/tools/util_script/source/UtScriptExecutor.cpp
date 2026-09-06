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

#include "UtScriptExecutor.hpp"

#include <cassert>
#include <deque>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>

#include "UtLog.hpp"
#include "UtScript.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptCore.hpp"
#include "UtScriptData.hpp"
#include "UtScriptDebugger.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptExecutorProgramCounter.hpp"
#include "UtTextDocument.hpp"

using script_code_t = UtScript::script_code_t;
namespace
{
inline UtStringId OpToStr(script_code_t aOp)
{
   return UtStringId::UnsafeFindStringId(static_cast<int>(aOp));
}

//! A global set of UtScriptExecutors
std::set<UtScriptExecutor*>& GetExecutors()
{
   static std::set<UtScriptExecutor*> e;
   return e;
}
UtScriptExecutor*    sCurrentExecutor = nullptr;
std::recursive_mutex executorListMutex;
} // namespace

UtScriptExecutor::UtScriptExecutor(UtScriptEnvironment* aEnvironmentPtr)
   : mTypesPtr(&aEnvironmentPtr->GetTypes())
   , mEnvironmentPtr(aEnvironmentPtr)
   , mCallStackSize(0)
   , mStackSize(0)
   , mDebuggerPtr(nullptr)
   , mIgnoreExceptions(false)
   , iId(mTypesPtr->GetTypeId("int"))
   , dId(mTypesPtr->GetTypeId("double"))
   , bId(mTypesPtr->GetTypeId("bool"))
   , sId(mTypesPtr->GetTypeId("string"))
   , mNullClassPtr(mTypesPtr->GetClass("null"))
   , mStringClassPtr(mTypesPtr->GetClass("string"))
{
   mExecuteFunc = &UtScriptExecutor::Execute_NoDebug;
   // The stack must have a fixed size, the code is not equipped to handle a stack reallocation
   mStack.resize(10000);

   std::lock_guard<std::recursive_mutex> lock(executorListMutex);
   GetExecutors().insert(this);
}

UtScriptExecutor::~UtScriptExecutor()
{
   for (size_t i = 0; i < mCallStack.size(); ++i)
   {
      delete mCallStack[i];
   }
   std::lock_guard<std::recursive_mutex> lock(executorListMutex);
   GetExecutors().erase(this);
}

void UtScriptExecutor::Execute(UtScript*               aScriptPtr,
                               UtScriptData&           aScriptRetVal,
                               const UtScriptDataList& aScriptArgs,
                               UtScriptContext&        aInstance)
{
   // caller is responsible for
   // pushing/poping arguements on/off the stack
   for (auto& aData : aScriptArgs)
   {
      mStack[static_cast<size_t>(mStackSize++)].ManagedElsewhereCopy(aData);
   }

   // Dispatch either Execute_Debug() or Execute_NoDebug()
   (this->*mExecuteFunc)(aScriptPtr, aScriptRetVal, aScriptArgs.size(), aInstance);

   for (size_t i = 0; i < aScriptArgs.size(); ++i)
   {
      mStack[static_cast<size_t>(--mStackSize)].Clear();
   }
}

UtScriptExecutorProgramCounter& UtScriptExecutor::PushCall()
{
   UtScriptExecutorProgramCounter* pc;
   if (mCallStackSize < mCallStack.size())
   {
      pc = mCallStack[mCallStackSize++];
   }
   else
   {
      ++mCallStackSize;
      mCallStack.push_back(new UtScriptExecutorProgramCounter);
      pc = mCallStack.back();
   }
   pc->mFrameIndex = mStackSize;
   pc->mCodeIndex  = -1;
   return *pc;
}

///////////////////////////////////////////////////////////////////////////////
// Ut Script VM
// At the start of a script's execution, a fixed-sized chunk of the script
// stack is allocated for the script's locals and temporaries.
// The script's opcodes are executed until the END instruction is reached.
//
// Some #DEFINE's are used to easily tweak the way variables are accessed for performance
// Short variable names are used.  Description:
//   sp - stack pointer
//   dp - data pointer
//   s - OP's 'store' stack index
//   l - OP's 'load' stack index
///////////////////////////////////////////////////////////////////////////////

// Note: using a local variable for the instruction counter is much faster
#if 1
#define CODE_INDEX() codeIndex
// SYNC_PC() is necessary when using a local code index so that stack traces
//           are accurate.
#define SYNC_PC() pc.mCodeIndex = CODE_INDEX()
#else
#define SYNC_PC() /*nothing*/
#define CODE_INDEX() pc.mCodeIndex
#endif
// Get the next opcode word
#define NEXT_OP() codePtr[++CODE_INDEX()]
// Declare variables and assign them to the next opcode words
#define OP1(OPNAME) script_code_t OPNAME = NEXT_OP()
#define OP2(OPNAME1, OPNAME2) \
   OP1(OPNAME1);              \
   OP1(OPNAME2);
#define OP3(OPNAME1, OPNAME2, OPNAME3) \
   OP2(OPNAME1, OPNAME2);              \
   OP1(OPNAME3);

// For some read-only parameters, we allow directly addressing data by supplying an index greater than cMAX_STACK
constexpr script_code_t cMAX_STACK_INDEX = 100000;
#define LOAD(IDX) (IDX < cMAX_STACK_INDEX ? sp[IDX] : dp[IDX - cMAX_STACK_INDEX])
#define LOAD_SP(IDX) sp[IDX]
#define LOAD_DP(IDX) dp[IDX]

// Exit the script
#define EXIT_SCRIPT() goto exit_script

#define UT_SCRIPT_EXECUTOR_DEBUG_METHOD 0
#include "UtScriptExecutor_impl.hpp"

#undef UT_SCRIPT_EXECUTOR_DEBUG_METHOD
#define UT_SCRIPT_EXECUTOR_DEBUG_METHOD 1
// The makedepend utility doesn't like including the same file twice
#ifndef MAKEDEPEND
#include "UtScriptExecutor_impl.hpp"
#endif

//! Prints a callstack
void UtScriptExecutor::PrintCallStack(ut::log::MessageStream& aStream, bool aFullCallstack)
{
   for (size_t i = 0; i < mCallStackSize; ++i)
   {
      UtScriptExecutorProgramCounter* pc = mCallStack[i];
      if (pc->mScriptPtr)
      {
         // pc->mScriptPtr->Print(os);

         // int srcOffset = pc->mScriptPtr->GetSourceOffsetFromInstructionIndex(pc->mCodeIndex);
         // UtTextDocument doc;
         // doc.ReadFile(pc->mScriptPtr->mSourceFilePath);
         size_t line = pc->mScriptPtr->GetLineFromInstructionIndex(pc->mCodeIndex);
         // int line = doc.PositionToLineNumber(srcOffset);
         // int col = srcOffset - doc.GetLinePosition(line);
         ++line;
         //++col;

         auto callNote = aStream.AddNote() << i << ": "
                                           << mEnvironmentPtr->GetPrototypeDescription(pc->mScriptPtr->mPrototype,
                                                                                       pc->mScriptPtr->GetName());
         callNote.AddNote() << "File: " << pc->mScriptPtr->mSourceFilePath;
         callNote.AddNote() << "Line: " << line;
         { // RAII block
            auto note = callNote.AddNote() << "this:";
            pc->mInstancePtr->Print(note);
         }
         { // RAII block
            auto note = callNote.AddNote() << "Args:";
            if (pc->mNumArgs == 0)
            {
               note << " (none)";
            }

            for (size_t j = 0; j < pc->mNumArgs; j++)
            {
               auto arg = note.AddNote();
               if (j < pc->mScriptPtr->mArgumentNames.size())
               {
                  arg << pc->mScriptPtr->mArgumentNames[j] << ": ";
               }
               else
               {
                  arg << "<unnamed>: ";
               }
               arg << mStack[static_cast<size_t>(pc->mFrameIndex) + j];
            }
         }
      }
   }
}

//! Prints a callstack for each UtScriptExecutor
void UtScriptExecutor::PrintAllCallStacks(ut::log::MessageStream& aStream, bool aOutputFullStack)
{
   for (const auto& i : GetExecutors())
   {
      i->PrintCallStack(aStream, aOutputFullStack);
   }
}

void UtScriptExecutor::Attach(UtScriptDebugger* aDebuggerPtr)
{
   mDebuggerPtr = aDebuggerPtr;
   if (mDebuggerPtr != nullptr)
   {
      mExecuteFunc = &UtScriptExecutor::Execute_Debug;
   }
   else
   {
      mExecuteFunc = &UtScriptExecutor::Execute_NoDebug;
   }
}

void UtScriptExecutor::DetatchDebugger()
{
   if (mDebuggerPtr != nullptr)
   {
      mDebuggerPtr->Detatching();
      if (!mDebuggerPtr->IsBreaking())
      {
         delete mDebuggerPtr;
         mDebuggerPtr = nullptr;
         mExecuteFunc = &UtScriptExecutor::Execute_NoDebug;
      }
   }
}

void UtScriptExecutor::DoBreak(int aBreakCode, UtScriptDebuggerControlState& aControlState)
{
   if (mDebuggerPtr != nullptr)
   {
      mDebuggerPtr->Break(this, aControlState, (UtScriptDebugger::BreakType)aBreakCode);
      if (mDebuggerPtr->IsDetatching())
      {
         delete mDebuggerPtr;
         mDebuggerPtr = nullptr;
      }
   }
}

UtScriptExecutor* UtScriptExecutor::GetMainExecutor()
{
   if (!GetExecutors().empty())
      return *GetExecutors().begin();
   return nullptr;
}

UtScriptExecutor* UtScriptExecutor::GetCurrentExecutor()
{
   return sCurrentExecutor;
}

void UtScriptExecutor::ScriptException(bool aFullStack, const std::string& aExceptionDescription)
{
   if (mIgnoreExceptions)
      return;
   auto out = ut::log::error() << "Script Exception:";
   out.AddNote() << "Description: " << aExceptionDescription;

   { // RAII block
      auto callStackNote = out.AddNote() << "Callstack:";
      PrintCallStack(callStackNote, aFullStack);
   }

   if (GetAttachedDebugger())
   {
      GetAttachedDebugger()->BreakForException(this, aExceptionDescription);
   }
}

UtScriptContext* UtScriptExecutor::GetTopContext()
{
   UtScriptExecutorProgramCounter* counter = GetCallstackTop();
   return counter ? counter->mInstancePtr : nullptr;
}

void UtScriptExecutor::VariableNotFoundError(const std::string& aVariableName)
{
   std::stringstream ss;
   ss << "Variable does not exist (" << aVariableName << ").";
   ScriptException(false, ss.str());
}

void UtScriptExecutor::AttributeDoesNotExist(const std::string& aName)
{
   std::stringstream ss;
   ss << "Attribute does not exist (" << aName << ").";
   ScriptException(false, ss.str());
}

namespace
{
class DataIndex
{
public:
   DataIndex(script_code_t aIndex)
      : mIndex(aIndex)
   {
   }
   script_code_t mIndex;
};

std::ostream& operator<<(std::ostream& aOut, const DataIndex& aValue)
{
   if (aValue.mIndex < cMAX_STACK_INDEX)
   {
      aOut << "$s" << aValue.mIndex;
   }
   else
   {
      aOut << "$d" << aValue.mIndex - cMAX_STACK_INDEX;
   }
   return aOut;
}

class DataValue
{
public:
   DataValue(const UtScriptData& aValue, script_code_t aIndex = ut::script::npos)
      : mValue(aValue)
      , mIndex(aIndex)
   {
   }
   const UtScriptData& mValue;
   script_code_t       mIndex;
};

std::ostream& operator<<(std::ostream& aOut, const DataValue& aValue)
{
   if (aValue.mIndex != ut::script::npos)
   {
      aOut << DataIndex(aValue.mIndex);
   }
   aOut << '(';
   const UtScriptData& data = aValue.mValue;
   switch (data.GetType())
   {
   case ut::script::Data::Type::cBOOL:
      aOut << "bool:" << ((data.GetBool()) ? "true" : "false");
      break;
   case ut::script::Data::Type::cDOUBLE:
      aOut << "double:" << data.GetDouble();
      break;
   case ut::script::Data::Type::cINT:
      aOut << "int:" << data.GetInt();
      break;
   case ut::script::Data::Type::cSTRING:
      aOut << '"' << data.GetString() << '"';
      break;
   case ut::script::Data::Type::cPOINTER:
   {
      UtScriptRef* refPtr = data.GetPointer();
      if ((refPtr != nullptr) && (refPtr->GetScriptClass() != nullptr))
      {
         aOut << "type:" << refPtr->GetScriptClass()->GetClassName();
      }
   }
   break;
   default:
      aOut << "<undefined-type>";
      break;
   }
   aOut << ')';
   return aOut;
}
} // namespace

#define DATA_VALUE(INDEX) DataValue(LOAD(INDEX), INDEX)
#define DATA_VALUE_SP(INDEX) DataValue(LOAD_SP(INDEX), INDEX)
#define DATA_VALUE_DP(INDEX) DataValue(LOAD_DP(INDEX), cMAX_STACK_INDEX + INDEX)

#define PRINT_ARGS(ARG_COUNT)                    \
   aOut << '(';                                  \
   for (script_code_t i = 0; i < ARG_COUNT; ++i) \
   {                                             \
      script_code_t idx = NEXT_OP();             \
      if (i > 0)                                 \
      {                                          \
         aOut << ", ";                           \
      }                                          \
      aOut << DataValue(LOAD(idx), idx);         \
   }                                             \
   aOut << ')';

// This method can be called to print the current instruction at run-time.
// A call to this method may be planted into the execution loop it UtScriptExecutor_impl.hpp.
void UtScriptExecutor::PrintCurrentInstruction(std::ostream&          aOut,
                                               const UtScript&        aScript,
                                               const UtScriptContext& aContext,
                                               const UtScriptData*    aStackPtr,
                                               const UtScriptData*    aDataPtr,
                                               size_t                 aCodeIndex)
{
   size_t               codeIndex = aCodeIndex;
   const UtScriptData*  sp        = aStackPtr;
   const UtScriptData*  dp        = aDataPtr;
   const script_code_t* codePtr   = aScript.mOps.data();

   aOut << std::setw(25) << aScript.GetName();
   aOut << " PC: " << std::setw(8) << codeIndex;
   script_code_t opCode = codePtr[codeIndex];
   aOut << std::setw(25) << UtScript::GetOpcodeString(opCode);
   bool from_sp = false;

   auto DATA_SPDP = [&](script_code_t aIndex) { return from_sp ? DATA_VALUE_SP(aIndex) : DATA_VALUE_DP(aIndex); };
   switch (static_cast<UtScript::Opcode>(opCode))
   {
   case UtScript::Opcode::CAST:
   {
      OP3(s, castTypeIndex, from);
      aOut << ' ' << DataIndex(s) << " = (" << aScript.GetScriptClass(castTypeIndex)->GetClassName() << ") "
           << DATA_VALUE(from);
   }
   break;
   case UtScript::Opcode::LOAD_LOCAL_VAR:
   {
      OP2(s, di);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE_DP(di);
   }
   break;
   case UtScript::Opcode::LOAD_STATIC_VAR:
   {
      OP2(s, i);
      UtScript&     script  = const_cast<UtScript&>(aScript);
      UtScriptData* dataPtr = script.GetStaticVariable(i);
      aOut << ' ' << DataIndex(s) << " = static " << *dataPtr << ';';
   }
   break;
   case UtScript::Opcode::LOAD_GLOBAL_VAR:
   {
      OP2(s, nameId);
      auto varName = OpToStr(nameId);
      aOut << ' ' << DataIndex(s) << " = " << varName << "; (";
      UtScriptData* varPtr = aContext.FindGlobalVar(varName);
      if (varPtr != nullptr)
      {
         aOut << *varPtr << ");";
      }
      else
      {
         aOut << "not-found);";
      }
   }
   break;
   case UtScript::Opcode::LOAD_STRUCT_VAR:
   {
      OP3(s, l, nameId);
      const UtScriptData& base    = sp[l];
      auto                varName = OpToStr(nameId);
      aOut << ' ' << DataIndex(s) << " = " << DataIndex(l);
      if (base.GetPointer() != nullptr)
      {
         UtScriptClass* classPtr = base.GetPointer()->GetScriptClass();
         UtScriptData*  attrPtr  = classPtr->GetAttribute(base.GetPointer()->GetAppObject(), varName);
         aOut << '(' << classPtr->GetClassName() << '.' << varName << ')';
         if (attrPtr != nullptr)
         {
            aOut << '=' << DataValue(*attrPtr);
         }
         aOut << ';';
      }
      else
      {
         aOut << " unknown." << varName << ';';
      }
   }
   break;
   case UtScript::Opcode::LOAD_READ_ONLY_FUNCTION_VAR:
   {
      OP2(s, nameId);
      auto varName = OpToStr(nameId);
      aOut << ' ' << DataIndex(s) << " = " << varName << "; (";
      UtScriptContext::VariableReadFunction* fnPtr = aContext.FindReadOnlyVariableFunction(varName);
      if (fnPtr != nullptr)
      {
         UtScriptData s;
         fnPtr->Read(s);
         aOut << s << ')';
      }
      else
      {
         aOut << "not-found)";
      }
   }
   break;
   case UtScript::Opcode::STORE_LOCAL_VAR_SP:
   {
      OP2(s, l);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(l) << ';';
   }
   break;
   case UtScript::Opcode::STORE_STATIC_VAR:
   {
      OP2(s, l);
      aOut << " static<" << s << "> = " << DATA_VALUE(l) << ';';
   }
   break;
   case UtScript::Opcode::STORE_GLOBAL_VAR:
   {
      OP2(nameId, l);
      aOut << " global<" << OpToStr(nameId) << "> = " << DATA_VALUE(l) << ';';
   }
   break;
   case UtScript::Opcode::STORE_RVALUE_GLOBAL_VAR:
   {
      OP2(nameId, l);
      aOut << " global<" << OpToStr(nameId) << "> = move " << DATA_VALUE(l) << ';';
   }
   break;
   case UtScript::Opcode::STORE_STRUCT_VAR:
   {
      OP3(baseIndex, nameId, rhs);
      std::string         className("unknown");
      auto                varName = OpToStr(nameId);
      const UtScriptData& base    = sp[baseIndex];
      if (base.GetPointer() != nullptr)
      {
         className = base.GetPointer()->GetScriptClass()->GetClassName();
      }
      aOut << ' ' << DataIndex(baseIndex) << '(' << className << '.' << varName << ") = " << DATA_VALUE(rhs) << ';';
   }
   break;
   case UtScript::Opcode::CREATE_VAR:
   {
      OP2(s, classIndex);
      std::string className(aScript.GetScriptClass(classIndex)->GetClassName());
      aOut << ' ' << className << ' ' << DataIndex(s) << " = " << className << "();";
   }
   break;
   case UtScript::Opcode::CREATE_NULL_VAR:
   {
      OP2(s, classIndex);
      std::string className(aScript.GetScriptClass(classIndex)->GetClassName());
      aOut << ' ' << className << ' ' << DataIndex(s) << ';';
   }
   break;
   case UtScript::Opcode::CLONE_VAR:
   {
      OP2(s, l);
      aOut << ' ' << DataIndex(s) << " = Clone(" << DATA_VALUE(l) << ");";
   }
   break;
   case UtScript::Opcode::INIT_VAR:
   {
      OP2(s, classIndex);
      std::string className(aScript.GetScriptClass(classIndex)->GetClassName());
      aOut << ' ' << className << ' ' << DataIndex(s) << ';';
   }
   break;
   case UtScript::Opcode::ADD:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " + " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::SUBTRACT:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " - " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::MULTIPLY:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " * " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::DIVIDE:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " / " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::NOT:
   {
      OP2(s, l);
      aOut << ' ' << DataIndex(s) << " = !(" << DATA_VALUE(l) << ");";
   }
   break;
   case UtScript::Opcode::NEGATE:
   {
      OP2(s, l);
      aOut << ' ' << DataIndex(s) << " = -(" << DATA_VALUE(l) << ");";
   }
   break;
   case UtScript::Opcode::IS_TRUE:
   {
      OP2(s, l);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(l) << ").IsTrue();";
   }
   break;
   case UtScript::Opcode::EQUAL:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " == " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::NOT_EQUAL:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " != " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::LESS:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " < " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::LESS_EQUAL:
   {
      OP3(s, a, b);
      aOut << ' ' << DataIndex(s) << " = " << DATA_VALUE(a) << " <= " << DATA_VALUE(b) << ';';
   }
   break;
   case UtScript::Opcode::JUMP:
   {
      OP1(next_pc);
      aOut << " PC: " << next_pc;
   }
   break;
   case UtScript::Opcode::JUMP_IF_TRUE:
   {
      OP2(next_pc, l);
      aOut << " PC: " << next_pc << " if (" << DATA_VALUE(l) << ");";
   }
   break;
   case UtScript::Opcode::JUMP_IF_FALSE:
   {
      OP2(next_pc, l);
      aOut << " PC: " << next_pc << " if (!" << DATA_VALUE(l) << ");";
   }
   break;
   case UtScript::Opcode::JUMP_IF_NOT_FIRST:
   {
      OP2(next_pc, isFirst);
      aOut << " PC: " << next_pc << " first=" << DATA_VALUE(isFirst) << ';';
   }
   break;
   case UtScript::Opcode::RETURN_FUNCTION_SP:
      from_sp = true; //[[fallthrough]];
   case UtScript::Opcode::RETURN_FUNCTION_DP:
   {
      OP1(l);
      aOut << ' ' << DATA_SPDP(l);
   }
   break;
   case UtScript::Opcode::CALL_APP_FUNCTION:
   {
      OP3(refIndex, methodIndex, argCount);
      aOut << ' ' << DataIndex(refIndex) << " = ";
      const UtScriptData& ref = sp[refIndex];
      if (ref.GetType() == ut::script::Data::Type::cPOINTER)
      {
         UtScriptRef*   refPtr   = ref.GetPointer();
         UtScriptClass* classPtr = refPtr->GetScriptClass();
         if (classPtr != nullptr)
         {
            UtScriptClass::InterfaceMethod* methodPtr = classPtr->GetMethodEntry(methodIndex);
            aOut << classPtr->GetClassName() << "::" << methodPtr->GetName();
         }
      }
      else if (ref.GetType() == ut::script::Data::Type::cSTRING)
      {
         aOut << ref.GetString();
      }
      else
      {
         aOut << "??? ref=" << ref << " methodIndex=" << methodIndex;
      }
      PRINT_ARGS(argCount);
      OP1(s);
      if (s != ut::script::npos)
      {
         aOut << " store_into " << DataIndex(s) << ';';
      }
      else
      {
         aOut << " returns void;";
      }
   }
   break;
   case UtScript::Opcode::CALL_STATIC_APP_FUNCTION:
   {
      OP3(classTypeIndex, methodIndex, argCount);
      UtScriptClass* classPtr = aScript.GetScriptClass(classTypeIndex);
      aOut << ' ' << classPtr->GetClassName() << '.' << classPtr->GetMethodEntry(methodIndex)->GetName();
      PRINT_ARGS(argCount);
      OP1(s);
      if (s != ut::script::npos)
      {
         aOut << " store_into " << DataIndex(s) << ';';
      }
      else
      {
         aOut << " returns void;";
      }
   }
   break;
   case UtScript::Opcode::CALL_SCRIPT:
   {
      OP2(scriptNameId, argCount);
      aOut << ' ' << OpToStr(scriptNameId);
      PRINT_ARGS(argCount);
      OP1(s);
      if (s != ut::script::npos)
      {
         aOut << " store_into " << DataIndex(s) << ';';
      }
      else
      {
         aOut << " returns void;";
      }
   }
   break;
   case UtScript::Opcode::CALL_STRUCT_SCRIPT:
   {
      OP3(baseIndex, scriptNameId, argCount);
      aOut << ' ' << DATA_VALUE(baseIndex) << '.' << OpToStr(scriptNameId);
      PRINT_ARGS(argCount);
      OP1(s);
      if (s != ut::script::npos)
      {
         aOut << " store_into " << DataIndex(s) << ';';
      }
      else
      {
         aOut << " returns void;";
      }
   }
   break;
   case UtScript::Opcode::CALL_DYN_STRUCT_SCRIPT:
   {
      OP3(baseIndex, scriptNameId, argCount);
      aOut << ' ' << DATA_VALUE(baseIndex) << '.' << OpToStr(scriptNameId);
      PRINT_ARGS(argCount);
      OP1(s);
      if (s != ut::script::npos)
      {
         aOut << " store_into " << DataIndex(s) << ';';
      }
      else
      {
         aOut << " returns void;";
      }
   }
   break;
   case UtScript::Opcode::SET_DEBUG_FLAG:
   {
   }
   break;
   case UtScript::Opcode::END:
   {
   }
   break;
   default:
      break;
   }
   aOut << std::endl;
}
