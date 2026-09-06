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

#include "WsfSixDOF_RollAngleManeuver.hpp"

#include "UtMath.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::RollAngleManeuver::RollAngleManeuver()
   : Maneuver{}
   , mAngle{0.0}
{
}

wsf::six_dof::RollAngleManeuver::RollAngleManeuver(const RollAngleManeuver& aOther)
   : Maneuver(aOther)
   , mAngle{aOther.mAngle}
{
}

double wsf::six_dof::RollAngleManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotRollAngle(mAngle * UtMath::cDEG_PER_RAD);
   return -1.0;
}
