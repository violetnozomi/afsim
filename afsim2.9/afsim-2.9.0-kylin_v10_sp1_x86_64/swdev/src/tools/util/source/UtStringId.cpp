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

#include "UtStringId.hpp"

#include <iostream>

#include "UtDictionary.hpp"

//! Get the string associated with a given ID.
// static
const std::string& UtStringId::GetString(int aId)
{
   return GetDictionary().GetString(aId);
}
//
//! Get the string associated with the ID.
const std::string& UtStringId::GetString() const
{
   return GetDictionary().GetString(mId);
}

UtStringId::operator const std::string&() const
{
   return GetString();
}

std::string operator+(UtStringId aLhs, const std::string& aRhs)
{
   return aLhs.GetString() + aRhs;
}
std::string operator+(UtStringId aLhs, const char* aRhsPtr)
{
   return aLhs.GetString() + aRhsPtr;
}
std::string operator+(const std::string& aLhs, UtStringId aRhs)
{
   return aLhs + aRhs.GetString();
}
std::string operator+(const char* aLhsPtr, UtStringId aRhs)
{
   return aLhsPtr + aRhs.GetString();
}

//! Set the ID equal to the ID of the specified string.
void UtStringId::SetString(const std::string& aString)
{
   mId = GetDictionary().GetNumber(aString);
}

//! Get the ID associated with the supplied string.
// static
int UtStringId::GetId(const std::string& aString)
{
   return GetDictionary().GetNumber(aString);
}
UtStringId UtStringId::UnsafeFindStringId(int aNumber)
{
   UtStringId id;
   id.mId = aNumber;
   return id;
}

//! Compare for equality vs. a string
bool UtStringId::operator==(const std::string& aString) const
{
   return GetDictionary().GetString(mId) == aString;
}

//! Compare for equality vs. a C-style string.
bool UtStringId::operator==(const char* aStringPtr) const
{
   if (aStringPtr != nullptr)
   {
      return GetDictionary().GetString(mId) == aStringPtr;
   }
   else
   {
      return mId == 0;
   }
}

void UtStringId::ResetDictionary()
{
   UtDictionary::Destroy();
}

UtStringId& UtStringId::operator=(const char* aRhsPtr)
{
   if (aRhsPtr == nullptr)
   {
      mId = 0;
   }
   else
   {
      mId = GetDictionary().GetNumber(aRhsPtr);
   }
   return *this;
}

bool UtStringId::Exists(const std::string& aString)
{
   return GetDictionary().Exists(aString);
}

UtStringId::UtStringId(const std::string& aString)
{
   SetString(aString);
}

UtStringId::UtStringId(const char* aStringPtr)
   : mId(0)
{
   if (aStringPtr != nullptr)
   {
      SetString(aStringPtr);
   }
}

UtDictionary& UtStringId::GetDictionary()
{
   return UtDictionary::GetDictionary();
}

// friend
std::ostream& operator<<(std::ostream& aOut, const UtStringId& aStringId)
{
   if (aStringId.GetId() != 0)
   {
      aOut << aStringId.GetString();
   }
   else
   {
      aOut << "<none>";
   }
   return aOut;
}
bool operator==(const std::string& aLhs, UtStringId aRhs)
{
   return aLhs == aRhs.GetString();
}
bool operator==(const char* aLhsPtr, UtStringId aRhs)
{
   return aLhsPtr == aRhs.GetString();
}
