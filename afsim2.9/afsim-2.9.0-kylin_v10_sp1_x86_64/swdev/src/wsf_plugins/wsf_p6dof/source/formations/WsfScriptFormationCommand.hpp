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

#ifndef WSFSCRIPTFORMATIONCOMMAND_HPP
#define WSFSCRIPTFORMATIONCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationCommandConstraint : public UtScriptClass
{
public:
   WsfScriptFormationCommandConstraint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationCommandConstraint() override = default;

   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(AT_TIME);
   UT_DECLARE_SCRIPT_METHOD(AT_RELATIVE_TIME);

   UT_DECLARE_SCRIPT_METHOD(GetConstraintTime);
   UT_DECLARE_SCRIPT_METHOD(SetConstraintTime);
};

class WSF_P6DOF_EXPORT WsfScriptFormationCommand : public UtScriptClass
{
public:
   WsfScriptFormationCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationCommand() override = default;

   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(IsInstantaneous);
   UT_DECLARE_SCRIPT_METHOD(IsSequence);

   UT_DECLARE_SCRIPT_METHOD(IsAssigned);
   UT_DECLARE_SCRIPT_METHOD(IsPending);
   UT_DECLARE_SCRIPT_METHOD(IsExecuting);
   UT_DECLARE_SCRIPT_METHOD(IsCompleted);
   UT_DECLARE_SCRIPT_METHOD(IsCanceling);
   UT_DECLARE_SCRIPT_METHOD(IsCanceled);
   UT_DECLARE_SCRIPT_METHOD(IsDropped);
   UT_DECLARE_SCRIPT_METHOD(IsRunning);

   UT_DECLARE_SCRIPT_METHOD(GetDebug);
   UT_DECLARE_SCRIPT_METHOD(SetDebug);

   UT_DECLARE_SCRIPT_METHOD(GetFormation);

   UT_DECLARE_SCRIPT_METHOD(Cancel);

   UT_DECLARE_SCRIPT_METHOD(GetConstraint);
   UT_DECLARE_SCRIPT_METHOD(SetConstraint);
};

#endif // WSFSCRIPTFORMATIONCOMMAND_HPP
