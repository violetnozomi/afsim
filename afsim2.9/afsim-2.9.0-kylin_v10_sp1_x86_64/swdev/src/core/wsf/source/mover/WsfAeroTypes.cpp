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

#include "WsfAeroTypes.hpp"

#include "UtMemory.hpp"
#include "WsfAero2D.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfAeroTypes& WsfAeroTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetAeroTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfAeroTypes& WsfAeroTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetAeroTypes();
}

// =================================================================================================
WsfAeroTypes::WsfAeroTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfAero>(aScenario, cREDEFINITION_ALLOWED, "aero")
{
   // Register pre-defined types.
   AddCoreType("WSF_AERO", ut::make_unique<WsfAero2D>(aScenario.GetAtmosphere()));
}
