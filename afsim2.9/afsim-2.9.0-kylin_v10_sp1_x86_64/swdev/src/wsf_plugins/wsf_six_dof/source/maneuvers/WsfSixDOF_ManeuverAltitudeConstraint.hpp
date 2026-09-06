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

#ifndef WSF_SIXDOF_MANEUVERALTITUDECONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERALTITUDECONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT ManeuverAltitudeConstraint : public ManeuverConstraint
{
public:
   ManeuverAltitudeConstraint() = default;
   ManeuverAltitudeConstraint(const ManeuverAltitudeConstraint& aOther);
   ~ManeuverAltitudeConstraint() override = default;

   ManeuverAltitudeConstraint& operator=(const ManeuverAltitudeConstraint& aOther) = delete;

   ManeuverAltitudeConstraint* Clone() const override { return new ManeuverAltitudeConstraint{*this}; }
   void                        Assign(double aSimTime, Mover* aMoverPtr) override;
   bool                        Satisfied(double aSimTime) const override;
   double                      NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "ALTITUDE"; }

   //! Get the altitude target for this constraint.
   double GetAltitudeTarget() const { return mAltitudeTarget; }

   //! Set the altitude target for this constraint.
   void SetAltitudeTarget(double aRollTarget) { mAltitudeTarget = aRollTarget; }

private:
   Mover*         mMoverPtr{nullptr};
   double         mAltitudeTarget = 0.0;
   mutable double mPreviousError  = 0.0;

   static const double cERROR_TOLERANCE;
   static const double cMAXIMUM_RECHECK_TIME;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERALTITUDECONSTRAINT_HPP
