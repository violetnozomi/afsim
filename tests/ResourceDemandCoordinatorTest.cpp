#include "nrm/ResourceDemandCoordinator.hpp"

#include <cassert>

namespace
{
nrm::ResourceDemandSet Set(const char* aBusiness)
{
   nrm::ResourceDemandSet set;
   set.demandSetId = "set-1";
   set.revision = 1;
   set.requestSource = "mission-planner";
   set.correlationId = "corr-001";
   nrm::ResourceDemand demand;
   demand.businessType = aBusiness;
   set.demands.push_back(demand);
   return set;
}

nrm::ResourceDemandBatchResult Batch(bool aPass)
{
   nrm::ResourceDemandBatchResult batch;
   batch.totalCount = 1;
   batch.satisfiedCount = aPass ? 1 : 0;
   batch.unsatisfiedCount = aPass ? 0 : 1;
   return batch;
}
}

int main()
{
   nrm::ResourceDemandCoordinator coordinator;
   const auto first = coordinator.Record(Set("C2"), Batch(true));
   assert(first.requestSource == "mission-planner");
   assert(first.correlationId == "corr-001");
   assert(first.classifications.size() == 4);
   assert(first.classifications[0] == "CONNECTIVITY");
   assert(first.classifications[1] == "PERFORMANCE");
   assert(first.classifications[2] == "NETWORK_SCALE");
   assert(first.classifications[3] == "TRAFFIC");
   assert(first.historicalPassRatioPercent == 100.0);
   const auto second = coordinator.Record(Set("C2"), Batch(false));
   assert(second.historicalPassRatioPercent == 50.0);
   const auto isolated = coordinator.Record(Set("VIDEO"), Batch(true));
   assert(isolated.historicalPassRatioPercent == 100.0);
   for (int index = 0; index < 129; ++index)
      coordinator.Record(Set("C2"), Batch(index % 2 == 0));
   assert(coordinator.HistorySize() == 128);
   return 0;
}
