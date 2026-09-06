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
#ifndef WSFXIO_JOB_BOARD_CONTROLSERVICE_HPP
#define WSFXIO_JOB_BOARD_CONTROLSERVICE_HPP

#include "WsfStringId.hpp"
#include "xio/WsfXIO_JobBoardPacketRegistry.hpp"
#include "xio/WsfXIO_Request.hpp"
#include "xio/WsfXIO_SafeRIPRProc.hpp"
#include "xio/WsfXIO_Service.hpp"

class WsfRIPRJob;
class UtScriptRef;
class WsfRIPRProcessor;

//! The Job Board Request contains the data necessary to subscribe to a
//! RIPR processor on a platform.  The request's HandleInitialize and
//! HandleUpdate methods receive the data published about the job board
//! from the remote WSF application.
//!
//! The Job Board Service is the "subscriber" in the publish-subscribe
//! pattern.
class WsfXIO_JobBoardRequest : public WsfXIO_Request
{
public:
   WsfXIO_JobBoardRequest(WsfXIO_Connection* aConnectionPtr,
                          int                aPlatformIndex,
                          WsfStringId        aProcessorName,
                          bool               aIsReliable = true);

   virtual void HandleInitialize(WsfXIO_JobBoardInitPkt& aPkt)      = 0;
   virtual void HandleUpdate(WsfXIO_JobBoardUpdatePkt& aPkt)        = 0;
   virtual void HandleUpdate(WsfXIO_ChannelIdUpdatePkt& aPkt)       = 0;
   virtual void HandleUpdate(WsfXIO_BidControlUpdatePkt& aPkt)      = 0;
   virtual void HandleUpdate(WsfXIO_JobBoardWinnersUpdatePkt& aPkt) = 0;

   unsigned int GetPlatformIndex() const { return mPlatformIndex; }
   WsfStringId  GetProcessorName() const { return mProcessorName; }

protected:
   virtual void HandleResponse(WsfXIO_ResponsePkt& aPkt);

   virtual void Initialized();

   unsigned int mPlatformIndex;
   WsfStringId  mProcessorName;
};

//! The JobBoardRequestManager is a container of JobBoardRequests.  The container
//! manages the memory of each request.
//!
//! NOTE: Class extends WsfXIO_RequestManager with protected status to
//! hide WsfXIO_RequestManager's public functions.
class WsfXIO_JobBoardRequestManager : protected WsfXIO_RequestManager
{
public:
   WsfXIO_JobBoardRequestManager(WsfXIO_Interface* aInterfacePtr);
   virtual ~WsfXIO_JobBoardRequestManager();

   void AddRequest(WsfXIO_JobBoardRequest* aRequestPtr);

   void RemoveRequest(const unsigned int aPlatformIndex);
   void RemoveRequest(const unsigned int aPlatformIndex, WsfStringId aProcessorName);

protected:
private:
   //! The UniqueProcessorId identifiers a processor by name and platform id.
   struct UniqueProcessorId
   {
      bool operator<(const UniqueProcessorId& aRhs) const
      {
         return ((mPlatformIndex < aRhs.mPlatformIndex) ||
                 (mPlatformIndex == aRhs.mPlatformIndex && mProcessorName < aRhs.mProcessorName));
      }
      int         mPlatformIndex;
      WsfStringId mProcessorName;
   };
   typedef std::map<UniqueProcessorId, int> ProcMap;
   ProcMap                                  mProcMap;
};

//! The Job Board Service manages requests for Job Board information
//! from remote applications.  The Job Board Service is the "publisher"
//! in the publish-subscribe pattern.
class WsfXIO_JobBoardService : public WsfXIO_Service
{
public:
   WsfXIO_JobBoardService(WsfXIO_Interface* aInterfacePtr);
   ~WsfXIO_JobBoardService();

protected:
   struct SubscriptionData
   {
      WsfXIO_Subscription* mSubscriptionPtr;
      unsigned int         mPlatformIndex;
      WsfRIPRProcessor*    mRIPRProcessorPtr;
      bool                 mHasControl;
   };
   typedef std::vector<SubscriptionData*>          SubscriptionList;
   typedef std::pair<unsigned int, int>            ProcessorId;
   typedef std::map<ProcessorId, SubscriptionData> SubscriptionDataMap;

   void HandleRequest(WsfXIO_JobBoardRequestPkt& aPkt);

   void HandleCommand(WsfXIO_JobBoardCommandPkt& aPkt);

   WsfRIPRProcessor* FindRIPRProcessor(unsigned int aPlatformIndex, WsfStringId aProcessorNameId);
   void              UpdateCallbacks();


   void SendInitial(SubscriptionData& aData);

   void JobAdded(double aSimTime, const WsfRIPRJob* aJobPtr, const UtScriptRef* aScriptRef);

   void JobRemoved(double aSimTime, const WsfRIPRJob* aJobPtr);

   //! Handles event of a job being won.
   void JobWon(double aSimTime, const unsigned int aWinnerProcessorId, const WsfRIPRJob* aJobPtr);

   //! Handles event of a job being cancelled.
   void JobCanceled(double aSimTime, const WsfRIPRJob* aJobPtr);

   void JobCompleted(double aSimTime, const WsfRIPRJob* aJobPtr);

   void JobBidChanged(double aSimTime, const WsfRIPRJob* aJobPtr);

