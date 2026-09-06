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

#ifndef WSFROUTECOMPUTER_HPP
#define WSFROUTECOMPUTER_HPP

#include "wsf_export.h"

class WsfMover;
class WsfRoute;

//! WsfRouteComputer provides an interface to the WsfRouteMover which allows modifying
//! the route to be traversed.
class WSF_EXPORT WsfRouteComputer
{
public:
   virtual ~WsfRouteComputer() {}

   //! Called when FindAndSetPath is called on a mover
   //! @param aMover The mover that is performing the movement.
   //! @param aRoute The route to be transformed.
   //! Returns 'true' if aRoute was modified.
   virtual bool ComputeFindPath(WsfMover& aMover, WsfRoute& aRoute) { return false; }

   //! Called when a mover is given an initial route.
   //! @param aMover The mover that is performing the movement.
   //! @param aRoute The route to be transformed.
   //! @param aInitialPointIndex The index of the point within the route where movement is to start.
   //! Returns 'true' if aRoute was modified.
   virtual bool ComputeSetRoute(WsfMover& aMover, WsfRoute& aRoute, int& aInitialPointIndex) { return false; }

   //! Called when a mover is given an updated route.
   //! @param aMover The mover that is performing the movement.
   //! @param aRoute The route to be transformed.
   //! Returns 'true' if aRoute was modified.
   virtual bool ComputeUpdateRoute(WsfMover& aMover, WsfRoute& aRoute) { return false; }

   //! Called when a mover begins extrapolating at the end of a route.
   //! @param aMover The mover that is performing the movement.
   //! @param aRoute A route which will replace extrapolation behavior if 'true' is returned.
   //! Returns 'true' if aRoute was modified.
   virtual bool ComputeExtrapolate(WsfMover& aMover, WsfRoute& aRoute) { return false; }

   virtual bool ReturnToRoute(WsfMover& aMover, WsfRoute& aRoute) { return false; }

   virtual bool IsNavMesh() { return false; }

   virtual bool GetEnabled() { return true; }
};

#endif
