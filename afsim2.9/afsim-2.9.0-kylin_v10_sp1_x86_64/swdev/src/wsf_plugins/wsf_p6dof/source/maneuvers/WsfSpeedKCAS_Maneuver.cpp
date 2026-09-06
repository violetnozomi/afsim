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

#include "WsfSpeedKCAS_Maneuver.hpp"

#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfSpeedKCAS_Maneuver::WsfSpeedKCAS_Maneuver()
   : WsfManeuver{}
   , mSpeed{0.0}
{
}

WsfSpeedKCAS_Maneuver::WsfSpeedKCAS_Maneuver(const WsfSpeedKCAS_Maneuver& aOther)
   : WsfManeuver(aOther)
   , mSpeed{aOther.mSpeed}
{
}

double WsfSpeedKCAS_Maneuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotSpeedKCAS(mSpeed);
   return -1.0;
}
