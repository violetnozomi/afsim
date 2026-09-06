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

#include "WsfWeaponTaskManager.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTaskManager.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"
#include "WsfUplinkProcessor.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "WsfWeaponTask.hpp"
#include "WsfWeaponTaskObserver.hpp"
#include "WsfWeaponTaskResource.hpp"

UtCallbackListN<void(const WsfWeapon*, bool&)> WsfWeaponTaskManager::IsWeaponReadyForLaunch;

namespace
{
//! The component factory that will attach a WsfWeaponTaskManager to ALL WsfTaskProcessors.
//! This enables all task manager instances to send and receive weapon-related tasks.
class WeaponTaskManagerComponentFactory : public WsfComponentFactory<WsfProcessor>
{
public:
   void PreInput(WsfProcessor& aParent) override
   {
      if (aParent.IsA_TypeOf("WSF_TASK_PROCESSOR"))
      {
         WsfWeaponTaskManager::FindOrCreate(aParent);
      }
   }
};

// ==============================================================================================
//! A event for delaying the uplink start
class BeginUplinkEvent : public WsfTaskManager::EventBase
{
public:
   BeginUplinkEvent(double aSimTime, WsfTaskManager* aManagerPtr, unsigned int aTaskId)
      : EventBase(aSimTime, aManagerPtr)
      , mTaskId(aTaskId)
   {
   }

   EventDisposition ExecuteEvent(WsfTaskManager* aTaskManagerPtr) override
   {
      WsfWeaponTaskManager* managerPtr = WsfWeaponTaskManager::Find(*dynamic_cast<WsfTaskManager*>(aTaskManagerPtr));
      if (managerPtr != nullptr)
      {
         managerPtr->UplinkStart(GetTime(), mTaskId);
      }
      return cDELETE;
   }

private:
   unsigned int mTaskId;
};
}; // namespace

// =================================================================================================
//! Register the component factory that handles input for this component.
void WsfWeaponTaskManager::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<WeaponTaskManagerComponentFactory>());
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
WsfWeaponTaskManager* WsfWeaponTaskManager::Find(const WsfProcessor& aParent)
{
   WsfWeaponTaskManager* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfWeaponTaskManager>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
WsfWeaponTaskManager* WsfWeaponTaskManager::FindOrCreate(WsfProcessor& aParent)
{
   WsfWeaponTaskManager* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new WsfWeaponTaskManager;
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
WsfWeaponTaskManager::WsfWeaponTaskManager()
   : mTaskManagerPtr(nullptr)
   , mCallbacks()
   , mWeaponTaskLog()
   , mWaitingTaskList()
   , mDefaultUplinkSource()
   , mDefaultUplinkComm()
   , mDefaultUplinkDelay(1.0E-3)
   , mUplinkSendInterval(0.0)
   , mAutoUplinkPlatform()
   , mAutoUplink(false)
{
}

// =================================================================================================
WsfWeaponTaskManager::WsfWeaponTaskManager(const WsfWeaponTaskManager& aSrc)
   : mTaskManagerPtr(nullptr)
   , mCallbacks()
   , mWeaponTaskLog()
   , mWaitingTaskList()
   , mDefaultUplinkSource(aSrc.mDefaultUplinkSource)
   , mDefaultUplinkComm(aSrc.mDefaultUplinkComm)
   , mDefaultUplinkDelay(aSrc.mDefaultUplinkDelay)
   , mUplinkSendInterval(aSrc.mUplinkSendInterval)
   , mAutoUplinkPlatform(aSrc.mAutoUplinkPlatform)
   , mAutoUplink(aSrc.mAutoUplink)
{
}

// =================================================================================================
WsfComponent* WsfWeaponTaskManager::CloneComponent() const
{
   return new WsfWeaponTaskManager(*this);
}

// =================================================================================================
WsfStringId WsfWeaponTaskManager::GetComponentName() const
{
   return UtStringIdLiteral("weapon_task_manager");
}

// =================================================================================================
const int* WsfWeaponTaskManager::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_WEAPON_TASK_MANAGER,
                               cWSF_COMPONENT_TASK_MANAGER_COMPONENT,
                               cWSF_COMPONENT_PROCESSOR,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* WsfWeaponTaskManager::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_WEAPON_TASK_MANAGER)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_TASK_MANAGER_COMPONENT)
   {
      return (WsfTaskManagerComponent*)this;
   }
   if (aRole == cWSF_COMPONENT_PROCESSOR)
   {
      return (WsfProcessorComponent*)this;
   }
   return nullptr;
}

// =================================================================================================
// virtual
bool WsfWeaponTaskManager::Initialize(double aSimTime)
{
   bool ok = true;

   // Eliminate lots of dynamic_cast'ing to get the pointer to the task manager.
   mTaskManagerPtr = dynamic_cast<WsfTaskManager*>(GetProcessor());
   assert(mTaskManagerPtr != nullptr);

   // Ensure the weapon uplink specifications are correct.
   WsfPlatform* platformPtr = GetPlatform();
   if ((platformPtr != nullptr) && (WsfUplinkProcessor::HasUplinkProcessor(*platformPtr)))
   {
      ok &= WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->VerifyUplinks();
   }

   // Subscribe to simulation events of interest.
   mCallbacks.Add(WsfObserver::WeaponFireAborted(GetSimulation()).Connect(&WsfWeaponTaskManager::WeaponFireAborted, this));
   mCallbacks.Add(WsfObserver::WeaponFired(GetSimulation()).Connect(&WsfWeaponTaskManager::WeaponFired, this));

   // Subscribe to task manager events of interest
   mCallbacks.Add(WsfObserver::StartingWeaponTask(GetSimulation()).Connect(&WsfWeaponTaskManager::StartingWeaponTask, this));

   // Subscribe to simulation events of interest.
   mCallbacks.Add(WsfObserver::WeaponTerminated(GetSimulation()).Connect(&WsfWeaponTaskManager::WeaponTerminated, this));

   return ok;
}

