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

#include "WsfSpeedMachManeuver.hpp"

#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

WsfSpeedMachManeuver::WsfSpeedMachManeuver()
   : WsfManeuver{}
   , mMach{0.0}
{
}

WsfSpeedMachManeuver::WsfSpeedMachManeuver(const WsfSpeedMachManeuver& aOther)
   : WsfManeuver(aOther)
   , mMach{aOther.mMach}
{
}

double WsfSpeedMachManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotSpeedMach(mMach);
   return -1.0;
}
