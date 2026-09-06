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

#include "WsfScriptSixDOF_FlightPathAngleManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_FlightPathAngleManeuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FlightPathAngleManeuver::FlightPathAngleManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FlightPathAngleManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<GetFlightPathAngle>());
}

UT_DEFINE_SCRIPT_METHOD(FlightPathAngleManeuver,
                        wsf::six_dof::FlightPathAngleManeuver,
                        Construct,
                        1,
                        "WsfSixDOF_FlightPathAngleManeuver",
                        "double")
{
   wsf::six_dof::FlightPathAngleManeuver* mvrPtr = new wsf::six_dof::FlightPathAngleManeuver{};
   mvrPtr->SetFlightPathAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FlightPathAngleManeuver, wsf::six_dof::FlightPathAngleManeuver, GetFlightPathAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFlightPathAngle() * UtMath::cDEG_PER_RAD);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
