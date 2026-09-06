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

#include "WsfSixDOF_ManeuverAltitudeConstraint.hpp"

#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::ManeuverAltitudeConstraint::ManeuverAltitudeConstraint(const ManeuverAltitudeConstraint& aOther)
   : mAltitudeTarget{aOther.mAltitudeTarget}
{
}

void wsf::six_dof::ManeuverAltitudeConstraint::Assign(double aSimTime, Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool wsf::six_dof::ManeuverAltitudeConstraint::Satisfied(double aSimTime) const
{
   double error = mAltitudeTarget - mMoverPtr->GetAltitudeMSL_m();

   bool crossover  = (error * mPreviousError < 0.0);
   bool acceptable = (std::abs(error) < cERROR_TOLERANCE);

   mPreviousError = error;

   return (crossover || acceptable);
}

double wsf::six_dof::ManeuverAltitudeConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = std::abs(mMoverPtr->GetVerticalSpeed());
   double delta = std::min(cMAXIMUM_RECHECK_TIME, cERROR_TOLERANCE / rate);
   return aSimTime + delta;
}

const double wsf::six_dof::ManeuverAltitudeConstraint::cERROR_TOLERANCE      = 50.0;
const double wsf::six_dof::ManeuverAltitudeConstraint::cMAXIMUM_RECHECK_TIME = 1.0;
