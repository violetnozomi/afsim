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

#ifndef RUNCONFIG_HPP
#define RUNCONFIG_HPP

class UtInput;

#include "OutputConfig.hpp"
#include "SiteConfig.hpp"
#include "TargetConfig.hpp"
#include "Utilities.hpp"

namespace engage
{

class InputConfig;
class Simulation;
class Task;

class RunConfig
{
public:
   typedef std::list<OutputConfig> OutputList;

   RunConfig(const InputConfig& aInputConfig);

   bool Complete(int aRunIndex, int aBaseTaskIndex);

   bool ProcessInput(UtInput& aInput);

   //! Return a reference to the OutputConfig object.
   const OutputList& GetOutputList() const { return mOutputList; }

   //! Return a reference to the SiteConfig object.
   const SiteConfig& GetSiteConfig() const { return mSiteConfig; }

   //! Return a reference to the TargetConfig object.
   const TargetConfig& GetTargetConfig() const { return mTargetConfig; }

   std::string GetRecordFileName(const Task& aTask) const;

   std::string GetEventOutputFileName(const Task& aTask) const;

   //! Get the repetition count for this run.
   int GetRepetitionCount() const { return mRepetitionCount; }

   //! Return the run index for this run.
   int GetRunIndex() const { return mRunIndex; }

   //! Return the number of tasks needed to complete this run.
   int GetTaskCount() const { return mTaskCount; }

   //! Return a reference to the utilities object for coordinate conversions
   const Utilities& GetUtilities() const { return mUtilities; }

   bool CreatePlatforms(Simulation& aSimulation, const Task& aTask) const;

   Task MakeTask(int aTaskIndex) const;

   //! Return the target type for Pk Table generation
   std::string GetPkTargetType() const { return mPkTargetType; }

   //! Return the site type for Pk table generation
   std::string GetPkSiteType() const { return mPkSiteType; }

   //! Return if Pk table generation is enabled for this run
   bool IsPkTableGen() const { return mEnablePkTableGen; }

   //! Return if the grid is a site grid
   bool IsSiteGrid() const { return mSiteGridRun; }

   //! Return the Pk table output directory path
   std::string GetPkPath() const { return mPkOutputPath; }

   //! Return if the grid should be periodically flushed
   bool IsPkPeriodicFlush() const { return mPkPeriodicFlush; }

   //! Return the units used for Pk table length output
   std::string GetPkLengthUnit() const { return mPkLengthUnits; }

   //! Return the units used for Pk table speed output
   std::string GetPkVelocityUnit() const { return mPkVelocityUnits; }

private:
   const InputConfig& mInputConfig;
   int                mRepetitionCount;        // REPS, 1.0
   bool               mRunAllRepetitions;      // REPALL, 1
   bool               mRunUntilNonZeroPk;      // FINDPK, 0
   double             mTrackDetectDelayTime;   // FDEL, 2.0
   double             mFirstEngageAttemptTime; // FTOL, 0.0
   double             mLastEngageAttemptTime;  // LTOL, 0.0
   double             mEngagementInterval;     // RATE, 10.0 sec
   bool               mPerformFlyouts;         // EXEFLY, 'true' if weapon flyouts are to be performed
   int                mSiteShotLimit;          // NSHOT, 1,
   bool               mShootLookShoot;         // SLSTYP, 2
   double             mSalvoDelay;             // SLSDEL, 5.0
   double             mSalvoDelay2;            // SLST2, 3.0
   std::string        mRecordFileBaseName;
   std::string        mEventOutputFileBaseName;

   double mAzCueingError; // AZCUER, 0.0
   double mElCueingError; // ELCUER, 0.0

   int          mPkTermType; // FINDPK, 0.0, 0=Run all, 1=stop after Pk>0.
   OutputList   mOutputList;
   SiteConfig   mSiteConfig;
   TargetConfig mTargetConfig;
   Utilities    mUtilities;

   int mRunIndex;      //!< The 0-based index of this run in the input file
   int mBaseTaskIndex; //!< The global task index of the first task in this run.
   int mTaskCount;

   std::string mPkTargetType;     //! The string used for target type in Pk table generation in this run
   std::string mPkSiteType;       //! The string usd for site type in Pk table generation in this run
   bool        mEnablePkTableGen; //! True if a Pk Table is generated for the run
   bool        mSiteGridRun;      //! True if the run is a site grid
   std::string mPkOutputPath;     //! The path to output the Pk Table directory hierarchy
   bool        mPkPeriodicFlush;  //! True if Pk Table writes to disc should occur during sim runs
   std::string mPkLengthUnits;    //! The output length unit of the Pk table (default "m")
   std::string mPkVelocityUnits;  //! The output speed unit of the Pk table (default "m/s")
};

} // namespace engage

#endif
