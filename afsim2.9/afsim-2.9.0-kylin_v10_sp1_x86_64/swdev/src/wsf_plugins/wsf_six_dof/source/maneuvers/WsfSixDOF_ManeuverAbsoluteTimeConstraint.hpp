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

#ifndef WSF_SIXDOF_MANEUVERABSOLUTETIMECONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERABSOLUTETIMECONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
class ManeuverAbsoluteTimeConstraint : public ManeuverConstraint
{
public:
   ManeuverAbsoluteTimeConstraint()                                             = default;
   ManeuverAbsoluteTimeConstraint(const ManeuverAbsoluteTimeConstraint& aOther) = default;
   ~ManeuverAbsoluteTimeConstraint() override                                   = default;

   ManeuverAbsoluteTimeConstraint& operator=(const ManeuverAbsoluteTimeConstraint& aOther) = delete;

   ManeuverAbsoluteTimeConstraint* Clone() const override { return new ManeuverAbsoluteTimeConstraint{*this}; }

   void   Assign(double aSimTime, Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   //! Get the absolute time at which this constraint is satisfied.
   double GetConstraintTime() const { return mConstraintTime; }

   //! Set the absolute time at which this constraint is satisfied.
   void SetConstraintTime(double aTime) { mConstraintTime = aTime; }

   const char* Type() const override { return "ABSOLUTE-TIME"; }

private:
   double mConstraintTime = -1.0;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERABSOLUTETIMECONSTRAINT_HPP
