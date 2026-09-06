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

#ifndef WSFPARSERULE_HPP
#define WSFPARSERULE_HPP

#include "wsf_parser_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtReferenceCount.hpp"
#include "UtTextDocument.hpp"
#include "WsfParseAction.hpp"
#include "WsfParseGrammar.hpp"

class WsfParseAction;
class WsfParseActionSub;
class WsfParseAlternate;
class WsfParseAttribute;
class WsfParseLiteral;
class WsfParseNamedRule;
class WsfParseNode;
class WsfParseRecurrence;
class WsfParseRule;
class WsfParseRuleDictionary;
class WsfParseRuleMemory;
class WsfParseRuleReference;
class WsfParseSequence;
class WsfParseStruct;
class WsfParser;

namespace std
{
void swap(WsfParseRule& aLhs, WsfParseRule& aRhs);
void swap(WsfParseAlternate& aLhs, WsfParseAlternate& aRhs);
void swap(WsfParseAttribute& aLhs, WsfParseAttribute& aRhs);
void swap(WsfParseNamedRule& aLhs, WsfParseNamedRule& aRhs);
void swap(WsfParseRuleReference& aLhs, WsfParseRuleReference& aRhs);
void swap(WsfParseSequence& aLhs, WsfParseSequence& aRhs);
void swap(WsfParseStruct& aLhs, WsfParseStruct& aRhs);
void swap(WsfParseAction& aLhs, WsfParseAction& aRhs);
} // namespace std

class WSF_PARSER_EXPORT WsfParseValueType
{
public:
   enum TypeKind
   {
      cSTRUCT,
      cVALUE,
      cLIST,
      cOBJECT_MAP
   };

   WsfParseValueType()
      : mKind(cVALUE)
      , mStructPtr(nullptr)
      , mValuePtr(nullptr)
      , mTemplateType(nullptr)
   {
   }
   WsfParseValueType(WsfParseValue* aValuePtr);
   WsfParseValueType(WsfParseStruct* aStructPtr);
   static WsfParseValueType* List(WsfParseValueType* aTemplatedType)
   {
      WsfParseValueType* v = new WsfParseValueType;
      v->mTemplateType     = aTemplatedType;
      v->mKind             = cLIST;
      v->mTypeName         = "List/";
      v->mTypeName += aTemplatedType->GetTypeName();
      return v;
   }
   static WsfParseValueType* ObjectMap(WsfParseValueType* aTemplatedType)
   {
      WsfParseValueType* v = new WsfParseValueType;
      v->mTemplateType     = aTemplatedType;
      v->mKind             = cOBJECT_MAP;
      v->mTypeName         = "ObjectMap/";
      v->mTypeName += aTemplatedType->GetTypeName();
      return v;
   }

   //! Determines whether the given type name is a valid List template
   //! @retval false if aTypeName is empty
   //! @retval true if aTypeName starts with "List"
   static bool IsValidListTemplate(const std::string& aTypeName) noexcept;

   //! Determines whether the given type name is a valid ObjectMap template
   //! @retval false if aTypeName is empty
   //! @retval true if aTypeName starts with "ObjectMap"
   static bool IsValidObjectMapTemplate(const std::string& aTypeName) noexcept;

   //! Determines whether the given type name is a valid ObjectMap template
   //! @retval false if aTypeName is empty
   //! @retval true if aTypeName starts with "List" or "ObjectMap"
   static bool IsValidTemplate(const std::string& aTypeName) noexcept;

   //! Get the contained type name from the template type name
   //! @retval an empty string if aTypeName is an invalid template
   static std::string GetContainedTypeName(const std::string& aTypeName) noexcept;

   std::string& GetTypeName() { return mTypeName; }

   WsfParseNamedRule* GetTypeRule();

   TypeKind           mKind;
   std::string        mTypeName;
   WsfParseStruct*    mStructPtr;
   WsfParseValue*     mValuePtr;
   WsfParseValueType* mTemplateType;
   // std::vector<WsfParseValue*>   mValueRules;
};

