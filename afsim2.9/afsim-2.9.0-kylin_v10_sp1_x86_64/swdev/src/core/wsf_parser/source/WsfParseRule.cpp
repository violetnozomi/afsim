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

#include "WsfParseRule.hpp"

#include "WsfParseAlternate.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParser.hpp"

namespace std
{
void swap(WsfParseAttribute& aLhs, WsfParseAttribute& aRhs)
{
   swap(aLhs.mName, aRhs.mName);
   swap(aLhs.mType, aRhs.mType);
   swap(aLhs.mDefault, aRhs.mDefault);
}

void swap(WsfParseRuleReference& aLhs, WsfParseRuleReference& aRhs)
{
   swap(aLhs.mRuleName, aRhs.mRuleName);
   // swap(aLhs.mRulePtr, aRhs.mRulePtr);
}
void swap(WsfParseStruct& aLhs, WsfParseStruct& aRhs)
{
   swap(aLhs.mAttributes, aRhs.mAttributes);
}
} // namespace std

bool WsfParseValueType::IsValidListTemplate(const std::string& aTypeName) noexcept
{
   if (aTypeName.empty())
   {
      return false;
   }

   size_t slashIndex{aTypeName.find_first_of('/')};
   return aTypeName.substr(0, slashIndex) == "List";
}

bool WsfParseValueType::IsValidObjectMapTemplate(const std::string& aTypeName) noexcept
{
   if (aTypeName.empty())
   {
      return false;
   }

   size_t slashIndex{aTypeName.find_first_of('/')};
   return aTypeName.substr(0, slashIndex) == "ObjectMap";
}

bool WsfParseValueType::IsValidTemplate(const std::string& aTypeName) noexcept
{
   return IsValidListTemplate(aTypeName) || IsValidObjectMapTemplate(aTypeName);
}

std::string WsfParseValueType::GetContainedTypeName(const std::string& aTypeName) noexcept
{
   if (!IsValidTemplate(aTypeName))
   {
      return std::string{};
   }

   size_t      slashIndex{aTypeName.find_first_of('/')};
   std::string containedTypeName;
   if (slashIndex != aTypeName.size() - 1)
   {
      containedTypeName = aTypeName.substr(slashIndex + 1);
   }
   return containedTypeName;
}

WsfParseRule::WsfParseRule(WsfParseRuleMemory& aMem, RuleType aType)
   : mRuleType(aType)
   , mReaderFlags()
   , mUserId(0)
   , mParentRulePtr(nullptr)
   , mSubordinateRulePtr(nullptr)
{
   aMem.RuleAdded(this);
}

WsfParseStruct* WsfParseRule::GetStruct()
{
   if (mRuleType == WsfParseRule::cSTRUCT)
   {
      return dynamic_cast<WsfParseStruct*>(this);
   }
   return GetContainingStruct();
}

WsfParseNamedRule* WsfParseRule::GetEntryType()
{
   WsfParseNamedRule* entryType = GetInputType();
   ;
   if (!entryType)
   {
      entryType = GetOutputType();
   }
   return entryType;
}

WsfParseNamedRule* WsfParseRule::GetInputType()
{
   if (IsNamedRule())
   {
      return (dynamic_cast<WsfParseNamedRule*>(this))->mInputTypePtr;
   }
   return nullptr;
}

WsfParseNamedRule* WsfParseRule::GetOutputType()
{
   if (IsNamedRule())
   {
      return (dynamic_cast<WsfParseNamedRule*>(this))->mOutputTypePtr;
   }
   return nullptr;
}

WsfParseNamedRule* WsfParseRule::GetVarContext()
{
   if (mRuleType == WsfParseRule::cSTRUCT || mRuleType == WsfParseRule::cVALUE)
   {
      return dynamic_cast<WsfParseNamedRule*>(this);
   }
   if (mParentRulePtr)
   {
      return mParentRulePtr->GetVarContext();
   }
   if (mRuleType == WsfParseRule::cNAMED_RULE)
   {
      WsfParseNamedRule* ctxP = (dynamic_cast<WsfParseNamedRule*>(this))->mContextParent;
      return ctxP ? ctxP->GetVarContext() : nullptr;
   }
   return nullptr;
}

