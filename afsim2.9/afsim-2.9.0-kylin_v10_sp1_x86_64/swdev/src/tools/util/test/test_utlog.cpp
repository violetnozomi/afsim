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
#include <iostream>

#include <gtest/gtest.h>

#include "UtLog.hpp"
#include "UtLogBatch.hpp"
#include "UtLogPublisher.hpp"
#include "UtLogSubscriberT.hpp"

/*
   Note: The Subscriber and Publisher components of this library are intended to be used in multi-threaded environments.
         These tests only test the functionality in a single thread.
*/

class TestProc final : public ut::log::MessageProcessor
{
public:
   TestProc() { ut::log::Publisher::ResetConsoleSubscriptions(); }

   void Process(const ut::log::Message& aMessage) override
   {
      mStream << aMessage.mData << '\n';

      for (const auto& note : aMessage.mNotes)
      {
         mStream << '\t' << note.mData << '\n';
      }
   }

   std::string String() const noexcept { return mStream.str(); }

private:
   std::stringstream mStream;
};

class DtorProc final : public ut::log::MessageProcessor
{
public:
   DtorProc(std::ostream& aStream)
      : mStream(aStream)
   {
      ut::log::Publisher::ResetConsoleSubscriptions();
   }

   void Process(const ut::log::Message& aMessage) override
   {
      mStream << aMessage.mData << '\n';

      for (const auto& note : aMessage.mNotes)
      {
         mStream << '\t' << note.mData << '\n';
      }
   }

private:
   std::ostream& mStream;
};


using TestSub = ut::log::SubscriberT<TestProc>;

// This test is not to test functionality. It is to test whether all the following forms compile.
TEST(TypeList, Constructors)
{
   ut::log::write({});
   ut::log::write("char *");
   std::string str = "string";
   ut::log::write(str);
   ut::log::write({"char * short list"});
   ut::log::write({"char * long list A", "char * long list B"});
   ut::log::write({str});
   ut::log::write({str, "other"});
}

TEST(Publisher, RegisterSubscriber)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   ut::log::Publisher::RegisterSubscriber(&sub); // Redundant register

   EXPECT_TRUE(ut::log::Publisher::IsSubscriberRegistered(&sub));

   ut::log::info() << "T1";

   ut::log::Publisher::UnregisterSubscriber(&sub);

   EXPECT_FALSE(ut::log::Publisher::IsSubscriberRegistered(&sub));

   ut::log::info() << "T2";

   EXPECT_EQ(sub.ProcessMessages(), 1);

   EXPECT_EQ(sub.GetProcessor().String(), "T1\n");
}

TEST(Publisher, StreamCapture)
{
   TestSub sub("captured");

   ut::log::Publisher::CaptureStdStreams(true, "captured");

   std::cout << "1234"
             << "5678";
   std::cout << 1234 << 5678;
   std::cout << '\n';
   std::cout << "No newline...";

   EXPECT_EQ(sub.ProcessMessages(), 0);

   std::cout.flush();

   EXPECT_EQ(sub.ProcessMessages(), 1);

   EXPECT_EQ(sub.GetProcessor().String(), "1234567812345678\n");

   // Clears out the stream buffers for the next test.
   ut::log::Publisher::CaptureStdStreams(false);
}

TEST(Publisher, StreamCaptureTypes)
{
   TestSub sub("raw");

   ut::log::Publisher::CaptureStdStreams(true);
   ut::log::Publisher::SubscribeConsoleTo(ut::log::Message::PresetDebug());

   std::cerr << "This "
             << "is ";
   std::cerr << "an "
             << "error";
   std::cerr << '\n';

   EXPECT_EQ(sub.ProcessMessages(), 1);

   std::cerr.flush();

   EXPECT_EQ(sub.ProcessMessages(), 0);

   // One newline from the text, one from the subscriber.
   EXPECT_EQ(sub.GetProcessor().String(), "***** ERROR: This is an error\n\n");

   // Clears out the stream buffers for the next test.
   ut::log::Publisher::CaptureStdStreams(false);
}

