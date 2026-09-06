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

#ifndef WSF_SIXDOF_MANEUVERRELATIVETIMECONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERRELATIVETIMECONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
class ManeuverRelativeTimeConstraint : public ManeuverConstraint
{
public:
   ManeuverRelativeTimeConstraint()                                             = default;
   ManeuverRelativeTimeConstraint(const ManeuverRelativeTimeConstraint& aOther) = default;
   ~ManeuverRelativeTimeConstraint() override                                   = default;

   ManeuverRelativeTimeConstraint& operator=(const ManeuverRelativeTimeConstraint& aOther) = delete;

   ManeuverRelativeTimeConstraint* Clone() const override { return new ManeuverRelativeTimeConstraint{*this}; }

   void   Assign(double aSimTime, Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   //! Get the absolute time at which this constraint is satisfied.
   double GetConstraintTime() const { return mConstraintTime + mAssignTime; }

   //! Set the relative time at which this constraint is satisfied.
   void SetConstraintTime(double aTime) { mConstraintTime = aTime; }

   const char* Type() const override { return "RELATIVE-TIME"; }

private:
   double mConstraintTime = 0.0;
   double mAssignTime     = 0.0;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERRELATIVETIMECONSTRAINT_HPP
