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

#ifndef UT_OPTIONAL_HPP
#define UT_OPTIONAL_HPP

#include <cassert>
#include <stdexcept>
#include <utility>

#include "UtTypeTraits.hpp"

//! The contents of this file are analogous to C++17's <optional> header.
//! For full documentation, see https://en.cppreference.com/w/cpp/utility/optional.
//! Some minor differences may exist between this and the standard.
namespace ut
{
class bad_optional_access : public std::exception
{
public:
   const char* what() const noexcept override { return "Invalid ut::optional access."; }
};

struct nullopt_t
{
   constexpr explicit nullopt_t(int) {}
};

static constexpr nullopt_t nullopt{0};

struct in_place_t
{
};
static constexpr in_place_t in_place;

//! ut::optional<T> is a utility class that represents an optional value.
//! The stored value is stored in the optional itself, meaning no heap allocation is required.
//! A default or nullopt constructed optional will not construct a T, not even using T's default constructor.
template<typename T>
class optional
{
   static_assert(!std::is_reference<T>::value, "Cannot have a ut::optional of a reference type.");
   static_assert(!std::is_void<T>::value, "Cannot have a ut::optional to void.");

public:
   using value_type = T;

   // Constructors
   //! Initializes an empty optional
   optional() noexcept
      : mDummy{}
   {
   }

   //! Initializes an empty optional
   optional(nullopt_t) noexcept
      : mDummy{}
   {
   }

   //! Copies aOther's value, if it exists.
   optional(const optional<T>& aOther)
   {
      if (aOther.has_value())
      {
         emplace(*aOther);
      }
   }

   //! Moves aOther's value, if it exists.
   //! Note that aOther still has a value afterwards, but it is moved from.
   optional(optional<T>&& aOther)
   {
      if (aOther.has_value())
      {
         emplace(std::move(*aOther));
      }
   }

   //! Copies aOther's value, if it exists.
   //! Only participates in overload resolution if T is explicitly, but not implicitly, constructible from U.
   template<typename U,
            ut::enable_if_t<std::is_constructible<T, U>::value && !std::is_constructible<T, optional<U>>::value &&
                               !std::is_convertible<optional<U>, T>::value && !std::is_convertible<const U&, T>::value,
                            bool> = false>
   explicit optional(const optional<U>& aOther)
   {
      if (aOther.has_value())
      {
         emplace(*aOther);
      }
   }

   //! Copies aOther's value, if it exists.
   //! Only participates in overload resolution if T is implicitly constructible from U.
   template<typename U,
            ut::enable_if_t<std::is_constructible<T, U>::value && !std::is_constructible<T, optional<U>>::value &&
                               !std::is_convertible<optional<U>, T>::value && std::is_convertible<const U&, T>::value,
                            bool> = false>
   optional(const optional<U>& aOther)
   {
      if (aOther.has_value())
      {
         emplace(*aOther);
      }
   }

   //! Moves aOther's value, if it exists.
   //! Note that aOther still has a value afterwards, but it is moved from.
   //! Only participates in overload resolution if T is explicitly, but not implicitly, constructible from U.
   template<typename U,
            ut::enable_if_t<std::is_constructible<T, U>::value && !std::is_constructible<T, optional<U>>::value &&
                               !std::is_convertible<optional<U>, T>::value && !std::is_convertible<U&&, T>::value,
                            bool> = false>
   explicit optional(optional<U>&& aOther)
   {
      if (aOther.has_value())
      {
         emplace(std::move(*aOther));
      }
   }

   //! Moves aOther's value, if it exists.
   //! Note that aOther still has a value afterwards, but it is moved from.
   //! Only participates in overload resolution if T is implicitly constructible from U.
   template<typename U,
            ut::enable_if_t<std::is_constructible<T, U>::value && !std::is_constructible<T, optional<U>>::value &&
                               !std::is_convertible<optional<U>, T>::value && std::is_convertible<U&&, T>::value,
                            bool> = false>
   optional(optional<U>&& aOther)
   {
      if (aOther.has_value())
      {
         emplace(std::move(*aOther));
      }
   }

