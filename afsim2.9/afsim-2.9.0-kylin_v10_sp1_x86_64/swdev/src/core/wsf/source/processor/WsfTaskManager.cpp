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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfTaskManager.hpp"

#include <algorithm>
#include <limits>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtScriptContext.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfAssociationMessage.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommUtil.hpp"
#include "WsfCommandChain.hpp"
#include "WsfEvent.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfProcessorObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfStringTable.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTaskCancelMessage.hpp"
#include "WsfTaskControlMessage.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTimeDelayQueue.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackProcessor.hpp"
#include "script/WsfScriptContext.hpp"

// TODO need to clear PendingMessageList on TurnOff.

// Notes about the 'PurgedTaskList'.
//
// The 'purged task list' is used to keep a record of tasks that have been purged on either end.
// This list is used to prevent the issuance of spurious warning messages that result from the
// assigner canceling a task and the assignee reporting the task complete at about the same time.
// Because of comm delays, the assignee may report a task complete that the assigner has already
// purged. At the current time this list is never cleaned up, but the entries are very small and
// there shouldn't be very many...
//
// In theory the assignee could purge the entry when it sees the task cancel message and the
// assigner could purge the entry when it sees the task complete message.

namespace
{

// ==============================================================================================
//! An event that is used to check for and retry unsuccessful message transmissions.
class CheckMessageEvent : public WsfTaskManager::EventBase
{
public:
   CheckMessageEvent(double aSimTime, WsfTaskManager* aManagerPtr, unsigned int aSerialNumber)
      : EventBase(aSimTime, aManagerPtr)
      , mSerialNumber(aSerialNumber)
   {
   }

   EventDisposition ExecuteEvent(WsfTaskManager* aManagerPtr) override
   {
      aManagerPtr->CheckMessageAcknowledged(GetTime(), mSerialNumber);
      return cDELETE;
   }

private:
   unsigned int mSerialNumber;
};

// ==============================================================================================
//! An event to finish a delay task.
class DelayCompleteEvent : public WsfTaskManager::EventBase
{
public:
   DelayCompleteEvent(double aSimTime, WsfTaskManager* aManagerPtr, const WsfTask& aTask)
      : EventBase(aSimTime, aManagerPtr)
      , mTask(aTask)
   {
   }

   EventDisposition ExecuteEvent(WsfTaskManager* aManagerPtr) override
   {
      aManagerPtr->ReportTaskComplete(GetTime(), mTask.GetTrackId(), mTask.GetTaskType(), mTask.GetResourceName(), nullptr);
      return cDELETE;
   }

private:
   WsfTask mTask;
};

// ==============================================================================================
//! A request to recognize a message.
class MessageRequest : public WsfTimeDelayQueue::Request
{
public:
   MessageRequest(double aTimeRequired, WsfTaskManager* aManagerPtr, const WsfMessage& aMessage)
      : WsfTimeDelayQueue::Request(aTimeRequired)
      , mManagerPtr(aManagerPtr)
      , mMessagePtr(aMessage.Clone())
   {
   }

   bool CompleteRequest(double aSimTime) override
   {
      mManagerPtr->ProcessMessage(aSimTime, *mMessagePtr);
      delete mMessagePtr;
      return true;
   }

   WsfTaskManager* mManagerPtr;
   WsfMessage*     mMessagePtr;
};

// ==============================================================================================
//! Returns true if the platform is in the platform list.
bool PlatformListContains(const WsfCommandChain::PlatformList& aPlatformList, const WsfPlatform* aPlatformPtr)
{
   return std::find(aPlatformList.begin(), aPlatformList.end(), aPlatformPtr) != aPlatformList.end();
}
} // End anonymous namespace

// =================================================================================================
//! Constructor
WsfTaskManager::WsfTaskManager(WsfScenario& aScenario, const std::string& aClassName /* = "WsfTaskManager" */)
   : WsfScriptProcessor(aScenario, aClassName, "PROCESSOR")
   , OnTaskAssigned()
   , OnTaskCanceled()
   , OnTaskCompleted()
   , OnTaskAssignSent()
   , OnTaskCompleteSent()
   , OnTaskCancelSent()
   , mTrackManagerPtr(nullptr)
   , mThinkerPtr(new WsfTimeDelayQueue())
   , mPendingMessageList()
   , mActiveTrackList()
   , mSensorRequestList()
   , mOperatingLevels()
   , mSensorsTurnedOn()
   , mScenarioPtr(&aScenario)
   , mCallbacks()
   , mXmtdTaskList()
   , mPurgedXmtdTaskList()
   , mRcvdTaskList()
   , mPurgedRcvdTaskList()
   , mPendingRcvdTasks()
   , mTrackProcessorName()
   , mTrackUpdateInterval(0.0)
   , mTimeToRecognizeMessage(0.0, WsfRandomVariable::cNON_NEGATIVE)
   , mCommRetryInterval(60.0)
   , mCommRetryAttempts(0)
   , mNextTaskId(0)
   , mShowTaskMessages(false)
   , mShowUncompletedTasks(false)
   , mTrackUpdateStrategy(cDEFAULT)
   , mTimeLastTaskAssigned(-1.0)
   , mTimeLastTaskReceived(-1.0)
   , mOnTaskAssignScriptPtr(nullptr)
   , mOnTaskCancelScriptPtr(nullptr)
   , mOnTaskCompleteScriptPtr(nullptr)
   , mOnTaskCompleteSentScriptPtr(nullptr)
   , mOnTaskAssignSentScriptPtr(nullptr)
   , mOnTaskCancelSentScriptPtr(nullptr)
   , mOnOperatingLevelChangeScriptPtr(nullptr)
   , mTrackVarPtr(nullptr)
   , mTaskClassPtr(nullptr)
   , mTrackClassPtr(nullptr)
{
   mTrackVarPtr = mContextPtr->GetContext().RegisterVariable("TRACK", "WsfLocalTrack");
}

// =================================================================================================
//! Copy constructor
WsfTaskManager::WsfTaskManager(const WsfTaskManager& aSrc)
   : WsfScriptProcessor(aSrc)
   , OnTaskAssigned()
   , OnTaskCanceled()
   , OnTaskCompleted()
   , OnTaskAssignSent()
   , OnTaskCompleteSent()
   , OnTaskCancelSent()
   , mTrackManagerPtr(nullptr)
   , mThinkerPtr(new WsfTimeDelayQueue(*aSrc.mThinkerPtr))
   , mPendingMessageList()
   , mActiveTrackList()
   , mSensorRequestList()
   , mOperatingLevels(aSrc.mOperatingLevels)
   , mSensorsTurnedOn()
   , mScenarioPtr(aSrc.mScenarioPtr)
   , mCallbacks()
   , mXmtdTaskList()
   , mPurgedXmtdTaskList()
   , mRcvdTaskList()
   , mPurgedRcvdTaskList()
   , mPendingRcvdTasks()
   , mTrackProcessorName(aSrc.mTrackProcessorName)
   , mTrackUpdateInterval(aSrc.mTrackUpdateInterval)
   , mTimeToRecognizeMessage(aSrc.mTimeToRecognizeMessage)
   , mCommRetryInterval(aSrc.mCommRetryInterval)
   , mCommRetryAttempts(aSrc.mCommRetryAttempts)
   , mNextTaskId(0)
   , mShowTaskMessages(aSrc.mShowTaskMessages)
   , mShowUncompletedTasks(aSrc.mShowUncompletedTasks)
   , mTrackUpdateStrategy(aSrc.mTrackUpdateStrategy)
   , mTimeLastTaskAssigned(-1.0)
   , mTimeLastTaskReceived(-1.0)
   , mOnTaskAssignScriptPtr(nullptr)
   , mOnTaskCancelScriptPtr(nullptr)
   , mOnTaskCompleteScriptPtr(nullptr)
   , mOnTaskCompleteSentScriptPtr(nullptr)
   , mOnTaskAssignSentScriptPtr(nullptr)
   , mOnTaskCancelSentScriptPtr(nullptr)
   , mOnOperatingLevelChangeScriptPtr(nullptr)
   , mTrackVarPtr(nullptr)
   , mTaskClassPtr(nullptr)
   , mTrackClassPtr(nullptr)
{
   mTrackVarPtr = mContextPtr->GetContext().RegisterVariable("TRACK", "WsfLocalTrack");
}

// =================================================================================================
// virtual
const char* WsfTaskManager::GetScriptClassName() const
{
   return "WsfTaskManager";
}

// =================================================================================================
//! A virtual copy constructor.
// virtual
WsfProcessor* WsfTaskManager::Clone() const
{
   return new WsfTaskManager(*this);
}

const int* WsfTaskManager::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_TASK_MANAGER,
                               cWSF_COMPONENT_PROCESSOR,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

void* WsfTaskManager::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_TASK_MANAGER)
   {
      return this;
   }
   return WsfScriptProcessor::QueryInterface(aRole);
}

// ================================================================================================
// virtual
bool WsfTaskManager::PreInitialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::PreInitialize(aSimTime);
   return ok;
}

// =================================================================================================
// virtual
bool WsfTaskManager::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);
   ok &= mThinkerPtr->Initialize(aSimTime, GetPlatform(), this, &GetScriptContext());
   return ok;
}

// =================================================================================================
// Most initialization is deferred to phase 2 because we may need to get the track manager of of a
// different processor, and it may not yet be initialized.
// virtual
bool WsfTaskManager::Initialize2(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize2(aSimTime);

   // If a specific track processor was specified, use its track manager as the source of tracks.
   mTrackManagerPtr = &(GetPlatform()->GetTrackManager()); // Assume the master track manager
   if (!mTrackProcessorName.IsNull())
   {
      WsfProcessor* procPtr = GetPlatform()->GetComponent<WsfProcessor>(mTrackProcessorName);
      if (procPtr != nullptr)
      {
         WsfTrackProcessor* trkProcPtr = dynamic_cast<WsfTrackProcessor*>(procPtr);
         if (trkProcPtr != nullptr)
         {
            mTrackManagerPtr = &(trkProcPtr->GetTrackManager());
         }
         else
         {
            auto out = ut::log::error() << "Track processor must be of type WSF_TRACK_PROCESSOR.";
            out.AddNote() << "Processor: " << mTrackProcessorName;
            ok = false;
         }
      }
      else
      {
         auto out = ut::log::error() << "Track processor could not be found.";
         out.AddNote() << "Processor: " << mTrackProcessorName;
         ok = false;
      }
   }

   // Subscribe to track manager events of interest
   mCallbacks.Add(mTrackManagerPtr->LocalTrackDropped.Connect(&WsfTaskManager::LocalTrackDropped, this));
   mCallbacks.Add(mTrackManagerPtr->LocalTrackInitiated.Connect(&WsfTaskManager::LocalTrackInitiated, this));
   mCallbacks.Add(mTrackManagerPtr->LocalTrackUpdated.Connect(&WsfTaskManager::LocalTrackUpdated, this));
   mCallbacks.Add(mTrackManagerPtr->TrackManagerChanged.Connect(&WsfTaskManager::TrackManagerChanged, this));

   mShowTaskMessages |= DebugEnabled();
   mShowUncompletedTasks |= DebugEnabled();

   // Capture pointers to the various 'on_' scripts
   mOnTaskAssignScriptPtr = mContextPtr->FindScript("on_task_assign");
   ok &= mContextPtr->ValidateScript(mOnTaskAssignScriptPtr, "void", "WsfTask, WsfTrack");
   mOnTaskAssignSentScriptPtr = mContextPtr->FindScript("on_task_assign_sent");
   ok &= mContextPtr->ValidateScript(mOnTaskAssignSentScriptPtr, "void", "WsfTask, WsfTrack");
   mOnTaskCancelScriptPtr = mContextPtr->FindScript("on_task_cancel");
   ok &= mContextPtr->ValidateScript(mOnTaskCancelScriptPtr, "void", "WsfTask");
   mOnTaskCancelSentScriptPtr = mContextPtr->FindScript("on_task_cancel_sent");
   ok &= mContextPtr->ValidateScript(mOnTaskCancelSentScriptPtr, "void", "WsfTask");
   mOnTaskCompleteScriptPtr = mContextPtr->FindScript("on_task_complete");
   ok &= mContextPtr->ValidateScript(mOnTaskCompleteScriptPtr, "void", "WsfTask");
   mOnTaskCompleteSentScriptPtr = mContextPtr->FindScript("on_task_complete_sent");
   ok &= mContextPtr->ValidateScript(mOnTaskCompleteSentScriptPtr, "void", "WsfTask");
   mOnOperatingLevelChangeScriptPtr = mContextPtr->FindScript("on_operating_level_change");
   ok &= mContextPtr->ValidateScript(mOnOperatingLevelChangeScriptPtr, "void", "string");

   mTaskClassPtr = mContextPtr->GetClass("WsfTask");
   ok &= (mTaskClassPtr != nullptr);
   mTrackClassPtr = mContextPtr->GetClass("WsfTrack");
   ok &= (mTrackClassPtr != nullptr);

   return ok;
}

