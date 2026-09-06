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

#include "WsfSixDOF_PitchDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::PitchDeltaManeuver::PitchDeltaManeuver()
   : Maneuver()
   , mDelta{0.0}
{
}

wsf::six_dof::PitchDeltaManeuver::PitchDeltaManeuver(const wsf::six_dof::PitchDeltaManeuver& aOther)
   : Maneuver(aOther)
   , mDelta{aOther.mDelta}
{
}

wsf::six_dof::PitchDeltaManeuver* wsf::six_dof::PitchDeltaManeuver::Clone() const
{
   return new PitchDeltaManeuver{*this};
}

double wsf::six_dof::PitchDeltaManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotDeltaPitch(mDelta * UtMath::cDEG_PER_RAD);
   return -1.0;
}
