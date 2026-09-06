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

#ifndef WSFMANEUVERALTITUDECONSTRAINT_HPP
#define WSFMANEUVERALTITUDECONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WSF_P6DOF_EXPORT WsfManeuverAltitudeConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverAltitudeConstraint()
      : mMoverPtr{nullptr}
      , mAltitudeTarget{0.0}
   {
   }
   WsfManeuverAltitudeConstraint(const WsfManeuverAltitudeConstraint& aOther);
   ~WsfManeuverAltitudeConstraint() override = default;

   WsfManeuverAltitudeConstraint& operator=(const WsfManeuverAltitudeConstraint& aOther) = delete;

   WsfManeuverAltitudeConstraint* Clone() const override { return new WsfManeuverAltitudeConstraint{*this}; }
   void                           Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool                           Satisfied(double aSimTime) const override;
   double                         NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "ALTITUDE"; }

   //! Get the altitiude target for this constraint.
   double GetAltitudeTarget() const { return mAltitudeTarget; }

   //! Set the altitude target for this constraint.
   void SetAltitudeTarget(double aRollTarget) { mAltitudeTarget = aRollTarget; }

private:
   WsfP6DOF_Mover* mMoverPtr;
   double          mAltitudeTarget;

   static const double cALTITUDE_TOLERANCE;
   static const double cMINIMUM_RECHECK_TIME;
};

#endif // WSFMANEUVERALTITUDECONSTRAINT_HPP
