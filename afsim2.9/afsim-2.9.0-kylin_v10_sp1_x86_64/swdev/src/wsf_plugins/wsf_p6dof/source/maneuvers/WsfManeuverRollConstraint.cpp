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

#include "WsfManeuverRollConstraint.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfManeuverRollConstraint::WsfManeuverRollConstraint(const WsfManeuverRollConstraint& aOther)
   : mMoverPtr{nullptr}
   , mRollTarget{aOther.mRollTarget}
{
}

void WsfManeuverRollConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool WsfManeuverRollConstraint::Satisfied(double aSimTime) const
{
   double psi, theta, phi;
   mMoverPtr->GetPlatform()->GetOrientationNED(psi, theta, phi);
   return (fabs(UtMath::AngleDifference(phi, mRollTarget)) < cROLL_TOLERANCE);
}

double WsfManeuverRollConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = fabs(mMoverPtr->GetRollRate());
   double delta = std::min(cROLL_TOLERANCE / rate, cDELTA_T_MIN);
   return aSimTime + delta;
}

const double WsfManeuverRollConstraint::cROLL_TOLERANCE = 1.0e-2;
const double WsfManeuverRollConstraint::cDELTA_T_MIN    = 1.0;
