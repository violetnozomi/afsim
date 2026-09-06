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

#include "WsfAcousticSignatureTypes.hpp"

#include "WsfMil.hpp"
#include "WsfStandardAcousticSignature.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfAcousticSignatureTypes& WsfAcousticSignatureTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetAcousticSignatureTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfAcousticSignatureTypes& WsfAcousticSignatureTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetAcousticSignatureTypes();
}

// =================================================================================================
WsfAcousticSignatureTypes::WsfAcousticSignatureTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfAcousticSignature>(aScenario, cSIGNATURE_FLAGS, "acoustic_signature")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardAcousticSignature>();
   mObjectFactoryList.push_back(WsfStandardAcousticSignature::ObjectFactory);

   // Add the interface entry to the prototype signature list.
   WsfAcousticSignature::RegisterInterface(aScenario);
}

// =================================================================================================
// virtual
bool WsfAcousticSignatureTypes::InitializeType(WsfAcousticSignature* aTypePtr)
{
   return aTypePtr->InitializeType();
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfAcousticSignatureTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
