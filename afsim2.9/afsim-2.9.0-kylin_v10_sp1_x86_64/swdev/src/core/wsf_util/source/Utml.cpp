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

#include "Utml.hpp"

#include <cctype>
#include <exception>
#include <iostream>
#include <ostream>
#include <sstream>

#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "UtTextDocument.hpp"

/*
label =>
   [a-z][a-zA-Z0-9_]*:
typename =>
   [A-Z][a-zA-Z0-9_]*
quote =>
   "([^"]|"")*"
word =>
   [^ \t\n"][^ \t\n]*         (does not end with ':')
unquoted =>
   <word>([ \t]*<word>)*
property =>
   <label> <object>
object =>
   <typename> '{' <property>* '}
 | <quote>
 | <unquoted>
*/

class UtmlParserP
{
public:
   char* p;
   char* end;

   bool atEnd() { return p >= end || at() == 0; }
   char at(int i = 0) const
   {
      if ((p + i) < end)
      {
         return p[i];
      }
      return 0;
   }
   char next(int adv = 1)
   {
      p += adv;
      return at(0);
   }
   char* save() { return p; }
   void  load(char* aP) { p = aP; }
   void  skipWhite()
   {
      for (;;)
      {
         while (at() && isspace(at()))
         {
            next();
         }
         // comments
         if (at() == '/' && at(1) == '/')
         {
            while (next() != '\n')
               ;
            continue;
         }
         break;
      }
   }
   void throwError(const std::string& aError = std::string()) { throw UtmlParseError(aError); }

