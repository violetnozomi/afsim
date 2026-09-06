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

#include "WsfParseRecurrence.hpp"

#include "WsfParseNode.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParser.hpp"


namespace std
{
void swap(WsfParseRecurrence& aLhs, WsfParseRecurrence& aRhs)
{
   swap((WsfParseRule&)aLhs, (WsfParseRule&)aRhs);
   swap(aLhs.mCount, aRhs.mCount);
   swap(aLhs.mEndKeyword, aRhs.mEndKeyword);
}

} // namespace std

bool WsfParseRecurrence::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   size_t minCount, maxCount;
   GetCountRange(minCount, maxCount);

   bool hasTerminator = !mEndKeyword.empty();
   if (hasTerminator)
   {
      aParser.PushTerminator(mEndKeyword.c_str());
   }

   UtTextDocumentRange blockRange;
   WsfParseNode*       blockNode = aParser.NewNode(this, "block");
   blockNode->mFlags |= WsfParseNode::cBLOCK_NODE;
   WsfParseNode* prevChildPtr    = nullptr;
   bool          ok              = true;
   size_t        recurrenceCount = 0;
   while (ok)
   {
      UtTextDocumentRange word = aParser.ReadWord();
      if (recurrenceCount == maxCount)
      {
         if (!mEndKeyword.empty())
         {
            aParser.AddError(this, word);
         }
         aParser.UndoRead();
         break;
      }
      if (word.Valid())
      {
         aParser.UndoRead();
         WsfParseNode* child = nullptr;
         ok                  = mSubordinateRulePtr->Read(aParser, child);
         if (ok && child)
         {
            // For large blocks, Add() can be very inefficient.
            if (prevChildPtr)
            {
               prevChildPtr->InsertAfter(child);
               prevChildPtr = child;
            }
            else
            {
               blockNode->Add(child);
               prevChildPtr = child;
            }
            ++recurrenceCount;
         }
         else if (mEndKeyword.empty())
         {
            // no end token means the block is done at first read error
            ok = true;
            if (recurrenceCount < minCount || recurrenceCount > maxCount)
            {
               blockNode->MarkIncomplete();
               aParser.AddError(this, word);
            }
            break;
         }
         else
         {
            ok = true;
            aParser.AddError(this, word);
            aParser.ReadWord(); // throw away a word
         }
      }
      else
      {
         if (hasTerminator)
         {
            aParser.PopTerminator();
            hasTerminator = false;
            aParser.UndoRead();
            break;
         }
         else
         {
            aParser.UndoRead();
            break;
         }
      }
   }
   if (!ok)
   {
      aParser.FreeNode(blockNode);
   }
   else
   {
      aNode = blockNode;
   }
   if (hasTerminator)
   {
      aParser.PopTerminator();
   }
   return ok;
}

WsfParseRecurrence::~WsfParseRecurrence() {}

void WsfParseRecurrence::SetRecurringRule(WsfParseRule* aRulePtr)
{
   SetSubordinateRule(aRulePtr);
}

void WsfParseRecurrence::Initialize(int aSequence)
{
   WsfParseRule::Initialize(aSequence);
   if (aSequence != 0)
   {
      return;
   }
   WsfParseRule* parentPtr = mParentRulePtr;
   // Set the terminating keyword if it is available
   if (parentPtr && parentPtr->Type() == WsfParseRule::cSEQUENCE)
   {
      size_t            thisIndex = 0;
      WsfParseSequence* parentSeq = dynamic_cast<WsfParseSequence*>(parentPtr);
      for (size_t i = 1; i < parentSeq->Sequence().size(); ++i)
      {
         if (parentSeq->Sequence()[i - 1] == this)
         {
            thisIndex              = i - 1;
            WsfParseRule* postRule = parentSeq->Sequence()[i];
            if (postRule->Type() == WsfParseRule::cLITERAL)
            {
               WsfParseLiteral* lit = dynamic_cast<WsfParseLiteral*>(postRule);
               mEndKeyword          = lit->mText;
               lit->mIsTerminator   = true;
               if (mCount >= cONE_TO_MANY)
               {
                  lit->mNodeFlags |= WsfParseNode::cBLOCK_END;
               }
            }
         }
      }

      // most blocks are of the form:    xyz ... end_xyz.
      // in which case, try to make sure we mark the right 'begin' keyword when it's supplied
      bool markedBeginBlock = false;
      if (mEndKeyword.size() > 4 && mEndKeyword.substr(0, 4) == "end_")
      {
         std::string expectedBeginKeyword = mEndKeyword.substr(4);
         for (size_t i = 0; i < thisIndex; ++i)
         {
            WsfParseRule* rulePtr = parentSeq->Sequence()[i];
            if (rulePtr->Type() == WsfParseRule::cLITERAL)
            {
               WsfParseLiteral* litPtr = dynamic_cast<WsfParseLiteral*>(rulePtr);
               if (litPtr->mText == expectedBeginKeyword)
               {
                  if (mCount >= cONE_TO_MANY)
                  {
                     litPtr->mNodeFlags |= WsfParseNode::cBLOCK_START;
                  }
                  markedBeginBlock = true;
               }
               break;
            }
         }
      }

      // if the begin keyword hasn't been marked, pick the first literal before the block starts
      if (!markedBeginBlock)
      {
         size_t i = thisIndex - 1;
         while (i != ut::npos)
         {
            WsfParseRule* rulePtr = parentSeq->Sequence()[i];

            if (rulePtr->Type() == WsfParseRule::cLITERAL)
            {
               WsfParseLiteral* litPtr = dynamic_cast<WsfParseLiteral*>(rulePtr);
               if (mCount >= cONE_TO_MANY)
               {
                  litPtr->mNodeFlags |= WsfParseNode::cBLOCK_START;
               }
               break;
            }
            --i;
         }
      }
   }
}
