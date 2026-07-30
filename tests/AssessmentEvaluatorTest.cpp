/**
 * @file AssessmentEvaluatorTest.cpp
 * @brief Unit coverage for current-graph assessment, margins, and reason codes.
 */

#include "nrm/AssessmentEvaluator.hpp"

#include <algorithm>
#include <cassert>

namespace
{
nrm::EndpointSnapshot Endpoint(const char* aId, const char* aPlatform)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId   = aId;
   endpoint.platformName = aPlatform;
   endpoint.state        = nrm::ResourceState::cONLINE;
   endpoint.networkType  = nrm::NetworkType::cLINK16;
   endpoint.networkName  = "nrm_link16_test";
   return endpoint;
}

void SetPosition(nrm::EndpointSnapshot& aEndpoint, double aLatitude, double aLongitude)
{
   aEndpoint.latitudeDeg.value = aLatitude;
   aEndpoint.latitudeDeg.valid = true;
   aEndpoint.longitudeDeg.value = aLongitude;
   aEndpoint.longitudeDeg.valid = true;
   aEndpoint.altitudeM.value = 1000.0;
   aEndpoint.altitudeM.valid = true;
}

nrm::LinkSnapshot Link(const char* aId,
                       const char* aSourceId,
                       const char* aDestinationId,
                       const char* aSourcePlatform,
                       const char* aDestinationPlatform,
                       double      aDelayMs,
                       double      aPdrPercent,
                       double      aBandwidthBps)
{
   nrm::LinkSnapshot link;
   link.linkId                = aId;
   link.sourceEndpointId      = aSourceId;
   link.destinationEndpointId = aDestinationId;
   link.sourcePlatform        = aSourcePlatform;
   link.destinationPlatform   = aDestinationPlatform;
   link.networkName           = "nrm_link16_test";
   link.networkType           = nrm::NetworkType::cLINK16;
   link.state                 = nrm::ResourceState::cONLINE;
   link.bandwidthBps.value    = aBandwidthBps;
   link.bandwidthBps.valid    = true;

   nrm::WindowMetrics window;
   window.windowS                       = 10.0;
   window.averageTransportDelayMs.value = aDelayMs;
   window.averageTransportDelayMs.valid = true;
   window.pdrPercent.value              = aPdrPercent;
   window.pdrPercent.valid              = true;
   link.windows.push_back(window);
   return link;
}

bool HasReason(const nrm::AssessmentResult& aResult, nrm::AssessmentReason aReason)
{
   return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) != aResult.reasons.end();
}
} // namespace

