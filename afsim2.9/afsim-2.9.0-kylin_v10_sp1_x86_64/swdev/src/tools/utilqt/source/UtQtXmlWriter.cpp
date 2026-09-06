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
#include "UtQtXmlWriter.hpp"

#include <cassert>

#include <QFile>
#include <QXmlStreamWriter>

#include "UtQtXmlReader.hpp"

class UtQtXmlWriter_imp : public QObject
{
public:
   UtQtXmlWriter_imp()
      : mOutput()
      , w(&mOutput)
   {
      w.setAutoFormatting(true);
      w.setAutoFormattingIndent(1);
   }
   QByteArray               mOutput;
   QXmlStreamWriter         w;
   std::vector<std::string> mElementStack;
};

UtQtXmlWriter::UtQtXmlWriter()
{
   impl = new UtQtXmlWriter_imp;
}

UtQtXmlWriter::~UtQtXmlWriter()
{
   delete impl;
}

bool UtQtXmlWriter::Open(const std::string& aFileName)
{
   QFile* filePtr = new QFile(aFileName.c_str());
   if (filePtr->open(QFile::WriteOnly | QFile::Truncate))
   {
      impl->w.setDevice(filePtr);
      filePtr->setParent(impl);
      return true;
   }
   delete filePtr;
   return false;
}

std::string UtQtXmlWriter::ToString()
{
   return std::string(impl->mOutput.begin(), impl->mOutput.end());
}

void UtQtXmlWriter::WriteAttributes(const UtQtXml::UtQtXmlAttributeList& aAttributes)
{
   for (size_t i = 0; i < aAttributes.size(); ++i)
   {
      WriteAttribute(aAttributes[i]);
   }
}

void UtQtXmlWriter::WriteAttribute(const UtQtXml::UtQtXmlAttribute& aAttribute)
{
   WriteAttribute(aAttribute.mName, aAttribute.mValue);
}

void UtQtXmlWriter::WriteAttribute(const std::string& aName, const std::string& aValue)
{
   // QByteArray is implicitly convertible to QString, so this is functionally equivalent to
   // QString::fromStdString. However, using QByteArray as an intermediate masks a string
   // termination vulnerability identified by static code analysis, with sink in QString's
   // use of 'strlen'. The source is std::string input from a file stream, which should be
   // null-terminated, so this is apparently a false positive, but this workaround might be
   // unnecessary in a future version of Qt.
   impl->w.writeAttribute(QByteArray::fromStdString(aName), QByteArray::fromStdString(aValue));
}

void UtQtXmlWriter::WriteStartElement(const std::string& aName)
{
   impl->mElementStack.push_back(aName);
   impl->w.writeStartElement(aName.c_str());
}

void UtQtXmlWriter::WriteEndElement()
{
   impl->mElementStack.pop_back();
   impl->w.writeEndElement();
}

// Same as WriteEndElement(), but checks that the element name is correct.
// If the element name differs, an assertion is thrown
void UtQtXmlWriter::WriteEndElement(const std::string& aName)
{
   assert(impl->mElementStack.back() == aName);
   WriteEndElement();
}

void UtQtXmlWriter::WriteStartDocument(const std::string& aVersion)
{
   impl->w.writeStartDocument(aVersion.c_str());
}

void UtQtXmlWriter::WriteEndDocument()
{
   impl->w.writeEndDocument();
}

void UtQtXmlWriter::WriteText(const std::string& aText)
{
   impl->w.writeCharacters(aText.c_str());
}

void UtQtXmlWriter::WriteElementFromReader(UtQtXmlReader& aReader)
{
   int depth = 1;
   // assume we are called with a start element already read
   WriteStartElement(aReader.GetName());
   WriteAttributes(aReader.GetAttributes());
   for (; depth > 0;)
   {
      UtQtXmlReader::TokenType tt = aReader.NextToken();
      switch (tt)
      {
      case UtQtXmlReader::cTT_NONE:
      case UtQtXmlReader::cTT_INVALID:
      case UtQtXmlReader::cTT_START_DOCUMENT:
      case UtQtXmlReader::cTT_END_DOCUMENT:
         break;
      case UtQtXmlReader::cTT_START_ELEMENT:
         ++depth;
         WriteStartElement(aReader.GetName());
         WriteAttributes(aReader.GetAttributes());
         break;
      case UtQtXmlReader::cTT_END_ELEMENT:
         --depth;
         WriteEndElement();
         break;
      case UtQtXmlReader::cTT_CHARACTERS:
         WriteText(aReader.GetText());
         break;
      case UtQtXmlReader::cTT_COMMENT:
         break;
      case UtQtXmlReader::cTT_DTD:
         break;
      case UtQtXmlReader::cTT_OTHER:
         break;
      }
   }
}
