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

#ifndef WSFSCRIPTFORMATIONCOMMANDSEQUENCE_HPP
#define WSFSCRIPTFORMATIONCOMMANDSEQUENCE_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationCommandSequence : public WsfScriptFormationCommand
{
public:
   WsfScriptFormationCommandSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationCommandSequence() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);

   UT_DECLARE_SCRIPT_METHOD(AppendCommand_1);
   UT_DECLARE_SCRIPT_METHOD(AppendCommand_2);
   UT_DECLARE_SCRIPT_METHOD(GetNumCommands);
   UT_DECLARE_SCRIPT_METHOD(GetCurrentCommand);
};

#endif // WSFSCRIPTFORMATIONCOMMANDSEQUENCE_HPP
