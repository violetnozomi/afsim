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

#ifndef WSFSCRIPTQUANTUMTASKERPROCESSOR_HPP
#define WSFSCRIPTQUANTUMTASKERPROCESSOR_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptProcessorClass.hpp"


class WSF_MIL_EXPORT WsfScriptQuantumTaskerProcessor : public WsfScriptProcessorClass
{
public:
   WsfScriptQuantumTaskerProcessor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   // See the base class for a description of Create, Clone, and Destroy.

   void*            Create(const UtScriptContext& aContext) override;
   void*            Clone(void* aObjectPtr) override;
   void             Destroy(void* aObjectPtr) override;
   UtScriptContext* GetContext(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(TasksReceived);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedOfType);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedOfTypeForTarget);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedForResourceType);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedForResourceName);
   UT_DECLARE_SCRIPT_METHOD(TasksReceivedForTarget);
   UT_DECLARE_SCRIPT_METHOD(SetTaskProgress);
   UT_DECLARE_SCRIPT_METHOD(SetTaskComplete);
   UT_DECLARE_SCRIPT_METHOD(SetTaskComplete_2);
   UT_DECLARE_SCRIPT_METHOD(RejectTask);
   UT_DECLARE_SCRIPT_METHOD(CancelTask);

   UT_DECLARE_SCRIPT_METHOD(TasksAssigned);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedOfType);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedForResourceType);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedForResourceName);
   UT_DECLARE_SCRIPT_METHOD(TasksAssignedForTarget);

   UT_DECLARE_SCRIPT_METHOD(TasksConsidered);
   UT_DECLARE_SCRIPT_METHOD(AssetsConsidered);
   UT_DECLARE_SCRIPT_METHOD(ValuesConsidered);
   UT_DECLARE_SCRIPT_METHOD(AssetAssigneesFor);
};

#endif
