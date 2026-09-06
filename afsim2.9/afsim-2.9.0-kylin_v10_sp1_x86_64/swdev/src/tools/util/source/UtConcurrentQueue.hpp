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

#ifndef UTCONCURRENTQUEUE_HPP
#define UTCONCURRENTQUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template<typename T, class Container = std::deque<T>>
class UtConcurrentQueue
{
public:
   UtConcurrentQueue()                         = default;
   UtConcurrentQueue(const UtConcurrentQueue&) = delete;
   UtConcurrentQueue& operator=(const UtConcurrentQueue&) = delete;

   T Pop()
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      mCondition.wait(mutex_lock, [this] { return !mQueue.empty(); });
      auto item = std::move(mQueue.front());
      mQueue.pop();
      return item;
   }

   void Pop(T& item)
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      mCondition.wait(mutex_lock, [this] { return !mQueue.empty(); });
      item = mQueue.front();
      mQueue.pop();
   }

   void Push(const T& item)
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      mQueue.push(item);
      mCondition.notify_one();
   }

   void Push(T&& item)
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      mQueue.push(std::move(item));
      mCondition.notify_one();
   }

   template<class... Args>
   void Emplace(Args&&... args)
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      mQueue.emplace(std::forward<Args>(args)...);
      mCondition.notify_one();
   }

   bool Get(T& item)
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      if (mQueue.empty())
      {
         return false;
      }
      item = mQueue.front();
      mQueue.pop();
      return true;
   }

   void Clear()
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      mQueue = std::queue<T, Container>();
   }

   bool Empty() const
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      return mQueue.empty();
   }

   size_t Size() const
   {
      std::unique_lock<std::mutex> mutex_lock(mMutex);
      return mQueue.size();
   }

private:
   std::queue<T, Container> mQueue;
   mutable std::mutex       mMutex;
   std::condition_variable  mCondition;
};

#endif