// =================================================================================================
// virtual
bool WsfTaskManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (WsfScriptProcessor::ProcessInput(aInput))
   {
   }
   else if (command == "comm_retry_attempts")
   {
      aInput.ReadValue(mCommRetryAttempts);
      aInput.ValueGreaterOrEqual(mCommRetryAttempts, 0);
   }
   else if (command == "comm_retry_interval")
   {
      aInput.ReadValueOfType(mCommRetryInterval, UtInput::cTIME);
      aInput.ValueGreater(mCommRetryInterval, 0.0);
   }
   else if ((command == "operating_level") || (command == "autonomy_level")) // For backward compatibility
   {
      std::string name;
      int         level;
      aInput.ReadValue(name);
      aInput.ReadValue(level);
      aInput.ValueGreaterOrEqual(level, 0);
      mOperatingLevels[name] = level;
   }
   else if (command == "track_processor")
   {
      std::string name;
      aInput.ReadValue(name);
      mTrackProcessorName = name;
   }
   else if (command == "time_to_recognize_messages")
   {
      mTimeToRecognizeMessage.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "track_update_interval")
   {
      aInput.ReadValueOfType(mTrackUpdateInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mTrackUpdateInterval, 0.0);
   }
   else if (command == "track_update_strategy")
   {
      std::string strategy;
      aInput.ReadValue(strategy);
      if (strategy == "default")
      {
         mTrackUpdateStrategy = cDEFAULT;
      }
      else if (strategy == "batch")
      {
         mTrackUpdateStrategy = cBATCH;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "show_task_messages")
   {
      mShowTaskMessages = true;
   }
   else if (command == "show_uncompleted_tasks")
   {
      mShowUncompletedTasks = true;
   }
   else if (mThinkerPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfTaskManager::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // If the platform being deleted has been assigned tasks then delete the tasks.
   //
   // NOTE: This is cheating to some extent because in reality it may take some time before someone
   // really knows that someone else is dead. For now we'll just process it immediately and come back
   // and revisit this later.
   //
   // NOTE: This process of canceling is somewhat naive in that is doesn't consider task dependencies.
   // A task that doesn't get canceled may be relying on the task that did get canceled.

   auto xtli = mXmtdTaskList.begin();
   while (xtli != mXmtdTaskList.end())
   {
      if (xtli->GetAssigneePlatformIndex() == aPlatformPtr->GetIndex())
      {
         // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task
         //        list before proceeding. A temporary list is used to store the task before deletion.
         TaskList removedTaskList;
         removedTaskList.splice(removedTaskList.end(), mXmtdTaskList, xtli++);
         CancelTask(GetSimulation()->GetSimTime(), removedTaskList.front(), false);
      }
      else
      {
         ++xtli;
      }
   }

   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->PlatformDeleted(aPlatformPtr);
   }
}

// =================================================================================================
// NOTE: Even though this method derives from WsfScriptProcessor, it does NOT invoke its ProcessMethod to
//       give it a shot at the message. This is left to the final derived class (typically WsfTaskProcessor)
//       to call and/or accept/reject the fact that the callee has indicated that it processed the message.
// virtual
bool WsfTaskManager::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   WsfStringId           messageType = aMessage.GetType();
   const WsfStringTable& strings     = mScenarioPtr->Strings();
   if (messageType == strings.cWSF_TRACK_MESSAGE)
   {
      ProcessTrackMessage(aSimTime, dynamic_cast<const WsfTrackMessage&>(aMessage));
   }
   else if (messageType == strings.cWSF_DROP_TRACK_MESSAGE)
   {
      ProcessTrackDropMessage(aSimTime, dynamic_cast<const WsfTrackDropMessage&>(aMessage));
   }
   else if (messageType == strings.cWSF_ASSOCIATION_MESSAGE)
   {
      ProcessAssociationMessage(aSimTime, dynamic_cast<const WsfAssociationMessage&>(aMessage));
   }
   else if (messageType == strings.cWSF_TASK_ASSIGN_MESSAGE)
   {
      ProcessTaskAssignMessage(aSimTime, dynamic_cast<const WsfTaskAssignMessage&>(aMessage));
   }
   else if (messageType == strings.cWSF_TASK_CANCEL_MESSAGE)
   {
      ProcessTaskCancelMessage(aSimTime, dynamic_cast<const WsfTaskCancelMessage&>(aMessage));
   }
   else if (messageType == strings.cWSF_TASK_CONTROL_MESSAGE)
   {
      ProcessTaskControlMessage(aSimTime, dynamic_cast<const WsfTaskControlMessage&>(aMessage));
   }
   else if (messageType == strings.cWSF_TASK_STATUS_MESSAGE)
   {
      ProcessTaskStatusMessage(aSimTime, dynamic_cast<const WsfTaskStatusMessage&>(aMessage));
   }
   // Even though we may process the message, we always return false to allow others to also do whatever
   // processing they require.
   return false;
}

// =================================================================================================
// virtual
bool WsfTaskManager::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   // If the message is a task-oriented message then make sure the message is intended for me.
   // (If sent through a broadcast medium then all parties will receive the message).

   bool        messageProcessed = false;
   bool        myTaskMessage    = false;
   bool        delayTaskMessage = true;
   WsfStringId messageType      = aMessage.GetType();

   if (messageType == mScenarioPtr->Strings().cWSF_TASK_ASSIGN_MESSAGE)
   {
      const WsfTaskAssignMessage& message = dynamic_cast<const WsfTaskAssignMessage&>(aMessage);
      if (GetPlatform()->GetIndex() == message.GetTask().GetAssigneePlatformIndex())
      {
         // If it is also directed to a specific processor then make sure this is the correct processor.
         if ((message.GetTask().GetAssigneeProcessorName() == nullptr) ||
             (message.GetTask().GetAssigneeProcessorName() == GetNameId()))
         {
            myTaskMessage = true;
         }
      }
   }
   else if (messageType == mScenarioPtr->Strings().cWSF_TASK_CANCEL_MESSAGE)
   {
      const WsfTaskCancelMessage& message = dynamic_cast<const WsfTaskCancelMessage&>(aMessage);
      if (GetPlatform()->GetIndex() == message.GetAssigneePlatformIndex())
      {
         // The message is intended for this platform. If it is also directed to a specific processor then
         // make sure this is the correct processor.
         if ((message.GetAssigneeProcessorName() == nullptr) || (message.GetAssigneeProcessorName() == GetNameId()))
         {
            myTaskMessage = true;
         }
      }
   }
   else if (messageType == mScenarioPtr->Strings().cWSF_TASK_CONTROL_MESSAGE)
   {
      const WsfTaskControlMessage& message = dynamic_cast<const WsfTaskControlMessage&>(aMessage);
      if (GetPlatform()->GetIndex() == message.GetAssigneePlatformIndex())
      {
         // The message is intended for this platform. If it is also directed to a specific processor then
         // make sure this is the correct processor.
         if ((message.GetAssigneeProcessorName() == nullptr) || (message.GetAssigneeProcessorName() == GetNameId()))
         {
            myTaskMessage = true;
         }
      }
   }
   else if (messageType == mScenarioPtr->Strings().cWSF_TASK_STATUS_MESSAGE)
   {
      // The status message is bi-directional so the recipient can be either the assigner or the assignee.
      const WsfTaskStatusMessage& message = dynamic_cast<const WsfTaskStatusMessage&>(aMessage);
      if (message.IsFromAssignee())
      {
         if (message.GetAssignerPlatformIndex() == GetPlatform()->GetIndex())
         {
            if ((message.GetAssignerProcessorName() == nullptr) || (message.GetAssignerProcessorName() == GetNameId()))
            {
               myTaskMessage = true;
            }
         }
      }
      else // Message is from assigner, so recipient should be assignee.
      {
         if (message.GetAssigneePlatformIndex() == GetPlatform()->GetIndex())
         {
            if ((message.GetAssigneeProcessorName() == nullptr) || (message.GetAssigneeProcessorName() == GetNameId()))
            {
               myTaskMessage = true;
            }
         }
      }
      // Acknowledgment messages are not delayed.
      delayTaskMessage = (message.GetAcknowledgeId() == 0);
   }
   else
   {
      messageProcessed = ProcessMessage(aSimTime, aMessage);
   }

   // If the message is one of my task messages then either process it directly (if the time_to_recongnize_messages
   // is zero) or queue it for a thinker to simulate the reaction time to the message.

   if (myTaskMessage)
   {
      double timeToRecognize = mTimeToRecognizeMessage.Draw();
      if (delayTaskMessage && (timeToRecognize > 0.0))
      {
         MessageRequest* requestPtr = new MessageRequest(timeToRecognize, this, aMessage);
         mThinkerPtr->SubmitRequest(aSimTime, requestPtr);
      }
      else
      {
         ProcessMessage(aSimTime, aMessage);
      }
      messageProcessed = true;
   }
   return messageProcessed;
}


// =================================================================================================
// virtual
void WsfTaskManager::TurnOff(double aSimTime)
{
   WsfScriptProcessor::TurnOff(aSimTime);
   mThinkerPtr->TurnOff(aSimTime);
}

// =================================================================================================
// virtual
void WsfTaskManager::TurnOn(double aSimTime)
{
   WsfScriptProcessor::TurnOn(aSimTime);
   mThinkerPtr->TurnOn(aSimTime);
}

// =================================================================================================
// virtual
void WsfTaskManager::Update(double aSimTime)
{
   WsfScriptProcessor::Update(aSimTime);
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->Update(aSimTime);
   }
}

// =================================================================================================
//! Assign a task.
//! @param aSimTime       The current simulation time.
//! @param aTrack         The track that represents the objective of the assignment.
//! @param aTask          The task to be assigned.
//! @param aLockTrack     'true' if the track should be locked to prevent deletion.
// virtual
bool WsfTaskManager::AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack)
{
   WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(aTask.GetAssigneePlatformIndex());
   if (assigneePtr == nullptr)
   {
      return false;
   }
   WsfStringId commName = SelectCommDevice(assigneePtr, aTask.GetCommName());
   if (mShowTaskMessages)
   {
      WsfStringId resourceName(aTask.GetResourceName());
      WsfStringId resourceType(aTask.GetResource().GetTypeName());

      auto out = ut::log::debug() << "Task manager assigning task via comm.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Target: " << aTrack.GetTargetName();
      out.AddNote() << "Track: " << aTrack.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Assignee: " << assigneePtr->GetName();
      out.AddNote() << "Resource: " << resourceName;
      out.AddNote() << "Resource Type: " << resourceType;
      out.AddNote() << "Comm: " << commName;
   }

   // Create a transmitted task list entry if one doesn't already exist.
   // If one already exists then we just reuse it.
   auto xtli =
      FindTask(mXmtdTaskList, assigneePtr->GetIndex(), aTrack.GetTrackId(), aTask.GetTaskType(), aTask.GetResourceName());
   if (xtli == mXmtdTaskList.end())
   {
      // Create the transmitted task list entry.
      WsfTask task(aTask);
      ++mNextTaskId;
      task.Initialize(aSimTime, mNextTaskId, this);
      task.SetTargetName(aTrack.GetTargetName());
      task.SetTrackId(aTrack.GetTrackId());
      task.SetLocalTrackId(aTrack.GetTrackId());
      task.SetCommName(commName);
      WsfTaskResource& resource = task.GetResource();
      WsfStringId      assigneeProcessorName;
      if ((resource.GetType() == cTASK_RESOURCE_TYPE_PROCESSOR) && (resource.mNameId != 0) &&
          (assigneePtr->GetComponent<WsfProcessor>(resource.mNameId) != nullptr))
      {
         // The selected resource is another processor (should be a task manager) on either this platform
         // or another platform.
         assigneeProcessorName = resource.mNameId;

         // If the assignment is to this platform then the comm name is the target processor. (See SendTaskMessage).
         if (assigneePtr == GetPlatform())
         {
            task.SetCommName(resource.mNameId);
         }
      }
      task.SetAssigneeProcessorName(assigneeProcessorName);

      mXmtdTaskList.push_front(task);
      xtli = mXmtdTaskList.begin();

      // Lock the local track use count to prevent it from getting purged by the track manager.
      if (aLockTrack && !aTrack.GetTrackId().IsNull())
      {
         mTrackManagerPtr->LockTrack(aSimTime, aTrack.GetTrackId());
      }
   }
   else // Resend the task...
   {
      WsfTask task(aTask);
      task.Initialize(aSimTime, xtli->GetTaskId(), this);
      task.SetTargetName(aTrack.GetTargetName());
      task.SetTrackId(aTrack.GetTrackId());
      task.SetLocalTrackId(aTrack.GetTrackId());
      task.SetCommName(commName);
      // overwrite the task in our transmitted list (update all the task data)
      *xtli = task;
   }

   // Send the assignment message to the assignee.

   WsfTask& task = *xtli;

   OnTaskAssignSent(aSimTime, task, aTrack);

   WsfObserver::TaskAssigned(GetSimulation())(aSimTime, &task, &aTrack);

   TaskAssignSent(aSimTime, aTrack, task); // protected virtual method (for any derived class processing)

   bool                 ok = true;
   WsfTaskAssignMessage message(GetPlatform());
   message.SetTask(task);
   message.SetTrack(aTrack);
   ok &= SendTaskMessage(aSimTime, message, task.GetAssigneePlatformIndex(), task.GetCommName(), true);
   mTimeLastTaskAssigned = aSimTime;

   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      ok &= i->AssignTask(aSimTime, aTrack, aTask, aLockTrack);
   }
   return ok;
}

