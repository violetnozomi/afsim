#include "nrm/BuiltInEnvironmentEffectAdapter.hpp"

#include <cassert>

namespace
{
void Set(nrm::MetricValue<double>& aMetric, double aValue)
{
   aMetric.value = aValue;
   aMetric.valid = true;
   aMetric.sampleTime = 10.0;
   aMetric.reason = nrm::MetricReason::cNONE;
}

nrm::ResourceSnapshot Snapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.simTime = 10.0;
   snapshot.environment.valid = true;
   snapshot.environment.sampleTime = 10.0;
   snapshot.environment.terrain.available = true;
   snapshot.environment.terrain.enabled = true;
   snapshot.environment.weather.available = true;
   snapshot.environment.celestial.available = true;
   snapshot.environment.interference.available = true;
   Set(snapshot.environment.weather.rainRateMmPerHour, 12.0);
   Set(snapshot.environment.interference.maximumFactorPercent, 30.0);
   snapshot.environment.interference.observedLinkCount = 1;

   nrm::EndpointSnapshot source;
   source.endpointId = "source";
   source.networkType = nrm::NetworkType::cCDL;
   nrm::EndpointSnapshot destination = source;
   destination.endpointId = "destination";
   snapshot.endpoints = {source, destination};
   nrm::LinkSnapshot link;
   link.sourceEndpointId = "source";
   link.destinationEndpointId = "destination";
   link.networkType = nrm::NetworkType::cCDL;
   Set(link.terrainBlockedFlag, 0.0);
   snapshot.links.push_back(link);
   return snapshot;
}
} // namespace

int main()
{
   const nrm::EnvironmentConfigRepository config =
      nrm::EnvironmentConfigRepository::BuiltInDemo();
   const nrm::BuiltInEnvironmentEffectAdapter adapter(config);
   const nrm::CapabilityRequest request;
   const std::vector<std::string> route{"source", "destination"};
   nrm::EnvironmentContext context;
   context.valid = true;
   context.sampleTime = 10.0;

   nrm::ResourceSnapshot snapshot = Snapshot();
   nrm::EnvironmentEffect current = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, snapshot, request, route, context);
   assert(current.valid);
   assert(current.capacityScale.value == 1.0);
   assert(current.origin == nrm::DataOrigin::cAFSIM_INTERNAL);

   context.applyParameterizedEffects = true;
   nrm::EnvironmentEffect candidate = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, snapshot, request, route, context);
   assert(candidate.valid);
   assert(candidate.capacityScale.value == 0.75);
   assert(candidate.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.confidence == nrm::Confidence::cLOW);

   snapshot.links[0].terrainBlockedFlag.value = 1.0;
   nrm::EnvironmentEffect terrain = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, snapshot, request, route, context);
   assert(terrain.valid);
   assert(terrain.hardBlocked);
   assert(terrain.reason == nrm::CapabilityReason::cENVIRONMENT_HARD_BLOCKED);
   return 0;
}
