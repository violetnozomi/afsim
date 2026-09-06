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

#ifndef UTSCRIPT_HPP
#define UTSCRIPT_HPP

#include "util_script_export.h"

#include <atomic>
#include <cstddef>
#include <string>
#include <vector>

#include "UtLog.hpp"
#include "UtScriptData.hpp"
class UtScriptEnvironment;
class UtScriptFunction;

namespace ut
{
namespace script
{
using script_code_t          = uint_fast32_t;
constexpr script_code_t npos = static_cast<script_code_t>(-1);
} // namespace script
} // namespace ut

class UTIL_SCRIPT_EXPORT UtScript
{
public:
   static const std::string      sOpcodeStrings[];
   static bool                   ScriptIsValid(UtScript* aScriptPtr);
   static std::vector<UtScript*> GetAllScripts();

   using script_code_t = ut::script::script_code_t;
   //! The set of opcodes used to implement scripts.
   //! _SP suffix means it is loading from the stack pointer (sp)
   //! _DP suffis means it is loading from the script's data pointer (dp)
   enum class Opcode : script_code_t
   {
      // Print Operations.
      CAST,

      // Stack Operations.
      LOAD_LOCAL_VAR,
      LOAD_STATIC_VAR,
      LOAD_GLOBAL_VAR,
      LOAD_STRUCT_VAR,
      LOAD_READ_ONLY_FUNCTION_VAR,

      STORE_LOCAL_VAR_SP,
      STORE_STATIC_VAR,
      STORE_GLOBAL_VAR,
      STORE_RVALUE_GLOBAL_VAR,
      STORE_STRUCT_VAR,

      CREATE_VAR,
      CREATE_NULL_VAR,
      CLONE_VAR,
      INIT_VAR,

      // Mathematical Operations.
      ADD,
      SUBTRACT,
      MULTIPLY,
      DIVIDE,

      // Logical Operations
      NOT,
      NEGATE,
      IS_TRUE,

      // Comparison Operations
      EQUAL,
      NOT_EQUAL,
      LESS,
      LESS_EQUAL,

      // Execution Order Operations
      JUMP,
      JUMP_IF_TRUE,
      JUMP_IF_FALSE,
      JUMP_IF_NOT_FIRST,

      // Function Operations
      RETURN_FUNCTION_SP,
      RETURN_FUNCTION_DP,

      // Application Interface Operations
      CALL_APP_FUNCTION,
      CALL_STATIC_APP_FUNCTION,
      CALL_SCRIPT,
      CALL_STRUCT_SCRIPT,
      CALL_DYN_STRUCT_SCRIPT,

      SET_DEBUG_FLAG,

      END
   };

   struct LocalVariable
   {
      bool      operator<(const LocalVariable& aRhs) const { return mName < aRhs.mName; }
      bool      IsStaticVariable() { return mStackIndex < 0; }
      ptrdiff_t GetStaticVariableIndex() { return -mStackIndex - 1; }

      std::string    mName;
      UtScriptClass* mTypePtr{nullptr};
      ptrdiff_t      mStackIndex; // negative for static variables (index into mStaticVariables)
      size_t         mValidAfter;
      size_t         mValidBefore;
   };

   UtScript();
   UtScript(const UtScript& aRhs) = delete;
   ~UtScript();
   UtScript& operator<<(Opcode aVal);
   //! Append opcode data to the script.
   UtScript& operator<<(script_code_t aVal)
   {
      mOps.push_back(aVal);
      return *this;
   }
   UtScript& operator<<(UtScriptClass* aClassPtr);

   size_t             GetArgumentCount() const;
   const std::string& GetName() const { return mName; }
   UtStringId         GetReturnType() const;
   void               Print(ut::log::MessageStream& aStream) const;
   void               PrintOpcode(ut::log::MessageStream& aStream, const script_code_t* aOpcodePtr) const;
   void               Clear()
   {
      mOps.clear();
      mData.clear();
      mData.resize(1);
   }
   static size_t             GetOpcodeLength(const script_code_t* aOpData);
   static const std::string& GetOpcodeString(script_code_t aOpcode);
   script_code_t             GetDataIndex(const UtScriptData& aValue);
   size_t                    GetSourceOffsetFromInstructionIndex(size_t aInstrIndex) const;
   size_t                    GetInstructionIndexFromLineNumber(size_t& aLineNumber) const;
   size_t                    GetLineFromInstructionIndex(size_t aInstrIndex) const;
   void                      GetSourceLineRange(size_t& aFirstLine, size_t& aLastLine) const;

   UtScriptClass* GetScriptClass(size_t aIndex) const { return mClassList[aIndex]; }

   //! Returns the static variable at aIndex
   UtScriptData* GetStaticVariable(size_t aIndex) { return &mStaticVariables[aIndex]; }

   //! Return the name of the script
   const std::string& GetName(size_t aIndex) const { return mNameList[aIndex]; }

   void Registered(UtScriptEnvironment* aEnvironmentPtr);

   //! Return the argument name at aIndex
   std::string GetArgumentName(size_t aIndex) const
   {
      return mArgumentNames.size() > aIndex ? mArgumentNames[aIndex] : "";
   }

   //! @name Reference count maintenance.
   //@{
   void AddRef();
   void Unref();
   //@}

   //! Contains the instructions to execute
   std::vector<script_code_t> mOps;

   //! Contains the data (constants) used by this script
   std::vector<UtScriptData> mData;

   //! Contains the script classes used by this script
   std::vector<UtScriptClass*> mClassList;

   //! Because opcodes are integer only, strings must be stored elsewhere.
   //! This is a list of names used by opcodes that reference variables by name
   std::vector<std::string> mNameList;

   //! Name of this script
   std::string mName;

   //! The signature of the script
   const UtScriptFunction* mPrototype;

   //! Names of the script parameters
   std::vector<std::string> mArgumentNames;

   //! Number of stack slots used by this script
   ptrdiff_t mStackSize;

   //! List of static variables used by this script
   std::vector<UtScriptData> mStaticVariables;

   //! Is this script a script_variables block
   bool mIsVarScript;

   //! Path to the file containing this script
   std::string mSourceFilePath;

   //! Maps location in the file to script opcode index
   struct SourceInstructionOffset
   {
      size_t mSourceOffset;
      size_t mSourceLine;
      size_t mInstructionIndex;
   };

   std::vector<SourceInstructionOffset> mSourceOffsetToInstruction;

   static bool      IsStaticVariableIndex(ptrdiff_t aIndex) { return aIndex < 0; }
   static ptrdiff_t GetStaticVariableIndex(ptrdiff_t aIndex) { return -aIndex - 1; }

   std::vector<LocalVariable> mLocalVariables;

   LocalVariable* FindLocalVariable(std::string& aName, size_t aCodeIndex);

private:
   UtScriptEnvironment* mEnvIfRegistered;

   std::atomic<int> mReferenceCount;
#ifndef NDEBUG
   size_t mDebug_PrevOpCodeIndex;
#endif
};

#endif