// =================================================================================================
// virtual
bool WsfWeaponTaskManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "weapon_uplink_path")
   {
      std::string sensorName;
      std::string commName;
      aInput.ReadValue(sensorName);
      aInput.ReadValue(commName);
      WsfPlatform* platformPtr = GetPlatform();
      if (platformPtr != nullptr/* &&
                          (WsfUplinkProcessor::GetUplinkProcessor(*platformPtr) != 0)*/)
      {
         WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->AddUplinkPath(sensorName, commName);
      }
   }
   else if (command == "uplink_source")
   {
      std::string uplinkSource;
      aInput.ReadValue(uplinkSource);
      mDefaultUplinkSource = uplinkSource;
   }
   else if (command == "uplink_comm")
   {
      std::string uplinkComm;
      aInput.ReadValue(uplinkComm);
      mDefaultUplinkComm = uplinkComm;
   }
   else if (command == "uplink_delay")
   {
      aInput.ReadValueOfType(mDefaultUplinkDelay, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDefaultUplinkDelay, 0.0);
      // Force a delay to allow the missile to initialize when uplinking to weapons
      if (mDefaultUplinkDelay < 1.0E-3)
      {
         mDefaultUplinkDelay = 1.0E-3;
      }
   }
   else if (command == "auto_weapon_uplink")
   {
      aInput.ReadValue(mAutoUplink);
   }
   else if (command == "auto_weapon_uplink_platform")
   {
      std::string uplinkPlatform;
      aInput.ReadValue(uplinkPlatform);
      mAutoUplinkPlatform = uplinkPlatform;
   }
   else if (command == "uplink_send_interval")
   {
      aInput.ReadValueOfType(mUplinkSendInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mUplinkSendInterval, 0.0);
      WsfPlatform* platformPtr = GetPlatform();
      if (platformPtr != nullptr/* &&
                          (WsfUplinkProcessor::GetUplinkProcessor(*platformPtr) != 0)*/)
      {
         WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->SetSendInterval(mUplinkSendInterval);
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfWeaponTaskManager::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   double simTime = GetSimulation()->GetSimTime();

   // Cancel any uplinks to the deleted platform.  This is a 2-step process because ReportTaskComplete will change mRcvdTaskList.
   WsfTaskManager::TaskList  completeTasks;
   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->ReceivedTaskList();
   for (WsfTask& task : taskList)
   {
      if (task.GetResourceType() == cTASK_RESOURCE_TYPE_UPLINK)
      {
         WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)task.GetResource();
         if (resource.mUplinkDestination == aPlatformPtr->GetName())
         {
            completeTasks.push_back(task);
         }
      }
   }
   for (WsfTask& task : completeTasks)
   {
      mTaskManagerPtr->ReportTaskComplete(simTime,
                                          task.GetTrackId(),
                                          task.GetTaskType(),
                                          task.GetResourceName(),
                                          GetProcessor()->GetScenario().Strings().cSUCCESSFUL);
   }
}

// =================================================================================================
//! Assign a task.
//! @param aSimTime       The current simulation time.
//! @param aTrack         The track that represents the objective of the assignment.
//! @param aTask          The task to be assigned.
//! @param aLockTrack     'true' if the track should be locked to prevent deletion.
// virtual
bool WsfWeaponTaskManager::AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack)
{
   WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(aTask.GetAssigneePlatformIndex());
   if (assigneePtr == nullptr)
   {
      return false;
   }

   if (TaskWaitToTransmit(aSimTime, aTrack, aTask))
   {
      return true;
   }

   if (!aTrack.GetTrackId().IsNull())
   {
      // If the resource is a weapon then create/update the weapon status data.
      if ((assigneePtr->GetComponent<WsfWeapon>(aTask.GetResourceName()) != nullptr) &&
          aTask.IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON)) // ignore the jammers
      {
         // log the fact that we assign a weapon task for this track (to a platform, with a resource)
         mWeaponTaskLog[aTrack.GetTrackId()].insert(std::make_pair(assigneePtr->GetIndex(), aTask.GetResourceName()));
      }
   }

   // see if task was just added
   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->AssignedTaskList();
   auto                      xtli     = WsfTaskManager::FindTask(taskList,
                                        assigneePtr->GetIndex(),
                                        aTrack.GetTrackId(),
                                        aTask.GetTaskType(),
                                        aTask.GetResourceName());
   if (xtli != taskList.end())
   {
      WsfTask& task = *xtli;

      bool newTask = (task.GetUpdateTime() == aSimTime);
      if (newTask && mAutoUplink && task.GetResourceType() == cTASK_RESOURCE_TYPE_WEAPON)
      {
         BeginAutoUplink(aSimTime, aTrack, task);
      }
   }

   return true;
}

