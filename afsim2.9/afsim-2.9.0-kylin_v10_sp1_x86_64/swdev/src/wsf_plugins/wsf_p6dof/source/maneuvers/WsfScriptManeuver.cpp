// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfManeuver.hpp"
#include "WsfManeuverConstraint.hpp"

WsfScriptManeuver::WsfScriptManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfManeuver");

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

void* WsfScriptManeuver::Clone(void* aObjectPtr)
{
   return static_cast<WsfManeuver*>(aObjectPtr)->Clone();
}

void WsfScriptManeuver::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfManeuver*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, GetDebug, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetDebug());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, SetDebug, 1, "void", "bool")
{
   aObjectPtr->SetDebug(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, GetMover, 0, "WsfP6DOF_Mover", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetMover(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsAssigned, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsAssigned());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsPending, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPending());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsExecuting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExecuting());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsCompleting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCompleting());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsCompleted, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCompleted());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsCanceled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCanceled());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, IsSequence, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsSequence());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, Cancel, 0, "void", "")
{
   aObjectPtr->Cancel();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, GetUrgency, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUrgency());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, SetUrgency, 1, "void", "double")
{
   aObjectPtr->SetUrgency(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, GetEntryConstraint, 0, "WsfManeuverConstraint", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetEntryConstraint(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, SetEntryConstraint, 1, "void", "WsfManeuverConstraint")
{
   auto input  = aVarArgs[0].GetPointer()->GetAppObject<WsfManeuverConstraint>();
   auto argPtr = ut::clone(input);
   aObjectPtr->SetEntryConstraint(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, GetExitConstraint, 0, "WsfManeuverConstraint", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetExitConstraint(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, SetExitConstraint, 1, "void", "WsfManeuverConstraint")
{
   auto input  = aVarArgs[0].GetPointer()->GetAppObject<WsfManeuverConstraint>();
   auto argPtr = ut::clone(input);
   aObjectPtr->SetExitConstraint(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuver, WsfManeuver, GetManeuverType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->Type());
}
