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

#include "WsfParseNode.hpp"

#include <cassert>

#include "UtLog.hpp"
#include "WsfParseAuxData.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseTypeInfoData.hpp"

void WsfParseNode::Add(WsfParseNode* aChild)
{
   if (mDown == nullptr)
   {
      assert(aChild->mParent == nullptr);
      assert(aChild->mRight == nullptr);
      assert(aChild->mLeft == nullptr);
      mDown           = aChild;
      aChild->mParent = this;
   }
   else
   {
      WsfParseNode* last = mDown->FarRight();
      last->InsertAfter(aChild);
   }
}

// static
std::string WsfParseNode::GetNodeString(WsfParseNode* aNodePtr)
{
   // returns the input contained in all sub nodes separated by space.
   // used as a quick hack to read values into Util unit classes
   std::string   result;
   WsfParseNode* end = aNodePtr->Next(false);
   while (aNodePtr != end)
   {
      if (aNodePtr->mValue.Valid())
      {
         if (!result.empty())
         {
            result += ' ';
         }
         result += aNodePtr->mValue.Text();
      }
      aNodePtr = aNodePtr->Next();
   }
   return result;
}

void WsfParseNode::DebugPrint(ut::log::MessageStream& aStream)
{
   std::string val = mValue.Text();
   if (val.empty())
   {
      val = "<empty>";
   }

   auto          out   = aStream.AddNote() << mType << ": " << val;
   WsfParseNode* child = mDown;
   while (child)
   {
      child->DebugPrint(out);
      child = child->Right();
   }
}

//! Returns the next node in a DFS traversal of the entire parse tree.
//! @param aRecurseDown if false, the DFS traversal will skip this subtree
WsfParseNode* WsfParseNode::Next(bool aRecurseDown) const
{
   bool          canDown(aRecurseDown);
   WsfParseNode* nodePtr = const_cast<WsfParseNode*>(this);
   while (nodePtr)
   {
      if (canDown && nodePtr->mDown)
      {
         return nodePtr->mDown;
      }
      if (nodePtr->mRight)
      {
         return nodePtr->mRight;
      }
      nodePtr = nodePtr->mParent;
      canDown = false;
   }
   return nullptr;
}

//! Returns the next node containing a value
WsfParseNode* WsfParseNode::NextLeaf(bool aRecurseDown) const
{
   WsfParseNode* nodePtr = Next(aRecurseDown);
   while (nodePtr && !nodePtr->mValue.Valid())
   {
      nodePtr = nodePtr->Next();
   }
   return nodePtr;
}

//! Returns the previous node containing a value
WsfParseNode* WsfParseNode::PreviousLeaf() const
{
   WsfParseNode* nodePtr = Previous();
   while (nodePtr && !nodePtr->mValue.Valid())
   {
      nodePtr = nodePtr->Previous();
   }
   return nodePtr;
}

//! Returns the previous node in a DFS traversal.
WsfParseNode* WsfParseNode::Previous() const
{
   WsfParseNode* nodePtr = const_cast<WsfParseNode*>(this);
   while (nodePtr)
   {
      if (nodePtr->mLeft)
      {
         return nodePtr->mLeft->GetRightmostLeaf();
      }
      if (nodePtr->mParent)
      {
         return nodePtr->mParent;
      }
      return nullptr;
   }
   return nullptr;
}

//! Returns the first child with the specified type.  Returns null if none is found.
WsfParseNode* WsfParseNode::FindChildByType(const std::string& aType) const
{
   WsfParseNode* child = mDown;
   while (child)
   {
      if (child->mType == aType)
      {
         break;
      }
      child = child->Right();
   }
   return child;
}
WsfParseNode* WsfParseNode::GetLeftmostLeaf()
{
   WsfParseNode* nodePtr = this;
   while (nodePtr->mDown)
   {
      nodePtr = nodePtr->mDown;
   }
   return nodePtr;
}

