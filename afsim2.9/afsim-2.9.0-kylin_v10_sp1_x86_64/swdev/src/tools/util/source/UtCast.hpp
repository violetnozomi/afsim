// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTCAST_HPP
#define UTCAST_HPP

#include <limits>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include "UtTypeInfo.hpp"
#include "UtTypeTraits.hpp"

namespace ut
{
constexpr size_t npos = static_cast<size_t>(-1);

namespace detail
{
template<typename TO, typename FROM>
TO cast_helper(FROM aValue, bool aOutOfRange)
{
   if (aOutOfRange)
   {
      std::stringstream ss;
      ss << "Error casting from " << ut::TypeNameOf<FROM>() << " to " << ut::TypeNameOf<TO>() << ": Value = " << aValue;
      throw std::out_of_range(ss.str());
   }
   return static_cast<TO>(aValue);
}
} // namespace detail

// Safe casting from signed to signed or unsigned to unsigned
template<typename TO, typename FROM>
ut::enable_if_t<std::is_signed<TO>::value == std::is_signed<FROM>::value, TO> safe_cast(FROM aValue)
{
   return detail::cast_helper<TO, FROM>(aValue,
                                        aValue < std::numeric_limits<TO>::lowest() ||
                                           aValue > std::numeric_limits<TO>::max());
}

// Safe casting from unsigned to signed
template<typename TO, typename FROM>
ut::enable_if_t<std::is_signed<TO>::value && std::is_unsigned<FROM>::value, TO> safe_cast(FROM aValue)
{
   using UTO = ut::make_unsigned_t<TO>;
   return detail::cast_helper<TO, FROM>(aValue, aValue > static_cast<UTO>(std::numeric_limits<TO>::max()));
}

// Safe casting from signed to unsigned
template<typename TO, typename FROM>
ut::enable_if_t<std::is_unsigned<TO>::value && std::is_signed<FROM>::value, TO> safe_cast(FROM aValue)
{
   using UFROM = ut::make_unsigned_t<FROM>;
   return detail::cast_helper<TO, FROM>(aValue, aValue < 0 || static_cast<UFROM>(aValue) > std::numeric_limits<TO>::max());
}

// Convenience method to cast from size_t to int
inline int cast_to_int(size_t aValue)
{
   return safe_cast<int>(aValue);
}

// Convenience method to cast from int to size_t
inline size_t cast_to_size_t(int aValue)
{
   return safe_cast<size_t>(aValue);
}
} // namespace ut


#endif // !UTCAST_HPP
