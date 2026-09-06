// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <array>
#include <atomic>
#include <thread>

#include <gtest/gtest.h>

#include "UtConcurrentQueue.hpp"
#include "UtMemory.hpp"
#include "UtThread.hpp"

class CountableThread : public UtThread
{
public:
   ~CountableThread() override = default;
   std::atomic<size_t> count{0};
};

class PushThread : public CountableThread
{
public:
   PushThread(UtConcurrentQueue<int>& aQueue)
      : mQueue(aQueue)
   {
   }
   ~PushThread() override = default;
   void Run() override
   {
      for (int i = 0; i < 100; ++i)
      {
         mQueue.Push(i);
         count++;
      }
   }

private:
   UtConcurrentQueue<int>& mQueue;
};

class PopThread : public CountableThread
{
public:
   PopThread(UtConcurrentQueue<int>& aQueue)
      : mQueue(aQueue)
   {
   }
   ~PopThread() override = default;
   void Run() override
   {
      for (int i = 0; i < 100; ++i)
      {
         auto item = mQueue.Pop();
         count++;
      }
   }

private:
   UtConcurrentQueue<int>& mQueue;
};

TEST(ConcurrentQueue, UtThread)
{
   UtConcurrentQueue<int>                          queue;
   std::array<std::unique_ptr<CountableThread>, 6> threads = {ut::make_unique<PushThread>(queue),
                                                              ut::make_unique<PushThread>(queue),
                                                              ut::make_unique<PushThread>(queue),
                                                              ut::make_unique<PopThread>(queue),
                                                              ut::make_unique<PopThread>(queue),
                                                              ut::make_unique<PopThread>(queue)};
   for (auto& t : threads)
   {
      t->Start();
   }
   for (auto& t : threads)
   {
      t->Join();
   }
   // Verify all threads pushed/popped the same
   for (const auto& t : threads)
   {
      ASSERT_EQ(threads[0]->count.load(), t->count.load());
   }
}

TEST(ConcurrentQueue, StdThread)
{
   UtConcurrentQueue<int> queue;
   std::atomic<size_t>    pushCount{0};
   std::atomic<size_t>    popCount{0};
   auto                   push = [&pushCount](UtConcurrentQueue<int>& aQueue)
   {
      for (int i = 0; i < 1000; ++i)
      {
         aQueue.Push(i);
         pushCount++;
      }
   };
   auto pop = [&popCount](UtConcurrentQueue<int>& aQueue)
   {
      for (int i = 0; i < 1000; ++i)
      {
         auto item = aQueue.Pop();
         popCount++;
      }
   };
   std::array<std::thread, 6> threads = {std::thread{push, std::ref(queue)},
                                         std::thread{push, std::ref(queue)},
                                         std::thread{push, std::ref(queue)},
                                         std::thread{pop, std::ref(queue)},
                                         std::thread{pop, std::ref(queue)},
                                         std::thread{pop, std::ref(queue)}};
   for (auto& t : threads)
   {
      t.join();
   }
   // Verify total number of pushes is the same as pops
   EXPECT_EQ(pushCount.load(), popCount.load());
}
