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

#ifndef WSFTASKMANAGER_HPP
#define WSFTASKMANAGER_HPP

#include <list>
#include <map>
#include <memory>
#include <set>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
class UtInput;
class WsfAssociationMessage;
#include "wsf_export.h"

#include "WsfEvent.hpp"
class WsfLocalTrack;
class WsfMessage;
class WsfPlatform;
#include "WsfProcessorComponent.hpp"
#include "WsfRandomVariable.hpp"
#include "script/WsfScriptProcessor.hpp"
class WsfSensor;
class WsfStatusMessage;
#include "WsfTask.hpp"
class WsfTaskAssignMessage;
class WsfTaskCancelMessage;
class WsfTaskControlMessage;
class WsfTaskStatusMessage;
#include "WsfTimeDelayQueue.hpp"
class WsfTrack;
class WsfTrackDropMessage;
#include "WsfTrackId.hpp"
class WsfTrackManager;
class WsfTrackMessage;

//! An abstract processor component that allows extension of the task manager.
//! Derived classes must provide the required component methods from WsfComponent
//! plus whatever methods optional methods from WsfComponent, WsfProcessorComponent
//! and WsfTaskManagerComponent as needed.
class WSF_EXPORT WsfTaskManagerComponent : public WsfProcessorComponent
{
public:
   //! @name Task manager component methods.
   //@{
   virtual void Update(double aSimTime) {}
   virtual void PlatformDeleted(WsfPlatform* aPlatformPtr) {}
   virtual bool AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack, bool& aStatusOk)
   {
      return false;
   }
   virtual bool ReleaseResource(double aSimTime, WsfTask& aTask) { return false; }

   virtual void TaskCorrelated(double aSimTime, WsfTask& aTask) {}
   virtual void TaskCanceled(double aSimTime, WsfTask& aTask) {}
   virtual void TaskCompleted(double aSimTime, WsfTask& aTask) {}

   virtual bool AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack = true)
   {
      return false;
   }

   virtual void PurgeReceivedTask(double aSimTime, WsfTask& aTask) {}
   virtual void PurgeTransmittedTask(double aSimTime, WsfTask& aTask) {}

   virtual void TrackDropped(double aSimTime, const WsfTrackId& aTrackId) {}

   virtual bool ProcessAssociationMessage(double aSimTime, const WsfAssociationMessage& aMessage) { return true; }
   virtual bool ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage) { return true; }
   //@}
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfTaskManagerComponent, cWSF_COMPONENT_TASK_MANAGER_COMPONENT)

//! WsfTaskManager is a class for task control.
//!
//! This class uses WsfTaskManagerBase for basic bookkeeping functions for sending and receiving task
//! assignments and the status information dealing with those assignments.
//!
//! This class implements various script features, like callbacks, for task events.
//!
//! In this implementation tasks are always associated with a track, and the track ID and an optional
//! user-selected 'task-type' are used to identify a task.
class WSF_EXPORT WsfTaskManager : public WsfScriptProcessor
{
public:
   //! typedef to simplify iteration for calling WsfTaskManagerComponent methods.
   using RoleIterator = WsfComponentList::RoleIterator<WsfTaskManagerComponent>;

   enum TrackUpdateStrategy
   {
      cDEFAULT,
      cBATCH
   };

   using TaskList = std::list<WsfTask>;

   //! Messages that have been sent but are waiting for a receipt.
   //! An entry is made into the 'pending message list' whenever a message must be sent 'reliably'.
   //! The transmission of a reliable message will be (re)attempted until either an acknowledgment
   //! is received or the number of retry attempts is exhausted.
   struct PendingMessage
   {
   public:
      PendingMessage(double            aSimTime,
                     WsfTaskManager*   aManagerPtr,
                     const WsfMessage& aMessage,
                     size_t            aReceipientIndex,
                     WsfStringId       aCommName,
                     int               aRetryAttempts);
      PendingMessage(const PendingMessage& aSrc);
      PendingMessage& operator=(const PendingMessage& aRhs);
      ~PendingMessage();

      double      mTimeSent;
      WsfMessage* mMessagePtr;
      size_t      mRecipientIndex;
      WsfStringId mCommName;
      int         mRetryAttempts;
      // See the constructor in the source code as to what this is for...
      bool mSuppressTimeoutMessage;
   };
   using PendingMessageList = std::list<PendingMessage>;

   //! A list of active track request.
   //! An entry is created in response to a WSF_ASSOCIATION_MESSAGE.
   //! An entry is removed when the sensor track is dropped.
   struct ActiveTrack
   {
      //! The track ID of the track that was supplied to WsfSensor::StartTracking.
      WsfTrackId mTaskTrackId;
      //! The name of the sensor that is generating the track.
      WsfStringId mSensorName;
      //! The sensor track that is being generated as a result of the tracking task.
      WsfTrackId mSensorTrackId;
   };
   using TrackList     = std::list<ActiveTrack>;
   using TrackListIter = std::list<ActiveTrack>::iterator;

