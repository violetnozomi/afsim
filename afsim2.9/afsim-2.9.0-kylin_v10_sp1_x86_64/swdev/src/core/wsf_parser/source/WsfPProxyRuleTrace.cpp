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

#include "WsfPProxyRuleTrace.hpp"

#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseSequence.hpp"

void VerifyChild(WsfParseRule* parent, WsfParseRule* child, int index)
{
   if (parent->GetSubordinateRule() == child)
   {
      return;
   }
   switch (parent->Type())
   {
   case WsfParseRule::cALTERNATE:
   {
      WsfParseAlternate*                alt  = (WsfParseAlternate*)parent;
      const std::vector<WsfParseRule*>& alts = alt->Alternates();
      for (size_t i = 0; i < alts.size(); ++i)
      {
         if (alts[i] == child)
         {
            return;
         }
      }
   }
   break;
   case WsfParseRule::cSEQUENCE:
   {
      WsfParseSequence* seq = (WsfParseSequence*)parent;
      if (seq->Sequence()[index] == child)
      {
         return;
      }
   }
   break;
   default:
      break;
   }
   assert(false);
}

bool IsAnAlternate(WsfParseAlternate* aAlt, WsfParseRule* aRulePtr)
{
   const std::vector<WsfParseRule*>& alts = aAlt->Alternates();
   for (size_t i = 0; i < alts.size(); ++i)
   {
      if (alts[i] == aRulePtr)
      {
         return true;
      }
   }
   return false;
}
//
// //static
// WsfPParseRuleTrace* WsfPParseRuleTrace::BuildTrace(WsfParseNode* aNodePtr, WsfPParseRuleTracer* aTracerPtr,
// WsfPProxyIndex& aProxyIndex)
// {
//    WsfPParseRuleTrace* rTracePtr = new WsfPParseRuleTrace(aTracerPtr);
//    WsfPParseRuleTrace* tracePtr = rTracePtr;
//    tracePtr->mNodePtr = aNodePtr;
//    tracePtr->mRulePtr = aNodePtr->mRulePtr;
//
//    bool expanding = true;
//    while (expanding)
//    {
//       // Some readers act as 'passthroughs' and obscure which reader is used to read a node.
//       // expand any passthroughs here:
//       if (tracePtr->mRulePtr && tracePtr->mRulePtr->GetReaderFlags() & WsfParseRule::cIS_PASSTHROUGH)
//       {
//          WsfParseRule* subRule = tracePtr->mRulePtr->GetPassthrough();
//          assert(subRule);
//          WsfPParseRuleTrace* sub = new WsfPParseRuleTrace(aTracerPtr);
//          sub->mRulePtr = subRule;
//          tracePtr->AddChild(sub);
//          std::swap(sub->mNodePtr, tracePtr->mNodePtr);
//          tracePtr = sub;
//          continue;
//       }
//       expanding = false;
//    }
//
//    // Add a 'context' if this is a recurrence node with up to 'many' recurrences.
//    // Search backward to find the active proxy value
//    if (tracePtr->mNodePtr && tracePtr->mRulePtr && tracePtr->mRulePtr->Type() == WsfParseRule::cRECURRENCE)
//    {
//       WsfParseRecurrence* rulePtr = (WsfParseRecurrence*)tracePtr->mRulePtr;
//       if (rulePtr->mCount == WsfParseRecurrence::cONE_TO_MANY
//        || rulePtr->mCount == WsfParseRecurrence::cZERO_TO_MANY)
//       {
//          const WsfPProxyPath* currentPath = aProxyIndex.FindCurrentPath(tracePtr->mNodePtr);
//          WsfPParseRuleTraceContext* ctx = new WsfPParseRuleTraceContext(new WsfPProxyPath(*currentPath));
//          ctx->mTracePtr = tracePtr;
//          tracePtr->mContextPtr = ctx;
//          aTracerPtr->ContextAdded(ctx);
//       }
//    }
//
//    WsfParseNode* n = aNodePtr->Down();
//    int childIndex = 0;
//    while (n)
//    {
//       WsfPParseRuleTrace* child = BuildTrace(n, aTracerPtr, aProxyIndex);
//       if (child)
//       {
//          tracePtr->AddChild(child);
//          if (tracePtr->mRulePtr && (tracePtr->mRulePtr->Type() != WsfParseRule::cTYPE_COMMAND))
//          {
//             //VerifyChild(tracePtr->mRulePtr, n->mRulePtr, childIndex);
//          }
//       }
//       ++childIndex;
//       n = n->Right();
//    }
//    return rTracePtr;
// }

