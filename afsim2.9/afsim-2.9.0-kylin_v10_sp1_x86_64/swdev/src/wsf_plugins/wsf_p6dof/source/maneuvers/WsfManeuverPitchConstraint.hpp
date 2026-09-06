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

#ifndef WSFMANEUVERPITCHCONSTRAINT_HPP
#define WSFMANEUVERPITCHCONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WSF_P6DOF_EXPORT WsfManeuverPitchConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverPitchConstraint()
      : mMoverPtr{nullptr}
      , mPitchTarget{0.0}
   {
   }
   WsfManeuverPitchConstraint(const WsfManeuverPitchConstraint& aOther);
   ~WsfManeuverPitchConstraint() override = default;

   WsfManeuverPitchConstraint& operator=(const WsfManeuverPitchConstraint& aOther) = delete;

   WsfManeuverPitchConstraint* Clone() const override { return new WsfManeuverPitchConstraint{*this}; }
   void                        Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool                        Satisfied(double aSimTime) const override;
   double                      NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "PITCH"; }

   //! Get the pitch target for this constraint.
   double GetPitchTarget() const { return mPitchTarget; }

   //! Set the pitch target for this constraint.
   void SetPitchTarget(double aPitchTarget) { mPitchTarget = aPitchTarget; }

private:
   WsfP6DOF_Mover* mMoverPtr;
   double          mPitchTarget;

   static const double cPITCH_TOLERANCE;
   static const double cPITCH_RATE_MINIMUM;
};

#endif // WSFMANEUVERPITCHCONSTRAINT_HPP
