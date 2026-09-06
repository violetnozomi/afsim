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

#include "WsfPProxyActionLookup.hpp"

#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseTypeLoadRule.hpp"

WsfPProxyActionLookup::WsfPProxyActionLookup(WsfPProxyRegistry* aRegistryPtr)
{
   mRegistryPtr = aRegistryPtr;
   mRootType    = aRegistryPtr->GetType("root");
   RecurseAllRules(mRegistryPtr->mRootRule->FindStruct("root"));
   RecurseAllRules(mRegistryPtr->mRootRule);
   // clean up
   mRulesEvaluating.clear();
   // remove entries with no modifications
   for (auto it = mRuleChangedAttributes.begin(); it != mRuleChangedAttributes.end();)
   {
      if (it->second.empty())
      {
         mRuleChangedAttributes.erase(it++);
      }
      else
      {
         ++it;
      }
   }
}

void WsfPProxyActionLookup::GetRulesThatModifyAttribute(WsfParseAlternate*          aAlternate,
                                                        size_t                      aAttributeIndex,
                                                        std::vector<WsfParseRule*>& aAlternateChoices)
{
   // Note: this could be accelerated
   const std::vector<WsfParseRule*>& alternates = aAlternate->Alternates();
   for (auto* seq : alternates)
   {
      while (seq && (seq->GetReaderFlags() & WsfParseRule::cIS_PASSTHROUGH))
      {
         seq = seq->GetPassthrough();
      }

      // Always try type commands
      if (seq && (seq->Type() == WsfParseRule::cTYPE_COMMAND))
      {
         aAlternateChoices.push_back(seq);
      }
      else
      {
         auto iter = mRuleChangedAttributes.find(seq);
         if (iter != mRuleChangedAttributes.end())
         {
            if (iter->second.find(aAttributeIndex) != iter->second.end())
            {
               aAlternateChoices.push_back(seq);
            }
         }
      }
   }
}

void WsfPProxyActionLookup::RecurseAllRules(WsfParseNamedRule* aNamedRulePtr)
{
   std::set<size_t>     indices;
   WsfParseNamedRule*   entryTypePtr = aNamedRulePtr->GetEntryType();
   WsfPProxyStructType* currentType  = nullptr;
   if (entryTypePtr && entryTypePtr->Type() == WsfParseRule::cSTRUCT)
   {
      currentType = mRegistryPtr->GetStruct((WsfParseStruct*)entryTypePtr);
   }
   GetChangedAttributes(aNamedRulePtr, indices, currentType);
   WsfParseRuleDictionary* nested = aNamedRulePtr->GetNestedRules();
   if (nested)
   {
      for (WsfParseRuleDictionary::NameRuleMap::iterator i = nested->mNameToRule.begin(); i != nested->mNameToRule.end();
           ++i)
      {
         RecurseAllRules(i->second);
      }
   }
}

