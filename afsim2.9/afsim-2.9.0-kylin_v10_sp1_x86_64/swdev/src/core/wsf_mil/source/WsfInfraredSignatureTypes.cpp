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

#include "WsfInfraredSignatureTypes.hpp"

#include "WsfMil.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardInfraredSignature.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfInfraredSignatureTypes& WsfInfraredSignatureTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetInfraredSignatureTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfInfraredSignatureTypes& WsfInfraredSignatureTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetInfraredSignatureTypes();
}

// =================================================================================================
WsfInfraredSignatureTypes::WsfInfraredSignatureTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfInfraredSignature>(aScenario, cSIGNATURE_FLAGS, "infrared_signature")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardInfraredSignature>();
   mObjectFactoryList.push_back(WsfStandardInfraredSignature::ObjectFactory);

   // Add the interface entry to the prototype signature list.
   WsfInfraredSignature::RegisterInterface(aScenario);
}

// =================================================================================================
bool WsfInfraredSignatureTypes::InitializeType(WsfInfraredSignature* aTypePtr)
{
   return aTypePtr->InitializeType();
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfInfraredSignatureTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
