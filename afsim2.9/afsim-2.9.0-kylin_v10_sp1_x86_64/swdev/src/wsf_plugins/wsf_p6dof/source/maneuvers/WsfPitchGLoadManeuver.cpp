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

#include "WsfPitchGLoadManeuver.hpp"

#include "WsfManeuverConstraint.hpp"
#include "WsfP6DOF_Mover.hpp"

WsfPitchGLoadManeuver::WsfPitchGLoadManeuver()
   : WsfManeuver()
   , mG_Load{0.0}
{
}

WsfPitchGLoadManeuver::WsfPitchGLoadManeuver(const WsfPitchGLoadManeuver& aOther)
   : WsfManeuver(aOther)
   , mG_Load{aOther.mG_Load}
{
}

WsfPitchGLoadManeuver* WsfPitchGLoadManeuver::Clone() const
{
   return new WsfPitchGLoadManeuver{*this};
}

double WsfPitchGLoadManeuver::Execute(double aSimTime)
{
   GetMover()->SetPitchGLoad(mG_Load);
   return -1.0;
}
