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

#include "WsfScriptSixDOF_RollDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_RollDeltaManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
RollDeltaManeuver::RollDeltaManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_RollDeltaManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetRollDelta>());
}

UT_DEFINE_SCRIPT_METHOD(RollDeltaManeuver, wsf::six_dof::RollDeltaManeuver, Construct, 1, "WsfSixDOF_RollDeltaManeuver", "double")
{
   wsf::six_dof::RollDeltaManeuver* mvrPtr = new wsf::six_dof::RollDeltaManeuver{};
   mvrPtr->SetDelta(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(RollDeltaManeuver, wsf::six_dof::RollDeltaManeuver, GetRollDelta, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDelta() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