WsfParseNode* WsfParseNode::GetRightmostLeaf()
{
   WsfParseNode* nodePtr = this;
   while (nodePtr->mDown)
   {
      nodePtr = nodePtr->mDown->FarRight();
   }
   return nodePtr;
}

WsfParseNode* WsfParseNode::GetLeftmostValue()
{
   WsfParseNode* left      = GetLeftmostLeaf();
   WsfParseNode* rightmost = GetRightmostLeaf();
   while (left && !left->mValue.mSource)
   {
      if (left == rightmost)
      {
         return nullptr;
      }
      left = left->Next();
   }
   return left;
}

WsfParseNode* WsfParseNode::GetRightmostValue()
{
   WsfParseNode* right = GetRightmostLeaf();
   while (right && !right->mValue.mSource)
   {
      if (right == this)
      {
         return nullptr;
      }
      right = right->Previous();
   }
   return right;
}

WsfParseNode* WsfParseNode::GetLeftmostValue(UtTextDocument* aSourcePtr)
{
   WsfParseNode* left      = GetLeftmostLeaf();
   WsfParseNode* rightmost = GetRightmostLeaf();
   while (left && left->mValue.mSource != aSourcePtr)
   {
      if (left == rightmost)
      {
         return nullptr;
      }
      left = left->Next();
   }
   return left;
}

WsfParseNode* WsfParseNode::GetRightmostValue(UtTextDocument* aSourcePtr)
{
   WsfParseNode* right = GetRightmostLeaf();
   while (right && right->mValue.mSource != aSourcePtr)
   {
      if (right == this)
      {
         return nullptr;
      }
      right = right->Previous();
   }
   return right;
}

UtTextRange WsfParseNode::SubtreeRange(UtTextDocument* aSource)
{
   WsfParseNode* left  = GetLeftmostValue(aSource);
   WsfParseNode* right = GetRightmostValue(aSource);
   if (left && right)
   {
      UtTextRange range = left->mValue;
      range.Extend(right->mValue);
      return range;
   }
   else if (left)
   {
      return left->mValue;
   }
   return UtTextRange();
}

UtTextDocumentRange WsfParseNode::SubtreeRange()
{
   WsfParseNode* left  = GetLeftmostValue();
   WsfParseNode* right = GetRightmostValue();
   if (left && right && left->mValue.Valid() && left->mValue.mSource == right->mValue.mSource)
   {
      UtTextDocumentRange range = left->mValue;
      range.Extend(right->mValue);
      return range;
   }
   else if (left)
   {
      return left->mValue;
   }
   return UtTextDocumentRange();
}

UtTextRange WsfParseNode::GetBoundingRange(UtTextDocument* aSource)
{
   WsfParseNode* leftBound  = PreviousLeaf();
   WsfParseNode* rightBound = NextLeaf(false);
   size_t        minPos     = 0;
   size_t        maxPos     = aSource->GetText().Size() - 1;
   if (leftBound && leftBound->mValue.mSource == aSource)
   {
      minPos = leftBound->mValue.GetEnd() + 1;
   }
   if (rightBound && rightBound->mValue.mSource == aSource)
   {
      maxPos = rightBound->mValue.GetBegin() - 1;
   }
   return UtTextRange(minPos, maxPos);
}

void WsfParseNode::Detatch()
{
   if (mParent)
   {
      if (mParent->mDown == this)
      {
         mParent->mDown = mRight;
      }
      if (mLeft)
      {
         mLeft->mRight = mRight;
      }
      if (mRight)
      {
         mRight->mLeft = mLeft;
      }
      mLeft = mRight = nullptr;
      mParent        = nullptr;
   }
}

