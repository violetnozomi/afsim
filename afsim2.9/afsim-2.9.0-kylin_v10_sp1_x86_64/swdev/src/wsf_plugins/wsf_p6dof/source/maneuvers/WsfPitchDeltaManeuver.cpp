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

#include "WsfPitchDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfPitchDeltaManeuver::WsfPitchDeltaManeuver()
   : WsfManeuver()
   , mDelta{0.0}
{
}

WsfPitchDeltaManeuver::WsfPitchDeltaManeuver(const WsfPitchDeltaManeuver& aOther)
   : WsfManeuver(aOther)
   , mDelta{aOther.mDelta}
{
}

WsfPitchDeltaManeuver* WsfPitchDeltaManeuver::Clone() const
{
   return new WsfPitchDeltaManeuver{*this};
}

double WsfPitchDeltaManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotDeltaPitch(mDelta * UtMath::cDEG_PER_RAD);
   return -1.0;
}
