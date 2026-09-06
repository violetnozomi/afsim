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

#ifndef WSF_SIXDOF_MANEUVERHEADINGCONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERHEADINGCONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT ManeuverHeadingConstraint : public ManeuverConstraint
{
public:
   ManeuverHeadingConstraint() = default;
   ManeuverHeadingConstraint(const ManeuverHeadingConstraint& aOther);
   ~ManeuverHeadingConstraint() override = default;

   ManeuverHeadingConstraint& operator=(const ManeuverHeadingConstraint& aOther) = delete;

   ManeuverHeadingConstraint* Clone() const override { return new ManeuverHeadingConstraint{*this}; }
   void                       Assign(double aSimTime, Mover* aMoverPtr) override;
   bool                       Satisfied(double aSimTime) const override;
   double                     NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "HEADING"; }

   //! Get the heading target for this constraint.
   double GetHeadingTarget() const { return mHeadingTarget; }

   //! Set the heading target for this constraint.
   void SetHeadingTarget(double aRollTarget) { mHeadingTarget = aRollTarget; }

private:
   Mover*         mMoverPtr{nullptr};
   double         mHeadingTarget = 0.0;
   mutable double mPreviousError = 0.0;

   static const double cERROR_TOLERANCE;
   static const double cMAXIMUM_RECHECK_TIME;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERHEADINGCONSTRAINT_HPP
