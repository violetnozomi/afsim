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

#ifndef WSFMANEUVERSCRIPTCONSTRAINT_HPP
#define WSFMANEUVERSCRIPTCONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include <string>

#include "WsfManeuverConstraint.hpp"

class UtScript;
class WsfSimulation;

class WsfManeuverScriptConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverScriptConstraint();
   WsfManeuverScriptConstraint(const WsfManeuverScriptConstraint& aOther) = default;
   ~WsfManeuverScriptConstraint() override                                = default;

   WsfManeuverScriptConstraint& operator=(const WsfManeuverScriptConstraint& aOther) = delete;

   WsfManeuverScriptConstraint* Clone() const override { return new WsfManeuverScriptConstraint{*this}; }

   void   Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "SCRIPT"; }

   void SetSatisfiedScript(const std::string& aScriptName, const WsfSimulation& aSimulation);
   void SetNextEvaluationTimeScript(const std::string& aScriptName, const WsfSimulation& aSimulation);

private:
   double          mAssignTime;
   WsfP6DOF_Mover* mMoverPtr;
   UtScript*       mSatisfiedPtr;
   UtScript*       mNextEvaluationTimePtr;
};

#endif // WSFMANEUVERSCRIPTCONSTRAINT_HPP
