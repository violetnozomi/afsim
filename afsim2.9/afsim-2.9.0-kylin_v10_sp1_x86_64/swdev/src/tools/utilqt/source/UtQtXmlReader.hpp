// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef VUTQTXMLREADER_HPP
#define VUTQTXMLREADER_HPP

#include "utilqt_export.h"

#include <string>

#include "UtQtXml.hpp"

class UtQtXmlReader_imp;

class UTILQT_EXPORT UtQtXmlReader
{
public:
   enum TokenType
   {
      cTT_NONE,
      cTT_INVALID,
      cTT_START_DOCUMENT,
      cTT_END_DOCUMENT,
      cTT_START_ELEMENT,
      cTT_END_ELEMENT,
      cTT_CHARACTERS,
      cTT_COMMENT,
      cTT_DTD,
      cTT_OTHER
   };

   UtQtXmlReader();
   ~UtQtXmlReader();

   bool Open(const std::string& aFileName);
   void SetInput(const std::string& aInputText);

   // Reads a token from the XML stream
   TokenType NextToken();
   // Calls NextToken() until a begin/end element token or error occurs
   TokenType NextElementToken();
   void      SkipToEndElement();
   // the token type of the previously read token
   TokenType GetTokenType();
   // name of the element when cTT_START_ELEMENT or cTT_END_ELEMENT are returned
   std::string GetName();
   // set of attributes when cTT_START_ELEMENT is returned
   UtQtXml::UtQtXmlAttributeList GetAttributes();
   // returns the text in cTT_CHARACTERS, cTT_DTD or cTT_OTHER
   std::string GetText();

   bool        HasAttribute(const std::string& aAttributeName);
   std::string Attribute(const std::string& aAttributeName);

private:
   UtQtXmlReader_imp* impl;
};

// A Safer version of UtQtXmlReader.  This class assists you in reading elements
// to avoid accidental processing of too many or too few end element tokens.
//
// ex:
//   ...UtQtXmlReader reader;
//  // initialize from a reader
//  UtQtXmlElementReader parent(reader);
//  for (;;)
//{
//    UtQtXmlElementReader child(parent);   // reads next child if one exists
//    if (! child) break;
//    ...process child
//}

class UTILQT_EXPORT UtQtXmlElementReader
{
public:
   // Initialize an element reader from an XML reader pointing to a start element
   UtQtXmlElementReader(UtQtXmlReader& aReader);

   // Read the next child of aParent
   UtQtXmlElementReader(UtQtXmlElementReader& aParent);

   // Destructor reads until the end element token
   ~UtQtXmlElementReader();

   UtQtXmlReader& GetReader() { return *mReaderPtr; }

   bool operator!() const { return mAtEnd; }

   // the name of the parent element
   const std::string& GetName() const { return mName; }

   UtQtXml::UtQtXmlAttributeList GetAttributes();

   bool HasAttribute(const std::string& aAttributeName);

   std::string Attribute(const std::string& aAttributeName);
   void        SetAtEnd();

private:
   // no assignment operator
   UtQtXmlElementReader& operator=(const UtQtXmlElementReader& aRhs) = delete;

   // Attribute access is only allowed before reading the next token
   bool           mFirstChildRead;
   bool           mAtEnd;
   std::string    mName;
   UtQtXmlReader* mReaderPtr;
};

#endif
