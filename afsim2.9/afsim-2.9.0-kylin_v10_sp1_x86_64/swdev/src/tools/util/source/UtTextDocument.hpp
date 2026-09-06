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
#ifndef UTTEXTDOCUMENT_HPP
#define UTTEXTDOCUMENT_HPP

#include "ut_export.h"

#include <algorithm>
#include <fstream>
#include <string>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtCast.hpp"
#include "UtGapBuffer.hpp"
#include "UtPath.hpp"
#include "UtStringRef.hpp"


//! Defines a range over a text document, defined by [mBegin, mEnd].  (NOT [mBegin, mEnd) )
class UtTextRange;

//! Maintains data about a line
struct UtTextDocumentLineData
{
   //! The offset at the start of the line from the beginning of the text buffer
   //! Do not use this directly, as it may not be up to date.
   size_t mPosition;
};

using UtTextDocumentBuffer     = UtGapBuffer<char>;
using UtTextDocumentLineBuffer = UtGapBuffer<UtTextDocumentLineData>;
using UtTextDocumentIterator   = UtTextDocumentBuffer::iterator;

class UT_EXPORT UtTextRange
{
public:
   UtTextRange() = default;
   UtTextRange(size_t aStart, size_t aEnd) { SetRange(aStart, aEnd); }
   UtTextRange(const UtTextDocumentIterator& aStart, const UtTextDocumentIterator& aEnd)
   {
      SetRange(aStart.GetIndex(), aEnd.GetIndex());
   }

   struct ByEnd
   {
      bool operator()(const UtTextRange& lhs, const UtTextRange& rhs) const { return lhs.mEnd < rhs.mEnd; }
   };

   //! Create a text range given an offset and size
   static UtTextRange Offset(size_t aOffset, size_t aSize = 0) { return UtTextRange(aOffset, aOffset + aSize - 1); }

   //! Extends the text range to contain aPos
   void Extend(size_t aPos)
   {
      if (aPos < mBegin)
      {
         mBegin = aPos;
      }
      else if (mEnd < aPos)
      {
         mEnd = aPos;
      }
   }

   //! Extends the text range to include aRange
   void Extend(const UtTextRange& aRange)
   {
      mBegin = std::min(mBegin, aRange.mBegin);
      mEnd   = std::max(mEnd, aRange.mEnd);
   }

   //! Moves the text range by the specified character offset
   void Move(int aOffset)
   {
      mBegin += aOffset;
      mEnd += aOffset;
   }

   //! Returns the number of characters contained in the range
   size_t Length() const { return mEnd - mBegin + 1; }

   //! Returns true if this range intersects aRange
   bool Intersects(const UtTextRange& aRange) const
   {
      if ((aRange.mEnd == ut::npos) || (mEnd == ut::npos))
      {
         return false;
      }
      return !(aRange.mEnd < mBegin || aRange.mBegin > mEnd);
   }

   //! Returns true if aPosition is contained in this range
   bool Contains(size_t aPosition) const
   {
      if (mEnd == ut::npos)
      {
         return false;
      }
      return mBegin <= aPosition && mEnd >= aPosition;
   }

   bool operator==(const UtTextRange& aRhs) const { return mBegin == aRhs.mBegin && mEnd == aRhs.mEnd; }

   const size_t GetBegin() const { return mBegin; }
   const size_t GetEnd() const { return mEnd; }
   void         SetRange(const size_t& aBegin, const size_t& aEnd)
   {
      mBegin = aBegin;
      mEnd   = aBegin <= aEnd ? aEnd : aBegin;
   }

   void SetBegin(const size_t& aBegin)
   {
      mBegin = aBegin;
      if (mBegin > mEnd)
      {
         mEnd = mBegin;
      }
   }

   void SetEnd(const size_t& aEnd)
   {
      mEnd = aEnd;
      if (mBegin > mEnd)
      {
         mBegin = mEnd;
      }
   }

private:
   size_t mBegin{0};
   size_t mEnd{ut::npos};
};

