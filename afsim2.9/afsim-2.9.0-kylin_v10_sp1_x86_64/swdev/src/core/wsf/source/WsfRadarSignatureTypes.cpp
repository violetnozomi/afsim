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

#include "WsfRadarSignatureTypes.hpp"

#include "WsfScenario.hpp"
#include "WsfStandardRadarSignature.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfRadarSignatureTypes& WsfRadarSignatureTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetRadarSignatureTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfRadarSignatureTypes& WsfRadarSignatureTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetRadarSignatureTypes();
}

// =================================================================================================
WsfRadarSignatureTypes::WsfRadarSignatureTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfRadarSignature>(aScenario, cSIGNATURE_FLAGS, "radar_signature")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardRadarSignature>();
   mObjectFactoryList.push_back(WsfStandardRadarSignature::ObjectFactory);

   // Add the interface entry to the prototype signature list.
   WsfRadarSignature::RegisterInterface(aScenario);
}

// =================================================================================================
// virtual
bool WsfRadarSignatureTypes::InitializeType(WsfRadarSignature* aTypePtr)
{
   return aTypePtr->InitializeType();
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfRadarSignatureTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
