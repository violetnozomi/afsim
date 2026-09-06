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

#include "WsfScriptSixDOF_RollAngleManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_RollAngleManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
RollAngleManeuver::RollAngleManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_RollAngleManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetRollAngle>());
}

UT_DEFINE_SCRIPT_METHOD(RollAngleManeuver, wsf::six_dof::RollAngleManeuver, Construct, 1, "WsfSixDOF_RollAngleManeuver", "double")
{
   wsf::six_dof::RollAngleManeuver* mvrPtr = new wsf::six_dof::RollAngleManeuver{};
   mvrPtr->SetAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(RollAngleManeuver, wsf::six_dof::RollAngleManeuver, GetRollAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAngle() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
