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

#include "WsfRollRateManeuver.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfRollRateManeuver::WsfRollRateManeuver()
   : WsfManeuver{}
   , mRate{0.0}
{
}

WsfRollRateManeuver::WsfRollRateManeuver(const WsfRollRateManeuver& aOther)
   : WsfManeuver(aOther)
   , mRate{aOther.mRate}
{
}

double WsfRollRateManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotRollRate(mRate * UtMath::cDEG_PER_RAD);
   return -1.0;
}
