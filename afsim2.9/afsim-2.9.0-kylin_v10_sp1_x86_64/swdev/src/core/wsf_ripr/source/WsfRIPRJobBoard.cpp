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
//*   WsfRIPRJobBoard.cpp
//*
//* PURPOSE:
//*   The WsfRIPRJobBoard class accepts jobs from a WsfRIPRProcessor and makes
//*   them available for browsing and claiming by subordinates.
//*
//* AUTHOR
//*   Michael Williams, michael.x.williams@boeing.com
//*
//* NOTES:
//*   None
//***************************************************************************

#include <cfloat>
#include <cmath>
#include <limits>
#include <map>
//#include "sml_Client.h"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtScriptRef.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfRIPRJob.hpp"
#include "WsfRIPRJobBoard.hpp"
#include "WsfRIPRManager.hpp"
#include "WsfRIPRObserver.hpp"
#include "WsfRIPRProcessor.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

// static map of job IDs (dependent) to sets of job IDs (depended-on)
std::map<int, std::set<int>> WsfRIPRJobBoard::mDependencies;

WsfRIPRJobBoard::WsfRIPRJobBoard()
   : WsfObject()
   , mOwnerPtr(nullptr)
   , mJobs()
   , mJobPrioritiesChanged(false)
   , mJobWinners()
   , mAllocationAttempts(3)
   , mAllocationMode(Spread)
   , mHighestBid(-DBL_MAX)
   , mLowestBid(DBL_MAX)
   , mAutoExclusiveMode(false)
   , mJobWindowOpen(false)
   , mBidWindowOpen(false)
   , mJobWindowOpenLength(0)
   , mBidWindowOpenLength(0)
   , mJobStickiness(1.5)
   , mDirtyJobs(true)
   , mPurgeTouchTime(0)
{
   SetName("default");
   SetType("WsfRIPRJobBoard");

   SetJobWindowOpen(true);
   SetBidWindowOpen(true);
}

WsfRIPRJobBoard::WsfRIPRJobBoard(const WsfRIPRProcessor* aOwnerPtr,
                                 bool                    aAutoExclusiveMode,
                                 double                  aJobWindowOpenLength,
                                 double                  aBidWindowOpenLength,
                                 bool                    aJobWindowOpen,
                                 bool                    aBidWindowOpen)
   : WsfObject()
   , mOwnerPtr(aOwnerPtr)
   , mJobs()
   , mJobPrioritiesChanged(false)
   , mJobWinners()
   , mAllocationAttempts(3)
   , mAllocationMode(Spread)
   , mHighestBid(-DBL_MAX)
   , mLowestBid(DBL_MAX)
   , mAutoExclusiveMode(aAutoExclusiveMode)
   , mJobWindowOpen(false)
   , // eliminates valgrind analysis errors
   mBidWindowOpen(false)
   , // eliminates valgrind analysis errors
   mJobWindowOpenLength(aJobWindowOpenLength)
   , mBidWindowOpenLength(aBidWindowOpenLength)
   , mJobStickiness(1.5)
   , mDirtyJobs(true)
   , mPurgeTouchTime(0)
{
   SetName("default");
   SetType("WsfRIPRJobBoard");
   mJobPrioritiesChanged = false;

   SetJobWindowOpen(aJobWindowOpen);
   SetBidWindowOpen(aBidWindowOpen);
}

//! Copy constructor (for Clone())
// protected
WsfRIPRJobBoard::WsfRIPRJobBoard(const WsfRIPRJobBoard& aSrc)
   : WsfObject()
   , mOwnerPtr(aSrc.mOwnerPtr)
   , mJobs(aSrc.mJobs)
   , mJobPrioritiesChanged(aSrc.mJobPrioritiesChanged)
   , mJobWinners(aSrc.mJobWinners)
   , mAllocationAttempts(aSrc.mAllocationAttempts)
   , mAllocationMode(aSrc.mAllocationMode)
   , mHighestBid(aSrc.mHighestBid)
   , mLowestBid(aSrc.mLowestBid)
   , mAutoExclusiveMode(aSrc.mAutoExclusiveMode)
   , mJobWindowOpen(aSrc.mJobWindowOpen)
   , mBidWindowOpen(aSrc.mBidWindowOpen)
   , mJobWindowOpenLength(aSrc.mJobWindowOpenLength)
   , mBidWindowOpenLength(aSrc.mBidWindowOpenLength)
   , mJobStickiness(aSrc.mJobStickiness)
   , mDirtyJobs(true)
   , mPurgeTouchTime(aSrc.mPurgeTouchTime)
{
   SetName(aSrc.GetName());
   SetType(aSrc.GetType());

   SetJobWindowOpen(aSrc.mJobWindowOpen);
   SetBidWindowOpen(aSrc.mBidWindowOpen);
}

// virtual
WsfRIPRJobBoard::~WsfRIPRJobBoard()
{
   // Clean up the 'newed' jobs that were added to this job board
   std::list<WsfRIPRJob*>::const_iterator iter = mJobs.begin();
   for (; iter != mJobs.end(); ++iter)
   {
      delete *iter;
   }
}

//! Returns 'true' if debugging is enabled for this instance.
bool WsfRIPRJobBoard::DebugEnabled() const
{
   if (mOwnerPtr != nullptr)
   {
      return mOwnerPtr->DebugEnabled();
   }

   return false;
}

