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

#include "WsfAssociationMessage.hpp"

#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization.
WsfAssociationMessage::WsfAssociationMessage()
   : WsfMessage(GetTypeId())
   , mSensorPlatformIndex(0)
   , mSensorName(nullptr)
   , mSubjectId()
   , mAssociatedId()
{
}

//! Constructor (Create an Association Message).
//! @param aPlatformPtr The sender of the message.
WsfAssociationMessage::WsfAssociationMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mSensorPlatformIndex(aPlatformPtr->GetIndex())
   , mSensorName(nullptr)
   , mSubjectId()
   , mAssociatedId()
{
}

//! Constructor (Create an Association Message).
//! @param aSensorPtr The sensor that is producing the subject track.
WsfAssociationMessage::WsfAssociationMessage(WsfSensor* aSensorPtr)
   : WsfMessage(GetTypeId(), aSensorPtr->GetPlatform())
   , mSensorPlatformIndex(aSensorPtr->GetPlatform()->GetIndex())
   , mSensorName(aSensorPtr->GetNameId())
   , mSubjectId()
   , mAssociatedId()
{
}

//! Create a clone of this message (the 'virtual copy constructor').
//! @return The pointer to the copy of this message.
// virtual
WsfMessage* WsfAssociationMessage::Clone() const
{
   return new WsfAssociationMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfAssociationMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_ASSOCIATION_MESSAGE");
}

//! Return the pointer to the sensing platform.
//! @return The pointer to the sensing platform, or zero if the platform has been deleted.
WsfPlatform* WsfAssociationMessage::GetSensorPlatform() const
{
   return GetSimulation()->GetPlatformByIndex(mSensorPlatformIndex);
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptAssociationMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptAssociationMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(SensorPlatform);
   UT_DECLARE_SCRIPT_METHOD(SensorName);
   UT_DECLARE_SCRIPT_METHOD(SetSensorName);
   UT_DECLARE_SCRIPT_METHOD(SubjectId);
   UT_DECLARE_SCRIPT_METHOD(SetSubjectId);
   UT_DECLARE_SCRIPT_METHOD(AssociatedId);
   UT_DECLARE_SCRIPT_METHOD(SetAssociatedId);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfAssociationMessage::CreateScriptClass(const std::string& aClassName,
                                                                        UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptAssociationMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfAssociationMessage::GetScriptClassName() const
{
   return "WsfAssociationMessage";
}


WsfScriptAssociationMessageClass::WsfScriptAssociationMessageClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAssociationMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<SensorPlatform>());
   AddMethod(ut::make_unique<SensorName>());
   AddMethod(ut::make_unique<SetSensorName>());
   AddMethod(ut::make_unique<SubjectId>());
   AddMethod(ut::make_unique<SetSubjectId>());
   AddMethod(ut::make_unique<AssociatedId>());
   AddMethod(ut::make_unique<SetAssociatedId>());
}

// virtual
void* WsfScriptAssociationMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfAssociationMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfAssociationMessage* messagePtr = new WsfAssociationMessage();
      WsfSimulation*         simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

// virtual
void* WsfScriptAssociationMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfAssociationMessage*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptAssociationMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfAssociationMessage*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, SensorPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetSensorPlatform(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, SensorName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorName().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, SetSensorName, 1, "void", "string")
{
   aObjectPtr->SetSensorName(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, SubjectId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetSubjectId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, SetSubjectId, 1, "void", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetSubjectId(*trackIdPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, AssociatedId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetAssociatedId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssociationMessageClass, WsfAssociationMessage, SetAssociatedId, 1, "void", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAssociatedId(*trackIdPtr);
}
