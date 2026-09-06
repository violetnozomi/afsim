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
//*   WsfRIPRJob.cpp
//*
//* PURPOSE:
//*   The WsfRIPRJob class stores information about one job that an agent can
//*   perform.  It is generally used with the WsfRIPRJobBoard.
//*
//* AUTHOR:
//*   Michael Williams, michael.x.williams@boeing.com
//*
//* NOTES:
//*   None
//***************************************************************************

#include <cfloat>
#include <cmath>
#include <limits>
#include <string>

//#include "sml_Client.h"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfMessage.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfRIPRJob.hpp"
#include "WsfRIPRJobBoard.hpp"
#include "WsfRIPRManager.hpp"
#include "WsfRIPRObserver.hpp"
#include "WsfRIPRProcessor.hpp"
#include "WsfSimulation.hpp"


int WsfRIPRJob::mNextJobId = 0;

WsfRIPRJob::WsfRIPRJob(WsfRIPRManager* aManagerPtr)
   : WsfObject()
   , mDescription("none")
   , mBidsChanged(false)
   , mBidWindowOpen(true)
   , mPriority(0)
   , mWinnersMax(1)
   , mWinnersMin(1)
   , mAssignerProcessorId(0)
   , mId(NextId())
   , mCurrentTime(0.0)
{
   mManagerPtr = aManagerPtr;
   SetName("none");
   SetType("WsfRIPRJob");
   mLastTouched = GetSimulation()->GetSimTime();
}

WsfRIPRJob::WsfRIPRJob(WsfRIPRManager* aManagerPtr,
                       unsigned int    aAssignerRIPRProcId,
                       std::string&    aJobName,
                       std::string&    aJobDescription,
                       double          aPriority,
                       int             aNumWinners)
   : WsfObject()
   , mDescription(aJobDescription)
   , mBidsChanged(false)
   , mBidWindowOpen(true)
   , mPriority(aPriority)
   , mWinnersMax(aNumWinners)
   , mWinnersMin(1)
   , mAssignerProcessorId(aAssignerRIPRProcId)
   , mId(NextId())
   , mCurrentTime(0.0)
{
   mManagerPtr = aManagerPtr;
   SetName(aJobName);
   SetType("WsfRIPRJob");
   mLastTouched = GetSimulation()->GetSimTime();
}

//! Copy constructor (for Clone())
// protected
WsfRIPRJob::WsfRIPRJob(const WsfRIPRJob& aSrc)
   : WsfObject()
   , mManagerPtr(aSrc.mManagerPtr)
   , mBidders(aSrc.mBidders)
   , mProgress(aSrc.mProgress)
   , mDescription(aSrc.mDescription)
   , mData(aSrc.mData)
   , mBidsChanged(aSrc.mBidsChanged)
   , mBidWindowOpen(aSrc.mBidWindowOpen)
   , mPriority(aSrc.mPriority)
   , mWinnersMax(aSrc.mWinnersMax)
   , mWinnersMin(aSrc.mWinnersMin)
   , mAssignerProcessorId(aSrc.mAssignerProcessorId)
   , mId(aSrc.mId)
   , mCurrentTime(aSrc.mCurrentTime)
   , mLastTouched(aSrc.mLastTouched)
{
   this->SetName(aSrc.GetName());
   this->SetType(aSrc.GetType());
}

// virtual
WsfRIPRJob::~WsfRIPRJob()
{
   mBidders.clear();
   mProgress.clear();
   mData.clear();
   mDescription.clear();
   UtReferenceTracked::FreeReference();
}

WsfRIPRJob& WsfRIPRJob::operator=(const WsfRIPRJob& aSrc)
{
   mManagerPtr          = aSrc.mManagerPtr;
   mBidders             = aSrc.mBidders;
   mProgress            = aSrc.mProgress;
   mDescription         = aSrc.mDescription;
   mData                = aSrc.mData;
   mBidsChanged         = aSrc.mBidsChanged;
   mBidWindowOpen       = aSrc.mBidWindowOpen;
   mPriority            = aSrc.mPriority;
   mWinnersMax          = aSrc.mWinnersMax;
   mWinnersMin          = aSrc.mWinnersMin;
   mAssignerProcessorId = aSrc.mAssignerProcessorId;
   mId                  = aSrc.mId;
   mCurrentTime         = aSrc.mCurrentTime;
   mLastTouched         = aSrc.mLastTouched;

   SetName(aSrc.GetName());
   SetType(aSrc.GetType());

   return *this;
}

//! if Update is called with a time parameter, update the time member var
//! then call the parameterless Update().
void WsfRIPRJob::Update(double aSimTime)
{
   mCurrentTime = aSimTime;
   Update();
}

//! If update is called with blank parameter, don't change the Current Time
//! settings.
void WsfRIPRJob::Update()
{
   // bids change so rapidly that it is best to constrain handling to the
   // update rate frequency.  This smooths out the number of updates RIPR Job
   // sends to the observer.
   if (mBidsChanged)
   {
      mBidders.sort(WsfRIPRJob::CompareBidders);
      mBidsChanged = false;

      // report the bid change to the observer.
      GetManager()->GetObserver().JobBidChanged(CurrentTime(), this);
   }
}

int WsfRIPRJob::NextId()
{
   return mNextJobId++;
}

int WsfRIPRJob::GetId() const
{
   return mId;
}

bool WsfRIPRJob::CompareBidders(const ripr::RIPRBidderPair& first, const ripr::RIPRBidderPair& second)
{
   return (first.second > second.second ? true : false);
}

double WsfRIPRJob::GetBestProgress()
{
   Update();

   ripr::RIPRBidderList::const_iterator i;
   double                               max = 0;

   for (i = mBidders.begin(); i != mBidders.end(); ++i)
   {
      if (mProgress[(*i).first] > max)
      {
         max = mProgress[(*i).first];
      }
   }

   return max;
}

void WsfRIPRJob::BidJob(unsigned int aBidderRIPRProcId, double aBid, double aProgress)
{
   if (!mBidWindowOpen)
   {
      ut::log::info() << "BID_JOB: failed, bid window closed.";
      return;
   }

   BidJob(aBidderRIPRProcId, aBid);
   //   SetProgress(aBidderRIPRProcId, aProgress);
   mProgress[aBidderRIPRProcId] = aProgress;
}

void WsfRIPRJob::BidJob(unsigned int aBidderRIPRProcId, double aBid)
{
   if (!mBidWindowOpen)
   {
      //      printf("BidJob: failed, bid window closed\n");
      return;
   }

   // remove an existing bid, if one exists
   UnbidJob(aBidderRIPRProcId);

   mBidsChanged = true;

   if (aBid > -(DBL_MAX - 1))
   {
      // push on new bid... but it won't be sorted until Update() is called
      mBidders.push_front(std::make_pair(aBidderRIPRProcId, aBid));
   }

   // mark the assigner's job board as "dirty" so that jobs are recalculated
   try
   {
      WsfRIPRJobBoard* jobBoardPtr = GetManager()->GetRiprProcById(aBidderRIPRProcId)->JobBoard();
      jobBoardPtr->SetDirtyJobs();
      if (aBid > -(DBL_MAX - 1))
      {
         jobBoardPtr->UpdateMinMaxBids(aBid);
      }
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Caught a thrown exception in the job board, min & max job values NOT updated.";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
   }
}

ripr::RIPRBidderList::iterator WsfRIPRJob::UnbidJob(unsigned int aQuitter)
{
   /*
   if (!mBidWindowOpen)
   {
      return mBidders.end();
   }
   */

   // mark the assigner's job board as "dirty" so that jobs are recalculated
   try
   {
      GetManager()->GetRiprProcById(mAssignerProcessorId)->JobBoard()->SetDirtyJobs();
   }
   catch (std::exception&)
   {
   }

   mProgress.erase(aQuitter);

   ripr::RIPRBidderList::iterator iter = mBidders.begin();
   while (iter != mBidders.end())
   {
      if ((*iter).first == aQuitter)
      {
         iter = mBidders.erase(iter);
      }
      else
      {
         ++iter;
      }
   }
   return iter;
}