//! Maintains a text document in an editable form.
//! This class is designed to allow many small modifications with an
//! amortized low overhead
class UT_EXPORT UtTextDocument
{
public:
   // first int is the character position in the text document that changed.
   // second int is the number of characters add or removed (removed is negative)
   static UtCallbackListN<void(const UtTextDocument*, size_t, int)> TextDocumentChanged;

   using TextBuffer = UtTextDocumentBuffer;
   using LineData   = UtTextDocumentLineData;
   using LineBuffer = UtTextDocumentLineBuffer;
   using iterator   = UtTextDocumentIterator;
   using TextRange  = std::pair<UtTextDocumentIterator, UtTextDocumentIterator>;

   struct LineCompare
   {
      bool operator()(const LineData& aLhs, size_t aRhs) const { return aLhs.mPosition < aRhs; }
      bool operator()(size_t aLhs, const LineData& aRhs) const { return aLhs < aRhs.mPosition; }
      bool operator()(const LineData& aLhs, const LineData& aRhs) const { return aLhs.mPosition < aRhs.mPosition; }
   };

   UtTextDocument();
   UtTextDocument(std::istream& aInput);
   UtTextDocument(const UtPath& aPath);
   UtTextDocument(const UtTextDocument& aSrc) = default;

   bool ReadFile(const UtPath& aPath);
   void Read(std::istream& aInput);
   void WriteFile(std::ofstream& aOutput);
   //! Returns an iterator to the beginning of the text
   iterator Begin() { return mText.begin(); }
   //! Returns an iterator to the end of the text
   iterator End() { return mText.end(); }

   //! Returns the text buffer.  Only expert users should modify this directly.
   UtTextDocumentBuffer&       GetText() { return mText; }
   const UtTextDocumentBuffer& GetText() const { return mText; }
   //! Returns the lines buffer.  Only expert users should modify this directly.
   LineBuffer&       GetLines() { return mLines; }
   const LineBuffer& GetLines() const { return mLines; }

   //! Returns iterators marking the begin and end of the specified line.
   TextRange GetLineRange(size_t aLine)
   {
      size_t pos = GetLinePosition(aLine);
      size_t len = GetLineLength(aLine);
      return std::pair<iterator, iterator>(iterator(mText, pos), iterator(mText, pos + len));
   }

   std::string GetLineString(size_t aLine);

   //! Returns the offset of the line from the start of the text document.
   size_t GetLinePosition(size_t aLine) const
   {
      size_t pos = mLines[aLine].mPosition;
      if (mLastUpdateLine < aLine)
      {
         return pos + mUpdateChars;
      }
      return pos;
   }

   //! Returns the length of the line (including the newline character)
   size_t GetLineLength(size_t aLine) const
   {
      size_t pos = GetLinePosition(aLine);
      if (mLines.Size() > aLine + 1)
      {
         return GetLinePosition(aLine + 1) - pos;
      }
      // last line
      return mText.Size() - pos - 1;
   }

   //! Finds the line which contains the given character offset in O(log n) time.
   size_t PositionToLineNumber(size_t aPosition);
   bool   PositionToLineColumn(size_t aPosition, size_t& aLine, size_t& aColumn);

   //! Inserts a segment of text before the character offset aPosition.
   void Insert(size_t aPosition, const char* aText, size_t aCount)
   {
      mText.Insert(aPosition, aText, aCount);
      InsertLineData(aPosition, aCount);
   }

   //! Inserts a single character before the character offset aPosition.
   void Insert(size_t aPosition, char aChar)
   {
      mText.Insert(aPosition, aChar);
      InsertLineData(aPosition, 1);
   }

   void Insert(size_t aPosition, const std::string& aText) { Insert(aPosition, aText.c_str(), (int)aText.size()); }