// =================================================================================================
// protected virtual
bool WsfWeaponTaskManager::AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack, bool& aStatusOk)
{
   aStatusOk = true;

   // Return immediately if no resource is associated with the task.
   WsfStringId resourceName(aTask.GetResourceName());
   if (resourceName.IsNull())
   {
      return false;
   }

   bool       acquiredResource = false;
   WsfWeapon* weaponPtr        = nullptr;
   if ((weaponPtr = GetPlatform()->GetComponent<WsfWeapon>(resourceName)) != nullptr)
   {
      if (aTask.IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON))
      {
         WsfWeaponTaskResource& resource      = static_cast<WsfWeaponTaskResource&>(aTask.GetResource());
         int                    resourceCount = resource.mCount;
         if (resourceCount < 0)
         {
            resourceCount = 1;
         }
         if (weaponPtr->FireSalvo(aSimTime, &aTrack, resourceCount))
         {
            aTask.SetObjectsPending(resourceCount);
            WsfObserver::StartingWeaponTask(
               GetSimulation())(aSimTime, aTask); // Directly inform the assigner (for weapon counting)
         }
         else
         {
            auto out = ut::log::warning() << "Failed FireSalvo for track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Weapon: " << weaponPtr->GetName();
            out.AddNote() << "Track Id: " << aTrack.GetTrackId();
            aStatusOk = false;
         }
         acquiredResource = true;
      }
      else if (aTask.IsResourceTypeOf(cTASK_RESOURCE_TYPE_JAMMER))
      {
         WsfJammerTaskResource& resource = static_cast<WsfJammerTaskResource&>(aTask.GetResource());
         if (resource.mFrequency > 0.0)
         {
            aStatusOk = weaponPtr->StartJamming(aSimTime,
                                                resource.mFrequency,
                                                resource.mBandwidth,
                                                resource.mBeamNumber,
                                                resource.mTechniqueName,
                                                aTrack.GetTrackId(),
                                                aTrack.GetTargetIndex());
         }
         else
         {
            aStatusOk = weaponPtr->StartJammingTrack(aSimTime, resource.mTechniqueName, &aTrack);
         }

         if ((aStatusOk == false) && GetProcessor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Failed StartJamming on Track Id.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Weapon: " << weaponPtr->GetName();
            out.AddNote() << "Track Id: " << aTrack.GetTrackId().GetOwningPlatformId() << ":"
                          << aTrack.GetTrackId().GetLocalTrackNumber();
            out.AddNote() << "Target Index: " << aTrack.GetTargetIndex();
            if (resource.mFrequency > 0.0)
            {
               out.AddNote() << "Frequency: " << resource.mFrequency;
               out.AddNote() << "Bandwidth: " << resource.mBandwidth;
            }
            if (resource.mBeamNumber > 0)
            {
               out.AddNote() << "Beam#: " << resource.mBeamNumber;
            }
         }
         acquiredResource = true;
      }
   }
   return acquiredResource;
}


// =================================================================================================
// protected virtual
bool WsfWeaponTaskManager::ReleaseResource(double aSimTime, WsfTask& aTask)
{
   // Return immediately if no resource is associated with the task.
   WsfStringId resourceName(aTask.GetResourceName());
   if (resourceName.IsNull())
   {
      return false;
   }

   bool       ok        = false;
   WsfWeapon* weaponPtr = nullptr;
   if ((weaponPtr = GetPlatform()->GetComponent<WsfWeapon>(resourceName)) != nullptr)
   {
      if (aTask.IsResourceTypeOf(cTASK_RESOURCE_TYPE_JAMMER))
      {
         WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aTask.GetResource();
         if (weaponPtr->StopJamming(aSimTime, resource.mFrequency, resource.mBandwidth, resource.mBeamNumber, aTask.GetTrackId()))
         {
            ok = true;
         }
         else
         {
            if (GetProcessor()->DebugEnabled())
            {
               { // RAII block
                  auto out = ut::log::warning() << "Failed StopJamming for frequency.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Weapon: " << weaponPtr->GetName();
                  out.AddNote() << "Frequency: " << resource.mFrequency;
                  out.AddNote() << "Bandwidth: " << resource.mBandwidth;
                  if (resource.mBeamNumber > 0)
                  {
                     out.AddNote() << "Beam: " << resource.mBeamNumber;
                  }
               }
               { // RAII block
                  auto out = ut::log::warning() << "Failed StopJamming on Track Id.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Weapon: " << weaponPtr->GetName();
                  out.AddNote() << "Track Id: " << aTask.GetLocalTrackId().GetOwningPlatformId() << ":"
                                << aTask.GetTrackId().GetLocalTrackNumber();
                  out.AddNote() << "Frequency: " << resource.mFrequency;
                  out.AddNote() << "Bandwidth: " << resource.mBandwidth;
                  if (resource.mBeamNumber > 0)
                  {
                     out.AddNote() << "Beam#: " << resource.mBeamNumber;
                  }
               }
            }
         }
      }
      ok = true; // processed
   }
   return ok;
}

// =================================================================================================
// virtual
void WsfWeaponTaskManager::TrackDropped(double aSimTime, const WsfTrackId& aTrackId)
{
   // remove the log entry for any weapon tasks for this track
   auto it = mWeaponTaskLog.find(aTrackId);
   if (it != mWeaponTaskLog.end())
   {
      mWeaponTaskLog.erase(it);
   }
}

