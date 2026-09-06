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

#include "WsfPProxySerialize.hpp"

#include <cassert>
#include <fstream>
#include <map>
#include <queue>

#include "UtAlgorithm.hpp"
#include "UtSleep.hpp"
#include "UtTextDocument.hpp"
#include "WsfPProxy.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyDiff.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyNode.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyRuleTrace.hpp"
#include "WsfPProxySatisfy.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfPProxyType.hpp"
#include "WsfPProxyUndo.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseDefinitions.hpp"
#include "WsfParseFormat.hpp"
#include "WsfParseIndex.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseTypeLoadRule.hpp"
#include "WsfParser.hpp"

class WsfPProxyDeserializeTracer : public WsfPProxyDeserializeTracerI, public WsfPProxyDeserializeObserver
{
public:
   enum ListEntryUse
   {
      cKEEP_ENTRY,
      cINSERT_NEW_ENTRY,
      cREMOVE_ENTRY
   };

   struct ListDiff
   {
      ListDiff()
         : mLastUsedContext(nullptr)
      {
      }
      std::queue<ListEntryUse>   mKeptEntries;
      std::vector<size_t>        mOldIndexToNewIndex;
      WsfPParseRuleTraceContext* mLastUsedContext;
   };

   enum SatisfyKind
   {
      cSK_ASSIGNMENT
   };

   struct ValueToSatisfy
   {
      SatisfyKind                mSatisfyKind;
      WsfPProxyPath              mPath;
      WsfPParseRuleTraceContext* mContextPtr;
   };

   WsfPProxyDeserializeTracer(WsfPProxyUndoValue& aDeserializingProxy,
                              WsfPProxyValue      aDesiredRoot,
                              WsfPProxyValue      aExistingProxy,
                              WsfPProxyRegistry*  aRegistryPtr)
      : mNodeTracer(aRegistryPtr)
   {
      mDeserializingProxy = aDeserializingProxy;
      mDesiredRoot        = aDesiredRoot;
      mExistingProxyRoot  = aExistingProxy;
      mSatisfy            = nullptr;

      //          std::cout << "DESIRED: ";
      //          aRegistryPtr->OutputJSON(mDesiredRoot["platform"]["p"]["defaultRoute"], mDesiredRoot, std::cout,
      //          WsfPProxyRegistry::cHIDE_UNSET_VALUES | WsfPProxyRegistry::cHIDE_INHERITED_VALUES); std::cout << '\n';
   }

   ~WsfPProxyDeserializeTracer() override {}

   void BeginNode(WsfParseNode* aNodePtr, const WsfPProxyPath& aCurrentPath) override
   {
      mNodeTracer.BeginNode(aNodePtr, aCurrentPath);
   }

   void EndNode(WsfParseNode* aNodePtr) override
   {
      mNodeTracer.EndNode(aNodePtr);
      //          WsfPParseRuleTrace* tracePtr = mNodeTracer.EndNode(aNodePtr);
      //          if (aNodePtr)
      //          {
      //             bool isPruned = 0 != (aNodePtr->mFlags & WsfParseNode::cPRUNE_NODE);
      //             if (tracePtr && tracePtr->GetContext() && !isPruned)
      //             {
      //                WsfPParseRuleTraceContext* ctx = tracePtr->GetContext();
      //                if (ctx)
      //                {
      //                   mSatisfy->SatisfyBlock(ctx, mExistingProxyRoot);
      //                }
      //             }
      //          }
   }

   void BeforeCopy(const WsfPProxyPath& aCopyFrom) override { FinalizeType(aCopyFrom); }

   WsfPProxyPath DesiredPath(const WsfPProxyPath aCurrentPath)
   {
      for (std::map<WsfPProxyPath, ListDiff>::iterator iter = mListDiffs.begin(); iter != mListDiffs.end(); ++iter)
      {
         if (iter->first.IsPrefixOf(aCurrentPath) && aCurrentPath.size() > iter->first.size())
         {
            size_t currentListIndex = aCurrentPath[iter->first.size()].GetIndex();
            size_t newListIndex     = currentListIndex;
            if (currentListIndex >= iter->second.mOldIndexToNewIndex.size() - iter->second.mKeptEntries.size() &&
                currentListIndex < iter->second.mOldIndexToNewIndex.size())
            {
               newListIndex = iter->second.mOldIndexToNewIndex[currentListIndex];
            }
            WsfPProxyPath desiredPath = aCurrentPath;
            desiredPath[iter->first.size()].SetIndex(newListIndex);
            return desiredPath;
         }
      }
      return aCurrentPath;
   }

