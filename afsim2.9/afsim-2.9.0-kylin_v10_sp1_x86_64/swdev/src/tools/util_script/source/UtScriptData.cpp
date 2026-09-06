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

#include "UtScriptData.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptTypes.hpp"
#include "UtStringUtil.hpp"

namespace ut
{

namespace script
{

// Initialize static data members.
Data::Data(UtScriptClass* aClassPtr, void* aAppDataPtr, UtScriptRef::MemManagement aManage)
   : mType(Type::cUNDEFINED)
   , mMemory(cDONT_MANAGE)
{
   if (aClassPtr != nullptr)
   {
      if (aClassPtr->IsBasicType())
      {
         aClassPtr->SetDefaultValue(*this);
      }
      else
      {
         SetPointer(new UtScriptRef(aAppDataPtr, aClassPtr, aManage));
      }
   }
}

void Data::SetDefaultValue(UtStringId aTypeId, const UtScriptTypes& aTypes)
{
   UtScriptClass* classPtr = aTypes.GetClass(aTypeId);
   if (classPtr != nullptr)
   {
      classPtr->SetDefaultValue(*this);
   }
}

static std::string sEmptyString;

const std::string& Data::GetString() const
{
   if ((mData.uStringPtr != nullptr) && (mType == Type::cSTRING))
   {
      return *mData.uStringPtr;
   }
   else
   {
      return sEmptyString;
   }
}

void Data::CastToInt()
{
   switch (mType)
   {
   case Type::cINT:
      break;
   case Type::cDOUBLE:
      mData.uInt = static_cast<int>(mData.uDouble);
      break;
   case Type::cBOOL:
      mData.uInt = static_cast<int>(mData.uBool);
      break;
   case Type::cSTRING:
      if (mData.uStringPtr != nullptr)
      {
         int intValue = UtStringUtil::ToInt(*mData.uStringPtr);
         if (mMemory == cMANAGE)
         {
            delete mData.uStringPtr;
         }
         mData.uInt = intValue;
      }
      else
      {
         mData.uInt = 0;
      }
      mMemory = cDONT_MANAGE;
      break;

   default:
      ut::log::warning() << "Cannot cast to an integer.";
   }
   mType = Type::cINT;
}

void Data::CastToDouble()
{
   switch (mType)
   {
   case Type::cINT:
      mData.uDouble = static_cast<double>(mData.uInt);
      break;
   case Type::cDOUBLE:
      break;
   case Type::cBOOL:
      mData.uDouble = static_cast<double>(mData.uBool);
      break;
   case Type::cSTRING:
      if (mData.uStringPtr != nullptr)
      {
         double value = UtStringUtil::ToDouble(*mData.uStringPtr);
         if (mMemory == cMANAGE)
         {
            delete mData.uStringPtr;
         }
         mData.uDouble = value;
      }
      else
      {
         mData.uDouble = 0.0;
      }
      mMemory = cDONT_MANAGE;
      break;
   default:
      ut::log::warning() << "Cannot cast to a double.";
      break;
   }
   mType = Type::cDOUBLE;
}

void Data::CastToBool()
{
   switch (mType)
   {
   case Type::cINT:
      mData.uBool = (mData.uInt != 0);
      break;
   case Type::cDOUBLE:
      mData.uBool = (mData.uDouble != 0.0);
      break;
   case Type::cBOOL:
      break;
   case Type::cSTRING:
      if (mData.uStringPtr != nullptr)
      {
         if (mMemory == cMANAGE)
         {
            delete mData.uStringPtr;
         }
         mData.uBool = true;
      }
      else
      {
         mData.uBool = false;
      }
      mMemory = cDONT_MANAGE;
      break;
   default:
      ut::log::warning() << "Cannot cast to a bool.";
      break;
   }
   mType = Type::cBOOL;
}

static std::string sStrTrue  = "1";
static std::string sStrFalse = "0";

void Data::CastToString()
{
   switch (mType)
   {
   case Type::cINT:
      mData.uStringPtr = new std::string(UtStringUtil::ToString(mData.uInt));
      break;
   case Type::cDOUBLE:
      mData.uStringPtr = new std::string(UtStringUtil::ToString(mData.uDouble));
      break;
   case Type::cBOOL:
      mData.uStringPtr = mData.uBool ? &sStrTrue : &sStrFalse;
      mType            = Type::cSTRING;
      mMemory          = cDONT_MANAGE;
      return;
   case Type::cSTRING:
      return;
   default:
      ut::log::warning() << "Cannot cast to a string";
      CleanUp();
      return;
   }
   mMemory = cMANAGE;
   mType   = Type::cSTRING;
}

void Data::CastToType(UtStringId aTypeId)
{
   if (mType == Type::cPOINTER)
   {
      mData.uPointer->Cast(aTypeId);
   }
   else
   {
      UtScriptClass::ScriptException(false, "Cannot cast a non-complex type to complex type");
   }
}

void Data::CastToType(UtScriptClass* aTypePtr)
{
   if (mType == Type::cPOINTER)
   {
      mData.uPointer->Cast(aTypePtr);
   }
   else
   {
      UtScriptClass::ScriptException(false, "Cannot cast a non-complex type to complex type");
   }
}

void* Data::GetVoidPointer()
{
   void* ptr = nullptr;
   switch (mType)
   {
   case Type::cBOOL:
      ptr = static_cast<void*>(&mData.uBool);
      break;
   case Type::cINT:
      ptr = static_cast<void*>(&mData.uInt);
      break;
   case Type::cDOUBLE:
      ptr = static_cast<void*>(&mData.uDouble);
      break;
   case Type::cSTRING:
      ptr = static_cast<void*>(mData.uStringPtr);
      break;
   case Type::cPOINTER:
      ptr = static_cast<void*>(mData.uPointer);
      break;
   default:
      UtScriptClass::ScriptException(false, "Bad type, (ut::script::Data::GetVoidPointer)");
      break;
   }
   return ptr;
}

bool Data::IsTrueP() const
{
   bool isTrue = false;

   switch (mType)
   {
   case Type::cBOOL:
      isTrue = mData.uBool;
      break;
   case Type::cINT:
      isTrue = (mData.uInt != 0);
      break;
   case Type::cDOUBLE:
      isTrue = (mData.uDouble != 0.0);
      break;
   case Type::cSTRING:
      isTrue = ((mData.uStringPtr != nullptr) && (mData.uStringPtr->length() > 0));
      break;
   case Type::cPOINTER:
      isTrue = ((mData.uPointer != nullptr) && mData.uPointer->IsValid());
      break;
   default:
      break;
   }
   return isTrue;
}

Data::operator bool() const
{
   return ToBool(*this);
}

Data& Data::operator+=(const Data& aData)
{
   bool invalid = false;
   switch (mType)
   {
   case Type::cINT:
      if (aData.mType == Type::cINT)
      {
         mData.uInt += aData.mData.uInt;
      }
      else if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble = static_cast<double>(mData.uInt) + aData.mData.uDouble;
         mType         = Type::cDOUBLE;
      }
      else
      {
         invalid = true;
      }
      break;
   case Type::cDOUBLE:
      if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble += aData.mData.uDouble;
      }
      else if (aData.mType == Type::cINT)
      {
         mData.uDouble += static_cast<double>(aData.mData.uInt);
      }
      else
      {
         invalid = true;
      }
      break;
   case Type::cSTRING:
      if (aData.mType == Type::cSTRING)
      {
         if (aData.mData.uStringPtr != nullptr)
         {
            if (mData.uStringPtr == nullptr)
            {
               mData.uStringPtr = new std::string(*aData.mData.uStringPtr);
            }
            else if (mMemory == cMANAGE)
            {
               (*mData.uStringPtr) += *aData.mData.uStringPtr;
            }
            else
            {
               mData.uStringPtr = new std::string(*mData.uStringPtr);
               (*mData.uStringPtr) += *aData.mData.uStringPtr;
            }
            mMemory = cMANAGE;
         }
      }
      else
      {
         invalid = true;
      }
      break;
   default:
      invalid = true;
      break;
   }
   if (invalid)
   {
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '+'");
   }
   return *this;
}