// =================================================================================================
//! Assign a task.
//! @param aSimTime       The current simulation time.
//! @param aTrack         The track that represents the objective of the assignment.
//! @param aTaskType      The 'type' of the task.
//! @param aResource      The resource specification of the resource to be allocated.
//! @param aAssigneePtr   A pointer to the platform to be assigned to the task.
//! @param aCommName      The name (as a string ID) of the communications device to be used to send the
//!                       task assignment.
// virtual
bool WsfTaskManager::AssignTask(double                 aSimTime,
                                const WsfTrack&        aTrack,
                                WsfStringId            aTaskType,
                                const WsfTaskResource& aResource,
                                WsfPlatform*           aAssigneePtr,
                                WsfStringId            aCommName)
{
   if (aAssigneePtr)
   {
      WsfTask task(aResource);
      task.SetTaskType(aTaskType);
      task.SetAssignee(aAssigneePtr);
      task.SetCommName(aCommName);
      return AssignTask(aSimTime, aTrack, task);
   }
   return false;
}

// =================================================================================================
//                      Start of non-public methods
// =================================================================================================

// =================================================================================================
// protected virtual
bool WsfTaskManager::AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack)
{
   // Return immediately if no resource specified.
   WsfStringId resourceName(aTask.GetResourceName());
   if (resourceName.IsNull())
   {
      return true;
   }

   bool ok = true;

   // Check components
   bool componentAcquired = false;
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      bool statusOk;
      componentAcquired |= i->AcquireResource(aSimTime, aTask, aTrack, statusOk);
      ok &= statusOk;
   }

   WsfSensor* sensorPtr = nullptr;
   if (componentAcquired)
   {
   }
   else if ((sensorPtr = GetPlatform()->GetComponent<WsfSensor>(resourceName)) != nullptr)
   {
      WsfStringId modeNameId = aTask.GetResourceMode();
      if ((modeNameId != 0) && (sensorPtr->GetModeIndex(modeNameId) >= sensorPtr->GetModeCount()))
      {
         auto out = ut::log::warning() << "Invalid sensor mode requested for track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << sensorPtr->GetName();
         out.AddNote() << "Mode: " << modeNameId;
         out.AddNote() << "Track: " << aTrack.GetTrackId();
         ok = false;
      }
      if (ok)
      {
         if (!sensorPtr->IsTurnedOn())
         {
            GetSimulation()->TurnPartOn(aSimTime, sensorPtr);
            mSensorsTurnedOn.insert(sensorPtr);
         }
         // If this is a tracking sensor then initiate tracking.
         if (sensorPtr->GetMaximumRequestCount(modeNameId) > 0)
         {
            if (sensorPtr->StartTracking(aSimTime, aTrack, modeNameId))
            {
               UpdateSensorRequestList(aSimTime, aTrack.GetTrackId(), sensorPtr, modeNameId, true);
            }
            else
            {
               auto out = ut::log::warning() << "Sensor failed StartTracking for track.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Sensor: " << sensorPtr->GetName();
               out.AddNote() << "Track: " << aTrack.GetTrackId();
               ok = false;
            }
         }
      }
   }
   else if (GetPlatform()->GetComponent<WsfProcessor>(resourceName) == this)
   {
      // I am the resource! (A task processor has assigned a task to another task processor on the same platform.)
   }
   else
   {
      ok       = false;
      auto out = ut::log::error() << "Unknown resource type in AcquireResource.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Resource Type: " << resourceName;
   }
   return ok;
}

// =================================================================================================
// protected virtual
bool WsfTaskManager::ReleaseResource(double aSimTime, WsfTask& aTask)
{
   // Return immediately if no resource is associated with the task.
   WsfStringId resourceName(aTask.GetResourceName());
   if (resourceName.IsNull())
   {
      return false;
   }

   // Check components
   bool componentReleased = false;
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      componentReleased |= i->ReleaseResource(aSimTime, aTask);
   }

   bool       ok        = true;
   WsfSensor* sensorPtr = nullptr;
   if (componentReleased)
   {
   }
   else if ((sensorPtr = GetPlatform()->GetComponent<WsfSensor>(resourceName)) != nullptr)
   {
      // If this is a tracking sensor then cancel tracking on this track
      if ((!aTask.GetTrackId().IsNull()) && sensorPtr->HaveRequestFor(aTask.GetTrackId()))
      {
         // This is a little tricky. Some users will assign multiple tasks for the same track to the same
         // sensor. This makes book-keeping easy for them - but harder for us. We must make sure we don't
         // stop the sensor request if there is still other tasks assigned.

         bool haveOtherTasks = false;
         for (const WsfTask& task : mRcvdTaskList)
         {
            if (aTask.GetTaskId() != task.GetTaskId()) // Not the same task
            {
               if ((task.GetTrackId() == aTask.GetTrackId()) && (task.GetResourceType() == cTASK_RESOURCE_TYPE_SENSOR) &&
                   (task.GetResourceName() == aTask.GetResourceName()))
               {
                  haveOtherTasks = true;
                  break;
               }
            }
         }

         if (!haveOtherTasks)
         {
            if (!sensorPtr->StopTracking(aSimTime, aTask.GetTrackId()))
            {
               auto out = ut::log::warning() << "Sensor failed StopTracking for track.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Sensor: " << sensorPtr->GetName();
               out.AddNote() << "Track: " << aTask.GetTrackId();
            }
            UpdateSensorRequestList(aSimTime, aTask.GetTrackId(), sensorPtr, nullptr, false);
         }
         if ((sensorPtr->GetActiveRequestCount() == 0) && (mSensorsTurnedOn.find(sensorPtr) != mSensorsTurnedOn.end()))
         {
            GetSimulation()->TurnPartOff(aSimTime, sensorPtr);
            mSensorsTurnedOn.erase(sensorPtr);
         }
      }
   }
   else if (GetPlatform()->GetComponent<WsfProcessor>(resourceName) == this)
   {
      // I am the resource! (A task processor has assigned a task to another task processor on the same platform.)
   }
   else
   {
      auto out = ut::log::error() << "Unknown resource type in ReleaseResource.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Resource Type: " << resourceName;
      ok = false;
   }
   return ok;
}

// =================================================================================================
// virtual
void WsfTaskManager::TaskCanceled(double aSimTime, WsfTask& aTask)
{
   // Invoke the user-specified on_task_cancel script
   if (!IsExternallyControlled() && mOnTaskCancelScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.emplace_back(new UtScriptRef(&aTask, mTaskClassPtr));
      mContextPtr->ExecuteScript(aSimTime, mOnTaskCancelScriptPtr, retVal, argList);
   }

   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->TaskCanceled(aSimTime, aTask);
   }
}

// =================================================================================================
// virtual
void WsfTaskManager::TaskCompleted(double aSimTime, WsfTask& aTask)
{
   // Invoke the user-specified on_task_complete script
   if (!IsExternallyControlled() && mOnTaskCompleteScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.emplace_back(new UtScriptRef(&aTask, mTaskClassPtr));
      mContextPtr->ExecuteScript(aSimTime, mOnTaskCompleteScriptPtr, retVal, argList);
   }

   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->TaskCompleted(aSimTime, aTask);
   }
}

// =================================================================================================
// virtual
void WsfTaskManager::TaskAssignSent(double aSimTime, const WsfTrack& aTrack, WsfTask& aTask)
{
   // Invoke the user-specified on_task_assign_sent script
   if (!IsExternallyControlled() && mOnTaskAssignSentScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.emplace_back(new UtScriptRef(&aTask, mTaskClassPtr));
      argList.emplace_back(new UtScriptRef(const_cast<WsfTrack*>(&aTrack), mTrackClassPtr));
      mContextPtr->ExecuteScript(aSimTime, mOnTaskAssignSentScriptPtr, retVal, argList);
   }
}

// =================================================================================================
// virtual
void WsfTaskManager::TaskAssigned(double aSimTime, const WsfTrack& aTrack, WsfTask& aTask)
{
   // Invoke the user-specified on_task_assign script
   if (!IsExternallyControlled() && mOnTaskAssignScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.emplace_back(new UtScriptRef(&aTask, mTaskClassPtr));
      argList.emplace_back(new UtScriptRef(const_cast<WsfTrack*>(&aTrack), mTrackClassPtr));
      mContextPtr->ExecuteScript(aSimTime, mOnTaskAssignScriptPtr, retVal, argList);
   }
}

// =================================================================================================
// virtual
void WsfTaskManager::OperatingLevelChanged(double aSimTime, std::string aLevel)
{
   if (!IsExternallyControlled() && mOnOperatingLevelChangeScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.emplace_back(aLevel);
      mContextPtr->ExecuteScript(aSimTime, mOnOperatingLevelChangeScriptPtr, retVal, argList);
   }
}

// =================================================================================================
// virtual
void WsfTaskManager::TrackDropped(double aSimTime, const WsfTrackId& aTrackId)
{
   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->TrackDropped(aSimTime, aTrackId);
   }
}

// =================================================================================================
//! Drop an entry from the purged task list.
//! This is called when the conditions have been met that allow an entry to be deleted from the purged task list
//! (i.e. receipt of a task cancel/completion or the corresponding acknowledgment of a task cancel/completion.
//! @param aPurgedTaskList Either the transmitted or received purged task list.
//! @param aAssignerIndex  The platform index of the task assigner.
//! @param aTaskId         The assigners task ID.
//! @returns true if found and dropped from the purged task list or 'false' if an entry does not exist.
// private
bool WsfTaskManager::DropFromPurgedTaskList(TaskList& aPurgedTaskList, size_t aAssignerIndex, unsigned int aTaskId)
{
   bool droppedFromList = false;
   for (TaskList::iterator ptli = aPurgedTaskList.begin(); ptli != aPurgedTaskList.end(); ++ptli)
   {
      WsfTask& task = (*ptli);
      if ((aAssignerIndex == task.GetAssignerPlatformIndex()) && (aTaskId == task.GetTaskId()))
      {
         droppedFromList = true;
         aPurgedTaskList.erase(ptli);
         break;
      }
   }

   // If the caller is waiting on a message related to this event then cancel the watchdog entry.
   if (droppedFromList)
   {
      for (PendingMessageList::iterator pmli = mPendingMessageList.begin(); pmli != mPendingMessageList.end(); ++pmli)
      {
         PendingMessage& pendingMessage = *pmli;
         bool            purgeMessage   = false;
         if (pendingMessage.mMessagePtr->GetType() == WsfTaskCancelMessage::GetTypeId())
         {
            WsfTaskCancelMessage* messagePtr = dynamic_cast<WsfTaskCancelMessage*>(pendingMessage.mMessagePtr);
            if ((aAssignerIndex == messagePtr->GetAssignerPlatformIndex()) && (aTaskId == messagePtr->GetTaskId()))
            {
               purgeMessage = true;
            }
         }
         else if (pendingMessage.mMessagePtr->GetType() == WsfTaskStatusMessage::GetTypeId())
         {
            WsfTaskStatusMessage* messagePtr = dynamic_cast<WsfTaskStatusMessage*>(pendingMessage.mMessagePtr);
            if ((aAssignerIndex == messagePtr->GetAssignerPlatformIndex()) && (aTaskId == messagePtr->GetTaskId()))
            {
               purgeMessage = true;
            }
         }
         if (purgeMessage)
         {
            mPendingMessageList.erase(pmli);
            break;
         }
      }
   }
   return droppedFromList;
}

// =================================================================================================
//! Cancel tasks and subtasks.
//! @param aSimTime       The current simulation time.
//! @param aTaskId        The task ID of the task to be canceled.
// virtual
bool WsfTaskManager::CancelTask(double aSimTime, unsigned int aTaskId)
{
   auto iter = FindTask(mXmtdTaskList, aTaskId);
   if (iter != mXmtdTaskList.end())
   {
      CancelTask(aSimTime, *iter, true);
      return true;
   }
   return false;
}

// =================================================================================================
//! An internal routine to cancel a task that we have assigned.
//! This routine sends the cancel message for the indicated task.
//! @note The caller is responsible for removing the task from the task list.
//! @param aSimTime           The current simulation time.
//! @param aTask              The task to be canceled.
//! @param aSendCancelMessage 'true' if a cancel message should be sent to the assignee.
// private
void WsfTaskManager::CancelTask(double aSimTime, WsfTask& aTask, bool aSendCancelMessage)
{
   // Invoke the user-specified on_task_cancel_sent script
   if (mOnTaskCancelSentScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.emplace_back(new UtScriptRef(&aTask, mTaskClassPtr));
      mContextPtr->ExecuteScript(aSimTime, mOnTaskCancelSentScriptPtr, retVal, argList);
   }

   if (mShowTaskMessages)
   {
      WsfStringId resourceName(aTask.GetResourceName());
      WsfStringId resourceType(aTask.GetResource().GetTypeName());
      auto        out = ut::log::debug() << "Canceling task on assignee via comm.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Target: " << aTask.GetTargetName();
      out.AddNote() << "Track ID: " << aTask.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Assignee: " << aTask.GetAssigneePlatformName();
      out.AddNote() << "Resource Name: " << resourceName;
      out.AddNote() << "Resource Type: " << resourceType;
      out.AddNote() << "Comm: " << aTask.GetCommName();
   }

   PurgeTransmittedTask(aSimTime, aTask);

   OnTaskCancelSent(aSimTime, aTask);

   WsfObserver::TaskCanceled(GetSimulation())(aSimTime, &aTask);

   // Send the cancellation message to the assignee.
   // A message is not sent if this is the result of noticing the death of the assignee.
   if (aSendCancelMessage)
   {
      mPurgedXmtdTaskList.push_back(aTask);
      WsfTaskCancelMessage message(GetPlatform(), aTask);
      SendTaskMessage(aSimTime, message, aTask.GetAssigneePlatformIndex(), aTask.GetCommName(), true);
   }
}

