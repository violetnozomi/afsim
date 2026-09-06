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

/******************************************************************************
//  This file defines the underlying mechanism for serialization.  To be
// serialized, a type needs to have one of the following:
//   * Contain a template method to serialize the type in the form:
//       template <typename AR> Serialize(AR& aAr) { ... }
//   * Have an non-memeber function defined to serialize the type in the
//       as follows:
//        // TheType must _not_ be const
//        template <typename AR> Serialize(AR& aAR, TheType& aVal) { ... }
//   * Have two non-memeber functions defined to save/load the type as follows:
//        template <typename AR> Load(AR& aAr, TheType& aVal) { ... }
//        template <typename AR> Save(AR& aAr, TheType& aVal) { ... }
//   * OR Belong to a set of types already defined including:
//     -basic types (int,float,double,...)
//     -std:: types (string,vector,list,map,set,pair,shared_ptr)
//        -std containers must be parameterized with serializable types
//
//  Operator &
//   To initiate serialization on a value, archive classes support the '&'
//    operator.  If the value has one of the defined serialization functions,
//    it will be executed.  If there is no way to serialize the type, it
//    will create a compiler error.
//
//  Pointers
//   When a pointer is serialized, it is a special case.  If the pointer is
//    null, it is serialized as a null pointer.  Otherwise, the pointer is
//    dereferenced and its value serialized.  When a pointer is serialized
//    from an input archive, a object is default constructed with new, and
//    then serialized.
//    If a type cannot be default-constructed, a template function may be
//    overridden:
//      template <class AR, typename T>
//      inline T* CreateType(AR& aAr) {}
//
// !Warning! This header file should be included after any user-defined
//           Serialize(), Save(), or Load() overrides.  Otherwise compiler
//           errors will result on some systems.
******************************************************************************/
#include "PakSerializeFwd.hpp"
#include "PakSerializeTraits.hpp"
#ifndef PAKSERIALIZE_HPP
#define PAKSERIALIZE_HPP
#include "packetio_export.h"

#include "PakSerialize_Detail.hpp"

#define PAK_SERIALIZE_DEFINED

/*
   This file defines how variables are serialized.

   Values are passed through in a series of steps:
   operator&
      - Provides front end interface to serialization
   SerializeToArchiveStep2
      - Distinguishes basic types from others
      if basic type(int, float, etc..), do AR.Serialize(T)
      else continue
   Serialize(AR, T&)
      - An interface to allow overloaded functions to serialize user's types
      if not overridden:
         if AR is input, Load() else Save()
   Load(AR, T&) or Save(AR, const T&)
      - An interface to allow overloaded functions to load or save user's types
      If not overridden, continue
   T.Serialize(AR)
      - Allows objects to define a Serialize() method
*/

namespace PakSerialization
{
template<typename AR, typename T>
inline void Serialize(AR& aAr, T& aValue);
template<class AR, typename T>
inline void Serialize(AR& aAr, T*& aPtr);

template<typename T>
inline void Load(PakI& aAr, T& aValue);
template<typename T>
inline void Save(PakO& aAr, T& aValue);

//! This may be overridden for types that do not have default constructors
template<typename T>
inline T* CreateType()
{
   return new T;
}

inline PACKETIO_EXPORT void Serialize(PakI& aAr, PakSerializeRawData& aData)
{
   aAr.GetBuffer()->GetRaw((char*)aData.mData, aData.mBytes);
}
inline PACKETIO_EXPORT void Serialize(PakO& aAr, PakSerializeRawData& aData)
{
   aAr.GetBuffer()->PutRaw((char*)aData.mData, aData.mBytes);
}
inline PACKETIO_EXPORT void Serialize(PakI& aAr, PakSerializeFixedString& aData)
{
   aAr.SerializeString(*aData.mStringPtr, aData.mMaxSize);
}
inline PACKETIO_EXPORT void Serialize(PakO& aAr, PakSerializeFixedString& aData)
{
   aAr.SerializeString(*aData.mStringPtr, aData.mMaxSize);
}
} // End Namespace PakSerialization

template<typename T>
inline PakI& operator&(PakI& aAr, T& aValue);
template<typename T>
inline PakI& operator>>(PakI& aAr, T& aValue);
template<typename T>
inline PakO& operator&(PakO& aAr, T& aValue);
template<typename T>
inline PakO& operator&(PakO& aAr, const T& aValue);
template<typename T>
inline PakO& operator<<(PakO& aAr, T& aValue);
template<typename T>
inline PakO& operator<<(PakO& aAr, const T& aValue);

// Define serialization for some basic types and containers

namespace PakSerialization
{
// Serialize a pointer
template<typename T>
inline void Load(PakI& aAr, PakSerialization::PakSerializePtr<T>& aPtr)
{
   bool isValid;
   aAr& isValid;
   if (isValid)
   {
      typedef typename Non_Const<T>::Type NC_Type;
      NC_Type*                            typePtr = CreateType<NC_Type>();
      aAr&*                               typePtr;
      *aPtr.mPtr = typePtr;
   }
   else
   {
      *aPtr.mPtr = nullptr;
   }
}

template<typename T>
inline void Save(PakO& aAr, PakSerialization::PakSerializePtr<T>& aPtr)
{
   bool isValid = (*aPtr.mPtr != nullptr);
   aAr& isValid;
   if (isValid)
   {
      aAr&(typename Non_Const<T>::Type&)**aPtr.mPtr;
   }
}
} // End Namespace PakSerialization

#include "PakSerializeTypes.hpp"

namespace PakSerialization
{
template<class AR, typename T>
void Serialize(AR& aAr, PakSerializeArray<T>& aArray)
{
   for (int i = 0; i < aArray.mSize; ++i)
   {
      aAr& aArray.mArrayPtr[i];
   }
}

template<typename T>
inline void Load(PakI& aAr, PakSerialization::PakSerializeEnum<T>& aEnum)
{
   int32_t value(0);
   aAr&    value;
   *aEnum.mEnumPtr = static_cast<T>(value);
}

template<typename T>
inline void Save(PakO& aAr, PakSerialization::PakSerializeEnum<T>& aEnum)
{
   int32_t value = static_cast<int32_t>(*aEnum.mEnumPtr);
   aAr&    value;
}
template<class AR>
inline void Serialize(AR& aAr, PakSerializeIgnoreData& aIgnore)
{
   aAr.IgnoreBytes(aIgnore.mBytes);
}
} // namespace PakSerialization

#endif
