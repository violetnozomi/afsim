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

#ifndef PAKSERIALIZEFWD_HPP
#define PAKSERIALIZEFWD_HPP

// This file defines some basic stuff used by Serialize() code
// including some intermediate types and helper functions used
// to serialize certain data types
#include <cstdint>
#include <string>

#include "PakI.hpp"
#include "PakIntTypes.hpp"
#include "PakO.hpp"
namespace PakSerialization
{
template<typename T>
class PakSerializePtr
{
public:
   PakSerializePtr(T*& aPtr)
      : mPtr(&aPtr)
   {
   }
   T** mPtr;
};

template<typename T>
class PakSerializePolyPtr
{
public:
   PakSerializePolyPtr(T*& aPtr)
      : mPtr(&aPtr)
   {
   }
   T** mPtr;
};

template<typename T>
class PakSerializeArray
{
public:
   PakSerializeArray(T* aArrayPtr, int aSize)
      : mArrayPtr(aArrayPtr)
      , mSize(aSize)
   {
   }
   T*  mArrayPtr;
   int mSize;
};

template<typename T>
class PakSerializeEnum
{
public:
   PakSerializeEnum(T& aEnum)
      : mEnumPtr(&aEnum)
   {
   }
   T* mEnumPtr;
};

class PakSerializeIgnoreData
{
public:
   PakSerializeIgnoreData(int aBytes)
      : mBytes(aBytes)
   {
   }
   int mBytes;
};

class PakSerializeRawData
{
public:
   PakSerializeRawData(void* aData, int aBytes)
      : mData(aData)
      , mBytes(aBytes)
   {
   }
   void* mData;
   int   mBytes;
};

class PakSerializeFixedString
{
public:
   PakSerializeFixedString(std::string& aString, int aMaxSize)
      : mStringPtr(&aString)
      , mMaxSize(aMaxSize)
   {
   }
   std::string* mStringPtr;
   int          mMaxSize;
};

// No longer necessary, the & operator accepts pointers
template<typename T>
inline const PakSerializePtr<T> Pointer(T*& aPtr)
{
   return PakSerializePtr<T>(aPtr);
}

//! Specifies that aPtr is a polymorphic type.  The correct type will be serialized
//! if PakTypeDictionary::RegisterType() has been called.
template<typename T>
inline const PakSerializePolyPtr<T> Polymorphic(T*& aPtr)
{
   return PakSerializePolyPtr<T>(aPtr);
}

//! Specifies an array to be serialized
template<typename T>
inline const PakSerializeArray<T> Array(T* aPtr, int aSize)
{
   return PakSerializeArray<T>(aPtr, aSize);
}

//! Serializes an enum to the archive
template<typename T>
inline const PakSerializeEnum<T> Enum(T& aEnum)
{
   return PakSerializeEnum<T>(aEnum);
}

//! Specifies to skip a number of bytes when serializing to the archive
inline const PakSerializeIgnoreData IgnoreBytes(int aBytes)
{
   return PakSerializeIgnoreData(aBytes);
}

//! Specifies a number of raw bytes to serialize with archive
inline const PakSerializeRawData RawData(void* aData, int aBytes)
{
   return PakSerializeRawData(aData, aBytes);
}

//! Specifies a number of raw bytes to serialize with archive
inline const PakSerializeRawData RawData(const void* aData, int aBytes)
{
   return PakSerializeRawData(const_cast<void*>(aData), aBytes);
}

//! Serializes a string with a fixed length to an archive
//! Avoiding the cost of storing the length of the string
inline const PakSerializeFixedString FixedString(std::string& aString, int aMaxSize)
{
   return PakSerializeFixedString(aString, aMaxSize);
}

template<typename AR, typename T, bool IS_OUTPUT>
struct PakSplitSerialize
{
   static void Go(AR& aAr, T& aValue) { aValue.Load(aAr); }
};

template<typename AR, typename T>
struct PakSplitSerialize<AR, T, true>
{
   static void Go(AR& aAr, T& aValue) { aValue.Save(aAr); }
};
} // namespace PakSerialization

#define PAK_SERIALIZE_SPLIT()                                                     \
   template<typename T>                                                           \
   void Serialize(AR& aAr)                                                        \
   {                                                                              \
      PakSerialization::PakSplitSerialize<AR, T, AR::cIS_OUTPUT>::Go(aAr, *this); \
   }

#endif
