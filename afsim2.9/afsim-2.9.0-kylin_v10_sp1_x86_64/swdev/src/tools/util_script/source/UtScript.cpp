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

#include "UtScript.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>

#include "UtScriptEnvironment.hpp"
#include "UtScriptFunction.hpp"
#include "UtScriptRegistry.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtTextDocument.hpp"

const std::string UtScript::sOpcodeStrings[static_cast<size_t>(UtScript::Opcode::END) + 1] = {
   "cast",
   "load_local_var",
   "load_static_var",
   "load_global_var",
   "load_struct_var",
   "load_read_only_function_var",
   "store_local_var_sp",
   "store_static_var",
   "store_global_var",
   "store_rvalue_global_var",
   "store_struct_var",
   "create_var",
   "create_null_var",
   "clone_var",
   "init_var",
   "add",
   "subtract",
   "multiply",
   "divide",
   "not",
   "negate",
   "is_true",
   "equal",
   "not_equal",
   "less",
   "less_equal",
   "jump",
   "jump_if_true",
   "jump_if_false",
   "jump_if_not_first",
   "return_function_sp",
   "return_function_dp",
   "call_app_function",
   "call_static_app_function",
   "call_script",
   "call_struct_script",
   "call_dyn_script_struct",
   "set_debug_flag",
   "end"};

// Used in subtraction and comparison operations so signed int rather than unsigned.
static const int cMAX_STACK_INDEX_INT = 100000;

UtScript::UtScript()
   : mData(1)
   , mPrototype()
   , mStackSize(-1)
   , mIsVarScript(false)
   , mEnvIfRegistered(nullptr)
   , mReferenceCount(1)
{
#ifndef NDEBUG
   mDebug_PrevOpCodeIndex = -1;
#endif
}

UtScript::~UtScript()
{
   if (mEnvIfRegistered != nullptr)
   {
      mEnvIfRegistered->GetRegistry()->UnregisterScript(this);
      mEnvIfRegistered->ScriptDeleted(this);
   }
}

void UtScript::Print(ut::log::MessageStream& aStream) const
{
   aStream.AddNote() << "Name: " << mName;
   if (mData.size() > 1)
   {
      auto dataNote = aStream.AddNote() << "Data:";
      for (size_t i = 1; i < mData.size(); ++i)
      {
         auto note = dataNote.AddNote() << i << ": ";
         switch (mData[i].GetType())
         {
         case ut::script::Data::Type::cUNDEFINED:
            note.AddNote() << "Type: UNDEF";
            break;
         case ut::script::Data::Type::cBOOL:
            note.AddNote() << "Type: bool";
            break;
         case ut::script::Data::Type::cINT:
            note.AddNote() << "Type: int";
            break;
         case ut::script::Data::Type::cDOUBLE:
            note.AddNote() << "Type: double";
            break;
         case ut::script::Data::Type::cSTRING:
            note.AddNote() << "Type: string";
            break;
         case ut::script::Data::Type::cPOINTER:
            note.AddNote() << "Type: ptr";
            break;
         }
         note.AddNote() << "Value: " << mData[i];
      }
   }

   size_t i = 0;

   size_t         currentLine = -1;
   UtTextDocument doc;
   doc.ReadFile(mSourceFilePath);
   if (!mSourceOffsetToInstruction.empty())
   {
      currentLine = doc.PositionToLineNumber(mSourceOffsetToInstruction[0].mSourceOffset) - 1;
   }

   auto instNote = aStream.AddNote() << "Instructions:";
   while (i < mOps.size())
   {
      size_t newLine = doc.PositionToLineNumber(GetSourceOffsetFromInstructionIndex(i));
      while (newLine > currentLine)
      {
         ++currentLine;
         instNote.AddNote() << doc.GetLineString(currentLine);
      }
      assert(mOps[i] <= static_cast<size_t>(Opcode::END));
      size_t len = GetOpcodeLength(&mOps[i]) - 1;
      assert(len + i <= mOps.size());

      auto note = instNote.AddNote() << i << ":";
      PrintOpcode(note, &mOps[i]);
      i += len + 1;
   }
}

