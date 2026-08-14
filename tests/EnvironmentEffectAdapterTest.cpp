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
   source.platformName = "source-platform";
   source.networkType = nrm::NetworkType::cCDL;
   Set(source.latitudeDeg, 1.0);
   Set(source.longitudeDeg, 1.0);
   Set(source.altitudeM, 1000.0);
   nrm::EndpointSnapshot destination = source;
   destination.endpointId = "destination";
   destination.platformName = "destination-platform";
   destination.longitudeDeg.value = 2.0;
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

   snapshot.links[0].terrainBlockedFlag.value = 0.0;
   snapshot.operationalArea.areaId = "demo-area";
   snapshot.operationalArea.points = {{0.0, 0.0, 0.0}, {0.0, 3.0, 0.0},
                                      {3.0, 3.0, 0.0}, {3.0, 0.0, 0.0}};
   nrm::EnvironmentEffect inside = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, snapshot, request, route, context);
   assert(!inside.hardBlocked);
   snapshot.endpoints[1].longitudeDeg.value = 4.0;
   nrm::EnvironmentEffect outside = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, snapshot, request, route, context);
   assert(outside.hardBlocked);
   assert(outside.evidence.front() == "OPERATIONAL_AREA_OUTSIDE");
   snapshot.endpoints[1].longitudeDeg.value = 2.0;

   nrm::CapabilityRequest timedRequest;
   timedRequest.taskStartTime = 12.0;
   timedRequest.taskEndTime = 20.0;
   context.validFrom = 10.0;
   context.validUntil = 30.0;
   nrm::EnvironmentEffect coveredWeather = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, snapshot, timedRequest, route, context);
   assert(coveredWeather.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   timedRequest.taskStartTime = 5.0;
   nrm::EnvironmentEffect uncoveredWeather = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, snapshot, timedRequest, route, context);
   assert(uncoveredWeather.origin == nrm::DataOrigin::cAFSIM_INTERNAL);

   snapshot.environment.interference.bands.push_back(
      {"jammer-1", 1000000000.0, 2000000.0, -40.0, true});
   nrm::CapabilityRequest frequencyRequest;
   frequencyRequest.frequencyHz = 1000000000.0;
   frequencyRequest.occupiedBandwidthHz = 2000000.0;
   nrm::EnvironmentEffect overlapped = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE,
      snapshot, frequencyRequest, route, context);
   assert(overlapped.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   frequencyRequest.frequencyHz = 1010000000.0;
   nrm::EnvironmentEffect clear = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE,
      snapshot, frequencyRequest, route, context);
   assert(clear.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   return 0;
}
