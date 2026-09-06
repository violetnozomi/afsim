/**
 * @file NrmTacticalActivity.hpp
 * @brief Pure helpers for rendering recent, observed communication activity.
 */

#ifndef NRM_TACTICAL_ACTIVITY_HPP
#define NRM_TACTICAL_ACTIVITY_HPP

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace WkNrm
{
inline bool HasObservedTraffic(const nrm::WindowMetrics& aWindow)
{
   return aWindow.messages.transmitted > 0 || aWindow.messages.received > 0 ||
          aWindow.messages.hops > 0 || aWindow.transmittedBits > 0 ||
          aWindow.deliveredBits > 0;
}

inline bool HasRecentLinkActivity(const nrm::LinkSnapshot& aLink)
{
   if (aLink.state != nrm::ResourceState::cONLINE)
   {
      return false;
   }

   // Ten seconds is long enough for a human-visible pulse while still keeping
   // old 60-second history from being presented as current link activity.
   for (const nrm::WindowMetrics& window : aLink.windows)
   {
      if (window.windowS > 0.0 && window.windowS <= 10.0 &&
          HasObservedTraffic(window))
      {
         return true;
      }
   }
   return false;
}

inline std::size_t RecentLinkActivityCount(
   const std::vector<nrm::LinkSnapshot>& aLinks)
{
   std::size_t count = 0;
   for (const nrm::LinkSnapshot& link : aLinks)
   {
      if (HasRecentLinkActivity(link))
      {
         ++count;
      }
   }
   return count;
}

inline double LinkActivityPhase(std::uint64_t aFrame, std::size_t aLane)
{
   const double base = static_cast<double>(aFrame % 40U) / 40.0;
   const double laneOffset = static_cast<double>(aLane % 3U) / 3.0;
   return std::fmod(base + laneOffset, 1.0);
}
} // namespace WkNrm

#endif
