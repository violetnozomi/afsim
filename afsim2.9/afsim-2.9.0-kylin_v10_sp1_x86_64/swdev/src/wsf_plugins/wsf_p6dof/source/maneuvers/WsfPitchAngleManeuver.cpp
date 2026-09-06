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

#include "WsfPitchAngleManeuver.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfPitchAngleManeuver::WsfPitchAngleManeuver()
   : WsfManeuver{}
   , mAngle{0.0}
{
}

WsfPitchAngleManeuver::WsfPitchAngleManeuver(const WsfPitchAngleManeuver& aOther)
   : WsfManeuver(aOther)
   , mAngle{aOther.mAngle}
{
}

double WsfPitchAngleManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotPitchAngle(mAngle * UtMath::cDEG_PER_RAD);
   return -1.0;
}
