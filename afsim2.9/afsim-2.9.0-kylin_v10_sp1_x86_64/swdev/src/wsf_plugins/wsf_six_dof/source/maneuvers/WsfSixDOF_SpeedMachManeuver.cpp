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

#include "WsfSixDOF_SpeedMachManeuver.hpp"

#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::SpeedMachManeuver::SpeedMachManeuver()
   : Maneuver{}
   , mMach{0.0}
{
}

wsf::six_dof::SpeedMachManeuver::SpeedMachManeuver(const SpeedMachManeuver& aOther)
   : Maneuver(aOther)
   , mMach{aOther.mMach}
{
}

double wsf::six_dof::SpeedMachManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotSpeedMach(mMach);
   return -1.0;
}
