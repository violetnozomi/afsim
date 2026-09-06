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

#ifndef WSFSCRIPTFORMATIONADDSUBCOMMAND_HPP
#define WSFSCRIPTFORMATIONADDSUBCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationAddSubCommand : public WsfScriptFormationCommand
{
public:
   WsfScriptFormationAddSubCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationAddSubCommand() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);

   UT_DECLARE_SCRIPT_METHOD(GetParentRelativeName);
   UT_DECLARE_SCRIPT_METHOD(GetFormationToAdd);
   UT_DECLARE_SCRIPT_METHOD(GetOffset);
};

#endif // WSFSCRIPTFORMATIONADDSUBCOMMAND_HPP