//! Returns the length of the opcode in words
//! Opcodes have variable length, and this method is needed to traverse
//! the instruction list
size_t UtScript::GetOpcodeLength(const script_code_t* aOpData)
{
   size_t opCode = 0;
   switch (static_cast<Opcode>(*aOpData))
   {
   case Opcode::CAST:
      opCode = 4;
      break;
   case Opcode::LOAD_LOCAL_VAR:
   case Opcode::LOAD_STATIC_VAR:
   case Opcode::LOAD_GLOBAL_VAR:
      opCode = 3;
      break;
   case Opcode::LOAD_STRUCT_VAR:
      opCode = 4;
      break;
   case Opcode::LOAD_READ_ONLY_FUNCTION_VAR:
      opCode = 3;
      break;
   case Opcode::STORE_LOCAL_VAR_SP:
   case Opcode::STORE_STATIC_VAR:
   case Opcode::STORE_GLOBAL_VAR:
   case Opcode::STORE_RVALUE_GLOBAL_VAR:
      opCode = 3;
      break;
   case Opcode::STORE_STRUCT_VAR:
      opCode = 4;
      break;
   case Opcode::CREATE_VAR:
   case Opcode::CREATE_NULL_VAR:
   case Opcode::CLONE_VAR:
   case Opcode::INIT_VAR:
      opCode = 3;
      break;
   case Opcode::ADD:
   case Opcode::SUBTRACT:
   case Opcode::MULTIPLY:
   case Opcode::DIVIDE:
      opCode = 4;
      break;
   case Opcode::NOT:
      opCode = 3;
      break;
   case Opcode::NEGATE:
      opCode = 3;
      break;
   case Opcode::IS_TRUE:
      opCode = 3;
      break;
   case Opcode::EQUAL:
   case Opcode::NOT_EQUAL:
   case Opcode::LESS:
   case Opcode::LESS_EQUAL:
      opCode = 4;
      break;
   case Opcode::JUMP:
      opCode = 2;
      break;
   case Opcode::JUMP_IF_TRUE:
      opCode = 3;
      break;
   case Opcode::JUMP_IF_FALSE:
      opCode = 3;
      break;
   case Opcode::JUMP_IF_NOT_FIRST:
      opCode = 3;
      break;
   case Opcode::RETURN_FUNCTION_SP:
   case Opcode::RETURN_FUNCTION_DP:
      opCode = 2;
      break;
   case Opcode::CALL_APP_FUNCTION:
      opCode = aOpData[3] + 5;
      break;
   case Opcode::CALL_STATIC_APP_FUNCTION:
      opCode = aOpData[3] + 5;
      break;
   case Opcode::CALL_SCRIPT:
      opCode = aOpData[2] + 4;
      break;
   case Opcode::CALL_STRUCT_SCRIPT:
      opCode = aOpData[3] + 5;
      break;
   case Opcode::CALL_DYN_STRUCT_SCRIPT:
      opCode = aOpData[3] + 5;
      break;
   case Opcode::SET_DEBUG_FLAG:
      opCode = 1;
      break;
   case Opcode::END:
      opCode = 1;
      break;
   default:
      assert(false);
      opCode = 0;
      break;
   }
   return opCode;
}

//! Returns the name of the opcode as a string.
const std::string& UtScript::GetOpcodeString(script_code_t aOpCode)
{
   return sOpcodeStrings[aOpCode];
}

//! Returns an index that points to the specified value
//! May use an existing value if it already exists
UtScript::script_code_t UtScript::GetDataIndex(const UtScriptData& aValue)
{
   for (size_t i = 0; i < mData.size(); ++i)
   {
      if (mData[i].GetType() == aValue.GetType() && mData[i] == aValue)
      {
         // verify narrowing conversion works.
         assert(static_cast<script_code_t>(i) == i);
         return static_cast<script_code_t>(i);
      }
   }
   mData.emplace_back(aValue);
   return static_cast<script_code_t>(mData.size() - 1);
}

