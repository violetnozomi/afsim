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

#include "WsfParseScriptScanner.hpp"

#include "UtCast.hpp"

WsfParseScriptScanner::WsfParseScriptScanner()
   : mScriptClassTypes()
   , mSourcePtr(nullptr)
   , mChBegin()
   , mChIter()
   , mChEnd()
   , mOffset(0)
   , ch()
   , cc()
{
   // Initialize the character code map.
   unsigned int i;
   for (i = 0; i <= 127; ++i)
   {
      mCharCodeMap[i] = cCC_ERROR;
   }
   for (i = 'a'; i <= 'z'; ++i)
   {
      mCharCodeMap[i] = cCC_LETTER;
   }
   for (i = 'A'; i <= 'Z'; ++i)
   {
      mCharCodeMap[i] = cCC_LETTER;
   }
   for (i = '0'; i <= '9'; ++i)
   {
      mCharCodeMap[i] = cCC_DIGIT;
   }

   mCharCodeMap[static_cast<unsigned int>('_')]  = cCC_LETTER;
   mCharCodeMap[static_cast<unsigned int>('+')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('-')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('*')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('#')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('/')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('=')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('^')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('.')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>(',')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('<')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('>')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('(')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>(')')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('[')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>(']')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('{')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('}')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>(';')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>(':')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('!')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('&')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>('|')]  = cCC_SPECIAL;
   mCharCodeMap[static_cast<unsigned int>(' ')]  = cCC_WHITE_SPACE;
   mCharCodeMap[static_cast<unsigned int>('\t')] = cCC_WHITE_SPACE;
   mCharCodeMap[static_cast<unsigned int>('\n')] = cCC_LINE_FEED;
   mCharCodeMap[static_cast<unsigned int>('\r')] = cCC_LINE_FEED;
   mCharCodeMap[static_cast<unsigned int>('\0')] = cCC_WHITE_SPACE;
   mCharCodeMap[static_cast<unsigned int>('\"')] = cCC_QUOTE;
   mCharCodeMap[static_cast<unsigned int>('\'')] = cCC_QUOTE;
   mCharCodeMap[static_cast<unsigned int>(0x7F)] = cCC_SPECIAL;

   // Add the reserved words to the map.
   mReservedWordMap["do"]                = cTC_DO;
   mReservedWordMap["if"]                = cTC_IF;
   mReservedWordMap["for"]               = cTC_FOR;
   mReservedWordMap["foreach"]           = cTC_FOREACH;
   mReservedWordMap["else"]              = cTC_ELSE;
   mReservedWordMap["while"]             = cTC_WHILE;
   mReservedWordMap["return"]            = cTC_RETURN;
   mReservedWordMap["break"]             = cTC_BREAK;
   mReservedWordMap["continue"]          = cTC_CONTINUE;
   mReservedWordMap["null"]              = cTC_NULL;
   mReservedWordMap["NULL"]              = cTC_NULL;
   mReservedWordMap["true"]              = cTC_TRUE;
   mReservedWordMap["false"]             = cTC_FALSE;
   mReservedWordMap["print"]             = cTC_PRINT;
   mReservedWordMap["write"]             = cTC_WRITE;
   mReservedWordMap["writeln"]           = cTC_WRITE_LINE;
   mReservedWordMap["write_d"]           = cTC_WRITE_D;
   mReservedWordMap["writeln_fatal"]     = cTC_WRITE_LINE_FATAL;
   mReservedWordMap["writeln_error"]     = cTC_WRITE_LINE_ERROR;
   mReservedWordMap["writeln_warning"]   = cTC_WRITE_LINE_WARNING;
   mReservedWordMap["writeln_debug"]     = cTC_WRITE_LINE_DEBUG;
   mReservedWordMap["writeln_developer"] = cTC_WRITE_LINE_DEVELOPER;
   mReservedWordMap["writeln_d"]         = cTC_WRITE_LINE_D;
   mReservedWordMap["write_str"]         = cTC_WRITE_STR;
   mReservedWordMap["read"]              = cTC_READ;
   mReservedWordMap["script"]            = cTC_SCRIPT;
   mReservedWordMap["int"]               = cTC_INT;
   mReservedWordMap["bool"]              = cTC_BOOL;
   mReservedWordMap["char"]              = cTC_CHAR;
   mReservedWordMap["string"]            = cTC_STR;
   mReservedWordMap["double"]            = cTC_DOUBLE;
   mReservedWordMap["void"]              = cTC_VOID;
   mReservedWordMap["global"]            = cTC_GLOBAL;
   mReservedWordMap["static"]            = cTC_STATIC;
   mReservedWordMap["extern"]            = cTC_EXTERN;
   mReservedWordMap["in"]                = cTC_IN;
}

