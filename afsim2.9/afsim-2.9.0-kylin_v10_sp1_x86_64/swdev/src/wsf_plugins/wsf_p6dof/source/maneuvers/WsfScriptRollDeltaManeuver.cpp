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

#include "WsfScriptRollDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfRollDeltaManeuver.hpp"

WsfScriptRollDeltaManeuver::WsfScriptRollDeltaManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRollDeltaManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetRollDelta>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRollDeltaManeuver, WsfRollDeltaManeuver, Construct, 1, "WsfRollDeltaManeuver", "double")
{
   WsfRollDeltaManeuver* mvrPtr = new WsfRollDeltaManeuver{};
   mvrPtr->SetDelta(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRollDeltaManeuver, WsfRollDeltaManeuver, GetRollDelta, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDelta() * UtMath::cDEG_PER_RAD);
}