std::string WsfPParseRuleTrace::GetText()
{
   if (HasInsertText())
   {
      return GetInsertText();
   }
   if (mNodePtr && mNodePtr->mValue.mSource)
   {
      return mNodePtr->mValue.Text();
   }
   return std::string();
}

WsfPParseRuleTrace::~WsfPParseRuleTrace()
{
   if (mContextPtr)
   {
      mTracerPtr->ContextRemoved(mContextPtr);
   }
   for (size_t i = 0; i < mChildren.size(); ++i)
   {
      delete mChildren[i];
   }
}


void WsfPParseRuleTrace::NewContext(const WsfPProxyPath& aPath)
{
   assert(mContextPtr == nullptr);
   auto ctx       = ut::make_unique<WsfPParseRuleTraceContext>(new WsfPProxyPath(aPath));
   ctx->mTracePtr = this;
   SetContext(ctx.get());
   mTracerPtr->ContextAdded(std::move(ctx));
}

WsfPParseRuleTraceContext* WsfPParseRuleTrace::EnclosingContext()
{
   WsfPParseRuleTrace* tracePtr = this;
   while (tracePtr)
   {
      if (tracePtr->mContextPtr)
      {
         return tracePtr->mContextPtr;
      }
      tracePtr = tracePtr->GetParent();
   }
   return nullptr;
}

void WsfPParseRuleTrace::SetPruned()
{
   WsfPParseRuleTrace* t = this;
   while (t->mParentPtr && (t->mParentPtr->mFlags & cINTERMEDIATE_TRACE) &&
          t->mParentPtr->mChildren[0] == t // need to document this, don't know why we need to
                                           // intermediate traces -- revisit intermediate traces...
                                           // Adding last check as assert was being triggered
   )
   {
      assert(t->mParentPtr->mChildren[0] == t);
      t = t->mParentPtr;
   }
   t->SetPrunedP();
   assert(IsPruned());
}

void WsfPParseRuleTrace::SetPrunedP()
{
   if (0 == (mFlags & cPRUNED))
   {
      mFlags |= cPRUNED;
      WsfParseNode* n = GetActualNode();
      if (n)
      {
         n->MarkPruned();
      }
      for (size_t i = 0; i < mChildren.size(); ++i)
      {
         mChildren[i]->SetPrunedP();
      }
   }
}

WsfParseNode* WsfPParseRuleTrace::GetActualNode()
{
   if (mNodePtr)
   {
      return mNodePtr;
   }
   if ((mFlags & cINTERMEDIATE_TRACE) && mChildren.size() > 0)
   {
      return mChildren[0]->GetActualNode();
   }
   return nullptr;
}

WsfPParseRuleTrace* WsfPParseRuleTrace::AddChild(WsfPParseRuleTrace* aChild, size_t aIndex /*= ut::npos */)
{
   if (aIndex == ut::npos)
   {
      mChildren.push_back(aChild);
   }
   else
   {
      mChildren.insert(mChildren.begin() + aIndex, aChild);
   }
   aChild->mParentPtr = this;
   if (IsPruned())
   {
      aChild->SetPruned();
   }
   else
   {
#if _DEBUG
      WsfPParseRuleTrace* t = this;
      while (t)
      {
         assert(!t->IsPruned());
         t = t->GetParent();
      }
#endif
   }
   //#if _DEBUG
   //   WsfParseNode* n = aChild->GetActualNode();
   //   if (n && n->GetParent())
   //   {
   //      bool found = false;
   //      WsfParseNode* pn = n->GetParent();
   //      WsfPParseRuleTrace* tp = this;
   //      while (tp)
   //      {
   //         if (tp->mNodePtr == pn)  { found = true; break; }
   //         tp = tp->mParentPtr;
   //      }
   //      assert(found);
   //   }
   //#endif
   return aChild;
}

