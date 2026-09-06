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

#include "WsfTurnToHeadingManeuver.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfManeuverHeadingConstraint.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfTurnToHeadingManeuver::WsfTurnToHeadingManeuver(double aHeading)
   : WsfManeuver()
   , mHeading{aHeading}
{
   UpdateExitConstraint();
}

double WsfTurnToHeadingManeuver::Execute(double aSimTime)
{
   WsfP6DOF_Mover* moverPtr = GetMover();
   moverPtr->TurnToHeading(aSimTime, mHeading, 0.0, WsfPath::cTURN_DIR_SHORTEST);
   moverPtr->SetAutopilotFlightPathAngle(0.0);
   return -1.0;
}

void WsfTurnToHeadingManeuver::SetHeading(double aHeading)
{
   mHeading = aHeading;
   UpdateExitConstraint();
}

void WsfTurnToHeadingManeuver::UpdateExitConstraint()
{
   auto exitConst = ut::make_unique<WsfManeuverHeadingConstraint>();
   exitConst->SetHeadingTarget(UtMath::NormalizeAngleMinusPi_Pi(mHeading));
   SetExitConstraint(std::move(exitConst));
}
