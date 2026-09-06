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

#include "WsfScriptSixDOF_PitchDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_PitchDeltaManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PitchDeltaManeuver::PitchDeltaManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_PitchDeltaManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPitchDelta>());
}

UT_DEFINE_SCRIPT_METHOD(PitchDeltaManeuver, wsf::six_dof::PitchDeltaManeuver, Construct, 1, "WsfSixDOF_PitchDeltaManeuver", "double")
{
   wsf::six_dof::PitchDeltaManeuver* mvrPtr = new wsf::six_dof::PitchDeltaManeuver{};
   mvrPtr->SetDelta(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(PitchDeltaManeuver, wsf::six_dof::PitchDeltaManeuver, GetPitchDelta, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDelta() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