// =================================================================================================
//! Cancel tasks and subtasks.
//! @param aSimTime       The current simulation time.
//! @param aTrackId       The track ID of the task(s) to be canceled.
//! @param aTaskType      The task type of the tasks to be canceled.
//!                       If zero then the task type is not a selection criteria.
//! @param aResource      The associated resource of the task to be canceled.
//!                       If a resource name is not specified it is not a selection criteria.
//! @param aAssigneeIndex The platform index to which the task is assigned.
//!                       If zero then the assignee is not a selection criteria.
// virtual
bool WsfTaskManager::CancelTask(double                 aSimTime,
                                const WsfTrackId&      aTrackId,
                                WsfStringId            aTaskType,
                                const WsfTaskResource& aResource,
                                size_t                 aAssigneeIndex)
{
   // Find and cancel the selected tasks.
   bool taskCanceled = false;
   auto xtli         = mXmtdTaskList.begin();
   while (xtli != mXmtdTaskList.end())
   {
      WsfTask& task              = *xtli;
      bool     trackIdCheck      = (aTrackId == task.GetTrackId());
      bool     localTrackIdCheck = (aTrackId == task.GetLocalTrackId());
      bool     taskTypeCheck     = (aTaskType == task.GetTaskType());
      bool     assigneeCheck     = (aAssigneeIndex == task.GetAssigneePlatformIndex());
      if ((aTrackId.IsNull() || (trackIdCheck || localTrackIdCheck)) && ((aTaskType == nullptr) || taskTypeCheck) &&
          ((aAssigneeIndex == 0) || assigneeCheck) && (task.GetResource().IsMatch(aResource)))
      {
         // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task list before
         // proceeding.
         //        A temporary list is used to store the task before deletion.
         TaskList removedTaskList;
         removedTaskList.splice(removedTaskList.end(), mXmtdTaskList, xtli++);

         CancelTask(aSimTime, removedTaskList.front(), true);
         taskCanceled = true;
      }
      else
      {
         ++xtli;
      }
   }
   return taskCanceled;
}

// =================================================================================================
//! Create and Assign a delay task.
//! A delay task is a pseudo-task that simply reports complete after a specified time interval.
//! has elapsed.
//! @param aSimTime       The current simulation time.
//! @param aDelayTime     The amount of time to delay.
//! @param aTrack         The track that represents the objective of the assignment.
//! In theory this could be a track ID, but we want this to look like an AssignTask request.
//! @param aTaskType      The 'type' of the task.
bool WsfTaskManager::CreateDelayTask(double aSimTime, double aDelayTime, const WsfTrack& aTrack, WsfStringId aTaskType)
{
   const WsfTrackId& trackId = aTrack.GetTrackId();
   WsfStringId       resourceName;

   // Make sure a task with the same id/type/resource does not exist on either the sent or received task list.
   auto xtli = FindTask(mXmtdTaskList, GetPlatform()->GetIndex(), trackId, aTaskType, resourceName);
   auto rtli = FindTask(mRcvdTaskList, GetPlatform()->GetIndex(), trackId, aTaskType, resourceName);
   if ((xtli != mXmtdTaskList.end()) || (rtli != mRcvdTaskList.end()))
   {
      return false;
   }

   if (!AssignTask(aSimTime, aTrack, aTaskType, WsfTaskResource(), GetPlatform(), WsfStringId()))
   {
      return false;
   }

   // Schedule the event to complete the task at the required time.
   xtli = FindTask(mXmtdTaskList, GetPlatform()->GetIndex(), trackId, aTaskType, resourceName);
   GetSimulation()->AddEvent(ut::make_unique<DelayCompleteEvent>(aSimTime + aDelayTime, this, *xtli));
   return true;
}

// =================================================================================================
//! Change the operating level for a given subordinate.
bool WsfTaskManager::ChangeOperatingLevel(double       aSimTime,
                                          WsfStringId  aName,
                                          int          aLevel,
                                          WsfPlatform* aAssigneePtr,
                                          WsfStringId  aCommName)
{
   if (aAssigneePtr)
   {
      WsfStringId commName = SelectCommDevice(aAssigneePtr, aCommName);
      if (mShowTaskMessages)
      {
         auto out = ut::log::debug() << "Changing operating level for subordinate via comm.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Subordinate: " << aName;
         out.AddNote() << "Task Level: " << aLevel;
         out.AddNote() << "Assignee: " << aAssigneePtr->GetName();
         out.AddNote() << "Comm: " << commName;
      }
      WsfTaskControlMessage message(this, aAssigneePtr);
      message.SetName(aName);
      message.SetLevel(aLevel);
      return SendTaskMessage(aSimTime, message, aAssigneePtr->GetIndex(), commName, false); // TODO - maybe reliable??
   }
   return false;
}


