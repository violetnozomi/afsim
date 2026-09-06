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

#include "WsfScriptSixDOF_Maneuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_Maneuver.hpp"
#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
Maneuver::Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_Maneuver");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<GetDebug>());
   AddMethod(ut::make_unique<SetDebug>());
   AddMethod(ut::make_unique<GetMover>());
   AddMethod(ut::make_unique<IsAssigned>());
   AddMethod(ut::make_unique<IsPending>());
   AddMethod(ut::make_unique<IsExecuting>());
   AddMethod(ut::make_unique<IsCompleting>());
   AddMethod(ut::make_unique<IsCompleted>());
   AddMethod(ut::make_unique<IsCanceled>());
   AddMethod(ut::make_unique<IsSequence>());
   AddMethod(ut::make_unique<Cancel>());
   AddMethod(ut::make_unique<GetUrgency>());
   AddMethod(ut::make_unique<SetUrgency>());
   AddMethod(ut::make_unique<GetEntryConstraint>());
   AddMethod(ut::make_unique<SetEntryConstraint>());
   AddMethod(ut::make_unique<GetExitConstraint>());
   AddMethod(ut::make_unique<SetExitConstraint>());
   AddMethod(ut::make_unique<GetManeuverType>());
}

void* Maneuver::Clone(void* aObjectPtr)
{
   return static_cast<wsf::six_dof::Maneuver*>(aObjectPtr)->Clone();
}

void Maneuver::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::six_dof::Maneuver*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, GetDebug, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetDebug());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, SetDebug, 1, "void", "bool")
{
   aObjectPtr->SetDebug(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, GetMover, 0, "WsfSixDOF_Mover", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetMover(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsAssigned, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsAssigned());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsPending, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPending());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsExecuting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExecuting());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsCompleting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCompleting());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsCompleted, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCompleted());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsCanceled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCanceled());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, IsSequence, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsSequence());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, Cancel, 0, "void", "")
{
   aObjectPtr->Cancel();
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, GetUrgency, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUrgency());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, SetUrgency, 1, "void", "double")
{
   aObjectPtr->SetUrgency(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, GetEntryConstraint, 0, "WsfSixDOF_ManeuverConstraint", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetEntryConstraint(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, SetEntryConstraint, 1, "void", "WsfSixDOF_ManeuverConstraint")
{
   auto input  = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::ManeuverConstraint>();
   auto argPtr = ut::clone(input);
   aObjectPtr->SetEntryConstraint(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, GetExitConstraint, 0, "WsfSixDOF_ManeuverConstraint", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetExitConstraint(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, SetExitConstraint, 1, "void", "WsfSixDOF_ManeuverConstraint")
{
   auto input  = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::ManeuverConstraint>();
   auto argPtr = ut::clone(input);
   aObjectPtr->SetExitConstraint(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(Maneuver, wsf::six_dof::Maneuver, GetManeuverType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->Type());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
