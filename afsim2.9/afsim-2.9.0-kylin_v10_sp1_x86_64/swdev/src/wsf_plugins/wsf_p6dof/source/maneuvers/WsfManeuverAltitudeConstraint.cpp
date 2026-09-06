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

#include "WsfManeuverAltitudeConstraint.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfManeuverAltitudeConstraint::WsfManeuverAltitudeConstraint(const WsfManeuverAltitudeConstraint& aOther)
   : mMoverPtr{nullptr}
   , mAltitudeTarget{aOther.mAltitudeTarget}
{
}

void WsfManeuverAltitudeConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool WsfManeuverAltitudeConstraint::Satisfied(double aSimTime) const
{
   double alt = mMoverPtr->GetPlatform()->GetAltitude();
   return (fabs(alt - mAltitudeTarget) < cALTITUDE_TOLERANCE);
}

double WsfManeuverAltitudeConstraint::NextEvaluationTime(double aSimTime) const
{
   double vVert = fabs(mMoverPtr->GetVerticalSpeed());
   double delta = std::min(cALTITUDE_TOLERANCE / vVert, cMINIMUM_RECHECK_TIME);
   return aSimTime + delta;
}

const double WsfManeuverAltitudeConstraint::cALTITUDE_TOLERANCE   = 50.0;
const double WsfManeuverAltitudeConstraint::cMINIMUM_RECHECK_TIME = 0.1;