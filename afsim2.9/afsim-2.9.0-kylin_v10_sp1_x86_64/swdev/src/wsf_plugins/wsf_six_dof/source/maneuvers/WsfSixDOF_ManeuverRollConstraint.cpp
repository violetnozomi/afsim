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

#include "WsfSixDOF_ManeuverRollConstraint.hpp"

#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::ManeuverRollConstraint::ManeuverRollConstraint(const ManeuverRollConstraint& aOther)
   : mRollTarget{aOther.mRollTarget}
{
}

void wsf::six_dof::ManeuverRollConstraint::Assign(double aSimTime, Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool wsf::six_dof::ManeuverRollConstraint::Satisfied(double aSimTime) const
{
   double error = UtMath::NormalizeAngleMinusPi_Pi(mRollTarget - mMoverPtr->GetRoll_rad());

   // Only check for crossover if the roll error is between +/- 90 degrees
   // This avoids the possibility of a false trip near when the mover is expected to roll through 180 degrees or more
   bool crossover = false;
   if (std::abs(error) < UtMath::cPI_OVER_2)
   {
      crossover = (error * mPreviousError < 0.0);
   }
   bool acceptable = (std::abs(error) < cERROR_TOLERANCE);

   mPreviousError = error;

   return (crossover || acceptable);
}

double wsf::six_dof::ManeuverRollConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = std::max(0.1, fabs(mMoverPtr->GetRollRate_rps()));
   double delta = std::min(cMAXIMUM_RECHECK_TIME, cERROR_TOLERANCE / rate);
   return aSimTime + delta;
}

const double wsf::six_dof::ManeuverRollConstraint::cERROR_TOLERANCE      = 0.01;
const double wsf::six_dof::ManeuverRollConstraint::cMAXIMUM_RECHECK_TIME = 1.0;
