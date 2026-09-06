// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_OUT_PTR_HPP
#define UT_OUT_PTR_HPP

#include <tuple>

#include "UtTypeTraits.hpp"

namespace ut
{
namespace ut_out_ptr_detail
{
//! An index sequence structure.
//! This is implemented at least 5 times throughout our codebase.
//! All the definitions should be consolidated into a single location.
template<std::size_t...>
struct index_sequence
{
};

template<std::size_t N, std::size_t... IntSeq>
struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, IntSeq...>
{
};
template<std::size_t... IntSeq>
struct make_index_sequence_impl<0, IntSeq...>
{
   using type = index_sequence<IntSeq...>;
};
template<std::size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;
} // namespace ut_out_ptr_detail

//! In-house implementation of C++23's std::out_ptr_t.
//! @par details
//!   This class is useful for using smart pointers with C APIs that take arguments by type**.
//!   Differences between this and the standard include:
//!   - This requires Smart to have a `pointer` member alias.
//!   - This requires Smart to have a `.reset` member with the correct parameter types.
//!   - This has a move constructor.
//! @tparam Smart is the type of object to adapt.  This is usually a smart pointer.
//! @tparam Pointer is the type of raw pointer to which the C API writes its result.
//! @tparam Args is the type list for the arguments forwarded when resetting the adapted object.
//! @note In general, users of this class should only have to call 'out_ptr(...)', instead of naming the class directly.
//! @see https://en.cppreference.com/w/cpp/memory/out_ptr_t
template<typename Smart, typename Pointer, typename... Args>
class out_ptr_t
{
   static_assert(std::is_object<Pointer>::value, "Pointer must be a concrete type.");

   template<typename Sequence>
   struct ResetImpl;
   template<std::size_t... Indices>
   struct ResetImpl<ut_out_ptr_detail::index_sequence<Indices...>>
   {
      void operator()(out_ptr_t& aOutPtr) const
      {
         using SP = typename Smart::pointer;
         aOutPtr.mSmart->reset(static_cast<SP>(aOutPtr.mPointer), std::forward<Args>(std::get<Indices>(aOutPtr.mArgs))...);
      }
   };

public:
   //! Binds *this to aSmart.
   //! Additional args may be passed that will be used for resetting aSmart upon destruction.
   //! @param aSmart is the object to adapt.
   //! @param aArgs are the arguments to forward when resetting the adapted object.
   explicit out_ptr_t(Smart& aSmart, Args... aArgs)
      : mSmart(&aSmart)
      , mPointer(nullptr)
      , mArgs(std::forward<Args>(aArgs)...)
   {
   }
   out_ptr_t(const out_ptr_t&) = delete;

   //! @par Copy Elision
   //!   std::out_ptr_t relies on guaranteed copy elision.
   //!   Since we don't have that in C++11, a move constructor is required to get the correct semantics.
   out_ptr_t(out_ptr_t&& aOther)
      : mSmart(aOther.mSmart)
      , mPointer(aOther.mPointer)
      , mArgs(std::move(aOther.mArgs))
   {
      aOther.mSmart = nullptr;
   }

   out_ptr_t& operator=(const out_ptr_t&) = delete;
   out_ptr_t& operator=(out_ptr_t&&) = delete;

   //! Resets the smart pointer using mPointer and the values passed as Args.
   //! @see ResetImpl
   ~out_ptr_t()
   {
      if (mSmart && mPointer)
      {
         using Sequence = ut_out_ptr_detail::make_index_sequence<sizeof...(Args)>;
         ResetImpl<Sequence>()(*this);
      }
   }

   //! Implicit conversion to Pointer*.
   //! @return a pointer to the underlying raw pointer
   operator Pointer*() const noexcept { return &mPointer; }
   //! Implicit conversion to void**.
   //! @note Only participates in overload resolution if Pointer is not void*.
   //! @return a pointer to the underlying raw pointer
   operator ut::enable_if_t<std::is_pointer<Pointer>::value && !std::is_same<Pointer, void*>::value, void**>() const noexcept
   {
      return reinterpret_cast<void**>(&mPointer);
   }

private:
   Smart*              mSmart;
   mutable Pointer     mPointer;
   std::tuple<Args...> mArgs;
};

namespace ut_out_ptr_detail
{
template<typename Smart, typename Pointer>
struct GetPointerType
{
   using type = Pointer;
};
template<typename Smart>
struct GetPointerType<Smart, void>
{
   using type = typename Smart::pointer;
};
template<typename Pointer, typename Smart, typename... Args>
using OutPtrType = ut::out_ptr_t<Smart, typename ut_out_ptr_detail::GetPointerType<Smart, Pointer>::type, Args...>;
} // namespace ut_out_ptr_detail

//! ut::out_ptr is an in-house implementation of std::out_ptr.
//! It is the preferred way to retrieve a ut::out_ptr_t.
//! @tparam Smart is the type of object to adapt.  This is usually a smart pointer.
//! @tparam Pointer is the type of raw pointer to which the C API writes its result.
//! @tparam Args is the type list for the arguments forwarded when resetting the adapted object.
//! @param aSmart is the object to adapt.
//! @param aArgs are the arguments to forward when resetting the adapted object.
//! @see https://en.cppreference.com/w/cpp/memory/out_ptr_t/out_ptr
template<typename Pointer = void, typename Smart, class... Args>
ut_out_ptr_detail::OutPtrType<Pointer, Smart, Args...> out_ptr(Smart& aSmart, Args&&... aArgs)
{
   return ut_out_ptr_detail::OutPtrType<Pointer, Smart, Args...>(aSmart, std::forward<Args>(aArgs)...);
}
} // namespace ut

#endif
