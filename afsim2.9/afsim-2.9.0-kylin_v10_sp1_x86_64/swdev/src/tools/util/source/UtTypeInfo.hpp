// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_TYPE_INFO_HPP
#define UT_TYPE_INFO_HPP

#include "ut_export.h"

#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

namespace ut
{
//! Returns the unmangled name of the type referred to by aType.
//! The returned string has "class", "struct", "union", "__ptr64", "__cdecl", and other undesired decorators removed.
//! Anonymous namespace names have been replaced with "`anonymous`" (back ticks included).
//! See test_uttypeinfo.cpp for examples of the returned type strings.
//! DOES NOT SUPPORT THE FOLLOWING:
//! - Lambdas
//! - Unnamed enums
//! - Function-local types
UT_EXPORT std::string GetTypeName(const std::type_index& aType);

//! Wrapper for @see GetTypeName
template<typename T>
std::string TypeNameOf()
{
   return GetTypeName(typeid(T));
}

//! Wrapper for @see GetTypeName
//! Restricted to polymorphic types to avoid unintentional misuse.
template<typename T>
std::string TypeNameOf(const T& aObject)
{
   static_assert(std::is_polymorphic<T>::value, "Use 'ut::TypeNameOf<T>()' for non-polymorphic types.");
   return GetTypeName(typeid(aObject));
}
} // namespace ut

#endif
