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

#ifndef VACALLBACKHOLDER_HPP
#define VACALLBACKHOLDER_HPP

#include "vespatk_export.h"

#include <list>

namespace vespa
{
class VaCallback;
class VaCallbackGroupId;
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
//!       VaCallbackHolder   mCallbacks;
//! };
//!
//! // And then in the constructor.
//! mCallbacks.Add(SensorTurnedOn.Connect(&MyObserver::SensorTurnedOn, this));
//! \endcode
class VESPATK_EXPORT VaCallbackHolder
{
public:
   //! Constructor.
   VaCallbackHolder()
      : mCallbacks()
   {
   }

   //! Copy Constructor.
   //! The callbacks are NOT copied. An empty callback list is created.
   VaCallbackHolder(const VaCallbackHolder& /*aSrc*/)
      : mCallbacks()
   {
   }

   //! Destructor.
   //! All callback connections are severed.
   ~VaCallbackHolder() { Clear(); }

   int         Add(VaCallback* aCallbackPtr);
   VaCallback* Remove(int aCallbackUID);
   bool        Disconnect(int aCallbackUID);

   bool Exists(int aCallbackUID) const;
   bool Exists(int aCallbackUID, const vespa::VaCallbackGroupId& aCallbackGroupId) const;

   //! Convenience operator for Add()
   void operator+=(VaCallback* aCallbackPtr) { Add(aCallbackPtr); }

   //! Delete all registered callbacks.
   //! This method disconnects all callbacks from their respective callback lists
   //! and deletes the callback object.
   void Clear();

   //! Return 'true' if no callbacks are contained
   bool Empty() const { return mCallbacks.empty(); }

   //! Return the number of callbacks contained
   std::size_t Size() const { return mCallbacks.size(); }

private:
   std::list<VaCallback*> mCallbacks;
};
} // end namespace vespa
#endif
