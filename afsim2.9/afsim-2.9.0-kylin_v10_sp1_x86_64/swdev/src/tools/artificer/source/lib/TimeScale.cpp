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

#include "TimeScale.hpp"

#include <stdexcept>
#include <string>

namespace artificer
{
std::string TimeScaleString(TimeScale aScale)
{
   switch (aScale)
   {
   case TimeScale::Seconds:
      return "s";
   case TimeScale::Milliseconds:
      return "ms";
   case TimeScale::Microseconds:
      return "us";
   case TimeScale::Nanoseconds:
      return "ns";
   }
   throw std::logic_error("Missing TimeScale string");
}

std::pair<int64_t, TimeScale> RescaleDuration(int64_t aDuration, TimeScale aScale)
{
   const int64_t kRescaleThreshold = 10000;
   while (aDuration >= kRescaleThreshold && aScale != TimeScale::Seconds)
   {
      aDuration /= 1000;
      if (aScale == TimeScale::Nanoseconds)
      {
         aScale = TimeScale::Microseconds;
      }
      else if (aScale == TimeScale::Microseconds)
      {
         aScale = TimeScale::Milliseconds;
      }
      else if (aScale == TimeScale::Milliseconds)
      {
         aScale = TimeScale::Seconds;
      }
   }
   return {aDuration, aScale};
}

std::string DurationString(int64_t aDuration, TimeScale aScale)
{
   const std::pair<int64_t, TimeScale> rescaled = RescaleDuration(aDuration, aScale);
   return std::to_string(rescaled.first) + TimeScaleString(rescaled.second);
}

} // namespace artificer
