// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <memory>

#include <gtest/gtest.h>

#include "UtMemory.hpp"
#include "effects/WsfCyberMitmMessage.hpp"

TEST(MitmMessageTest, Construction)
{
   wsf::cyber::MitmMessage msg(nullptr, 0, false);
   ASSERT_TRUE(msg.GetMessage() == nullptr);
   ASSERT_EQ(0, msg.GetDelay());
   ASSERT_FALSE(msg.GetDrop());
   ASSERT_EQ(WsfStringId("WsfCyberMitmMessage"), msg.GetScriptClassName());
}

TEST(MitmMessageTest, ChangeDelay)
{
   wsf::cyber::MitmMessage msg(nullptr, 10, false);
   ASSERT_EQ(10, msg.GetDelay());
   msg.SetDelay(20);
   ASSERT_EQ(20, msg.GetDelay());
}

TEST(MitmMessageTest, ChangeDrop)
{
   wsf::cyber::MitmMessage msg(nullptr, 0, false);
   ASSERT_FALSE(msg.GetDrop());
   msg.SetDrop(true);
   ASSERT_TRUE(msg.GetDrop());
}

TEST(MitmMessageTest, ChangeMessage)
{
   WsfMessage              testMessage("test_message");
   wsf::cyber::MitmMessage msg(nullptr, 0, false);
   ASSERT_TRUE(msg.GetMessage() == nullptr);
   msg.SetMessage(ut::make_unique<wsf::comm::Message>(ut::clone(testMessage)));
   ASSERT_EQ("test_message", msg.GetMessage()->SourceMessage()->GetType());
}

TEST(MitmMessageTest, Clone)
{
   WsfMessage              testMessage("test_message");
   wsf::cyber::MitmMessage msg(ut::make_unique<wsf::comm::Message>(ut::clone(testMessage)), 42, false);
   auto                    clonedMessage = ut::clone(msg);
   ASSERT_FALSE(clonedMessage->GetDrop());
   ASSERT_EQ("test_message", clonedMessage->GetMessage()->SourceMessage()->GetType());
   ASSERT_EQ(42, clonedMessage->GetDelay());
}
