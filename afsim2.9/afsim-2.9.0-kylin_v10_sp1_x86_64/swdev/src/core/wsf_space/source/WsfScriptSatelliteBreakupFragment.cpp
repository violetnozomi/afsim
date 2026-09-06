// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSatelliteBreakupFragment.hpp"

#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfSatelliteBreakupModel.hpp"

WsfScriptSatelliteBreakupFragment::WsfScriptSatelliteBreakupFragment(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass{"WsfSatelliteBreakupFragment", aScriptTypesPtr}
{
   SetClassName("WsfSatelliteBreakupFragment");

   AddMethod(ut::make_unique<GetLength>());
   AddMethod(ut::make_unique<GetAoverM>());
   AddMethod(ut::make_unique<GetMass>());
   AddMethod(ut::make_unique<GetArea>());
   AddMethod(ut::make_unique<GetDeltaV>());
   AddMethod(ut::make_unique<GetName>());
   AddMethod(ut::make_unique<IsInitialized>());
}

void WsfScriptSatelliteBreakupFragment::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfSatelliteBreakupModel::Fragment*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, GetLength, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mLength);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, GetAoverM, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mAoverM);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, GetMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mMass);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, GetArea, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mArea);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, GetDeltaV, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->mDeltaV_ECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, GetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->mName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSatelliteBreakupFragment, WsfSatelliteBreakupModel::Fragment, IsInitialized, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsInitialized());
}
