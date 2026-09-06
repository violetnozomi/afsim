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

#include "WsfSixDOF_SpeedKTAS_Maneuver.hpp"

#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::SpeedKTAS_Maneuver::SpeedKTAS_Maneuver()
   : Maneuver{}
   , mSpeed{0.0}
{
}

wsf::six_dof::SpeedKTAS_Maneuver::SpeedKTAS_Maneuver(const SpeedKTAS_Maneuver& aOther)
   : Maneuver(aOther)
   , mSpeed{aOther.mSpeed}
{
}

double wsf::six_dof::SpeedKTAS_Maneuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotSpeedKTAS(mSpeed);
   return -1.0;
}
