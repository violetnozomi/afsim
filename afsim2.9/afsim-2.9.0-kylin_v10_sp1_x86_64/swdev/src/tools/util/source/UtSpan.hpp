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

#ifndef UTSPAN_H
#define UTSPAN_H

#include <array>
#include <iterator>
#include <limits>

#include "UtBytes.hpp"
#include "UtCast.hpp"
#include "UtTypeTraits.hpp"

namespace ut
{

static constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

template<class T, std::size_t Extent = dynamic_extent>
class span
{
public:
   using element_type           = T;
   using value_type             = ut::remove_cv_t<T>;
   using index_type             = std::size_t;
   using difference_type        = std::ptrdiff_t;
   using pointer                = T*;
   using const_pointer          = const T*;
   using reference              = T&;
   using const_reference        = const T&;
   using iterator               = T*;
   using const_iterator         = const T*;
   using reverse_iterator       = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;


   constexpr span() noexcept = default;
   constexpr span(T* aPtr, std::size_t aCount)
      : mData{aPtr}
      , mSize{aCount}
   {
   }
   constexpr span(T* aFirst, T* aLast)
      : mData{aFirst}
      , mSize{ut::safe_cast<size_t>(aLast - aFirst)}
   {
   }

   template<std::size_t N>
   constexpr span(T (&arr)[N]) noexcept
      : mData{arr}
      , mSize{N}
   {
   }
   template<std::size_t N>
   constexpr span(std::array<T, N>& aArray) noexcept
      : mData{aArray.data()}
      , mSize{N}
   {
   }
   template<std::size_t N>
   constexpr span(const std::array<T, N>& aArray) noexcept
      : mData{aArray.data()}
      , mSize{N}
   {
   }
   template<class Container>
   constexpr span(Container& aContainer)
      : mData{aContainer.data()}
      , mSize{aContainer.size()}
   {
   }
   constexpr span(const span& other) noexcept = default;

   constexpr iterator begin() const { return mData; }

   constexpr iterator end() const { return mData + mSize; }

   constexpr const_iterator cbegin() const { return mData; }

   constexpr const_iterator cend() const { return mData + mSize; }

   constexpr reverse_iterator rbegin() const { return mData + mSize - 1; }

   constexpr const_reverse_iterator rend() const { return mData - 1; }

   constexpr const_reverse_iterator crbegin() const { return rbegin(); }

   constexpr const_reverse_iterator crend() const { return rend(); }

   constexpr reference front() const { return mData[0]; }

   constexpr reference back() const { return mData[mSize - 1]; }

   constexpr reference operator[](index_type idx) const { return mData[idx]; }

   constexpr pointer data() const { return mData; }

   // Observers

   constexpr std::size_t size() const { return mSize; }

   constexpr std::size_t size_bytes() const { return sizeof(T) * mSize; }

   constexpr bool empty() const { return mSize == 0; }

   // Subviews

   template<std::size_t Count>
   constexpr span<element_type, Count> first() const
   {
      return {mData, Count};
   }

   constexpr span<element_type, dynamic_extent> first(std::size_t count) const { return {mData, count}; }

   template<std::size_t Count>
   constexpr span<element_type, Count> last() const
   {
      return {mData + (mSize - Count), Count};
   }

   constexpr span<element_type, dynamic_extent> last(std::size_t count) const
   {
      return {mData + (mSize - count), count};
   }

   template<std::size_t Offset, std::size_t Count = dynamic_extent>
   constexpr auto subspan() const
      -> span<element_type, (Count != dynamic_extent ? Count : (Extent != dynamic_extent ? Extent - Offset : Count))>
   {
      return {mData + Offset, Count == dynamic_extent ? mSize - Offset : Count};
   }

   constexpr span<element_type, dynamic_extent> subspan(std::size_t offset, std::size_t count = dynamic_extent) const
   {
      return {mData + offset, (count == dynamic_extent) ? mSize - offset : count};
   }


private:
   T*          mData{nullptr};
   std::size_t mSize{0};
};

template<class T>
constexpr typename span<T>::iterator begin(span<T> s) noexcept
{
   return s.begin();
}

template<class T>
constexpr typename span<T>::iterator end(span<T> s) noexcept
{
   return s.end();
}

// This can be updated to use std::byte over ut::byte once this is being compiled with C++17
template<class T, std::size_t N>
auto as_bytes(span<T, N> s) noexcept -> span<const byte, (N == dynamic_extent ? dynamic_extent : sizeof(T) * N)>
{
   return {reinterpret_cast<const byte*>(s.data()), s.size_bytes()};
}

// This can be updated to use std::byte over ut::byte once this is being compiled with C++17
template<class T, std::size_t N>
auto as_writable_bytes(span<T, N> s) noexcept -> span<byte, (N == dynamic_extent ? dynamic_extent : sizeof(T) * N)>
{
   return {reinterpret_cast<byte*>(s.data()), s.size_bytes()};
}

template<std::size_t I, class T, std::size_t N>
constexpr ut::enable_if_t<N != dynamic_extent, T&> get(span<T, N> s) noexcept
{
   static_assert(I < N && I >= 0, "Template index parameter I must be less than the size of the span");
   return s[I];
}

// Not implemented: tuple_size, tuple_element
// Reasoning: At this point there is no tuple class available, this can be revisited when
//            the standard we are using is moved to C++17 or greater.

// Deduction Guides - Can be uncommented when moved to C++17
/*
template<class T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template<class T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template<class T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template<class Container>
span(Container&) -> span<typename Container::value_type>;

template<class Container>
span(const Container&) -> span<const typename Container::value_type>;
*/

} // namespace ut

#endif // UTSPAN_H