int main()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 42;
   snapshot.simTime         = 20.0;
   snapshot.endpoints.push_back(Endpoint("A", "source"));
   snapshot.endpoints.push_back(Endpoint("B", "relay"));
   snapshot.endpoints.push_back(Endpoint("C", "destination"));
   snapshot.endpoints.push_back(Endpoint("D", "backup_relay"));
   for (std::size_t index = 0; index < snapshot.endpoints.size(); ++index)
   {
      SetPosition(snapshot.endpoints[index], 35.0 + 0.05 * index, 120.0 + 0.05 * index);
   }
   snapshot.links.push_back(Link("A->B", "A", "B", "source", "relay", 20.0, 95.0, 1000.0));
   snapshot.links.push_back(Link("B->C", "B", "C", "relay", "destination", 20.0, 95.0, 800.0));
   snapshot.links.push_back(
      Link("A->D", "A", "D", "source", "backup_relay", 30.0, 94.0, 900.0));
   snapshot.links.push_back(
      Link("D->C", "D", "C", "backup_relay", "destination", 30.0, 94.0, 850.0));

   nrm::AssessmentTask task;
   task.taskId                  = "TASK-OK";
   task.sourcePlatform          = "source";
   task.destinationPlatform     = "destination";
   task.requiredBandwidthBps    = 700.0;
   task.maximumDelayMs          = 50.0;
   task.minimumPdrPercent       = 90.0;
   task.allowedNetworks.push_back(nrm::NetworkType::cLINK16);

   nrm::AssessmentEvaluator evaluator;
   const nrm::AssessmentResult passed = evaluator.Evaluate(snapshot, task);
   assert(passed.reachable);
   assert(passed.canEstablish);
   assert(passed.canComplete);
   assert(passed.stable);
   assert(passed.primaryRoute.size() == 3);
   assert(passed.backupRoute.size() == 3);
   assert(passed.backupRoute[1] == "backup_relay");
   assert(!passed.backupRouteUsesCandidate);
   assert(passed.predictedDelayMs.valid && passed.predictedDelayMs.value == 40.0);
   assert(passed.estimatedPdrPercent.valid);
   assert(passed.estimatedPdrPercent.value > 90.2 && passed.estimatedPdrPercent.value < 90.3);
   assert(passed.bottleneckBandwidthBps.valid && passed.bottleneckBandwidthBps.value == 800.0);
   assert(passed.bandwidthMarginBps.value == 100.0);
   assert(passed.delayMarginMs.value == 10.0);
   assert(passed.reliabilityMarginPercent.value > 0.2);
   assert(passed.reasons.empty());

   task.taskId               = "TASK-BANDWIDTH";
   task.requiredBandwidthBps = 900.0;
   const nrm::AssessmentResult bandwidthFailed = evaluator.Evaluate(snapshot, task);
   assert(bandwidthFailed.reachable);
   assert(!bandwidthFailed.canComplete);
   assert(HasReason(bandwidthFailed, nrm::AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE));
   assert(bandwidthFailed.bandwidthMarginBps.value == -100.0);

   task.allowedNetworks.clear();
   task.allowedNetworks.push_back(nrm::NetworkType::cCDL);
   const nrm::AssessmentResult noPath = evaluator.Evaluate(snapshot, task);
   assert(!noPath.reachable);
   assert(HasReason(noPath, nrm::AssessmentReason::cNO_CURRENT_PATH));

   task.destinationPlatform = "missing";
   const nrm::AssessmentResult missing = evaluator.Evaluate(snapshot, task);
   assert(HasReason(missing, nrm::AssessmentReason::cNODE_NOT_FOUND));

   nrm::ResourceSnapshot candidateSnapshot;
   candidateSnapshot.snapshotVersion = 43;
   nrm::EndpointSnapshot candidateSource = Endpoint("X", "candidate_source");
   nrm::EndpointSnapshot candidateDestination = Endpoint("Y", "candidate_destination");
   SetPosition(candidateSource, 35.0, 120.0);
   SetPosition(candidateDestination, 35.2, 120.2);
   candidateSnapshot.endpoints.push_back(candidateSource);
   candidateSnapshot.endpoints.push_back(candidateDestination);

   nrm::AssessmentTask candidateTask;
   candidateTask.taskId = "TASK-CANDIDATE";
   candidateTask.sourcePlatform = "candidate_source";
   candidateTask.destinationPlatform = "candidate_destination";
   candidateTask.maximumDelayMs = 100.0;
   candidateTask.minimumPdrPercent = 90.0;
   candidateTask.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   const nrm::AssessmentResult candidate = evaluator.Evaluate(candidateSnapshot, candidateTask);
   assert(!candidate.reachable);
   assert(candidate.canEstablish);
   assert(candidate.canComplete);
   assert(!candidate.stable);
   assert(candidate.primaryRouteUsesCandidate);
   assert(candidate.primaryRoute.size() == 2);
   assert(candidate.predictedDelayMs.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.predictedDelayMs.confidence == nrm::Confidence::cLOW);
   assert(candidate.delayMarginMs.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(HasReason(candidate, nrm::AssessmentReason::cNO_CURRENT_PATH));
   assert(!HasReason(candidate, nrm::AssessmentReason::cLINK_NOT_ESTABLISHABLE));

   nrm::ResourceSnapshot differentNetworkSnapshot = candidateSnapshot;
   differentNetworkSnapshot.snapshotVersion = 44;
   differentNetworkSnapshot.endpoints[1].networkName = "another_link16_network";
   const nrm::AssessmentResult differentNetwork =
      evaluator.Evaluate(differentNetworkSnapshot, candidateTask);
   assert(!differentNetwork.reachable);
   assert(!differentNetwork.canEstablish);
   assert(HasReason(differentNetwork, nrm::AssessmentReason::cNO_CURRENT_PATH));
   assert(HasReason(differentNetwork, nrm::AssessmentReason::cLINK_NOT_ESTABLISHABLE));
   return 0;
}
