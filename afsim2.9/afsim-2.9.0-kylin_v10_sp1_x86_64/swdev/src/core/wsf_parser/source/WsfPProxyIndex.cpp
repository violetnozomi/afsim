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

#include "WsfPProxyIndex.hpp"

#include <limits>

#include "UtCast.hpp"
#include "WsfParseNode.hpp"

WsfPProxyIndexNode* WsfPProxyIndex::Find(const WsfPProxyPath& aPath)
{
   WsfPProxyIndexNode* root       = &mRoot;
   size_t              entryCount = aPath.size();
   for (size_t i = 0; i < entryCount && root; ++i)
   {
      const WsfPProxyKey& e = aPath[i];
      root                  = root->FindChild(e);
   }
   return root;
}

WsfPProxyIndexNode& WsfPProxyIndex::Get(const WsfPProxyPath& aPath)
{
   WsfPProxyIndexNode* root       = &mRoot;
   size_t              entryCount = aPath.size();
   for (size_t i = 0; i < entryCount; ++i)
   {
      const WsfPProxyKey& e = aPath[i];
      root                  = &root->GetChild(e);
   }
   return *root;
}

void WsfPProxyIndex::BuildReverseIndex()
{
   BuildReverseIndexR(mRoot);
   std::sort(mReverseIndex.begin(), mReverseIndex.end());

   // Build a mapping from parse node index to a reverse proxy index
   mNodeIndexMappingList.resize(mReverseIndex.size());
   for (size_t i = 0; i < mReverseIndex.size(); ++i)
   {
      ReverseIndexEntry& entry = mReverseIndex[i];
      NodeIndexMapping&  nim   = mNodeIndexMappingList[i];
      nim.mLexicalIndex        = entry.mLexicalIndex;
      nim.mReverseIndex        = i;
      nim.mEntryIndex          = entry.mEntryIndex;
   }
   std::sort(mNodeIndexMappingList.begin(), mNodeIndexMappingList.end());
}

void WsfPProxyIndex::BuildReverseIndexR(WsfPProxyIndexNode& aNode)
{
   for (size_t i = 0; i < aNode.mEntries.size(); ++i)
   {
      WsfPProxyIndexEntry& e      = aNode.mEntries[i];
      WsfParseNode*        valPtr = e.mNodePtr->GetLeftmostValue();
      if (!valPtr && e.mNodePtr->Previous()) // For type load node, they do not have children.  In that case, just use
                                             // the value to the left.
      {
         valPtr = e.mNodePtr->Previous()->GetLeftmostValue();
      }
      if (valPtr)
      {
         ReverseIndexEntry entry;
         entry.mLocation     = valPtr->mValue.GetPosition();
         entry.mLexicalIndex = valPtr->LexicalIndex();
         entry.mEntryIndex   = i;
         entry.mNodePtr      = &aNode;
         mReverseIndex.push_back(entry);
      }
   }
   for (WsfPProxyIndexNode::NodeMap::iterator iter = aNode.mChildren.begin(); iter != aNode.mChildren.end(); ++iter)
   {
      BuildReverseIndexR(*iter->second);
   }
}

WsfPProxyIndex::ReverseIndexEntry* WsfPProxyIndex::FindByParseNodeIndex(size_t& aParseNodeIndex, size_t& aSubEntry)
{
   if (mNodeIndexMappingList.empty())
   {
      return nullptr;
   }

   std::vector<NodeIndexMapping>::iterator iter =
      std::lower_bound(mNodeIndexMappingList.begin(), mNodeIndexMappingList.end(), NodeIndexMapping(aParseNodeIndex));
   if (iter == mNodeIndexMappingList.end())
   {
      return nullptr;
   }
   aParseNodeIndex = iter->mLexicalIndex;
   aSubEntry       = iter->mEntryIndex;
   return &mReverseIndex[iter->mReverseIndex];
}

void WsfPProxyIndex::SetCurrentProxyPath(const WsfPProxyPath& aPath, WsfParseNode* aNodePtr)
{
   if (aPath.Empty())
   {
      mNodeCurrentPaths[aNodePtr->LexicalIndex()] = mEmptyPath;
   }
   else
   {
      WsfPProxyPath* newPath = nullptr;
      if (aNodePtr->GetParent())
      {
         auto i = mNodeCurrentPaths.find(aNodePtr->GetParent()->LexicalIndex());
         if (i != mNodeCurrentPaths.end() && i->second)
         {
            if (*i->second == aPath)
            {
               newPath = i->second;
            }
         }
      }
      if (!newPath)
      {
         newPath = new WsfPProxyPath(aPath);
      }
      mNodeCurrentPaths[aNodePtr->LexicalIndex()] = newPath;
   }
}

WsfPProxyIndex::~WsfPProxyIndex()
{
   // Delete all paths stored in mNodeCurrentPaths
   // Because some are reused, we have to first create a unique list
   std::vector<WsfPProxyPath*> uniquePaths;
   for (auto i = mNodeCurrentPaths.begin(); i != mNodeCurrentPaths.end(); ++i)
   {
      uniquePaths.push_back(i->second);
   }
   uniquePaths.push_back(mEmptyPath);
   std::sort(uniquePaths.begin(), uniquePaths.end());
   size_t uniqueCount = (std::unique(uniquePaths.begin(), uniquePaths.end()) - uniquePaths.begin());
   for (size_t i = 0; i < uniqueCount; ++i)
   {
      delete uniquePaths[i];
   }
}

WsfPProxyIndex::WsfPProxyIndex()
   : mRecordCurrentPath(false)
   , mRoot(nullptr, WsfPProxyKey())
{
   mEmptyPath = new WsfPProxyPath;
}

const WsfPProxyPath* WsfPProxyIndex::FindCurrentPath(WsfParseNode* aNodePtr)
{
   while (aNodePtr)
   {
      auto i = mNodeCurrentPaths.find(aNodePtr->LexicalIndex());
      if (i != mNodeCurrentPaths.end())
      {
         return i->second;
      }
      aNodePtr = aNodePtr->GetParent();
   }
   return mEmptyPath;
}

WsfPProxyIndex::NodeIndexMapping::NodeIndexMapping(size_t aLexicalIndex)
   : mLexicalIndex(aLexicalIndex)
   , mReverseIndex(std::numeric_limits<size_t>::max())
   , mEntryIndex(std::numeric_limits<size_t>::max())
{
}
