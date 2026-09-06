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

#include "WsfScriptHoldCurrentSpeedManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfHoldCurrentSpeedManeuver.hpp"

WsfScriptHoldCurrentSpeedManeuver::WsfScriptHoldCurrentSpeedManeuver(const std::string& aClassName,
                                                                     UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfHoldCurrentSpeedManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptHoldCurrentSpeedManeuver,
                        WsfHoldCurrentSpeedManeuver,
                        Construct,
                        0,
                        "WsfHoldCurrentSpeedManeuver",
                        "")
{
   WsfHoldCurrentSpeedManeuver* mvrPtr = new WsfHoldCurrentSpeedManeuver{};
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
