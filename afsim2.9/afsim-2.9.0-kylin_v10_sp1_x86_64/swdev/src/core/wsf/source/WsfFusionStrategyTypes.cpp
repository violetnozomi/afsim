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

#include "WsfFusionStrategyTypes.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfDefaultFusion.hpp"
#include "WsfScenario.hpp"

bool WsfFusionStrategyTypes::mCheckFuseEstimates = false;

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfFusionStrategyTypes& WsfFusionStrategyTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetFusionTypes();
}

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfFusionStrategyTypes& WsfFusionStrategyTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetFusionTypes();
}

// =================================================================================================
WsfFusionStrategyTypes::WsfFusionStrategyTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfFusionStrategy>(aScenario, "fusion_strategy")
{
   Add("replacement", ut::make_unique<WsfDefaultFusion>(aScenario));
   Add("default", ut::make_unique<WsfDefaultFusion>(aScenario)); // "default" is the same as replacement fusion (deprecated).
   Add("weighted_average", ut::make_unique<WsfDefaultFusion>(aScenario, true)); // The "true" parameter indicates the
                                                                                // use of weighted-average fusion.
   mDefaultStrategyName = "replacement";
}

// =================================================================================================
//! Return the name of the default strategy (used when the strategy_method input is missing
//! in a track_manager input block).
const std::string& WsfFusionStrategyTypes::GetDefaultStrategyName() const
{
   return mDefaultStrategyName;
}

// =================================================================================================
//! Set the default strategy name.  If the name does not correspond to a known
//! strategy type, an exception will be thrown.
void WsfFusionStrategyTypes::SetDefaultStrategyName(const std::string& aName)
{
   if (Find(aName) != nullptr)
   {
      mDefaultStrategyName = aName;
   }
   else
   {
      throw UtException("Fusion Strategy: Unknown Strategy Name: " + aName);
   }
}

// =================================================================================================
//! Configure the global default fusion strategy using the default_fusion_strategy keyword.
//! If no default strategy exists, "replacement" will be used as the default.
bool WsfFusionStrategyTypes::ProcessInput(UtInput& aInput)
{
   bool        processed = false;
   std::string command   = aInput.GetCommand();
   if (command == "default_fusion_strategy")
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
   else if (command == "check_fuse_estimates") // Undocumented flag to be set in order to avoid run-time singular
                                               // exceptions thrown in "FuseEstimates."
   {
      aInput.ReadValue(mCheckFuseEstimates);
      processed = true;
   }
   else
   {
      processed = WsfObjectTypeList<WsfFusionStrategy>::ProcessInput(aInput);
   }
   return processed;
}

// =================================================================================================
//! (Factory Method) Create a new instance of a strategy with the given name.
//! @param aName The name of the desired strategy.
//! @return A new instance of the desired strategy, or 0 if the strategy is not registered.
WsfFusionStrategy* WsfFusionStrategyTypes::Create(const std::string& aName) const
{
   WsfFusionStrategy* strategyPtr    = Find(aName);
   WsfFusionStrategy* newStrategyPtr = nullptr;
   if (strategyPtr != nullptr)
   {
      newStrategyPtr = strategyPtr->Clone();
   }
   return newStrategyPtr;
}

// =================================================================================================
// static
bool WsfFusionStrategyTypes::LoadInstance(UtInput& aInput, WsfFusionStrategy*& aStrategyPtr) const

{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "fusion_method")
   {
      myCommand = true;
      std::string type;
      aInput.ReadCommand(type);
      aStrategyPtr = Create(type);
      if (aStrategyPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown track fusion method: " + type);
      }

      // If the input is the non-block, legacy format, the next command should be a valid track manager command;
      // otherwise we follow the new block format, and all commands within the block should be fusion strategy commands
      // exclusively. If for some reason track manager commands  erroneously get placed inside the fusion_method block,
      // the "end_fusion_method" won't be recognized and will throw and error.
      bool blockRequired = false;
      bool doContinue    = true;
      do
      {
         aInput.ReadCommand(command);
         if (command == "end_fusion_method")
         {
            doContinue = false;
         }
         else if (aStrategyPtr->ProcessInput(aInput))
         {
            blockRequired = true; // Legacy input did not process any keywords, so being here means input is in a block.
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
               out.AddNote() << "Expected: end_fusion_method";
               throw UtInput::UnknownCommand(aInput);
            }
         }
      } while (doContinue);
   }
   return myCommand;
}
