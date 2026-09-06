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

#include "UtLogSubscriber.hpp"

#include "UtLogPublisher.hpp"

ut::log::SubscriberBase::SubscriberBase(TypeList aSubscriptions, bool aRegister /*= true*/) noexcept
   : mSubscriptions(std::move(aSubscriptions))
{
   if (aRegister)
   {
      ut::log::Publisher::RegisterSubscriber(this);
   }
}

// virtual
ut::log::SubscriberBase::~SubscriberBase() noexcept
{
   if (mRegistered)
   {
      ut::log::Publisher::UnregisterSubscriber(this);
   }
}

void ut::log::SubscriberBase::ResetSubscriptions(TypeList aTypes /*= TypeList()*/) noexcept
{
   Lock l(mMutex);
   mSubscriptions = std::move(aTypes);
}

void ut::log::SubscriberBase::SubscribeTo(TypeList aTypes) noexcept
{
   Lock l(mMutex);
   mSubscriptions.Insert(std::move(aTypes));
}

void ut::log::SubscriberBase::UnsubscribeFrom(const TypeList& aTypes) noexcept
{
   Lock l(mMutex);
   mSubscriptions.Remove(aTypes);
}

bool ut::log::SubscriberBase::IsSubscribedTo(const std::string& aType) const noexcept
{
   Lock l(mMutex);
   return mSubscriptions.Contains(aType);
}

ut::log::TypeList ut::log::SubscriberBase::GetSubscriptions() const noexcept
{
   Lock l(mMutex);
   return mSubscriptions;
}

bool ut::log::SubscriberBase::AddMessage(const Message& aMessage) noexcept
{
   Lock l(mMutex);
   if (mSubscriptions.Overlaps(aMessage.mTypes))
   {
      return AddMessageP(aMessage);
   }

   return false;
}

size_t ut::log::SubscriberBase::AddMessages(const ut::span<Message> aMessages) noexcept
{
   Lock l(mMutex);

   size_t count = 0;
   for (const auto& msg : aMessages)
   {
      if (mSubscriptions.Overlaps(msg.mTypes))
      {
         count += AddMessageP(msg);
      }
   }

   return count;
}

// protected
ut::log::Lock ut::log::SubscriberBase::LockMutex() const noexcept
{
   return Lock{mMutex};
}

// private
void ut::log::SubscriberBase::ReceiveMessage(Message&& aMessage) noexcept
{
   AddMessage(aMessage);
}


std::queue<ut::log::Message> ut::log::Subscriber::TakeQueue() noexcept
{
   std::queue<Message> theQueue;
   Lock                l = LockMutex();
   theQueue.swap(mQueue);
   return theQueue;
}

// private
// Not thread safe
bool ut::log::Subscriber::AddMessageP(const Message& aMessage) noexcept
{
   mQueue.emplace(aMessage);
   return true;
}