bool WsfRIPRJobBoard::IsJobWindowOpen(double aSimTime)
{
   UpdateWindows(aSimTime);
   return mJobWindowOpen;
}

bool WsfRIPRJobBoard::IsBidWindowOpen(double aSimTime)
{
   UpdateWindows(aSimTime);
   return mBidWindowOpen;
}


//! Update job and bid window status if necessary
void WsfRIPRJobBoard::UpdateWindows(double aSimTime)
{
   if (mAutoExclusiveMode)
   {
      // bool oldBidWindowStatus = mBidWindowOpen;
      double totalWindowSpan = mBidWindowOpenLength + mJobWindowOpenLength;

      if (totalWindowSpan == 0)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "UW AEM:";
            out.AddNote() << "Total Window Span: 0";
            out.AddNote() << "Both windows opened.";
         }

         SetJobWindowOpen(true);
         SetBidWindowOpen(true);
      }
      else
      {
         double elapsedPeriods = aSimTime / totalWindowSpan;
         double placeInPeriod  = elapsedPeriods - floor(elapsedPeriods);
         double bidProp        = mBidWindowOpenLength / totalWindowSpan;

         SetBidWindowOpen((placeInPeriod < bidProp));
         SetJobWindowOpen(!mBidWindowOpen);

         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "UW AEM:";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Bid: " << (mBidWindowOpen ? "open" : "closed");
            out.AddNote() << "Jobs: " << (mJobWindowOpen ? "open" : "closed");
         }
      }
   }
   else
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "UW ~AEM:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Bid: " << (mBidWindowOpen ? "open" : "closed");
         out.AddNote() << "Jobs: " << (mJobWindowOpen ? "open" : "closed");
      }
   }
}

//! Sets mJobWindowOpen to aOpen; should be the ONLY place this value is set
void WsfRIPRJobBoard::SetJobWindowOpen(bool aOpen)
{
   mJobWindowOpen = aOpen;
}

//! Sets mBidWindowOpen to aOpen; should be the ONLY place this value is set
void WsfRIPRJobBoard::SetBidWindowOpen(bool aOpen)
{
   bool oldBidWindowStatus = mBidWindowOpen;
   mBidWindowOpen          = aOpen;

   if (mBidWindowOpen != oldBidWindowStatus)
   {
      std::list<WsfRIPRJob*>::iterator i;
      for (i = mJobs.begin(); i != mJobs.end(); ++i)
      {
         (*i)->SetBidWindowOpen(mBidWindowOpen);
      }
   }
}

//! Purge deleted jobs and bidders and reprioritize jobs if necessary
void WsfRIPRJobBoard::Update(double aSimTime = 0)
{
   // printf("@@@ DebugEnabled == %d\n", DebugEnabled());

   UpdateWindows(aSimTime);

   CompletedJobsCallbacks(aSimTime);
   DeleteCompletedJobs(aSimTime);
   PurgeDeadBidders(aSimTime);

   if (mJobPrioritiesChanged)
   {
      mJobs.sort(WsfRIPRJobBoard::CompareJobPriorities);
      mJobPrioritiesChanged = false;
   }

   /* don't update jobs here, we'll do it when they're accessed
   std::list<WsfRIPRJob*>::iterator i;
   for(i=mJobs.begin(); i!=mJobs.end(); ++i)
   {
      (*i)->Update(aSimTime);
   }
   */
}

bool WsfRIPRJobBoard::CompareJobPriorities(WsfRIPRJob* firstPtr, WsfRIPRJob* secondPtr)
{
   return (firstPtr->GetPriority() > secondPtr->GetPriority() ? true : false);
}


std::list<WsfRIPRJob*> WsfRIPRJobBoard::GetJobs()
{
   Update(GetSimulation()->GetSimTime());
   return mJobs;
}

void WsfRIPRJobBoard::AddJob(WsfRIPRJob* aJobPtr)
{
   aJobPtr->SetBidWindowOpen(IsBidWindowOpen(GetSimulation()->GetSimTime()));
   mJobs.push_back(aJobPtr);

   mJobPrioritiesChanged = true;
   SetDirtyJobs();
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Adding job.";
      out.AddNote() << "Job: " << aJobPtr->mDescription;
   }
   GetManager()->GetObserver().JobAdded(GetSimulation()->GetSimTime(), aJobPtr);
}

void WsfRIPRJobBoard::RemoveJob(WsfRIPRJob* aJobPtr)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Removing job.";
      out.AddNote() << "Job: " << aJobPtr->mDescription;
   }

   SetDirtyJobs();

   mJobs.remove(aJobPtr);

   GetManager()->GetObserver().JobRemoved(GetSimulation()->GetSimTime(), aJobPtr);
   delete aJobPtr;
}

void WsfRIPRJobBoard::RemoveAllJobs()
{
   if (DebugEnabled())
   {
      ut::log::debug() << "Removing all jobs from job board.";
   }

   // trigger observer callback for each dropped job.
   // NOTE: this should be rolled into a single callback that sends
   // all deleted jobs (perhaps by ID in a single burst).
   for (const auto& job : mJobs)
   {
      GetManager()->GetObserver().JobRemoved(GetSimulation()->GetSimTime(), job);
      delete job;
   }

   // clear the map of all jobs.
   mJobs.clear();
   SetDirtyJobs();
}

