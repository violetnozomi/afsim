// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtLogStream.hpp"

#include "UtLogBatch.hpp"
#include "UtLogPublisher.hpp"

ut::log::MessageStream::MessageStream(TypeList aTypes, detail::MessageReceiver* aReceiver) noexcept
   : mStream(std::ios::ate)
   , mTypes(std::move(aTypes))
   , mReceiver(aReceiver)
{
}

ut::log::MessageStream::~MessageStream() noexcept
{
   Send();
}

ut::log::MessageStream::MessageStream(MessageStream&& aOther) noexcept
   : mStream(aOther.mStream.str(),
             std::ios::ate) // Move construction of mStream does not work on Linux at the time of writing.
   , mTypes(aOther.mTypes)  // Constants are copied
   , mNotes(std::move(aOther.mNotes))
   , mReceiver(aOther.mReceiver)
{
   mStream.flags(aOther.mStream.flags());
   mStream.precision(aOther.mStream.precision());
   aOther.Clear();
}

void ut::log::MessageStream::Clear() noexcept
{
   mStream.str(std::string()); // Clears the contents
   mStream.clear();            // Clears the error flags
   mNotes.clear();
}

bool ut::log::MessageStream::Send(detail::MessageReceiver* aReceiver) noexcept
{
   if (IsEmpty())
   {
      // clears the stream's error flags
      Clear();
      return false;
   }

   ForceSend(aReceiver);
   return true;
}

void ut::log::MessageStream::ForceSend(detail::MessageReceiver* aReceiver) noexcept
{
   if (aReceiver != nullptr)
   {
      aReceiver->ReceiveMessage(TakeMessage());
   }
   else if (mReceiver != nullptr)
   {
      mReceiver->ReceiveMessage(TakeMessage());
   }
   else
   {
      ut::log::Publisher::Publish(TakeMessage());
   }
}

bool ut::log::MessageStream::IsEmpty() const noexcept
{
   return mStream.str().empty() && mNotes.empty();
}

std::ostream& ut::log::MessageStream::GetStream() & noexcept
{
   return mStream;
}

ut::log::MessageStream ut::log::MessageStream::AddNote(TypeList aTypes) & noexcept
{
   MessageStream retval(std::move(aTypes), this);

   retval.GetStream().flags(mStream.flags());
   retval.GetStream().precision(mStream.precision());

   return retval;
}

void ut::log::MessageStream::InsertNote(Message aNote) noexcept
{
   mNotes.emplace_back(std::move(aNote));
}

ut::log::Message ut::log::MessageStream::TakeMessage() noexcept
{
   Message retval;

   // Notes are moved, types are copied.
   retval.mData  = mStream.str();
   retval.mNotes = std::move(mNotes);
   retval.mTypes = mTypes;

   Clear();

   return retval;
}

void ut::log::MessageStream::ReceiveMessage(Message&& aMessage) noexcept
{
   InsertNote(std::move(aMessage));
}