WsfParseStruct* WsfParseRule::GetContainingStruct() const
{
   WsfParseNamedRule* ctx = GetContext();
   if (ctx)
   {
      if (ctx->Type() == WsfParseRule::cSTRUCT)
      {
         return dynamic_cast<WsfParseStruct*>(ctx);
      }
      return ctx->GetContainingStruct();
   }
   else
   {
      if (mParentRulePtr)
      {
         if (mParentRulePtr->Type() == WsfParseRule::cSTRUCT)
         {
            return dynamic_cast<WsfParseStruct*>(mParentRulePtr);
         }
         return mParentRulePtr->GetContainingStruct();
      }
      return nullptr;
   }
}
WsfParseNamedRule* WsfParseRule::GetBestContext()
{
   switch (mRuleType)
   {
   case WsfParseRule::cNAMED_RULE:
   case WsfParseRule::cSTRUCT:
   case WsfParseRule::cVALUE:
      return dynamic_cast<WsfParseNamedRule*>(this);
   default:
      break;
   }
   if (mParentRulePtr)
   {
      return mParentRulePtr->GetBestContext();
   }
   return nullptr;
}

void WsfParseRule::Initialize(int aSequence)
{
   if (aSequence != 0)
   {
      return;
   }
   WsfParseRule* subPtr = GetSubordinateRule();
   if (subPtr && subPtr->IsNamedRule())
   {
      WsfParseNamedRule* nrp = dynamic_cast<WsfParseNamedRule*>(subPtr);
      nrp->mReferencingRules.push_back(this);
   }
}

void WsfParseRuleDictionary::Add(WsfParseNamedRule* aRule)
{
   mNameToRule[aRule->mName] = aRule;
}

WsfParseNamedRule* WsfParseRuleDictionary::Find(const std::string& aRule)
{
   std::map<std::string, WsfParseNamedRule*>::iterator i = mNameToRule.find(aRule);
   if (i != mNameToRule.end())
   {
      return i->second;
   }
   return nullptr;
}

WsfParseRuleDictionary::~WsfParseRuleDictionary() {}

//
// WsfParseValue* WsfParseRuleDictionary::FindValue(const std::string& aTypeName)
// {
//    ValueTypeMap::iterator i = mTypeNameToValue.find(aTypeName);
//    if (i != mTypeNameToValue.end())
//    {
//       return i->second;
//    }
//    return nullptr;
// }

WsfParseNamedRule::WsfParseNamedRule(WsfParseRuleMemory& aMem)
   : WsfParseRule(aMem, cNAMED_RULE)
   , mContextParent()
   , mInputTypePtr(nullptr)
   , mOutputTypePtr(nullptr)
   , mNestedRules()
{
   mReaderFlags |= cIS_PASSTHROUGH;
   mSubordinateRulePtr                 = new WsfParseAlternate(aMem);
   mSubordinateRulePtr->mParentRulePtr = this;
}

WsfParseNamedRule::WsfParseNamedRule(WsfParseRuleMemory& aMem, const std::string& aName, WsfParseRule* aRulePtr)
   : WsfParseRule(aMem, cNAMED_RULE)
   , mContextParent()
   , mName(aName)
   , mInputTypePtr(nullptr)
   , mOutputTypePtr(nullptr)
   , mNestedRules()
{
   SetSubordinateRule(aRulePtr);
   mReaderFlags |= cIS_PASSTHROUGH;
}

WsfParseNamedRule::~WsfParseNamedRule()
{
   delete mNestedRules;
}

