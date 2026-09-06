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

#include "WsfRouteNetworkTypes.hpp"

#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfRouteNetworkTypes& WsfRouteNetworkTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetRouteNetworkTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfRouteNetworkTypes& WsfRouteNetworkTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetRouteNetworkTypes();
}

// =================================================================================================
WsfRouteNetworkTypes::WsfRouteNetworkTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfRouteNetwork>(aScenario, "route_network")
{
   SetSingularBaseType();
}
