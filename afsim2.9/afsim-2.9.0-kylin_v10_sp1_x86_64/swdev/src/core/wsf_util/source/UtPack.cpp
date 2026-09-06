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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "UtPack.hpp"

#include <iostream>
#include <set>
#include <sstream>

#include "UtCompilerAttributes.hpp"
#include "UtMemory.hpp"
#include "UtPackSchema.hpp"
#include "UtPackStream.hpp"
#include "UtStringUtil.hpp"
#include "Utml.hpp"

class UtPackStream;

namespace
{
UT_INLINE_DECL bool testBit(int* aBits, int aIndex)
{
   return 0 != (aBits[aIndex / 32] & (1 << (aIndex % 32)));
}
} // namespace

class UtPackSerializeFunctor_List : public UtPackSerializeFunctor
{
public:
   UtPackSerializeFunctor_List(UtPackListType* aType);

   void Initialize(UtPackSerializer& aSerializer) override;
   void Read(UtPackStream& bs, void* aDataPtr) override;
   void SkipRead(UtPackStream& bs) override;
   void Write(UtPackStream& bs, void* aDataPtr) override;
   void Print(void* aDataPtr, UtPackPrinter& aStream) override;

   // local type nested in list
   UtPackSerializerTypeData*               mTypeData;
   UtPackListType*                         mListType;
   std::shared_ptr<UtPackSerializeFunctor> mSerializeFn;
   int                                     mTypeSizeBytes;
};

// map a foreign field to a local field
struct UtPackFieldForeignToLocal
{
   UtPackFieldForeignToLocal()
      : mSerializeFunctorPtr(nullptr)
      , mLocalFieldOffset(0)
      , mOptionIndex(-1)
      , mLocalFieldIndex(-1)
   {
   }

   // if skipping, need to know how to skip
   // A function that takes the foreign field and reads into the local field
   std::shared_ptr<UtPackSerializeFunctor> mSerializeFunctorPtr;
   short                                   mLocalFieldOffset;
   short                                   mOptionIndex;
   short                                   mLocalFieldIndex;
};

// Serializes structs (defined in the schema)
class UtPackSerializeFunctor_Struct : public UtPackSerializeFunctor
{
   /* Encoding:
   - Bits for optional fields and boolean values
   - Required fields in order of definition
   - Optional fields in order of optional bit
   */
public:
   UtPackSerializeFunctor_Struct(UtPackStruct* aStructPtr, UtPackStructLayout* aLayout)
      : mForeign()
      , mStructPtr(aStructPtr)
      , mLayoutPtr(aLayout)
      , mOptionalFlagsBytes(0)
   {
   }

   void Initialize(UtPackSerializer& aSerializer) override;
   void Read(UtPackStream& bs, void* aDataPtr) override;

   void Write(UtPackStream& bs, void* aDataPtr) override;

   void SkipRead(UtPackStream& bs) override;

   void Print(void* aDataPtr, UtPackPrinter& aStream) override
   {
      aStream.Begin("Obj");
      aStream.Field("type");
      aStream.Value(mStructPtr->GetTypeName());

      int  optionalBytes[UtPackSerializer::cMAXIMUM_OPTIONAL_BITS / 32] = {0};
      int* localOptionsBits = (int*)((char*)aDataPtr + mLayoutPtr->mOptionalFlagsOffset);
      memcpy(optionalBytes, localOptionsBits, mLayoutPtr->mOptionalFlagsBytes);

      for (const UtPackFieldForeignToLocal& ftl : mForeign)
      {
         // is the field required or optionally provided?
         bool fieldAvailable = (ftl.mOptionIndex < 0 || testBit(localOptionsBits, ftl.mOptionIndex));
         if (fieldAvailable)
         {
            // is the field in the schema definition?
            if (ftl.mLocalFieldOffset >= 0)
            {
               void* memberPtr = ((char*)aDataPtr) + ftl.mLocalFieldOffset;
               aStream.Field(mLayoutPtr->mMembers[ftl.mLocalFieldIndex].mName);
               ftl.mSerializeFunctorPtr->Print(memberPtr, aStream);
            }
         }
      }
      aStream.End();
   }
   // For each field in the struct as defined by the foreign schema
   // define how the field is read or skipped
   std::vector<UtPackFieldForeignToLocal> mForeign;
   // The (foreign) struct's schema
   UtPackStruct* mStructPtr;
   // The native layout of the struct
   UtPackStructLayout* mLayoutPtr;
   // Number of bytes used as optional flags in message header
   int mOptionalFlagsBytes;
};

// Serializes unions (defined in the schema)
class UtPackSerializeFunctor_Union : public UtPackSerializeFunctor
{
   /* Encoding:
   - Field Option Index (1 byte)
   - Field Data
   */
public:
   UtPackSerializeFunctor_Union(UtPackUnion* aUnionPtr, UtPackStructLayout* aLayout)
      : mUnionPtr(aUnionPtr)
      , mLayoutPtr(aLayout)
   {
   }

