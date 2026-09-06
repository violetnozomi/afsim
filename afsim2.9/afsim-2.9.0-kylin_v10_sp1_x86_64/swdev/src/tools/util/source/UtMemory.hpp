// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTMEMORY_HPP
#define UTMEMORY_HPP

#include <memory>

#include "UtTypeTraits.hpp"

namespace ut
{
//! Make a unique_ptr.
//! This is a convenience method. When the code base is moved to C++14 or further, this implementation
//! can be removed and all usages of "ut::make_unique" can be converted to "std::make_unique"
template<class T, class... Args>
ut::enable_if_t<!std::is_array<T>::value && std::is_constructible<T, Args...>::value, std::unique_ptr<T>>
make_unique(Args&&... args)
{
   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
ut::enable_if_t<std::is_array<T>::value && std::extent<T>::value == 0, std::unique_ptr<T>> make_unique(std::size_t size)
{
   using U = ut::remove_extent_t<T>;
   return std::unique_ptr<T>(new U[size]());
}

template<class Cloneable>
std::unique_ptr<Cloneable> clone(const Cloneable* aObject)
{
   return std::unique_ptr<Cloneable>(aObject->Clone());
}

template<class Cloneable>
ut::enable_if_t<!std::is_pointer<Cloneable>::value, std::unique_ptr<Cloneable>> clone(const Cloneable& aObject)
{
   return std::unique_ptr<Cloneable>(aObject.Clone());
}

template<class Cloneable>
std::unique_ptr<Cloneable> clone(const std::unique_ptr<Cloneable>& aObject)
{
   return std::unique_ptr<Cloneable>(aObject->Clone());
}

template<class Cloneable>
std::unique_ptr<Cloneable> clone(const std::shared_ptr<Cloneable>& aObject)
{
   return std::unique_ptr<Cloneable>(aObject->Clone());
}
} // namespace ut

#endif
