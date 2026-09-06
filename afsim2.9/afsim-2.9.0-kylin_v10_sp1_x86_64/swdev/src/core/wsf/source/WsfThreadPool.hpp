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

#ifndef WSFTHREADPOOL_HPP
#define WSFTHREADPOOL_HPP

#include "UtSleep.hpp"
#include "WsfThread.hpp"

// template <typename T>
// class Singleton
//{
//    static T& Instance();
// };
//
// template <typename T>
// T& Singleton<T>::Instance()
//{
//    static T mt;
//    return mt;
// }

template<typename T>
struct WsfThreadPool_DefaultWorkerFactor
{
   T* operator()() const { return new T; }
};

// ============================================================================
template<class Worker, class WorkerFactory = WsfThreadPool_DefaultWorkerFactor<Worker>>
class WsfThreadPool
{
public:
   typedef std::vector<Worker*> WorkerThreads;

   WsfThreadPool(const WorkerFactory& aFactory = WorkerFactory());
   ~WsfThreadPool();

   void Start(unsigned int aNumThreads);
   void Stop();

   //! @param aNumTasks the number of threads to wake up. use zero to wake up all threads.
   bool    AssignWork(size_t aNumTasks = 0u);
   Worker* FindAvailableThread(double aSleepTime = 0.001);
   bool    AllWorkDone();

   void WaitUntilAllWorkDone();
   bool TryWaitUntilAllWorkDone(double aSecsToWait);

private:
   WorkerFactory mFactory;
   WorkerThreads mThread_vector;
};

template<class Worker, class WorkerFactory>
WsfThreadPool<Worker, WorkerFactory>::WsfThreadPool(const WorkerFactory& aFactory)
   : mFactory(aFactory)
{
}

template<class Worker, class WorkerFactory>
WsfThreadPool<Worker, WorkerFactory>::~WsfThreadPool()
{
   // Shut down the thread pool
   Stop();

   // delete threads and clear the thread pool
   for (unsigned int index = 0; index < mThread_vector.size(); ++index)
   {
      delete mThread_vector[index];
   }
   mThread_vector.clear();
}

template<class Worker, class WorkerFactory>
void WsfThreadPool<Worker, WorkerFactory>::Start(unsigned int aNumThreads)
{
   // Check to see if the threads have already been created
   if (mThread_vector.empty())
   {
      // Create the threads and start them running
      mThread_vector.reserve(aNumThreads);
      for (unsigned int count = 1; count <= aNumThreads; ++count)
      {
         Worker* workerPtr = mFactory();
         workerPtr->Start();
         mThread_vector.push_back(workerPtr);
      }
   }
}

template<class Worker, class WorkerFactory>
void WsfThreadPool<Worker, WorkerFactory>::Stop()
{
   // Stop the work
   UtThread::UtThreads tempThreads;
   for (unsigned int index = 0; index < mThread_vector.size(); ++index)
   {
      mThread_vector[index]->StopWork();
      mThread_vector[index]->EndThread();
      tempThreads.push_back(mThread_vector[index]);
   }

   // Wait for threads to die
   if (!tempThreads.empty())
   {
      UtThread::JoinAll(tempThreads);
   }
}

template<class Worker, class WorkerFactory>
bool WsfThreadPool<Worker, WorkerFactory>::AssignWork(size_t aNumTasks /*= 0*/)
{
   // Loop through the vector of thread and find the first available
   bool startedWork(false);
   for (auto* threadPtr : mThread_vector)
   {
      if (threadPtr->AssignWork())
      {
         startedWork = true;
         --aNumTasks;
         if (aNumTasks == 0)
         {
            break;
         }
      }
   }
   return startedWork;
}

template<class Worker, class WorkerFactory>
Worker* WsfThreadPool<Worker, WorkerFactory>::FindAvailableThread(double aSleepTime)
{
   // Loop through the vector of thread and find the first available
   Worker*      availableThreadPtr = 0;
   unsigned int threadPoolSize(mThread_vector.size());
   for (unsigned int index = 0; index < threadPoolSize; ++index)
   {
      if (mThread_vector[index]->GetFunction() == WsfThread::AVAILABLE)
      {
         availableThreadPtr = mThread_vector[index];
         break;
      }
   }

   // All threads are busy; sleep a little bit
   if (availableThreadPtr == 0)
   {
      UtSleep::Sleep(aSleepTime);
   }

   return availableThreadPtr;
}

template<class Worker, class WorkerFactory>
bool WsfThreadPool<Worker, WorkerFactory>::AllWorkDone()
{
   bool   atLeastOneStillWorking(false);
   size_t threadPoolSize(mThread_vector.size());
   for (size_t index = 0; index < threadPoolSize; ++index)
   {
      if (mThread_vector[index]->GetFunction() != WsfThread::AVAILABLE)
      {
         atLeastOneStillWorking = true;
         break;
      }
   }
   return (!atLeastOneStillWorking);
}

template<class Worker, class WorkerFactory>
void WsfThreadPool<Worker, WorkerFactory>::WaitUntilAllWorkDone()
{
   for (auto threadPtr : mThread_vector)
   {
      threadPtr->WaitUntilWorkDone();
   }
}

template<class Worker, class WorkerFactory>
bool WsfThreadPool<Worker, WorkerFactory>::TryWaitUntilAllWorkDone(double aSecsToWait)
{
   auto start = std::chrono::system_clock::now();
   for (auto threadPtr : mThread_vector)
   {
      if (!threadPtr->TryWaitUntilWorkDone(aSecsToWait))
      {
         return false;
      }
      auto now     = std::chrono::system_clock::now();
      using secs_t = std::chrono::duration<double>;
      aSecsToWait -= secs_t{now - start}.count();
      start = now;
   }
   return true;
}

#endif // WSFTHREADPOOL_HPP
