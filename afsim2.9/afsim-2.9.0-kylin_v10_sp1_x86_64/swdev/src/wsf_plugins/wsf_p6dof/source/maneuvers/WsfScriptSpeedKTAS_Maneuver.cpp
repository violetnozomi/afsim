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

#include "WsfScriptSpeedKTAS_Maneuver.hpp"

#include "UtMemory.hpp"
#include "WsfSpeedKTAS_Maneuver.hpp"

WsfScriptSpeedKTAS_Maneuver::WsfScriptSpeedKTAS_Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSpeedKTAS_Maneuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetSpeed>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpeedKTAS_Maneuver, WsfSpeedKTAS_Maneuver, Construct, 1, "WsfSpeedKTAS_Maneuver", "double")
{
   WsfSpeedKTAS_Maneuver* mvrPtr = new WsfSpeedKTAS_Maneuver{};
   mvrPtr->SetSpeed(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpeedKTAS_Maneuver, WsfSpeedKTAS_Maneuver, GetSpeed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeed());
}
