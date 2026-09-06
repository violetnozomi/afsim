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

#ifndef WSFPPROXYLIST_HPP
#define WSFPPROXYLIST_HPP

#include <vector>

#include "UtMemoryDebug.hpp"

// clang-format off
#include "WsfPProxyValue.hpp"
#include "WsfPProxyType.hpp"
// clang-format on

//
// Proxy lists are list types that may contain a list of proxy value which share the same type.
//
class WSF_PARSER_EXPORT WsfPProxyListType;

// This is an instance of a proxy list
class WSF_PARSER_EXPORT WsfPProxyList
{
public:
   WsfPProxyList(const WsfPProxyListType* aTypePtr);
   ~WsfPProxyList();
   WsfPProxyType* GetDataType() const;
   void           PushBack();
   void           PushBack(WsfPProxyValue aValue);
   void           PopBack();
   void           Insert(WsfPProxyValue aValue, size_t aIndex);
   void           Remove(size_t aIndex);
   WsfPProxyValue RemoveNoErase(size_t aIndex);
   WsfPProxyValue Back();
   WsfPProxyValue Get(size_t aIndex);
   bool           Set(size_t aIndex, WsfPProxyValue aValue);
   size_t         Size();
   void           Clear();
   size_t         MemoryUsage() const;
   // The proxy type for this list
   const WsfPProxyListType* mListTypePtr;
   // The list of values
   std::vector<void*> mValues;
   bool               mIsInherited;
   bool               mIsDeleted;
   char               mFiller[3];
   UT_MEMORY_DEBUG_MARKER(cMDB_PROXY_LIST);
};

// The proxy type class for the proxy list for a particular type
class WSF_PARSER_EXPORT WsfPProxyListType : public WsfPProxyType
{
public:
   WsfPProxyListType(WsfPProxyType* aDataTypePtr);

   void Construct(void* aValuePtr) const override;

   void Destroy(void* aValuePtr) const override;

   void Copy(void* dest, void* src, int flags) const override;

   bool IsUnset(void* aValuePtr) const override;

   void SetUnset(void* aValuePtr) const override;

   bool IsInherited(void* aValuePtr) const override;

   void SetInherited(void* aValuePtr, bool aIsInherited) const override;

   WsfPProxyValue GetAtIndex(void* aPtr, size_t aIndex) const override;
   size_t         GetAttrCount(void* aPtr) const override;
   WsfPProxyType* GetDataType() const { return mTypePtr; }
   WsfPProxyType* GetContainedType(size_t aIndex) const override;

   WsfPProxyValue GetAttr(void*, const std::string&) const override { return WsfPProxyValue(); }
   bool           SwapAttr(void* aPtr, const WsfPProxyKey& aKey, WsfPProxyValue& aValue) const override;
   //    virtual WsfPProxyHash Hash(void* aDataPtr) const;
   size_t MemoryUsage(void* aDataPtr) const override;

private:
   // The type of values the list contains
   WsfPProxyType* mTypePtr;
};
#endif