   //! Erases a block of text starting at the character offset aPosition
   void Erase(size_t aPosition, size_t aCount = 1);

   //! Returns the number of lines in the document.
   //! An empty document has 1 (empty) line
   size_t GetLineCount() { return mLines.Size(); }

   //! Returns a pointer to document text at the given offset location.
   //! The text of the document is guaranteed to be contiguous up to either the end
   //! of the document, or aPosition + aLengthValid.
   char* GetPointer(size_t aPosition = 0, size_t aLengthValid = ut::npos)
   {
      return mText.GetPointer(aPosition, aLengthValid);
   }
   const char* GetPointer(size_t aPosition = 0, size_t aLengthValid = ut::npos) const
   {
      // This may actually mutate mText, so mText must be mutable
      return mText.GetPointer(aPosition, aLengthValid);
   }

   void Clear();

   void FreeMemory();

   char  operator[](size_t aOffset) const { return mText[aOffset]; }
   char& operator[](size_t aOffset) { return mText[aOffset]; }

   const UtPath& GetFilePath() const { return mFilePath; }
   size_t        Size() const { return mText.Size(); }

   iterator Search(const std::string& aText, iterator aStart);

   void Swap(UtTextDocument& aRhs)
   {
      std::swap(mText, aRhs.mText);
      std::swap(mLines, aRhs.mLines);
      std::swap(mLastUpdateLine, aRhs.mLastUpdateLine);
      std::swap(mUpdateChars, aRhs.mUpdateChars);
      std::swap(mFilePath, aRhs.mFilePath);
   }

   bool operator==(const UtTextDocument& aRhs)
   {
      const std::string thisStr(GetPointer());
      const std::string aRhsStr(aRhs.GetPointer());
      return (mFilePath == aRhs.mFilePath) && (thisStr == aRhsStr);
   }

   bool operator!=(const UtTextDocument& aRhs) { return !(*this == aRhs); }

   // Asserts that line endings are accurately maintained.
   void ValidateLines();

protected:
   void InitLines()
   {
      LineData firstLine;
      firstLine.mPosition = 0;
      mLines.Insert(0, firstLine);
   }

   //! Inserts a segment of text before the character offset aPosition.
   void InsertLineData(size_t aPosition, size_t aCount)
   {
      size_t currentLine = PositionToLineNumber(aPosition);
      size_t lineColumn  = aPosition - GetLinePosition(currentLine);
      size_t charOffset  = 0;
      for (size_t i = 0; i < aCount; ++i)
      {
         char c = mText[i + aPosition];
         if (c == '\n')
         {
            size_t insertChars = i - charOffset + 1;
            charOffset         = i + 1;
            InsertTextNoLine(currentLine, insertChars);
            SplitLine(currentLine, lineColumn + insertChars);
            ++currentLine;
            lineColumn = 0;
            if (i < aCount - 1)
            {
               //++i;
            }
         }
      }
      if (aCount > charOffset)
      {
         InsertTextNoLine(currentLine, aCount - charOffset);
      }

      TextDocumentChanged(this, aPosition, static_cast<int>(aCount));
   }

   void InsertTextNoLine(size_t aPosition, const char* /*aText*/, size_t aCount)
   {
      size_t currentLine = PositionToLineNumber(aPosition);
      if (currentLine + 1 < mLines.Size())
      {
         MoveLineBreak(currentLine);
         mUpdateChars += aCount;
      }
   }

   void InsertTextNoLine(size_t aLine, size_t aCount)
   {
      MoveLineBreak(aLine);
      mUpdateChars += aCount;
   }