   void AfterLoad(const WsfPProxyPath& aLoadPath) override
   {
      // This is tricky.  we don't want to remove bad inputs that aren't being changed explicitly
      // because we can't be 100% sure our perception is correct.
      // Therefore, only prune commands that attempt to load types that are newly removed.

      WsfPProxyPath  desiredPath = DesiredPath(aLoadPath);
      WsfPProxyValue desired     = mDesiredRoot.Lookup(desiredPath);
      if (desired)
      {
         return;
      }
      WsfPProxyValue existingValue = mExistingProxyRoot.Lookup(aLoadPath);
      if (existingValue)
      {
         WsfPParseRuleTrace* traceToPrune = FindPrunePoint();
         if (traceToPrune)
         {
            traceToPrune->SetPruned();
         }
      }
   }

   void BeforeAssignment(const WsfPProxyPath& aPath, WsfPProxyValue& aNewValue) override
   {
      //          std::cout << "DESIRED: ";
      //          mSatisfy->mRegistryPtr->OutputJSON(mDesiredRoot["platform"]["p"]["defaultRoute"], mDesiredRoot, std::cout,
      //          WsfPProxyRegistry::cHIDE_UNSET_VALUES | WsfPProxyRegistry::cHIDE_INHERITED_VALUES); std::cout << '\n';
      WsfPProxyPath  desiredPath = DesiredPath(aPath);
      WsfPProxyValue desired     = mDesiredRoot.Lookup(desiredPath);
      if (desired && desired.IsBasicType())
      {
         WsfPProxyBasicValue desiredBasic(desired);

         // If the desired value is equal to the final result with no changes,
         // then allow this assignment even if it doesn't match the final desired value.
         WsfPProxyValue existingValue = mExistingProxyRoot.Lookup(aPath);
         if (desiredBasic.IsEqual(existingValue))
         {
            return;
         }

         if (!desiredBasic.IsEqual(aNewValue))
         {
            // Existing value is desired, don't apply this new value
            WsfPParseRuleTrace* traceToPrune = FindPrunePoint();
            if (traceToPrune)
            {
               traceToPrune->SetPruned();
               WsfPParseRuleTraceContext* ctx = traceToPrune->EnclosingContext();
               WsfParseNode*              n   = traceToPrune->GetNearestNode();
               if (ctx && n)
               {
                  SatisfyValueLater(cSK_ASSIGNMENT, n, aPath, ctx);
               }
            }
         }
      }
   }
   void BeforeMapInsert(const WsfPProxyPath& aMapPath, const std::string& aMapKey, WsfPProxyValue aValue) override
   {
      // If the new item isn't in the desired proxy, prune the inputs
      bool           allow      = true;
      WsfPProxyValue desiredMap = mDesiredRoot.Lookup(aMapPath);
      if (desiredMap)
      {
         WsfPProxyValue desiredVal = desiredMap.GetAttr(aMapKey);
         if (!desiredVal)
         {
            allow = false;
         }
         else
         {
            // Type changing, prune commands
            // TODO: it may make more sense to just modify the type
            if (desiredVal.GetType() != aValue.GetType())
            {
               allow = false;
            }
         }
      }
      if (allow)
      {
         if (mDeserializingProxy.Get().Lookup(aMapPath).GetAttr(aMapKey))
         {
            // already exists...
            allow = false;
         }
      }
      if (!allow)
      {
         WsfPParseRuleTrace* traceToPrune = FindPrunePoint();
         if (traceToPrune)
         {
            traceToPrune->SetPruned();
         }
      }
   }