// =================================================================================================
// virtual
bool WsfTaskManager::ReportTaskComplete(double            aSimTime,
                                        const WsfTrackId& aTrackId,
                                        WsfStringId       aTaskType,
                                        WsfStringId       aResourceName,
                                        WsfStringId       aSubStatus)
{
   WsfStringId subStatus = aSubStatus;
   if (!subStatus)
   {
      subStatus = mScenarioPtr->Strings().cSUCCESSFUL;
   }

   bool ok   = true;
   auto rtli = FindTask(mRcvdTaskList, GetPlatform()->GetIndex(), aTrackId, aTaskType, aResourceName);
   if (rtli != mRcvdTaskList.end())
   {
      // NOTE - In order to avoid problems in callbacks, the task is removed from the received task list before proceeding.
      //        splice() is used to move the task to the front of mPurgedRcvdTaskList without deletion.
      mPurgedRcvdTaskList.splice(mPurgedRcvdTaskList.begin(), mRcvdTaskList, rtli);
      WsfTask& taskRef = mPurgedRcvdTaskList.front();
      if (mShowTaskMessages)
      {
         WsfStringId resourceName(taskRef.GetResourceName());
         WsfStringId resourceType(taskRef.GetResource().GetTypeName());
         auto        out = ut::log::info() << "Reporting completion status for task.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Status: " << subStatus;
         out.AddNote() << "Target: " << taskRef.GetTargetName();
         out.AddNote() << "Track ID: " << taskRef.GetTrackId();
         out.AddNote() << "Task Type: " << taskRef.GetTaskType();
         out.AddNote() << "Resource Name: " << resourceName;
         out.AddNote() << "Resource Type: " << resourceType;
      }

      OnTaskCompleteSent(aSimTime, taskRef, subStatus);

      WsfObserver::TaskCompleted(GetSimulation())(aSimTime, &taskRef, subStatus);

      // Invoke the user-specified on_task_complete_sent script
      if (!IsExternallyControlled() && mOnTaskCompleteSentScriptPtr != nullptr)
      {
         UtScriptData     retVal;
         UtScriptDataList argList;
         argList.emplace_back(new UtScriptRef(&taskRef, mTaskClassPtr));
         mContextPtr->ExecuteScript(aSimTime, mOnTaskCompleteSentScriptPtr, retVal, argList);
      }

      // Use a copy of the task so that it remains valid after removal from mPurgedRcvdTaskList.
      WsfTask taskCopy = taskRef;

      WsfTaskStatusMessage message("COMPLETED", GetPlatform(), taskCopy);
      message.SetSubStatus(subStatus);
      ok &= SendTaskMessage(aSimTime, message, taskCopy.GetAssignerPlatformIndex(), taskCopy.GetCommName(), true);

      PurgeReceivedTask(aSimTime, taskCopy);
   }
   else
   {
      ok = false;
      if (FindTask(mPurgedRcvdTaskList, GetPlatform()->GetIndex(), aTrackId, aTaskType, aResourceName) !=
          mPurgedRcvdTaskList.end())
      {
         auto out = ut::log::warning() << "Reporting completion status for task.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Status: " << subStatus;
         out.AddNote() << "Track: " << aTrackId;
         out.AddNote() << "Assignee: " << GetPlatform()->GetNameId();
         out.AddNote() << "Resource: " << aResourceName;
         out.AddNote() << "Could not find task.";
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfTaskManager::ReportTaskProgress(double            aSimTime,
                                        const WsfTrackId& aTrackId,
                                        WsfStringId       aTaskType,
                                        WsfStringId       aResourceName,
                                        WsfStringId       aSubStatus)
{
   bool ok   = true;
   auto rtli = FindTask(mRcvdTaskList, GetPlatform()->GetIndex(), aTrackId, aTaskType, aResourceName);
   if (rtli != mRcvdTaskList.end())
   {
      WsfTask& task = *rtli;
      if (mShowTaskMessages)
      {
         WsfStringId resourceName(task.GetResourceName());
         WsfStringId resourceType(task.GetResource().GetTypeName());
         auto        out = ut::log::info() << "Reporting progress status for task.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Status: " << mScenarioPtr->Strings().cIN_PROGRESS;
         out.AddNote() << "Target: " << task.GetTargetName();
         out.AddNote() << "Track ID: " << task.GetTrackId();
         out.AddNote() << "Task Type: " << task.GetTaskType();
         out.AddNote() << "Resource Name: " << resourceName;
         out.AddNote() << "Resource Type: " << resourceType;
      }

      WsfTaskStatusMessage message(mScenarioPtr->Strings().cIN_PROGRESS, GetPlatform(), task);
      message.SetSubStatus(aSubStatus);
      ok &= SendTaskMessage(aSimTime, message, task.GetAssignerPlatformIndex(), task.GetCommName(), false);
   }
   else
   {
      ok = false;
      auto out =
         ut::log::warning() << "Reporting progress status. Could not find Track's Assignee Platform or Resource";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Status: " << mScenarioPtr->Strings().cIN_PROGRESS;
      out.AddNote() << "Assignee: " << GetPlatform()->GetNameId();
      out.AddNote() << "Resource: " << aResourceName;
      out.AddNote() << "Could not find task.";
   }
   return ok;
}

// =================================================================================================
//! Return the list of platform (indexes) of the assignees for the specified task.
//! @param aTrackId   The local track ID of the track of interest.
//!                   If null then the track ID is not a selection criteria.
//! @param aTaskType  The task type of the tasks to be counted.
//!                   If zero then the task type is not a selection criteria.
//! @param aAssignees The indexes of the platforms that have been assigned to the specified task.
void WsfTaskManager::AssigneesForTask(const WsfTrackId& aTrackId, WsfStringId aTaskType, std::vector<size_t>& aAssignees) const
{
   aAssignees.clear();
   for (const WsfTask& task : mXmtdTaskList)
   {
      if ((aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())) &&
          ((aTaskType == nullptr) || (aTaskType == task.GetTaskType())))
      {
         size_t assigneeIndex = task.GetAssigneePlatformIndex();
         if (std::find(aAssignees.begin(), aAssignees.end(), assigneeIndex) == aAssignees.end())
         {
            aAssignees.push_back(task.GetAssigneePlatformIndex());
         }
      }
   }
}

// =================================================================================================
//! Return the platform (index) of the assignee for the specified task.
//! @param aTrackId      The local track ID of the task of interest.
//! @param aTaskType     The task type of the task of interest.
//! @param aResourceName The name (ID) of the resource of interest.
//! @returns The platform index of the assignee, which can be zero.
size_t WsfTaskManager::AssigneeForTask(const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const
{
   size_t assigneeIndex = 0;
   for (const WsfTask& task : mXmtdTaskList)
   {
      if ((aTrackId == task.GetLocalTrackId()) && (aTaskType == task.GetTaskType()) &&
          (aResourceName == task.GetResourceName()))
      {
         assigneeIndex = task.GetAssigneePlatformIndex();
         break;
      }
   }
   return assigneeIndex;
}

// =================================================================================================
//! Return the current operating level for the specified condition name or status.
int WsfTaskManager::OperatingLevelFor(WsfStringId aName) const
{
   int  level = 0;
   auto oli   = mOperatingLevels.find(aName);
   if (oli != mOperatingLevels.end())
   {
      level = oli->second;
   }
   return level;
}

// =================================================================================================
//! Return the count of the number of tasks that have been assigned to the specified subordinate.
//! @param aAssigneeIndex The platform index of the assigned platform to be queried.
//! @param aTrackId       The local track ID of the track of interest.
//!                       If null then the track ID is not a selection criteria.
//! @param aTaskType      The task type of the tasks to be counted.
//!                       If zero then the task type is not a selection criteria.
//! @param aResourceName  The resource name ID (sensor/processor) of the task to be counted.
//!                       If zero then resource name is not a selection criteria.
//! @returns The number of tasks that have been assigned.
int WsfTaskManager::TasksAssignedTo(size_t            aAssigneeIndex,
                                    const WsfTrackId& aTrackId,
                                    WsfStringId       aTaskType,
                                    WsfStringId       aResourceName) const
{
   // If an index was not supplied then assume we're counting tasks on this platform.
   size_t assigneeIndex = aAssigneeIndex;
   if (assigneeIndex == 0)
   {
      assigneeIndex = GetPlatform()->GetIndex();
   }

   int taskCount = 0;
   for (const WsfTask& task : mXmtdTaskList)
   {
      if ((assigneeIndex == task.GetAssigneePlatformIndex()) &&
          ((aTrackId.IsNull()) || (aTrackId == task.GetLocalTrackId())) &&
          ((aTaskType == nullptr) || (aTaskType == task.GetTaskType())) &&
          ((aResourceName == nullptr) || (aResourceName == task.GetResourceName())))
      {
         ++taskCount;
      }
   }
   return taskCount;
}

// =================================================================================================
//! Return the count of the number of tasks that have been assigned to the specified track.
//! @param aTrackId       The local track ID of the track of interest.
//! @param aTaskType      The task type of the tasks to be counted.
//!                       If zero then the task type is not a selection criteria.
//! @param aResourceName  The resource name ID (sensor/processor) of the task to be counted.
//!                       If zero then resource name is not a selection criteria.
//! @returns The number of tasks that have been assigned.
// virtual
int WsfTaskManager::TasksAssignedFor(const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const
{
   int taskCount = 0;
   for (const WsfTask& task : mXmtdTaskList)
   {
      if ((aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())) &&
          ((aTaskType == nullptr) || (aTaskType == task.GetTaskType())) &&
          ((aResourceName == nullptr) || (aResourceName == task.GetResourceName())))
      {
         ++taskCount;
      }
   }
   return taskCount;
}

// =================================================================================================
//! Return the count of the number of tasks that have received for the specified track.
//! @param aTrackId       The local track ID of the track of interest.
//! @param aTaskType      The task type of the tasks to be counted.
//!                       If zero then the task type is not a selection criteria.
//! @param aResourceName  The resource name ID (sensor/processor) of the task to be counted.
//!                       If zero then resource name is not a selection criteria.
//! @returns The number of tasks that have been received.
int WsfTaskManager::TasksReceivedFor(const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const
{
   int taskCount = 0;
   for (const WsfTask& task : mRcvdTaskList)
   {
      if ((aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())) &&
          ((aTaskType == nullptr) || (aTaskType == task.GetTaskType())) &&
          ((aResourceName == nullptr) || (aResourceName == task.GetResourceName())))
      {
         ++taskCount;
      }
   }
   return taskCount;
}

// =================================================================================================
//! Return the simulation time when a task was assigned.
double WsfTaskManager::TimeTaskAssigned(const WsfTrackId& aTrackId, WsfStringId aTaskType, size_t aAssigneeIndex) const
{
   double timeAssigned = -1.0;
   for (const WsfTask& task : mXmtdTaskList)
   {
      if ((aAssigneeIndex == task.GetAssigneePlatformIndex()) && (aTrackId == task.GetLocalTrackId()) &&
          (aTaskType == task.GetTaskType()))

      {
         timeAssigned = task.GetAssignTime();
      }
   }
   return timeAssigned;
}

// =================================================================================================
//! Find a task with the specified task ID, assignee name and resource Id.
//! @param aTaskList     The task list to be searched.
//! @param aAssigneeIndex The platform index of the assignee to the task.
//! @param aTrackId      The task ID of the task to be located.
//! @param aTaskType     The type of the task to be located.
//! @param aResourceName The resource ID of the task to be located.
//! @returns An iterator for the located task or aTaskList.end() if the task could not be found.
// public
WsfTaskManager::TaskList::iterator WsfTaskManager::FindTask(TaskList&         aTaskList,
                                                            size_t            aAssigneeIndex,
                                                            const WsfTrackId& aTrackId,
                                                            WsfStringId       aTaskType,
                                                            WsfStringId       aResourceName)
{
   return std::find_if(aTaskList.begin(),
                       aTaskList.end(),
                       [=](WsfTask const& task)
                       {
                          return aAssigneeIndex == task.GetAssigneePlatformIndex() && aTaskType == task.GetTaskType() &&
                                 aResourceName == task.GetResourceName() &&
                                 // The task ID could be either a track ID (from the commander) or a local track ID that
                                 // is associated with the task.
                                 (aTrackId == task.GetTrackId() || aTrackId == task.GetLocalTrackId());
                       });
}

// =================================================================================================
//! Find a task with the specified task ID
//! @param aTaskList     The task list to be searched.
//! @param aTaskId      The task ID of the task to be located.
//! @returns An iterator for the located task or aTaskList.end() if the task could not be found.
// public
WsfTaskManager::TaskList::iterator WsfTaskManager::FindTask(TaskList& aTaskList, unsigned int aTaskId)
{
   return std::find_if(aTaskList.begin(),
                       aTaskList.end(),
                       [aTaskId](WsfTask const& task) { return task.GetTaskId() == aTaskId; });
}

WsfTaskManager::TaskList::iterator WsfTaskManager::FindTask(TaskList& aTaskList, unsigned int aTaskId, size_t aAssigneeIndex)
{
   return std::find_if(aTaskList.begin(),
                       aTaskList.end(),
                       [aTaskId, aAssigneeIndex](WsfTask const& task)
                       { return task.GetTaskId() == aTaskId && task.GetAssigneePlatformIndex() == aAssigneeIndex; });
}

// =================================================================================================
//! Given a message, get the platform index of the sender and the local device on which the message was received.
// private
bool WsfTaskManager::GetMessageSender(const WsfMessage& aMessage, size_t& aSenderIndex, WsfStringId& aRcvrCommName)
{
   aSenderIndex  = 0;
   aRcvrCommName = nullptr;

   // Determine the platform index of the sender.
   WsfStringId senderName = aMessage.GetOriginator();

   if (senderName == nullptr)
   {
      // Local message not sent via comm.
      aSenderIndex = GetPlatform()->GetIndex();
   }
   else
   {
      WsfPlatform* senderPtr = GetSimulation()->GetPlatformByName(senderName);
      if (senderPtr != nullptr)
      {
         aSenderIndex = senderPtr->GetIndex();
      }
   }

   // If not sent via comm then determine the address of the receiver on which the message was received.

   if (GetPlatform()->GetIndex() != aSenderIndex)
   {
      // Determine the comm device on which the message was received.
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
      {
         if (aMessage.GetDstAddr() == iter->GetAddress())
         {
            aRcvrCommName = iter->GetNameId();
            break;
         }
      }
      if (aRcvrCommName == nullptr)
      {
         auto out = ut::log::error() << "Unable to find comm device for address.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Address: " << aMessage.GetDstAddr();
         return false;
      }
   }
   return true;
}

// =================================================================================================
//! The acknowledgment for the specified message has been received.
//! @param aSerialNumber The serial number of the message that has been acknowledged.
// private
void WsfTaskManager::MessageAcknowledged(unsigned int aSerialNumber)
{
   mPendingMessageList.remove_if([aSerialNumber](const PendingMessage& pm)
                                 { return aSerialNumber == pm.mMessagePtr->GetSerialNumber(); });
}

// =================================================================================================
//! Internal method to check if an acknowledgment has been received for a message requiring reliable service.
//! This method is invoked by CheckMessageEvent.
//! @note This is not part of the external interface, and should not be called by users.
void WsfTaskManager::CheckMessageAcknowledged(double aSimTime, unsigned int aSerialNumber)
{
   auto pmli = std::find_if(mPendingMessageList.begin(),
                            mPendingMessageList.end(),
                            [aSerialNumber](const PendingMessage& pm)
                            { return aSerialNumber == pm.mMessagePtr->GetSerialNumber(); });
   if (pmli != mPendingMessageList.end())
   {
      PendingMessage& pendingMessage = *pmli;

      // The message has yet to be acknowledged. Determine if we should resend the message.
      if (pendingMessage.mRetryAttempts > 0)
      {
         --pendingMessage.mRetryAttempts;

         // Schedule the watchdog event.
         GetSimulation()->AddEvent(ut::make_unique<CheckMessageEvent>(aSimTime + mCommRetryInterval, this, aSerialNumber));

         // Note: the message is sent 'unreliable' on subsequent attempts to prevent re-creation of the
         // pending message entry and scheduling of the watchdog event.

         SendTaskMessage(aSimTime, *(pendingMessage.mMessagePtr), pendingMessage.mRecipientIndex, pendingMessage.mCommName, false);
      }
      else
      {
         // No pending retries.
         //
         // If the message was an 'Assign Task' then we must remove the task from our list of tasks that
         // we've assigned. For 'Cancel Task' we've already removed the task from the transmitted task list.
         // For 'Task Status/Complete' we've already removed the task from our received task list.

         if (!pendingMessage.mSuppressTimeoutMessage)
         {
            auto out = ut::log::warning() << "Message timed out.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Task Manager: " << GetName();
            out.AddNote() << "Sent At: " << pendingMessage.mTimeSent;
            out.AddNote() << "Message Type: " << pendingMessage.mMessagePtr->GetType();
            out.AddNote() << "Recipient: " << GetSimulation()->GetPlatformNameId(pendingMessage.mRecipientIndex);
         }

         if (pendingMessage.mMessagePtr->GetType() == WsfTaskAssignMessage::GetTypeId())
         {
            WsfTaskAssignMessage* messagePtr = dynamic_cast<WsfTaskAssignMessage*>(pendingMessage.mMessagePtr);
            WsfTask&              task       = messagePtr->GetTask();
            auto                  xtli       = FindTask(mXmtdTaskList,
                                 task.GetAssigneePlatformIndex(),
                                 task.GetTrackId(),
                                 task.GetTaskType(),
                                 task.GetResourceName());
            if (xtli != mXmtdTaskList.end())
            {
               // NOTE - In order to avoid problems in callbacks, a copy of the task must be made and
               // the original removed from the list before proceeding.
               WsfTask taskCopy = *xtli;
               mXmtdTaskList.erase(xtli);
               CancelTask(aSimTime, taskCopy, false);
            }
         }

         mPendingMessageList.erase(pmli);
      }
   }
}

// =================================================================================================
//! Internal method to send a track update to an assignee.
//! This method is invoked by a periodic TrackUpdateEvent to send the most current track to the assignee.
//! @param aSimTime  The current simulation time.
//! @param aTaskId   The unique identifier for the task.
//! @returns The simulation time when the next track update should be sent. The return value is
//! less than zero if no further updates should be sent.
//! @note This is not part of the external interface, and should not be called by users.
double WsfTaskManager::SendTrackUpdate(double aSimTime, unsigned int aTaskId)
{
   double nextUpdateTime = -1.0;
   for (TaskList::const_iterator xtli = mXmtdTaskList.begin(); xtli != mXmtdTaskList.end(); ++xtli)
   {
      const WsfTask& task = *xtli;
      if (aTaskId == task.GetTaskId())
      {
         const WsfTrack* trackPtr = mTrackManagerPtr->FindTrack(task.GetLocalTrackId());
         if (trackPtr != nullptr)
         {
            if (!trackPtr->IsStale())
            {
               if (DebugEnabled())
               {
                  auto out = ut::log::debug() << "Sent track update to assignee.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Task Manager: " << GetName();
                  out.AddNote() << "Assignee: " << task.GetAssigneePlatformName();
                  out.AddNote() << "Target: " << trackPtr->GetTargetName();
                  out.AddNote() << "Track ID: " << trackPtr->GetTrackId();
               }
               WsfTrackMessage message(GetPlatform(), *trackPtr);
               if (SendTaskMessage(aSimTime, message, task.GetAssigneePlatformIndex(), task.GetCommName(), false))
               {
                  nextUpdateTime = aSimTime + mTrackUpdateInterval;
               }
            }
            else
            {
               // If the track is stale we still reschedule the event as it may receive an update at a later time.
               nextUpdateTime = aSimTime + mTrackUpdateInterval;
            }
         }
         break;
      }
   }
   return nextUpdateTime;
}


// =================================================================================================
//! Internal method to select a comm device if one wasn't explicitly provided.
//! If the assignee platform is not the same as the assigner platform and if a comm device was
//! not explicitly selected, then attempt to choose a comm device over which a message can be
//! sent to the assignee.
//!
//! @param aRecipientPtr The intended recipient.
//! @param aCommName     The supplied comm device (or zero if to be determined).
//! @returns The name (string ID) of the comm device to be used.
WsfStringId WsfTaskManager::SelectCommDevice(WsfPlatform* aRecipientPtr, WsfStringId aCommName)
{
   WsfStringId commName = aCommName;
   if ((aCommName == nullptr) &&         // A comm device was not explicitly specified
       (aRecipientPtr != nullptr) &&     // ... and a recipient was specified
       (aRecipientPtr != GetPlatform())) // ... and it isn't the same as the sender
   {
      auto* commPtr = wsf::comm::util::SelectDevice(GetPlatform(), aRecipientPtr);
      if (commPtr != nullptr)
      {
         commName = commPtr->GetNameId();
      }
   }
   return commName;
}

// =================================================================================================
//! Attempt to send a message.
//! This routine will send a message to the desired recipient. If the recipient of the message is the
//! same as the sending platform then this processors ProcessMessage() method will be called to handle
//! the message.
//!
//! @param aSimTime        The current simulation time.
//! @param aMessage        The message to be sent.
//! @param aRecipientIndex The platform index of the recipient.
//! @param aCommName       For an inter-platform message, this is the name (string ID) of the communications
//!                        device over which the message is to be sent. For an intra-platform message, this is
//!                        the name (string ID) of the processor to whom the message is intended.
//! @param aReliable       If true, the message must be delivered 'reliably'. The message will be retransmitted
//!                        (if allowed) until the receiver responds with a message indicating they received
//!                        the message.
bool WsfTaskManager::SendTaskMessage(double            aSimTime,
                                     const WsfMessage& aMessage,
                                     size_t            aRecipientIndex,
                                     WsfStringId       aCommName,
                                     bool              aReliable)
{
   // If the message is to be sent reliably then schedule the event that checks for receipt of the acknowledgment.
   // This must be done before the message is actually sent.
   if (aReliable && (mCommRetryInterval > 0.0))
   {
      PendingMessage pendingMessage(aSimTime, this, aMessage, aRecipientIndex, aCommName, mCommRetryAttempts);
      mPendingMessageList.push_back(pendingMessage);

      GetSimulation()->AddEvent(
         ut::make_unique<CheckMessageEvent>(aSimTime + mCommRetryInterval, this, aMessage.GetSerialNumber()));
   }

   // NOTE: The checks for comm/recipient/processor existence should match the order in the PendingMessageConstructor.

   bool messageSent = true;
   if (aRecipientIndex != GetPlatform()->GetIndex())
   {
      // Inter-platform message.

      // Find the comm device.
      wsf::comm::Comm* commPtr = GetPlatform()->GetComponent<wsf::comm::Comm>(aCommName);
      if (commPtr == nullptr)
      {
         auto out = ut::log::error() << "Unable to send message. Comm device does not exist.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Recipient: " << GetSimulation()->GetPlatformNameId(aRecipientIndex);
         out.AddNote() << "Comm: " << aCommName;
         return false;
      }

      // Make sure the recipient still exists.
      WsfPlatform* recipientPtr = GetSimulation()->GetPlatformByIndex(aRecipientIndex);
      if (recipientPtr == nullptr)
      {
         auto out = ut::log::error() << "Unable to send message. Recipient no longer exists.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Recipient: " << GetSimulation()->GetPlatformNameId(aRecipientIndex);
         out.AddNote() << "Comm: " << aCommName;
         return false;
      }

      // TODO-HACK JTIDS command chain for task messages.
      // The JTIDS comm device requires a command chain. For now we'll simply choose the first chain
      // that contains the recipient. If no chain contains the recipient then choose the default chain.
      //
      // Note that with JTIDS, the command chain selects the slot group. If the recipient is on more than
      // one chain this may cause the wrong slot group to be used.

      WsfCommandChain* ccPtr = nullptr;
      for (WsfComponentList::RoleIterator<WsfCommandChain> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
      {
         if (recipientPtr == iter->GetCommander() || PlatformListContains(iter->GetSubordinates(), recipientPtr) ||
             PlatformListContains(iter->GetPeers(), recipientPtr))
         {
            ccPtr = *iter;
            break;
         }
      }

      aMessage.SetGroup(UtStringIdLiteral("default"));
      if (ccPtr)
      {
         aMessage.SetGroup(ccPtr->GetNameId());
      }

      //! Determine the destination comm. We select any comm that is the same network
      //! and belongs to the destination.
      wsf::comm::Comm* destCommPtr = nullptr;
      for (auto componentPtr : recipientPtr->GetComponents())
      {
         auto curCommPtr = dynamic_cast<wsf::comm::Comm*>(componentPtr);
         if (curCommPtr)
         {
            auto allNetworkMembers =
               GetSimulation()->GetCommNetworkManager()->GetAddressesInNetwork(commPtr->GetNetwork());
            for (const auto& memberAddress : allNetworkMembers)
            {
               auto memberCommPtr = GetSimulation()->GetCommNetworkManager()->GetComm(memberAddress);
               if (memberCommPtr && memberCommPtr->GetPlatform() == recipientPtr)
               {
                  destCommPtr = memberCommPtr;
               }
            }
         }
      }

      if (destCommPtr)
      {
         commPtr->Send(aSimTime, ut::clone(aMessage), destCommPtr->GetAddress());
      }
   }
   else if ((aCommName == nullptr) || (aCommName == GetNameId()))
   {
      // Intra-platform message destined for THIS processor.
      ProcessMessage(aSimTime, aMessage);

      // Assume THIS processor can successfully talk to itself
      messageSent = true;
   }
   else
   {
      // Intra-platform message destined for another processor.
      WsfProcessor* processorPtr = GetPlatform()->GetComponent<WsfProcessor>(aCommName);
      if (processorPtr != nullptr)
      {
         messageSent = processorPtr->ReceiveMessage(aSimTime, aMessage);
      }
      else
      {
         auto out = ut::log::error() << "Unable to send message. Target processor does not exist.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Recipient: " << GetSimulation()->GetPlatformNameId(aRecipientIndex);
         out.AddNote() << "Target Processor: " << aCommName;
         messageSent = false;
      }
   }
   return messageSent;
}

// =================================================================================================
//! Process the sensor track request association message.
//!
//! When a sensor initiates a track that is the result of StartTracking() request, it will send a
//! WSF_TRACK_ASSOCIATION_MESSAGE prior to sending the first WSF_TRACK_MESSAGE. This information
//! provides the track ID of the StartTracking() request and the track ID of the track that is
//! being produced as a result of that request.
//!
//! @returns 'false' to allow other message listeners to look at the message.
// private
bool WsfTaskManager::ProcessAssociationMessage(double aSimTime, const WsfAssociationMessage& aMessage)
{
   bool ok = true;
   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      ok &= i->ProcessAssociationMessage(aSimTime, aMessage);
   }

   if (ok)
   {
      const WsfTrackId& taskTrackId   = aMessage.GetSubjectId();
      const WsfTrackId& sensorTrackId = aMessage.GetAssociatedId();
      if (mShowTaskMessages)
      {
         auto out = ut::log::debug() << "Received association status message.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Task Track: " << taskTrackId;
         out.AddNote() << "Sensor Track: " << sensorTrackId;
      }

      // Make sure the track task isn't already cached.
      for (TrackList::const_iterator atli = mActiveTrackList.begin(); atli != mActiveTrackList.end(); ++atli)
      {
         const ActiveTrack& activeTrack = *atli;
         if ((activeTrack.mTaskTrackId == taskTrackId) && (activeTrack.mSensorTrackId == sensorTrackId))
         {
            return false;
         }
      }
      ActiveTrack activeTrack;
      activeTrack.mTaskTrackId   = taskTrackId;
      activeTrack.mSensorTrackId = sensorTrackId;
      activeTrack.mSensorName    = aMessage.GetSensorName();
      mActiveTrackList.push_back(activeTrack);
      ok = false; // allow other message listeners to look at message
   }
   return ok; // TODO-AWK should this also return false if other message listeners need to look at message?
}


// =================================================================================================
// private
bool WsfTaskManager::ProcessTaskAssignMessage(double aSimTime, const WsfTaskAssignMessage& aMessage)
{
   size_t            assignerIndex = aMessage.GetTask().GetAssignerPlatformIndex();
   const WsfTrackId& trackId       = aMessage.GetTask().GetTrackId();
   WsfStringId       taskType      = aMessage.GetTask().GetTaskType();
   const WsfTrack&   track         = aMessage.GetTrack();
   WsfStringId       resourceName  = aMessage.GetTask().GetResourceName();
   if (mShowTaskMessages)
   {
      WsfStringId resourceType(aMessage.GetTask().GetResource().GetTypeName());
      auto        out = ut::log::debug() << "Received task assign message.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Target: " << aMessage.GetTask().GetTargetName();
      out.AddNote() << "Track ID: " << trackId;
      out.AddNote() << "Task Type: " << taskType;
      out.AddNote() << "Resource Name: " << resourceName;
      out.AddNote() << "Resource Type: " << resourceType;
   }

   // Determine the communications device on which the task assignment message was received.
   // For now we'll assume that all responses will be sent back via the same device. This
   // obviously doesn't work if the transmitter and receiver are separate devices or if it is
   // intended to respond back over a different 'channel'.

   size_t      senderIndex = 0;
   WsfStringId commName;
   if (!GetMessageSender(aMessage, senderIndex, commName))
   {
      auto out = ut::log::error() << "Task assignment internal error 1.";
      out.AddNote() << "T =" << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      return true;
   }
   // If the resulting comm name is 0 it means the assigner and assignee are on the same platform.
   // Set the comm name to the assigning processor name so status messages get routed correctly (see SendTaskMessage).
   if (commName == nullptr)
   {
      commName = aMessage.GetTask().GetAssignerProcessorName();
   }

   // If this is an anonymous task (i.e.: a null track ID in the task) then it should be a resource task
   // that does not need to be tracked. These are used to simply turn sensors on and off remotely.

   if (trackId.IsNull())
   {
      // Anonymous task.

      WsfStringId status = mScenarioPtr->Strings().cACCEPTED;
      WsfTask     task(aMessage.GetTask()); // need a non-const task
      if (!AcquireResource(aSimTime, task, aMessage.GetTrack()))
      {
         status = mScenarioPtr->Strings().cREJECTED;
      }
      WsfTaskStatusMessage message(status, GetPlatform(), task);
      message.SetAcknowledgeId(aMessage.GetSerialNumber());
      SendTaskMessage(aSimTime, message, assignerIndex, commName, false);
   }
   else
   {
      // Non-anonymous task.

      // The first assignment of the task will cause the task to be registered while subsequent assignments
      // will just cause an update of the task.
      //
      // NOTE: This must be done before any object spawning because there are some callbacks that update the data.

      auto rtli = FindTask(mRcvdTaskList, GetPlatform()->GetIndex(), trackId, taskType, resourceName);
      if (rtli == mRcvdTaskList.end())
      {
         // Create the task from the assignment.
         WsfTask task(aMessage.GetTask());
         task.SetCommName(commName);
         mRcvdTaskList.push_front(task);
         rtli = mRcvdTaskList.begin();
      }
      else
      {
         // copy over all task data (overwrite previous task)
         WsfTask task(aMessage.GetTask());
         task.SetCommName(commName);
         *rtli = task;
      }

      // Enter the track into the local track list.
      //
      // NOTE: This must be done after adding the entry to the received task list.  When the track manager
      // correlates the track report it will invoke our LocalTrack<xxx> method, which searches the received
      // task list for the original track. This callback may occur immediately or may occur sometime in the
      // future, depending on the track manager...

      WsfTask& task = *rtli;
      if (!track.GetTrackId().IsNull())
      {
         if (assignerIndex == GetPlatform()->GetIndex())
         {
            // Task is self-assigned. The track ID should be the local track ID.
            task.SetLocalTrackId(track.GetTrackId());
            TaskCorrelated(aSimTime, task);
         }
         else
         {
            // Add the track to the list of tracks that need correlation with a local track.
            mPendingRcvdTasks.insert(task.GetTrackId());

            // Add the track report.
            mTrackManagerPtr->AddTrackReport(aSimTime, track);

            // If the track manager did not immediately correlate the track the track with a local track
            // then determine if a correlation already existed.
            if (task.GetLocalTrackId().IsNull())
            {
               UpdateTaskCorrelation(aSimTime, task);
            }
            else
            {
               mPendingRcvdTasks.erase(task.GetTrackId());
            }
         }
      }

      // Perform sensor or processor processing.

      WsfStringId status = mScenarioPtr->Strings().cACCEPTED;
      if (!AcquireResource(aSimTime, task, track))
      {
         status = mScenarioPtr->Strings().cREJECTED;
      }

      OnTaskAssigned(aSimTime, task, track);

      WsfObserver::TaskAccepted(GetSimulation())(aSimTime, &task, &track);

      TaskAssigned(aSimTime, track, task);

      // Send the response to indicate acceptance or rejection of the task.
      WsfTaskStatusMessage message(status, GetPlatform(), task);
      message.SetAcknowledgeId(aMessage.GetSerialNumber());
      SendTaskMessage(aSimTime, message, assignerIndex, commName, false);

      // If the request failed then tell the observers and purge the received task.
      if (status != mScenarioPtr->Strings().cACCEPTED)
      {
         // NOTE - In order to avoid problems in callbacks, the task is removed from the received task list before proceeding.
         //        A temporary list is used to store the task before deletion.
         TaskList removedTaskList;
         removedTaskList.splice(removedTaskList.end(), mRcvdTaskList, rtli);

         WsfObserver::TaskCompleted(GetSimulation())(aSimTime, &task, status);
         PurgeReceivedTask(aSimTime, task);
      }
   }
   mTimeLastTaskReceived = aSimTime;
   return true;
}

// =================================================================================================
// private
bool WsfTaskManager::ProcessTaskCancelMessage(double aSimTime, const WsfTaskCancelMessage& aMessage)
{
   unsigned int      taskId        = aMessage.GetTaskId();
   size_t            assignerIndex = aMessage.GetAssignerPlatformIndex();
   const WsfTrackId& trackId       = aMessage.GetTrackId();
   WsfStringId       taskType      = aMessage.GetTaskType();
   WsfStringId       resourceName  = aMessage.GetResourceName();
   if (mShowTaskMessages)
   {
      auto out = ut::log::debug() << "Received task cancel message.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Track ID: " << trackId;
      out.AddNote() << "Task Type: " << taskType;
   }

   // Find and purge the task from the local task list.

   auto rtli = FindTask(mRcvdTaskList, GetPlatform()->GetIndex(), trackId, taskType, resourceName);
   if (rtli != mRcvdTaskList.end())
   {
      WsfTask& task = *rtli;
      // NOTE - In order to avoid problems in callbacks, the task is removed from the received task list before proceeding.
      //        A temporary list is used to store the task before deletion.
      TaskList removedTaskList;
      removedTaskList.splice(removedTaskList.end(), mRcvdTaskList, rtli);

      // Send the acknowledgment message.
      WsfTaskStatusMessage message(mScenarioPtr->Strings().cACKNOWLEDGE_CANCEL, GetPlatform(), task);
      message.SetAcknowledgeId(aMessage.GetSerialNumber());
      SendTaskMessage(aSimTime, message, task.GetAssignerPlatformIndex(), task.GetCommName(), false);

      OnTaskCanceled(aSimTime, task);

      TaskCanceled(aSimTime, task); // protected virtual method (for any derived class processing)

      PurgeReceivedTask(aSimTime, task);
   }
   else if (!DropFromPurgedTaskList(mPurgedRcvdTaskList, assignerIndex, taskId))
   {
      auto out = ut::log::error() << "Cannot find received task to cancel.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Track ID: " << trackId;
      out.AddNote() << "Task Type: " << taskType;
      out.AddNote() << "Resource: " << resourceName;
   }
   return true;
}

// =================================================================================================
// private
bool WsfTaskManager::ProcessTaskControlMessage(double aSimTime, const WsfTaskControlMessage& aMessage)
{
   bool        messageProcessed = false;
   WsfStringId name             = aMessage.GetName();
   int         level            = aMessage.GetLevel();
   if ((name != 0) && (level >= 0))
   {
      if (mShowTaskMessages)
      {
         auto out = ut::log::debug() << "Changing operating level for control message.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Message: " << aMessage.GetName();
         out.AddNote() << "New Level: " << level;
         out.AddNote() << "Old Level: " << mOperatingLevels[name];
      }
      messageProcessed       = true;
      mOperatingLevels[name] = level;

      OperatingLevelChanged(aSimTime, aMessage.GetName().GetString());

      WsfObserver::OperatingLevelChanged(GetSimulation())(aSimTime, this, name, level);
   }
   return messageProcessed;
}

// =================================================================================================
// private
bool WsfTaskManager::ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage)
{
   bool messageProcessed = true;

   // If the message contains an acknowledgment of a message that was sent 'reliably' then
   // indicate that the message was successfully received by the recipient.
   if (aMessage.GetAcknowledgeId() != 0)
   {
      MessageAcknowledged(aMessage.GetAcknowledgeId());
   }

   size_t       assignerIndex = aMessage.GetAssignerPlatformIndex();
   unsigned int taskId        = aMessage.GetTaskId();
   WsfStringId  statusId      = aMessage.GetStatus();
   if ((statusId == mScenarioPtr->Strings().cCOMPLETED) || (statusId == mScenarioPtr->Strings().cACCEPTED) ||
       (statusId == mScenarioPtr->Strings().cREJECTED) || (statusId == mScenarioPtr->Strings().cIN_PROGRESS))
   {
      size_t            assigneeIndex = aMessage.GetAssigneePlatformIndex();
      WsfStringId       assigneeName  = aMessage.GetAssigneePlatformName();
      const WsfTrackId& trackId       = aMessage.GetTrackId();
      WsfStringId       taskType      = aMessage.GetTaskType();
      WsfStringId       resourceName  = aMessage.GetResourceName();
      if (mShowTaskMessages)
      {
         auto out = ut::log::debug() << "Received task status message.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Status: " << statusId;
         out.AddNote() << "Track ID: " << trackId;
         out.AddNote() << "Task Type: " << taskType;
         out.AddNote() << "Assignee: " << assigneeName;
         out.AddNote() << "Resource: " << resourceName;
      }

      // Find the task that was sent to the assignee

      auto xtli = FindTask(mXmtdTaskList, assigneeIndex, trackId, taskType, resourceName);
      if (xtli == mXmtdTaskList.end())
      {
         bool showWarning = true;
         if (statusId == mScenarioPtr->Strings().cCOMPLETED)
         {
            showWarning = (!DropFromPurgedTaskList(mPurgedXmtdTaskList, assignerIndex, taskId));
         }
         if (showWarning)
         {
            auto out = ut::log::warning() << "Could not find task.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Task Manager: " << GetName();
            out.AddNote() << "Track ID: " << trackId;
            out.AddNote() << "Task Type: " << taskType;
            out.AddNote() << "Assignee: " << assigneeName;
            out.AddNote() << "Resource: " << resourceName;
         }
         messageProcessed = false;
      }

      if (messageProcessed)
      {
         WsfTask& task = *xtli;
         task.SetUpdateTime(aSimTime);
         task.SetStatus(statusId);
         task.SetSubStatus(aMessage.GetSubStatus());

         // If the task has been accepted then start sending periodic track updates.
         if ((statusId == mScenarioPtr->Strings().cACCEPTED) && (mTrackUpdateStrategy == cDEFAULT) &&
             (mTrackUpdateInterval > 0.0))
         {
            GetSimulation()->AddEvent(
               ut::make_unique<TrackUpdateEvent>(aSimTime + mTrackUpdateInterval, this, task.GetTaskId()));
         }

         // If an assignee has indicated that it has completed the task then we must acknowledge the message.
         if (statusId == mScenarioPtr->Strings().cCOMPLETED)
         {
            WsfTaskStatusMessage message(mScenarioPtr->Strings().cACKNOWLEDGE_COMPLETE, GetPlatform(), task);
            message.SetIsFromAssignee(false);
            message.SetAcknowledgeId(aMessage.GetSerialNumber());
            SendTaskMessage(aSimTime, message, task.GetAssigneePlatformIndex(), task.GetCommName(), false);
         }

         // Purge the task if no longer needed.
         if ((statusId == mScenarioPtr->Strings().cCOMPLETED) || (statusId == mScenarioPtr->Strings().cREJECTED))
         {
            // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task list before
            // proceeding.
            //        A temporary list is used to store the task before deletion.
            TaskList removedTaskList;
            removedTaskList.splice(removedTaskList.end(), mXmtdTaskList, xtli);

            OnTaskCompleted(aSimTime, task);

            TaskCompleted(aSimTime, task); // protected virtual method (for any derived class processing)

            PurgeTransmittedTask(aSimTime, task);
         }
      }
   }
   else if (statusId == mScenarioPtr->Strings().cACKNOWLEDGE_CANCEL)
   {
      auto iter = FindTask(mXmtdTaskList, taskId);
      if (iter != mXmtdTaskList.end())
      {
         mXmtdTaskList.erase(iter);
      }
      DropFromPurgedTaskList(mPurgedXmtdTaskList, assignerIndex, taskId);
   }
   else if (statusId == mScenarioPtr->Strings().cACKNOWLEDGE_COMPLETE)
   {
      DropFromPurgedTaskList(mPurgedRcvdTaskList, assignerIndex, taskId);
   }
   else
   {
      messageProcessed = false;
   }

   if (!messageProcessed)
   {
      // Check components
      for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
      {
         if (i->ProcessTaskStatusMessage(aSimTime, aMessage))
         {
            messageProcessed = true;
            break; // break on first component to process, TODO-AWK, is this what we want to do?
         }
      }
   }
   return messageProcessed;
}

// =================================================================================================
// private
bool WsfTaskManager::ProcessTrackDropMessage(double aSimTime, const WsfTrackDropMessage& aMessage)
{
   // Delete the associated track task entry if one is present.
   auto atli = mActiveTrackList.begin();
   while (atli != mActiveTrackList.end())
   {
      if (atli->mSensorTrackId == aMessage.GetTrackId())
      {
         if (mShowTaskMessages)
         {
            auto out = ut::log::debug() << "Sensor track dropped.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Task Manager: " << GetName();
            out.AddNote() << "Message: " << aMessage.GetTrackId();
            out.AddNote() << "Track ID: " << atli->mTaskTrackId;
         }

         // If we've received a tracking task for this track then we must report the task complete.
         for (WsfTask& task : mRcvdTaskList)
         {
            if ((atli->mSensorName == task.GetResourceName()) && (atli->mTaskTrackId == task.GetLocalTrackId()))
            {
               ReportTaskComplete(aSimTime,
                                  task.GetLocalTrackId(),
                                  task.GetTaskType(),
                                  task.GetResourceName(),
                                  mScenarioPtr->Strings().cSUCCESSFUL);
               break;
            }
         }

         atli = mActiveTrackList.erase(atli);
      }
      else
      {
         ++atli;
      }
   }
   return false; // return false because others may be interested
}

// =================================================================================================
// private
bool WsfTaskManager::ProcessTrackMessage(double aSimTime, const WsfTrackMessage& aMessage)
{
   return false; // return false because others may be interested
}

// =================================================================================================
//! Perform the actions necessary to purge a received task.
//! This routine is invoked by the receiver of a task after it has completed the task.
//!
//! @param aSimTime The current simulation time.
//! @param aTask    The task being purged.
//! @note The caller is responsible for deleting the task from the received task list.
// private
void WsfTaskManager::PurgeReceivedTask(double aSimTime, WsfTask& aTask)
{
   // If this is a 'primary task' (i.e.: one whose resource name is zero) then also cancel all
   // tasks that have been transmitted that are supporting this task.

   // Note: This is not done on a self-assigned task as we will be canceling tasks we shouldn't be canceling.
   if ((aTask.GetAssignerPlatformIndex() != aTask.GetAssigneePlatformIndex()) && (aTask.GetResourceName() == nullptr))
   {
      // Potentially remove the task from the pending received task list.
      mPendingRcvdTasks.erase(aTask.GetTrackId());

      auto xtli = mXmtdTaskList.begin();
      while (xtli != mXmtdTaskList.end())
      {
         if (aTask.GetTrackId() == (*xtli).GetTrackId())
         {
            // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task list before
            // proceeding.
            //        A temporary list is used to store the task before deletion.
            TaskList removedTaskList;
            removedTaskList.splice(removedTaskList.end(), mXmtdTaskList, xtli++);

            CancelTask(aSimTime, removedTaskList.front(), false);
         }
         else
         {
            ++xtli;
         }
      }
   }

   // If this is a 'resource task' (i.e.: one whose resource name is zero) then release the resource.

   ReleaseResource(aSimTime, aTask);

   // There used to be a piece of code here that would remove any entry from mActiveTrackList but it had
   // to be removed. That list represented the associations between the task track ID and the sensor track ID
   // and it is the track drop for the sensor track that will cause that.

   // Unlock the local track to allow it to be purged.

   if ((aTask.GetAssignerPlatformIndex() != aTask.GetAssigneePlatformIndex()) && //! not self assigned...
       (!aTask.GetLocalTrackId().IsNull()))
   {
      mTrackManagerPtr->UnlockTrack(aSimTime, aTask.GetLocalTrackId());

      // This is a little dicey because there are a lot of conditions. When we receive a task assignment
      // (see ProcessTaskAssignMessage) we invoke WsfTrackManager::AddTrackReport to add the supplied track to the
      // track manager. This track report will either be correlated with an existing local track OR a new local
      // track will be created. If a track processor has been defined which will purge tracks, then the raw track
      // we added and the associated local track will be purged when it deems appropriate. The problem comes when
      // a track processor is NOT present (such as in the case when one has a very simple radar site or simple
      // command node that simply responds to a command and has no need for complex track processing).
      //
      // NOTE: This should be done AFTER UnlockTrack because DropTrack will ALSO drop the local track if it is
      //       the last raw track that correlates with the local track. For now it will continue work either way
      //       because Unlock will also drop the local track if it has no contributors, but this is the
      //       prescribed method.

      if (!mTrackManagerPtr->WillPurgeInactiveTracks())
      {
         if (mShowTaskMessages)
         {
            auto out = ut::log::debug() << "Dropping raw track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Task Manager: " << GetName();
            out.AddNote() << "Track ID: " << aTask.GetTrackId();
         }
         mTrackManagerPtr->DropTrack(aSimTime, aTask.GetTrackId());
      }
   }

   if (mShowTaskMessages)
   {
      WsfStringId resourceName(aTask.GetResourceName());
      WsfStringId resourceType(aTask.GetResource().GetTypeName());
      auto        out = ut::log::debug() << "Purged received task.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Track ID: " << aTask.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Assignee: " << aTask.GetAssigneePlatformName();
      out.AddNote() << "Resource Name: " << resourceName;
      out.AddNote() << "Resource Type: " << resourceType;
      out.AddNote() << "Local Track ID: " << aTask.GetLocalTrackId();
   }

   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->PurgeReceivedTask(aSimTime, aTask);
   }
}

// =================================================================================================
//! Perform the actions necessary to purge a transmitted task.
//! This routine is invoked by the transmitter of a task after it has been notified that
//! the receiver has completed the task (successfully, unsuccessfully or rejected) or if
//! assignee did not accept the task in the required time interval.
//!
//! @param aSimTime The current simulation time.
//! @param aTask    The task being purge.
//! @note The caller is responsible for deleting the task from the transmitted task list.
void WsfTaskManager::PurgeTransmittedTask(double aSimTime, WsfTask& aTask)
{
   // Unlock the local track to allow the track manager to purge the track.
   // (It was incremented in AssignTask).
   if (!aTask.GetTrackId().IsNull())
   {
      mTrackManagerPtr->UnlockTrack(aSimTime, aTask.GetTrackId());
   }

   if (mShowTaskMessages)
   {
      WsfStringId resourceName(aTask.GetResourceName());
      WsfStringId resourceType(aTask.GetResource().GetTypeName());
      auto        out = ut::log::debug() << "Purged transmitted task.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Track ID: " << aTask.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Assignee: " << aTask.GetAssigneePlatformName();
      out.AddNote() << "Resource Name: " << resourceName;
      out.AddNote() << "Resource Type: " << resourceType;
   }

   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->PurgeTransmittedTask(aSimTime, aTask);
   }
}

