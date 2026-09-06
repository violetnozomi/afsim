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

#ifndef UTTHREAD_HPP
#define UTTHREAD_HPP

#define MAX_THREADS 64

#include "ut_export.h"

#include <thread>
#include <vector>

//! An interface to operating system multi-threading services.
//!
//! This class provides the ability to create and control threads.
//!
//! To use this class, the application must:
//!
//! - Create a derived class that implements the Run() method.
//! - Instantiate a object of the derived class to represent the thread.
//! - Invoke the Start() method of the object to start the thread.
//! - Invoke the Join() method of the object to wait for the thread to finish.
//!
//! @see std::mutex for a system-independent interface to operating system mutexes
//! (which can be used to control access to resources that are shared between
//! threads).
//!
//! @note Utilizes C++11 standard thread interface.

class UT_EXPORT UtThread
{
public:
   using UtThreads = std::vector<UtThread*>;

   virtual ~UtThread();

   //! Define the application specific code for the thread.
   virtual void Run() = 0;

   //! Start the thread running.
   //! This invokes the necessary operating system methods to cause the
   //! Run() method to start executing in its own thread.
   //! @returns 'true' if the thread was started or 'false' if not.
   bool Start();

   //! Wait for the thread to complete.
   //! It is an error to call this method without first calling Start().
   //! @returns 'true' if successful or 'false' if not.
   bool Join();

   //! Wait for ALL of the specified threads to complete.
   //! It is an error to call this method without first calling Start().
   //! @returns 'true' if successful or 'false' if not.
   static bool JoinAll(UtThreads& aThreads);

   //! Gets the thread reference.
   //! @returns The thread reference.
   const std::thread& GetThreadData() const { return *mThreadPtr; };

   //! Gets the exception thrown by the thread, if any.
   std::exception_ptr GetException() const { return mException; }

   static unsigned int GetCoreCount();

protected:
   UtThread();
   UtThread(const UtThread& aSrc) = delete;
   UtThread& operator=(const UtThread& aRhs) = delete;

   std::exception_ptr mException;

private:
   //! Pointer to the operating system-specific implementation.
   std::thread* mThreadPtr;
};

//! An adapter to allow a function to be specified as a thread.
//! This eliminates the need for the user to create a class in the trivial case.
//! The function must declared:
//! - void FunctionName()
class UtThreadAdapter : public UtThread
{
public:
   UtThreadAdapter(void (*aFunctionPtr)())
      : mFunctionPtr(aFunctionPtr)
   {
   }

   void Run() override { (*mFunctionPtr)(); }

private:
   void (*mFunctionPtr)();
};

#endif
