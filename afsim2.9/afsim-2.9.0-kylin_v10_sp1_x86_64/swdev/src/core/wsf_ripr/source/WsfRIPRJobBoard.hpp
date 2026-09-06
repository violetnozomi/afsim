// ****************************************************************************
// CUI
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

//***************************************************************************
//* FILE NAME:
//*   WsfRIPRJobBoard.hpp
//*
//* PURPOSE:
//*   The WsfRIPRJobBoard class accepts jobs from a WsfRIPRProcessor and makes
//*   them available for browsing and claiming by subordinates.
//*
//* NOTES:
//*   None
//***************************************************************************

#ifndef WSFRIPRJOBBOARD_HPP
#define WSFRIPRJOBBOARD_HPP

#include <set>

#include "WsfObject.hpp"
#include "WsfRIPRCommon.hpp"
class WsfRIPRProcessor;
class WsfRIPRJob;
class WsfRIPRJobBoard;
class WsfRIPRManager;
class WsfSimulation;

// WsfRIPRJobBoard holds a vector of jobs for its owner that its subordinates
// can browse, claim, and perform.

class WsfRIPRJobBoard : public WsfObject
{
public:
   WsfRIPRJobBoard();
   //      WsfRIPRJobBoard(const WsfRIPRProcessor* aOwner);
   WsfRIPRJobBoard(const WsfRIPRProcessor* aOwnerPtr,
                   bool                    aAutoExclusiveMode   = false,
                   double                  aJobWindowOpenLength = 0,
                   double                  aBidWindowOpenLength = 0,
                   bool                    aJobWindowOpen       = true,
                   bool                    aBidWindowOpen       = true);
   WsfRIPRJobBoard(const WsfRIPRJobBoard& aSrc);
   WsfRIPRJobBoard& operator=(const WsfRIPRJobBoard&) = delete;
   WsfRIPRJobBoard* Clone() const override { return new WsfRIPRJobBoard(*this); }
   ~WsfRIPRJobBoard() override;

   //! Returns 'true' if debugging is enabled for this instance.
   bool DebugEnabled() const;

   // Add aJobPtr to the job board
   void AddJob(WsfRIPRJob* aJobPtr);

   // Remove aJobPtr from the job board //and notify the takers if any
   void RemoveJob(WsfRIPRJob* aJobPtr);
   void RemoveAllJobs();

   // Delete jobs with at least one taker with progress == 1
   int DeleteCompletedJobs(double aSimTime);

   // Delete jobs with at least one taker with progress == 1 using current sim time.
   int DeleteCompletedJobs();

   // Delete jobs that haven't been touched in mPurgeTouchTime or aStaleTime seconds
   int PurgeUntouchedJobs();
   int PurgeUntouchedJobs(double aStaleTime);

   // perform callbacks of any completed jobs
   void CompletedJobsCallbacks(double simTime);

   // Purge dead bidders from jobs
   int PurgeDeadBidders(double aSimTime);

   void                    SetOwner(const WsfRIPRProcessor* aOwnerPtr) { mOwnerPtr = aOwnerPtr; }
   const WsfRIPRProcessor* GetOwner() { return mOwnerPtr; }

   std::list<WsfRIPRJob*> GetJobs(); // TODO change to const & instead of *?

   static bool CompareJobPriorities(WsfRIPRJob* firstPtr, WsfRIPRJob* secondPtr);

   void Update(double aSimTime);

   WsfRIPRJob* JobFor(double aSimTime, unsigned int aBidderRIPRProcId);

   WsfRIPRJob* GetJobByData(std::string aDataKey, UtScriptData aDataValue);
   WsfRIPRJob* GetJobById(int aJobId);

   //! Returns a list of winners (referenced by WsfRIPRProcessor::Id()) and
   //! the job they have been assigned to.
   std::map<unsigned int, WsfRIPRJob*> GetWinnerList(const double aSimTime);

   //! Returns a list of winners (referenced by WsfRIPRProcessor::GetId()) and
   //! the id of the job they have been assigned to.
   ripr::RIPRWinnerMap GetWinnerListAsIds() const;

   void AllocateJobs(double aSimTime, bool aForceReallocation);

   //! Allocates jobs with current sim time as the parameter.
   void AllocateJobs(bool aForceReallocation);

   double NormalizeBid(double);

   enum AllocationMode
   {
      Spread          = 0,
      Greedy          = 1,
      CoverThenGreedy = 2,
      CoverThenSpread = 3,
      StrictPriority  = 4
   };

   AllocationMode GetAllocationMode() { return mAllocationMode; }

   void SetAllocationMode(AllocationMode aMode) { mAllocationMode = aMode; }

   void ClearBidsFor(double aSimTime, unsigned int aBidderRIPRProcId);

   void UpdateWindows(double aSimTime);

