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

#ifndef TASK_HPP
#define TASK_HPP

class WsfScenario;


namespace engage
{

class InputConfig;
class RunConfig;
class TaskOutput;

//! A task represents the smallest indivisible unit of work that may be performed independently of another task.
//!
//! ENGAGE maintains the same overall execution architecture as ESAMS. The execution loop is nested as follows:
//!    Runs
//!       Targets (flight path or target grid)
//!          Sites (most often only 1, but may be many)
//!             Shots/Salvos
//!                Monte-Carlo Repetitions
//!
//! The first three are truly independent. That is, the processing for a particular Run/Target/Site is independent
//! of another, and therefore are defined to be what we call a 'task'.
//!
//! The last two in general are not independent, and are run as part of the task.  For instance, shots after the first
//! must have knowledge of the flight path up until the shot. This path may include reactions to previous shots. In
//! addition, not all Monte-Carlo repetions may be executed (for example, if the FINDPK option is specified).
//!
//! It may be theoretically possible to push the definition of a task down if the number of shots taken per site is
//! 'one' and the equivalent of the ESAMS 'REPALL' is specified (run ALL Monte-Carlo repetitions).
class Task
{
public:
   Task()
      : mRunConfigPtr(nullptr)
      , mTaskIndex(-1)
      , mTargetIndex(-1)
      , mSiteIndex(-1)
      , mRepetition(-1)
      , mThreadId(-1)
      , mTaskOutputPtr(nullptr)
   {
   }

   Task(const RunConfig& aRunConfig, int aTaskIndex, int aTargetIndex, int aSiteIndex, int aRepetition)
      : mRunConfigPtr(&aRunConfig)
      , mTaskIndex(aTaskIndex)
      , mTargetIndex(aTargetIndex)
      , mSiteIndex(aSiteIndex)
      , mRepetition(aRepetition)
      , mThreadId(-1)
      , mTaskOutputPtr(nullptr)
   {
   }

   const RunConfig& GetRunConfig() const { return *mRunConfigPtr; }
   int              GetTaskIndex() const { return mTaskIndex; }
   int              GetTargetIndex() const { return mTargetIndex; }
   int              GetSiteIndex() const { return mSiteIndex; }
   int              GetRepetition() const { return mRepetition; }

   void Execute(int aThreadId, WsfScenario& aScenario, const InputConfig& aInputConfig);

   int         GetThreadId() const { return mThreadId; }
   TaskOutput& GetTaskOutput() const { return *mTaskOutputPtr; }

private:
   const RunConfig* mRunConfigPtr;
   int              mTaskIndex;
   int              mTargetIndex;
   int              mSiteIndex;
   int              mRepetition;

   int         mThreadId;
   TaskOutput* mTaskOutputPtr;
};

} // namespace engage

#endif
