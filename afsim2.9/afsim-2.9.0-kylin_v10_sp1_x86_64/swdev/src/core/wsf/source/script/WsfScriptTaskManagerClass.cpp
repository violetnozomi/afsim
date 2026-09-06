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

#include "script/WsfScriptTaskManagerClass.hpp"

#include <iostream>

#include "UtMemory.hpp"
#include "WsfCommandChain.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTaskManager.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptPlatformListClass.hpp"

namespace
{
//! A helper method to get a track ID from a script argument.
//! This method will return the track ID from a script argument. If the pointer to the track ID is null then
//! a null track ID object will be returned. This allows a script user to pass a track ID reference which has
//! not been populated.
WsfTrackId GetTrackId(const UtScriptData& aScriptArg)
{
   WsfTrackId        trackId;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aScriptArg.GetPointer()->GetAppObject());
   if (trackIdPtr != nullptr)
   {
      trackId = *trackIdPtr;
   }
   return trackId;
}
} // namespace

WsfScriptTaskManagerClass::WsfScriptTaskManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTaskManager");
   AddMethod(ut::make_unique<AssignTask>("AssignTask"));
   AddMethod(ut::make_unique<AssignLocalTask_1>("AssignTask"));
   AddMethod(ut::make_unique<AssignLocalTask_2>("AssignTask"));
   AddMethod(ut::make_unique<AssignRemoteTask_1>("AssignTask"));
   AddMethod(ut::make_unique<AssignRemoteTask_2>("AssignTask"));
   AddMethod(ut::make_unique<AssignRemoteTask_3>("AssignTask"));
   AddMethod(ut::make_unique<AssignRemoteTask_4>("AssignTask"));
   AddMethod(ut::make_unique<AssigneesForTask>());
   AddMethod(ut::make_unique<AssigneeForTask>());
   AddMethod(ut::make_unique<CancelTask_1>("CancelTask"));
   AddMethod(ut::make_unique<CancelTask_2>("CancelTask"));
   AddMethod(ut::make_unique<CancelTask_3>("CancelTask"));
   AddMethod(ut::make_unique<CancelTask_4>("CancelTask"));
   AddMethod(ut::make_unique<CancelTask_5>("CancelTask"));
   AddMethod(ut::make_unique<CancelTask_6>("CancelTask"));
   AddMethod(ut::make_unique<DelayTask>());
   AddMethod(ut::make_unique<TaskComplete_1>("TaskComplete"));
   AddMethod(ut::make_unique<TaskComplete_2>("TaskComplete"));
   AddMethod(ut::make_unique<TaskComplete_3>("TaskComplete"));
   AddMethod(ut::make_unique<TaskComplete_4>("TaskComplete"));

   AddMethod(ut::make_unique<AssignedTaskList_1>("AssignedTaskList"));
   AddMethod(ut::make_unique<AssignedTaskList_2>("AssignedTaskList"));
   AddMethod(ut::make_unique<ReceivedTaskList_1>("ReceivedTaskList"));
   AddMethod(ut::make_unique<ReceivedTaskList_2>("ReceivedTaskList"));
   AddMethod(ut::make_unique<TasksAssigned>());
   AddMethod(ut::make_unique<TasksAssignedTo_1>("TasksAssignedTo"));
   AddMethod(ut::make_unique<TasksAssignedTo_2>("TasksAssignedTo"));
   AddMethod(ut::make_unique<TasksAssignedTo_3>("TasksAssignedTo"));
   AddMethod(ut::make_unique<TasksAssignedTo_4>("TasksAssignedTo"));
   AddMethod(ut::make_unique<TasksAssignedTo_5>("TasksAssignedTo"));
   AddMethod(ut::make_unique<TasksAssignedTo_6>("TasksAssignedTo"));
   AddMethod(ut::make_unique<TasksAssignedFor_1>("TasksAssignedFor"));
   AddMethod(ut::make_unique<TasksAssignedFor_2>("TasksAssignedFor"));
   AddMethod(ut::make_unique<TasksReceived>());
   AddMethod(ut::make_unique<TasksReceivedFor_1>("TasksReceivedFor"));
   AddMethod(ut::make_unique<TasksReceivedFor_2>("TasksReceivedFor"));
   AddMethod(ut::make_unique<TimeSinceAssigned>());
   AddMethod(ut::make_unique<TimeSinceLastTaskAssigned>());
   AddMethod(ut::make_unique<TimeSinceLastTaskReceived>());

   AddMethod(ut::make_unique<StartLocalTracking>("StartTracking"));
   AddMethod(ut::make_unique<StartRemoteTracking_1>("StartTracking"));
   AddMethod(ut::make_unique<StartRemoteTracking_2>("StartTracking"));
   AddMethod(ut::make_unique<StopLocalTracking>("StopTracking"));
   AddMethod(ut::make_unique<StopRemoteTracking>("StopTracking"));

   AddMethod(ut::make_unique<LocalOperatingLevelFor>("OperatingLevelFor"));
   AddMethod(ut::make_unique<RemoteOperatingLevelFor>("OperatingLevelFor"));
   AddMethod(ut::make_unique<ChangeLocalOperatingLevel>("ChangeOperatingLevel"));
   AddMethod(ut::make_unique<ChangeRemoteOperatingLevel_1>("ChangeOperatingLevel"));
   AddMethod(ut::make_unique<ChangeRemoteOperatingLevel_2>("ChangeOperatingLevel"));

   AddMethod(ut::make_unique<LocalOperatingLevelFor>("AutonomyLevelFor")); // NO_DOC | DEPRECATED backward compatibility
   AddMethod(ut::make_unique<RemoteOperatingLevelFor>("AutonomyLevelFor")); // NO_DOC | DEPRECATED backward compatibility
   AddMethod(ut::make_unique<ChangeLocalOperatingLevel>("ChangeAutonomyLevel")); // NO_DOC | DEPRECATED backward compatibility
   AddMethod(ut::make_unique<ChangeRemoteOperatingLevel_1>("ChangeAutonomyLevel")); // NO_DOC | DEPRECATED backward compatibility
   AddMethod(ut::make_unique<ChangeRemoteOperatingLevel_2>("ChangeAutonomyLevel")); // NO_DOC | DEPRECATED backward compatibility
}

