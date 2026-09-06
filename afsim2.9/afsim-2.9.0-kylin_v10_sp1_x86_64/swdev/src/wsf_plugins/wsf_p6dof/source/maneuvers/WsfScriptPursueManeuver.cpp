// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptPursueManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfPursueManeuver.hpp"

WsfScriptPursueManeuver::WsfScriptPursueManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptRelativeManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPursueManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPursuitDistanceMeters>());
   AddMethod(ut::make_unique<SetPursuitDistanceMeters>());
   AddMethod(ut::make_unique<GetTargetPlatformName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPursueManeuver, WsfPursueManeuver, Construct, 2, "WsfPursueManeuver", "string, double")
{
   auto mvrPtr = ut::make_unique<WsfPursueManeuver>();
   mvrPtr->SetTargetPlatformName(aVarArgs[0].GetString());
   mvrPtr->SetPursuitDistanceMeters(aVarArgs[1].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPursueManeuver, WsfPursueManeuver, GetPursuitDistanceMeters, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPursuitDistanceMeters());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPursueManeuver, WsfPursueManeuver, SetPursuitDistanceMeters, 1, "void", "double")
{
   aObjectPtr->SetPursuitDistanceMeters(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPursueManeuver, WsfPursueManeuver, GetTargetPlatformName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTargetPlatformName());
}
