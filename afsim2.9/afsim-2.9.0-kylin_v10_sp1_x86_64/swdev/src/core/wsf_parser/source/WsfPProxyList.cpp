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

#include "WsfPProxyList.hpp"

WsfPProxyList::WsfPProxyList(const WsfPProxyListType* aTypePtr)
   : mListTypePtr(aTypePtr)
   , mIsInherited(false)
   , mIsDeleted(false)
{
   mFiller[0] = mFiller[1] = mFiller[2] = 0;
}

size_t WsfPProxyList::Size()
{
   return mValues.size();
}

void WsfPProxyList::PushBack()
{
   mValues.push_back(WsfPProxyValue::ConstructNew(mListTypePtr->GetDataType()).GetDataPtr());
}
void WsfPProxyList::PushBack(WsfPProxyValue aValue)
{
   mValues.push_back(aValue.GetDataPtr());
}
void WsfPProxyList::Insert(WsfPProxyValue aValue, size_t aIndex)
{
   assert(aIndex <= mValues.size());
   mValues.insert(mValues.begin() + aIndex, aValue.GetDataPtr());
}
void WsfPProxyList::Remove(size_t aIndex)
{
   assert(aIndex <= mValues.size());
   Get(aIndex).Delete();
   mValues.erase(mValues.begin() + aIndex);
}
WsfPProxyValue WsfPProxyList::RemoveNoErase(size_t aIndex)
{
   assert(aIndex <= mValues.size());
   WsfPProxyValue val = Get(aIndex);
   mValues.erase(mValues.begin() + aIndex);
   return val;
}

void WsfPProxyList::PopBack()
{
   if (!mValues.empty())
   {
      Back().Delete();
      mValues.pop_back();
   }
}


WsfPProxyValue WsfPProxyList::Get(size_t aIndex)
{
   if (!mValues.empty())
   {
      if (mValues.size() > aIndex)
      {
         void* dataPtr = mValues.at(aIndex);
         if (dataPtr != nullptr)
         {
            return WsfPProxyValue(dataPtr, GetDataType());
         }
      }
      return WsfPProxyValue();
   }
   return WsfPProxyValue();
}

bool WsfPProxyList::Set(size_t aIndex, WsfPProxyValue aValue)
{
   WsfPProxyValue val(mValues.at(aIndex), GetDataType());
   if (val.GetType() == aValue.GetType())
   {
      val.Swap(aValue);
      aValue.Delete();
      return true;
   }
   return false;
}


WsfPProxyValue WsfPProxyList::Back()
{
   return WsfPProxyValue(mValues.back(), GetDataType());
}

WsfPProxyType* WsfPProxyList::GetDataType() const
{
   return mListTypePtr->GetDataType();
}

void WsfPProxyList::Clear()
{
   WsfPProxyType* thisTypePtr = mListTypePtr->GetDataType();
   for (size_t i = 0; i < mValues.size(); ++i)
   {
      void* valuePtr = mValues[i];
      thisTypePtr->Destroy(valuePtr);
      delete[](char*) valuePtr;
   }
   mValues.clear();
}

WsfPProxyList::~WsfPProxyList()
{
   Clear();
}

size_t WsfPProxyList::MemoryUsage() const
{
   size_t usage = sizeof(*this);
   for (size_t i = 0; i < mValues.size(); ++i)
   {
      usage += GetDataType()->MemoryUsage(mValues[i]);
   }
   return usage;
}


void WsfPProxyListType::Construct(void* aValuePtr) const
{
   new (aValuePtr) WsfPProxyList(this);
}

void WsfPProxyListType::Destroy(void* aValuePtr) const
{
   WsfPProxyList* ptr = (WsfPProxyList*)aValuePtr;
   ptr->~WsfPProxyList();
}

WsfPProxyValue WsfPProxyListType::GetAtIndex(void* aPtr, size_t aIndex) const
{
   WsfPProxyList* listPtr = (WsfPProxyList*)aPtr;
   return listPtr->Get(aIndex);
}

void WsfPProxyListType::Copy(void* dest, void* src, int flags) const
{
   WsfPProxyList* destList = (WsfPProxyList*)dest;
   WsfPProxyList* srcList  = (WsfPProxyList*)src;
   destList->Clear();

   size_t listSize = srcList->Size();
   destList->mValues.resize(listSize);
   for (size_t i = 0; i < listSize; ++i)
   {
      WsfPProxyValue valCopy = srcList->Get(i).Copy(flags);
      destList->mValues[i]   = valCopy.GetDataPtr();
   }
   destList->mIsInherited = false;
}
/*
WsfPProxyHash WsfPProxyListType::Hash(void* aDataPtr) const
{
   UtSHA sha;
   WsfPProxyList* list = (WsfPProxyList*)aDataPtr;
   for (size_t i = 0; i < list->mValues.size(); ++i)
   {
      mTypePtr->Hash(list->mValues[i]).AddData(sha);
   }
   UtSHA_Digest digest;
   sha.FinalDigest(digest);
   return digest;
}
*/

WsfPProxyListType::WsfPProxyListType(WsfPProxyType* aDataTypePtr)
   : WsfPProxyType(WsfProxy::cLIST)
   , mTypePtr(aDataTypePtr)
{
   mDataSize = sizeof(WsfPProxyList);
}

bool WsfPProxyListType::IsUnset(void* aValuePtr) const
{
   auto* ptr = static_cast<WsfPProxyList*>(aValuePtr);
   return ptr->mValues.empty();
}

void WsfPProxyListType::SetUnset(void* aValuePtr) const
{
   auto* ptr = static_cast<WsfPProxyList*>(aValuePtr);
   ptr->Clear();
}

bool WsfPProxyListType::IsInherited(void* aValuePtr) const
{
   auto* ptr = static_cast<WsfPProxyList*>(aValuePtr);
   return ptr->mIsInherited;
}

void WsfPProxyListType::SetInherited(void* aValuePtr, bool aIsInherited) const
{
   auto* ptr         = static_cast<WsfPProxyList*>(aValuePtr);
   ptr->mIsInherited = aIsInherited;
}

bool WsfPProxyListType::SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const
{
   WsfPProxyList* listPtr = (WsfPProxyList*)aPtr;

   size_t idx = aKey.GetIndex();
   if (idx >= listPtr->Size())
   {
      return false;
   }

   WsfPProxyValue val(listPtr->mValues[idx], GetDataType());

   if (val.GetType() == aValue.GetType() || (aValue.GetType() == nullptr))
   {
      void* tmpPtr          = aValue.GetDataPtr();
      aValue                = val;
      listPtr->mValues[idx] = tmpPtr;
      return true;
   }
   return false;
}

WsfPProxyType* WsfPProxyListType::GetContainedType(size_t aIndex) const
{
   return mTypePtr;
}

size_t WsfPProxyListType::MemoryUsage(void* aDataPtr) const
{
   WsfPProxyList* listPtr = (WsfPProxyList*)aDataPtr;
   if (listPtr)
   {
      return listPtr->MemoryUsage();
   }
   return 0;
}

size_t WsfPProxyListType::GetAttrCount(void* aDataPtr) const
{
   WsfPProxyList* listPtr = (WsfPProxyList*)aDataPtr;
   if (listPtr)
   {
      return listPtr->Size();
   }
   return 0;
}
