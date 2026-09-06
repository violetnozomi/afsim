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

#include "WsfScriptQuantumTaskerProcessor.hpp"

#if defined(unix)
#include <stdint.h>
#elif defined(_WIN32)
#include <cstddef>
#endif

#include "WsfQuantumTaskerProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTask.hpp"
#include "WsfTaskManager.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
// UtWeakReference will keep inform the script accessors if the task is no longer in memory.
// This is required because we are copying the task list here.
using ScriptTaskList = std::vector<UtWeakReference<WsfTask>>;
ScriptTaskList* CreateTaskList(const WsfTaskManager::TaskList& aTaskList)
{
   ScriptTaskList* taskListPtr = new ScriptTaskList();
   for (WsfTaskManager::TaskList::const_iterator tli = aTaskList.begin(); tli != aTaskList.end(); ++tli)
   {
      const WsfTask& task = *tli;
      taskListPtr->push_back(UtWeakReference<WsfTask>());
      taskListPtr->back() = const_cast<WsfTask*>(&task);
   }
   return taskListPtr;
}

ScriptTaskList* CreateTaskListFromPointers(std::list<WsfTask*> aTaskList)
{
   ScriptTaskList* taskListPtr = new ScriptTaskList();
   for (std::list<WsfTask*>::iterator tli = aTaskList.begin(); tli != aTaskList.end(); ++tli)
   {
      WsfTask* taskPtr = *tli;
      taskListPtr->push_back(UtWeakReference<WsfTask>());
      taskListPtr->back() = const_cast<WsfTask*>(taskPtr);
   }
   return taskListPtr;
}


} // namespace


WsfScriptQuantumTaskerProcessor::WsfScriptQuantumTaskerProcessor(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfQuantumTaskerProcessor");

   AddMethod(ut::make_unique<TasksReceived>());       // NO_DOC | DEPRECATED (using legacy WsfTaskManager names now)
   AddMethod(ut::make_unique<TasksReceivedOfType>()); // string type
   AddMethod(ut::make_unique<TasksReceivedOfTypeForTarget>()); // string type, track target (track id)
   AddMethod(ut::make_unique<TasksReceivedForResourceType>()); // string resource type (weapon, sensor, jammer, etc)
   AddMethod(ut::make_unique<TasksReceivedForResourceName>()); // string resource name (actual name)
   AddMethod(ut::make_unique<TasksReceivedForTarget>());       // track target

   AddMethod(ut::make_unique<TasksReceived>("ReceivedTaskList"));
   AddMethod(ut::make_unique<TasksReceivedOfType>("ReceivedTaskListOfType"));                   // string type
   AddMethod(ut::make_unique<TasksReceivedOfTypeForTarget>("ReceivedTaskListOfTypeForTarget")); // string type, track
                                                                                                // target (track id)
   AddMethod(ut::make_unique<TasksReceivedForResourceType>(
      "ReceivedTaskListForResourceType")); // string resource type (weapon, sensor, jammer, etc)
   AddMethod(ut::make_unique<TasksReceivedForResourceName>(
      "ReceivedTaskListForResourceName")); // string resource name (actual name)
   AddMethod(ut::make_unique<TasksReceivedForTarget>("ReceivedTaskListForTarget")); // track target

   AddMethod(ut::make_unique<TasksAssigned>());       // NO_DOC | DEPRECATED (using legacy WsfTaskManager names now)
   AddMethod(ut::make_unique<TasksAssignedOfType>()); // string type
   AddMethod(ut::make_unique<TasksAssignedForResourceType>()); // string resource type (weapon, sensor, jammer, etc)
   AddMethod(ut::make_unique<TasksAssignedForResourceName>()); // string resource name (actual name)
   AddMethod(ut::make_unique<TasksAssignedForTarget>());       // track target

   AddMethod(ut::make_unique<TasksAssigned>("AssignedTaskList"));
   AddMethod(ut::make_unique<TasksAssignedOfType>("AssignedTaskListOfType")); // string type
   AddMethod(ut::make_unique<TasksAssignedForResourceType>(
      "AssignedTaskListForResourceType")); // string resource type (weapon, sensor, jammer, etc)
   AddMethod(ut::make_unique<TasksAssignedForResourceName>(
      "AssignedTaskListForResourceName")); // string resource name (actual name)
   AddMethod(ut::make_unique<TasksAssignedForTarget>("AssignedTaskListForTarget")); // track target

   AddMethod(ut::make_unique<SetTaskProgress>());
   AddMethod(ut::make_unique<SetTaskComplete>());
   AddMethod(ut::make_unique<SetTaskComplete_2>("SetTaskComplete"));
   AddMethod(ut::make_unique<RejectTask>());
   AddMethod(ut::make_unique<CancelTask>());

   AddMethod(ut::make_unique<TasksConsidered>());
   AddMethod(ut::make_unique<AssetsConsidered>());
   AddMethod(ut::make_unique<ValuesConsidered>());
   AddMethod(ut::make_unique<AssetAssigneesFor>());
}

