// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSixDOF_PursueManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_PursueManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PursueManeuver::PursueManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : RelativeManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_PursueManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPursuitDistanceMeters>());
   AddMethod(ut::make_unique<SetPursuitDistanceMeters>());
   AddMethod(ut::make_unique<GetTargetPlatformName>());
}

UT_DEFINE_SCRIPT_METHOD(PursueManeuver, wsf::six_dof::PursueManeuver, Construct, 2, "WsfSixDOF_PursueManeuver", "string, double")
{
   auto mvrPtr = ut::make_unique<wsf::six_dof::PursueManeuver>();
   mvrPtr->SetTargetPlatformName(aVarArgs[0].GetString());
   mvrPtr->SetPursuitDistanceMeters(aVarArgs[1].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(PursueManeuver, wsf::six_dof::PursueManeuver, GetPursuitDistanceMeters, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPursuitDistanceMeters());
}

UT_DEFINE_SCRIPT_METHOD(PursueManeuver, wsf::six_dof::PursueManeuver, SetPursuitDistanceMeters, 1, "void", "double")
{
   aObjectPtr->SetPursuitDistanceMeters(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(PursueManeuver, wsf::six_dof::PursueManeuver, GetTargetPlatformName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTargetPlatformName());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
