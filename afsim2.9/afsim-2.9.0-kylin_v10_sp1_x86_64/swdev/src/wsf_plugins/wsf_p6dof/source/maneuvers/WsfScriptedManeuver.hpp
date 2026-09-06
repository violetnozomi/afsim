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

#ifndef WSFSCRIPTEDMANEUVER_HPP
#define WSFSCRIPTEDMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include <string>

#include "WsfManeuver.hpp"

class UtScript;
class WsfSimulation;

class WSF_P6DOF_EXPORT WsfScriptedManeuver : public WsfManeuver
{
public:
   WsfScriptedManeuver();
   WsfScriptedManeuver(const WsfScriptedManeuver& aOther);
   ~WsfScriptedManeuver() override = default;

   WsfScriptedManeuver& operator=(const WsfScriptedManeuver& aOther) = delete;

   WsfScriptedManeuver* Clone() const override { return new WsfScriptedManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfScriptedManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SCRIPTED"; }

   void SetExecuteScript(const std::string& aScriptName, const WsfSimulation& aSimulation);

private:
   UtScript* mExecutePtr;
};

#endif // WSFSCRIPTEDMANEUVER_HPP
