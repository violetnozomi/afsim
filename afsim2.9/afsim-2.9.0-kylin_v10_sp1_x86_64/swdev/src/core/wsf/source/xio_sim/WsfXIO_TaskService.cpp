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

#include "xio_sim/WsfXIO_TaskService.hpp"

#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTaskProcessor.hpp"
#include "WsfTrackManager.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_TaskService::WsfXIO_TaskService(WsfXIO_Simulation& aXIO_Simulation)
   : WsfXIO_Service(aXIO_Simulation.GetInterface())
   , mXIO_Simulation(aXIO_Simulation)
   , mSimulation(aXIO_Simulation.GetSimulation())
{
   mCallbacks += GetInterface()->Connect(&WsfXIO_TaskService::HandleRequest, this);
   mCallbacks += GetInterface()->Connect(&WsfXIO_TaskService::HandleCommand, this);
}

WsfXIO_TaskService::~WsfXIO_TaskService() {}

// virtual
void WsfXIO_TaskService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   for (SubscriptionDataMap::iterator i = mSubscriptionsInfo.begin(); i != mSubscriptionsInfo.end(); ++i)
   {
      if (i->second.mSubscriptionPtr == aSubscriptionPtr)
      {
         if (i->second.mHasControl)
         {
            i->second.mTaskProcessorPtr->SetExternallyControlled(mSimulation.GetSimTime(), false);
         }
         mSubscriptionsInfo.erase(i);
         UpdateCallbacks();
         break;
      }
   }
}

void WsfXIO_TaskService::HandleRequest(WsfXIO_TaskRequestPkt& aPkt)
{
   WsfTaskProcessor* taskProcessorPtr = FindTaskProcessor(mSimulation, aPkt.mPlatformIndex, aPkt.mTaskProcessorName);

   if ((taskProcessorPtr != nullptr) && (!aPkt.mTakeControl || !taskProcessorPtr->IsExternallyControlled()))
   {
      WsfPlatform*         platformPtr     = taskProcessorPtr->GetPlatform();
      WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
      ProcessorId          processorId(platformPtr->GetIndex(), taskProcessorPtr->GetNameId());
      SubscriptionData&    info = mSubscriptionsInfo[processorId];
      info.mPlatformIndex       = processorId.first;
      info.mSubscriptionPtr     = subscriptionPtr;
      info.mTaskProcessorPtr    = taskProcessorPtr;
      info.mHasControl          = aPkt.mTakeControl;
      if (aPkt.mTakeControl)
      {
         taskProcessorPtr->SetExternallyControlled(mSimulation.GetSimTime(), true);
      }
      SendInitial(info);
      UpdateCallbacks();
   }
   else
   {
      DenyRequest(aPkt);
   }
}

void WsfXIO_TaskService::UpdateCallbacks()
{
   if (mTaskCallbacks.Empty() && !mSubscriptionsInfo.empty())
   {
      mTaskCallbacks.Add(WsfObserver::TaskAccepted(&mSimulation).Connect(&WsfXIO_TaskService::TaskAccepted, this));
      mTaskCallbacks.Add(WsfObserver::TaskAssigned(&mSimulation).Connect(&WsfXIO_TaskService::TaskAssigned, this));
      mTaskCallbacks.Add(WsfObserver::TaskCanceled(&mSimulation).Connect(&WsfXIO_TaskService::TaskCanceled, this));
      mTaskCallbacks.Add(WsfObserver::TaskCompleted(&mSimulation).Connect(&WsfXIO_TaskService::TaskCompleted, this));
   }
   else if (!mTaskCallbacks.Empty() && mSubscriptionsInfo.empty())
   {
      mTaskCallbacks.Clear();
   }
}

