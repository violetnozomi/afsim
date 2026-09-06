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

#include "Simulation.hpp"

#include <iostream>

#include "Platform.hpp"

namespace engage
{

// =================================================================================================
Simulation::Simulation(WsfScenario& aScenario, Task& aTask)
   : WsfFrameStepSimulation(aScenario, 1)
   , mTask(aTask)
   , mPhase(cACQUIRING)
   , mPhaseStartTime(cPHASE_COUNT, 0.0)
   , mTargetPtr(nullptr)
   , mTrackerPtr(nullptr)
   , mLauncherPtr(nullptr)
   , mWeaponPtr(nullptr)
{
}

// =================================================================================================
// virtual
Simulation::~Simulation()
{
   delete mTargetPtr;
   delete mTrackerPtr;
   delete mLauncherPtr;
   delete mWeaponPtr;
}

// =================================================================================================
void Simulation::SetPhase(Phase aPhase, double aSimTime)
{
   mPhase                  = aPhase;
   mPhaseStartTime[aPhase] = aSimTime;
   std::cout << "T=" << aSimTime << " Phase = " << mPhase << std::endl;
}

} // namespace engage
