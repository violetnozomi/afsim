#include "nrm/CommunicationCapabilityService.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

namespace
{
nrm::EndpointSnapshot Endpoint(const char* aId,
                               const char* aPlatform,
                               double aLatitude,
                               double aLongitude)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = aId;
   endpoint.platformName = aPlatform;
   endpoint.networkName = "capability_link16";
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = nrm::ResourceState::cONLINE;
   endpoint.latitudeDeg.value = aLatitude;
   endpoint.latitudeDeg.valid = true;
   endpoint.longitudeDeg.value = aLongitude;
   endpoint.longitudeDeg.valid = true;
   endpoint.altitudeM.value = 1000.0;
   endpoint.altitudeM.valid = true;
   return endpoint;
}

nrm::LinkSnapshot Link(const char* aId,
                       const char* aSourceId,
                       const char* aDestinationId,
                       const char* aSourcePlatform,
                       const char* aDestinationPlatform,
                       double aDistanceM,
                       double aBandwidthBps,
                       double aDelayMs,
                       double aDeliveryRatioPercent,
                       double aDeliveredThroughputBps)
{
   nrm::LinkSnapshot link;
   link.linkId = aId;
   link.sourceEndpointId = aSourceId;
   link.destinationEndpointId = aDestinationId;
   link.sourcePlatform = aSourcePlatform;
   link.destinationPlatform = aDestinationPlatform;
   link.networkName = "capability_link16";
   link.networkType = nrm::NetworkType::cLINK16;
   link.state = nrm::ResourceState::cONLINE;
   link.distanceM.value = aDistanceM;
   link.distanceM.valid = true;
   link.distanceM.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   link.distanceM.confidence = nrm::Confidence::cHIGH;
   link.bandwidthBps.value = aBandwidthBps;
   link.bandwidthBps.valid = true;
   link.bandwidthBps.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   link.bandwidthBps.confidence = nrm::Confidence::cHIGH;

   nrm::WindowMetrics window;
   window.windowS = 10.0;
   window.averageTransportDelayMs.value = aDelayMs;
   window.averageTransportDelayMs.valid = true;
   window.averageTransportDelayMs.origin = nrm::DataOrigin::cDERIVED;
   window.averageTransportDelayMs.confidence = nrm::Confidence::cHIGH;
   window.deliveryRatioPercent.value = aDeliveryRatioPercent;
   window.deliveryRatioPercent.valid = true;
   window.deliveryRatioPercent.origin = nrm::DataOrigin::cDERIVED;
   window.deliveryRatioPercent.confidence = nrm::Confidence::cHIGH;
   window.deliveredThroughputBps.value = aDeliveredThroughputBps;
   window.deliveredThroughputBps.valid = true;
   window.deliveredThroughputBps.origin = nrm::DataOrigin::cDERIVED;
   window.deliveredThroughputBps.confidence = nrm::Confidence::cHIGH;
   link.windows.push_back(window);
   return link;
}

nrm::ResourceSnapshot CurrentSnapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 80;
   snapshot.simTime = 25.0;
   snapshot.configVersion = "capability-test-v1";
   snapshot.endpoints.push_back(Endpoint("A", "source", 35.0, 120.0));
   snapshot.endpoints.push_back(Endpoint("B", "relay", 35.01, 120.01));
   snapshot.endpoints.push_back(Endpoint("C", "destination", 35.02, 120.02));
   snapshot.links.push_back(
      Link("A-B", "A", "B", "source", "relay", 1000.0, 1000.0, 10.0, 90.0, 600.0));
   snapshot.links.push_back(
      Link("B-C", "B", "C", "relay", "destination", 2000.0, 800.0, 20.0, 80.0, 500.0));

   nrm::NetworkSnapshot network;
   network.networkName = "capability_link16";
   network.networkType = nrm::NetworkType::cLINK16;
   network.endpointCount = 3;
   network.onlineCount = 3;
   snapshot.networks.push_back(network);
   return snapshot;
}

nrm::CapabilityRequest Request()
{
   nrm::CapabilityRequest request;
   request.requestId = "CAP-TEST";
   request.sourcePlatform = "source";
   request.destinationPlatform = "destination";
   request.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   return request;
}

bool HasReason(const nrm::CapabilityResult& aResult, nrm::CapabilityReason aReason)
{
   return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) !=
          aResult.reasons.end();
}

class TestEnvironmentAdapter : public nrm::EnvironmentEffectAdapter
{
public:
   nrm::EnvironmentEffect Evaluate(
      nrm::EnvironmentDomain aDomain,
      const nrm::ResourceSnapshot&,
      const nrm::CapabilityRequest&,
      const std::vector<std::string>& aEndpointRoute,
      const nrm::EnvironmentContext& aContext) const override
   {
      ++callCount;
      lastEndpointRoute = aEndpointRoute;

      nrm::EnvironmentEffect effect;
      effect.domain = aDomain;
      effect.valid = true;
      effect.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
      effect.confidence = nrm::Confidence::cMEDIUM;
      effect.reason = nrm::CapabilityReason::cNONE;
      effect.providerId = aContext.providerId;
      effect.effectId = "TEST-EFFECT";
      effect.sampleTime = aContext.sampleTime;
      lastApplicationMode = aContext.applicationMode;
      lastApplyParameterizedEffects = aContext.applyParameterizedEffects;
      SetMetric(effect.pathLossDeltaDb,
                aContext.applyParameterizedEffects ? 3.0 : 0.0);
      SetMetric(effect.capacityScale,
                aContext.applyParameterizedEffects ? 0.8 : 1.0);
      SetMetric(effect.packetLossDeltaPercent,
                aContext.applyParameterizedEffects ? 2.0 : 0.0);
      SetMetric(effect.delayDeltaMs,
                aContext.applyParameterizedEffects ? 1.5 : 0.0);
      return effect;
   }

   mutable std::size_t callCount = 0;
   mutable std::vector<std::string> lastEndpointRoute;
   mutable nrm::EnvironmentApplicationMode lastApplicationMode =
      nrm::EnvironmentApplicationMode::cINFORMATION_ONLY;
   mutable bool lastApplyParameterizedEffects = false;

private:
   static void SetMetric(nrm::MetricValue<double>& aMetric, double aValue)
   {
      aMetric.value = aValue;
      aMetric.valid = true;
      aMetric.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
      aMetric.confidence = nrm::Confidence::cMEDIUM;
   }
};
} // namespace

