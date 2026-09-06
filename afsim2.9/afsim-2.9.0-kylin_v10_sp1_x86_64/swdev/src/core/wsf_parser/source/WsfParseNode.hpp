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

#ifndef WSFPARSENODE_HPP
#define WSFPARSENODE_HPP

#include "wsf_parser_export.h"

#include <cstdlib>
#include <vector>

#include "UtLog.hpp"
#include "UtMemoryPool.hpp"
#include "UtStringRef.hpp"
#include "UtTextDocument.hpp"
#include "WsfParseAuxData.hpp"
#include "WsfParseDebugMarkers.hpp"
#include "WsfParseRule.hpp"

class WsfParseNodePool;

class WSF_PARSER_EXPORT WsfParseFileReferenceData : public WsfParseAuxData
{
public:
   WsfParseFileReferenceData(const std::string& aFilePath)
      : WsfParseAuxData(cFILE_REFERENCE)
      , mFilePath(aFilePath)
   {
   }
   ~WsfParseFileReferenceData() override = default;
   UtStringRef mFilePath;
};

class WSF_PARSER_EXPORT WsfParseDelayLoadData : public WsfParseAuxData
{
public:
   WsfParseDelayLoadData()
      : WsfParseAuxData(cDELAY_LOAD_DATA)
      , mLoadOrder(ut::npos)
   {
   }
   size_t mLoadOrder;
};

class WsfParseNodeP;

//! A node resulting from WsfParser execution.  Each node represents
//! a matched rule from the grammar.
class WSF_PARSER_EXPORT WsfParseNode
{
public:
   friend class WsfParseNodePool;
   friend class WsfParseNodeP;

   void                Add(WsfParseNode* aChild);
   void                MarkIncomplete() { mFlags |= cERROR_INCOMPLETE; }
   void                SetFlags(int aMask) { mFlags |= aMask; }
   int                 GetFlags() const { return mFlags; }
   void                DebugPrint(ut::log::MessageStream& aStream);
   WsfParseNode*       Next(bool aRecurseDown = true) const;
   WsfParseNode*       NextLeaf(bool aRecurseDown = true) const;
   WsfParseNode*       PreviousLeaf() const;
   WsfParseNode*       Previous() const;
   WsfParseNode*       FindChildByType(const std::string& aType) const;
   UtTextRange         SubtreeRange(UtTextDocument* aSource);
   UtTextDocumentRange SubtreeRange();
   UtTextRange         GetBoundingRange(UtTextDocument* aSource);
   WsfParseNode*       GetLeftmostValue(UtTextDocument* aSourcePtr);
   WsfParseNode*       GetRightmostValue(UtTextDocument* aSourcePtr);
   WsfParseNode*       GetLeftmostValue();
   WsfParseNode*       GetRightmostValue();
   WsfParseNode*       GetRightmostLeaf();
   WsfParseNode*       GetLeftmostLeaf();
   WsfParseAuxData*    GetAuxiliaryValue() const { return mAuxiliaryValue; }
   // Note: the auxiliary value's memory must be managed elsewhere.
   //       The usual place is WsfParser.AddAuxiliaryValue()
   void          SetAuxiliaryValue(WsfParseAuxData* aValuePtr) { mAuxiliaryValue = aValuePtr; }
   WsfParseNode* GetParent() const { return mParent; }
   WsfParseNode* GetChild(size_t aIndex) const;
   void          Detatch();
   void          Replace(WsfParseNode* aNodePtr);

   void          ResetLinks();
   WsfParseNode* Right() { return mRight; }
   WsfParseNode* FarRight();
   WsfParseNode* Left() { return mLeft; }
   WsfParseNode* FarLeft();
   WsfParseNode* Down() { return mDown; }
   void          InsertBefore(WsfParseNode* aNodePtr);
   void          InsertAfter(WsfParseNode* aNodePtr);
   size_t        LexicalIndex() const { return mLexicalIndex; }
   size_t        LoadOrder() const;
   void          SetLexicalIndex(size_t aLexicalIndex) { mLexicalIndex = aLexicalIndex; }

   static bool   GetRangeNodes(WsfParseNode*                     aRootNodePtr,
                               const UtTextDocumentRange&        aRange,
                               const std::vector<WsfParseNode*>& aFileTransitionNodes,
                               std::vector<WsfParseNode*>&       aNodes);
   WsfParseRule* GetTopLevelRule() const;
   void          MarkPruned();

   std::string GetTextValue();

   void Print(std::ostream& aStream, int aIndent, bool aVerbose);

   static std::string GetNodeString(WsfParseNode* aNodePtr);

   enum Flags
   {
      //! Flag indicates the node is the result of a partially matched rule
      cERROR_INCOMPLETE         = 0x1,
      cTYPE_NAME_NODE           = 0x2,
      cTYPE_REFERENCE_NODE      = 0x4,
      cTYPE_MASK                = cTYPE_NAME_NODE | cTYPE_REFERENCE_NODE,
      cNAMED_NODE               = 0x8,
      cLOAD_TYPE_NODE           = 0x10,
      cLAZY_TYPE_REFERENCE_NODE = 0x20,
      cLAZY_TYPENAME_NODE       = 0x40,
      cLAZY_TYPE_MASK           = cLAZY_TYPE_REFERENCE_NODE | cLAZY_TYPENAME_NODE,
      cBLOCK_NODE               = 0x80,
      cSCRIPT_BLOCK             = 0x100,
      cSCRIPT_FUNCTION          = 0x200,
      cSCRIPT_VARIABLES         = 0x400,
      cSCRIPT_MASK              = 0x700,
      cDELAY_LOAD               = 0x800,
      cBLOCK_START              = 0x1000,
      cBLOCK_END                = 0x2000,
      // Flags used for serialization only:
      cPRUNE_NODE = 0x4000
   };
   //! Bit-set of flags for the node
   int mFlags;

