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

#ifndef UTVARIANT_HPP
#define UTVARIANT_HPP

#include "ut_export.h"

#include <cassert>
#include <string>

#include "UtStringEnum.hpp"
#include "UtStringUtil.hpp"

namespace ut
{

class Variant;

struct VariantHash
{
   size_t operator()(const Variant& aRhs) const;
};

//! A simple variant class which allows limited data types,
//! but can be copied / serialized quickly and easily
class UT_EXPORT Variant
{
public:
   friend struct VariantHash;

   enum class Type
   {
      cUNSET       = 0,
      cBOOL        = 1,
      cINT         = 2,
      cDOUBLE      = 3,
      cSTRING_ENUM = 4,
      cSTRING      = 5,
      cPOINTER     = 7
   };

   union Value
   {
      bool                 uBool;
      int                  uInt;
      double               uDouble;
      UtStringEnum::IdType uStringEnum;
      // Note: uStringPtr will be null if representing an empty string
      std::string* uStringPtr;
      void*        uPointer;
      long long    uBytes;
   };

   //! Construct a the variant as an integer = 0
   Variant() noexcept = default;
   ~Variant() noexcept { CleanUp(); }
   Variant(const Variant& aRhs)
      : mType(aRhs.mType)
      , mData(aRhs.mData)
   {
      if (mType == Type::cSTRING && mData.uStringPtr != nullptr)
      {
         mData.uStringPtr = new std::string(*mData.uStringPtr);
      }
   }

   Variant(Variant&& aRhs) noexcept
      : mType{aRhs.mType}
      , mData(aRhs.mData)
   {
      if (aRhs.mType == Type::cSTRING)
      {
         aRhs.mData.uStringPtr = nullptr;
      }
      else if (aRhs.mType == Type::cPOINTER)
      {
         aRhs.mData.uPointer = nullptr;
      }
   }

   Variant& operator=(const Variant& aRhs)
   {
      if (*this != aRhs)
      {
         CleanUp();
         mType = aRhs.mType;
         mData = aRhs.mData;
         if (mType == Type::cSTRING && mData.uStringPtr != nullptr)
         {
            mData.uStringPtr = new std::string(*mData.uStringPtr);
         }
      }
      return *this;
   }

   Variant& operator=(Variant&& aRhs) noexcept
   {
      if (*this != aRhs)
      {
         CleanUp();
         mType = aRhs.mType;
         mData = aRhs.mData;
         if (aRhs.mType == Type::cSTRING)
         {
            aRhs.mData.uStringPtr = nullptr;
         }
         else if (aRhs.mType == Type::cPOINTER)
         {
            aRhs.mData.uPointer = nullptr;
         }
      }
      return *this;
   }

   Variant(bool aValue) noexcept
      : mType(Type::cBOOL)
   {
      mData.uBool = aValue;
   }
   Variant(int aValue) noexcept
      : mType(Type::cINT)
   {
      mData.uInt = aValue;
   }
   Variant(double aValue) noexcept
      : mType(Type::cDOUBLE)
   {
      mData.uDouble = aValue;
   }
   Variant(void* aValue) noexcept
      : mType(Type::cPOINTER)
   {
      mData.uPointer = aValue;
   }
   Variant(UtStringEnumId aValue) noexcept
      : mType(Type::cSTRING_ENUM)
   {
      mData.uStringEnum = (int)aValue;
   }
   Variant(UtStringEnum aValue) noexcept
      : mType(Type::cSTRING_ENUM)
   {
      mData.uStringEnum = aValue.GetId();
   }
   Variant(std::string aValue)
      : mType(Type::cSTRING)
   {
      mData.uStringPtr = nullptr;
      if (aValue.length() > 0)
      {
         mData.uStringPtr = new std::string(std::move(aValue));
      }
   }
   Variant(const char* aValue)
      : Variant(std::string(aValue))
   {
   }

   //! Sets the variant to a bool value.
   Variant& operator=(bool aValue)
   {
      CleanUp();
      mType       = Type::cBOOL;
      mData.uBool = aValue;
      return *this;
   }
   //! Sets the variant to a int value.
   Variant& operator=(int aValue)
   {
      CleanUp();
      mType      = Type::cINT;
      mData.uInt = aValue;
      return *this;
   }
   //! Sets the variant to a double value.
   Variant& operator=(double aValue)
   {
      CleanUp();
      mType         = Type::cDOUBLE;
      mData.uDouble = aValue;
      return *this;
   }
   //! Sets the variant to a double value.
   Variant& operator=(UtStringEnum aValue)
   {
      CleanUp();
      mType             = Type::cSTRING_ENUM;
      mData.uStringEnum = aValue.GetId();
      return *this;
   }
   //! Sets the variant to a pointer value.  Note: UtVariant does no memory management.
   Variant& operator=(void* aValue)
   {
      CleanUp();
      mType          = Type::cPOINTER;
      mData.uPointer = aValue;
      return *this;
   }
   //! Sets the variant to a string value.
   Variant& operator=(std::string aValue);
   //! Sets the variant to a string value.
   Variant& operator=(const char* aValue);

   //! Return true if the variant is unset
   bool IsUnset() const { return mType == Type::cUNSET; }

