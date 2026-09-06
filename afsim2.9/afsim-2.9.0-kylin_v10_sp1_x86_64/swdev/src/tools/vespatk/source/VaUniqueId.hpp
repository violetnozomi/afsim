// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VaUniqueId_hpp
#define VaUniqueId_hpp

#include "vespatk_export.h"

#include <string>

//! A class that maintains a unique identifier for an object.
//!
//! It is sometimes beneficial for objects to have the concept of a unique
//! identifier that distinguishes it from all other objects.  This provides
//! such a capability.  Any object of any class that derives from this class
//! will be assigned a unique integer when it is created.  Copy construction
//! will create a new identifier for the new object.  Assignment will result
//! in the target object retaining its original value.  Deleting an object
//! will NOT result in the reassignment of a unique identifier.
//!
//! The unique identifier is an unsigned integer so it is very efficient key
//! for a map or a hash_map.

namespace vespa
{
class VESPATK_EXPORT VaUniqueId
{
public:
   //! Construct an object with the next unique identifier.
   VaUniqueId();

   //! Copy construct an object the next unique identifier.
   VaUniqueId(const VaUniqueId& aSrc, bool aCreateNewUniqueId = true);

   //! The assignment operator does nothing as it must preserve the unique
   //! identification of the target.
   VaUniqueId& operator=(const VaUniqueId& /* aRhs */) { return *this; }

   //! Get the unique identifier assigned to this object.
   unsigned int GetUniqueId() const { return mUniqueId; }

   const std::string& GetUniqueIdString() const { return mUniqueIdString; }

   //! Reset the unique identifier.
   static void ResetUniqueIds();

private:
   //! The unique ID that is assigned to this object.
   unsigned int mUniqueId;

   //! The unique ID as a string.
   std::string mUniqueIdString;

   //! The last unique ID that was generated.
   static unsigned int sLastUniqueId;
};
} // end namespace vespa
#endif