// virtual
void* WsfScriptQuantumTaskerProcessor::Create(const UtScriptContext& aContext)
{
   return static_cast<void*>(new WsfQuantumTaskerProcessor(*WsfScriptContext::GetSCENARIO(aContext)));
}

// virtual
void* WsfScriptQuantumTaskerProcessor::Clone(void* aObjectPtr)
{
   WsfQuantumTaskerProcessor* objPtr = static_cast<WsfQuantumTaskerProcessor*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptQuantumTaskerProcessor::Destroy(void* aObjectPtr)
{
   WsfQuantumTaskerProcessor* objPtr = static_cast<WsfQuantumTaskerProcessor*>(aObjectPtr);
   delete objPtr;
}

UtScriptContext* WsfScriptQuantumTaskerProcessor::GetContext(void* aObjectPtr)
{
   UtScriptContext*           contextPtr   = nullptr;
   WsfQuantumTaskerProcessor* processorPtr = (WsfQuantumTaskerProcessor*)aObjectPtr;
   if (processorPtr != nullptr)
   {
      contextPtr = &(processorPtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, TasksReceived, 0, "WsfTaskList", "")
{
   ScriptTaskList* taskListPtr = CreateTaskList(aObjectPtr->ReceivedTaskList());
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, TasksReceivedOfType, 1, "WsfTaskList", "string")
{
   ScriptTaskList* taskListPtr;
   taskListPtr = CreateTaskListFromPointers(aObjectPtr->TaskListReceivedOfType(aVarArgs[0].GetString()));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksReceivedOfTypeForTarget,
                        2,
                        "WsfTaskList",
                        "string, WsfTrackId")
{
   ScriptTaskList*   taskListPtr;
   WsfStringId       taskType = aVarArgs[0].GetString();
   WsfTrackId        trackId;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   if (trackIdPtr != nullptr)
   {
      trackId = *trackIdPtr;
   }
   taskListPtr = CreateTaskListFromPointers(aObjectPtr->TaskListReceivedOfTypeForTrack(taskType, trackId));

   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksReceivedForResourceType,
                        1,
                        "WsfTaskList",
                        "string")
{
   ScriptTaskList* taskListPtr;
   std::string     type         = aVarArgs[0].GetString();
   int             resourceType = cTASK_RESOURCE_TYPE_NONE;
   if (type == "weapon" || type == "WEAPON")
   {
      resourceType = cTASK_RESOURCE_TYPE_WEAPON;
   }
   else if (type == "sensor" || type == "SENSOR")
   {
      resourceType = cTASK_RESOURCE_TYPE_SENSOR;
   }
   else if (type == "jammer" || type == "JAMMER")
   {
      resourceType = cTASK_RESOURCE_TYPE_JAMMER;
   }
   taskListPtr = CreateTaskListFromPointers(aObjectPtr->TaskListReceivedForResourceType(resourceType));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksReceivedForResourceName,
                        1,
                        "WsfTaskList",
                        "string")
{
   ScriptTaskList* taskListPtr;
   WsfStringId     resourceId = aVarArgs[0].GetString();
   taskListPtr                = CreateTaskListFromPointers(aObjectPtr->TaskListReceivedForResourceName(resourceId));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksReceivedForTarget,
                        1,
                        "WsfTaskList",
                        "WsfTrackId")
{
   ScriptTaskList*   taskListPtr;
   WsfTrackId        trackId;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (trackIdPtr != nullptr)
   {
      trackId = *trackIdPtr;
   }
   taskListPtr = CreateTaskListFromPointers(aObjectPtr->TaskListReceivedForTrack(trackId));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, TasksAssigned, 0, "WsfTaskList", "")
{
   ScriptTaskList* taskListPtr = CreateTaskList(aObjectPtr->AssignedTaskList());
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, TasksAssignedOfType, 1, "WsfTaskList", "string")
{
   ScriptTaskList* taskListPtr;
   WsfStringId     taskType = aVarArgs[0].GetString();
   taskListPtr              = CreateTaskListFromPointers(aObjectPtr->TaskListAssignedOfType(taskType));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksAssignedForResourceType,
                        1,
                        "WsfTaskList",
                        "string")
{
   ScriptTaskList* taskListPtr;
   std::string     type         = aVarArgs[0].GetString();
   int             resourceType = cTASK_RESOURCE_TYPE_NONE;
   if (type == "weapon" || type == "WEAPON")
   {
      resourceType = cTASK_RESOURCE_TYPE_WEAPON;
   }
   else if (type == "sensor" || type == "SENSOR")
   {
      resourceType = cTASK_RESOURCE_TYPE_SENSOR;
   }
   else if (type == "jammer" || type == "JAMMER")
   {
      resourceType = cTASK_RESOURCE_TYPE_JAMMER;
   }
   taskListPtr = CreateTaskListFromPointers(aObjectPtr->TaskListAssignedForResourceType(resourceType));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksAssignedForResourceName,
                        1,
                        "WsfTaskList",
                        "string")
{
   ScriptTaskList* taskListPtr;
   WsfStringId     resourceId = aVarArgs[0].GetString();
   taskListPtr                = CreateTaskListFromPointers(aObjectPtr->TaskListAssignedForResourceName(resourceId));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        TasksAssignedForTarget,
                        1,
                        "WsfTaskList",
                        "WsfTrackId")
{
   ScriptTaskList*   taskListPtr;
   WsfTrackId        trackId;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (trackIdPtr != nullptr)
   {
      trackId = *trackIdPtr;
   }
   taskListPtr = CreateTaskListFromPointers(aObjectPtr->TaskListAssignedForTrack(trackId));
   aReturnVal.SetPointer(new UtScriptRef(taskListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, SetTaskProgress, 2, "void", "WsfTask, string")
{
   WsfTask*    taskPtr = (WsfTask*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfStringId status  = aVarArgs[1].GetString();
   aObjectPtr->ReportTaskProgress(TIME_NOW, *taskPtr, status);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, SetTaskComplete, 1, "void", "WsfTask")
{
   WsfTask* taskPtr = (WsfTask*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->ReportTaskComplete(TIME_NOW, *taskPtr, "SUCCESSFUL");
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, SetTaskComplete_2, 2, "void", "WsfTask, string")
{
   WsfTask* taskPtr = (WsfTask*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->ReportTaskComplete(TIME_NOW, *taskPtr, aVarArgs[1].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, RejectTask, 1, "void", "WsfTask")
{
   WsfTask* taskPtr = (WsfTask*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->RejectTask(TIME_NOW, *taskPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, TasksConsidered, 0, "Array<WsfQuantumTask>", "")
{
   std::vector<WsfQuantumTask*>& tasks    = aObjectPtr->Tasks();
   UtScriptClass*                classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>*    arrayPtr = new std::vector<UtScriptData>();
   for (auto task : tasks)
   {
      arrayPtr->emplace_back(UtScriptRef::Ref(task->Clone(), classPtr, UtScriptRef::cMANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(arrayPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        AssetsConsidered,
                        0,
                        "Array<WsfAssetPerception>",
                        "")
{
   std::vector<UtScriptData>*        scriptAssetsPtr = new std::vector<UtScriptData>();  // this array is returned
   UtScriptClass*                    classPtr = aReturnClassPtr->GetContainerDataType(); // type of object in array
   std::vector<WsfAssetPerception*>& assets   = aObjectPtr->Assets();
   for (auto asset : assets)
   {
      scriptAssetsPtr->emplace_back(UtScriptRef::Ref(asset->Clone(), classPtr, UtScriptRef::cMANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssetsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, ValuesConsidered, 0, "Array<Array<double>>", "")
{
   std::vector<std::vector<double>>& aValues       = aObjectPtr->Values();
   std::vector<UtScriptData>*        array2dPtr    = new std::vector<UtScriptData>();
   UtScriptClass*                    arrayClassPtr = aContext.GetTypes()->GetClass("Array<double>");
   for (unsigned int i = 0; i < aValues.size(); ++i)
   {
      std::vector<UtScriptData>* arrayPtr = new std::vector<UtScriptData>();
      for (unsigned int j = 0; j < aValues[i].size(); ++j)
      {
         arrayPtr->push_back(UtScriptData(aValues[i][j]));
      }
      array2dPtr->push_back(UtScriptData(new UtScriptRef(arrayPtr, arrayClassPtr, UtScriptRef::cMANAGE)));
   }
   UtScriptClass* valuesClassPtr = aContext.GetTypes()->GetClass("Array<Array<double>>");

   // scriptArgs.push_back(UtScriptData(new UtScriptRef(array2dPtr, valuesClass, UtScriptRef::cMANAGE)));
   aReturnVal.SetPointer(new UtScriptRef(array2dPtr, valuesClassPtr, UtScriptRef::cMANAGE));
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor,
                        WsfQuantumTaskerProcessor,
                        AssetAssigneesFor,
                        1,
                        "Array<WsfAssetPerception>",
                        "WsfQuantumTask")
{
   WsfQuantumTask*               qtPtr           = (WsfQuantumTask*)aVarArgs[0].GetPointer()->GetAppObject();
   std::vector<UtScriptData>*    scriptAssetsPtr = new std::vector<UtScriptData>();         // this array is returned
   UtScriptClass*                classPtr        = aReturnClassPtr->GetContainerDataType(); // type of object in array
   std::set<WsfAssetPerception*> assets          = aObjectPtr->AssetAssigneesForTask(qtPtr);
   for (auto asset : assets)
   {
      scriptAssetsPtr->emplace_back(UtScriptRef::Ref(asset->Clone(), classPtr, UtScriptRef::cMANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssetsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskerProcessor, WsfQuantumTaskerProcessor, CancelTask, 1, "bool", "WsfTrackId")
{
   WsfTrackId*     trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTaskResource resource;
   resource.mNameId = nullptr;

   WsfStringId  taskType      = nullptr;
   unsigned int assigneeIndex = 0;
   bool         ok            = aObjectPtr->CancelTask(TIME_NOW, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}
