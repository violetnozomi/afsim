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

#include "WsfFlightPathAngleManeuver.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfFlightPathAngleManeuver::WsfFlightPathAngleManeuver()
   : WsfManeuver{}
   , mAngle{0.0}
{
}

WsfFlightPathAngleManeuver::WsfFlightPathAngleManeuver(const WsfFlightPathAngleManeuver& aOther)
   : WsfManeuver(aOther)
   , mAngle{aOther.mAngle}
{
}

double WsfFlightPathAngleManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotFlightPathAngle(mAngle * UtMath::cDEG_PER_RAD);
   return -1.0;
}
