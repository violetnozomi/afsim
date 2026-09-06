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

#ifndef UT_STACKTRACE_HPP
#define UT_STACKTRACE_HPP

#include "ut_export.h"

#include <iosfwd>
#include <iterator>
#include <string>
#include <vector>

#include "UtCompilerAttributes.hpp"
#include "UtOptional.hpp"

namespace ut
{
namespace detail
{
constexpr std::size_t cMAX_STACKTRACE_SIZE = 1024;

//! Contains values retrieved by symbol lookup for a stack frame.
struct StackFrameDetails
{
   StackFrameDetails(std::string aDescription, std::string aSourceFile, std::uint_least32_t aSourceLine)
      : mDescription(std::move(aDescription))
      , mSourceFile(std::move(aSourceFile))
      , mSourceLine(aSourceLine)
   {
   }

   StackFrameDetails() = default;

   std::string         mDescription;
   std::string         mSourceFile;
   std::uint_least32_t mSourceLine = 0;
};

using StackFrameCache = ut::optional<StackFrameDetails>;
} // namespace detail

//! ut::stacktrace_entry is an in-house implementation of std::stacktrace_entry.
//! More details may be found at https://en.cppreference.com/w/cpp/utility/stacktrace_entry
//! @note There may be slight differences between this implementation and what is in the standard.
class UT_EXPORT stacktrace_entry
{
public:
   using native_handle_type = std::uintptr_t;

   //! Constructs an empty/invalid stacktrace entry.
   stacktrace_entry() = default;

   //! This constructor is an implementation detail and is not part of the standard.
   explicit stacktrace_entry(native_handle_type aNativeHandle) noexcept
      : mNativeHandle(aNativeHandle)
   {
   }

   //! Returns the native handle to the stack trace entry.
   //! On Windows, this represents the program counter.
   //! On Linux, this represents the function's return address.
   native_handle_type native_handle() const noexcept { return mNativeHandle; }

   //! Checks whether the stacktrace_entry is non-empty.
   explicit operator bool() const noexcept { return mNativeHandle != 0; }

   //! Returns a description of the evaluation represented by *this on success.
   //! Returns empty on failure.
   std::string description() const
   {
      if (*this)
      {
         InitializeDetails();
         return mDetails->mDescription;
      }
      return std::string();
   }

   //! Returns the name of the source file that contains the expression or statement represented by *this.
   //! Returns empty on failure.
   //! @warning source_file() is the path to the .so file on Linux.
   std::string source_file() const
   {
      if (*this)
      {
         InitializeDetails();
         return mDetails->mSourceFile;
      }
      return std::string();
   }

   //! Returns a 1-based line number that represents the expression or statement represented by *this.
   //! Returns 0 on failure.
   //! @warning source_line() is always 0 on Linux.
   std::uint_least32_t source_line() const
   {
      if (*this)
      {
         InitializeDetails();
         return mDetails->mSourceLine;
      }
      return 0;
   }

private:
   //! If mDetails is uninitialized, initializes it.
   //! Will not re-initialize mDetails if it is already set.
   void InitializeDetails() const;

   native_handle_type mNativeHandle = 0;
   //! mDetails is a cache of the description and source information.
   //! Its value is not set until requested.
   //! This makes stacktraces much cheaper to construct.
   mutable detail::StackFrameCache mDetails;
};

//! ut::stacktrace is an in-house implementation of std::stacktrace.
//! For simplicity's sake, we are not implementing std::basic_stacktrace.
//! More details may be found at https://en.cppreference.com/w/cpp/utility/basic_stacktrace
//! @note There may be slight differences between this implementation and what is in the standard.
//! @warning Function inlining and other optimizations may result in stack traces different from what is in code.
class UT_EXPORT stacktrace
{
public:
   using value_type             = ut::stacktrace_entry;
   using const_reference        = const value_type&;
   using reference              = value_type&;
   using const_iterator         = std::vector<value_type>::const_iterator;
   using iterator               = const_iterator;
   using reverse_iterator       = std::reverse_iterator<iterator>;
   using reverse_const_iterator = std::reverse_iterator<const_iterator>;
   using difference_type        = std::ptrdiff_t;
   using size_type              = std::size_t;
   using allocator_type         = std::allocator<ut::stacktrace_entry>;

   stacktrace() = default;

   //! This constructor is an implementation detail and is not part of the standard.
   explicit stacktrace(std::vector<value_type>&& aContainer) noexcept
      : mContainer(std::move(aContainer))
   {
   }

