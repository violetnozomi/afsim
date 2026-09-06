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

#include "UtPackSchema.hpp"

#include <array>
#include <limits>
#include <set>
#include <sstream>
#include <typeinfo>

#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "Utml.hpp"

void UtPackType::Read(UtmlObject& aInput)
{
   mTypeName = aInput["id"].GetValue();
   ReadNestedTypes(aInput);
}

void UtPackType::ReadNestedTypes(UtmlObject& aInput)
{
   for (int i = 0; i < aInput.GetObjectCount(); ++i)
   {
#undef GetObject // Avoid conflict with Windows macro
      auto subType = UtPackSchema::ReadType(aInput.GetObject(i), *this);
      if (subType)
      {
         // Must get name first to avoid UB when moving from subType.
         std::string typeName = subType->GetTypeName();
         mNestedTypes.emplace_back(std::move(typeName), std::move(subType));
      }
   }
}

UtPackType* UtPackType::GetRootType()
{
   return mParentType == nullptr ? this : mParentType->GetRootType();
}

const UtPackType* UtPackType::GetRootType() const
{
   return mParentType == nullptr ? this : mParentType->GetRootType();
}

UtPackType::UtPackType(UtPackType* aParentTypePtr)
   : mEncoding(cENC_NONE)
   , mParentType(aParentTypePtr)
{
}

std::string UtPackType::GetTypePath() const
{
   if (!mParentType)
   {
      return mTypeName;
   }
   std::string path = mParentType->GetTypePath();
   if (!path.empty())
   {
      return path + "." + mTypeName;
   }
   return mTypeName;
}

const std::string& UtPackType::GetTypeName() const
{
   return mTypeName;
}

UtPackSchema* UtPackType::GetSchema() const
{
   UtPackNamespace* typePtr = dynamic_cast<UtPackNamespace*>(const_cast<UtPackType*>(GetRootType()));
   return typePtr->GetSchema();
}

UtPackType* UtPackType::FindType(const std::string& aTypeName)
{
   return const_cast<UtPackType*>(const_cast<const UtPackType*>(this)->FindType(aTypeName));
}

const UtPackType* UtPackType::FindType(const std::string& aTypeName) const
{
   std::vector<std::string> words;
   UtStringUtil::Parse(aTypeName, words, '.');
   if (words.empty())
   {
      return nullptr;
   }
   const UtPackType* typePtr = FindTypeP(words[0]);
   for (size_t i = 1; i < words.size() && typePtr; ++i)
   {
      typePtr = typePtr->FindTypeP(words[i]);
   }
   return typePtr;
}

const UtPackType* UtPackType::FindTypeP(const std::string& aTypeName) const
{
   for (const auto& type : mNestedTypes)
   {
      if (type.first == aTypeName)
      {
         return type.second.get();
      }
   }

   if (mParentType)
   {
      return mParentType->FindTypeP(aTypeName);
   }
   return nullptr;
}

void UtPackType::Resolve()
{
   for (auto& type : mNestedTypes)
   {
      type.second->Resolve();
   }
}

bool UtPackType::IsStruct() const
{
   return typeid(*this) == typeid(const UtPackStruct&);
}

bool UtPackType::IsUnion() const
{
   return typeid(*this) == typeid(const UtPackUnion&);
}

bool UtPackType::IsBasicType() const
{
   return typeid(*this) == typeid(const UtPackBasicType&);
}

bool UtPackType::IsEnum() const
{
   return typeid(*this) == typeid(const UtPackEnumType&);
}

bool UtPackType::IsList() const
{
   return typeid(*this) == typeid(const UtPackListType&);
}

UtPackTypeReference::UtPackTypeReference(const std::string& aTypeName, UtPackType* aParentType)
   : mTypeName(aTypeName)
   , mContextType(aParentType)
   , mTypePtr(nullptr)
{
}

void UtPackTypeReference::Resolve()
{
   if (mTypePtr)
   {
      return;
   }
   assert(mContextType);
   mTypePtr = mContextType->FindType(mTypeName);
   if (!mTypePtr)
   {
      std::stringstream ss;
      ss << "Referenced type not defined: " + mTypeName << " from context " << mContextType->GetTypePath();
      throw UtPackException(ss.str());
   }
}

