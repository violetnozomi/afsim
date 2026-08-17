#include "nrm/BuiltInEnvironmentEffectAdapter.hpp"

#include <algorithm>
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
   link.linkId = "link-1";
   link.sourceEndpointId = "source";
   link.destinationEndpointId = "destination";
   link.networkType = nrm::NetworkType::cCDL;
   Set(link.terrainBlockedFlag, 0.0);
   snapshot.links.push_back(link);
   return snapshot;
}

bool HasEvidence(const nrm::EnvironmentEffect& aEffect,
                 const char* aEvidence)
{
   return std::find(aEffect.evidence.begin(), aEffect.evidence.end(),
                    aEvidence) != aEffect.evidence.end();
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
   context.applicationMode = nrm::EnvironmentApplicationMode::cINFORMATION_ONLY;

   nrm::ResourceSnapshot snapshot = Snapshot();
   nrm::EnvironmentEffect current = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, snapshot, request, route, context);
   assert(current.valid);
   assert(current.capacityScale.value == 1.0);
   assert(current.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   assert(current.evidence.front() == "AFSIM_ENVIRONMENT_INFORMATION_ONLY");

   context.applicationMode = nrm::EnvironmentApplicationMode::cALREADY_INCLUDED;
   nrm::EnvironmentEffect alreadyIncluded = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, snapshot, request, route, context);
   assert(alreadyIncluded.capacityScale.value == 1.0);
   assert(alreadyIncluded.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   assert(alreadyIncluded.evidence.front() ==
          "AFSIM_ENVIRONMENT_ALREADY_INCLUDED");

   // A declared blocked link is informational unless the customer explicitly
   // requests candidate adjustment.  The compatibility bool must not override
   // either non-applying mode.
   snapshot.environment.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   snapshot.environment.terrain.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   snapshot.environment.terrain.blockedLinkIds = {"link-1"};
   context.applicationMode = nrm::EnvironmentApplicationMode::cINFORMATION_ONLY;
   context.applyParameterizedEffects = true;
   nrm::EnvironmentEffect informationalTerrain = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, snapshot, request, route, context);
   assert(!informationalTerrain.hardBlocked);
   assert(informationalTerrain.capacityScale.value == 1.0);

   context.applicationMode = nrm::EnvironmentApplicationMode::cALREADY_INCLUDED;
   nrm::EnvironmentEffect includedTerrain = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, snapshot, request, route, context);
   assert(!includedTerrain.hardBlocked);
   assert(includedTerrain.capacityScale.value == 1.0);

   context.applicationMode = nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
   context.applyParameterizedEffects = true;
   nrm::EnvironmentEffect customerBlockedTerrain = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, snapshot, request, route, context);
   assert(customerBlockedTerrain.hardBlocked);
   assert(customerBlockedTerrain.evidence.front() ==
          "CUSTOMER_TERRAIN_BLOCKED");
   snapshot.environment.terrain.blockedLinkIds.clear();
   snapshot.environment.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
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
   snapshot.environment.terrain.blockedLinkIds.push_back("link-1");
   terrain = adapter.Evaluate(nrm::EnvironmentDomain::cTERRAIN, snapshot,
                              request, route, context);
   assert(terrain.hardBlocked);
   snapshot.environment.terrain.blockedLinkIds.clear();
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
   snapshot.environment.interference.affectedLinkIds = {"other-link"};
   nrm::EnvironmentEffect unaffected = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE,
      snapshot, frequencyRequest, route, context);
   assert(unaffected.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   snapshot.environment.interference.affectedLinkIds = {"link-1"};
   Set(snapshot.environment.interference.capacityScale, 0.61);
   nrm::EnvironmentEffect explicitScale = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE,
      snapshot, frequencyRequest, route, context);
   assert(explicitScale.capacityScale.value == 0.61);
   assert(explicitScale.origin ==
          snapshot.environment.interference.origin);
   frequencyRequest.frequencyHz = 1010000000.0;
   nrm::EnvironmentEffect clear = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE,
      snapshot, frequencyRequest, route, context);
   assert(clear.origin == nrm::DataOrigin::cAFSIM_INTERNAL);

   // A partial customer update owns only the supplied domain.  Its global
   // application mode must not adjust or block AFSIM-owned terrain or
   // interference data.
   nrm::ResourceSnapshot mixed = Snapshot();
   mixed.environment.weather.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   mixed.environment.weather.confidence = nrm::Confidence::cHIGH;
   mixed.environment.terrain.blockedLinkIds = {"link-1"};
   mixed.environment.interference.capacityScale.valid = true;
   mixed.environment.interference.capacityScale.value = 0.55;
   mixed.environment.interference.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   mixed.environment.interference.confidence = nrm::Confidence::cMEDIUM;
   nrm::EnvironmentContext partialCustomer;
   partialCustomer.valid = true;
   partialCustomer.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   partialCustomer.applicationMode =
      nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
   partialCustomer.applyParameterizedEffects = true;
   partialCustomer.customerProvidedDomains = {
      nrm::EnvironmentDomain::cWEATHER};

   const nrm::EnvironmentEffect mixedWeather = adapter.Evaluate(
      nrm::EnvironmentDomain::cWEATHER, mixed, request, route,
      partialCustomer);
   assert(mixedWeather.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(HasEvidence(mixedWeather, "PARAMETERIZED_CANDIDATE_EFFECT"));
   assert(HasEvidence(mixedWeather, "CUSTOMER_ENVIRONMENT_SOURCE"));

   const nrm::EnvironmentEffect mixedTerrain = adapter.Evaluate(
      nrm::EnvironmentDomain::cTERRAIN, mixed, request, route,
      partialCustomer);
   assert(!mixedTerrain.hardBlocked);
   assert(mixedTerrain.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   assert(mixedTerrain.evidence.front() ==
          "AFSIM_ENVIRONMENT_INFORMATION_ONLY");

   const nrm::EnvironmentEffect mixedInterference = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE, mixed,
      request, route, partialCustomer);
   assert(mixedInterference.capacityScale.value == 1.0);
   assert(mixedInterference.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   assert(mixedInterference.evidence.front() ==
          "AFSIM_ENVIRONMENT_INFORMATION_ONLY");

   partialCustomer.customerProvidedDomains = {
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE};
   mixed.environment.interference.origin =
      nrm::DataOrigin::cCUSTOMER_MODULE;
   mixed.environment.interference.confidence = nrm::Confidence::cHIGH;
   const nrm::EnvironmentEffect customerInterference = adapter.Evaluate(
      nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE, mixed,
      request, route, partialCustomer);
   assert(customerInterference.capacityScale.value == 0.55);
   assert(customerInterference.origin == nrm::DataOrigin::cCUSTOMER_MODULE);
   assert(customerInterference.confidence == nrm::Confidence::cHIGH);
   assert(HasEvidence(customerInterference, "CUSTOMER_ENVIRONMENT_SOURCE"));
   return 0;
}
