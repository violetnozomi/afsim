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

#ifndef WSF_SIXDOF_MANEUVERPITCHCONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERPITCHCONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT ManeuverPitchConstraint : public ManeuverConstraint
{
public:
   ManeuverPitchConstraint() = default;
   ManeuverPitchConstraint(const ManeuverPitchConstraint& aOther);
   ~ManeuverPitchConstraint() override = default;

   ManeuverPitchConstraint& operator=(const ManeuverPitchConstraint& aOther) = delete;

   ManeuverPitchConstraint* Clone() const override { return new ManeuverPitchConstraint{*this}; }
   void                     Assign(double aSimTime, Mover* aMoverPtr) override;
   bool                     Satisfied(double aSimTime) const override;
   double                   NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "PITCH"; }

   //! Get the pitch target for this constraint.
   double GetPitchTarget() const { return mPitchTarget; }

   //! Set the pitch target for this constraint.
   void SetPitchTarget(double aPitchTarget) { mPitchTarget = aPitchTarget; }

private:
   Mover*         mMoverPtr{nullptr};
   double         mPitchTarget   = 0.0;
   mutable double mPreviousError = 0.0;

   static const double cERROR_TOLERANCE;
   static const double cMAXIMUM_RECHECK_TIME;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERPITCHCONSTRAINT_HPP
