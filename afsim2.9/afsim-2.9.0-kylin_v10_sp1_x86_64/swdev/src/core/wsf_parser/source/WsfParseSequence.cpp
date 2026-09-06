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

#include "WsfParseSequence.hpp"

#include <sstream>

#include "UtCast.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseTypeLoadRule.hpp"
#include "WsfParser.hpp"

namespace std
{
void swap(WsfParseSequence& aLhs, WsfParseSequence& aRhs)
{
   aLhs.Swap(aRhs);
}
} // namespace std

WsfParseSequence::WsfParseSequence(WsfParseRuleMemory& aMem)
   : WsfParseRule(aMem, cSEQUENCE)
   , mSpecialCommand(false)
   , mHasResolutionValues(false)
   , mHasUndoValues(false)
   , mNodeFlags(0)
   , mDelayLoadIndex(ut::npos)
{
}

WsfParseSequence::~WsfParseSequence()
{
   for (size_t i = 0; i < mActions.size(); ++i)
   {
      delete mActions[i];
   }
}

bool WsfParseSequence::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   bool          hasReadBlock   = false;
   const int     cMAX_BACKTRACK = 7;
   size_t        startWordsRead = aParser.GetWordsRead();
   bool          success        = true;
   WsfParseNode* thisNode       = nullptr;
   size_t        childIndex     = 0;
   for (; childIndex < mSequence.size(); ++childIndex)
   {
      if (childIndex == mDelayLoadIndex && !aParser.IsDelayLoading())
      {
         return CompleteDelayLoad(aParser, aNode, thisNode);
      }

      WsfParseRule* rdr   = mSequence[childIndex];
      WsfParseNode* child = nullptr;
      hasReadBlock        = hasReadBlock || (rdr->Type() == WsfParseRule::cRECURRENCE);
      if (rdr->Read(aParser, child))
      {
         // Some readers do not create matches
         if (child)
         {
            if (!thisNode)
            {
               thisNode = aParser.NewNode(this, "sequence");
               aParser.PushSequence(thisNode);
            }
            thisNode->Add(child);
            child->mFlags |= mNodeFlags;
         }
      }
      else
      {
         size_t wordsRead      = aParser.GetWordsRead() - startWordsRead;
         bool   allowBacktrack = 0 == wordsRead || (!hasReadBlock && wordsRead < cMAX_BACKTRACK);
         if (!allowBacktrack)
         {
            success = (thisNode != nullptr);
            if (thisNode)
            {
               thisNode->MarkIncomplete();
            }
            break;
         }
         else
         {
            aParser.UndoRead(wordsRead);
            success = false;
            break;
         }
      }
   }
   if (success && thisNode)
   {
      aNode = thisNode;
      aNode->mFlags |= mNodeFlags;
      --childIndex;
      if (mHasResolutionValues)
      {
         ResolveChildren(aParser, aNode);
      }
      if (mSpecialCommand)
      {
         aParser.ProcessSpecialNode(aNode);
      }
      aParser.PopSequence();
   }
   else
   {
      bool allowBacktrack = (aParser.GetWordsRead() - startWordsRead) < cMAX_BACKTRACK;
      if (thisNode)
      {
         aParser.PopSequence();
         aParser.FreeNode(thisNode);
      }
      thisNode = nullptr;
      if (allowBacktrack)
      {
         size_t wordsToUndo = aParser.GetWordsRead() - startWordsRead;
         if (wordsToUndo > 0)
         {
            aParser.PartialMatch(aParser.UndoReadAndGetRange(wordsToUndo));
         }
      }
      if (mHasUndoValues)
      {
         UndoChildren(aParser, childIndex - 1);
      }
   }
   return success;
}

bool WsfParseSequence::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   if (mSequence.size() == 1)
   {
      return mSequence[0]->ReadRange(aParser, aRange, aNode, aCreateNode);
   }
   return false;
}

void WsfParseSequence::Initialize(int aSequence)
{
   for (size_t i = 0; i < mSequence.size(); ++i)
   {
      WsfParseRule* rulePtr = mSequence[i];
      mHasResolutionValues  = mHasResolutionValues || ((rulePtr->GetReaderFlags() & WsfParseRule::cCAN_RESOLVE) != 0);
      mHasUndoValues        = mHasUndoValues || ((rulePtr->GetReaderFlags() & WsfParseRule::cCAN_UNDO) != 0);

      // If we find an appropriate type loading rule, and a recurrence, we will mark this sequence as delay load
      if (rulePtr->Type() == WsfParseRule::cTYPE_LOAD)
      {
         WsfParseTypeLoadRule* typeLoader = dynamic_cast<WsfParseTypeLoadRule*>(rulePtr);
         if (((typeLoader->GetOperation() == WsfParseTypeInfoData::cLOAD_TYPE) &&
              (typeLoader->mLoadType.mNestedLookup == false)) ||
             ((typeLoader->GetOperation() == WsfParseTypeInfoData::cCREATE_TYPE) &&
              (typeLoader->mSaveType.mNestedLookup == false)))
         {
            for (size_t j = i + 1; j < mSequence.size(); ++j)
            {
               if (mSequence[j]->Type() == WsfParseRule::cRECURRENCE)
               {
                  mDelayLoadIndex = i;
                  break;
               }
            }
         }
      }
   }
   if (mSequence.size() == 1)
   {
      mReaderFlags |= (mSequence[0]->GetReaderFlags() & WsfParseRule::cIS_SINGLE_TOKEN);
   }
   WsfParseRule::Initialize(aSequence);
}

void WsfParseSequence::ResolveChildren(WsfParser& aParser, WsfParseNode* aNode)
{
   for (auto s = mSequence.rbegin(); s != mSequence.rend(); ++s)
   {
      WsfParseRule* readerPtr = *s;
      if (readerPtr->GetReaderFlags() & WsfParseRule::cCAN_RESOLVE)
      {
         readerPtr->Resolve(aParser, aNode);
      }
   }
}
void WsfParseSequence::UndoChildren(WsfParser& aParser, size_t aMaxIndex)
{
   if (aMaxIndex != ut::npos)
   {
      for (size_t i = 0; i <= aMaxIndex; ++i)
      {
         WsfParseRule* readerPtr = mSequence[aMaxIndex - i];
         if (readerPtr->GetReaderFlags() & WsfParseRule::cCAN_UNDO)
         {
            readerPtr->Undo(aParser);
         }
      }
   }
}

std::string WsfParseSequence::GetRuleDescription()
{
   std::stringstream ss;
   ss << "{ ";
   for (size_t i = 0; i < mSequence.size(); ++i)
   {
      if (i > 0)
      {
         ss << ' ';
      }
      ss << mSequence[i]->GetRuleDescription();
   }
   ss << " }";
   return ss.str();
}

bool WsfParseSequence::CompleteDelayLoad(WsfParser& aParser, WsfParseNode*& aNode, WsfParseNode* aThisNodePtr)
{
   WsfParseTypeLoadRule* loadRulePtr = dynamic_cast<WsfParseTypeLoadRule*>(mSequence[mDelayLoadIndex]);
   WsfParseIndex&        index       = *aParser.GetParseIndex();
   WsfParseTypePath      delayLoadKey;
   UtTextDocumentRange   loadInputRange;
   if (loadRulePtr->GetOperation() == WsfParseTypeInfoData::cLOAD_TYPE)
   {
      if (!loadRulePtr->CreateTypePath(delayLoadKey, aParser, index, loadRulePtr->mLoadType, loadInputRange))
      {
         return false;
      }
   }
   else
   {
      if (!loadRulePtr->CreateTypePath(delayLoadKey, aParser, index, loadRulePtr->mSaveType, loadInputRange))
      {
         return false;
      }
   }

   // Get the recurrence rule
   WsfParseRecurrence* blockRulePtr = nullptr;
   {
      size_t recurrenceIndex = mDelayLoadIndex + 1;
      while (recurrenceIndex < mSequence.size())
      {
         if (mSequence[recurrenceIndex]->Type() == WsfParseRule::cRECURRENCE)
         {
            break;
         }
      }
      blockRulePtr = dynamic_cast<WsfParseRecurrence*>(mSequence[recurrenceIndex]);
   }

   std::string kind, name;
   kind = delayLoadKey[0].Get();
   name = delayLoadKey.back().Get();
   for (size_t i = 1; i + 1 < delayLoadKey.size(); ++i)
   {
      kind += '.';
      kind += delayLoadKey[i].Get();
   }
   WsfParseNode* firstNodePtr = aThisNodePtr->GetLeftmostValue();
   if (firstNodePtr && firstNodePtr->mValue.Valid())
   {
      {
         UtTextDocumentRange termWord;
         for (;;)
         {
            termWord = aParser.ReadWord();
            if (termWord == blockRulePtr->mEndKeyword)
            {
               break;
            }
            else if (!termWord.Valid())
            {
               UtTextDocumentRange errorRange = firstNodePtr->mValue;
               aParser.AddError(firstNodePtr->mRulePtr, errorRange);
               return false;
            }
         }
      }
      if (!aNode)
      {
         aNode = aThisNodePtr;
      }

      // The grammar depends on backtracking, so when scheduling a delay load rule it may
      // need to be processed by another sequence.  A 90% solution is used here: just
      // let the parent rule do the delay load processing if it is an alternate.
      WsfParseRule* delayLoadRule = this;
      if (mParentRulePtr && mParentRulePtr->Type() == WsfParseRule::cALTERNATE)
      {
         delayLoadRule = mParentRulePtr;
      }
      aParser.DelayLoad(kind, name, aThisNodePtr, firstNodePtr->mValue.GetBegin(), delayLoadRule);

      return true;
   }
   return false;
}

void WsfParseSequence::SetAction(size_t aIndex, WsfParseAction* aActionPtr)
{
   while (mActions.size() <= aIndex)
   {
      mActions.push_back(nullptr);
   }
   delete mActions[aIndex];
   mActions[aIndex] = aActionPtr;
}
