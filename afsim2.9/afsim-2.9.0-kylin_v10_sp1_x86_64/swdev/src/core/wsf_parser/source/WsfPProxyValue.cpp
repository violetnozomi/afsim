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

#include "WsfPProxyValue.hpp"

#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyStructType.hpp"

// static
WsfPProxyValue WsfPProxyValue::ConstructNew(const WsfPProxyType* aType)
{
   if (aType != nullptr)
   {
      void* bufferPtr = new char[aType->GetDataSize()];
      aType->Construct(bufferPtr);
      return WsfPProxyValue(bufferPtr, aType);
   }
   return WsfPProxyValue();
}

WsfPProxyValue WsfPProxyValue::GetAtIndex(size_t aIndex) const
{
   return mTypePtr ? mTypePtr->GetAtIndex(mDataPtr, aIndex) : WsfPProxyValue();
}

size_t WsfPProxyValue::GetAttrCount() const
{
   if (mTypePtr)
   {
      return mTypePtr->GetAttrCount(mDataPtr);
   }
   return 0;
}

bool WsfPProxyValue::IsBasicType() const
{
   return mTypePtr && GetType()->IsBasicType();
}

bool WsfPProxyValue::IsStruct() const
{
   return mTypePtr && GetType()->IsStruct();
}

std::string WsfPProxyValue::NameAtIndex(size_t aIndex) const
{
   if (mTypePtr != nullptr)
   {
      return mTypePtr->GetNameAtIndex(aIndex);
   }
   return std::string();
}

WsfPProxyList* WsfPProxyValue::GetList() const
{
   if (mTypePtr && mTypePtr->IsList())
   {
      return (WsfPProxyList*)mDataPtr;
   }
   return nullptr;
}

WsfPProxyObjectMap* WsfPProxyValue::GetObjectMap() const
{
   if (mTypePtr && mTypePtr->IsObjectMap())
   {
      return (WsfPProxyObjectMap*)mDataPtr;
   }
   return nullptr;
}

void WsfPProxyValue::SetUnset()
{
   if (mTypePtr)
   {
      mTypePtr->SetUnset(mDataPtr);
   }
}

void WsfPProxyValue::ClearUnset()
{
   if (mTypePtr)
   {
      mTypePtr->ClearUnset(mDataPtr);
   }
}
bool WsfPProxyValue::IsInherited() const
{
   if (mTypePtr)
   {
      return mTypePtr->IsInherited(mDataPtr);
   }
   return false;
}

void WsfPProxyValue::SetInherited(bool aIsInherited)
{
   if (mTypePtr)
   {
      mTypePtr->SetInherited(mDataPtr, aIsInherited);
   }
}

bool WsfPProxyValue::Copy(WsfPProxyValue aSrc, int flags) const
{
   if (mTypePtr && aSrc.mTypePtr->IsTypeOf(mTypePtr))
   {
      mTypePtr->Copy(mDataPtr, aSrc.mDataPtr, flags);
      return true;
   }
   return false;
}

WsfPProxyValue WsfPProxyValue::Copy(int flags) const
{
   if (mTypePtr)
   {
      WsfPProxyValue r = ConstructNew(mTypePtr);
      mTypePtr->Copy(r.mDataPtr, mDataPtr, flags);
      return r;
   }
   else
   {
      return WsfPProxyValue();
   }
}

WsfPProxyValue WsfPProxyValue::GetAttr(const std::string& aName) const
{
   return mTypePtr ? mTypePtr->GetAttr(mDataPtr, aName) : WsfPProxyValue();
}

WsfPProxyValue WsfPProxyValue::GetAttr(size_t aIndex) const
{
   return mTypePtr ? mTypePtr->GetAtIndex(mDataPtr, aIndex) : WsfPProxyValue();
}

bool WsfPProxyValue::IsUnset() const
{
   return !mTypePtr || mTypePtr->IsUnset(mDataPtr);
}

void WsfPProxyValue::Delete()
{
   if (mTypePtr)
   {
      mTypePtr->Destroy(mDataPtr);
      delete[](char*) mDataPtr;
   }
   mTypePtr = nullptr;
}

WsfPProxyHash WsfPProxyValue::Hash()
{
   if (mTypePtr)
   {
      return mTypePtr->Hash(mDataPtr);
   }
   return WsfPProxyHash();
}

WsfPProxyValue WsfPProxyValue::Lookup(const WsfPProxyPath& aPath, size_t aElements) const
{
   size_t         maxIndex = std::min(aElements, aPath.size());
   WsfPProxyValue val      = *this;
   for (size_t i = 0; (i < maxIndex) && val; ++i)
   {
      const WsfPProxyKey& e = aPath[i];
      if (e.IsString())
      {
         val = val.GetAttr(e.GetMapKey());
      }
      else
      {
         val = val.GetAtIndex(e.GetIndex());
      }
   }
   return val;
}

size_t WsfPProxyValue::GetMemberIndex(const std::string&) const
{
   return ut::npos;
}

WsfPProxyType* WsfPProxyValue::GetMemberType(const std::string&) const
{
   return nullptr;
}

WsfPProxyType* WsfPProxyValue::GetMemberType(size_t) const
{
   return nullptr;
}