//! Simulation observer for TaskAccepted.
void WsfXIO_TaskService::TaskAccepted(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr)
{
   SubscriptionList assigneeSubscriptions;
   SubscriptionList assignerSubscriptions;
   GetSubscriptions(*aTaskPtr, assignerSubscriptions, assigneeSubscriptions);
   if (!assignerSubscriptions.empty() || !assigneeSubscriptions.empty())
   {
      WsfXIO_TaskUpdatePkt pkt;
      pkt.mUpdateType = WsfXIO_TaskUpdatePkt::cTASK_ACCEPTED;
      pkt.mTaskPtr    = aTaskPtr;
      pkt.mTrackPtr   = aTrackPtr;
      for (size_t i = 0; i < assigneeSubscriptions.size(); ++i)
      {
         assigneeSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
   }
}

//! Simulation observer for TaskAssigned.
void WsfXIO_TaskService::TaskAssigned(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr)
{
   SubscriptionList assigneeSubscriptions;
   SubscriptionList assignerSubscriptions;
   GetSubscriptions(*aTaskPtr, assignerSubscriptions, assigneeSubscriptions);
   if (!assignerSubscriptions.empty() || !assigneeSubscriptions.empty())
   {
      WsfXIO_TaskUpdatePkt pkt;
      pkt.mUpdateType = WsfXIO_TaskUpdatePkt::cTASK_ASSIGNED;
      pkt.mTaskPtr    = aTaskPtr;
      pkt.mTrackPtr   = aTrackPtr;
      for (size_t i = 0; i < assignerSubscriptions.size(); ++i)
      {
         assignerSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
      for (size_t i = 0; i < assigneeSubscriptions.size(); ++i)
      {
         assigneeSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
   }
}

//! Simulation observer for TaskCanceled.
void WsfXIO_TaskService::TaskCanceled(double aSimTime, const WsfTask* aTaskPtr)
{
   SubscriptionList assigneeSubscriptions;
   SubscriptionList assignerSubscriptions;
   GetSubscriptions(*aTaskPtr, assignerSubscriptions, assigneeSubscriptions);
   if (!assignerSubscriptions.empty() || !assigneeSubscriptions.empty())
   {
      WsfXIO_TaskUpdatePkt pkt;
      pkt.mUpdateType = WsfXIO_TaskUpdatePkt::cTASK_CANCELED;
      pkt.mTaskPtr    = aTaskPtr;
      pkt.mTrackPtr   = nullptr;
      for (size_t i = 0; i < assignerSubscriptions.size(); ++i)
      {
         assignerSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
      for (size_t i = 0; i < assigneeSubscriptions.size(); ++i)
      {
         assigneeSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
   }
}

//! Simulation observer for TaskCompleted.
void WsfXIO_TaskService::TaskCompleted(double aSimTime, const WsfTask* aTaskPtr, WsfStringId aStatus)
{
   SubscriptionList assigneeSubscriptions;
   SubscriptionList assignerSubscriptions;
   GetSubscriptions(*aTaskPtr, assignerSubscriptions, assigneeSubscriptions);
   if (!assignerSubscriptions.empty() || !assigneeSubscriptions.empty())
   {
      WsfXIO_TaskUpdatePkt pkt;
      pkt.mUpdateType = WsfXIO_TaskUpdatePkt::cTASK_COMPLETED;
      pkt.mTaskPtr    = aTaskPtr;
      pkt.mTrackPtr   = nullptr;
      pkt.mTaskStatus = aStatus;
      for (size_t i = 0; i < assignerSubscriptions.size(); ++i)
      {
         assignerSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
      for (size_t i = 0; i < assigneeSubscriptions.size(); ++i)
      {
         assigneeSubscriptions[i]->mSubscriptionPtr->SendResponse(pkt);
      }
   }
}

void WsfXIO_TaskService::GetProcessorIds(const WsfTask& aTask, ProcessorId& aAssigner, ProcessorId& aAssignee)
{
   aAssigner = ProcessorId(aTask.GetAssignerPlatformIndex(), aTask.GetAssignerProcessorName());
   aAssignee = ProcessorId(aTask.GetAssigneePlatformIndex(), aTask.GetAssigneeProcessorName());
}

void WsfXIO_TaskService::GetSubscriptions(const WsfTask&    aTask,
                                          SubscriptionList& aAssignerSubscriptions,
                                          SubscriptionList& aAssigneeSubscriptions)
{
   ProcessorId assignerId, assigneeId;
   GetProcessorIds(aTask, assignerId, assigneeId);
   SubscriptionDataMap::iterator i = mSubscriptionsInfo.lower_bound(assignerId);
   for (; i != mSubscriptionsInfo.end() && assignerId.first == i->first.first; ++i)
   {
      if (assignerId.second == nullptr || assignerId.second == i->first.second)
      {
         aAssignerSubscriptions.push_back(&i->second);
      }
   }
   i = mSubscriptionsInfo.lower_bound(assigneeId);
   for (; i != mSubscriptionsInfo.end() && assigneeId.first == i->first.first; ++i)
   {
      if (assigneeId.second == nullptr || assigneeId.second == i->first.second)
      {
         aAssigneeSubscriptions.push_back(&i->second);
      }
   }
}

void WsfXIO_TaskService::SendInitial(SubscriptionData& aData)
{
   WsfXIO_Connection& connection = aData.mSubscriptionPtr->GetConnection();
   WsfXIO_TaskInitPkt pkt;
   pkt.mReceivedTasks.insert(pkt.mReceivedTasks.end(),
                             aData.mTaskProcessorPtr->ReceivedTaskList().begin(),
                             aData.mTaskProcessorPtr->ReceivedTaskList().end());
   pkt.mAssignedTasks.insert(pkt.mAssignedTasks.end(),
                             aData.mTaskProcessorPtr->AssignedTaskList().begin(),
                             aData.mTaskProcessorPtr->AssignedTaskList().end());
   pkt.mRequestId = aData.mSubscriptionPtr->GetRequestId();
   connection.Send(pkt);
}

void WsfXIO_TaskService::HandleCommand(WsfXIO_TaskCommandPkt& aPkt)
{
   // A = assigner
   // B = assignee
   // A & B is easier to identify than 'er' vs. 'ee'

   WsfTaskProcessor* taskProcA_Ptr =
      FindTaskProcessor(mSimulation, aPkt.mAssignerPlatformIndex, aPkt.mAssignerProcessorName);
   WsfTaskProcessor* taskProcB_Ptr =
      FindTaskProcessor(mSimulation, aPkt.mAssigneePlatformIndex, aPkt.mAssigneeProcessorName);

   double simTime(mSimulation.GetSimTime());
   if (taskProcA_Ptr != nullptr)
   {
      // if the resource isn't initialized, initialize it as a default WsfTaskResource.
      WsfTaskResource* resourcePtr = aPkt.mResourcePtr;
      if (resourcePtr == nullptr)
      {
         resourcePtr = new WsfTaskResource();
      }

      WsfTrack* trackPtr = taskProcA_Ptr->GetPlatform()->GetTrackManager().FindTrack(aPkt.mTrackId);
      // If track can't be found using TrackId, use Target Name.
      if (trackPtr == nullptr)
      {
         WsfLocalTrackList& trackList = taskProcA_Ptr->GetPlatform()->GetTrackManager().GetTrackList();
         for (unsigned int i = 0; i < trackList.GetTrackCount(); ++i)
         {
            if (trackList.GetTrackEntry(i)->GetTargetName() == aPkt.mTargetNameId)
            {
               trackPtr = trackList.GetTrackEntry(i);
               break;
            }
         }
      }

      if (aPkt.mCommandType == WsfXIO_TaskCommandPkt::cTASK_ASSIGN)
      {
         WsfPlatform* platformB_Ptr = nullptr;
         if (aPkt.mAssigneePlatformIndex == 0)
         {
            platformB_Ptr = mSimulation.GetPlatformByIndex(aPkt.mAssigneePlatformIndex);
         }
         else
         {
            assert(taskProcB_Ptr != nullptr);
            platformB_Ptr = taskProcB_Ptr->GetPlatform();
         }

         if (platformB_Ptr != nullptr && trackPtr != nullptr)
         {
            taskProcA_Ptr->AssignTask(simTime, *trackPtr, aPkt.mTaskType, *resourcePtr, platformB_Ptr, aPkt.mCommName);
         }
      }
      else if (aPkt.mCommandType == WsfXIO_TaskCommandPkt::cTASK_CANCEL)
      {
         taskProcA_Ptr->CancelTask(simTime, aPkt.mTrackId, aPkt.mTaskType, *resourcePtr, aPkt.mAssigneePlatformIndex);
      }
      else if (aPkt.mCommandType == WsfXIO_TaskCommandPkt::cTASK_COMPLETE)
      {
         taskProcA_Ptr->ReportTaskComplete(simTime, aPkt.mTrackId, aPkt.mTaskType, nullptr, aPkt.mStatus);
      }
      else if (aPkt.mCommandType == WsfXIO_TaskCommandPkt::cTASK_PROGRESS)
      {
         taskProcA_Ptr->ReportTaskProgress(simTime, aPkt.mTrackId, aPkt.mTaskType, nullptr, aPkt.mStatus);
      }
      else if (aPkt.mCommandType == WsfXIO_TaskCommandPkt::cCHANGE_OPERATING_LEVEL)
      {
         WsfPlatform* platformB_Ptr = nullptr;
         if (aPkt.mAssigneePlatformIndex == 0)
         {
            platformB_Ptr = mSimulation.GetPlatformByIndex(aPkt.mAssigneePlatformIndex);
         }
         else
         {
            assert(taskProcB_Ptr != nullptr);
            platformB_Ptr = taskProcB_Ptr->GetPlatform();
         }
         taskProcA_Ptr->ChangeOperatingLevel(simTime, aPkt.mTaskType, aPkt.mOperatingLevel, platformB_Ptr, aPkt.mCommName);
      }
      else
      {
         // Not handled explicitly above, so try the components.
         const WsfXIO_ComponentList& components(mXIO_Simulation);
         for (WsfXIO_ComponentList::Iterator it = components.Begin(); it != components.End(); ++it)
         {
            if ((*it)->HandleTaskCommand(simTime, aPkt, taskProcA_Ptr, taskProcA_Ptr, trackPtr, resourcePtr))
            {
               break;
            }
         }
      }
   }
}

//! Find a task processor on a platform.
//! @param aSimulation      The simulation containing the platform.
//! @param aPlatformIndex   The index of the platform to be searched.
//! @param aProcessorNameId The name of the task processor on the platform.
//! @returns Pointer to the task processor, or nullptr if it could not be found.
// static
WsfTaskProcessor* WsfXIO_TaskService::FindTaskProcessor(WsfSimulation& aSimulation,
                                                        size_t         aPlatformIndex,
                                                        WsfStringId&   aProcessorNameId)
{
   WsfTaskProcessor* taskProcessorPtr = nullptr;
   WsfPlatform*      platformPtr      = aSimulation.GetPlatformByIndex(aPlatformIndex);
   if (platformPtr != nullptr)
   {
      if (aProcessorNameId != 0)
      {
         WsfProcessor* processorPtr = platformPtr->GetComponent<WsfProcessor>(aProcessorNameId);
         if (processorPtr != nullptr)
         {
            taskProcessorPtr = dynamic_cast<WsfTaskProcessor*>(processorPtr);
         }
      }
      else
      {
         for (WsfComponentList::RoleIterator<WsfProcessor> iter(*platformPtr); !iter.AtEnd(); ++iter)
         {
            WsfProcessor* processorPtr = *iter;
            taskProcessorPtr           = dynamic_cast<WsfTaskProcessor*>(processorPtr);
         }
      }
   }
   return taskProcessorPtr;
}
