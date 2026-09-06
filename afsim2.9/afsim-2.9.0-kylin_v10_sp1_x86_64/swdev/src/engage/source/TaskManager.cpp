// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "TaskManager.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

#include "InputConfig.hpp"
#include "Simulation.hpp"
#include "SimulationThread.hpp"
#include "Task.hpp"
#include "TaskOutput.hpp"
#include "WsfScenario.hpp"

namespace engage
{

// =================================================================================================
TaskManager::TaskManager(WsfScenario& aScenario, const InputConfig& aInputConfig)
   : mScenario(aScenario)
   , mInputConfig(aInputConfig)
   , mTaskQueueLock()
   , mTaskCount(0)
   , mNextTaskIndex(0)
{
}

// =================================================================================================
TaskManager::~TaskManager() {}

// =================================================================================================
void TaskManager::Execute()
{
   // Reset Task Output summary data
   TaskOutput::ResetSummary(mInputConfig);

   // Initialize Pk table output, if required
   int numPkRuns = 0;
   for (InputConfig::RunList::const_iterator rli = mInputConfig.GetRunList().begin();
        rli != mInputConfig.GetRunList().end();
        ++rli)
   {
      if (rli->IsPkTableGen())
      {
         numPkRuns++;
      }
   }

   if (numPkRuns > 0)
   {
      TaskOutput::InitializePkOutput(&mInputConfig.GetRunList(), numPkRuns);
   }

   // Initialize the queue of tasks that need to be done. Note that this must be done before starting the threads
   mTaskCount     = mInputConfig.GetTaskCount();
   mNextTaskIndex = 0;

   // Determine the number of threads to allocate. If a value wasn't specified then use the number of cores - 2.
   int threadCount = mInputConfig.GetThreadCount();
   if (threadCount == 0)
   {
      threadCount = UtThread::GetCoreCount();
      if (threadCount > 2)
      {
         threadCount -= 2;
      }
   }
   threadCount = std::min(threadCount, mTaskCount); // Don't allocate more threads than tasks

   if (threadCount <= 1)
   {
      // Only one thread or one task. Execute the task(s) sequentially.
      Task task;
      while (GetNextTask(task))
      {
         task.Execute(0, mScenario, mInputConfig);
      }
   }
   else
   {
      // More than one thread and more than one task. Allocate threads and start them running.
      // The threads will pull tasks from the task queue until no more tasks remain.
      //
      // Note that one extra thread is start to run the output amalgamation thread.

      UtThread::UtThreads threads(threadCount);
      for (int i = 0; i < threadCount; ++i)
      {
         threads[i] = new SimulationThread(i + 1, *this);
      }

      // Start all of the threads running
      for (int i = 0; i < threadCount; ++i)
      {
         threads[i]->Start();
      }

      // Wait for all of the execution threads to finish.
      UtThread::JoinAll(threads);

      // Delete the threads
      for (int i = 0; i < threadCount; ++i)
      {
         delete threads[i];
      }
   }

   // If a Pk Table was requested, and the write is non-periodic, do it now. All
   // Pk table data that can be created has been at this point
   for (InputConfig::RunList::const_iterator rli = mInputConfig.GetRunList().begin();
        rli != mInputConfig.GetRunList().end();
        ++rli)
   {
      if (rli->IsPkTableGen() && !rli->IsPkPeriodicFlush())
      {
         TaskOutput::FlushPk(*rli);
      }
   }

   TaskOutput::WriteSummary();
   TaskOutput::CloseAllFiles();
}

// =================================================================================================
//! Get the index of the next task to be run.
//!
//! This is called by a simulation thread to get the next task to be run.
//!
//! @param aTask [output] The information of the task to run.
//! @returns true if a task is to be run or false if no more tasks remain.
bool TaskManager::GetNextTask(Task& aTask)
{
   std::lock_guard<std::recursive_mutex> lock(mTaskQueueLock);

   // If the task index is less than the total task count then it represents a valid task.
   bool haveTask  = false; // Assume there is no work remaining
   int  taskIndex = mNextTaskIndex;
   if (taskIndex < mTaskCount)
   {
      aTask = mInputConfig.MakeTask(taskIndex);
      ++mNextTaskIndex; // Index of the next task to be assigned
      haveTask = true;  // We have a valid task this time.
   }
   return haveTask;
}

} // namespace engage
