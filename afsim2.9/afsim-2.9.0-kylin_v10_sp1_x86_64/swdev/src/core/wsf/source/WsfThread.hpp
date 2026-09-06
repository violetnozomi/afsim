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

#ifndef WSFTHREAD_HPP
#define WSFTHREAD_HPP

#include "wsf_export.h"

#include <condition_variable>
#include <mutex>

#include "UtThread.hpp"

class WSF_EXPORT WsfThread : public UtThread
{
public:
   enum FunctionType
   {
      STOPPED   = 0,
      AVAILABLE = 1,
      ASSIGNED  = 2,
      PAUSED    = 3,
      CRITICAL  = 4
   };

   WsfThread()           = default;
   ~WsfThread() override = default;

   void Run() override;

   FunctionType GetFunction() const { return mFunction; }

   //! Thread Run() function status methods
   //@{
   bool CriticalWork() const { return (mFunction == CRITICAL); }
   bool ReadyForWork() const { return (mFunction == AVAILABLE); }
   bool Stopped() const { return (mFunction == STOPPED); }
   //@}
   //! Thread internal Run() control methods.
   //@{
   bool AssignWork() { return SetFunction(AVAILABLE, ASSIGNED); }
   bool StopWork() { return SetFunction(AVAILABLE, STOPPED); }
   bool ForceStopWork(); // attempts to stop thread, even if not available
   bool PauseWork();
   //@}

   //! Sets the flag to break the thread Run() function out of its loop.
   //! This will cause the thread function to return ending / terminating the thread.
   //! This method should be called as part of a destructor or clean up
   void EndThread();

   void WaitUntilWorkDone();
   bool TryWaitUntilWorkDone(double aTimeoutSecs);
   //! Methods called by the thread's Run() method
   //@{

   //! Define what happens when the thread is in the AVAILABLE state.
   //! The default is to do nothing.
   //! @note This method does not return the next state of the thread,
   //! as this easily leads to a race condition.  Instead, use the
   //! WsfThread::AssignWork and WsfThread::StopWork methods to transition
   //! to other states.
   virtual void Available();

   //! The user defines the processing to be performed during the ASSIGNED state
   //! by implementing this method.
   //! @return The FunctionType defining the state the thread will be in
   //! after completion of the work.  If the work is complete, return either
   //! AVAILABLE or PAUSED; otherwise return ASSIGNED.
   virtual FunctionType DoWork() = 0;

   //! The user defines the processing to be performed during the PAUSED state
   //! by implementing this method.
   //! @return The FunctionType defining the state the thread will be in
   //! after completion of the work.  If the pause is complete, return either
   //! AVAILABLE or ASSIGNED; otherwise return PAUSED.
   virtual FunctionType Pause() = 0;
   //@}

protected:
   bool BeginCriticalWork();
   bool EndCriticalWork();

   FunctionType mSavedState = STOPPED;

   bool SetFunction(FunctionType aOldFunction, FunctionType aNewFunction);

   std::condition_variable mWorkDoneCond;
   std::mutex              mFunctionWriteMutex;

private:
   std::condition_variable mWorkAddedCond;

   //! The current operational state of the thread. Used to control the Run function
   //! @note This attribute cannot be used to end the thread externally as the thread
   //! Run() is continuosuly updating the value
   FunctionType mFunction = AVAILABLE;

   //! Flag to externally break out of the thread run loop and end (terminate) the thread
   //! Defaulted to 'false', setting this 'true' will end the thread.
   bool mEndThread = false;
};

#endif // WSFTHREAD_HPP