   WsfPParseRuleTrace* FindPrunePoint()
   {
      WsfPParseRuleTrace* traceToPrune = nullptr;
      const auto&         currentNodes = mNodeTracer.GetCurrentNodes();
      for (auto i = currentNodes.rbegin(); i != currentNodes.rend(); ++i)
      {
         WsfParseNode* n = (*i)->mNodePtr;
         if (n)
         {
            if (n && (!n->mRulePtr || n->mRulePtr->Type() == WsfParseRule::cRECURRENCE))
            {
               break;
            }
         }
         traceToPrune = *i;
      }
      return traceToPrune;
   }

   void BeforeMapDelete(const WsfPProxyPath& aMapPath, const std::string& aMapKey) override {}

   void BeforeListPush(const WsfPProxyPath& aListPath) override
   {
      WsfPProxyValue desired     = mDesiredRoot.Lookup(aListPath);
      WsfPProxyList* desiredList = desired.GetList();
      if (desiredList)
      {
         WsfPProxyList* currentList = mDeserializingProxy.Get().Lookup(aListPath).GetList();
         ListDiff*      listDiff    = GetListDiff(aListPath);
         if (!listDiff)
         {
            return;
         }
         bool prune = false;
         while (!listDiff->mKeptEntries.empty())
         {
            ListEntryUse usage = listDiff->mKeptEntries.front();
            listDiff->mKeptEntries.pop();
            listDiff->mLastUsedContext = mNodeTracer.GetCurrentNodes().back()->EnclosingContext();
            if (usage == cKEEP_ENTRY)
            {
               break;
            }
            else if (usage == cREMOVE_ENTRY)
            {
               prune = true;
               break;
            }
            else if (usage == cINSERT_NEW_ENTRY)
            {
               WsfPProxyPath entryPath = aListPath;
               entryPath += currentList->Size();
               WsfPParseRuleTraceContext* ctx       = mNodeTracer.GetCurrentNodes().back()->EnclosingContext();
               size_t                     commitPos = mDeserializingProxy.GetHistorySize();
               mSatisfy->mRecurrenceInsertBeforeEnd = true;
               /*WsfPProxySatisfy::SatisfyResult result = */ mSatisfy->SatisfyAttributeInContext(entryPath, ctx);
               mSatisfy->mRecurrenceInsertBeforeEnd = false;
               mDeserializingProxy.Commit(commitPos);
            }
         }
         if (prune)
         {
            // Existing value is desired, don't apply this new value
            WsfPParseRuleTrace* traceToPrune = FindPrunePoint();
            if (traceToPrune)
            {
               traceToPrune->SetPruned();
               //                   int childrenCount = ctx->mTracePtr->Children().size();
               //                   WsfPProxySatisfy::SatisfyResult result = mSatisfy->SatisfyAttributeInContext(aPath,
               //                   ctx); if (result.TargetComplete())
               //                   {
               //                      if (ctx->mTracePtr->Children().size() > childrenCount)
               //                      {
               //                         WsfPParseRuleTrace* newChild = ctx->mTracePtr->Children().back();
               //                         int stop = 5;
               //                      }
               //                      int stop = 5;
               //                   }
            }
         }
         else
         {
         }
      }
   }