WsfPProxyValue WsfPProxyValue::Lookup(const WsfParseTypePath& aPath) const
{
   WsfPProxyValue val = *this;
   for (size_t i = 0; i < aPath.size() && val; ++i)
   {
      val = val.GetAttr(aPath[i].Get());
   }
   return val;
}


bool WsfPProxyValue::SwapAttr(const WsfPProxyKey& aEntry, WsfPProxyValue& aValue) const
{
   if (!mTypePtr)
   {
      return false;
   }
   return mTypePtr->SwapAttr(mDataPtr, aEntry, aValue);
}

void WsfPProxyValue::SetValue(const std::string& aText)
{
   if (mTypePtr && mTypePtr->IsBasicType())
   {
      WsfPProxyBasicType* typePtr = (WsfPProxyBasicType*)mTypePtr;
      typePtr->SetValue(mDataPtr, aText);
   }
}

void WsfPProxyValue::SetBoolValue(bool value)
{
   SetValue(value ? "true" : "false");
}

std::string WsfPProxyValue::ValueToString() const
{
   if (mTypePtr && mTypePtr->IsBasicType())
   {
      WsfPProxyBasicType* typePtr = (WsfPProxyBasicType*)mTypePtr;
      return typePtr->ToString(mDataPtr);
   }
   else if (mTypePtr && mTypePtr->IsStruct())
   {
      auto* typePtr = static_cast<const WsfPProxyStructType*>(mTypePtr);
      return typePtr->GetTypeName();
   }
   return "";
}

WsfPProxyPath* WsfPProxyValue::GetBasePath() const
{
   if (!mTypePtr)
   {
      return nullptr;
   }
   if (!mTypePtr->IsStruct())
   {
      return nullptr;
   }
   WsfPProxyStructHeader* hdr = (WsfPProxyStructHeader*)mDataPtr;
   if (hdr && !hdr->GetBasePath().Empty())
   {
      return &hdr->GetBasePath();
   }
   return nullptr;
}

bool WsfPProxyValue::IsBoolValue()
{
   return mTypePtr && mTypePtr->mTypeStoredKind == WsfProxy::cBOOL_VALUE;
}

bool WsfPProxyValue::GetIntegerValue(int& aValue)
{
   WsfProxy::Int* intPtr = *this;
   if (intPtr && !intPtr->IsUnset())
   {
      aValue = intPtr->GetValue();
      return true;
   }
   return false;
}

bool WsfPProxyValue::GetBoolValue()
{
   WsfProxy::Bool* boolPtr = *this;
   if (boolPtr)
   {
      return boolPtr->IsTrue();
   }
   return false; // should call IsBoolValue() first
}

void WsfPProxyValue::DebugOut(std::ostream& aStream, unsigned int aDepth) const
{
   if (mTypePtr && mDataPtr)
   {
      aStream << "(" << GetType()->GetTypeName() << ") : " << ValueToString() << std::endl;

      WsfPProxyList* plist = GetList();
      if (plist)
      {
         for (size_t j = 0; j < plist->Size(); ++j)
         {
            for (unsigned int i = 0; i < aDepth + 1; ++i)
            {
               aStream << "  ";
            }
            aStream << "[" << j << "]";
            plist->Get(j).DebugOut(aStream, aDepth + 1);
         }
      }
      WsfPProxyObjectMap* pmap = GetObjectMap();
      if (pmap)
      {
         std::map<std::string, WsfPProxyValue> values = pmap->GetValues();
         for (auto&& it : values)
         {
            for (unsigned int i = 0; i < aDepth + 1; ++i)
            {
               aStream << "  ";
            }
            aStream << "[" << it.first << "]";
            it.second.DebugOut(aStream, aDepth + 1);
         }
      }

      for (size_t j = 0; j < GetAttrCount(); ++j)
      {
         for (unsigned int i = 0; i < aDepth + 1; ++i)
         {
            aStream << "  ";
         }
         aStream << NameAtIndex(j);
         GetAttr(j).DebugOut(aStream, aDepth + 1);
      }
   }
}

void WsfPProxyValue::SetIntegerValue(int aValue)
{
   WsfProxy::Int* intPtr = *this;
   if (intPtr)
   {
      intPtr->SetValue(aValue);
   }
}

WsfPProxyValue& WsfPProxyValue::operator=(const std::string& aStringValue)
{
   const WsfPProxyType* typePtr = GetType();
   if (typePtr->IsBasicType() && GetDataPtr())
   {
      ((const WsfPProxyBasicType*)typePtr)->SetValue(GetDataPtr(), aStringValue);
   }
   return *this;
}

WsfPProxyValue& WsfPProxyValue::operator=(bool aBoolValue)
{
   return operator=(WsfProxy::Bool(aBoolValue));
}

WsfPProxyValue& WsfPProxyValue::operator=(const char* aStringPtr)
{
   return this->operator=(std::string(aStringPtr));
}

bool WsfPProxyValue::IsAttributeUsable(const std::string& aAttribute) const noexcept
{
   auto attrVal = GetAttr(aAttribute);
   return attrVal && !attrVal.IsUnset();
}