//! Remove this node from the tree, replacing it with aNodePtr.
//! aNodePtr should have no siblings or parent
void WsfParseNode::Replace(WsfParseNode* aNodePtr)
{
   if (mParent)
   {
      assert(aNodePtr->mLeft == nullptr);
      assert(aNodePtr->mRight == nullptr);
      assert(aNodePtr->mParent == nullptr);
      aNodePtr->mParent = mParent;
      aNodePtr->mLeft   = mLeft;
      aNodePtr->mRight  = mRight;

      if (mParent->mDown == this)
      {
         mParent->mDown = aNodePtr;
      }
      if (mLeft)
      {
         mLeft->mRight = aNodePtr;
      }
      if (mRight)
      {
         mRight->mLeft = aNodePtr;
      }
      mLeft = mRight = nullptr;
      mParent        = nullptr;
   }
}

void WsfParseNode::ResetLinks()
{
   mRight = mLeft = mDown = mParent = nullptr;
}

void WsfParseNode::InsertBefore(WsfParseNode* aNodePtr)
{
   assert(aNodePtr->mParent == nullptr);
   assert(aNodePtr->mRight == nullptr);
   assert(aNodePtr->mLeft == nullptr);
   if (mLeft)
   {
      mLeft->mRight = aNodePtr;
   }
   else if (mParent)
   {
      assert(mParent->mDown == this);
      mParent->mDown = aNodePtr;
   }
   aNodePtr->mRight  = this;
   aNodePtr->mLeft   = mLeft;
   mLeft             = aNodePtr;
   aNodePtr->mParent = GetParent();
}

void WsfParseNode::InsertAfter(WsfParseNode* aNodePtr)
{
   assert(aNodePtr->mParent == nullptr);
   assert(aNodePtr->mRight == nullptr);
   assert(aNodePtr->mLeft == nullptr);
   if (mRight)
   {
      mRight->mLeft = aNodePtr;
   }
   aNodePtr->mRight  = mRight;
   mRight            = aNodePtr;
   aNodePtr->mLeft   = this;
   aNodePtr->mParent = GetParent();
}

WsfParseNode* WsfParseNode::GetChild(size_t aIndex) const
{
   WsfParseNode* child = mDown;
   for (size_t i = 0; i < aIndex && child; ++i)
   {
      child = child->mRight;
   }
   return child;
}

WsfParseNode* WsfParseNode::FarRight()
{
   WsfParseNode* right = this;
   while (right->mRight)
   {
      right = right->mRight;
   }
   return right;
}

WsfParseNode* WsfParseNode::FarLeft()
{
   WsfParseNode* left = this;
   while (left->mLeft)
   {
      left = left->mLeft;
   }
   return left;
}