Data& Data::operator+=(int aInt)
{
   switch (mType)
   {
   case Type::cINT:
      mData.uInt += aInt;
      break;
   case Type::cDOUBLE:
      mData.uDouble += static_cast<double>(aInt);
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T+int'");
   }
   return *this;
}
Data& Data::operator+=(double aDouble)
{
   switch (mType)
   {
   case Type::cDOUBLE:
      mData.uDouble += aDouble;
      break;
   case Type::cINT:
      mData.uDouble = static_cast<double>(mData.uInt) + aDouble;
      mType         = Type::cDOUBLE;
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T+double'");
   }
   return *this;
}
Data& Data::operator-=(int aInt)
{
   switch (mType)
   {
   case Type::cINT:
      mData.uInt -= aInt;
      break;
   case Type::cDOUBLE:
      mData.uDouble -= static_cast<double>(aInt);
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T-int'");
   }
   return *this;
}
Data& Data::operator-=(double aDouble)
{
   switch (mType)
   {
   case Type::cDOUBLE:
      mData.uDouble -= aDouble;
      break;
   case Type::cINT:
      mData.uDouble = static_cast<double>(mData.uInt) - aDouble;
      mType         = Type::cDOUBLE;
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T-double");
   }
   return *this;
}

Data& Data::operator-=(const Data& aData)
{
   bool invalid = false;
   switch (mType)
   {
   case Type::cINT:
      if (aData.mType == Type::cINT)
      {
         mData.uInt -= aData.mData.uInt;
      }
      else if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble = static_cast<double>(mData.uInt) - aData.mData.uDouble;
         mType         = Type::cDOUBLE;
      }
      else
      {
         invalid = true;
      }
      break;
   case Type::cDOUBLE:
      if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble -= aData.mData.uDouble;
      }
      else if (aData.mType == Type::cINT)
      {
         mData.uDouble -= static_cast<double>(aData.mData.uInt);
      }
      else
      {
         invalid = true;
      }
      break;
   default:
      invalid = true;
      break;
   }
   if (invalid)
   {
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '-'");
   }
   return *this;
}

