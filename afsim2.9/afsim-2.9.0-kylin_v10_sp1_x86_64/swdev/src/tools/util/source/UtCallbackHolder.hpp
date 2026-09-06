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

#ifndef UTCALLBACKHOLDER_HPP
#define UTCALLBACKHOLDER_HPP

#include "ut_export.h"

#include <memory>
#include <vector>

#include "UtCallback.hpp"

// ----------------------------------------------------------------------------
// A helper class.
// ----------------------------------------------------------------------------

//! A helper class that will automatically disconnect observers on deletion.
//!
//! This is a helper class that can be used a member variable in an class that
//! connects to callbacks. A programmer can store all the callbacks in this
//! object and when it is deleted, all of the callbacks will automatically be
//! disconnected.
//!
//! \code
//! Class MyObserver
//! {
//!    public:
//!       void SensorTurnedOn(double     aSimTime,
//!                           WsfSensor* aSensorPtr);
//!    private:
//!       UtCallbackHolder   mCallbacks;
//! };
//!
//! // And then in the constructor.
//! mCallbacks.Add(SensorTurnedOn.Connect(&MyObserver::SensorTurnedOn, this));
//! \endcode
class UT_EXPORT UtCallbackHolder
{
public:
   //! Constructor.
   UtCallbackHolder() = default;

   //! Copy Constructor.
   //! The callbacks are NOT copied. An empty callback list is created.
   UtCallbackHolder(const UtCallbackHolder& /*aSrc*/)
      : mCallbacks()
   {
   }
   UtCallbackHolder& operator=(const UtCallbackHolder&) = delete;

   //! Destructor.
   //! All callback connections are severed.
   ~UtCallbackHolder() = default;

   void Add(std::unique_ptr<UtCallback> aCallbackPtr);

   std::unique_ptr<UtCallback> Remove(UtCallback* aCallbackPtr);

   //! Convenience operator for Add()
   void operator+=(std::unique_ptr<UtCallback> aCallbackPtr) { Add(std::move(aCallbackPtr)); }

   //! Delete all registered callbacks.
   //! This method disconnects all callbacks from their respective callback lists
   //! and deletes the callback object.
   void Clear();

   //! Return 'true' if no callbacks are contained
   bool Empty() const { return mCallbacks.empty(); }

private:
   std::vector<std::unique_ptr<UtCallback>> mCallbacks;
};

#endif
