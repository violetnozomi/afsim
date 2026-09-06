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

#include "WsfTrackDropMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! Constructor, for XIO (de)serialization.
WsfTrackDropMessage::WsfTrackDropMessage()
   : WsfMessage(GetTypeId())
   , mTime(0.0)
   , mTrackId()
   , mTargetIndex(0)
{
}

//! Constructor.
//!   @param aPlatformPtr The originator of the message.
//!   @param aTime   The simulation time at which the track was dropped.
//!   @param aTrackId   The platforms track ID corresponding to the dropped track.
//!   @param aPlatformIndex the (truth) platform index corresponding to the dropped track.
WsfTrackDropMessage::WsfTrackDropMessage(WsfPlatform* aPlatformPtr, double aTime, const WsfTrackId& aTrackId, size_t aPlatformIndex)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mTime(aTime)
   , mTrackId(aTrackId)
   , mTargetIndex(aPlatformIndex)
{
}

// static
//! Get the message type ID for messages of this class.
//!   @returns The integer message type ID for messages of this type.
WsfStringId WsfTrackDropMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_DROP_TRACK_MESSAGE");
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptTrackDropMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTrackDropMessageClass(const std::string& aClassName, UtScriptTypes* aTypePtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Time);
   UT_DECLARE_SCRIPT_METHOD(TrackId);
   UT_DECLARE_SCRIPT_METHOD(TargetIndex);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTrackDropMessage::CreateScriptClass(const std::string& aClassName,
                                                                      UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTrackDropMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTrackDropMessage::GetScriptClassName() const
{
   return "WsfTrackDropMessage";
}

WsfScriptTrackDropMessageClass::WsfScriptTrackDropMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTrackDropMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<Time>());
   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<TargetIndex>());
}

// virtual
void* WsfScriptTrackDropMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);
   double       simTime     = WsfScriptContext::GetTIME_NOW(aInstance);
   WsfTrack*    trackPtr    = WsfScriptContext::GetTRACK(aInstance);

   if ((platformPtr != nullptr) && (trackPtr != nullptr))
   {
      return new WsfTrackDropMessage(platformPtr, simTime, trackPtr->GetTrackId(), trackPtr->GetTargetIndex());
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfTrackDropMessage* messagePtr = new WsfTrackDropMessage();
      WsfSimulation*       simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

// virtual
void* WsfScriptTrackDropMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfTrackDropMessage*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptTrackDropMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfTrackDropMessage*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackDropMessageClass, WsfTrackDropMessage, Time, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackDropMessageClass, WsfTrackDropMessage, TrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = const_cast<WsfTrackId*>(&aObjectPtr->GetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackDropMessageClass, WsfTrackDropMessage, TargetIndex, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetTargetIndex()));
}
