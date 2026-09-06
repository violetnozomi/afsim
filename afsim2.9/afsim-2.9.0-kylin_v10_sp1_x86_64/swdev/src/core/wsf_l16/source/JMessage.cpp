// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "JMessage.hpp"

#include "UtStringIdLiteral.hpp"
#include "WsfStringId.hpp"

namespace WsfL16
{

//! Initialize the message.
//! JMessage will handle deletion of aMessage.
JMessage::JMessage(MessagePtr aMessage, WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mMessagePtr(aMessage)
{
   // 70 bits for each word in the message.
   // TODO: include extra header info which would
   //       be on a real message.
   SetSizeBits(static_cast<int>((aMessage->GetMessageLength() + 1) * 70));
}

JMessage::~JMessage() {}

// static
WsfStringId JMessage::GetTypeId()
{
   return UtStringIdLiteral("L16_JMESSAGE");
}

WsfMessage* JMessage::Clone() const
{
   return new JMessage(*this);
}

JMessage& JMessage::operator=(JMessage& aMsg)
{
   WsfMessage& msgRef = *this;
   msgRef             = aMsg;
   mMessagePtr        = aMsg.mMessagePtr;
   return *this;
}

JMessage::JMessage(const JMessage& aSrc)
   : WsfMessage(aSrc)
   , mMessagePtr(aSrc.mMessagePtr)
{
}

} // namespace WsfL16