   ListDiff* GetListDiff(const WsfPProxyPath& aListPath)
   {
      auto iter = mListDiffs.find(aListPath);
      if (iter != mListDiffs.end())
      {
         return &iter->second;
      }
      WsfPProxyList* desiredList = mDesiredRoot.Lookup(aListPath).GetList();
      if (!desiredList)
      {
         return nullptr;
      }
      WsfPProxyList* existingList = mExistingProxyRoot.Lookup(aListPath).GetList();
      if (!existingList)
      {
         return nullptr;
      }


      using HashList = std::vector<WsfPProxyHash>;
      using HashEq   = std::equal_to<WsfPProxyHash>;

      std::vector<WsfPProxyHash> oldHash(existingList->mValues.size());
      for (size_t i = 0; i < oldHash.size(); ++i)
      {
         oldHash[i] = existingList->Get(i).Hash();
      }
      std::vector<WsfPProxyHash> newHash(desiredList->mValues.size());
      for (size_t i = 0; i < newHash.size(); ++i)
      {
         newHash[i] = desiredList->Get(i).Hash();
      }

      // mSatisfy->mRegistryPtr->OutputJSON(mExistingProxyRoot.Lookup(aListPath), mExistingProxyRoot, std::cout,
      // WsfPProxyRegistry::cHIDE_UNSET_VALUES | WsfPProxyRegistry::cHIDE_INHERITED_VALUES);

      // mSatisfy->mRegistryPtr->OutputJSON(mDesiredRoot.Lookup(aListPath), mDesiredRoot, std::cout,
      // WsfPProxyRegistry::cHIDE_UNSET_VALUES | WsfPProxyRegistry::cHIDE_INHERITED_VALUES);

      ListDiff&                        listDiff = mListDiffs[aListPath];
      UtSequenceDiff<HashList, HashEq> differ(oldHash, newHash, HashEq());
      differ.Diff();
      const auto& matches = differ.GetMatches();

      size_t oldIndex = 0;
      size_t newIndex = 0;
      for (size_t i = 0; i < matches.size(); ++i)
      {
         size_t l = matches[i].first;
         size_t r = matches[i].second;
         //             while (l > oldIndex && r > newIndex)
         //             {
         //                //TODO: We would like to keep nodes that don't quite match, but there are problems that occur
         //                later if we do this
         //                // the next match skips entries on both sides
         //                //listDiff.mKeptEntries.push(cKEEP_ENTRY);
         //                //listDiff.mOldIndexToNewIndex.push_back(newIndex);
         //                //++oldIndex;
         //                //++newIndex;
         //                listDiff.mKeptEntries.push(cREMOVE_ENTRY);
         //                ++oldIndex;
         //             }
         while (l > oldIndex) // Remove entries that don't match new ones
         {
            listDiff.mKeptEntries.push(cREMOVE_ENTRY);
            ++oldIndex;
         }
         while (r > newIndex) // Insert missing entries
         {
            listDiff.mKeptEntries.push(cINSERT_NEW_ENTRY);
            ++newIndex;
         }
         if (i + 1 < matches.size())
         {
            assert(l == oldIndex && r == newIndex);
            // Add the matched entry
            listDiff.mKeptEntries.push(cKEEP_ENTRY);
            listDiff.mOldIndexToNewIndex.push_back(newIndex);
            ++oldIndex;
            ++newIndex;
         }
      }
      while (newIndex < newHash.size())
      {
         listDiff.mKeptEntries.push(cINSERT_NEW_ENTRY);
         ++newIndex;
      }
      return &listDiff;
   }

   DeserializeAction CanContinue(WsfParseNode* aParentNodePtr, WsfParseNode* aChildNodePtr) override
   {
      if (aChildNodePtr->mFlags & WsfParseNode::cPRUNE_NODE)
      {
         return WsfPProxyDeserializeTracerI::cDA_ROLLBACK;
      }
      return WsfPProxyDeserializeTracerI::cDA_CONTINUE;
   }

   void SatisfyValue(ValueToSatisfy& aValue)
   {
      switch (aValue.mSatisfyKind)
      {
      case cSK_ASSIGNMENT:
      {
         WsfPProxyDiff differ;
         if (!differ.IsEqual(mDeserializingProxy.Get().Lookup(aValue.mPath), mDesiredRoot.Lookup(aValue.mPath)))
         {
            WsfPParseRuleTraceContext*      ctx              = aValue.mContextPtr;
            size_t                          beforeSatisfyPos = mDeserializingProxy.GetHistorySize();
            WsfPProxySatisfy::SatisfyResult result           = mSatisfy->SatisfyAttributeInContext(aValue.mPath, ctx);
            if (result.TargetComplete())
            {
               mDeserializingProxy.GetUndo()->Commit(beforeSatisfyPos);
            }
         }
      }
      break;
      }
   }

   void RollbackComplete(WsfParseNode* aParentNodePtr) override
   {
      ValuesToSatisfyMap::iterator iter = mValuesToSatisfy.find(aParentNodePtr);
      if (iter != mValuesToSatisfy.end())
      {
         for (size_t i = 0; i < iter->second.size(); ++i)
         {
            SatisfyValue(iter->second[i]);
         }
         mValuesToSatisfy.erase(iter);
      }
   }

