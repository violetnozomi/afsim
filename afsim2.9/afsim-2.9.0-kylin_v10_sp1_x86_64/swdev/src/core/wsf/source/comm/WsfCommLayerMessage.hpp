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

#ifndef WSFCOMMLAYERMESSAGE_HPP
#define WSFCOMMLAYERMESSAGE_HPP

#include "wsf_export.h"

#include <string>

namespace wsf
{
namespace comm
{
namespace layer
{

//! The Message class for distinguishing between layer messages.
//! Users may create their own instances for new layer implementations.
//! Implementations only parse the size_t primitive, and the character array is
//! used for output, debugging, and maintenance.
//!
//! Note: Should be updated when string view is available with C++17
//!
//! These messages may have different meanings to each layer implementation.
//! The use case, however, is generally consistent across each layer.
class WSF_EXPORT Message
{
public:
   constexpr Message(size_t aValue, char const* aName, bool aLowerSender)
      : mValue(aValue)
      , mName(aName)
      , mLower(aLowerSender)
   {
   }
   ~Message() = default;

   constexpr size_t      GetValue() const { return mValue; }
   constexpr char const* GetName() const { return mName; }
   constexpr bool        FromLower() const { return mLower; }

private:
   size_t      mValue;
   char const* mName;
   bool        mLower;
};

//! Common layer type messages used in the AFSIM core.
//! Note that the term "lower" refers to the distance from hardware,
//! s.t. the "lowest" layer is typically the physical layer.
//! Note that going "down" the stack means going to a "lower" layer,
//! regardless of sending or receiving (Sending a message goes "down"
//! the stack, while receiving a message goes "up" the stack.)
//!
//! The names here generalize the direction of travel in the stack,
//! although this may not originate at either end of the stack.
//! The name character string provides a general message TO the
//! intended recipient, such that a message traveling down the
//! stack is from an upper layer.
//!
//! General message instances.
static constexpr const Message cDOWN_ACK_SEND(0U, "UPPER_LAYER_ACK_SEND", false);
static constexpr const Message cDOWN_NACK_SEND(1U, "UPPER_LAYER_NACK_SEND", false);
static constexpr const Message cUP_ACK_SEND(2U, "LOWER_LAYER_ACK_SEND", true);
static constexpr const Message cUP_NACK_SEND(3U, "LOWER_LAYER_NACK_SEND", true);
static constexpr const Message cDOWN_ACK_RECEIVE(4U, "UPPER_LAYER_ACK_RECEIVE", false);
static constexpr const Message cDOWN_NACK_RECEIVE(5U, "UPPER_LAYER_NACK_RECEIVE", false);
static constexpr const Message cUP_ACK_RECEIVE(6U, "LOWER_LAYER_ACK_RECEIVE", true);
static constexpr const Message cUP_NACK_RECEIVE(7U, "LOWER_LAYER_NACK_RECEIVE", true);
//! Layer specific instances.
//! Network Layer - These messages are used in the same layer, but on the
//! opposite side of the stack (send vs. receive).
static constexpr const Message cNETWORK_FORWARD(8U, "FORWARD_MESSAGE", false);
static constexpr const Message cNETWORK_MULTIPLE_SEND(9U, "MULTIPLE_SEND", false);
//! Datalink Layer
//! These messages are solely used for future event scheduling for the datalink layer.
//! They are only used internally to this layer.
static constexpr const Message cDATALINK_READY(10U, "DATALINK_READY", false);
static constexpr const Message cDATALINK_QUEUE_PURGE(11U, "DATALINK_QUEUE_PURGE", false);

} // namespace layer
} // namespace comm
} // namespace wsf

#endif
