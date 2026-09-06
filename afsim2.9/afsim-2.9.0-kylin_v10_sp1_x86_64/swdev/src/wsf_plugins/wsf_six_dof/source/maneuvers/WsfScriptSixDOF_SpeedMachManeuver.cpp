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

#include "WsfScriptSixDOF_SpeedMachManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_SpeedMachManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
SpeedMachManeuver::SpeedMachManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_SpeedMachManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetMach>());
}

UT_DEFINE_SCRIPT_METHOD(SpeedMachManeuver, wsf::six_dof::SpeedMachManeuver, Construct, 1, "WsfSixDOF_SpeedMachManeuver", "double")
{
   wsf::six_dof::SpeedMachManeuver* mvrPtr = new wsf::six_dof::SpeedMachManeuver{};
   mvrPtr->SetMach(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(SpeedMachManeuver, wsf::six_dof::SpeedMachManeuver, GetMach, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMach());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
