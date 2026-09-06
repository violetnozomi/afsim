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

#include "Task.hpp"

#include <iostream>
#include <mutex>

#include "InputConfig.hpp"
#include "Platform.hpp"
#include "RunConfig.hpp"
#include "Simulation.hpp"
#include "TaskOutput.hpp"
#include "WsfPlatform.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEngagement.hpp"

namespace engage
{

namespace
{
//! Mutex for implementing critical sections within individual threads
static std::mutex sCriticalSection;
} // namespace

// =================================================================================================
void Task::Execute(int aThreadId, WsfScenario& aScenario, const InputConfig& aInputConfig)
{
   mThreadId = aThreadId;

   // Allocate a simulation
   Simulation sim(aScenario, *this);

   // Instantiate the task output object and open the files.
   mTaskOutputPtr = new TaskOutput(aThreadId, *this, sim);
   mTaskOutputPtr->OpenFiles();

   sim.Initialize(); // Initialize the simulation

   // The simulation observers are created during 'Initialize'. At this point we can tell our observer about the
   // objects to which it needs access.

   sim.Start();                                // Start the simulation
   GetRunConfig().CreatePlatforms(sim, *this); // Add the target and other site-related platforms

   while (sim.IsActive()) // Loop through time
   {
      sim.AdvanceTime();
   }

   // If we generate Pk Tables from this task, check the results and save to the table
   if (GetRunConfig().IsPkTableGen() && GetRunConfig().IsSiteGrid())
   {
      Platform* tempWeapon = sim.GetWeapon();
      // It's possible there isn't a weapon instantiation, such as a weapon that never fired.
      // In such a case, the Pk is automatically assumed to be 0 for this instance
      if (tempWeapon)
      {
         WsfWeaponEngagement* tempEngagePtr = WsfWeaponEngagement::Find(*tempWeapon->GetWsfPlatform());
         assert(tempEngagePtr);
         double pkVal = tempEngagePtr->GetWeaponEffects()->GetInterceptPk();
         TaskOutput::InsertPk(pkVal, GetRunConfig(), GetTaskIndex(), GetTargetIndex());
      }
      else
      {
         TaskOutput::InsertPk(0.0, GetRunConfig(), GetTaskIndex(), GetTargetIndex());
      }
   }

   std::cout << "Complete" << std::endl;
   sim.Complete(sim.GetEndTime()); // Indicate the simulation is complete

   sCriticalSection.lock();
   std::cout << "\nThread " << mThreadId << " Task " << GetTaskIndex() << " complete" << std::endl;
   sCriticalSection.unlock();

   mTaskOutputPtr->Write(sim.GetEndTime());      // write out any outstanding variable data
   mTaskOutputPtr->WriteEvent(sim.GetEndTime()); // write out any outstanding events and clear event list
   mTaskOutputPtr->WriteTaskSummary();
   mTaskOutputPtr->CloseFiles();
   delete mTaskOutputPtr;
}

} // namespace engage
