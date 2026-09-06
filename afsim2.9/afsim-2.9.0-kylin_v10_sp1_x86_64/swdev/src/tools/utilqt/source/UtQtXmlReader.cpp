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
#include "UtQtXmlReader.hpp"

#include <cassert>

#include <QFile>
#include <QXmlStreamReader>

class UtQtXmlReader_imp : public QObject
{
public:
   QXmlStreamReader mReader;
   std::string      mCurrentName;
};

UtQtXmlReader::UtQtXmlReader()
{
   impl = new UtQtXmlReader_imp;
}

UtQtXmlReader::~UtQtXmlReader()
{
   delete impl;
}

UtQtXmlReader::TokenType Convert(QXmlStreamReader::TokenType aType)
{
   switch (aType)
   {
   case QXmlStreamReader::NoToken:
      return UtQtXmlReader::cTT_NONE;
   case QXmlStreamReader::Invalid:
      return UtQtXmlReader::cTT_INVALID;
   case QXmlStreamReader::StartDocument:
      return UtQtXmlReader::cTT_START_DOCUMENT;
   case QXmlStreamReader::EndDocument:
      return UtQtXmlReader::cTT_END_DOCUMENT;
   case QXmlStreamReader::StartElement:
      return UtQtXmlReader::cTT_START_ELEMENT;
   case QXmlStreamReader::EndElement:
      return UtQtXmlReader::cTT_END_ELEMENT;
   case QXmlStreamReader::Characters:
      return UtQtXmlReader::cTT_CHARACTERS;
   case QXmlStreamReader::Comment:
      return UtQtXmlReader::cTT_COMMENT;
   case QXmlStreamReader::DTD:
      return UtQtXmlReader::cTT_DTD;
   default:
      return UtQtXmlReader::cTT_OTHER;
   }
}
UtQtXmlReader::TokenType UtQtXmlReader::NextToken()
{
   impl->mReader.readNext();
   return GetTokenType();
}

UtQtXmlReader::TokenType UtQtXmlReader::GetTokenType()
{
   return Convert(impl->mReader.tokenType());
}

UtQtXml::UtQtXmlAttributeList UtQtXmlReader::GetAttributes()
{
   UtQtXml::UtQtXmlAttributeList attrList;
   QXmlStreamAttributes          attributes = impl->mReader.attributes();
   for (int i = 0; i < attributes.size(); ++i)
   {
      QXmlStreamAttribute& attr = attributes[i];
      attrList.push_back(
         UtQtXml::UtQtXmlAttribute(attr.name().toString().toStdString(), attr.value().toString().toStdString()));
   }
   return attrList;
}

std::string UtQtXmlReader::GetText()
{
   return impl->mReader.text().toString().toStdString();
}

UtQtXmlReader::TokenType UtQtXmlReader::NextElementToken()
{
   TokenType t = NextToken();
   while (1)
   {
      switch (t)
      {
      case cTT_START_ELEMENT:
      case cTT_END_ELEMENT:
         impl->mCurrentName = GetName();
         return t;
      case cTT_INVALID:
         return t;
      default:
         break;
      }
      t = NextToken();
   }
   // unreachable
}
bool UtQtXmlReader::Open(const std::string& aFileName)
{
   QFile* infile = new QFile(aFileName.c_str());
   if (infile->open(QFile::ReadOnly))
   {
      impl->mReader.clear();
      impl->mReader.setDevice(infile);
      infile->setParent(impl);
      return true;
   }
   delete infile;
   return false;
}
void UtQtXmlReader::SetInput(const std::string& aInputText)
{
   impl->mReader.clear();
   impl->mReader.addData(QString::fromStdString(aInputText));
}

std::string UtQtXmlReader::GetName()
{
   return impl->mReader.name().toString().toStdString();
}
void UtQtXmlReader::SkipToEndElement()
{
   int       depth = 1;
   TokenType t     = NextToken();
   while (1)
   {
      if (t == cTT_END_ELEMENT)
      {
         --depth;
      }
      if (depth == 0)
         break;
      if (t == cTT_START_ELEMENT)
         ++depth;
      if (t == cTT_END_DOCUMENT || t == cTT_INVALID)
         break;
      t = NextToken();
   }
   impl->mCurrentName = GetName();
}

bool UtQtXmlReader::HasAttribute(const std::string& aAttributeName)
{
   return impl->mReader.attributes().hasAttribute(aAttributeName.c_str());
}

std::string UtQtXmlReader::Attribute(const std::string& aAttributeName)
{
   QStringRef val = impl->mReader.attributes().value(QString::fromStdString(aAttributeName));
   if (!val.isNull())
      return val.toString().toStdString();
   return std::string();
}


UtQtXmlElementReader::~UtQtXmlElementReader()
{
   if (!mAtEnd)
   {
      mReaderPtr->SkipToEndElement();
   }
}

std::string UtQtXmlElementReader::Attribute(const std::string& aAttributeName)
{
   assert(!mFirstChildRead);
   return mReaderPtr->Attribute(aAttributeName);
}

bool UtQtXmlElementReader::HasAttribute(const std::string& aAttributeName)
{
   assert(!mFirstChildRead);
   return mReaderPtr->HasAttribute(aAttributeName);
}

UtQtXml::UtQtXmlAttributeList UtQtXmlElementReader::GetAttributes()
{
   assert(!mFirstChildRead);
   return mReaderPtr->GetAttributes();
}

UtQtXmlElementReader::UtQtXmlElementReader(UtQtXmlReader& aReader)
{
   assert(aReader.GetTokenType() == UtQtXmlReader::cTT_START_ELEMENT);
   mReaderPtr      = &aReader;
   mAtEnd          = false;
   mFirstChildRead = false;
   mName           = mReaderPtr->GetName();
}

UtQtXmlElementReader::UtQtXmlElementReader(UtQtXmlElementReader& aParent)
{
   mFirstChildRead = false;
   mReaderPtr      = aParent.mReaderPtr;
   if (aParent.mAtEnd)
   {
      mAtEnd = true;
   }
   else
   {
      aParent.mFirstChildRead = true;
      mAtEnd                  = false;
      bool hasNext            = mReaderPtr->NextElementToken() == UtQtXmlReader::cTT_START_ELEMENT;
      if (!hasNext)
      {
         aParent.mAtEnd = mAtEnd = true;
         assert(mReaderPtr->GetName() == aParent.mName);
      }
      else
      {
         mName = mReaderPtr->GetName();
      }
   }
}

// Inform the XML Element reader that the end has already been consumed.
// useful if another method used the UtQtXmlReader directly to read the element
void UtQtXmlElementReader::SetAtEnd()
{
   mAtEnd = true;
}
