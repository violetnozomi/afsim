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

#include "WsfParseAlternate.hpp"

#include <sstream>

#include "UtCast.hpp"
#include "WsfParseBasicRules.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseUtil.hpp"
#include "WsfParser.hpp"

namespace std
{
void swap(WsfParseAlternate& aLhs, WsfParseAlternate& aRhs)
{
   aLhs.Swap(aRhs);
}

} // namespace std

WsfParseAlternate::~WsfParseAlternate()
{
   ClearAccelerators();
}

bool WsfParseAlternate::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   if (mAccelerators.empty())
   {
      for (size_t i = 0; i < mAlternates.size(); ++i)
      {
         if (mAlternates[i]->Read(aParser, aNode))
         {
            aParser.ClearPartialMatches();

            WsfParseNode* altNode = aParser.NewNode(this, "alternate");
            if (aNode)
            {
               altNode->Add(aNode);
            }
            aNode = altNode;
            return true;
         }
      }
   }
   else
   {
      UtTextDocumentRange word = aParser.ReadWord();
      if (word.Valid())
      {
         aParser.UndoRead();
         char                  c           = (*word.mSource)[word.GetBegin()];
         CharacterAccelerator* charAccel   = mAccelerators[(unsigned char)c];
         size_t                readerCount = mAlternates.size();
         size_t                i = 0, ni = readerCount, j = 0, nj = readerCount;
         size_t                I = 0, J = 0;
         if (charAccel)
         {
            ni = ut::cast_to_size_t(charAccel->mChildIndices[0]);
            I  = charAccel->mChildIndices.size();
         }
         if (!mUnfiltered.mChildIndices.empty())
         {
            nj = ut::cast_to_size_t(mUnfiltered.mChildIndices[0]);
            J  = mUnfiltered.mChildIndices.size();
         }
         while (nj < readerCount || ni < readerCount)
         {
            WsfParseRule* readerPtr = nullptr;
            if (nj < ni)
            {
               readerPtr = mAlternates[nj];
               ++j;
               nj = j < J ? mUnfiltered.mChildIndices[j] : readerCount;
            }
            else
            {
               readerPtr = mAlternates[ni];
               ++i;
               ni = i < I ? charAccel->mChildIndices[i] : readerCount;
            }
            if (readerPtr->Read(aParser, aNode))
            {
               aParser.ClearPartialMatches();

               WsfParseNode* altNode = aParser.NewNode(this, "alternate");
               altNode->Add(aNode);
               aNode = altNode;
               return true;
            }
         }
      }
      else
      {
         aParser.UndoRead();
      }
   }
   return false;
}

bool WsfParseAlternate::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   for (size_t i = 0; i < mAlternates.size(); ++i)
   {
      if (mAlternates[i]->ReadRange(aParser, aRange, aNode, aCreateNode))
      {
         return true;
      }
   }
   return false;
}

void WsfParseAlternate::Initialize()
{
   if (mAlternates.size() == 1)
   {
      mReaderFlags |= mAlternates[0]->GetReaderFlags() & cIS_SINGLE_TOKEN;
   }

   // Compute 'accelerators' which determine the subset of readers that need to be evaluated
   // given the starting character.  This optimization can reduce parse time in half.
   size_t cMIN_READERS_FOR_ACCELERATION = 1;
   if (!WsfParseUtil::cOPTIMIZE_PARSE)
   {
      cMIN_READERS_FOR_ACCELERATION = 1000;
   }
   if (mAlternates.size() >= cMIN_READERS_FOR_ACCELERATION)
   {
      mAccelerators.resize(255);
      for (int c = 1; c < 256; ++c)
      {
         CharacterAccelerator current;
         for (size_t i = 0; i < mAlternates.size(); ++i)
         {
            int val = ReaderStartsWithChar(mAlternates[i], (char)c);
            if (val == 1)
            {
               current.mChildIndices.push_back(ut::cast_to_int(i));
            }
            else if (val > 1)
            {
               if (c == 1)
               {
                  mUnfiltered.mChildIndices.push_back(ut::cast_to_int(i));
               }
            }
         }
         if (!current.mChildIndices.empty())
         {
            mAccelerators[c] = new CharacterAccelerator;
            std::swap(*mAccelerators[c], current);
         }
      }
      if (mAlternates.size() - mUnfiltered.mChildIndices.size() <= 1)
      {
         ClearAccelerators();
         mUnfiltered.mChildIndices.clear();
      }
   }
}

