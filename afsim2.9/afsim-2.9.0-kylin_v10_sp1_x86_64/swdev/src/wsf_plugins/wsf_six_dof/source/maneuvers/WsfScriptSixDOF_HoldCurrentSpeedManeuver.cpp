// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSixDOF_HoldCurrentSpeedManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_HoldCurrentSpeedManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
HoldCurrentSpeedManeuver::HoldCurrentSpeedManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_HoldCurrentSpeedManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(HoldCurrentSpeedManeuver,
                        wsf::six_dof::HoldCurrentSpeedManeuver,
                        Construct,
                        0,
                        "WsfSixDOF_HoldCurrentSpeedManeuver",
                        "")
{
   wsf::six_dof::HoldCurrentSpeedManeuver* mvrPtr = new wsf::six_dof::HoldCurrentSpeedManeuver{};
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
