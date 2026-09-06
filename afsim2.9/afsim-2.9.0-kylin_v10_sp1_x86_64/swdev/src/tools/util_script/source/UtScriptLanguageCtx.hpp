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

#ifndef UTSCRIPTLANGUAGECTX_HPP
#define UTSCRIPTLANGUAGECTX_HPP

#include <map>

#include "UtScript.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptFunction.hpp"
#include "UtScriptLanguageScanner.hpp"

////////////////////////////////////////////////////////////////////
// WARNING: This file is used as a template for another script parser.
//          Code is organized to keep definitions easy to share.
////////////////////////////////////////////////////////////////////
// Script Parser:
// The script parser is divided into these parts:
//  * script.atg - The grammar file.  An input into the COCO/R parser generator.
//                 Results in the UtScriptLanguageParser and UtScriptLanguageScanner classes
//  * UsCtx - Maintains a syntactic context while parsing a script.  Used almost exclusively by script.atg
//  * UsVal - A script object, function, value, etc... to be used while parsing.
//  * UtScriptParser - A front end to the script parser

class UtScriptScope;

namespace UtScriptLanguage
{
class Parser;
class UsCtx;
class UsConstCtx;
class UsInitializerList;
struct UsInitializerListData;
class UsVal;

using Cx            = UsCtx;
using Ty            = UtScriptClass;
using Fn            = UtScriptFunction;
using Val           = UsVal;
using script_code_t = ut::script::script_code_t;
using offset_t      = script_code_t;

// Initializer lists are the result of parsing the '{' ... '}' syntax for representing containers
// This object stores information about the values as they were parsed without knowledge of any left-hand
// side type information.  At a later time, initializer lists are 'realized' as a container value.
class UsInitializerList
{
public:
   UsInitializerList();
   ~UsInitializerList();

   UtReferenceCount* GetReferenceCount() const { return mRefCount; }

   void AddRef() { mRefCount->AddStrongRef(); }
   void RemoveRef()
   {
      if (mRefCount->RemoveStrongRef())
         delete this;
   }

   UsInitializerListData* mDataPtr;

private:
   UtReferenceCount* mRefCount;
};

// Represents a script value during the compilation process
class UsVal
{
public:
   enum Flags
   {
      cLOCAL_VARIABLE              = 1,
      cSTATIC_VARIABLE             = 2,
      cGLOBAL_VARIABLE             = 4,
      cEXTERN_VARIABLE             = 8,
      cTEMPORARY_VARIABLE          = 16,
      cREAD_ONLY_FUNCTION_VARIABLE = 32,
      cSTRUCT_VARIABLE             = 64
   };

   static Ty* sVarClassPtr;

   UsVal() = default;

   UsVal(const UsVal& aRhs)
      : mType(aRhs.mType)
      , mInitList(aRhs.mInitList)
      , mValName(aRhs.mValName)
      , mMethodNameId(aRhs.mMethodNameId)
      , mValue(aRhs.mValue)
      , mPrototypePtr(aRhs.mPrototypePtr)
      , mStackIndex(aRhs.mStackIndex)
      , mFlags(aRhs.mFlags)
      , mStaticVarIndex(aRhs.mStaticVarIndex)
      , mMethodIndex(aRhs.mMethodIndex)
      , mStructVarName(aRhs.mStructVarName)
      , mValidAfterInstruction(aRhs.mValidAfterInstruction)
      , mValNameId(aRhs.mValNameId)
      , mStructVarNameId(aRhs.mStructVarNameId)
   {
      if (mValue != nullptr)
      {
         mValue = new UtScriptData(*mValue);
      }
   }

   UsVal& operator=(const UsVal& aRhs)
   {
      if (this == &aRhs)
         return *this;
      mType         = aRhs.mType;
      mInitList     = aRhs.mInitList;
      mValName      = aRhs.mValName;
      mMethodNameId = aRhs.mMethodNameId;
      delete mValue;
      mValue = aRhs.mValue;
      if (mValue != nullptr)
      {
         mValue = new UtScriptData(*mValue);
      }
      mPrototypePtr          = aRhs.mPrototypePtr;
      mStackIndex            = aRhs.mStackIndex;
      mFlags                 = aRhs.mFlags;
      mStaticVarIndex        = aRhs.mStaticVarIndex;
      mMethodIndex           = aRhs.mMethodIndex;
      mStructVarName         = aRhs.mStructVarName;
      mValidAfterInstruction = aRhs.mValidAfterInstruction;
      mValNameId             = aRhs.mValNameId;
      mStructVarNameId       = aRhs.mStructVarNameId;
      return *this;
   }

