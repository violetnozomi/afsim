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

#include "WsfScriptSixDOF_FormationCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationCommandConstraint::FormationCommandConstraint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationCommandConstraint");

   AddStaticMethod(ut::make_unique<AT_TIME>());
   AddStaticMethod(ut::make_unique<AT_RELATIVE_TIME>());

   AddMethod(ut::make_unique<GetConstraintTime>());
   AddMethod(ut::make_unique<SetConstraintTime>());
}

void* FormationCommandConstraint::Create(const UtScriptContext& aContext)
{
   return new wsf::six_dof::FormationCommand::Constraint{};
}

void FormationCommandConstraint::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::six_dof::FormationCommand::Constraint*>(aObjectPtr);
}

void* FormationCommandConstraint::Clone(void* aObjectPtr)
{
   return new wsf::six_dof::FormationCommand::Constraint(
      *static_cast<wsf::six_dof::FormationCommand::Constraint*>(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandConstraint,
                        wsf::six_dof::FormationCommand::Constraint,
                        AT_TIME,
                        1,
                        "WsfSixDOF_FormationCommandConstraint",
                        "double")
{
   wsf::six_dof::FormationCommand::Constraint* consPtr = new wsf::six_dof::FormationCommand::Constraint{};
   consPtr->SetAbsoluteTimeConstraint(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandConstraint,
                        wsf::six_dof::FormationCommand::Constraint,
                        AT_RELATIVE_TIME,
                        1,
                        "WsfSixDOF_FormationCommandConstraint",
                        "double")
{
   wsf::six_dof::FormationCommand::Constraint* consPtr = new wsf::six_dof::FormationCommand::Constraint{};
   consPtr->SetRelativeTimeConstraint(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandConstraint, wsf::six_dof::FormationCommand::Constraint, GetConstraintTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAbsoluteTimeConstraint());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandConstraint, wsf::six_dof::FormationCommand::Constraint, SetConstraintTime, 1, "void", "double")
{
   aObjectPtr->SetAbsoluteTimeConstraint(aVarArgs[0].GetDouble());
}

FormationCommand::FormationCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfSixDOF_FormationCommand");

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

void* FormationCommand::Clone(void* aObjectPtr)
{
   return static_cast<wsf::six_dof::FormationCommand*>(aObjectPtr)->Clone();
}

void FormationCommand::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::six_dof::FormationCommand*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsInstantaneous, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsInstantaneous());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsSequence, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsSequence());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsAssigned, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsAssigned());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsPending, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPending());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsExecuting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExecuting());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsCompleted, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCompleted());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsCanceling, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCanceling());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsCanceled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCanceled());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsDropped, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsDropped());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, IsRunning, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsRunning());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, GetDebug, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetDebug());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, SetDebug, 1, "void", "bool")
{
   aObjectPtr->SetDebug(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, GetFormation, 0, "WsfSixDOF_Formation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetFormation(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, Cancel, 0, "void", "")
{
   aObjectPtr->Cancel();
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand,
                        wsf::six_dof::FormationCommand,
                        GetConstraint,
                        0,
                        "WsfSixDOF_FormationCommandConstraint",
                        "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetConstraint(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommand, wsf::six_dof::FormationCommand, SetConstraint, 1, "void", "WsfSixDOF_FormationCommandConstraint")
{
   auto appObjectPtr  = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand::Constraint>();
   auto constraintPtr = ut::clone(appObjectPtr);
   aObjectPtr->SetConstraint(std::move(constraintPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