// =================================================================================================
//! Called when a task has been correlated with a local track
void WsfTaskManager::TaskCorrelated(double aSimTime, WsfTask& aTask)
{
   // Check components
   for (RoleIterator i(GetComponents()); !i.AtEnd(); ++i)
   {
      i->TaskCorrelated(aSimTime, aTask);
   }
}

// =================================================================================================
//! Update correlations for received tasks.
void WsfTaskManager::UpdatePendingReceivedTasks(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   if (!mPendingRcvdTasks.empty())
   {
      // Determine if any received task needs to have an associated local track Id.
      for (WsfTask& task : mRcvdTaskList)
      {
         if ((task.GetLocalTrackId().IsNull()) && (!task.GetTrackId().IsNull()))
         {
            // If the assigned task track ID is correlated with the created/updated local track then
            // link the assigned task track ID with the local track ID.
            if (aLocalTrackPtr->IsCorrelatedWith(task.GetTrackId()))
            {
               UpdateTaskCorrelation(aSimTime, task);
            }
         }
      }
   }
}

// =================================================================================================
//! Update the sensor request list.
void WsfTaskManager::UpdateSensorRequestList(double            aSimTime,
                                             const WsfTrackId& aTrackId,
                                             WsfSensor*        aSensorPtr,
                                             WsfStringId       aModeNameId,
                                             bool              aAddEntry)
{
   // Locate the sensor request list entry.
   SensorRequestList::iterator srli;
   for (srli = mSensorRequestList.begin(); srli != mSensorRequestList.end(); ++srli)
   {
      SensorRequest& request = *srli;
      if ((request.mSensorPtr == aSensorPtr) && (request.mSensorTrackId == aTrackId))
      {
         break;
      }
   }

   std::string action;
   if (srli != mSensorRequestList.end()) // Entry exists
   {
      if (aAddEntry) // Update existing entry
      {
         (*srli).mModeNameId = aModeNameId;
         action              = "Updated";
      }
      else // Delete existing entry
      {
         mSensorRequestList.erase(srli);
         action = "Deleted";
      }
   }
   else if (aAddEntry)
   {
      // No existing entry and requesting an addition
      SensorRequest request;
      request.mSensorTrackId = aTrackId;
      request.mSensorPtr     = aSensorPtr;
      request.mModeNameId    = aModeNameId;
      mSensorRequestList.push_back(request);
      action = "Added";
   }

   if (mShowTaskMessages)
   {
      auto out = ut::log::debug() << action << " sensor request.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Track ID: " << aTrackId;
      out.AddNote() << "Sensor: " << aSensorPtr->GetName();
      out.AddNote() << "Mode: " << aModeNameId;
   }
}

