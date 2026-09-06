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

#ifndef WSF_SIXDOF_SCRIPTMANEUVER_HPP
#define WSF_SIXDOF_SCRIPTMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT Maneuver : public UtScriptClass
{
public:
   Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Maneuver() override = default;

   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(GetDebug);
   UT_DECLARE_SCRIPT_METHOD(SetDebug);

   UT_DECLARE_SCRIPT_METHOD(GetMover);

   UT_DECLARE_SCRIPT_METHOD(IsAssigned);
   UT_DECLARE_SCRIPT_METHOD(IsPending);
   UT_DECLARE_SCRIPT_METHOD(IsExecuting);
   UT_DECLARE_SCRIPT_METHOD(IsCompleting);
   UT_DECLARE_SCRIPT_METHOD(IsCompleted);
   UT_DECLARE_SCRIPT_METHOD(IsCanceled);

   UT_DECLARE_SCRIPT_METHOD(IsSequence);

   UT_DECLARE_SCRIPT_METHOD(Cancel);

   UT_DECLARE_SCRIPT_METHOD(GetUrgency);
   UT_DECLARE_SCRIPT_METHOD(SetUrgency);

   UT_DECLARE_SCRIPT_METHOD(GetEntryConstraint);
   UT_DECLARE_SCRIPT_METHOD(SetEntryConstraint);
   UT_DECLARE_SCRIPT_METHOD(GetExitConstraint);
   UT_DECLARE_SCRIPT_METHOD(SetExitConstraint);

   UT_DECLARE_SCRIPT_METHOD(GetManeuverType);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTMANEUVER_HPP
