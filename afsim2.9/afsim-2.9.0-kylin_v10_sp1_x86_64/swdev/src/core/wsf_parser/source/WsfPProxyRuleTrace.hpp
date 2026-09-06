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

#ifndef WSFPPROXYRULETRACE_HPP
#define WSFPPROXYRULETRACE_HPP

#include <map>

#include "UtCast.hpp"
#include "UtSortedVector.hpp"
#include "WsfPProxyI.hpp"
#include "WsfPProxyPath.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfParseRule.hpp"

class WsfPProxyIndex;
class WsfPParseRuleTrace;
class WsfPParseRuleTraceContext;


class WsfPParseRuleTracer
{
public:
   struct Contexts
   {
      std::vector<std::unique_ptr<WsfPParseRuleTraceContext>> mContextList;
   };

   WsfPParseRuleTracer(WsfPProxyRegistry* aRegistry);
   ~WsfPParseRuleTracer();
   void      ContextAdded(std::unique_ptr<WsfPParseRuleTraceContext> aContext);
   void      ContextRemoved(WsfPParseRuleTraceContext* aContext);
   Contexts* FindContexts(const WsfPProxyPath& aPath);

   WsfPParseRuleTrace* FindTrace(WsfParseNode* aNodePtr);

   void Finalize(WsfPParseRuleTrace* aTraceRoot);

   WsfPParseRuleTrace* Root() { return mRoot; }

   void BeginNode(WsfParseNode* aNodePtr, const WsfPProxyPath& aCurrentPath);

   WsfPParseRuleTrace* EndNode(WsfParseNode* aNodePtr);

   const std::vector<WsfPParseRuleTrace*>& GetCurrentNodes() const { return mBuildNodeStack; }
   WsfParseRule*                           GetRootRule() const { return mRootRule; }

protected:
   void                                        AddToIndex(WsfPParseRuleTrace* aTracePtr);
   UtSortedVector<size_t, WsfPParseRuleTrace*> mNodeIndexToTrace;
   WsfPParseRuleTrace*                         mRoot;
   std::map<WsfPProxyPath, Contexts>           mContextMap;


   std::vector<WsfPParseRuleTrace*>             mBuildNodeStack;
   std::map<WsfParseNode*, WsfPParseRuleTrace*> mNodeToTrace;
   WsfParseRule*                                mRootRule;
};


class WsfPParseRuleTrace
{
public:
   friend class WsfPParseRuleTracer;

   //       static WsfPParseRuleTrace* BuildTrace(WsfParseNode*        aNodePtr,
   //                                             WsfPParseRuleTracer* aTracerPtr,
   //                                             WsfPProxyIndex&      aProxy);

   WsfPParseRuleTrace(WsfPParseRuleTracer* aTracer);
   ~WsfPParseRuleTrace();
   enum Flags
   {
      cHAS_INSERT_TEXT    = 1,
      cPRUNED             = 2,
      cINTERMEDIATE_TRACE = 4
   };
   WsfPParseRuleTrace* AddChild(WsfPParseRuleTrace* aChild, size_t aIndex = ut::npos);
   WsfPParseRuleTrace* AddChild(WsfParseRule* aRulePtr, size_t aIndex = ut::npos);
   WsfPParseRuleTrace* PopChild(size_t aChildIndex = ut::npos)
   {
      if (aChildIndex == ut::npos)
      {
         aChildIndex = mChildren.size() - 1;
      }
      WsfPParseRuleTrace* child = mChildren[aChildIndex];
      mChildren.erase(mChildren.begin() + aChildIndex);
      return child;
   }
   WsfPParseRuleTrace* LastChild() { return mChildren.back(); }
   WsfPParseRuleTrace* FindRootTrace()
   {
      if (mParentPtr)
      {
         WsfPParseRuleTrace* rootTrace = mParentPtr->FindRootTrace();
         if (rootTrace)
         {
            return rootTrace;
         }
         if (mRulePtr)
         {
            return this;
         }
      }
      return nullptr;
   }
   void SetInsertText(const std::string& aInsertText)
   {
      mFlags |= cHAS_INSERT_TEXT;
      mInsertText = aInsertText;
   }
   bool         HasInsertText() { return mFlags & cHAS_INSERT_TEXT; }
   std::string& GetInsertText() { return mInsertText; }
   std::string  GetText();
   void         SetPruned();
   bool         IsPruned() const { return 0 != (mFlags & cPRUNED); }

