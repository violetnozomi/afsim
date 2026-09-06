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

#include "WsfTrackNotifyMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

/** Constructor.
   @param aPlatformPtr The originator of the message.
   @param aTrackId   The local track ID corresponding with the track of interest.
   @param aTrackProcessorNameId The name of the track processor that originated this message.
   @param aChangedReason The changed reason as enumerated in WsfTrackManager.  These
      can be one of (CREATED, UPDATED, DROPPED, REMOVED).
*/
WsfTrackNotifyMessage::WsfTrackNotifyMessage(WsfPlatform*      aPlatformPtr,
                                             const WsfTrackId& aTrackId,
                                             WsfStringId       aTrackProcessorNameId,
                                             int               aChangedReason)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mTrackId(aTrackId)
   , mTrackProcessorNameId(aTrackProcessorNameId)
   , mChangedReason(aChangedReason)
{
}

// virtual
WsfTrackNotifyMessage::~WsfTrackNotifyMessage() {}

// static
//! Return the specific type id corresponding with the name of this message type (WSF_TRACK_NOTIFY_MESSAGE).
WsfStringId WsfTrackNotifyMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_TRACK_NOTIFY_MESSAGE");
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptTrackNotifyMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTrackNotifyMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(TrackId);            // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(ChangedReason);      // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(TrackProcessorName); // NO_DOC | PENDING
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTrackNotifyMessage::CreateScriptClass(const std::string& aClassName,
                                                                        UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTrackNotifyMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTrackNotifyMessage::GetScriptClassName() const
{
   return "WsfTrackNotifyMessage";
}


WsfScriptTrackNotifyMessageClass::WsfScriptTrackNotifyMessageClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTrackNotifyMessage");

   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<TrackProcessorName>());
   AddMethod(ut::make_unique<ChangedReason>());
}

// virtual
void* WsfScriptTrackNotifyMessageClass::Create(const UtScriptContext& aInstance)
{
   return nullptr; // Not default-constructible
}

// virtual
void* WsfScriptTrackNotifyMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfTrackNotifyMessage*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptTrackNotifyMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfTrackNotifyMessage*>(aObjectPtr);
}

// Define all the script methods

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackNotifyMessageClass, WsfTrackNotifyMessage, TrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = const_cast<WsfTrackId*>(&aObjectPtr->GetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackNotifyMessageClass, WsfTrackNotifyMessage, ChangedReason, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetChangedReason());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackNotifyMessageClass, WsfTrackNotifyMessage, TrackProcessorName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTrackProcessorNameId());
}