   //! Return the variant converted to boolean
   bool ToBool() const;

   //! Return the variant converted to integer
   int ToInt() const;

   //! Return the variant converted to double
   double ToDouble() const;

   //! Return the variant converted to string
   std::string ToString() const;

   //! Returns the value of the variant if it is of type boolean
   bool GetBool() const noexcept { return mType == Type::cBOOL ? mData.uBool : false; }

   //! Returns the value of the variant if it is of type int
   int GetInt() const noexcept { return mType == Type::cINT ? mData.uInt : 0; }

   //! Returns the value of the variant if it is of type double
   double GetDouble() const noexcept { return mType == Type::cDOUBLE ? mData.uDouble : 0.0; }

   //! Returns the value of the variant if it is of type String Enum
   UtStringEnum GetStringEnum() const noexcept
   {
      return mType == Type::cSTRING_ENUM ? UtStringEnum(mData.uStringEnum) : UtStringEnum();
   }

   //! Returns the value of the variant if it is of type string
   std::string GetString() const
   {
      return (mType == Type::cSTRING && mData.uStringPtr) ? *mData.uStringPtr : std::string();
   }

   //! Returns the value of the variant if it is of type pointer
   void* GetPointer() const noexcept { return mType == Type::cPOINTER ? mData.uPointer : nullptr; }

   //! Returns the type of the variant value
   Type GetType() const noexcept { return mType; }

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      int    typeNumber = static_cast<int>(mType);
      aBuff& typeNumber;
      mType = static_cast<Type>(typeNumber);
      switch (mType)
      {
      case Type::cBOOL:
         (aBuff & mData.uBool);
         break;
      case Type::cINT:
         (aBuff & mData.uInt);
         break;
      case Type::cDOUBLE:
         (aBuff & mData.uDouble);
         break;
      case Type::cSTRING:
         (aBuff & mData.uStringPtr);
         break;
      case Type::cPOINTER: // No cheap portable way to do this, just skip it
      default:
         if (T::cIS_OUTPUT)
            mData.uPointer = nullptr;
         break;
      }
   }

   bool operator<(const Variant& aRhs) const
   {
      if (mType < aRhs.mType)
      {
         return true;
      }
      if (mType > aRhs.mType)
      {
         return false;
      }
      switch (mType)
      {
      case Type::cUNSET:
         return false;
      case Type::cBOOL:
         return mData.uBool < aRhs.mData.uBool;
      case Type::cINT:
         return mData.uInt < aRhs.mData.uInt;
      case Type::cDOUBLE:
         return mData.uDouble < aRhs.mData.uDouble;
      case Type::cSTRING_ENUM:
         return mData.uStringEnum < aRhs.mData.uStringEnum;
      case Type::cSTRING:
      {
         const bool lhsNull = mData.uStringPtr == nullptr;
         const bool rhsNull = aRhs.mData.uStringPtr == nullptr;

         if (lhsNull || rhsNull)
         {
            return lhsNull && !rhsNull;
         }
         return *mData.uStringPtr < *aRhs.mData.uStringPtr;
      }
      case Type::cPOINTER:
         return mData.uPointer < aRhs.mData.uPointer;
      default:
         return false;
      }
   }
   bool operator==(const Variant& aRhs) const
   {
      if (aRhs.mType != mType)
      {
         return false;
      }
      switch (mType)
      {
      case Type::cUNSET:
         return true;
      case Type::cBOOL:
         return mData.uBool == aRhs.mData.uBool;
      case Type::cINT:
         return mData.uInt == aRhs.mData.uInt;
      case Type::cDOUBLE:
         return mData.uDouble == aRhs.mData.uDouble;
      case Type::cSTRING_ENUM:
         return mData.uStringEnum == aRhs.mData.uStringEnum;
      case Type::cSTRING:
      {
         const bool lhsNull = mData.uStringPtr == nullptr;
         const bool rhsNull = aRhs.mData.uStringPtr == nullptr;

         if (lhsNull || rhsNull)
         {
            return lhsNull == rhsNull;
         }
         return *mData.uStringPtr == *aRhs.mData.uStringPtr;
      }
      case Type::cPOINTER:
         return mData.uPointer == aRhs.mData.uPointer;
      default:
         return true;
      }
   }
   bool operator!=(const Variant& aRhs) const { return !operator==(aRhs); }

   void Swap(Variant& aRhs)
   {
      assert(sizeof(mData.uBytes) == sizeof(mData));
      using std::swap;
      swap(mType, aRhs.mType);
      swap(mData, aRhs.mData);
   }

protected:
   void CleanUp()
   {
      if ((mType == Type::cSTRING) && (mData.uStringPtr != nullptr))
      {
         delete mData.uStringPtr;
      }
      mType        = Type::cINT;
      mData.uBytes = 0;
   }

   Type  mType{Type::cUNSET};
   Value mData{false};
};

inline void swap(Variant& a, Variant& b) noexcept
{
   a.Swap(b);
}

} // namespace ut

// C++14
// using UtVariantHash [[deprecated]] = ut::VariantHash;
// using UtVariant [[deprecated]] = ut::Variant;

// C++11
using UtVariantHash = ut::VariantHash;
using UtVariant     = ut::Variant;

#endif
