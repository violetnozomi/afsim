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

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>

#include "Constants.hpp"
#include "Task.hpp"
#include "WsfFrameStepSimulation.hpp"

namespace engage
{

class Platform;

//! The simulation object to execute an portion of a task.
//!
//! At this point this exists simply as a place to store data that needs to be exchanged between
//! The Task::Execute (and things that it calls) and the SimulationObserver.
class Simulation : public WsfFrameStepSimulation
{
public:
   Simulation(WsfScenario& aScenario, Task& aTask);
   ~Simulation() override;

   Task& GetTask() const { return mTask; }
   Phase GetPhase() const { return mPhase; }

   void SetPhase(Phase aPhase, double aSimTime);

   double GetPhaseStartTime() const { return mPhaseStartTime[mPhase]; }
   double GetPhaseStartTime(Phase aPhase) const { return mPhaseStartTime[aPhase]; }

   //@name Standard 'known' platforms.
   //@{
   void SetTarget(Platform* aTargetPtr) { mTargetPtr = aTargetPtr; }
   void SetTracker(Platform* aTrackerPtr) { mTrackerPtr = aTrackerPtr; }
   void SetLauncher(Platform* aLauncherPtr) { mLauncherPtr = aLauncherPtr; }
   void SetWeapon(Platform* aWeaponPtr) { mWeaponPtr = aWeaponPtr; }

   Platform* GetTarget() const { return mTargetPtr; }
   Platform* GetTracker() const { return mTrackerPtr; }
   Platform* GetLauncher() const { return mLauncherPtr; }
   Platform* GetWeapon() const { return mWeaponPtr; }
   //@}

private:
   Task& mTask;

   //! The current phase of the simulation.
   Phase               mPhase;
   std::vector<double> mPhaseStartTime;

   Platform* mTargetPtr;
   Platform* mTrackerPtr;
   Platform* mLauncherPtr;
   Platform* mWeaponPtr;
};

} // namespace engage

#endif
