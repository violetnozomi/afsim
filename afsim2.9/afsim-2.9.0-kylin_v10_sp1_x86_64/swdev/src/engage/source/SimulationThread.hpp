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

#ifndef SIMULATIONTHREAD_HPP
#define SIMULATIONTHREAD_HPP

#include "UtThread.hpp"


namespace engage
{

class TaskManager;

class SimulationThread : public UtThread
{
public:
   SimulationThread(int aThreadId, TaskManager& aManager);

   // Provides the implementation of the abstract base class method.
   void Run() override;

private:
   // The thread ID is just a 1-based thread number (mostly for writing status messages).
   int          mThreadId;
   TaskManager& mManager;
};

} // namespace engage

#endif