// =================================================================================================
//! Update the task track/local track correlation in a received task.
void WsfTaskManager::UpdateTaskCorrelation(double aSimTime, WsfTask& aTask)
{
   // Attempt to locate the local track that is associated with the track that was provided with the task.
   WsfLocalTrack* trackPtr = mTrackManagerPtr->FindCorrelatedTrack(aTask.GetTrackId());
   if (trackPtr != nullptr)
   {
      aTask.SetLocalTrackId(trackPtr->GetTrackId());
      // Lock the track to prevent it from being purged by the track manager.
      mTrackManagerPtr->LockTrack(aSimTime, trackPtr->GetTrackId());
      mPendingRcvdTasks.erase(aTask.GetTrackId());
      TaskCorrelated(aSimTime, aTask);
      if (mShowTaskMessages)
      {
         auto out = ut::log::debug() << "Updating task and local track correlation.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Task Manager: " << GetName();
         out.AddNote() << "Task Track ID: " << aTask.GetTrackId();
         out.AddNote() << "Local Track ID:  " << aTask.GetLocalTrackId();
      }
   }
}

// =================================================================================================
//! A track manager callback to handle the drop of a local track.
//! @param aSimTime       The current simulation time.
//! @param aLocalTrackPtr The local track that is being dropped.
// private
void WsfTaskManager::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   if (mShowTaskMessages)
   {
      auto out = ut::log::debug() << "Dropped local track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Task Manager: " << GetName();
      out.AddNote() << "Target: " << aLocalTrackPtr->GetTargetName();
      out.AddNote() << "Track ID: " << aLocalTrackPtr->GetTrackId();
   }

   // Drop any tasks that I have assigned to the track.
   CancelTask(aSimTime, aLocalTrackPtr->GetTrackId(), nullptr, WsfTaskResource(), 0);

   TrackDropped(aSimTime, aLocalTrackPtr->GetTrackId()); // protected virtual method (for derived class processing)

   // If I have been assigned a task that is associated with this track then inform the assigner that
   // we're no longer doing anything.

   bool reportSent = true;
   while (reportSent)
   {
      reportSent = false;
      for (WsfTask& task : mRcvdTaskList)
      {
         if (task.GetLocalTrackId() == aLocalTrackPtr->GetTrackId())
         {
            reportSent = true;
            ReportTaskComplete(aSimTime,
                               task.GetTrackId(),
                               task.GetTaskType(),
                               task.GetResourceName(),
                               mScenarioPtr->Strings().cSUCCESSFUL);
            break;
         }
      }
   }
}

