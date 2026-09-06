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

#include "UtLogPublisher.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>

#include "UtAlgorithm.hpp"
#include "UtLogBatch.hpp"
#include "UtLogSubscriber.hpp"

using Lock = std::unique_lock<std::recursive_mutex>;

//! Instance of global Publisher
ut::log::Publisher ut::log::Publisher::mInstance;

namespace
{
struct StdStreamBuffer
{
   ~StdStreamBuffer() noexcept { Sync(true); }

   void Sync(bool aIncludeTrailingCharacters)
   {
      if (aIncludeTrailingCharacters)
      {
         if (!mBuffer.empty() && mBuffer.back() != '\n')
         {
            mBuffer.push_back('\n');
         }
      }
      ut::log::Publisher::PublishLines(mBuffer, mTypes);
   }

   ut::log::TypeList mTypes;
   std::string       mBuffer;
   bool              mProcessing = false;
};

//! These are here to allow thread-safe usage of
static thread_local StdStreamBuffer sConsoleBuffer;
} // namespace

//! Note: it is legal to create references to the standard streams before they're initialized,
//!   as long as we do not modify or access them.
ut::log::Publisher::Publisher() noexcept
   : mCoutRedirector(std::cout)
   , mCerrRedirector(std::cerr)
   , mClogRedirector(std::clog)
   , mConsoleSubscriptions({Message::PresetDebug(), Message::Format()})
{
}

ut::log::Publisher::~Publisher() noexcept
{
   Lock l(mMutex);
   for (auto* sub : mSubscribers)
   {
      sub->mRegistered = false;
   }
}

// static
void ut::log::Publisher::RegisterSubscriber(SubscriberBase* aSub) noexcept
{
   if (aSub != nullptr)
   {
      auto& out = Publisher::mInstance;
      Lock  l(out.mMutex);
      if (out.Find(aSub) == out.mSubscribers.end())
      {
         out.mSubscribers.push_back(aSub);
         aSub->mRegistered = true;
      }
   }
}

// static
void ut::log::Publisher::UnregisterSubscriber(SubscriberBase* aSub) noexcept
{
   if (aSub->mRegistered)
   {
      auto& out = Publisher::mInstance;
      Lock  l(out.mMutex);
      auto  it = out.Find(aSub);
      if (it != out.mSubscribers.end())
      {
         (*it)->mRegistered = false;
         out.mSubscribers.erase(it);
      }
   }
}

// static
bool ut::log::Publisher::IsSubscriberRegistered(const SubscriberBase* aSub) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   return out.Find(aSub) != out.mSubscribers.end();
}

// static
void ut::log::Publisher::Publish(const Message& aMessage) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.SyncStdStreams();
   for (auto sub : out.mSubscribers)
   {
      sub->AddMessage(aMessage);
   }

   if (out.mConsoleSubscriptions.Overlaps(aMessage.mTypes))
   {
      // Writing to cout will result in an infinite loop if the stream is being redirected.
      // Instead, we write directly to stdout.
      std::ostringstream oss;
      bool               needsFlush = DefaultFormat(aMessage, oss);
      WriteRawToConsole(oss.str(), needsFlush);
   }
}

// static
void ut::log::Publisher::Publish(const ut::span<Message> aMessages) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.SyncStdStreams();
   for (auto* sub : out.mSubscribers)
   {
      sub->AddMessages(aMessages);
   }

   std::ostringstream oss;
   bool               needsFlush = false;
   for (const auto& msg : aMessages)
   {
      if (out.mConsoleSubscriptions.Overlaps(msg.mTypes))
      {
         needsFlush |= DefaultFormat(msg, oss);
      }
   }
   WriteRawToConsole(oss.str(), needsFlush);
}

