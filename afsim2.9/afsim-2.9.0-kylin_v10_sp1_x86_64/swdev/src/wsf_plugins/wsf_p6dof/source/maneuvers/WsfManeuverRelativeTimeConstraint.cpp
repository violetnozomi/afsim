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

#include "WsfManeuverRelativeTimeConstraint.hpp"

#include "WsfP6DOF_Mover.hpp"

void WsfManeuverRelativeTimeConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mAssignTime = aSimTime;
}

bool WsfManeuverRelativeTimeConstraint::Satisfied(double aSimTime) const
{
   return aSimTime >= GetConstraintTime();
}

double WsfManeuverRelativeTimeConstraint::NextEvaluationTime(double aSimTime) const
{
   return GetConstraintTime();
}