#include "script/WsfScriptDefs.hpp"

//! Assign a task.
//! bool ok = AssignTask(WsfTask aTask);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignTask, 2, "bool", "WsfTrack, WsfTask")
{
   bool      ok       = false;
   WsfTrack* trackPtr = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTask*  taskPtr  = static_cast<WsfTask*>(aVarArgs[1].GetPointer()->GetAppObject());
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   if (taskPtr->GetAssigneePlatformIndex() == 0)
   {
      taskPtr->SetAssignee(aObjectPtr->GetPlatform());
   }
   ok = aObjectPtr->AssignTask(simTime, *trackPtr, *taskPtr);
   aReturnVal.SetBool(ok);
}


//! Assign a task to myself.
//! bool ok = AssignTask(WsfTrack aTrack, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignLocalTask_1, 2, "bool", "WsfTrack, string")
{
   bool         ok          = false;
   WsfPlatform* assigneePtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfTrack*    trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (assigneePtr != nullptr)
   {
      WsfStringId taskType = WsfStringId(aVarArgs[1].GetString());
      WsfStringId commName = nullptr;
      double      simTime  = WsfScriptContext::GetTIME_NOW(aContext);
      ok = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, WsfTaskResource(), assigneePtr, commName);
   }
   aReturnVal.SetBool(ok);
}

//! Assign a task to another task manager on the same platform.
//! bool ok = AssignTask(WsfTrack aTrack, string aTaskType, string aProcName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignLocalTask_2, 3, "bool", "WsfTrack, string, string")
{
   bool         ok          = false;
   WsfPlatform* assigneePtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfTrack*    trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (assigneePtr != nullptr)
   {
      WsfTaskResource resource(cTASK_RESOURCE_TYPE_PROCESSOR);
      resource.mNameId = aVarArgs[2].GetString();

      WsfStringId taskType = aVarArgs[1].GetString();
      WsfStringId commName = nullptr;
      double      simTime  = WsfScriptContext::GetTIME_NOW(aContext);
      ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   }
   aReturnVal.SetBool(ok);
}

