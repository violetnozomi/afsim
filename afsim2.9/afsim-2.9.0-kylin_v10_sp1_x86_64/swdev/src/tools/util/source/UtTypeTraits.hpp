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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTTYPETRAITS_HPP
#define UTTYPETRAITS_HPP

#include <type_traits>

// This header implements some helper types from <type_traits> that are not available in the
// current C++ standard version.
namespace ut
{
//! C++14
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

//! C++14
template<class T>
using make_unsigned_t = typename std::make_unsigned<T>::type;

//! C++14
template<class T>
using remove_extent_t = typename std::remove_extent<T>::type;

//! C++14
template<class T>
using decay_t = typename std::decay<T>::type;

//! C++14
template<class T>
using remove_const_t = typename std::remove_const<T>::type;

//! C++14
template<class T>
using remove_cv_t = typename std::remove_cv<T>::type;

//! C++14
template<class T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

//! C++14
template<class T>
using remove_reference_t = typename std::remove_reference<T>::type;

//! C++14
template<class T>
using underlying_type_t = typename std::underlying_type<T>::type;

//! C++14
template<bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

//! C++14
using true_type = std::integral_constant<bool, true>;
//! C++14
using false_type = std::integral_constant<bool, false>;

//! C++17
template<class...>
using void_t = void;

//! C++17
template<class...>
struct conjunction : true_type
{
};
template<class B1>
struct conjunction<B1> : B1
{
};
template<class B1, class... Bn>
struct conjunction<B1, Bn...> : conditional_t<bool(B1::value), conjunction<Bn...>, B1>
{
};

//! C++17
template<class...>
struct disjunction : false_type
{
};
template<class B1>
struct disjunction<B1> : B1
{
};
template<class B1, class... Bn>
struct disjunction<B1, Bn...> : conditional_t<bool(B1::value), B1, disjunction<Bn...>>
{
};

//! C++17
template<class B>
struct negation : std::integral_constant<bool, !bool(B::value)>
{
};
} // namespace ut

#endif
