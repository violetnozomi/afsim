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

#include "WsfPM_Simulation.hpp"

WsfPM_Simulation::WsfPM_Simulation() {}

WsfPM_Simulation::WsfPM_Simulation(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

WsfProxy::Time WsfPM_Simulation::GetEndTime() const
{
   return GetAttributeT<WsfProxy::Time>("endTime");
}

WsfProxy::Time WsfPM_Simulation::GetMinimumMoverTimeStep() const
{
   return GetAttributeT<WsfProxy::Time>("minimumMoverTimestep");
}