Data& Data::operator*=(int aInt)
{
   switch (mType)
   {
   case Type::cINT:
      mData.uInt *= aInt;
      break;
   case Type::cDOUBLE:
      mData.uDouble *= static_cast<double>(aInt);
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T*int'");
   }
   return *this;
}
Data& Data::operator*=(double aDouble)
{
   switch (mType)
   {
   case Type::cDOUBLE:
      mData.uDouble *= aDouble;
      break;
   case Type::cINT:
      mData.uDouble = static_cast<double>(mData.uInt) * aDouble;
      mType         = Type::cDOUBLE;
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T*double'");
   }
   return *this;
}
Data& Data::operator*=(const Data& aData)
{
   bool invalid = false;
   switch (mType)
   {
   case Type::cINT:
      if (aData.mType == Type::cINT)
      {
         mData.uInt *= aData.mData.uInt;
      }
      else if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble = static_cast<double>(mData.uInt) * aData.mData.uDouble;
         mType         = Type::cDOUBLE;
      }
      else
      {
         invalid = true;
      }
      break;
   case Type::cDOUBLE:
      if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble *= aData.mData.uDouble;
      }
      else if (aData.mType == Type::cINT)
      {
         mData.uDouble *= static_cast<double>(aData.mData.uInt);
      }
      else
      {
         invalid = true;
      }
      break;
   default:
      invalid = true;
      break;
   }
   if (invalid)
   {
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '*'");
   }
   return *this;
}

Data& Data::operator/=(int aInt)
{
   switch (mType)
   {
   case Type::cINT:
      mData.uInt /= aInt;
      break;
   case Type::cDOUBLE:
      mData.uDouble /= static_cast<double>(aInt);
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T/int'");
   }
   return *this;
}

