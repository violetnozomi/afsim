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

#include "WsfScriptPitchGLoadManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfPitchGLoadManeuver.hpp"

WsfScriptPitchGLoadManeuver::WsfScriptPitchGLoadManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPitchGLoadManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetG_Load>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchGLoadManeuver, WsfPitchGLoadManeuver, Construct, 1, "WsfPitchGLoadManeuver", "double")
{
   WsfPitchGLoadManeuver* mvrPtr = new WsfPitchGLoadManeuver{};
   mvrPtr->SetG_Load(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPitchGLoadManeuver, WsfPitchGLoadManeuver, GetG_Load, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetG_Load());
}
