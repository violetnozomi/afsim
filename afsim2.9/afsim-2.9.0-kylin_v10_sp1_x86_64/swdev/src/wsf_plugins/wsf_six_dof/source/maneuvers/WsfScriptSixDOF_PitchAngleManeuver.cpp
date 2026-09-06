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

#include "WsfScriptSixDOF_PitchAngleManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_PitchAngleManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PitchAngleManeuver::PitchAngleManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_PitchAngleManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPitchAngle>());
}

UT_DEFINE_SCRIPT_METHOD(PitchAngleManeuver, wsf::six_dof::PitchAngleManeuver, Construct, 1, "WsfSixDOF_PitchAngleManeuver", "double")
{
   wsf::six_dof::PitchAngleManeuver* mvrPtr = new wsf::six_dof::PitchAngleManeuver{};
   mvrPtr->SetAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(PitchAngleManeuver, wsf::six_dof::PitchAngleManeuver, GetPitchAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAngle() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
