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

#ifndef WSFTRACKID_HPP
#define WSFTRACKID_HPP

#include "wsf_export.h"

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>

#include "UtScriptAccessible.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "WsfStringId.hpp"

//! Provide unique identification of a track within the simulation.
//!
//! A 'track ID' is simply a concatenation of two items:
//!   - The platform ID of 'owner' of the track.
//!   - The owners local track number of the track.
class WSF_EXPORT WsfTrackId : public UtScriptAccessible
{
public:
   WsfTrackId()
      : mOwningPlatformId()
      , mLocalTrackNumber(0)
   {
   }

   WsfTrackId(WsfStringId aOwningPlatformId, int aLocalTrackNumber)
      : mOwningPlatformId(aOwningPlatformId)
      , mLocalTrackNumber(aLocalTrackNumber)
   {
   }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   const char* GetScriptClassName() const override { return "WsfTrackId"; }

   //! Return 'true' if two track IDs arr 'equal'.
   //!
   //! Two track ID's are equal only if both the owning platform ID and local track number are identical.
   bool operator==(const WsfTrackId& aRhs) const
   {
      return ((mOwningPlatformId == aRhs.mOwningPlatformId) && (mLocalTrackNumber == aRhs.mLocalTrackNumber));
   }

   //! Return 'true' if two IDs are 'not equal'.
   //!
   //! The two track ID's are not equal if either the owning platform ID or local track number are not equal.
   bool operator!=(const WsfTrackId& aRhs) const { return (!operator==(aRhs)); }

   //! Return 'true' if 'this' is less than the supplied track ID.
   //!
   //! Defining operator\< allows objects of this type to be used in sortable
   //! containers or by algorithms that use ordering.
   bool operator<(const WsfTrackId& aRhs) const
   {
      if (mOwningPlatformId < aRhs.mOwningPlatformId)
      {
         return true;
      }
      else if (mOwningPlatformId > aRhs.mOwningPlatformId)
      {
         return false;
      }
      return (mLocalTrackNumber < aRhs.mLocalTrackNumber);
   }

   //! Get the owning platform ID (from WsfPlatform::GetNameId());
   WsfStringId GetOwningPlatformId() const { return mOwningPlatformId; }

   //! Set the owning platform ID (from WsfPlatform::GetNameId());
   void SetOwningPlatformId(WsfStringId aOwningPlatformId) { mOwningPlatformId = aOwningPlatformId; }

   //! Get the platform-relative local track number
   int GetLocalTrackNumber() const { return mLocalTrackNumber; }

   //! Set the platform-relative local track number
   void SetLocalTrackNumber(int aLocalTrackNumber) { mLocalTrackNumber = aLocalTrackNumber; }

   //! Indicates whether the ID has been set to reference a platform.
   bool IsNull() const { return mOwningPlatformId.IsNull(); }

   //! Increment the local track number.  This can be used to increment an
   //! object being used as a source of track numbers for the platform.
   int IncrementLocalTrackNumber() { return ++mLocalTrackNumber; }

   //! Invalidate the ID by zeroing out the owning platform and local track number.
   void Null()
   {
      mOwningPlatformId = nullptr;
      mLocalTrackNumber = 0;
   }

   friend WSF_EXPORT std::ostream& operator<<(std::ostream& aOut, const WsfTrackId& aTrackId);
   friend WSF_EXPORT std::istream& operator>>(std::istream& aIn, WsfTrackId& aTrackId);

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
   size_t operator()(const WsfTrackId& aTrackId) const { return static_cast<size_t>(aTrackId.GetLocalTrackNumber()); }

   //! Only Microsoft Developer Studio seems to require this.
   bool operator()(const WsfTrackId& aLhs, const WsfTrackId& aRhs) const { return (aLhs < aRhs); }
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mOwningPlatformId& mLocalTrackNumber;
   }

private:
   WsfStringId mOwningPlatformId; //!< Name Id of Owning platform
   int32_t     mLocalTrackNumber; //!< Platform-relative track number
};

#endif