//! The base class for all parse rules.
//! A parse rule defines rules for matching a piece of wsf core text.
//! Parse rules are created while processing the wsf core grammar files.
class WSF_PARSER_EXPORT WsfParseRule
{
public:
   enum RuleType
   {
      cNAMED_RULE,
      cVALUE,
      cSTRUCT,
      cSEQUENCE,
      cALTERNATE,
      cRECURRENCE,
      cRULE_REFERENCE,
      cREAL,
      cINT,
      cSTRING,
      cLINE_STRING,
      cQUOTED_STRING,
      cLITERAL,
      cDELIMITED,
      cTYPE_LOAD,
      cTYPE_COMMAND,
      cPASSTHROUGH,
      cSCRIPT_BLOCK
   };

   enum ReaderFlags
   {
      // Indicates the Resolve() function should be called at the completion of the sequence
      cCAN_RESOLVE = 1,
      // Indicates the reader makes no change that can't be undone (disallow backtracking)
      cCAN_UNDO = 2,
      // Indicates this reader always results in matching in a single token
      cIS_SINGLE_TOKEN = 4,
      // Indicates the rule simply passes processing to a child ( bypassing creating another node )
      // If this is set, the created node should point to this reader, and the GetPassthrough() method should be implemented
      cIS_PASSTHROUGH = 8,
      // Indicates the rule is a '(value xyz)', or a neste rule in a value rule which should
      // be treated as a value rule
      cIS_VALUE_RULE = 0x10
   };
   //! The parse rule constructor requires a 'WsfParseRuleMemory' object to keep track of parse rules in memory
   //! and a rule type.
   WsfParseRule(WsfParseRuleMemory& aMem, RuleType aType);
   virtual ~WsfParseRule()           = default;
   WsfParseRule(const WsfParseRule&) = default; // copy constructor

   void Swap(WsfParseRule& aRhs)
   {
      std::swap(mRuleType, aRhs.mRuleType);
      std::swap(mReaderFlags, aRhs.mReaderFlags);
      std::swap(mUserId, aRhs.mUserId);
      std::swap(mParentRulePtr, aRhs.mParentRulePtr);
      std::swap(mSubordinateRulePtr, aRhs.mSubordinateRulePtr);
   }

   //! Returns an enumeration specifying the derived type of the reader
   RuleType Type() const { return mRuleType; }

   //! Override to parse and generate a parse node.  Parse nodes should be
   //! generated even on error conditions if possible.
   //! @param aParser The parser.
   //! @param aNode   Set to the resulting parse node.
   //! @return 'true' if a parse node was generated.
   virtual bool Read(WsfParser& aParser, WsfParseNode*& aNode) = 0;

   //! Override to parse and generate a parse node given a single block of text.
   //! This method must be implemented to take part in delimited expressions.
   virtual bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
   {
      return false;
   }

   //! Called by a command reader if the reader just read in a value that has now been rejected due to
   //! a later failure.  You must set cCAN_UNDO in the reader flags to receive this call.
   virtual void Undo(WsfParser& aParser) {}

   //! Called by a command reader if the full command is successfully read.
   //! You must set cCAN_RESOLVE in the reader flags to receive this call.
   virtual void Resolve(WsfParser& aParser, WsfParseNode* aNodePtr) {}

   virtual void Initialize(int aSequence);

   // These functions allow walking the rule structure without inspecting each rule type
   virtual std::vector<WsfParseRule*> GetSequence() { return std::vector<WsfParseRule*>(); }
   virtual std::vector<WsfParseRule*> GetAlternates() { return std::vector<WsfParseRule*>(); }
   virtual WsfParseRuleDictionary*    GetNestedRules() { return nullptr; }
   virtual WsfParseNamedRule*         GetContext() const { return nullptr; }
   virtual WsfParseRule*              GetPassthrough() { return nullptr; }