   ~UsVal() { delete mValue; }

   static UsVal GlobalVar(const std::string& aName, Ty* aTypePtr)
   {
      UsVal v;
      v.mFlags |= cGLOBAL_VARIABLE;
      v.mType    = aTypePtr;
      v.mValName = aName;
      return v;
   }

   static UsVal Script(const std::string& aName, UtStringId aNameId, const Fn* aFunc)
   {
      UsVal v;
      v.mValName      = aName;
      v.mValNameId    = aNameId;
      v.mPrototypePtr = aFunc;
      return v;
   }

   static UsVal Temporary(Ty* aTypePtr, ptrdiff_t aStackIndex = -1)
   {
      UsVal v;
      v.mType = aTypePtr;
      v.mFlags |= cTEMPORARY_VARIABLE;
      v.mStackIndex = aStackIndex;
      return v;
   }

   static UsVal LocalVariable(const std::string& aName, Ty* aTypePtr, ptrdiff_t aStackIndex = -1)
   {
      UsVal v;
      v.mValName = aName;
      v.mType    = aTypePtr;
      v.mFlags |= cLOCAL_VARIABLE;
      v.mStackIndex = aStackIndex;
      return v;
   }

   static UsVal StaticVariable(const std::string& aName, Ty* aTypePtr, offset_t aStaticIndex)
   {
      UsVal v;
      v.mValName = aName;
      v.mType    = aTypePtr;
      v.mFlags |= cSTATIC_VARIABLE;
      v.mStaticVarIndex = aStaticIndex;
      return v;
   }

   static UsVal Method(const UsVal& aObject, UtStringId aMethodName)
   {
      UsVal v         = aObject;
      v.mMethodNameId = aMethodName;
      return v;
   }

   static UsVal Value(const UtScriptData& aValue, Ty* aTypePtr)
   {
      UsVal v;
      v.mValue = new UtScriptData(aValue);
      v.mType  = aTypePtr;
      return v;
   }

   UsVal GetMethodBase()
   {
      UsVal v = *this;
      v.mMethodNameId.Clear();
      return v;
   }

   ptrdiff_t GetStackIndex() const
   {
      // NOTE: As long as the variable is marked as being a struct then it does not have a stack index.
      //       When ToStack loads allocates a stack slot then it clears the name.
      return (mStructVarName < 0) ? mStackIndex : -1;
   }

   offset_t GetMethodIndex() const { return mMethodIndex; }

   bool IsAppMethod() const { return (!mMethodNameId.IsNull()) && (mPrototypePtr == nullptr) && (mType != nullptr); }

   bool IsScript() const { return !mValName.empty() && (mPrototypePtr != nullptr); }

   bool IsVariable() const
   {
      return 0 != (mFlags & (cLOCAL_VARIABLE | cSTATIC_VARIABLE | cGLOBAL_VARIABLE | cTEMPORARY_VARIABLE));
   }

   //! Return true if this does not represent any value
   bool IsValid() { return !(mFlags == 0 && mType == nullptr && mValName.empty() && !mInitList.IsValid()); }

   Ty*         Type(const UsConstCtx& aCtx) const;
   std::string TypeName(const UsConstCtx& aCtx);

   // return mValName as a UtStringId
   UtStringId GetValNameId() const
   {
      if (mValNameId.Empty())
      {
         mValNameId = mValName;
      }
      return mValNameId;
   }

   // return mStructVarName as a UtStringId
   UtStringId GetStructVarNameId(const std::vector<std::string>& aNameList) const
   {
      if (mStructVarNameId.Empty())
      {
         mStructVarNameId = aNameList[static_cast<size_t>(mStructVarName)];
      }
      return mStructVarNameId;
   }


