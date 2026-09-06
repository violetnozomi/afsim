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

#include "WsfImageMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfImage.hpp"
#include "script/WsfScriptMessageClass.hpp"

// PROGRAMMING NOTE:
//
// Do not attempt to be tricky and avoid cloning the image. The image being
// pointed to on the constructor argument may be updated later while the
// message is in transit (such as by a sensor or fusion processor).  We can't
// magically allow a message to be modified after it has been sent!!!!

// =================================================================================================
WsfImageMessage::WsfImageMessage(WsfPlatform* aPlatformPtr, const WsfImage& aImage)

   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mImagePtr(aImage.Clone())
{
   SetSizeBits(mImagePtr->GetMessageLength());
   SetDataTag(mImagePtr->GetMessageDataTag());
}

// =================================================================================================
// protected
WsfImageMessage::WsfImageMessage(const WsfImageMessage& aSrc)
   : WsfMessage(aSrc)
   , mImagePtr(aSrc.mImagePtr->Clone())
{
}

// =================================================================================================
// virtual
WsfImageMessage::~WsfImageMessage()
{
   delete mImagePtr;
}

// =================================================================================================
// virtual
WsfMessage* WsfImageMessage::Clone() const
{
   return new WsfImageMessage(*this);
}

// =================================================================================================
// static
WsfStringId WsfImageMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_IMAGE_MESSAGE");
}
// =================================================================================================
// static
const char* WsfImageMessage::GetMessageType()
{
   return "WSF_IMAGE_MESSAGE";
}

// =================================================================================================
// virtual
WsfStringId WsfImageMessage::GetComponentNameId() const
{
   return mImagePtr->GetSensorNameId();
}

// =================================================================================================
// virtual
WsfStringId WsfImageMessage::GetComponentTypeId() const
{
   return mImagePtr->GetSensorTypeId();
}

// =================================================================================================
// virtual
WsfStringId WsfImageMessage::GetComponentModeId() const
{
   return mImagePtr->GetSensorModeId();
}

// =================================================================================================
// The script interface 'class'
// =================================================================================================

class WSF_EXPORT WsfScriptImageMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptImageMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(Image);
};

// =================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfImageMessage::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptImageMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfImageMessage::GetScriptClassName() const
{
   return "WsfImageMessage";
}


// =================================================================================================
WsfScriptImageMessageClass::WsfScriptImageMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfImageMessage");

   mCloneable = true;

   AddMethod(ut::make_unique<Image>());
}

// =================================================================================================
// virtual
void* WsfScriptImageMessageClass::Create(const UtScriptContext& aInstance)
{
   return nullptr;
}

// =================================================================================================
// virtual
void* WsfScriptImageMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfImageMessage*>(aObjectPtr)->Clone();
}

// =================================================================================================
// virtual
void WsfScriptImageMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfImageMessage*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageMessageClass, WsfImageMessage, Image, 0, "WsfImage", "")
{
   WsfImage* imagePtr = aObjectPtr->GetImage();
   aReturnVal.SetPointer(new UtScriptRef(imagePtr, aReturnClassPtr));
}
