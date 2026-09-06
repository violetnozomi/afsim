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

#include "WsfScriptFormationCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormation.hpp"
#include "WsfFormationCommand.hpp"


WsfScriptFormationCommandConstraint::WsfScriptFormationCommandConstraint(const std::string& aClassName,
                                                                         UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationCommandConstraint");

   AddStaticMethod(ut::make_unique<AT_TIME>());
   AddStaticMethod(ut::make_unique<AT_RELATIVE_TIME>());

   AddMethod(ut::make_unique<GetConstraintTime>());
   AddMethod(ut::make_unique<SetConstraintTime>());
}

void* WsfScriptFormationCommandConstraint::Create(const UtScriptContext& aContext)
{
   return new WsfFormationCommand::Constraint{};
}

void WsfScriptFormationCommandConstraint::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfFormationCommand::Constraint*>(aObjectPtr);
}

void* WsfScriptFormationCommandConstraint::Clone(void* aObjectPtr)
{
   return new WsfFormationCommand::Constraint(*static_cast<WsfFormationCommand::Constraint*>(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandConstraint,
                        WsfFormationCommand::Constraint,
                        AT_TIME,
                        1,
                        "WsfFormationCommandConstraint",
                        "double")
{
   WsfFormationCommand::Constraint* consPtr = new WsfFormationCommand::Constraint{};
   consPtr->SetAbsoluteTimeConstraint(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandConstraint,
                        WsfFormationCommand::Constraint,
                        AT_RELATIVE_TIME,
                        1,
                        "WsfFormationCommandConstraint",
                        "double")
{
   WsfFormationCommand::Constraint* consPtr = new WsfFormationCommand::Constraint{};
   consPtr->SetRelativeTimeConstraint(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandConstraint, WsfFormationCommand::Constraint, GetConstraintTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAbsoluteTimeConstraint());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandConstraint, WsfFormationCommand::Constraint, SetConstraintTime, 1, "void", "double")
{
   aObjectPtr->SetAbsoluteTimeConstraint(aVarArgs[0].GetDouble());
}

WsfScriptFormationCommand::WsfScriptFormationCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfFormationCommand");

   AddMethod(ut::make_unique<IsInstantaneous>());
   AddMethod(ut::make_unique<IsSequence>());
   AddMethod(ut::make_unique<IsAssigned>());
   AddMethod(ut::make_unique<IsPending>());
   AddMethod(ut::make_unique<IsExecuting>());
   AddMethod(ut::make_unique<IsCompleted>());
   AddMethod(ut::make_unique<IsCanceling>());
   AddMethod(ut::make_unique<IsCanceled>());
   AddMethod(ut::make_unique<IsDropped>());
   AddMethod(ut::make_unique<IsRunning>());
   AddMethod(ut::make_unique<GetDebug>());
   AddMethod(ut::make_unique<SetDebug>());
   AddMethod(ut::make_unique<GetFormation>());
   AddMethod(ut::make_unique<Cancel>());
   AddMethod(ut::make_unique<GetConstraint>());
   AddMethod(ut::make_unique<SetConstraint>());
}

void* WsfScriptFormationCommand::Clone(void* aObjectPtr)
{
   return static_cast<WsfFormationCommand*>(aObjectPtr)->Clone();
}

void WsfScriptFormationCommand::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfFormationCommand*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsInstantaneous, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsInstantaneous());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsSequence, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsSequence());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsAssigned, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsAssigned());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsPending, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPending());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsExecuting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExecuting());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsCompleted, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCompleted());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsCanceling, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCanceling());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsCanceled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCanceled());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsDropped, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsDropped());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, IsRunning, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsRunning());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, GetDebug, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetDebug());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, SetDebug, 1, "void", "bool")
{
   aObjectPtr->SetDebug(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, GetFormation, 0, "WsfFormation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetFormation(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, Cancel, 0, "void", "")
{
   aObjectPtr->Cancel();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, GetConstraint, 0, "WsfFormationCommandConstraint", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetConstraint(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommand, WsfFormationCommand, SetConstraint, 1, "void", "WsfFormationCommandConstraint")
{
   auto appObjectPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfFormationCommand::Constraint>();
   auto constraintPtr = ut::clone(appObjectPtr);
   aObjectPtr->SetConstraint(std::move(constraintPtr));
}
