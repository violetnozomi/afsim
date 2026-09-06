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

#include "WsfOpticalReflectivityTypes.hpp"

#include "WsfMil.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardOpticalReflectivity.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfOpticalReflectivityTypes& WsfOpticalReflectivityTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetOpticalReflectivityTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfOpticalReflectivityTypes& WsfOpticalReflectivityTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Find(aScenario)->GetOpticalReflectivityTypes();
}

// =================================================================================================
WsfOpticalReflectivityTypes::WsfOpticalReflectivityTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfOpticalReflectivity>(aScenario, cREDEFINITION_ALLOWED, "optical_reflectivity")
{
   SetObjectFactory(&mObjectFactoryList);
   SetObjectFactoryDefault<WsfStandardOpticalReflectivity>();
   mObjectFactoryList.push_back(WsfStandardOpticalReflectivity::ObjectFactory);

   // Add the interface entry to the prototype signature list.
   WsfOpticalReflectivity::RegisterInterface(aScenario);
}

// =================================================================================================
// virtual
bool WsfOpticalReflectivityTypes::InitializeType(WsfOpticalReflectivity* aTypePtr)
{
   return aTypePtr->InitializeType();
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfOpticalReflectivityTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}
