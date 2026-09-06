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

#include "WsfInherentContrastTypes.hpp"

#include "WsfMil.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardInherentContrast.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfInherentContrastTypes& WsfInherentContrastTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetInherentContrastTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfInherentContrastTypes& WsfInherentContrastTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetInherentContrastTypes();
}

// =================================================================================================
WsfInherentContrastTypes::WsfInherentContrastTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfInherentContrast>(aScenario, cSIGNATURE_FLAGS, "inherent_contrast")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardInherentContrast>();
   mObjectFactoryList.push_back(WsfStandardInherentContrast::ObjectFactory);

   // Add the interface entry to the prototype signature list.
   WsfInherentContrast::RegisterInterface(aScenario);
}

// =================================================================================================
bool WsfInherentContrastTypes::InitializeType(WsfInherentContrast* aTypePtr)
{
   return aTypePtr->InitializeType();
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfInherentContrastTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
