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

#include "WsfTaskControlMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTask.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization.
WsfTaskControlMessage::WsfTaskControlMessage()
   : WsfMessage(GetTypeId())
   , mAssignerPlatformIndex(0)
   , mAssignerPlatformName()
   , mAssignerProcessorName()
   , mAssigneePlatformIndex(0)
   , mAssigneePlatformName()
   , mAssigneeProcessorName()
   , mName()
   , mLevel(-1)
{
}

//! Constructor
//! @param aXmtrPtr The processor that is sending the control message.
//! @param aRcvrPtr The platform that is to receive the control message.
WsfTaskControlMessage::WsfTaskControlMessage(WsfProcessor* aXmtrPtr, WsfPlatform* aRcvrPtr)
   : WsfMessage(GetTypeId(), nullptr, aXmtrPtr->GetPlatform())
   , mAssignerPlatformIndex(aXmtrPtr->GetPlatform()->GetIndex())
   , mAssignerPlatformName(aXmtrPtr->GetPlatform()->GetNameId())
   , mAssignerProcessorName(aXmtrPtr->GetNameId())
   , mAssigneePlatformIndex(aRcvrPtr->GetIndex())
   , mAssigneePlatformName(aRcvrPtr->GetNameId())
   , mAssigneeProcessorName(nullptr)
   , mName()
   , mLevel(-1)
{
}

//! Create a clone of this message (the 'virtual copy constructor').
//! @return The pointer to the copy of this message.
// virtual
WsfMessage* WsfTaskControlMessage::Clone() const
{
   return new WsfTaskControlMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfTaskControlMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_TASK_CONTROL_MESSAGE");
}

//! Return the pointer to the assigned platform.
//! @return The pointer to the assigned platform, or zero if the platform has been deleted.
WsfPlatform* WsfTaskControlMessage::GetAssignee() const
{
   return GetSimulation()->GetPlatformByIndex(mAssigneePlatformIndex);
}

//! Return the pointer to the assigning platform.
//! @return The pointer to the assigning platform, or zero if the platform has been deleted.
WsfPlatform* WsfTaskControlMessage::GetAssigner() const
{
   return GetSimulation()->GetPlatformByIndex(mAssignerPlatformIndex);
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptTaskControlMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTaskControlMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Assigner);
   UT_DECLARE_SCRIPT_METHOD(AssignerName);
   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(Level);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTaskControlMessage::CreateScriptClass(const std::string& aClassName,
                                                                        UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTaskControlMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTaskControlMessage::GetScriptClassName() const
{
   return "WsfTaskControlMessage";
}


WsfScriptTaskControlMessageClass::WsfScriptTaskControlMessageClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTaskControlMessage");

   AddMethod(ut::make_unique<Assigner>());
   AddMethod(ut::make_unique<AssignerName>());
   AddMethod(ut::make_unique<Name>());
   AddMethod(ut::make_unique<Level>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskControlMessageClass, WsfTaskControlMessage, Assigner, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr =
      WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByIndex(aObjectPtr->GetAssignerPlatformIndex());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskControlMessageClass, WsfTaskControlMessage, AssignerName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssignerPlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskControlMessageClass, WsfTaskControlMessage, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskControlMessageClass, WsfTaskControlMessage, Level, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetLevel());
}
