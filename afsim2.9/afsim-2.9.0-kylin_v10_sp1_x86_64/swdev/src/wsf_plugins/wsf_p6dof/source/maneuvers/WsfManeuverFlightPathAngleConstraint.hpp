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

#ifndef WSFMANEUVERFLIGHTPATHANGLECONSTRAINT_HPP
#define WSFMANEUVERFLIGHTPATHANGLECONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuverConstraint.hpp"

class WsfManeuverFlightPathAngleConstraint : public WsfManeuverConstraint
{
public:
   WsfManeuverFlightPathAngleConstraint()
      : mPathAngle{0.0}
      , mMoverPtr{nullptr}
   {
   }
   WsfManeuverFlightPathAngleConstraint(const WsfManeuverFlightPathAngleConstraint& aOther);
   ~WsfManeuverFlightPathAngleConstraint() override = default;

   WsfManeuverFlightPathAngleConstraint& operator=(const WsfManeuverFlightPathAngleConstraint& aOther) = delete;

   WsfManeuverFlightPathAngleConstraint* Clone() const override
   {
      return new WsfManeuverFlightPathAngleConstraint{*this};
   }

   void   Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) override;
   bool   Satisfied(double aSimTime) const override;
   double NextEvaluationTime(double aSimTime) const override;

   const char* Type() const override { return "FLIGHT-PATH-ANGLE"; }

   //! Get the flight path angle.
   double GetFlightPathAngle() const { return mPathAngle; }

   //! Set the flight path angle.
   void SetFlightPathAngle(double aPathAngle) { mPathAngle = aPathAngle; }

private:
   double          mPathAngle;
   WsfP6DOF_Mover* mMoverPtr;

   static const double cFLIGHT_PATH_ANGLE_TOLERANCE;
   static const double cPITCH_RATE_MINIMUM;
};

#endif // WSFMANEUVERFLIGHTPATHANGLECONSTRAINT_HPP
