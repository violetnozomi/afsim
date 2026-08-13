#include "nrm/DegradationPolicy.hpp"

#include <algorithm>
#include <cassert>

namespace
{
bool Contains(const std::vector<std::string>& aValues, const char* aValue)
{
   return std::find(aValues.begin(), aValues.end(), aValue) != aValues.end();
}

void Metric(nrm::MetricValue<double>& aMetric, double aValue)
{
   aMetric.value = aValue;
   aMetric.valid = true;
   aMetric.confidence = nrm::Confidence::cHIGH;
   aMetric.reason = nrm::MetricReason::cNONE;
}

nrm::ResourceSnapshot L0Snapshot()
{
   nrm::ResourceSnapshot snapshot;
   nrm::EndpointSnapshot source;
   source.endpointId = "source";
   source.platformName = "source";
   source.networkType = nrm::NetworkType::cLINK16;
   source.state = nrm::ResourceState::cONLINE;
   nrm::EndpointSnapshot destination = source;
   destination.endpointId = "destination";
   destination.platformName = "destination";
   snapshot.endpoints = {source, destination};
   return snapshot;
}
}

int main()
{
   nrm::DegradationPolicy policy;
   const nrm::DegradationStatus l0 = policy.Evaluate(L0Snapshot());
   assert(l0.level == nrm::DegradationLevel::cL0_TOPOLOGY);
   assert(l0.degraded);
   assert(Contains(l0.missingFields, "links"));
   assert(Contains(l0.usedDefaults, "NETWORK_PROFILE_CAPACITY"));
   assert(l0.confidence == nrm::Confidence::cLOW);

   nrm::ResourceSnapshot l1Snapshot = L0Snapshot();
   nrm::LinkSnapshot link;
   link.sourceEndpointId = "source";
   link.destinationEndpointId = "destination";
   link.networkType = nrm::NetworkType::cLINK16;
   link.state = nrm::ResourceState::cONLINE;
   Metric(link.bandwidthBps, 1000.0);
   nrm::WindowMetrics window;
   window.windowS = 10.0;
   Metric(window.averageTransportDelayMs, 10.0);
   link.windows.push_back(window);
   l1Snapshot.links.push_back(link);
   const nrm::DegradationStatus l1 = policy.Evaluate(l1Snapshot);
   assert(l1.level == nrm::DegradationLevel::cL1_LINK);
   assert(Contains(l1.usedDefaults, "PDR_ESTIMATE"));

   Metric(l1Snapshot.links[0].windows[0].deliveryRatioPercent, 99.0);
   Metric(l1Snapshot.links[0].snrDb, 15.0);
   const nrm::DegradationStatus l3 = policy.Evaluate(l1Snapshot, true);
   assert(l3.level == nrm::DegradationLevel::cL3_RESOURCE);
   assert(!l3.degraded);
   assert(l3.dataCoveragePercent == 100.0);
   assert(Contains(l3.usedDefaults, "FOUR_NETWORK_ACCEPTANCE_QUOTAS"));

   const nrm::DegradationStatus empty = policy.Evaluate(nrm::ResourceSnapshot());
   assert(empty.level == nrm::DegradationLevel::cUNAVAILABLE);
   assert(!empty.usable);
   return 0;
}
