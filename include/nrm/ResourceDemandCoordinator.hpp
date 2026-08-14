/**
 * @file ResourceDemandCoordinator.hpp
 * @brief Correlates demand feedback and maintains a bounded business history.
 */

#ifndef NRM_RESOURCE_DEMAND_COORDINATOR_HPP
#define NRM_RESOURCE_DEMAND_COORDINATOR_HPP

#include <deque>
#include <string>

#include "nrm/ResourceDemandTypes.hpp"

namespace nrm
{
class ResourceDemandCoordinator
{
public:
   ResourceDemandFeedback Record(const ResourceDemandSet& aSet,
                                 const ResourceDemandBatchResult& aBatch)
   {
      ResourceDemandFeedback feedback;
      feedback.requestSource = aSet.requestSource.empty()
                                  ? aSet.providerId : aSet.requestSource;
      feedback.correlationId = aSet.correlationId.empty()
                                  ? aSet.demandSetId : aSet.correlationId;
      feedback.classifications = {"CONNECTIVITY", "PERFORMANCE",
                                  "NETWORK_SCALE", "TRAFFIC"};
      feedback.batch = aBatch;
      const std::string business = aSet.demands.empty() ||
                                   aSet.demands.front().businessType.empty()
                                      ? "UNKNOWN" : aSet.demands.front().businessType;
      const bool passed = aBatch.totalCount > 0 &&
                          aBatch.satisfiedCount == aBatch.totalCount;
      mHistory.push_back({business, passed});
      while (mHistory.size() > 128) mHistory.pop_front();
      std::size_t samples = 0;
      std::size_t successes = 0;
      for (const History& item : mHistory)
      {
         if (item.businessType != business) continue;
         ++samples;
         if (item.passed) ++successes;
      }
      feedback.historySampleCount = samples;
      feedback.historicalPassRatioPercent = samples == 0
         ? 0.0 : 100.0 * successes / static_cast<double>(samples);
      return feedback;
   }

   std::size_t HistorySize() const { return mHistory.size(); }

private:
   struct History
   {
      std::string businessType;
      bool passed = false;
   };
   std::deque<History> mHistory;
};
}

#endif