//! Assign a task to another platform.
//! bool ok = AssignTask(WsfTrack aTrack, string aTaskType, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignRemoteTask_1, 3, "bool", "WsfTrack, string, WsfPlatform")
{
   WsfPlatform* assigneePtr = static_cast<WsfPlatform*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*    trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());

   WsfStringId taskType = aVarArgs[1].GetString();
   WsfStringId commName = nullptr;
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   bool        ok = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, WsfTaskResource(), assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Assign a task to another platform.
//! bool ok = AssignTask(WsfTrack aTrack, string aTaskType, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignRemoteTask_2, 4, "bool", "WsfTrack, string, WsfPlatform, string")
{
   WsfPlatform* assigneePtr = static_cast<WsfPlatform*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*    trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());

   WsfStringId taskType = aVarArgs[1].GetString();
   WsfStringId commName = aVarArgs[3].GetString();
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   bool        ok = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, WsfTaskResource(), assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Assign a task to a specific task manager on another platform.
//! bool ok = AssignTask(WsfTrack aTrack, string aTaskType, string aProcName, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignRemoteTask_3, 4, "bool", "WsfTrack, string, string, WsfPlatform")
{
   WsfPlatform*    assigneePtr = static_cast<WsfPlatform*>(aVarArgs[3].GetPointer()->GetAppObject());
   WsfTrack*       trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_PROCESSOR);
   resource.mNameId = aVarArgs[2].GetString();

   WsfStringId taskType = aVarArgs[1].GetString();
   WsfStringId commName = nullptr;
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   bool        ok       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Assign a task to a specific task manager on another platform.
//! bool ok = AssignTask(WsfTrack aTrack, string aTaskType, string aProcName, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass,
                        WsfTaskManager,
                        AssignRemoteTask_4,
                        5,
                        "bool",
                        "WsfTrack, string, string, WsfPlatform, string")
{
   WsfPlatform*    assigneePtr = static_cast<WsfPlatform*>(aVarArgs[3].GetPointer()->GetAppObject());
   WsfTrack*       trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_PROCESSOR);
   resource.mNameId = aVarArgs[2].GetString();

   WsfStringId taskType = aVarArgs[1].GetString();
   WsfStringId commName = aVarArgs[4].GetString();
   double      simTime  = TIME_NOW;
   bool        ok       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Return the list of assignees for a specified task.
