// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptPitchAngleManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPitchAngleManeuver.hpp"

WsfScriptPitchAngleManeuver::WsfScriptPitchAngleManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPitchAngleManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPitchAngle>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchAngleManeuver, WsfPitchAngleManeuver, Construct, 1, "WsfPitchAngleManeuver", "double")
{
   WsfPitchAngleManeuver* mvrPtr = new WsfPitchAngleManeuver{};
   mvrPtr->SetAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchAngleManeuver, WsfPitchAngleManeuver, GetPitchAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAngle() * UtMath::cDEG_PER_RAD);
}