   //! In-place construction.
   //! Uses direct initialization, not list initialization.
   template<typename... Args>
   optional(in_place_t, Args&&... aArgs)
   {
      emplace(std::forward<Args>(aArgs)...);
   }

   //! Only participates in overload resolution if T is explicitly, but not implicitly, constructible from U.
   template<typename U,
            ut::enable_if_t<std::is_constructible<T, U&&>::value && !std::is_same<ut::decay_t<U>, ut::in_place_t>::value &&
                               !std::is_same<ut::decay_t<U>, optional<T>>::value && !std::is_convertible<U&&, T>::value,
                            bool> = false>
   explicit optional(U&& aValue)
   {
      emplace(std::forward<U>(aValue));
   }

   //! Moves aValue into the contained value.
   //! Only participates in overload resolution if T is implicitly constructible from U.
   template<typename U,
            ut::enable_if_t<std::is_constructible<T, U&&>::value && !std::is_same<ut::decay_t<U>, ut::in_place_t>::value &&
                               !std::is_same<ut::decay_t<U>, optional<T>>::value && std::is_convertible<U&&, T>::value,
                            bool> = false>
   optional(U&& aValue)
   {
      emplace(std::forward<U>(aValue));
   }

   // Destructor
   //! If *this contains a value, destructs it.
   ~optional() noexcept { reset(); }

   // Assignment

   //! If *this contains a value, destructs it.
   //! *this will be empty after.
   optional<T>& operator=(nullopt_t) noexcept
   {
      reset();
      return *this;
   }

   //! If *this contains a value, calls operator= on it.
   //! Otherwise, constructs a T.
   //! Only participates in overload resolution if T is constructible from U.
   template<typename U>
   ut::enable_if_t<std::is_constructible<T, U>::value && std::is_assignable<T&, U>::value &&
                      !std::is_same<optional<T>, ut::decay_t<U>>::value &&
                      (!std::is_scalar<T>::value || !std::is_same<T, ut::decay_t<U>>::value),
                   optional<T>&>
   operator=(U&& aValue)
   {
      if (mExists)
      {
         mValue = std::forward<U>(aValue);
      }
      else
      {
         emplace(std::forward<U>(aValue));
      }
      return *this;
   }

   //! If aOther is empty, *this will be empty after calling.
   //! Otherwise, if *this contains a value, calls operator= on it.
   //! Otherwise, copies aOther's value.
   optional<T>& operator=(const optional<T>& aOther)
   {
      if (aOther.has_value())
      {
         if (mExists)
         {
            mValue = *aOther;
         }
         else
         {
            emplace(*aOther);
         }
      }
      else
      {
         reset();
      }
      return *this;
   }

   //! If aOther is empty, *this will be empty after calling.
   //! Otherwise, if *this contains a value, calls operator= on it.
   //! Otherwise, moves aOther's value.
   optional<T>& operator=(optional<T>&& aOther)
   {
      if (aOther.has_value())
      {
         if (mExists)
         {
            mValue = std::move(*aOther);
         }
         else
         {
            emplace(std::move(*aOther));
         }
      }
      else
      {
         reset();
      }
      return *this;
   }

   //! If aOther is empty, *this will be empty after calling.
   //! Otherwise, if *this contains a value, calls operator= on it.
   //! Otherwise, copies aOther's value.
   //! Only participates in overload resolution if U is implicitly convertible to T.
   template<typename U>
   ut::enable_if_t<std::is_constructible<T, const U&>::value && std::is_assignable<T&, const U&>::value &&
                      !std::is_constructible<T, optional<U>>::value && !std::is_convertible<optional<U>, T>::value &&
                      !std::is_assignable<T&, optional<U>>::value,
                   optional<T>&>
   operator=(const optional<U>& aOther)
   {
      if (aOther.has_value())
      {
         if (mExists)
         {
            mValue = *aOther;
         }
         else
         {
            emplace(*aOther);
         }
      }
      else
      {
         reset();
      }
      return *this;
   }

