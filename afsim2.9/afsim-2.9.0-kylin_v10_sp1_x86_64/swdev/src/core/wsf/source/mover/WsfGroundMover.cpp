// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfGroundMover.hpp"

#include "UtInput.hpp"
#include "UtMath.hpp"

WsfGroundMover::WsfGroundMover(const WsfScenario& aScenario)
   : WsfWaypointMover(aScenario)
{
   EnableTerrain(true);
   SetIsOnGround(true);
   SetEndOfPath(WsfPath::cEP_STOP);
   SetAltRef(WsfPath::cALT_REF_AGL);
   mMoverConstraints.mMaxRadialAccel = 1.0E10;
}

/** Copy constructor (for Clone()) */
WsfGroundMover::WsfGroundMover(const WsfGroundMover& aSrc)
   : WsfWaypointMover(aSrc)
{
}

// virtual
WsfGroundMover::~WsfGroundMover() {}

// virtual
WsfMover* WsfGroundMover::Clone() const
{
   return new WsfGroundMover(*this);
}

// virtual
bool WsfGroundMover::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());

   // Prohibit invalid commands for this Mover:
   if ((command == "airborne") || (command == "in_air"))
   {
      throw UtInput::BadValue(aInput, "Command " + command + " cannot be used with a WSF_GROUND_MOVER.");
   }

   return WsfWaypointMover::ProcessInput(aInput);
}
