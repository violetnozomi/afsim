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

#ifndef WSFMANEUVERHEADINGCONSTRAINT_HPP
#define WSFMANEUVERHEADINGCONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WSF_P6DOF_EXPORT WsfManeuverHeadingConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverHeadingConstraint()
      : mMoverPtr{nullptr}
      , mHeadingTarget{0.0}
   {
   }
   WsfManeuverHeadingConstraint(const WsfManeuverHeadingConstraint& aOther);
   ~WsfManeuverHeadingConstraint() override = default;

   WsfManeuverHeadingConstraint& operator=(const WsfManeuverHeadingConstraint& aOther) = delete;

   WsfManeuverHeadingConstraint* Clone() const override { return new WsfManeuverHeadingConstraint{*this}; }
   void                          Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool                          Satisfied(double aSimTime) const override;
   double                        NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "HEADING"; }

   //! Get the heading target for this constraint.
   double GetHeadingTarget() const { return mHeadingTarget; }

   //! Set the heading target for this constraint.
   void SetHeadingTarget(double aRollTarget) { mHeadingTarget = aRollTarget; }

private:
   WsfP6DOF_Mover* mMoverPtr;
   double          mHeadingTarget;

   static const double cHEADING_TOLERANCE;
   static const double cYAW_RATE_MINIMUM;
};

#endif // WSFMANEUVERHEADINGCONSTRAINT_HPP
