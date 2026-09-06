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

#include "UtLogBatch.hpp"

#include "UtLogPublisher.hpp"

ut::log::Batch::Batch(TypeList aTypes) noexcept
   : mDefaultTypes(aTypes)
{
}

ut::log::Batch::~Batch() noexcept
{
   Publish();
}

void ut::log::Batch::AddMessage(Message aMessage) noexcept
{
   mMessages.emplace_back(std::move(aMessage));
}

size_t ut::log::Batch::Publish() noexcept
{
   const size_t count = mMessages.size();
   if (count != 0)
   {
      ut::log::Publisher::Publish(mMessages);
      mMessages.clear();
   }
   return count;
}

void ut::log::Batch::Clear() noexcept
{
   mMessages.clear();
}

// -------------------------------------------------------------------------------------------------------

ut::log::MessageStream ut::log::Batch::write(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   return MessageStream(std::move(aTypes), this);
}

ut::log::MessageStream ut::log::Batch::fatal(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   aTypes.Insert(Message::Fatal());
   return MessageStream(std::move(aTypes), this);
}

ut::log::MessageStream ut::log::Batch::error(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   aTypes.Insert(Message::Error());
   return MessageStream(std::move(aTypes), this);
}

ut::log::MessageStream ut::log::Batch::warning(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   aTypes.Insert(Message::Warning());
   return MessageStream(std::move(aTypes), this);
}

ut::log::MessageStream ut::log::Batch::debug(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   aTypes.Insert(Message::Debug());
   return MessageStream(std::move(aTypes), this);
}

ut::log::MessageStream ut::log::Batch::info(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   aTypes.Insert(Message::Info());
   return MessageStream(std::move(aTypes), this);
}

ut::log::MessageStream ut::log::Batch::developer(TypeList aTypes) & noexcept
{
   aTypes.Insert(mDefaultTypes);
   aTypes.Insert(Message::Developer());
   return MessageStream(std::move(aTypes), this);
}

// private
void ut::log::Batch::ReceiveMessage(Message&& aMessage) noexcept
{
   AddMessage(std::move(aMessage));
}
