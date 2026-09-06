// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef PROFILINGTIMEUTIL_HPP
#define PROFILINGTIMEUTIL_HPP

#include <chrono>
#include <cstdint>
#include <string>

// Use "if constexpr" if C++17 or greater.
#if __cplusplus >= 201703L
#define AFSIM_PROFILING_IF_CONSTEXPR if constexpr
#else
#define AFSIM_PROFILING_IF_CONSTEXPR if
#endif

// Permit constexpr functions returning std::string, if C++20 or greater.
// std::string only gets constexpr constructors, etc. in C++20.
#if __cplusplus >= 202002L
#include <concepts>
#define AFSIM_PROFILING_CONSTEXPR_STRING_RETURN constexpr std::string
#else
#define AFSIM_PROFILING_CONSTEXPR_STRING_RETURN std::string
#endif

namespace profiling
{
namespace impl
{
/// @brief Fall-back implementation of @c profiling::DurationUnitString
///    (see below) for unsupported std::ratio types.
template<std::intmax_t Num, std::intmax_t Den>
AFSIM_PROFILING_CONSTEXPR_STRING_RETURN DurationUnitStringFallback(const std::ratio<Num, Den> /* aRatio */)
{
   return std::string("(") + std::to_string(Num) + "/" + std::to_string(Den) + ")";
}

} // namespace impl

/// Human-readable string representing a time duration unit.
///
/// @param aRatio The @c period type of a @c std::chrono::duration.
///
/// We need this because @c operator<< for @c std::chrono::duration
/// is a C++20 feature.  Marking this function constexpr would require C++20,
/// but in that case, we would just use operator<< for duration.
template<std::intmax_t Num, std::intmax_t Den>
AFSIM_PROFILING_CONSTEXPR_STRING_RETURN DurationUnitString(const std::ratio<Num, Den> aRatio)
{
   // "if constexpr" requires C++17, but it would be nice to use if available,
   // so we define a macro for it.  Single-quote integer separators require C++14.
   AFSIM_PROFILING_IF_CONSTEXPR(Num == 1)
   {
      AFSIM_PROFILING_IF_CONSTEXPR(Den == 1) { return "s"; }
      else AFSIM_PROFILING_IF_CONSTEXPR(Den == 1000) { return "ms"; }
      else AFSIM_PROFILING_IF_CONSTEXPR(Den == 1000000) { return "μs"; }
      else AFSIM_PROFILING_IF_CONSTEXPR(Den == 1000000000) { return "ns"; }
      else { return impl::DurationUnitStringFallback(aRatio); }
   }
   else { return impl::DurationUnitStringFallback(aRatio); }
}

// We need this because operator<< for duration is a C++20 feature.
// Marking this function constexpr would require C++20,
// but in that case, we would just use operator<< for duration.
//
// With C++20, we could constrain Rep as std::integral.
template<class Rep, std::intmax_t Num, std::intmax_t Den>
#if __cplusplus >= 202002L
requires std::integral<Rep>
#endif
   std::string DurationString(const std::chrono::duration<Rep, std::ratio<Num, Den>> aDuration)
{
   using Duration = std::chrono::duration<Rep, std::ratio<Num, Den>>;
   using Period   = typename Duration::period;
   return std::to_string(aDuration.count()) + " " + profiling::DurationUnitString(Period{});
}

/// String representing the current time.
///
/// This uses the system clock, because that is the only C++ clock
/// that can map its time points to C-style time.
/// We'll need that feature until we can use C++20 std::format for time points.
std::string TimeString(const std::chrono::system_clock::time_point now);

} // namespace profiling

#endif