   void JobPriorityChanged(double aSimTime, const WsfRIPRJob* aJobPtr);

   void JobMaxWinnersChanged(double aSimTime, const WsfRIPRJob* aJobPtr);

   void ChannelIdListChanged(double                  aSimTime,
                             const unsigned int      aPlatformIndex,
                             WsfStringId             aRIPRProcessorName,
                             const JobChannelIdList& aChannelIdList);

   void BidControlChanged(double             aSimTime,
                          const unsigned int aPlatformIndex,
                          WsfStringId        aRIPRProcessorName,
                          const bool         aBidHijacked);

   void WinnersReallocated(double                     aSimTime,
                           const unsigned int         aPlatformIndex,
                           WsfStringId                aRIPRProcessorName,
                           const ripr::RIPRWinnerMap& aWinnerMap);

   //! PackAndSend can pack and send any <timestamp,WsfRIPRJob> message.
   //! @param aUpdatePkt The packet that will be populated by reference.
   void PackUpdate(double                    aSimTime,
                   const WsfRIPRJob*         aJobPtr,
                   xio_ripr::UpdateType      aUpdateType,
                   WsfXIO_JobBoardUpdatePkt& aUpdatePkt);

   //! PackAndSend can pack and send any <timestamp,WsfRIPRJob> message.
   //! @param aUpdatePkt The packet that will be populated by reference.
   void PackUpdate(double aSimTime, const RIPRWinnerMap& aWinnerList, WsfXIO_JobBoardWinnersUpdatePkt& aPkt);

   //! Send a packed message to subscribers.
   void Send(WsfXIO_JobBoardUpdatePkt& aPkt);

   //! Send a packed message to subscribers.
   void Send(WsfXIO_ChannelIdUpdatePkt& aPkt);

   //! Send a packed message to subscribers.
   void Send(WsfXIO_BidControlUpdatePkt& aPkt);

   //! Send a packed message to subscribers.
   void Send(WsfXIO_JobBoardWinnersUpdatePkt& aPkt);


   void GetProcessorIds(const WsfRIPRJob& aJob, ProcessorId& aAssigner, ProcessorId& aAssignee);

   void GetSubscriptions(const WsfRIPRJob& aJob,
                         SubscriptionList& aAssignerSubscriptions,
                         SubscriptionList& aAssigneeSubscriptions);

   void GetSubscriptions(const unsigned int aPlatformIndex, WsfStringId aProcessorName, SubscriptionList& aOwnerSubscriptions);

   virtual void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr);

   SubscriptionDataMap mSubscriptionsInfo;
   UtCallbackHolder    mJobBoardCallbacks;
};

//! The RIPR Manager Request contains the data necessary to subscribe to a
//! RIPR Manager within an application.  The request's HandleInitialize and
//! HandleUpdate methods receive the data published about the RIPR manager
//! from the remote WSF application.
//!
//! The RIPR Manager Service is the "subscriber" in the publish-subscribe
//! pattern.
class WsfXIO_RIPRManagerRequest : public WsfXIO_Request
{
public:
   WsfXIO_RIPRManagerRequest(WsfXIO_Connection* aConnectionPtr, bool aIsReliable = true);

   virtual void HandleInitialize(WsfXIO_RIPRManagerInitPkt& aPkt) = 0;
   virtual void HandleUpdate(WsfXIO_RIPRManagerUpdatePkt& aPkt)   = 0;

protected:
   virtual void HandleResponse(WsfXIO_ResponsePkt& aPkt);

   virtual void Initialized();
};

//! The JobBoardRequestManager is a container of JobBoardRequests.  The container
//! manages the memory of each request.
//!
//! NOTE: Class extends WsfXIO_RequestManager with protected status to
//! hide WsfXIO_RequestManager's public functions.
class WsfXIO_RIPRManagerRequestManager : public WsfXIO_RequestManager
{
public:
   WsfXIO_RIPRManagerRequestManager(WsfXIO_Interface* aInterfacePtr);
   virtual ~WsfXIO_RIPRManagerRequestManager();

protected:
private:
};

//! The RIPR Manager Service manages requests for RIPR Manager information
//! from remote applications.  The RIPR Manager Service is the "publisher"
//! in the publish-subscribe pattern.
class WsfXIO_RIPRManagerService : public WsfXIO_Service
{
public:
   WsfXIO_RIPRManagerService(WsfXIO_Interface* aInterfacePtr);
   ~WsfXIO_RIPRManagerService();

protected:
   static void CopyIdMap(XIO_RIPRProcIdMap& aIdMap);

   typedef std::vector<WsfXIO_Subscription*> SubscriptionList;

   void HandleRequest(WsfXIO_RIPRManagerRequestPkt& aPkt);

   void UpdateCallbacks();

   void SendInitial(WsfXIO_Subscription* aSubscriptionPtr);

   void RIPRManagerUpdated();

   void Populate(WsfXIO_RIPRManagerInitPkt& aPkt);
   void Populate(WsfXIO_RIPRManagerUpdatePkt& aPkt);

   //! Send a packed message to subscribers.
   void Send(WsfXIO_RIPRManagerUpdatePkt& aPkt);

   virtual void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr);

   SubscriptionList mRIPRSubscriptions;
   UtCallbackHolder mRIPRCallbacks;
};
#endif
