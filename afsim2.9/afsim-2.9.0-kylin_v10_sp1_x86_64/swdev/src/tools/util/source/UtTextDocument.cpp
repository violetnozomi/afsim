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

#include "UtTextDocument.hpp"

#include <algorithm>

#include "UtCast.hpp"

UtCallbackListN<void(const UtTextDocument*, size_t, int)> UtTextDocument::TextDocumentChanged;

UtTextDocument::UtTextDocument(const UtPath& aPath)
{
   InitLines();
   ReadFile(aPath);
}

UtTextDocument::UtTextDocument()
   : mLastUpdateLine(0)
   , mUpdateChars(0)
{
   mText.Insert(0, 0); // null terminator
   InitLines();
}

UtTextDocument::UtTextDocument(std::istream& aInput)
{
   InitLines();
   Read(aInput);
}

//! Read a file from an input file path.
bool UtTextDocument::ReadFile(const UtPath& aPath)
{
   mFilePath = aPath;
   std::ifstream f(mFilePath.GetSystemPath(), std::ios::in | std::ios::binary);
   const bool    opened = f.is_open();

   if (opened)
   {
      Read(f);
   }

   return opened;
}

//! Read a file from an input stream.
//! End-of-line characters are normalized to a '\n' upon read.
//! Binary input is expected.
void UtTextDocument::Read(std::istream& aInput)
{
   Clear();
   aInput.seekg(0, std::ios::end);
   int fileSize = (int)aInput.tellg();
   if (fileSize > 0)
   {
      aInput.seekg(0, std::ios::beg);
      mText.Resize(fileSize);
      aInput.read(mText.GetPointer(0), fileSize);
      mText.Insert(fileSize, 0); // null terminator
      // Normalize any sort of end-line designations to '\n'
      // '\r\n' -> '\n' (Windows / DOS)
      // '\n\r' -> '\n' (Rare)
      // Note: Mac OS up to version 9 uses '\r' ONLY.  This code will not work for this case.
      for (int i = 0; i < fileSize; ++i)
      {
         if (mText[i] == '\r')
         {
            mText.Erase(i);
            --i;
            --fileSize;
         }
      }
      // Remove gap from gap buffer, makes debugging easier for read-only docs
      mText.GetPointer();
   }
   else
   {
      mText.Insert(0, 0); // null terminator
   }

   // Update line info
   InsertLineData(0, mText.Size());
}

//! Writes the text file to an output stream.
//! Uses '\r\n' on windows platforms.
//! Binary ofstream is expected.
void UtTextDocument::WriteFile(std::ofstream& aOutput)
{
   for (iterator i = Begin(); i != End(); ++i)
   {
      char c = *i;
      if (c == 0)
      {
         break;
      }
#ifdef _WIN32
      if (c == '\n')
      {
         aOutput.put('\r');
      }
#endif
      aOutput.put(c);
   }
}

//! Return the line number (starting with 0) containing the specified character offset
size_t UtTextDocument::PositionToLineNumber(size_t aPosition)
{
   if (mLines.Empty())
   {
      return ut::npos;
   }

   if (aPosition == 0)
   {
      return 0U;
   }

   LineData midLine  = mLines[mLastUpdateLine];
   size_t   line1Pos = midLine.mPosition;
   if (aPosition < line1Pos)
   {
      LineBuffer::iterator beginIter = mLines.begin();
      LineBuffer::iterator endIter   = mLines.begin() + mLastUpdateLine;
      LineBuffer::iterator iter      = std::upper_bound(beginIter, endIter, aPosition, LineCompare());
      if (iter != beginIter)
      {
         --iter;
      }
      return ut::safe_cast<size_t, ptrdiff_t>(iter - beginIter);
   }
   if (mLines.Size() > mLastUpdateLine + 1)
   {
      size_t line2Pos = mLines[mLastUpdateLine + 1].mPosition + mUpdateChars;
      if (aPosition < line2Pos)
      {
         return mLastUpdateLine;
      }
      size_t               pseudoPos = aPosition - mUpdateChars;
      LineBuffer::iterator beginIter = mLines.begin() + (mLastUpdateLine + 1);
      LineBuffer::iterator endIter   = mLines.end();
      LineBuffer::iterator iter      = std::upper_bound(beginIter, endIter, pseudoPos, LineCompare());
      if (iter != beginIter)
      {
         --iter;
      }
      return ut::safe_cast<size_t, ptrdiff_t>(iter - mLines.begin());
   }
   return mLastUpdateLine;
}

bool UtTextDocument::PositionToLineColumn(size_t aPosition, size_t& aLine, size_t& aColumn)
{
   aLine = PositionToLineNumber(aPosition);
   if (aLine != ut::npos)
   {
      size_t lineStart = GetLinePosition(aLine);
      aColumn          = aPosition - lineStart;
      return true;
   }
   else
   {
      aColumn = 0;
      return false;
   }
}

//! Remove the characters in the range [aPosition, aPosition + aCount)
void UtTextDocument::Erase(size_t aPosition, size_t aCount /*= 1*/)
{
   size_t line     = PositionToLineNumber(aPosition);
   size_t lastLine = PositionToLineNumber(aPosition + aCount);
   MoveLineBreak(line);
   mUpdateChars -= aCount;
   mText.Erase(aPosition, aCount);
   size_t lineEnd = line + 1;
   mLines.Erase(lineEnd, lastLine - line);

   TextDocumentChanged(this, aPosition, -1 * static_cast<int>(aCount));
}

//! Deletes all text.  Memory is retained for efficiency.
void UtTextDocument::Clear()
{
   mText.Clear();
   mLines.Resize(1); // keep first line
   mLines[0].mPosition = 0;
   mLastUpdateLine     = 0;
   mUpdateChars        = 0;
}

//! Deletes all text and frees all memory.
void UtTextDocument::FreeMemory()
{
   Clear();
   mText.FreeMemory();
   mLines.FreeMemory();
}

//! Returns a string containing the text of the specified line.
std::string UtTextDocument::GetLineString(size_t aLine)
{
   TextRange range = GetLineRange(aLine);
   if (range.second > range.first)
   {
      return std::string(range.first, range.second);
   }
   return std::string();
}

//! Returns an iterator to the next location a string is found in the document
//! Returns End() if the text is not found.
UtTextDocument::iterator UtTextDocument::Search(const std::string& aText, iterator aStart)
{
   return std::search(aStart, End(), aText.begin(), aText.end());
}

void UtTextDocument::ValidateLines()
{
   assert(GetLinePosition(0) == 0);
   size_t line     = 0;
   size_t textSize = mText.Size();
   for (size_t i = 0; i < textSize; ++i)
   {
      if (mText[i] == '\n')
      {
         ++line;
         assert(GetLinePosition(line) == i + 1);
      }
   }
}

void UtTextDocumentAutoUpdateRange::TextDocumentChangedCB(const UtTextDocument* aTextDocument, size_t aPosition, int aCount)
{
   if (aTextDocument == mSource)
   {
      if (aPosition <= GetBegin())
      {
         SetBegin(GetBegin() + aCount);
      }

      if (aPosition < GetEnd())
      {
         SetEnd(GetEnd() + aCount);
      }
   }
}