int main()
{
   const nrm::CommunicationCapabilityService service;
   const nrm::CapabilityRequest request = Request();
   nrm::ResourceSnapshot snapshot = CurrentSnapshot();

   const std::uint64_t versionBefore = snapshot.snapshotVersion;
   const std::size_t endpointCountBefore = snapshot.endpoints.size();
   const std::size_t linkCountBefore = snapshot.links.size();
   const double bandwidthBefore = snapshot.links[0].bandwidthBps.value;
   const nrm::CapabilityResult current = service.Query(snapshot, request);
   assert(current.requestValid);
   assert(current.pathAvailable);
   assert(!current.usesCandidate);
   assert(current.endpointRoute.size() == 3);
   assert(current.communicationDistanceM.valid);
   assert(current.communicationDistanceM.value == 3000.0);
   assert(current.communicationDistanceM.confidence == nrm::Confidence::cHIGH);
   assert(current.maximumHopDistanceM.valid);
   assert(current.maximumHopDistanceM.value == 2000.0);
   assert(current.transmissionRateBps.valid);
   assert(current.transmissionRateBps.value == 800.0);
   assert(current.transmissionRateBps.confidence == nrm::Confidence::cHIGH);
   assert(current.packetLossPercent.valid);
   assert(std::abs(current.packetLossPercent.value - 28.0) < 0.001);
   assert(current.transmissionDelayMs.valid);
   assert(current.transmissionDelayMs.value == 30.0);
   assert(current.transmissionDelayMs.confidence == nrm::Confidence::cHIGH);
   assert(current.networkThroughputBps.valid);
   assert(current.networkThroughputBps.value == 500.0);
   assert(current.accessRatioPercent.valid);
   assert(current.accessRatioPercent.value == 100.0);
   assert(current.environmentEffects.size() == 4);
   for (const nrm::EnvironmentEffect& effect : current.environmentEffects)
   {
      assert(!effect.valid);
      assert(effect.reason == nrm::CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE);
   }
   assert(HasReason(current, nrm::CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE));
   assert(snapshot.snapshotVersion == versionBefore);
   assert(snapshot.endpoints.size() == endpointCountBefore);
   assert(snapshot.links.size() == linkCountBefore);
   assert(snapshot.links[0].bandwidthBps.value == bandwidthBefore);

   nrm::ResourceSnapshot candidateSnapshot;
   candidateSnapshot.snapshotVersion = 81;
   candidateSnapshot.simTime = 30.0;
   candidateSnapshot.endpoints.push_back(Endpoint("X", "candidate_source", 35.0, 120.0));
   candidateSnapshot.endpoints.push_back(
      Endpoint("Y", "candidate_destination", 35.1, 120.1));
   nrm::NetworkSnapshot candidateNetwork;
   candidateNetwork.networkName = "capability_link16";
   candidateNetwork.networkType = nrm::NetworkType::cLINK16;
   candidateNetwork.endpointCount = 2;
   candidateNetwork.onlineCount = 2;
   candidateSnapshot.networks.push_back(candidateNetwork);
   nrm::CapabilityRequest candidateRequest = request;
   candidateRequest.sourcePlatform = "candidate_source";
   candidateRequest.destinationPlatform = "candidate_destination";
   const nrm::CapabilityResult candidate = service.Query(candidateSnapshot, candidateRequest);
   assert(candidate.requestValid);
   assert(candidate.pathAvailable);
   assert(candidate.usesCandidate);
   assert(candidate.transmissionRateBps.valid);
   assert(candidate.transmissionRateBps.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.transmissionRateBps.confidence == nrm::Confidence::cLOW);
   assert(candidate.packetLossPercent.valid);
   assert(candidate.packetLossPercent.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.packetLossPercent.confidence == nrm::Confidence::cLOW);
   assert(candidate.transmissionDelayMs.valid);
   assert(candidate.transmissionDelayMs.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.transmissionDelayMs.confidence == nrm::Confidence::cLOW);
   assert(!candidate.networkThroughputBps.valid);
   assert(HasReason(candidate, nrm::CapabilityReason::cPARAMETERIZED_CANDIDATE));
   assert(HasReason(candidate, nrm::CapabilityReason::cTHROUGHPUT_NOT_OBSERVED));

   nrm::CapabilityRequest noPathRequest = request;
   noPathRequest.allowedNetworks.clear();
   noPathRequest.allowedNetworks.push_back(nrm::NetworkType::cCDL);
   const nrm::CapabilityResult noPath = service.Query(snapshot, noPathRequest);
   assert(noPath.requestValid);
   assert(!noPath.pathAvailable);
   assert(HasReason(noPath, nrm::CapabilityReason::cNO_PATH));
   assert(!noPath.communicationDistanceM.valid);
   assert(noPath.communicationDistanceM.reason == nrm::MetricReason::cNO_PATH);

   nrm::ResourceSnapshot offlineSnapshot = snapshot;
   offlineSnapshot.endpoints[2].state = nrm::ResourceState::cOFFLINE;
   const nrm::CapabilityResult offline = service.Query(offlineSnapshot, request);
   assert(!offline.pathAvailable);
   assert(HasReason(offline, nrm::CapabilityReason::cNODE_OFFLINE));

   nrm::ResourceSnapshot zeroMemberSnapshot = snapshot;
   zeroMemberSnapshot.networks[0].endpointCount = 0;
   zeroMemberSnapshot.networks[0].onlineCount = 0;
   const nrm::CapabilityResult zeroMember = service.Query(zeroMemberSnapshot, request);
   assert(zeroMember.pathAvailable);
   assert(!zeroMember.accessRatioPercent.valid);
   assert(zeroMember.accessRatioPercent.reason == nrm::MetricReason::cZERO_DENOMINATOR);
   assert(HasReason(zeroMember, nrm::CapabilityReason::cACCESS_DENOMINATOR_ZERO));

   nrm::CapabilityRequest nanRequest = request;
   nanRequest.maximumDelayMs = std::numeric_limits<double>::quiet_NaN();
   const nrm::CapabilityResult nanResult = service.Query(snapshot, nanRequest);
   assert(!nanResult.requestValid);
   assert(HasReason(nanResult, nrm::CapabilityReason::cNON_FINITE_INPUT));

   nrm::CapabilityRequest infinityRequest = request;
   infinityRequest.requiredBandwidthBps = std::numeric_limits<double>::infinity();
   const nrm::CapabilityResult infinityResult = service.Query(snapshot, infinityRequest);
   assert(!infinityResult.requestValid);
   assert(HasReason(infinityResult, nrm::CapabilityReason::cNON_FINITE_INPUT));

   nrm::ResourceSnapshot invalidTimeSnapshot = snapshot;
   invalidTimeSnapshot.simTime = std::numeric_limits<double>::infinity();
   const nrm::CapabilityResult invalidTime = service.Query(invalidTimeSnapshot, request);
   assert(!invalidTime.requestValid);
   assert(invalidTime.simTime == 0.0);
   assert(invalidTime.communicationDistanceM.sampleTime == 0.0);

   nrm::ResourceSnapshot noThroughputSnapshot = snapshot;
   for (nrm::LinkSnapshot& link : noThroughputSnapshot.links)
   {
      link.windows[0].deliveredThroughputBps.valid = false;
   }
   const nrm::CapabilityResult noThroughput =
      service.Query(noThroughputSnapshot, request);
   assert(noThroughput.pathAvailable);
   assert(!noThroughput.networkThroughputBps.valid);
   assert(noThroughput.networkThroughputBps.reason == nrm::MetricReason::cNO_SAMPLES);
   assert(HasReason(noThroughput, nrm::CapabilityReason::cTHROUGHPUT_NOT_OBSERVED));

   nrm::ResourceSnapshot noDelaySnapshot = snapshot;
   for (nrm::LinkSnapshot& link : noDelaySnapshot.links)
   {
      link.windows[0].averageTransportDelayMs.valid = false;
   }
   const nrm::CapabilityResult noDelay = service.Query(noDelaySnapshot, request);
   assert(noDelay.pathAvailable);
   assert(!noDelay.usesCandidate);
   assert(noDelay.endpointRoute.size() == 3);
   assert(!noDelay.transmissionDelayMs.valid);
   assert(noDelay.communicationDistanceM.valid);
   assert(noDelay.transmissionRateBps.valid);

   nrm::ResourceSnapshot lowConfidenceSnapshot = snapshot;
   for (nrm::LinkSnapshot& link : lowConfidenceSnapshot.links)
   {
      link.distanceM.confidence = nrm::Confidence::cLOW;
      link.bandwidthBps.confidence = nrm::Confidence::cLOW;
      link.windows[0].averageTransportDelayMs.confidence = nrm::Confidence::cLOW;
   }
   const nrm::CapabilityResult lowConfidence =
      service.Query(lowConfidenceSnapshot, request);
   assert(lowConfidence.pathAvailable);
   assert(lowConfidence.communicationDistanceM.confidence == nrm::Confidence::cLOW);
   assert(lowConfidence.maximumHopDistanceM.confidence == nrm::Confidence::cLOW);
   assert(lowConfidence.transmissionRateBps.confidence == nrm::Confidence::cLOW);
   assert(lowConfidence.transmissionDelayMs.confidence == nrm::Confidence::cLOW);

   TestEnvironmentAdapter environmentAdapter;
   const nrm::CommunicationCapabilityService environmentService(
      nrm::NetworkProfileRepository::BuiltInDemo(), &environmentAdapter);
   nrm::EnvironmentContext environmentContext;
   environmentContext.contextId = "ENV-CONTEXT";
   environmentContext.providerId = "TEST-PROVIDER";
   environmentContext.sampleTime = 24.0;
   environmentContext.valid = true;
   const nrm::CapabilityResult withEnvironment =
      environmentService.Query(snapshot, request, environmentContext);
   assert(environmentAdapter.callCount == 4);
   assert(environmentAdapter.lastEndpointRoute.size() == 3);
   assert(withEnvironment.environmentEffects.size() == 4);
   assert(!HasReason(withEnvironment,
                     nrm::CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE));
   for (const nrm::EnvironmentEffect& effect : withEnvironment.environmentEffects)
   {
      assert(effect.valid);
      assert(effect.reason == nrm::CapabilityReason::cNONE);
      assert(effect.providerId == "TEST-PROVIDER");
      assert(effect.pathLossDeltaDb.valid);
      assert(effect.pathLossDeltaDb.unit == "dB");
      assert(effect.capacityScale.valid);
      assert(effect.capacityScale.unit == "ratio");
      assert(effect.packetLossDeltaPercent.valid);
      assert(effect.packetLossDeltaPercent.unit == "percentage_point");
      assert(effect.delayDeltaMs.valid);
      assert(effect.delayDeltaMs.unit == "ms");
   }
   // Current AFSIM paths only carry environment evidence. Their RF-derived
   // metrics must not be degraded a second time.
   assert(withEnvironment.transmissionRateBps.value == current.transmissionRateBps.value);
   assert(withEnvironment.packetLossPercent.value == current.packetLossPercent.value);
   assert(withEnvironment.transmissionDelayMs.value == current.transmissionDelayMs.value);

   const nrm::CapabilityResult candidateWithEnvironment =
      environmentService.Query(candidateSnapshot, candidateRequest, environmentContext);
   assert(candidateWithEnvironment.usesCandidate);
   assert(candidateWithEnvironment.transmissionRateBps.valid);
   assert(candidateWithEnvironment.transmissionRateBps.value <
          candidate.transmissionRateBps.value);
   assert(candidateWithEnvironment.packetLossPercent.value >
          candidate.packetLossPercent.value);
   assert(candidateWithEnvironment.transmissionDelayMs.value >
          candidate.transmissionDelayMs.value);

   nrm::EnvironmentContext informationOnly = environmentContext;
   informationOnly.schemaVersion = "nrm.customer.environment_report.v1";
   informationOnly.applicationMode =
      nrm::EnvironmentApplicationMode::cINFORMATION_ONLY;
   informationOnly.applyParameterizedEffects = false;
   const nrm::CapabilityResult informationOnlyCandidate =
      environmentService.Query(candidateSnapshot, candidateRequest,
                               informationOnly);
   assert(informationOnlyCandidate.usesCandidate);
   assert(!environmentAdapter.lastApplyParameterizedEffects);
   assert(environmentAdapter.lastApplicationMode ==
          nrm::EnvironmentApplicationMode::cINFORMATION_ONLY);
   assert(informationOnlyCandidate.transmissionRateBps.value ==
          candidate.transmissionRateBps.value);

   const std::size_t callsBeforeInvalidRequest = environmentAdapter.callCount;
   nrm::CapabilityRequest invalidEnvironmentRequest = request;
   invalidEnvironmentRequest.maximumDelayMs =
      std::numeric_limits<double>::quiet_NaN();
   const nrm::CapabilityResult invalidEnvironment = environmentService.Query(
      snapshot, invalidEnvironmentRequest, environmentContext);
   assert(!invalidEnvironment.requestValid);
   assert(environmentAdapter.callCount == callsBeforeInvalidRequest);
   return 0;
}