// =================================================================================================
//! A track manager callback to handle the initiation of a local track.
//! @param aSimTime       The current simulation time.
//! @param aLocalTrackPtr The local track.
//! @param aRawTrackPtr   The raw track that caused the creation of the local track (may be zero).
// private
void WsfTaskManager::LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   // Determine if the local track completes the correlation for a received task.
   UpdatePendingReceivedTasks(aSimTime, aLocalTrackPtr);
}

// =================================================================================================
//! A track manager callback to handle the update of a local track.
//! @param aSimTime       The current simulation time.
//! @param aLocalTrackPtr The local track.
//! @param aRawTrackPtr   The raw track that caused the update of the local track (may be zero).
// private
void WsfTaskManager::LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   // Determine if the local track completes the correlation for a received task.
   UpdatePendingReceivedTasks(aSimTime, aLocalTrackPtr);

   // If the track has been assigned to an off-board asset then determine if an update should be sent.
   if (mTrackUpdateStrategy == cBATCH)
   {
      for (WsfTask& task : mXmtdTaskList)
      {
         if ((task.GetLocalTrackId() == aLocalTrackPtr->GetTrackId()) &&
             ((aSimTime - task.GetUpdateTime()) > mTrackUpdateInterval) &&
             (task.GetAssigneePlatformIndex() != GetPlatform()->GetIndex()))
         {
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Sent track update to assignee.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Task Manager: " << GetName();
               out.AddNote() << "Assignee: " << task.GetAssigneePlatformName();
               out.AddNote() << "Target: " << aLocalTrackPtr->GetTargetName();
               out.AddNote() << "Track ID: " << aLocalTrackPtr->GetTrackId();
            }
            WsfTrackMessage message(GetPlatform(), *aLocalTrackPtr);
            if (SendTaskMessage(aSimTime, message, task.GetAssigneePlatformIndex(), task.GetCommName(), false))
            {
               task.SetUpdateTime(aSimTime);
            }
         }
      }
   }
}

// =================================================================================================
void WsfTaskManager::TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr)
{
   if (aNewManagerPtr)
   {
      mTrackManagerPtr = aNewManagerPtr;
   }
}

// =================================================================================================
// Nested class 'PendingMessage'
// =================================================================================================

WsfTaskManager::PendingMessage::PendingMessage(double            aSimTime,
                                               WsfTaskManager*   aManagerPtr,
                                               const WsfMessage& aMessage,
                                               size_t            aRecipientIndex,
                                               WsfStringId       aCommName,
                                               int               aRetryAttempts)
   : mTimeSent(aSimTime)
   , mMessagePtr(aMessage.Clone())
   , mRecipientIndex(aRecipientIndex)
   , mCommName(aCommName)
   , mRetryAttempts(aRetryAttempts)
   , mSuppressTimeoutMessage(false)
{
   // Determine if the 'timeout' message issued by CheckMessageAcknowledged() should be suppressed.
   //
   // The message is suppressed if, when the message is initially created, there is absolutely no possibility
   // the message can be delivered. This occurs for one of three reasons:
   //
   // 1) The intended recipient is another platform and it does not exist in the simulation.
   // 2) The intended recipient is another platform and the transmitting comm device does not exist on the local platform.
   // 3) The intended recipient is a local processor and it does not exist on the local platform.
   //
   // In any of these cases it is assumed SendTaskMessage() has already issued a message, so there is no need to duplicate.
   //
   // In cases 1 and 2, any subsequent retries are disabled because this is really an unrecoverable user error.
   // In case 3 retries are still performed as the platform may have been killed, and we want to simulate the delay.
   // (for instance, assigning a task to a deleted platform...)

   WsfPlatform* senderPtr = aManagerPtr->GetPlatform();
   if (senderPtr->GetIndex() != aRecipientIndex)
   {
      // Off-board recipient. The local comm device must exist and the recipient platform must exist.
      if (senderPtr->GetComponent<wsf::comm::Comm>(aCommName) == nullptr)
      {
         mSuppressTimeoutMessage = true;
         mRetryAttempts          = 0;
      }
      else if (aManagerPtr->GetSimulation()->GetPlatformByIndex(aRecipientIndex) == nullptr)
      {
         mSuppressTimeoutMessage = true;
      }
   }
   else
   {
      // Local recipient. aCommName is the target processor.
      if (senderPtr->GetComponent<WsfProcessor>(aCommName) == nullptr)
      {
         mSuppressTimeoutMessage = true;
         mRetryAttempts          = 0;
      }
   }
}

// =================================================================================================
WsfTaskManager::PendingMessage::PendingMessage(const PendingMessage& aSrc)
   : mTimeSent(aSrc.mTimeSent)
   , mMessagePtr(aSrc.mMessagePtr->Clone())
   , mRecipientIndex(aSrc.mRecipientIndex)
   , mCommName(aSrc.mCommName)
   , mRetryAttempts(aSrc.mRetryAttempts)
   , mSuppressTimeoutMessage(aSrc.mSuppressTimeoutMessage)
{
}

// =================================================================================================
WsfTaskManager::PendingMessage& WsfTaskManager::PendingMessage::operator=(const PendingMessage& aRhs)
{
   if (this != &aRhs)
   {
      mTimeSent               = aRhs.mTimeSent;
      mMessagePtr             = aRhs.mMessagePtr->Clone();
      mRecipientIndex         = aRhs.mRecipientIndex;
      mCommName               = aRhs.mCommName;
      mRetryAttempts          = aRhs.mRetryAttempts;
      mSuppressTimeoutMessage = aRhs.mSuppressTimeoutMessage;
   }
   return *this;
}

// =================================================================================================
WsfTaskManager::PendingMessage::~PendingMessage()
{
   delete mMessagePtr;
}

// =================================================================================================
// Nested class 'EventBase'
// =================================================================================================
WsfTaskManager::EventBase::EventBase(double aSimTime, WsfTaskManager* aManagerPtr)
   : WsfEvent(aSimTime)
   , mManagerPtr(aManagerPtr)
   , mPlatformIndex(aManagerPtr->GetPlatform()->GetIndex())
{
}

WsfEvent::EventDisposition WsfTaskManager::EventBase::Execute()
{
   EventDisposition disposition = cDELETE;
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr)
   {
      disposition = ExecuteEvent(mManagerPtr);
   }
   return disposition;
}

// =================================================================================================
// Nested class 'TaskCompleteEvent'
// =================================================================================================
WsfTaskManager::TaskCompleteEvent::TaskCompleteEvent(double          aSimTime,
                                                     WsfTaskManager* aManagerPtr,
                                                     WsfTrackId      aTrackId,
                                                     WsfStringId     aTaskType,
                                                     WsfStringId     aResourceName,
                                                     WsfStringId     aSubStatus)
   : EventBase(aSimTime, aManagerPtr)
   , mTrackId(aTrackId)
   , mTaskType(aTaskType)
   , mResourceName(aResourceName)
   , mSubStatus(aSubStatus)
{
}

WsfEvent::EventDisposition WsfTaskManager::TaskCompleteEvent::ExecuteEvent(WsfTaskManager* aManagerPtr)
{
   aManagerPtr->ReportTaskComplete(GetTime(), mTrackId, mTaskType, mResourceName, mSubStatus);
   return cDELETE;
}
// =================================================================================================
// Nested class 'TrackUpdateEvent'
// =================================================================================================
WsfTaskManager::TrackUpdateEvent::TrackUpdateEvent(double aSimTime, WsfTaskManager* aManagerPtr, unsigned int aTaskId)
   : EventBase(aSimTime, aManagerPtr)
   , mTaskId(aTaskId)
{
}

WsfEvent::EventDisposition WsfTaskManager::TrackUpdateEvent::ExecuteEvent(WsfTaskManager* aManagerPtr)
{
   EventDisposition disposition    = cDELETE;
   double           nextUpdateTime = aManagerPtr->SendTrackUpdate(GetTime(), mTaskId);
   if (nextUpdateTime >= 0.0)
   {
      SetTime(nextUpdateTime);
      disposition = cRESCHEDULE;
   }
   return disposition;
}
