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

#ifndef WSFPARSEACTION_HPP
#define WSFPARSEACTION_HPP

#include "wsf_parser_export.h"

#include <string>
#include <vector>

#include "UtCast.hpp"

class WsfParseStruct;
class WsfParseValue;
class WsfParseValueType;

// Stores an address for accessing a value in the proxy.
// struct members are stored as integer indexes
// object map keys are stored as C strings.
class WsfParseActionAddress
{
public:
   enum class EntryType
   {
      cSTRING,
      cROOT,
      cNODE,
      cATTRIBUTE
   };

   class Entry
   {
   public:
      Entry()
         : Entry(EntryType::cATTRIBUTE)
      {
      }
      Entry(EntryType aType, size_t aIndex = ut::npos, const char* aName = nullptr)
         : mType(aType)
         , mIndex(aIndex)
         , mName(aName)
      {
      }
      Entry(const char* aName)
         : Entry(EntryType::cSTRING, ut::npos, aName)
      {
      }

      bool IsNodeIndex() const { return mType == EntryType::cNODE; }
      bool IsAttributeIndex() const { return mType == EntryType::cATTRIBUTE; }
      bool IsString() const { return mType == EntryType::cSTRING; }

      size_t GetNodeIndex() const { return mIndex; }

      EntryType mType;
      size_t    mIndex;

      // This string's memory is handled by WsfParseDefinitions
      const char* mName;
   };

   WsfParseActionAddress& operator+=(const WsfParseActionAddress& aRhs)
   {
      mAttrIndices.insert(mAttrIndices.end(), aRhs.mAttrIndices.begin(), aRhs.mAttrIndices.end());
#ifdef WSF_PARSE_DEBUG
      if (!mDEBUG_Addr.empty())
      {
         mDEBUG_Addr += '.';
      }
      mDEBUG_Addr += aRhs.mDEBUG_Addr;
#endif
      return *this;
   }
   WsfParseActionAddress& operator+=(const Entry& aEntry)
   {
      mAttrIndices.push_back(aEntry);
      return *this;
   }
   bool               IsEmpty() const { return mAttrIndices.empty(); }
   std::vector<Entry> mAttrIndices;
#ifdef WSF_PARSE_DEBUG
   std::string mDEBUG_Addr;
#endif
};

class WsfParseActionPart
{
public:
   enum ActionPartType
   {
      cASSIGN,
      // cGET_RETURN,
      cLIST_OP,
      cOBJECT_MAP_OP,
      cLITERAL,
      cNODE_REFERENCE,
      cNEGATE,
      cPUSH,
      cDEBUG_PRINT,
      cSKIP,
      cCOPY,
      cCALL
   };
   WsfParseActionPart(ActionPartType aType)
      : mActionPartType(aType)
      , mRHS_Type(nullptr)
   {
   }
   virtual ~WsfParseActionPart();
   ActionPartType     mActionPartType;
   WsfParseValueType* mRHS_Type;
};

class WsfParseActionFunction
{
public:
   std::vector<std::string>        mSignatureStrings;
   std::vector<WsfParseValueType*> mSignature;
   std::vector<std::string>        mArgumentNames;
};

class WsfParseActionAssign : public WsfParseActionPart
{
public:
   WsfParseActionAssign();
   ~WsfParseActionAssign() override;
   WsfParseActionAssign(const WsfParseActionAssign&) = default; // copy constructor

   std::string           mLHS_Name;
   WsfParseActionAddress mLHS_Address;
   WsfParseValueType*    mLHS_Type;
   WsfParseActionPart*   mRHS;
};

// TODO:
class WsfParseActionCopy : public WsfParseActionPart
{
public:
   WsfParseActionCopy()
      : WsfParseActionPart(cCOPY)
      , mDstName()
      , mDstAddr()
      , mDstType(nullptr)
      , mSrcName()
      , mSrcAddr()
   {
   }
   ~WsfParseActionCopy() override;
   WsfParseActionCopy(const WsfParseActionCopy&) = default; // copy constructor

   std::string mDstName;
   // Address of the attribute to copy
   WsfParseActionAddress mDstAddr;
   WsfParseValueType*    mDstType;
   std::string           mSrcName;
   WsfParseActionAddress mSrcAddr;
};

class WsfParseActionPush : public WsfParseActionPart
{
public:
   WsfParseActionPush(const std::string& aAttrName)
      : WsfParseActionPart(cPUSH)
      , mAttributeName(aAttrName)
   {
   }
   ~WsfParseActionPush() override                = default;
   WsfParseActionPush(const WsfParseActionPush&) = default; // copy constructor

   std::string mAttributeName;
   // Address of the attribute to push onto the stack
   WsfParseActionAddress mAttributeAddr;
};