// =================================================================================================
//! Process the sensor track request association message.
//!
//! When a sensor initiates a track that is the result of StartTracking() request, it will send a
//! WSF_TRACK_ASSOCIATION_MESSAGE prior to sending the first WSF_TRACK_MESSAGE. This information
//! provides the track ID of the StartTracking() request and the track ID of the track that is
//! being produced as a result of that request.
//!
//! This information is used to potentially uplink track data to a launched missile.
//!
//! @returns 'false' to allow other message listeners to look at the message.
// private
bool WsfWeaponTaskManager::ProcessAssociationMessage(double aSimTime, const WsfAssociationMessage& aMessage)
{
   // If this processor is acting as an uplink router then cache the request/track association data.
   WsfPlatform* platformPtr = GetPlatform();
   if ((platformPtr == nullptr) || (!WsfUplinkProcessor::HasUplinkProcessor(*platformPtr)) ||
       WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->IsUplinkPathListEmpty())
   {
      return false;
   }

   // LBM - this is not the place to rebuild the uplink list, no adds or removes will take place here
   //       the mWeaponTaskLog[] needs added to before a new entry in the mActiveTrackList means anything
   // LBM - this is done on the uplink processor now
   return true;
}

// =================================================================================================
// private
bool WsfWeaponTaskManager::ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage)
{
   bool messageProcessed = false;
   if (aMessage.GetStatus() == GetProcessor()->GetScenario().Strings().cOBJECT_RELEASED)
   {
      // Initiate a waiting uplink task if one matches the weapon task
      WsfPlatform* weaponPlatformPtr = GetSimulation()->GetPlatformByIndex(aMessage.GetObjectPlatformIndex());
      if (weaponPlatformPtr != nullptr)
      {
         for (WsfTask& task : mWaitingTaskList)
         {
            if ((task.GetTaskType() == aMessage.GetTaskType()) && (task.GetTrackId() == aMessage.GetTrackId()) &&
                (task.GetResourceType() == cTASK_RESOURCE_TYPE_UPLINK))
            {
               WsfTask                uplinkTask(task);
               WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)uplinkTask.GetResource();
               resource.mUplinkDestination     = weaponPlatformPtr->GetName();
               WsfTrack* weaponTrackPtr        = GetPlatform()->GetTrackManager().FindTrack(task.GetTrackId());
               if (weaponTrackPtr != nullptr)
               {
                  mTaskManagerPtr->AssignTask(aSimTime, *weaponTrackPtr, uplinkTask);
               }
            }
         }
      }
      messageProcessed = true;
   }
   return messageProcessed;
}

// =================================================================================================
//! Perform the actions necessary to purge a received task.
//! This routine is invoked by the receiver of a task after it has completed the task.
//!
//! @param aSimTime The current simulation time.
//! @param aTask    The task being purged.
//! @note The caller is responsible for deleting the task from the received task list.
// private
void WsfWeaponTaskManager::PurgeReceivedTask(double aSimTime, WsfTask& aTask)
{
   WsfPlatform* platformPtr = GetPlatform();
   if ((platformPtr != nullptr) && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->RemoveUplink(aTask.GetTaskId());
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
void WsfWeaponTaskManager::PurgeTransmittedTask(double aSimTime, WsfTask& aTask)
{
   // Remove entries from mWaitingTaskList if no other transmitted tasks match
   for (auto i = mWaitingTaskList.begin(); i != mWaitingTaskList.end();)
   {
      WsfTaskManager::TaskList::iterator waitIter = i++;
      WsfTask&                           waitTask = *waitIter;
      if ((waitTask.GetTrackId() == aTask.GetTrackId()) && (waitTask.GetTaskType() == aTask.GetTaskType()))
      {
         bool                      haveXmtdTask = false;
         WsfTaskManager::TaskList& taskList     = mTaskManagerPtr->AssignedTaskList();
         for (const WsfTask& xmtTask : taskList)
         {
            if ((xmtTask.GetTrackId() == waitTask.GetTrackId()) && (xmtTask.GetTaskType() == waitTask.GetTaskType()))
            {
               haveXmtdTask = true;
               break;
            }
         }
         if (!haveXmtdTask)
         {
            mWaitingTaskList.erase(waitIter);
         }
      }
   }
}

// =================================================================================================
//! Called when a task has been correlated with a local track
void WsfWeaponTaskManager::TaskCorrelated(double aSimTime, WsfTask& aTask)
{
   if (aTask.GetResourceType() == cTASK_RESOURCE_TYPE_UPLINK)
   {
      WsfUplinkTaskResource& resource    = (WsfUplinkTaskResource&)aTask.GetResource();
      double                 uplinkDelay = resource.mUplinkDelay;
      if (uplinkDelay < 0.0)
      {
         uplinkDelay = mDefaultUplinkDelay;
      }
      // Begin uplinking after the uplink delay
      GetSimulation()->AddEvent(
         ut::make_unique<BeginUplinkEvent>(aSimTime + uplinkDelay, mTaskManagerPtr, aTask.GetTaskId()));
   }
}

// =================================================================================================
// virtual
void WsfWeaponTaskManager::TaskCanceled(double aSimTime, WsfTask& aTask)
{
   // TOOD-AWK this was moved from bool WsfTaskManagerBase::ProcessTaskCancelMessage(...)
   // after the TaskCancel(...) call NEED TO TEST that it work properly.

   // If this is a weapon task then also cancel any pending shots... (This isn't done in ReleaseResource
   // because that also handles a normal resource release - which this is not...)

   if (aTask.GetResourceName() != 0)
   {
      WsfWeapon* weaponPtr = GetPlatform()->GetComponent<WsfWeapon>(aTask.GetResourceName());
      if (weaponPtr != nullptr)
      {
         weaponPtr->AbortSalvo(aSimTime, aTask.GetTrackId());
      }
   }
}

// =================================================================================================
//! Return the simulation time of the most recent weapon firing against the specified track.
double WsfWeaponTaskManager::TimeWeaponLastFiredFor(const WsfTrackId& aTrackId)
{
   double simTime = -1.0;

   auto lit = mWeaponTaskLog.find(aTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      for (const auto& entry : lit->second)
      {
         WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(entry.first);
         if (assigneePtr != nullptr)
         {
            simTime = std::max(simTime, WsfWeapon::TimeWeaponLastFiredFor(*assigneePtr, aTrackId, entry.second));
         }
      }
   }
   if (simTime < 0.0)
   {
      simTime = std::numeric_limits<double>::max();
   }
   return simTime;
}

// =================================================================================================
//! Return the simulation time of the most recent weapon termination against the specified track.
double WsfWeaponTaskManager::TimeWeaponLastTerminatedFor(const WsfTrackId& aTrackId)
{
   double simTime = -1.0;

   auto lit = mWeaponTaskLog.find(aTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      for (const auto& entry : lit->second)
      {
         WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(entry.first);
         if (assigneePtr != nullptr)
         {
            simTime = std::max(simTime, WsfWeapon::TimeWeaponLastTerminatedFor(*assigneePtr, aTrackId, entry.second));
         }
      }
   }
   if (simTime < 0.0)
   {
      simTime = std::numeric_limits<double>::max();
   }
   return simTime;
}

// =================================================================================================
//! Return the count of the number of weapons that are active against the specified track.
//! @param aTrackId The local track ID of the track of interest.
int WsfWeaponTaskManager::WeaponsActiveFor(const WsfTrackId& aTrackId)
{
   return WeaponsActiveFor(aTrackId, nullptr); // Simply invoke the more general form.
}

// =================================================================================================
//! Return the count of the number of weapons that are active against the specified track for the given platform.
//! @param aTrackId The local track ID of the track of interest.
//! @param aPlatformPtr The assignee platform of interest.
int WsfWeaponTaskManager::WeaponsActiveFor(const WsfTrackId& aTrackId, WsfPlatform* aPlatformPtr)
{
   int    weaponCount   = 0;
   size_t assigneeIndex = 0;
   if (aPlatformPtr != nullptr)
   {
      assigneeIndex = aPlatformPtr->GetIndex();
   }

   // Count uncommitted rounds in outstanding transmitted weapon tasks.
   // These are counted as active until they are aborted or terminated.
   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->AssignedTaskList();
   for (const WsfTask& task : taskList)
   {
      if ((task.GetResourceName() != 0) && task.IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON))
      {
         if ((aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())) &&
             ((assigneeIndex == 0) || (assigneeIndex == task.GetAssigneePlatformIndex())))
         {
            weaponCount += ((WsfWeaponTaskResource&)task.GetResource()).mCount;
         }
      }
   }

   // Count rounds that have been fired (or in the process of being fired)
   auto lit = mWeaponTaskLog.find(aTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      for (const auto& entry : lit->second)
      {
         if ((assigneeIndex == 0) || (assigneeIndex == entry.first))
         {
            WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(entry.first);
            if (assigneePtr != nullptr)
            {
               weaponCount += WsfWeapon::WeaponsActiveAndPendingFor(*assigneePtr, aTrackId, entry.second);
            }
         }
      }
   }
   return weaponCount;
}

