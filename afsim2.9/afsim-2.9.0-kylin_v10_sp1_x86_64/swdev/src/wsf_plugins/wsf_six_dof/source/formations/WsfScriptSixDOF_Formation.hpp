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

#ifndef WSF_SIXDOF_SCRIPTFORMATION_HPP
#define WSF_SIXDOF_SCRIPTFORMATION_HPP

#include "wsf_six_dof_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT Formation : public UtScriptClass
{
public:
   Formation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Formation() override = default;

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
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTFORMATION_HPP