bool WsfParseNamedRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   WsfParseNode* subNodePtr = nullptr;
   bool          ok         = mSubordinateRulePtr->Read(aParser, subNodePtr);
   if (subNodePtr)
   {
      if (aParser.FullyExpandNodes())
      {
         aNode = aParser.NewNode(this, mName.c_str());
         aNode->Add(subNodePtr);
      }
      else
      {
         subNodePtr->mRulePtr = this;
         aNode                = subNodePtr;
      }
   }
   return ok;
}

bool WsfParseNamedRule::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   // TODO: we should set the reader on aNode to this
   bool ok = mSubordinateRulePtr->ReadRange(aParser, aRange, aNode, aCreateNode);
   if (aNode)
   {
      aNode->mRulePtr = this;
   }
   return ok;
}

std::string WsfParseNamedRule::GetFullName() const
{
   std::string        name = mName;
   WsfParseNamedRule* p    = GetContainingStruct();
   while (p)
   {
      name = p->mName + '.' + name;
      p    = p->GetContainingStruct();
   }
   return name;
}

bool WsfParseNamedRule::InitializeInputsOutputs()
{
   WsfParseNamedRule* ctx = GetVarContext();
   if (mOutputTypeName.empty() && mInputTypeName.empty() && ctx)
   {
      // inherit from parent if no inputs or outputs defined:
      mOutputTypeName = ctx->mOutputTypeName;
      mInputTypeName  = ctx->mInputTypeName;
   }
   if (!mOutputTypeName.empty() && !mOutputTypePtr)
   {
      mOutputTypePtr = Find(mOutputTypeName);
      if (!mOutputTypePtr || !mOutputTypePtr->GetType())
      {
         return false;
      }
   }
   if (!mInputTypeName.empty() && !mInputTypePtr)
   {
      mInputTypePtr = Find(mInputTypeName);
      if (!mInputTypePtr || !mInputTypePtr->GetType())
      {
         return false;
      }
   }
   return true;
}

void WsfParseNamedRule::Initialize(int aSequence)
{
   WsfParseRule::Initialize(aSequence);
   if (aSequence == 1)
   {
      std::sort(mReferencingRules.begin(), mReferencingRules.end());
   }
   WsfParseNamedRule* r = this;
   while (r)
   {
      if (r->Type() == cVALUE)
      {
         mReaderFlags |= cIS_VALUE_RULE;
         break;
      }
      r = r->mContextParent;
   }
}


WsfParseStruct* WsfParseNamedRule::FindStruct(const std::string& aRuleName)
{
   WsfParseNamedRule* r = Find(aRuleName);
   if (r && r->Type() == WsfParseRule::cSTRUCT)
   {
      return dynamic_cast<WsfParseStruct*>(r);
   }
   return nullptr;
}

WsfParseNamedRule* WsfParseNamedRule::Find(const std::string& aRuleName)
{
   WsfParseNamedRule* lookup  = nullptr;
   WsfParseNamedRule* current = this;
   while (current && !lookup)
   {
      if (current->mNestedRules)
      {
         lookup = current->mNestedRules->Find(aRuleName);
      }
      current = current->mContextParent;
   }
   return lookup;
}

// Find a nested named rule.  Do not look on parent
WsfParseNamedRule* WsfParseNamedRule::FindImmediate(const std::string& aRuleName)
{
   WsfParseNamedRule* lookup = nullptr;
   if (mNestedRules)
   {
      lookup = mNestedRules->Find(aRuleName);
   }
   return lookup;
}

WsfParseValue* WsfParseNamedRule::FindValue(const std::string& aTypeName)
{
   WsfParseRule* rulePtr = Find(aTypeName);
   if (rulePtr && rulePtr->Type() == WsfParseRule::cVALUE)
   {
      return dynamic_cast<WsfParseValue*>(rulePtr);
   }
   return nullptr;
}

WsfParseRuleDictionary* WsfParseNamedRule::GetNested()
{
   if (!mNestedRules)
   {
      mNestedRules = new WsfParseRuleDictionary;
   }
   return mNestedRules;
}

