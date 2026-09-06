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

#ifndef UTINPUTDOCUMENT_HPP
#define UTINPUTDOCUMENT_HPP

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBuffer.hpp"
#include "UtReferenceCount.hpp"
#include "UtTextDocument.hpp"

/**
   Implements the UtInputBuffer interface using a UtTextDocument.
   UtInputFile and UtInputString inherit this for a trivial implementation.
*/
class UT_EXPORT UtInputDocument : public UtInputBuffer
{
public:
   /** An exception that is thrown if the input file cannot be opened. */
   class UT_EXPORT OpenError : public UtException
   {
   public:
      OpenError(const std::string& aFileName);
   };

   UtInputDocument();
   UtInputDocument(const UtInputDocument& aRhs);

   UtInputBuffer* Clone() const override;

   ~UtInputDocument() override;

   std::string GetFileName() const override;

   bool Get(char& aCh) override;
   bool UnGet() override;
   char Peek() override;

   const UtTextDocument&    GetDocument() { return *mDocument; }
   UtTextDocument::iterator GetDocumentPosition() const;

   void   SeekTo(size_t aOffset) override;
   size_t GetOffset() override { return GetDocumentPosition().GetIndex(); }

   std::string GetLocationAtOffset(size_t aOffset);

   void        GetLineColumn(size_t& aLine, size_t& aColumn) override;
   std::string RealGetSource() override;
   std::string RealGetLocation() override;

protected:
   bool Eof() const;

   mutable UtTextDocument* mDocument;
   UtReferenceCount*       mDocumentReferenceCount;

   UtTextDocument::iterator mPos;

   bool mEndOfLine;
};

#endif
