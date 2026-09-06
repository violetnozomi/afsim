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

#include "WsfManeuverAbsoluteTimeConstraint.hpp"

#include "WsfP6DOF_Mover.hpp"

void WsfManeuverAbsoluteTimeConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr) {}

bool WsfManeuverAbsoluteTimeConstraint::Satisfied(double aSimTime) const
{
   return aSimTime >= GetConstraintTime();
}

double WsfManeuverAbsoluteTimeConstraint::NextEvaluationTime(double aSimTime) const
{
   return GetConstraintTime();
}
