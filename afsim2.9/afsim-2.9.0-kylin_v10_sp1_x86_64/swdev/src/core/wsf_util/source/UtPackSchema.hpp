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

#ifndef UTPACKSCHEMA_HPP
#define UTPACKSCHEMA_HPP

#include <exception>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "WsfUtExport.hpp"

class UtmlObject;
class UtPackNamespace;
class UtPackType;
class UtPackTypeReference;
class UtPackField;
class UtPackStruct;
class UtPackListType;
class UtPackBasicType;

// see UtPack.hpp
//
// This file contains classes related to UtPack's schema.
//

class UtPackSchema;


class WSF_UT_EXPORT UtPackException : public std::exception
{
public:
   UtPackException(const std::string& aReason);
   ~UtPackException() override {}
   const char* what() const noexcept override;
   std::string mReason;
};

class WSF_UT_EXPORT UtPackPrinter
{
public:
   UtPackPrinter(std::ostream* aStream)
      : mStreamPtr(aStream)
   {
      mIndent = 0;
   }

   void Begin(const std::string& aType)
   {
      WriteIndent();
      (*mStreamPtr) << aType << " {\n";
      mIndent += 2;
   }

   void End()
   {
      mIndent -= 2;
      WriteIndent();
      (*mStreamPtr) << "}\n";
   }

   void Field(const std::string& aName)
   {
      WriteIndent();
      (*mStreamPtr) << aName << ": ";
   }
   void Value(const std::string& aValue) { (*mStreamPtr) << aValue << '\n'; }

private:
   void WriteIndent()
   {
      for (int i = 0; i < mIndent; ++i)
      {
         (*mStreamPtr) << ' ';
      }
   }
   int           mIndent;
   std::ostream* mStreamPtr;
};

// Contains the schema read from the schema text definitions
class WSF_UT_EXPORT UtPackSchema
{
public:
   UtPackSchema();

   void                               Read(UtmlObject& aInput);
   void                               Resolve();
   static std::unique_ptr<UtPackType> ReadType(UtmlObject& aInput, UtPackType& aParentType);
   UtPackType*                        FindType(const std::string& aTypeName);
   const UtPackType*                  FindType(const std::string& aTypeName) const;

   std::vector<std::string> GetTypeNames() const;
   // Namespace name is the C++ namespace used when generating code
   const std::string& GetNamespaceName() const { return mNamespaceName; }

private:
   std::shared_ptr<UtPackNamespace> mRoot;
   std::string                      mNamespaceName;
};

// Base type for all schema types
class WSF_UT_EXPORT UtPackType
{
public:
   using NestedTypesList = std::vector<std::pair<std::string, std::unique_ptr<UtPackType>>>;

   UtPackType(const UtPackType&) = delete;
   UtPackType& operator=(const UtPackType&) = delete;

   // Destructor must be made public to allow use with unique_ptr
   virtual ~UtPackType() = default;

   enum Encoding
   {
      cENC_NONE,          // don't know how to encode the type
      cENC_LITTLE_ENDIAN, // little endian plain ol' data
      cENC_VAR_BYTES,     // size + byte array
      cENC_BIT,           // a bit inside a struct's bitset
      cENC_STRUCT,        // a struct
      cENC_LIST           // a list
   };

   const std::string& GetTypeName() const;
   //! Returns the full path to the type, with '.' as a separator.  example: "Track.TrackId"
   std::string            GetTypePath() const;
   UtPackType*            FindType(const std::string& aTypeName);
   const UtPackType*      FindType(const std::string& aTypeName) const;
   virtual void           Resolve();
   Encoding               GetEncoding() const { return mEncoding; }
   bool                   IsStruct() const;
   bool                   IsUnion() const;
   bool                   IsBasicType() const;
   bool                   IsEnum() const;
   bool                   IsList() const;
   const NestedTypesList& GetNestedTypes() const { return mNestedTypes; }

protected:
   const UtPackType* FindTypeP(const std::string& aTypeName) const;
   UtPackType(UtPackType* aParentTypePtr);
   const UtPackType* GetRootType() const;
   UtPackType*       GetRootType();
   UtPackSchema*     GetSchema() const;

   void Read(UtmlObject& aInput);
   void ReadNestedTypes(UtmlObject& aInput);

   Encoding        mEncoding;
   UtPackType*     mParentType;
   std::string     mTypeName;
   NestedTypesList mNestedTypes;
};

// A reference to a type
class WSF_UT_EXPORT UtPackTypeReference final
{
public:
   UtPackTypeReference()
      : mContextType(nullptr)
      , mTypePtr(nullptr)
   {
   }
   UtPackTypeReference(const std::string& aTypeName, UtPackType* aParentType);

   void        Resolve();
   UtPackType* GetType() const { return mTypePtr; }

   bool IsNull() const { return mTypeName.empty(); }

private:
   std::string mTypeName;
   UtPackType* mContextType;
   UtPackType* mTypePtr;
};