void WsfRIPRJob::ClearBidders()
{
   if (!mBidWindowOpen)
   {
      return;
   }

   // mark the assigner's job board as "dirty" so that jobs are recalculated
   try
   {
      GetManager()->GetRiprProcById(mAssignerProcessorId)->JobBoard()->SetDirtyJobs();
   }
   catch (std::exception&)
   {
   }

   mBidders.clear();
   mProgress.clear();
}

double WsfRIPRJob::GetBid(unsigned int aBidderRIPRProcId) const
{
   ripr::RIPRBidderList::const_iterator i;

   for (i = mBidders.begin(); i != mBidders.end(); ++i)
   {
      if ((*i).first == aBidderRIPRProcId)
      {
         return (*i).second;
      }
   }
   return -DBL_MAX;
}

void WsfRIPRJob::SetProgress(unsigned int aRIPRProcId, double aProgress)
{
   WsfRIPRProcessor* bidderPtr = GetManager()->GetRiprProcById(aRIPRProcId);
   if (bidderPtr != nullptr)
   {
      int channel = bidderPtr->JobChannelByProcId(aRIPRProcId);
      if (channel != -1)
      {
         SetProgress(bidderPtr, channel, aProgress);
      }
   }
}

void WsfRIPRJob::SetProgress(WsfRIPRProcessor* aBidderPtr, int aChannelIndex, double aProgress)
{
   mProgress[aBidderPtr->GetId(aChannelIndex)] = aProgress;
}

double WsfRIPRJob::GetProgress(unsigned int aBidderRIPRProcId)
{
   if (mProgress.find(aBidderRIPRProcId) != mProgress.end())
   {
      return mProgress.find(aBidderRIPRProcId)->second;
   }
   else
   {
      return -1;
   }
}

int WsfRIPRJob::GetRank(unsigned int aBidderRIPRProcId)
{
   Update();
   ripr::RIPRBidderList::iterator i;

   int rank = 1;
   for (i = mBidders.begin(); i != mBidders.end(); ++i, rank++)
   {
      if ((*i).first == aBidderRIPRProcId)
      {
         return rank;
      }
   }
   return -1;
}

WsfStringId WsfRIPRJob::AssignerPlatformName() const
{
   WsfPlatform* platformPtr = AssignerPlatform();
   if (platformPtr != nullptr)
   {
      return platformPtr->GetNameId();
   }
   return WsfStringId();
}

size_t WsfRIPRJob::AssignerPlatformIndex() const
{
   WsfPlatform* platformPtr = AssignerPlatform();
   if (platformPtr != nullptr)
   {
      return platformPtr->GetIndex();
   }
   return 0;
}

WsfPlatform* WsfRIPRJob::AssignerPlatform() const
{
   WsfRIPRProcessor* procPtr = GetManager()->GetRiprProcById(this->mAssignerProcessorId);
   if (procPtr != nullptr)
   {
      WsfPlatform* platformPtr = procPtr->GetPlatform();
      if (platformPtr != nullptr)
      {
         return platformPtr;
      }
   }
   return nullptr;
}

WsfStringId WsfRIPRJob::AssignerProcessorName() const
{
   WsfRIPRProcessor* procPtr = GetManager()->GetRiprProcById(mAssignerProcessorId);
   if (procPtr != nullptr)
   {
      return procPtr->GetNameId();
   }
   return WsfStringId();
}

WsfRIPRProcessor* WsfRIPRJob::Assigner() const
{
   return GetManager()->GetRiprProcById(this->mAssignerProcessorId);
}

//! Sets the maximum number of bidders who can win this job.
void WsfRIPRJob::SetWinnersMax(const int aNum)
{
   if (mWinnersMax != aNum)
   {
      mWinnersMax                      = aNum;
      WsfRIPRProcessor* myProcessorPtr = Assigner();
      myProcessorPtr->JobBoard()->SetDirtyJobs(); // job board needs to reallocate.
      GetManager()->GetObserver().JobMaxWinnersChanged(CurrentTime(), this);
   }
}

