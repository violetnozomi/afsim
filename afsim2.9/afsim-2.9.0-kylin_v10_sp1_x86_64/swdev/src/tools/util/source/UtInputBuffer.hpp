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

#ifndef UTINPUTBUFFER_HPP
#define UTINPUTBUFFER_HPP

#include "ut_export.h"

#include <string>

/**
   Interface to a buffer of input data.  Used by UtInput to read data from files or strings.
*/
class UT_EXPORT UtInputBuffer
{
public:
   friend class UtInput;

   virtual ~UtInputBuffer() = default;

   virtual std::string GetFileName() const { return std::string(); }

   virtual UtInputBuffer* Clone() const = 0;

   virtual void SeekTo(size_t aOffset) = 0;

   virtual size_t GetOffset() = 0;

   //! Read a single character from the input buffer
   //! Returns 'false' if no characters remain
   virtual bool Get(char& aCh) = 0;

   virtual bool UnGet() = 0;

   virtual char Peek() = 0;

   /** Get the source name of the stream (filename, string, etc.).
   @return A string representing the source name of the stream.
   */
   virtual std::string RealGetSource() = 0;

   /** Get the current location within the input stream for reporting errors.

   @return A string representing the current position in the file.
   @note This is only considered valid during exception processing.
   */
   virtual std::string RealGetLocation() = 0;

   //! Returns by reference the current line and column number
   virtual void GetLineColumn(size_t& aLine, size_t& aColumn) = 0;

   void Ignore(int aCharCount, int aDelimiter /*= -1*/);

protected:
   bool RealReadLine(std::string& aValue, bool aIgnoreCurrentLine);

   virtual bool RealReadString(std::string& aValue);
   bool         RealReadStringQuoted(std::string& aValue);
   void         SkipWhiteSpace();
   bool         SkipComments(char c);

private:
   void SearchEndOfLine();

   bool mEndOfLine;
};
#endif
