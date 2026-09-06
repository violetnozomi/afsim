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

#include "RunConfig.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>

#include "InputConfig.hpp"
#include "Simulation.hpp"
#include "Task.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"

namespace engage
{

// =================================================================================================
RunConfig::RunConfig(const InputConfig& aInputConfig)
   : mInputConfig(aInputConfig)
   , mRepetitionCount(1)
   , mRunAllRepetitions(true)
   , mRunUntilNonZeroPk(false)
   , mTrackDetectDelayTime(2.0)
   , mFirstEngageAttemptTime(0.0)
   , mLastEngageAttemptTime(0.0)
   , mEngagementInterval(10.0)
   , mPerformFlyouts(true)
   , mSiteShotLimit(1)
   , mShootLookShoot(true)
   , mSalvoDelay(5.0)
   , mSalvoDelay2(3.0)
   , mRecordFileBaseName()
   , mEventOutputFileBaseName()
   , mAzCueingError(0.0)
   , mElCueingError(0.0)
   , mPkTermType(0)
   , mOutputList()
   , mSiteConfig()
   , mTargetConfig()
   , mUtilities()
   , mRunIndex(0)
   , mBaseTaskIndex(0)
   , mTaskCount(0)
   , mPkTargetType("DEFAULT")
   , mPkSiteType("DEFAULT")
   , mEnablePkTableGen(false)
   , mSiteGridRun(false)
   , mPkOutputPath(".")
   , mPkPeriodicFlush(false)
   , mPkLengthUnits("m")
   , mPkVelocityUnits("m/s")
{
#if 0
   // Initialize the output slots with the ESAMS defaults

   // Slot 0
   {
      Output& slot = mOutputList[0];
      slot.SetFileName("fort.6");                // FORTRAN unit 6
      slot.SetFormat(Output::cCOLUMN);
      slot.SetPhase(cACQUISITION);
      slot.SetSummaryOutput(true);
      slot.SetEventOutput(true);
      slot.SetRateTableName("default");
      slot.mFltVars[0] = 1;
      slot.mFltVars[1] = 2;
      slot.mFltVars[2] = 3;
      slot.mFltVars[3] = 4;
   }

   // Slot 1
   {
      Output& slot = mOutputList[1];
      slot.SetFileName("fort.6");                // FORTRAN unit 6
      slot.SetFormat(Output::cCOLUMN);
      slot.SetPhase(cTRACKING);
      //slot.SetSummaryOutput(true);
      //slot.SetEventOutput(true);
      slot.SetRateTableName("default");
   }

   // Slot 2
   {
      Output& slot = mOutputList[2];
      slot.SetFileName("fort.6");                // FORTRAN unit 6
      slot.SetFormat(Output::cCOLUMN);
      slot.SetPhase(cACQUISITION);
      //slot.SetSummaryOutput(true);
      //slot.SetEventOutput(true);
      slot.SetRateTableName("default");
   }
#endif
   // The other slots are left to their empty values.
}

// =================================================================================================
//! This is called when all of the input has been read.
bool RunConfig::Complete(int aRunIndex, int aBaseTaskIndex)
{
   mRunIndex      = aRunIndex;
   mBaseTaskIndex = aBaseTaskIndex;

   bool ok(true);
   for (OutputList::iterator oli = mOutputList.begin(); oli != mOutputList.end(); ++oli)
   {
      OutputConfig& output(*oli);
      // Resolve the output rate table.
      std::string       rateTableName(output.GetRateTableName());
      const OutputRate* outputRatePtr = mInputConfig.GetOutputRate(rateTableName);
      if (outputRatePtr != nullptr)
      {
         output.SetRateTable(*outputRatePtr);
      }
      else
      {
         std::cout << "***** ERROR: unable to find output rate table named " << rateTableName << std::endl;
         ok = false;
      }
   }
   ok &= mSiteConfig.Complete(mSiteGridRun);
   ok &= mTargetConfig.Complete(!mSiteGridRun);

   mTaskCount = mTargetConfig.GetPointCount() * mSiteConfig.GetPointCount() * mRepetitionCount;
   return ok;
}

// =================================================================================================
bool RunConfig::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "center_location")
   {
      double centerLat(0.0);
      double centerLon(0.0);
      aInput.ReadValueOfType(centerLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(centerLon, UtInput::cLONGITUDE);
      mUtilities.SetCenter(centerLat, centerLon);
   }
   else if (command == "event_output_file_base_name")
   {
      aInput.ReadValueQuoted(mEventOutputFileBaseName);
      mEventOutputFileBaseName = aInput.SubstitutePathVariables(mEventOutputFileBaseName);
   }
   else if (command == "output")
   {
      UtInputBlock inputBlock(aInput);
      OutputConfig output;
      while (inputBlock.ReadCommand(command))
      {
         if (output.ProcessInput(aInput))
         {
         }
         // TODO-Add ability to use an output template
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mOutputList.push_back(output);
   }
   else if (command == "perform_flyouts")
   {
      aInput.ReadValue(mPerformFlyouts);
   }
   else if (command == "record_file_base_name")
   {
      aInput.ReadValueQuoted(mRecordFileBaseName);
      mRecordFileBaseName = aInput.SubstitutePathVariables(mRecordFileBaseName);
   }
   else if (command == "repetition_count")
   {
      aInput.ReadValue(mRepetitionCount);
      aInput.ValueGreater(mRepetitionCount, 1);
   }
   else if (command == "site_grid")
   {
      UtInputBlock inputBlock(aInput);
      mSiteGridRun = true;
      while (inputBlock.ReadCommand(command))
      {
         if (mSiteConfig.ProcessSiteGridInput(aInput))
         {
         }
         else if (mTargetConfig.ProcessSiteGridInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "enable_Pk_table_generation")
   {
      aInput.ReadValue(mEnablePkTableGen);
   }
   else if (command == "Pk_table_target_type")
   {
      aInput.ReadValue(mPkTargetType);
   }
   else if (command == "Pk_table_site_type")
   {
      aInput.ReadValue(mPkSiteType);
   }
   else if (command == "Pk_table_output_directory")
   {
      aInput.ReadValue(mPkOutputPath);
   }
   else if (command == "enable_Pk_table_periodic_flush")
   {
      aInput.ReadValue(mPkPeriodicFlush);
   }
   else if (command == "Pk_output_length_units")
   {
      aInput.ReadValue(mPkLengthUnits);
   }
   else if (command == "Pk_output_speed_units")
   {
      aInput.ReadValue(mPkVelocityUnits);
   }
   else if (mSiteConfig.ProcessInput(aInput))
   {
   }
   else if (mTargetConfig.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
std::string RunConfig::GetRecordFileName(const Task& aTask) const
{
   if (mRecordFileBaseName.empty())
   {
      return std::string();
   }

   std::ostringstream oss;
   oss << mRecordFileBaseName << "_r" << mRunIndex + 1 << "_t" << aTask.GetTargetIndex() + 1 << "_s"
       << aTask.GetSiteIndex() + 1 << "_i" << aTask.GetRepetition() + 1 << ".rep";
   return (oss.str());
}

// =================================================================================================
std::string RunConfig::GetEventOutputFileName(const Task& aTask) const
{
   if (mEventOutputFileBaseName.empty())
   {
      return std::string();
   }

   std::ostringstream oss;
   oss << mEventOutputFileBaseName << "_r" << mRunIndex + 1 << "_t" << aTask.GetTargetIndex() + 1 << "_s"
       << aTask.GetSiteIndex() + 1 << "_i" << aTask.GetRepetition() + 1 << ".evt";
   return (oss.str());
}

// =================================================================================================
//! Create the platforms for the associated task index.
//! Create the launcher, tracker and target platforms and inject them into the simulation.
//! @param aSimulation [input] The simulation into which the platforms are to be injected.
//! @param aTask       [input] The task.
//! @returns true if successful or false if not.
bool RunConfig::CreatePlatforms(Simulation& aSimulation, const Task& aTask) const
{
   bool ok(true);
   // The target must be created before the site because the site may reference the target name in a prebrief track
   ok &= mTargetConfig.CreatePlatform(aSimulation, aTask);
   ok &= mSiteConfig.CreatePlatforms(aSimulation, aTask);
   return ok;
}

// =================================================================================================
//! Create a task object that represents the task with the specified global task index.
//! @param aTaskIndex [input] The global task index of the task.
//! @returns The Task object that represents the supplied global task index.
Task RunConfig::MakeTask(int aTaskIndex) const
{
   // Within a run, the tasks are executed as a series of nested loops as follows:
   //    1) outer  loop is over the target points
   //    2) middle loop is over the site points
   //    3) inner  loop is over the Monte-Carlo repetitions

   int dim2 = mSiteConfig.GetPointCount();
   int dim3 = mRepetitionCount;
   assert(mTargetConfig.GetPointCount() > 0);
   assert(dim2 > 0);
   assert(dim3 > 0);

   int index = aTaskIndex - mBaseTaskIndex;
   assert(index >= 0);
   assert(index < mTaskCount);

   int dim2x3      = dim2 * dim3;
   int targetIndex = index / dim2x3;
   index -= (targetIndex * dim2x3);
   int siteIndex  = index / dim3;
   int repetition = index - (siteIndex * dim3);

   assert(targetIndex >= 0);
   assert(targetIndex < mTargetConfig.GetPointCount());
   assert(siteIndex >= 0);
   assert(siteIndex < mSiteConfig.GetPointCount());
   assert(repetition >= 0);
   assert(repetition < mRepetitionCount);
   // cout << "Task " << aTaskIndex << ": T" << targetIndex << " S" << siteIndex << " R" << repetition << endl;
   return Task(*this, aTaskIndex, targetIndex, siteIndex, repetition);
}

} // namespace engage