void WsfParseScriptScanner::Prime(const UtTextDocumentRange& aInput)
{
   mOffset    = aInput.GetBegin();
   mSourcePtr = aInput.mSource;
   mChBegin   = (char*)aInput.Pointer();
   mChIter    = mChBegin;
   mChEnd     = mChIter + aInput.Length();
   --mChIter;
   GetCh();
   Get();
}

const WsfParseScriptToken& WsfParseScriptScanner::Get()
{
   std::swap(mTok, mNextTok);
   ReadToken(mNextTok);
   return mTok;
}

const WsfParseScriptToken& WsfParseScriptScanner::Peek()
{
   return mNextTok;
}

void WsfParseScriptScanner::ReadToken(WsfParseScriptToken& aToken)
{
   int   state    = cNORMAL;
   char* tokBegin = mChIter;
   while (mChIter < mChEnd)
   {
      switch (state)
      {
      case cNUMBER:
         switch (cc)
         {
         case cCC_DIGIT:
            GetCh();
            break;
         default:
            if (ch == '.')
            {
               state = cNUMBER_DECIMAL;
               GetCh();
               break;
            }
            else if (ch == 'e' || ch == 'E')
            {
               GetCh();
               state = cNUMBER_EXPONENT;
               break;
            }
            SetToken(aToken, tokBegin, mChIter - 1, cTC_NUMBER);
            return;
         }
         break;
      case cNUMBER_EXPONENT:
         switch (cc)
         {
         case cCC_DIGIT:
            GetCh();
            break;
         default:
            SetToken(aToken, tokBegin, mChIter, cTC_NUMBER);
            return;
         }
      case cNUMBER_DECIMAL:
         switch (cc)
         {
         case cCC_DIGIT:
            GetCh();
            break;
         case cCC_LETTER:
            if (ch == 'e' || ch == 'E')
            {
               GetCh();
               if (ch == '-' || ch == '+')
               {
                  GetCh();
               }
               state = cNUMBER_EXPONENT;
            }
            else
            {
               SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
               return;
            }
            break;
         default:
            SetToken(aToken, tokBegin, mChIter - 1, cTC_NUMBER);
            return;
         }
         break;
      case cSINGLE_QUOTE:
         if (ch == '\'')
         {
            SetToken(aToken, tokBegin, mChIter, cTC_STRING);
            GetCh();
            return;
         }
         else if (cc == cCC_EOF)
         {
            SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
            return;
         }
         GetCh();
         break;
      case cDOUBLE_QUOTE:
         if (ch == '\"')
         {
            SetToken(aToken, tokBegin, mChIter, cTC_STRING);
            GetCh();
            return;
         }
         else if (cc == cCC_EOF)
         {
            SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
            return;
         }
         GetCh();
         break;
      case cIDENTIFIER:
         if (cc == cCC_LETTER || cc == cCC_DIGIT)
         {
            GetCh();
         }
         else
         {
            SetToken(aToken, tokBegin, mChIter - 1, cTC_IDENTIFIER);
            std::string tokStr = aToken.Text();
            auto        iter   = mReservedWordMap.find(tokStr);
            if (iter != mReservedWordMap.end())
            {
               aToken.mTokenCode = iter->second;
            }
            else
            {
               if (mScriptClassTypes.find(tokStr) != mScriptClassTypes.end())
               {
                  aToken.mTokenCode = cTC_TYPE;
               }
            }
            return;
         }
         break;
      case cLINE_COMMENT:
         if (ch == '\n')
         {
            state = cNORMAL;
         }
         else
         {
            GetCh();
         }
         break;
      case cBLOCK_COMMENT:
         if (ch == '*')
         {
            GetCh();
            if (ch == '/')
            {
               state = cNORMAL;
               GetCh();
            }
         }
         else if (cc == cCC_EOF)
         {
            SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
            return;
         }
         else
         {
            GetCh();
         }
         break;
      case cNORMAL:
         tokBegin = mChIter;
         switch (cc)
         {
         case cCC_LETTER:
            state = cIDENTIFIER;
            GetCh();
            break;
         case cCC_DIGIT:
            state = cNUMBER;
            break;
         case cCC_WHITE_SPACE:
            GetCh();
            break;
         case cCC_QUOTE:
            if (ch == '\'')
            {
               state = cSINGLE_QUOTE;
            }
            else
            {
               state = cDOUBLE_QUOTE;
            }
            GetCh();
            break;
         case cCC_LINE_FEED:
            GetCh();
            break;
         case cCC_EOF:
            SetToken(aToken, tokBegin, 0, cTC_EOF);
            return;
         case cCC_SPECIAL:
            switch (ch)
            {
            case '*':
               SetToken(aToken, tokBegin, 1, cTC_STAR);
               GetCh();
               return;
            case '(':
               SetToken(aToken, tokBegin, 1, cTC_LEFT_PAREN);
               GetCh();
               return;
            case ')':
               SetToken(aToken, tokBegin, 1, cTC_RIGHT_PAREN);
               GetCh();
               return;
            case '-':
               SetToken(aToken, tokBegin, 1, cTC_MINUS);
               GetCh();
               return;
            case '+':
               SetToken(aToken, tokBegin, 1, cTC_PLUS);
               GetCh();
               return;
            case '=': // = or ==
            {
               GetCh();
               if (ch == '=')
               {
                  SetToken(aToken, tokBegin, 2, cTC_EQUAL_TO);
                  GetCh();
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_EQUAL);
               }
               return;
            }
            case '[':
               SetToken(aToken, tokBegin, 1, cTC_LEFT_BRACKET);
               GetCh();
               return;
            case ']':
               SetToken(aToken, tokBegin, 1, cTC_RIGHT_BRACKET);
               GetCh();
               return;
            case '{':
               SetToken(aToken, tokBegin, 1, cTC_LEFT_BRACE);
               GetCh();
               return;
            case '}':
               SetToken(aToken, tokBegin, 1, cTC_RIGHT_BRACE);
               GetCh();
               return;
            case ':':
               SetToken(aToken, tokBegin, 1, cTC_COLON);
               GetCh();
               return;
            case ';':
               SetToken(aToken, tokBegin, 1, cTC_SEMICOLON);
               GetCh();
               return;
            case ',':
               SetToken(aToken, tokBegin, 1, cTC_COMMA);
               GetCh();
               return;
            case '.':
            {
               GetCh();
               if (cc == cCC_DIGIT)
               {
                  state = cNUMBER_DECIMAL;
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_PERIOD);
                  return;
               }
            }
            break;
            // SetToken(aToken, tokBegin, 1, cTC_PERIOD); GetCh();  break;
            case '#':
            {
               state = cLINE_COMMENT;
               GetCh();
               break;
            }
            case '/':
            {
               GetCh();
               if (ch == '/')
               {
                  state = cLINE_COMMENT;
               }
               else if (ch == '*')
               {
                  state = cBLOCK_COMMENT;
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_SLASH);
                  return;
               }
            }
            break;
            case '!': // ! or !=
            {
               GetCh();
               if (ch == '=')
               {
                  SetToken(aToken, tokBegin, 2, cTC_NOT_EQUAL);
                  GetCh();
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_NOT);
               }
               return;
            }
            case 0:
            case 0x7F:
               SetToken(aToken, tokBegin, 1, cTC_EOF);
               GetCh();
               break;
            case '&':
            {
               GetCh();
               if (ch == '&')
               {
                  SetToken(aToken, tokBegin, 2, cTC_AND);
                  GetCh();
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_ERROR);
               }
               return;
            }
            case '|':
            {
               GetCh();
               if (ch == '|')
               {
                  SetToken(aToken, tokBegin, 2, cTC_OR);
                  GetCh();
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_ERROR);
               }
               return;
            }
            case '<': // < or <=
            {
               GetCh();
               if (ch == '=')
               {
                  SetToken(aToken, tokBegin, 2, cTC_LESS_EQUAL);
                  GetCh();
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_LESS_THAN);
               }
               return;
            }
            case '>': // > or >=
            {
               GetCh();
               if (ch == '=')
               {
                  SetToken(aToken, tokBegin, 2, cTC_GREATER_EQUAL);
                  GetCh();
               }
               else
               {
                  SetToken(aToken, tokBegin, 1, cTC_GREATER_THAN);
               }
               return;
            }
            default:
               SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
               GetCh();
               return;
            }
            break;
         default:
            SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
            GetCh();
            return;
         }
         break;
      default:
         SetToken(aToken, tokBegin, mChIter, cTC_ERROR);
         GetCh();
         return;
      }
   }
   SetToken(aToken, tokBegin, 1, cTC_EOF);
}

void WsfParseScriptScanner::SetToken(WsfParseScriptToken& aToken, char* aBeginPtr, char* aEndPtr, int aTokenCode)
{
   aToken.SetRange(aBeginPtr - mChBegin + mOffset, aEndPtr - mChBegin + mOffset);
   aToken.mSource    = mSourcePtr;
   aToken.mTokenCode = aTokenCode;
}

void WsfParseScriptScanner::SetToken(WsfParseScriptToken& aToken, char* aBeginPtr, int aLength, int aTokenCode)
{
   size_t startPos = aBeginPtr - mChBegin + mOffset;
   aToken.SetRange(startPos, startPos + ut::cast_to_size_t(aLength) - 1);
   aToken.mSource    = mSourcePtr;
   aToken.mTokenCode = aTokenCode;
}