   void FinalizeType(const WsfPProxyPath& aPath)
   {
      if (mFinalizedTypes.find(aPath) == mFinalizedTypes.end())
      {
         WsfPParseRuleTracer::Contexts* ctxs = mNodeTracer.FindContexts(aPath);
         if (ctxs)
         {
            // TODO: Shouldn't iterate all lists here...
            for (std::map<WsfPProxyPath, ListDiff>::iterator i = mListDiffs.begin(); i != mListDiffs.end();)
            {
               if (aPath.IsPrefixOf(i->first))
               {
                  FinalizeList(&i->second, i->first);
                  mListDiffs.erase(i++);
               }
               else
               {
                  ++i;
               }
            }
            for (size_t i = 0; i < ctxs->mContextList.size(); ++i)
            {
               auto* ctx = ctxs->mContextList[i].get();
               if (!ctx->Trace()->IsPruned())
               {
                  mSatisfy->SatisfyBlock(ctx, mExistingProxyRoot);
               }
            }
         }
         mFinalizedTypes.insert(aPath);
      }
   }

   void FinalizeList(ListDiff* listDiff, const WsfPProxyPath& aPath)
   {
      const WsfPProxyPath& listPath    = aPath;
      WsfPProxyValue       desired     = mDesiredRoot.Lookup(listPath);
      WsfPProxyList*       desiredList = desired.GetList();
      if (desiredList && listDiff->mLastUsedContext)
      {
         WsfPProxyList* currentList = mDeserializingProxy.Get().Lookup(listPath).GetList();
         if (currentList)
         {
            while (!listDiff->mKeptEntries.empty())
            {
               ListEntryUse usage = listDiff->mKeptEntries.front();
               listDiff->mKeptEntries.pop();
               if (usage == cKEEP_ENTRY)
               {
                  // Should not get here!
                  assert(false);
                  break;
               }
               else if (usage == cREMOVE_ENTRY)
               {
                  // Should not get here
                  assert(false);
                  break;
               }
               else if (usage == cINSERT_NEW_ENTRY)
               {
                  WsfPProxyPath entryPath = listPath;
                  entryPath += currentList->Size();
                  WsfPParseRuleTraceContext* ctx = listDiff->mLastUsedContext;
                  // mSatisfy->mRecurrenceInsertBeforeEnd = true;
                  /*WsfPProxySatisfy::SatisfyResult result = */ mSatisfy->SatisfyAttributeInContext(entryPath, ctx);
                  // mSatisfy->mRecurrenceInsertBeforeEnd = false;
               }
            }
         }
      }
   }
   void FinalizeLists()
   {
      for (std::map<WsfPProxyPath, ListDiff>::iterator i = mListDiffs.begin(); i != mListDiffs.end(); ++i)
      {
         ListDiff* listDiff = &i->second;
         FinalizeList(listDiff, i->first);
      }
   }

   void SatisfyValueLater(SatisfyKind                aKind,
                          WsfParseNode*              aNodePtr,
                          const WsfPProxyPath&       aPath,
                          WsfPParseRuleTraceContext* aContext)
   {
      ValueToSatisfy vts;
      vts.mSatisfyKind = aKind;
      vts.mPath        = aPath;
      vts.mContextPtr  = aContext;
      mValuesToSatisfy[aNodePtr].push_back(vts);
   }

   using ValuesToSatisfyMap = std::map<WsfParseNode*, std::vector<ValueToSatisfy>>;
   ValuesToSatisfyMap mValuesToSatisfy;

   WsfPProxyValue      mExistingProxyRoot;
   WsfPProxyUndoValue  mDeserializingProxy;
   WsfPProxyValue      mDesiredRoot;
   WsfPParseRuleTracer mNodeTracer;


   WsfPProxySatisfy* mSatisfy;

   std::map<WsfPProxyPath, ListDiff> mListDiffs;
   std::set<WsfPProxyPath>           mFinalizedTypes;
};

void FindCommandsChangingValue(WsfPProxyIndex& aIndex, const WsfPProxyPath& aPath, std::vector<WsfParseNode*>& aNodes)
{
   WsfPProxyIndexNode* idxNode = aIndex.Find(aPath);
   for (size_t i = 0; i < idxNode->mEntries.size(); ++i)
   {
      if (idxNode->mEntries[i].mEntryType == WsfPProxyIndex::cASSIGNED)
      {
         aNodes.push_back(idxNode->mEntries[i].mNodePtr);
      }
   }
}

