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

#ifndef WSFPPROXYINDEX_HPP
#define WSFPPROXYINDEX_HPP

#include "wsf_parser_export.h"

#include <map>
#include <memory>
#include <unordered_map>

#include "UtMemory.hpp"
#include "UtTextDocument.hpp"
#include "WsfPProxyPath.hpp"
class WsfPProxyIndexNode;

class WsfParseNode;
struct WsfPProxyIndexEntry
{
   int mEntryType;
   // If set, this is a parse node containing commands that operate on this proxy value
   WsfParseNode* mNodePtr;
};

class WSF_PARSER_EXPORT WsfPProxyIndexNode
{
public:
   WsfPProxyIndexNode(WsfPProxyIndexNode* aParentPtr, const WsfPProxyKey& e)
      : mAddr(e)
      , mParentPtr(aParentPtr)
   {
   }

   WsfPProxyIndexNode(const WsfPProxyIndexNode&) = delete;
   WsfPProxyIndexNode& operator=(const WsfPProxyIndexNode&) = delete;

   ~WsfPProxyIndexNode() {}

   WsfPProxyIndexNode* FindChild(const WsfPProxyKey& e)
   {
      auto iter = mChildren.find(e);
      if (iter != mChildren.end())
      {
         return iter->second.get();
      }
      return nullptr;
   }

   WsfPProxyIndexNode& GetChild(const WsfPProxyKey& e)
   {
      auto it = mChildren.find(e);
      if (it == mChildren.end())
      {
         std::tie(it, std::ignore) = mChildren.insert(std::make_pair(e, ut::make_unique<WsfPProxyIndexNode>(this, e)));
      }
      return *(it->second);
   }

   void AddLocation(WsfParseNode* aNodePtr, int aEntryType)
   {
      WsfPProxyIndexEntry e;
      e.mNodePtr   = aNodePtr;
      e.mEntryType = aEntryType;
      mEntries.push_back(e);
   }
   void GetPath(WsfPProxyPath& aPath)
   {
      if (mParentPtr)
      {
         mParentPtr->GetPath(aPath);
         aPath += mAddr;
      }
   }
   WsfPProxyKey                     mAddr;
   WsfPProxyIndexNode*              mParentPtr;
   std::vector<WsfPProxyIndexEntry> mEntries;
   using NodeMap = std::unordered_map<WsfPProxyKey, std::unique_ptr<WsfPProxyIndexNode>, WsfPProxyPathEntryHash>;
   NodeMap mChildren;

private:
};

class WSF_PARSER_EXPORT WsfPProxyIndex
{
public:
   enum EntryType
   {
      cADDED,
      cEDITED,
      cASSIGNED,
      cREMOVED
   };
   WsfPProxyIndex();
   ~WsfPProxyIndex();
   WsfPProxyIndexNode* Find(const WsfPProxyPath& aPath);
   WsfPProxyIndexNode& Get(const WsfPProxyPath& aPath);
   WsfPProxyIndexNode& Root() { return mRoot; }

   void SetCurrentProxyPath(const WsfPProxyPath& aPath, WsfParseNode* aNodePtr);

   void BuildReverseIndex();

   struct ReverseIndexEntry
   {
      bool                   operator<(const ReverseIndexEntry& aRhs) const { return mLocation < aRhs.mLocation; }
      UtTextDocumentLocation mLocation;
      WsfPProxyIndexNode*    mNodePtr;
      size_t                 mLexicalIndex;
      size_t                 mEntryIndex;
   };
   const std::vector<ReverseIndexEntry>& GetReverseIndex() const { return mReverseIndex; }

   ReverseIndexEntry* FindByParseNodeIndex(size_t& aParseNodeIndex, size_t& aSubEntry);

   const WsfPProxyPath* FindCurrentPath(WsfParseNode* aNodePtr);
   bool                 mRecordCurrentPath;

private:
   void BuildReverseIndexR(WsfPProxyIndexNode& aNode);

   //! Given a location in a file, this provides quick access to the proxy changes being made
   std::vector<ReverseIndexEntry> mReverseIndex;
   struct NodeIndexMapping
   {
      NodeIndexMapping()
         : mLexicalIndex(0)
         , mReverseIndex(0)
         , mEntryIndex(0)
      {
      }
      NodeIndexMapping(size_t aLexicalIndex);

      bool operator<(const NodeIndexMapping& aRhs) const { return mLexicalIndex < aRhs.mLexicalIndex; }
      bool operator<(size_t aLexicalIndex) const { return mLexicalIndex < aLexicalIndex; }

      size_t mLexicalIndex;
      size_t mReverseIndex;
      size_t mEntryIndex;
   };
   //! Given a parse node index, this provides quick access to the proxy changes being made
   std::vector<NodeIndexMapping> mNodeIndexMappingList;

   //! Root of the tree storing information about proxy changes.
   WsfPProxyIndexNode mRoot;

   // WsfPProxyPath                          mRecordNodeCurrentPath;
   WsfPProxyPath*                   mEmptyPath;
   std::map<size_t, WsfPProxyPath*> mNodeCurrentPaths;

   UT_MEMORY_DEBUG_MARKER(cMDB_PROXY_INDEX);
};

#endif
