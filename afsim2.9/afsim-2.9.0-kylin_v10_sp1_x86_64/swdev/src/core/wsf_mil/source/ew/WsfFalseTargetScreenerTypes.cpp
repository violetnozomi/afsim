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

#include "WsfFalseTargetScreenerTypes.hpp"

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfMil.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfFalseTargetScreenerTypes& WsfFalseTargetScreenerTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetFalseTargetScreenerTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfFalseTargetScreenerTypes& WsfFalseTargetScreenerTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetFalseTargetScreenerTypes();
}

// =================================================================================================
WsfFalseTargetScreenerTypes::WsfFalseTargetScreenerTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfFalseTargetScreener>(aScenario, "false_target_screener")
{
   Add("WSF_FT_SCREENER", ut::make_unique<WsfFalseTargetScreener>(aScenario));
}

// =================================================================================================
//! Process instance commands
//! @param   aInput       The input stream.
//! @param   aInstancePtr [output] a pointer to the new filter instance or 0.
//! @returns 'true' if the current command is a filter command or 'false' if not.
//! @throws  An exception is thrown if the input contains errors.
bool WsfFalseTargetScreenerTypes::LoadInstance(UtInput& aInput, WsfFalseTargetScreener*& aInstancePtr) const
{
   bool myCommand = false;
   if (aInput.GetCommand() == "false_target_screener")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      if (aInstancePtr != nullptr)
      {
         delete aInstancePtr;
      }
      std::string instanceName;
      aInput.ReadValue(instanceName);
      std::string baseType;
      aInput.ReadValue(baseType);
      aInstancePtr = Clone(baseType);
      if (aInstancePtr != nullptr)
      {
         aInstancePtr->SetName(instanceName);
         inputBlock.ProcessInput(aInstancePtr);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown screener type: " + baseType);
      }
   }
   return myCommand;
}
