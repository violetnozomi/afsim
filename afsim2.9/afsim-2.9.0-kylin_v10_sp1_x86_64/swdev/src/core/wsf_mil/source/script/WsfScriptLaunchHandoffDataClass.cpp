// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptLaunchHandoffDataClass.hpp"

#include "WsfLaunchHandoffData.hpp"

// =================================================================================================
WsfScriptLaunchHandoffDataClass::WsfScriptLaunchHandoffDataClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfLaunchHandoffData");

   // AddMethod(ut::make_unique<Originator>());
}

// =================================================================================================
// virtual
void* WsfScriptLaunchHandoffDataClass::Create(const UtScriptContext& aInstance)
{
   return nullptr;
}

// =================================================================================================
// virtual
void WsfScriptLaunchHandoffDataClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfLaunchHandoffData*>(aObjectPtr);
}

// =================================================================================================
// UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchHandoffDataClass, WsfLaunchHandoffData, Originator, 0, "WsfPlatform", "")
//{
//   WsfPlatform* platformPtr = WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByIndex(aObjectPtr->GetPlatformIndex());
//   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
//}