void WsfRIPRJobBoard::CompletedJobsCallbacks(double aSimTime)
{
   if (mOwnerPtr != nullptr)
   {
      for (const auto& job : mJobs)
      {
         if (job->GetBestProgress() == 1)
         {
            // job completed, check for callback
            std::string scriptName = job->Data("OnJobComplete").GetString();
            if (!scriptName.empty())
            {
               UtScriptData     scriptRetVal;
               UtScriptDataList scriptArgs;
               UtScriptData     jobName(job->GetId());
               scriptArgs.push_back(jobName);
               // scriptArgs[0].SetString( (*i)->GetName() );
               // scriptArgs[0].SetInt( (*i)->Id() );
               const_cast<WsfRIPRProcessor*>(mOwnerPtr)->ExecuteScript(aSimTime, scriptName, scriptRetVal, scriptArgs);

               // inform the WsfRIPRObserver so XIO can perform transmissions.
               GetManager()->GetObserver().JobCompleted(aSimTime, job);
            }
         }
      }
   }
}

int WsfRIPRJobBoard::DeleteCompletedJobs()
{
   double simTime = GetSimulation()->GetSimTime();
   return DeleteCompletedJobs(simTime);
}

int WsfRIPRJobBoard::DeleteCompletedJobs(double aSimTime)
{
   int  deleted = 0;
   auto i       = mJobs.begin();
   while (i != mJobs.end())
   {
      if ((*i)->GetBestProgress() == 1)
      {
         WsfRIPRJob* removeMePtr = *i;
         ++i;
         RemoveJob(removeMePtr);
         ++deleted;
      }
      else
      {
         ++i;
      }
   }
   SetDirtyJobs();
   return deleted;
}

int WsfRIPRJobBoard::PurgeUntouchedJobs()
{
   return PurgeUntouchedJobs(mPurgeTouchTime);
}

int WsfRIPRJobBoard::PurgeUntouchedJobs(double aStaleTime)
{
   int    deleted      = 0;
   double mCurrentTime = GetSimulation()->GetSimTime();
   auto   i            = mJobs.begin();
   while (i != mJobs.end())
   {
      if (aStaleTime > 0 && mCurrentTime - aStaleTime > (*i)->TouchTime())
      {
         WsfRIPRJob* removeMePtr = *i;
         ++i;
         RemoveJob(removeMePtr);
         deleted++;
      }
      else
      {
         ++i;
      }
   }
   SetDirtyJobs();
   return deleted;
}

int WsfRIPRJobBoard::PurgeDeadBidders(double aSimTime)
{
   int purged = 0;
   for (auto& job : mJobs)
   {
      for (auto j = job->mBidders.begin(); j != job->mBidders.end(); ++j)
      {
         WsfRIPRProcessor* procPtr = GetManager()->GetRiprProcById((*j).first);

         if (procPtr == nullptr || procPtr->GetPlatform()->GetDamageFactor() >= 1)
         {
            j = job->UnbidJob((*j).first);
            purged++;

            // we've reached the end of the list of bidders, so we can stop looking
            if (j == job->mBidders.end())
            {
               break;
            }
            /*
            std::list< std::pair<unsigned int, double> >::iterator temp = (*i)->UnbidJob((*j).first);
            if (temp != (*i)->mBidders.end())
            {
               j = temp--;
            }
            purged++;
            */
         }
      }
   }
   SetDirtyJobs();
   return purged;
}

void WsfRIPRJobBoard::AllocateJobs(double aSimTime, bool aForceReallocation = false)
{
   // only reallocate if something could possibly change or if its forced
   if (!aForceReallocation)
   {
      if (!mDirtyJobs || !IsJobWindowOpen(aSimTime))
      {
         return;
      }
   }

   // update the min/max bid so the normalize() method works correctly
   mHighestBid = -DBL_MAX;
   mLowestBid  = DBL_MAX;
   for (const auto& jobPtr : mJobs)
   {
      ripr::RIPRBidderList bidders = jobPtr->GetBidders();
      for (const auto& bidder : bidders)
      {
         double bid = bidder.second;
         UpdateMinMaxBids(bid);
      }
   }

   // perform designated allocation algorithm
   switch ((int)mAllocationMode)
   {
   case Spread:
      AllocateJobsCover(aSimTime);
      break;
   case Greedy:
      AllocateJobsGreedy(aSimTime);
      break;
   case CoverThenGreedy:
      AllocateJobsCoverThenGreedy(aSimTime);
      break;
   case CoverThenSpread:
      AllocateJobsCoverThenSpread(aSimTime);
      break;
   case StrictPriority:
      AllocateJobsCover(aSimTime, true);
      break;
   default:
      AllocateJobsCover(aSimTime);
   }

   GetManager()->GetObserver().WinnersReallocated(GetSimulation()->GetSimTime(),
                                                  GetOwner()->GetPlatform()->GetIndex(), // Platform Index
                                                  GetOwner()->GetNameId(),               // processor name
                                                  GetWinnerListAsIds());

   mDirtyJobs = false;
}

void WsfRIPRJobBoard::AllocateJobs(bool aForceReallocation = false)
{
   AllocateJobs(GetSimulation()->GetSimTime(), aForceReallocation);
}

