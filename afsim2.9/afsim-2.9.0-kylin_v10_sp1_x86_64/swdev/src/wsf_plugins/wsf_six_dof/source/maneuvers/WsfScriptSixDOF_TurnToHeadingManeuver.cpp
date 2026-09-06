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

#include "WsfScriptSixDOF_TurnToHeadingManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_TurnToHeadingManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
TurnToHeadingManeuver::TurnToHeadingManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_TurnToHeadingManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetHeading>());
}

UT_DEFINE_SCRIPT_METHOD(TurnToHeadingManeuver,
                        wsf::six_dof::TurnToHeadingManeuver,
                        Construct,
                        1,
                        "WsfSixDOF_TurnToHeadingManeuver",
                        "double")
{
   wsf::six_dof::TurnToHeadingManeuver* mvrPtr =
      new wsf::six_dof::TurnToHeadingManeuver(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(TurnToHeadingManeuver, wsf::six_dof::TurnToHeadingManeuver, GetHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeading() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