   void SetAutoExclusiveMode(bool aAutoExclusiveMode) { mAutoExclusiveMode = aAutoExclusiveMode; };
   void SetJobWindowOpenLength(double aJobWindowOpenLength) { mJobWindowOpenLength = aJobWindowOpenLength; };
   void SetBidWindowOpenLength(double aBidWindowOpenLength) { mBidWindowOpenLength = aBidWindowOpenLength; };

   bool IsJobWindowOpen(double aSimTime);
   bool IsBidWindowOpen(double aSimTime);

   void SetJobWindowOpen(bool aOpen);
   void SetBidWindowOpen(bool aOpen);

   void SetJobStickiness(double aStickiness) { mJobStickiness = aStickiness; };

   void SetOwner(WsfRIPRProcessor* aOwnerPtr) { mOwnerPtr = aOwnerPtr; };

   void SetDirtyJobs() { mDirtyJobs = true; };
   void UpdateMinMaxBids(double aBid);

   void   SetPurgeTouchTime(double aPurgeTime) { mPurgeTouchTime = aPurgeTime; };
   double PurgeTouchTime() { return mPurgeTouchTime; };

   // managing job dependencies
   static std::map<int, std::set<int>> DependenciesAll() { return mDependencies; }
   static std::set<int>&               DependenciesForJob(int aJobId) { return mDependencies[aJobId]; }
   static void                         DependencyAdd(int aDependentJobId, int aDependedOnJobId)
   {
      mDependencies[aDependentJobId].insert(aDependedOnJobId);
   }
   static void DependencyRemove(int aDependentJobId, int aDependedOnJobId)
   {
      mDependencies[aDependentJobId].erase(aDependedOnJobId);
   }
   static bool DependencyCheck(int aDependentJobId, int aDependedOnJobId)
   {
      return (mDependencies[aDependentJobId].find(aDependedOnJobId) == mDependencies[aDependentJobId].end() ? false : true);
   }
   static int  DependencyCount(int aDependentJobId) { return static_cast<int>(mDependencies[aDependentJobId].size()); }
   static void DependencyClearAll() { mDependencies.clear(); }

   WsfSimulation*  GetSimulation() const;
   WsfRIPRManager* GetManager() const;

protected:
   const WsfRIPRProcessor* mOwnerPtr;

   std::list<WsfRIPRJob*>              mJobs;
   bool                                mJobPrioritiesChanged;
   std::map<unsigned int, WsfRIPRJob*> mJobWinners; // key = proc id, value = job pointer
   int                                 mAllocationAttempts;
   AllocationMode                      mAllocationMode;
   double                              mHighestBid;
   double                              mLowestBid;
   bool                                mAutoExclusiveMode;

   std::map<unsigned int, double> AllocateJobsCover(double aSimTime, bool aStrictPriority = false);
   void                           AllocateJobsGreedy(double aSimTime);
   void                           AllocateJobsCoverThenGreedy(double aSimTime);
   void                           AllocateJobsCoverThenSpread(double aSimTime);

   //! SetJobWon registers a job as being won and notifies the observer class.
   void SetJobWon(const unsigned int aBidder, WsfRIPRJob* aJobPtr);

   bool BlockJobsWithoutMinWinners(std::list<WsfRIPRJob*>&              aJobs,
                                   std::map<WsfRIPRJob*, int>&          aCurWinners,
                                   std::set<int>&                       aBlockedJobs,
                                   std::map<unsigned int, WsfRIPRJob*>& aJobWinners,
                                   std::map<unsigned int, double>&      aProcProfit);

   bool BlockJobsWithFailedDependencies(std::list<WsfRIPRJob*>&              aJobs,
                                        std::map<WsfRIPRJob*, int>&          aCurWinners,
                                        std::set<int>&                       aBlockedJobs,
                                        std::map<unsigned int, WsfRIPRJob*>& aJobWinners,
                                        std::map<unsigned int, double>&      aProcProfit);

   bool DependencyTest(WsfRIPRJob*                 aJobPtr,
                       std::map<WsfRIPRJob*, int>& aCurWinners,  // current number of winners for each job
                       std::set<int>&              aBlockedJobs, // set of jobs that are currently blocked
                       std::set<int>&              aKnownGood,   // set of jobs that are known to have ok dependencies
                       std::set<int>&              aVisitedJobs);             // set of jobs that have already been visited

private:
   std::map<unsigned int, double>
   AllocateJobsCover(double aSimTime, double aOldJobBonus, int aExtraSlots, bool aStrictPriority = false);

   bool mJobWindowOpen;
   bool mBidWindowOpen;

   // never SET these, always use SetXWindowOpen()
   // reading ok!
   double mJobWindowOpenLength;
   double mBidWindowOpenLength;
   double mJobStickiness;
   bool   mDirtyJobs;
   double mPurgeTouchTime;

   static std::map<int, std::set<int>> mDependencies;
};

#endif