void WsfParseNamedRule::SetDefinition(WsfParseRule* aRulePtr)
{
   SetSubordinateRule(aRulePtr);
   aRulePtr->mParentRulePtr = this;
}

// GetRuleValue() returns pointer to 'Position' rule when
// called on Position or Position.lat-lon
// (value Position
//  (rule lat-lon :id 1 { <Latitude> <Longitude> })
//  { position <lat-lon> }
// )
WsfParseValue* WsfParseNamedRule::GetRuleValue()
{
   if (0 == (mReaderFlags & cIS_VALUE_RULE))
   {
      return nullptr;
   }
   WsfParseNamedRule* r = this;
   while (r)
   {
      if (r->Type() == cVALUE)
      {
         return dynamic_cast<WsfParseValue*>(r);
      }
      r = r->mContextParent;
   }
   assert(false);
   return nullptr;
}


bool WsfParsePassthrough::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   bool rval = mSubordinateRulePtr->Read(aParser, aNode);
   if (rval && aNode)
   {
      ProcessNode(*aNode);
   }
   return rval;
}

bool WsfParsePassthrough::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   bool rval = mSubordinateRulePtr->ReadRange(aParser, aRange, aNode, aCreateNode);
   if (rval && aNode)
   {
      ProcessNode(*aNode);
   }
   return rval;
}

void WsfParsePassthrough::ProcessNode(WsfParseNode& aNode)
{
   switch (mKind)
   {
   case cERROR_RULE:
      aNode.mFlags |= WsfParseNode::cERROR_INCOMPLETE;
      break;
   default:
      break;
   }
   aNode.mRulePtr = this;
}

void WsfParsePassthrough::SetPassthroughRule(WsfParseRule* aRulePtr)
{
   SetSubordinateRule(aRulePtr);
}


WsfParseAttribute* WsfParseStruct::FindAttribute(const std::string& aName)
{
   for (size_t i = 0; i < mAttributes.size(); ++i)
   {
      if (mAttributes[i].mName == aName)
      {
         return &mAttributes[i];
      }
   }
   if (mBasePtr)
   {
      return mBasePtr->FindAttribute(aName);
   }
   return nullptr;
}

size_t WsfParseStruct::FindAttributeIndex(const std::string& aName)
{
   auto a = std::find_if(mAttributes.begin(),
                         mAttributes.end(),
                         [aName](WsfParseAttribute attribute) { return attribute.mName == aName; });

   if (a != mAttributes.end())
   {
      return ut::safe_cast<size_t, ptrdiff_t>(std::distance(mAttributes.begin(), a)) + mBaseAttributeCount;
   }
   if (mBasePtr != nullptr)
   {
      return mBasePtr->FindAttributeIndex(aName);
   }

   return ut::npos;
}

size_t WsfParseStruct::GetAttributeCount()
{
   return mAttributes.size() + mBaseAttributeCount;
}

void WsfParseStruct::SetBase(WsfParseStruct* aBasePtr)
{
   mBasePtr                = aBasePtr;
   WsfParseStruct* basePtr = mBasePtr;
   mBaseAttributeCount     = 0;
   while (basePtr)
   {
      mBaseAttributeCount += basePtr->mAttributes.size();
      basePtr = basePtr->GetBase();
   }
}

void WsfParseStruct::Initialize(int aSequence)
{
   WsfParseRule::Initialize(aSequence);
}

WsfParseValueType* WsfParseStruct::GetType()
{
   if (mTypePtr)
   {
      return mTypePtr;
   }
   mTypePtr = new WsfParseValueType(this);
   return mTypePtr;
}

WsfParseStruct::WsfParseStruct(WsfParseRuleMemory& aMem, const std::string& aRuleName)
   : WsfParseNamedRule(aMem)
   , mBaseAttributeCount(0)
   , mBasePtr(nullptr)
   , mTypePtr(nullptr)
{
   mName          = aRuleName;
   mRuleType      = cSTRUCT;
   mInputTypeName = mName;
   mInputTypePtr  = this;
}

