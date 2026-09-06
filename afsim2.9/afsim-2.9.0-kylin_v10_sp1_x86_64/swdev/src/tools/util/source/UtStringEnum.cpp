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

#include "UtStringEnum.hpp"

#include <unordered_map>
#include <vector>

namespace
{
struct HashStringPtr
{
   size_t operator()(const std::string* aStringPtr) const
   {
      std::hash<std::string> hf;
      return hf(*aStringPtr);
   }
};
struct EqStringPtr
{
   bool operator()(const std::string* aLhs, const std::string* aRhs) const { return *aLhs == *aRhs; }
};

std::vector<std::string*>       mAllocStrings;
std::vector<const std::string*> mEnumToString;
using StringToEnumMap = std::unordered_map<const std::string*, int, HashStringPtr, EqStringPtr>;
StringToEnumMap           mStringToEnum;
std::string               mLookupFailureString = "-invalid-string-enum-";
inline const std::string* findByEnum(unsigned int aEnum)
{
   if (mEnumToString.size() > aEnum)
      return mEnumToString[aEnum];
   return nullptr;
}
inline int findByString(const std::string& aString)
{
   auto i = mStringToEnum.find(&aString);
   if (i != mStringToEnum.end())
      return i->second;
   return -1;
}
} // namespace

const std::string& UtStringEnum::MakeGlobalString(const std::string& aString)
{
   auto i = mStringToEnum.find(&aString);
   if (i != mStringToEnum.end())
   {
      return *i->first;
   }
   mAllocStrings.push_back(new std::string(aString));
   return *mAllocStrings.back();
}

void UtStringEnum::ClearStringTable()
{
   for (auto& allocString : mAllocStrings)
   {
      delete allocString;
   }
   mAllocStrings.clear();
}

UtStringEnum UtStringEnum::Create(const std::string& aString)
{
   int id = findByString(aString);
   if (id < 0)
   {
      id = (int)mEnumToString.size();
      RegisterString(id, aString.c_str());
   }
   return UtStringEnum(id);
}

void UtStringEnum::RegisterString(int aStringId, const char* aStringPtr)
{
   if ((int)mEnumToString.size() <= aStringId)
      mEnumToString.resize(aStringId + 1);

   const std::string* strPtr = &MakeGlobalString(aStringPtr);
   mEnumToString[aStringId]  = strPtr;
   mStringToEnum[strPtr]     = aStringId;
}

void UtStringEnum::RegisterStrings()
{
   RegisterString(0, "");

#ifdef UT_GENERATED_STRING_LIST
#include "UtStringEnum_gen_imp.hpp"
#endif
}

UtStringEnum::UtStringEnum(const std::string& aString)
{
   mStringId = findByString(aString);
}

UtStringEnum::operator const std::string&() const
{
   return GetString();
}

const std::string& UtStringEnum::GetString() const
{
   const std::string* s = findByEnum(mStringId);
   if (s != nullptr)
      return *s;
   return mLookupFailureString;
}
