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

#include "WsfParseBasicRules.hpp"

#include <assert.h>
#include <stdlib.h>

#include "WsfParseIndex.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseUtil.hpp"
#include "WsfParser.hpp"

bool WsfParseRealRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange word = aParser.ReadWord();
   if (!word.Valid())
   {
      aParser.UndoRead();
      return false;
   }
   if (!WsfParseRealRule::ReadRange(aParser, word, aNode, true))
   {
      aParser.UndoRead();
      return false;
   }
   return true;
}

// On windows, strtod calls strlen() which causes very poor performance.
// This implementation is much faster:
// Returns true if the text range is a real number
bool ParseReal(const char* buffer, const char* endPtr)
{
   //[+-](digit)*(.(digit)*)[[eE][+-](digit)+]
   int         state = 0;
   const char* p     = buffer;
   while (p < endPtr)
   {
      char c = *p;
      switch (state)
      {
      case 0:
         if (c == '+' || c == '-')
         {
            ++p;
         }
         state = 1;
         break;
      case 1:
         if (c == '.')
         {
            state = 2;
            ++p;
         }
         else if (WsfParseUtil::IsDigit(c))
         {
            ++p;
         }
         else
         {
            state = 3;
         }
         break;
      case 2:
         if (WsfParseUtil::IsDigit(c))
         {
            ++p;
            break;
         }
         state = 3;
         break;
      case 3:
         if (c == 'e' || c == 'E')
         {
            state = 4;
            ++p;
            break;
         }
         state = 6;
         break;
      case 4:
         if (c == '+' || c == '-')
         {
            state = 5;
            ++p;
            break;
         }
         state = 5;
      // fall through
      case 5:
         if (WsfParseUtil::IsDigit(c))
         {
            ++p;
            break;
         }
         state = 6;
         break;
      case 6:
         return false;
      }
   }
   return state == 1 || state == 2 || state == 5 || state == 6;
}

bool WsfParseRealRule::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   char* ptr = (char*)aRange.Pointer();

   const char* begin = ptr;
   char*       end   = ptr + aRange.Length();
   if (ParseReal(begin, end))
   {
      if (aCreateNode)
      {
         aNode = aParser.NewNode(this, "real", aRange);
      }
      return true;
   }
   else
   {
      ParseReal(begin, end);
   }
   return false;
}
// Returns true if the text range is an integer
bool ParseInt(const char* buffer, const char* endPtr)
{
   //[+-](digit)+
   int         state = 0;
   const char* p     = buffer;
   while (p < endPtr)
   {
      char c = *p;
      switch (state)
      {
      case 0:
         if (c == '+' || c == '-')
         {
            ++p;
         }
         state = 1;
         break;
      case 1:
         if (WsfParseUtil::IsDigit(c))
         {
            ++p;
            state = 2;
            break;
         }
         return false;
      case 2:
         if (WsfParseUtil::IsDigit(c))
         {
            ++p;
            break;
         }
         return false;
         break;
      }
   }
   return state == 2;
}

bool WsfParseIntRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange word = aParser.ReadWord();
   if (!word.Valid())
   {
      aParser.UndoRead();
      return false;
   }
   if (!WsfParseIntRule::ReadRange(aParser, word, aNode, true))
   {
      aParser.UndoRead();
      return false;
   }
   return true;
}

bool WsfParseIntRule::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   char*       ptr   = (char*)aRange.Pointer();
   const char* begin = ptr;
   char*       end   = ptr + aRange.Length();
   if (!ParseInt(begin, end))
   {
      return false;
   }
   if (aCreateNode)
   {
      aNode = aParser.NewNode(this, "int", aRange);
   }
   return true;
}

bool WsfParseStringRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange word = aParser.ReadWord();
   if (!word.Valid())
   {
      aParser.UndoRead();
      return false;
   }
   bool ok = WsfParseStringRule::ReadRange(aParser, word, aNode, true);
   assert(ok == true);
   return ok;
}

bool WsfParseStringRule::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   if (aCreateNode)
   {
      if (mNodeType.empty())
      {
         aNode = aParser.NewNode(this, "string", aRange);
      }
      else
      {
         aNode = aParser.NewNode(this, mNodeType.c_str(), aRange);
      }
      aNode->SetFlags(mFlags);
   }
   return true;
}

bool WsfParseStringWithExceptionsRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   bool match = WsfParseStringRule::Read(aParser, aNode);
   if (match)
   {
      if (mExceptions.find(aNode->mValue.Text()) != mExceptions.end())
      {
         match = false;
         aParser.UndoRead();
         aParser.FreeNode(aNode);
         aNode = nullptr;
      }
   }
   return match;
}

bool WsfParseStringWithExceptionsRule::ReadRange(WsfParser&           aParser,
                                                 UtTextDocumentRange& aRange,
                                                 WsfParseNode*&       aNode,
                                                 bool                 aCreateNode)
{
   bool match = false;
   if (aRange.Valid())
   {
      match = true;
      if (mExceptions.find(aRange.Text()) != mExceptions.end())
      {
         match = false;
      }
      if (match && aCreateNode)
      {
         if (mNodeType.empty())
         {
            aNode = aParser.NewNode(this, "string", aRange);
         }
         else
         {
            aNode = aParser.NewNode(this, mNodeType.c_str(), aRange);
         }
         aNode->SetFlags(mFlags);
      }
   }
   return match;
}