   void SplitLine(size_t aLineNumber, size_t aColumn)
   {
      if (mLines.Size() - 1 == aLineNumber)
      {
         // last line
         MoveLineBreak(aLineNumber);
         LineData l;
         l.mPosition = mLines[aLineNumber].mPosition + aColumn - mUpdateChars;
         mLines.Insert(aLineNumber + 1, l);
      }
      else
      {
         MoveLineBreak(aLineNumber + 1);
         LineData l;
         l.mPosition = mLines[aLineNumber].mPosition + aColumn;
         mLines.Insert(aLineNumber + 1, l);
         ++mLastUpdateLine;
      }
   }

   void InsertLine(size_t aLineNumber, size_t aLinePos)
   {
      LineData l;
      l.mPosition = aLinePos;
      if (mLastUpdateLine < aLineNumber)
      {
         l.mPosition += mUpdateChars;
      }
      mLines.Insert(aLinePos, l);
   }

   void MoveLineBreak(size_t aLineNumber)
   {
      while (mLastUpdateLine < aLineNumber)
      {
         mLines[++mLastUpdateLine].mPosition += mUpdateChars;
      }
      while (mLastUpdateLine > aLineNumber)
      {
         mLines[mLastUpdateLine].mPosition -= mUpdateChars;
         --mLastUpdateLine;
      }
   }
   mutable UtTextDocumentBuffer mText;
   UtTextDocumentLineBuffer     mLines;

   // The line information would need to be updated after every update unless some extra care
   // was taken.  mLastUpdateTime keeps track of the last line which has a correct position.
   // All other lines need their position computed by adding mUpdateChars.
   size_t mLastUpdateLine;
   size_t mUpdateChars;

   UtPath mFilePath;
};

namespace std
{
template<>
inline void swap(UtTextDocument& aLhs, UtTextDocument& aRhs)
{
   aLhs.Swap(aRhs);
}
} // namespace std

//! A position in a text document
class UT_EXPORT UtTextDocumentLocation
{
public:
   UtTextDocumentLocation()
      : mSource()
      , mPosition(0)
   {
   }
   UtTextDocumentLocation(UtTextDocument* aSource, size_t aPosition)
      : mSource(aSource)
      , mPosition(aPosition)
   {
   }
   UtTextDocument* mSource;
   size_t          mPosition;
   bool            operator<(const UtTextDocumentLocation& aRhs) const
   {
      if (mSource < aRhs.mSource)
      {
         return true;
      }
      if (mSource > aRhs.mSource)
      {
         return false;
      }
      return mPosition < aRhs.mPosition;
   }
   bool operator==(const UtTextDocumentLocation& aRhs) const
   {
      return mSource == aRhs.mSource && mPosition == aRhs.mPosition;
   }
};

//! Combines a range and a source document.
class UT_EXPORT UtTextDocumentRange : public UtTextRange
{
public:
   UtTextDocumentRange()
      : mSource()
   {
   }
   UtTextDocumentRange(UtTextDocument* aSource, size_t aBegin, size_t aEnd)
      : UtTextRange(aBegin, aEnd)
      , mSource(aSource)
   {
   }

   UtTextDocumentRange(UtTextDocument* aSource, const UtTextRange& aRange)
      : UtTextRange(aRange)
      , mSource(aSource)
   {
   }

   UtTextDocumentRange(const UtTextDocumentLocation& aLocation, size_t aLength)
      : UtTextRange(aLocation.mPosition, aLocation.mPosition + aLength - 1)
      , mSource(aLocation.mSource)
   {
   }

   UtTextDocumentLocation GetPosition() const { return UtTextDocumentLocation(mSource, GetBegin()); }

   //! Return a pointer to the text range, valid from mBegin to mEnd.
   //! No null terminator should be expected.
   const char* Pointer() const { return mSource->GetPointer(GetBegin(), Length()); }

   //! Returns a string copy of the text range.
   std::string Text() const
   {
      if ((mSource == nullptr) || GetEnd() == ut::npos)
      {
         return std::string();
      }
      UtTextDocument::iterator iter = mSource->Begin() + GetBegin();
      return std::string(iter, iter + Length());
   }

