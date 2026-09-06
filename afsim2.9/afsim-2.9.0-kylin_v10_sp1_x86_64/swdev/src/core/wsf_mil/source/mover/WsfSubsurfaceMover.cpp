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

#include "WsfSubsurfaceMover.hpp"

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfTerrain.hpp"

WsfSubsurfaceMover::WsfSubsurfaceMover(WsfScenario& aScenario)
   : WsfWaypointMover(aScenario)
{
   SetEndOfPath(WsfPath::cEP_STOP);
}

// virtual
bool WsfSubsurfaceMover::Initialize(double aSimTime)
{
   return WsfWaypointMover::Initialize(aSimTime);
}

// virtual
WsfMover* WsfSubsurfaceMover::Clone() const
{
   return new WsfSubsurfaceMover(*this);
}

// protected virtual
/** Set the state of the mover consistent with being on the surface of the water */
void WsfSubsurfaceMover::Surface()
{
   mIsOnGround = true;
}

// protected virtual
/** Set the state of the mover consistent with being under water. */
void WsfSubsurfaceMover::Dive()
{
   mIsOnGround = false;
}

// protected virtual
void WsfSubsurfaceMover::PostMoveUpdate(double aSimTime)
{
   double lat;
   double lon;
   double alt;
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   double depth = GetHeightOfSurface() - alt;
   // Allow the platform to surface
   if (!IsOnGround())
   {
      if (depth < -1.0)
      {
         Surface();
      }
   }

   WsfWaypointMover::PostMoveUpdate(aSimTime);

   // Don't let the platform re-surface if it is not intended
   if ((depth < -1.0) && (!IsOnGround()))
   {
      GetPlatform()->SetLocationLLA(lat, lon, GetHeightOfSurface());
      // depth = 0.0;
   }
}
