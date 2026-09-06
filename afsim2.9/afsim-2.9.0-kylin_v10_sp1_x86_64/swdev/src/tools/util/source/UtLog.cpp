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

#include "UtLog.hpp"

ut::log::MessageStream ut::log::write(TypeList aTypes) noexcept
{
   return MessageStream(std::move(aTypes));
}

ut::log::MessageStream ut::log::fatal(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Fatal());
   return MessageStream(std::move(aTypes));
}

ut::log::MessageStream ut::log::error(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Error());
   return MessageStream(std::move(aTypes));
}

ut::log::MessageStream ut::log::warning(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Warning());
   return MessageStream(std::move(aTypes));
}

ut::log::MessageStream ut::log::debug(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Debug());
   return MessageStream(std::move(aTypes));
}

ut::log::MessageStream ut::log::info(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Info());
   return MessageStream(std::move(aTypes));
}

ut::log::MessageStream ut::log::developer(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Developer());
   return MessageStream(std::move(aTypes));
}

void ut::log::newline(TypeList aTypes) noexcept
{
   aTypes.Insert(Message::Format());
   write(aTypes).ForceSend();
}

ut::log::DataMessageHelper::DataMessageHelper(MessageStream& aStream, std::string aLabel) noexcept
   : mStream(aStream)
   , mLabel(std::move(aLabel))
{
}

ut::log::DataMessageHelper::~DataMessageHelper() noexcept
{
   if (!mStream.IsEmpty())
   {
      mStream << mLabel;
   }
}