   //! The sensor request list is a list sensor/track pairs that guide the updating of sensor requests
   //! when a track update is received. Entries are added when a sensor resource task is received and
   //! deleted when the task is deleted.
   struct SensorRequest
   {
      //! The track ID.
      WsfTrackId mSensorTrackId;
      //! The pointer to the sensor.
      WsfSensor* mSensorPtr;
      //! The mode for the sensor.
      WsfStringId mModeNameId;
   };
   using SensorRequestList = std::list<SensorRequest>;

   WsfTaskManager(WsfScenario& aScenario, const std::string& aClassName = "WsfTaskManager");
   WsfTaskManager& operator=(const WsfTaskManager&) = delete;
   ~WsfTaskManager() override                       = default;

   const char* GetScriptClassName() const override;

   //! @name Common framework methods
   //@{
   WsfProcessor* Clone() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;

   bool PreInitialize(double aSimTime) override;
   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   void Update(double aSimTime) override;
   //@}

   //! @name Command methods.
   //@{
   virtual bool AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack = true);

   virtual bool AssignTask(double                 aSimTime,
                           const WsfTrack&        aTrack,
                           WsfStringId            aTaskType,
                           const WsfTaskResource& aResource,
                           WsfPlatform*           aAssigneePtr,
                           WsfStringId            aCommName);

   virtual bool CancelTask(double aSimTime, unsigned int aTaskId);

   virtual void CancelTask(double aSimTime, WsfTask& aTask, bool aSendCancelMessage);

   virtual bool CancelTask(double                 aSimTime,
                           const WsfTrackId&      aTrackId,
                           WsfStringId            aTaskType,
                           const WsfTaskResource& aResource,
                           size_t                 aAssigneeIndex);

   virtual bool CreateDelayTask(double aSimTime, double aDelayTime, const WsfTrack& aTrack, WsfStringId aTaskType);

   virtual bool
   ChangeOperatingLevel(double aSimTime, WsfStringId aName, int aLevel, WsfPlatform* aAssigneePtr, WsfStringId aCommName);
   //@}

   //! @name Reporting methods.
   //! These methods are used by task assignees to report completion or progress of a task.
   //@{
   virtual bool ReportTaskComplete(double            aSimTime,
                                   const WsfTrackId& aTrackId,
                                   WsfStringId       aTaskType,
                                   WsfStringId       aResourceName,
                                   WsfStringId       aSubStatus);

   virtual bool ReportTaskProgress(double            aSimTime,
                                   const WsfTrackId& aTrackId,
                                   WsfStringId       aTaskType,
                                   WsfStringId       aResourceName,
                                   WsfStringId       aSubStatus);
   //@}

   //! @name Query methods
   //@{
   void   AssigneesForTask(const WsfTrackId& aTrackId, WsfStringId aTaskType, std::vector<size_t>& aAssignees) const;
   size_t AssigneeForTask(const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const;
   int    OperatingLevelFor(WsfStringId aConditionName) const;
   int    TasksAssignedTo(size_t aAssigneeIndex, const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const;
   int    TasksAssignedFor(const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const;
   int    TasksReceivedFor(const WsfTrackId& aTrackId, WsfStringId aTaskType, WsfStringId aResourceName) const;
   double TimeTaskAssigned(const WsfTrackId& aTrackId, WsfStringId aTaskType, size_t aAssigneeIndex) const;

   //! Return the list of tasks this TaskManager has assigned.
   TaskList& AssignedTaskList() { return mXmtdTaskList; }

   //! Return the list of tasks this TaskManager has received.
   TaskList& ReceivedTaskList() { return mRcvdTaskList; }

   static TaskList::iterator FindTask(TaskList&         aTaskList,
                                      size_t            aAssigneeIndex,
                                      const WsfTrackId& aTrackId,
                                      WsfStringId       aTaskType,
                                      WsfStringId       aResourceName);

   static TaskList::iterator FindTask(TaskList& aTaskList, unsigned int aTaskId);

   static TaskList::iterator FindTask(TaskList& aTaskList, unsigned int aTaskId, size_t aAssigneeIndex);

   //! Get the time a task assignment was last made.
   //! @return Time a task assignment was last made or < 0 if an assignment has never been made.
   double TimeLastTaskAssigned() const { return mTimeLastTaskAssigned; }

   //! Get the time a task assignment was last received.
   //! @return The time a task assignment was last received or < 0 if an assignment has never been received.
   double TimeLastTaskReceived() const { return mTimeLastTaskReceived; }

   //@}

   //! @name Callbacks
   //! These callbacks may be attached to, but should only be called from WsfTaskManager internally.
   //@{
   //! Executes when an assigned task is accepted (by this processor).
   UtCallbackListN<void(double, WsfTask&, const WsfTrack&)> OnTaskAssigned;

   //! Executes when a task cancel message is processed.
   UtCallbackListN<void(double, WsfTask&)> OnTaskCanceled;

   //! Executes when an task complete is received.
   UtCallbackListN<void(double, WsfTask&)> OnTaskCompleted;

   //! Executes when a task assignment is sent.
   UtCallbackListN<void(double, WsfTask&, const WsfTrack&)> OnTaskAssignSent;

   //! Executes when a task complete is sent.
   UtCallbackListN<void(double, WsfTask&, WsfStringId)> OnTaskCompleteSent;

   //! Executes when a task cancel is sent.
   UtCallbackListN<void(double, WsfTask&)> OnTaskCancelSent;
   //@}

   //! @name Internal methods.
   //! These methods are not intended for public use.
   //@{
   void CheckMessageAcknowledged(double aSimTime, unsigned int aSerialNumber);

   bool SendTaskMessage(double aSimTime, const WsfMessage& aMessage, size_t aRecipientIndex, WsfStringId aCommName, bool aReliable);

   double SendTrackUpdate(double aSimTime, unsigned int aTaskId);

   WsfStringId SelectCommDevice(WsfPlatform* aAssigneePtr, WsfStringId aCommName);
   //@}

   //! Return the list of tasks this TaskManager has active.
   const TrackList& ActiveTrackList() const { return mActiveTrackList; }

   bool ShowTaskMessages() const { return mShowTaskMessages; }

   //! @name Events.
   //! Some are public because they are used by derived classes or extensions.
   //! Others are private and exist with anonymous namespaces.
   //@{
   //! Base class for task manager events.
   //! It ensures the platform is still alive prior to calling ExecuteEvent()
   class WSF_EXPORT EventBase : public WsfEvent
   {
   public:
      EventBase(double aSimTime, WsfTaskManager* aManagerPtr);
      EventDisposition         Execute() override;
      virtual EventDisposition ExecuteEvent(WsfTaskManager* aTaskManagerPtr) = 0;

   private:
      WsfTaskManager* mManagerPtr;
      size_t          mPlatformIndex;
   };

   //! A event for scheduling the completion of a task
   class WSF_EXPORT TaskCompleteEvent : public WsfTaskManager::EventBase
   {
   public:
      TaskCompleteEvent(double          aSimTime,
                        WsfTaskManager* aManagerPtr,
                        WsfTrackId      aTrackId,
                        WsfStringId     aTaskType,
                        WsfStringId     aResourceName,
                        WsfStringId     aSubStatus);

      EventDisposition ExecuteEvent(WsfTaskManager* aManagerPtr) override;

   private:
      WsfTrackId  mTrackId;
      WsfStringId mTaskType;
      WsfStringId mResourceName;
      WsfStringId mSubStatus;
   };

   //! A event for sending track updates to assignees.
   class WSF_EXPORT TrackUpdateEvent : public WsfTaskManager::EventBase
   {
   public:
      TrackUpdateEvent(double aSimTime, WsfTaskManager* aManagerPtr, unsigned int aTaskId);

      EventDisposition ExecuteEvent(WsfTaskManager* aManagerPtr) override;

   private:
      unsigned int mTaskId;
   };
   //@}

protected:
   WsfTaskManager(const WsfTaskManager& aSrc);

   virtual bool AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack);

   virtual bool ReleaseResource(double aSimTime, WsfTask& aTask);

   virtual void TaskCanceled(double aSimTime, WsfTask& aTask);

   virtual void TaskCompleted(double aSimTime, WsfTask& aTask);

   virtual void TaskAssignSent(double aSimTime, const WsfTrack& aTrack, WsfTask& aTask);

   virtual void TaskAssigned(double aSimTime, const WsfTrack& aTrack, WsfTask& aTask);

   virtual void TaskCorrelated(double aSimTime, WsfTask& aTask);

   virtual void OperatingLevelChanged(double aSimTime, std::string aLevel);

   virtual void TrackDropped(double aSimTime, const WsfTrackId& aTrackId);

   bool DropFromPurgedTaskList(TaskList& aPurgedTaskList, size_t aAssignerIndex, unsigned int aTaskId);

   bool GetMessageSender(const WsfMessage& aMessage, size_t& aSenderIndex, WsfStringId& aRcvrCommName);

   void MessageAcknowledged(unsigned int aSerialNumber);

   //! @name Message handlers
   //@{
   virtual bool ProcessAssociationMessage(double aSimTime, const WsfAssociationMessage& aMessage);

   virtual bool ProcessTaskAssignMessage(double aSimTime, const WsfTaskAssignMessage& aMessage);

   virtual bool ProcessTaskCancelMessage(double aSimTime, const WsfTaskCancelMessage& aMessage);

   virtual bool ProcessTaskControlMessage(double aSimTime, const WsfTaskControlMessage& aMessage);

   virtual bool ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage);

   //! any sensor tasks for the dropped track will be reported complete/successful
   virtual bool ProcessTrackDropMessage(double aSimTime, const WsfTrackDropMessage& aMessage);

   virtual bool ProcessTrackMessage(double aSimTime, const WsfTrackMessage& aMessage);
   //@}

   //! The track manager that owns the tracks.
   WsfTrackManager* mTrackManagerPtr;

   //! The pointer to the 'thinker'.
   std::unique_ptr<WsfTimeDelayQueue> mThinkerPtr;

   PendingMessageList mPendingMessageList;
   TrackList          mActiveTrackList;
   SensorRequestList  mSensorRequestList;

   //! The operating level by condition name.
   //! These are assigned in the input file (or by default). The may be updated by ChangeOperatingLevel.
   //! The key is the string ID of some operating condition or state name.
   //! The value is the level of the operating condition or state.
   std::map<WsfStringId, int> mOperatingLevels;

   //! The sensors that have been implicitly turned on.
   std::set<WsfSensor*> mSensorsTurnedOn;

   WsfScenario* mScenarioPtr;

   UtCallbackHolder mCallbacks;

   //! Tasks I have sent to others.
   TaskList mXmtdTaskList;

   //! A list of transmitted tasks that have recently been canceled.
   //! An entry remains in this list until one of the following:
   //! - We receive a task completion message for the same task. This generally occurs if the assignee
   //! sends a task completion message before they receive the task cancel message from the assigner.
   //! - We receive acknowledgment of a task cancel message we sent.
   TaskList mPurgedXmtdTaskList;

   //! Tasks I have received from others.
   TaskList mRcvdTaskList;

   //! A list of received tasks that have been reported as completed.
   //! An entry remains in this list until one of the following:
   //! - We receive acknowledgment of the task completion we sent.
   //! - We receive a task cancel message that may have been sent by the assigner before it
   //! received the task completion message the assignee sent.
   TaskList mPurgedRcvdTaskList;

   //! The set of assigned track ID's that are awaiting correlation with local track ID's.
   std::set<WsfTrackId> mPendingRcvdTasks;

   //! The name of the track processor whose track manager is the source of tracks.
   WsfStringId mTrackProcessorName;

   //! The time interval for sending track updates to assignees.
   //! If zero then track updates are not sent.
   double mTrackUpdateInterval;

   //! The amount of time it takes to recognize a message.
   WsfRandomVariable mTimeToRecognizeMessage;

   //! The amount of time between attempts to retry failed communications.
   double mCommRetryInterval;

   //! The number of attempts to retry failed communications.
   int mCommRetryAttempts;

   //! The next task ID to be assigned.
   unsigned int mNextTaskId;

   //! True if task related messages are to be displayed
   bool mShowTaskMessages;

   //! True if leak termination data is to be shown.
   bool mShowUncompletedTasks;

   //! Determine how track updates are sent.
   TrackUpdateStrategy mTrackUpdateStrategy;

   //! The time a task was last sent.
   double mTimeLastTaskAssigned;

   //! The time a task was last received.
   double mTimeLastTaskReceived;

private:
   virtual void PurgeReceivedTask(double aSimTime, WsfTask& aTask);

   virtual void PurgeTransmittedTask(double aSimTime, WsfTask& aTask);

   void UpdatePendingReceivedTasks(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void UpdateSensorRequestList(double            aSimTime,
                                const WsfTrackId& aTrackId,
                                WsfSensor*        aSensorPtr,
                                WsfStringId       aModeNameId,
                                bool              aAddEntry);

   virtual void UpdateTaskCorrelation(double aSimTime, WsfTask& aTask);

   //! @name Track manager observer callbacks
   //@{
   //! cancels any sent tasks for the dropped track
   //! reports any received tasks for the track as complete/successful
   virtual void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   virtual void LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   virtual void LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   void TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr);

   //@}

   //! Pointers to various scripts (may be zero)
   //@{
   UtScript*      mOnTaskAssignScriptPtr;
   UtScript*      mOnTaskCancelScriptPtr;
   UtScript*      mOnTaskCompleteScriptPtr;
   UtScript*      mOnTaskCompleteSentScriptPtr;
   UtScript*      mOnTaskAssignSentScriptPtr;
   UtScript*      mOnTaskCancelSentScriptPtr;
   UtScript*      mOnOperatingLevelChangeScriptPtr;
   UtScriptData*  mTrackVarPtr;
   UtScriptClass* mTaskClassPtr;
   UtScriptClass* mTrackClassPtr;
   //@}
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfTaskManager, cWSF_COMPONENT_TASK_MANAGER)

#endif