// =================================================================================================
//! Return the count of the number of salvos that have been fired at the specified track.
//! @param aTrackId The local track ID of the track of interest.
int WsfWeaponTaskManager::SalvosFiredAt(const WsfTrackId& aTrackId)
{
   int salvosFired = 0;

   auto lit = mWeaponTaskLog.find(aTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      for (const auto& entry : lit->second)
      {
         WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(entry.first);
         if (assigneePtr != nullptr)
         {
            salvosFired += WsfWeapon::SalvosFiredAt(*assigneePtr, aTrackId, entry.second);
         }
      }
   }
   return salvosFired;
}

// =================================================================================================
//! Return the count of the number of rounds that have been fired at the specified track .
//! @param aTrackId The local track ID of the track of interest.
int WsfWeaponTaskManager::RoundsFiredAt(const WsfTrackId& aTrackId)
{
   return RoundsFiredAt(aTrackId, nullptr); // Invoke the more general method
}

// =================================================================================================
//! Return the count of the number of rounds that have been fired at the specified track for a given platform.
//! @param aTrackId The local track ID of the track of interest.
//! @param aPlatformPtr The assignee platform.
int WsfWeaponTaskManager::RoundsFiredAt(const WsfTrackId& aTrackId, WsfPlatform* aPlatformPtr)
{
   int roundsFired = 0;

   size_t assigneeIndex = 0;
   if (aPlatformPtr != nullptr)
   {
      assigneeIndex = aPlatformPtr->GetIndex();
   }

   auto lit = mWeaponTaskLog.find(aTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      for (const auto& entry : lit->second)
      {
         if ((assigneeIndex == 0) || (assigneeIndex == entry.first))
         {
            WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(entry.first);
            if (assigneePtr != nullptr)
            {
               roundsFired += WsfWeapon::RoundsFiredAt(*assigneePtr, aTrackId, entry.second);
            }
         }
      }
   }
   return roundsFired;
}

