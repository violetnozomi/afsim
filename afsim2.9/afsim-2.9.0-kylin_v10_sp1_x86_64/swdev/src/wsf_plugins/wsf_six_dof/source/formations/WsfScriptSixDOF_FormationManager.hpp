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

#ifndef WSF_SIXDOF_SCRIPTFORMATIONMANAGER_HPP
#define WSF_SIXDOF_SCRIPTFORMATIONMANAGER_HPP

#include "wsf_six_dof_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT FormationManager : public UtScriptClass
{
public:
   FormationManager(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~FormationManager() override = default;

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

} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTFORMATIONMANAGER_HPP
