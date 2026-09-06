// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEventPipeFileWriteWorker.hpp"

wsf::eventpipe::FileWriterWorker::FileWriterWorker()
   : UtThread()
   , mClock()
{
   mClock.ResetClock();
}

void wsf::eventpipe::FileWriterWorker::Push(std::unique_ptr<UtPackMessage> msg)
{
   // add the message
   std::lock_guard<std::mutex> lock(mFrontAccessMutex);
   mFrontMessageQueue.push(std::move(msg));

   // if the clock has advanced 1/20 second or our front buffer has 200 messages, pass it to the writing thread
   if ((mClock.GetClock() > 0.05) || (mFrontMessageQueue.size() > 200))
   {
      size_t queueSize = 0;
      mClock.ResetClock();
      // context scope the lock
      {
         std::lock_guard<std::mutex> lock(mGeneralMutex);
         // swap the buffers instead of copying if possible
         if (mBackMessageQueue.empty())
         {
            mBackMessageQueue.swap(mFrontMessageQueue);
         }
         // copy the front buffer to the back buffer
         while (!mFrontMessageQueue.empty())
         {
            mBackMessageQueue.push(std::move(mFrontMessageQueue.front()));
            mFrontMessageQueue.pop();
         }
         queueSize = mBackMessageQueue.size();
      }
      mBackMessagesAdded.notify_one();

      // limit queue size, let the write thread catch up
      // Note: this is blocking sim-thread execution, this should be avoided
      // In practice the write thread shouldn't have a problem keeping up.
      if (queueSize > 5000)
      {
         // ut::log::error() << "QUEUE LIMIT EXCEEDED!";
         std::unique_lock<std::mutex> lock(mGeneralMutex);
         mBackMessagesPopped.wait(lock, [this]() { return mBackMessageQueue.size() <= 5000; });
      }
   }
}

void wsf::eventpipe::FileWriterWorker::Run()
{
   const double cFLUSH_INTERVAL = .5;
   UtWallClock  clock;
   double       nextFlush       = clock.GetRawClock() + cFLUSH_INTERVAL;
   unsigned int messagesWritten = 0;
   bool         hasBackMessages = !mBackMessageQueue.empty();
   while (mRunning || hasBackMessages)
   {
      // if there is nothing else for the thread to do
      if (!hasBackMessages)
      {
         // if nothing much is in the pipe, there is no need to flush
         if (messagesWritten > 100)
         {
            // check for low output condition, and force flush every while
            double t = clock.GetRawClock();
            if (t > nextFlush)
            {
               mStreamPtr->Flush();
               messagesWritten = 0;
               nextFlush       = t + cFLUSH_INTERVAL;
            }
         }

         std::unique_lock<std::mutex> lock(mGeneralMutex);
         mBackMessagesAdded.wait(lock, [this]() { return !mRunning || !mBackMessageQueue.empty(); });
         hasBackMessages = !mBackMessageQueue.empty();
      }
      else // if there are messages to write
      {
         std::unique_ptr<UtPackMessage> msgPtr;
         // context scope the lock
         size_t msgsLeft;
         {
            std::lock_guard<std::mutex> lock(mGeneralMutex);
            // pull a message
            msgPtr = std::move(mBackMessageQueue.front());
            mBackMessageQueue.pop();
            msgsLeft = mBackMessageQueue.size();
         }
         hasBackMessages = (msgsLeft != 0);
         if (msgsLeft == 5000)
         {
            mBackMessagesPopped.notify_all();
         }
         // write a message
         mStreamPtr->Write(*msgPtr);
         ++messagesWritten;
      }
   }
   // clear out the front buffer when everything else is done
   std::lock_guard<std::mutex> lock(mFrontAccessMutex);
   while (!mFrontMessageQueue.empty())
   {
      auto msgPtr = std::move(mFrontMessageQueue.front());
      mFrontMessageQueue.pop();
      mStreamPtr->Write(*msgPtr);
   }
}

void wsf::eventpipe::FileWriterWorker::StartRunning()
{
   mRunning = true;
}

void wsf::eventpipe::FileWriterWorker::StopRunning()
{
   // context scope the lock
   {
      std::lock_guard<std::mutex> lock(mGeneralMutex);
      mRunning = false;
   }
   // awaken the running thread to let it know that it has stopped.
   mBackMessagesAdded.notify_one();
}
