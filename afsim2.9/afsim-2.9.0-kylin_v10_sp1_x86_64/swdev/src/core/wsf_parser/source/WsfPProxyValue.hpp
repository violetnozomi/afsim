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

#ifndef WSFPPROXYVALUE_HPP
#define WSFPPROXYVALUE_HPP

#include <string>

#include "WsfPProxyBasicValues.hpp"
#include "WsfPProxyHash.hpp"
#include "WsfPProxyKey.hpp"
#include "WsfParseType.hpp"
class WsfPProxyType;

class WsfPProxyObjectMap;
class WsfPProxyList;
class WsfPProxyPath;

// This is a helper class for holding and operating on proxy values
// Notice: Instances of WsfPProxyValue act as pointers to proxy values when using the copy constructor or assignment operator
class WSF_PARSER_EXPORT WsfPProxyValue
{
public:
   WsfPProxyValue()
      : mTypePtr(nullptr)
      , mDataPtr(nullptr)
   {
   }

   WsfPProxyValue(void* aDataPtr, WsfPProxyType const* aTypePtr)
      : mTypePtr(aTypePtr)
      , mDataPtr(aDataPtr)
   {
   }

   WsfPProxyValue(const WsfPProxyValue&) = default;
   WsfPProxyValue(WsfPProxyValue&&)      = default;
   WsfPProxyValue& operator=(const WsfPProxyValue&) = default;
   WsfPProxyValue& operator=(WsfPProxyValue&&) = default;
   virtual ~WsfPProxyValue()                   = default;

   static WsfPProxyValue ConstructNew(const WsfPProxyType* aType);

   bool IsBasicType() const;

   bool IsStruct() const;

   bool                 IsValid() const { return mTypePtr != nullptr; }
   void*                GetDataPtr() const { return mDataPtr; }
   const WsfPProxyType* GetType() const { return mTypePtr; }
   WsfPProxyValue       GetAttr(const WsfPProxyKey& aEntry) const
   {
      if (aEntry.IsString())
      {
         return GetAttr(aEntry.GetMapKey());
      }
      return GetAttr(aEntry.GetIndex());
   }
   WsfPProxyValue GetAttr(const std::string& aName) const;
   WsfPProxyValue GetAttr(size_t aIndex) const;

   //! Set attr, except the old value is saved
   bool SwapAttr(const WsfPProxyKey& aEntry, WsfPProxyValue& aValue) const;

   WsfPProxyValue         GetAtIndex(size_t aIndex) const;
   size_t                 GetAttrCount() const;
   std::string            NameAtIndex(size_t aIndex) const;
   virtual size_t         GetMemberIndex(const std::string& aName) const;
   virtual WsfPProxyType* GetMemberType(const std::string& aName) const;
   virtual WsfPProxyType* GetMemberType(size_t aMemberIndex) const;

   WsfPProxyList*      GetList() const;
   WsfPProxyObjectMap* GetObjectMap() const;
   bool                Copy(WsfPProxyValue aSrc, int flags = WsfProxy::cCOPY_ALL) const;
   void                SetValue(const std::string& aText);
   void                SetBoolValue(bool value);
   std::string         ValueToString() const;
   // Returns a new copy of this value.  Warning: These values are NOT reference counted, and extra care must be
   // taken to ensure the value is deleted to avoid a memory leak;
   WsfPProxyValue Copy(int flags = WsfProxy::cCOPY_ALL) const;
   void           SetUnset();
   void           ClearUnset();
   bool           IsUnset() const;
   bool           IsInherited() const;
   void           SetInherited(bool aIsInherited);

   //! Returns the proxy path to the value this was originally copied from
   //! This is only available on struct instances, and only when an inheritance type relationship exists
   WsfPProxyPath* GetBasePath() const;


   operator bool() const { return IsValid(); }
   operator bool() { return IsValid(); }
   // Returns true if both proxy values point to the same underlying value.  Does not test for equality of value.
   bool operator==(const WsfPProxyValue& aRhs) const { return mDataPtr == aRhs.mDataPtr; }
   bool operator<(const WsfPProxyValue& aRhs) const { return mDataPtr < aRhs.mDataPtr; }
   void Delete();
   void Swap(WsfPProxyValue& aRhs) noexcept
   {
      std::swap(mTypePtr, aRhs.mTypePtr);
      std::swap(mDataPtr, aRhs.mDataPtr);
   }
   WsfPProxyValue operator[](const char* aAttrName) { return GetAttr(aAttrName); }
   WsfPProxyValue operator[](const std::string& aAttrName) { return GetAttr(aAttrName); }
   WsfPProxyValue operator[](size_t aIndex) { return GetAtIndex(aIndex); }
   WsfPProxyHash  Hash();

