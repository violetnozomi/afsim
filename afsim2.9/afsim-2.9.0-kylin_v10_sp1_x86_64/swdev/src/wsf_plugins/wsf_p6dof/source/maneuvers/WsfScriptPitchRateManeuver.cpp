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

#include "WsfScriptPitchRateManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPitchRateManeuver.hpp"

WsfScriptPitchRateManeuver::WsfScriptPitchRateManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPitchRateManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPitchRate>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchRateManeuver, WsfPitchRateManeuver, Construct, 1, "WsfPitchRateManeuver", "double")
{
   WsfPitchRateManeuver* mvrPtr = new WsfPitchRateManeuver{};
   mvrPtr->SetRate(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchRateManeuver, WsfPitchRateManeuver, GetPitchRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRate() * UtMath::cDEG_PER_RAD);
}