// static
void ut::log::Publisher::PublishLines(std::string& aLines, TypeList aTypes) noexcept
{
   ut::log::Batch messages{std::move(aTypes)};

   std::size_t beginIndex = 0;
   std::size_t endIndex   = aLines.find('\n');

   while (endIndex != std::string::npos)
   {
      messages.write() << aLines.substr(beginIndex, endIndex - beginIndex);

      beginIndex = endIndex + 1;
      endIndex   = aLines.find('\n', beginIndex);
   }

   if (beginIndex > 0)
   {
      aLines = aLines.substr(beginIndex);
   }
}

// static
void ut::log::Publisher::ResetConsoleSubscriptions(TypeList aTypes) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mConsoleSubscriptions = std::move(aTypes);
}

// static
void ut::log::Publisher::SubscribeConsoleTo(TypeList aTypes) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mConsoleSubscriptions.Insert(aTypes);
}

// static
void ut::log::Publisher::UnsubscribeConsoleFrom(const TypeList& aTypes) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mConsoleSubscriptions.Remove(aTypes);
}

// static
bool ut::log::Publisher::IsConsoleSubscribedTo(const std::string& aType) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   return out.mConsoleSubscriptions.Contains(aType);
}

// static
ut::log::TypeList ut::log::Publisher::GetConsoleSubscriptions() noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   return out.mConsoleSubscriptions;
}

// static
void ut::log::Publisher::SetConsoleEnabled(bool aEnable) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mConsoleEnabled = aEnable;
}

// static
bool ut::log::Publisher::IsConsoleEnabled() noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   return out.mConsoleEnabled;
}

// static
void ut::log::Publisher::WriteRawToConsole(const std::string& aText, bool aFlush) noexcept
{
   // Writing to cout will result in an infinite loop if the stream is being redirected.
   // Instead, we write directly to stdout.
   if (!aText.empty())
   {
      auto& out = Publisher::mInstance;
      Lock  l(out.mMutex);

      if (out.mConsoleEnabled)
      {
         std::fputs(aText.c_str(), stdout);
         if (aFlush)
         {
            std::fflush(stdout);
         }
      }

      // For devices mirroring the console, we re-publish this data as "raw" output.
      ut::log::Message msg;
      msg.mTypes = ut::log::Message::Raw();
      msg.mData  = aText;
      ut::log::Publisher::Publish(msg);
   }
}

// static
void ut::log::Publisher::CaptureStdStreams(bool aCapture) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mCoutRedirector.CaptureStream(aCapture, Message::Info());
   out.mCerrRedirector.CaptureStream(aCapture, Message::Error());
   out.mClogRedirector.CaptureStream(aCapture, Message::Error());
}

// static
void ut::log::Publisher::CaptureStdStreams(bool aCapture, const TypeList& aTypes) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mCoutRedirector.CaptureStream(aCapture, aTypes);
   out.mCerrRedirector.CaptureStream(aCapture, aTypes);
   out.mClogRedirector.CaptureStream(aCapture, aTypes);
}

// static
void ut::log::Publisher::CaptureStdOut(bool aCapture, const TypeList& aTypes) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mCoutRedirector.CaptureStream(aCapture, aTypes);
}

// static
void ut::log::Publisher::CaptureStdErr(bool aCapture, const TypeList& aTypes) noexcept
{
   auto& out = Publisher::mInstance;
   Lock  l(out.mMutex);
   out.mCerrRedirector.CaptureStream(aCapture, aTypes);
   out.mClogRedirector.CaptureStream(aCapture, aTypes);
}