   virtual std::string GetRuleDescription() { return "<...>"; }

   //! If the rule is also a type, return the type.
   virtual WsfParseValueType* GetType() { return nullptr; }

   WsfParseStruct* GetContainingStruct() const;
   WsfParseStruct* GetStruct();

   WsfParseNamedRule* GetEntryType();
   WsfParseNamedRule* GetInputType();
   WsfParseNamedRule* GetOutputType();

   WsfParseNamedRule* GetBestContext();
   WsfParseNamedRule* GetVarContext();
   bool               IsNamedRule() const
   {
      switch (mRuleType)
      {
      case cNAMED_RULE:
      case cVALUE:
      case cSTRUCT:
         return true;
      default:
         return false;
      }
   }

   int GetReaderFlags() const { return mReaderFlags; }

   RuleType mRuleType;
   int      mReaderFlags;
   int      mUserId;

   //! Pointer to the parent rule.  This should be valid for unnamed rules
   WsfParseRule* mParentRulePtr;
   WsfParseRule* GetSubordinateRule() const { return mSubordinateRulePtr; }
   void          SetSubordinateRule(WsfParseRule* aRulePtr) { mSubordinateRulePtr = aRulePtr; }

protected:
   WsfParseRule* mSubordinateRulePtr;
};

// Keeps a list of parse rules for deletion later
class WSF_PARSER_EXPORT WsfParseRuleMemory
{
public:
   WsfParseRuleMemory() = default;
   ~WsfParseRuleMemory();
   WsfParseRuleMemory(const WsfParseRuleMemory&) = default; // copy constructor

   void RuleAdded(WsfParseRule* aRulePtr) { mRules.push_back(aRulePtr); }

   std::vector<WsfParseValueType*> CollectTypes();
   std::vector<WsfParseRule*>      mRules;
};

namespace std
{
inline void swap(WsfParseRule& aLhs, WsfParseRule& aRhs)
{
   aLhs.Swap(aRhs);
}
} // namespace std

class WSF_PARSER_EXPORT WsfParsePassthrough : public WsfParseRule
{
public:
   WsfParsePassthrough(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cPASSTHROUGH)
      , mKind(cNO_CHANGE)
   {
      mReaderFlags |= cIS_PASSTHROUGH;
   }
   enum Kind
   {
      cERROR_RULE,
      cNO_CHANGE
   };

   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
   WsfParseRule*              GetPassthrough() override { return mSubordinateRulePtr; }
   void                       ProcessNode(WsfParseNode& aNode);
   void                       SetPassthroughRule(WsfParseRule* aRulePtr);
   std::vector<WsfParseRule*> GetSequence() override { return std::vector<WsfParseRule*>(1, mSubordinateRulePtr); }
   Kind                       mKind;
};

//! Matches a string of text with no whitespace.
class WSF_PARSER_EXPORT WsfParseLiteral : public WsfParseRule
{
public:
   WsfParseLiteral(WsfParseRuleMemory& aMem);
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;

   bool MatchesString(const std::string& aString);

   std::string GetRuleDescription() override { return mText; }

   bool         mCaseSensitive;
   bool         mIsTerminator;
   std::string  mText;
   unsigned int mNodeFlags;
};

namespace std
{
inline void swap(WsfParseLiteral& aLhs, WsfParseLiteral& aRhs)
{
   swap((WsfParseRule&)aLhs, (WsfParseRule&)aRhs);
   swap(aLhs.mCaseSensitive, aRhs.mCaseSensitive);
   swap(aLhs.mIsTerminator, aRhs.mIsTerminator);
   swap(aLhs.mText, aRhs.mText);
}
} // namespace std
class WsfParseValue;

class WSF_PARSER_EXPORT WsfParseRuleDictionary
{
public:
   ~WsfParseRuleDictionary();
   WsfParseNamedRule* Find(const std::string& aRule);
   void               Add(WsfParseNamedRule* aRule);
   using NameRuleMap = std::map<std::string, WsfParseNamedRule*>;
   NameRuleMap mNameToRule;
};


