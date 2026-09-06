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

#include "UtScriptStream.hpp"

#include <iostream>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputFile.hpp"

UtScriptStream::UtScriptStream(UtInput& aInput, const std::string& aEndBlockToken)
{
   mBufferPtr = aInput.GetCurrentInput();

   UtInputBuffer* inputPtr = aInput.GetCurrentInput();
   mPos                    = inputPtr->GetOffset();
   if (!aInput.GetPushedBackStrings().empty())
   {
      mPos -= aInput.GetPushedBackStrings().back().size();
   }

   size_t endOffset = mPos;
   // Search for end
   mEndPos = mPos;
   for (;;)
   {
      std::string tok;
      if (!aInput.TryReadCommand(tok) || aInput.GetCurrentInput() != mBufferPtr)
      {
         if (tok.empty())
         {
            throw UtInput::EndOfData(aInput);
         }
         endOffset = mEndPos = mBufferPtr->GetOffset();
         break;
      }
      if (!aEndBlockToken.empty() && tok == aEndBlockToken)
      {
         endOffset = mBufferPtr->GetOffset();
         mEndPos   = endOffset - (aEndBlockToken.size() + 1);
         break;
      }
   }
   aInput.ClearPushedBackStrings();

   mBufferPtr->SeekTo(mPos);

   // Copy the script text out of the input buffer
   for (size_t i = mPos; i < mEndPos; ++i)
   {
      char c(0);
      mBufferPtr->Get(c);
      if (c)
      {
         mText += c;
      }
   }
   mBufferPtr->SeekTo(endOffset);
}

size_t UtScriptStream::GetOffset()
{
   return mPos;
}

void UtScriptStream::OffsetToLineColumn(size_t aOffset, size_t& aLine, size_t& aColumn)
{
   size_t offsetNow = mBufferPtr->GetOffset();
   mBufferPtr->SeekTo(aOffset);
   mBufferPtr->GetLineColumn(aLine, aColumn);
   mBufferPtr->SeekTo(offsetNow);
}

std::string UtScriptStream::GetLineStringFromOffset(size_t aOffset)
{
   size_t saveOffset = mBufferPtr->GetOffset();

   for (size_t i = (aOffset > 0 ? aOffset - 1 : 0);; i--)
   {
      mBufferPtr->SeekTo(i);
      char c(0);
      mBufferPtr->Get(c);
      if (c == '\n')
      {
         break;
      }

      // If we reach the end of the buffer, and no newline was found all the text should be included.
      //  Need to reset mPos of the buffer back to 0, so the first character is not skipped.
      if (i == 0)
      {
         mBufferPtr->SeekTo(0);
         break;
      }
   }
   std::string lineText;
   for (char c = '.'; mBufferPtr->Get(c) && c != '\n';)
   {
      lineText += c;
   }
   mBufferPtr->SeekTo(saveOffset);
   return lineText;
}

std::string UtScriptStream::GetFilePath()
{
   return mBufferPtr->GetFileName();
}

const char* UtScriptStream::GetText(int& aLength)
{
   aLength = (int)mText.size();
   return mText.c_str();
}
