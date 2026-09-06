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

#include "WsfSixDOF_ManeuverFlightPathAngleConstraint.hpp"

#include "UtMath.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::ManeuverFlightPathAngleConstraint::ManeuverFlightPathAngleConstraint(const ManeuverFlightPathAngleConstraint& aOther)
   : mPathAngle{aOther.mPathAngle}
{
}

void wsf::six_dof::ManeuverFlightPathAngleConstraint::Assign(double aSimTime, Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool wsf::six_dof::ManeuverFlightPathAngleConstraint::Satisfied(double aSimTime) const
{
   double error = UtMath::NormalizeAngleMinusPi_Pi(mPathAngle - mMoverPtr->GetFlightPathAngle_rad());

   // Check for crossover between +/- 45 degrees flight path angle
   // This avoids the possibility of a false trip during vertical loop events
   bool crossover = false;
   if (std::abs(error) < UtMath::cPI_OVER_4)
   {
      crossover = (error * mPreviousError < 0.0);
   }
   bool acceptable = (std::abs(error) < cERROR_TOLERANCE);

   mPreviousError = error;

   return (crossover || acceptable);
}

double wsf::six_dof::ManeuverFlightPathAngleConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = std::abs(mMoverPtr->GetPitchRate_rps());
   double delta = std::min(cMAXIMUM_RECHECK_TIME, cERROR_TOLERANCE / rate);
   return aSimTime + delta;
}

const double wsf::six_dof::ManeuverFlightPathAngleConstraint::cERROR_TOLERANCE      = 0.01;
const double wsf::six_dof::ManeuverFlightPathAngleConstraint::cMAXIMUM_RECHECK_TIME = 1.0;
