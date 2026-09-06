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

#include "WsfRollDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfRollDeltaManeuver::WsfRollDeltaManeuver()
   : WsfManeuver()
   , mDelta{0.0}
{
}

WsfRollDeltaManeuver::WsfRollDeltaManeuver(const WsfRollDeltaManeuver& aOther)
   : WsfManeuver(aOther)
   , mDelta{aOther.mDelta}
{
}

WsfRollDeltaManeuver* WsfRollDeltaManeuver::Clone() const
{
   return new WsfRollDeltaManeuver{*this};
}

double WsfRollDeltaManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotDeltaRoll(mDelta * UtMath::cDEG_PER_RAD);
   return -1.0;
}
