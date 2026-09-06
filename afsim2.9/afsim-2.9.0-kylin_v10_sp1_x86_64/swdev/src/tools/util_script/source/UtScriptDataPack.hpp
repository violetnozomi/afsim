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

#ifndef UTSCRIPTDATAPACK_HPP
#define UTSCRIPTDATAPACK_HPP

#include <string>

#include "UtMemory.hpp"
#include "UtScriptData.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "UtSpan.hpp"

namespace UtScriptDataPack_Detail
{
// The SFINAE expression is used to prevent runtime errors caused by implicit bool conversion
// when packing a T*, where T is a UtScriptAccessible and a definition for T is not available
template<typename T>
inline ut::enable_if_t<std::is_same<bool, T>::value> PackValue(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, T aValue)
{
   aObject.SetBool(aValue);
}
// Pack any integral type (except bool)
template<typename T>
inline ut::enable_if_t<std::is_integral<T>::value && !std::is_same<bool, T>::value> PackValue(UtScriptTypes* aTypesPtr,
                                                                                              ut::script::Data& aObject,
                                                                                              T                 aValue)
{
   aObject.SetInt(ut::cast_to_int(aValue));
}
inline void PackValue(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, const std::string& aValue)
{
   aObject.SetString(aValue);
}
inline void PackValue(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, double aValue)
{
   aObject.SetDouble(aValue);
}
inline void PackValue(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, UtScriptAccessible* aValue)
{
   UtScriptRef* refPtr = UtScriptRef::Ref(aValue, aTypesPtr);
   if (refPtr != nullptr)
   {
      aObject.SetPointer(refPtr);
   }
}
template<typename T>
inline void PackValue(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, const std::vector<T>& aValue)
{
   auto dataPtr = ut::make_unique<std::vector<ut::script::Data>>();
   for (const auto& val : aValue)
   {
      dataPtr->emplace_back(val);
   }
   aObject.SetPointer(new UtScriptRef(dataPtr.release(), aTypesPtr->GetClass("Array<Object>"), UtScriptRef::cMANAGE));
}
inline void PackValue(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, UtScriptRef* aValue)
{
   aObject.SetPointer(aValue);
}

template<typename T>
struct Packer
{
   static bool Go(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, T& aValue)
   {
      aObject = ut::script::Data(aValue);
      return true;
   }
};


template<>
struct Packer<const UtScriptAccessible*>
{
   static bool Go(UtScriptTypes* aTypesPtr, ut::script::Data& aObject, UtScriptAccessible* aValuePtr) { return true; }
};

// Defined is a specialized template class for each type needing unpacked
template<typename T, bool = std::is_integral<T>::value && !std::is_same<bool, T>::value>
struct Unpacker
{
   static bool Go(const ut::script::Data& aObject, T& aValue) { return false; }
};

// Unpack integral (non-bool) types
template<typename T>
struct Unpacker<T, true>
{
   static bool Go(const ut::script::Data& aObject, T& aValue)
   {
      switch (aObject.GetType())
      {
      case ut::script::Data::Type::cINT:
      case ut::script::Data::Type::cDOUBLE:
      case ut::script::Data::Type::cBOOL:
         aValue = ut::safe_cast<T>(ut::script::Data::ToInt(aObject));
         return true;
      default:
         aValue = 0;
         return false;
      }
   }
};

// Unpack bool
template<>
struct Unpacker<bool>
{
   static bool Go(const ut::script::Data& aObject, bool& aValue)
   {
      switch (aObject.GetType())
      {
      case ut::script::Data::Type::cINT:
      case ut::script::Data::Type::cDOUBLE:
      case ut::script::Data::Type::cBOOL:
         aValue = ut::script::Data::ToBool(aObject);
         return true;
      default:
         aValue = false;
         return false;
      }
   }
};

// Unpack string
template<>
struct Unpacker<std::string>
{
   static bool Go(const ut::script::Data& aObject, std::string& aValue)
   {
      if (aObject.GetType() == ut::script::Data::Type::cSTRING)
      {
         aValue = aObject.GetString();
         return true;
      }
      return false;
   }
};

// Unpack double
template<>
struct Unpacker<double>
{
   static bool Go(const ut::script::Data& aObject, double& aValue)
   {
      switch (aObject.GetType())
      {
      case ut::script::Data::Type::cINT:
      case ut::script::Data::Type::cDOUBLE:
         aValue = ut::script::Data::ToDouble(aObject);
         return true;
      default:
         aValue = 0.0;
         return false;
      }
   }
};

// Unpack Array<Object> -> std::vector
template<typename T>
struct Unpacker<std::vector<T>, false>
{
   static bool Go(const ut::script::Data& aObject, std::vector<T>& aValue)
   {
      if (aObject.GetType() == ut::script::Data::Type::cPOINTER)
      {
         bool         ok      = true;
         UtScriptRef* refPtr  = aObject.GetPointer();
         auto*        dataPtr = refPtr->GetAppObject<std::vector<ut::script::Data>>();
         aValue.resize(dataPtr->size());
         for (size_t i = 0; i < dataPtr->size(); ++i)
         {
            ok &= Unpacker<T>::Go((*dataPtr)[i], aValue[i]);
         }
         return ok;
      }
      return false;
   }
};

// Unpack Application object
template<typename T>
struct Unpacker<T*, false>
{
   static bool Go(const ut::script::Data& aObject, T*& aValue)
   {
      bool               success(false);
      const UtScriptRef* refPtr = aObject.GetPointer();
      if (refPtr != nullptr)
      {
         // Can't currently test script types to ensure a match
         aValue  = refPtr->GetAppObject<T>();
         success = true;
      }
      else
      {
         aValue = nullptr;
      }
      return success;
   }
};

// Unpack UtScriptData
template<>
struct Unpacker<ut::script::Data>
{
   static bool Go(const ut::script::Data& aObject, ut::script::Data& aValue)
   {
      aValue = aObject;
      return true;
   }
};

// Unpack UtScriptRef
template<>
struct Unpacker<UtScriptRef>
{
   static bool Go(const ut::script::Data& aObject, UtScriptRef& aValue)
   {
      if (!aObject.GetPointer())
      {
         return false;
      }
      aValue = *aObject.GetPointer();
      return true;
   }
};
} // namespace UtScriptDataPack_Detail

