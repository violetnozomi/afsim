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
#ifndef UTCOMPILERATTRIBUTES_HPP
#define UTCOMPILERATTRIBUTES_HPP

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// Platform / Compiler independent attributes / hints
//
//
// Forced inline usage:
// UT_INLINE_DECL   void myMethodOrFunction(int arg1, char* arg2)
//
// or  void myFunction(int) UT_INLINE_ATTR;    ( only in gcc )
//
#ifdef __GNUC__
#define UT_INLINE_ATTR __attribute__((always_inline)) inline
#define UT_FLATTEN_ATTR __attribute__((flatten))
#else
#define UT_INLINE_ATTR
#define UT_FLATTEN_ATTR
#endif

#ifdef _MSC_VER
#define UT_INLINE_DECL __forceinline
#else
#define UT_INLINE_DECL __attribute__((always_inline)) inline
#endif

// Cache Prefetch
// UT_PREFETCH(ptr);  -- Loads the cache line containing the address if not already loaded
#ifdef __GNUC__
#define UT_PREFETCH(addr) __builtin_prefetch(addr)
#elif defined(_MSC_VER)
#include <intrin.h>
#define UT_PREFETCH(addr) _m_prefetch(addr)
#endif

//! MSVC does not set the pre-defined '__cplusplus' macro to the correct value,
//!    but does provide the '_MSVC_LANG' macro with the correct value.
//! Define 'CPP_VERSION' as a platform-independent way to get the language version.
#ifdef _WIN32
#define CPP_VERSION _MSVC_LANG
#else
#define CPP_VERSION __cplusplus
#endif

//! Convenience macros for C++14
#if CPP_VERSION >= 201402L
#define CPP14_DEPRECATED [[deprecated]]
#define CPP14_DEPRECATED_REASON(reason) [[deprecated(reason)]]
#elif defined(__GNUC__)
#define CPP14_DEPRECATED __attribute__((deprecated))
#define CPP14_DEPRECATED_REASON(reason) __attribute__((deprecated(reason)))
#elif defined(_WIN32)
#define CPP14_DEPRECATED __declspec(deprecated)
#define CPP14_DEPRECATED_REASON(reason) __declspec(deprecated(reason))
#endif

//! Convenience macros for C++17
#if CPP_VERSION >= 201703L
#define CPP17_FALLTHROUGH [[fallthrough]]
#define CPP17_NODISCARD [[nodiscard]]
#elif defined(__GNUC__)
#define CPP17_FALLTHROUGH
#define CPP17_NODISCARD __attribute__((warn_unused_result))
#else
#define CPP17_FALLTHROUGH
#define CPP17_NODISCARD
#endif

//! Convenience macros for C++20
#if CPP_VERSION >= 202002L
#define CPP20_NODISCARD_REASON(reason) [[nodiscard(reason)]]
#define CPP20_LIKELY [[likely]]
#define CPP20_UNLIKELY [[unlikely]]
#define CPP20_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define CPP20_NODISCARD_REASON(reason) CPP17_NODISCARD
#define CPP20_LIKELY
#define CPP20_UNLIKELY
#define CPP20_NO_UNIQUE_ADDRESS
#endif

#endif