WsfPProxySerialize::WsfPProxySerialize()
{
   mDeserializeTracer = nullptr;
   mTracer            = nullptr;
   mProxyIndex        = nullptr;
   mParserPtr         = nullptr;
   mProxyRegistry     = nullptr;
}

WsfPProxySerialize::~WsfPProxySerialize()
{
   delete mProxyIndex;
   delete mDeserializeTracer;
}

void WsfPProxySerialize::Initialize(WsfPProxyRegistry* aProxyRegistry, WsfParser* aParserPtr, const std::vector<UtPath>& aFiles)
{
   mParserPtr     = aParserPtr;
   mFiles         = aFiles;
   mProxyRegistry = aProxyRegistry;
}

bool CompareRangeOnly(const std::pair<UtTextRange, std::string>& lhs, const std::pair<UtTextRange, std::string>& rhs)
{
   return lhs.first.GetEnd() < rhs.first.GetEnd();
}

void WsfPProxySerialize::PreSerialize(std::vector<UtTextDocument*>& aChangedFiles)
{
   // if objects have been renamed, we first go through and update files so that
   // the objects are named correctly.  In this way, the proxy serialize logic
   // doesn't need to understand the rename operation
   // Note: this only renames the objects, not references to them
   //       references should be handled through changes to the proxy (before we get here)
   if (mRenameMap.empty())
   {
      return;
   }

   WsfParseNode* treePtr = mParserPtr->ParseFiles(mFiles, 0);
   {
      std::vector<WsfParseNode*> fileTransitionNodes;
      mParserPtr->FinalizeParseTree(treePtr, fileTransitionNodes);
   }
   WsfPProxyValue existingProxy;
   auto           indexPtr      = ut::make_unique<WsfPProxyIndex>();
   indexPtr->mRecordCurrentPath = true;
   {
      WsfPProxyDeserialize deserialize(mProxyRegistry, indexPtr.get());
      existingProxy = WsfPProxyValue::ConstructNew(mProxyRegistry->GetType("root"));
      WsfPProxyUndoValue existingProxyUndo(existingProxy);
      existingProxyUndo.DisableUndo();
      deserialize.Deserialize(existingProxyUndo, treePtr);
   }
   WsfParseNode* n = treePtr;

   using RangeAndString         = std::pair<UtTextRange, std::string>;
   using DocumentReplacementMap = std::map<UtTextDocument*, std::vector<RangeAndString>>;

   DocumentReplacementMap documentReplacements;
   WsfPProxy              proxy;
   proxy.mIndex     = std::move(indexPtr);
   proxy.mRoot      = existingProxy;
   proxy.mBasicRoot = mProxyRegistry->GetBasicRoot();

   WsfPProxyNode rootNode(&proxy, WsfPProxyPath());

   while (n)
   {
      if (n->GetFlags() & WsfParseNode::cLOAD_TYPE_NODE)
      {
         const WsfParseTypeName& saveType    = ((WsfParseTypeLoadRule*)n->mRulePtr)->GetSaveType();
         const WsfParseTypeName& loadType    = ((WsfParseTypeLoadRule*)n->mRulePtr)->GetLoadType();
         const WsfPProxyPath*    currentPath = proxy.mIndex->FindCurrentPath(n);
         if (saveType.mOrdinal != ut::npos)
         {
            WsfPProxyNode pn(&proxy, *currentPath);
            pn = Lookup(pn, saveType, n->GetParent());
            if (mRenameMap.find(pn.GetPath()) != mRenameMap.end())
            {
               UtTextDocumentRange range = n->GetParent()->GetChild(saveType.mOrdinal)->SubtreeRange();
               documentReplacements[range.mSource].push_back(RangeAndString(range, mRenameMap[pn.GetPath()]));
            }
         }
         if (loadType.mOrdinal != ut::npos)
         {
            WsfPProxyNode pn(&proxy, *currentPath);
            pn = Lookup(pn, loadType, n->GetParent());
            if (mRenameMap.find(pn.GetPath()) != mRenameMap.end())
            {
               UtTextDocumentRange range = n->GetParent()->GetChild(loadType.mOrdinal)->SubtreeRange();
               documentReplacements[range.mSource].push_back(RangeAndString(range, mRenameMap[pn.GetPath()]));
            }
         }
      }
      //       else if ((n->GetFlags() & WsfParseNode::cLAZY_TYPE_REFERENCE_NODE) && n->mType != 0 && n->mType[0] != '.')
      //       {
      //          WsfPProxyNode ref = rootNode;
      //          ref += n->mType;
      //          ref += n->GetTextValue();
      //          if (mRenameMap.find(ref.GetPath()) != mRenameMap.end())
      //          {
      //             UtTextDocumentRange range = n->SubtreeRange();
      //             documentReplacements[range.mSource].push_back(RangeAndString(range, mRenameMap[ref.GetPath()]));
      //          }
      //       }
      n = n->Next();
   }

   if (!documentReplacements.empty())
   {
      for (DocumentReplacementMap::iterator i = documentReplacements.begin(); i != documentReplacements.end(); ++i)
      {
         UtTextDocument*              docPtr       = i->first;
         std::vector<RangeAndString>& replacements = i->second;
         std::sort(replacements.begin(), replacements.end(), CompareRangeOnly);
         for (auto r = replacements.rbegin(); r != replacements.rend(); ++r)
         {
            RangeAndString& rep = *r;
            docPtr->Erase(rep.first.GetBegin(), rep.first.Length());
            docPtr->Insert(rep.first.GetBegin(), rep.second);
         }
         aChangedFiles.push_back(docPtr);
      }
   }

   mParserPtr->DeleteAllNodes();
}