UtStringId UtScript::GetReturnType() const
{
   if (mPrototype != nullptr)
   {
      return mPrototype->mReturnClassPtr->GetClassName();
   }
   return UtStringIdLiteral("void");
}

void UtScript::AddRef()
{
   mReferenceCount.fetch_add(1, std::memory_order_relaxed);
}

void UtScript::Unref()
{
   auto newcount = mReferenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
   assert(newcount >= 0);
   if (newcount == 0)
   {
      delete this;
   }
}

//! Append opcode data to the script.
UtScript& UtScript::operator<<(Opcode aVal)
{
   // In debug mode, check that when a new opcode is started, the previous one is the correct length
#ifndef NDEBUG
   if (mDebug_PrevOpCodeIndex != static_cast<size_t>(-1))
   {
      size_t opLen = GetOpcodeLength(&mOps[mDebug_PrevOpCodeIndex]);
      assert(opLen + mDebug_PrevOpCodeIndex == mOps.size());
   }
   mDebug_PrevOpCodeIndex = mOps.size();
#endif
   mOps.push_back(static_cast<script_code_t>(aVal));
   return *this;
}

UtScript& UtScript::operator<<(UtScriptClass* aClassPtr)
{
   auto i = std::find(mClassList.begin(), mClassList.end(), aClassPtr);
   if (i != mClassList.end())
   {
      auto offset = i - mClassList.begin();
      // check that offset fits into script_code_t
      assert(static_cast<decltype(offset)>(static_cast<script_code_t>(offset)) == offset);
      mOps.push_back(static_cast<script_code_t>(offset));
   }
   else
   {
      // check that size() fits into script_code_t
      assert(static_cast<size_t>(static_cast<script_code_t>(mClassList.size())) == mClassList.size());
      mOps.push_back(static_cast<script_code_t>(mClassList.size()));
      mClassList.push_back(aClassPtr);
   }
   return *this;
}

//! Given an instruction offset, return the position in the source file
//! which contributed to the instruction.  This provides a column resolution
//! file position for runtime errors and the (future) debugger.
size_t UtScript::GetSourceOffsetFromInstructionIndex(size_t aInstrIndex) const
{
   size_t sourceOffset = 0;
   for (const auto& i : mSourceOffsetToInstruction)
   {
      if (i.mInstructionIndex > aInstrIndex)
      {
         break;
      }
      sourceOffset = i.mSourceOffset;
   }
   return sourceOffset;
}

size_t UtScript::GetLineFromInstructionIndex(size_t aInstrIndex) const
{
   size_t line = 0;
   for (const auto& i : mSourceOffsetToInstruction)
   {
      if (i.mInstructionIndex > aInstrIndex)
      {
         break;
      }
      line = i.mSourceLine;
   }
   return line;
}

