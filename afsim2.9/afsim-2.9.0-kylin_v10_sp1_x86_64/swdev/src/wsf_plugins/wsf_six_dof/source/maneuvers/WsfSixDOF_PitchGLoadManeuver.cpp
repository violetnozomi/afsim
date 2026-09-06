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

#include "WsfSixDOF_PitchGLoadManeuver.hpp"

#include "WsfSixDOF_ManeuverConstraint.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::PitchGLoadManeuver::PitchGLoadManeuver()
   : Maneuver()
   , mG_Load{0.0}
{
}

wsf::six_dof::PitchGLoadManeuver::PitchGLoadManeuver(const PitchGLoadManeuver& aOther)
   : Maneuver(aOther)
   , mG_Load{aOther.mG_Load}
{
}

wsf::six_dof::PitchGLoadManeuver* wsf::six_dof::PitchGLoadManeuver::Clone() const
{
   return new PitchGLoadManeuver{*this};
}

double wsf::six_dof::PitchGLoadManeuver::Execute(double aSimTime)
{
   GetMover()->SetPitchGLoad(mG_Load);
   return -1.0;
}
