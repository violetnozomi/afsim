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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTSEMAPHORE_HPP
#define UTSEMAPHORE_HPP

#include "ut_export.h"

class UtSemaphoreImpl;

//! A system-independent interface to thread-specific operating system semaphores.
//!
//! Semaphores are often used to control access to resources that are shared
//! between threads in a multi-threaded application.
//!
//! @see UtThread for a system-independent interface to operating system threads.
//!
//! @note On Microsoft Windows this class uses the standard Windows functions.
//! On Unix systems this class uses 'semaphore.h'.
class UT_EXPORT UtSemaphore
{
public:
   //! Creates a semaphore
   //! @param aInitialCount  The initial count of the semaphore
   UtSemaphore(int aInitialCount);

   //! Do not allow copy construction or assignment
   UtSemaphore(const UtSemaphore& aSrc) = delete;
   UtSemaphore& operator=(const UtSemaphore&) = delete;

   //! Destroys the semaphore
   ~UtSemaphore();

   //! Increase the 'value' of the semaphore
   //! @param aCount  The value to increase the semaphore by
   //! @note This method will never block
   void Release(unsigned int aCount = 1);

   //! Decrement the value of the semaphore.
   //! This method will block until the value of the semaphore
   //! is greater than 0.
   void Acquire();

   //! Attempt to decrement the value of the semaphore
   //! @return 'true' if the semaphore's value was decremented.
   //!         'false' if the semaphore's value was 0, and could
   //!            not be decremented.
   bool TryAcquire();

private:
   //! pointer to the operating system specific implementation of
   //! the semaphore.
   UtSemaphoreImpl* mImplPtr;
};

#endif