   void Initialize(UtPackSerializer& aSerializer) override
   {
      // Grab the list of fields for this struct and all base classes
      std::vector<UtPackField> fields = mUnionPtr->GetFields();

      for (const UtPackField& field : fields)
      {
         UtPackType*               fieldTypePtr = field.GetType().GetType();
         UtPackSerializerTypeData* typeDataPtr  = aSerializer.FindTypeData(fieldTypePtr->GetTypePath());
         if (!typeDataPtr)
         {
            aSerializer.FindTypeData(fieldTypePtr->GetTypePath());
         }
         UtPackFieldForeignToLocal ftl;
         ftl.mOptionIndex                               = field.GetOptionIndex();
         bool                              foundMapping = false;
         const UtPackStructLayout::Member* memberPtr    = mLayoutPtr->FindMember(field.GetFieldName());
         if (memberPtr)
         {
            std::string fullTypeName = memberPtr->mTypeName;
            if (fieldTypePtr->GetTypeName() == fullTypeName)
            {
               fullTypeName = fieldTypePtr->GetTypePath();
            }
            ftl.mSerializeFunctorPtr = aSerializer.FindSerializeFunction(fullTypeName, fieldTypePtr->GetTypePath());
            if (ftl.mSerializeFunctorPtr)
            {
               foundMapping          = true;
               ftl.mLocalFieldOffset = memberPtr->mOffset;
               ftl.mLocalFieldIndex  = mLayoutPtr->GetMemberIndex(field.GetFieldName());
            }
         }
         if (!foundMapping)
         {
            // Always need a serializer, even if skipping
            ftl.mSerializeFunctorPtr = typeDataPtr->mSerializer;
            ftl.mLocalFieldOffset    = -1; // not available
         }
         if (ftl.mSerializeFunctorPtr)
         {
            if ((short)mForeign.size() <= ftl.mOptionIndex)
            {
               mForeign.resize(ftl.mOptionIndex + 1);
            }
            mForeign[ftl.mOptionIndex] = ftl;
         }
      }
   }
   void Read(UtPackStream& bs, void* aDataPtr) override
   {
      unsigned char fieldType;
      bs.Read(&fieldType, 1);
      if (fieldType != 255)
      {
         assert(fieldType < mForeign.size());
         if (fieldType < mForeign.size())
         {
            const UtPackFieldForeignToLocal& ftl = mForeign[fieldType];
            if (ftl.mLocalFieldIndex != -1)
            {
               (*mLayoutPtr->mSetFieldTypeFuncPtr)(aDataPtr, fieldType);
               void* memberPtr = ((char*)aDataPtr) + ftl.mLocalFieldOffset;
               ftl.mSerializeFunctorPtr->Read(bs, memberPtr);
            }
            else
            {
               // C struct doesn't have that member, must skip
               if (ftl.mSerializeFunctorPtr)
               {
                  ftl.mSerializeFunctorPtr->SkipRead(bs);
               }
            }
         }
      }
   }

   void Write(UtPackStream& bs, void* aDataPtr) override
   {
      unsigned char fieldType = *(unsigned char*)aDataPtr;
      bs.Write(&fieldType, 1);
      if (fieldType != 255)
      {
         if (fieldType < mForeign.size())
         {
            const UtPackFieldForeignToLocal& ftl = mForeign[fieldType];
            if (ftl.mLocalFieldIndex >= 0)
            {
               void* memberPtr = ((char*)aDataPtr) + ftl.mLocalFieldOffset;
               ftl.mSerializeFunctorPtr->Write(bs, memberPtr);
            }
         }
      }
   }

   void SkipRead(UtPackStream& bs) override
   {
      unsigned char fieldType;
      bs.Read(&fieldType, 1);
      if (fieldType != 255)
      {
         assert(fieldType < mForeign.size());
         if (fieldType < mForeign.size())
         {
            const UtPackFieldForeignToLocal& ftl = mForeign[fieldType];
            if (ftl.mLocalFieldIndex != -1)
            {
               // C struct doesn't have that member, must skip
               if (ftl.mSerializeFunctorPtr)
               {
                  ftl.mSerializeFunctorPtr->SkipRead(bs);
               }
            }
         }
      }
   }

   void Print(void* aDataPtr, UtPackPrinter& aStream) override
   {
      aStream.Begin("Obj");
      aStream.Field("type");
      aStream.Value(mUnionPtr->GetTypeName());
      // todo
      aStream.End();
   }

   // indexed by enumeration value
   std::vector<UtPackFieldForeignToLocal> mForeign;
   // the foreign union's schema
   UtPackUnion* mUnionPtr;
   // The native layout of the union
   UtPackStructLayout* mLayoutPtr;
};

// Serialize a string
class UtPackSerializeFunctor_String : public UtPackSerializeFunctor
{
public:
   UtPackSerializeFunctor_String()
   {
      mTempBufferSize = 1024;
      mTempBuffer     = new char[mTempBufferSize];
   }
   ~UtPackSerializeFunctor_String() override { delete[] mTempBuffer; }
   void Read(UtPackStream& bs, void* aDataPtr) override
   {
      std::string* strPtr = (std::string*)aDataPtr;
      int          size;
      bs.Read(&size, sizeof(int));
      // use a fixed-size buffer when the size isn't too big
      // otherwise allocate on the heap
      if (size > mTempBufferSize)
      {
         char* txtBuf = new char[size];
         bs.Read(txtBuf, size);
         strPtr->assign(txtBuf, txtBuf + size);
         delete[] txtBuf;
      }
      else
      {
         bs.Read(mTempBuffer, size);
         strPtr->assign(mTempBuffer, mTempBuffer + size);
      }
   }

   void Write(UtPackStream& bs, void* aDataPtr) override
   {
      std::string* strPtr = (std::string*)aDataPtr;
      int          size   = (int)strPtr->size();
      bs.Write(&size, sizeof(int));
      bs.Write(strPtr->c_str(), size);
   }
   void SkipRead(UtPackStream& bs) override
   {
      int size;
      bs.Read(&size, sizeof(int));
      bs.SkipRead(size);
   }
   void Print(void* aDataPtr, UtPackPrinter& aStream) override { aStream.Value(*(std::string*)aDataPtr); }
   bool FromString(void* aDataPtr, const std::string& aString) override
   {
      *(std::string*)aDataPtr = aString;
      return true;
   }

   char* mTempBuffer;
   int   mTempBufferSize;
};

// Serialize a basic type (int, float, etc...)
template<typename T>
class UtPackSerializeFunctorC_Type : public UtPackSerializeFunctor
{
public:
   void Read(UtPackStream& bs, void* aDataPtr) override { bs.Read((T*)aDataPtr, sizeof(T)); }
   void Write(UtPackStream& bs, void* aDataPtr) override { bs.Write((T*)aDataPtr, sizeof(T)); }
   void SkipRead(UtPackStream& bs) override { bs.SkipRead(sizeof(T)); }
   void Print(void* aDataPtr, UtPackPrinter& aStream) override
   {
      std::stringstream ss;
      ss << *(T*)aDataPtr;
      aStream.Value(ss.str());
   }
   bool FromString(void* aDataPtr, const std::string& aString) override
   {
      std::stringstream ss(aString);
      ss >> *(T*)aDataPtr;
      if (ss)
      {
         return true;
      }
      return false;
   }
};

// Serialize from one basic type to another (int -> float, uint8 -> int16, etc...)
// Allows backwards compatibility when changing schema types
template<typename DESERIALIZED_TYPE, typename SERIALIZED_TYPE>
class UtPackSerializeFunctorCastConvert : public UtPackSerializeFunctor
{
public:
   ~UtPackSerializeFunctorCastConvert() override = default;

   void Read(UtPackStream& bs, void* aDataPtr) override
   {
      // read as one type and cast to other type
      SERIALIZED_TYPE value;
      bs.Read((SERIALIZED_TYPE*)&value, sizeof(SERIALIZED_TYPE));
      *((DESERIALIZED_TYPE*)aDataPtr) = (DESERIALIZED_TYPE)value;
   }

   void Write(UtPackStream& bs, void* aDataPtr) override
   {
      SERIALIZED_TYPE value = (SERIALIZED_TYPE) * ((DESERIALIZED_TYPE*)aDataPtr);
      bs.Write((SERIALIZED_TYPE*)&value, sizeof(SERIALIZED_TYPE));
   }

   void SkipRead(UtPackStream& bs) override { bs.SkipRead(sizeof(SERIALIZED_TYPE)); }

   // Print value as human readable text
   // should be implemented by every type
   void Print(void* aDataPtr, UtPackPrinter& aStream) override
   {
      // unused
   }
};

void UtPackSerializeFunctor_Struct::Initialize(UtPackSerializer& aSerializer)
{
   // Grab the list of fields for this struct and all base classes
   std::vector<UtPackField> fields;
   {
      std::vector<UtPackStruct*> allStructs;
      UtPackStruct*              structPtr = mStructPtr;
      while (structPtr)
      {
         allStructs.push_back(structPtr);
         structPtr = dynamic_cast<UtPackStruct*>(structPtr->GetBaseType().GetType());
      }
      for (int i = (int)allStructs.size() - 1; i >= 0; --i)
      {
         const std::vector<UtPackField>& theseFields = allStructs[i]->GetFields();
         fields.insert(fields.end(), theseFields.begin(), theseFields.end());
      }
   }

   for (const UtPackField& field : fields)
   {
      UtPackType*               fieldTypePtr = field.GetType().GetType();
      UtPackSerializerTypeData* typeDataPtr  = aSerializer.FindTypeData(fieldTypePtr->GetTypePath());
      if (!typeDataPtr)
      {
         aSerializer.FindTypeData(fieldTypePtr->GetTypePath());
      }
      UtPackFieldForeignToLocal ftl;
      ftl.mOptionIndex                               = field.GetOptionIndex();
      bool                              foundMapping = false;
      const UtPackStructLayout::Member* memberPtr    = mLayoutPtr->FindMember(field.GetFieldName());
      if (memberPtr)
      {
         // we found the member in the native struct

         std::string fullTypeName = memberPtr->mTypeName;
         if (fieldTypePtr->GetTypeName() == fullTypeName)
         {
            fullTypeName = fieldTypePtr->GetTypePath();
         }
         std::string foreignTypeName = fieldTypePtr->GetTypePath();

         // special support for enums, determine enum's size and treat as integer
         if (fieldTypePtr->IsEnum())
         {
            UtPackEnumType* enumType = dynamic_cast<UtPackEnumType*>(fieldTypePtr);
            foreignTypeName          = enumType->GetUnderlyingType();
         }

         ftl.mSerializeFunctorPtr = aSerializer.FindSerializeFunction(fullTypeName, foreignTypeName);
         if (ftl.mSerializeFunctorPtr)
         {
            foundMapping          = true;
            ftl.mLocalFieldOffset = memberPtr->mOffset;
            ftl.mLocalFieldIndex  = mLayoutPtr->GetMemberIndex(field.GetFieldName());
         }
      }
      if (!foundMapping)
      {
         // Always need a serializer, even if skipping
         // Except in the case of 'bool', which is transmitted in the bits section
         if (fieldTypePtr->GetTypePath() == "bool")
         {
            ftl.mSerializeFunctorPtr = nullptr;
         }
         else
         {
            ftl.mSerializeFunctorPtr = typeDataPtr->mSerializer;
         }
         ftl.mLocalFieldOffset = -1; // not available
      }
      if (ftl.mSerializeFunctorPtr)
      {
         mForeign.push_back(ftl);
      }
   }
   mOptionalFlagsBytes = mStructPtr->GetBitsetBytes();
}

void UtPackSerializeFunctor_Struct::Write(UtPackStream& bs, void* aDataPtr)
{
   if (mStructPtr->GetBitsetSize() == 0)
   {
      // no optional fields
      for (const UtPackFieldForeignToLocal& ftl : mForeign)
      {
         // is the field in the schema definition?
         if (ftl.mLocalFieldOffset >= 0)
         {
            void* memberPtr = ((char*)aDataPtr) + ftl.mLocalFieldOffset;
            ftl.mSerializeFunctorPtr->Write(bs, memberPtr);
         }
      }
   }
   else
   {
      int  optionalBytes[UtPackSerializer::cMAXIMUM_OPTIONAL_BITS / 32] = {0};
      int* localOptionsBits = (int*)((char*)aDataPtr + mLayoutPtr->mOptionalFlagsOffset);
      memcpy(optionalBytes, localOptionsBits, mLayoutPtr->mOptionalFlagsBytes);
      bs.Write(optionalBytes, mStructPtr->GetBitsetBytes());

      for (const UtPackFieldForeignToLocal& ftl : mForeign)
      {
         // is the field required or optionally provided?
         bool fieldAvailable = (ftl.mOptionIndex < 0 || testBit(localOptionsBits, ftl.mOptionIndex));
         if (fieldAvailable)
         {
            // is the field in the schema definition?
            if (ftl.mLocalFieldOffset >= 0)
            {
               void* memberPtr = ((char*)aDataPtr) + ftl.mLocalFieldOffset;
               ftl.mSerializeFunctorPtr->Write(bs, memberPtr);
            }
         }
      }
   }
}