   // The type of value.  May be null.
   // In the case of an App method, this is the type of the class containing the method
   Ty* mType{};
   // If this value is an initializer list, this points to it
   UtReferencePtr<UsInitializerList> mInitList;
   // If this is a script, this is the script name
   // If this is a global variable, this is the global name
   std::string mValName{};
   // If this is a method, this is the method name.
   UtStringId mMethodNameId{};
   // If this is a literal value, mValue is non-null
   UtScriptData* mValue{nullptr};
   // If this is a reference to a script, this is the prototype
   const Fn* mPrototypePtr{nullptr};

   // If this is a nameless stack variable, this is the index.  -1 if not used
   ptrdiff_t mStackIndex{-1};

   int mFlags{0};
   // For a cSTATIC_VARIABLE, this is index of the static variable.
   offset_t mStaticVarIndex{ut::script::npos};
   // If this is a class method, this is the index of the method within the script class.
   offset_t mMethodIndex{ut::script::npos};
   // If this represents a variable within a struct then this is the name ID of the variable.
   // NOTE: This is also used for the method name when using the old '->' syntax.
   int mStructVarName{-1};
   // For cLOCAL_VARIABLE or cTEMPORARY_VARIABLE, this
   offset_t mValidAfterInstruction{ut::script::npos};

   // cached conversion of mValName to a UtStringId
   mutable UtStringId mValNameId;
   // cached conversion of mStructVarName to a UtStringId
   mutable UtStringId mStructVarNameId;
};

// The data inside an initializer list.  Required to be separate due to order of definition
struct UsInitializerListData
{
   struct Entry
   {
      UsVal key;
      UsVal val;
   };
   std::vector<Entry> mEntries;
};

using UsValList = std::vector<UsVal>;

// Contains script information that is shared by all contexts
class UsConstCtx
{
public:
   UtScriptLanguage::Parser* mParser;
   UtScriptTypes*            mTypes;
   UtScriptEnvironment*      mEnv;

   UsConstCtx();
   ~UsConstCtx();

   UsVal ValFromType(Ty* aType)
   {
      UsVal v;
      v.mType = aType;
      return v;
   }

   void Reset();

   UsCtx*                     mTopCtx;
   Ty*                        tyNull;
   Ty*                        tyVoid;
   Ty*                        tyVar;
   Ty*                        tyBool;
   Ty*                        tyInt;
   Ty*                        tyDouble;
   Ty*                        tyString;
   Ty*                        tyObject;
   bool                       mGlobalVarMode;
   UtScriptScope*             mScopePtr;
   UtScriptContext*           mInstancePtr;
   std::string                mNamespace;
   UtScript*                  mScriptPtr;
   UsVal                      mScriptVal;
   std::map<std::string, int> mNameListMap;
   int                        mNextNameId;

   // UtTextDocument*            mDocumentPtr;
   std::string mDocumentPath;
   size_t      mDocumentOffset;
   size_t      mDocumentLineOffset;
};

// Represents a script context.  This is the primary interface used by the parser.
class UsCtx
{
public:
   enum ContextFlags
   {
      cTEMPORARY_SYMBOL_TABLE = 1
   };

   enum BlockType
   {
      cSCRIPT_BLOCK,
      cNORMAL_BLOCK,
      cWHILE_BLOCK,
      cDO_BLOCK,
      cFOR_BLOCK,
      cFOREACH_BLOCK
   };
   // Parent context.  This context is syntactically nested in the parent.
   UsCtx* mParentCtx;
   // Pointer to data shared by all contexts
   UsConstCtx* mConst;
   // True if the block has been completed prior to the destructor being called
   bool mBlockEnded;
   // True if the block has returned.
   bool mEmptyBlock;
   bool mBlockHasReturned;
   bool mAllCodePathsReturn;
   // The next available index for a stack variable if mFreeStackSlots is empty
   int mNextAutomaticVarIndex;
   // The next available index for a static variable
   offset_t mNextStaticVarIndex;
   // When a script is completed successfully, its pointer is stored here.
   UtScript* mRegisteredScriptPtr;
   BlockType mBlockType;
   // Location to which 'continue' statements will jump
   offset_t mContinueJumpOffset;
   // These values are used to backfill jump locations for various constructs.
   offset_t mWhileStartInstruction;
   offset_t mForCondOffset;
   offset_t mForCondJump;
   offset_t mStaticVariableJump;

   bool mIfBlockParent;
   bool mIfBlock;