// A rule definition
class WSF_PARSER_EXPORT WsfParseNamedRule : public WsfParseRule
{
public:
   WsfParseNamedRule(WsfParseRuleMemory& aMem);
   WsfParseNamedRule(WsfParseRuleMemory& aMem, const std::string& aName, WsfParseRule* aRulePtr);
   ~WsfParseNamedRule() override;

   bool InitializeInputsOutputs();
   void Initialize(int aSequence) override;

   WsfParseStruct* FindStruct(const std::string& aRuleName);

   WsfParseNamedRule* Find(const std::string& aRuleName);

   // Find a nested named rule.  Do not look on parent
   WsfParseNamedRule* FindImmediate(const std::string& aRuleName);

   WsfParseValue* FindValue(const std::string& aTypeName);

   bool HasNestedRules() const { return mNestedRules != nullptr; }

   WsfParseRuleDictionary* GetNestedRules() override { return mNestedRules; }

   WsfParseRuleDictionary* GetNested();

   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;

   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;


   void SetDefinition(WsfParseRule* aRulePtr);

   std::string GetFullName() const;

   WsfParseRule*              GetDefinition() const { return mSubordinateRulePtr; }
   WsfParseRule*&             GetDefinition() { return mSubordinateRulePtr; }
   std::vector<WsfParseRule*> GetSequence() override { return std::vector<WsfParseRule*>(1, mSubordinateRulePtr); }
   WsfParseNamedRule*         GetContext() const override { return mContextParent; }
   WsfParseRule*              GetPassthrough() override { return mSubordinateRulePtr; }
   std::string                GetRuleDescription() override { return "<" + GetFullName() + ">"; }
   WsfParseValue*             GetRuleValue();
   bool                       IsPartValueRule() { return 0 != (mReaderFlags & cIS_VALUE_RULE); }

   void Swap(WsfParseNamedRule& aRhs)
   {
      std::swap((WsfParseRule&)*this, (WsfParseRule&)aRhs);
      std::swap(mName, aRhs.mName);
      std::swap(mNestedRules, aRhs.mNestedRules);
      std::swap(mContextParent, aRhs.mContextParent);
      std::swap(mInputTypeName, aRhs.mInputTypeName);
      std::swap(mInputTypePtr, aRhs.mInputTypePtr);
      std::swap(mOutputTypeName, aRhs.mOutputTypeName);
      std::swap(mOutputTypePtr, aRhs.mOutputTypePtr);
      std::swap(mReferencingRules, aRhs.mReferencingRules);
   }

   // WsfParseNamedRule*      mParentRule;
   //! The rule inside of which this rule is nested
   WsfParseNamedRule* mContextParent;
   std::string        mName;
   std::string        mInputTypeName;
   WsfParseNamedRule* mInputTypePtr;
   std::string        mOutputTypeName;
   WsfParseNamedRule* mOutputTypePtr;

   // List of rules which directly link to this rule
   std::vector<WsfParseRule*> mReferencingRules;

protected:
   WsfParseRuleDictionary* mNestedRules;
};

namespace std
{
inline void swap(WsfParseNamedRule& aLhs, WsfParseNamedRule& aRhs)
{
   aLhs.Swap(aRhs);
}
} // namespace std

// A reference to a rule
class WSF_PARSER_EXPORT WsfParseRuleReference : public WsfParseRule
{
public:
   WsfParseRuleReference(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cRULE_REFERENCE)
      , mRuleName()
   {
      mReaderFlags |= cIS_PASSTHROUGH;
   }
   ~WsfParseRuleReference() override;

   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
   void SetReferencedRule(WsfParseRule* aRulePtr)
   {
      if (aRulePtr)
      {
         mUserId = aRulePtr->mUserId;
      }
      SetSubordinateRule(aRulePtr);
   }
   std::vector<WsfParseRule*> GetSequence() override { return std::vector<WsfParseRule*>(1, GetSubordinateRule()); }
   WsfParseNamedRule*         GetRulePtr() { return dynamic_cast<WsfParseNamedRule*>(GetSubordinateRule()); }
   WsfParseRule*              GetPassthrough() override { return GetSubordinateRule(); }
   std::string                GetRuleDescription() override { return "<" + mRuleName + ">"; }

