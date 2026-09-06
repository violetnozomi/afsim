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
#ifndef UTQTXMLWRITER_HPP
#define UTQTXMLWRITER_HPP

#include "utilqt_export.h"

#include <string>

#include "UtQtXml.hpp"


class UtQtXmlReader;
class UtQtXmlWriter_imp;

class UTILQT_EXPORT UtQtXmlWriter
{
public:
   UtQtXmlWriter();
   ~UtQtXmlWriter();

   bool        Open(const std::string& aFileName);
   std::string ToString();

   void WriteAttributes(const UtQtXml::UtQtXmlAttributeList& aAttributes);
   void WriteAttribute(const UtQtXml::UtQtXmlAttribute& aAttribute);
   void WriteAttribute(const std::string& aName, const std::string& aValue);
   void WriteStartElement(const std::string& aName);
   void WriteEndElement();
   void WriteEndElement(const std::string& aName);

   void WriteStartDocument(const std::string& aVersion);
   void WriteEndDocument();

   void WriteText(const std::string& aText);
   //! Read the current element from aReader and write the element to this writer
   void WriteElementFromReader(UtQtXmlReader& aReader);

   UtQtXmlWriter_imp* impl;
};
#endif
