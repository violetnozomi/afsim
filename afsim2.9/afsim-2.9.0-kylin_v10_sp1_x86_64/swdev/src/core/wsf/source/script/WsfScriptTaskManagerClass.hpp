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

#ifndef WSFSCRIPTTASKMANAGERCLASS_HPP
#define WSFSCRIPTTASKMANAGERCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptProcessorClass.hpp"

//! Define script methods for WsfTaskManager.
class WSF_EXPORT WsfScriptTaskManagerClass : public WsfScriptProcessorClass
{
public:
   WsfScriptTaskManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(AssignTask);
   UT_DECLARE_SCRIPT_METHOD(AssignLocalTask_1);
   UT_DECLARE_SCRIPT_METHOD(AssignLocalTask_2);
   UT_DECLARE_SCRIPT_METHOD(AssignRemoteTask_1);
   UT_DECLARE_SCRIPT_METHOD(AssignRemoteTask_2);
   UT_DECLARE_SCRIPT_METHOD(AssignRemoteTask_3);
   UT_DECLARE_SCRIPT_METHOD(AssignRemoteTask_4);
   UT_DECLARE_SCRIPT_METHOD(AssigneesForTask);
   UT_DECLARE_SCRIPT_METHOD(AssigneeForTask);
   UT_DECLARE_SCRIPT_METHOD(CancelTask_1);
   UT_DECLARE_SCRIPT_METHOD(CancelTask_2);
   UT_DECLARE_SCRIPT_METHOD(CancelTask_3);
   UT_DECLARE_SCRIPT_METHOD(CancelTask_4);
   UT_DECLARE_SCRIPT_METHOD(CancelTask_5);
   UT_DECLARE_SCRIPT_METHOD(CancelTask_6);
   UT_DECLARE_SCRIPT_METHOD(DelayTask);
   UT_DECLARE_SCRIPT_METHOD(TaskComplete_1);
   UT_DECLARE_SCRIPT_METHOD(TaskComplete_2);
   UT_DECLARE_SCRIPT_METHOD(TaskComplete_3);
   UT_DECLARE_SCRIPT_METHOD(TaskComplete_4);

   UT_DECLARE_SCRIPT_METHOD(AssignedTaskList_1);
   UT_DECLARE_SCRIPT_METHOD(AssignedTaskList_2);
   UT_DECLARE_SCRIPT_METHOD(ReceivedTaskList_1);
   UT_DECLARE_SCRIPT_METHOD(ReceivedTaskList_2);
   UT_DECLARE_SCRIPT_METHOD(TasksAssigned);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedTo_1);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedTo_2);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedTo_3);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedTo_4);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedTo_5);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedTo_6);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedFor_1);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedFor_2);
   UT_DECLARE_SCRIPT_METHOD(TasksReceived);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedFor_1);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedFor_2);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceAssigned);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceLastTaskAssigned);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceLastTaskReceived);

   UT_DECLARE_SCRIPT_METHOD(StartLocalTracking);
   UT_DECLARE_SCRIPT_METHOD(StartRemoteTracking_1);
   UT_DECLARE_SCRIPT_METHOD(StartRemoteTracking_2);
   UT_DECLARE_SCRIPT_METHOD(StopLocalTracking);
   UT_DECLARE_SCRIPT_METHOD(StopRemoteTracking);

   UT_DECLARE_SCRIPT_METHOD(LocalOperatingLevelFor);
   UT_DECLARE_SCRIPT_METHOD(RemoteOperatingLevelFor);
   UT_DECLARE_SCRIPT_METHOD(ChangeLocalOperatingLevel);
   UT_DECLARE_SCRIPT_METHOD(ChangeRemoteOperatingLevel_1);
   UT_DECLARE_SCRIPT_METHOD(ChangeRemoteOperatingLevel_2);
};

#endif
