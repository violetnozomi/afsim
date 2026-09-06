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

#include "WsfScriptRollAngleManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfRollAngleManeuver.hpp"

WsfScriptRollAngleManeuver::WsfScriptRollAngleManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRollAngleManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetRollAngle>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRollAngleManeuver, WsfRollAngleManeuver, Construct, 1, "WsfRollAngleManeuver", "double")
{
   WsfRollAngleManeuver* mvrPtr = new WsfRollAngleManeuver{};
   mvrPtr->SetAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRollAngleManeuver, WsfRollAngleManeuver, GetRollAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAngle() * UtMath::cDEG_PER_RAD);
}
