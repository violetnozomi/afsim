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

#include "UtTokenizer.hpp"

#include <cstdlib>
#include <iostream>

#include "UtLog.hpp"


//-----------------------------------------------------
//! Use this constructor if you want to tokenize a file
//!   with a specific delimiter(s).
//-----------------------------------------------------
UtTokenizer::UtTokenizer(const std::string& aFilename, const std::string& aDelimiter, const std::string& aComment)
   : mDelimiter(aDelimiter)
   , mLastPos(0)
   , mPos(0)
   , mCurTokenIndex(-1)
{
   mFile.open(aFilename.c_str());

   mOpenFlag = mFile.good();

   if (mOpenFlag)
   {
      char tmp[1024];
      while (mFile >> mToken)
      {
         bool addToken = true;

         mOpenFlag = mFile.good();
         if (!mOpenFlag)
         {
            break;
         }

         // If there is a comment at the beginning of the token,
         // throw away the rest of the line.
         std::string::size_type pLast = 0;
         std::string::size_type pNext = 0;
         std::string            comment;
         while (pNext != std::string::npos)
         {
            pNext   = aComment.find(" ", pLast);
            comment = aComment.substr(pLast, pNext);
            pLast   = pNext + 1;

            if (mToken.find_first_of(comment) == 0)
            {
               mFile.getline(tmp, sizeof(tmp));
               addToken = false;
               break;
            }
         }

         // If the token starts with a " and doesn't end with a ",
         // then the string is spread across multiple tokens.  Search
         // forward in the stream to find the closing "" and store the
         // the whole string as a single token.
         if (mToken.length() > 0 && mToken[0] == '\"')
         {
            if ((mToken.length() == 1) ||              // The case where we have a single " as a token.
                (mToken[mToken.length() - 1] != '\"')) // Make sure the last character isn't already a ".
            {
               char nextChar;
               mFile.get(nextChar);
               while (nextChar != '\"')
               {
                  mToken += nextChar;
                  mFile.get(nextChar);
               }
               mToken += nextChar;
            }
         }

         // Add the token to the buffer.
         if (addToken)
         {
            mTokenList.push_back(mToken);
         }
      }
   }
}

UtTokenizer::~UtTokenizer()
{
   mFile.close();
}

//--------------------------------------------------
//! Return true if the file is open, otherwise false.
//--------------------------------------------------
bool UtTokenizer::IsOpen() const
{
   return mOpenFlag;
}

//-----------------------------------------------
//! Set the delimiter that separates your tokens
//!    (i.e. "," or "\t" etc).
//-----------------------------------------------
void UtTokenizer::SetDelimiter(const std::string& aDelimiter)
{
   mDelimiter = aDelimiter;
}

//-----------------------------------------------
//! Set a string to parse based on the currently
//!   set delimiter.
//-----------------------------------------------
void UtTokenizer::SetParseString(const std::string& aBuffer)
{
   mBuffer = aBuffer;

   mToken = "";

   mLastPos = mBuffer.find_first_not_of(mDelimiter.c_str(), 0);
   mPos     = mBuffer.find_first_of(mDelimiter.c_str(), mLastPos);
}

//-------------------------------------------------
//! Calling NextToken will parse the next token.
//!   If no token exits false will be returned.
//-------------------------------------------------
bool UtTokenizer::NextToken()
{
   if ((mCurTokenIndex + 1) < (int)mTokenList.size())
   {
      ++mCurTokenIndex;
      return true;
   }
   return false;
}

//-----------------------------------------------------
//! GetToken will return the current token as a string.
//-----------------------------------------------------
std::string UtTokenizer::GetToken() const
{
   if (mCurTokenIndex >= 0 && mCurTokenIndex < (int)mTokenList.size())
   {
      return mTokenList[mCurTokenIndex];
   }

   auto out = ut::log::warning() << "UtTokenizer::GetToken: Index out of bounds.";
   out.AddNote() << "Index: " << mCurTokenIndex;
   return "";
}

//-----------------------------------------------------------------------
//! return the token we just read interpreted at a floating point value.
//-----------------------------------------------------------------------
double UtTokenizer::GetFloatValue() const
{
   return atof(mTokenList[mCurTokenIndex].c_str());
}

//-----------------------------------------------------------------------
//! return the token we just read interpreted at an integer value.
//-----------------------------------------------------------------------
int UtTokenizer::GetIntValue() const
{
   return atoi(mTokenList[mCurTokenIndex].c_str());
}

//-----------------------------
//! return the next token.
//-----------------------------
std::string UtTokenizer::GetNextToken()
{
   NextToken();
   return GetToken();
}

//--------------------------------------
//! return the next token's float value.
//--------------------------------------
double UtTokenizer::GetNextFloatValue()
{
   NextToken();
   return GetFloatValue();
}

//--------------------------------------
//! return the next token's int value.
//--------------------------------------
int UtTokenizer::GetNextIntValue()
{
   NextToken();
   return GetIntValue();
}

//---------------------------------
//! Prints the entire buffer.
//---------------------------------
void UtTokenizer::Print()
{
   ut::log::info() << mBuffer;
}
