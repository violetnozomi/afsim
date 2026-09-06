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

#ifndef WSFSCRIPTFORMATIONDETACHCOMMAND_HPP
#define WSFSCRIPTFORMATIONDETACHCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationDetachCommand : public WsfScriptFormationCommand
{
public:
   WsfScriptFormationDetachCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationDetachCommand() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
};

#endif // WSFSCRIPTFORMATIONDETACHCOMMAND_HPP
