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

#include "WsfScriptFlightPathAngleManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfFlightPathAngleManeuver.hpp"

WsfScriptFlightPathAngleManeuver::WsfScriptFlightPathAngleManeuver(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFlightPathAngleManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetFlightPathAngle>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFlightPathAngleManeuver,
                        WsfFlightPathAngleManeuver,
                        Construct,
                        1,
                        "WsfFlightPathAngleManeuver",
                        "double")
{
   WsfFlightPathAngleManeuver* mvrPtr = new WsfFlightPathAngleManeuver{};
   mvrPtr->SetFlightPathAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFlightPathAngleManeuver, WsfFlightPathAngleManeuver, GetFlightPathAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFlightPathAngle() * UtMath::cDEG_PER_RAD);
}