   //! Creates a stacktrace of the calling thread's current execution,
   //!    but skips the most recent aSkip entries and contains at most aMaxDepth entries.
   //! The frame at entry at index 0 is the one that called this function.
   //! Returns an empty stacktrace on failure.
   //! @note The standard implementation breaks this into 3 separate overloads and has an extra allocator argument.
   static ut::stacktrace current(size_type aSkip = 0, size_type aMaxDepth = detail::cMAX_STACKTRACE_SIZE) noexcept;

   //! Returns the underlying container's allocator.
   allocator_type get_allocator() const noexcept { return mContainer.get_allocator(); }

   //! Returns an iterator to the first entry.
   //! The first entry is the most recent call, and the last entry is the root call.
   //@{
   const_iterator begin() const noexcept { return mContainer.begin(); }
   const_iterator cbegin() const noexcept { return mContainer.cbegin(); }
   //@}

   //! Returns an iterator one-past-the-last entry.
   //! The first entry is the most recent call, and the last entry is the root call.
   //@{
   const_iterator end() const noexcept { return mContainer.end(); }
   const_iterator cend() const noexcept { return mContainer.cend(); }
   //@}

   //! Returns a reverse iterator to the first entry of the reversed stacktrace.
   //! This corresponds to the last entry, or root call, of the trace.
   //@{
   reverse_const_iterator rbegin() const noexcept { return mContainer.rbegin(); }
   reverse_const_iterator crbegin() const noexcept { return mContainer.crbegin(); }
   //@}

   //! Returns a reverse iterator one-past-the-last entry of the reversed stacktrace.
   //! This corresponds to one-before-the-first entry, or most recent call, of the trace.
   //@{
   reverse_const_iterator rend() const noexcept { return mContainer.rend(); }
   reverse_const_iterator crend() const noexcept { return mContainer.crend(); }
   //@}

   //! Returns true if the stacktrace is empty.
   CPP17_NODISCARD bool empty() const noexcept { return mContainer.empty(); }
   //! Returns the number of elements in the stacktrace.
   CPP17_NODISCARD size_type size() const noexcept { return mContainer.size(); }
   //! Returns the maximum number of elements the underlying container is able to hold.
   CPP17_NODISCARD size_type max_size() const noexcept { return mContainer.max_size(); }

   //! Returns a reference to the entry at the specified position. No bounds checking is performed.
   const_reference operator[](size_type aPosition) const { return mContainer[aPosition]; }
   //! Returns a reference to the entry at the specified position.
   //! If aPosition is not within range, throws a std::out_of_range exception.
   const_reference at(size_type aPosition) const { return mContainer.at(aPosition); }

   //! Swaps the contents of two stacktraces.
   void swap(stacktrace& aOther) noexcept { mContainer.swap(aOther.mContainer); }

private:
   std::vector<value_type> mContainer;
};

namespace log
{
class MessageStream;
}

//! Prints aTrace to aStream.
//! However, if aTrace is empty or invalid, prints a warning instead with possible reasons the trace may have failed.
UT_EXPORT void PrintStacktrace(std::ostream& aStream, const ut::stacktrace& aTrace = ut::stacktrace::current());

//! Prints aTrace to aStream as a list of note.
//! Does not set the data of aStream.
//! However, if aTrace is empty or invalid, prints a warning instead with possible reasons the trace may have failed.
UT_EXPORT void AppendStacktrace(ut::log::MessageStream& aStream, const ut::stacktrace& aTrace = ut::stacktrace::current());

//! Prints aTrace to aStream.
//! If aEntry is non-empty and all fields are valid,
//!    format is "handle (description) at source_file: source_line"
//! There is no trailing newline.
//! The handle is in hex format.
//! Empty or invalid fields may be omitted or replaced with "??".
//! @note The format in the standard is implementation defined.
UT_EXPORT std::ostream& operator<<(std::ostream& aStream, const ut::stacktrace_entry& aEntry);

//! Prints aTrace to aStream.
//! Each entry is printed on a separate line.
//! If aTrace is empty, prints nothing.
//! Format is "index# entry"
//! @note The format in the standard is implementation defined.
UT_EXPORT std::ostream& operator<<(std::ostream& aStream, const ut::stacktrace& aTrace);
} // namespace ut

namespace std
{
inline void swap(ut::stacktrace& aLeft, ut::stacktrace& aRight) noexcept
{
   aLeft.swap(aRight);
}
} // namespace std

#endif