// Computes changes to input files which will satisfy the new proxy root value.
// Does not change any input files until Finalize() is called so that
// input documents can be shared by another thread in read-only mode.
void WsfPProxySerialize::Serialize(WsfPProxyValue aNewValue)
{
   // Can't serialize if there are no files
   if (mFiles.empty())
   {
      return;
   }

   // mParserPtr->SetFullyExpandNodes(true);
   WsfParseNode* treePtr = mParserPtr->ParseFiles(mFiles, 0);
   {
      std::vector<WsfParseNode*> fileTransitionNodes;
      mParserPtr->FinalizeParseTree(treePtr, fileTransitionNodes);
   }
   std::vector<UtStringRef> includeDirectories = mParserPtr->GetParseIndex()->GetIncludePath();
   UtPath                   workingDir         = mParserPtr->GetWorkingDirectory();

   //   mParserPtr->SetFullyExpandNodes(false);

   ////////////////////////////////////////////////////////////////////////
   // Build up the proxy for the current state of input files
   ////////////////////////////////////////////////////////////////////////
   WsfPProxyValue existingProxy;
   {
      WsfPProxyDeserialize deserialize(mProxyRegistry, nullptr);
      existingProxy = WsfPProxyValue::ConstructNew(mProxyRegistry->GetType("root"));
      WsfPProxyUndoValue existingProxyUndo(existingProxy);
      existingProxyUndo.DisableUndo();
      deserialize.Deserialize(existingProxyUndo, treePtr);
   }

   mProxyIndex                     = new WsfPProxyIndex;
   mProxyIndex->mRecordCurrentPath = true;
   WsfPProxyDeserialize deserialize(mProxyRegistry, mProxyIndex);

   auto               oldProxy = WsfPProxyValue::ConstructNew(mProxyRegistry->GetType("root"));
   WsfPProxyUndoValue oldProxyUndo(oldProxy);
   mDeserializeTracer        = new WsfPProxyDeserializeTracer(oldProxyUndo, aNewValue, existingProxy, mProxyRegistry);
   mTracer                   = &mDeserializeTracer->mNodeTracer;
   deserialize.mTraceBuilder = mDeserializeTracer;
   deserialize.mObserver     = mDeserializeTracer;

   WsfPProxySatisfy sat(mProxyRegistry, aNewValue, oldProxyUndo, mTracer);
   mDeserializeTracer->mSatisfy = &sat;

   // oldProxyUndo.DisableUndo();B
   deserialize.Deserialize(oldProxyUndo, treePtr);

   mDeserializeTracer->FinalizeLists();

   mProxyIndex->BuildReverseIndex();
   WsfPParseRuleTrace* tracePtr = mDeserializeTracer->mNodeTracer.Root();

   // WsfPParseRuleTrace* tracePtr = WsfPParseRuleTrace::BuildTrace(treePtr, &mTracer, *mProxyIndex);
   mTracer->Finalize(tracePtr);
   sat.mTracePtr = mTracer->Root();
   sat.SatisfyRootDifferences();

   // Apply changes to the input files

   mFileWriter.SetRootFile(mParserPtr->FindSource(mFiles[0].GetSystemPath()));
   mFileWriter.SetIncludeDirectories(workingDir, includeDirectories);
   mFileWriter.SetSourceProvider(mParserPtr->GetSourceProvider());
   mFileWriter.WriteTrace(tracePtr);
   mFileWriter.ApplyChanges();

   oldProxy.Delete();
   existingProxy.Delete();
}

