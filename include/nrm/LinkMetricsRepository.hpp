/**
 * @file LinkMetricsRepository.hpp
 * @brief Correlates physical per-hop delivery attempts with receive events.
 */

#ifndef NRM_LINK_METRICS_REPOSITORY_HPP
#define NRM_LINK_METRICS_REPOSITORY_HPP

#include <cstdint>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "nrm/RollingMetrics.hpp"

namespace nrm
{
class LinkMetricsRepository
{
public:
   void Clear()
   {
      mMetrics.clear();
      mAttemptTimes.clear();
      mAttemptObservedLinks.clear();
   }

   void RecordDeliveryAttempt(const std::string& aLinkId,
                              std::uint64_t      aMessageId,
                              double             aSimTime,
                              std::uint64_t      aBits)
   {
      RollingMetrics& metrics = mMetrics[aLinkId];
      metrics.SetTransmitObservationAvailable(true);
      metrics.RecordTransmit(aSimTime, aBits);
      mAttemptObservedLinks.insert(aLinkId);
      mAttemptTimes[CorrelationKey(aLinkId, aMessageId)] = aSimTime;
   }

   void RecordReceive(const std::string& aLinkId,
                      std::uint64_t      aMessageId,
                      double             aSimTime,
                      std::uint64_t      aBits)
   {
      RollingMetrics& metrics = mMetrics[aLinkId];
      double          transportDelayMs = -1.0;
      const auto      attemptIt = mAttemptTimes.find(CorrelationKey(aLinkId, aMessageId));
      if (attemptIt != mAttemptTimes.end())
      {
         transportDelayMs = 1000.0 * (aSimTime - attemptIt->second);
         mAttemptTimes.erase(attemptIt);
      }
      else if (mAttemptObservedLinks.find(aLinkId) == mAttemptObservedLinks.end())
      {
         // Some third-party communication implementations may emit receive
         // without exposing a delivery-attempt event.  Preserve that fact so
         // a receive count is not presented as a valid PDR denominator.
         metrics.SetTransmitObservationAvailable(false);
      }
      metrics.RecordReceive(aSimTime, aBits, transportDelayMs);
   }

   const RollingMetrics* Find(const std::string& aLinkId) const
   {
      const auto it = mMetrics.find(aLinkId);
      return it == mMetrics.end() ? nullptr : &it->second;
   }

   void Prune(double aSimTime)
   {
      const double oldestAllowed = aSimTime - RollingMetrics::cMAX_WINDOW_S;
      for (auto it = mAttemptTimes.begin(); it != mAttemptTimes.end();)
      {
         it = it->second <= oldestAllowed ? mAttemptTimes.erase(it) : std::next(it);
      }
   }

private:
   using CorrelationKey = std::pair<std::string, std::uint64_t>;

   std::map<std::string, RollingMetrics> mMetrics;
   std::map<CorrelationKey, double>       mAttemptTimes;
   std::set<std::string>                  mAttemptObservedLinks;
};
} // namespace nrm

#endif