   WsfParseNode* GetNearestNode()
   {
      if (mNodePtr)
      {
         return mNodePtr;
      }
      for (size_t i = 0; i < mChildren.size(); ++i)
      {
         WsfParseNode* nn = mChildren[i]->GetNearestNode();
         if (nn)
         {
            return nn;
         }
      }
      return nullptr;
      // if (! mParentPtr) return 0;
      // return mParentPtr->GetNearestNode();
   }
   const std::vector<WsfPParseRuleTrace*>& Children() const { return mChildren; }
   size_t                                  UnprunedChildCount() const
   {
      size_t ct = 0;
      for (size_t i = 0; i < mChildren.size(); ++i)
      {
         if (!mChildren[i]->IsPruned())
         {
            ++ct;
         }
      }
      return ct;
   }
   WsfParseRule* Rule() const { return mRulePtr; }
   WsfParseRule* ActualRule() const
   {
      return (mRulePtr->GetReaderFlags() & WsfParseRule::cIS_PASSTHROUGH) ? mRulePtr->GetPassthrough() : mRulePtr;
   }
   void SetRule(WsfParseRule* aRulePtr) { mRulePtr = aRulePtr; }

   WsfPParseRuleTracer&       Tracer() { return *mTracerPtr; }
   WsfPParseRuleTrace*        GetParent() { return mParentPtr; }
   WsfParseNode*              GetActualNode();
   WsfPParseRuleTraceContext* GetContext() { return mContextPtr; }
   WsfPParseRuleTraceContext* EnclosingContext();
   void                       SetContext(WsfPParseRuleTraceContext* aContext)
   {
      assert(mContextPtr == nullptr);
      mContextPtr = aContext;
   }
   void                NewContext(const WsfPProxyPath& aPath);
   WsfPParseRuleTrace* GetTraceRoot()
   {
      if (mRulePtr == mTracerPtr->GetRootRule())
      {
         return this;
      }
      if (mParentPtr)
      {
         return mParentPtr->GetTraceRoot();
      }
      return nullptr;
   }
   // std::vector<WsfPProxyPath>       mDefinitions;

protected:
   void SetPrunedP();

   unsigned int               mFlags;
   WsfPParseRuleTracer*       mTracerPtr;
   WsfPParseRuleTrace*        mParentPtr;
   WsfPParseRuleTraceContext* mContextPtr;
   WsfParseRule*              mRulePtr;

public:
   WsfParseNode* mNodePtr;

protected:
   std::vector<WsfPParseRuleTrace*> mChildren;
   std::string                      mInsertText;
};

class WsfPParseRuleTraceContext
{
public:
   WsfPParseRuleTraceContext(WsfPProxyPath* aAddr = nullptr)
      : mTracePtr(nullptr)
      , mParent(nullptr)
      , mAddr(aAddr)
   {
   }
   ~WsfPParseRuleTraceContext();
   const WsfPProxyPath&       GetAddr() const;
   WsfPParseRuleTraceContext* GetParent() const;

   bool HasAncestor(WsfPParseRuleTraceContext* aContextPtr)
   {
      for (auto* c = this; c != nullptr; c = c->GetParent())
      {
         if (c == aContextPtr)
         {
            return true;
         }
      }
      return false;
   }
   WsfPParseRuleTrace* Trace() { return mTracePtr; }
   WsfPParseRuleTrace* mTracePtr;

private:
   mutable WsfPParseRuleTraceContext* mParent;
   WsfPProxyPath*                     mAddr;
};


#endif