WsfParseStruct::~WsfParseStruct()
{
   for (size_t i = 0; i < mInitialActions.size(); ++i)
   {
      delete mInitialActions[i];
   }
}

// Returns the list of all script variables for this type including variables inherited from the base class.
std::vector<WsfParseStruct::ScriptVariable> WsfParseStruct::GetAllScriptVariables()
{
   std::vector<WsfParseStruct::ScriptVariable> scriptVars = mScriptVariables;
   if (mBasePtr)
   {
      std::vector<WsfParseStruct::ScriptVariable> baseVars = mBasePtr->GetAllScriptVariables();
      for (const auto& baseVar : baseVars)
      {
         auto found = std::find_if(scriptVars.begin(),
                                   scriptVars.end(),
                                   [&baseVar](const WsfParseStruct::ScriptVariable& aLhs)
                                   { return aLhs.mVariableName == baseVar.mVariableName; });
         if (found == scriptVars.end())
         {
            scriptVars.push_back(baseVar);
         }
      }
   }
   return scriptVars;
}

bool WsfParseRuleReference::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   WsfParseNode* subNodePtr = nullptr;
   bool          ok         = mSubordinateRulePtr->Read(aParser, subNodePtr);
   if (subNodePtr)
   {
      if (aParser.FullyExpandNodes())
      {
         aNode = aParser.NewNode(this, "rule-reference");
         aNode->Add(subNodePtr);
      }
      else
      {
         subNodePtr->mRulePtr = this;
         aNode                = subNodePtr;
      }
   }
   return ok;
}

bool WsfParseRuleReference::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   bool ok = mSubordinateRulePtr->ReadRange(aParser, aRange, aNode, aCreateNode);
   if (aNode)
   {
      aNode->mRulePtr = this;
   }
   return ok;
}

WsfParseRuleReference::~WsfParseRuleReference() {}

WsfParseValueType::WsfParseValueType(WsfParseValue* aValuePtr)
   : mKind(cVALUE)
   , mTypeName(aValuePtr->GetFullName())
   , mStructPtr(nullptr)
   , mValuePtr(aValuePtr)
   , mTemplateType(nullptr)
{
}

WsfParseValueType::WsfParseValueType(WsfParseStruct* aStructPtr)
   : mKind(cVALUE)
   , mTypeName(aStructPtr->GetFullName())
   , mStructPtr(aStructPtr)
   , mValuePtr(nullptr)
   , mTemplateType(nullptr)
{
}

WsfParseNamedRule* WsfParseValueType::GetTypeRule()
{
   if (mStructPtr)
   {
      return mStructPtr;
   }
   return mValuePtr;
}

WsfParseValue::WsfParseValue(WsfParseRuleMemory& aMem, const std::string& aName)
   : WsfParseNamedRule(aMem)
   , mTypePtr(nullptr)
   , mIsEnumeration(false)
{
   mRuleType       = cVALUE;
   mName           = aName;
   mOutputTypeName = mName;
   mOutputTypePtr  = this;
}

void WsfParseValue::Initialize(int aSequence)
{
   WsfParseRule::Initialize(aSequence);
}

WsfParseValueType* WsfParseValue::GetType()
{
   if (mTypePtr)
   {
      return mTypePtr;
   }
   mTypePtr = new WsfParseValueType(this);
   return mTypePtr;
}

std::vector<WsfParseValueType*> WsfParseRuleMemory::CollectTypes()
{
   std::vector<WsfParseValueType*> types;
   for (size_t i = 0; i < mRules.size(); ++i)
   {
      types.push_back(mRules[i]->GetType());
   }
   std::sort(types.begin(), types.end());
   types.erase(std::unique(types.begin(), types.end()), types.end());
   return types;
}

WsfParseRuleMemory::~WsfParseRuleMemory()
{
   for (size_t i = 0; i < mRules.size(); ++i)
   {
      delete mRules[i];
   }
}