// http://en.wikipedia.org/wiki/Generalized_assignment_problem#Greedy_approximation_algorithm
// http://en.wikipedia.org/wiki/Hungarian_algorithm
// http://en.wikipedia.org/wiki/Assignment_problem
// AllocateJobsFrugal() allocates the number of assets a job requests, in order of highest bids, highest priority jobs
// first returns a map of all known bidders
std::map<unsigned int, double> WsfRIPRJobBoard::AllocateJobsCover(double aSimTime, bool aStrictPriority /*=false*/)
{
   return AllocateJobsCover(aSimTime, mJobStickiness, 0, aStrictPriority);
}

std::map<unsigned int, double> WsfRIPRJobBoard::AllocateJobsCover(double aSimTime,
                                                                  double aOldJobBonus,
                                                                  int    aExtraSlots,
                                                                  bool   aStrictPriority /*=false*/)
{
   std::map<unsigned int, double>      procProfit;
   std::map<WsfRIPRJob*, int>          curWinners;
   std::map<unsigned int, double>      knownProcs;
   std::map<unsigned int, WsfRIPRJob*> oldWinners = mJobWinners;
   std::set<int>                       blockedJobs;
   int                                 failedAttempts = 0;

   mJobWinners.clear();

   // make sure jobs and bidders are sorted
   Update(aSimTime);
   // jobs are in order of priority now

   std::list<WsfRIPRJob*>::iterator job_iter;
   if (DebugEnabled())
   {
      size_t jobNum = 1;
      auto   out    = ut::log::debug() << "Jobs:";
      for (job_iter = mJobs.begin(); job_iter != mJobs.end(); ++job_iter)
      {
         WsfRIPRJob* jobPtr = (*job_iter);
         out.AddNote() << "Job " << jobNum << ": " << jobPtr->mDescription;
         ++jobNum;
      }
   }

   if (DebugEnabled())
   {
      ut::log::debug() << "Allocation Attempts:";
   }

   // we'll make mAllocationAttempts attempts to allocate jobs
   // each iteration, we'll only make changes that improve our profit
   for (int attempts = 0; attempts - failedAttempts < mAllocationAttempts; ++attempts)
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "Attempt #" << attempts << ":";
      }

      // loop through jobs and allocate them to winners
      for (job_iter = mJobs.begin(); job_iter != mJobs.end(); ++job_iter)
      {
         WsfRIPRJob* jobPtr = (*job_iter);

         // if this job has been (temporarily) blocked then don't let anyone win it
         if (blockedJobs.find(jobPtr->GetId()) != blockedJobs.end())
         {
            continue;
         }

         jobPtr->Update(aSimTime);
         std::list<std::pair<unsigned int, double>>::iterator bid_iter;

         // loop through all the bidders for this job and let the first mNumWinners win it
         for (bid_iter = (*job_iter)->mBidders.begin(); bid_iter != (*job_iter)->mBidders.end(); ++bid_iter)
         {
            unsigned int bidder   = (*bid_iter).first;
            double       rawBid   = (*bid_iter).second;
            int          curWin_d = curWinners[jobPtr];
            int          maxWin_d = jobPtr->GetWinnersMax();

            // don't bid on jobs with min bid
            if (curWin_d < maxWin_d + aExtraSlots && rawBid > -DBL_MAX)
            {
               if (oldWinners[bidder] == jobPtr)
               {
                  rawBid *= aOldJobBonus;
                  UpdateMinMaxBids(rawBid);
               }

               double normBid   = NormalizeBid(rawBid);
               double newProfit = normBid * jobPtr->GetPriority();

               if (DebugEnabled())
               {
                  auto out = ut::log::debug() << "Bid:";
                  out.AddNote() << "Job: " << jobPtr->mDescription;
                  out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
                  out.AddNote() << "Raw Bid: " << rawBid;
                  out.AddNote() << "Norm Bid: " << normBid;
               }

               double curProfit = -1;
               if (procProfit.find(bidder) != procProfit.end())
               {
                  curProfit = procProfit[bidder];
               }

               // for strict priority, ignore any bids for jobs that are of lower priority.
               if (aStrictPriority && mJobWinners.find(bidder) != mJobWinners.end())
               {
                  if (mJobWinners[bidder]->GetPriority() > jobPtr->GetPriority())
                  {
                     newProfit = 0.0;
                  }
               }

               // if the new profit is more than the old profit for this bidder, assign the bidder we're considering as
               // the winner and unassign them from their previous job
               if (curProfit < newProfit)
               {
                  // decrement the number of winners of bidder's old job
                  if (mJobWinners.find(bidder) != mJobWinners.end())
                  {
                     curWinners[mJobWinners[bidder]] = std::max(0, curWinners[mJobWinners[bidder]] - 1);
                  }

                  // increment the number of winners of bidder's new job
                  curWinners[jobPtr]++;

                  SetJobWon(bidder, jobPtr); // set bidder's new job and inform observers.
                  procProfit[bidder] = newProfit;

                  if (DebugEnabled())
                  {
                     auto out = ut::log::debug() << "Winner:";
                     out.AddNote() << "Job: " << jobPtr->mDescription;
                     out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
                     out.AddNote() << "New Profit: " << newProfit;
                  }
               }
            }
            knownProcs[bidder] = 0;
         }
      }

      // block highest priority job that didn't satisfy dependency or min winner requirements
      // first loop over jobs and block those who don't have the minimum number of winners
      bool fail = false;

      if (!fail)
      {
         fail = BlockJobsWithoutMinWinners(mJobs, curWinners, blockedJobs, mJobWinners, procProfit);
      }

      if (!fail)
      {
         fail = BlockJobsWithFailedDependencies(mJobs, curWinners, blockedJobs, mJobWinners, procProfit);
      }

      if (DebugEnabled())
      {
         auto                                          out = ut::log::debug() << "Cover Results:";
         std::map<unsigned int, WsfRIPRJob*>::iterator proc_iter;
         for (proc_iter = mJobWinners.begin(); proc_iter != mJobWinners.end(); ++proc_iter)
         {
            double      profit     = procProfit[(*proc_iter).first];
            std::string bidderName = GetManager()->GetRiprProcById((*proc_iter).first)->GetPlatform()->GetName();
            std::string jobName    = "no job";
            if ((*proc_iter).second != nullptr)
            {
               jobName = (*proc_iter).second->mDescription;
            }

            auto winnerNote = out.AddNote() << "Winner:";
            winnerNote.AddNote() << "Job: " << jobName;
            winnerNote.AddNote() << "Bidder: " << bidderName;
            winnerNote.AddNote() << "Profit: " << profit;
         }
      }

      if (fail)
      {
         ++failedAttempts;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Rolling back due to failed min winners or dependencies.";
            out.AddNote() << "Failures: " << failedAttempts;
         }
      }
   }
   return knownProcs;
}

