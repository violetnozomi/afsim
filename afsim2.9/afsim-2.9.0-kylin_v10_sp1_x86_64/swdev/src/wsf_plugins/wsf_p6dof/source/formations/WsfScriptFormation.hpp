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

#ifndef WSFSCRIPTFORMATION_HPP
#define WSFSCRIPTFORMATION_HPP

#include "wsf_p6dof_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormation : public UtScriptClass
{
public:
   WsfScriptFormation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormation() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetQualifiedName);
   UT_DECLARE_SCRIPT_METHOD(GetRelativeName);

   UT_DECLARE_SCRIPT_METHOD(GetParentFormation);
   UT_DECLARE_SCRIPT_METHOD(IsRoot);
   UT_DECLARE_SCRIPT_METHOD(IsLeaf);
   UT_DECLARE_SCRIPT_METHOD(GetNumSubFormations);
   UT_DECLARE_SCRIPT_METHOD(GetSubFormation);
   UT_DECLARE_SCRIPT_METHOD(GetAllSubFormations);

   UT_DECLARE_SCRIPT_METHOD(IsAttached);

   UT_DECLARE_SCRIPT_METHOD(GetLeadSubFormation);
   UT_DECLARE_SCRIPT_METHOD(GetFormationLeader);
   UT_DECLARE_SCRIPT_METHOD(HasLeader);
   UT_DECLARE_SCRIPT_METHOD(IsLeader);

   UT_DECLARE_SCRIPT_METHOD(GetNumMemberPlatforms);
   UT_DECLARE_SCRIPT_METHOD(GetMemberPlatform);
   UT_DECLARE_SCRIPT_METHOD(HasMemberPlatform);

   UT_DECLARE_SCRIPT_METHOD(GetOffset);

   UT_DECLARE_SCRIPT_METHOD(AddSubFormation);
   UT_DECLARE_SCRIPT_METHOD(RemoveSubFormation);
   UT_DECLARE_SCRIPT_METHOD(SetLeader);
   UT_DECLARE_SCRIPT_METHOD(SetMember_1);
   UT_DECLARE_SCRIPT_METHOD(SetMember_2);
   UT_DECLARE_SCRIPT_METHOD(Rename);

   UT_DECLARE_SCRIPT_METHOD(ExecuteCommand_1);
   UT_DECLARE_SCRIPT_METHOD(ExecuteCommand_2);
   UT_DECLARE_SCRIPT_METHOD(GetCurrentCommand);
};

#endif // WSFSCRIPTFORMATION_HPP
