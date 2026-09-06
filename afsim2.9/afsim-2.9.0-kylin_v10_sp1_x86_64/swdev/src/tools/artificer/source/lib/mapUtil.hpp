// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef MAPUTIL_HPP
#define MAPUTIL_HPP

#include "artificer_export.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace artificer
{
/// @brief Given a map `aCounts` counting the number of occurences of a series of a keys, increments the count for `aKey`
template<typename T>
void IncrementCount(std::map<T, uint64_t>& aCounts, const T& aKey)
{
   const auto found = aCounts.find(aKey);
   if (found == aCounts.end())
   {
      aCounts.insert({aKey, 1});
   }
   else
   {
      found->second++;
   }
}

/// @brief Returns a vector containing pointers to the data in `aMap` in the order defined by `aCompare`
template<typename MapKey, typename MapValue, typename Comparator>
std::vector<const std::pair<const MapKey, MapValue>*> MapOrderedBy(const std::map<MapKey, MapValue>& aMap,
                                                                   const Comparator&                 aCompare)
{
   using ValueType = typename std::map<MapKey, MapValue>::value_type;
   std::vector<const ValueType*> reorderedData;
   reorderedData.reserve(aMap.size());
   for (const ValueType& pair : aMap)
   {
      reorderedData.push_back(&pair);
   }
   std::sort(reorderedData.begin(), reorderedData.end(), aCompare);
   return reorderedData;
}

///@brief Parses a string in format "name1=value1;name2=value2;..." into a { {"name1", "value1"}, {"name2", "value2"}, ... } map
std::map<std::string, std::string> ARTIFICER_EXPORT ParseNameValuePairs(const std::string& tagsStr);

} // namespace artificer

#endif
