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
#include "xio/WsfXIO_JobBoardService.hpp"

#include <iostream>

#include "SafeRIPRProc.hpp"
#include "UtScriptRef.hpp"
#include "WsfObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfRIPRJobBoard.hpp"
#include "WsfRIPRManager.hpp"
#include "WsfRIPRObserver.hpp"
#include "WsfRIPRProcessor.hpp"
#include "WsfSimulation.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_JobBoardPacketRegistry.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

////////////////////////////////////////////////////////////////////////
// WsfXIO_JobBoardRequest Implementation
////////////////////////////////////////////////////////////////////////

WsfXIO_JobBoardRequest::WsfXIO_JobBoardRequest(WsfXIO_Connection* aConnectionPtr,
                                               int                aPlatformIndex,
                                               WsfStringId        aProcessorName,
                                               bool               aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
   , mPlatformIndex(aPlatformIndex)
   , mProcessorName(aProcessorName)
{
}

// virtual
void WsfXIO_JobBoardRequest::HandleResponse(WsfXIO_ResponsePkt& aPkt)
{
   if (WsfXIO_JobBoardInitPkt::cPACKET_ID == aPkt.ID())
   {
      HandleInitialize((WsfXIO_JobBoardInitPkt&)aPkt);
   }
   else if (WsfXIO_JobBoardUpdatePkt::cPACKET_ID == aPkt.ID())
   {
      HandleUpdate((WsfXIO_JobBoardUpdatePkt&)aPkt);
   }
   else if (WsfXIO_JobBoardWinnersUpdatePkt::cPACKET_ID == aPkt.ID())
   {
      HandleUpdate((WsfXIO_JobBoardWinnersUpdatePkt&)aPkt);
   }
   else if (WsfXIO_ChannelIdUpdatePkt::cPACKET_ID == aPkt.ID())
   {
      HandleUpdate((WsfXIO_ChannelIdUpdatePkt&)aPkt);
   }
   else if (WsfXIO_BidControlUpdatePkt::cPACKET_ID == aPkt.ID())
   {
      HandleUpdate((WsfXIO_BidControlUpdatePkt&)aPkt);
   }
}

// virtual
void WsfXIO_JobBoardRequest::Initialized()
{
   WsfXIO_JobBoardRequestPkt pkt;
   pkt.mPlatformIndex     = mPlatformIndex;
   pkt.mRIPRProcessorName = mProcessorName;
   SendRequest(pkt);
}

////////////////////////////////////////////////////////////////////////
// WsfXIO_JobBoardRequestManager Implementation
////////////////////////////////////////////////////////////////////////
WsfXIO_JobBoardRequestManager::WsfXIO_JobBoardRequestManager(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_RequestManager(aInterfacePtr)
   , mProcMap()
{
}

WsfXIO_JobBoardRequestManager::~WsfXIO_JobBoardRequestManager()
{
   //! Remove the cached Job Board Requests.
   for (ProcMap::const_iterator iter = mProcMap.begin(); iter != mProcMap.end(); ++iter)
   {
      WsfXIO_Request* pRequestPtr = FindRequest(iter->second);
      if (pRequestPtr != 0)
      {
         WsfXIO_RequestManager::RemoveRequest(pRequestPtr);
      }
   }
}

void WsfXIO_JobBoardRequestManager::AddRequest(WsfXIO_JobBoardRequest* aRequestPtr)
{
   WsfXIO_RequestManager::AddRequest(aRequestPtr);
   UniqueProcessorId id;
   id.mPlatformIndex = aRequestPtr->GetPlatformIndex();
   id.mProcessorName = aRequestPtr->GetProcessorName();
   mProcMap[id]      = aRequestPtr->GetRequestId();
}

void WsfXIO_JobBoardRequestManager::RemoveRequest(const unsigned int aPlatformIndex)
{
   // match the unique processor id to the request id.
   // get the request and delete the request through the parent method.

   // Can be more than one processor per platform, hence the for-loop.

   for (ProcMap::iterator iter = mProcMap.begin(); iter != mProcMap.end(); ++iter)
   {
      UniqueProcessorId id = iter->first;
      if (id.mPlatformIndex == static_cast<int>(aPlatformIndex))
      {
         RemoveRequest(aPlatformIndex, id.mProcessorName);
      }
   }
}

void WsfXIO_JobBoardRequestManager::RemoveRequest(const unsigned int aPlatformIndex, WsfStringId aProcessorName)
{
   // match the unique processor id to the request id.
   // get the request and delete the request through the parent method.

   UniqueProcessorId uniqueProc;
   uniqueProc.mPlatformIndex = aPlatformIndex;
   uniqueProc.mProcessorName = aProcessorName;

   ProcMap::iterator iter = mProcMap.find(uniqueProc);
   if (iter == mProcMap.end())
   {
      std::cout << "Requested to remove a Job Board request that doesn't exist." << std::endl;
      std::cout << "Platform Index = " << uniqueProc.mPlatformIndex;
      std::cout << ", Processor Name = " << uniqueProc.mProcessorName.GetString();
      std::cout << std::endl;
   }
   else
   {
      WsfXIO_Request* pRequestPtr = FindRequest(iter->second);
      if (pRequestPtr == 0)
      {
         std::cout << "Requested to remove a Job Board request that doesn't exist." << std::endl std::cout
                   << "Platform Index = " << uniqueProc.mPlatformIndex;
         std::cout << ", Processor Name = " << uniqueProc.mProcessorName.GetString();
         std::cout << std::endl;
      }
      else
      {
         WsfXIO_RequestManager::RemoveRequest(pRequestPtr);
      }
   }
}

////////////////////////////////////////////////////////////////////////
// WsfXIO_JobBoardService Implementation
////////////////////////////////////////////////////////////////////////

WsfXIO_JobBoardService::WsfXIO_JobBoardService(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_Service(aInterfacePtr)
{
   mCallbacks.Add(mInterfacePtr->Connect(&WsfXIO_JobBoardService::HandleRequest, this));
   mCallbacks.Add(mInterfacePtr->Connect(&WsfXIO_JobBoardService::HandleCommand, this));
}

WsfXIO_JobBoardService::~WsfXIO_JobBoardService() {}

// virtual
void WsfXIO_JobBoardService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   for (SubscriptionDataMap::iterator i = mSubscriptionsInfo.begin(); i != mSubscriptionsInfo.end(); ++i)
   {
      if (i->second.mSubscriptionPtr == aSubscriptionPtr)
      {
         if (i->second.mHasControl)
         {
            i->second.mRIPRProcessorPtr->SetExternallyControlled(WsfSimulation::GetSimTime(), false);
         }
         mSubscriptionsInfo.erase(i);
         UpdateCallbacks();
         break;
      }
   }
}

void WsfXIO_JobBoardService::HandleRequest(WsfXIO_JobBoardRequestPkt& aPkt)
{
   WsfRIPRProcessor* riprProcessorPtr = FindRIPRProcessor(aPkt.mPlatformIndex, aPkt.mRIPRProcessorName);

   // may need to do additional logic to handle event of a ripr processor already hijacked,
   // but this requestor wishes to hijack it again for itself.
   if (riprProcessorPtr != 0)
   {
      WsfPlatform*         platformPtr     = riprProcessorPtr->GetPlatform();
      WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
      ProcessorId          processorId(platformPtr->GetIndex(), riprProcessorPtr->GetNameId());
      SubscriptionData&    info = mSubscriptionsInfo[processorId];
      info.mPlatformIndex       = processorId.first;
      info.mSubscriptionPtr     = subscriptionPtr;
      info.mRIPRProcessorPtr    = riprProcessorPtr;
      SendInitial(info);
      UpdateCallbacks();
   }
   else
   {
      DenyRequest(aPkt);
   }
}

void WsfXIO_JobBoardService::UpdateCallbacks()
{
   if (mJobBoardCallbacks.Empty() && !mSubscriptionsInfo.empty())
   {
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobAdded.Connect(&WsfXIO_JobBoardService::JobAdded, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobRemoved.Connect(&WsfXIO_JobBoardService::JobRemoved, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobCanceled.Connect(&WsfXIO_JobBoardService::JobCanceled, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobCompleted.Connect(&WsfXIO_JobBoardService::JobCompleted, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobBidChanged.Connect(&WsfXIO_JobBoardService::JobBidChanged, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobPriorityChanged.Connect(&WsfXIO_JobBoardService::JobPriorityChanged, this));
      mJobBoardCallbacks.Add(
         WsfRIPRObserver::JobMaxWinnersChanged.Connect(&WsfXIO_JobBoardService::JobMaxWinnersChanged, this));

      // the following four calls produce an error in VS2005 unless you have the appropriate hotfix installed.
      mJobBoardCallbacks.Add(WsfRIPRObserver::JobWon.Connect(&WsfXIO_JobBoardService::JobWon, this));
      mJobBoardCallbacks.Add(
         WsfRIPRObserver::ChannelIdListChanged.Connect(&WsfXIO_JobBoardService::ChannelIdListChanged, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::BidControlChanged.Connect(&WsfXIO_JobBoardService::BidControlChanged, this));
      mJobBoardCallbacks.Add(WsfRIPRObserver::WinnersReallocated.Connect(&WsfXIO_JobBoardService::WinnersReallocated, this));
   }
   else if (!mJobBoardCallbacks.Empty() && mSubscriptionsInfo.empty())
   {
      mJobBoardCallbacks.Clear();
   }
}

void WsfXIO_JobBoardService::JobAdded(double aSimTime, const WsfRIPRJob* aJobPtr, const UtScriptRef* aScriptRefPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_ADDED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::JobRemoved(double aSimTime, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_REMOVED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::JobWon(double aSimTime, const unsigned int aWinnerProcessorId, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_WON, pkt);
   pkt.mWinningBidId = aWinnerProcessorId;
   Send(pkt);
}

void WsfXIO_JobBoardService::JobCanceled(double aSimTime, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_CANCELED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::JobCompleted(double aSimTime, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_COMPLETED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::JobBidChanged(double aSimTime, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_BID_CHANGED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::JobPriorityChanged(double aSimTime, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_PRIORITY_CHANGED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::JobMaxWinnersChanged(double aSimTime, const WsfRIPRJob* aJobPtr)
{
   WsfXIO_JobBoardUpdatePkt pkt;
   PackUpdate(aSimTime, aJobPtr, cJOB_MAX_WINNERS_CHANGED, pkt);
   Send(pkt);
}

void WsfXIO_JobBoardService::WinnersReallocated(double                     aSimTime,
                                                const unsigned int         aPlatformIndex,
                                                WsfStringId                aRIPRProcessorName,
                                                const ripr::RIPRWinnerMap& aWinnerMap)
{
   WsfXIO_JobBoardWinnersUpdatePkt pkt;
   pkt.mPlatformIndex     = aPlatformIndex;
   pkt.mRIPRProcessorName = aRIPRProcessorName;
   pkt.mWinners           = aWinnerMap;
   Send(pkt);
}

void WsfXIO_JobBoardService::PackUpdate(double                    aSimTime,
                                        const WsfRIPRJob*         aJobPtr,
                                        UpdateType                aUpdateType,
                                        WsfXIO_JobBoardUpdatePkt& aPkt)
{
   aPkt.mUpdateType   = aUpdateType;
   aPkt.mJobPtr       = aJobPtr;
   aPkt.mWinningBidId = UINT_MAX;
}

void WsfXIO_JobBoardService::ChannelIdListChanged(double                  aSimTime,
                                                  const unsigned int      aPlatformIndex,
                                                  WsfStringId             aRIPRProcessorName,
                                                  const JobChannelIdList& aChannelIdList)
{
   WsfXIO_ChannelIdUpdatePkt pkt;
   pkt.mUpdateType        = cCHANNEL_ID_CHANGED;
   pkt.mPlatformIndex     = aPlatformIndex;
   pkt.mRIPRProcessorName = aRIPRProcessorName;
   pkt.mJobChannelIds     = aChannelIdList;
   Send(pkt);
}

void WsfXIO_JobBoardService::BidControlChanged(double             aSimTime,
                                               const unsigned int aPlatformIndex,
                                               WsfStringId        aRIPRProcessorName,
                                               const bool         aBidHijacked)
{
   WsfXIO_BidControlUpdatePkt pkt;
   pkt.mUpdateType        = cBID_CONTROL_CHANGED;
   pkt.mPlatformIndex     = aPlatformIndex;
   pkt.mRIPRProcessorName = aRIPRProcessorName;
   pkt.mBidControl        = aBidHijacked;
   Send(pkt);
}

void WsfXIO_JobBoardService::Send(WsfXIO_JobBoardUpdatePkt& aPkt)
{
   SubscriptionList assigneeSubscriptions;
   SubscriptionList assignerSubscriptions;
   GetSubscriptions(*(aPkt.mJobPtr), assignerSubscriptions, assigneeSubscriptions);
   if (!assignerSubscriptions.empty() || !assigneeSubscriptions.empty())
   {
      // send the message to all registered listeners.
      for (size_t i = 0; i < assignerSubscriptions.size(); ++i)
      {
         assignerSubscriptions[i]->mSubscriptionPtr->SendResponse(aPkt);
      }
      for (size_t i = 0; i < assigneeSubscriptions.size(); ++i)
      {
         assigneeSubscriptions[i]->mSubscriptionPtr->SendResponse(aPkt);
      }
   }
}

void WsfXIO_JobBoardService::Send(WsfXIO_ChannelIdUpdatePkt& aPkt)
{
   SubscriptionList ownerSubscriptions;
   GetSubscriptions(aPkt.mPlatformIndex, aPkt.mRIPRProcessorName, ownerSubscriptions);

   SubscriptionList::iterator iter    = ownerSubscriptions.begin();
   SubscriptionList::iterator iterEnd = ownerSubscriptions.end();
   // send the message to all registered listeners.
   for (; iter != iterEnd; ++iter)
   {
      (*iter)->mSubscriptionPtr->SendResponse(aPkt);
   }
}

void WsfXIO_JobBoardService::Send(WsfXIO_BidControlUpdatePkt& aPkt)
{
   SubscriptionList ownerSubscriptions;
   GetSubscriptions(aPkt.mPlatformIndex, aPkt.mRIPRProcessorName, ownerSubscriptions);

   SubscriptionList::iterator iter    = ownerSubscriptions.begin();
   SubscriptionList::iterator iterEnd = ownerSubscriptions.end();
   // send the message to all registered listeners.
   for (; iter != iterEnd; ++iter)
   {
      (*iter)->mSubscriptionPtr->SendResponse(aPkt);
   }
}

void WsfXIO_JobBoardService::Send(WsfXIO_JobBoardWinnersUpdatePkt& aPkt)
{
   SubscriptionList ownerSubscriptions;
   GetSubscriptions(aPkt.mPlatformIndex, aPkt.mRIPRProcessorName, ownerSubscriptions);

   SubscriptionList::iterator iter    = ownerSubscriptions.begin();
   SubscriptionList::iterator iterEnd = ownerSubscriptions.end();
   // send the message to all registered listeners.
   for (; iter != iterEnd; ++iter)
   {
      (*iter)->mSubscriptionPtr->SendResponse(aPkt);
   }
}

void WsfXIO_JobBoardService::GetProcessorIds(const WsfRIPRJob& aJob, ProcessorId& aAssigner, ProcessorId& aAssignee)
{
   aAssigner = ProcessorId(aJob.AssignerPlatformIndex(), aJob.AssignerProcessorName());
   //   aAssignee = ProcessorId(aJob.GetAssigneePlatformIndex(), aJob.GetAssigneeProcessorName());
}

void WsfXIO_JobBoardService::GetSubscriptions(const WsfRIPRJob& aJob,
                                              SubscriptionList& aAssignerSubscriptions,
                                              SubscriptionList& aAssigneeSubscriptions)
{
   ProcessorId assignerId, assigneeId;
   GetProcessorIds(aJob, assignerId, assigneeId);
   SubscriptionDataMap::iterator i = mSubscriptionsInfo.lower_bound(assignerId);
   for (; i != mSubscriptionsInfo.end() && assignerId.first == i->first.first; ++i)
   {
      if (assignerId.second == 0 || assignerId.second == i->first.second)
      {
         aAssignerSubscriptions.push_back(&i->second);
      }
   }
   i = mSubscriptionsInfo.lower_bound(assigneeId);
   for (; i != mSubscriptionsInfo.end() && assigneeId.first == i->first.first; ++i)
   {
      if (assigneeId.second == 0 || assigneeId.second == i->first.second)
      {
         aAssigneeSubscriptions.push_back(&i->second);
      }
   }
}

void WsfXIO_JobBoardService::GetSubscriptions(const unsigned int aPlatformIndex,
                                              WsfStringId        aProcessorName,
                                              SubscriptionList&  aOwnerSubscriptions)
{
   ProcessorId                   ownerId(aPlatformIndex, aProcessorName);
   SubscriptionDataMap::iterator i = mSubscriptionsInfo.lower_bound(ownerId);
   for (; i != mSubscriptionsInfo.end() && ownerId.first == i->first.first; ++i)
   {
      if (ownerId.second == 0 || ownerId.second == i->first.second)
      {
         aOwnerSubscriptions.push_back(&i->second);
      }
   }
}

void WsfXIO_JobBoardService::SendInitial(SubscriptionData& aData)
{
   typedef std::list<WsfRIPRJob*> JobPtrList;
   typedef std::list<WsfRIPRJob>  JobList;

   WsfXIO_Connection&     connection = aData.mSubscriptionPtr->GetConnection();
   WsfXIO_JobBoardInitPkt pkt;

   // Populate the list of jobs in the job board.
   WsfRIPRJobBoard* jobBoardPtr = aData.mRIPRProcessorPtr->JobBoard();
   if (jobBoardPtr != 0)
   {
      JobPtrList jobsPtrList = jobBoardPtr->GetJobs();
      for (JobPtrList::const_iterator iter = jobsPtrList.begin(); iter != jobsPtrList.end(); ++iter)
      {
         WsfRIPRJob* jobPtr = *iter;
         pkt.mJobs.push_back(*jobPtr);
      }
   }

   // Populate list of commanders.
   pkt.mCommanders = aData.mRIPRProcessorPtr->Commanders();

   // populate job channel count.
   pkt.mJobChannelIds = aData.mRIPRProcessorPtr->JobChannelIds();

   pkt.mRequestId = aData.mSubscriptionPtr->GetRequestId();

   pkt.mBiddingHijacked = aData.mRIPRProcessorPtr->BiddingHijacked();

   connection.Send(pkt);
}

void WsfXIO_JobBoardService::HandleCommand(WsfXIO_JobBoardCommandPkt& aPkt)
{
   WsfRIPRProcessor* riprProcessorPtr = FindRIPRProcessor(aPkt.mJobBoardPlatformIndex, aPkt.mJobBoardProcessorName);
   if (riprProcessorPtr != 0)
   {
      WsfRIPRJobBoard* jobBoardPtr = riprProcessorPtr->JobBoard();
      if (jobBoardPtr != 0)
      {
         if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cJOB_CANCEL)
         {
            WsfRIPRJob* jobPtr = jobBoardPtr->GetJobById(aPkt.mJobId);
            jobBoardPtr->RemoveJob(jobPtr);
         }
         else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cJOB_BID)
         {
            WsfRIPRJob* jobPtr = jobBoardPtr->GetJobById(aPkt.mJobId);
            if (jobPtr == 0)
            {
               std::cout << "INFORMATION: RIPR XIO received a bid change for null job (id=" << aPkt.mJobId
                         << ") on processor " << aPkt.mJobBoardProcessorName.GetString() << std::endl;
               return;
            }
            unsigned int bidderId = aPkt.mBidderUniqueRiprId;
            jobPtr->BidJob(bidderId, aPkt.mBidOrPriority);
            jobBoardPtr->AllocateJobs(true);
         }
         else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cJOB_ADD)
         {
            // not currently supported.
         }
         else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cPURGE_COMPLETED_JOBS) // delete completed jobs.
         {
            jobBoardPtr->DeleteCompletedJobs();
         }
         else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cCHANGE_JOB_PRIORITY)
         {
            WsfRIPRJob* jobPtr = jobBoardPtr->GetJobById(aPkt.mJobId);
            jobPtr->SetPriority(aPkt.mBidOrPriority);
            //            jobBoardPtr->AllocateJobs(true);

            cout << "PRIORITY: Plat: " << aPkt.mJobBoardPlatformIndex;
            cout << ", Proc: " << aPkt.mJobBoardProcessorName.GetString();
            cout << ", Job ID = " << jobPtr->GetId();
            cout << ", Priority = " << aPkt.mBidOrPriority << std::endl;
         }
         else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cCHANGE_MAX_WINNERS)
         {
            WsfRIPRJob* jobPtr = jobBoardPtr->GetJobById(aPkt.mJobId);
            jobPtr->SetWinnersMax((int)(aPkt.mBidOrPriority));
            jobBoardPtr->AllocateJobs(true);
         }
         else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cTAKE_CONTROL_OR_GIVE_BACK_CONTROL)
         {
            riprProcessorPtr->BiddingHijacked(aPkt.mTakeControl);
         }
         //       else if (aPkt.mCommandType == WsfXIO_JobBoardCommandPkt::cJOB_DATA_MODIFY)
         //       {
         //          Would allow user to change the Job Attribute Fields.
         //       }
      }
   }
}

WsfRIPRProcessor* WsfXIO_JobBoardService::FindRIPRProcessor(unsigned int aPlatformIndex, WsfStringId aProcessorNameId)
{
   WsfRIPRProcessor* riprProcessorPtr = 0;
   WsfPlatform*      platformPtr      = WsfSimulation::GetPlatformByIndex(aPlatformIndex);
   if (platformPtr != 0)
   {
      WsfProcessor* processorPtr = platformPtr->GetProcessor(aProcessorNameId);
      if (processorPtr != 0)
      {
         riprProcessorPtr = dynamic_cast<WsfRIPRProcessor*>(processorPtr);
      }
   }
   return riprProcessorPtr;
}

////////////////////////////////////////////////////////////////////////
// WsfXIO_RIPRManagerRequest Implementation
////////////////////////////////////////////////////////////////////////

WsfXIO_RIPRManagerRequest::WsfXIO_RIPRManagerRequest(WsfXIO_Connection* aConnectionPtr, bool aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
{
}

// virtual
void WsfXIO_RIPRManagerRequest::HandleResponse(WsfXIO_ResponsePkt& aPkt)
{
   if (WsfXIO_RIPRManagerInitPkt::cPACKET_ID == aPkt.ID())
   {
      HandleInitialize((WsfXIO_RIPRManagerInitPkt&)aPkt);
   }
   else if (WsfXIO_JobBoardUpdatePkt::cPACKET_ID == aPkt.ID())
   {
      HandleUpdate((WsfXIO_RIPRManagerUpdatePkt&)aPkt);
   }
}

// virtual
void WsfXIO_RIPRManagerRequest::Initialized()
{
   WsfXIO_RIPRManagerRequestPkt pkt;
   SendRequest(pkt);
}

////////////////////////////////////////////////////////////////////////
// WsfXIO_RIPRManagerRequestManager Implementation
////////////////////////////////////////////////////////////////////////
WsfXIO_RIPRManagerRequestManager::WsfXIO_RIPRManagerRequestManager(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_RequestManager(aInterfacePtr)
{
}

WsfXIO_RIPRManagerRequestManager::~WsfXIO_RIPRManagerRequestManager() {}

////////////////////////////////////////////////////////////////////////
// WsfXIO_RIPRManagerService Implementation
////////////////////////////////////////////////////////////////////////

WsfXIO_RIPRManagerService::WsfXIO_RIPRManagerService(WsfXIO_Interface* aInterfacePtr)
   : WsfXIO_Service(aInterfacePtr)
{
   mCallbacks.Add(mInterfacePtr->Connect(&WsfXIO_RIPRManagerService::HandleRequest, this));
}

WsfXIO_RIPRManagerService::~WsfXIO_RIPRManagerService() {}

// virtual
void WsfXIO_RIPRManagerService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   for (SubscriptionList::iterator i = mRIPRSubscriptions.begin(); i != mRIPRSubscriptions.end(); ++i)
   {
      if (*i == aSubscriptionPtr)
      {
         mRIPRSubscriptions.erase(i); // iterator is now invalidated.
         UpdateCallbacks();
         break;
      }
   }
   WsfXIO_Service::SubscriptionCancelled(aSubscriptionPtr);
}

void WsfXIO_RIPRManagerService::HandleRequest(WsfXIO_RIPRManagerRequestPkt& aPkt)
{
   WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
   SendInitial(subscriptionPtr);
   UpdateCallbacks();
}

void WsfXIO_RIPRManagerService::UpdateCallbacks()
{
   if (mRIPRCallbacks.Empty() && !mRIPRSubscriptions.empty())
   {
      mRIPRCallbacks.Add(WsfRIPRObserver::RIPRManagerChanged.Connect(&WsfXIO_RIPRManagerService::RIPRManagerUpdated, this));
   }
   else if (!mRIPRCallbacks.Empty() && mRIPRSubscriptions.empty())
   {
      mRIPRCallbacks.Clear();
   }
}

void WsfXIO_RIPRManagerService::RIPRManagerUpdated()
{
   WsfXIO_RIPRManagerUpdatePkt pkt;
   Populate(pkt);
   Send(pkt);
}

void WsfXIO_RIPRManagerService::Send(WsfXIO_RIPRManagerUpdatePkt& aPkt)
{
   SubscriptionList::iterator iter    = mRIPRSubscriptions.begin();
   SubscriptionList::iterator iterEnd = mRIPRSubscriptions.end();
   // send the message to all registered listeners.
   for (; iter != iterEnd; ++iter)
   {
      (*iter)->SendResponse(aPkt);
   }
}

void WsfXIO_RIPRManagerService::SendInitial(WsfXIO_Subscription* aSubscriptionPtr)
{
   WsfXIO_RIPRManagerInitPkt pkt;
   Populate(pkt);
   pkt.mRequestId = aSubscriptionPtr->GetRequestId();

   aSubscriptionPtr->GetConnection().Send(pkt);
}

void WsfXIO_RIPRManagerService::Populate(WsfXIO_RIPRManagerInitPkt& aPkt)
{
   aPkt.mTime = WsfSimulation::GetSimTime();
   CopyIdMap(aPkt.mIdMap);
}

void WsfXIO_RIPRManagerService::Populate(WsfXIO_RIPRManagerUpdatePkt& aPkt)
{
   aPkt.mTime = WsfSimulation::GetSimTime();
   CopyIdMap(aPkt.mIdMap);
}

typedef std::map<unsigned int, SafeRIPRProc> MgrIdMap;

void WsfXIO_RIPRManagerService::CopyIdMap(XIO_RIPRProcIdMap& aIdMap)
{
   MgrIdMap                 sourceMap = WsfRIPRManager::GetProcIdMap();
   MgrIdMap::const_iterator iter      = sourceMap.begin();
   MgrIdMap::const_iterator iterEnd   = sourceMap.end();
   // copy the map, converting the SafeRIPRProcs to WsfXIO_SafeRIPRProcs.
   for (; iter != iterEnd; ++iter)
   {
      aIdMap.insert(XIO_RIPRProcIdMap::value_type(iter->first, WsfXIO_SafeRIPRProc(iter->second)));
   }
}
