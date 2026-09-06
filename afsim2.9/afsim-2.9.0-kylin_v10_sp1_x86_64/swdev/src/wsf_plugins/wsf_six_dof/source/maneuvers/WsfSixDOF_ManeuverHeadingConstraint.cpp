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

#include "WsfSixDOF_ManeuverHeadingConstraint.hpp"

#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::ManeuverHeadingConstraint::ManeuverHeadingConstraint(const ManeuverHeadingConstraint& aOther)
   : mHeadingTarget{aOther.mHeadingTarget}
{
}

void wsf::six_dof::ManeuverHeadingConstraint::Assign(double aSimTime, Mover* aMoverPtr)
{
   mMoverPtr = aMoverPtr;
}

bool wsf::six_dof::ManeuverHeadingConstraint::Satisfied(double aSimTime) const
{
   double error = UtMath::NormalizeAngleMinusPi_Pi(mHeadingTarget - mMoverPtr->GetHeading_rad());

   // Only check for crossover if the heading error is between +/- 90 degrees
   // This avoids the possibility of a false trip near when the mover is expected to change heading by 180 degrees or more
   bool crossover = false;
   if (std::abs(error) < UtMath::cPI_OVER_2)
   {
      crossover = (error * mPreviousError < 0.0);
   }
   bool acceptable = (std::abs(error) < cERROR_TOLERANCE);

   mPreviousError = error;

   return (crossover || acceptable);
}

double wsf::six_dof::ManeuverHeadingConstraint::NextEvaluationTime(double aSimTime) const
{
   double rate  = fabs(mMoverPtr->GetYawRate_rps());
   double delta = std::min(cMAXIMUM_RECHECK_TIME, cERROR_TOLERANCE / rate);
   return aSimTime + delta;
}

const double wsf::six_dof::ManeuverHeadingConstraint::cERROR_TOLERANCE      = 0.005;
const double wsf::six_dof::ManeuverHeadingConstraint::cMAXIMUM_RECHECK_TIME = 1.0;