//! Sets the minimum number of bidders who can win this job.
void WsfRIPRJob::SetWinnersMin(const int aNum)
{
   if (mWinnersMin != aNum)
   {
      mWinnersMin = aNum;
      // needed?     WsfRIPRObserver::JobMaxWinnersChanged(CurrentTime(), this);
   }
}

//! Set the job's priority.
void WsfRIPRJob::SetPriority(const double aPriority)
{
   double epsilon = 0.001; // or DBL_MIN;
   double diff    = fabs(mPriority - aPriority);

   if (diff > epsilon)
   {
      mPriority                        = aPriority;
      WsfRIPRProcessor* myProcessorPtr = Assigner();
      myProcessorPtr->JobBoard()->SetDirtyJobs(); // job board needs to reallocate.
      GetManager()->GetObserver().JobPriorityChanged(CurrentTime(), this);
   }
}

UtScriptData WsfRIPRJob::Data(const std::string& aDataKey) const
{
   ripr::RIPRJobDataMap::const_iterator i = mData.find(UtScriptData(aDataKey));
   if (i != mData.end())
   {
      return i->second;
   }
   else
   {
      return UtScriptData(0);
   }
}

void WsfRIPRJob::Touch()
{
   mLastTouched = GetSimulation()->GetSimTime();
};