//! Returns the ranges of nodes containing the values in the given range.
//! @param aRootNodePtr          The root node of the tree.
//! @param aRange                The range of text to find nodes in.
//! @param aFileTransitionNodes  The first node of each file.  Used for accelerating searches.
//! @param aNodes                A vector containing a pair of nodes for each range marking the begin and end.
//! @note  Run time is O([Tokens In File] - [Tokens in range] + [Files in scenario])
//!        When requesting the entire file range, O([Files in scenario])
bool WsfParseNode::GetRangeNodes(WsfParseNode*                     aRootNodePtr,
                                 const UtTextDocumentRange&        aRange,
                                 const std::vector<WsfParseNode*>& aFileTransitionNodes,
                                 std::vector<WsfParseNode*>&       aNodes)
{
   WsfParseNode* lastNodePtr = aFileTransitionNodes.back();
   while (lastNodePtr->Next())
   {
      lastNodePtr = lastNodePtr->FarRight();
      if (lastNodePtr->Next())
      {
         lastNodePtr = lastNodePtr->Next();
      }
   }
   bool found = false;
   for (size_t i = 0; i < aFileTransitionNodes.size(); ++i)
   {
      WsfParseNode* transNodePtr = aFileTransitionNodes[i];
      if (transNodePtr && (transNodePtr->mValue.mSource == aRange.mSource) &&
          (transNodePtr->mValue.GetBegin() <= aRange.GetEnd()))
      {
         WsfParseNode* lastPtr = nullptr;
         if (i + 1 < aFileTransitionNodes.size())
         {
            lastPtr = aFileTransitionNodes[i + 1]->PreviousLeaf();
         }
         else
         {
            lastPtr = lastNodePtr;
         }
         if (transNodePtr && lastPtr)
         {
            UtTextRange range = transNodePtr->mValue;
            range.Extend(lastPtr->mValue);
            if (range.Intersects(aRange))
            {
               while (transNodePtr->mValue.GetEnd() < aRange.GetBegin())
               {
                  WsfParseNode* n = transNodePtr->NextLeaf();
                  if (!n)
                  {
                     transNodePtr = nullptr;
                     break;
                  }
                  else
                  {
                     transNodePtr = n;
                  }
               }
               if (transNodePtr)
               {
                  if (!(lastPtr && lastPtr->mValue.Intersects(aRange)))
                  {
                     while (lastPtr)
                     {
                        WsfParseNode* n = lastPtr->PreviousLeaf();
                        if (!n || n->mValue.mSource != aRange.mSource)
                        {
                           break;
                        }
                        else if (n->mValue.GetEnd() < aRange.GetEnd())
                        {
                           if (n->mValue.Intersects(aRange))
                           {
                              lastPtr = n;
                           }
                           break;
                        }
                        else
                        {
                           lastPtr = n;
                        }
                     }
                  }
                  if (lastPtr)
                  {
                     // Adjust the first node up until we reach a node containing other leafs
                     while (!transNodePtr->Left())
                     {
                        WsfParseNode* p = transNodePtr->GetParent();
                        if (p && p->Down() == transNodePtr)
                        {
                           transNodePtr = p;
                        }
                        else
                        {
                           break;
                        }
                     }
                     aNodes.push_back(transNodePtr);
                     aNodes.push_back(lastPtr);
                     found = true;
                  }
               }
            }
         }
      }
   }
   return found;
}

WsfParseRule* WsfParseNode::GetTopLevelRule() const
{
   WsfParseRule* rulePtr = mRulePtr;
   if (rulePtr == nullptr)
   {
      return nullptr;
   }
   while (rulePtr->mParentRulePtr != nullptr)
   {
      rulePtr = rulePtr->mParentRulePtr;
   }
   return rulePtr;
}

void WsfParseNode::MarkPruned()
{
   if (0 == (mFlags & cPRUNE_NODE))
   {
      mFlags |= cPRUNE_NODE;
      WsfParseNode* c = Down();
      while (c)
      {
         c->MarkPruned();
         c = c->Right();
      }
   }
}

std::string WsfParseNode::GetTextValue()
{
   WsfParseNode* n = this;
   if (!n)
   {
      return std::string();
   }
   std::string rval;
   if (n)
   {
      WsfParseNode* last = n->Next(false);
      while (n != last)
      {
         if (n->mValue.Valid())
         {
            if (!rval.empty())
            {
               rval += ' ';
            }
            rval += n->mValue.Text();
         }
         n = n->Next();
      }
   }
   return rval;
}

