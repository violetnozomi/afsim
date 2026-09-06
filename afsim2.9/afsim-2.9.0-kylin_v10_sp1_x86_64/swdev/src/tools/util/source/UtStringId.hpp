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

#ifndef UTSTRINGID_HPP
#define UTSTRINGID_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>

class UtDictionary;

//! A simple class used to store and manipulate string ID's.
//!
//! String ID's are used to replace a string with an single integer. This reduces string
//! comparisons to comparing two integers, as well as reducing the time for assignment
//! copy construction. An obviously, an integer takes less space than most strings.
//!
//! @see UtDictionary

class UT_EXPORT UtStringId
{
public:
   //! Create an instance with the default (null) identifier.
   UtStringId() noexcept
      : mId(0)
   {
   }

   UtStringId(const UtStringId&)     = default;
   UtStringId(UtStringId&&) noexcept = default;

   //! Create an instance from the specified string.
   UtStringId(const std::string& aString);

   //! Create an instance from the specified C string.
   UtStringId(const char* aStringPtr);

   //! True if the string ID is the empty string
   bool IsNull() const { return mId == 0; }
   bool Empty() const { return mId == 0; }
   bool operator!() const { return mId == 0; }

   operator const std::string&() const;

   //! @name Explicit accessor and mutator methods.
   //@{
   const std::string& GetString() const;

   void SetString(const std::string& aString);
   //@}

   //! @name Assignment operators.
   //@{
   //! Assign the ID to the null 'string'.
   void Clear() { mId = 0; }

   //! Assign one ID to another.
   UtStringId& operator=(const UtStringId& aRhs) = default;
   UtStringId& operator=(UtStringId&& aRhs) noexcept = default;

   //! Assign a string to a string ID.
   UtStringId& operator=(const std::string& aRhs)
   {
      SetString(aRhs);
      return *this;
   }

   UtStringId& operator=(const char* aRhsPtr);
   //@}

   //! @name Relational operators
   //@{
   //! Compare two string ID's for equality.
   bool operator==(const UtStringId& aRhs) const { return (mId == aRhs.mId); }

   bool operator==(const std::string& aString) const;

   bool operator==(const char* aStringPtr) const;

   //! Compare for inequality vs. another ID.
   bool operator!=(const UtStringId& aRhs) const { return (mId != aRhs.mId); }

   //! Compare for inequality vs. an int.
   bool operator!=(int aId) const { return (mId != aId); }
   bool operator!=(const std::string& aString) const { return !operator==(aString); }
   bool operator!=(const char* aStringPtr) const { return !operator==(aStringPtr); }

   //! operator< implemented to allow use by associative containers and algorithms.
   bool operator<(const UtStringId& aRhs) const { return mId < aRhs.mId; }
   bool operator>(const UtStringId& aRhs) const { return mId > aRhs.mId; }
   //@}

   //! @name Interface for hash_map and hash_set.
   //! The following implements the interface that allows an instance of this class to be used as the function object
   //! that defines the hash function for hash_map and hash_set. It could have been defined as a different class,
   //! but UtMappedList assumes that hash function is provided by the key class.
   //!
   //! hash_map and hash_set are non-standard, but things are defined so they won't be used unless they are available.
   //@{

   //! Microsoft Developer Studio requires the following definitions.
   //! They cannot be defined with Developer Studio 6, however, because it causes a compile error.
#if defined(_WIN32) && _MSC_VER >= 1300
   static const size_t bucket_size = 4;
   static const size_t min_buckets = 8;
#endif

   //! All implementations seem to require this.
   size_t operator()(const UtStringId& aStringId) const { return static_cast<size_t>(aStringId.GetId()); }

   //! Only Microsoft Developer Studio seems to require this.
   bool operator()(const UtStringId& aLhs, const UtStringId& aRhs) const { return (aLhs < aRhs); }
   //@}

   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtStringId& aStringId);

   static UtDictionary& GetDictionary();

   static void       ResetDictionary();
   static int        UnsafeGetNumber(UtStringId aId) { return aId.mId; }
   static UtStringId UnsafeFindStringId(int aNumber);
   static bool       Exists(const std::string& aString);

protected:
   //! Get the string ID.
   int GetId() const { return mId; }

   //! Set the string ID.
   void SetId(int aId) { mId = aId; }

   static const std::string& GetString(int aId);
   static int                GetId(const std::string& aString);
   static int                GetNumber(const std::string& aString);

   //! The string ID.
   int mId;
};

UT_EXPORT std::string operator+(UtStringId aLhs, const std::string& aRhs);
UT_EXPORT std::string operator+(UtStringId aLhs, const char* aRhsPtr);
UT_EXPORT std::string operator+(const std::string& aLhs, UtStringId aRhs);
UT_EXPORT std::string operator+(const char* aLhsPtr, UtStringId aRhs);
UT_EXPORT bool        operator==(const std::string& aLhs, UtStringId aRhs);
UT_EXPORT bool        operator==(const char* aLhsPtr, UtStringId aRhs);

namespace std
{
template<>
struct hash<UtStringId>
{
   size_t operator()(const UtStringId aId) const noexcept { return std::hash<int>{}(UtStringId::UnsafeGetNumber(aId)); }
};
} // namespace std
#endif
