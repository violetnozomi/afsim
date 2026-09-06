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

#ifndef UTSCRIPTSTREAM_HPP
#define UTSCRIPTSTREAM_HPP

#include "util_script_export.h"

#include <string>

class UtInput;
class UtInputBuffer;

//--------------------------------------------------------------
//! UtScriptStream is an abstract base class for script streams
//! that the scanner and parser operate on.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT UtScriptStream
{
public:
   UtScriptStream(UtInput& aInput, const std::string& aEndBlockToken);

   virtual ~UtScriptStream() = default;

   const char* GetText(int& aLength);

   size_t GetOffset();

   void OffsetToLineColumn(size_t aOffset, size_t& aLine, size_t& aColumn);

   std::string GetLineStringFromOffset(size_t aOffset);
   std::string GetFilePath();

private:
   char GetLine();

   void        PushBackTokens(const std::string& aTokens);
   std::string mEndBlockToken;

   size_t         mPos;
   size_t         mEndPos;
   UtInputBuffer* mBufferPtr;
   std::string    mText;
};

#endif
