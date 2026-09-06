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

#include "WsfTrackMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

// =================================================================================================
//! Constructor, for XIO (de)serialization.
WsfTrackMessage::WsfTrackMessage()
   : WsfMessage(GetTypeId())
   , mTrackPtr(nullptr)
   , mSenderId()
   , mReplyId()
{
}

// =================================================================================================
WsfTrackMessage::WsfTrackMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mTrackPtr(nullptr)
   , mSenderId()
   , mReplyId()
{
}

// =================================================================================================
WsfTrackMessage::WsfTrackMessage(WsfPlatform* aPlatformPtr, const WsfTrack& aTrack)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mTrackPtr(nullptr)
   , mSenderId()
   , mReplyId()
{
   SetTrack(aTrack);
}

// =================================================================================================
//! Copy constructor (for Clone())
WsfTrackMessage::WsfTrackMessage(const WsfTrackMessage& aSrc)
   : WsfMessage(aSrc)
   , mTrackPtr(nullptr)
   , mSenderId(aSrc.mSenderId)
   , mReplyId(aSrc.mReplyId)
{
   if (aSrc.mTrackPtr != nullptr)
   {
      mTrackPtr = aSrc.mTrackPtr->Clone();
   }
}

// =================================================================================================
// virtual
WsfTrackMessage::~WsfTrackMessage()
{
   delete mTrackPtr;
}

// =================================================================================================
//! Sets the message's track.
//! @param aTrack The track that will be cloned and stored.
void WsfTrackMessage::SetTrack(const WsfTrack& aTrack)
{
   delete mTrackPtr;
   mTrackPtr = aTrack.Clone();
   SetDataTag(mTrackPtr->GetMessageDataTag());
}

// =================================================================================================
// static
//! Get the message type ID for messages of this class.
//! @returns The integer message type ID for messages of this type.
WsfStringId WsfTrackMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_TRACK_MESSAGE");
}

// =================================================================================================
// virtual
bool WsfTrackMessage::CanBeReplacedBy(const WsfMessage& aMessage) const
{
   bool canBeReplacedBy = false;
   if (aMessage.GetType() == GetTypeId())
   {
      const WsfTrackMessage& message = static_cast<const WsfTrackMessage&>(aMessage);
      if ((mTrackPtr != nullptr) && (message.mTrackPtr != nullptr) &&
          (mTrackPtr->GetTrackId() == message.mTrackPtr->GetTrackId()))
      {
         canBeReplacedBy = true;
      }
   }
   return canBeReplacedBy;
}

// =================================================================================================
// virtual
WsfStringId WsfTrackMessage::GetComponentNameId() const
{
   return mTrackPtr->GetSensorNameId();
}

// =================================================================================================
// virtual
WsfStringId WsfTrackMessage::GetComponentTypeId() const
{
   return mTrackPtr->GetSensorTypeId();
}

// =================================================================================================
// virtual
WsfStringId WsfTrackMessage::GetComponentModeId() const
{
   return mTrackPtr->GetSensorModeId();
}

// =================================================================================================
// The script interface 'class'
// =================================================================================================

class WSF_EXPORT WsfScriptTrackMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptTrackMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(SetTrack);
   UT_DECLARE_SCRIPT_METHOD(Track);
};

// =================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTrackMessage::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTrackMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfTrackMessage::GetScriptClassName() const
{
   return "WsfTrackMessage";
}


// =================================================================================================
WsfScriptTrackMessageClass::WsfScriptTrackMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTrackMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<SetTrack>());
   AddMethod(ut::make_unique<Track>());
}

// =================================================================================================
// virtual
void* WsfScriptTrackMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfTrackMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfTrackMessage* messagePtr = new WsfTrackMessage();
      WsfSimulation*   simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

// =================================================================================================
// virtual
void* WsfScriptTrackMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfTrackMessage*>(aObjectPtr)->Clone();
}

// =================================================================================================
// virtual
void WsfScriptTrackMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfTrackMessage*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackMessageClass, WsfTrackMessage, SetTrack, 1, "void", "WsfTrack")
{
   WsfTrack* trackPtr = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetTrack(*trackPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackMessageClass, WsfTrackMessage, Track, 0, "WsfTrack", "")
{
   WsfTrack* trackPtr = aObjectPtr->GetTrack();
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr));
}
