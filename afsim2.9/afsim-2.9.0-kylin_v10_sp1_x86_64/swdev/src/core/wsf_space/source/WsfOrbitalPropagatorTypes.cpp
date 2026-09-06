// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalPropagatorTypes.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfSpace.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfOrbitalPropagatorTypes& WsfOrbitalPropagatorTypes::Get(WsfScenario& aScenario)
{
   return WsfSpaceExtension::Get(aScenario).GetOrbitalPropagatorTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfOrbitalPropagatorTypes& WsfOrbitalPropagatorTypes::Get(const WsfScenario& aScenario)
{
   return WsfSpaceExtension::Get(aScenario).GetOrbitalPropagatorTypes();
}

// =================================================================================================
std::unique_ptr<UtOrbitalPropagatorBase> WsfOrbitalPropagatorTypes::CreateInstance(const std::string& aTypeName) const
{
   for (const auto& factoryPtr : mObjectFactoryList)
   {
      auto instancePtr = factoryPtr(aTypeName);
      if (instancePtr)
      {
         return instancePtr;
      }
   }
   return nullptr;
}

// =================================================================================================
//! Process propagator type input.
//! @param aInput              The current input stream.
//! @param aOrbitalPropagatorPtr The pointer to the orbital propagator.
//! @return 'true' if the current command was recognized and processed, 'false' if not recognized.
bool WsfOrbitalPropagatorTypes::LoadInstance(UtInput&                                  aInput,
                                             std::unique_ptr<UtOrbitalPropagatorBase>& aOrbitalPropagatorPtr) const
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "propagator")
   {
      myCommand = true;
      std::string propagatorType;
      aInput.ReadValue(propagatorType);
      aOrbitalPropagatorPtr = CreateInstance(propagatorType);
      if (aOrbitalPropagatorPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown propagator type: " + propagatorType);
      }

      UtInputBlock block(aInput);
      block.ProcessInput(aOrbitalPropagatorPtr.get());
   }
   return myCommand;
}

// ================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be added only once.
void WsfOrbitalPropagatorTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(std::move(aFactoryPtr));
}
