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

#include "WsfCorrelationStrategyTypes.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfNearestNeighborCorrelation.hpp"
#include "WsfPerfectCorrelation.hpp"
#include "WsfScenario.hpp"
#include "WsfTruthCorrelation.hpp"


// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfCorrelationStrategyTypes& WsfCorrelationStrategyTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetCorrelationTypes();
}

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfCorrelationStrategyTypes& WsfCorrelationStrategyTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetCorrelationTypes();
}

// =================================================================================================
WsfCorrelationStrategyTypes::WsfCorrelationStrategyTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfCorrelationStrategy>(aScenario, "correlation_method")
   , mDefaultStrategyName("perfect")
{
   AddCoreType("perfect", ut::make_unique<WsfPerfectCorrelation>());
   AddCoreType("nearest_neighbor", ut::make_unique<WsfNearestNeighborCorrelation>());
   AddCoreType("truth", ut::make_unique<WsfTruthCorrelation>());
}

// =================================================================================================
//! Return the name of the default strategy (used when the strategy_method input is missing
//! in a track_manager input block).
const std::string& WsfCorrelationStrategyTypes::GetDefaultStrategyName() const
{
   return mDefaultStrategyName;
}

// =================================================================================================
//! Set the default strategy name.  If the name does not correspond to a known
//! strategy type, an exception will be thrown.
void WsfCorrelationStrategyTypes::SetDefaultStrategyName(const std::string& aName)
{
   if (Find(aName) != nullptr)
   {
      mDefaultStrategyName = aName;
   }
   else
   {
      throw UtException("Correlation Strategy: Unknown Strategy Name: " + aName);
   }
}

// =================================================================================================
//! Configure the global default fusion strategy using the default_fusion_method keyword.
//! If no default strategy exists, "replacement" will be used as the default.
bool WsfCorrelationStrategyTypes::ProcessInput(UtInput& aInput)
{
   bool        processed = false;
   std::string command   = aInput.GetCommand();
   if (command == "default_correlation_method")
   {
      processed = true;
      std::string type;
      aInput.ReadValue(type);
      if (Find(type) != nullptr)
      {
         mDefaultStrategyName = type;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else
   {
      processed = WsfObjectTypeList<WsfCorrelationStrategy>::ProcessInput(aInput);
   }
   return processed;
}

// =================================================================================================
//! (Factory Method) Create a new instance of a strategy with the given name.
//! @param aName The name of the desired strategy.
//! @return A new instance of the desired strategy, or 0 if the strategy is not registered.
WsfCorrelationStrategy* WsfCorrelationStrategyTypes::Create(const std::string& aName) const
{
   WsfCorrelationStrategy* strategyPtr    = Find(aName);
   WsfCorrelationStrategy* newStrategyPtr = nullptr;
   if (strategyPtr != nullptr)
   {
      newStrategyPtr = strategyPtr->Clone();
   }
   return newStrategyPtr;
}

// =================================================================================================
// static
bool WsfCorrelationStrategyTypes::LoadInstance(UtInput& aInput, WsfCorrelationStrategy*& aStrategyPtr) const

{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "correlation_method")
   {
      myCommand = true;
      std::string type;
      aInput.ReadCommand(type);
      aStrategyPtr = Create(type);
      if (aStrategyPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown track correlation method: " + type);
      }

      // If the input is the non-block, legacy format, the next command should be a valid track manager command;
      // otherwise we follow the new block format, and all commands within the block should be correlation strategy
      // commands exclusively. If for some reason track manager commands  erroneously get placed inside the
      // correlation_method block, the "end_correlation_method" won't be recognized and will throw and error.
      bool blockRequired = false;
      bool doContinue    = true;
      do
      {
         aInput.ReadCommand(command);
         if (command == "end_correlation_method")
         {
            doContinue = false;
         }
         else if (aStrategyPtr->ProcessInput(aInput))
         {
            blockRequired = true; // Even though some legacy keywords were accepted they were not
                                  // formally documented, so we will assume that if any strategy input is processed,
                                  // a block is required.
         }
         else // potentially a valid track manager input
         {
            doContinue = false;
            if (!blockRequired)
            {
               aInput.PushBack(command); // let the track manager handle it on the next pass
            }
            else
            {
               // The full block was required, and this command was not recognized.
               auto out = ut::log::error() << "Missing block terminator.";
               out.AddNote() << "Expected: end_correlation_method";
               throw UtInput::UnknownCommand(aInput);
            }
         }
      } while (doContinue);
   }

   return myCommand;
}
