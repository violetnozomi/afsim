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

#include "WsfManeuverFlightPathAngleConstraint.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfManeuverFlightPathAngleConstraint::WsfManeuverFlightPathAngleConstraint(const WsfManeuverFlightPathAngleConstraint& aOther)
   : mPathAngle{aOther.mPathAngle}
   , mMoverPtr{nullptr}
{
}

void WsfManeuverFlightPathAngleConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool WsfManeuverFlightPathAngleConstraint::Satisfied(double aSimTime) const
{
   double fpa = mMoverPtr->GetFlightPathAngle();
   return fabs(fpa * UtMath::cRAD_PER_DEG - mPathAngle) < cFLIGHT_PATH_ANGLE_TOLERANCE;
}

double WsfManeuverFlightPathAngleConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = std::max(fabs(mMoverPtr->GetPitchRate()), cPITCH_RATE_MINIMUM);
   double delta = cFLIGHT_PATH_ANGLE_TOLERANCE / rate;
   return aSimTime + delta;
}

const double WsfManeuverFlightPathAngleConstraint::cFLIGHT_PATH_ANGLE_TOLERANCE = 1.0e-2;
const double WsfManeuverFlightPathAngleConstraint::cPITCH_RATE_MINIMUM          = 1.0e-4;
