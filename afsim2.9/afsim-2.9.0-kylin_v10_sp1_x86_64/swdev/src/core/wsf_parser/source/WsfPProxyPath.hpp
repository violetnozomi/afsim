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

#ifndef WSFPPROXYPATH_HPP
#define WSFPPROXYPATH_HPP
#include "wsf_parser_export.h"

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "UtStringRef.hpp"
#include "WsfPProxyKey.hpp"

// clang-format off
#include "WsfPProxyValue.hpp"
#include "WsfPProxyType.hpp"
// clang-format on

using WsfParseTypePath = std::vector<UtStringRef>;

class WsfPProxyValue;

struct WsfPProxyPathEntryHash
{
   static const int bucket_size = 8;
   static const int min_buckets = 4;
   static size_t    hashString(const std::string& aValue)
   {
      size_t value = 0;
      for (size_t i = 0; i < aValue.size(); ++i)
      {
         value += value * 5 + aValue[i];
      }
      return value;
   }

   size_t operator()(const WsfPProxyKey& x) const
   {
      if (x.IsIndex())
      {
         return x.GetIndex();
      }
      return ~hashString(x.GetMapKey());
   }
   bool operator()(const WsfPProxyKey& _Left, const WsfPProxyKey& _Right) const { return (_Left < _Right); }
};

//! Represents a path or unique key to a value in the proxy.
class WSF_PARSER_EXPORT WsfPProxyPath
{
public:
   WsfPProxyPath() {}
   WsfPProxyPath(const WsfPProxyPath& aRhs)
      : mEntries(aRhs.mEntries)
   {
   }
   WsfPProxyPath(const WsfPProxyPath& aRhs, size_t aSubPathSize)
      : mEntries(aRhs.mEntries.begin(), aRhs.mEntries.begin() + aSubPathSize)
   {
   }
   WsfPProxyPath(const WsfPProxyKey& aEntry) { *this += aEntry; }

   WsfPProxyPath& operator+=(size_t aIndex)
   {
      mEntries.push_back(aIndex);
      return *this;
   }
   WsfPProxyPath& operator+=(const std::string& aStr)
   {
      mEntries.push_back(aStr);
      return *this;
   }
   WsfPProxyPath& operator+=(const WsfPProxyPath& aSuffixPath)
   {
      mEntries.insert(mEntries.end(), aSuffixPath.mEntries.begin(), aSuffixPath.mEntries.end());
      return *this;
   }
   WsfPProxyPath& operator+=(const WsfPProxyKey& aEntry)
   {
      mEntries.push_back(aEntry);
      return *this;
   }
   WsfPProxyPath operator+(const WsfPProxyKey& aEntry) const
   {
      WsfPProxyPath p(*this);
      p += aEntry;
      return p;
   }
   WsfPProxyPath operator+(const WsfPProxyPath& aPath) const
   {
      WsfPProxyPath p(*this);
      for (size_t i = 0; i < aPath.size(); ++i)
      {
         p += aPath[i];
      }
      return p;
   }
   WsfPProxyPath operator+(const std::string& aEntry) const
   {
      WsfPProxyPath p(*this);
      p += aEntry;
      return p;
   }
   bool Push(const WsfPProxyValue& aBaseValue, const std::string& aAttrName);
   bool operator==(const WsfPProxyPath& aRhs) const { return mEntries == aRhs.mEntries; }
   bool operator!=(const WsfPProxyPath& aRhs) const { return !(*this == aRhs); }
   bool operator<(const WsfPProxyPath& aRhs) const
   {
      return std::lexicographical_compare(mEntries.begin(), mEntries.end(), aRhs.mEntries.begin(), aRhs.mEntries.end());
   }
   bool           operator>=(const WsfPProxyPath& aRhs) const { return !(*this < aRhs); }
   WsfPProxyValue Lookup(WsfPProxyValue aRoot);

   static WsfPProxyPath FromString(WsfPProxyValue aRoot, const std::string& aStr);
   static WsfPProxyPath FromStringList(WsfPProxyValue aRoot, const std::vector<std::string>& aStrList);
   static WsfPProxyPath FromTypePath(WsfPProxyValue aRoot, const WsfParseTypePath& aStrList);

   // Return 'true' if this is a prefix of aPath
   bool IsPrefixOf(const WsfPProxyPath& aPath) const
   {
      if (mEntries.size() <= aPath.mEntries.size())
      {
         return std::equal(mEntries.begin(), mEntries.end(), aPath.mEntries.begin());
      }
      return false;
   }

   size_t              size() const { return mEntries.size(); }
   const WsfPProxyKey& operator[](size_t i) const { return mEntries[i]; }
   WsfPProxyKey&       operator[](size_t i) { return mEntries[i]; }
   void                Clear() { mEntries.clear(); }
   void                Swap(WsfPProxyPath& aRhs) { std::swap(mEntries, aRhs.mEntries); }
   std::string         ToString(const WsfPProxyValue& aRootProxy) const;

   std::string              ToString(const WsfPProxyValue& aRootProxy, size_t aEntryIndex) const;
   std::vector<std::string> ToStringList(const WsfPProxyValue& aRootProxy) const;
   void                     Pop() { mEntries.pop_back(); }
   bool                     Empty() const { return mEntries.empty(); }
   WsfPProxyKey&            Back() { return mEntries.back(); }
   const WsfPProxyKey&      Back() const { return mEntries.back(); }

   size_t MemoryUsage() const;

   void Trim(size_t aPathLength)
   {
      if (mEntries.size() > aPathLength)
      {
         mEntries.resize(aPathLength);
      }
   }

private:
   std::vector<WsfPProxyKey> mEntries;
};

namespace std
{
template<>
inline void swap(WsfPProxyPath& a, WsfPProxyPath& b)
{
   a.Swap(b);
}
} // namespace std

using WsfPProxyPathSet = std::set<WsfPProxyPath>;
using WsfPProxyPathMap = std::map<WsfPProxyPath, WsfPProxyPathSet>;

#endif
