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

#include "WsfTaskStatusMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTask.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization.
WsfTaskStatusMessage::WsfTaskStatusMessage()
   : WsfMessage(GetTypeId())
   , mAcknowledgeId(0)
   , mTaskId(0)
   , mAssignerPlatformIndex(0)
   , mAssignerPlatformName()
   , mAssignerProcessorName()
   , mAssigneePlatformIndex(0)
   , mAssigneePlatformName()
   , mAssigneeProcessorName()
   , mStatus()
   , mSubStatus()
   , mTrackId()
   , mTaskType()
   , mResourceName()
   , mObjectPlatformIndex(0)
   , mIsFromAssignee(true)
{
}

//! Constructor
//! @param aPlatformPtr The originator of the message.
WsfTaskStatusMessage::WsfTaskStatusMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), nullptr, aPlatformPtr)
   , mAcknowledgeId(0)
   , mTaskId(0)
   , mAssignerPlatformIndex(0)
   , mAssignerPlatformName()
   , mAssignerProcessorName()
   , mAssigneePlatformIndex(aPlatformPtr->GetIndex())
   , mAssigneePlatformName(aPlatformPtr->GetNameId())
   , mAssigneeProcessorName()
   , mStatus()
   , mSubStatus()
   , mTrackId()
   , mTaskType()
   , mResourceName()
   , mObjectPlatformIndex(0)
   , mIsFromAssignee(true)
{
}

//! Constructor.
//! @param aStatus    The status as a string ID.
//! @param aSenderPtr The originator of the message. In most cases this will be the assignee, but it
//! may be the assigner if it is sending the acknowledgment of the task complete status.
//! @param aTask      The task whose status is being reported.
WsfTaskStatusMessage::WsfTaskStatusMessage(WsfStringId aStatus, WsfPlatform* aSenderPtr, const WsfTask& aTask)
   : WsfMessage(GetTypeId(), WsfStringId(), aSenderPtr)
   , mAcknowledgeId(0)
   , mTaskId(aTask.GetTaskId())
   , mAssignerPlatformIndex(aTask.GetAssignerPlatformIndex())
   , mAssignerPlatformName(aTask.GetAssignerPlatformName())
   , mAssignerProcessorName(aTask.GetAssignerProcessorName())
   , mAssigneePlatformIndex(aTask.GetAssigneePlatformIndex())
   , mAssigneePlatformName(aTask.GetAssigneePlatformName())
   , mAssigneeProcessorName(aTask.GetAssigneeProcessorName())
   , mStatus(aStatus)
   , mSubStatus()
   , mTrackId(aTask.GetTrackId())
   , mTaskType(aTask.GetTaskType())
   , mResourceName(aTask.GetResourceName())
   , mObjectPlatformIndex(0)
   , mIsFromAssignee(true)
{
}

//! Create a clone of this message (the 'virtual copy constructor').
//! @return The pointer to the copy of this message.
// virtual
WsfMessage* WsfTaskStatusMessage::Clone() const
{
   return new WsfTaskStatusMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfTaskStatusMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_TASK_STATUS_MESSAGE");
}

//! Return the pointer to the assigned platform.
//! @return The pointer to the assigned platform, or zero if the platform has been deleted.
WsfPlatform* WsfTaskStatusMessage::GetAssignee() const
{
   return GetSimulation()->GetPlatformByIndex(mAssigneePlatformIndex);
}

//! Return the pointer to the assigning platform.
//! @return The pointer to the assigning platform, or zero if the platform has been deleted.
WsfPlatform* WsfTaskStatusMessage::GetAssigner() const
{
   return GetSimulation()->GetPlatformByIndex(mAssignerPlatformIndex);
}

// ***************************************************************************

class WSF_EXPORT WsfScriptTaskStatusMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTaskStatusMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);

   UT_DECLARE_SCRIPT_METHOD(Assigner);
   UT_DECLARE_SCRIPT_METHOD(AssignerName);
   UT_DECLARE_SCRIPT_METHOD(Assignee);
   UT_DECLARE_SCRIPT_METHOD(AssigneeName);
   UT_DECLARE_SCRIPT_METHOD(Status);
   UT_DECLARE_SCRIPT_METHOD(SetStatus);
   UT_DECLARE_SCRIPT_METHOD(SubStatus);
   UT_DECLARE_SCRIPT_METHOD(SetSubStatus);
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
std::unique_ptr<UtScriptClass> WsfTaskStatusMessage::CreateScriptClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTaskStatusMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTaskStatusMessage::GetScriptClassName() const
{
   return "WsfTaskStatusMessage";
}


WsfScriptTaskStatusMessageClass::WsfScriptTaskStatusMessageClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTaskStatusMessage");

   AddMethod(ut::make_unique<Assigner>());
   AddMethod(ut::make_unique<AssignerName>());
   AddMethod(ut::make_unique<Assignee>());
   AddMethod(ut::make_unique<AssigneeName>());
   AddMethod(ut::make_unique<Status>());
   AddMethod(ut::make_unique<SubStatus>());
   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<TaskType>());
   AddMethod(ut::make_unique<ResourceName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, Assigner, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetAssigner(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, AssignerName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssignerPlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, Assignee, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetAssignee(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, AssigneeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssigneePlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, Status, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetStatus());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, SubStatus, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSubStatus());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, TrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, TaskType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTaskType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskStatusMessageClass, WsfTaskStatusMessage, ResourceName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetResourceName());
}
