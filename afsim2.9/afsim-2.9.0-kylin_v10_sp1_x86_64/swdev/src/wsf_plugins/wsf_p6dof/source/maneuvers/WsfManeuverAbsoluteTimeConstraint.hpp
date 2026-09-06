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

#ifndef WSFMANEUVERABSOLUTETIMECONSTRAINT_HPP
#define WSFMANEUVERABSOLUTETIMECONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WsfManeuverAbsoluteTimeConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverAbsoluteTimeConstraint()
      : mConstraintTime{-1.0}
   {
   }
   WsfManeuverAbsoluteTimeConstraint(const WsfManeuverAbsoluteTimeConstraint& aOther) = default;
   ~WsfManeuverAbsoluteTimeConstraint() override                                      = default;

   WsfManeuverAbsoluteTimeConstraint& operator=(const WsfManeuverAbsoluteTimeConstraint& aOther) = delete;

   WsfManeuverAbsoluteTimeConstraint* Clone() const override { return new WsfManeuverAbsoluteTimeConstraint{*this}; }

   void   Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   //! Get the absolute time at which this constraint is satisfied.
   double GetConstraintTime() const { return mConstraintTime; }

   //! Set the absolute time at which this constraint is satisfied.
   void SetConstraintTime(double aTime) { mConstraintTime = aTime; }

   const char* Type() const override { return "ABSOLUTE-TIME"; }

private:
   double mConstraintTime;
};

#endif // WSFMANEUVERABSOLUTETIMECONSTRAINT_HPP
