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

#ifndef WSF_SIXDOF_MANEUVERSCRIPTCONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERSCRIPTCONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include <string>

#include "WsfSixDOF_ManeuverConstraint.hpp"

class UtScript;
class WsfSimulation;

namespace wsf
{
namespace six_dof
{
class ManeuverScriptConstraint : public ManeuverConstraint
{
public:
   ManeuverScriptConstraint()                                       = default;
   ManeuverScriptConstraint(const ManeuverScriptConstraint& aOther) = default;
   ~ManeuverScriptConstraint() override                             = default;

   ManeuverScriptConstraint& operator=(const ManeuverScriptConstraint& aOther) = delete;

   ManeuverScriptConstraint* Clone() const override { return new ManeuverScriptConstraint{*this}; }

   void   Assign(double aSimTime, Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "SCRIPT"; }

   void SetSatisfiedScript(const std::string& aScriptName, const WsfSimulation& aSimulation);
   void SetNextEvaluationTimeScript(const std::string& aScriptName, const WsfSimulation& aSimulation);

private:
   double    mAssignTime = -1.0;
   Mover*    mMoverPtr{nullptr};
   UtScript* mSatisfiedPtr{nullptr};
   UtScript* mNextEvaluationTimePtr{nullptr};
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERSCRIPTCONSTRAINT_HPP
