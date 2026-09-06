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

#ifndef UTPACK_HPP
#define UTPACK_HPP

#include <cassert>
#include <exception>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "UtBuffer.hpp"
#include "UtPackMessage.hpp"

// UtPack    -
//    This code uses a schema to generate C++ classes for use with serialization.
// 1. Create a schema file with the data types you need.
// 2. Run pack_to_cpp to generate C++ header files containing class definitions.
// 3. Use generated classes to pack messages, and write them to a file or socket
//       with UtPackMessageStreamO.
// 4. Use UtPackMessageStdStreamI to reconstitute the messages from the binary stream
//
// Benefits
// - stream objects in a self-describing format
// - compact streaming format
//    -minimal type information
//    -unset fields aren't transmitted
// - messages can be modified while retaining backwards compatibility
//
// Compatibility
// Limited changes allow for backwards compatibility of messages.  These kinds of
// changes are allowed
// - add new message types
// - add new optional fields to existing data types
// - existing fields' data types may be modified in a limited way:
//    - numeric types are interchangeable:  double <-> int8 <-> int32.   (one side will lose precision)
//
// - required fields may not be added or removed, therefore required should be used cautiously

#include "WsfUtExport.hpp"

class UtPackType;
class UtPackSerializer;
#include "UtPackSchema.hpp"
#include "UtPackStream.hpp"

//! Contains information about the byte layout of the struct generated from pack_to_cpp
//! basically reflection data built from UtPackReflector
class WSF_UT_EXPORT UtPackStructLayout
{
public:
   struct Member
   {
      int            mOffset;
      unsigned short mMemberSize;
      short          mOptionalIndex;
      const char*    mName;
      const char*    mTypeName;
   };

   UtPackStructLayout()
      : mOptionalFlagsOffset(0)
      , mOptionalFlagsBytes(0)
      , mStructName()
      , mStructSizeBytes(0)
      , mSetFieldTypeFuncPtr()
   {
   }

   const Member* FindMember(const std::string& aMemberName) const
   {
      for (const auto& mem : mMembers)
      {
         if (aMemberName == mem.mName)
         {
            return &mem;
         }
      }
      return nullptr;
   }

   int GetMemberIndex(const std::string& aMemberName) const
   {
      for (size_t i = 0; i < mMembers.size(); ++i)
      {
         if (aMemberName == mMembers[i].mName)
         {
            return (int)i;
         }
      }
      return -1;
   }

   int GetOptionalFlagsBytes() const { return (int)mOptionalFlagsBytes; }

   int                 mOptionalFlagsOffset;
   unsigned char       mOptionalFlagsBytes;
   const char*         mStructName;
   std::vector<Member> mMembers;
   int                 mStructSizeBytes;
   using Union_SetFieldTypeFuncPtr = void (*)(void* aUnion, int aFieldType);
   Union_SetFieldTypeFuncPtr mSetFieldTypeFuncPtr;
};

class UtPackSerializerTypeData
{
public:
   UtPackSerializerTypeData()
      : mId(0)
      , mTypePtr(nullptr)
      , mSerializer(nullptr)
      , mStructLayout(nullptr)
   {
   }

   int                                     mId;
   UtPackType*                             mTypePtr;
   std::shared_ptr<UtPackSerializeFunctor> mSerializer;
   UtPackStructLayout*                     mStructLayout;
};

class WSF_UT_EXPORT UtPackSerializer
{
public:
   static const int cMAXIMUM_OPTIONAL_BITS = 128;
   using MessageFactoryFnPtr               = UtPackMessage* (*)();
   using TypeData                          = UtPackSerializerTypeData;
   friend class UtPackReflector;

   UtPackSerializer() = default;
   ~UtPackSerializer();
   void      RegisterBuiltinTypes();
   void      RegisterBasicType(const std::string& aName, std::unique_ptr<UtPackSerializeFunctor> aSerializeFunctor);
   void      RegisterListType(const std::string& aTypeName, const std::string& aContainedTypeName);
   void      RegisterConversion(const std::string&                      aType1,
                                const std::string&                      aType2,
                                std::unique_ptr<UtPackSerializeFunctor> aSerializeToType1,
                                std::unique_ptr<UtPackSerializeFunctor> aSerializeToType2);
   void      RegisterMessage(int aMessageId, MessageFactoryFnPtr aFactoryFunctionPtr);
   TypeData* FindTypeData(const std::string& aTypeName);
   std::shared_ptr<UtPackSerializeFunctor> FindSerializeFunction(const std::string& aTypeInMemory,
                                                                 const std::string& aSerializedType);
   void                                    Initialize(UtPackSchema& aSchema);
   TypeData*                               FindLocalContainedListType(const std::string& aListTypeName);
   std::string                             FindLocalContainedListTypeName(std::string aListTypeName);

   struct MessageInfo
   {
      MessageInfo()
         : mMessageFactory()
         , mSerializeFunction()
      {
      }
      MessageFactoryFnPtr                     mMessageFactory;
      std::shared_ptr<UtPackSerializeFunctor> mSerializeFunction;
   };

   const MessageInfo* GetMessageInfo(int aMessageId) const
   {
      auto it = mMessageInfoMap.find(aMessageId);
      if (it != mMessageInfoMap.end())
      {
         return &(it->second);
      }
      return nullptr;
   }

   MessageInfo* GetMessageInfo(int aMessageId)
   {
      auto it = mMessageInfoMap.find(aMessageId);
      if (it != mMessageInfoMap.end())
      {
         return &(it->second);
      }
      return nullptr;
   }

   void Print(std::ostream& aOstream, UtPackMessage& aMessage);

private:
   TypeData* UseType(UtPackType* aTypePtr);
   void      AddStructLayout(UtPackStructLayout* aLayoutPtr);
   // FindTypeData(const std::string& aTypeName)

   // int -> type data
   using TypeIdMap = std::map<std::string, int>;
   std::vector<TypeData> mTypes;
   TypeIdMap             mTypeIds;

   using TypeConversionMap = std::map<std::string, std::map<std::string, std::shared_ptr<UtPackSerializeFunctor>>>;
   TypeConversionMap                  mTypeConversions;
   std::map<std::string, std::string> mListTypes;
   std::map<int, MessageInfo>         mMessageInfoMap;

   UtPackSchema mSchema;
};

#endif // UTPACK_HPP
