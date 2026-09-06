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

#include "WsfSixDOF_ManeuverAbsoluteTimeConstraint.hpp"

#include "WsfSixDOF_Mover.hpp"

void wsf::six_dof::ManeuverAbsoluteTimeConstraint::Assign(double aSimTime, Mover* aMoverPtr) {}

bool wsf::six_dof::ManeuverAbsoluteTimeConstraint::Satisfied(double aSimTime) const
{
   return aSimTime >= GetConstraintTime();
}

double wsf::six_dof::ManeuverAbsoluteTimeConstraint::NextEvaluationTime(double aSimTime) const
{
   return GetConstraintTime();
}