   void Text(UtStringRef& aText)
   {
      if ((mSource == nullptr) || GetEnd() == ut::npos)
      {
         aText = UtStringRef();
      }
      else
      {
         std::string*             strPtr = new std::string;
         UtTextDocument::iterator iter   = mSource->Begin() + GetBegin();
         strPtr->assign(iter, iter + Length());
         aText = UtStringRef(strPtr);
      }
   }

   void Text(std::string& aText) const
   {
      if ((mSource == nullptr) || GetEnd() == ut::npos)
      {
         aText.clear();
      }
      else
      {
         UtTextDocument::iterator iter = mSource->Begin() + GetBegin();
         aText.assign(iter, iter + Length());
      }
   }

   //! Returns true if the range is valid.
   bool Valid() const { return mSource != nullptr; }

   //! Quickly compares a string to this text range for equivalence.
   bool operator==(const std::string& aWord) const
   {
      size_t size = aWord.length();
      if (size != Length())
      {
         return false;
      }
      if (size == 0)
      {
         return true;
      }
      const char* ptr     = Pointer();
      const char* wordPtr = aWord.c_str();
      return std::equal(wordPtr, wordPtr + size, ptr);
   }

   //! Quickly compares a null-terminated string to this text range for equivalence.
   bool operator==(const char* aWord) const
   {
      size_t size = Length();
      if (size == 0)
      {
         return aWord[0] == 0;
      }
      const char* ptr = Pointer();
      for (size_t i = 0; i < size; ++i)
      {
         char c1 = ptr[i];
         char c2 = aWord[i];
         if (c1 != c2)
         {
            return false;
         }
      }
      return aWord[size] == 0;
   }

   bool operator==(const UtTextDocumentRange& aRhs) const
   {
      return (mSource == aRhs.mSource) && UtTextRange::operator==(aRhs);
   }

   UtTextDocument* mSource;
};

//! Combines a range and a source document.
class UT_EXPORT UtTextDocumentAutoUpdateRange : public UtTextDocumentRange
{
public:
   UtTextDocumentAutoUpdateRange()
      : UtTextDocumentRange()
   {
      mCallbackPtr = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   UtTextDocumentAutoUpdateRange(UtTextDocument* aSource, int aBegin, int aEnd)
      : UtTextDocumentRange(aSource, aBegin, aEnd)
   {
      mCallbackPtr = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   UtTextDocumentAutoUpdateRange(UtTextDocument* aSource, const UtTextRange& aRange)
      : UtTextDocumentRange(aSource, aRange)
   {
      mCallbackPtr = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   UtTextDocumentAutoUpdateRange(const UtTextDocumentLocation& aLocation, int aLength)
      : UtTextDocumentRange(aLocation, aLength)
   {
      mCallbackPtr = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   UtTextDocumentAutoUpdateRange(const UtTextDocumentAutoUpdateRange& aSrc)
      : UtTextDocumentRange(aSrc)
   {
      mCallbackPtr = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   UtTextDocumentAutoUpdateRange(const UtTextDocumentRange& aRange)
      : UtTextDocumentRange(aRange)
   {
      mCallbackPtr = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   ~UtTextDocumentAutoUpdateRange() { delete mCallbackPtr; }

   UtTextDocumentRange GetRange() { return UtTextDocumentRange(mSource, GetBegin(), GetEnd()); }

   void operator=(const UtTextDocumentAutoUpdateRange& aRhs)
   {
      UtTextDocumentRange::operator=(aRhs);
      mCallbackPtr                 = new UtCallbackHolder();
      mCallbackPtr->Add(
         UtTextDocument::TextDocumentChanged.Connect(&UtTextDocumentAutoUpdateRange::TextDocumentChangedCB, this));
   }

   void TextDocumentChangedCB(const UtTextDocument* aTextDocument, size_t aPosition, int aCount);

private:
   UtCallbackHolder* mCallbackPtr;
};

#endif
