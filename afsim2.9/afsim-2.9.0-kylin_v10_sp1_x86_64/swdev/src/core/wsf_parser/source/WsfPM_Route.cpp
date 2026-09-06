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

#include "WsfPM_Route.hpp"

WsfPM_Route::WsfPM_Route() {}

WsfPM_Route::WsfPM_Route(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

size_t WsfPM_Route::WaypointCount() const
{
   return WaypointList().size();
}

WsfPM_Waypoint WsfPM_Route::GetWaypoint(size_t aIndex) const
{
   WsfPM_WaypointList waypts = WaypointList();
   if (waypts.size() > aIndex)
   {
      return waypts[aIndex];
   }
   return WsfPM_Waypoint();
}

bool WsfPM_Route::RemoveWaypoint(size_t aIndex)
{
   return WaypointList().remove(aIndex);
}

bool WsfPM_Route::InsertWaypoint(WsfPProxyValue aWaypoint, size_t aIndex)
{
   return WaypointList().insert(aWaypoint, aIndex);
}