   // Instruction offset where break jump locations need to be filled in
   std::vector<offset_t> mBreakJumps;
   // Instruction offset where continue jump locations need to be filled in
   std::vector<offset_t> mContinueJumps;
   // If this is the script block, this is the list of arg names
   std::vector<std::string> mArgNames;

   // Currently, temporaries are given a stack index, and stored here.
   // When the statement is complete, all associated stack indices for temporaries are
   // cleaned up for use in subsequent statements.
   // TempSlots are actually a stack of arrays slots for each statement, where the arrays
   // for each statement are preceeded by the negative value of the statement id.
   // Temp slots to be freed after the current statement ends.
   std::vector<ptrdiff_t> mCurTempSlots;
   // Temp slots to be freed after the parent statement ends.
   std::vector<ptrdiff_t> mParentTempSlots;
   // Set of free stack slots below mNextAutomaticVarIndex
   std::vector<ptrdiff_t> mFreeStackSlots;

   // Stack of current statement ID's
   // Used in conjunction with mStatementTemporaries
   std::vector<int> mCurrentStatements;
   // named variables local to this block are stored here.
   std::map<std::string, UsVal> mLocalSymbols;

   explicit UsCtx(UsConstCtx* c);
   UsCtx(UsCtx& p);

   ~UsCtx();
   void EndContext();

   Ty* qType(Token* t);
   Ty* Type(Token* t);
   Ty* qType(Ty* aTypePtr, std::pair<Ty*, Ty*> aArgs);

   void AddParam(Fn& fn, Ty* type, Token* aIdent, bool aIsFormalParam = false);

   void AddImplicitParam(Ty* type, const std::string& aIdent);

   UsVal Identifier(Token* t);

   void SetRType(Fn& fn, Ty* type);
   void FuncDefStart(UsVal& aScriptVal);

   UsVal AddEntry(Token* aIdent, Fn& aFunc, bool aExtern, UsCtx* aFnCtx = nullptr);
   UsVal AddEntry(const std::string& aIdent, int aIdentPos, Fn& aFunc, bool aExtern, UsCtx* aFnCtx = nullptr);

   UsVal Search(const std::string& aName);

   UsVal       Search(Token* aIdent);
   Ty*         FuncRType();
   std::string Text(Token* t);
   enum OperationId
   {
      cASSIGN,
      cASSIGN_INITIAL,
      cASSIGN_PLUS,
      cASSIGN_MINUS,
      cASSIGN_TIMES,
      cASSIGN_DIV,
      cADD,
      cSUBTRACT,
      cMULTIPLY,
      cDIVIDE,
      cOR,
      cAND,
      cEQ,
      cNE,
      cLT,
      cGT,
      cLE,
      cGE,
      cUNARY_MINUS,
      cUNARY_PLUS,
      cINDEX,
      cNOT
   };
   UsVal Op(UsVal& v, int op);
   UsVal Ctor(Ty* aType, const UsValList& aArgs);
   UsVal Op(UsVal& lhs, UsVal& idx, UsVal& rhs, int op);
   UsVal Op(UsVal& lhs, UsVal& rhs, int op);
   // Returns a map's key type if the type is a map
   Ty* GetContainerKey(Ty* aType);
   // Returns the type of value a container holds
   Ty* GetContainerValue(Ty* aType);
   // { ...    p1 : p2 , ... }
   void  AddToInitializerList(UsVal& initList, UsVal& p1, UsVal* p2); // UsVal& val, UsVal* key);
   UsVal Cast(UsVal& v, Ty* ty);
   UsVal StringLiteral(Token* t);
   UsVal DoubleLiteral(Token* t);
   UsVal IntLiteral(Token* t);
   UsVal BoolLiteral(Token* t);
   UsVal NullLiteral(Token* t);
   UsVal NewInitializerList();
   UsVal RealizeContainer(UsInitializerList& aInitializer, Ty* aLhsType);
   UsVal ImplicitCast(UsVal v, Ty* ty);
   UsVal ExplicitCast(UsVal& v, Ty* ty);
   void  ExpectedType(UsVal& v, Ty* t);
   UsVal GetAttribute(Ty* ty, Token* t);
   UsVal GetDynAttribute(const UsVal& v, Token* t);
   UsVal GetClassMethod(UsVal& v, const char* ident);
   UsVal GetClassVariable(UsVal& v, const char* ident);
   Ty*   GetDataType(const UtScriptData& aData);
   UsVal VarDecl(Ty* ty, Token* t, int flags = 0);
   UsVal Call(UsVal& fn, UsValList args, UsVal* aReturnVal = nullptr);