bool UtPackField::Read(UtmlObject& aInput, UtPackType& aContainer)
{
   mName = aInput["id"].GetValue();

   std::string type = aInput["type"].GetValue();
   if (type == "index")
   {
      mIsIndex = true;
      type     = "uint";
   }
   else if (type == "time")
   {
      mIsTime = true;
      type    = "double";
   }
   mType = UtPackTypeReference(type, &aContainer);

   mDefaultValue           = aInput.PropertyValue("default", "");
   std::string optionValue = aInput.PropertyValue("option", "");
   if (!optionValue.empty())
   {
      mOptionIndex = UtStringUtil::ToInt(optionValue);
   }
   std::string bitValue = aInput.PropertyValue("bit", "");
   if (!bitValue.empty())
   {
      mBitIndex = UtStringUtil::ToInt(bitValue);
   }
   mIsObsolete = aInput.PropertyValue("obsolete", "false") != "false";
   return true;
}

void UtPackStruct::Read(UtmlObject& aInput, UtPackType& aContainer)
{
   UtPackType::Read(aInput);
   std::string messageProperty = aInput.PropertyValue("message", "-1");
   if (messageProperty == "virtual")
   {
      mMessageId = -2;
   }
   else
   {
      mMessageId = UtStringUtil::ToInt(messageProperty);
   }
   std::string baseClass = aInput.PropertyValue("base", "");
   if (!baseClass.empty())
   {
      mBaseType = UtPackTypeReference(baseClass, this);
   }
   for (int i = 0; i < aInput.GetObjectCount(); ++i)
   {
      UtmlObject& obj = aInput.GetObject(i);
      if (obj.GetType() == "Field")
      {
         UtPackField field;
         if (field.Read(obj, *this))
         {
            if (field.IsOptional())
            {
               ++mBitsetSize;
            }
            if (field.GetBitIndex() >= 0)
            {
               ++mBitsetSize;
            }
            mFields.push_back(field);
         }
      }
   }
}

UtPackStruct::UtPackStruct(UtPackType& aParentType)
   : UtPackType(&aParentType)
{
   mMessageId  = -1;
   mBitsetSize = 0;
}

void UtPackStruct::Resolve()
{
   std::set<std::string> usedFieldNames;
   UtPackType::Resolve();
   if (!mBaseType.IsNull())
   {
      mBaseType.Resolve();
   }
   std::set<int> bitsUsed;
   for (auto& field : mFields)
   {
      std::string name = field.GetFieldName();

      // Check that no bits are used twice
      std::array<int, 2> bitIndices = {field.GetOptionIndex(), field.GetBitIndex()};
      for (int bit : bitIndices)
      {
         if (bit != -1)
         {
            if (!bitsUsed.insert(bit).second)
            {
               std::stringstream ss;
               ss << "Bit " << bit << " used more than once in " << GetTypePath();
               throw UtPackException(ss.str());
            }
         }
      }

      // check that no fields are used twice
      if (!usedFieldNames.insert(name).second)
      {
         throw UtPackException("Field name not unique: " + name + " in " + GetTypePath());
      }

      field.GetType().Resolve();
   }
}

std::vector<UtPackField> UtPackStruct::GetAllFields() const
{
   if (!GetBaseType().IsNull())
   {
      const auto* base = dynamic_cast<UtPackStruct*>(GetBaseType().GetType());
      if (base != nullptr)
      {
         auto baseFields = base->GetAllFields();
         baseFields.insert(baseFields.end(), mFields.begin(), mFields.end());
         return baseFields;
      }
   }

   return mFields;
}