// AllocateJobsGreedy() ignores the number of bidders a job requests and lets every asset take the job they bid highest on
void WsfRIPRJobBoard::AllocateJobsGreedy(double aSimTime)
{
   if (DebugEnabled())
   {
      ut::log::debug() << "AllocateJobsGreedy()";
   }

   mJobWinners.clear();
   std::map<unsigned int, double> procProfit;

   // make sure jobs and bidders are sorted
   Update(aSimTime);

   std::list<WsfRIPRJob*>::iterator job_iter;
   // jobs are in order of priority now

   // loop through all jobs
   for (job_iter = mJobs.begin(); job_iter != mJobs.end(); ++job_iter)
   {
      (*job_iter)->Update(aSimTime);
      std::list<std::pair<unsigned int, double>>::iterator bid_iter;

      // loop through all bidders and set their job to this job if their bid for this job is higher than the bid for
      // their current job
      for (bid_iter = (*job_iter)->mBidders.begin(); bid_iter != (*job_iter)->mBidders.end(); ++bid_iter)
      {
         unsigned int bidder  = (*bid_iter).first;
         WsfRIPRJob*  jobPtr  = (*job_iter);
         double       normBid = NormalizeBid((*bid_iter).second);

         // don't bid on jobs min raw bids
         if ((*bid_iter).second <= -DBL_MAX)
         {
            continue;
         }

         double newProfit = normBid * jobPtr->GetPriority();
         double curProfit = -1;
         if (procProfit.find(bidder) != procProfit.end())
         {
            curProfit = procProfit[bidder];
         }

         // if bidder will make more from this job than his current job, assign him here
         if ((curProfit < newProfit))
         {
            // set bidder's new job
            mJobWinners[bidder] = jobPtr;
            // save our new profit
            procProfit[bidder] = newProfit;

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Winner:";
               out.AddNote() << "Job: " << jobPtr->mDescription;
               out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
               out.AddNote() << "New Profit: " << newProfit;
            }
         }
      }
   }

   if (DebugEnabled())
   {
      auto                                          out = ut::log::debug() << "Results:";
      std::map<unsigned int, WsfRIPRJob*>::iterator proc_iter;
      for (proc_iter = mJobWinners.begin(); proc_iter != mJobWinners.end(); ++proc_iter)
      {
         if ((*proc_iter).second == nullptr)
         {
            continue;
         }

         double      profit     = procProfit[(*proc_iter).first];
         std::string bidderName = GetManager()->GetRiprProcById((*proc_iter).first)->GetPlatform()->GetName();
         std::string jobName    = (*proc_iter).second->mDescription;

         auto winnerNote = out.AddNote() << "Winner:";
         winnerNote.AddNote() << "Job Name: " << jobName;
         winnerNote.AddNote() << "Bidder Name: " << bidderName;
         winnerNote.AddNote() << "Profit: " << profit;
      }
   }
}

