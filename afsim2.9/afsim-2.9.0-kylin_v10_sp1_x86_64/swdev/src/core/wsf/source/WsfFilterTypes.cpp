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

#include "WsfFilterTypes.hpp"

#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfScenario.hpp"

// Pre-defined types to be registered.
#include "WsfAlphaBetaFilter.hpp"
#include "WsfAlphaBetaGammaFilter.hpp"
#include "WsfKalmanFilter.hpp"
#include "WsfKalmanFilter2D_RB.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfFilterTypes& WsfFilterTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetFilterTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfFilterTypes& WsfFilterTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetFilterTypes();
}

// =================================================================================================
WsfFilterTypes::WsfFilterTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfFilter>(aScenario, "filter")
{
   AddCoreType("WSF_ALPHA_BETA_FILTER", ut::make_unique<WsfAlphaBetaFilter>());
   AddCoreType("WSF_ALPHA_BETA_GAMMA_FILTER", ut::make_unique<WsfAlphaBetaGammaFilter>());
   AddCoreType("WSF_KALMAN_FILTER", ut::make_unique<WsfKalmanFilter>());
   AddCoreType("WSF_KALMAN_FILTER_2D_RB", ut::make_unique<WsfKalmanFilter2D_RB>());
}

// =================================================================================================
//! Create an instance of a 'type object' of this class
//! Process the current command if it would create an instance of an 'type object' of this class.
//! @param aInput The input stream.
//! @param aInstancePtr A reference to the pointer of the created instance.
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
bool WsfFilterTypes::LoadInstance(UtInput& aInput, WsfFilter*& aInstancePtr) const
{
   bool myCommand = false;
   if (aInput.GetCommand() == "filter")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      aInstancePtr = nullptr;
      std::string baseType;
      aInput.ReadValue(baseType);
      GetScenario()->GetDeferredInput().MaybeRequires("filter", baseType);
      aInstancePtr = Clone(baseType);
      if (aInstancePtr != nullptr)
      {
         inputBlock.ProcessInput(aInstancePtr);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown filter type: " + baseType);
      }
   }
   return myCommand;
}

void WsfFilterTypes::Add(const std::string& aName, WsfFilter* aFilter)
{
   AddCoreType(aName, std::unique_ptr<WsfFilter>(aFilter));
}
