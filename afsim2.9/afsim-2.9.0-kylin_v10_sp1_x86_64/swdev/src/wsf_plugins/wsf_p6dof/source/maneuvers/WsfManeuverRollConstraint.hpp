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

#ifndef WSFMANEUVERROLLCONSTRAINT_HPP
#define WSFMANEUVERROLLCONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WSF_P6DOF_EXPORT WsfManeuverRollConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverRollConstraint()
      : mMoverPtr{nullptr}
      , mRollTarget{0.0}
   {
   }
   WsfManeuverRollConstraint(const WsfManeuverRollConstraint& aOther);
   ~WsfManeuverRollConstraint() override = default;

   WsfManeuverRollConstraint& operator=(const WsfManeuverRollConstraint& aOther) = delete;

   WsfManeuverRollConstraint* Clone() const override { return new WsfManeuverRollConstraint{*this}; }

   void   Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "ROLL"; }

   //! Get the roll target for this constraint.
   double GetRollTarget() const { return mRollTarget; }

   //! Set the roll target for this constraint.
   void SetRollTarget(double aRollTarget) { mRollTarget = aRollTarget; }

private:
   WsfP6DOF_Mover* mMoverPtr;
   double          mRollTarget;

   static const double cROLL_TOLERANCE;
   static const double cDELTA_T_MIN;
};

#endif // WSFMANEUVERROLLCONSTRAINT_HPP
