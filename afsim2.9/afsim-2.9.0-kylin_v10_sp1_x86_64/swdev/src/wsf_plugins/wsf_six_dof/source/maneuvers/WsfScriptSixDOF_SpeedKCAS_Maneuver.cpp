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

#include "WsfScriptSixDOF_SpeedKCAS_Maneuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_SpeedKCAS_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
SpeedKCAS_Maneuver::SpeedKCAS_Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_SpeedKCAS_Maneuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetSpeed>());
}

UT_DEFINE_SCRIPT_METHOD(SpeedKCAS_Maneuver, wsf::six_dof::SpeedKCAS_Maneuver, Construct, 1, "WsfSixDOF_SpeedKCAS_Maneuver", "double")
{
   wsf::six_dof::SpeedKCAS_Maneuver* mvrPtr = new wsf::six_dof::SpeedKCAS_Maneuver{};
   mvrPtr->SetSpeed(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(SpeedKCAS_Maneuver, wsf::six_dof::SpeedKCAS_Maneuver, GetSpeed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeed());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