   //   "([^"] | "")*"
   bool quote(std::string& aValue)
   {
      assert(!isspace(at())); // assumes skipWhite() before call
      if (at() == '"')
      {
         aValue.clear();
         for (;;)
         {
            next();
            switch (at())
            {
            case '"':
               if (at(1) == '"')
               {
                  next();
                  aValue += '"';
                  break;
               }
               next();
               return true;
            case 0:
               throw UtmlParseError("End of stream while looking for '\"'");
            default:
               aValue += at();
            }
         }
      }
      return false;
   }
   bool unquotedValue(std::string& aValue)
   {
      char* checkpoint = save();
      char* start      = checkpoint;
      if (word(aValue))
      {
         checkpoint = save();
         for (;;)
         {
            for (;;)
            {
               char c = at();
               if (c == ' ' || c == '\t')
               {
                  next();
                  continue;
               }
               break;
            }
            if (!isspace(at()) && word(aValue))
            {
               checkpoint = save();
               continue;
            }
            break;
         }
         load(checkpoint);
         aValue.assign(start, checkpoint);
         return true;
      }
      return false;
   }
   // Unquoted value [^ \t\n"][^ \t\n]*
   bool word(std::string& aValue)
   {
      char* beforeWord = save();
      if (wordP(aValue))
      {
         char* afterWord = save();
         skipWhite();
         // this is an object, not a word-value
         if (at() == '{')
         {
            aValue.clear();
            load(beforeWord);
            return false;
         }
         load(afterWord);
         return true;
      }
      return false;
   }
   bool wordP(std::string& aValue)
   {
      assert(!isspace(at())); // assumes skipWhite() before call
      switch (at())
      {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case '"':
      case '\0':
      case '{':
      case '}':
         return false;
      default:
         aValue.clear();
         for (;;)
         {
            aValue += at();
            switch (next())
            {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '}':
               if (aValue[aValue.size() - 1] == ':')
               {
                  return false;
               } // can't look like a label
               return true;
            case '/':
               if (at(1) == '/')
               {
                  if (aValue[aValue.size() - 1] == ':')
                  {
                     return false;
                  }            // can't look like a label
                  return true; // comment
               }
               break;
            case '\0':
               throwError("End of file while reading word");
            default:
               break;
            }
         }
      }
   }
   //[A-Z][a-zA-Z0-9_]*
   bool typeName(std::string& aValue)
   {
      assert(!isspace(at())); // assumes skipWhite() before call
      if (at() >= 'A' && at() <= 'Z')
      {
         char* rollback = save();
         aValue.clear();
         for (;;)
         {
            aValue += at();
            char c = next();
            if (isspace(c))
            {
               return true;
            }
            if ('a' <= c && 'z' >= c)
            {
               continue;
            }
            if ('A' <= c && 'Z' >= c)
            {
               continue;
            }
            if ('0' <= c && '9' >= c)
            {
               continue;
            }
            if (c == '_' || c == '-')
            {
               continue;
            }
            load(rollback);
            return false;
         }
      }
      return false;
   }
   // [a - z][a - zA - Z0 - 9_]*
   bool label(std::string& aValue)
   {
      assert(!isspace(at())); // assumes skipWhite() before call
      if (at() >= 'a' && at() <= 'z')
      {
         char* rollback = save();
         aValue.clear();
         for (;;)
         {
            aValue += at();
            char c = next();
            if ('a' <= c && 'z' >= c)
            {
               continue;
            }
            if ('A' <= c && 'Z' >= c)
            {
               continue;
            }
            if ('0' <= c && '9' >= c)
            {
               continue;
            }
            if (c == '_' || c == '-')
            {
               continue;
            }
            if (c == ':')
            {
               next();
               return true;
            }
            load(rollback);
            return false;
         }
      }
      return false;
   }
   // property =>
   //  <label> <object>
   bool property(std::string& aLabel, UtmlObject& aObject)
   {
      assert(!isspace(at())); // assumes skipWhite() before call
      if (!label(aLabel))
      {
         return false;
      }
      skipWhite();
      if (!objectOrValue(aObject))
      {
         throwError();
      }
      return true;
   }
   bool objectOrValue(UtmlObject& aObject)
   {
      assert(!isspace(at())); // assumes skipWhite() before call
      std::string value;
      UtmlObject  childObject;
      if (quote(value) || unquotedValue(value))
      {
         aObject = value;
         return true;
      }
      else if (object(aObject))
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   bool object(UtmlObject& aObject)
   {
      std::string typeText;
      if (typeName(typeText))
      {
         UtmlObject childObject;
         aObject = UtmlObject::makeContainer(typeText);
         skipWhite();
         if (at() != '{')
         {
            throwError();
         }
         next();
         for (;;)
         {
            skipWhite();
            std::string label;
            UtmlObject  prop;
            if (property(label, prop))
            {
               aObject.GiveProperty(label, prop);
            }
            else if (object(childObject))
            {
               aObject.GiveObject(childObject);
            }
            else if (at() == '}')
            {
               next();
               return true;
            }
            else
            {
               throwError("Expected property, object, or '}'");
            }
         }
      }
      else
      {
         return false;
      }
   }

public:
   UtmlParserP(char* aText, size_t aSize)
   {
      p   = aText;
      end = aText + aSize;
   }

   void RootObject(UtmlObject& aObject)
   {
      for (;;)
      {
         skipWhite();
         if (atEnd())
         {
            break;
         }
         UtmlObject obj;
         if (object(obj))
         {
            aObject.GiveObject(obj);
         }
         else
         {
            throwError("Expected object");
         }
      }
   }
};

UtmlObject UtmlObject::makeContainer(const std::string& aType)
{
   UtmlObject obj;
   obj.mText          = aType;
   obj.mContainerData = new ContainerData;
   return obj;
}

UtmlObject UtmlObject::parse(const std::string& aContainerType, const std::string& aText)
{
   UtmlObject     obj = makeContainer(aContainerType);
   UtTextDocument doc;
   doc.Insert(0, aText);
   UtmlParser parser(doc);
   if (parser.Parse(obj))
   {
      return obj;
   }
   return UtmlObject();
}

UtmlObject& UtmlObject::operator=(const UtmlObject& aRhs)
{
   if (this != &aRhs)
   {
      mText = aRhs.mText;
      delete mContainerData;
      mContainerData = aRhs.mContainerData;
      if (mContainerData)
      {
         mContainerData = new ContainerData(*mContainerData);
      }
   }
   return *this;
}

UtmlObject::UtmlObject(const UtmlObject& aRhs)
{
   mContainerData = nullptr;
   (*this)        = aRhs;
}

UtmlObject::UtmlObject(const std::string& aValue)
   : mText(aValue)
   , mContainerData(nullptr)
{
}

UtmlObject::~UtmlObject()
{
   delete mContainerData;
}

UtmlObject& UtmlObject::operator[](const std::string& aName)
{
   return GetProperties()[aName];
}

void UtmlObject::GiveProperty(const std::string& aName, UtmlObject& aObject)
{
   GetProperties()[aName].Swap(aObject);
}

void UtmlObject::GiveObject(UtmlObject& aObject)
{
   UtmlObject* newChild = new UtmlObject;
   newChild->Swap(aObject);
   GetObjects().push_back(newChild);
}

UtmlObject* UtmlObject::FindProperty(const std::string& aName) const
{
   if (!mContainerData)
   {
      return nullptr;
   }
   if (mContainerData->mProperties.find(aName) != mContainerData->mProperties.end())
   {
      return &mContainerData->mProperties[aName];
   }
   return nullptr;
}

std::string UtmlObject::PropertyValue(const std::string& aPropertyName, const std::string& aDefaultValue) const
{
   UtmlObject* propertyPtr = FindProperty(aPropertyName);
   if (propertyPtr)
   {
      return propertyPtr->GetValue();
   }
   return aDefaultValue;
}

std::string UtmlObject::ToText() const
{
   if (IsValue())
   {
      return GetValue();
   }
   else
   {
      UtmlWriter writer;
      writer.mIndent = 1;
      std::stringstream ss;
      writer.WriteRoot(*this, ss);
      return ss.str();
   }
}

bool UtmlObject::Contains(const std::string& aPropertyName) const
{
   if (mContainerData == nullptr)
   {
      return false;
   }
   return mContainerData->mProperties.find(aPropertyName) != mContainerData->mProperties.end();
}

UtmlObject UtmlObject::parseFile(const std::string& aFilePath)
{
   UtPath         docPath(aFilePath);
   UtTextDocument doc(docPath);
   UtmlParser     parser(doc);
   UtmlObject     root = UtmlObject::makeContainer("root");
   try
   {
      parser.Parse(root);
   }
   catch (UtmlParseError& e)
   {
      auto logger = ut::log::error() << "UtmlParseError:";
      logger.AddNote() << e.what();
   }
   return root;
}

std::vector<std::string> UtmlObject::GetValueLines() const
{
   std::vector<std::string> lines = UtStringUtil::Split(mText, '\n');
   for (size_t i = 0; i < lines.size(); ++i)
   {
      UtStringUtil::TrimWhiteSpace(lines[i]);
      if (lines[i].empty())
      {
         lines.erase(lines.begin() + i);
      }
      else
      {
         ++i;
      }
   }
   return lines;
}

void UtmlObject::SetValueLines(const std::vector<std::string>& aLines)
{
   mText = UtStringUtil::Join("\n", aLines);
}

bool UtmlObject::operator==(const UtmlObject& aRhs) const
{
   if (mText != aRhs.mText)
   {
      return false;
   }
   if ((mContainerData == nullptr) != (aRhs.mContainerData == nullptr))
   {
      return false;
   }
   if (mContainerData)
   {
      return *mContainerData == *aRhs.mContainerData;
   }
   return false;
}

bool UtmlObject::GetBoolValue() const
{
   return mText == "true";
}

void UtmlObject::SetBoolValue(bool aValue)
{
   mText = aValue ? "true" : "false";
}

UtmlObject* UtmlObject::FirstObjectByType(const std::string& aTypeName) const
{
   if (!mContainerData)
   {
      return nullptr;
   }
   for (size_t i = 0; i < mContainerData->mObjects.size(); ++i)
   {
      if (mContainerData->mObjects[i]->GetType() == aTypeName)
      {
         return mContainerData->mObjects[i];
      }
   }
   return nullptr;
}


UtmlObject::ContainerData::ContainerData() {}

UtmlObject::ContainerData::ContainerData(const ContainerData& aSrc)
   : mProperties(aSrc.mProperties)
   , mObjects(aSrc.mObjects)
{
   for (size_t i = 0; i < mObjects.size(); ++i)
   {
      mObjects[i] = new UtmlObject(*mObjects[i]);
   }
}

UtmlObject::ContainerData::~ContainerData()
{
   for (size_t i = 0; i < mObjects.size(); ++i)
   {
      delete mObjects[i];
   }
}

UtmlParser::UtmlParser(UtTextDocument& aDocument)
{
   mDocumentPtr = &aDocument;
   char* text   = mDocumentPtr->GetText().GetPointer();
   mImp         = new UtmlParserP(text, mDocumentPtr->GetText().Size());
}

bool UtmlParser::Parse(UtmlObject& aContainer)
{
   assert(aContainer.IsContainer());
   try
   {
      mImp->RootObject(aContainer);
   }
   catch (UtmlParseError& e)
   {
      size_t offset = mImp->p - mDocumentPtr->GetText().GetPointer();
      size_t line, col;
      mDocumentPtr->PositionToLineColumn(offset, line, col);
      std::stringstream ss;
      ss << mDocumentPtr->GetFilePath().GetSystemPath() << "  Line: " << line << "  Col: " << col << '\n';
      ss << "       " << e.mError;
      e.mError = ss.str();
      throw;
   }
   return true;
}

UtmlParser::~UtmlParser()
{
   delete mImp;
}

const char* UtmlParseError::what() const noexcept
{
   return mError.c_str();
}

UtmlParseError::UtmlParseError(const std::string& aError)
   : mError(aError)
{
}

UtmlParseError::~UtmlParseError() {}

namespace
{
void WriteValue(const std::string& aValue, std::ostream& aStream)
{
   bool needsQuoted = aValue.empty();
   for (size_t i = 0; i < aValue.size() && !needsQuoted; ++i)
   {
      char c = aValue[i];
      // does it look like a label?
      if (c == ':' && (i + 1 >= aValue.size() || isspace(aValue[i + 1])))
      {
         needsQuoted = true;
      }
      // newlines
      if (c == '\n' || c == '\r')
      {
         needsQuoted = true;
      }
      // could technically put quotes inside words, but don't show them unquoted
      if (c == '"')
      {
         needsQuoted = true;
      }
   }
   if (!needsQuoted)
   {
      aStream << aValue;
   }
   else
   {
      aStream << '"';
      for (size_t i = 0; i < aValue.size(); ++i)
      {
         char c = aValue[i];
         if (c == '\"')
         {
            aStream << "\"\"";
         }
         else
         {
            aStream << c;
         }
      }
      aStream << '"';
   }
}
void WriteIndent(int aIndent, std::ostream& aStream)
{
   for (int i = 0; i < aIndent; ++i)
   {
      aStream << ' ';
   }
}
void WriteP(const UtmlObject& aObject, int aCurrentIndent, int aIndent, std::ostream& aStream)
{
   if (aObject.IsValue())
   {
      WriteValue(aObject.GetValue(), aStream);
   }
   else
   {
      aStream << aObject.GetType() << " {";
      const UtmlObject::PropertyMap& props = aObject.GetProperties();
      for (UtmlObject::PropertyMap::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         aStream << "\n";
         WriteIndent(aCurrentIndent + aIndent, aStream);
         aStream << i->first << ":  ";
         WriteP(i->second, aCurrentIndent + aIndent, aIndent, aStream);
      }
      for (int i = 0; i < aObject.GetObjectCount(); ++i)
      {
         aStream << "\n";
         WriteIndent(aCurrentIndent + aIndent, aStream);
#undef GetObject // Avoid conflict with Windows macro
         WriteP(aObject.GetObject(i), aCurrentIndent + aIndent, aIndent, aStream);
      }
      aStream << "\n";
      WriteIndent(aCurrentIndent, aStream);
      aStream << "}";
   }
}
} // namespace
void UtmlWriter::Write(const UtmlObject& aObject, std::ostream& aStream)
{
   WriteP(aObject, 0, mIndent, aStream);
}

void UtmlWriter::WriteRoot(const UtmlObject& aObject, std::ostream& aStream)
{
   for (int i = 0; i < aObject.GetObjectCount(); ++i)
   {
      WriteP(aObject.GetObject(i), 0, mIndent, aStream);
   }
}
void UtmlWriter::WriteFile(const UtmlObject& aObject, const std::string& aFileName, bool aObjectIsImplicitRoot)
{
   std::ofstream fs(aFileName);
   if (aObjectIsImplicitRoot)
   {
      WriteRoot(aObject, fs);
   }
   else
   {
      Write(aObject, fs);
   }
}
