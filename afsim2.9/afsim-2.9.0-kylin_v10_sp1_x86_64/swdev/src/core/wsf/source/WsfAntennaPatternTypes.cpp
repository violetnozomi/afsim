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

#include "WsfAntennaPatternTypes.hpp"

#include "WsfScenario.hpp"
#include "WsfStandardAntennaPattern.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfAntennaPatternTypes& WsfAntennaPatternTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetAntennaPatternTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfAntennaPatternTypes& WsfAntennaPatternTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetAntennaPatternTypes();
}

// =================================================================================================
WsfAntennaPatternTypes::WsfAntennaPatternTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfAntennaPattern>(aScenario, cNO_DELAY_LOAD, "antenna_pattern")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardAntennaPattern>();
}

// =================================================================================================
bool WsfAntennaPatternTypes::InitializeType(WsfAntennaPattern* aObjectPtr)
{
   return aObjectPtr->Initialize(nullptr);
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfAntennaPatternTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
