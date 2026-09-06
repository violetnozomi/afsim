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

#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <mutex>

class WsfScenario;


namespace engage
{

class InputConfig;
class Task;

class TaskManager
{
public:
   TaskManager(WsfScenario& aScenario, const InputConfig& aInputConfig);
   ~TaskManager();

   void Execute();

   bool GetNextTask(Task& aTask);

   const InputConfig& GetInputConfig() const { return mInputConfig; }

   WsfScenario& GetScenario() const { return mScenario; }

private:
   WsfScenario&       mScenario;
   const InputConfig& mInputConfig;

   std::recursive_mutex mTaskQueueLock;
   int                  mTaskCount;
   int                  mNextTaskIndex;
};

} // namespace engage

#endif
