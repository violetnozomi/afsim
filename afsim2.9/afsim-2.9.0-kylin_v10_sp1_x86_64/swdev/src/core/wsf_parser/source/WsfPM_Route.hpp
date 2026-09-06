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

#ifndef WSFPM_ROUTE_HPP
#define WSFPM_ROUTE_HPP

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_Waypoint.hpp"
#include "WsfPProxyNode.hpp"

class WsfPProxyList;

class WSF_PARSER_EXPORT WsfPM_Route : public WsfPProxyNode
{
public:
   WsfPM_Route();
   WsfPM_Route(const WsfPProxyNode& aNode);

   size_t         WaypointCount() const;
   WsfPM_Waypoint GetWaypoint(size_t aIndex) const;

   bool RemoveWaypoint(size_t aIndex);
   bool InsertWaypoint(WsfPProxyValue aWaypoint, size_t aIndex);

   WsfPM_WaypointList WaypointList() const { return *this + "waypoints"; }
};

using WsfPM_RouteMap = WsfPM_ObjectMapT<WsfPM_Route>;

#endif
