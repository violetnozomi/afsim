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

#include "WsfPitchRateManeuver.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfPitchRateManeuver::WsfPitchRateManeuver()
   : WsfManeuver{}
   , mRate{0.0}
{
}

WsfPitchRateManeuver::WsfPitchRateManeuver(const WsfPitchRateManeuver& aOther)
   : WsfManeuver(aOther)
   , mRate{aOther.mRate}
{
}

double WsfPitchRateManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotPitchRate(mRate * UtMath::cDEG_PER_RAD);
   return -1.0;
}
