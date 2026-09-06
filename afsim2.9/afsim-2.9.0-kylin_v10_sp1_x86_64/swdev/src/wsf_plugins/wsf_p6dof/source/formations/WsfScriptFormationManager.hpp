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

#ifndef WSFSCRIPTFORMATIONMANAGER_HPP
#define WSFSCRIPTFORMATIONMANAGER_HPP

#include "wsf_p6dof_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationManager : public UtScriptClass
{
public:
   WsfScriptFormationManager(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationManager() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetFormation);
   UT_DECLARE_SCRIPT_METHOD(HasFormation);
   UT_DECLARE_SCRIPT_METHOD(GetTopLevelFormations);
   UT_DECLARE_SCRIPT_METHOD(GetAllFormations);
   UT_DECLARE_SCRIPT_METHOD(GetFormationFromMember);

   UT_DECLARE_SCRIPT_METHOD(DisbandFormation);

   UT_DECLARE_SCRIPT_METHOD(CreateUnit);
   UT_DECLARE_SCRIPT_METHOD(CreateSection);
   UT_DECLARE_SCRIPT_METHOD(CreateFormation_1);
   UT_DECLARE_SCRIPT_METHOD(CreateFormation_2);
};

#endif // WSFSCRIPTFORMATIONMANAGER_HPP