void UtPackSerializeFunctor_Struct::Read(UtPackStream& bs, void* aDataPtr)
{
   // Read optional flags
   int optionalBytes[UtPackSerializer::cMAXIMUM_OPTIONAL_BITS / 32] = {0};
   bs.Read(&optionalBytes, mOptionalFlagsBytes);

   // copy optional flags to local struct
   int* localOptionsBits = (int*)((char*)aDataPtr + mLayoutPtr->mOptionalFlagsOffset);
   memcpy(localOptionsBits, optionalBytes, mLayoutPtr->mOptionalFlagsBytes);

   for (const UtPackFieldForeignToLocal& ftl : mForeign)
   {
      bool fieldAvailable = (ftl.mOptionIndex < 0 || testBit(optionalBytes, ftl.mOptionIndex));
      if (fieldAvailable)
      {
         if (ftl.mLocalFieldOffset >= 0)
         {
            void* memberPtr = ((char*)aDataPtr) + ftl.mLocalFieldOffset;
            ftl.mSerializeFunctorPtr->Read(bs, memberPtr);
         }
         else
         {
            // C struct doesn't have that member, must skip
            if (ftl.mSerializeFunctorPtr)
            {
               ftl.mSerializeFunctorPtr->SkipRead(bs);
            }
         }
      }
   }
}

void UtPackSerializeFunctor_Struct::SkipRead(UtPackStream& bs)
{
   // Read optional flags
   int optionalBytes[UtPackSerializer::cMAXIMUM_OPTIONAL_BITS / 32] = {0};
   bs.Read(&optionalBytes, mOptionalFlagsBytes);

   for (const UtPackFieldForeignToLocal& ftl : mForeign)
   {
      bool fieldAvailable = (ftl.mOptionIndex < 0 || testBit(optionalBytes, ftl.mOptionIndex));
      if (fieldAvailable)
      {
         if (ftl.mSerializeFunctorPtr)
         {
            ftl.mSerializeFunctorPtr->SkipRead(bs);
         }
      }
   }
}

// Register conversion functions between two types
void UtPackSerializer::RegisterConversion(const std::string&                      aType1,
                                          const std::string&                      aType2,
                                          std::unique_ptr<UtPackSerializeFunctor> aSerializeToType1,
                                          std::unique_ptr<UtPackSerializeFunctor> aSerializeToType2)
{
   if (aType1 != aType2)
   {
      //
      auto& f1 = mTypeConversions[aType1][aType2]; // = aSerializeToType1;
      auto& f2 = mTypeConversions[aType2][aType1];
      if (!f1)
      {
         f1 = std::move(aSerializeToType1);
         f2 = std::move(aSerializeToType2);
      }
   }
}

std::shared_ptr<UtPackSerializeFunctor> UtPackSerializer::FindSerializeFunction(const std::string& aTypeInMemory,
                                                                                const std::string& aSerializedType)
{
   if (aTypeInMemory == aSerializedType)
   {
      TypeData* typePtr = FindTypeData(aTypeInMemory);
      if (typePtr)
      {
         return typePtr->mSerializer;
      }
      return nullptr;
   }
   auto i = mTypeConversions.find(aTypeInMemory);
   if (i != mTypeConversions.end())
   {
      auto j = i->second.find(aSerializedType);
      if (j != i->second.end())
      {
         return j->second;
      }
   }
   return nullptr;
}

void UtPackSerializer::RegisterBuiltinTypes()
{
   RegisterBasicType("int", ut::make_unique<UtPackSerializeFunctorC_Type<int>>());
   RegisterBasicType("uint", ut::make_unique<UtPackSerializeFunctorC_Type<unsigned int>>());
   RegisterBasicType("int8", ut::make_unique<UtPackSerializeFunctorC_Type<std::int8_t>>());
   RegisterBasicType("uint8", ut::make_unique<UtPackSerializeFunctorC_Type<std::uint8_t>>());
   RegisterBasicType("int16", ut::make_unique<UtPackSerializeFunctorC_Type<std::int16_t>>());
   RegisterBasicType("uint16", ut::make_unique<UtPackSerializeFunctorC_Type<std::uint16_t>>());
   RegisterBasicType("int64", ut::make_unique<UtPackSerializeFunctorC_Type<std::int64_t>>());
   RegisterBasicType("uint64", ut::make_unique<UtPackSerializeFunctorC_Type<std::uint64_t>>());

   RegisterBasicType("float", ut::make_unique<UtPackSerializeFunctorC_Type<float>>());
   RegisterBasicType("double", ut::make_unique<UtPackSerializeFunctorC_Type<double>>());

   RegisterBasicType("string", ut::make_unique<UtPackSerializeFunctor_String>());

   // Add conversions between all integer types.  Use the preprocessor to make it easier to add types:
#define AC(A, B)                                                                  \
   RegisterConversion(#A,                                                         \
                      #B,                                                         \
                      ut::make_unique<UtPackSerializeFunctorCastConvert<A, B>>(), \
                      ut::make_unique<UtPackSerializeFunctorCastConvert<B, A>>())
#define AC_ALL(A)        \
   AC(A, std::int8_t);   \
   AC(A, std::int16_t);  \
   AC(A, std::int64_t);  \
   AC(A, unsigned int);  \
   AC(A, std::uint8_t);  \
   AC(A, std::uint16_t); \
   AC(A, std::uint64_t); \
   AC(A, float);         \
   AC(A, double)

   AC_ALL(int);
   AC_ALL(double);
   AC_ALL(float);
   AC_ALL(std::uint64_t);
   AC_ALL(std::uint16_t);
   AC_ALL(std::uint8_t);
   AC_ALL(unsigned int);
   AC_ALL(std::int64_t);
   AC_ALL(std::int16_t);
   AC_ALL(std::int8_t);
}

