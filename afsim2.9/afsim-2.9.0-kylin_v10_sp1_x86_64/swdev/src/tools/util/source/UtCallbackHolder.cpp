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

#include "UtCallbackHolder.hpp"

#include "UtCallback.hpp"

void UtCallbackHolder::Add(std::unique_ptr<UtCallback> aCallbackPtr)
{
   mCallbacks.push_back(std::move(aCallbackPtr));
}

//! Delete all registered callbacks.
//! This method disconnects all callbacks from their respective callback lists
//! and deletes the callback object.
void UtCallbackHolder::Clear()
{
   mCallbacks.clear();
}

//! Remove the given callback from the list.
//! Ownership of the callback object is transferred to the caller
std::unique_ptr<UtCallback> UtCallbackHolder::Remove(UtCallback* aCallbackPtr)
{
   std::unique_ptr<UtCallback> callbackPtr{nullptr};
   auto                        it = std::find_if(std::begin(mCallbacks),
                          std::end(mCallbacks),
                          [aCallbackPtr](const std::unique_ptr<UtCallback>& cb) { return (cb.get() == aCallbackPtr); });

   if (it != std::end(mCallbacks))
   {
      callbackPtr = std::move(*it);
      mCallbacks.erase(it);
   }

   return callbackPtr;
}