WsfPProxyType* WsfPProxyActionLookup::GetAddressType(WsfPProxyType* aCurrentTypePtr, WsfParseActionAddress& aAddr)
{
   WsfPProxyType* curTypePtr = aCurrentTypePtr;
   for (size_t i = 0; i < aAddr.mAttrIndices.size() && curTypePtr; ++i)
   {
      WsfParseActionAddress::Entry& e = aAddr.mAttrIndices[i];
      if (e.IsAttributeIndex())
      {
         curTypePtr = curTypePtr->GetContainedType(e.mIndex);
      }
      else
      {
         curTypePtr = curTypePtr->GetContainedType(0);
      }
   }
   return curTypePtr;
}
void WsfPProxyActionLookup::GetChangedAttributes_ApplyActions(WsfParseSequence* aSequencePtr,
                                                              WsfParseAction*   aAction,
                                                              std::set<size_t>& aAttributeIndices,
                                                              WsfPProxyType*&   aCurrentTypePtr)
{
   WsfPProxyType* curTypePtr = aCurrentTypePtr;
   for (size_t i = 0; i < aAction->mSubActions.size(); ++i)
   {
      WsfParseActionPart*   a = aAction->mSubActions[i];
      WsfParseActionAddress changedAddr;
      switch (a->mActionPartType)
      {
      case WsfParseActionPart::cASSIGN:
      {
         WsfParseActionAssign* ass = (WsfParseActionAssign*)a;
         if (curTypePtr == aCurrentTypePtr)
         {
            changedAddr = ass->mLHS_Address;
         }
      }
      break;
      case WsfParseActionPart::cPUSH:
      {
         WsfParseActionPush* push = (WsfParseActionPush*)a;
         if (curTypePtr == aCurrentTypePtr)
         {
            changedAddr = push->mAttributeAddr;
         }
         curTypePtr = GetAddressType(curTypePtr, push->mAttributeAddr);
      }
      break;
      case WsfParseActionPart::cOBJECT_MAP_OP:
      {
         WsfParseActionObjectMapOp* op = (WsfParseActionObjectMapOp*)a;
         if (curTypePtr == aCurrentTypePtr)
         {
            changedAddr = op->mAttributeAddr;
         }
         curTypePtr = GetAddressType(curTypePtr, op->mAttributeAddr);
         if (op->mOperation == WsfParseActionObjectMapOp::cNEW || op->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
         {
            if (curTypePtr)
            {
               curTypePtr = curTypePtr->GetContainedType(0);
            }
         }
         else if (op->mOperation == WsfParseActionObjectMapOp::cAPPLY)
         {
            size_t typeNode = aSequencePtr->FindActionIndex(aAction) - 1;
            if (typeNode != ut::npos)
            {
               WsfParseTypeLoadRule* typeRule = (WsfParseTypeLoadRule*)aSequencePtr->Sequence()[typeNode];
               if (typeRule->GetOperation() == WsfParseTypeInfoData::cLOAD_TYPE)
               {
                  if (curTypePtr == aCurrentTypePtr && curTypePtr && curTypePtr->IsStruct() &&
                      !typeRule->mLoadType.mPath.empty())
                  {
                     assert(typeRule->mLoadType.mNestedLookup);
                     WsfPProxyStructType* structPtr = (WsfPProxyStructType*)curTypePtr;
                     size_t               idx       = structPtr->GetMemberIndex(typeRule->mLoadType.mPath[0]);
                     if (idx != ut::npos)
                     {
                        changedAddr.mAttrIndices.emplace_back(WsfParseActionAddress::EntryType::cATTRIBUTE, idx);
                     }
                  }
                  curTypePtr = LookupType(curTypePtr, typeRule->mLoadType);
               }
               else if (typeRule->GetOperation() == WsfParseTypeInfoData::cCREATE_TYPE)
               {
                  if (curTypePtr == aCurrentTypePtr && curTypePtr && curTypePtr->IsStruct() &&
                      !typeRule->mSaveType.mPath.empty())
                  {
                     if (typeRule->mSaveType.mNestedLookup)
                     {
                        WsfPProxyStructType* structPtr = (WsfPProxyStructType*)curTypePtr;
                        size_t               idx       = structPtr->GetMemberIndex(typeRule->mSaveType.mPath[0]);
                        if (idx != ut::npos)
                        {
                           changedAddr.mAttrIndices.emplace_back(WsfParseActionAddress::EntryType::cATTRIBUTE, idx);
                        }
                     }
                  }
                  curTypePtr = LookupType(curTypePtr, typeRule->mSaveType);
               }
            }
         }
      }
      break;
      case WsfParseActionPart::cLIST_OP:
      {
         WsfParseActionListOp* op = (WsfParseActionListOp*)a;
         if (curTypePtr == aCurrentTypePtr)
         {
            changedAddr = op->mAttributeAddr;
         }
         curTypePtr = GetAddressType(curTypePtr, op->mAttributeAddr);
      }
      break;
      case WsfParseActionPart::cCOPY:
      {
         WsfParseActionCopy* cp = (WsfParseActionCopy*)a;
         if (curTypePtr == aCurrentTypePtr)
         {
            changedAddr = cp->mDstAddr;
         }
      }
      break;
      default:
         break;
      }
      if (!changedAddr.IsEmpty())
      {
         WsfParseActionAddress::Entry& e = changedAddr.mAttrIndices[0];
         if (e.IsAttributeIndex())
         {
            aAttributeIndices.insert(e.mIndex);
         }
      }
      if (!curTypePtr)
      {
         break;
      }
   }
   aCurrentTypePtr = curTypePtr;
}

void WsfPProxyActionLookup::GetChangedAttributes(WsfParseRule*     aRulePtr,
                                                 std::set<size_t>& aAttributeIndices,
                                                 WsfPProxyType*    aCurrentTypePtr)
{
   if (aRulePtr->GetReaderFlags() & WsfParseRule::cIS_PASSTHROUGH)
   {
      return GetChangedAttributes(aRulePtr->GetPassthrough(), aAttributeIndices, aCurrentTypePtr);
   }

   // Return early if already computed.
   if (mRuleChangedAttributes.find(aRulePtr) != mRuleChangedAttributes.end())
   {
      auto& changed = mRuleChangedAttributes[aRulePtr];
      aAttributeIndices.insert(changed.begin(), changed.end());
      return;
   }
   if (mRulesEvaluating.find(aRulePtr) != mRulesEvaluating.end())
   {
      // avoid infinite recursion.
      return;
   }
   mRulesEvaluating.insert(aRulePtr);
   // set of indices possibly changed in this rule
   std::set<size_t> ruleChangedIndices;
   if (aRulePtr->GetEntryType())
   {
      aCurrentTypePtr = mRegistryPtr->GetType(aRulePtr->GetEntryType());
   }

   // std::string desc = aRulePtr->GetRuleDescription(); std::cout << desc << '\n';

   // Only keep attribute indices changed if they change the current type at the start of the rule.
   // Otherwise just insert indices in this throw-away set
   std::set<size_t> indices;

   WsfPProxyType* curTypePtr = aCurrentTypePtr;
   if (aRulePtr->Type() == WsfParseRule::cSEQUENCE)
   {
      WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(aRulePtr);
      for (size_t i = 0; i < seq->Sequence().size(); ++i)
      {
         WsfParseAction* actionPtr = seq->GetActionBefore(i);
         if (actionPtr)
         {
            indices.clear();
            GetChangedAttributes_ApplyActions(seq,
                                              actionPtr,
                                              (curTypePtr == aCurrentTypePtr) ? ruleChangedIndices : indices,
                                              curTypePtr);
         }
         indices.clear();
         GetChangedAttributes(seq->Sequence()[i], (curTypePtr == aCurrentTypePtr) ? ruleChangedIndices : indices, curTypePtr);
         curTypePtr = aCurrentTypePtr;
      }
      WsfParseAction* actionPtr = seq->GetActionBefore(seq->Sequence().size());
      if (actionPtr)
      {
         indices.clear();
         GetChangedAttributes_ApplyActions(seq,
                                           actionPtr,
                                           (curTypePtr == aCurrentTypePtr) ? ruleChangedIndices : indices,
                                           curTypePtr);
      }
   }
   else
   {
      std::vector<WsfParseRule*> seq = aRulePtr->GetSequence();
      for (size_t i = 0; i < seq.size(); ++i)
      {
         GetChangedAttributes(seq[i], ruleChangedIndices, curTypePtr);
      }
      std::vector<WsfParseRule*> alt = aRulePtr->GetAlternates();
      for (size_t i = 0; i < alt.size(); ++i)
      {
         GetChangedAttributes(alt[i], ruleChangedIndices, curTypePtr);
      }
   }
   if (!aCurrentTypePtr)
   {
      ruleChangedIndices.clear();
   }
   mRuleChangedAttributes[aRulePtr] = ruleChangedIndices;
   // Rules with output types do not change values by reference
   if (!aRulePtr->GetOutputType())
   {
      aAttributeIndices.insert(ruleChangedIndices.begin(), ruleChangedIndices.end());
   }
}

WsfPProxyType* WsfPProxyActionLookup::LookupType(WsfPProxyType* aTypePtr, const std::string& aSubType)
{
   if (!aTypePtr)
   {
      return nullptr;
   }
   if (aTypePtr->IsStruct())
   {
      WsfPProxyStructType* structPtr = (WsfPProxyStructType*)aTypePtr;
      size_t               idx       = structPtr->GetMemberIndex(aSubType);
      if (idx != ut::npos)
      {
         return structPtr->GetContainedType(idx);
      }
      return nullptr;
   }
   else if (aTypePtr->IsList() || aTypePtr->IsObjectMap())
   {
      return aTypePtr->GetContainedType(0);
   }
   return nullptr;
}

WsfPProxyType* WsfPProxyActionLookup::LookupType(WsfPProxyType* aBaseType, WsfParseTypeName& aPath)
{
   WsfPProxyType* typePtr = aBaseType;
   // assert(aPath.mNestedLookup);
   if (!aPath.mNestedLookup)
   {
      typePtr = mRegistryPtr->GetType("root");
   }
   for (size_t i = 0; i < aPath.mPath.size(); ++i)
   {
      typePtr = LookupType(typePtr, aPath.mPath[i].Get());
   }
   //    if (aPath.mOrdinal != -1)
   //    {
   //       typePtr = LookupType(typePtr, "");
   //    }
   return typePtr;
}
