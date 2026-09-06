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

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "UtInputFile.hpp"

UtInputDocument::UtInputDocument()
   : UtInputBuffer()
   , mDocument(new UtTextDocument)
   , mDocumentReferenceCount(new UtReferenceCount(1))
   , mEndOfLine(false)
{
}

//! Create a copy of an input document which shares the source
UtInputDocument::UtInputDocument(const UtInputDocument& aRhs)
   : mDocument(aRhs.mDocument)
   , mDocumentReferenceCount(aRhs.mDocumentReferenceCount)
   , mPos(aRhs.mPos)
   , mEndOfLine(aRhs.mEndOfLine)
{
   mDocumentReferenceCount->AddStrongRef();
}

// virtual
UtInputDocument::~UtInputDocument()
{
   if (mDocumentReferenceCount->RemoveStrongRef())
   {
      delete mDocument;
   }
}

UtInputBuffer* UtInputDocument::Clone() const
{
   return new UtInputDocument(*this);
}

// virtual
std::string UtInputDocument::GetFileName() const
{
   return mDocument->GetFilePath().GetSystemPath();
}

// protected
// virtual
std::string UtInputDocument::RealGetSource()
{
   size_t line;
   size_t col;
   GetLineColumn(line, col);

   std::ostringstream oss;
   oss << GetFileName() << ", line " << line + 1;
   return oss.str();
}

// protected
// virtual
std::string UtInputDocument::RealGetLocation()
{
   std::ostringstream oss;

   oss << "'" << GetFileName();
   if (Eof())
   {
      oss << "' at end-of-file";
   }
   else
   {
      size_t line;
      size_t col;
      GetLineColumn(line, col);
      oss << "', line " << line + 1 << ", near column " << col + 1;
   }
   return oss.str();
}

//! Extracts a characters from the buffer and stores it in 'aCh'.
//!
//! @param aCh The character to fill.
//! @return Returns true if a character was successfully extracted.

// private
bool UtInputDocument::Get(char& aCh)
{
   if (mPos != mDocument->End())
   {
      aCh = *mPos;
      ++mPos;
      // Last character is '\0'
      return mPos != mDocument->End();
   }
   return false;
}

//! Decrements the buffer's get pointer by one.
//!
//! @return Returns true if the unget was successful.

// private
bool UtInputDocument::UnGet()
{
   bool success = false;
   if (mPos != mDocument->Begin())
   {
      --mPos;
      success = true;
   }
   return success;
}

//! Returns the next character in the buffer without extracting it.
//!
//! @return Returns the next character. If there is no character, '0' is returned.

// private
char UtInputDocument::Peek()
{
   char ch = '0';
   if (mPos != mDocument->End())
   {
      ch = *mPos;
   }
   return ch;
}

void UtInputDocument::SeekTo(size_t aOffset)
{
   mPos = std::min(mDocument->End(), mDocument->Begin() + aOffset);
}

//! Indicates that the last character has been extracted.
//!
//! @return Returns true if the last character has been extracted.

// private
bool UtInputDocument::Eof() const
{
   return (mPos == mDocument->End());
}

//! Returns the zero-relative line & column of the document read location
void UtInputDocument::GetLineColumn(size_t& aLine, size_t& aColumn)
{
   aLine   = mDocument->PositionToLineNumber(mPos.GetIndex());
   aColumn = mPos.GetIndex() - mDocument->GetLinePosition(aLine);
}

UtTextDocument::iterator UtInputDocument::GetDocumentPosition() const
{
   UtTextDocument::iterator curPos = mPos;
   return curPos;
}

std::string UtInputDocument::GetLocationAtOffset(size_t aOffset)
{
   std::ostringstream oss;

   oss << "in '" << GetFileName();
   size_t line = mDocument->PositionToLineNumber(aOffset);
   size_t col  = aOffset - mDocument->GetLinePosition(line);

   oss << "', line " << line + 1 << ", near column " << col + 1;
   return oss.str();
}

UtInputDocument::OpenError::OpenError(const std::string& aFileName)
   : UtException("***** ERROR: Cannot open file: " + aFileName)
{
}
