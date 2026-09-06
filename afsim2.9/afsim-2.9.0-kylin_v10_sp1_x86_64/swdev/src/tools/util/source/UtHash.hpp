// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTHASH_HPP
#define UTHASH_HPP

#include "UtMath.hpp"

namespace ut
{
// This hash_combine algorithm is borrowed from https://github.com/cbreeden/fxhash, used in Firefox and rustc.

namespace detail
{
constexpr uint8_t  cROTATE = 5;
constexpr uint32_t cSEED32 = 0x9e3779b9;
constexpr uint64_t cSEED64 = 0x517cc1b727220a95;
} // namespace detail

//! @brief Combine two 32 bit hashes.
inline uint32_t HashCombine(uint32_t aHash, uint32_t aOther)
{
   return (UtMath::rotl(aHash, detail::cROTATE) ^ aOther) * detail::cSEED32;
}

//! @brief Combine two 64 bit hashes.
inline uint64_t HashCombine(uint64_t aHash, uint64_t aOther)
{
   return (UtMath::rotl(aHash, detail::cROTATE) ^ aOther) * detail::cSEED64;
}

//! @brief Given N hashes, combine them into a single hash.
//! @return A hash which combines all the given hashes into one.
template<typename... Args>
size_t HashCombine(size_t aHash, size_t aOther, Args... aRest)
{
   return HashCombine(HashCombine(aHash, aOther), aRest...);
}

// @overload
inline size_t HashCombine(size_t aHash)
{
   return aHash;
}

//! @brief Hashes each argument and then combines them into a single hash.
//! @return The combined hash of every argument.
template<typename T, typename... Args>
size_t Hash(T&& aFirst, Args&&... aArgs)
{
   return HashCombine(std::hash<ut::decay_t<T>>{}(std::forward<T>(aFirst)),
                      (std::hash<ut::decay_t<Args>>{}(std::forward<Args>(aArgs)))...);
}

//! @brief Hash function suitable for std::unordered_map allowing you to hash a std::pair.
struct pairhash
{
public:
   template<typename T, typename U>
   std::size_t operator()(const std::pair<T, U>& x) const
   {
      return HashCombine(std::hash<T>()(x.first), std::hash<U>()(x.second));
   }
};

} // namespace ut
#endif
