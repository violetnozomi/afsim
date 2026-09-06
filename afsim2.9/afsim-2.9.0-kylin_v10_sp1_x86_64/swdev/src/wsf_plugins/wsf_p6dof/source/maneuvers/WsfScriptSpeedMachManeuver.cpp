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

#include "WsfScriptSpeedMachManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfSpeedMachManeuver.hpp"

WsfScriptSpeedMachManeuver::WsfScriptSpeedMachManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSpeedMachManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetMach>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpeedMachManeuver, WsfSpeedMachManeuver, Construct, 1, "WsfSpeedMachManeuver", "double")
{
   WsfSpeedMachManeuver* mvrPtr = new WsfSpeedMachManeuver{};
   mvrPtr->SetMach(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpeedMachManeuver, WsfSpeedMachManeuver, GetMach, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMach());
}
