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

#include "WsfLaunchComputerTypes.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfATA_LaunchComputer.hpp"
#include "WsfATG_LaunchComputer.hpp"
#include "WsfAirToAirLaunchComputer.hpp"
#include "WsfBallisticLaunchComputer.hpp"
#include "WsfBallisticMissileLaunchComputer.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfMil.hpp"
#include "WsfOrbitalLaunchComputer.hpp"
#include "WsfSAM_LaunchComputer.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptLaunchComputer.hpp"
#include "WsfTabularLaunchComputer.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfLaunchComputerTypes& WsfLaunchComputerTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetLaunchComputerTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfLaunchComputerTypes& WsfLaunchComputerTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetLaunchComputerTypes();
}

// =================================================================================================
WsfLaunchComputerTypes::WsfLaunchComputerTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfLaunchComputer>(aScenario, "launch_computer")
{
   Add(WsfLaunchComputer::BaseTypeName(), ut::make_unique<WsfLaunchComputer>());

   AddCoreType(WsfATA_LaunchComputer::BaseTypeName(), ut::make_unique<WsfATA_LaunchComputer>());
   AddCoreType(WsfAirToAirLaunchComputer::BaseTypeName(),
               ut::make_unique<WsfAirToAirLaunchComputer>(aScenario.GetAtmosphere()));
   AddCoreType(WsfATG_LaunchComputer::BaseTypeName(), ut::make_unique<WsfATG_LaunchComputer>());
   AddCoreType(WsfBallisticLaunchComputer::BaseTypeName(), ut::make_unique<WsfBallisticLaunchComputer>());
   AddCoreType(WsfBallisticMissileLaunchComputer::BaseTypeName(), ut::make_unique<WsfBallisticMissileLaunchComputer>());
   AddCoreType(WsfOrbitalLaunchComputer::BaseTypeName(), ut::make_unique<WsfOrbitalLaunchComputer>());
   AddCoreType(WsfScriptLaunchComputer::BaseTypeName(), ut::make_unique<WsfScriptLaunchComputer>(aScenario));
   AddCoreType(WsfSAM_LaunchComputer::BaseTypeName(), ut::make_unique<WsfSAM_LaunchComputer>());
   AddCoreType(WsfTabularLaunchComputer::BaseTypeName(), ut::make_unique<WsfTabularLaunchComputer>());
}

// =================================================================================================
//! Create an instance of a 'type object' of this class
//! Process the current command if it would create an instance of an 'type object' of this class.
//! @param aInput       The input stream.
//! @param aWeaponPtr    The weapon to which the instance is to be added.
//! @param aAddInstance 'true' if a new instance is to be created and added to the weapon.
//! (the first word is the name of the instance to be added and the second word is the name
//! of the 'type object' to be used as the base object)
//! or 'false' if the commands are to update an existing instance on the LaunchComputer.
//! (The first word specifies the name of the instance to be updated).
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
bool WsfLaunchComputerTypes::LoadInstance(UtInput& aInput, WsfWeapon* aWeaponPtr, bool aAddInstance) const
{
   bool myCommand = false;

   if (aInput.GetCommand() == "launch_computer")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      if (aAddInstance)
      {
         // Adding a new instance
         std::string baseType;
         aInput.ReadValue(baseType);
         mScenarioPtr->GetDeferredInput().MaybeRequires("launch_computer", baseType);
         std::unique_ptr<WsfLaunchComputer> instancePtr(Clone(baseType));
         if (instancePtr != nullptr)
         {
            inputBlock.ProcessInput(instancePtr.get());
            // If anything is amiss, an exception will be thrown.
            // Otherwise, all is well, so continue:
            aWeaponPtr->SetLaunchComputer(instancePtr.release());
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown launch_computer type: " + baseType);
         }
      }
      else
      {
         // Edit an existing instance
         WsfLaunchComputer* instancePtr = aWeaponPtr->GetLaunchComputer();
         // instancePtr has already been checked for non-zero.
         inputBlock.ProcessInput(instancePtr);
      }
   }
   return myCommand;
}
