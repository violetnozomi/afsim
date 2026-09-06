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

#ifndef UTQML_HPP
#define UTQML_HPP
#include <cassert>
#include <exception>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "WsfUtExport.hpp"


class UtTextDocument;

/* 'Utml' is a simple text file format and associated DOM which looks like a simplified QML
               see http://en.wikipedia.org/wiki/QML
   - There is no javascript
   - There are no special value types
   - There are no lists, aka, '[ ... ]'

  Example:

  // comments are like this
  // Define an object of type 'Widget'
  // Object types must begin with a capital letter
  Widget {
     // Any number of named properties can be set
     // properties begin with a lower-case letter
     id:    foo
     width: 5 meters
     info:  "arbitrary strings
   can be inserted between
      quotes.  Embed quotes like ""this"" "
     stuff: single line text can be used without quotes

     // Objects may contain other objects, not connected to any property
     Part {
       id: part1
     }
     Part {
       id: part2
     }

     object_property: Part {
       // property values can be objects too
       id: part3
     }

  }

*/

class UtmlParserP;

class WSF_UT_EXPORT UtmlParseError : public std::exception
{
public:
   UtmlParseError(const std::string& aError);
   ~UtmlParseError() override;
   const char* what() const noexcept override;

   std::string mError;
};

// An object in a UTML document, or a simple value.
// if IsContainer() is true, this represents an object with this syntax:
// <TypeName> {
//    property_name: property value
//    <...sub-objects and properties...>
// }
// if IsContainer() is false, this represents a single property value
class WSF_UT_EXPORT UtmlObject
{
public:
   typedef std::map<std::string, UtmlObject> PropertyMap;

   UtmlObject()
      : mContainerData(nullptr)
   {
   }
   UtmlObject(const std::string& aValue);
   UtmlObject(const UtmlObject& aRhs);
   UtmlObject& operator=(const UtmlObject& aRhs);

   //! Returns a container object of the specified type
   static UtmlObject makeContainer(const std::string& aType);
   //! Parse a string and return it as a Utml container
   static UtmlObject parse(const std::string& aContainerType, const std::string& aText);

   //! Parse a file and return it as a Utml container
   static UtmlObject parseFile(const std::string& aFilePath);

   ~UtmlObject();

   //! An object can either be a container or a simple value.
   //! Returns 'true' if this is a simple value
   bool IsValue() const { return mContainerData == nullptr; }
   //! An object can either be a container or a simple value.
   //! Returns 'true' if this is a container
   bool IsContainer() const { return mContainerData != nullptr; }

   //! To be used when IsContainer() is false, the value as a string
   const std::string& GetValue() const
   {
      assert(!IsContainer());
      return mText;
   }

   //! Assign the value.  Not valid for containers
   void SetValue(const std::string& aValue)
   {
      assert(!IsContainer());
      mText = aValue;
   }

   //!{ Standard-value processing
   //!    values are free-form, but these helper methods are provided to read specific value formats
   //! Multi-line values are allowed.  This is a helper method to assign a multi-line value
   void SetValueLines(const std::vector<std::string>& aLines);

   //! Multi-line values are allowed.  This is a helper method to parse multiple lines, trimming whitespace
   std::vector<std::string> GetValueLines() const;

   bool GetBoolValue() const;

   void SetBoolValue(bool aValue);

   //!}


   //! Container methods
   //! These methods are for operating on container objects.
   //! To be called only if IsContainer() is true.
   //!{

   //! Returns the type name of the object
   const std::string& GetType() const
   {
      assert(IsContainer());
      return mText;
   }

   //! Returns the list of properties on the object
   std::map<std::string, UtmlObject>& GetProperties()
   {
      assert(mContainerData);
      return mContainerData->mProperties;
   }

   //! Returns the list of properties on the object
   const std::map<std::string, UtmlObject>& GetProperties() const
   {
      assert(mContainerData);
      return mContainerData->mProperties;
   }
   //! Returns the named property if it exists
   UtmlObject* FindProperty(const std::string& aName) const;

