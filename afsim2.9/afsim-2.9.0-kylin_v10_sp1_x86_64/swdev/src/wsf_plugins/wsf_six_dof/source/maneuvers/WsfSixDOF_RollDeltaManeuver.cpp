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

#include "WsfSixDOF_RollDeltaManeuver.hpp"

#include "UtMath.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::RollDeltaManeuver::RollDeltaManeuver()
   : Maneuver()
   , mDelta{0.0}
{
}

wsf::six_dof::RollDeltaManeuver::RollDeltaManeuver(const RollDeltaManeuver& aOther)
   : Maneuver(aOther)
   , mDelta{aOther.mDelta}
{
}

wsf::six_dof::RollDeltaManeuver* wsf::six_dof::RollDeltaManeuver::Clone() const
{
   return new RollDeltaManeuver{*this};
}

double wsf::six_dof::RollDeltaManeuver::Execute(double aSimTime)
{
   GetMover()->SetAutopilotDeltaRoll(mDelta * UtMath::cDEG_PER_RAD);
   return -1.0;
}