TEST(Publisher, AutoFlush)
{
   TestSub sub("captured");

   ut::log::Publisher::CaptureStdStreams(true, "captured");

   std::cout << "1234"
             << "5678";
   std::cout << 1234 << 5678;
   std::cout << '\n';
   std::cout << "No newline...";

   EXPECT_EQ(sub.ProcessMessages(), 0);

   ut::log::write("captured") << "Abcdefg";

   EXPECT_EQ(sub.ProcessMessages(), 3);

   EXPECT_EQ(sub.GetProcessor().String(), "1234567812345678\nNo newline...\nAbcdefg\n");

   ut::log::Publisher::CaptureStdStreams(false);
}

TEST(Subscriber, SubscribeTo)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   sub.SubscribeTo(ut::log::Message::Info()); // Redundant subscription

   ut::log::error() << "e" << 'r' << "r";

   ut::log::warning() << "w" << 'a' << "r" << 'n';

   ut::log::info() << "i" << 'n' << "f" << 'o';

   sub.UnsubscribeFrom(ut::log::Message::Info());

   ut::log::info() << "unsub";

   EXPECT_EQ(sub.ProcessMessages(), 3);

   EXPECT_EQ(sub.GetProcessor().String(), "err\nwarn\ninfo\n");
}

TEST(Subscriber, AddMessage)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   sub.UnsubscribeFrom(ut::log::Message::Info());

   ut::log::Message msg;
   msg.mTypes.Insert(ut::log::Message::Info());
   msg.mData = "test";

   sub.AddMessage(msg);

   EXPECT_EQ(sub.ProcessMessages(), 0);
   EXPECT_EQ(sub.GetProcessor().String(), "");
}

TEST(Subscriber, Destructor)
{
   std::stringstream ss;
   {
      ut::log::SubscriberT<DtorProc> sub(ut::log::Message::Developer(), ss);

      ut::log::developer() << "Test";

      EXPECT_EQ(ss.str(), "");
   }

   EXPECT_EQ(ss.str(), "Test\n");
}

TEST(MessageStream, Destructor)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   {
      auto out = ut::log::info();
      out.AddNote();
   }

   EXPECT_EQ(sub.ProcessMessages(), 0);
   EXPECT_EQ(sub.GetProcessor().String(), "");
}

TEST(MessageStream, Move)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   auto stream = ut::log::debug() << "L1";
   stream << "L2";

   stream.Send();

   stream.Send();

   EXPECT_EQ(sub.ProcessMessages(), 1);
   EXPECT_EQ(sub.GetProcessor().String(), "L1L2\n");
}

TEST(MessageStream, AddNote)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   auto stream = ut::log::debug() << "Test message.";

   stream.AddNote() << "Note.";
   stream.AddNote("type") << "Note 2.";
   stream.AddNote(); // Should not send anything
   stream.Send();

   EXPECT_EQ(sub.ProcessMessages(), 1);
   EXPECT_EQ(sub.GetProcessor().String(), "Test message.\n\tNote.\n\tNote 2.\n");
}

TEST(MessageStream, StreamModifiers)
{
   TestSub sub(ut::log::Message::PresetDeveloper());

   auto stream = ut::log::fatal() << std::boolalpha;

   stream << true;

   stream.AddNote(ut::log::Message::Fatal()) << false;

   stream.Send();

   EXPECT_EQ(sub.ProcessMessages(), 1);
   EXPECT_EQ(sub.GetProcessor().String(), "true\n\tfalse\n");
}

TEST(Batch, Write)
{
   TestSub sub("test");

   {
      ut::log::Batch log("test");
      ut::log::Batch log2("test");

      log.write() << "A";
      log2.write() << "1";
      log.write() << "B";
      log2.write() << "2";
      log.write() << "C";
      log2.write() << "3";
   }
   EXPECT_EQ(sub.ProcessMessages(), 6);
   EXPECT_EQ(sub.GetProcessor().String(), "1\n2\n3\nA\nB\nC\n");
}