// AllocateJobsCoverThenGreedy() allocates the number of assets a job requests, in order of highest bids, highest
// priority jobs first, and then lets any remaining assets take whatever job they bid highest on
void WsfRIPRJobBoard::AllocateJobsCoverThenGreedy(double aSimTime)
{
   if (DebugEnabled())
   {
      ut::log::debug() << "AllocateJobsCoverThenGreedy()";
   }

   std::map<unsigned int, double> procProfit;
   std::map<WsfRIPRJob*, int>     curWinners;
   std::map<unsigned int, double> knownProcs;

   knownProcs = AllocateJobsCover(aSimTime);

   // now allocate any assets that don't yet have jobs
   std::map<unsigned int, double>::iterator bid_iter;
   std::map<unsigned int, WsfRIPRJob*>      bestJob;

   for (bid_iter = knownProcs.begin(); bid_iter != knownProcs.end(); ++bid_iter)
   {
      unsigned int bidder = (*bid_iter).first;
      if (mJobWinners.find(bidder) == mJobWinners.end())
      {
         std::list<WsfRIPRJob*>::iterator job_iter;
         // loop through jobs and find our bidder
         for (job_iter = mJobs.begin(); job_iter != mJobs.end(); ++job_iter)
         {
            WsfRIPRJob* jobPtr = (*job_iter);

            if (GetManager()->GetRiprProcById(bidder) == nullptr)
            {
               // our bidding platform no longer exists
               break;
            }
            double bid = jobPtr->GetBid(bidder);

            double normBid   = NormalizeBid(bid);
            double newProfit = normBid * jobPtr->GetPriority();
            double curProfit = (*bid_iter).second;

            // don't bid on jobs with min bid
            if (bid > -DBL_MAX)
            {
               if (DebugEnabled())
               {
                  auto out = ut::log::debug() << "Bid:";
                  out.AddNote() << "Job: " << jobPtr->mDescription;
                  out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
                  out.AddNote() << "Profit: " << newProfit;
               }
               if (bestJob.find(bidder) == bestJob.end() || curProfit < newProfit)
               {
                  bestJob[bidder]    = jobPtr;
                  (*bid_iter).second = newProfit;
               }
            }
            else
            {
               if (DebugEnabled())
               {
                  auto out = ut::log::debug() << "Bid:";
                  out.AddNote() << "Job: " << jobPtr->mDescription;
                  out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
                  out.AddNote() << "Norm Bid: " << normBid;
               }
            }
         }
         SetJobWon(bidder, bestJob[bidder]);
         procProfit[bidder] = (*bid_iter).second;
      }
   }

   if (DebugEnabled())
   {
      auto                                          out = ut::log::debug() << "Greedy results:";
      std::map<unsigned int, WsfRIPRJob*>::iterator proc_iter;
      for (proc_iter = mJobWinners.begin(); proc_iter != mJobWinners.end(); ++proc_iter)
      {
         if ((*proc_iter).second == nullptr)
         {
            continue;
         }

         double      profit     = procProfit[(*proc_iter).first];
         std::string bidderName = GetManager()->GetRiprProcById((*proc_iter).first)->GetPlatform()->GetName();
         std::string jobName    = (*proc_iter).second->mDescription;

         if (profit > 0)
         {
            auto winnerNote = out.AddNote() << "Winner:";
            winnerNote.AddNote() << "Job: " << jobName;
            winnerNote.AddNote() << "Bidder: " << bidderName;
            winnerNote.AddNote() << "Profit: " << profit;
         }
      }
   }
}

// AllocateJobsCoverThenSpread() first allocates jobs using AllocateJobsSpread(), then (virtually) increments all job
// max winners by 1 and continues to spread out assets until all are assigned
void WsfRIPRJobBoard::AllocateJobsCoverThenSpread(double aSimTime)
{
   if (DebugEnabled())
   {
      ut::log::debug() << "AllocateJobsCoverThenSpread()";
   }

   std::map<unsigned int, double>   knownProcs;
   std::list<WsfRIPRJob*>::iterator job_iter;
   int                              jobSlots = 0;

   // loop through jobs and count job slots and bidders
   for (job_iter = mJobs.begin(); job_iter != mJobs.end(); ++job_iter)
   {
      (*job_iter)->Update(aSimTime);
      jobSlots += (*job_iter)->GetWinnersMax();
      std::list<std::pair<unsigned int, double>>::iterator bid_iter;

      // loop through all the bidders for this job and store all the known bidders
      for (bid_iter = (*job_iter)->mBidders.begin(); bid_iter != (*job_iter)->mBidders.end(); ++bid_iter)
      {
         knownProcs[(*bid_iter).first] = 0;
      }
   }

   knownProcs       = AllocateJobsCover(aSimTime);
   int requiredJobs = static_cast<int>(knownProcs.size());

   if (requiredJobs > jobSlots)
   {
      AllocateJobsCover(aSimTime, 10 * mJobStickiness, (int)ceil((double)(requiredJobs - jobSlots) / (double)mJobs.size()));
   }

   if (DebugEnabled())
   {
      auto                                          out = ut::log::debug() << "Spread results:";
      std::map<unsigned int, WsfRIPRJob*>::iterator proc_iter;
      for (proc_iter = mJobWinners.begin(); proc_iter != mJobWinners.end(); ++proc_iter)
      {
         std::string bidderName = GetManager()->GetRiprProcById((*proc_iter).first)->GetPlatform()->GetName();
         std::string jobName    = (*proc_iter).second->mDescription;

         auto winnerNote = out.AddNote() << "Winner:";
         winnerNote.AddNote() << "Job: " << jobName;
         winnerNote.AddNote() << "Bidder: " << bidderName;
      }
   }
}

WsfRIPRJob* WsfRIPRJobBoard::JobFor(double aSimTime, unsigned int aBidderRIPRProcId)
{
   if (IsJobWindowOpen(aSimTime))
   {
      AllocateJobs(aSimTime);
   }

   std::map<unsigned int, WsfRIPRJob*>::iterator it;
   it = mJobWinners.find(aBidderRIPRProcId);
   if (it != mJobWinners.end())
   {
      return it->second;
   }
   return nullptr;
}

