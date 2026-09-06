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

#ifndef UT_LOG_STREAM_HPP
#define UT_LOG_STREAM_HPP

#include <sstream>

#include "UtLogMessage.hpp"
#include "UtTypeTraits.hpp"

namespace ut
{
namespace log
{
//! MessageStreams are objects that simplify the creation and publication of Messages.
//! Designed to make writing messages similar to writing to standard streams.
//! The preferred way to create new MessageStreams is via the helper functions in UtLog.hpp or via a ut::log::Batch.
class UT_EXPORT MessageStream final : public detail::MessageReceiver
{
public:
   explicit MessageStream(TypeList aTypes, detail::MessageReceiver* aReceiver = nullptr) noexcept;

   //! If the Message is non-empty, sends it to the Publisher.
   //! A Message is considered non-empty if it has any data or Notes.
   //! Otherwise, does nothing.
   ~MessageStream() noexcept;

   MessageStream(const MessageStream&) = delete;
   MessageStream& operator=(const MessageStream&) = delete;

   //! Moves MessageStream to a new location.
   //! Moved from MessageStream is put into an invalid state.
   MessageStream(MessageStream&& aOther) noexcept;
   MessageStream& operator=(MessageStream&& aOther) = delete;

   //! Clears the Message contents, including data and notes.
   //! The Message is NOT sent to the Publisher.
   //! Does NOT clear types.
   void Clear() noexcept;

   //! If IsEmpty() is false, Sends the message to its receiver.
   //! If aReceiver is provided, uses it as the destination.
   //! Otherwise, uses the one provided by the constructor.
   //! Uses the Publisher if no receiver was provided to the constructor.
   //! Afterwards, the Message is cleared.
   //! Returns true if anything was sent.
   bool Send(detail::MessageReceiver* aReceiver = nullptr) noexcept;

   //! Same as Send(), but skips the IsEmpty() check.
   //! USE SPARINGLY.
   void ForceSend(detail::MessageReceiver* aReceiver = nullptr) noexcept;

   //! Returns true if the Message data is empty and there are no Notes.
   bool IsEmpty() const noexcept;

   //! Adds data to the stream. Follows the same syntax as std::ostream.
   //! SFINAE allows IDEs to detect errors at call sites.
   template<typename T, typename = ut::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
   MessageStream&& operator<<(const T& aValue)
   {
      mStream << aValue;
      return std::move(*this); // std::move is required here.
   }

   //! Returns an ostream reference that can be passed to functions that take a std::ostream&.
   std::ostream& GetStream() & noexcept;

   //! Returns a MessageStream configured to add its message as a note on the calling object upon destruction.
   //! Can only be called from an lvalue to prevent this situation:
   //! ``` auto out = ut::log::info().AddNote();
   //! Upon destruction, out would write to a value that no longer exists.
   MessageStream AddNote(TypeList aTypes = TypeList()) & noexcept;

   //! Appends a note item to the current message.
   //! Users should prefer AddNote() to InsertNote().
   void InsertNote(Message aNote) noexcept;

   //! Returns the created message and clears the stream.
   Message TakeMessage() noexcept;

private:
   void ReceiveMessage(Message&& aMessage) noexcept final;

   std::ostringstream mStream;

   const TypeList       mTypes;
   std::vector<Message> mNotes;

   //! If null, sends directly to Publisher.
   //! Otherwise sends to receiver.
   detail::MessageReceiver* const mReceiver = nullptr;
};
} // namespace log
} // namespace ut

#endif