Data& Data::operator/=(double aDouble)
{
   switch (mType)
   {
   case Type::cDOUBLE:
      mData.uDouble /= aDouble;
      break;
   case Type::cINT:
      mData.uDouble = static_cast<double>(mData.uInt) / aDouble;
      mType         = Type::cDOUBLE;
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator 'T/double'");
   }
   return *this;
}

Data& Data::operator/=(const Data& aData)
{
   bool invalid = false;
   switch (mType)
   {
   case Type::cINT:
      if (aData.mType == Type::cINT)
      {
         mData.uInt /= aData.mData.uInt;
      }
      else if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble = static_cast<double>(mData.uInt) / aData.mData.uDouble;
         mType         = Type::cDOUBLE;
      }
      else
      {
         invalid = true;
      }
      break;
   case Type::cDOUBLE:
      if (aData.mType == Type::cDOUBLE)
      {
         mData.uDouble /= aData.mData.uDouble;
      }
      else if (aData.mType == Type::cINT)
      {
         mData.uDouble /= static_cast<double>(aData.mData.uInt);
      }
      else
      {
         invalid = true;
      }
      break;
   default:
      invalid = true;
      break;
   }
   if (invalid)
   {
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '/'");
   }
   return *this;
}

void Data::operator-()
{
   switch (mType)
   {
   case Type::cINT:
      mData.uInt = -mData.uInt;
      break;
   case Type::cDOUBLE:
      mData.uDouble = -mData.uDouble;
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operand for unary operator '-'");
      break;
   }
}

Data Data::operator!()
{
   return Data(!IsTrue());
}

bool Data::operator==(bool aBool) const
{
   if (mType == Type::cBOOL)
   {
      return mData.uBool == aBool;
   }
   if (mType == Type::cINT)
   {
      return mData.uInt == static_cast<int>(aBool);
   }
   if (mType == Type::cDOUBLE)
   {
      return mData.uDouble == static_cast<double>(aBool);
   }
   return false;
}

bool Data::operator==(int aInt) const
{
   if (mType == Type::cINT)
   {
      return mData.uInt == aInt;
   }
   if (mType == Type::cDOUBLE)
   {
      return mData.uDouble == static_cast<double>(aInt);
   }
   if (mType == Type::cBOOL)
   {
      return static_cast<int>(mData.uBool) == aInt;
   }
   return false;
}

bool Data::operator==(double aDouble) const
{
   if (mType == Type::cDOUBLE)
   {
      return mData.uDouble == aDouble;
   }
   if (mType == Type::cINT)
   {
      return static_cast<double>(mData.uInt) == aDouble;
   }
   if (mType == Type::cBOOL)
   {
      return static_cast<double>(mData.uBool) == aDouble;
   }
   return false;
}

bool Data::operator==(const std::string& aString) const
{
   if (mType == Type::cSTRING)
   {
      if (mData.uStringPtr != nullptr)
      {
         return *mData.uStringPtr == aString;
      }
      else
      {
         // LHS is a null. RHS must be null or an empty string
         return aString.empty();
      }
   }
   return false;
}


