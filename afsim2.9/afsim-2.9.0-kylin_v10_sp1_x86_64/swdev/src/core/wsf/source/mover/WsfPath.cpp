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

#include "WsfPath.hpp"

//! Obtain the altitude reference, allowing for the WsfWaypoint value to override the WsfMover default.
// static
WsfPath::AltRef WsfPath::GetAltRef(AltRef aMoverDefault, AltRef aWaypointValue)
{
   AltRef value = aMoverDefault;
   if (aWaypointValue != cALT_REF_MOVER_DEFAULT)
   {
      value = aWaypointValue;
   }
   return value;
}
