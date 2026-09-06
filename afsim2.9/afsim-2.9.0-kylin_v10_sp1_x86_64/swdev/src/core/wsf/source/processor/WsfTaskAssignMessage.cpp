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

#include "WsfTaskAssignMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization
WsfTaskAssignMessage::WsfTaskAssignMessage()
   : WsfMessage(GetTypeId())
   , mTask()
   , mTrack()
{
}

//! Constructor
//! @param aPlatformPtr The originator of the message.
WsfTaskAssignMessage::WsfTaskAssignMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mTask()
   , mTrack()
{
}

//! Copy constructor.
WsfTaskAssignMessage::WsfTaskAssignMessage(const WsfTaskAssignMessage& aSrc)
   : WsfMessage(aSrc)
   , mTask(aSrc.mTask)
   , mTrack(aSrc.mTrack)
{
}

// virtual
WsfMessage* WsfTaskAssignMessage::Clone() const
{
   return new WsfTaskAssignMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfTaskAssignMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_TASK_ASSIGN_MESSAGE");
}

//! Set the task to be assigned.
//! @param aTask The task to be assigned.
void WsfTaskAssignMessage::SetTask(const WsfTask& aTask)
{
   mTask = aTask;

   // The track ID on the receiving end is the local track ID on this end.
   mTask.SetTrackId(aTask.GetLocalTrackId());

   // The local track ID on the receiving end must be determined by the receiver.
   mTask.SetLocalTrackId(WsfTrackId());

   // Clear the fields in the transmitted track that must be filled in by the receiver.

   mTask.SetCommName(nullptr);
   mTask.SetObjectsPending(0);
}

//! Set the track to be sent.
//! @param aTrack the track to be associated with the assignment.
void WsfTaskAssignMessage::SetTrack(const WsfTrack& aTrack)
{
   mTrack = aTrack;
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptTaskAssignMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTaskAssignMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Assigner);
   UT_DECLARE_SCRIPT_METHOD(AssignerName);
   UT_DECLARE_SCRIPT_METHOD(Track);
   UT_DECLARE_SCRIPT_METHOD(TaskType);
   UT_DECLARE_SCRIPT_METHOD(ResourceName);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTaskAssignMessage::CreateScriptClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTaskAssignMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTaskAssignMessage::GetScriptClassName() const
{
   return "WsfTaskAssignMessage";
}


WsfScriptTaskAssignMessageClass::WsfScriptTaskAssignMessageClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTaskAssignMessage");

   AddMethod(ut::make_unique<Assigner>());
   AddMethod(ut::make_unique<AssignerName>());
   AddMethod(ut::make_unique<Track>());
   AddMethod(ut::make_unique<TaskType>());
   AddMethod(ut::make_unique<ResourceName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskAssignMessageClass, WsfTaskAssignMessage, Assigner, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr =
      WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByIndex(aObjectPtr->GetTask().GetAssignerPlatformIndex());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskAssignMessageClass, WsfTaskAssignMessage, AssignerName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTask().GetAssignerPlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskAssignMessageClass, WsfTaskAssignMessage, Track, 0, "WsfTrack", "")
{
   WsfTrack* targetPtr = &(aObjectPtr->GetTrack());
   aReturnVal.SetPointer(new UtScriptRef(targetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskAssignMessageClass, WsfTaskAssignMessage, TaskType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTask().GetTaskType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskAssignMessageClass, WsfTaskAssignMessage, ResourceName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTask().GetResourceName());
}
