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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "SimulationThread.hpp"

#include "Simulation.hpp"
#include "TaskManager.hpp"

namespace engage
{

// =================================================================================================
SimulationThread::SimulationThread(int aThreadId, TaskManager& aManager)
   : mThreadId(aThreadId)
   , mManager(aManager)
{
}

// =================================================================================================
//! Implementation of the base class method Run().
//!
//! This enters a loop which pulls the next unit of work from the TaskManager, executes the work
//! and then cycles back to get the next unit of work. If there is no more work then the method
//! simply returns.
// virtual
void SimulationThread::Run()
{
   Task task;
   while (mManager.GetNextTask(task))
   {
      // Execute the task just retrieved from the task manager.
      task.Execute(mThreadId, mManager.GetScenario(), mManager.GetInputConfig());
   }
}

} // namespace engage
