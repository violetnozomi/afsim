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

#include "WsfScriptSixDOF_ManeuverSequence.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_Maneuver.hpp"
#include "WsfSixDOF_ManeuverSequence.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
ManeuverSequence::ManeuverSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_ManeuverSequence");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<IsLoop>());
   AddMethod(ut::make_unique<SetLoop>());
   AddMethod(ut::make_unique<Clear>());
   AddMethod(ut::make_unique<Append>());
   AddMethod(ut::make_unique<InsertNext>());
   AddMethod(ut::make_unique<GetCurrentManeuver>());
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, Construct, 0, "WsfSixDOF_ManeuverSequence", "")
{
   aReturnVal.SetPointer(new UtScriptRef(new wsf::six_dof::ManeuverSequence{}, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, IsLoop, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsLoop());
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, SetLoop, 1, "void", "bool")
{
   aObjectPtr->SetLoop(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, Clear, 0, "void", "")
{
   aObjectPtr->Clear();
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, Append, 1, "void", "WsfSixDOF_Maneuver")
{
   auto mvrPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::Maneuver>();
   auto argPtr = ut::clone(mvrPtr);
   aObjectPtr->Append(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, InsertNext, 1, "void", "WsfSixDOF_Maneuver")
{
   auto mvrPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::Maneuver>();
   auto argPtr = ut::clone(mvrPtr);
   aObjectPtr->InsertNext(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverSequence, wsf::six_dof::ManeuverSequence, GetCurrentManeuver, 0, "WsfSixDOF_Maneuver", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetCurrentManeuver(), aReturnClassPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
