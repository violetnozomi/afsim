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

#include "WsfMaskingPatternTypes.hpp"

#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfMaskingPatternTypes& WsfMaskingPatternTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetMaskingPatternTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfMaskingPatternTypes& WsfMaskingPatternTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetMaskingPatternTypes();
}

// =================================================================================================
WsfMaskingPatternTypes::WsfMaskingPatternTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfMaskingPattern>(aScenario, cREDEFINITION_ALLOWED, "masking_pattern")
{
   SetSingularBaseType();
}

// =================================================================================================
bool WsfMaskingPatternTypes::InitializeType(ObjectType* aTypePtr)
{
   return aTypePtr->Initialize();
}
