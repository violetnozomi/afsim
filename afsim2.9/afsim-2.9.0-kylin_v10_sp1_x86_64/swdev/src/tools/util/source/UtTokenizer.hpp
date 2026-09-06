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

#ifndef UTTOKENIZER_HPP
#define UTTOKENIZER_HPP

#include "ut_export.h"

#include <fstream>
#include <string>
#include <vector>

class UT_EXPORT UtTokenizer
{
public:
   //-----------------------------------------------------
   // Use the default constructor if you want to tokenize
   //   a user provide string.
   //-----------------------------------------------------
   UtTokenizer() = default;

   //-----------------------------------------------------
   // Use this constructor if you want to tokenize a file
   //   with a specific delimiter(s). aComment is the comment
   //   style.  More than one comment style can be used by
   //   separating them by spaces.  For example, aComment = "// #"
   //-----------------------------------------------------
   UtTokenizer(const std::string& aFilename, const std::string& aDelimiter, const std::string& aComment = "//");

   ~UtTokenizer();

   //--------------------------------------------------
   // Return true if the file is open, otherwise false.
   //--------------------------------------------------
   bool IsOpen() const;

   //-----------------------------------------------
   // Set the delimiter that separates your tokens
   //    (i.e. "," or "\t" etc).
   //-----------------------------------------------
   void SetDelimiter(const std::string& aDelimiter);

   //-----------------------------------------------
   // Set a string to parse based on the currently
   //   set delimiter.
   //-----------------------------------------------
   void SetParseString(const std::string& aBuffer);

   //-------------------------------------------------
   // Calling NextToken will parse the next token.
   //   If no token exits false will be returned.
   //-------------------------------------------------
   bool NextToken();

   //-----------------------------------------------------
   // GetToken will return the current token as a string.
   //-----------------------------------------------------
   std::string GetToken() const;

   //-----------------------------------------------------------------------
   // return the token we just read interpreted at a floating point value.
   //-----------------------------------------------------------------------
   double GetFloatValue() const;

   //-----------------------------------------------------------------------
   // return the token we just read interpreted at an integer value.
   //-----------------------------------------------------------------------
   int GetIntValue() const;

   //-----------------------------
   // return the next token.
   //-----------------------------
   std::string GetNextToken();

   //--------------------------------------
   // return the next token's float value.
   //--------------------------------------
   double GetNextFloatValue();

   //--------------------------------------
   // return the next token's int value.
   //--------------------------------------
   int GetNextIntValue();

   //-----------------------------
   // Prints the entire buffer.
   //-----------------------------
   void Print();

private:
   std::ifstream          mFile;
   std::string            mToken;
   std::string            mBuffer;
   std::string            mDelimiter;
   std::string::size_type mLastPos{0};
   std::string::size_type mPos{0};
   bool                   mOpenFlag{false};

   std::vector<std::string> mTokenList;
   int                      mCurTokenIndex{-1};
};

#endif
