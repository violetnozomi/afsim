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

#include "WsfSixDOF_SpeedKCAS_Maneuver.hpp"

#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::SpeedKCAS_Maneuver::SpeedKCAS_Maneuver()
   : Maneuver{}
   , mSpeed{0.0}
{
}

wsf::six_dof::SpeedKCAS_Maneuver::SpeedKCAS_Maneuver(const SpeedKCAS_Maneuver& aOther)
   : Maneuver(aOther)
   , mSpeed{aOther.mSpeed}
{
}

double wsf::six_dof::SpeedKCAS_Maneuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotSpeedKCAS(mSpeed);
   return -1.0;
}
