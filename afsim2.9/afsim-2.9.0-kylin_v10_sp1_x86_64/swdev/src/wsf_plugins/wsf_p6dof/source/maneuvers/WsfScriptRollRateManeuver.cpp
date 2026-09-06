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

#include "WsfScriptRollRateManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfRollRateManeuver.hpp"

WsfScriptRollRateManeuver::WsfScriptRollRateManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRollRateManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetRollRate>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRollRateManeuver, WsfRollRateManeuver, Construct, 1, "WsfRollRateManeuver", "double")
{
   WsfRollRateManeuver* mvrPtr = new WsfRollRateManeuver{};
   mvrPtr->SetRate(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRollRateManeuver, WsfRollRateManeuver, GetRollRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRate() * UtMath::cDEG_PER_RAD);
}
