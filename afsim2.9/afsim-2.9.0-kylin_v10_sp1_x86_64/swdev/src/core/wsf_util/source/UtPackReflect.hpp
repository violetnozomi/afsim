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

#ifndef UTPACKREFLECT_HPP
#define UTPACKREFLECT_HPP

#include "UtCast.hpp"
#include "UtPack.hpp"
class UtPackType;
class UtPackTypeReference;
class UtPackField;
class UtPackSerializeFunctor;

// See UtPack.hpp
// This file contains the classes related to registering UtPack generated code
// with a UtPackSerializer

class WSF_UT_EXPORT UtPackReflector
{
public:
   friend class UtPackSerializer;
   UtPackReflector(UtPackSerializer& aSerializer);
   ~UtPackReflector() = default;

   template<typename T>
   void BeginStruct(const char* aName, T& aInstance)
   {
      assert(mCurrentInstancePtr == nullptr);
      mCurrentInstancePtr                 = (char*)&aInstance;
      mCurrentLayoutPtr                   = new UtPackStructLayout;
      mCurrentLayoutPtr->mStructName      = aName;
      mCurrentLayoutPtr->mStructSizeBytes = sizeof(T);
   }

   template<typename T>
   struct CallUnionSetFieldType
   {
      static void Call(void* aUnionPtr, int aFieldType)
      {
         ((T*)aUnionPtr)->SetFieldType((typename T::FieldType)aFieldType);
      }
   };

   template<typename T>
   void BeginUnion(const char* aName, T& aInstance)
   {
      BeginStruct(aName, aInstance);
      mCurrentLayoutPtr->mSetFieldTypeFuncPtr = &CallUnionSetFieldType<T>::Call;
   }

   void End();

   template<typename T>
   void OptionalFlags(T& aMember)
   {
      mCurrentLayoutPtr->mOptionalFlagsOffset = (int)((char*)&aMember - mCurrentInstancePtr);
      mCurrentLayoutPtr->mOptionalFlagsBytes  = sizeof(T);
   }

   template<typename T>
   void OptionalMember(const char* aTypeName, const char* aMemberName, T& aMember, int aOptionIndex)
   {
      assert(mCurrentInstancePtr);
      AddMember(aTypeName,
                aMemberName,
                reinterpret_cast<char*>(&aMember),
                ut::safe_cast<unsigned short, size_t>(sizeof(T)),
                aOptionIndex);
   }

   template<typename T>
   void RequiredMember(const char* aTypeName, const char* aMemberName, T& aMember)
   {
      assert(mCurrentInstancePtr);
      AddMember(aTypeName,
                aMemberName,
                reinterpret_cast<char*>(&aMember),
                ut::safe_cast<unsigned short, size_t>(sizeof(T)),
                -1);
   }
   template<typename T>
   struct MessageFactory
   {
      static UtPackMessage* Create() { return new T; }
   };
   template<typename T>
   void RegisterMessage(T& aMessage)
   {
      mSerializerPtr->RegisterMessage(aMessage.GetMessageId(), MessageFactory<T>::Create);
   }

private:
   void AddMember(const char* aTypeName, const char* aMemberName, char* aMemberPtr, unsigned short aMemberSize, int aOptionIndex);

   UtPackStructLayout* mCurrentLayoutPtr;
   char*               mCurrentInstancePtr;
   UtPackSerializer*   mSerializerPtr;
   int                 mNextOptionIndex;
};

#endif