bool Data::operator==(const Data& aRHS) const
{
   bool equalTo = false;

   // Pointers are strictly less-than all other types.
   if ((aRHS.mType == Type::cPOINTER) != (mType == Type::cPOINTER))
   {
      return false;
   }

   switch (mType)
   {
   // Numeric comparisons must occur using the widest type of the two operands.
   case Type::cBOOL:
   {
      if (aRHS.mType == Type::cBOOL)
      {
         equalTo = (mData.uBool == aRHS.mData.uBool);
      }
      else if (aRHS.mType == Type::cINT)
      {
         equalTo = (static_cast<int>(mData.uBool) == aRHS.mData.uInt);
      }
      else if (aRHS.mType == Type::cDOUBLE)
      {
         equalTo = (static_cast<double>(mData.uBool) == aRHS.mData.uDouble);
      }
      else
      {
         UtScriptClass::ScriptException(false, "Can't convert type to a bool");
      }
   }
   break;
   case Type::cINT:
   {
      if (aRHS.mType == Type::cINT)
      {
         equalTo = (mData.uInt == aRHS.mData.uInt);
      }
      else if (aRHS.mType == Type::cDOUBLE)
      {
         // LHS is int. If RHS is wider (double) then cast the LHS to double, otherwise RHS is same width
         // or narrower and can be casted to the type of the LHS (int).
         equalTo = (static_cast<double>(mData.uInt) == aRHS.mData.uDouble);
      }
      else if (aRHS.mType == Type::cBOOL)
      {
         equalTo = (mData.uInt == static_cast<int>(aRHS.mData.uBool));
      }
      else
      {
         UtScriptClass::ScriptException(false, "Can't convert type to an integer");
      }
   }
   break;
   case Type::cDOUBLE:
   {
      // Nothing is wider than a double, so cast RHS to double.
      equalTo = (mData.uDouble == Data::ToDouble(aRHS));
   }
   break;
   case Type::cSTRING:
      if (aRHS.mType == Type::cSTRING)
      {
         if ((mData.uStringPtr != nullptr) && (aRHS.mData.uStringPtr != nullptr))
         {
            equalTo = (*mData.uStringPtr == *aRHS.mData.uStringPtr);
         }
         else if (mData.uStringPtr == nullptr)
         {
            // LHS is a null. RHS must be null or an empty string
            equalTo = (aRHS.mData.uStringPtr == nullptr) || ((*aRHS.mData.uStringPtr).empty());
         }
         else
         {
            // RHS is null. LHS must be null or an empty string
            equalTo = (mData.uStringPtr == nullptr) || ((*mData.uStringPtr).empty());
         }
      }
      break;
   // Use the application object's pointer as the comparison.
   case Type::cPOINTER:
   {
      if ((mData.uPointer != nullptr) && (aRHS.mData.uPointer != nullptr))
      {
         void* lhsPtr = mData.uPointer->GetAppObject();
         void* rhsPtr = aRHS.mData.uPointer->GetAppObject();
         equalTo      = std::equal_to<void*>()(lhsPtr, rhsPtr);
         if (!equalTo && (lhsPtr != nullptr) && (rhsPtr != nullptr))
         {
            equalTo = (mData.uPointer->GetScriptClass()->EqualTo(lhsPtr, rhsPtr));
         }
      }

      break;
   }
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '=='");
   }
   return equalTo;
}

bool Data::operator<(bool aBool) const
{
   if (mType == Type::cBOOL)
   {
      return (mData.uBool < aBool);
   }
   if (mType == Type::cINT)
   {
      return (mData.uInt < static_cast<int>(aBool));
   }
   if (mType == Type::cDOUBLE)
   {
      return (mData.uDouble < static_cast<double>(aBool));
   }
   if (mType == Type::cPOINTER)
   { // pointers are less than all other types
      return true;
   }
   // (string < not-string-not-pointer) is false.
   return false;
}

bool Data::operator<(int aInt) const
{
   if (mType == Type::cINT)
   {
      return mData.uInt < aInt;
   }
   if (mType == Type::cDOUBLE)
   {
      return mData.uDouble < static_cast<double>(aInt);
   }
   if (mType == Type::cBOOL)
   {
      return static_cast<int>(mData.uBool) < aInt;
   }
   if (mType == Type::cPOINTER)
   { // pointers are less than all other types
      return true;
   }
   // (string < not-string-not-pointer) is false.
   return false;
}

