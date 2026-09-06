// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSTRINGIDLITERAL_HPP
#define UTSTRINGIDLITERAL_HPP

#include "UtStringId.hpp"

//! returns a value that is implicitly convertable to a UtStringId that was globally initialized
//! during application startup with the provided cstring_literal.
//! A UtStringId is not directly returned to avoid misusing UtStringIdLiteral in contexts that
//! use a std::string or const char*
#define UtStringIdLiteral(cstring_literal)                            \
   (                                                                  \
      []()                                                            \
      {                                                               \
         struct compile_time_str                                      \
         {                                                            \
            const char* chars = cstring_literal;                      \
         };                                                           \
         return ::ut::detail::UtStringIdLiteralP<compile_time_str>{}; \
      }())


// ============================================
// = UtStringIdLiteral implementation details =
// ============================================
namespace ut
{
namespace detail
{

template<class Str>
struct UtStringIdLiteralP
{
   static const UtStringId cId;
   //! convertible only to StringId, not std::string or const char*
   operator UtStringId() const noexcept { return cId; }
};
template<class Str>
const UtStringId UtStringIdLiteralP<Str>::cId = Str{}.chars;

} // end namespace detail
} // end namespace ut

#endif