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

#include "WsfTaskCancelMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTask.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization.
WsfTaskCancelMessage::WsfTaskCancelMessage()
   : WsfMessage(GetTypeId())
   , mTaskId(0)
   , mAssignerPlatformIndex(0)
   , mAssignerPlatformName()
   , mAssignerProcessorName(nullptr)
   , mAssigneePlatformIndex(0)
   , mAssigneePlatformName(nullptr)
   , mAssigneeProcessorName(nullptr)
   , mTrackId()
   , mTaskType(nullptr)
   , mResourceName(nullptr)
{
}

//! Constructor
//! @param aPlatformPtr The originator of the message.
WsfTaskCancelMessage::WsfTaskCancelMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), nullptr, aPlatformPtr)
   , mTaskId(0)
   , mAssignerPlatformIndex(aPlatformPtr->GetIndex())
   , mAssignerPlatformName(aPlatformPtr->GetNameId())
   , mAssignerProcessorName(nullptr)
   , mAssigneePlatformIndex(0)
   , mAssigneePlatformName(nullptr)
   , mAssigneeProcessorName(nullptr)
   , mTrackId()
   , mTaskType(nullptr)
   , mResourceName(nullptr)
{
}

//! Constructor
//! @param aPlatformPtr The originator of the message.
//! @param aTask        The task to be canceled.
WsfTaskCancelMessage::WsfTaskCancelMessage(WsfPlatform* aPlatformPtr, const WsfTask& aTask)
   : WsfMessage(GetTypeId(), nullptr, aPlatformPtr)
   , mTaskId(aTask.GetTaskId())
   , mAssignerPlatformIndex(aPlatformPtr->GetIndex())
   , mAssignerPlatformName(aPlatformPtr->GetNameId())
   , mAssignerProcessorName(aTask.GetAssignerProcessorName())
   , mAssigneePlatformIndex(aTask.GetAssigneePlatformIndex())
   , mAssigneePlatformName(aTask.GetAssigneePlatformName())
   , mAssigneeProcessorName(aTask.GetAssigneeProcessorName())
   , mTrackId(aTask.GetTrackId())
   , mTaskType(aTask.GetTaskType())
   , mResourceName(aTask.GetResourceName())
{
}

//! Create a clone of this message (the 'virtual copy constructor').
//! @return The pointer to the copy of this message.
// virtual
WsfMessage* WsfTaskCancelMessage::Clone() const
{
   return new WsfTaskCancelMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfTaskCancelMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_TASK_CANCEL_MESSAGE");
}

//! Return the pointer to the assigning platform.
//! @return The pointer to the assigning platform, or zero if the platform has been deleted.
WsfPlatform* WsfTaskCancelMessage::GetAssigner() const
{
   return GetSimulation()->GetPlatformByIndex(mAssignerPlatformIndex);
}

//! Return the pointer to the assigned platform.
//! @return The pointer to the assigned platform, or zero if the platform has been deleted.
WsfPlatform* WsfTaskCancelMessage::GetAssignee() const
{
   return GetSimulation()->GetPlatformByIndex(mAssigneePlatformIndex);
}

// ***************************************************************************

class WSF_EXPORT WsfScriptTaskCancelMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTaskCancelMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);

   UT_DECLARE_SCRIPT_METHOD(Assigner);
   UT_DECLARE_SCRIPT_METHOD(AssignerName);
   UT_DECLARE_SCRIPT_METHOD(Assignee);
   UT_DECLARE_SCRIPT_METHOD(AssigneeName);
   UT_DECLARE_SCRIPT_METHOD(TrackId);
   UT_DECLARE_SCRIPT_METHOD(SetTrackId);
   UT_DECLARE_SCRIPT_METHOD(TaskType);
   UT_DECLARE_SCRIPT_METHOD(SetTaskType);
   UT_DECLARE_SCRIPT_METHOD(ResourceName);
   UT_DECLARE_SCRIPT_METHOD(SetResourceName);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTaskCancelMessage::CreateScriptClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTaskCancelMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTaskCancelMessage::GetScriptClassName() const
{
   return "WsfTaskCancelMessage";
}

WsfScriptTaskCancelMessageClass::WsfScriptTaskCancelMessageClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTaskCancelMessage");

   AddMethod(ut::make_unique<Assigner>());
   AddMethod(ut::make_unique<AssignerName>());
   AddMethod(ut::make_unique<Assignee>());
   AddMethod(ut::make_unique<AssigneeName>());
   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<TaskType>());
   AddMethod(ut::make_unique<ResourceName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, Assigner, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetAssigner(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, AssignerName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssignerPlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, Assignee, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetAssignee(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, AssigneeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssigneePlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, TrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, TaskType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTaskType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskCancelMessageClass, WsfTaskCancelMessage, ResourceName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetResourceName());
}
