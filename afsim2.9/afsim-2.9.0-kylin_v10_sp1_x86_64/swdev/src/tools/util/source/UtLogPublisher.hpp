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

#ifndef UT_LOG_PUBLISHER_HPP
#define UT_LOG_PUBLISHER_HPP

#include "ut_export.h"

#include <mutex>
#include <ostream>
#include <sstream>
#include <vector>

#include "UtLogMessage.hpp"
#include "UtSpan.hpp"

namespace ut
{
namespace log
{
class SubscriberBase;

//! Publisher is a singleton that manages Subscribers.
//! All functions in the public API are thread-safe.
//! While most of its public API is primarily intended for internal library use,
//!   users should note the presence of functions to modify the subscriptions for console-stdout.
//! The console is a special case and is printed to directly from the Publisher.
//!    Data written to the console by the Publisher is also published as "raw" messages WITHOUT notes.
//! Any data written directly to std::cout, std::cerr, or std::clog may be captured by
//!    the publisher and sent to subscribers.
//! By default it is subscribed to PresetRelease (Fatal, Error, Warning, and Info).
class UT_EXPORT Publisher final : public detail::MessageReceiver
{
public:
   //! Note: Fails assertion if not all Subscribers are unregistered.
   ~Publisher() noexcept;

   Publisher(const Publisher&) = delete;
   Publisher& operator=(const Publisher&) = delete;
   Publisher(Publisher&&)                 = delete;
   Publisher& operator=(Publisher&&) = delete;

   //! Registers a subscriber, allowing it to receive messages.
   //! This function is primarily intended for use by Subscriber internal functions.
   //! Redundant calls to this will not result in multiple copies sent.
   static void RegisterSubscriber(SubscriberBase* aSub) noexcept;

   //! Unregisters a subscriber, stopping it from receiving any messages.
   //! This function is primarily intended for use by Subscriber internal functions.
   //! Users should prefer Subscriber::ResetSubscriptions().
   //! Redundant calls to this function are safe.
   static void UnregisterSubscriber(SubscriberBase* aSub) noexcept;

   //! Returns true if the Subscriber is registered by the Publisher.
   static bool IsSubscriberRegistered(const SubscriberBase* aSub) noexcept;

   //! Sends a Message to all appropriate Subscribers.
   //! This function is primarily intended for use by MessageStream internal functions.
   //! Before processing aMessage, flushes and publishes all the standard stream redirectors.
   //! Any "leftover" text in the redirector is written to the raw output immediately.
   static void Publish(const Message& aMessage) noexcept;

   //! Sends a block of Messages to all appropriate Subscribers.
   //! Messages are guaranteed to arrive together in multi-threaded environments.
   //! This function is primarily intended for use by Batch message batches.
   //! Before processing aMessage, flushes and publishes all the standard stream redirectors.
   //! Any "leftover" text in the redirector is written to the raw output immediately.
   static void Publish(const ut::span<Message> aMessages) noexcept;

   //! Breaks aLines into individual lines and publishes them individually.
   //! Any characters left after the last newline are not published.
   //! @param aLines is an in-out parameter. After calling, it contains any unpublished characters.
   static void PublishLines(std::string& aLines, TypeList aTypes) noexcept;

   //! These functions are the console-stdout analogs to the similarly named methods in Subscriber.
   //@{
   static void     ResetConsoleSubscriptions(TypeList aTypes = TypeList()) noexcept;
   static void     SubscribeConsoleTo(TypeList aTypes) noexcept;
   static void     UnsubscribeConsoleFrom(const TypeList& aTypes) noexcept;
   static bool     IsConsoleSubscribedTo(const std::string& aType) noexcept;
   static TypeList GetConsoleSubscriptions() noexcept;
   //@}

   //! Sets whether WriteRawToConsole writes to stdout or just publishes a "raw" message.
   //! Setting this to false will also prevent the console subcriptions from writing to stdout.
   //! By default, console output is enabled.
   static void SetConsoleEnabled(bool aEnable) noexcept;

   //! Returns true if console output is enabled.
   //! Enabled by default.
   static bool IsConsoleEnabled() noexcept;

