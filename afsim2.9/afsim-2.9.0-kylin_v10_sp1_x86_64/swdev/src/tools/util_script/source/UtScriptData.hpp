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

#ifndef UT_SCRIPTDATA_HPP
#define UT_SCRIPTDATA_HPP

#include "util_script_export.h"

#include <cassert>
#include <iosfwd>
#include <string>
#include <vector>

#include "UtCompilerAttributes.hpp"
#include "UtSpan.hpp"
#include "UtStringId.hpp"
class UtScriptClass;
class UtScriptData_LessThan;
#include "UtScriptRef.hpp"
class UtScriptTypes;

namespace ut
{
namespace script
{
//--------------------------------------------------------------
//! Data is the base data type that represents all types
//! used by the scripting system.  It provides a slew of operator
//! overloads (+, -, *, /, etc) for basic data types.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT Data
{
public:
   enum class Type : unsigned char
   {
      cUNDEFINED = 0,
      cBOOL,
      cINT,
      cDOUBLE,
      cSTRING,
      cPOINTER // A pointer to a managed object in the application layer.
   };

   union DataValue
   {
      bool   uBool;
      int    uInt;
      double uDouble;
      // Note: uStringPtr will be null if representing an empty string
      std::string* uStringPtr;
      UtScriptRef* uPointer;
   };

private:
   //! Resets (possibly deletes) the data based on its type.
   UT_INLINE_DECL
   void CleanUp()
   {
      if (mMemory == cMANAGE)
      {
         CleanUpP();
      }
      mType   = Type::cUNDEFINED;
      mMemory = cDONT_MANAGE;
   }

   void Copy(const Data& aSrc)
   {
      switch (aSrc.mType)
      {
      case Type::cPOINTER:
         CopyPointer(aSrc);
         break;
      case Type::cSTRING:
         mType = aSrc.mType;
         if (aSrc.mMemory != cDONT_MANAGE)
         {
            mData.uStringPtr = new std::string(*aSrc.mData.uStringPtr);
            mMemory          = cMANAGE;
         }
         else
         {
            mData.uStringPtr = aSrc.mData.uStringPtr;
            mMemory          = cDONT_MANAGE;
         }
         break;
      default:
         mType   = aSrc.mType;
         mData   = aSrc.mData;
         mMemory = aSrc.mMemory;
         break;
      }
   }

public:
   Data() = default;

   explicit Data(UtScriptClass*             aClassPtr,
                 void*                      aAppDataPtr = nullptr,
                 UtScriptRef::MemManagement aManage     = UtScriptRef::cMANAGE);

   Data(const Data& aSrc) { Copy(aSrc); }

   Data(Data&& aSrc) noexcept
      : mType(aSrc.mType)
      , mMemory(aSrc.mMemory)
      , mData(aSrc.mData)
   {
      aSrc.mType   = Type::cUNDEFINED;
      aSrc.mMemory = cDONT_MANAGE;
   }

   ~Data()
   {
      if (mMemory == cMANAGE)
      {
         CleanUpP();
      }
   }

   UT_INLINE_DECL
   Data& operator=(Data&& aSrc) noexcept
   {
      Swap(aSrc);
      return *this;
   }

   UT_INLINE_DECL
   Data& operator=(const Data& aSrc)
   {
      // Fast-track the common case
      if ((mMemory | aSrc.mMemory) == cDONT_MANAGE)
      {
         mType   = aSrc.mType;
         mMemory = aSrc.mMemory;
         mData   = aSrc.mData;
      }
      else
      {
         CleanUp();
         Copy(aSrc);
      }
      return *this;
   }

   //! perform a shallow-copy, with the invariant that the contents
   //! of aSrc will outlive the destination.
   UT_INLINE_DECL
   void ManagedElsewhereCopy(const Data& aSrc)
   {
      assert(mMemory != cMANAGE);
      mType   = aSrc.mType;
      mMemory = (aSrc.mMemory == cMANAGE) ? cMANAGED_ELSEWHERE : aSrc.mMemory;
      mData   = aSrc.mData;
   }

   //! perform a copy that allows forwarding of shallow copies.
   UT_INLINE_DECL
   Data& LocalStackCopy(const Data& aSrc)
   {
      if ((mMemory | aSrc.mMemory) <= cMANAGED_ELSEWHERE)
      {
         mType   = aSrc.mType;
         mMemory = aSrc.mMemory;
         mData   = aSrc.mData;
      }
      else
      {
         CleanUp();
         Copy(aSrc);
      }
      return *this;
   }

   //! perform a copy when setting a return UtScriptData from a local UtScriptData
   UT_INLINE_DECL
   void ReturningCopy(Data& aLocalSrc)
   {
      if (aLocalSrc.mMemory != cMANAGED_ELSEWHERE)
      {
         Swap(aLocalSrc);
      }
      else
      {
         CleanUp();
         Copy(aLocalSrc);
      }
   }

   //! performed on a UtScriptData that has just been returned from a function that
   //! may be using the cMANAGED_ELSEWHERE logic.
   UT_INLINE_DECL
   void PromoteManagedElsewhere()
   {
      if (mMemory == cMANAGED_ELSEWHERE)
      {
         if (mType == Type::cPOINTER)
         {
            CopyPointer(*this);
            mMemory = cMANAGE;
         }
         if (mType == Type::cSTRING)
         {
            mData.uStringPtr = new std::string(*mData.uStringPtr);
            mMemory          = cMANAGE;
         }
      }
   }

   explicit Data(bool aBool)
      : mType(Type::cBOOL)
      , mMemory(cDONT_MANAGE)
   {
      mData.uBool = aBool;
   }

   explicit Data(int aInt)
      : mType(Type::cINT)
      , mMemory(cDONT_MANAGE)
   {
      mData.uInt = aInt;
   }

   explicit Data(double aDouble)
      : mType(Type::cDOUBLE)
      , mMemory(cDONT_MANAGE)
   {
      mData.uDouble = aDouble;
   }

   explicit Data(const std::string& aString)
      : mType(Type::cSTRING)
      , mMemory(cDONT_MANAGE)
   {
      mData.uStringPtr = nullptr;
      if (aString.length() > 0)
      {
         mData.uStringPtr = new std::string(aString);
         mMemory          = cMANAGE;
      }
   }

   explicit Data(std::string&& aString)
      : mType(Type::cSTRING)
      , mMemory(cDONT_MANAGE)
   {
      mData.uStringPtr = nullptr;
      if (aString.length() > 0)
      {
         mData.uStringPtr = new std::string(std::move(aString));
         mMemory          = cMANAGE;
      }
   }

   explicit Data(const char* aStringLiteral)
      : Data(std::string(aStringLiteral))
   {
   }

   explicit Data(UtStringId aStringId);

   //--------------------------------------------------------------
   //! Construct the Data object as a pointer.
   //!
   //! aScriptRef: A pointer to a UtScriptRef, which contains an
   //!             application pointer and class type.
   //--------------------------------------------------------------

   Data(UtScriptRef* aPointer)
      : mType(Type::cPOINTER)
      , mMemory(cMANAGE)
   {
      assert(aPointer != nullptr);
      mData.uPointer = aPointer;
   }

   Data* Clone() const { return new Data(*this); }

   // Mutators

   void SetDefaultValue(UtStringId aTypeId, const UtScriptTypes& aTypes);

   void Set(const Data& aData)
   {
      CleanUp();
      Copy(aData);
   }

   void SetBool(bool aBool)
   {
      CleanUp();
      mType       = Type::cBOOL;
      mData.uBool = aBool;
   }
   void  SetBool(int) = delete;
   Data& operator     =(bool aBool)
   {
      SetBool(aBool);
      return *this;
   }

   void SetInt(int aInt)
   {
      CleanUp();
      mType      = Type::cINT;
      mData.uInt = aInt;
   }
   Data& operator=(int aInt)
   {
      SetInt(aInt);
      return *this;
   }

   void SetDouble(double aDouble)
   {
      CleanUp();
      mType         = Type::cDOUBLE;
      mData.uDouble = aDouble;
   }
   void  SetDouble(int) = delete;
   Data& operator       =(double aDouble)
   {
      SetDouble(aDouble);
      return *this;
   }

   void SetString(const std::string& aString)
   {
      CleanUp();
      mType            = Type::cSTRING;
      mData.uStringPtr = nullptr;
      if (!aString.empty())
      {
         mData.uStringPtr = new std::string(aString);
         mMemory          = cMANAGE;
      }
   }
   void SetString(std::string&& aString)
   {
      CleanUp();
      mType            = Type::cSTRING;
      mData.uStringPtr = nullptr;
      if (aString.length() > 0)
      {
         mData.uStringPtr = new std::string(std::move(aString));
         mMemory          = cMANAGE;
      }
   }
   void SetString(const char* aCString)
   {
      CleanUp();
      mType            = Type::cSTRING;
      mData.uStringPtr = nullptr;
      if (aCString != nullptr && aCString[0] != '\0')
      {
         mData.uStringPtr = new std::string(aCString);
         mMemory          = cMANAGE;
      }
   }
   void SetString(UtStringId aStringId);

   Data& operator=(const std::string& aString)
   {
      SetString(aString);
      return *this;
   }
   Data& operator=(std::string&& aString)
   {
      SetString(std::move(aString));
      return *this;
   }
   Data& operator=(const char* aCString)
   {
      SetString(aCString);
      return *this;
   }
   Data& operator=(UtStringId aStringId);

   //! used for creating string script data from strings that will outlive the returned script data.
   void SetExternallyManagedString(const std::string& aStr);

   void SetNullString()
   {
      CleanUp();
      mType            = Type::cSTRING;
      mData.uStringPtr = nullptr;
   }

   //! Reset the script value to 'undefined'
   void Clear() { CleanUp(); }

   void SetPointer(UtScriptRef* aPointer)
   {
      assert(aPointer != nullptr);
      CleanUp();
      mType          = Type::cPOINTER;
      mData.uPointer = aPointer;
      mMemory        = cMANAGE;
   }

   Data& operator=(UtScriptRef* aPointer)
   {
      SetPointer(aPointer);
      return *this;
   }

   void CastToInt();
   void CastToDouble();
   void CastToBool();
   void CastToString();
   void CastToType(UtStringId aTypeId);
   void CastToType(UtScriptClass* aTypePtr);

   // Accessors

   // Returns the data type.
   Type GetType() const { return mType; }

   bool               GetBool() const { return ToBool(*this); }
   int                GetInt() const { return ToInt(*this); }
   double             GetDouble() const { return ToDouble(*this); }
   const std::string& GetString() const;
   // int            GetStringId()   const;
   UtScriptRef* GetPointer() const { return mData.uPointer; }
   void*        GetVoidPointer();

   bool IsTrue() const
   {
      switch (mType)
      {
      case Type::cUNDEFINED:
         return false;
      case Type::cBOOL:
         return mData.uBool;
      case Type::cINT:
         return mData.uInt != 0;
      default:
         return IsTrueP();
      }
   }
   bool IsTrueP() const;

   operator bool() const;

   Data& operator+=(const Data& aData);
   Data& operator+=(int aInt);
   Data& operator+=(double aDouble);
   Data& operator-=(const Data& aData);
   Data& operator-=(int aInt);
   Data& operator-=(double aDouble);
   Data& operator*=(const Data& aData);
   Data& operator*=(int aInt);
   Data& operator*=(double aDouble);
   Data& operator/=(const Data& aData);
   Data& operator/=(int aInt);
   Data& operator/=(double aDouble);

   void operator-();

   Data operator!();
   bool operator&&(const Data& aData) const { return (IsTrue() && aData.IsTrue()); }
   bool operator||(const Data& aData) const { return (IsTrue() || aData.IsTrue()); }
   bool operator==(const Data& aData) const;
   bool operator!=(const Data& aData) const { return !(*this == aData); }
   bool operator>(const Data& aData) const { return aData < *this; }
   bool operator>=(const Data& aData) const { return !(*this < aData); }
   bool operator<(const Data& aData) const;
   bool operator<=(const Data& aData) const { return !(aData < *this); }

   bool operator==(bool aBool) const;
   bool operator==(int aInt) const;
   bool operator==(double aDouble) const;
   bool operator==(const std::string& aString) const;
   bool operator<(bool aBool) const;
   bool operator<(int aInt) const;
   bool operator<(double aDouble) const;
   bool operator<(const std::string& aString) const;

   //! Provided for backwards compatibility. Deprecated as of 2.8.
   using LessThan CPP14_DEPRECATED_REASON("Use std::less instead") = std::less<Data>;

   void ToStringN(std::ostream& aOut, int aMaxStringSize) const;

   static int    ToInt(const Data& aData);
   static double ToDouble(const Data& aData);
   static bool   ToBool(const Data& aData);

   void Swap(Data& aRhs) noexcept
   {
      using std::swap;
      swap(mType, aRhs.mType);
      swap(mData, aRhs.mData);
      swap(mMemory, aRhs.mMemory);
   }

   //! @name binary operators.
   /// @{
   // Declared as friends instead of in the global namespace  so that they
   // will only be found through argument dependent lookup, thus improving compile times.
   friend inline Data operator*(const Data& l, const Data& r)
   {
      Data ret(l);
      return ret *= r;
   }
   friend inline Data operator/(const Data& l, const Data& r)
   {
      Data ret(l);
      return ret /= r;
   }
   friend inline Data operator+(const Data& l, const Data& r)
   {
      Data ret(l);
      return ret += r;
   }
   friend inline Data operator-(const Data& l, const Data& r)
   {
      Data ret(l);
      return ret -= r;
   }

   UTIL_SCRIPT_EXPORT friend std::ostream& operator<<(std::ostream& aOut, const Data& aData);
   UTIL_SCRIPT_EXPORT friend std::istream& operator>>(std::istream& aIn, Data& aData);
   /// @}
private:
   enum MemManagement : unsigned char
   {
      cDONT_MANAGE,       // Indicates that this UtScriptData will not manage the memory (default).
      cMANAGED_ELSEWHERE, // Indicates the data is managed elsewhere, with the invariant that the data will not out-live
                          // the elsewhere managed data
      cMANAGE             // Indicates that this UtScriptData will manage the memory.
   };

   void CopyPointer(const Data& aSrc);

   void CleanUpP();

   Type          mType{Type::cUNDEFINED};
   MemManagement mMemory{cDONT_MANAGE};
   DataValue     mData;
};

// Provide a swap that can be found by ADL.
inline void swap(Data& aLhs, Data& aRhs) noexcept
{
   aLhs.Swap(aRhs);
}


using DataList   = std::vector<Data>;
using MethodArgs = ut::span<const Data>;

} // namespace script

} // namespace ut
// C++14
// using UtScriptData [[deprecated]] = ut::script::Data;
// using UtScriptDataList [[deprecated]] = ut::script::DataList;
// using UtScriptMethodArgs [[deprecated]] = ut::script::MethodArgs;

// C++11
using UtScriptData       = ut::script::Data;
using UtScriptDataList   = ut::script::DataList;
using UtScriptMethodArgs = ut::script::MethodArgs;
#endif
