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

#include "WsfScriptTurnToHeadingManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfTurnToHeadingManeuver.hpp"

WsfScriptTurnToHeadingManeuver::WsfScriptTurnToHeadingManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTurnToHeadingManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetHeading>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTurnToHeadingManeuver, WsfTurnToHeadingManeuver, Construct, 1, "WsfTurnToHeadingManeuver", "double")
{
   WsfTurnToHeadingManeuver* mvrPtr = new WsfTurnToHeadingManeuver(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTurnToHeadingManeuver, WsfTurnToHeadingManeuver, GetHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeading() * UtMath::cDEG_PER_RAD);
}