WsfPParseRuleTrace* WsfPParseRuleTrace::AddChild(WsfParseRule* aRulePtr, size_t aIndex /*= ut::npos */)
{
   WsfPParseRuleTrace* sub = new WsfPParseRuleTrace(mTracerPtr);
   sub->mRulePtr           = aRulePtr;
   return AddChild(sub, aIndex);
}

WsfPParseRuleTrace::WsfPParseRuleTrace(WsfPParseRuleTracer* aTracer)
   : mFlags(0)
   , mTracerPtr(aTracer)
   , mParentPtr(nullptr)
   , mContextPtr(nullptr)
   , mRulePtr(nullptr)
   , mNodePtr(nullptr)
{
}


const WsfPProxyPath& WsfPParseRuleTraceContext::GetAddr() const
{
   if (mAddr)
   {
      return *mAddr;
   }
   if (GetParent())
   {
      return GetParent()->GetAddr();
   }
   static WsfPProxyPath empty;
   return empty;
}

WsfPParseRuleTraceContext* WsfPParseRuleTraceContext::GetParent() const
{
   if (mParent)
   {
      return mParent;
   }
   if (!mTracePtr)
   {
      return nullptr;
   }
   WsfPParseRuleTrace* tracePtr = mTracePtr->GetParent();
   while (tracePtr && !mParent)
   {
      if (tracePtr->GetContext())
      {
         mParent = tracePtr->GetContext();
      }
      tracePtr = tracePtr->GetParent();
   }
   return mParent;
}

WsfPParseRuleTraceContext::~WsfPParseRuleTraceContext()
{
   delete mAddr;
}

void WsfPParseRuleTracer::ContextAdded(std::unique_ptr<WsfPParseRuleTraceContext> aContext)
{
   Contexts& ctxs = mContextMap[aContext->GetAddr()];
   ctxs.mContextList.push_back(std::move(aContext));
}

void WsfPParseRuleTracer::ContextRemoved(WsfPParseRuleTraceContext* aContext)
{
   Contexts& ctxs = mContextMap[aContext->GetAddr()];
   for (auto it = ctxs.mContextList.rbegin(); it != ctxs.mContextList.rend(); ++it)
   {
      if (it->get() == aContext)
      {
         ctxs.mContextList.erase(std::next(it).base());
         return;
      }
   }
   assert(false); // never found context!
}

WsfPParseRuleTracer::Contexts* WsfPParseRuleTracer::FindContexts(const WsfPProxyPath& aPath)
{
   std::map<WsfPProxyPath, Contexts>::iterator i = mContextMap.find(aPath);
   if (i != mContextMap.end())
   {
      return &i->second;
   }
   return nullptr;
}

WsfPParseRuleTrace* WsfPParseRuleTracer::FindTrace(WsfParseNode* aNodePtr)
{
   auto i = mNodeIndexToTrace.find(aNodePtr->LexicalIndex());
   if (i != mNodeIndexToTrace.end())
   {
      return i->second;
   }
   return nullptr;
}

void WsfPParseRuleTracer::Finalize(WsfPParseRuleTrace* aTraceRoot)
{
   mRoot = aTraceRoot;
   AddToIndex(mRoot);
   mNodeIndexToTrace.sort();
}

void WsfPParseRuleTracer::AddToIndex(WsfPParseRuleTrace* aTracePtr)
{
   size_t childrenCount = aTracePtr->Children().size();
   if (aTracePtr->mNodePtr)
   {
      mNodeIndexToTrace.push_back(aTracePtr->mNodePtr->LexicalIndex(), aTracePtr);
   }
   for (size_t i = 0; i < childrenCount; ++i)
   {
      AddToIndex(aTracePtr->Children()[i]);
   }
}