// static
bool ut::log::Publisher::DefaultFormat(const Message& aMessage, std::ostream& aStream, int aDepth /*= 0*/)
{
   for (int i = 0; i < aDepth; i++)
   {
      // Using 4 spaces for indentation.
      aStream << "    ";
   }

   bool requiresFlush = false;

   if (aDepth > 0 && aMessage.mTypes.empty())
   {
      // Optimization path for notes w/o type.
      // Do nothing.
   }
   else if (aMessage.mTypes.Contains(Message::Fatal()))
   {
      aStream << "***** FATAL: ";
      requiresFlush = true;
   }
   else if (aMessage.mTypes.Contains(Message::Error()))
   {
      aStream << "***** ERROR: ";
      requiresFlush = true;
   }
   else if (aMessage.mTypes.Contains(Message::Warning()))
   {
      aStream << "***** WARNING: ";
      requiresFlush = true;
   }
   else if (aMessage.mTypes.Contains(Message::Info()) || aMessage.mTypes.Contains(Message::Format()))
   {
      // no tag
   }
   else if (aMessage.mTypes.Contains(Message::Debug()))
   {
      // The analyst team has decided that debug messages should not have a special tag.

      /* aStream << "***** DEBUG: "; */
   }
   else if (aMessage.mTypes.Contains(Message::Developer()))
   {
      aStream << "***** DEVELOPER: ";
   }
   else if (aDepth == 0 && !aMessage.mTypes.empty())
   {
      aStream << "***** UNKNOWN: ";
   }

   aStream << aMessage.mData << '\n';

   for (const Message& theNote : aMessage.mNotes)
   {
      requiresFlush |= DefaultFormat(theNote, aStream, aDepth + 1);
   }

   return requiresFlush;
}

// static
ut::log::Publisher* ut::log::Publisher::Instance() noexcept
{
   return &mInstance;
}

// private
void ut::log::Publisher::ReceiveMessage(Message&& aMessage) noexcept
{
   Publish(aMessage);
}

// private
// Not thread safe!
// Intentionally not const
typename std::vector<ut::log::SubscriberBase*>::iterator ut::log::Publisher::Find(const SubscriberBase* aSub) noexcept
{
   return std::find(mSubscribers.begin(), mSubscribers.end(), aSub);
}

void ut::log::Publisher::SyncStdStreams() noexcept
{
   if (!sConsoleBuffer.mProcessing)
   {
      sConsoleBuffer.mProcessing = true;
      sConsoleBuffer.Sync(true);
      sConsoleBuffer.mProcessing = false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// explicit
ut::log::Publisher::Redirector::Redirector(std::ostream& aStream) noexcept
   : mStream(aStream)
{
}

ut::log::Publisher::Redirector::~Redirector() noexcept
{
   if (mBuffer)
   {
      mStream.rdbuf(mBuffer);
      mBuffer = nullptr;
   }
}

void ut::log::Publisher::Redirector::CaptureStream(bool aCapture, const TypeList& aTypes) noexcept
{
   mTypes = aTypes;

   if (aCapture && !mBuffer)
   {
      mBuffer = mStream.rdbuf(this);
      mBuffer->pubsync();
   }
   else if (!aCapture && mBuffer)
   {
      mStream.rdbuf(mBuffer);
      mBuffer = nullptr;
      sConsoleBuffer.Sync(true);
   }
}

bool ut::log::Publisher::Redirector::IsCapturing() const noexcept
{
   return mBuffer != nullptr;
}

std::streamsize ut::log::Publisher::Redirector::xsputn(const char* aText, std::streamsize aCount)
{
   if (!ut::ranges::equal(sConsoleBuffer.mTypes, mTypes))
   {
      sConsoleBuffer.Sync(true);
      sConsoleBuffer.mTypes = mTypes;
   }

   sConsoleBuffer.mBuffer.append(aText, aCount);

   return aCount;
}

ut::log::Publisher::Redirector::int_type ut::log::Publisher::Redirector::overflow(int_type aCharacter)
{
   if (traits_type::not_eof(aCharacter))
   {
      char ch = traits_type::to_char_type(aCharacter);
      sConsoleBuffer.mBuffer.push_back(ch);
      return aCharacter;
   }
   return traits_type::eof();
}

int ut::log::Publisher::Redirector::sync()
{
   sConsoleBuffer.mProcessing = true;
   sConsoleBuffer.Sync(false);
   sConsoleBuffer.mProcessing = false;
   return 0;
}
