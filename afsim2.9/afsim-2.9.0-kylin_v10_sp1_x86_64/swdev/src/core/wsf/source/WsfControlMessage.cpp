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

#include "WsfControlMessage.hpp"

#include <memory>

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization.
WsfControlMessage::WsfControlMessage()
   : WsfMessage(GetTypeId())
   , mRequestId()
   , mResourceId(nullptr)
   , mTrack()
   , mRoute()
{
}

//! Constructor (create a control message)
//!
//! @param aPlatformPtr The originator of the message.
WsfControlMessage::WsfControlMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mRequestId()
   , mResourceId(nullptr)
   , mTrack()
   , mRoute()
{
}

//! Copy constructor.
// protected
WsfControlMessage::WsfControlMessage(const WsfControlMessage& aSrc)
   : WsfMessage(aSrc)
   , mRequestId(aSrc.mRequestId)
   , mResourceId(aSrc.mResourceId)
   , mTrack(aSrc.mTrack)
   , mRoute(aSrc.mRoute)
{
}

WsfControlMessage& WsfControlMessage::operator=(const WsfControlMessage& aRhs)
{
   if (this != &aRhs)
   {
      WsfMessage::operator=(aRhs);
      mRequestId          = aRhs.mRequestId;
      mResourceId         = aRhs.mResourceId;
      mTrack              = aRhs.mTrack;
      mRoute              = aRhs.mRoute;
   }
   return *this;
}

// virtual
WsfMessage* WsfControlMessage::Clone() const
{
   return new WsfControlMessage(*this);
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId WsfControlMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_CONTROL_MESSAGE");
}

//! Get the function (string form).
WsfStringId WsfControlMessage::GetFunction() const
{
   return GetSubType();
}

//! Set the function (string form).
void WsfControlMessage::SetFunction(WsfStringId aFunction)
{
   SetSubType(aFunction);
}

//! Get the resource (string form).
std::string WsfControlMessage::GetResource() const
{
   return mResourceId.GetString();
}

//! Set the resource (string form).
void WsfControlMessage::SetResource(const std::string& aResource)
{
   mResourceId = aResource;
}

//! Set the route.
//!
//! A command may or may not have an associate route.
//!
//! @param aRoute The route to be associated with the command.

void WsfControlMessage::SetRoute(const WsfRoute& aRoute)
{
   mRoute = aRoute;
}

//! Set the track to be associated with the message.
//!
//! A command may or may not have an associated track.
//!
//! @param aTrack The associated track.

void WsfControlMessage::SetTrack(const WsfTrack& aTrack)
{
   mTrack = aTrack;
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptControlMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptControlMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(SetFunction);
   UT_DECLARE_SCRIPT_METHOD(Function);
   UT_DECLARE_SCRIPT_METHOD(RequestId);
   UT_DECLARE_SCRIPT_METHOD(SetRequestId);
   UT_DECLARE_SCRIPT_METHOD(SetResource);
   UT_DECLARE_SCRIPT_METHOD(Resource);
   UT_DECLARE_SCRIPT_METHOD(SetTrack);
   UT_DECLARE_SCRIPT_METHOD(Track);
   UT_DECLARE_SCRIPT_METHOD(SetRoute_1); // SetRoute(WsfRoute)
   UT_DECLARE_SCRIPT_METHOD(SetRoute_2); // SetRoute(string)
   UT_DECLARE_SCRIPT_METHOD(Route);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfControlMessage::CreateScriptClass(const std::string& aClassName,
                                                                    UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptControlMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfControlMessage::GetScriptClassName() const
{
   return "WsfControlMessage";
}


WsfScriptControlMessageClass::WsfScriptControlMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfControlMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<SetFunction>());
   AddMethod(ut::make_unique<Function>());
   AddMethod(ut::make_unique<RequestId>());
   AddMethod(ut::make_unique<SetRequestId>());
   AddMethod(ut::make_unique<SetResource>());
   AddMethod(ut::make_unique<Resource>());
   AddMethod(ut::make_unique<SetTrack>());
   AddMethod(ut::make_unique<Track>());
   AddMethod(ut::make_unique<SetRoute_1>("SetRoute")); // SetRoute(WsfRoute)
   AddMethod(ut::make_unique<SetRoute_2>("SetRoute")); // SetRoute(string)
   AddMethod(ut::make_unique<Route>());
}

// virtual
void* WsfScriptControlMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfControlMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfControlMessage* messagePtr = new WsfControlMessage();
      WsfSimulation*     simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

// virtual
void* WsfScriptControlMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfControlMessage*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptControlMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfControlMessage*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, SetFunction, 1, "void", "string")
{
   aObjectPtr->SetFunction(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, Function, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetFunction());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, RequestId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetRequestId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, SetRequestId, 1, "void", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetRequestId(*trackIdPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, SetResource, 1, "void", "string")
{
   aObjectPtr->SetResourceId(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, Resource, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetResourceId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, SetTrack, 1, "void", "WsfTrack")
{
   WsfTrack* trackPtr = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetTrack(*trackPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, Track, 0, "WsfTrack", "")
{
   WsfTrack* targetPtr = &(aObjectPtr->GetTrack());
   aReturnVal.SetPointer(new UtScriptRef(targetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, SetRoute_1, 1, "void", "WsfRoute")
{
   WsfRoute* routePtr = static_cast<WsfRoute*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetRoute(*routePtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, SetRoute_2, 1, "void", "string")
{
   WsfRoute* routePtr = WsfScriptContext::GetSCENARIO(aContext)->FindTypeT<WsfRoute>(aVarArgs[0].GetString());
   if (routePtr != nullptr)
   {
      aObjectPtr->SetRoute(*routePtr);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptControlMessageClass, WsfControlMessage, Route, 0, "WsfRoute", "")
{
   WsfRoute* routePtr = &(aObjectPtr->GetRoute());
   aReturnVal.SetPointer(new UtScriptRef(routePtr, aReturnClassPtr));
}
