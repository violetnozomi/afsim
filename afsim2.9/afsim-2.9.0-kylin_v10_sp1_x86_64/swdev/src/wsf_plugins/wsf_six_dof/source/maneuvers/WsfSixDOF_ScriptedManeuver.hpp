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

#ifndef WSF_SIXDOF_SCRIPTEDMANEUVER_HPP
#define WSF_SIXDOF_SCRIPTEDMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include <string>

#include "WsfSixDOF_Maneuver.hpp"

class UtScript;
class WsfSimulation;

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT ScriptedManeuver : public Maneuver
{
public:
   ScriptedManeuver();
   ScriptedManeuver(const ScriptedManeuver& aOther);
   ~ScriptedManeuver() override = default;

   ScriptedManeuver& operator=(const ScriptedManeuver& aOther) = delete;

   ScriptedManeuver* Clone() const override { return new ScriptedManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_ScriptedManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SCRIPTED"; }

   void SetExecuteScript(const std::string& aScriptName, const WsfSimulation& aSimulation);

private:
   UtScript* mExecutePtr;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTEDMANEUVER_HPP
