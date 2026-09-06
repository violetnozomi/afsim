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

#ifndef WSFPARSESCRIPTSCANNER_HPP
#define WSFPARSESCRIPTSCANNER_HPP
#include <map>
#include <set>

#include "UtTextDocument.hpp"

class WsfParseScriptToken : public UtTextDocumentRange
{
public:
   WsfParseScriptToken()
      : mTokenCode()
   {
   }
   WsfParseScriptToken(UtTextDocumentRange& aRange, int aTokenCode)
      : UtTextDocumentRange(aRange)
      , mTokenCode(aTokenCode)
   {
   }
   int mTokenCode;
};

class WsfParseScriptScanner
{
public:
   WsfParseScriptScanner();
   void Prime(const UtTextDocumentRange& aInput);

   const WsfParseScriptToken& Get();
   const WsfParseScriptToken& Peek();

   //! Enumerations for each specific possible token type.
   enum TokenCode
   {
      cTC_DUMMY,
      cTC_IDENTIFIER,
      cTC_NUMBER,
      cTC_STRING,
      cTC_EOF,
      cTC_ERROR,

      cTC_LEFT_PAREN,
      cTC_RIGHT_PAREN,
      cTC_LEFT_BRACKET,
      cTC_RIGHT_BRACKET,
      cTC_LEFT_BRACE,
      cTC_RIGHT_BRACE,

      cTC_COLON,
      cTC_SEMICOLON,
      cTC_COMMA,
      cTC_PERIOD,

      cTC_PLUS,
      cTC_MINUS,
      cTC_STAR,
      cTC_POUND,
      cTC_SLASH,
      cTC_LESS_THAN,
      cTC_LESS_EQUAL,
      cTC_GREATER_THAN,
      cTC_GREATER_EQUAL,
      cTC_EQUAL_TO,
      cTC_NOT_EQUAL,
      cTC_AND,
      cTC_OR,
      cTC_NOT,
      cTC_EQUAL,

      // There is a dependency on cTC_START_OF_RESERVED_WORDS being at this location
      // in the enumeration list. If you want to move it, make sure to check the source
      // for how it's position is used.
      cTC_START_OF_RESERVED_WORDS,

      cTC_CASE,
      cTC_CONST,
      cTC_DO,
      cTC_ELSE,
      cTC_FOR,
      cTC_FOREACH,
      cTC_IF,
      cTC_NULL,
      cTC_WHILE,
      cTC_BREAK,
      cTC_RETURN,
      cTC_CONTINUE,
      cTC_TRUE,
      cTC_FALSE,
      cTC_PRINT,
      cTC_WRITE,
      cTC_WRITE_LINE,
      cTC_WRITE_D,
      cTC_WRITE_LINE_FATAL,
      cTC_WRITE_LINE_ERROR,
      cTC_WRITE_LINE_WARNING,
      cTC_WRITE_LINE_DEBUG,
      cTC_WRITE_LINE_D,
      cTC_WRITE_LINE_DEVELOPER,
      cTC_WRITE_STR,
      cTC_READ,
      cTC_SCRIPT,
      cTC_FUNCTION,
      cTC_INT,
      cTC_BOOL,
      cTC_CHAR,
      cTC_STR,
      cTC_DOUBLE,
      cTC_VOID,
      cTC_TYPE,
      cTC_GLOBAL,
      cTC_STATIC,
      cTC_EXTERN,
      cTC_IN
   };
   //! Enumeration for each possible character type the
   //! scanner may encounter.
   enum CharCode
   {
      cCC_LETTER,
      cCC_DIGIT,
      cCC_SPECIAL,
      cCC_QUOTE,
      cCC_WHITE_SPACE,
      cCC_LINE_FEED,
      cCC_EOF,
      cCC_ERROR
   };
   std::set<std::string> mScriptClassTypes; // temporary

protected:
   enum State
   {
      cNORMAL,
      cNUMBER,
      cNUMBER_DECIMAL,
      cNUMBER_EXPONENT,
      cSINGLE_QUOTE,
      cDOUBLE_QUOTE,
      cIDENTIFIER,
      cLINE_COMMENT,
      cBLOCK_COMMENT
   };
   void ReadToken(WsfParseScriptToken& aToken);
   void SkipWhiteSpace();
   void SetToken(WsfParseScriptToken& aToken, char* aBeginPtr, char* aEndPtr, int aTokenCode);
   void SetToken(WsfParseScriptToken& aToken, char* aBeginPtr, int aLength, int aTokenCode);
   void Ch()
   {
      if (mChIter < mChEnd)
      {
         ch = *mChIter;
         cc = (CharCode)mCharCodeMap[static_cast<int>(ch)];
      }
      else
      {
         ch = 0;
         cc = cCC_EOF;
      }
   }

   void GetCh()
   {
      ++mChIter;
      Ch();
   }
   void BackCh()
   {
      --mChIter;
      Ch();
   }

   UtTextDocument*     mSourcePtr;
   WsfParseScriptToken mTok;
   WsfParseScriptToken mNextTok;
   char*               mChBegin;
   char*               mChIter;
   char*               mChEnd;
   ptrdiff_t           mOffset;
   //! Maps a character to its code
   CharCode                         mCharCodeMap[128];
   char                             ch;
   CharCode                         cc;
   std::map<std::string, TokenCode> mReservedWordMap;
};

#endif