void UtPackSerializer::RegisterBasicType(const std::string& aName, std::unique_ptr<UtPackSerializeFunctor> aSerializeFunctor)
{
   TypeData typeData;
   typeData.mId         = (int)mTypes.size();
   typeData.mSerializer = std::move(aSerializeFunctor);
   mTypes.push_back(typeData);
   mTypeIds[aName] = typeData.mId;
}

UtPackSerializer::~UtPackSerializer()
{
   for (auto& it : mTypes)
   {
      if (it.mStructLayout != nullptr)
      {
         delete it.mStructLayout;
      }
   }
}

void UtPackSerializer::Initialize(UtPackSchema& aSchema)
{
   mSchema                           = aSchema;
   std::vector<std::string> allTypes = mSchema.GetTypeNames();
   for (const auto& type : allTypes)
   {
      UtPackType* typePtr = mSchema.FindType(type);
      if (typePtr)
      {
         UseType(typePtr);
      }
   }
   for (TypeData& td : mTypes)
   {
      if (td.mSerializer)
      {
         continue;
      }
      if (td.mStructLayout)
      {
         if (td.mTypePtr && td.mTypePtr->IsStruct())
         {
            UtPackStruct* structPtr = dynamic_cast<UtPackStruct*>(td.mTypePtr);
            td.mSerializer          = std::make_shared<UtPackSerializeFunctor_Struct>(structPtr, td.mStructLayout);
            if (structPtr->IsMessage())
            {
               MessageInfo* infoPtr = GetMessageInfo(structPtr->GetMessageId());
               if (infoPtr)
               {
                  infoPtr->mSerializeFunction = td.mSerializer;
               }
            }
         }
         else if (td.mTypePtr && td.mTypePtr->IsUnion())
         {
            UtPackUnion* unionPtr = dynamic_cast<UtPackUnion*>(td.mTypePtr);
            td.mSerializer        = std::make_shared<UtPackSerializeFunctor_Union>(unionPtr, td.mStructLayout);
         }
      }
      else if (td.mTypePtr && td.mTypePtr->IsList())
      {
         td.mSerializer = std::make_shared<UtPackSerializeFunctor_List>(dynamic_cast<UtPackListType*>(td.mTypePtr));
      }

      if (td.mTypePtr && td.mTypePtr->IsStruct())
      {
         UtPackStruct* structPtr = dynamic_cast<UtPackStruct*>(td.mTypePtr);
         if (structPtr->IsMessage() && !structPtr->IsVirtual())
         {
            MessageInfo* infoPtr = GetMessageInfo(structPtr->GetMessageId());
            if (infoPtr)
            {
               infoPtr->mSerializeFunction = td.mSerializer;
            }
         }
      }
   }

   for (TypeData& td : mTypes)
   {
      if (!td.mSerializer)
      {
         continue;
      }
      td.mSerializer->Initialize(*this);
   }
}

UtPackSerializer::TypeData* UtPackSerializer::UseType(UtPackType* aTypePtr)
{
   TypeData* dataPtr = FindTypeData(aTypePtr->GetTypePath());
   if (!dataPtr)
   {
      mTypes.emplace_back();
      dataPtr                           = &mTypes.back();
      mTypeIds[aTypePtr->GetTypePath()] = dataPtr->mId = (int)mTypes.size() - 1;
      dataPtr->mTypePtr                                = aTypePtr;
   }
   else
   {
      assert(dataPtr->mTypePtr == aTypePtr || dataPtr->mTypePtr == nullptr);
      dataPtr->mTypePtr = aTypePtr;
   }
   return dataPtr;
}

UtPackSerializer::TypeData* UtPackSerializer::FindLocalContainedListType(const std::string& aListTypeName)
{
   if (mListTypes.find(aListTypeName) != mListTypes.end())
   {
      return FindTypeData(mListTypes[aListTypeName]);
   }
   return nullptr;
}

std::string UtPackSerializer::FindLocalContainedListTypeName(std::string aListTypeName)
{
   if (mListTypes.find(aListTypeName) != mListTypes.end())
   {
      return mListTypes[aListTypeName];
   }
   return "";
}

UtPackSerializer::TypeData* UtPackSerializer::FindTypeData(const std::string& aTypeName)
{
   auto iter = mTypeIds.find(aTypeName);
   if (iter != mTypeIds.end())
   {
      return &mTypes[iter->second];
   }
   return nullptr;
}

