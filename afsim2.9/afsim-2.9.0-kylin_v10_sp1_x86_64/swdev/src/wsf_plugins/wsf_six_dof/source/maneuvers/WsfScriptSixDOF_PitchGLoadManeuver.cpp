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

#include "WsfScriptSixDOF_PitchGLoadManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_PitchGLoadManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PitchGLoadManeuver::PitchGLoadManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_PitchGLoadManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetG_Load>());
}

UT_DEFINE_SCRIPT_METHOD(PitchGLoadManeuver, wsf::six_dof::PitchGLoadManeuver, Construct, 1, "WsfSixDOF_PitchGLoadManeuver", "double")
{
   wsf::six_dof::PitchGLoadManeuver* mvrPtr = new wsf::six_dof::PitchGLoadManeuver{};
   mvrPtr->SetG_Load(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(PitchGLoadManeuver, wsf::six_dof::PitchGLoadManeuver, GetG_Load, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetG_Load());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
