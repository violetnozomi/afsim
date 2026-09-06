// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_EntityMessage.hpp"

#include "UtMemory.hpp"
#include "WsfSA_EntityPerception.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"

WsfSA_EntityMessage::WsfSA_EntityMessage()
   : WsfMessage(GetTypeId())
   , mEntityPtr(nullptr)
{
}

WsfSA_EntityMessage::WsfSA_EntityMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mEntityPtr(nullptr)
{
}

WsfSA_EntityMessage::WsfSA_EntityMessage(WsfPlatform* aPlatformPtr, WsfSA_EntityPerception* aEntityPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mEntityPtr(aEntityPtr)
{
}

//! Copy constructor (for Clone())
WsfSA_EntityMessage::WsfSA_EntityMessage(const WsfSA_EntityMessage& aSrc)
   : WsfMessage(aSrc)
   , mEntityPtr(nullptr)
{
   if (aSrc.mEntityPtr != nullptr)
   {
      mEntityPtr = ut::clone(aSrc.mEntityPtr);
   }
}

void WsfSA_EntityMessage::SetEntity(WsfSA_EntityPerception* aEntityPtr)
{
   mEntityPtr = std::unique_ptr<WsfSA_EntityPerception>(aEntityPtr);
}

WsfStringId WsfSA_EntityMessage::GetTypeId()
{
   return "WSF_SA_ENTITY_MESSAGE";
}

bool WsfSA_EntityMessage::CanBeReplacedBy(const WsfMessage& aMessage) const
{
   bool canBeReplacedBy = false;
   if (aMessage.GetType() == GetTypeId())
   {
      const WsfSA_EntityMessage& message = static_cast<const WsfSA_EntityMessage&>(aMessage);
      if ((mEntityPtr != nullptr) && (message.mEntityPtr != nullptr) &&
          (mEntityPtr->GetEntityPlatformIndex() ==
           message.mEntityPtr->GetEntityPlatformIndex()) && // asset message for same platform
          (mEntityPtr->Time() <= message.mEntityPtr->Time()))
      {
         canBeReplacedBy = true;
      }
   }
   return canBeReplacedBy;
}

// =================================================================================================
// The script interface 'class'
// =================================================================================================

// =================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
UtScriptClass* WsfSA_EntityMessage::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return new WsfScriptSA_EntityMessage(aClassName, aScriptTypesPtr);
}

const char* WsfSA_EntityMessage::GetScriptClassName() const
{
   return "WsfSA_EntityMessage";
}

WsfScriptSA_EntityMessage::WsfScriptSA_EntityMessage(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSA_EntityMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<SetEntity>());
   AddMethod(ut::make_unique<Entity>());
}

void* WsfScriptSA_EntityMessage::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfSA_EntityMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfSA_EntityMessage* messagePtr = new WsfSA_EntityMessage();
      WsfSimulation*       simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

void* WsfScriptSA_EntityMessage::Clone(void* aObjectPtr)
{
   return static_cast<WsfSA_EntityMessage*>(aObjectPtr)->Clone();
}

void WsfScriptSA_EntityMessage::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfSA_EntityMessage*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityMessage, WsfSA_EntityMessage, SetEntity, 1, "void", "WsfSA_EntityPerception")
{
   WsfSA_EntityPerception* assetPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetEntity(assetPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityMessage, WsfSA_EntityMessage, Entity, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->GetEntity();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}
