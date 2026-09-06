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

#include "WsfScriptSixDOF_PitchRateManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_PitchRateManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PitchRateManeuver::PitchRateManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_PitchRateManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetPitchRate>());
}

UT_DEFINE_SCRIPT_METHOD(PitchRateManeuver, wsf::six_dof::PitchRateManeuver, Construct, 1, "WsfSixDOF_PitchRateManeuver", "double")
{
   wsf::six_dof::PitchRateManeuver* mvrPtr = new wsf::six_dof::PitchRateManeuver{};
   mvrPtr->SetRate(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(PitchRateManeuver, wsf::six_dof::PitchRateManeuver, GetPitchRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRate() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