bool WsfParseLineStringRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange firstWord = aParser.ReadWord();
   UtTextDocumentRange prevWord  = firstWord;
   if (firstWord.Valid())
   {
      size_t endPos = ut::npos;
      while (true)
      {
         UtTextDocumentRange word = aParser.ReadWord();
         if (word.mSource == prevWord.mSource)
         {
            for (size_t i = prevWord.GetEnd() + 1; i < word.GetBegin(); ++i)
            {
               if ((*word.mSource)[i] == '\n')
               {
                  endPos = i;
                  aParser.UndoRead();
                  break;
               }
            }
            if (endPos != ut::npos)
            {
               break;
            }
         }
         else
         {
            aParser.UndoRead();
            endPos = prevWord.mSource->GetText().Size() - 1;
            break;
         }
         prevWord = word;
      }
      firstWord.SetEnd(endPos);
      aNode = aParser.NewNode(this, "line-string", firstWord);
      return true;
   }
   return false;
}

bool WsfParseQuotedStringRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange word = aParser.ReadWord();
   if (!word.Valid())
   {
      aParser.UndoRead();
      return false;
   }

   if (*word.Pointer() != '\"')
   {
      if (mQuotesOptional)
      {
         aNode = aParser.NewNode(this, "string", word);
         if (mFileReference)
         {
            SetFileReference(word.Text(), aParser, aNode);
         }
         return true;
      }
      else
      {
         aParser.UndoRead();
         return false;
      }
   }
   UtTextDocumentRange range        = word;
   bool                notFirstWord = false;
   bool                foundEnd     = false;
   while (word.Valid())
   {
      std::string text = word.Text();
      for (size_t i = 0; i < text.size(); ++i)
      {
         if (text[i] == '\"' && (i > 0 || notFirstWord))
         {
            range.Extend(word.GetBegin() + i);
            foundEnd = true;
            break;
         }
      }
      if (foundEnd)
      {
         break;
      }
      notFirstWord = true;
      word         = aParser.ReadWord();
   }
   aNode = aParser.NewNode(this, "string", range);
   if (mFileReference)
   {
      UtTextDocumentRange noQuote(range.mSource, range.GetBegin() + 1, range.GetEnd() - 1);
      SetFileReference(noQuote.Text(), aParser, aNode);
   }
   return true;
}
void WsfParseQuotedStringRule::SetFileReference(const std::string& aFilePath, WsfParser& aParser, WsfParseNode* aNodePtr)
{
   std::string filePath;
   if (mFileSearchPaths)
   {
      UtTextDocument* sourcePtr = aParser.FindSource(aFilePath, mFileSearchPaths, false);
      if (sourcePtr)
      {
         filePath = sourcePtr->GetFilePath().GetSystemPath();
      }
   }
   if (filePath.empty())
   {
      filePath =
         (aParser.GetWorkingDirectory() + aParser.GetParseIndex()->SubstitutePathVariables(aFilePath)).GetNormalizedPath();
   }
   aNodePtr->SetAuxiliaryValue(new WsfParseFileReferenceData(filePath));
   aParser.AddAuxiliaryValue(aNodePtr->GetAuxiliaryValue());
}

WsfParseLiteral::WsfParseLiteral(WsfParseRuleMemory& aMem)
   : WsfParseRule(aMem, cLITERAL)
   , mCaseSensitive(true)
   , mIsTerminator(false)
   , mNodeFlags(0)
{
   mReaderFlags |= cIS_SINGLE_TOKEN;
   if (mText == "$EOF")
   {
      mText.clear();
   }
}

bool WsfParseLiteral::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange word;
   if (!mIsTerminator)
   {
      word = aParser.ReadWord();
   }
   else
   {
      word = aParser.ReadTerminator(mText);
   }
   bool pass = WsfParseLiteral::ReadRange(aParser, word, aNode, true);
   if (!pass)
   {
      aParser.UndoRead();
   }
   return pass;
}

bool RangeEqualNoCase(const std::string& aText, UtTextDocumentRange& aRange)
{
   bool   pass    = false;
   size_t txtSize = aText.size();
   if (aRange.Valid() && aRange.Length() == txtSize)
   {
      const char* wordPtr = aRange.Pointer();
      pass                = true;
      for (size_t i = 0; i < txtSize && pass; ++i)
      {
         pass = tolower(wordPtr[i]) == aText[i];
      }
   }
   return pass;
}

bool WsfParseLiteral::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   bool pass = false;
   if (mCaseSensitive)
   {
      if (aRange.Valid() && aRange == mText)
      {
         if (aCreateNode)
         {
            aNode = aParser.NewNode(this, "keyword", aRange);
         }
         pass = true;
      }
   }
   else
   {
      if (RangeEqualNoCase(mText, aRange))
      {
         if (aCreateNode)
         {
            aNode = aParser.NewNode(this, "keyword", aRange);
         }
         pass = true;
      }
   }

   if (mNodeFlags && pass && aNode)
   {
      aNode->SetFlags(aNode->GetFlags() | mNodeFlags);
   }
   return pass;
}

