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

#include "UtVariant.hpp"

#include <unordered_map>

namespace ut
{

int Variant::ToInt() const
{
   switch (mType)
   {
   case Type::cUNSET:
      return 0;
   case Type::cBOOL:
      return mData.uBool ? 1 : 0;
   case Type::cINT:
      return mData.uInt;
   case Type::cDOUBLE:
      return static_cast<int>(mData.uDouble);
   case Type::cSTRING_ENUM:
      return UtStringUtil::ToInt(UtStringEnum(mData.uStringEnum));
   case Type::cSTRING:
      return mData.uStringPtr != nullptr ? UtStringUtil::ToInt(*mData.uStringPtr) : 0;
   case Type::cPOINTER:
      return static_cast<int>(reinterpret_cast<intptr_t>(mData.uPointer));
   default:
      return mData.uInt;
   }
}

bool Variant::ToBool() const
{
   switch (mType)
   {
   case Type::cUNSET:
      return false;
   case Type::cBOOL:
      return mData.uBool;
   case Type::cINT:
      return mData.uInt != 0;
   case Type::cDOUBLE:
      return mData.uDouble != 0.0;
   case Type::cSTRING_ENUM:
      return mData.uStringEnum != 0;
   case Type::cSTRING:
      return mData.uStringPtr != nullptr;
   case Type::cPOINTER:
      return mData.uPointer != nullptr;
   default:
      return false;
   }
}

double Variant::ToDouble() const
{
   switch (mType)
   {
   case Type::cUNSET:
      return 0.0;
   case Type::cBOOL:
      return mData.uBool ? 1.0 : 0.0;
   case Type::cINT:
      return static_cast<double>(mData.uInt);
   case Type::cDOUBLE:
      return mData.uDouble;
   case Type::cSTRING_ENUM:
      return UtStringUtil::ToDouble(UtStringEnum(mData.uStringEnum));
   case Type::cSTRING:
      return mData.uStringPtr != nullptr ? UtStringUtil::ToDouble(*mData.uStringPtr) : 0;
   case Type::cPOINTER: // Fallthrough
   default:
      return 0;
   }
}

std::string Variant::ToString() const
{
   switch (mType)
   {
   case Type::cUNSET:
      return std::string();
   case Type::cBOOL:
      return mData.uBool ? "true" : "false";
   case Type::cINT:
      return UtStringUtil::ToString(mData.uInt);
   case Type::cDOUBLE:
      return UtStringUtil::ToString(mData.uDouble);
   case Type::cSTRING_ENUM:
      return UtStringEnum(mData.uStringEnum);
   case Type::cSTRING:
      return mData.uStringPtr != nullptr ? *mData.uStringPtr : std::string();
   case Type::cPOINTER: // Fallthrough
   default:
      return std::string();
   }
}

Variant& Variant::operator=(std::string aValue)
{
   CleanUp();
   mType            = Type::cSTRING;
   mData.uStringPtr = nullptr;
   if (aValue.length() > 0)
   {
      mData.uStringPtr = new std::string(std::move(aValue));
   }
   return *this;
}

Variant& Variant::operator=(const char* aValue)
{
   *this = std::string(aValue);
   return *this;
}

namespace
{
template<typename T>
size_t eightByteHash(T aValue)
{
   using EightBytes = long long;
   std::hash<EightBytes> eightByteHash;
   return eightByteHash((long long)aValue);
}
} // namespace

size_t VariantHash::operator()(const Variant& aValue) const
{
   using EightBytes = long long;
   size_t h         = static_cast<size_t>(aValue.GetType()) * 4821;
   switch (aValue.GetType())
   {
   case Variant::Type::cUNSET:
      return h;
   case Variant::Type::cBOOL:
      return h ^ eightByteHash(static_cast<EightBytes>(aValue.mData.uBool));
   case Variant::Type::cINT:
      return h ^ eightByteHash(static_cast<EightBytes>(aValue.mData.uInt));
   case Variant::Type::cDOUBLE:
      return h ^ eightByteHash(reinterpret_cast<const EightBytes&>(aValue.mData.uDouble));
   case Variant::Type::cSTRING_ENUM:
      return h ^ eightByteHash(aValue.mData.uStringEnum);
   case Variant::Type::cSTRING:
   {
      std::hash<std::string> hasher;
      return h ^ (aValue.mData.uStringPtr != nullptr ? hasher(*aValue.mData.uStringPtr) : 0);
   }
   case Variant::Type::cPOINTER:
      return h ^ reinterpret_cast<size_t>(aValue.mData.uPointer);
   default:
      return h; // shouldn't get here
   }
}

} // namespace ut
