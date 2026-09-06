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

#ifndef WSFDEFAULTTRACKEXTRAPOLATIONSTRATEGY_HPP
#define WSFDEFAULTTRACKEXTRAPOLATIONSTRATEGY_HPP

#include "wsf_export.h"

class WsfMeasurement;
#include "WsfTrackExtrapolationStrategy.hpp"

namespace wsf
{
class TrackExtrapolators;

//! The default track extrapolation strategy.
class WSF_EXPORT DefaultTrackExtrapolationStrategy : public TrackExtrapolationStrategy
{
public:
   DefaultTrackExtrapolationStrategy();
   DefaultTrackExtrapolationStrategy(const DefaultTrackExtrapolationStrategy& aSrc) = default;
   ~DefaultTrackExtrapolationStrategy() override                                    = default;

   void InitializeExtrapolation(const WsfMeasurement& aMeasurement, TrackExtrapolators& aExtrapolators) const override;
};
} // namespace wsf

#endif