   Ty* LookupMethodParameterType(const UsVal& fn, int aParameterIndex);

   void  VarDeclInit(UsVal& v);
   void  VarDeclPreAssign(UsVal& v);
   Ty*   BoolTy() { return mConst->tyBool; }
   Ty*   VoidTy() { return mConst->tyVoid; }
   Ty*   IntTy() { return mConst->tyInt; }
   void  AtDot(Ty* aTypePtr, int pos);
   void  AtDot(const UsVal& aVal, int pos);
   void  HitEOF();
   UsVal AndComplete(UsVal orResult, UsVal rhs, int aJumpOffset);
   void  WhileCond(UsVal& v);
   void  WhileEnd(int aEnd);
   int   OrCheck(UsVal& aLhs, UsVal& aOrResult);
   UsVal OrComplete(UsVal orResult, UsVal rhs, int aJumpOffset);

   int      AndCheck(UsVal& aLhs, UsVal& aOrResult);
   int      IfCond(UsVal& v);
   int      IfTrueEnd(int falseTarget, bool aHasElse);
   void     IfElseEnd(int aElseEnd);
   void     SetBlockType(BlockType aBlockType);
   void     Break();
   void     Continue();
   void     DoWhileStart();
   void     DoCond(UsVal& v);
   void     ForCond(UsVal& v);
   void     ForInc();
   void     ForEnd();
   void     ForEachBegin(UsVal& key, UsVal& val, UsVal& container);
   void     Return(UsVal& v);
   void     FuncComplete(UsVal aFnVal, UsCtx& aFnCtx);
   offset_t CodeOffset();
   UsCtx*   ScriptContext();
   int      BeginStat();
   void     EndStat(int aStatNum);

   void SemErr(const char* aMsg);

   UsVal CreateVal(Ty* aClassPtr, bool aCallCreate = true);

   ptrdiff_t StackAlloc(bool aTempSlot = true);
   void      FreeIfTemporary(const UsVal& v);
   UsVal     ToStack(const UsVal& v, bool aDirectLoadData = false);
   UsVal     TempStack(const UsVal& v, bool aDirectLoadData = false);
   UsVal     CreateData(bool aData);
   UsVal     CreateData(int aData);
   UsVal     CreateData(double aData);
   UsVal     CreateData(const std::string& aData);

   int StringId(const std::string& aStr);

   UsVal CastToBasicType(UsVal& v, Ty* ty);

   script_code_t DataIndex(const UtScriptData& aData);

   Ty* GetReturnClass(Ty::InterfaceMethod* aMethodPtr);

   UtScript& Instr();
   UtScript& Code() const { return *mConst->mScriptPtr; }

   int VarRef(const std::string& aName);
   int NameId(const std::string& aName);

   UsCtx* GetBreakableBlock();

   UtScriptScope&       Scope() const { return *mConst->mScopePtr; }
   UtScriptContext&     Instance() const { return *mConst->mInstancePtr; }
   UtScriptTypes*       Types() const { return mConst->mTypes; }
   UtScriptEnvironment* Env() const { return mConst->mEnv; }
   Ty*                  ValType(const UsVal& aVal) const { return aVal.Type(*mConst); }

   UtScript& TagStoreLoc();
   //! Indicate index into code that contains the stack index of the most recent write.
   size_t mLastStoreCodeLoc = ut::script::npos;
   //! Indicates a stack index that is forbiden to have the mLastStoreCodeLoc optimization recycle into.
   ptrdiff_t mNoRecycleLastIdx = static_cast<ptrdiff_t>(ut::script::npos);

private:
   void InitP();
};

// Inline unused functions here
inline void UsCtx::HitEOF() {}
inline void UsCtx::AtDot(Ty* aTypePtr, int pos) {}
inline void UsCtx::AtDot(const UsVal& aVal, int pos) {}
} // namespace UtScriptLanguage

#endif