   //! The WsfParseReader object which created this node.
   WsfParseRule* mRulePtr;
   //! String indicating the type of node.  Either a user-defined string from the grammar
   //! file, or a predefined type.  Ex: 'real' or 'platform-part-command'
   const char*      mType;
   WsfParseAuxData* mAuxiliaryValue;
   //! Leaf nodes in the parse tree should have a value.  The value is the range
   //! of characters matched by the rule.  User should verify the range is valid
   //! before using.
   UtTextDocumentRange mValue;

protected:
   WsfParseNode(WsfParseRule* aReader, const char* aType, UtTextDocumentRange aValue = UtTextDocumentRange())
      : mFlags()
      , mRulePtr(aReader)
      , mType(aType)
      , mAuxiliaryValue()
      , mValue(aValue)
      , mParent()
      , mRight()
      , mLeft()
      , mDown()
      , mLexicalIndex(ut::npos)
   {
   }
   WsfParseNode()
      : mFlags()
      , mRulePtr()
      , mType()
      , mAuxiliaryValue()
      , mValue()
      , mParent()
      , mRight()
      , mLeft()
      , mDown()
      , mLexicalIndex(ut::npos)
   {
   }
   ~WsfParseNode()                   = default;
   WsfParseNode(const WsfParseNode&) = default; // copy constructor

private:
   WsfParseNode* NextP(WsfParseNode* aPreviousChild);

   //! Parent node in the parse tree.  Null for the root.
   WsfParseNode* mParent;
   //! Next sibling
   WsfParseNode* mRight;
   //! Previous sibling
   WsfParseNode* mLeft;
   //! First Child
   WsfParseNode* mDown;
   //! Index of this node in order of text file traversal
   size_t mLexicalIndex;
};


//! To improve efficiency, WsfParseNodePool handles the allocation and deletion of WsfParseNode's.
class WSF_PARSER_EXPORT WsfParseNodePool
{
   UtMemoryPool* mAlloc;

   // This intermediate class merely allows access to the constructor
   class WSF_PARSER_EXPORT WsfParseNodeP : public WsfParseNode
   {
   public:
      WsfParseNodeP(WsfParseRule* aReader, const char* aType, UtTextDocumentRange aValue = UtTextDocumentRange())
         : WsfParseNode(aReader, aType, aValue)
      {
      }
      WsfParseNodeP()
         : WsfParseNode()
      {
      }
   };

public:
   WsfParseNodePool() { mAlloc = new UtMemoryPool(sizeof(WsfParseNodeP)); }
   ~WsfParseNodePool()
   {
      delete mAlloc;
      for (size_t i = 0; i < mAuxVals.size(); ++i)
      {
         delete mAuxVals[i];
      }
   }
   WsfParseNodePool(const WsfParseNodePool&) = default; // copy constructor

   void DeleteAllNodes()
   {
#ifndef NDEBUG
      mAlloc->DebugClear();
#endif
      mAlloc->Reset();
      for (size_t i = 0; i < mAuxVals.size(); ++i)
      {
         delete mAuxVals[i];
      }
      mAuxVals.clear();
   }

   void SwapPool(WsfParseNodePool& aRhs)
   {
      std::swap(mAlloc, aRhs.mAlloc);
      std::swap(mAuxVals, aRhs.mAuxVals);
   }

   //! Create a new parse node.
   WsfParseNode* NewNode(WsfParseRule* aRule, const char* aType, UtTextDocumentRange aValue = UtTextDocumentRange())
   {
      void* memPtr = mAlloc->Alloc();
      if (!memPtr)
      {
         std::string ruleDescription = "root-commands";
         if (aRule)
         {
            ruleDescription = aRule->GetRuleDescription();
         }
         auto logger = ut::log::error() << "Memory allocation failed on parsing.";
         logger.AddNote() << "Rule: " << ruleDescription;
         logger.AddNote() << "File: " << aValue.mSource->GetFilePath().GetFileName();
         throw std::bad_alloc();
      }
      return new (memPtr) WsfParseNodeP(aRule, aType, aValue);
   }

   //! Create a new parse node.
   WsfParseNode* NewNode()
   {
      void* memPtr = mAlloc->Alloc();
      if (!memPtr)
      {
         ut::log::error() << "Memory allocation failure.";
         throw std::bad_alloc();
      }
      return new (memPtr) WsfParseNodeP();
   }

   void FreeNode(WsfParseNode* aNodePtr)
   {
      // Currently, there is no way to free a single node.  This is a stub to keep track of places
      // that nodes are no longer needed.
   }

   // Because WsfParseNode does not have a destructor, the lifetime of WsfParseAuxData objects is handled by WsfParseNodePool
   void                          AddAuxiliaryValue(WsfParseAuxData* aDataPtr) { mAuxVals.push_back(aDataPtr); }
   std::vector<WsfParseAuxData*> mAuxVals;
   UT_MEMORY_DEBUG_MARKER(cMDB_PARSE_NODE_POOL);
};

using WsfParseNodeList = std::vector<WsfParseNode*>;

#endif
