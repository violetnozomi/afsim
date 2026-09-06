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
#ifndef L16_JMESSAGE_HPP
#define L16_JMESSAGE_HPP

#include <memory>

#include "Messages.hpp"
#include "WsfMessage.hpp"

namespace WsfL16
{
//! Passes J-Messages across WSF.
//! Uses a SharedPtr to avoid copying the message
class JMessage : public WsfMessage
{
public:
   typedef std::shared_ptr<const Messages::InitialBase> MessagePtr;

   JMessage(MessagePtr aMessage, WsfPlatform* aPlatformPtr);
   JMessage(const JMessage& aSrc);
   ~JMessage() override;

   WsfMessage* Clone() const override;
   JMessage&   operator=(JMessage& aMsg);

   //! Returns the message
   MessagePtr Get() { return mMessagePtr; }

   const MessagePtr Get() const { return mMessagePtr; }

   static WsfStringId GetTypeId();

private:
   void       Destroy();
   MessagePtr mMessagePtr;
};
} // namespace WsfL16

#endif
