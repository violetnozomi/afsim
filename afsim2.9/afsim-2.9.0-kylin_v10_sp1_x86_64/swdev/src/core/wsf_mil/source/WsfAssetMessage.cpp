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

#include "WsfAssetMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"

WsfAssetMessage::WsfAssetMessage()
   : WsfMessage(GetTypeId())
   , mAssetPtr(nullptr)
{
}

WsfAssetMessage::WsfAssetMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mAssetPtr(nullptr)
{
}

WsfAssetMessage::WsfAssetMessage(WsfPlatform* aPlatformPtr, std::unique_ptr<WsfAssetPerception> aAssetPtr)
   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mAssetPtr(std::move(aAssetPtr))
{
}

void WsfAssetMessage::SetAsset(WsfAssetPerception* aAssetPtr)
{
   mAssetPtr = std::unique_ptr<WsfAssetPerception>(aAssetPtr);
}

// static
WsfStringId WsfAssetMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_ASSET_MESSAGE");
}

// virtual
bool WsfAssetMessage::CanBeReplacedBy(const WsfMessage& aMessage) const
{
   bool canBeReplacedBy = false;
   if (aMessage.GetType() == GetTypeId())
   {
      const WsfAssetMessage& message = static_cast<const WsfAssetMessage&>(aMessage);
      if ((mAssetPtr != nullptr) && (message.mAssetPtr != nullptr) &&
          (mAssetPtr->mIndex == message.mAssetPtr->mIndex) && // asset message for same platform
          (mAssetPtr->mTime <= message.mAssetPtr->mTime))
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
UtScriptClass* WsfAssetMessage::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return new WsfScriptAssetMessageClass(aClassName, aScriptTypesPtr);
}


const char* WsfAssetMessage::GetScriptClassName() const
{
   return "WsfAssetMessage";
}


WsfScriptAssetMessageClass::WsfScriptAssetMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAssetMessage");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<SetAsset>());
   AddMethod(ut::make_unique<Asset>());
}


// virtual
void* WsfScriptAssetMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfAssetMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfAssetMessage* messagePtr = new WsfAssetMessage();
      WsfSimulation*   simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}


// virtual
void* WsfScriptAssetMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfAssetMessage*>(aObjectPtr)->Clone();
}


// virtual
void WsfScriptAssetMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfAssetMessage*>(aObjectPtr);
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetMessageClass, WsfAssetMessage, SetAsset, 1, "void", "WsfAssetPerception")
{
   WsfAssetPerception* assetPtr = static_cast<WsfAssetPerception*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAsset(assetPtr);
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetMessageClass, WsfAssetMessage, Asset, 0, "WsfAssetPerception", "")
{
   WsfAssetPerception* assetPtr = aObjectPtr->GetAsset();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}