void UtPackUnion::Read(UtmlObject& aInput, UtPackType& aContainer)
{
   UtPackType::Read(aInput);
   for (int i = 0; i < aInput.GetObjectCount(); ++i)
   {
      UtmlObject& obj = aInput.GetObject(i);
      if (obj.GetType() == "Field")
      {
         UtPackField field;
         if (field.Read(obj, *this))
         {
            if (!field.IsOptional())
            {
               std::stringstream ss;
               ss << "Field " << field.GetFieldName() << " in union " << mTypeName << " must be optional.";
               throw UtPackException(ss.str());
            }
            mFields.push_back(field);
         }
      }
   }
}

UtPackUnion::UtPackUnion(UtPackType& aParentType)
   : UtPackType(&aParentType)
{
}

void UtPackUnion::Resolve()
{
   std::set<std::string> usedFieldNames;
   UtPackType::Resolve();
   std::set<int> bitsUsed;
   for (auto& field : mFields)
   {
      std::string name = field.GetFieldName();

      // Check that no bits are used twice
      std::array<int, 2> bitIndices = {field.GetOptionIndex(), field.GetBitIndex()};
      for (int bit : bitIndices)
      {
         if (bit != -1)
         {
            if (!bitsUsed.insert(bit).second)
            {
               std::stringstream ss;
               ss << "Bit " << bit << " used more than once in " << GetTypePath();
               throw UtPackException(ss.str());
            }
         }
      }

      // check that no fields are used twice
      if (!usedFieldNames.insert(name).second)
      {
         throw UtPackException("Field name not unique: " + name + " in " + GetTypePath());
      }

      field.GetType().Resolve();
   }
}

const std::string& UtPackBasicType::GetCPP_Type() const
{
   return mCPP_Type;
}

void UtPackBasicType::Read(UtmlObject& aInput)
{
   mTypeName               = aInput["id"].GetValue();
   mCPP_Type               = aInput["cpp_type"].GetValue();
   mSize                   = UtStringUtil::ToInt(aInput.PropertyValue("size", "0"));
   std::string encodingVal = aInput.PropertyValue("encoding", "");
   if (!encodingVal.empty())
   {
      if (encodingVal == "le")
      {
         mEncoding = cENC_LITTLE_ENDIAN;
      }
      else if (encodingVal == "varbytes")
      {
         mEncoding = cENC_VAR_BYTES;
      }
      else if (encodingVal == "bit")
      {
         mEncoding = cENC_BIT;
      }
      else
      {
         throw UtPackException("Unknown encoding: " + encodingVal);
      }
   }
}

void UtPackEnumType::Read(UtmlObject& aInput)
{
   UtPackType::Read(aInput);
   for (int i = 0; i < aInput.GetObjectCount(); ++i)
   {
      UtmlObject& child = aInput.GetObject(i);
      if (child.GetType() == "Value")
      {
         std::string valId = child.PropertyValue("id", "");
         if (valId.empty())
         {
            throw UtPackException("enum value has no id in " + mTypeName);
         }
         if (mEnumValues.find(valId) != mEnumValues.end())
         {
            throw UtPackException("enum value not unique: " + mTypeName + "." + valId);
         }


         std::string val = child.PropertyValue("value", "");
         if (val.empty())
         {
            throw UtPackException("No enum value in " + mTypeName + "." + valId);
         }
         std::stringstream ss(val);
         int               valInt(0);
         if (ss >> valInt)
         {
            mEnumValues[valId]   = valInt;
            mValueToName[valInt] = valId;
         }
         else
         {
            throw UtPackException("Enum value not integer: " + mTypeName + "." + valId);
         }
      }
   }
}

UtPackEnumType::UtPackEnumType(UtPackType& aParent)
   : UtPackType(&aParent)
{
}

