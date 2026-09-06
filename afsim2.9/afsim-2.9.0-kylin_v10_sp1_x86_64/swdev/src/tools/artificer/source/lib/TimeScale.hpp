// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef TIMESCALE_HPP
#define TIMESCALE_HPP

#include "artificer_export.h"

#include <string>
#include <utility>


namespace artificer
{
enum class TimeScale
{
   Nanoseconds = 0,
   Microseconds,
   Milliseconds,
   Seconds
};

///@brief Returns a string abbrieviation for the given `aScale` constant
std::string TimeScaleString(TimeScale aScale);
///@brief Returns a pair indicating an equivalent duration to `aDuration` in `aScale` but possibly in an adjusted scale
/// for readability.
std::pair<int64_t, TimeScale> ARTIFICER_EXPORT RescaleDuration(int64_t aDuration, TimeScale aScale);
///@brief Returns a human readble string representing the duration given by `aDuration` and `aScale`
std::string DurationString(int64_t aDuration, TimeScale aScale);

} // namespace artificer

#endif
