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

#include "WsfManeuverPitchConstraint.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfManeuverPitchConstraint::WsfManeuverPitchConstraint(const WsfManeuverPitchConstraint& aOther)
   : mMoverPtr{nullptr}
   , mPitchTarget{aOther.mPitchTarget}
{
}

void WsfManeuverPitchConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool WsfManeuverPitchConstraint::Satisfied(double aSimTime) const
{
   double psi, theta, phi;
   mMoverPtr->GetPlatform()->GetOrientationNED(psi, theta, phi);
   return (fabs(UtMath::AngleDifference(theta, mPitchTarget)) < cPITCH_TOLERANCE);
}

double WsfManeuverPitchConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = std::max(fabs(mMoverPtr->GetPitchRate()), cPITCH_RATE_MINIMUM);
   double delta = cPITCH_TOLERANCE / rate;
   return aSimTime + delta;
}

const double WsfManeuverPitchConstraint::cPITCH_TOLERANCE    = 1.0e-2;
const double WsfManeuverPitchConstraint::cPITCH_RATE_MINIMUM = 1.0e-4;