void UtPackSerializer::RegisterListType(const std::string& aTypeName, const std::string& aContainedTypeName)
{
   mListTypes[aTypeName] = aContainedTypeName;
}

void UtPackSerializer::RegisterMessage(int aMessageId, MessageFactoryFnPtr aFactoryFunctionPtr)
{
   mMessageInfoMap[aMessageId].mMessageFactory = aFactoryFunctionPtr;
}

void UtPackSerializer::AddStructLayout(UtPackStructLayout* aLayoutPtr)
{
   TypeData* dataPtr = FindTypeData(aLayoutPtr->mStructName);
   if (!dataPtr)
   {
      mTypes.emplace_back();
      dataPtr                           = &mTypes.back();
      mTypeIds[aLayoutPtr->mStructName] = dataPtr->mId = (int)mTypes.size() - 1;
   }
   if (dataPtr)
   {
      assert(!dataPtr->mStructLayout);
      dataPtr->mStructLayout = aLayoutPtr;
   }
}

void UtPackSerializer::Print(std::ostream& aOstream, UtPackMessage& aMessage)
{
   UtPackPrinter printer(&aOstream);
   mMessageInfoMap[aMessage.GetMessageId()].mSerializeFunction->Print(&aMessage, printer);
}


void UtPackSerializeFunctor_List::Read(UtPackStream& bs, void* aDataPtr)
{
   UtPackListInterface* listPtr = static_cast<UtPackListInterface*>(aDataPtr);
   int                  size;
   bs.Read(&size, sizeof(int));
   listPtr->Resize(size);
   char* listDataPtr = (char*)listPtr->GetPtr();
   for (int i = 0; i < size; ++i)
   {
      char* listEntryPtr = (char*)(listDataPtr + i * mTypeSizeBytes);
      mTypeData->mSerializer->Read(bs, listEntryPtr);
   }
}

void UtPackSerializeFunctor_List::SkipRead(UtPackStream& bs)
{
   int size;
   bs.Read(&size, sizeof(int));
   for (int i = 0; i < size; ++i)
   {
      mTypeData->mSerializer->SkipRead(bs);
   }
}

void UtPackSerializeFunctor_List::Write(UtPackStream& bs, void* aDataPtr)
{
   UtPackListInterface* listPtr = static_cast<UtPackListInterface*>(aDataPtr);
   int                  size    = listPtr->GetSize();
   bs.Write(&size, sizeof(int));
   char* listDataPtr = (char*)listPtr->GetPtr();
   for (int i = 0; i < size; ++i)
   {
      char* listEntryPtr = (char*)(listDataPtr + i * mTypeSizeBytes);
      mTypeData->mSerializer->Write(bs, listEntryPtr);
   }
}

void UtPackSerializeFunctor_List::Print(void* aDataPtr, UtPackPrinter& aStream)
{
   aStream.Begin("List");

   UtPackListInterface* listPtr     = static_cast<UtPackListInterface*>(aDataPtr);
   int                  size        = listPtr->GetSize();
   char*                listDataPtr = (char*)listPtr->GetPtr();
   for (int i = 0; i < size; ++i)
   {
      std::stringstream ss;
      ss << "entry_" << i;
      aStream.Field(ss.str());
      char* listEntryPtr = (char*)(listDataPtr + i * mTypeSizeBytes);
      mTypeData->mSerializer->Print(listEntryPtr, aStream);
   }

   aStream.End();
}

void UtPackSerializeFunctor_List::Initialize(UtPackSerializer& aSerializer)
{
   // our local list may not match the schema definition,
   // grab the type actually contained in our local list
   mTypeData = aSerializer.FindLocalContainedListType(mListType->GetTypePath());
   if (!mTypeData)
   {
      throw UtPackException("Cannot determine type...");
   }
   std::string localContainedType = aSerializer.FindLocalContainedListTypeName(mListType->GetTypePath());
   // Find a serialize function for our local contained type and the schema contained type
   mSerializeFn =
      aSerializer.FindSerializeFunction(localContainedType, mListType->mContainedType.GetType()->GetTypePath());
   if (!mSerializeFn)
   {
      std::stringstream ss;
      ss << "Cannot serialize between " << mTypeData->mTypePtr->GetTypePath() << " and " << mListType->GetTypePath() << ".";
      throw UtPackException(ss.str());
   }

   mTypeData = aSerializer.FindTypeData(mListType->mContainedType.GetType()->GetTypePath());

   if (mTypeData->mStructLayout)
   {
      mTypeSizeBytes = mTypeData->mStructLayout->mStructSizeBytes;
      assert(mTypeSizeBytes > 0);
   }
   else if (mTypeData->mTypePtr && mTypeData->mTypePtr->IsBasicType())
   {
      UtPackBasicType* basicTypePtr = dynamic_cast<UtPackBasicType*>(mTypeData->mTypePtr);
      mTypeSizeBytes                = basicTypePtr->GetSize();
      if (mTypeSizeBytes == 0)
      {
         // needed for lists
         if (basicTypePtr->GetCPP_Type() == "std::string")
         {
            mTypeSizeBytes = sizeof(std::string);
         }
      }
      assert(mTypeSizeBytes > 0);
   }
   else
   {
      throw UtPackException("Cannot make a list of type " + mTypeData->mTypePtr->GetTypeName());
   }
}

