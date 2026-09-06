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

#include "WsfParseFormat.hpp"

#include <algorithm>

#include "WsfParseNode.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseUtil.hpp"

//
// Formatting wsf core source:
// This provides a very minimal formatting.  Spaces and newlines are only added, not removed,
// with the exception of dedenting text.
// This code essentially makes sure that each command lies on a new line, with a fixed
// indentation for each block.  Any command with non-whitespace characters in front will
// be brought to the next line.  Any command with too much indentation will be dedented.
// Text inside a command, i.e.:  position[ /*comment*/
//                                              1n    1e]
// is never changed.
//
// Instead of updating the parse tree as the file is changed, the character offset is kept for each file;
// This allows each text change to take constant time.  The parse tree will be invalid after formatting.


WsfParseFormat::WsfParseFormat()
   : mBlockIndent(3)
{
}

void WsfParseFormat::Format(const std::set<WsfParseNode*>& aFormatNodes)
{
   if (aFormatNodes.empty())
   {
      return;
   }
   mFormatNodes       = aFormatNodes;
   WsfParseNode* root = *aFormatNodes.begin();
   while (root->GetParent())
   {
      root = root->GetParent();
   }
   FormatBlockNodes(root);
}

namespace
{
bool IsBlockNode(WsfParseNode* aNodePtr)
{
   if (aNodePtr && aNodePtr->mRulePtr && aNodePtr->mRulePtr->Type() == WsfParseRule::cRECURRENCE)
   {
      WsfParseRecurrence* rec = dynamic_cast<WsfParseRecurrence*>(aNodePtr->mRulePtr);
      if (rec->mCount != WsfParseRecurrence::cZERO_OR_ONE)
      {
         return true;
      }
   }
   return false;
}
} // namespace

void WsfParseFormat::FormatBlockNodes(WsfParseNode* aBlockNode)
{
   std::string whitespace("                  ");
   size_t      indentLevel = 0;
   {
      WsfParseNode* bn = aBlockNode;
      while (bn)
      {
         if (bn->mRulePtr && bn->mRulePtr->Type() == WsfParseRule::cRECURRENCE)
         {
            if (IsBlockNode(bn))
            {
               indentLevel += mBlockIndent;
            }
         }
         bn = bn->GetParent();
      }
   }
   while (whitespace.length() < indentLevel)
   {
      whitespace += "    ";
   }

   for (WsfParseNode* cmd = aBlockNode->Down(); cmd != nullptr; cmd = cmd->Right())
   {
      WsfParseNode* cmdEnd          = cmd->Next(false);
      bool          foundFirstToken = false;
      for (WsfParseNode* n = cmd; n != cmdEnd;)
      {
         if (IsBlockNode(n))
         {
            foundFirstToken = false;
            FormatBlockNodes(n);
            n = n->Next(false);
         }
         else
         {
            bool moved = false;
            if (!foundFirstToken && n->mValue.Valid())
            {
               foundFirstToken = true;
               if (NodeFormatRequested(n))
               {
                  UtTextDocument*        src           = n->mValue.mSource;
                  size_t&                insertedChars = InsertedChars(src);
                  UtTextDocumentLocation pos           = n->mValue.GetPosition();
                  pos.mPosition += insertedChars;

                  // n->mValue.Move(insertedChars);
                  size_t line, col;
                  if (src->PositionToLineColumn(pos.mPosition, line, col))
                  {
                     size_t      linePos       = src->GetLinePosition(line);
                     const char* text          = src->GetPointer(linePos, col);
                     bool        isStartOfLine = true;
                     for (const char* i = text; i != text + col && isStartOfLine; ++i)
                     {
                        isStartOfLine = WsfParseUtil::IsWhitespace(*i);
                     }
                     // bool isStartOfLine = std::all_of(text, text + col, WsfParseUtil::IsWhitespace);
                     bool reIndent = true;
                     if (!isStartOfLine)
                     {
                        if (pos.mPosition > 0 && (*src)[pos.mPosition - 1] == ' ')
                        {
                           // spaces are inserted between commands, so if we are about to insert an endline, it should
                           // replace the space if possible.
                           src->Erase(pos.mPosition - 1, 1);
                           src->Insert(pos.mPosition - 1, "\n", 1);
                        }
                        else
                        {
                           src->Insert(pos.mPosition, "\n", 1);
                           ++insertedChars;
                           ++pos.mPosition;
                        }
                        // n->mValue.Move(1);
                        ++line;
                        col = 0;
                        mModifiedDocuments.insert(src);
                     }
                     else if (col == indentLevel)
                     {
                        reIndent = false;
                        // if indentation is correct, don't modify the text
                        for (size_t i = 0; i < col; ++i)
                        {
                           if (text[i] != ' ')
                           {
                              reIndent = true;
                              break;
                           }
                        }
                     }
                     if (reIndent)
                     {
                        src->Erase(pos.mPosition - col, col);
                        src->Insert(pos.mPosition - col, whitespace.c_str(), indentLevel);
                        insertedChars += indentLevel - col;
                        mModifiedDocuments.insert(src);
                        moved = true;
                     }
                  }
               }
            }
            if (!moved)
            {
               if (n->mValue.Valid())
               {
                  // n->mValue.Move(InsertedChars(n->mValue.mSource));
               }
            }
            n = n->Next();
         }
      }
   }
}

size_t& WsfParseFormat::InsertedChars(UtTextDocument* aDocPtr)
{
   return mInsertedChars[aDocPtr];
}

bool WsfParseFormat::NodeFormatRequested(WsfParseNode* aNodePtr) const
{
   WsfParseNode* n = aNodePtr;
   while (n)
   {
      if (mFormatNodes.find(n) != mFormatNodes.end())
      {
         return true;
      }
      n = n->GetParent();
   }
   return false;
}
