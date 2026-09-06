// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "MessageAccessor.hpp"

#include <algorithm>

#include "FieldTypes.hpp"
#include "Messages.hpp"

namespace WsfL16
{
namespace Messages
{

std::map<std::type_index, FieldAccessorType> FieldAccessorType::sFieldAccessorTypes;

FieldAccessorType::FieldAccessorType()
   : mNoStatementValue(-1)
   , mIsSpare(false)
   , mIsDisused(false)
{
}
FieldAccessorType* FieldAccessorType::Find(const std::type_index& aType)
{
   auto it = sFieldAccessorTypes.find(aType);
   if (it != sFieldAccessorTypes.end())
   {
      return &it->second;
   }
   return nullptr;
}

//! Reformat enumerations
//! from: "cANTISURFACE_WARFARE"
//! to:   "Antisurface Warfare"
void FieldAccessorType::FormatEnumNames()
{
   for (std::map<int, std::string>::iterator i = mEnumMap.begin(); i != mEnumMap.end(); ++i)
   {
      std::string& name = i->second;
      name              = name.substr(1);
      std::replace(name.begin(), name.end(), '_', ' ');
      bool hitSpace = true;
      for (unsigned ind = 0; ind < name.length(); ++ind)
      {
         char c       = name[ind];
         bool alpha   = (isalpha(c) != 0);
         bool isSpace = c == ' ';
         if (hitSpace && alpha)
         {
            name[ind] = toupper(c);
            hitSpace  = false;
         }
         else if (alpha)
         {
            name[ind] = tolower(c);
         }
         else if (isSpace)
         {
            hitSpace = true;
         }
      }
   }
}

std::map<int, MessageAccessorType> MessageAccessorType::sMessageAccessorTypes;

FieldAccessor MessageAccessorType::GetField(Messages::Base* aMessage, int aFieldIndex)
{
   void* fieldPtr = (((char*)(aMessage)) + mFields[aFieldIndex].mByteOffset);
   return FieldAccessor(fieldPtr, mFields[aFieldIndex].mAccessorType, &mFields[aFieldIndex].mName);
}

MessageAccessorType* MessageAccessorType::Find(Messages::Base* aMessage)
{
   if (aMessage)
   {
      return &sMessageAccessorTypes[aMessage->GetUID()];
   }
   return nullptr;
}

FieldAccessor::DataType FieldAccessor::GetType() const
{
   return (DataType)mAccessorType->mAccessor;
}

FieldAccessor::FieldAccessor(void* aFieldPtr, FieldAccessorType* aAccessorType, const std::string* aNamePtr)
   : mFieldPtr(aFieldPtr)
   , mAccessorType(aAccessorType)
   , mNamePtr(aNamePtr)
{
}

void FieldAccessor::Set(bool aValue)
{
   FieldAccessorType::Variant val;
   val.mBoolean = aValue;
   (*mAccessorType->mFieldSetter)(mFieldPtr, val);
}

void FieldAccessor::Set(int aValue)
{
   FieldAccessorType::Variant val;
   val.mInteger = aValue;
   (*mAccessorType->mFieldSetter)(mFieldPtr, val);
}

void FieldAccessor::Set(const std::string& aValue)
{
   FieldAccessorType::Variant val;
   val.mString = aValue;
   (*mAccessorType->mFieldSetter)(mFieldPtr, val);
}

void FieldAccessor::Set(double aValue)
{
   FieldAccessorType::Variant val;
   val.mDouble = aValue;
   (*mAccessorType->mFieldSetter)(mFieldPtr, val);
}

bool FieldAccessor::GetBool() const
{
   FieldAccessorType::Variant val;
   (*mAccessorType->mFieldGetter)(mFieldPtr, val);
   return val.mBoolean;
}

int FieldAccessor::GetInt() const
{
   FieldAccessorType::Variant val;
   (*mAccessorType->mFieldGetter)(mFieldPtr, val);
   return val.mInteger;
}

std::string FieldAccessor::GetString() const
{
   FieldAccessorType::Variant val;
   (*mAccessorType->mFieldGetter)(mFieldPtr, val);
   return val.mString;
}

double FieldAccessor::GetDouble() const
{
   FieldAccessorType::Variant val;
   (*mAccessorType->mFieldGetter)(mFieldPtr, val);
   return val.mDouble;
}

// Sets the data of the field with the raw integer value.
// Determines the field's base type using the bit size of the field.
void FieldAccessor::SetRawData(int aValue)
{
   int bits = mAccessorType->mFieldSizeBits;
   switch (bits)
   {
   case 1:
      ((FieldBase<1>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 2:
      ((FieldBase<2>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 3:
      ((FieldBase<3>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 4:
      ((FieldBase<4>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 5:
      ((FieldBase<5>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 6:
      ((FieldBase<6>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 7:
      ((FieldBase<7>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 8:
      ((FieldBase<8>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 9:
      ((FieldBase<9>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 10:
      ((FieldBase<10>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 11:
      ((FieldBase<11>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 12:
      ((FieldBase<12>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 13:
      ((FieldBase<13>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 14:
      ((FieldBase<14>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 15:
      ((FieldBase<15>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 16:
      ((FieldBase<16>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 17:
      ((FieldBase<17>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 18:
      ((FieldBase<18>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 19:
      ((FieldBase<19>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 20:
      ((FieldBase<20>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 21:
      ((FieldBase<21>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 22:
      ((FieldBase<22>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 23:
      ((FieldBase<23>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 24:
      ((FieldBase<24>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 25:
      ((FieldBase<25>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 26:
      ((FieldBase<26>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 27:
      ((FieldBase<27>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 28:
      ((FieldBase<28>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 29:
      ((FieldBase<29>*)mFieldPtr)->SetRawData(aValue);
      break;
   case 30:
      ((FieldBase<30>*)mFieldPtr)->SetRawData(aValue);
      break;
   }
}

// Gets the raw integer value of a field.
// Determines the field's base type using the bit size of the field.
int FieldAccessor::GetRawData() const
{
   int bits = mAccessorType->mFieldSizeBits;
   switch (bits)
   {
   case 1:
      return ((FieldBase<1>*)mFieldPtr)->GetRawData();
   case 2:
      return ((FieldBase<2>*)mFieldPtr)->GetRawData();
   case 3:
      return ((FieldBase<3>*)mFieldPtr)->GetRawData();
   case 4:
      return ((FieldBase<4>*)mFieldPtr)->GetRawData();
   case 5:
      return ((FieldBase<5>*)mFieldPtr)->GetRawData();
   case 6:
      return ((FieldBase<6>*)mFieldPtr)->GetRawData();
   case 7:
      return ((FieldBase<7>*)mFieldPtr)->GetRawData();
   case 8:
      return ((FieldBase<8>*)mFieldPtr)->GetRawData();
   case 9:
      return ((FieldBase<9>*)mFieldPtr)->GetRawData();
   case 10:
      return ((FieldBase<10>*)mFieldPtr)->GetRawData();
   case 11:
      return ((FieldBase<11>*)mFieldPtr)->GetRawData();
   case 12:
      return ((FieldBase<12>*)mFieldPtr)->GetRawData();
   case 13:
      return ((FieldBase<13>*)mFieldPtr)->GetRawData();
   case 14:
      return ((FieldBase<14>*)mFieldPtr)->GetRawData();
   case 15:
      return ((FieldBase<15>*)mFieldPtr)->GetRawData();
   case 16:
      return ((FieldBase<16>*)mFieldPtr)->GetRawData();
   case 17:
      return ((FieldBase<17>*)mFieldPtr)->GetRawData();
   case 18:
      return ((FieldBase<18>*)mFieldPtr)->GetRawData();
   case 19:
      return ((FieldBase<19>*)mFieldPtr)->GetRawData();
   case 20:
      return ((FieldBase<20>*)mFieldPtr)->GetRawData();
   case 21:
      return ((FieldBase<21>*)mFieldPtr)->GetRawData();
   case 22:
      return ((FieldBase<22>*)mFieldPtr)->GetRawData();
   case 23:
      return ((FieldBase<23>*)mFieldPtr)->GetRawData();
   case 24:
      return ((FieldBase<24>*)mFieldPtr)->GetRawData();
   case 25:
      return ((FieldBase<25>*)mFieldPtr)->GetRawData();
   case 26:
      return ((FieldBase<26>*)mFieldPtr)->GetRawData();
   case 27:
      return ((FieldBase<27>*)mFieldPtr)->GetRawData();
   case 28:
      return ((FieldBase<28>*)mFieldPtr)->GetRawData();
   case 29:
      return ((FieldBase<29>*)mFieldPtr)->GetRawData();
   case 30:
      return ((FieldBase<30>*)mFieldPtr)->GetRawData();
   }
   return 0;
}

//! Gets the string value of an enumeration.
//! Returns 'true' if the value is mapped to an enumeration
//! Returns 'false' if an enumeration cannot be found
bool FieldAccessor::GetEnumeration(std::string& aEnum) const
{
   bool                                 found = false;
   std::map<int, std::string>::iterator i     = mAccessorType->mEnumMap.find(GetRawData());
   if (i != mAccessorType->mEnumMap.end())
   {
      aEnum = i->second;
      found = true;
   }
   return found;
}

//! Returns 'true' if the field contains a value meaning 'no statement'
//! Returns 'false' otherwise
bool FieldAccessor::IsNoStatement() const
{
   if (mAccessorType->mNoStatementValue != -1)
   {
      if (GetRawData() == mAccessorType->mNoStatementValue)
      {
         return true;
      }
   }
   return false;
}

//! Returns 'true' if this is a spare field
bool FieldAccessor::IsSpare() const
{
   return mAccessorType->mIsSpare;
}

//! Returns 'true' if this is a disused field
bool FieldAccessor::IsDisused() const
{
   return mAccessorType->mIsDisused;
}

//! If this field has a no statement value, it is set to no statement.
void FieldAccessor::SetNoStatement()
{
   if (mAccessorType->mNoStatementValue != -1)
   {
      SetRawData(mAccessorType->mNoStatementValue);
   }
}

//! Returns a string specifying the human-readable units type
//! used in this field by default.
bool FieldAccessor::GetUnits(std::string& aUnitsType) const
{
   bool unitsDefined = mAccessorType->mHasUnits;
   if (unitsDefined)
   {
      aUnitsType = mAccessorType->mUnitsName;
   }
   return unitsDefined;
}

MessageAccessor::MessageAccessor(const Messages::Base* aMessage)
   : mMessagePtr(aMessage)
{
   int                  id           = aMessage->GetUID();
   MessageAccessorType& accessorType = MessageAccessorType::sMessageAccessorTypes[id];
   mAccessorType                     = &accessorType;
}

//! Returns the FieldAccessor for the field with the index: aIndex
FieldAccessor MessageAccessor::GetField(int aIndex)
{
   MessageAccessorType::FieldData& fieldData = mAccessorType->mFields[aIndex];

   return FieldAccessor(((char*)mMessagePtr) + fieldData.mByteOffset, fieldData.mAccessorType, &fieldData.mName);
}

//! Returns the number of fields in this message
int MessageAccessor::FieldCount()
{
   return static_cast<int>(mAccessorType->mFields.size());
}

} // namespace Messages
} // namespace WsfL16
