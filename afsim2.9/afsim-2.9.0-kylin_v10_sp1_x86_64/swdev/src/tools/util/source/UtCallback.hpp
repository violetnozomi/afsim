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

#ifndef UTCALLBACK_HPP
#define UTCALLBACK_HPP

#include "ut_export.h"

//! A 'simplified' type-safe callback mechanism.
//!
//! This class defines templates that provides type-safe callbacks. Callback objects are registered
//! with callback lists. Connections between callback lists and registered callback objects are
//! automatically broken if either is deleted.
//!
//! The implementation is pretty funky, but drastic measures were required to
//! make it work with older compilers. Some of the stuff in here is very similar
//! to the signal/slot mechanism that is present in 'boost'.

#include <algorithm>
#include <functional>
#include <list>

#include "UtBinder.hpp"
class UtCallback;
class UtCallbackList;

//! A helper class to link the callback with the callback list.
class UT_EXPORT UtCallbackLink
{
public:
   friend class UtCallback;

   UtCallbackLink(UtCallbackList* aCallbackListPtr)
      : mCallbackListPtr(aCallbackListPtr)
   {
   }
   ~UtCallbackLink() = default; // To avoid warnings about virtual methods without virtual destructor
   void Disconnect(UtCallback* aCallbackPtr);
   void Block(UtCallback* aCallbackPtr, bool aBlock);

protected:
   void            Merge(UtCallbackList* aOtherCallbackList);
   UtCallbackList* mCallbackListPtr;
};


//! The non-templated base class for a callback.
//! The lowest level of the callback maintains the connection to the non-templated callback list.
//! No knowledge of the derived callback list type is required because the callback list pointer is
//! only used to invoke the Disconnect method, which doesn't require the derived type.
//! note: Not DLL exporting class due to visual studio behavior of exporting derived classes
class UtCallback
{
public:
   friend class UtCallbackList;

   UT_EXPORT UtCallback()
      : mCallbackLinkPtr(nullptr)
      , mIsBlocked(false)
   {
   }
   UT_EXPORT UtCallback(const UtCallback& /*aSrc*/)
      : mCallbackLinkPtr(nullptr)
      , mIsBlocked(false)
   {
   }
   UT_EXPORT virtual ~UtCallback();

   //! Disconnect from the callback list to which this is connected.
   //! This does nothing if not currently connected.
   //!
   //! This is declared inline after UtCallbackList so it can invoke UtCallbackList::Disconnect()
   UT_EXPORT void Disconnect();

   //! Sets the callback to blocked or unblocked.
   //! Blocking the callback will temporarily prevent it from being invoked.
   //! Unblocking the callback resumes normal behavior.
   UT_EXPORT void Block(bool aBlock = true);

   //! Unblocks the callback.  See Block().
   void Unblock() { Block(false); }

   //! Returns true if this callback is currently blocked.
   bool IsBlocked() const { return mIsBlocked; }

private:
   void Merge(UtCallbackList* aOtherCallbackList) { mCallbackLinkPtr->Merge(aOtherCallbackList); }

   //! Assignment operator declared but not define to prevent use.
   UtCallback& operator=(const UtCallback&) = delete;

   //! The pointer to the UtCallbackLink that links the callback with its callback list.
   UtCallbackLink* mCallbackLinkPtr;
   bool            mIsBlocked;
};

//! The non-templated base class for the callback list.
//! The base class UtCallback maintains a pointer to an instance of this and it uses it to invoke
//! the Disconnect method.
class UT_EXPORT UtCallbackList
{
public:
   friend class UtCallback;
   friend class UtCallbackLink;
   using CallbackLink = UtCallbackLink;

   using cUT_SERIALIZE_IGNORE = bool;

   using ListType = std::list<UtCallback*>;
   using IterType = std::list<UtCallback*>::iterator;

   explicit UtCallbackList();
   explicit UtCallbackList(const UtCallbackList& /*aSrc*/);
   virtual ~UtCallbackList();

   //! Disconnect the indicated callback from the callback list.
   //! Note: The callback object (pointed to by aCallbackPtr) is NOT deleted. It can be
   //! reattached to another callback list.
   //! @param aCallbackPtr The callback to be disconnected.
   void Disconnect(UtCallback* aCallbackPtr);

   //! Disconnect all callbacks.
   //! @note The callback objects are not deleted.
   void DisconnectAll();

   //! Returns 'true' if the callback list contains no callbacks
   bool IsEmpty() const { return mCallbackList.empty(); }

protected:
   //! Connect the indicated callback to the callback list.
   //! If the callback is currently attached to a callback list then it will be
   //! disconnected from that list before connecting to this list.
   //! @param aCallbackPtr The callback to be connected.
   void ConnectP(UtCallback* aCallbackPtr);

   void MergeP(UtCallbackList& aOtherCallbackList);

   ListType mCallbackList;

private:
   ListType mBlockedCallbackList;
   //! Assignment operator declared but not defined to prevent use.
   UtCallbackList& operator=(const UtCallbackList&) = delete;
};

#include "UtCallbackN.hpp"

#endif