void WsfParseNode::Print(std::ostream& aStream, int aIndent, bool aVerbose)
{
   std::string flags;
   if (mFlags & cERROR_INCOMPLETE)
   {
      flags += ";incomplete";
   }
   if (mFlags & cTYPE_NAME_NODE)
   {
      flags += ";type-name";
   }
   if (mFlags & cTYPE_REFERENCE_NODE)
   {
      flags += ";type-reference";
   }
   if (mFlags & cNAMED_NODE)
   {
      flags += ";named";
   }
   if (mFlags & cLOAD_TYPE_NODE)
   {
      flags += ";load-type";
   }
   if (mFlags & cLAZY_TYPE_REFERENCE_NODE)
   {
      flags += ";lazy-type-ref";
   }
   if (mFlags & cLAZY_TYPENAME_NODE)
   {
      flags += ";lazy-type-name";
   }
   if (mFlags & cBLOCK_NODE)
   {
      flags += ";block";
   }
   if (mFlags & cSCRIPT_BLOCK)
   {
      flags += ";script-block";
   }
   if (mFlags & cSCRIPT_FUNCTION)
   {
      flags += ";script-func";
   }
   if (mFlags & cSCRIPT_VARIABLES)
   {
      flags += ";script-variables";
   }
   if (mFlags & cDELAY_LOAD)
   {
      flags += ";delay-load";
   }
   if (mFlags & cBLOCK_START)
   {
      flags += ";start-block";
   }
   if (mFlags & cBLOCK_END)
   {
      flags += ";end-block";
   }
   aStream << '\n';
   for (int i = 0; i < aIndent; ++i)
   {
      aStream << ' ';
   }
   aStream << "(";
   if (mType)
   {
      aStream << mType;
   }
   aStream << ' ';
   if (aVerbose)
   {
      if (mRulePtr && mRulePtr->IsNamedRule())
      {
         aStream << "Rule='" << (dynamic_cast<WsfParseNamedRule*>(mRulePtr))->GetFullName() << "'";
      }
      else if (mRulePtr)
      {
         aStream << "Rule='" << mRulePtr->GetRuleDescription() << "' ";
      }
   }
   if (mAuxiliaryValue)
   {
      if (WsfParseAuxData::cDELAY_LOAD_DATA == mAuxiliaryValue->mDataType)
      {
         aStream << "LoadOrder=" << (dynamic_cast<WsfParseDelayLoadData*>(mAuxiliaryValue))->mLoadOrder << " ";
      }
      else if (WsfParseAuxData::cFILE_REFERENCE == mAuxiliaryValue->mDataType)
      {
         aStream << "FileRef='" << (dynamic_cast<WsfParseFileReferenceData*>(mAuxiliaryValue))->mFilePath << "' ";
      }
      else if (WsfParseAuxData::cTYPE_DATA == mAuxiliaryValue->mDataType)
      {
         WsfParseTypeInfoData* tinfo = dynamic_cast<WsfParseTypeInfoData*>(mAuxiliaryValue);
         if (WsfParseTypeInfoData::cLOAD_TYPE == tinfo->mOperation)
         {
            aStream << "LoadType=" << WsfParseTypePathString(tinfo->mLoadKey) << " ";
         }
         else if (WsfParseTypeInfoData::cCREATE_TYPE == tinfo->mOperation)
         {
            aStream << "CreateType=" << WsfParseTypePathString(tinfo->mSaveKey) << " "
                    << WsfParseTypePathString(tinfo->mLoadKey) << " ";
         }
         else if (WsfParseTypeInfoData::cDELETE_TYPE == tinfo->mOperation)
         {
            aStream << "DeleteType=" << WsfParseTypePathString(tinfo->mLoadKey) << " ";
         }
      }
   }
   if (mValue.Valid())
   {
      aStream << "Value='" << mValue.Text() << "' ";
   }

   if (!flags.empty())
   {
      aStream << "Flags='" << flags << "' ";
   }
   aStream << ")";
   WsfParseNode* child = Down();
   while (child)
   {
      child->Print(aStream, aIndent + 2, aVerbose);
      child = child->Right();
   }
}

// Get the node's load order
// the combination of load order, lexical index
// reveals the actual order the node is processed after considering delay load
size_t WsfParseNode::LoadOrder() const
{
   if (mAuxiliaryValue && mAuxiliaryValue->mDataType == WsfParseAuxData::cDELAY_LOAD_DATA)
   {
      WsfParseDelayLoadData* delayPtr = dynamic_cast<WsfParseDelayLoadData*>(mAuxiliaryValue);
      return delayPtr->mLoadOrder;
   }
   if (!mParent)
   {
      return ut::npos;
   }
   return mParent->LoadOrder();
}