void UtPackEnumType::Resolve()
{
   // pick a C++ type that fits the values.  prefer smaller, and signed
   int minVal      = mValueToName.begin()->first;
   int maxVal      = (--mValueToName.end())->first;
   mCPP_Type       = "int";
   mUnderlyingType = "int";
   if (minVal >= std::numeric_limits<unsigned short>::min() && maxVal <= std::numeric_limits<unsigned short>::max())
   {
      mCPP_Type       = "unsigned short";
      mUnderlyingType = "uint16";
   }
   if (minVal >= std::numeric_limits<short>::min() && maxVal <= std::numeric_limits<short>::max())
   {
      mCPP_Type       = "short";
      mUnderlyingType = "int16";
   }
   if (minVal >= std::numeric_limits<unsigned char>::min() && maxVal <= std::numeric_limits<unsigned char>::max())
   {
      mCPP_Type       = "unsigned char";
      mUnderlyingType = "uint8";
   }
   if (minVal >= std::numeric_limits<char>::min() && maxVal <= std::numeric_limits<char>::max())
   {
      mCPP_Type       = "char";
      mUnderlyingType = "int8";
   }
}

UtPackBasicType::UtPackBasicType(UtPackType& aParent)
   : UtPackType(&aParent)
{
   mEncoding = cENC_LITTLE_ENDIAN;
   mSize     = 0;
}

void UtPackNamespace::Read(UtmlObject& aInput)
{
   ReadNestedTypes(aInput);
}

UtPackException::UtPackException(const std::string& aReason)
   : mReason(aReason)
{
}

const char* UtPackException::what() const noexcept
{
   return mReason.c_str();
}

std::unique_ptr<UtPackType> UtPackSchema::ReadType(UtmlObject& aInput, UtPackType& aParent)
{
   if (aInput.GetType() == "BasicType")
   {
      auto typePtr = ut::make_unique<UtPackBasicType>(aParent);
      typePtr->Read(aInput);
      // std::move to appease the GCC gods.
      return std::move(typePtr);
   }
   else if (aInput.GetType() == "Struct")
   {
      auto typePtr = ut::make_unique<UtPackStruct>(aParent);
      typePtr->Read(aInput, aParent);
      return std::move(typePtr);
   }
   else if (aInput.GetType() == "Union")
   {
      auto typePtr = ut::make_unique<UtPackUnion>(aParent);
      typePtr->Read(aInput, aParent);
      return std::move(typePtr);
   }
   else if (aInput.GetType() == "List")
   {
      auto typePtr = ut::make_unique<UtPackListType>(aParent);
      typePtr->Read(aInput);
      return std::move(typePtr);
   }
   else if (aInput.GetType() == "Enum")
   {
      auto typePtr = ut::make_unique<UtPackEnumType>(aParent);
      typePtr->Read(aInput);
      return std::move(typePtr);
   }
   return nullptr;
}

UtPackSchema::UtPackSchema()
   : mRoot(std::make_shared<UtPackNamespace>(nullptr))
{
}

void UtPackSchema::Read(UtmlObject& aInput)
{
   for (int i = 0; i < aInput.GetObjectCount(); ++i)
   {
      UtmlObject& obj = aInput.GetObject(i);
      if (obj.GetType() == "Settings")
      {
         mNamespaceName = obj.PropertyValue("namespace", "");
      }
   }
   mRoot->Read(aInput);
}

void UtPackSchema::Resolve()
{
   mRoot->Resolve();
}

UtPackType* UtPackSchema::FindType(const std::string& aTypeName)
{
   return mRoot->FindType(aTypeName);
}

const UtPackType* UtPackSchema::FindType(const std::string& aTypeName) const
{
   return mRoot->FindType(aTypeName);
}

namespace
{
void CollectTypeNames(const UtPackType& aType, std::vector<std::string>& aTypes)
{
   aTypes.push_back(aType.GetTypePath());
   for (const auto& type : aType.GetNestedTypes())
   {
      CollectTypeNames(*type.second, aTypes);
   }
}
} // namespace
std::vector<std::string> UtPackSchema::GetTypeNames() const
{
   std::vector<std::string> typeNames;
   CollectTypeNames(*mRoot, typeNames);
   return typeNames;
}


void UtPackListType::Resolve()
{
   mContainedType.Resolve();
}

bool UtPackListType::Read(UtmlObject& aInput)
{
   UtPackType::Read(aInput);
   std::string typeName = aInput.PropertyValue("type", "");
   if (typeName.empty())
   {
      return false;
   }
   mContainedType = UtPackTypeReference(typeName, this);
   return true;
}
