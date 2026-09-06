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

#ifndef WSFXIO_TASKSERVICE_HPP
#define WSFXIO_TASKSERVICE_HPP

#include "wsf_export.h"

#include "WsfStringId.hpp"
class WsfSimulation;
class WsfTask;
class WsfTaskProcessor;
class WsfTrack;
class WsfXIO_ResponsePkt;
#include "xio/WsfXIO_Service.hpp"
class WsfXIO_Simulation;
class WsfXIO_TaskCommandPkt;
class WsfXIO_TaskRequestPkt;
class WsfXIO_TaskUpdatePkt;
class WsfXIO_TaskInitPkt;

class WSF_EXPORT WsfXIO_TaskService : public WsfXIO_Service
{
public:
   WsfXIO_TaskService(WsfXIO_Simulation& aXIO_Simulation);
   ~WsfXIO_TaskService() override;

   static WsfTaskProcessor* FindTaskProcessor(WsfSimulation& aSimulation,
                                              size_t         aPlatformIndex,
                                              WsfStringId&   aProcessorNameId);

protected:
   struct SubscriptionData
   {
      WsfXIO_Subscription* mSubscriptionPtr;
      size_t               mPlatformIndex;
      WsfTaskProcessor*    mTaskProcessorPtr;
      bool                 mHasControl;
   };
   typedef std::vector<SubscriptionData*>          SubscriptionList;
   typedef std::pair<size_t, WsfStringId>          ProcessorId;
   typedef std::map<ProcessorId, SubscriptionData> SubscriptionDataMap;

   void HandleRequest(WsfXIO_TaskRequestPkt& aPkt);

   void HandleCommand(WsfXIO_TaskCommandPkt& aPkt);

   void UpdateCallbacks();

   void SendInitial(SubscriptionData& aData);

   void TaskAccepted(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr);

   void TaskAssigned(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr);

   void TaskCanceled(double aSimTime, const WsfTask* aTaskPtr);

   void TaskCompleted(double aSimTime, const WsfTask* aTaskPtr, WsfStringId aStatus);

   void GetProcessorIds(const WsfTask& aTask, ProcessorId& aAssigner, ProcessorId& aAssignee);

   void GetSubscriptions(const WsfTask&    aTask,
                         SubscriptionList& aAssignerSubscriptions,
                         SubscriptionList& aAssigneeSubscriptions);

   void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr) override;

   WsfXIO_Simulation&  mXIO_Simulation;
   WsfSimulation&      mSimulation;
   SubscriptionDataMap mSubscriptionsInfo;
   UtCallbackHolder    mTaskCallbacks;
};

#endif