class WsfParseActionPrint : public WsfParseActionPart
{
public:
   WsfParseActionPrint(const std::string& aString)
      : WsfParseActionPart(cDEBUG_PRINT)
      , mString(aString)
   {
   }
   ~WsfParseActionPrint() override                 = default;
   WsfParseActionPrint(const WsfParseActionPrint&) = default; // copy constructor

   std::string mString;
};

class WsfParseActionListOp : public WsfParseActionPart
{
public:
   WsfParseActionListOp()
      : WsfParseActionPart(cLIST_OP)
      , mOperation()
      , mAttributeName()
      , mAttributeAddr()
      , mPushedValuePtr(nullptr)
   {
   }
   ~WsfParseActionListOp() override;
   WsfParseActionListOp(const WsfParseActionListOp&) = default; // copy constructor

   enum ListOperation
   {
      cPUSH_BACK
   };

   ListOperation mOperation;
   std::string   mAttributeName;
   // Address of the list object
   WsfParseActionAddress mAttributeAddr;
   WsfParseActionPart*   mPushedValuePtr;
};

class WsfParseActionObjectMapOp : public WsfParseActionPart
{
public:
   WsfParseActionObjectMapOp();
   ~WsfParseActionObjectMapOp() override;
   WsfParseActionObjectMapOp(const WsfParseActionObjectMapOp&) = default; // copy constructor

   enum ObjectMapOperation
   {
      cGET,
      cNEW,
      // Same as cNEW when the item doesn't yet exist
      // otherwise, does cGET
      cMAP_SET,
      cDELETE,
      cAPPLY
   };

   ObjectMapOperation mOperation;
   std::string        mAttributeName;
   int                mApplyOrd;
   // Address of the map object
   WsfParseActionAddress mAttributeAddr;
   WsfParseValueType*    mObjectMapTypePtr;
   // Resolves to the name of the object in the map
   WsfParseActionPart* mObjectName;
};

class WsfParseActionLiteral : public WsfParseActionPart
{
public:
   WsfParseActionLiteral(const std::string& aText)
      : WsfParseActionPart(cLITERAL)
      , mText(aText)
   {
   }
   ~WsfParseActionLiteral() override                   = default;
   WsfParseActionLiteral(const WsfParseActionLiteral&) = default; // copy constructor

   std::string mText;
};

class WsfParseActionNodeReference : public WsfParseActionPart
{
public:
   WsfParseActionNodeReference()
      : WsfParseActionPart(cNODE_REFERENCE)
      , mIndex(ut::npos)
      , mValueTypePtr(nullptr)
   {
   }
   ~WsfParseActionNodeReference() override                         = default;
   WsfParseActionNodeReference(const WsfParseActionNodeReference&) = default; // copy constructor

   size_t             mIndex;
   WsfParseValueType* mValueTypePtr;
};

class WsfParseActionNegate : public WsfParseActionPart
{
public:
   WsfParseActionNegate()
      : WsfParseActionPart(cNEGATE)
      , mSubPart(nullptr)
   {
   }
   ~WsfParseActionNegate() override;
   WsfParseActionNegate(const WsfParseActionNegate&) = default; // copy constructor

   WsfParseActionPart* mSubPart;
};

class WsfParseActionSkip : public WsfParseActionPart
{
public:
   WsfParseActionSkip()
      : WsfParseActionPart(cSKIP)
   {
   }
   ~WsfParseActionSkip() override                = default;
   WsfParseActionSkip(const WsfParseActionSkip&) = default; // copy constructor
};

class WsfParseActionCall : public WsfParseActionPart
{
public:
   WsfParseActionCall()
      : WsfParseActionPart(cCALL)
      , mFunctionName()
      , mFunctionPtr(nullptr)
      , mArgs()
   {
   }
   ~WsfParseActionCall() override;
   WsfParseActionCall(const WsfParseActionCall&) = default; // copy constructor

   std::string                      mFunctionName;
   WsfParseActionFunction*          mFunctionPtr;
   std::vector<WsfParseActionPart*> mArgs;
};

class WSF_PARSER_EXPORT WsfParseAction
{
public:
   WsfParseAction()
      : mSubActions()
   {
   }
   WsfParseAction(WsfParseActionPart* aSub) { mSubActions.push_back(aSub); }
   ~WsfParseAction();
   WsfParseAction(const WsfParseAction&) = default; // copy constructor

   void AddPart(WsfParseActionPart* aPartPtr) { mSubActions.push_back(aPartPtr); }
   void Swap(WsfParseAction& aRhs) { std::swap(mSubActions, aRhs.mSubActions); }
   void Merge(WsfParseAction& aRhs)
   {
      mSubActions.insert(mSubActions.end(), aRhs.mSubActions.begin(), aRhs.mSubActions.end());
      aRhs.mSubActions.clear();
   }

   std::vector<WsfParseActionPart*> mSubActions;
};

#endif