   //! If *this contains a value, calls operator= on it.
   //! Otherwise, copies aOther's value.
   //! Only participates in overload resolution if U is implicitly convertible to T.
   template<typename U>
   ut::enable_if_t<std::is_constructible<T, U>::value && std::is_assignable<T&, U>::value &&
                      !std::is_constructible<T, optional<U>>::value && !std::is_convertible<optional<U>, T>::value &&
                      !std::is_assignable<T&, optional<U>>::value,
                   optional<T>&>
   operator=(optional<U>&& aOther)
   {
      if (aOther.has_value())
      {
         if (mExists)
         {
            mValue = std::move(*aOther);
         }
         else
         {
            emplace(std::move(*aOther));
         }
      }
      else
      {
         reset();
      }
      return *this;
   }

   // Accessors

   //! Indirect member access operators.
   //! {
   T* operator->()
   {
      assert(has_value());
      return &mValue;
   }

   const T* operator->() const
   {
      assert(has_value());
      return &mValue;
   }
   //! }

   //! Indirection operators.
   //! The value category ((lvalue / rvalue) & (const / non-const)) is the same as the calling object.
   //! Unlike value(), behavior is undefined if *this does not contain a value.
   //! {
   T& operator*() &
   {
      assert(has_value());
      return mValue;
   }

   const T& operator*() const&
   {
      assert(has_value());
      return mValue;
   }

   T&& operator*() &&
   {
      assert(has_value());
      return std::move(mValue);
   }

   const T&& operator*() const&&
   {
      assert(has_value());
      return std::move(mValue);
   }
   //! }

   //! Returns whether or not *this contains a value.
   //! {
   explicit operator bool() const noexcept { return mExists; }

   bool has_value() const noexcept { return mExists; }
   //! }

   //! A function for getting the contained value.
   //! The value category ((lvalue / rvalue) & (const / non-const)) is the same as the calling object.
   //! Unlike operator*, throws an bad_optional_access exception if *this does not contain a value.
   //! {
   T& value() &
   {
      if (!mExists)
      {
         throw bad_optional_access();
      }
      return mValue;
   }

   const T& value() const&
   {
      if (!mExists)
      {
         throw bad_optional_access();
      }
      return mValue;
   }

   T&& value() &&
   {
      if (!mExists)
      {
         throw bad_optional_access();
      }
      return std::move(mValue);
   }

   const T&& value() const&&
   {
      if (!mExists)
      {
         throw bad_optional_access();
      }
      return std::move(mValue);
   }
   //! }

   //! If *this contains a value, returns a copy of it.
   //! Otherwise, returns the provided value.
   //! Only used if calling object is const or an lvalue.
   template<typename U>
   T value_or(U&& aDefaultValue) const&
   {
      if (mExists)
      {
         return mValue;
      }
      return static_cast<T>(std::forward<U>(aDefaultValue));
   }

   //! If *this contains a value, returns a moved-into copy of it.
   //! Otherwise, returns the provided value.
   //! Only used if calling object is a non-const rvalue.
   template<typename U>
   T value_or(U&& aDefaultValue) &&
   {
      if (mExists)
      {
         return std::move(mValue);
      }
      return static_cast<T>(std::forward<U>(aDefaultValue));
   }

   // Mutators

   //! Swaps two optionals.
   void swap(optional<T>& aOther)
   {
      if (mExists)
      {
         if (aOther.mExists)
         {
            // Written this way based on cppreference.
            using std::swap;
            swap(**this, *aOther);
         }
         else
         {
            aOther.emplace(std::move(**this));
            reset();
         }
      }
      else
      {
         if (aOther.mExists)
         {
            emplace(std::move(*aOther));
            aOther.reset();
         }
         // no else clause
      }
   }