void WsfRIPRJob::SendMessageSetProgress(wsf::comm::Comm* aCommPtr, WsfRIPRProcessor* aBidderPtr, int aChannelIndex, double aProgress)
{
   WsfPlatform* assignerPlatPtr  = this->AssignerPlatform();
   WsfStringId  assignerProcName = this->AssignerProcessorName();

   for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*assignerPlatPtr); !it.AtEnd(); ++it)
   {
      auto commRcvrPtr = *it;
      if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
      {
         auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
         if (networkManagerPtr->PathExists(aCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
         {
            auto jbmPtr = ut::make_unique<WsfMessage>(UtStringIdLiteral("JOB_BOARD_MESSAGE"),
                                                      UtStringIdLiteral("SET_PROGRESS"),
                                                      aBidderPtr->GetPlatform());
            //   jbmPtr->SetDestination(assignerPlatPtr->GetNameId());
            WsfAttributeContainer& aux = jbmPtr->GetAuxData();
            aux.Assign("progress", aProgress);
            aux.Assign("target_processor_name", assignerProcName.GetString());
            aux.Assign("job_id", this->GetId());
            aux.Assign("bidder_proc_id", (int)aBidderPtr->GetId(aChannelIndex));

            aCommPtr->Send(GetSimulation()->GetSimTime(), std::move(jbmPtr), commRcvrPtr->GetAddress());
            break;
         }
      }
   }
}

void WsfRIPRJob::SendMessageBidJob(wsf::comm::Comm*  aCommPtr,
                                   WsfRIPRProcessor* aBidderPtr,
                                   int               aChannelIndex,
                                   double            aBid,
                                   double            aProgress)
{
   WsfPlatform* assignerPlatPtr  = this->AssignerPlatform();
   WsfStringId  assignerProcName = this->AssignerProcessorName();

   for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*assignerPlatPtr); !it.AtEnd(); ++it)
   {
      auto commRcvrPtr = *it;
      if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
      {
         auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
         if (networkManagerPtr->PathExists(aCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
         {
            auto jbmPtr = ut::make_unique<WsfMessage>(UtStringIdLiteral("JOB_BOARD_MESSAGE"),
                                                      UtStringIdLiteral("BID_JOB"),
                                                      aBidderPtr->GetPlatform());
            //   jbmPtr->SetDestination(assignerPlatPtr->GetNameId());
            WsfAttributeContainer& aux = jbmPtr->GetAuxData();
            aux.Assign("bid", aBid);
            aux.Assign("progress", aProgress);
            aux.Assign("target_processor_name", assignerProcName.GetString());
            aux.Assign("job_id", this->GetId());
            aux.Assign("bidder_proc_id", (int)aBidderPtr->GetId(aChannelIndex));

            aCommPtr->Send(GetSimulation()->GetSimTime(), std::move(jbmPtr), commRcvrPtr->GetAddress());
            break;
         }
      }
   }
}

void WsfRIPRJob::SendMessageUnbidJob(wsf::comm::Comm* aCommPtr, WsfRIPRProcessor* aBidderPtr, int aChannelIndex)
{
   WsfPlatform* assignerPlatPtr  = this->AssignerPlatform();
   WsfStringId  assignerProcName = this->AssignerProcessorName();

   for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*assignerPlatPtr); !it.AtEnd(); ++it)
   {
      auto commRcvrPtr = *it;
      if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
      {
         auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
         if (networkManagerPtr->PathExists(aCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
         {
            auto jbmPtr = ut::make_unique<WsfMessage>(UtStringIdLiteral("JOB_BOARD_MESSAGE"),
                                                      UtStringIdLiteral("UNBID_JOB"),
                                                      aBidderPtr->GetPlatform());
            //   jbmPtr->SetDestination(assignerPlatPtr->GetNameId());
            WsfAttributeContainer& aux = jbmPtr->GetAuxData();
            aux.Assign("target_processor_name", assignerProcName.GetString());
            aux.Assign("job_id", this->GetId());
            aux.Assign("bidder_proc_id", (int)aBidderPtr->GetId(aChannelIndex));

            aCommPtr->Send(GetSimulation()->GetSimTime(), std::move(jbmPtr), commRcvrPtr->GetAddress());
            break;
         }
      }
   }
}

WsfSimulation* WsfRIPRJob::GetSimulation() const
{
   return &GetManager()->GetSimulation();
}


WsfScriptRIPRJobClass::WsfScriptRIPRJobClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfRIPRJob");
   AddStaticMethod(ut::make_unique<Create>());
   AddMethod(ut::make_unique<Assigner>());
   AddMethod(ut::make_unique<Description>());
   AddMethod(ut::make_unique<Assigner>("GetAssigner"));       // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Description>("GetDescription")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<GetName>());
   AddMethod(ut::make_unique<SetPriority>());
   AddMethod(ut::make_unique<Priority>());
   AddMethod(ut::make_unique<Priority>("GetPriority")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Winners>());
   AddMethod(ut::make_unique<WinnersActual>());
   AddMethod(ut::make_unique<WinnersMin>());
   AddMethod(ut::make_unique<WinnersMax>());
   AddMethod(ut::make_unique<WinnersMax>("NumWinners"));
   AddMethod(ut::make_unique<WinnersMax>("GetNumWinners")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<SetWinnersMin>());
   AddMethod(ut::make_unique<SetWinnersMax>());
   AddMethod(ut::make_unique<SetWinnersMax>("SetNumWinners"));
   AddMethod(ut::make_unique<Data_1>("Data"));
   AddMethod(ut::make_unique<Data_1>("GetData")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Data_2>("Data"));    // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Data_2>("GetData")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<SetData>("SetData"));
   AddMethod(ut::make_unique<SetProgress_1>("SetProgress")); // comm
   AddMethod(ut::make_unique<SetProgress_2>("SetProgress")); // comm
   AddMethod(ut::make_unique<BestProgress>());
   AddMethod(ut::make_unique<BestProgress>("GetBestProgress")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Progress_1>("Progress"));
   AddMethod(ut::make_unique<Progress_1>("GetProgress")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Progress_2>("Progress"));
   AddMethod(ut::make_unique<Progress_2>("GetProgress")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<BidJob_1>("BidJob"));        // comm
   AddMethod(ut::make_unique<BidJob_2>("BidJob"));        // comm
   AddMethod(ut::make_unique<BidJob_3>("BidJob"));        // comm
   AddMethod(ut::make_unique<BidJob_4>("BidJob"));        // comm
   AddMethod(ut::make_unique<UnbidJob_1>("UnbidJob"));    // comm
   AddMethod(ut::make_unique<UnbidJob_2>("UnbidJob"));    // comm
   AddMethod(ut::make_unique<ClearBidders>());
   AddMethod(ut::make_unique<GetBid_1>("GetBid"));
   AddMethod(ut::make_unique<GetBid_2>("GetBid"));
   AddMethod(ut::make_unique<Rank_1>("Rank"));
   AddMethod(ut::make_unique<Rank_1>("GetRank")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Rank_2>("Rank"));
   AddMethod(ut::make_unique<Rank_2>("GetRank")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Id>());
   AddMethod(ut::make_unique<Id>("GetId")); // NO_DOC | DEPRECATED - should not have the 'get'
   AddMethod(ut::make_unique<Touch>());     // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<TouchTime>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<DependenciesForJob>());
   AddMethod(ut::make_unique<DependencyAdd>());
   AddMethod(ut::make_unique<DependencyRemove>());
   AddMethod(ut::make_unique<DependencyCheck>());
   AddMethod(ut::make_unique<DependencyCount>());
}

