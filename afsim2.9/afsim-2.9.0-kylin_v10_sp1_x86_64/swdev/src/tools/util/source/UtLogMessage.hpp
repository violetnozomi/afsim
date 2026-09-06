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

#ifndef UT_LOG_MESSAGE_HPP
#define UT_LOG_MESSAGE_HPP

#include "ut_export.h"

#include <string>
#include <vector>

namespace ut
{
namespace log
{
//! TypeList is essentially a std::set<std::string>.
//! The main reasons to use this over the alternative are...
//! 1) A constructor that takes a std::string.
//!    This allows for the elimination of a lot of code duplication.
//! 2) An Overlaps function that makes testing subscribers simpler.
//! 3) TypeList should run faster for the sizes of lists we expect to be working with.
class UT_EXPORT TypeList final
{
public:
   using iterator = typename std::vector<std::string>::const_iterator;

   TypeList() = default;
   TypeList(const char* aString) noexcept;
   TypeList(std::string aString) noexcept;
   TypeList(std::initializer_list<TypeList> aList) noexcept;

   bool Contains(const char* aString) const noexcept;
   bool Contains(const std::string& aString) const noexcept;

   bool Overlaps(const TypeList& aList) const noexcept;

   bool   Insert(const char* aString) noexcept;
   bool   Insert(const std::string& aString) noexcept;
   bool   Insert(std::string&& aString) noexcept;
   size_t Insert(const TypeList& aList) noexcept;
   size_t Insert(TypeList&& aList) noexcept;

   bool   Remove(const char* aString) noexcept;
   bool   Remove(const std::string& aString) noexcept;
   size_t Remove(const TypeList& aList) noexcept;

   iterator begin() const noexcept;
   iterator end() const noexcept;

   size_t size() const noexcept;
   bool   empty() const noexcept;

   void clear() noexcept;

private:
   std::vector<std::string> mData;
};

//! Messages are what are passed around in the ut::log framework.
//! They consist of 3 parts:
//!    1) Types: A set of strings containing information about a message.
//!          For example, it could contain an error code, a file and line number, or what plugin it's coming from.
//!          Additionally, the Type is used for UserType messages as a part of the Type.
//!    2) Data: The text of the message.
//!    3) Notes: Addenda to the message.
//!          For example, could contain hints for how to fix an error.
//! The easiest (and preferred) way to create new Messages is via a MessageStream object.
struct UT_EXPORT Message final
{
   static std::string Fatal();
   static std::string Error();
   static std::string Warning();
   static std::string Debug();
   static std::string Info();
   static std::string Developer();

   static std::string Format();
   static std::string Raw();

   static TypeList PresetErrors();
   static TypeList PresetWarnings();
   static TypeList PresetRelease();
   static TypeList PresetDebug();
   static TypeList PresetDeveloper();

   std::string          mData;
   TypeList             mTypes;
   std::vector<Message> mNotes;
};

// Forward declaration needed for MessageReceiver.
class MessageStream;

namespace detail
{
//! A MessageReceiver is any class that MessageStream can send a message to in its Send function.
//! Examples include: Publisher, Batch, Subscriber, and MessageStream.
struct MessageReceiver
{
   virtual ~MessageReceiver() noexcept                      = default;
   virtual void ReceiveMessage(Message&& aMessage) noexcept = 0;
};
} // namespace detail
} // namespace log
} // namespace ut

#endif