   //! If *this contains a value, destructs it.
   //! *this will be empty after calling.
   void reset() noexcept
   {
      if (mExists)
      {
         mExists = false;
         mValue.T::~T();
      }
   }

   //! Constructs a T from Args...
   //! If *this contains a value, destructs it before constructing a new one.
   //! *this will contain a value after calling.
   //! If T throws during construction, *this will NOT contain a value.
   template<typename... Args>
   T& emplace(Args&&... aArgs)
   {
      reset();
      mExists = true;
      try
      {
         new (&mValue) T(std::forward<Args>(aArgs)...);
      }
      catch (...)
      {
         mExists = false;
         throw;
      }
      return mValue;
   }

private:
   bool mExists = false;
   union
   {
      char mDummy;
      T    mValue;
   };
};

// Comparison of optional<T> and optional<U>
//! Two empty optionals always compare equal.
//! An empty and a non-empty optional always compare not equal.
//! An empty optional always compares less than a non-empty optional.
//! {
template<typename T, typename U>
bool operator==(const optional<T>& aLeft, const optional<U>& aRight)
{
   if (aLeft.has_value() && aRight.has_value())
   {
      return *aLeft == *aRight;
   }
   return !aLeft.has_value() && !aRight.has_value();
}

template<typename T, typename U>
bool operator!=(const optional<T>& aLeft, const optional<U>& aRight)
{
   return !(aLeft == aRight);
}

template<typename T, typename U>
bool operator<(const optional<T>& aLeft, const optional<U>& aRight)
{
   if (aLeft.has_value() && aRight.has_value())
   {
      return *aLeft < *aRight;
   }
   return !aLeft.has_value() && aRight.has_value();
}

template<typename T, typename U>
bool operator<=(const optional<T>& aLeft, const optional<U>& aRight)
{
   return !(aRight < aLeft);
}

template<typename T, typename U>
bool operator>(const optional<T>& aLeft, const optional<U>& aRight)
{
   return aRight < aLeft;
}

template<typename T, typename U>
bool operator>=(const optional<T>& aLeft, const optional<U>& aRight)
{
   return !(aLeft < aRight);
}
//! }

// Comparison of optional and nullopt_t

//! nullopt always compares equal to empty optionals.
//! nullopt always compares less than non-empty optionals.
//! {
template<typename T>
bool operator==(const optional<T>& aValue, nullopt_t) noexcept
{
   return !aValue.has_value();
}

template<typename T>
bool operator!=(const optional<T>& aValue, nullopt_t) noexcept
{
   return aValue.has_value();
}

template<typename T>
bool operator<(const optional<T>& aValue, nullopt_t) noexcept
{
   return false;
}

template<typename T>
bool operator<=(const optional<T>& aValue, nullopt_t) noexcept
{
   return !aValue.has_value();
}

template<typename T>
bool operator>(const optional<T>& aValue, nullopt_t) noexcept
{
   return aValue.has_value();
}

template<typename T>
bool operator>=(const optional<T>& aValue, nullopt_t) noexcept
{
   return true;
}

// Comparison of nullopt_t and optional

template<typename T>
bool operator==(nullopt_t, const optional<T>& aValue) noexcept
{
   return !aValue.has_value();
}

template<typename T>
bool operator!=(nullopt_t, const optional<T>& aValue) noexcept
{
   return aValue.has_value();
}

template<typename T>
bool operator<(nullopt_t, const optional<T>& aValue) noexcept
{
   return aValue.has_value();
}

template<typename T>
bool operator<=(nullopt_t, const optional<T>& aValue) noexcept
{
   return true;
}

template<typename T>
bool operator>(nullopt_t, const optional<T>& aValue) noexcept
{
   return false;
}

template<typename T>
bool operator>=(nullopt_t, const optional<T>& aValue) noexcept
{
   return !aValue.has_value();
}
//! }

// Comparison of optional and value

//! An empty optional always compares strictly less than values.
//! {
template<typename T, typename U>
bool operator==(const optional<T>& aOptional, const U& aValue)
{
   if (aOptional.has_value())
   {
      return *aOptional == aValue;
   }
   return false;
}

template<typename T, typename U>
bool operator!=(const optional<T>& aOptional, const U& aValue)
{
   if (aOptional.has_value())
   {
      return *aOptional != aValue;
   }
   return true;
}

template<typename T, typename U>
bool operator<(const optional<T>& aOptional, const U& aValue)
{
   if (aOptional.has_value())
   {
      return *aOptional < aValue;
   }
   return true;
}

template<typename T, typename U>
bool operator<=(const optional<T>& aOptional, const U& aValue)
{
   if (aOptional.has_value())
   {
      return *aOptional <= aValue;
   }
   return true;
}

template<typename T, typename U>
bool operator>(const optional<T>& aOptional, const U& aValue)
{
   if (aOptional.has_value())
   {
      return *aOptional > aValue;
   }
   return false;
}

template<typename T, typename U>
bool operator>=(const optional<T>& aOptional, const U& aValue)
{
   if (aOptional.has_value())
   {
      return *aOptional >= aValue;
   }
   return false;
}

// Comparison of value and optional

template<typename T, typename U>
bool operator==(const T& aValue, const optional<U>& aOptional)
{
   if (aOptional.has_value())
   {
      return aValue == *aOptional;
   }
   return false;
}

template<typename T, typename U>
bool operator!=(const T& aValue, const optional<U>& aOptional)
{
   if (aOptional.has_value())
   {
      return aValue != *aOptional;
   }
   return true;
}

template<typename T, typename U>
bool operator<(const T& aValue, const optional<U>& aOptional)
{
   if (aOptional.has_value())
   {
      return aValue < *aOptional;
   }
   return false;
}

template<typename T, typename U>
bool operator<=(const T& aValue, const optional<U>& aOptional)
{
   if (aOptional.has_value())
   {
      return aValue <= *aOptional;
   }
   return false;
}

template<typename T, typename U>
bool operator>(const T& aValue, const optional<U>& aOptional)
{
   if (aOptional.has_value())
   {
      return aValue > *aOptional;
   }
   return true;
}

template<typename T, typename U>
bool operator>=(const T& aValue, const optional<U>& aOptional)
{
   if (aOptional.has_value())
   {
      return aValue >= *aOptional;
   }
   return true;
}
//! }

// helper functions

//! Creates an optional from a value.
//! Returned optional's value_type is non-const, non-volatile, non-reference.
template<typename T>
optional<ut::decay_t<T>> make_optional(T&& aValue)
{
   return optional<ut::decay_t<T>>(std::forward<T>(aValue));
}

//! Creates an optional.
//! Similar to make_unique for std::unique_ptr's.
template<typename T, typename... Args>
optional<T> make_optional(Args&&... aArgs)
{
   return optional<T>(in_place, std::forward<Args>(aArgs)...);
}
} // namespace ut

namespace std
{
//! Specialization of std::swap that avoids unnecessary copies in default implementation.
template<typename T>
void swap(ut::optional<T>& aLeft, ut::optional<T>& aRight)
{
   aLeft.swap(aRight);
}

//! Specialization of std::hash.
//! Empty optionals always hash to 0.
//! Non-empty optionals use the hash of their value_type.
template<typename T>
struct hash<ut::optional<T>>
{
   size_t operator()(const ut::optional<T>& aValue) const
   {
      if (aValue.has_value())
      {
         return std::hash<ut::remove_const_t<T>>()(*aValue);
      }
      return 0;
   }
};
} // namespace std

#endif