bool Data::operator<(double aDouble) const
{
   if (mType == Type::cDOUBLE)
   {
      return mData.uDouble < aDouble;
   }
   if (mType == Type::cINT)
   {
      return static_cast<double>(mData.uDouble) < aDouble;
   }
   if (mType == Type::cBOOL)
   {
      return static_cast<double>(mData.uBool) < aDouble;
   }
   if (mType == Type::cPOINTER)
   { // pointers are less than all other types
      return true;
   }
   // (string < not-string-not-pointer) is false.
   return false;
}
bool Data::operator<(const std::string& aString) const
{
   if (mType == Type::cSTRING)
   {
      if (mData.uStringPtr != nullptr)
      {
         return (*mData.uStringPtr < aString);
      }
      else
      {
         // LHS is null. LHS is less than only if RHS is not null and not empty
         return (!(aString).empty());
      }
   }
   else
   {
      // Pointers are less than all other types.
      // Also, (string < not-string-not-pointer) is false,
      // so (not-string-not-pointer < string) must be true.
      return true;
   }
}

bool Data::operator<(const Data& aRHS) const
{
   bool lessThan = false;

   // Pointers are strictly less-than all other types.
   if (mType == Type::cPOINTER && aRHS.mType != Type::cPOINTER)
   {
      return false;
   }
   if (mType != Type::cPOINTER && aRHS.mType == Type::cPOINTER)
   {
      return true;
   }

   switch (mType)
   {
   // Numeric comparisons must occur using the widest type of the two operands.
   case Type::cBOOL:
   {
      if (aRHS.mType == Type::cBOOL)
      {
         lessThan = (mData.uBool < aRHS.mData.uBool);
      }
      else if (aRHS.mType == Type::cINT)
      {
         lessThan = (static_cast<int>(mData.uBool) < aRHS.mData.uInt);
      }
      else if (aRHS.mType == Type::cDOUBLE)
      {
         lessThan = (static_cast<double>(mData.uBool) < aRHS.mData.uDouble);
      }
      else if (aRHS.mType == Type::cSTRING)
      { // (string < not-pointer-not-string) is always false, so (bool < string) is always true
         lessThan = true;
      }
      else
      {
         UtScriptClass::ScriptException(false, "Can't convert type to a bool");
      }
   }
   break;
   case Type::cINT:
   {
      if (aRHS.mType == Type::cINT)
      {
         lessThan = (mData.uInt < aRHS.mData.uInt);
      }
      else if (aRHS.mType == Type::cDOUBLE)
      {
         // LHS is int. If RHS is wider (double) then cast the LHS to double, otherwise RHS is same width
         // or narrower and can be casted to the type of the LHS (int).
         lessThan = (static_cast<double>(mData.uInt) < aRHS.mData.uDouble);
      }
      else if (aRHS.mType == Type::cBOOL)
      {
         lessThan = (mData.uInt < static_cast<int>(aRHS.mData.uBool));
      }
      else if (aRHS.mType == Type::cSTRING)
      { // (string < not-pointer-not-string) is always false, so (int < string) must be true
         lessThan = true;
      }
      else
      {
         UtScriptClass::ScriptException(false, "Can't convert type to an integer");
      }
   }
   break;
   case Type::cDOUBLE:
   {
      if (aRHS.mType != Type::cSTRING)
      {
         // Nothing is wider than a double, so cast RHS to double.
         lessThan = (mData.uDouble < Data::ToDouble(aRHS));
      }
      else
      { // (string < not-pointer-not-string) is always false, so (double < string) must be true
         lessThan = true;
      }
   }
   break;
   case Type::cSTRING:
      if (aRHS.mType == Type::cSTRING)
      {
         if ((mData.uStringPtr != nullptr) && (aRHS.mData.uStringPtr != nullptr))
         {
            lessThan = (*mData.uStringPtr < *aRHS.mData.uStringPtr);
         }
         else if ((mData.uStringPtr == nullptr) && (aRHS.mData.uStringPtr != nullptr))
         {
            // LHS is null. LHS is less than only if RHS is not null and not empty
            lessThan = (!(*aRHS.mData.uStringPtr).empty());
         }
      }
      else
      { // (string < not-pointer-not-string) is always false
      }
      break;
   case Type::cPOINTER:
   {
      if (mData.uPointer != nullptr)
      {
         if ((mData.uPointer->GetAppObject() != nullptr) && (aRHS.mData.uPointer->GetAppObject() != nullptr) &&
             // Make sure this type is less-than comparable before actually comparing.
             mData.uPointer->GetScriptClass()->IsLessThanComparable())
         {
            lessThan = (mData.uPointer->GetScriptClass()->LessThan(mData.uPointer->GetAppObject(),
                                                                   aRHS.mData.uPointer->GetAppObject()));
         }
         else
         {
            lessThan = std::less<void*>()(mData.uPointer->GetAppObject(), aRHS.mData.uPointer->GetAppObject());
         }
      }
   }
   break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '<'");
      break;
   }
   return lessThan;
}