   std::string mRuleName;
   // This is a reference, not an ownership
   // WsfParseNamedRule*   mRulePtr;
};

//! An attribute which belongs to a WsfParseStruct
class WSF_PARSER_EXPORT WsfParseAttribute
{
public:
   enum Flags
   {
      cIS_FIXED = 1
   };

   WsfParseAttribute()
      : mFlags(0)
      , mName()
      , mType()
      , mDefault()
      , mTypePtr(nullptr)
      , mHasDefaultValue(false)
      , mIsPointer(false)
   {
   }

   unsigned int       mFlags;
   std::string        mName;
   std::string        mType;
   std::string        mDefault;
   WsfParseValueType* mTypePtr;
   bool               mHasDefaultValue;
   bool               mIsPointer;
};


class WSF_PARSER_EXPORT WsfParseValue : public WsfParseNamedRule
{
public:
   WsfParseValue(WsfParseRuleMemory& aMem, const std::string& aName);
   void Initialize(int aSequence) override;
   ~WsfParseValue() override = default;
   WsfParseValueType* GetType() override;

   WsfParseValueType* mTypePtr;
   bool               mIsEnumeration;
};


class WSF_PARSER_EXPORT WsfParseEnumerationValues
{
public:
   using NameValue = std::pair<std::string, int>;
   std::vector<NameValue> mNameValues;
};


class WSF_PARSER_EXPORT WsfParseEnumeration : public WsfParseValue
{
public:
   WsfParseEnumeration(WsfParseRuleMemory& aMem, const std::string& aName)
      : WsfParseValue(aMem, aName)
      , mValues()
   {
      mRuleType      = cVALUE;
      mIsEnumeration = true;
   }
   WsfParseEnumerationValues mValues;
};


class WSF_PARSER_EXPORT WsfParseStruct : public WsfParseNamedRule
{
public:
   struct ScriptVariable
   {
      bool        mIsThisType;
      std::string mTypeName;
      std::string mVariableName;
   };

   WsfParseStruct(WsfParseRuleMemory& aMem, const std::string& aRuleName);
   ~WsfParseStruct() override;
   WsfParseAttribute* FindAttribute(const std::string& aName);
   size_t             FindAttributeIndex(const std::string& aName);
   void               Initialize(int aSequence) override;

   WsfParseAttribute& GetAttribute(size_t aIndex)
   {
      if (aIndex >= mBaseAttributeCount)
      {
         return mAttributes[aIndex - mBaseAttributeCount];
      }
      return mBasePtr->GetAttribute(aIndex);
   }

   size_t          GetAttributeCount();
   void            SetBase(WsfParseStruct* aBasePtr);
   WsfParseStruct* GetBase() { return mBasePtr; }
   bool            IsType(WsfParseStruct* aBasePtr)
   {
      if (aBasePtr == this)
      {
         return true;
      }
      if (!mBasePtr)
      {
         return false;
      }
      return (mBasePtr == aBasePtr || mBasePtr->IsType(aBasePtr));
   }
   WsfParseValueType* GetType() override;

   std::vector<ScriptVariable> GetAllScriptVariables();

   size_t                         mBaseAttributeCount;
   std::vector<WsfParseAttribute> mAttributes;
   std::string                    mBaseName;
   std::vector<ScriptVariable>    mScriptVariables;

   // Set of actions specified with the ' (initially ...) ' command
   std::vector<WsfParseAction*> mInitialActions;

protected:
   WsfParseStruct*    mBasePtr;
   WsfParseValueType* mTypePtr;
};

#endif