   //! If aText is non-empty, then
   //!    1) If IsConsoleEnabled(), writes aText to stdout.
   //!    2) Publishes aText to the "raw" type. The published message will have no notes.
   //! If aFlush is true, flushes the stream after writing.
   static void WriteRawToConsole(const std::string& aText, bool aFlush) noexcept;

   //! Redirects the standard streams (cout, cerr, clog) through the Publisher.
   //! If a TypeList is provided, the redirected text will be published with the given types.
   //! If no TypeList is provided, cout is published as "info"; cerr and clog are published as "error".
   //! If aCapture is false, the value of aTypes does not matter.
   //@{
   static void CaptureStdStreams(bool aCapture) noexcept;
   static void CaptureStdStreams(bool aCapture, const TypeList& aTypes) noexcept;
   //@}

   //! Redirects std::cout through the Publisher.
   //! The redirected text will be published with the given types.
   //! If aCapture is false, the value of aTypes does not matter.
   static void CaptureStdOut(bool aCapture, const TypeList& aTypes = Message::Info()) noexcept;

   //! Redirects std::cerr AND std::clog through the Publisher.
   //! The redirected text will be published with the given types.
   //! If aCapture is false, the value of aTypes does not matter.
   static void CaptureStdErr(bool aCaptrue, const TypeList& aTypes = Message::Error()) noexcept;

   //! Default formatting for writing messages to std::ostream.
   //! Used to prepare text for console printing.
   //! Returns true if the message requires immediate flushing.
   //! DO NOT PASS STD::COUT TO THIS FUNCTION!
   static bool DefaultFormat(const Message& aMessage, std::ostream& aStream, int aDepth = 0);

   static Publisher* Instance() noexcept;

private:
   Publisher() noexcept;

   void ReceiveMessage(Message&& aMessage) noexcept final;

   static Publisher mInstance;

   // Not thread safe!
   // Intentionally not const
   typename std::vector<SubscriberBase*>::iterator Find(const SubscriberBase* aSub) noexcept;

   //! Redirects the standard output streams through the Publisher.
   //! Call `CaptureStdStreams()` or its kin to use this feature.
   //! To allow for safe multi-threaded access to the streams,
   //!    the virtual overrides forward to thread_local buffers.
   //! The thread_local buffers are declared and defined in the cpp file
   //!    because they cannot be dll exported from the header file.
   class Redirector final : public std::streambuf
   {
   public:
      //! Prepares the Redirector to redirect aStream.
      //! Call CaptureStream() to actually redirect the stream.
      explicit Redirector(std::ostream& aStream) noexcept;

      ~Redirector() noexcept override;

      //! If aCapture is true, captures mStream and forwards future contents
      //!    to the Publisher with type aTypes.
      //! If already capturing, adjusts the types.
      //! If aCapture is false, un-captures the stream.
      //! In this case, the value of aTypes does not matter.
      void CaptureStream(bool aCapture, const TypeList& aTypes) noexcept;

      //! Returns true if the redirector is redirecting the standard stream.
      bool IsCapturing() const noexcept;

   private:
      std::ostream&   mStream;
      std::streambuf* mBuffer = nullptr;
      TypeList        mTypes;

      //! Handles input of unformatted strings.
      //! Appends aText to a thread_local buffer.
      std::streamsize xsputn(const char* aText, std::streamsize aCount) override;

      //! Since *this never allocates a buffer,
      //!    any added text that does not go through xsputn will result in an overflow.
      //! Appends aCharacter to a thread_local buffer.
      int_type overflow(int_type aCharacter) override;

      //! Flushes the contents to the Publisher.
      //! Data will be broken into messages split at newlines.
      //! Afterwards, the thread_local buffer will be cleared.
      int sync() override;
   };

   //! Syncs the standard stream redirectors.
   //! Data will be broken into messages split at newlines.
   //! Any "leftover" data will be published.
   void SyncStdStreams() noexcept;

   // These pointers are non-owning.
   std::vector<SubscriberBase*> mSubscribers;
   std::recursive_mutex         mMutex;

   Redirector mCoutRedirector;
   Redirector mCerrRedirector;
   Redirector mClogRedirector;

   TypeList mConsoleSubscriptions;
   bool     mConsoleEnabled = true;
};
} // namespace log
} // namespace ut

#endif