   WsfPProxyValue Lookup(const WsfPProxyPath& aPath, size_t aElements = 0x7fffffff) const;
   WsfPProxyValue Lookup(const WsfParseTypePath& aPath) const;

   WsfPProxyValue& operator=(const std::string& aStringValue);
   WsfPProxyValue& operator=(const char* aStringPtr);
   WsfPProxyValue& operator=(bool aBoolValue);
#define ADD_TYPE_OPERATOR(VALUE_TYPE)            \
                   operator VALUE_TYPE*() const; \
   WsfPProxyValue& operator=(const VALUE_TYPE& aVal);
   ADD_TYPE_OPERATOR(WsfProxy::String)
   ADD_TYPE_OPERATOR(WsfProxy::Position)
   ADD_TYPE_OPERATOR(WsfProxy::Length2)
   ADD_TYPE_OPERATOR(WsfProxy::Bool)
   ADD_TYPE_OPERATOR(WsfProxy::Double)
   ADD_TYPE_OPERATOR(WsfProxy::Int)
   ADD_TYPE_OPERATOR(WsfProxy::Latitude)
   ADD_TYPE_OPERATOR(WsfProxy::Longitude)
   ADD_TYPE_OPERATOR(WsfProxy::Length)
   ADD_TYPE_OPERATOR(WsfProxy::Time)
   ADD_TYPE_OPERATOR(WsfProxy::Speed)
   ADD_TYPE_OPERATOR(WsfProxy::DataSize)
   ADD_TYPE_OPERATOR(WsfProxy::Power)
   ADD_TYPE_OPERATOR(WsfProxy::PowerDB)
   ADD_TYPE_OPERATOR(WsfProxy::SolidAngle)
   ADD_TYPE_OPERATOR(WsfProxy::Mass)
   ADD_TYPE_OPERATOR(WsfProxy::Force)
   ADD_TYPE_OPERATOR(WsfProxy::Torque)
   ADD_TYPE_OPERATOR(WsfProxy::Area)
   ADD_TYPE_OPERATOR(WsfProxy::AreaDB)
   ADD_TYPE_OPERATOR(WsfProxy::Volume)
   ADD_TYPE_OPERATOR(WsfProxy::Time2)
   ADD_TYPE_OPERATOR(WsfProxy::Acceleration)
   ADD_TYPE_OPERATOR(WsfProxy::Frequency)
   ADD_TYPE_OPERATOR(WsfProxy::Angle)
   ADD_TYPE_OPERATOR(WsfProxy::AngularRate)
   ADD_TYPE_OPERATOR(WsfProxy::AngularAcceleration)
   ADD_TYPE_OPERATOR(WsfProxy::DataRate)
   ADD_TYPE_OPERATOR(WsfProxy::MassDensity)
   ADD_TYPE_OPERATOR(WsfProxy::MassTransfer)
   ADD_TYPE_OPERATOR(WsfProxy::Energy)
   ADD_TYPE_OPERATOR(WsfProxy::Fluence)
   ADD_TYPE_OPERATOR(WsfProxy::Irradiance)
   ADD_TYPE_OPERATOR(WsfProxy::Ratio)
   ADD_TYPE_OPERATOR(WsfProxy::NoisePressure)
   ADD_TYPE_OPERATOR(WsfProxy::Pressure)
   ADD_TYPE_OPERATOR(WsfProxy::Temperature)
   ADD_TYPE_OPERATOR(WsfProxy::SpecificRange)
   ADD_TYPE_OPERATOR(WsfProxy::AngularInertia)
#undef ADD_TYPE_OPERATOR

   bool GetIntegerValue(int& aValue);

   void SetIntegerValue(int aValue);


   bool IsBoolValue();
   bool GetBoolValue();

   void DebugOut(std::ostream& aStream, unsigned int aDepth = 0) const;

   //       template <typename VALUE_TYPE>
   //       inline VALUE_TYPE* proxy_cast(const WsfPProxyValue& aValue)
   //       {
   //          WsfPProxyType* typePtr = aValue.GetType();
   //          if (typePtr && typePtr->mTypeStoredKind == VALUE_TYPE::cPROXY_TYPE_ID)
   //          {
   //             return (VALUE_TYPE*)aValue.GetDataPtr();
   //          }
   //          return 0;
   //       }

   bool IsAttributeUsable(const std::string& aAttribute) const noexcept;

protected:
   template<typename T>
   T* CastToValue() const;
   // Pointer to the value's type
   const WsfPProxyType* mTypePtr;
   // Raw pointer to the data
   void* mDataPtr;
};

namespace std
{
template<>
inline void swap(WsfPProxyValue& a, WsfPProxyValue& b) noexcept
{
   a.Swap(b);
}
} // namespace std

#include "WsfPProxyValue_inline.hpp"

#endif