// =================================================================================================
//! Internal method to begin uplinking a track as requested by a received task.
//! This method is invoked by BeginUplinkEvent.
//! @note This is not part of the external interface, and should not be called by users.
void WsfWeaponTaskManager::UplinkStart(double aSimTime, unsigned int aTaskId)
{
   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->ReceivedTaskList();
   auto                      tli      = WsfTaskManager::FindTask(taskList, aTaskId);
   if (tli != taskList.end())
   {
      WsfTask&               task           = *tli;
      WsfUplinkTaskResource& resource       = (WsfUplinkTaskResource&)task.GetResource();
      WsfPlatform*           destinationPtr = GetSimulation()->GetPlatformByName(resource.mUplinkDestination);
      if (destinationPtr != nullptr)
      {
         WsfStringId uplinkComm = resource.mUplinkCommName;
         if (uplinkComm == nullptr)
         {
            uplinkComm = mDefaultUplinkComm;
         }
         uplinkComm                     = mTaskManagerPtr->SelectCommDevice(destinationPtr, uplinkComm);
         wsf::comm::Comm* uplinkCommPtr = GetPlatform()->GetComponent<wsf::comm::Comm>(uplinkComm);
         if (uplinkCommPtr != nullptr)
         {
            if (resource.mUplinkSource == nullptr)
            {
               resource.mUplinkSource = mDefaultUplinkSource;
            }
            // match up the sensor track ID if it's correlated
            WsfTrackId sensorTrackId;
            if (resource.mUplinkSource != 0)
            {
               WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().FindTrack(task.GetLocalTrackId());
               if (localTrackPtr != nullptr)
               {
                  const WsfLocalTrack::RawTrackIdList& rawTrackIds = localTrackPtr->GetRawTrackIds();
                  for (unsigned int i = 0; i < rawTrackIds.GetCount(); ++i)
                  {
                     const WsfTrackId* idPtr    = rawTrackIds.GetEntry(i);
                     WsfTrack*         trackPtr = GetPlatform()->GetTrackManager().FindRawTrack(*idPtr);
                     if (trackPtr != nullptr)
                     {
                        if (trackPtr->GetSensorNameId() == resource.mUplinkSource)
                        {
                           sensorTrackId = trackPtr->GetTrackId();
                        }
                     }
                  }
               }
            }
            WsfPlatform* platformPtr = GetPlatform();
            if (platformPtr != nullptr)
            {
               WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)
                  ->AddUplink(task.GetTrackId(),
                              task.GetLocalTrackId(),
                              sensorTrackId,
                              destinationPtr,
                              uplinkCommPtr,
                              resource.mUplinkSource,
                              aTaskId,
                              -1.0E20);
            }
         }
         else if (GetProcessor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "No comm found to uplink to target.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Processor: " << GetProcessor()->GetName();
            out.AddNote() << "Target: " << resource.mUplinkDestination;
         }
      }
      else if (GetProcessor()->DebugEnabled())
      {
         auto out = ut::log::debug() << "Uplink Start:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetProcessor()->GetName();
         out.AddNote() << "No Destination Found: " << resource.mUplinkDestination;
      }
   }
}

// =================================================================================================
//! Abort weapon firing against a specified target.
//! This is like a CancelTask for a weapon task except that it goes further and aborts ANY inflight rounds
//! against the specified track ID that were initiated via command from this task manager.
// virtual
bool WsfWeaponTaskManager::AbortFiring(double aSimTime, const WsfTrackId& aTrackId)
{
   auto lit = mWeaponTaskLog.find(aTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      for (const auto& entry : lit->second)
      {
         WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(entry.first);
         if (assigneePtr != nullptr)
         {
            auto weaponPlatforms = WsfWeapon::ActiveWeaponPlatformsFor(*assigneePtr, aTrackId, entry.second);
            for (size_t weaponPlatform : weaponPlatforms)
            {
               // Delete the platform. This will terminate the weapon engagement...
               WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(weaponPlatform);
               if (platformPtr != nullptr)
               {
                  WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*platformPtr);
                  if (engagementPtr != nullptr)
                  {
                     engagementPtr->SetExtendedResult("Aborted by command");
                     engagementPtr->Terminate(aSimTime, WsfWeaponEngagement::cDUD);
                  }
                  else
                  {
                     GetSimulation()->DeletePlatform(aSimTime, platformPtr);
                  }
               }
            }
         }
      }
   }

   // Cancel any uncompleted weapon requests.

   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->AssignedTaskList();
   auto                      xtli     = taskList.begin();
   while (xtli != taskList.end())
   {
      WsfTask& task = *xtli;
      if (task.IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON) && (aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())))
      {
         // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task list before
         // proceeding.
         //        A temporary list is used to store the task before deletion.
         WsfTaskManager::TaskList removedTaskList;
         removedTaskList.splice(removedTaskList.end(), taskList, xtli++);

         mTaskManagerPtr->CancelTask(aSimTime, removedTaskList.front(), true);
      }
      else
      {
         ++xtli;
      }
   }
   return true;
}