bool WsfParseLiteral::MatchesString(const std::string& aString)
{
   if (mCaseSensitive)
   {
      return aString == mText;
   }
   else
   {
      bool   pass    = false;
      size_t txtSize = mText.size();
      if (aString.size() == txtSize)
      {
         const char* wordPtr = aString.c_str();
         pass                = true;
         for (size_t i = 0; i < txtSize && pass; ++i)
         {
            pass = tolower(wordPtr[i]) == mText[i];
         }
      }
      return pass;
   }
}

bool WsfParseDelimited::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   UtTextDocumentRange range = aParser.ReadWord();
   if (range.Valid())
   {
      if (WsfParseDelimited::ReadRange(aParser, range, aNode, true))
      {
         return true;
      }
   }
   aParser.UndoRead();
   return false;
}

namespace
{
struct NoCasePred
{
   bool operator()(char a, char b) { return tolower(a) == tolower(b); }
};
} // namespace

WsfParseDelimited::WsfParseDelimited(WsfParseRuleMemory& aMem)
   : WsfParseRule(aMem, cDELIMITED)
   , mCaseSensitive(true)
   , mSubnodes()
   , mWords()
{
   mReaderFlags |= cIS_SINGLE_TOKEN;
}

bool WsfParseDelimited::ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode)
{
   size_t              wordCount  = 0;
   bool                pass       = true;
   UtTextDocument*     sourcePtr  = aRange.mSource;
   const char*         wordPtr    = aRange.Pointer();
   size_t              wordLength = aRange.Length();
   const char*         wordEndPtr = wordPtr + wordLength;
   UtTextDocumentRange remainder  = aRange;
   size_t              curPos     = 0;
   std::string&        firstDelim = mSections[0].mPreDelimiter;
   if (!firstDelim.empty())
   {
      size_t      dSize      = firstDelim.size();
      const char* delimBegin = firstDelim.c_str();
      bool        ok(false);
      if (wordLength >= dSize)
      {
         if (mCaseSensitive)
         {
            ok = std::equal(wordPtr, wordPtr + dSize, delimBegin);
         }
         else
         {
            ok = std::equal(wordPtr, wordPtr + dSize, delimBegin, NoCasePred());
         }
      }
      if (!ok)
      {
         return false;
      }
      curPos += dSize;
   }
   size_t readerCount = 1;
   for (size_t i = 1; i < mSections.size() && pass; ++i)
   {
      const Section& currentSection = mSections[i];
      if (currentSection.mRulePtr != nullptr)
      {
         ++readerCount;
      }
      const std::string& delim       = currentSection.mPreDelimiter;
      const size_t       delimSize   = delim.size();
      const char*        delimPtr    = delim.c_str();
      const char*        delimEndPtr = delimPtr + delimSize;
      if (delim.size() <= remainder.Length())
      {
         const char* delimBegin;
         size_t      searchStart = curPos;
         if (currentSection.mRulePtr == nullptr && mSections.size() == i + 1)
         {
            searchStart = wordLength - delimSize;
         }
         if (mCaseSensitive)
         {
            delimBegin = std::search(wordPtr + searchStart /*curPos*/, wordPtr + wordLength, delimPtr, delimEndPtr);
         }
         else
         {
            delimBegin =
               std::search(wordPtr + searchStart /*curPos*/, wordPtr + wordLength, delimPtr, delimEndPtr, NoCasePred());
         }
         if (delimBegin == wordEndPtr)
         {
            pass = false;
            break;
         }
         size_t offset = delimBegin - wordPtr;
         if (offset > curPos)
         {
            mWords[wordCount++] =
               UtTextDocumentRange(sourcePtr, aRange.GetBegin() + curPos, aRange.GetBegin() + offset - 1);
            curPos = offset + delim.size();
         }
      }
   }
   if (curPos < wordLength)
   {
      mWords[wordCount++] = UtTextDocumentRange(sourcePtr, aRange.GetBegin() + curPos, aRange.GetBegin() + wordLength - 1);
   }
   if (wordCount == readerCount)
   {
      for (size_t i = 0; i < wordCount && pass; ++i)
      {
         WsfParseNode* nodePtr = nullptr;
         pass                  = mSections[i].mRulePtr->ReadRange(aParser, mWords[i], nodePtr, false);
      }
   }

   if (!pass)
   {
   }
   else
   {
      if (aCreateNode)
      {
         aNode         = aParser.NewNode(this, "delimited");
         aNode->mValue = aRange;
      }
   }
   return pass;
}

// void WsfParseDelimitedReader::AddSection(const std::string& aPreDelimiter,
//                                          WsfParseReader*    aReaderPtr)
// {
//    Section section;
//    section.mPreDelimiter = aPreDelimiter;
//    section.mReaderPtr = aReaderPtr;
//    mSections.push_back(section);
// }