// Lists documents that will be modified when Finalize() is called
// Must be called only after Serialize()
void WsfPProxySerialize::GetChangedFiles(std::vector<UtTextDocument*>& aTextDocumentList)
{
   const auto& docCopies = mFileWriter.GetDocumentCopies();
   for (auto i = docCopies.begin(); i != docCopies.end(); ++i)
   {
      aTextDocumentList.push_back(i->first);
   }
}

// Finalizes the serialization by writing changes to file buffers,
// re-parsing and formatting new commands
void WsfPProxySerialize::Finalize()
{
   // Copy changed files
   const auto& docCopies = mFileWriter.GetDocumentCopies();
   for (auto i = docCopies.begin(); i != docCopies.end(); ++i)
   {
      i->first->Clear();
      i->first->Insert(0, i->second->GetPointer(), i->second->Size());
      // i->first->ValidateLines();
   }

   // Re-parse files and pretty-format new commands
   WsfParseNode* newTreePtr = mParserPtr->ParseFiles(mFiles, 0);
   if (newTreePtr)
   {
      std::vector<WsfParseNode*> fileTransitionNodes;
      mParserPtr->FinalizeParseTree(newTreePtr, fileTransitionNodes);

      // format source
      WsfParseFormat                          formatter;
      std::set<WsfParseNode*>                 changedNodes;
      const std::vector<UtTextDocumentRange>& newRanges = mFileWriter.GetNewRanges();
      for (size_t i = 0; i < newRanges.size(); ++i)
      {
         std::vector<WsfParseNode*> rangeNodes;
         WsfParseNode::GetRangeNodes(newTreePtr, newRanges[i], fileTransitionNodes, rangeNodes);
         changedNodes.insert(rangeNodes.begin(), rangeNodes.end());
      }
      formatter.Format(changedNodes);
   }


   for (auto i = docCopies.begin(); i != docCopies.end(); ++i)
   {
      i->first->ValidateLines();
   }
}

WsfPProxyNode WsfPProxySerialize::Lookup(const WsfPProxyNode&    aNode,
                                         const WsfParseTypeName& aTypeName,
                                         WsfParseNode*           aParseNodePtr)
{
   WsfPProxyNode referencedNode = (aTypeName.mNestedLookup ? aNode : aNode.Root());
   if (aTypeName.mOrdinal == ut::npos)
   {
      referencedNode += aTypeName.mPath;
   }
   else
   {
      for (size_t i = 0; i + 1 < aTypeName.mPath.size(); ++i)
      {
         referencedNode += aTypeName.mPath[i].Get();
      }
      if (aParseNodePtr)
      {
         WsfParseNode* nodePtr = aParseNodePtr->GetChild(aTypeName.mOrdinal);
         if (nodePtr)
         {
            referencedNode += nodePtr->GetTextValue();
         }
      }
   }
   return referencedNode;
}

void WsfPProxySerialize::SetSaveFiles(const std::map<WsfPProxyPath, std::string>& aSaveFiles)
{
   mFileWriter.SetSaveFiles(aSaveFiles);
}