// =================================================================================================
//! A simulation observer callback to handle notification that a weapon firing request was aborted.
//!
//! If the callback was invoked as the result of a weapon assignment from this platform, decrement
//! the pending weapon count to account for the fact that some of the weapons were not released.
//!
//! @param aSimTime        The current simulation time.
//! @param aWeaponPtr      A pointer to the weapon.
//! @param aTargetTrackPtr A pointer to the target track (may be 0).
//! @param aQuantity       The quantity of the weapon for which firing was requested but not completed.
// protected virtual
void WsfWeaponTaskManager::WeaponFireAborted(double          aSimTime,
                                             WsfWeapon*      aWeaponPtr,
                                             const WsfTrack* aTargetTrackPtr,
                                             double          aQuantity)
{
   if ((aWeaponPtr == nullptr) || (aTargetTrackPtr == nullptr))
   {
      return;
   }

   const WsfTrackId& targetTrackId    = aTargetTrackPtr->GetTrackId();
   size_t            assigneeIndex    = aWeaponPtr->GetPlatform()->GetIndex();
   WsfStringId       weaponSystemName = aWeaponPtr->GetNameId();

   // Find the received task for this weapon launch and decrement the number of weapons that remain to be launched.
   // Find the received task for this weapon launch. If the number of weapons that remain to be
   // launched has gone to zero then send back the response that indicates the task has been completed.
   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->ReceivedTaskList();
   for (WsfTask& task : taskList)
   {
      if ((task.GetTrackId() == targetTrackId) || (task.GetLocalTrackId() == targetTrackId))
      {
         if ((task.GetAssigneePlatformIndex() == assigneeIndex) && (task.GetResourceName() == weaponSystemName))
         {
            int weaponsPending = std::max(0, task.GetObjectsPending() - static_cast<int>(aQuantity));
            task.SetObjectsPending(weaponsPending);
            if (task.GetObjectsPending() <= 0)
            {
               mTaskManagerPtr->ReportTaskComplete(aSimTime,
                                                   task.GetLocalTrackId(),
                                                   task.GetTaskType(),
                                                   task.GetResourceName(),
                                                   GetProcessor()->GetScenario().Strings().cUNSUCCESSFUL);
            }
            break;
         }
      }
   }
}

// =================================================================================================
//! A simulation observer callback to handle weapon firing events.
//! @param aSimTime        The current simulation time.
//! @param aEngagementPtr  A pointer to the weapon engagement data.
//! @param aTargetTrackPtr A pointer to the target track (may be 0).
// private
void WsfWeaponTaskManager::WeaponFired(double                     aSimTime,
                                       const WsfWeaponEngagement* aEngagementPtr,
                                       const WsfTrack*            aTargetTrackPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetInitialTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }
   const WsfWeapon* weaponPtr = aEngagementPtr->GetWeaponSystem();
   if (weaponPtr == nullptr)
   {
      return;
   }
   // Other applications may decide the weapon state, allow for those weapons to
   // control part of the launch sequence.
   bool bWeaponReady = true;
   IsWeaponReadyForLaunch(weaponPtr, bWeaponReady);
   if (!bWeaponReady)
   {
      return;
   }

   size_t      assigneeIndex    = aEngagementPtr->GetFiringPlatformIndex();
   WsfStringId weaponSystemName = aEngagementPtr->GetWeaponSystemName();

   // If this weapon launch is associated to an assignment from this platform then the uplinks might need updating
   bool isTaskAssigner = false;
   auto lit            = mWeaponTaskLog.find(targetTrackId);
   if (lit != mWeaponTaskLog.end())
   {
      auto eit = lit->second.find(std::make_pair(assigneeIndex, weaponSystemName));
      if (eit != lit->second.end())
      {
         isTaskAssigner = true;
      }
   }

   // Find the received task for this weapon launch and decrement the number of weapons that remain to be
   // launched. Send task status message for weapon release
   // Find the received task for this weapon launch.  If the number of weapons that remain to be
   // launched has gone to zero then send back the response that indicates the task has been completed.
   WsfTaskManager::TaskList& taskList = mTaskManagerPtr->ReceivedTaskList();
   for (WsfTask& task : taskList)
   {
      if ((task.GetTrackId() == targetTrackId) || (task.GetLocalTrackId() == targetTrackId))
      {
         if ((task.GetAssigneePlatformIndex() == assigneeIndex) && (task.GetResourceName() == weaponSystemName))
         {
            int weaponsPending = std::max(0, task.GetObjectsPending() - 1);
            task.SetObjectsPending(weaponsPending);
            WsfTaskStatusMessage weaponReleasedMessage(GetProcessor()->GetScenario().Strings().cOBJECT_RELEASED,
                                                       GetPlatform(),
                                                       task);
            weaponReleasedMessage.SetObjectPlatformIndex(aEngagementPtr->GetWeaponPlatformIndex());
            mTaskManagerPtr->SendTaskMessage(aSimTime,
                                             weaponReleasedMessage,
                                             task.GetAssignerPlatformIndex(),
                                             task.GetCommName(),
                                             false);

            if (task.GetObjectsPending() <= 0)
            {
               GetSimulation()->AddEvent(
                  ut::make_unique<WsfTaskManager::TaskCompleteEvent>(aSimTime + .01,
                                                                     mTaskManagerPtr,
                                                                     task.GetLocalTrackId(),
                                                                     task.GetTaskType(),
                                                                     task.GetResourceName(),
                                                                     GetProcessor()->GetScenario().Strings().cSUCCESSFUL));
            }
            break;
         }
      }
   }

   // The uplink lists might needed updating if one of the following is true:
   //
   // - This is the processor that assigned the weapon resource task that resulted in the launch of the weapon.
   //   This is needed in order to abort in-flight rounds.
   // - This processor has an 'uplink path' that may be needed to uplink data to the weapon.
   //
   // At the current time this only applies to explicitly modeled weapons.
   WsfPlatform* platformPtr = GetPlatform();
   if ((platformPtr != nullptr) && (aEngagementPtr->GetWeaponPlatformIndex() != 0) &&
       WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      WsfUplinkProcessor* uplinkProcessor = WsfUplinkProcessor::GetUplinkProcessor(*platformPtr);
      if (uplinkProcessor == nullptr)
      {
         return;
      }

      if (isTaskAssigner || (!uplinkProcessor->IsUplinkPathListEmpty()))
      {
         if (mTaskManagerPtr->ShowTaskMessages())
         {
            auto out = ut::log::info() << "Received WeaponFired for active weapon.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Processor: " << GetProcessor()->GetName();
            out.AddNote() << "Target Track Id: " << targetTrackId;
            out.AddNote() << "Sequence: " << aEngagementPtr->GetSerialNumber();
         }
         // have to add an entry (as if we fired this weapon) so we can uplink to it
         if (!uplinkProcessor->IsUplinkPathListEmpty())
         {
            const WsfTrackId&                             newTargetTrackId = aTargetTrackPtr->GetTrackId();
            std::pair<WeaponTaskLogEntry::iterator, bool> ret;
            ret = mWeaponTaskLog[newTargetTrackId].insert(std::make_pair(assigneeIndex, weaponSystemName));

            // LBM - do not rebuild the uplink list every time, just insert new additions now
            auto                      eit       = ret.first;
            WsfTaskManager::TrackList trackList = mTaskManagerPtr->ActiveTrackList();
            for (const WsfTaskManager::ActiveTrack& activeTrack : trackList)
            {
               if (activeTrack.mTaskTrackId != newTargetTrackId)
               {
                  continue;
               }
               // Determine if the source sensor is one that can be used for uplink.
               wsf::comm::Comm* commPtr = uplinkProcessor->GetUplinkCommForTrack(activeTrack.mSensorName);
               if (commPtr == nullptr)
               {
                  continue;
               }
               WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByIndex(eit->first);
               if (assigneePtr == nullptr)
               {
                  continue;
               }
               WsfPlatform* weaponPlatformPtr = aEngagementPtr->GetWeaponPlatform();
               if (weaponPlatformPtr != nullptr)
               {
                  uplinkProcessor->AddTrackUplink(activeTrack.mSensorTrackId,
                                                  weaponPlatformPtr,
                                                  commPtr,
                                                  aEngagementPtr->GetTargetPlatformIndex());
               }
            }
         }
      }
   }
}

