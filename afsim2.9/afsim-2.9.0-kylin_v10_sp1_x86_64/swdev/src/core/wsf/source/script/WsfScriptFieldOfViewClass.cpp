// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptFieldOfViewClass.hpp"

#include "WsfFieldOfView.hpp"

// =================================================================================================
WsfScriptFieldOfViewClass::WsfScriptFieldOfViewClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFieldOfView");
   mIsScriptAccessible = true;
   mCloneable          = true;

   AddMethod(ut::make_unique<Type>());
}

// =================================================================================================
void* WsfScriptFieldOfViewClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfFieldOfView*>(aObjectPtr)->Clone();
}

// =================================================================================================
void WsfScriptFieldOfViewClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfFieldOfView*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptFieldOfViewClass, WsfFieldOfView, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}