void WsfRIPRJobBoard::ClearBidsFor(double aSimTime, unsigned int aBidderRIPRProcId)
{
   if (!IsJobWindowOpen(aSimTime))
   {
      return;
   }

   SetDirtyJobs();
   std::list<WsfRIPRJob*>::iterator i;
   for (i = mJobs.begin(); i != mJobs.end(); ++i)
   {
      std::list<std::pair<unsigned int, double>>::iterator j;
      for (j = (*i)->mBidders.begin(); j != (*i)->mBidders.end(); ++j)
      {
         if ((*j).first == aBidderRIPRProcId)
         {
            j = (*i)->UnbidJob((*j).first);
            // we've reached the end of the list of bidders, so we can stop looking
            if (j == (*i)->mBidders.end())
            {
               break;
            }
         }
      }
   }
}

WsfRIPRJob* WsfRIPRJobBoard::GetJobById(int aJobId)
{
   std::list<WsfRIPRJob*>::iterator i;
   for (i = mJobs.begin(); i != mJobs.end(); ++i)
   {
      if ((*i)->GetId() == aJobId)
      {
         return (*i);
      }
   }
   return nullptr;
}

WsfRIPRJob* WsfRIPRJobBoard::GetJobByData(std::string aDataKey, UtScriptData aDataValue)
{
   std::list<WsfRIPRJob*>::iterator i;
   UtScriptData                     key(std::move(aDataKey));
   for (i = mJobs.begin(); i != mJobs.end(); ++i)
   {
      WsfRIPRJob* jobPtr = *i;
      auto        iter   = jobPtr->mData.find(key);
      if ((iter != jobPtr->mData.end()) && (iter->second == aDataValue))
      {
         return (*i);
      }
   }
   return nullptr;
}

void WsfRIPRJobBoard::UpdateMinMaxBids(double aBid)
{
   // if aBid is higher than the highest bid, save aBid
   if (mHighestBid < aBid)
   {
      mHighestBid = aBid;
   }
   if (mLowestBid > aBid)
   {
      mLowestBid = aBid;
   }
}

// normalize aBid and return a number between 0 and 1.
double WsfRIPRJobBoard::NormalizeBid(double aBid)
{
   if (fabs(mLowestBid - mHighestBid) < 0.00000001)
   {
      return 0.5;
   }
   double normBid = std::min(std::max((aBid - mLowestBid) / (mHighestBid - mLowestBid), 0.0), 1.0);
   return normBid;
}


void WsfRIPRJobBoard::SetJobWon(const unsigned int aBidder, WsfRIPRJob* aJobPtr)
{
   if (aJobPtr == nullptr)
   {
      mJobWinners.erase(aBidder);
   }
   else
   {
      mJobWinners[aBidder] = aJobPtr;
   }

   // update remote listeners.
   GetManager()->GetObserver().JobWon(GetSimulation()->GetSimTime(), aBidder, aJobPtr);
}

//! check aJobs and block single highest priority unblocked job without the min number of winners
bool WsfRIPRJobBoard::BlockJobsWithoutMinWinners(
   std::list<WsfRIPRJob*>&              aJobs,        // list of jobs to check
   std::map<WsfRIPRJob*, int>&          aCurWinners,  // current number of winners for each job
   std::set<int>&                       aBlockedJobs, // set of jobs that are currently blocked
   std::map<unsigned int, WsfRIPRJob*>& aJobWinners,  // map of proc IDs to jobs they've won
   std::map<unsigned int, double>&      aProcProfit)       // current profit for each proc ID
{
   std::list<WsfRIPRJob*>::iterator job_iter;
   // jobs should be sorted in order of priority
   for (job_iter = aJobs.begin(); job_iter != aJobs.end(); ++job_iter)
   {
      WsfRIPRJob* jobPtr = (*job_iter);

      if (aBlockedJobs.find(jobPtr->GetId()) != aBlockedJobs.end())
      {
         continue;
      }

      // only block jobs that have > 0 winners and < min required winners
      // (so jobs with mWinnersMin == 1 will never be blocked)
      if (aCurWinners[jobPtr] < jobPtr->GetWinnersMin() && aCurWinners[jobPtr] > 0)
      {
         aBlockedJobs.insert(jobPtr->GetId());

         // loop through all the bidders for this job and remove them
         std::list<std::pair<unsigned int, double>>::iterator bid_iter;

         for (bid_iter = (*job_iter)->mBidders.begin(); bid_iter != (*job_iter)->mBidders.end(); ++bid_iter)
         {
            unsigned int bidder = (*bid_iter).first;
            // decrement the number of winners of bidder's old job
            if (aJobWinners.find(bidder) != aJobWinners.end())
            {
               aCurWinners[aJobWinners[bidder]] = std::max(0, aCurWinners[aJobWinners[bidder]] - 1);
            }

            SetJobWon(bidder, nullptr); // set bidder's new job and inform observers.
            aProcProfit[bidder] = 0;

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Job removed. Too few winners.";
               out.AddNote() << "Job: " << jobPtr->mDescription;
               out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
            }
         }
         return true;
      }
   }
   return false;
}

