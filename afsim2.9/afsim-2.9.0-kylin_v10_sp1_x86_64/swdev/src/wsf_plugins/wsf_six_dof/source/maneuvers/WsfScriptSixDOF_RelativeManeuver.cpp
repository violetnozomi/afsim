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

#include "WsfScriptSixDOF_RelativeManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_RelativeManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
RelativeManeuver::RelativeManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_RelativeManeuver");

   AddMethod(ut::make_unique<GetG_LoadMax>());
   AddMethod(ut::make_unique<SetG_LoadMax>());
   AddMethod(ut::make_unique<GetG_LoadMin>());
   AddMethod(ut::make_unique<SetG_LoadMin>());
   AddMethod(ut::make_unique<GetSpeedKTAS_Max>());
   AddMethod(ut::make_unique<SetSpeedKTAS_Max>());
   AddMethod(ut::make_unique<GetSpeedKTAS_Min>());
   AddMethod(ut::make_unique<SetSpeedKTAS_Min>());
   AddMethod(ut::make_unique<GetSpeedMpsMax>());
   AddMethod(ut::make_unique<SetSpeedMpsMax>());
   AddMethod(ut::make_unique<GetSpeedMpsMin>());
   AddMethod(ut::make_unique<SetSpeedMpsMin>());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, GetG_LoadMax, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetG_LoadMax());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, SetG_LoadMax, 1, "void", "double")
{
   aObjectPtr->SetG_LoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, GetG_LoadMin, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetG_LoadMin());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, SetG_LoadMin, 1, "void", "double")
{
   aObjectPtr->SetG_LoadMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, GetSpeedKTAS_Max, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeedMpsMax() * UtMath::cNMPH_PER_MPS);
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, SetSpeedKTAS_Max, 1, "void", "double")
{
   aObjectPtr->SetSpeedMpsMax(aVarArgs[0].GetDouble() / UtMath::cNMPH_PER_MPS);
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, GetSpeedKTAS_Min, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeedMpsMin() * UtMath::cNMPH_PER_MPS);
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, SetSpeedKTAS_Min, 1, "void", "double")
{
   aObjectPtr->SetSpeedMpsMin(aVarArgs[0].GetDouble() / UtMath::cNMPH_PER_MPS);
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, GetSpeedMpsMax, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeedMpsMax());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, SetSpeedMpsMax, 1, "void", "double")
{
   aObjectPtr->SetSpeedMpsMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, GetSpeedMpsMin, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSpeedMpsMin());
}

UT_DEFINE_SCRIPT_METHOD(RelativeManeuver, wsf::six_dof::RelativeManeuver, SetSpeedMpsMin, 1, "void", "double")
{
   aObjectPtr->SetSpeedMpsMin(aVarArgs[0].GetDouble());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
