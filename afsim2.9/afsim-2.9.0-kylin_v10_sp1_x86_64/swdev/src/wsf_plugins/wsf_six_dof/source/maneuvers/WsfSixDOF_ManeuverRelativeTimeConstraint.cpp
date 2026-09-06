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

#include "WsfSixDOF_ManeuverRelativeTimeConstraint.hpp"

#include "WsfSixDOF_Mover.hpp"

void wsf::six_dof::ManeuverRelativeTimeConstraint::Assign(double aSimTime, Mover* aMoverPtr)
{
   mAssignTime = aSimTime;
}

bool wsf::six_dof::ManeuverRelativeTimeConstraint::Satisfied(double aSimTime) const
{
   return aSimTime >= GetConstraintTime();
}

double wsf::six_dof::ManeuverRelativeTimeConstraint::NextEvaluationTime(double aSimTime) const
{
   return GetConstraintTime();
}
