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

#include "UtInputBuffer.hpp"

#include <iostream>

#include "UtLog.hpp"

// private
//! Extracts characters from the buffer and discards them.
//! The extraction ends when 'aNumberCharacters' has been extracted or when the
//! character 'aDelimiter' is found, whichever comes first.
//!
//! @param aNumberCharacters The maximum number of characters to extract.
//! @param aDelimiter The delimiting character.
void UtInputBuffer::Ignore(int aNumberCharacters, int aDelimiter /*= -1*/)
{
   for (int i = 0; i < aNumberCharacters; ++i)
   {
      char c;
      if (!Get(c))
      {
         break;
      }
      if (aDelimiter != -1 && aDelimiter == c)
      {
         break;
      }
   }
}


/** Read the next line from the input stream.
@return aValue The line read from the input stream.
@return 'true' if successful or 'false' if end of data encountered.
*/
bool UtInputBuffer::RealReadLine(std::string& aValue, bool aIgnoreCurrentLine)
{
   bool inputOk = false;
   aValue       = "";
   if (aIgnoreCurrentLine)
   {
      // first backtrack one character, then eat until newline
      char c;
      UnGet();
      while (Get(c) && c != '\n')
      {
      }
   }

   // Attempt to read the next line up to the first comment, skipping
   // lines that are entirely commented.
   bool processingComment = false;
   char ch;
   while (Get(ch))
   {
      // Comment types.
      bool pound      = !processingComment && (ch == '#');
      bool slashSlash = !processingComment && ((ch == '/') && (Peek() == '/'));
      bool slashStar  = !processingComment && ((ch == '/') && (Peek() == '*'));

      if (ch == '\n')
      {
         mEndOfLine = true;
         if (!aValue.empty())
         {
            inputOk = true;
            break;
         }
      }
      // If we encounter a '#' or '//', ignore the whole line.
      else if (pound || slashSlash)
      {
         processingComment = true;

         Ignore(99999, '\n');
         mEndOfLine = true;
         if (!aValue.empty())
         {
            break;
         }
      }
      // If we encounter a '/*', read characters until '*/' is encountered.
      else if (slashStar)
      {
         processingComment = true;

         while (Get(ch))
         {
            if ((ch == '*') && (Peek() == '/'))
            {
               // Read the closing '/'
               Get(ch);
               inputOk = true;
               break;
            }
            else if ((ch == '/') && (Peek() == '*'))
            {
               ut::log::warning() << "Mismatched /* */ comment.";
            }
         }
      }
      else
      {
         aValue += ch;
         inputOk = true;
      }
   }
   return inputOk;
}

// protected
// virtual
/** Read the next word (string) from the input stream.
@param aValue The next word from the input stream.
@return 'true' if successful or 'false' if end of data encountered.
*/
bool UtInputBuffer::RealReadString(std::string& aValue)
{
   bool inputOk = false;
   aValue       = "";

   // Skip leading white space
   SkipWhiteSpace();

   // Assemble the string.

   char ch;
   mEndOfLine = false;
   while (Get(ch))
   {
      if (isspace((unsigned char)ch) == 0)
      {
         if (!SkipComments(ch))
         {
            if (ch == '"')
            {
               // Quoted strings may have comments in them.  In that case, we should ignore them,
               // so we call RealReadStringQuoted instead.
               UnGet();
               std::string quoted;
               inputOk = RealReadStringQuoted(quoted);
               aValue += '"';
               aValue += quoted;
               aValue += '"';
            }
            else
            {
               aValue += ch;
               inputOk = true;
            }
         }
      }
      else
      {
         if (ch == '\n')
         {
            mEndOfLine = true;
         }
         UnGet();
         SearchEndOfLine();
         break;
      }
   }
   return inputOk;
}

//! Skip white space and comments.
void UtInputBuffer::SkipWhiteSpace()
{
   // Skip white space

   char ch;
   while (Get(ch))
   {
      if (isspace((unsigned char)ch) == 0)
      {
         if (!SkipComments(ch))
         {
            UnGet();
            break;
         }
      }
   }
}

//! Determine whether a single character is a comment.
//! Comments include c-style comments (/* */), c++-style
//! (//) and pound-hash (#).  If a comment is encountered the rest of the
//! line is ingored.
//!@param aCh The charater to be evaluted.
//!@return Whether that character is a comment.
bool UtInputBuffer::SkipComments(char aCh)
{
   bool skippedComments = false;
   char ch              = aCh;

   // Comment types.
   bool pound      = (ch == '#');
   bool slashSlash = ((ch == '/') && (Peek() == '/'));
   bool slashStar  = ((ch == '/') && (Peek() == '*'));

   // If we encounter a '#' or '//', remove the rest of the line.
   if (pound || slashSlash)
   {
      Ignore(99999, '\n');
      skippedComments = true;
   }
   // If we encounter a '/*', remove characters until '*/' is encountered.
   else if (slashStar)
   {
      while (Get(ch))
      {
         if ((ch == '*') && (Peek() == '/'))
         {
            // Read the closing '/'
            Get(ch);
            skippedComments = true;
            break;
         }
         else if ((ch == '/') && (Peek() == '*'))
         {
            ut::log::warning() << "Mismatched /* */ comment.";
         }
         else if (ch == '\n')
         {
         }
      }
   }
   return skippedComments;
}

// protected
// virtual
//! Read values inside quotes.
//!@param aValue The string inside quotes.
//!@return Whether the value was read successfully.
bool UtInputBuffer::RealReadStringQuoted(std::string& aValue)
{
   bool inputOk = false;
   aValue       = "";

   // Skip leading white space, but do include comment characters.
   char ch;
   while (Get(ch))
   {
      if (isspace((unsigned char)ch) == 0)
      {
         UnGet();
         break;
      }
   }

   // Assemble the string.
   // char ch;
   Get(ch);

   // If the string is quoted, read it verbatim.
   if (ch == '"')
   {
      mEndOfLine = false;
      while (Get(ch))
      {
         if (ch != '"')
         {
            aValue += ch;
            inputOk = true;
         }
         else
         {
            inputOk = true;
            SearchEndOfLine();
            break;
         }
      }
   }
   // If the string is unquoted, use the standard read method.
   else
   {
      UnGet();
      inputOk = RealReadString(aValue);
   }
   return inputOk;
}

// private
//! Searches for the end of the current line, if all non-whitespace
//! characters are encountered, mEndOfLine is set to true;
//!
//! @note Characters are not consumed.
void UtInputBuffer::SearchEndOfLine()
{
   if (mEndOfLine)
   {
      return;
   }

   int  count = 0;
   char ch;
   while (Get(ch))
   {
      ++count;
      if (ch == '\n')
      {
         mEndOfLine = true;
         break;
      }
      if (isspace((unsigned char)ch) == 0)
      {
         break;
      }
   }

   // Move the get pointer back to where we started.
   while (count > 0)
   {
      --count;
      UnGet();
   }
}