UtPackSerializeFunctor_List::UtPackSerializeFunctor_List(UtPackListType* aType)
   : mTypeData(nullptr)
   , mListType(aType)
   , mSerializeFn(nullptr)
   , mTypeSizeBytes(0)
{
}

//
// UtPackMessage* UtPackMessageStream::Read()
// {
//    int msgId;
//    mDataStream.Read(&msgId, 4);
//    UtPackSerializer::MessageInfo* infoPtr = mSerializer->GetMessageInfo(msgId);
//    if (!infoPtr) return 0;
//    UtPackMessage* msg = (*infoPtr->mMessageFactory)();
//    infoPtr->mSerializeFunction->Read(mDataStream, msg);
// }

UtPackMessageStreamO::UtPackMessageStreamO(UtPackSerializer* aSerializer)
   : mSerializer(aSerializer)
{
}

void UtPackMessageStreamO::Flush() {}

void UtPackMessageStreamO::Write(UtPackMessage& aMessage)
{
   int                            msgId   = aMessage.GetMessageId();
   UtPackSerializer::MessageInfo* msgInfo = mSerializer->GetMessageInfo(msgId);
   if (!msgInfo)
   {
      return;
   }
   UtBuffer& buf = mMessageStream.mBuffer;
   buf.Reset();
   buf.SetPutPos(4); // leave space for packet length
   mMessageStream.Write(&msgId, 4);
   msgInfo->mSerializeFunction->Write(mMessageStream, &aMessage);
   size_t messageSize = buf.GetPutPos();
   buf.SetPutPos(0);
   buf.Put((int)messageSize - 4);
   {
      buf.SetGetPos(8);
      buf.SetPutPos(messageSize);
      msgInfo->mSerializeFunction->Read(mMessageStream, &aMessage);
   }

   WritePacket(buf.GetBuffer(), (int)messageSize);
}

void UtPackMessageStdStreamO::WritePacket(const void* aDataPtr, int aBytes)
{
   mStreamPtr->write((const char*)aDataPtr, aBytes);
}

void UtPackMessageStdStreamO::Flush()
{
   mStreamPtr->flush();
}

void UtPackMessageStream::WriteMessageToOStream(const UtPackMessage& aMessage,
                                                UtPackSerializer&    aSerializer,
                                                std::ostream&        aStream,
                                                UtPackStream&        aPackStream)
{
   aPackStream.mBuffer.Reset();
   aPackStream.mBuffer.SetPutPos(4); // leave space for packet length
   int                            msgId   = aMessage.GetMessageId();
   UtPackSerializer::MessageInfo* msgInfo = aSerializer.GetMessageInfo(msgId);
   if (msgInfo)
   {
      aPackStream.mBuffer.Put(msgId);
      msgInfo->mSerializeFunction->Write(aPackStream, (void*)&aMessage);
   }
}

UtPackMessageStream::ReadResult UtPackMessageStream::ReadMessageFromIStream(UtPackSerializer& aSerializer,
                                                                            std::istream&     aStream,
                                                                            UtBuffer&         aBuffer)
{
   int                                   streamStartPos = (int)aStream.tellg();
   UtPackMessageStream::PacketHeaderInfo hdr;
   aStream.read((char*)&hdr, sizeof(hdr));
   if (!aStream)
   {
      aStream.seekg(streamStartPos);
      return cEND_OF_STREAM;
   }
   UtPackSerializer::MessageInfo* msgInfo = aSerializer.GetMessageInfo(hdr.mPacketType);
   if (!msgInfo)
   {
      aStream.seekg(hdr.mSize, std::ios::cur);
      return cSKIP_MESSAGE;
   }

   aBuffer.CheckPutSpace(hdr.mSize);
   aStream.read(aBuffer.GetBuffer() + aBuffer.GetPutPos(), hdr.mSize);
   int bytesRead = (int)aStream.gcount();
   if (bytesRead < hdr.mSize)
   {
      aStream.seekg(streamStartPos);
      return cEND_OF_STREAM;
   }
   aBuffer.SetPutPos(aBuffer.GetPutPos() + hdr.mSize);
   return cREAD_MESSAGE;
}
