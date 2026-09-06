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

#include "WsfSurfaceMover.hpp"

#include "UtInput.hpp"
#include "UtMath.hpp"

WsfSurfaceMover::WsfSurfaceMover(const WsfScenario& aScenario)
   : WsfWaypointMover(aScenario)
{
   // Set update time tolerance as the time required to travel 1 meter at 120 mi/hr (about 1.8E-2)
   mUpdateTimeTolerance = 1.0 / (120.0 * 5280.0 * UtMath::cFT_PER_M / 3600.0);
   // On ground and on the surface of the water are equivalent.
   mIsOnGround = true;
   // Pitch and roll are set zero, not to change (the ocean doesn't have a slope).
   mPitchEnable = false;
   // The water is not road, but in this context, it will force roll = 0.0, which is what we want.
   mIsOnRoad = true;
   // Default behavior.
   SetEndOfPath(WsfPath::cEP_STOP);
}

/** Copy constructor (for Clone()) */
// protected
WsfSurfaceMover::WsfSurfaceMover(const WsfSurfaceMover& aSrc)
   : WsfWaypointMover(aSrc)
{
}

// virtual
WsfSurfaceMover::~WsfSurfaceMover() {}

// virtual
WsfMover* WsfSurfaceMover::Clone() const
{
   return new WsfSurfaceMover(*this);
}

// virtual
bool WsfSurfaceMover::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());

   // Prohibit invalid commands for this Mover:
   if ((command == "pitch_enable") || (command == "airborne") || (command == "in_air") || (command == "off_road"))
   {
      throw UtInput::BadValue(aInput, "Command " + command + " cannot be used with a WSF_SURFACE_MOVER.");
   }

   return WsfWaypointMover::ProcessInput(aInput);
}
