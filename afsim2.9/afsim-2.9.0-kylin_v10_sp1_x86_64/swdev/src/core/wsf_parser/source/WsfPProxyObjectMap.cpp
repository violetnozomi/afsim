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

#include "WsfPProxyObjectMap.hpp"

#include <cassert>

void WsfPProxyObjectMapType::Construct(void* aValuePtr) const
{
   new (aValuePtr) WsfPProxyObjectMap(this);
}

void WsfPProxyObjectMapType::Destroy(void* aValuePtr) const
{
   WsfPProxyObjectMap* ptr = (WsfPProxyObjectMap*)aValuePtr;
   ptr->~WsfPProxyObjectMap();
}

void WsfPProxyObjectMapType::Copy(void* dest, void* src, int flags) const
{
   WsfPProxyObjectMap* d = (WsfPProxyObjectMap*)dest;
   WsfPProxyObjectMap* s = (WsfPProxyObjectMap*)src;
   d->Clear();
   d->mTypePtr     = s->mTypePtr;
   d->mValues      = s->mValues;
   d->mIsInherited = false;
   for (auto iter = d->mValues.begin(); iter != d->mValues.end(); ++iter)
   {
      iter->second = iter->second.Copy(flags);
   }
}

WsfPProxyValue WsfPProxyObjectMapType::GetAttr(void* aValuePtr, const std::string& aName) const
{
   WsfPProxyObjectMap*                             ptr    = (WsfPProxyObjectMap*)aValuePtr;
   std::map<std::string, WsfPProxyValue>&          valMap = ptr->GetValues();
   std::map<std::string, WsfPProxyValue>::iterator i      = valMap.find(aName);
   if (i != valMap.end())
   {
      return i->second;
   }
   return WsfPProxyValue();
}

void WsfPProxyObjectMapType::SetUnset(void* aValuePtr) const
{
   WsfPProxyObjectMap* ptr = (WsfPProxyObjectMap*)aValuePtr;
   ptr->Clear();
}

bool WsfPProxyObjectMapType::IsUnset(void* aValuePtr) const
{
   WsfPProxyObjectMap* ptr = (WsfPProxyObjectMap*)aValuePtr;
   return ptr->GetValues().empty();
}

WsfPProxyObjectMap::WsfPProxyObjectMap(const WsfPProxyObjectMap& aRhs)
   : mValues(aRhs.mValues)
   , mIsInherited(false)
{
   mFiller[0] = mFiller[1] = mFiller[2] = 0;
   mTypePtr                             = aRhs.mTypePtr;
   for (auto iter = mValues.begin(); iter != mValues.end(); ++iter)
   {
      iter->second = iter->second.Copy();
   }
}

WsfPProxyObjectMap::~WsfPProxyObjectMap()
{
   Clear();
}

WsfPProxyValue WsfPProxyObjectMap::AddNew(const std::string& aName, WsfPProxyValue aValue, bool aOverrideExisting)
{
   assert(!aValue || aValue.GetType()->IsTypeOf(GetDataType()));
   assert(!aName.empty());
   if (mValues.find(aName) != mValues.end())
   {
      if (!aOverrideExisting)
      {
         return WsfPProxyValue();
      }
      WsfPProxyValue& existing = mValues[aName];
      existing.Delete();
      existing = aValue;
   }
   else
   {
      mValues[aName] = aValue;
   }
   return aValue;
}

WsfPProxyValue WsfPProxyObjectMap::GetAt(const std::string& aName)
{
   assert(!aName.empty());
   auto i = mValues.find(aName);
   if (i != mValues.end())
   {
      return i->second;
   }
   return WsfPProxyValue();
}

WsfPProxyValue* WsfPProxyObjectMap::FindAt(const std::string& aName)
{
   auto i = mValues.find(aName);
   if (i != mValues.end())
   {
      return &i->second;
   }
   return nullptr;
}

bool WsfPProxyObjectMap::Erase(const std::string& aName)
{
   auto       i     = mValues.find(aName);
   const bool found = (i != mValues.end());

   if (found)
   {
      i->second.Delete();
      mValues.erase(i);
   }

   return found;
}

bool WsfPProxyObjectMap::EraseNoDelete(const std::string& aName)
{
   auto       i     = mValues.find(aName);
   const bool found = (i != mValues.end());

   if (found)
   {
      mValues.erase(i);
   }

   return found;
}

bool WsfPProxyObjectMap::UserErase(const std::string& aName)
{
   const bool erased = Erase(aName);

   if (erased)
   {
      mDeletedValues.insert(aName);
   }

   return erased;
}

bool WsfPProxyObjectMap::UserEraseNoDelete(const std::string& aName)
{
   const bool erased = EraseNoDelete(aName);

   if (erased)
   {
      mDeletedValues.insert(aName);
   }

   return erased;
}

bool WsfPProxyObjectMap::UserUndelete(const std::string& aName)
{
   auto       valueIter = mDeletedValues.find(aName);
   const bool found     = (valueIter != mDeletedValues.end());

   if (found)
   {
      mDeletedValues.erase(valueIter);
   }

   return found;
}

bool WsfPProxyObjectMap::HasUserDeleted(const std::string& aName) const
{
   return (mDeletedValues.find(aName) != mDeletedValues.end());
}

WsfPProxyType* WsfPProxyObjectMap::GetDataType()
{
   return ((WsfPProxyObjectMapType*)mTypePtr)->GetDataType();
}

void WsfPProxyObjectMap::Clear()
{
   for (auto i = mValues.begin(); i != mValues.end(); ++i)
   {
      i->second.Delete();
   }
   mValues.clear();
}

size_t WsfPProxyObjectMap::MemoryUsage() const
{
   size_t usage = sizeof(WsfPProxyObjectMap);
   for (auto i = mValues.begin(); i != mValues.end(); ++i)
   {
      usage += sizeof(i->first) + i->first.size();
      usage += sizeof(i->second);
      if (i->second.IsValid())
      {
         usage += i->second.GetType()->MemoryUsage(i->second.GetDataPtr());
      }
   }
   return usage;
}

bool WsfPProxyObjectMapType::SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const
{
   auto*           ptr    = static_cast<WsfPProxyObjectMap*>(aPtr);
   WsfPProxyValue* valPtr = ptr->FindAt(aKey.GetMapKey());
   if (valPtr)
   {
      valPtr->Swap(aValue);
      return true;
   }
   return false;
}

size_t WsfPProxyObjectMapType::MemoryUsage(void* aDataPtr) const
{
   WsfPProxyObjectMap* mapPtr = static_cast<WsfPProxyObjectMap*>(aDataPtr);
   if (mapPtr)
   {
      return mapPtr->MemoryUsage();
   }
   return 0;
}
