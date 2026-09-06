// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_REFLECT_DERIVE_HPP
#define UT_REFLECT_DERIVE_HPP

#include <type_traits>

#include "UtReflectNode.hpp"

namespace ut
{
namespace reflect
{
//! CRTP template base class for deriving new node types.
//! The value added is overriding the NodeTypeCount and GetNodeType methods.
//! These overrides allow @see ut::reflect::Visitor to select the best
//!    function when visiting nodes.
//! Example:
//! @code
//! struct Foo : ut::reflect::Derive<Foo, Base>
//! @endcode
template<typename DERIVED, typename BASE>
struct Derive : BASE
{
   static_assert(std::is_base_of<Node, BASE>::value, "BASE must derive from Node.");

   using ReflectBase = Derive<DERIVED, BASE>;

   using BASE::BASE;

   //! Constexpr variable moves computations to compile time.
   static constexpr int cNODE_TYPE_COUNT = BASE::cNODE_TYPE_COUNT + 1;

   //! Returns the number of types in the hierarchy.
   int NodeTypeCount() const override { return cNODE_TYPE_COUNT; }

   //! If aIndex is 0, returns the derived type.
   //! Otherwise, forwards to the base type.
   std::type_index GetNodeType(int aIndex) const override
   {
      if (aIndex == 0)
      {
         return typeid(DERIVED);
      }
      return BASE::GetNodeType(aIndex - 1);
   }
};

//! Due to dependent name lookup rules, 'using ReflectBase::ReflectBase;` does not work for templated classes.
//! This alias should help alleviate this.
template<typename T>
using ReflectBaseT = typename T::ReflectBase;

namespace detail
{
template<typename D, typename T>
struct IsDerivedHelper : std::false_type
{
};

template<typename U, typename T>
struct IsDerivedHelper<Derive<T, U>, T> : std::is_base_of<Derive<T, U>, T>
{
};
} // namespace detail

//! A type trait for determining whether a type T properly derives from ut::reflect::Derive<T, X>.
//! This should typically be used in conjunction with std::is_base_of.
template<typename T>
struct is_derived : detail::IsDerivedHelper<ReflectBaseT<T>, T>
{
};
} // namespace reflect
} // namespace ut

#endif