void Data::ToStringN(std::ostream& aOut, int aMaxStringSize) const
{
   switch (mType)
   {
   case Type::cUNDEFINED:
      aOut << "null";
      break;
   case Type::cBOOL:
      aOut << mData.uBool;
      break;
   case Type::cINT:
      aOut << mData.uInt;
      break;
   case Type::cDOUBLE:
      aOut << mData.uDouble;
      break;
   case Type::cSTRING:
      if (mData.uStringPtr != nullptr)
      {
         if (static_cast<int>(mData.uStringPtr->size()) > aMaxStringSize)
         {
            aOut << mData.uStringPtr->substr(0, aMaxStringSize);
         }
         else
         {
            aOut << *mData.uStringPtr;
         }
      }
      break;
   case Type::cPOINTER:
   {
      UtScriptRef* refPtr = GetPointer();
      if (refPtr != nullptr)
      {
         if (refPtr->GetScriptClass())
         {
            if (refPtr->IsValid())
            {
               std::string objectStr = refPtr->GetScriptClass()->ToStringN(refPtr->GetAppObject(), aMaxStringSize);
               if (objectStr.empty())
               {
                  aOut << refPtr->GetScriptClass()->GetClassName();
               }
               else
               {
                  aOut << objectStr;
               }
            }
            else
            {
               aOut << refPtr->GetScriptClass()->GetClassName() + "(null)";
            }
         }
      }
   }
   break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '<<'");
      break;
   }
}

std::ostream& operator<<(std::ostream& aOut, const Data& aData)
{
   aData.ToStringN(aOut, 100000);
   return aOut;
}

std::istream& operator>>(std::istream& aIn, Data& aData)
{
   switch (aData.mType)
   {
   case Data::Type::cBOOL:
      aIn >> aData.mData.uBool;
      break;
   case Data::Type::cINT:
      aIn >> aData.mData.uInt;
      break;
   case Data::Type::cDOUBLE:
      aIn >> aData.mData.uDouble;
      break;
   default:
      UtScriptClass::ScriptException(false, "Invalid operands for binary operator '>>'");
      break;
   }
   return aIn;
}

//! Finish cleanup of a pointer or string
// private
void Data::CleanUpP()
{
   assert(mMemory == cMANAGE);
   if (mType == Type::cPOINTER)
   {
      // Delete the pointer if we are managing its memory.
      delete mData.uPointer;
   }
   else if (mType == Type::cSTRING)
   {
      delete mData.uStringPtr;
   }
   mData.uPointer = nullptr;
}