//! 'Unpacks' a script object to the c++ equivalent.  The caller must ensure
//! that aValue matches the script type.
template<typename T>
inline bool UtScriptDataUnpack(const ut::script::Data& aObject, T& aValue)
{
   return UtScriptDataPack_Detail::Unpacker<T>::Go(aObject, aValue);
}

//! 'Unpacks' a std::vector of script objects to the c++ equivalent.  The caller must ensure
//! that the output parameters match the script object types.
template<class A1>
inline bool UtScriptDataUnpack(ut::span<const ut::script::Data> aObjects, A1& aValue)
{
   bool ok = true;
   ok &= UtScriptDataUnpack(aObjects[0], aValue);
   return ok;
}

//! 'Unpacks' a std::vector of script objects to the c++ equivalent.  The caller must ensure
//! that the output parameters match the script object types.
template<typename A1, class... Args>
inline bool UtScriptDataUnpack(ut::span<const ut::script::Data> aObjects, A1& aValue1, Args&&... aValues)
{
   bool ok = true;
   ok &= UtScriptDataUnpack(aObjects[0], aValue1);
   ok &= UtScriptDataUnpack(aObjects.subspan(1), std::forward<Args>(aValues)...);
   return ok;
}

struct UtScriptDataPacker
{
   UtScriptDataPacker(UtScriptTypes* aTypesPtr)
      : mTypesPtr(aTypesPtr)
   {
   }
   template<typename T, typename = ut::enable_if_t<!std::is_pointer<T>::value && !std::is_base_of<UtScriptAccessible, T>::value>>
   UtScriptDataPacker& operator<<(const T& aValue)
   {
      using namespace UtScriptDataPack_Detail;
      ut::script::Data data;
      PackValue(mTypesPtr, data, const_cast<T&>(aValue));
      mDataList.push_back(data);
      return *this;
   }
   UtScriptDataPacker& operator<<(const UtScriptAccessible& aValue)
   {
      using namespace UtScriptDataPack_Detail;
      ut::script::Data data;
      PackValue(mTypesPtr, data, const_cast<UtScriptAccessible*>(&aValue));
      mDataList.push_back(data);
      return *this;
   }
   template<typename T>
   UtScriptDataPacker& operator<<(const T* aValue)
   {
      using namespace UtScriptDataPack_Detail;
      ut::script::Data data;
      PackValue(mTypesPtr, data, const_cast<T*>(aValue));
      mDataList.push_back(data);
      return *this;
   }
   UtScriptDataPacker& Push(const void*                aObjectPtr,
                            const char*                aClassNamePtr,
                            UtScriptRef::MemManagement aMemory = UtScriptRef::cDONT_MANAGE)
   {
      UtScriptClass* classPtr = mTypesPtr->GetClassFromCharArray(aClassNamePtr);
      if (classPtr != nullptr)
      {
         (*this) << new UtScriptRef((void*)aObjectPtr, classPtr, aMemory);
      }
      else
      {
         mDataList.emplace_back();
      }
      return *this;
   }
                        operator ut::script::DataList&() { return mDataList; }
                        operator const ut::script::DataList&() const { return mDataList; }
   ut::script::DataList mDataList;
   UtScriptTypes*       mTypesPtr;
};

namespace detail
{
//! Base case for recursion, zero arguments (no-op)
inline void Pack(UtScriptDataPacker& aScriptArgs) {}

template<typename A1, typename... Args>
inline void Pack(UtScriptDataPacker& aScriptArgs, A1&& aArg1, Args&&... args)
{
   aScriptArgs << aArg1;
   Pack(aScriptArgs, std::forward<Args>(args)...);
}
} // namespace detail

//! Helper function to insert elements from a parameter pack, in order, into a UtScriptDataPacker
template<typename... Args>
inline void UtScriptDataPack(UtScriptDataPacker& aScriptArgs, Args&&... args)
{
   detail::Pack(aScriptArgs, std::forward<Args>(args)...);
}

#endif
