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

#ifndef WSFTRACKEXTRAPOLATIONSTRATEGYTYPES_HPP
#define WSFTRACKEXTRAPOLATIONSTRATEGYTYPES_HPP

#include "wsf_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfTrackExtrapolationStrategy.hpp"

namespace wsf
{
class DefaultTrackExtrapolationStrategy;

//! A types factory of wsf::TrackExtrapolationStrategy types.
//! @note Currently only the default strategy is used by the core (see WsfMeasurement::GetTrackExtrapolators).
//! However, this capability could eventually be used to configure individual trackers with a specific strategy.
class TrackExtrapolationStrategyTypes : public WsfObjectTypeList<TrackExtrapolationStrategy>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT TrackExtrapolationStrategyTypes& Get(WsfScenario& aScenario);

   static WSF_EXPORT const TrackExtrapolationStrategyTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit TrackExtrapolationStrategyTypes(WsfScenario& aScenario);

   //! Get the default strategy.
   DefaultTrackExtrapolationStrategy& GetDefaultStrategy() const { return *mDefaultStrategyPtr; }

private:
   DefaultTrackExtrapolationStrategy* mDefaultStrategyPtr{nullptr};
};

} // namespace wsf
#endif
