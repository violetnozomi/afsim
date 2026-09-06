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

#include "WsfManeuverHeadingConstraint.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfManeuverHeadingConstraint::WsfManeuverHeadingConstraint(const WsfManeuverHeadingConstraint& aOther)
   : mMoverPtr{nullptr}
   , mHeadingTarget{aOther.mHeadingTarget}
{
}

void WsfManeuverHeadingConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool WsfManeuverHeadingConstraint::Satisfied(double aSimTime) const
{
   double psi, theta, phi;
   mMoverPtr->GetPlatform()->GetOrientationNED(psi, theta, phi);
   return (fabs(psi - mHeadingTarget) < cHEADING_TOLERANCE);
}

double WsfManeuverHeadingConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = std::max(fabs(mMoverPtr->GetYawRate()), cYAW_RATE_MINIMUM);
   double delta = cHEADING_TOLERANCE / rate;
   return aSimTime + delta;
}

const double WsfManeuverHeadingConstraint::cHEADING_TOLERANCE = 5.0e-3;
const double WsfManeuverHeadingConstraint::cYAW_RATE_MINIMUM  = 1.0e-3;
