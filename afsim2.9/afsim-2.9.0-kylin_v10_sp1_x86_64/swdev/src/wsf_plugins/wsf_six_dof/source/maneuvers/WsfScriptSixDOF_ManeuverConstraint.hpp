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

#ifndef WSF_SIXDOF_SCRIPTMANEUVERCONSTRAINT_HPP
#define WSF_SIXDOF_SCRIPTMANEUVERCONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT ManeuverConstraint : public UtScriptClass
{
public:
   ManeuverConstraint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ManeuverConstraint() override = default;

   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(AT_TIME);
   UT_DECLARE_SCRIPT_METHOD(AT_RELATIVE_TIME);
   UT_DECLARE_SCRIPT_METHOD(AT_ROLL_ANGLE);
   UT_DECLARE_SCRIPT_METHOD(AT_PITCH_ANGLE);
   UT_DECLARE_SCRIPT_METHOD(AT_HEADING);
   UT_DECLARE_SCRIPT_METHOD(AT_ALTITUDE);
   UT_DECLARE_SCRIPT_METHOD(AT_FLIGHT_PATH_ANGLE);
   UT_DECLARE_SCRIPT_METHOD(SCRIPT);

   UT_DECLARE_SCRIPT_METHOD(Type);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTMANEUVERCONSTRAINT_HPP
