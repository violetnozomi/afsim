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

#include "WsfVideoMessage.hpp"

#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfImage.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptMessageClass.hpp"

// PROGRAMMING NOTE:
//
// Do not attempt to be tricky and avoid cloning the image. The image being
// pointed to on the constructor argument may be updated later while the
// message is in transit (such as by a sensor or fusion processor).  We can't
// magically allow a message to be modified after it has been sent!!!!

WsfVideoMessage::WsfVideoMessage(WsfPlatform* aPlatformPtr, const WsfImage& aImage)

   : WsfMessage(GetTypeId(), aPlatformPtr)
   , mImagePtr(ut::clone(aImage))
{
   SetSizeBits(mImagePtr->GetMessageLength());
   SetDataTag(mImagePtr->GetMessageDataTag());
}

// virtual
WsfMessage* WsfVideoMessage::Clone() const
{
   return new WsfVideoMessage(*this);
}

// static
WsfStringId WsfVideoMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_VIDEO_MESSAGE");
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptVideoMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptVideoMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Image);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfVideoMessage::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptVideoMessageClass>(aClassName, aScriptTypesPtr);
}

const char* WsfVideoMessage::GetScriptClassName() const
{
   return "WsfVideoMessage";
}

const char* WsfVideoMessage::GetMessageType()
{
   return "WSF_VIDEO_MESSAGE";
}

WsfStringId WsfVideoMessage::GetComponentNameId() const
{
   return mImagePtr->GetSensorNameId();
}

WsfStringId WsfVideoMessage::GetComponentTypeId() const
{
   return mImagePtr->GetSensorTypeId();
}

WsfStringId WsfVideoMessage::GetComponentModeId() const
{
   return mImagePtr->GetSensorModeId();
}

WsfScriptVideoMessageClass::WsfScriptVideoMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptMessageClass(aClassName, aTypesPtr)
{
   SetClassName("WsfVideoMessage");

   mCloneable = true;

   AddMethod(ut::make_unique<Image>());
}

// virtual
void* WsfScriptVideoMessageClass::Create(const UtScriptContext& aInstance)
{
   return nullptr; // Not default constructible
}

// virtual
void* WsfScriptVideoMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfVideoMessage*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptVideoMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfVideoMessage*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptVideoMessageClass, WsfVideoMessage, Image, 0, "WsfImage", "")
{
   WsfImage* imagePtr = aObjectPtr->GetImage();
   aReturnVal.SetPointer(new UtScriptRef(imagePtr, aReturnClassPtr));
}