//////////////////////////////////////////////////////////////////////////
// BSD Type classes - Basic type schema
//////////////////////////////////////////////////////////////////////////

//! A field in a struct or union
class WSF_UT_EXPORT UtPackField final
{
public:
   UtPackField()
      : mIsObsolete(false)
      , mOptionIndex(-1)
      , mBitIndex(-1)
      , mIsIndex(false)
      , mIsTime(false)
   {
   }

   bool                       Read(UtmlObject& aInput, UtPackType& aContainer);
   UtPackTypeReference&       GetType() { return mType; }
   const UtPackTypeReference& GetType() const { return mType; }
   std::string                GetFieldName() const { return mName; }
   bool                       IsOptional() const { return mOptionIndex != -1; }
   std::string                GetDefaultValue() const { return mDefaultValue; }
   // Optional field bit number, -1 if not optional
   int GetOptionIndex() const { return mOptionIndex; }
   // For boolean values only, the bit used to store the value.  -1 if none
   int GetBitIndex() const { return mBitIndex; }
   // Used for accessing AFSIM platform indices from structs.
   bool IsIndex() const { return mIsIndex; }
   // Used for accessing sim times from structs.
   bool IsTime() const { return mIsTime; }

   bool IsObsolete() const { return mIsObsolete; }

private:
   std::string         mName;
   UtPackTypeReference mType;
   bool                mIsObsolete;
   std::string         mDefaultValue;
   short               mOptionIndex;
   short               mBitIndex;
   bool                mIsIndex;
   bool                mIsTime;
};

//! A struct is a set of fields
class WSF_UT_EXPORT UtPackStruct final : public UtPackType
{
public:
   UtPackStruct(UtPackType& aParentType);
   void                            Read(UtmlObject& aInput, UtPackType& aContainer);
   void                            Resolve() override;
   const std::vector<UtPackField>& GetFields() const { return mFields; }
   std::vector<UtPackField>        GetAllFields() const;

   int GetBitsetSize() const { return mBitsetSize; }
   int GetBitsetBytes() const { return (mBitsetSize + 7) / 8; }
   //! Returns -1 if struct is not a message
   int                        GetMessageId() const { return mMessageId; }
   bool                       IsMessage() const { return mMessageId != -1; }
   bool                       IsVirtual() const { return mMessageId == -2; }
   const UtPackTypeReference& GetBaseType() const { return mBaseType; }

private:
   int                      mBitsetSize;
   std::vector<UtPackField> mFields;
   UtPackTypeReference      mBaseType;
   int                      mMessageId;
};

//! A union is a choice of a single field from many
class WSF_UT_EXPORT UtPackUnion final : public UtPackType
{
public:
   UtPackUnion(UtPackType& aParentType);
   void                            Read(UtmlObject& aInput, UtPackType& aContainer);
   void                            Resolve() override;
   const std::vector<UtPackField>& GetFields() const { return mFields; }

private:
   std::vector<UtPackField> mFields;
};

//! A list type defined in the schema
class WSF_UT_EXPORT UtPackListType final : public UtPackType
{
public:
   UtPackListType(UtPackType& aParent)
      : UtPackType(&aParent)
   {
   }
   bool Read(UtmlObject& aInput);
   void Resolve() override;

   UtPackTypeReference mContainedType;
};

//! A basic type defined in the schema
class WSF_UT_EXPORT UtPackBasicType final : public UtPackType
{
public:
   UtPackBasicType(UtPackType& aParent);
   void               Read(UtmlObject& aInput);
   const std::string& GetCPP_Type() const;
   int                GetSize() const { return mSize; }

private:
   std::string mCPP_Type;
   int         mSize;
};

//! A enum type defined in the schema
class WSF_UT_EXPORT UtPackEnumType final : public UtPackType
{
public:
   UtPackEnumType(UtPackType& aParent);
   void                              Read(UtmlObject& aInput);
   void                              Resolve() override;
   std::string                       GetCPP_Type() const { return mCPP_Type; }
   std::string                       GetUnderlyingType() const { return mUnderlyingType; }
   const std::map<std::string, int>& GetEnumValues() const { return mEnumValues; }
   const std::map<int, std::string>& GetValueToName() const { return mValueToName; }

private:
   std::map<std::string, int> mEnumValues;
   std::map<int, std::string> mValueToName;
   std::string                mCPP_Type;
   std::string                mUnderlyingType;
};

class WSF_UT_EXPORT UtPackNamespace final : public UtPackType
{
public:
   UtPackNamespace(UtPackSchema* aSchemaPtr)
      : UtPackType(nullptr)
      , mSchemaPtr(aSchemaPtr)
   {
      mTypeName = "";
   }
   void          Read(UtmlObject& aInput);
   UtPackSchema* GetSchema() const { return mSchemaPtr; }

private:
   UtPackSchema* mSchemaPtr;
};

#endif
