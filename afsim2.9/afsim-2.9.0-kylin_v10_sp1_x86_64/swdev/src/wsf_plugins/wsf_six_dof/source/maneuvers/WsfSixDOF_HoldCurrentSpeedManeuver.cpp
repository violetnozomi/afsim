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

#include "WsfSixDOF_HoldCurrentSpeedManeuver.hpp"

#include "WsfSixDOF_Mover.hpp"

double wsf::six_dof::HoldCurrentSpeedManeuver::Execute(double aSimTime)
{
   double speedKCAS = GetMover()->GetSpeed_KCAS();
   GetMover()->SetAutopilotSpeedKCAS(speedKCAS);
   return -1.0;
}