//! CopyPointer handles copying a ut::script::Data object that contains a UtScriptRef pointer.
//! It is assumed that Cleanup() has been called prior to this method.
// private
void Data::CopyPointer(const Data& aSrc)
{
   UtScriptClass* typePtr = aSrc.mData.uPointer->GetScriptClass();

   // If this is a basic type (int, double, char, bool, string) then
   // copy it off as the appropriate type.
   if (typePtr->IsBasicType())
   {
      void* objPtr = aSrc.mData.uPointer->GetAppObject();
      if (typePtr->IsInt())
      {
         mType      = Type::cINT;
         mData.uInt = (objPtr == nullptr) ? 0 : *static_cast<int*>(objPtr);
      }
      else if (typePtr->IsDouble())
      {
         mType         = Type::cDOUBLE;
         mData.uDouble = (objPtr == nullptr) ? 0.0 : *static_cast<double*>(objPtr);
      }
      else if (typePtr->IsBool())
      {
         mType       = Type::cBOOL;
         mData.uBool = (objPtr == nullptr) ? false : *static_cast<bool*>(objPtr);
      }
      else if (typePtr->IsString())
      {
         mType = Type::cSTRING;
         if (objPtr != nullptr && (!static_cast<std::string*>(objPtr)->empty()))
         {
            mData.uStringPtr = new std::string(*static_cast<std::string*>(objPtr));
            mMemory          = cMANAGE;
         }
         else
         {
            mMemory          = cDONT_MANAGE;
            mData.uStringPtr = nullptr;
         }
      }
      else
      {
         UtScriptClass::ScriptException(false,
                                        "Type should be a base type (int, double, char, bool, or string), "
                                        "since it is not a complex type. ");
      }
   }
   else
   {
      mType = aSrc.mType;

      // Copy the script reference if we are managing the memory.
      if (aSrc.mMemory != cDONT_MANAGE)
      {
         mData.uPointer = new UtScriptRef(*(aSrc.GetPointer()));
         mMemory        = cMANAGE;
      }
      else
      {
         mData.uPointer = aSrc.mData.uPointer;
         mMemory        = cDONT_MANAGE;
      }
   }
}

//! Given a data object of type double, int, bool or char this method returns the value as an integer.
//! @param aData The data object of type cDOUBLE, cINT, cBOOL or cCHAR
// static
int Data::ToInt(const Data& aData)
{
   switch (aData.mType)
   {
   case Type::cINT:
      return aData.mData.uInt;
   case Type::cDOUBLE:
      return static_cast<int>(aData.mData.uDouble);
   case Type::cBOOL:
      return static_cast<int>(aData.mData.uBool);
   default:
      UtScriptClass::ScriptException(false, "Can't convert type to an integer");
      break;
   }
   return 0;
}

//! Given a data object of type double, int, bool or char this method returns the value as a double.
//! @param aData The data object of type cDOUBLE, cINT, cBOOL or cCHAR
// static
double Data::ToDouble(const Data& aData)
{
   switch (aData.mType)
   {
   case Type::cDOUBLE:
      return aData.mData.uDouble;
   case Type::cINT:
      return static_cast<double>(aData.mData.uInt);
   case Type::cBOOL:
      return static_cast<double>(aData.mData.uBool);
   default:
      UtScriptClass::ScriptException(false, "Can't convert type to a double");
      break;
   }
   return 0.0;
}

//! Given a data object of type double, int, bool or char this method returns the value as a boolean.
//! @param aData The data object of type cDOUBLE, cINT, cBOOL or cCHAR
// static
bool Data::ToBool(const Data& aData)
{
   switch (aData.mType)
   {
   case Type::cBOOL:
      return aData.mData.uBool;
   case Type::cINT:
      return (aData.mData.uInt != 0);
   case Type::cDOUBLE:
      return (aData.mData.uDouble != 0.0);
   default:
      UtScriptClass::ScriptException(false, "Can't convert type to a bool");
      break;
   }
   return false;
}

void Data::SetExternallyManagedString(const std::string& aStr)
{
   CleanUp();

   mType   = Type::cSTRING;
   mMemory = cDONT_MANAGE;
   // const_cast is safe because an invariant of an unmanaged
   // script data string is that it will never be mutated.
   mData.uStringPtr = const_cast<std::string*>(&aStr);
}

Data::Data(UtStringId aStringId)
   : mType(Type::cSTRING)
   , mMemory(cDONT_MANAGE)
{
   // const_cast is safe because an invariant of an unmanaged
   // script data string is that it will never be mutated.
   mData.uStringPtr = const_cast<std::string*>(&aStringId.GetString());
}

void Data::SetString(UtStringId aStringId)
{
   SetExternallyManagedString(aStringId);
}

Data& Data::operator=(UtStringId aStringId)
{
   SetString(aStringId);
   return *this;
}

} // namespace script

} // namespace ut
