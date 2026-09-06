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

#ifndef UT_REFLECT_NODE_HPP
#define UT_REFLECT_NODE_HPP

#include <typeindex>
#include <typeinfo>

namespace ut
{
namespace reflect
{
//! Node is a base type for all data being reflected on.
//! Custom base nodes can be declared by deriving from Node.
//! Further specialized node types should not derive from Node directly.
//! Instead, use @see ut::reflect::Derive as an intermediary.
struct Node
{
   virtual ~Node() noexcept = default;

   //! Constexpr variable moves computations to compile time.
   static constexpr int cNODE_TYPE_COUNT = 0;

   //! Returns the number of layers in the type hierarchy.
   virtual int NodeTypeCount() const = 0;

   //! Returns the type_inexes of the desired layer in the type hierarchy.
   //! Layer 0 is the top-most layer.
   //! Use @see ut::reflect::Derive to define overrides.
   virtual std::type_index GetNodeType(int aIndex) const { throw std::bad_typeid(); }
};
} // namespace reflect
} // namespace ut

#endif
