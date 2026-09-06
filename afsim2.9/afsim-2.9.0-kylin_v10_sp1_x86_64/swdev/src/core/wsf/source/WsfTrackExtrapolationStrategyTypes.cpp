//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "WsfTrackExtrapolationStrategyTypes.hpp"

#include "UtMemory.hpp"
#include "WsfDefaultTrackExtrapolationStrategy.hpp"
#include "WsfScenario.hpp"

namespace wsf
{

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const TrackExtrapolationStrategyTypes& TrackExtrapolationStrategyTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetTrackExtrapolationStrategyTypes();
}

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
TrackExtrapolationStrategyTypes& TrackExtrapolationStrategyTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetTrackExtrapolationStrategyTypes();
}

// =================================================================================================
TrackExtrapolationStrategyTypes::TrackExtrapolationStrategyTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<TrackExtrapolationStrategy>(aScenario, "track_extrapolation_strategy")
{
   AddCoreType("default", ut::make_unique<DefaultTrackExtrapolationStrategy>());
   mDefaultStrategyPtr = static_cast<wsf::DefaultTrackExtrapolationStrategy*>(Find("default"));
}

} // namespace wsf
