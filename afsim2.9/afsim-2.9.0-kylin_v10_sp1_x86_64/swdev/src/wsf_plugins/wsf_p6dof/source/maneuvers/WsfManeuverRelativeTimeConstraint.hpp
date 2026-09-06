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

#ifndef WSFMANEUVERRELATIVETIMECONSTRAINT_HPP
#define WSFMANEUVERRELATIVETIMECONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WsfManeuverRelativeTimeConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverRelativeTimeConstraint()
      : mConstraintTime{0.0}
      , mAssignTime{0.0}
   {
   }
   WsfManeuverRelativeTimeConstraint(const WsfManeuverRelativeTimeConstraint& aOther) = default;
   ~WsfManeuverRelativeTimeConstraint() override                                      = default;

   WsfManeuverRelativeTimeConstraint& operator=(const WsfManeuverRelativeTimeConstraint& aOther) = delete;

   WsfManeuverRelativeTimeConstraint* Clone() const override { return new WsfManeuverRelativeTimeConstraint{*this}; }

   void   Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   //! Get the absolute time at which this constraint is satisfied.
   double GetConstraintTime() const { return mConstraintTime + mAssignTime; }

   //! Set the relative time at which this constraint is satisfied.
   void SetConstraintTime(double aTime) { mConstraintTime = aTime; }

   const char* Type() const override { return "RELATIVE-TIME"; }

private:
   double mConstraintTime;
   double mAssignTime;
};

#endif // WSFMANEUVERRELATIVETIMECONSTRAINT_HPP