//! WsfPlatformList assignees = AssigneesForTask(WsfTrackId aTrackId, string aTaskType)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssigneesForTask, 2, "WsfPlatformList", "WsfTrackId, string")
{
   WsfTrackId          trackId(GetTrackId(aVarArgs[0]));
   WsfStringId         taskType = aVarArgs[1].GetString();
   std::vector<size_t> assignees;
   aObjectPtr->AssigneesForTask(trackId, taskType, assignees);
   WsfCommandChain::PlatformList* platformListPtr = new WsfCommandChain::PlatformList();
   for (size_t assignee : assignees)
   {
      WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(assignee);
      if (platformPtr != nullptr)
      {
         platformListPtr->push_back(platformPtr);
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Return the assignee associated with the indicated task.
//! WsfPlatform assignee = AssigneeForTask(WsfTrackId aTrackId, string aTaskType)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssigneeForTask, 2, "WsfPlatform", "WsfTrackId, string")
{
   WsfPlatform* assigneePtr = nullptr;
   WsfTrackId*  trackIdPtr  = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfStringId  taskType    = aVarArgs[1].GetString();
   WsfStringId  resourceName;
   size_t       assigneeIndex = aObjectPtr->AssigneeForTask(*trackIdPtr, taskType, resourceName);
   assigneePtr                = SIMULATION->GetPlatformByIndex(assigneeIndex);
   aReturnVal.SetPointer(new UtScriptRef(assigneePtr, aReturnClassPtr));
}

//! bool ok = CancelTask(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, CancelTask_1, 1, "bool", "WsfTrackId")
{
   WsfTrackId*     trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource;
   resource.mNameId = nullptr;

   WsfStringId taskType      = nullptr;
   size_t      assigneeIndex = 0;
   bool        ok            = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! bool ok = CancelTask(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, CancelTask_2, 2, "bool", "WsfTrackId, string")
{
   WsfTrackId* trackIdPtr    = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfStringId taskType      = aVarArgs[1].GetString();
   size_t      assigneeIndex = 0;
   bool        ok = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, WsfTaskResource(), assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! bool ok = CancelTask(WsfTrackId aTrackId, string aTaskType, string aResourceType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, CancelTask_3, 3, "bool", "WsfTrackId, string, string")
{
   WsfTrackId*     trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource;
   resource.mNameId = aVarArgs[2].GetString();

   WsfStringId taskType      = aVarArgs[1].GetString();
   size_t      assigneeIndex = 0;
   bool        ok            = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! bool ok = CancelTask(WsfPlatform aAssignee, WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, CancelTask_4, 2, "bool", "WsfPlatform, WsfTrackId")
{
   WsfTrackId* trackIdPtr    = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfStringId taskType      = nullptr;
   size_t      assigneeIndex = static_cast<size_t>(aVarArgs[0].GetPointer()->GetAuxData());
   bool        ok = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, WsfTaskResource(), assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! bool ok = CancelTask(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, CancelTask_5, 3, "bool", "WsfPlatform, WsfTrackId, string")
{
   WsfTrackId* trackIdPtr    = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfStringId taskType      = aVarArgs[2].GetString();
   size_t      assigneeIndex = static_cast<size_t>(aVarArgs[0].GetPointer()->GetAuxData());
   bool        ok = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, WsfTaskResource(), assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! bool ok = CancelTask(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType, string aResourceType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, CancelTask_6, 4, "bool", "WsfPlatform, WsfTrackId, string, string")
{
   WsfTrackId*     trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfTaskResource resource;
   resource.mNameId = aVarArgs[3].GetString();

   WsfStringId taskType      = aVarArgs[2].GetString();
   size_t      assigneeIndex = static_cast<size_t>(aVarArgs[0].GetPointer()->GetAuxData());
   bool        ok            = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! Assign a delay pseudo-task.
//! bool ok = DelayTask(double aDelayTime, WsfTrack aTrack, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, DelayTask, 3, "bool", "double, WsfTrack, string")
{
   bool         ok          = false;
   WsfPlatform* assigneePtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfTrack*    trackPtr    = static_cast<WsfTrack*>(aVarArgs[1].GetPointer()->GetAppObject());
   if (assigneePtr != nullptr)
   {
      double      delayTime = aVarArgs[0].GetDouble();
      WsfStringId taskType  = aVarArgs[2].GetString();
      ok                    = aObjectPtr->CreateDelayTask(TIME_NOW, delayTime, *trackPtr, taskType);
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = TaskComplete(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TaskComplete_1, 2, "bool", "WsfTrackId, string")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfStringId       taskType   = aVarArgs[1].GetString();
   WsfStringId       resourceName;
   bool              ok = aObjectPtr->ReportTaskComplete(TIME_NOW, *trackIdPtr, taskType, resourceName, "SUCCESSFUL");
   aReturnVal.SetBool(ok);
}

//! bool ok = TaskComplete(WsfTrackId aTrackId, string aTaskType, string aResourceName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TaskComplete_2, 3, "bool", "WsfTrackId, string, string")
{
   const WsfTrackId* trackIdPtr   = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfStringId       taskType     = aVarArgs[1].GetString();
   WsfStringId       resourceName = aVarArgs[2].GetString();
   bool              ok = aObjectPtr->ReportTaskComplete(TIME_NOW, *trackIdPtr, taskType, resourceName, "SUCCESSFUL");
   aReturnVal.SetBool(ok);
}

//! bool ok = TaskComplete(string aStatus, WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TaskComplete_3, 3, "bool", "string, WsfTrackId, string")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfStringId       taskType   = aVarArgs[2].GetString();
   WsfStringId       resourceName;
   WsfStringId       status = aVarArgs[0].GetString();
   bool              ok     = aObjectPtr->ReportTaskComplete(TIME_NOW, *trackIdPtr, taskType, resourceName, status);
   aReturnVal.SetBool(ok);
}

//! bool ok = TaskComplete(string aStatus, WsfTrackId aTrackId, string aTaskType, string aResourceName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TaskComplete_4, 4, "bool", "string, WsfTrackId, string, string")
{
   const WsfTrackId* trackIdPtr   = static_cast<const WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfStringId       taskType     = aVarArgs[2].GetString();
   WsfStringId       resourceName = aVarArgs[3].GetString();
   WsfStringId       status       = aVarArgs[0].GetString();
   bool              ok = aObjectPtr->ReportTaskComplete(TIME_NOW, *trackIdPtr, taskType, resourceName, status);
   aReturnVal.SetBool(ok);
}

// ------------------------------------------------------------------------------------------------
// Start of query methods.

namespace
{
// UtWeakReference will keep inform the script accessors if the task is no longer in memory.
// This is required because we are copying the task list here.
typedef std::vector<UtWeakReference<WsfTask>> ScriptTaskList;
ScriptTaskList* CreateTaskList(const WsfTaskManager::TaskList& aTaskList, const WsfTrackId& aTrackId, WsfStringId aTaskType)
{
   ScriptTaskList* taskListPtr = new ScriptTaskList();
   for (const WsfTask& task : aTaskList)
   {
      if ((aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())) &&
          ((!aTaskType) || (aTaskType == task.GetTaskType())))
      {
         taskListPtr->push_back(UtWeakReference<WsfTask>());
         taskListPtr->back() = const_cast<WsfTask*>(&task);
      }
   }
   return taskListPtr;
}
} // namespace

//! Return a list of task assignments that have been made.
//! WsfTaskList taskList = AssignedTaskList();
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignedTaskList_1, 0, "WsfTaskList", "")
{
   ScriptTaskList* taskListPtr = CreateTaskList(aObjectPtr->AssignedTaskList(), WsfTrackId(), nullptr);
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Return a list of task assignments that have been made.
//! WsfTaskList taskList = AssignedTaskList(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, AssignedTaskList_2, 2, "WsfTaskList", "WsfTrackId, string")
{
   WsfTrackId      trackId(GetTrackId(aVarArgs[0]));
   WsfStringId     taskType    = aVarArgs[1].GetString();
   ScriptTaskList* taskListPtr = CreateTaskList(aObjectPtr->AssignedTaskList(), trackId, taskType);
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Return a list of task assignments that have been received.
//! WsfTaskList taskList = ReceivedTaskList();
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, ReceivedTaskList_1, 0, "WsfTaskList", "")
{
   ScriptTaskList* taskListPtr = CreateTaskList(aObjectPtr->ReceivedTaskList(), WsfTrackId(), nullptr);
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Return a list of task assignments that have been received.
//! WsfTaskList taskList = ReceivedTaskList(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, ReceivedTaskList_2, 2, "WsfTaskList", "WsfTrackId, string")
{
   WsfTrackId      trackId(GetTrackId(aVarArgs[0]));
   WsfStringId     taskType    = aVarArgs[1].GetString();
   ScriptTaskList* taskListPtr = CreateTaskList(aObjectPtr->ReceivedTaskList(), trackId, taskType);
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Return the number of tasks assignments that have been made.
//! int count = TasksAssigned();
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssigned, 0, "int", "")
{
   int taskCount = aObjectPtr->TasksAssignedFor(WsfTrackId(), nullptr, nullptr);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned to the specified platform.
//! int count = TasksAssignedTo(WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedTo_1, 1, "int", "WsfPlatform")
{
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData();
   int          taskCount     = aObjectPtr->TasksAssignedTo(assigneeIndex, WsfTrackId(), nullptr, nullptr);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned to the specified platform.
//! int count = TasksAssignedTo(WsfPlatform aAssignee, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedTo_2, 2, "int", "WsfPlatform, string")
{
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData();
   WsfStringId  taskName      = aVarArgs[1].GetString();
   int          taskCount     = aObjectPtr->TasksAssignedTo(assigneeIndex, WsfTrackId(), taskName, nullptr);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned to the specified platform.
//! int count = TasksAssignedTo(WsfPlatform aAssignee, string aTaskType, string aResourceName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedTo_3, 3, "int", "WsfPlatform, string, string")
{
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData();
   WsfStringId  taskName      = aVarArgs[1].GetString();
   WsfStringId  resourceName  = aVarArgs[2].GetString();
   int          taskCount     = aObjectPtr->TasksAssignedTo(assigneeIndex, WsfTrackId(), taskName, resourceName);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned to the specified platform.
//! int count = TasksAssignedTo(WsfPlatform aAssignee, WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedTo_4, 2, "int", "WsfPlatform, WsfTrackId")
{
   WsfTrackId   trackId(GetTrackId(aVarArgs[1]));
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData();
   int          taskCount     = aObjectPtr->TasksAssignedTo(assigneeIndex, trackId, nullptr, nullptr);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned to the specified platform.
//! int count = TasksAssignedTo(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedTo_5, 3, "int", "WsfPlatform, WsfTrackId, string")
{
   WsfTrackId   trackId(GetTrackId(aVarArgs[1]));
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData();
   WsfStringId  taskName      = aVarArgs[2].GetString();
   int          taskCount     = aObjectPtr->TasksAssignedTo(assigneeIndex, trackId, taskName, nullptr);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned to the specified platform.
//! int count = TasksAssignedTo(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType, string aResourceName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedTo_6, 4, "int", "WsfPlatform, WsfTrackId, string, string")
{
   WsfTrackId   trackId(GetTrackId(aVarArgs[1]));
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData();
   WsfStringId  taskName      = aVarArgs[2].GetString();
   WsfStringId  resourceName  = aVarArgs[3].GetString();
   int          taskCount     = aObjectPtr->TasksAssignedTo(assigneeIndex, trackId, taskName, resourceName);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned for a specified track.
//! int count = TasksAssignedFor(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedFor_1, 1, "int", "WsfTrackId")
{
   WsfTrackId  trackId(GetTrackId(aVarArgs[0]));
   WsfStringId taskName;
   WsfStringId resourceName;
   int         taskCount = aObjectPtr->TasksAssignedFor(trackId, taskName, resourceName);
   aReturnVal.SetInt(taskCount);
}

//! Return the number of tasks that have been assigned for a specified track.
//! int count = TasksAssignedFor(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksAssignedFor_2, 2, "int", "WsfTrackId, string")
{
   WsfTrackId  trackId(GetTrackId(aVarArgs[0]));
   WsfStringId taskName = aVarArgs[1].GetString();
   WsfStringId resourceName;
   int         taskCount = aObjectPtr->TasksAssignedFor(trackId, taskName, resourceName);
   aReturnVal.SetInt(taskCount);
}

//! Returned the total number of tasks assignments that have been received.
//! int count = TasksReceived();
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksReceived, 0, "int", "")
{
   int taskCount = aObjectPtr->TasksReceivedFor(WsfTrackId(), nullptr, nullptr);
   aReturnVal.SetInt(taskCount);
}

//! Returned the number of tasks that have be received for the specified task.
//! int count = TasksReceivedFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksReceivedFor_1, 1, "int", "WsfTrackId")
{
   WsfTrackId  trackId(GetTrackId(aVarArgs[0]));
   WsfStringId taskName;
   WsfStringId resourceName;
   int         taskCount = aObjectPtr->TasksReceivedFor(trackId, taskName, resourceName);
   aReturnVal.SetInt(taskCount);
}

//! Returned the number of tasks that have be received for the specified task.
//! int count = TasksReceivedFor(WsfTrackId aTrackId, string aTaskType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TasksReceivedFor_2, 2, "int", "WsfTrackId, string")
{
   WsfTrackId  trackId(GetTrackId(aVarArgs[0]));
   WsfStringId taskName = aVarArgs[1].GetString();
   WsfStringId resourceName;
   int         taskCount = aObjectPtr->TasksReceivedFor(trackId, taskName, resourceName);
   aReturnVal.SetInt(taskCount);
}

// Return the elapsed time since the task assignment was made to the indicated platform.
// The return value will be less than zero if no task has been assigned to the indicated platform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TimeSinceAssigned, 3, "double", "WsfTrackId, string, WsfPlatform")
{
   double            timeSince     = -1.0;
   const WsfTrackId* trackIdPtr    = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfStringId       taskType      = aVarArgs[1].GetString();
   unsigned int      assigneeIndex = (unsigned int)aVarArgs[2].GetPointer()->GetAuxData();
   double            timeAssigned  = aObjectPtr->TimeTaskAssigned(*trackIdPtr, taskType, assigneeIndex);
   if (timeAssigned >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeAssigned;
   }
   aReturnVal.SetDouble(timeSince);
}

// Return the elapsed time since the last task was assigned.
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TimeSinceLastTaskAssigned, 0, "double", "")
{
   double timeSince = aObjectPtr->TimeLastTaskAssigned();
   if (timeSince >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeSince;
   }
   aReturnVal.SetDouble(timeSince);
}

// Return the elapsed time since the last task was received.
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, TimeSinceLastTaskReceived, 0, "double", "")
{
   double timeSince = aObjectPtr->TimeLastTaskReceived();
   if (timeSince >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeSince;
   }
   aReturnVal.SetDouble(timeSince);
}

//! StartTracking a local sensor to a track.
//! This is a more obvious form of tracking instead of using AssignTask.
//! bool ok = StartTracking(WsfTrack aTrack, string aTaskType, WsfSensor aSensor, string aSensorMode);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, StartLocalTracking, 4, "bool", "WsfTrack, string, WsfSensor, string")
{
   bool            ok(false);
   WsfSensor*      sensorPtr = static_cast<WsfSensor*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*       trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_SENSOR); // resource info
   resource.mNameId     = sensorPtr->GetNameId();        // - sensor name
   resource.mModeNameId = aVarArgs[3].GetString();       // - sensor mode

   WsfPlatform* assigneePtr = sensorPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm     - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartTracking a remote sensor to a track.
//! This is a more obvious form of tracking instead of using AssignTask.
//! bool ok = StartTracking(WsfTrack aTrack, string aTaskType, string aResourceName, string aSensorMode,
//!                         WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass,
                        WsfTaskManager,
                        StartRemoteTracking_1,
                        5,
                        "bool",
                        "WsfTrack, string, string, string, WsfPlatform")
{
   bool            ok(false);
   WsfTrack*       trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*    assigneePtr = static_cast<WsfPlatform*>(aVarArgs[4].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_SENSOR); // resource info
   resource.mNameId     = aVarArgs[2].GetString();       // - sensor name
   resource.mModeNameId = aVarArgs[3].GetString();       // - sensor mode

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartTracking a remote sensor to a track.
//! This is a more obvious form of tracking instead of using AssignTask.
//! bool ok = StartTracking(WsfTrack aTrack, string aTaskType, string aResourceName, string aSensorMode,
//!                         WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass,
                        WsfTaskManager,
                        StartRemoteTracking_2,
                        6,
                        "bool",
                        "WsfTrack, string, string, string, WsfPlatform, string")
{
   bool            ok(false);
   WsfTrack*       trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*    assigneePtr = static_cast<WsfPlatform*>(aVarArgs[4].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_SENSOR); // resource info
   resource.mNameId     = aVarArgs[2].GetString();       // - sensor name
   resource.mModeNameId = aVarArgs[3].GetString();       // - sensor mode

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[5].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StopTracking a local sensor to a track.
//! This is a more obvious form of canceling tracking instead of using CancelTask.
//! bool ok = StopTracking(WsfTrackId aTrackId, string aTaskType, string aResourceType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, StopLocalTracking, 3, "bool", "WsfTrackId, string, string")
{
   bool            ok(false);
   WsfTrackId*     trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_SENSOR); // resource info
   resource.mNameId = aVarArgs[2].GetString();           // - sensor name

   WsfStringId  taskType      = aVarArgs[1].GetString();                 // taskType
   unsigned int assigneeIndex = 0;                                       // assignee index - N/A
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! StopTracking a remote sensor to a track.
//! This is a more obvious form of canceling tracking instead of using CancelTask.
//! bool ok = StopTracking(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType, string aResourceType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, StopRemoteTracking, 4, "bool", "WsfPlatform, WsfTrackId, string, string")
{
   bool            ok(false);
   WsfTrackId*     trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfTaskResource resource(cTASK_RESOURCE_TYPE_SENSOR); // resource info
   resource.mNameId = aVarArgs[3].GetString();           // - sensor name

   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData(); // assignee index
   WsfStringId  taskType      = aVarArgs[2].GetString();                              // taskType
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext);              // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! Return the operating level for the specified task type in this task manager.
//! int operatingLevel = OperatingLevelFor(string aName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, LocalOperatingLevelFor, 1, "int", "string")
{
   WsfStringId name = aVarArgs[0].GetString();
   aReturnVal.SetInt(aObjectPtr->OperatingLevelFor(name));
}

//! Return the operating level for the specified task type in a remote task manager.
//! int operatingLevel = OperatingLevelFor(string aName, WsfPlatform aAssignee)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, RemoteOperatingLevelFor, 2, "int", "string, WsfPlatform")
{
   int          level       = 0;
   WsfPlatform* assigneePtr = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*assigneePtr); !iter.AtEnd(); ++iter)
   {
      WsfProcessor*   processorPtr = *iter;
      WsfTaskManager* managerPtr   = dynamic_cast<WsfTaskManager*>(processorPtr);
      if (managerPtr != nullptr)
      {
         WsfStringId name = aVarArgs[0].GetString();
         level            = managerPtr->OperatingLevelFor(name);
         break;
      }
   }
   aReturnVal.SetInt(level);
}

//! bool ok = ChangeOperatingLevel(string aName, int aOperatingLevel);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, ChangeLocalOperatingLevel, 2, "bool", "string, int")
{
   WsfStringId name    = aVarArgs[0].GetString();
   int         level   = aVarArgs[1].GetInt();
   double      simTime = WsfScriptContext::GetTIME_NOW(aContext);
   bool        ok      = aObjectPtr->ChangeOperatingLevel(simTime, name, level, aObjectPtr->GetPlatform(), nullptr);
   aReturnVal.SetBool(ok);
}

//! bool ok = ChangeOperatingLevel(string aName, int aOperatingLevel, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass, WsfTaskManager, ChangeRemoteOperatingLevel_1, 3, "bool", "string, int, WsfPlatform")
{
   WsfPlatform* assigneePtr = static_cast<WsfPlatform*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfStringId  name        = aVarArgs[0].GetString();
   int          level       = aVarArgs[1].GetInt();
   WsfStringId  commName    = nullptr;
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   bool         ok          = aObjectPtr->ChangeOperatingLevel(simTime, name, level, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! bool ok = ChangeOperatingLevel(string aName, int aOperatingLevel, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskManagerClass,
                        WsfTaskManager,
                        ChangeRemoteOperatingLevel_2,
                        4,
                        "bool",
                        "string, int, WsfPlatform, string")
{
   WsfPlatform* assigneePtr = static_cast<WsfPlatform*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfStringId  name        = aVarArgs[0].GetString();
   int          level       = aVarArgs[1].GetInt();
   WsfStringId  commName    = aVarArgs[3].GetString();
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   bool         ok          = aObjectPtr->ChangeOperatingLevel(simTime, name, level, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}
