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

#include "WsfStatusMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization (create a Status message)
WsfStatusMessage::WsfStatusMessage()
   : WsfMessage(GetTypeId())
   , mRequestId()
   , mSystemNameId(nullptr)
   , mPlatformIndex(0)
{
}

//! Constructor (create a Status message)
//! @param aPlatformPtr The originator of the message.
WsfStatusMessage::WsfStatusMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), nullptr, aPlatformPtr)
   , mRequestId()
   , mSystemNameId(nullptr)
   , mPlatformIndex(0)
{
}

//! Constructor (create a Status message)
//! @param aStatusId    The status as a string ID.
//! @param aPlatformPtr The originator of the message.
WsfStatusMessage::WsfStatusMessage(WsfStringId aStatusId, WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aStatusId, aPlatformPtr)
   , mRequestId()
   , mSystemNameId(nullptr)
   , mPlatformIndex(0)
{
}

//! Create a clone of this message (the 'virtual copy constructor').
//! @return The pointer to the copy of this message.
// virtual
WsfMessage* WsfStatusMessage::Clone() const
{
   return new WsfStatusMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfStatusMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_STATUS_MESSAGE");
}

//! Get the Status (string form).
const std::string& WsfStatusMessage::GetStatus() const
{
   return GetSubType().GetString();
}

//! Set the Status (string form).
//! @param aStatus The status.
void WsfStatusMessage::SetStatus(WsfStringId aStatus)
{
   SetSubType(aStatus);
}

//! Get the system name (string form).
//! @return The system name, or an empty string if the status is not associated
//! with a particular system.
std::string WsfStatusMessage::GetSystemName() const
{
   return mSystemNameId.GetString();
}

//! Set the system name (string form).
//! @param aSystemName The system name to which the status applied.
//! If empty then the status does not apply to a particular system.
void WsfStatusMessage::SetSystemName(const std::string& aSystemName)
{
   mSystemNameId = aSystemName;
}

//! Get the pointer to the other platform.
//! @return The pointer to the platform or 0 if the status does not have an associated
//! platform or if the associated platform has been deleted.
WsfPlatform* WsfStatusMessage::GetPlatform() const
{
   return GetSimulation()->GetPlatformByIndex(mPlatformIndex);
}

//! Set the pointer to the other platform.
//! @param aPlatformPtr The pointer to the
void WsfStatusMessage::SetPlatform(WsfPlatform* aPlatformPtr)
{
   mPlatformIndex = 0;
   if (aPlatformPtr != nullptr)
   {
      mPlatformIndex = aPlatformPtr->GetIndex();
   }
}

// ***************************************************************************

class WSF_EXPORT WsfScriptStatusMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptStatusMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(Status);
   UT_DECLARE_SCRIPT_METHOD(SetStatus);
   UT_DECLARE_SCRIPT_METHOD(RequestId);
   UT_DECLARE_SCRIPT_METHOD(SetRequestId);
   UT_DECLARE_SCRIPT_METHOD(SystemName);
   UT_DECLARE_SCRIPT_METHOD(SetSystemName);
   UT_DECLARE_SCRIPT_METHOD(Platform);
   UT_DECLARE_SCRIPT_METHOD(SetPlatform);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfStatusMessage::CreateScriptClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptStatusMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfStatusMessage::GetScriptClassName() const
{
   return "WsfStatusMessage";
}


WsfScriptStatusMessageClass::WsfScriptStatusMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfStatusMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<Status>());
   AddMethod(ut::make_unique<SetStatus>());
   AddMethod(ut::make_unique<RequestId>());
   AddMethod(ut::make_unique<SetRequestId>());
   AddMethod(ut::make_unique<SystemName>());
   AddMethod(ut::make_unique<SetSystemName>());
   AddMethod(ut::make_unique<Platform>());
   AddMethod(ut::make_unique<SetPlatform>());
}

// virtual
void* WsfScriptStatusMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfStatusMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfStatusMessage* messagePtr = new WsfStatusMessage();
      WsfSimulation*    simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

// virtual
void* WsfScriptStatusMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfStatusMessage*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptStatusMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfStatusMessage*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, Status, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetStatusId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, SetStatus, 1, "void", "string")
{
   aObjectPtr->SetStatusId(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, RequestId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetRequestId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, SetRequestId, 1, "void", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetRequestId(*trackIdPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, SystemName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSystemNameId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, SetSystemName, 1, "void", "string")
{
   aObjectPtr->SetSystemNameId(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, Platform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = aObjectPtr->GetPlatform();
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStatusMessageClass, WsfStatusMessage, SetPlatform, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetPlatform(platformPtr);
}
