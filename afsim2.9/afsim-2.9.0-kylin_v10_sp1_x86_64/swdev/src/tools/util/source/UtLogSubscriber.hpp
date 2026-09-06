// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_LOG_SUBSCRIBER_HPP
#define UT_LOG_SUBSCRIBER_HPP

#include "ut_export.h"

#include <mutex>
#include <queue>
#include <utility>

#include "UtLogMessage.hpp"
#include "UtSpan.hpp"

namespace ut
{
namespace log
{
using Lock = std::unique_lock<std::recursive_mutex>;
class Publisher;

namespace detail
{
//! Ideally, the lifetime rules of C++ would be enough to make this struct unnecessary.
//! However, there are a couple of ways to get around these rules that makes this necessary.
//! This struct allows only the registered flag to be made visible to the Publisher
//!    without exposing the entire internals of SubscriberBase.
class UT_EXPORT RegisterFlag
{
protected:
   friend class ut::log::Publisher;
   bool mRegistered = false;
};
} // namespace detail

//! SubscriberBase abstracts away the knowledge of the Publisher singleton
//! and provides a basic interface for receiving and processing messages.
//! A more complete processing interface is provided by Subscriber and SubscriberT.
class UT_EXPORT SubscriberBase : public detail::MessageReceiver, public detail::RegisterFlag
{
public:
   //! Sets subscriptions and can register with the Publisher.
   explicit SubscriberBase(TypeList aSubscriptions, bool aRegister = true) noexcept;

   SubscriberBase(const SubscriberBase&) = delete;
   SubscriberBase& operator=(const SubscriberBase&) = delete;

   //! Moves a Subscriber to a new location.
   //! Moved from Subscriber is unregistered from Publisher.
   SubscriberBase(SubscriberBase&& aSource) = delete;
   SubscriberBase& operator=(SubscriberBase&& aSource) = delete;

   //! Unregisters from Publisher.
   virtual ~SubscriberBase() noexcept;

   //! Resets the current subscriptions.
   void ResetSubscriptions(TypeList aTypes = TypeList()) noexcept;

   //! Subscribes to messages of type aType.
   void SubscribeTo(TypeList aTypes) noexcept;

   //! Unsubscribes from messages with typs in aTypes.
   void UnsubscribeFrom(const TypeList& aTypes) noexcept;

   //! Returns true if receiving messages of type aType.
   bool IsSubscribedTo(const std::string& aType) const noexcept;

   //! Returns a list of the types the subscriber is listening to.
   TypeList GetSubscriptions() const noexcept;

   //! Adds a message to the subscriber if the subscriber is subscribed to its type.
   //! Returns true if the message was added.
   bool AddMessage(const Message& aMessage) noexcept;

   //! Adds a list of messages to the subscriber if the subscriber is subscribed to their types.
   //! Messages are guaranteed to be processed together in multi-threaded environments.
   //! Returns the number of messages added.
   size_t AddMessages(const ut::span<Message> aMessages) noexcept;

protected:
   //! Not thread safe. Caller responsible for locking before calling.
   //! Called immediately after aMessage has been published.
   virtual bool AddMessageP(const Message& aMessage) noexcept = 0;

   //! Locks the mutex and returns the scoped lock.
   Lock LockMutex() const noexcept;

private:
   void ReceiveMessage(Message&& aMessage) noexcept final;

   mutable std::recursive_mutex mMutex;
   TypeList                     mSubscriptions;
};

//! Provides a buffer of messages that can be safely taken and processed.
class UT_EXPORT Subscriber : public SubscriberBase
{
public:
   using SubscriberBase::SubscriberBase;

   //! Returns a queue of the received messages and resets the internal queue.
   //! Similar to double-buffering to minimize the time the mutex is locked while processing.
   std::queue<Message> TakeQueue() noexcept;

private:
   //! Appends aMessage to mQueue for future processing.
   //! Called immediately after aMessage is published.
   bool AddMessageP(const Message& aMessage) noexcept override;

   std::queue<Message> mQueue;
};
} // namespace log
} // namespace ut

#endif
