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

#include "WsfSpeedKTAS_Maneuver.hpp"

#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfSpeedKTAS_Maneuver::WsfSpeedKTAS_Maneuver()
   : WsfManeuver{}
   , mSpeed{0.0}
{
}

WsfSpeedKTAS_Maneuver::WsfSpeedKTAS_Maneuver(const WsfSpeedKTAS_Maneuver& aOther)
   : WsfManeuver(aOther)
   , mSpeed{aOther.mSpeed}
{
}

double WsfSpeedKTAS_Maneuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotSpeedKTAS(mSpeed);
   return -1.0;
}
