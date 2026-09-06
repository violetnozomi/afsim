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

#include "WsfSixDOF_RollRateManeuver.hpp"

#include "UtMath.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::RollRateManeuver::RollRateManeuver()
   : Maneuver{}
   , mRate{0.0}
{
}

wsf::six_dof::RollRateManeuver::RollRateManeuver(const RollRateManeuver& aOther)
   : Maneuver(aOther)
   , mRate{aOther.mRate}
{
}

double wsf::six_dof::RollRateManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotRollRate(mRate * UtMath::cDEG_PER_RAD);
   return -1.0;
}
