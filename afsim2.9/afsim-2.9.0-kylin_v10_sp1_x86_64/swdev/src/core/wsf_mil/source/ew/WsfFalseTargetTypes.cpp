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

#include "WsfFalseTargetTypes.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfMil.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfFalseTargetTypes& WsfFalseTargetTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetFalseTargetTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfFalseTargetTypes& WsfFalseTargetTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetFalseTargetTypes();
}

// =================================================================================================
WsfFalseTargetTypes::WsfFalseTargetTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfFalseTarget>(aScenario, "false_target")
{
   AddCoreType("WSF_FALSE_TARGET", ut::make_unique<WsfFalseTarget>(aScenario));
}

// =================================================================================================
bool WsfFalseTargetTypes::InitializeType(WsfFalseTarget* aObjectPtr)
{
   return aObjectPtr->Initialize(nullptr);
}

// =================================================================================================
//! Load a type instance from an input stream.
//! @param   aInput       The input stream.
//! @param   aInstancePtr [output] a pointer to the new type instance or 0.
//! @returns 'true' if the current command is a type instance command or 'false' if not.
//! @throws  An exception is thrown if the input contains errors.
bool WsfFalseTargetTypes::LoadInstance(UtInput& aInput, WsfFalseTarget*& aInstancePtr)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "false_target")
   {
      myCommand = true;
      std::string instanceName;
      aInput.ReadValue(instanceName);
      std::string baseType;
      aInput.ReadValue(baseType);
      aInstancePtr = Clone(baseType);
      UtInputBlock inputBlock(aInput);
      if (aInstancePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown false target type: " + baseType);
      }
      else
      {
         aInstancePtr->SetName(instanceName);
         inputBlock.ProcessInput(aInstancePtr);
         if (!aInstancePtr->Initialize(nullptr))
         {
            throw UtInput::BadValue(aInput, "False Target type did not Initialize, and was not viable.");
         }
      }
   }
   return myCommand;
}
