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

#ifndef WSFPARSESEQUENCE_HPP
#define WSFPARSESEQUENCE_HPP
#include "WsfParseRule.hpp"

// A sequence of rules
class WsfParseSequence : public WsfParseRule
{
public:
   WsfParseSequence(WsfParseRuleMemory& aMem);
   ~WsfParseSequence() override;
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
   void Initialize(int aSequence) override;

   void ResolveChildren(WsfParser& aParser, WsfParseNode* aNode);
   void UndoChildren(WsfParser& aParser, size_t aMaxIndex);

   std::vector<WsfParseRule*> GetSequence() override { return mSequence; }

   const std::vector<WsfParseRule*>& Sequence() const { return mSequence; }
   std::vector<WsfParseRule*>&       ChangeSequence() { return mSequence; }
   void                              AddRule(WsfParseRule* aRulePtr)
   {
      mSequence.push_back(aRulePtr);
      aRulePtr->mParentRulePtr = this;
   }
   void            Swap(WsfParseSequence& aRhs) { std::swap(mSequence, aRhs.mSequence); }
   void            SetAction(size_t aIndex, WsfParseAction* aActionPtr);
   WsfParseAction* GetActionBefore(size_t aSequenceIndex) const
   {
      return (mActions.size() <= aSequenceIndex) ? nullptr : mActions[aSequenceIndex];
   }
   // relinquishes ownership of an action
   WsfParseAction* TakeActionBefore(size_t aSequenceIndex)
   {
      WsfParseAction* rval     = mActions[aSequenceIndex];
      mActions[aSequenceIndex] = nullptr;
      return rval;
   }

   bool        HasActions() const { return !mActions.empty(); }
   std::string GetRuleDescription() override;
   size_t      FindChildIndex(WsfParseRule* aChildPtr)
   {
      auto s = std::find(mSequence.begin(), mSequence.end(), aChildPtr);
      return (s != mSequence.end()) ? ut::safe_cast<size_t, ptrdiff_t>(std::distance(mSequence.begin(), s)) : ut::npos;
   }
   size_t FindActionIndex(WsfParseAction* aActionPtr)
   {
      auto a = std::find(mActions.begin(), mActions.end(), aActionPtr);
      return (a != mActions.end()) ? ut::safe_cast<size_t, ptrdiff_t>(std::distance(mActions.begin(), a)) : ut::npos;
   }

protected:
   bool CompleteDelayLoad(WsfParser& aParser, WsfParseNode*& aNode, WsfParseNode* aThisNodePtr);

   std::vector<WsfParseRule*>   mSequence;
   std::vector<WsfParseAction*> mActions;

public:
   //! Does the command require special handling
   bool mSpecialCommand;
   bool mHasResolutionValues;
   bool mHasUndoValues;
   int  mNodeFlags;
   // If this sequence is delay-loaded, this is the index
   // of the reader which reads the type
   size_t mDelayLoadIndex;
};
#endif
