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

#include "WsfScriptPitchDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPitchDeltaManeuver.hpp"

WsfScriptPitchDeltaManeuver::WsfScriptPitchDeltaManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPitchDeltaManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPitchDelta>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchDeltaManeuver, WsfPitchDeltaManeuver, Construct, 1, "WsfPitchDeltaManeuver", "double")
{
   WsfPitchDeltaManeuver* mvrPtr = new WsfPitchDeltaManeuver{};
   mvrPtr->SetDelta(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchDeltaManeuver, WsfPitchDeltaManeuver, GetPitchDelta, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDelta() * UtMath::cDEG_PER_RAD);
}
