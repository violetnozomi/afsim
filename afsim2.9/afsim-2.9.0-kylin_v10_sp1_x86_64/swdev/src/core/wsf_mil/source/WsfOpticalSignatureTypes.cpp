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

#include "WsfOpticalSignatureTypes.hpp"

#include "WsfMil.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardOpticalSignature.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfOpticalSignatureTypes& WsfOpticalSignatureTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetOpticalSignatureTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfOpticalSignatureTypes& WsfOpticalSignatureTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetOpticalSignatureTypes();
}

// =================================================================================================
WsfOpticalSignatureTypes::WsfOpticalSignatureTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfOpticalSignature>(aScenario, cSIGNATURE_FLAGS, "optical_signature")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardOpticalSignature>();
   mObjectFactoryList.push_back(WsfStandardOpticalSignature::ObjectFactory);

   // Add the interface entry to the prototype signature list.
   WsfOpticalSignature::RegisterInterface(aScenario);
}

// =================================================================================================
bool WsfOpticalSignatureTypes::InitializeType(WsfOpticalSignature* aTypePtr)
{
   return aTypePtr->InitializeType();
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfOpticalSignatureTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
