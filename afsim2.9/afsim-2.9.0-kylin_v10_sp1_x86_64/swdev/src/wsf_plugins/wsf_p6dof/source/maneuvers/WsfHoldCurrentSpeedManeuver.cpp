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

#include "WsfHoldCurrentSpeedManeuver.hpp"

#include "WsfP6DOF_Mover.hpp"

double WsfHoldCurrentSpeedManeuver::Execute(double aSimTime)
{
   double speedKCAS = GetMover()->GetSpeed_KCAS();
   GetMover()->SetAutopilotSpeedKCAS(speedKCAS);
   return -1.0;
}
