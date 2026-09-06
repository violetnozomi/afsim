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

#ifndef UT_LOG_BATCH_HPP
#define UT_LOG_BATCH_HPP

#include "ut_export.h"

#include <string>

#include "UtLogMessage.hpp"
#include "UtLogStream.hpp"

namespace ut
{
namespace log
{
//! The Batch class provides an interface for creating custom MessageStream's.
//! All of the Batch's types are included in the created Message's types.
//! Additionally, the Batch may hold a "batch" of messages which are guaranteed to
//!    be processed together in multi-threaded environments.
class UT_EXPORT Batch final : public detail::MessageReceiver
{
public:
   Batch() = delete;
   explicit Batch(TypeList aTypes) noexcept;

   //! Publishes the message batch
   ~Batch() noexcept;

   //! Used by MessageStream to add a message to the message batch.
   void AddMessage(Message aMessage) noexcept;

   //! Sends the message batch to the Publisher.
   //! Batches are always guaranteed to be processed together in multi-threaded environments.
   //! Does not affect streams that have been created but have not finished yet.
   size_t Publish() noexcept;

   //! Clears the message batch.
   //! Nothing is sent to the Publisher.
   //! Does not affect streams that have been created but have not finished yet.
   void Clear() noexcept;

   //! Similar to the global functions of the same names.
   //! The created streams are configured to add to the batch instead of sending directly to the Publisher.
   //! mDefaultTypes are always included in the type.
   //! {
   MessageStream write(TypeList aTypes = TypeList()) & noexcept;
   MessageStream fatal(TypeList aTypes = TypeList()) & noexcept;
   MessageStream error(TypeList aTypes = TypeList()) & noexcept;
   MessageStream warning(TypeList aTypes = TypeList()) & noexcept;
   MessageStream debug(TypeList aTypes = TypeList()) & noexcept;
   MessageStream info(TypeList aTypes = TypeList()) & noexcept;
   MessageStream developer(TypeList aTypes = TypeList()) & noexcept;
   //! }

private:
   void ReceiveMessage(Message&& aMessage) noexcept final;

   TypeList             mDefaultTypes;
   std::vector<Message> mMessages;
};
} // namespace log
} // namespace ut

#endif