// =================================================================================================
//! A simulation observer callback to handle weapon termination events.
//! @param aSimTime        The current simulation time.
//! @param aEngagementPtr  A pointer to the weapon engagement data.
// private
void WsfWeaponTaskManager::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetInitialTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }

   // LBM - do not rebuild whole list, only removed uplinks for weapon that is now gone
   //       this is done on the uplink processor now
}

// =================================================================================================
//! A task manager callback to handle notification that a successful FireSalvo has been issued.
//!
//! If the callback was invoked due to an assignment from this platform, increment the pending round count.
//!
//! @param aSimTime  The current simulation time.
//! @param aTask     The weapon task being started (as viewed by the assignee).
// private
void WsfWeaponTaskManager::StartingWeaponTask(double aSimTime, const WsfTask& aTask)
{
   if ((aTask.GetResourceName() == nullptr) || (!aTask.IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON)))
   {
      return;
   }

   // WsfWeaponTaskResource& taskResource = (WsfWeaponTaskResource&)aTask.GetResource();
   size_t                    assigneeIndex = aTask.GetAssigneePlatformIndex();
   const WsfTrackId&         trackId       = aTask.GetTrackId(); // The track ID as seen by the assigner.
   WsfStringId               taskType      = aTask.GetTaskType();
   WsfStringId               resourceName  = aTask.GetResourceName();
   WsfTaskManager::TaskList& taskList      = mTaskManagerPtr->AssignedTaskList();
   auto                      xtli = WsfTaskManager::FindTask(taskList, assigneeIndex, trackId, taskType, resourceName);
   if (xtli != taskList.end())
   {
      // Clear the resource count in the task and increment the weapons pending count in the 'weapon status'.
      // This ensures pending rounds are only counted once in WeaponsActiveFor().

      WsfTask&               task     = *xtli;
      WsfWeaponTaskResource& resource = static_cast<WsfWeaponTaskResource&>(task.GetResource());
      resource.mCount                 = 0;
   }
}

// =================================================================================================
void WsfWeaponTaskManager::BeginAutoUplink(double aSimTime, const WsfTrack& aTrack, const WsfTask& aFireTask)
{
   WsfUplinkTaskResource resource;
   resource.mUplinkCommName = mDefaultUplinkComm;
   WsfTask uplinkTask(resource);
   uplinkTask.SetTaskType(aFireTask.GetTaskType());
   WsfPlatform* assigneePtr = GetSimulation()->GetPlatformByName(mAutoUplinkPlatform);
   if (assigneePtr != nullptr)
   {
      uplinkTask.SetAssignee(assigneePtr);
      mTaskManagerPtr->AssignTask(aSimTime, aTrack, uplinkTask);
   }
   else if (mAutoUplinkPlatform == nullptr)
   {
      uplinkTask.SetAssignee(GetPlatform());
      mTaskManagerPtr->AssignTask(aSimTime, aTrack, uplinkTask);
   }
}

// =================================================================================================
bool WsfWeaponTaskManager::TaskWaitToTransmit(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask)
{
   bool wait = false;
   if (aTask.GetResourceType() == cTASK_RESOURCE_TYPE_UPLINK)
   {
      WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aTask.GetResource();
      if (resource.mUplinkDestination == nullptr)
      {
         wait = true;
         WsfTask taskCopy(aTask);
         taskCopy.SetTargetName(aTrack.GetTargetName());
         taskCopy.SetTrackId(aTrack.GetTrackId());
         mWaitingTaskList.push_back(taskCopy);
      }
   }
   return wait;
}
