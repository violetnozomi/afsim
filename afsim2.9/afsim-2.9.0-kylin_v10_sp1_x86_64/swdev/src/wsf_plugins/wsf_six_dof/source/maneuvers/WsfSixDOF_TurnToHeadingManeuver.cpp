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

#include "WsfSixDOF_TurnToHeadingManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_ManeuverHeadingConstraint.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::TurnToHeadingManeuver::TurnToHeadingManeuver(double aHeading)
   : Maneuver()
   , mHeading{aHeading}
{
   UpdateExitConstraint();
}

double wsf::six_dof::TurnToHeadingManeuver::Execute(double aSimTime)
{
   Mover* moverPtr = GetMover();
   moverPtr->TurnToHeading(aSimTime, mHeading, 0.0, WsfPath::cTURN_DIR_SHORTEST);
   moverPtr->SetAutopilotFlightPathAngle(0.0);
   return -1.0;
}

void wsf::six_dof::TurnToHeadingManeuver::SetHeading(double aHeading)
{
   mHeading = aHeading;
   UpdateExitConstraint();
}

void wsf::six_dof::TurnToHeadingManeuver::UpdateExitConstraint()
{
   auto exitConst = ut::make_unique<ManeuverHeadingConstraint>();
   exitConst->SetHeadingTarget(UtMath::NormalizeAngleMinusPi_Pi(mHeading));
   SetExitConstraint(std::move(exitConst));
}