// virtual
void WsfScriptRIPRJobClass::Destroy(void* aObjectPtr)
{
   WsfRIPRJob* objPtr = static_cast<WsfRIPRJob*>(aObjectPtr);
   delete objPtr;
}

#include "script/WsfScriptDefs.hpp"
#define RIPR_MANAGER static_cast<WsfRIPRManager*>(SIMULATION->FindExtension("wsf_ripr"))
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Create, 5, "WsfRIPRJob", "WsfRIPRProcessor, string, string, double, int")
{
   WsfRIPRProcessor* assignerPtr    = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string       jobName        = aVarArgs[1].GetString();
   std::string       jobDescription = aVarArgs[2].GetString();
   double            jobPriority    = aVarArgs[3].GetDouble();
   int               numWinners     = aVarArgs[4].GetInt();
   WsfRIPRJob*       tempPtr =
      new WsfRIPRJob(RIPR_MANAGER, assignerPtr->GetId(), jobName, jobDescription, jobPriority, numWinners);
   aReturnVal.SetPointer(new UtScriptRef(tempPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Assigner, 0, "WsfRIPRProcessor", "")
{
   WsfRIPRProcessor* assignerPtr = RIPR_MANAGER->GetRiprProcById(aObjectPtr->AssignerProcessorId());
   aReturnVal.SetPointer(new UtScriptRef(assignerPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Description, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->mDescription);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, GetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, SetPriority, 1, "void", "double")
{
   aObjectPtr->SetPriority(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Priority, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPriority());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Winners, 0, "Array<WsfPlatform>", "")
{
   std::vector<UtScriptData>* resultPtr = new std::vector<UtScriptData>();

   UtScriptClass* platformClassPtr = aContext.GetTypes()->GetClass("WsfPlatform");

   std::map<unsigned int, WsfRIPRJob*> winnerList =
      aObjectPtr->Assigner()->JobBoard()->GetWinnerList(TIME_NOW); // all job assignments on the job board
   std::map<unsigned int, WsfRIPRJob*>::const_iterator winnerIter = winnerList.begin();
   for (; winnerIter != winnerList.end(); ++winnerIter)
   {
      WsfRIPRJob* jobPtr = (*winnerIter).second;

      // if the job ids match, add the winning platform to the results
      if (jobPtr->GetId() == aObjectPtr->GetId())
      {
         int               procId  = (*winnerIter).first;
         WsfRIPRProcessor* procPtr = RIPR_MANAGER->GetRiprProcById(procId);
         if (procPtr != nullptr)
         {
            WsfPlatform* platPtr = procPtr->GetPlatform();
            if (platPtr != nullptr)
            {
               resultPtr->push_back(UtScriptData(new UtScriptRef(platPtr, platformClassPtr)));
            }
         }
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(resultPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, WinnersActual, 0, "int", "")
{
   int                                 numWinners(0);
   std::map<unsigned int, WsfRIPRJob*> winnerList = aObjectPtr->Assigner()->JobBoard()->GetWinnerList(TIME_NOW);
   std::map<unsigned int, WsfRIPRJob*>::const_iterator iter = winnerList.begin();
   for (; iter != winnerList.end(); ++iter)
   {
      int jobId = iter->second->GetId();
      if (jobId == aObjectPtr->GetId())
      {
         numWinners++;
      }
   }

   aReturnVal.SetInt(numWinners);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, SetWinnersMax, 1, "void", "int")
{
   aObjectPtr->SetWinnersMax(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, SetWinnersMin, 1, "void", "int")
{
   aObjectPtr->SetWinnersMin(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, WinnersMax, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetWinnersMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, WinnersMin, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetWinnersMin());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Data_1, 0, "Map<string,Object>", "")
{
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->mData, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Data_2, 1, "Object", "string")
{
   std::string  dataKey = aVarArgs[0].GetString();
   UtScriptData jobData = aObjectPtr->Data(dataKey);
   aReturnVal.Set(jobData);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, SetData, 2, "void", "string, Object")
{
   // UtScriptData temp1 = aVarArgs[0];
   // UtScriptData temp2 = aVarArgs[1];
   if (aObjectPtr->mData.find(aVarArgs[0]) != aObjectPtr->mData.end())
   {
      aObjectPtr->mData[aVarArgs[0]].Clear(); // memory leak
   }
   aObjectPtr->mData[aVarArgs[0]] = aVarArgs[1];
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, SetProgress_1, 2, "void", "WsfRIPRProcessor, double")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   double            progress  = aVarArgs[1].GetDouble();
   int               channel   = 0;

   wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();

   if (jbCommPtr != nullptr)
   {
      aObjectPtr->SendMessageSetProgress(jbCommPtr, bidderPtr, channel, progress);
   }
   else
   {
      aObjectPtr->SetProgress(bidderPtr, channel, progress);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, SetProgress_2, 3, "void", "WsfRIPRProcessor, int, double")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   unsigned int      channel   = (unsigned int)aVarArgs[1].GetInt();
   double            progress  = aVarArgs[2].GetDouble();

   wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();

   if (jbCommPtr != nullptr)
   {
      aObjectPtr->SendMessageSetProgress(jbCommPtr, bidderPtr, channel, progress);
   }
   else
   {
      aObjectPtr->SetProgress(bidderPtr, channel, progress);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, BestProgress, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBestProgress());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Progress_1, 1, "double", "WsfRIPRProcessor")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetDouble(aObjectPtr->GetProgress(bidderPtr->GetId()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Progress_2, 2, "double", "WsfRIPRProcessor, int")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   unsigned int      channel   = (unsigned int)aVarArgs[1].GetInt();
   aReturnVal.SetDouble(aObjectPtr->GetProgress(bidderPtr->GetId(channel)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, BidJob_1, 3, "void", "WsfRIPRProcessor, double, double")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (!bidderPtr->BiddingHijacked())
   {
      unsigned int channel = 0;
      if (bidderPtr->ValidJobTypeChannel(aObjectPtr->GetName(), channel))
      {
         double bid      = aVarArgs[1].GetDouble();
         double progress = aVarArgs[2].GetDouble();

         wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();
         if (jbCommPtr != nullptr)
         {
            aObjectPtr->SendMessageBidJob(jbCommPtr, bidderPtr, channel, bid, progress);
         }
         else
         {
            aObjectPtr->BidJob(bidderPtr->GetId(), bid, progress);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, BidJob_2, 4, "void", "WsfRIPRProcessor, int, double, double")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (!bidderPtr->BiddingHijacked())
   {
      unsigned int channel = (unsigned int)aVarArgs[1].GetInt();

      if (bidderPtr->ValidJobTypeChannel(aObjectPtr->GetName(), channel))
      {
         double bid      = aVarArgs[2].GetDouble();
         double progress = aVarArgs[3].GetDouble();

         wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();
         //      WsfComm* jbCommPtr = bidderPtr->JobBoardComm();
         if (jbCommPtr != nullptr)
         {
            aObjectPtr->SendMessageBidJob(jbCommPtr, bidderPtr, channel, bid, progress);
         }
         else
         {
            aObjectPtr->BidJob(bidderPtr->GetId(channel), bid, progress);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, BidJob_3, 2, "void", "WsfRIPRProcessor, double")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (!bidderPtr->BiddingHijacked())
   {
      unsigned int channel = 0;
      if (bidderPtr->ValidJobTypeChannel(aObjectPtr->GetName(), channel))
      {
         double bid = aVarArgs[1].GetDouble();

         wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();
         if (jbCommPtr != nullptr)
         {
            aObjectPtr->SendMessageBidJob(jbCommPtr, bidderPtr, channel, bid);
         }
         else
         {
            aObjectPtr->BidJob(bidderPtr->GetId(), bid);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, BidJob_4, 3, "void", "WsfRIPRProcessor, int, double")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (!bidderPtr->BiddingHijacked())
   {
      unsigned int channel = (unsigned int)aVarArgs[1].GetInt();

      if (bidderPtr->ValidJobTypeChannel(aObjectPtr->GetName(), channel))
      {
         double bid = aVarArgs[2].GetDouble();

         wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();
         if (jbCommPtr != nullptr)
         {
            aObjectPtr->SendMessageBidJob(jbCommPtr, bidderPtr, channel, bid);
         }
         else
         {
            aObjectPtr->BidJob(bidderPtr->GetId(channel), bid);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, UnbidJob_1, 1, "void", "WsfRIPRProcessor")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (!bidderPtr->BiddingHijacked())
   {
      wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();
      if (jbCommPtr != nullptr)
      {
         aObjectPtr->SendMessageUnbidJob(jbCommPtr, bidderPtr, 0);
      }
      else
      {
         aObjectPtr->UnbidJob(bidderPtr->GetId());
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, UnbidJob_2, 2, "void", "WsfRIPRProcessor, int")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (!bidderPtr->BiddingHijacked())
   {
      unsigned int     channel   = (unsigned int)aVarArgs[1].GetInt();
      wsf::comm::Comm* jbCommPtr = aObjectPtr->Assigner()->JobBoardComm();
      if (jbCommPtr != nullptr)
      {
         aObjectPtr->SendMessageUnbidJob(jbCommPtr, bidderPtr, channel);
      }
      else
      {
         aObjectPtr->UnbidJob(bidderPtr->GetId(channel));
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, ClearBidders, 0, "void", "")
{
   aObjectPtr->ClearBidders();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, GetBid_1, 1, "double", "WsfRIPRProcessor")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetDouble(aObjectPtr->GetBid(bidderPtr->GetId()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, GetBid_2, 2, "double", "WsfRIPRProcessor, int")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   unsigned int      channel   = (unsigned int)aVarArgs[1].GetInt();
   aReturnVal.SetDouble(aObjectPtr->GetBid(bidderPtr->GetId(channel)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Rank_1, 1, "int", "WsfRIPRProcessor")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(aObjectPtr->GetRank(bidderPtr->GetId()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Rank_2, 2, "int", "WsfRIPRProcessor, int")
{
   WsfRIPRProcessor* bidderPtr = static_cast<WsfRIPRProcessor*>(aVarArgs[0].GetPointer()->GetAppObject());
   unsigned int      channel   = (unsigned int)aVarArgs[1].GetInt();
   aReturnVal.SetInt(aObjectPtr->GetRank(bidderPtr->GetId(channel)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Id, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, Touch, 0, "void", "") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->Touch();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, TouchTime, 0, "double", "") // NO_DOC | FOR_TEST_ONLY
{
   aReturnVal.SetDouble(aObjectPtr->TouchTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, DependenciesForJob, 0, "Set<int>", "")
{
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->Dependencies(), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, DependencyAdd, 1, "void", "int")
{
   int dependedOnJobId = (int)aVarArgs[0].GetInt();
   aObjectPtr->DependencyAdd(dependedOnJobId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, DependencyRemove, 1, "void", "int")
{
   int dependedOnJobId = (int)aVarArgs[0].GetInt();
   aObjectPtr->DependencyRemove(dependedOnJobId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, DependencyCheck, 1, "bool", "int")
{
   int  dependedOnJobId = (int)aVarArgs[0].GetInt();
   bool exists          = aObjectPtr->DependencyCheck(dependedOnJobId);
   aReturnVal.SetBool(exists);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRJobClass, WsfRIPRJob, DependencyCount, 0, "int", "")
{
   int count = aObjectPtr->DependencyCount();
   aReturnVal.SetInt(count);
}
