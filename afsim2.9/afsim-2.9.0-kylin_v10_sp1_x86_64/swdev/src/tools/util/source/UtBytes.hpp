// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_BYTES_HPP
#define UT_BYTES_HPP

#include "UtTypeTraits.hpp"

namespace ut
{

enum class byte : unsigned char
{
};

template<class IntegerType>
using is_integral_t = ut::enable_if_t<std::is_integral<IntegerType>::value, byte>;

template<class IntegerType>
constexpr is_integral_t<IntegerType> to_integer(byte b) noexcept
{
   return IntegerType(b);
}

template<class IntegerType>
constexpr is_integral_t<IntegerType>& operator<<=(byte& b, IntegerType shift) noexcept
{
   return b = b << shift;
}

template<class IntegerType>
constexpr is_integral_t<IntegerType>& operator>>=(byte& b, IntegerType shift) noexcept
{
   return b = b >> shift;
}

template<class IntegerType>
constexpr is_integral_t<IntegerType> operator<<(byte b, IntegerType shift) noexcept
{
   return byte(static_cast<unsigned int>(b) << shift);
}

template<class IntegerType>
constexpr is_integral_t<IntegerType> operator>>(byte b, IntegerType shift) noexcept
{
   return byte(static_cast<unsigned int>(b) >> shift);
}

constexpr byte operator|(byte l, byte r) noexcept
{
   return byte(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
}

constexpr byte operator&(byte l, byte r) noexcept
{
   return byte(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
}

constexpr byte operator^(byte l, byte r) noexcept
{
   return byte(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
}

constexpr byte operator~(byte b) noexcept
{
   return byte(~static_cast<unsigned int>(b));
}

// Can't mark these as constexpr until C++17
inline byte& operator|=(byte& l, byte r) noexcept
{
   return (l = l | r);
}
inline byte& operator&=(byte& l, byte r) noexcept
{
   return l = l & r;
}
inline byte& operator^=(byte& l, byte r) noexcept
{
   return l = l ^ r;
}

// constexpr byte& operator|=(byte& l, byte r) noexcept { return l = l | r; }
// constexpr byte& operator&=(byte& l, byte r) noexcept { return l = l & r; }
// constexpr byte& operator^=(byte& l, byte r) noexcept { return l = l ^ r; }

} // namespace ut

#include <cstddef>

#endif