//! Print a single opcode
void UtScript::PrintOpcode(ut::log::MessageStream& aStream, const script_code_t* aOpcodePtr) const
{
   UtScriptClass* classPtr = nullptr;
   size_t         i        = 0;
   Opcode         op       = (Opcode)aOpcodePtr[i];
   aStream.AddNote() << "Op Number: " << static_cast<size_t>(op);
   aStream.AddNote() << "Op Name:" << sOpcodeStrings[static_cast<script_code_t>(op)];
   auto   params = aStream.AddNote() << "Params:";
   size_t len    = GetOpcodeLength(&aOpcodePtr[i]) - 1;
   for (size_t j = 0; j < len; ++j)
   {
      ++i;
      if ((op == Opcode::LOAD_GLOBAL_VAR && j == 1) || (op == Opcode::STORE_GLOBAL_VAR && j == 0) ||
          (op == Opcode::LOAD_STRUCT_VAR && j == 2) || (op == Opcode::STORE_STRUCT_VAR && j == 1) ||
          (op == Opcode::LOAD_READ_ONLY_FUNCTION_VAR && j == 1) || (op == Opcode::CALL_SCRIPT && j == 0) ||
          (op == Opcode::CALL_STRUCT_SCRIPT && j == 1) || (op == Opcode::CALL_DYN_STRUCT_SCRIPT && j == 1))
      {
         auto name = UtStringId::UnsafeFindStringId(static_cast<int>(aOpcodePtr[i]));
         params.AddNote() << "name: " << name;
      }
      else if ((op == Opcode::CALL_APP_FUNCTION && j == 2) || (op == Opcode::CALL_STATIC_APP_FUNCTION && j == 2) ||
               (op == Opcode::CALL_SCRIPT && j == 1) || (op == Opcode::CALL_STRUCT_SCRIPT && j == 2) ||
               (op == Opcode::CALL_DYN_STRUCT_SCRIPT && j == 4))
      {
         params.AddNote() << "argc: " << aOpcodePtr[i]; // number of args
      }
      else if ((op == Opcode::CREATE_VAR && j == 1) || (op == Opcode::CREATE_NULL_VAR && j == 1) ||
               (op == Opcode::INIT_VAR && j == 1) || (op == Opcode::CALL_STATIC_APP_FUNCTION && j == 0))
      {
         size_t classIndex = aOpcodePtr[i];
         if (classIndex < mClassList.size())
         {
            classPtr = GetScriptClass(classIndex);
            params.AddNote() << "class: " << classPtr->GetClassName();
         }
         else
         {
            params.AddNote() << "classIndex: " << classIndex;
            params.AddNote() << "size: " << mClassList.size();
         }
      }
      else if ((op == Opcode::CALL_STATIC_APP_FUNCTION && j == 1) || (op == Opcode::CALL_APP_FUNCTION && j == 1))
      {
         params.AddNote() << "method: " << aOpcodePtr[i];
      }
      else
      {
         size_t o = aOpcodePtr[i];
         if (o > cMAX_STACK_INDEX_INT && mData.size() > o - cMAX_STACK_INDEX_INT)
         {
            // data value
            params.AddNote() << "data: " << mData[o - cMAX_STACK_INDEX_INT];
         }
         else
         {
            // stack index
            params.AddNote() << "stackIndex: $" << o;
         }
      }
   }
}

size_t UtScript::GetArgumentCount() const
{
   return mPrototype->mArgs.size();
}

void UtScript::GetSourceLineRange(size_t& aFirstLine, size_t& aLastLine) const
{
   if (!mSourceOffsetToInstruction.empty())
   {
      aFirstLine = mSourceOffsetToInstruction[0].mSourceLine;
      aLastLine  = mSourceOffsetToInstruction.back().mSourceLine;
   }
   else
   {
      aFirstLine = aLastLine = 0;
   }
}

size_t UtScript::GetInstructionIndexFromLineNumber(size_t& aLineNumber) const
{
   for (SourceInstructionOffset sio : mSourceOffsetToInstruction)
   {
      if (sio.mSourceLine == aLineNumber)
      {
         aLineNumber = sio.mSourceLine;
         return sio.mInstructionIndex;
      }
   }
   return static_cast<size_t>(-1);
}

void UtScript::Registered(UtScriptEnvironment* aEnvironmentPtr)
{
   if (mEnvIfRegistered == nullptr)
   {
      mEnvIfRegistered = aEnvironmentPtr;
      mEnvIfRegistered->ScriptCreated(this);
      std::sort(mLocalVariables.begin(), mLocalVariables.end());
   }
}

UtScript::LocalVariable* UtScript::FindLocalVariable(std::string& aName, size_t aCodeIndex)
{
   LocalVariable tmp;
   tmp.mName = aName;
   size_t i  = (std::lower_bound(mLocalVariables.begin(), mLocalVariables.end(), tmp) - mLocalVariables.begin());
   for (; i < mLocalVariables.size() && mLocalVariables[i].mName == aName; ++i)
   {
      if (aCodeIndex > mLocalVariables[i].mValidAfter && aCodeIndex <= mLocalVariables[i].mValidBefore)
      {
         return &mLocalVariables[i];
      }
   }
   return nullptr;
}
