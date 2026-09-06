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

#ifndef WSF_SIXDOF_MANEUVERFLIGHTPATHANGLECONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERFLIGHTPATHANGLECONSTRAINT_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_ManeuverConstraint.hpp"

namespace wsf
{
namespace six_dof
{
class ManeuverFlightPathAngleConstraint : public ManeuverConstraint
{
public:
   ManeuverFlightPathAngleConstraint() = default;
   ManeuverFlightPathAngleConstraint(const ManeuverFlightPathAngleConstraint& aOther);
   ~ManeuverFlightPathAngleConstraint() override = default;

   ManeuverFlightPathAngleConstraint& operator=(const ManeuverFlightPathAngleConstraint& aOther) = delete;

   ManeuverFlightPathAngleConstraint* Clone() const override { return new ManeuverFlightPathAngleConstraint{*this}; }

   void   Assign(double aSimTime, Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "FLIGHT-PATH-ANGLE"; }

   //! Get the flight path angle.
   double GetFlightPathAngle() const { return mPathAngle; }

   //! Set the flight path angle.
   void SetFlightPathAngle(double aPathAngle) { mPathAngle = aPathAngle; }

private:
   Mover*         mMoverPtr{nullptr};
   double         mPathAngle     = 0.0;
   mutable double mPreviousError = 0.0;

   static const double cERROR_TOLERANCE;
   static const double cMAXIMUM_RECHECK_TIME;
};
} // namespace six_dof
} // namespace wsf


#endif // WSF_SIXDOF_MANEUVERFLIGHTPATHANGLECONSTRAINT_HPP
