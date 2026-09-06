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

#include "WsfScriptSixDOF_SpeedKTAS_Maneuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_SpeedKTAS_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
SpeedKTAS_Maneuver::SpeedKTAS_Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_SpeedKTAS_Maneuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetSpeed>());
}

UT_DEFINE_SCRIPT_METHOD(SpeedKTAS_Maneuver, wsf::six_dof::SpeedKTAS_Maneuver, Construct, 1, "WsfSixDOF_SpeedKTAS_Maneuver", "double")
{
   wsf::six_dof::SpeedKTAS_Maneuver* mvrPtr = new wsf::six_dof::SpeedKTAS_Maneuver{};
   mvrPtr->SetSpeed(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(SpeedKTAS_Maneuver, wsf::six_dof::SpeedKTAS_Maneuver, GetSpeed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeed());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