//! check aJobs and block single highest priority job that has failed or blocked dependencies
bool WsfRIPRJobBoard::BlockJobsWithFailedDependencies(
   std::list<WsfRIPRJob*>&              aJobs,        // list of jobs to check
   std::map<WsfRIPRJob*, int>&          aCurWinners,  // current number of winners for each job
   std::set<int>&                       aBlockedJobs, // set of jobs that are currently blocked
   std::map<unsigned int, WsfRIPRJob*>& aJobWinners,  // map of proc IDs to jobs they've won
   std::map<unsigned int, double>&      aProcProfit)       // current profit for each proc ID
{
   std::list<WsfRIPRJob*>::iterator job_iter;
   for (job_iter = aJobs.begin(); job_iter != aJobs.end(); ++job_iter)
   {
      WsfRIPRJob* jobPtr = (*job_iter);

      if (aBlockedJobs.find(jobPtr->GetId()) != aBlockedJobs.end())
      {
         continue;
      }

      std::set<int> visitedJobs;
      std::set<int> knownGood;
      bool          dependenciesOk = DependencyTest(jobPtr, aCurWinners, aBlockedJobs, knownGood, visitedJobs);

      if (!dependenciesOk)
      {
         aBlockedJobs.insert(jobPtr->GetId());

         // loop through all the bidders for this job and remove them
         std::list<std::pair<unsigned int, double>>::iterator bid_iter;

         for (bid_iter = (*job_iter)->mBidders.begin(); bid_iter != (*job_iter)->mBidders.end(); ++bid_iter)
         {
            unsigned int bidder = (*bid_iter).first;
            // decrement the number of winners of bidder's old job
            if (aJobWinners.find(bidder) != aJobWinners.end())
            {
               aCurWinners[aJobWinners[bidder]] = std::max(0, aCurWinners[aJobWinners[bidder]] - 1);
            }

            SetJobWon(bidder, nullptr); // set bidder's new job and inform observers.
            aProcProfit[bidder] = 0;

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Job removed. Failed dependencies.";
               out.AddNote() << "Job: " << jobPtr->mDescription;
               out.AddNote() << "Platform: " << GetManager()->GetRiprProcById(bidder)->GetPlatform()->GetName();
            }
         }
         return true;
      }
   }
   return false;
}

// check all the jobs that aJob depends on to see if they have been won.  Block aJob if all dependencies are not satisfied
bool WsfRIPRJobBoard::DependencyTest(WsfRIPRJob*                 aJobPtr,
                                     std::map<WsfRIPRJob*, int>& aCurWinners,  // current number of winners for each job
                                     std::set<int>&              aBlockedJobs, // set of jobs that are currently blocked
                                     std::set<int>& aKnownGood, // set of jobs that are known to have dependencies satisfied
                                     std::set<int>& aVisited) // set of jobs that have already been visited by this process
{
   bool dependenciesOk = true;

   if (aJobPtr->DependencyCount() == 0)
   {
      return dependenciesOk;
   }

   std::set<int>::iterator it;

   for (it = aJobPtr->Dependencies().begin(); it != aJobPtr->Dependencies().end(); ++it)
   {
      WsfRIPRJob* dependedOnJobPtr = WsfRIPRJobBoard::GetJobById((*it));
      if (aKnownGood.find((*it)) != aKnownGood.end())
      {
         continue;
      }
      else if (aVisited.find((*it)) != aVisited.end())
      // we've got a cycle, which we're disallowing for now
      {
         dependenciesOk = false;

         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Job dependency cycle.";
            out.AddNote() << "Job: " << aJobPtr->mDescription;
            out.AddNote() << "Dependency: " << GetJobById((*it))->mDescription;
         }

         break;
      }

      aVisited.insert(dependedOnJobPtr->GetId());

      if (aBlockedJobs.find((*it)) != aBlockedJobs.end())
      {
         dependenciesOk = false;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Job depends on blocked job.";
            out.AddNote() << "Job: " << aJobPtr->mDescription;
            out.AddNote() << "Dependency: " << GetJobById((*it))->mDescription;
         }
         break;
      }

      if (aCurWinners[dependedOnJobPtr] < dependedOnJobPtr->GetWinnersMin())
      {
         dependenciesOk = false;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Job dependency has too few winners.";
            out.AddNote() << "Job: " << aJobPtr->mDescription;
            out.AddNote() << "Dependency: " << GetJobById((*it))->mDescription;
         }
         break;
      }

      bool ok = DependencyTest(dependedOnJobPtr, aCurWinners, aBlockedJobs, aKnownGood, aVisited);

      if (!ok)
      {
         dependenciesOk = false;
         break;
      }
      else
      {
         aKnownGood.insert(dependedOnJobPtr->GetId());
      }
   }
   return dependenciesOk;
}

ripr::RIPRWinnerMap WsfRIPRJobBoard::GetWinnerListAsIds() const
{
   ripr::RIPRWinnerMap myList;

   for (const auto& winner : mJobWinners)
   {
      myList.emplace(winner.first, winner.second->GetId());
   }
   return myList;
}

std::map<unsigned int, WsfRIPRJob*> WsfRIPRJobBoard::GetWinnerList(const double aSimTime)
{
   if (IsJobWindowOpen(aSimTime))
   {
      AllocateJobs(aSimTime);
   }

   std::map<unsigned int, WsfRIPRJob*> result;
   result.insert(mJobWinners.begin(), mJobWinners.end());
   return result;
}

WsfSimulation* WsfRIPRJobBoard::GetSimulation() const
{
   return (mOwnerPtr != nullptr) ? mOwnerPtr->GetSimulation() : nullptr;
}

WsfRIPRManager* WsfRIPRJobBoard::GetManager() const
{
   return (mOwnerPtr != nullptr) ? mOwnerPtr->GetManager() : nullptr;
}