int WsfParseAlternate::ReaderStartsWithChar(WsfParseRule* aReaderPtr, char aChar)
{
   switch (aReaderPtr->Type())
   {
   case cREAL:
      return ((aChar > 0 && isdigit(aChar)) || aChar == '.' || aChar == '-' || aChar == '+') ? 1 : 0;
   case cINT:
      return ((aChar > 0 && isdigit(aChar)) || aChar == '-' || aChar == '+') ? 1 : 0;
   case cSTRING:
      return 2;
   case cLINE_STRING:
      return 2;
   case cQUOTED_STRING:
      return (dynamic_cast<WsfParseQuotedStringRule*>(aReaderPtr))->mQuotesOptional ? 2 : aChar == '"';
   case cLITERAL:
   {
      WsfParseLiteral* evRdrPtr = dynamic_cast<WsfParseLiteral*>(aReaderPtr);
      if (!evRdrPtr->mCaseSensitive)
      {
         return ((dynamic_cast<WsfParseLiteral*>(aReaderPtr))->mText[0] == tolower(aChar));
      }
      else
      {
         return ((dynamic_cast<WsfParseLiteral*>(aReaderPtr))->mText[0] == aChar);
      }
   }
   //       case cDELIMITED:
   //          {
   //             WsfParseDelimited* delimRdrPtr = (WsfParseDelimited*)aReaderPtr;
   //             if (! delimRdrPtr->mSections[0].mPreDelimiter.empty())
   //             {
   //                return aChar == delimRdrPtr->mSections[0].mPreDelimiter[0];
   //             }
   //             else
   //             {
   //                return ReaderStartsWithChar(delimRdrPtr->mSections[0].mReaderPtr, aChar);
   //             }
   //          }
   case cSEQUENCE:
   {
      int               rval   = 0;
      WsfParseSequence* rdrPtr = dynamic_cast<WsfParseSequence*>(aReaderPtr);
      if (!rdrPtr->Sequence().empty())
      {
         rval = ReaderStartsWithChar(rdrPtr->Sequence()[0], aChar);
      }
      return rval;
   }
   case cALTERNATE:
   {
      int                rval   = 0;
      WsfParseAlternate* rdrPtr = dynamic_cast<WsfParseAlternate*>(aReaderPtr);
      for (size_t i = 0; i < rdrPtr->mAlternates.size(); ++i)
      {
         rval |= ReaderStartsWithChar(rdrPtr->mAlternates[i], aChar);
      }
      return rval;
   }
   case cRECURRENCE:
      return 2;
   //       case cTYPE_LOAD:
   //          return 2;
   //       case cTYPE_COMMAND:
   //          return 2;
   //       case cSCRIPT_BLOCK:
   //          return 2;
   default:
      return 2;
   }
}

void WsfParseAlternate::ClearAccelerators()
{
   for (size_t i = 0; i < mAccelerators.size(); ++i)
   {
      delete mAccelerators[i];
   }
   mAccelerators.clear();
}

void WsfParseAlternate::AddAlternate(WsfParseRule* aSequence)
{
   if (!aSequence)
   {
      throw std::invalid_argument("nullptr passed as argument");
   }
   mAlternates.push_back(aSequence);
   aSequence->mParentRulePtr = this;
}

std::string WsfParseAlternate::GetRuleDescription()
{
   std::stringstream ss;
   for (size_t i = 0; i < mAlternates.size(); ++i)
   {
      if (i > 0)
      {
         ss << " | ";
      }
      ss << mAlternates[i]->GetRuleDescription();
   }
   return ss.str();
}