void WsfPParseRuleTracer::BeginNode(WsfParseNode* aNodePtr, const WsfPProxyPath& aCurrentPath)
{
   if (!aNodePtr)
   {
      return;
   }
   WsfPParseRuleTrace* traceParent = nullptr;
   if (!mBuildNodeStack.empty())
   {
      traceParent = mBuildNodeStack.back();
   }


   WsfPParseRuleTrace* rTracePtr = nullptr;
   WsfPParseRuleTrace* tracePtr  = nullptr;
   if (traceParent == nullptr)
   {
      if (mNodeToTrace.find(aNodePtr) != mNodeToTrace.end())
      {
         rTracePtr = mNodeToTrace[aNodePtr];
      }
      else
      {
         if (mNodeToTrace.find(aNodePtr->GetParent()) != mNodeToTrace.end())
         {
            traceParent = mNodeToTrace[aNodePtr->GetParent()];
         }
         else
         {
            assert(mRoot == nullptr);
         }
      }
   }
   if (!rTracePtr)
   {
      rTracePtr = new WsfPParseRuleTrace(this);
      if (!mRoot)
      {
         mRoot = rTracePtr;
      }
      rTracePtr->mNodePtr    = aNodePtr;
      mNodeToTrace[aNodePtr] = rTracePtr;
      rTracePtr->mRulePtr    = aNodePtr->mRulePtr;
      tracePtr               = rTracePtr;

      bool expanding = true;
      while (expanding)
      {
         // Some readers act as 'passthroughs' and obscure which reader is used to read a node.
         // expand any passthroughs here:
         if (tracePtr->mRulePtr && tracePtr->mRulePtr->GetReaderFlags() & WsfParseRule::cIS_PASSTHROUGH)
         {
            WsfParseRule* subRule = tracePtr->mRulePtr->GetPassthrough();
            assert(subRule);
            WsfPParseRuleTrace* sub = new WsfPParseRuleTrace(this);
            sub->mRulePtr           = subRule;
            tracePtr->AddChild(sub);
            std::swap(sub->mNodePtr, tracePtr->mNodePtr);
            tracePtr->mFlags |= WsfPParseRuleTrace::cINTERMEDIATE_TRACE;
            tracePtr = sub;
            continue;
         }
         expanding = false;
      }
   }


   mBuildNodeStack.push_back(tracePtr);

   // Add a 'context' if this is a recurrence node with up to 'many' recurrences.
   // Search backward to find the active proxy value
   if (tracePtr->mNodePtr && tracePtr->mRulePtr && tracePtr->mRulePtr->Type() == WsfParseRule::cRECURRENCE)
   {
      WsfParseRecurrence* rulePtr = (WsfParseRecurrence*)tracePtr->mRulePtr;
      if (rulePtr->mCount == WsfParseRecurrence::cONE_TO_MANY || rulePtr->mCount == WsfParseRecurrence::cZERO_TO_MANY)
      {
         auto ctx       = ut::make_unique<WsfPParseRuleTraceContext>(new WsfPProxyPath(aCurrentPath));
         ctx->mTracePtr = tracePtr;
         assert(tracePtr->mContextPtr == nullptr);
         tracePtr->mContextPtr = ctx.get();
         ContextAdded(std::move(ctx));
      }
   }

   if (traceParent)
   {
      traceParent->AddChild(rTracePtr);
   }
}

WsfPParseRuleTrace* WsfPParseRuleTracer::EndNode(WsfParseNode* aNodePtr)
{
   if (!aNodePtr)
   {
      return nullptr;
   }
   assert(mBuildNodeStack.back()->GetNearestNode() == aNodePtr);
   WsfPParseRuleTrace* tracePtr = mBuildNodeStack.back();
   mBuildNodeStack.pop_back();
   return tracePtr;
}

WsfPParseRuleTracer::WsfPParseRuleTracer(WsfPProxyRegistry* aRegistry)
{
   mRoot     = nullptr;
   mRootRule = aRegistry->mRootRule->Find("root-command");
}

WsfPParseRuleTracer::~WsfPParseRuleTracer()
{
   delete mRoot;
}
