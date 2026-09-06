// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef STREAMSTHREAD_HPP
#define STREAMSTHREAD_HPP

#include <functional>
#include <thread>

#include "UtConcurrentQueue.hpp"
#include "UtException.hpp"


// NOT USED OR TESTED, PLACEHOLDER CLASS FOR FUTURE USE.
template<typename T>
class UtConcurrentJob
{
public:
   UtConcurrentJob(T aResource = T{})
      : mResource(aResource)
      , mQueue()
      , mThread()
      , mThreadRun(true)
   {
      // Start worker thread. Loops, popping function object off queue and executing until done.
      mThread = std::thread(
         [this]
         {
            ut::SetupThreadErrorHandling();
            while (mThreadRun)
            {
               mQueue.Pop()();
            }
         });
   }

   ~UtConcurrentJob()
   {
      mThreadRun = false;
      mThread.join();
   }

   template<typename F>
   void operator()(F f)
   {
      mQueue.Push([=] { f(mResource); });
   }

private:
   mutable UtConcurrentQueue<std::function<void()>> mQueue;
   mutable T                                        mResource;
   std::thread                                      mThread;
   bool                                             mThreadRun;
};

#endif
