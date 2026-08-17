/**
 * @file AssessmentEvaluatorTest.cpp
 * @brief Unit coverage for current-graph assessment, margins, and reason codes.
 */

#include "nrm/AssessmentEvaluator.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

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
      const double offset = 0.05 * static_cast<double>(index);
      SetPosition(snapshot.endpoints[index], 35.0 + offset, 120.0 + offset);
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
   assert(passed.backupRoute.size() == 2);
   assert(passed.backupRouteUsesCandidate);
   assert(passed.predictedDelayMs.valid && passed.predictedDelayMs.value == 40.0);
   assert(passed.estimatedPdrPercent.valid);
   assert(passed.estimatedPdrPercent.value > 90.2 && passed.estimatedPdrPercent.value < 90.3);
   assert(passed.bottleneckBandwidthBps.valid && passed.bottleneckBandwidthBps.value == 800.0);
   assert(passed.bandwidthMarginBps.value == 100.0);
   assert(passed.delayMarginMs.value == 10.0);
   assert(passed.reliabilityMarginPercent.value > 0.2);
   assert(passed.reasons.empty());

   nrm::AssessmentTask relaxedBackupTask = task;
   relaxedBackupTask.maximumDelayMs = 70.0;
   relaxedBackupTask.minimumPdrPercent = 85.0;
   const nrm::AssessmentResult currentBackup = evaluator.Evaluate(snapshot, relaxedBackupTask);
   assert(currentBackup.backupRoute.size() == 3);
   assert(currentBackup.backupRoute[1] == "backup_relay");
   assert(!currentBackup.backupRouteUsesCandidate);

   nrm::ResourceSnapshot candidateBackupSnapshot = snapshot;
   candidateBackupSnapshot.links.resize(2);
   const nrm::AssessmentResult candidateBackup =
      evaluator.Evaluate(candidateBackupSnapshot, task);
   assert(candidateBackup.canComplete);
   assert(!candidateBackup.primaryRouteUsesCandidate);
   assert(candidateBackup.backupRouteUsesCandidate);
   assert(!candidateBackup.backupRoute.empty());

   nrm::ResourceSnapshot infeasibleBackupSnapshot = candidateBackupSnapshot;
   for (nrm::LinkSnapshot& link : infeasibleBackupSnapshot.links)
   {
      link.windows[0].pdrPercent.value = 100.0;
   }
   nrm::AssessmentTask strictBackupTask = task;
   strictBackupTask.minimumPdrPercent = 99.0;
   const nrm::AssessmentResult infeasibleBackup =
      evaluator.Evaluate(infeasibleBackupSnapshot, strictBackupTask);
   assert(infeasibleBackup.canComplete);
   assert(infeasibleBackup.backupRoute.empty());

   nrm::AssessmentTask invalidConstraintTask = task;
   invalidConstraintTask.maximumDelayMs = std::numeric_limits<double>::quiet_NaN();
   const nrm::AssessmentResult invalidConstraint =
      evaluator.Evaluate(snapshot, invalidConstraintTask);
   assert(HasReason(invalidConstraint, nrm::AssessmentReason::cDATA_INVALID));
   assert(invalidConstraint.failedConstraints[0] == "PATH_CONSTRAINTS_INVALID");

   nrm::ResourceSnapshot profileCapacitySnapshot = snapshot;
   for (nrm::LinkSnapshot& link : profileCapacitySnapshot.links)
   {
      link.bandwidthBps.valid = false;
   }
   const nrm::AssessmentResult profileCapacity =
      evaluator.Evaluate(profileCapacitySnapshot, task);
   assert(profileCapacity.canComplete);
   assert(profileCapacity.bottleneckBandwidthBps.value == 238000.0);
   assert(profileCapacity.bottleneckBandwidthBps.origin ==
          nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(profileCapacity.bottleneckBandwidthBps.confidence == nrm::Confidence::cLOW);
   assert(!profileCapacity.profileIds.empty());

   task.taskId               = "TASK-BANDWIDTH";
   task.requiredBandwidthBps = 900.0;
   nrm::ResourceSnapshot noCandidateMetrics = snapshot;
   for (nrm::EndpointSnapshot& endpoint : noCandidateMetrics.endpoints)
   {
      endpoint.latitudeDeg.valid = false;
      endpoint.longitudeDeg.valid = false;
   }
   const nrm::AssessmentResult bandwidthFailed =
      evaluator.Evaluate(noCandidateMetrics, task);
   assert(bandwidthFailed.reachable);
   assert(!bandwidthFailed.canComplete);
   assert(HasReason(bandwidthFailed, nrm::AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE));
   assert(bandwidthFailed.bandwidthMarginBps.value == -100.0);

   task.taskId = "TASK-FEASIBLE-ALTERNATIVE";
   task.requiredBandwidthBps = 825.0;
   task.maximumDelayMs = 70.0;
   task.minimumPdrPercent = 85.0;
   const nrm::AssessmentResult feasibleAlternative = evaluator.Evaluate(snapshot, task);
   assert(feasibleAlternative.canComplete);
   assert(!feasibleAlternative.primaryRouteUsesCandidate);
   assert(feasibleAlternative.primaryRoute[1] == "backup_relay");
   assert(feasibleAlternative.predictedDelayMs.value == 60.0);
   assert(feasibleAlternative.selectedPathRank == 2);

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

   nrm::ResourceSnapshot loadedCandidate = candidateSnapshot;
   nrm::NetworkSnapshot loadedNetwork;
   loadedNetwork.networkName = "nrm_link16_test";
   loadedNetwork.networkType = nrm::NetworkType::cLINK16;
   nrm::WindowMetrics loadWindow;
   loadWindow.windowS = 10.0;
   loadWindow.offeredLoadBps.value = 50000.0;
   loadWindow.offeredLoadBps.valid = true;
   loadedNetwork.windows.push_back(loadWindow);
   loadedCandidate.networks.push_back(loadedNetwork);
   candidateTask.requiredBandwidthBps = 100000.0;
   const nrm::AssessmentResult lightLoad =
      evaluator.Evaluate(loadedCandidate, candidateTask);
   assert(lightLoad.canComplete);
   assert(lightLoad.bottleneckBandwidthBps.value == 188000.0);

   loadedCandidate.networks[0].windows[0].offeredLoadBps.value = 200000.0;
   const nrm::AssessmentResult congestion =
      evaluator.Evaluate(loadedCandidate, candidateTask);
   assert(!congestion.canComplete);
   assert(congestion.bottleneckBandwidthBps.value == 38000.0);
   assert(HasReason(congestion, nrm::AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE));
   return 0;
}
