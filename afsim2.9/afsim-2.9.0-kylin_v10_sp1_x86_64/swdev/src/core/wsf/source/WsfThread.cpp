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

#include "WsfThread.hpp"

#include "UtSleep.hpp"

// ****************************************************************************
// Public

// ============================================================================
// Virtual
//! This method is the main thread function. It calls all other methods that
//! are part of this thread. The while loop will execute until the 'end thread'
//! flag is set externally to this method or the thread function is set to stopped
//! internally by this method. Either one will then cause this function return,
//! ending the thread.
void WsfThread::Run()
{
   // This loop runs until mEndThread is set true or one of
   // the thread methods (e.g. DoWork) sets the function to STOPPED
   while (!mEndThread && mFunction != STOPPED)
   {
      switch (mFunction)
      {
      case AVAILABLE:
      {
         // Thread is available
         mWorkDoneCond.notify_all();
         Available();
      }
      break;
      case ASSIGNED:
      {
         // Do the work
         FunctionType newFunction = DoWork();
         SetFunction(mFunction, newFunction);
      }
      break;
      case PAUSED:
      {
         // Pause the work
         FunctionType newFunction = Pause();
         SetFunction(mFunction, newFunction);
      }
      break;
      default:
         break;
      }
   }
   {
      std::lock_guard<std::mutex> lock(mFunctionWriteMutex);
      mFunction  = AVAILABLE; // set function back to default
      mEndThread = true;
   }
   mWorkDoneCond.notify_all();
}

void WsfThread::EndThread()
{
   {
      std::lock_guard<std::mutex> lock(mFunctionWriteMutex);
      mEndThread = true;
   }
   mWorkAddedCond.notify_one(); // tell the waiting for work thread to exit
}

void WsfThread::WaitUntilWorkDone()
{
   std::unique_lock<std::mutex> lock(mFunctionWriteMutex);
   mWorkDoneCond.wait(lock, [this]() { return mFunction == AVAILABLE || mEndThread; });
}

bool WsfThread::TryWaitUntilWorkDone(double aTimeoutSecs)
{
   auto                         secs = std::chrono::duration<double>(aTimeoutSecs);
   std::unique_lock<std::mutex> lock(mFunctionWriteMutex);
   return mWorkDoneCond.wait_for(lock, secs, [&]() { return mFunction == AVAILABLE || mEndThread; });
}

void WsfThread::Available()
{
   std::unique_lock<std::mutex> lock(mFunctionWriteMutex);
   mWorkAddedCond.wait(lock, [this]() { return mFunction != AVAILABLE || mEndThread; });
}

// ============================================================================
bool WsfThread::PauseWork()
{
   if (mFunction == CRITICAL)
   {
      // Can't interrupt thread when doing critical work
      return false;
   }
   else
   {
      FunctionType currentFunction(mFunction);
      return SetFunction(currentFunction, PAUSED);
   }
}

// ============================================================================
bool WsfThread::ForceStopWork()
{
   FunctionType currentFunction(mFunction);
   return SetFunction(currentFunction, STOPPED);
}

// ****************************************************************************
// Protected

// ============================================================================
bool WsfThread::BeginCriticalWork()
{
   mSavedState = mFunction;
   return SetFunction(mSavedState, CRITICAL);
}

// ============================================================================
bool WsfThread::EndCriticalWork()
{
   return SetFunction(CRITICAL, mSavedState);
}

// ============================================================================
bool WsfThread::SetFunction(FunctionType aOldFunction, FunctionType aNewFunction)
{
   if (mFunction != aOldFunction)
   {
      return false;
   }
   if (aOldFunction != aNewFunction)
   {
      {
         std::lock_guard<std::mutex> lock(mFunctionWriteMutex);
         mFunction = aNewFunction;
      }
      if (aOldFunction == AVAILABLE)
      {
         mWorkAddedCond.notify_one();
      }
   }
   return true;
}