   //! If the name property exists and is a simple value, returns the value as a string.
   //! otherwise, aDefaultValue is returned.
   std::string PropertyValue(const std::string& aPropertyName, const std::string& aDefaultValue) const;

   //! Returns 'true' if the property by this name exists on this object
   bool Contains(const std::string& aPropertyName) const;

   //! Return a property if it exists.  If the property does not exist, create
   //! a new property with an empty value
   UtmlObject& operator[](const std::string& aName);

   //! Add a sub-object to this object.
   UtmlObject& operator+=(const UtmlObject& aRhs)
   {
      assert(IsContainer());
      mContainerData->mObjects.push_back(new UtmlObject(aRhs));
      return *this;
   }

   //! Returns the number of objects in the container
   int GetObjectCount() const { return mContainerData ? (int)mContainerData->mObjects.size() : 0; }
#undef GetObject // Avoid conflict with Windows macro
   //! returns the object at the specified index
   UtmlObject& GetObject(int aIndex)
   {
      assert(GetObjectCount() > aIndex && aIndex >= 0);
      return *mContainerData->mObjects[aIndex];
   }
   //! returns the object at the specified index
   const UtmlObject& GetObject(int aIndex) const
   {
      assert(GetObjectCount() > aIndex && aIndex >= 0);
      return *mContainerData->mObjects[aIndex];
   }

   //! Returns a pointer to the first object with the specified type.
   //! Returns 'null' if none exist.
   UtmlObject* FirstObjectByType(const std::string& aTypeName) const;

   //! Consumes aObject and adds the object as a named property of this container
   void GiveProperty(const std::string& aName, UtmlObject& aObject);
   //! Consumes aObject and adds the object as a sub-object
   void GiveObject(UtmlObject& aObject);

   //!}

   //! Swaps the value of two objects in constant time
   void Swap(UtmlObject& aRhs)
   {
      std::swap(mText, aRhs.mText);
      std::swap(mContainerData, aRhs.mContainerData);
   }

   std::string ToText() const;

   bool operator==(const UtmlObject& aRhs) const;

   bool operator!=(const UtmlObject& aRhs) const { return !this->operator==(aRhs); }

private:
   std::vector<UtmlObject*>& GetObjects() { return mContainerData->mObjects; }

   struct ContainerData
   {
      ContainerData();
      ~ContainerData();
      ContainerData(const ContainerData& aSrc);

      bool operator==(const ContainerData& aRhs) const
      {
         if (this == &aRhs)
         {
            return true;
         }
         if (mObjects.size() != aRhs.mObjects.size())
         {
            return false;
         }
         if (mProperties != aRhs.mProperties)
         {
            return false;
         }
         for (size_t i = 0; i < mObjects.size(); ++i)
         {
            if (*mObjects[i] != *aRhs.mObjects[i])
            {
               return false;
            }
         }
         return true;
      }

      bool operator!=(const ContainerData& aRhs) const { return !this->operator==(aRhs); }

      std::map<std::string, UtmlObject> mProperties;
      std::vector<UtmlObject*>          mObjects;

   private:
      ContainerData& operator=(const ContainerData&); // no imp
   };
   std::string    mText; // value or type
   ContainerData* mContainerData;
};


//! Read a Utml file into properties and sub-objects of a root UtmlObject
class WSF_UT_EXPORT UtmlParser
{
public:
   UtmlParser(UtTextDocument& aDocument);
   ~UtmlParser();
   bool Parse(UtmlObject& aContainer);

private:
   UtmlParser&     operator=(const UtmlParser&); // not implemented
   UtmlParserP*    mImp;
   UtTextDocument* mDocumentPtr;
};

//! Write a Utml object to a text stream
class WSF_UT_EXPORT UtmlWriter
{
public:
   UtmlWriter()
      : mIndent(2)
   {
   }
   void Write(const UtmlObject& aObject, std::ostream& aStream);
   void WriteRoot(const UtmlObject& aObject, std::ostream& aStream);

   void WriteFile(const UtmlObject& aObject, const std::string& aFileName, bool aObjectIsImplicitRoot);
   int  mIndent;

private:
};

#endif
