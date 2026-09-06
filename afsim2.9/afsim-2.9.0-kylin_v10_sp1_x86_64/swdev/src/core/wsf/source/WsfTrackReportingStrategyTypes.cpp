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

#include "WsfTrackReportingStrategyTypes.hpp"

#include "UtMemory.hpp"
#include "WsfBatchTrackReporting.hpp"
#include "WsfCyclicTrackReporting.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfTrackReportingStrategyTypes& WsfTrackReportingStrategyTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetTrackReportingStrategyTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfTrackReportingStrategyTypes& WsfTrackReportingStrategyTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetTrackReportingStrategyTypes();
}

// =================================================================================================
WsfTrackReportingStrategyTypes::WsfTrackReportingStrategyTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfTrackReportingStrategy>(aScenario, "reporting_strategy")
{
   Add("batch", ut::make_unique<WsfBatchTrackReporting>());
   Add("cyclic", ut::make_unique<WsfCyclicTrackReporting>());
}

// =================================================================================================
//! (Factory Method) Create a new instance of a strategy with the given name.
//! @param aName The name of the desired strategy.
//! @param aTrackProcessor The instance of WsfTrackProcessor that will be used by this strategy internally.
//! @return A new instance of the desired strategy, or 0 if the strategy is not registered.
WsfTrackReportingStrategy* WsfTrackReportingStrategyTypes::Create(WsfStringId aName, WsfTrackProcessor& aTrackProcessor) const
{
   WsfTrackReportingStrategy* strategyPtr    = Find(aName);
   WsfTrackReportingStrategy* newStrategyPtr = nullptr;
   if (strategyPtr != nullptr)
   {
      newStrategyPtr = strategyPtr->Clone();
      newStrategyPtr->SetTrackProcessor(aTrackProcessor);
   }
   return newStrategyPtr;
}
