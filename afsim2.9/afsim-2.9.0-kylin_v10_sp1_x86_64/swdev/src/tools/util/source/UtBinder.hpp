// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTBINDER_HPP
#define UTBINDER_HPP

#include <cstddef>
#include <functional>
#include <type_traits>

//! A helper class to use templated function binding without the need for placeholders.
//!
//! Instead of the normal std::binder(&<function>, this, std::placeholders::_1) this
//! implementation allows the placeholders to be omitted in the bind per
//! UtStd::Bind(&<function>, this)
//!
//! Used to specialize 'is_placeholder' for specializations of a custom template.
//! typically used in bind

//! An integer sequence structure.
template<int...>
struct int_sequence
{
};

template<int N, int... IntSeq>
struct make_int_sequence : make_int_sequence<N - 1, N - 1, IntSeq...>
{
};
template<int... IntSeq>
struct make_int_sequence<0, IntSeq...> : int_sequence<IntSeq...>
{
};

//! Custom placeholder template.
template<int> // begin with 0 here!
struct placeholder_template
{
};

//! Specialization of is_placeholder published to std namespace.
namespace std
{
template<int N>
struct is_placeholder<placeholder_template<N>> : integral_constant<int, N + 1> // the one is important
{
};
}; // namespace std

namespace UtStd
{
// Binds a static function with specified integer sequence for placeholders.
template<typename R, typename... Args, int... IntSeq>
std::function<R(Args...)> Bind(R (*aFunc)(Args...), int_sequence<IntSeq...>)
{
   return std::bind(aFunc, placeholder_template<IntSeq>{}...);
}

// Binds a non-static function with specified integer sequence for placeholders.
template<typename R, typename... Args, typename CT, int... IntSeq>
std::function<R(Args...)> Bind(R (CT::*aFunc)(Args...), CT* aObjPtr, int_sequence<IntSeq...>)
{
   return std::bind(aFunc, aObjPtr, placeholder_template<IntSeq>{}...);
}

// Binds a static function.
template<typename R, typename... Args>
std::function<R(Args...)> Bind(R (*aFunc)(Args...))
{
   return Bind(aFunc, make_int_sequence<sizeof...(Args)>{});
}

// Binds a non-static function.
template<typename R, typename... Args, typename CT>
std::function<R(Args...)> Bind(R (CT::*aFunc)(Args...), CT* aObjPtr)
{
   return Bind(aFunc, aObjPtr, make_int_sequence<sizeof...(Args)>{});
}
}; // namespace UtStd

#endif
