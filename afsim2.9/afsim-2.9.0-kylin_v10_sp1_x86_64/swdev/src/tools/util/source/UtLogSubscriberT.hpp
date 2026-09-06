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

#ifndef UT_LOG_SUBSCRIBER_T_HPP
#define UT_LOG_SUBSCRIBER_T_HPP

#include "UtLogPublisher.hpp"
#include "UtLogSubscriber.hpp"

namespace ut
{
namespace log
{
//! MessageProcessor defines the interface for types that SubscriberT uses to process Messages.
class MessageProcessor
{
public:
   //! Virtual destructor
   virtual ~MessageProcessor() noexcept = default;

   //! Function to be overridden to determine how messages are processed.
   virtual void Process(const Message& aMessage) = 0;

   //! Function called by SubscriberT before processing the first message.
   //! Not called if there are no messages to process.
   //! \param aCount is the number of messages to be processed.
   virtual void PreProcess(size_t aCount) {}

   //! Function called by SubscriberT after all messages have been processed.
   //! Not called if there are no messages processed.
   //! \param aCount is the number of messages processed.
   virtual void PostProcess(size_t aCount) {}
};

//! Specialization of Subscriber that simplifies common use cases.
//! Automatically processes messages on destruction.
//! Note: T must derive from MessageProcessor.
template<typename T>
class SubscriberT : public Subscriber
{
   static_assert(std::is_base_of<MessageProcessor, T>::value,
                 "Template argument must derive from ut::log::MessageProcessor.");

public:
   template<typename... Args>
   explicit SubscriberT(TypeList aSubscriptions, Args&&... aArgs)
      : Subscriber(std::move(aSubscriptions))
      , mProcessor(std::forward<Args>(aArgs)...)
   {
   }

   //! Processes messages and destroys the object.
   ~SubscriberT() noexcept override
   {
      ProcessMessages();
      ut::log::Publisher::UnregisterSubscriber(this);
   }

   //! Exposes the MessageProcessor.
   T& GetProcessor() { return mProcessor; }

   //! Exposes the MessageProcessor.
   const T& GetProcessor() const { return mProcessor; }

   //! Should be called periodically by owner to process message backlog.
   //! Returns the number of messages processed.
   //! Performs double-buffering so adding messages in another thread does not block.
   size_t ProcessMessages()
   {
      std::queue<Message> theQueue = TakeQueue();
      const size_t        count    = theQueue.size();

      if (count > 0)
      {
         mProcessor.PreProcess(count);

         while (!theQueue.empty())
         {
            Message item = std::move(theQueue.front());
            theQueue.pop();
            mProcessor.Process(item);
         }

         mProcessor.PostProcess(count);
      }

      return count;
   }

private:
   T mProcessor;
};
} // namespace log
} // namespace ut

#endif
