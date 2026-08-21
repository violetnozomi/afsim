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
   endpoint.platformId   = aPlatform;
   endpoint.platformName = aPlatform;
   endpoint.state        = nrm::ResourceState::cONLINE;
   endpoint.networkId    = "nrm_link16_test";
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
   link.networkId             = "nrm_link16_test";
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
   assert(passed.primaryRouteHops.size() == 2);
   const nrm::AssessmentRouteHop& firstHop = passed.primaryRouteHops[0];
   assert(firstHop.hopIndex == 1);
   assert(firstHop.kind == nrm::AssessmentRouteHopKind::cCURRENT_LINK);
   assert(firstHop.sourceEndpointId == "A");
   assert(firstHop.destinationEndpointId == "B");
   assert(firstHop.sourcePlatform == "source");
   assert(firstHop.destinationPlatform == "relay");
   assert(firstHop.sourceNetworkId == "nrm_link16_test");
   assert(firstHop.destinationNetworkId == "nrm_link16_test");
   assert(firstHop.sourceNetworkType == nrm::NetworkType::cLINK16);
   assert(firstHop.destinationNetworkType == nrm::NetworkType::cLINK16);
   assert(!firstHop.candidate);
   assert(!firstHop.gateway);
   assert(passed.primaryRouteHops[1].hopIndex == 2);
   assert(passed.primaryRouteHops[1].sourceEndpointId == "B");
   assert(passed.primaryRouteHops[1].destinationEndpointId == "C");
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

   nrm::ResourceSnapshot delayUnconstrainedSnapshot = snapshot;
   for (nrm::LinkSnapshot& link : delayUnconstrainedSnapshot.links)
      link.windows.front().averageTransportDelayMs.valid = false;
   for (nrm::EndpointSnapshot& endpoint : delayUnconstrainedSnapshot.endpoints)
   {
      endpoint.latitudeDeg.valid = false;
      endpoint.longitudeDeg.valid = false;
   }
   nrm::AssessmentTask delayUnconstrainedTask = task;
   delayUnconstrainedTask.maximumDelayMs = 0.0;
   delayUnconstrainedTask.requireObservedCurrentMetrics = true;
   // Legacy callers leave this compatibility flag at its default true.  A
   // zero maximum still means that delay is not a feasibility constraint.
   assert(delayUnconstrainedTask.requireDelayMetricForFeasibility);
   const nrm::AssessmentResult delayUnconstrained = evaluator.Evaluate(
      delayUnconstrainedSnapshot, delayUnconstrainedTask);
   assert(delayUnconstrained.canComplete);
   assert(!HasReason(delayUnconstrained, nrm::AssessmentReason::cDATA_INVALID));

   nrm::AssessmentTask relaxedBackupTask = task;
   relaxedBackupTask.maximumDelayMs = 70.0;
   relaxedBackupTask.minimumPdrPercent = 85.0;
   const nrm::AssessmentResult currentBackup = evaluator.Evaluate(snapshot, relaxedBackupTask);
   assert(currentBackup.backupRoute.size() == 3);
   assert(currentBackup.backupRoute[1] == "backup_relay");
   assert(!currentBackup.backupRouteUsesCandidate);
   assert(currentBackup.backupRouteHops.size() == 2);
   assert(currentBackup.backupRouteHops[0].hopIndex == 1);
   assert(currentBackup.backupRouteHops[1].hopIndex == 2);
   assert(currentBackup.backupRouteHops[0].destinationPlatform == "backup_relay");

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
   assert(candidate.primaryRouteHops.size() == 1);
   assert(candidate.primaryRouteHops[0].hopIndex == 1);
   assert(candidate.primaryRouteHops[0].kind ==
          nrm::AssessmentRouteHopKind::cCANDIDATE_LINK);
   assert(candidate.primaryRouteHops[0].candidate);
   assert(!candidate.primaryRouteHops[0].gateway);
   assert(candidate.predictedDelayMs.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.predictedDelayMs.confidence == nrm::Confidence::cLOW);
   assert(candidate.delayMarginMs.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(HasReason(candidate, nrm::AssessmentReason::cNO_CURRENT_PATH));
   assert(!HasReason(candidate, nrm::AssessmentReason::cLINK_NOT_ESTABLISHABLE));

   nrm::ResourceSnapshot mixedHopSnapshot;
   mixedHopSnapshot.snapshotVersion = 46;
   nrm::EndpointSnapshot mixedSource = Endpoint("MX", "mixed_source");
   nrm::EndpointSnapshot mixedRelay = Endpoint("MR", "mixed_relay");
   nrm::EndpointSnapshot mixedDestination = Endpoint("MY", "mixed_destination");
   SetPosition(mixedSource, 35.0, 120.0);
   SetPosition(mixedRelay, 35.0, 124.0);
   SetPosition(mixedDestination, 35.0, 128.0);
   mixedHopSnapshot.endpoints = {mixedSource, mixedRelay, mixedDestination};
   mixedHopSnapshot.links.push_back(Link(
      "MX->MR", "MX", "MR", "mixed_source", "mixed_relay",
      1.0, 100.0, 1000000.0));
   nrm::AssessmentTask mixedHopTask = candidateTask;
   mixedHopTask.taskId = "TASK-MIXED-HOPS";
   mixedHopTask.sourcePlatform = "mixed_source";
   mixedHopTask.destinationPlatform = "mixed_destination";
   const nrm::AssessmentResult mixedHops =
      evaluator.Evaluate(mixedHopSnapshot, mixedHopTask);
   assert(mixedHops.canComplete);
   assert(mixedHops.primaryRouteHops.size() == 2);
   assert(mixedHops.primaryRouteHops[0].kind ==
          nrm::AssessmentRouteHopKind::cCURRENT_LINK);
   assert(!mixedHops.primaryRouteHops[0].candidate);
   assert(mixedHops.primaryRouteHops[1].kind ==
          nrm::AssessmentRouteHopKind::cCANDIDATE_LINK);
   assert(mixedHops.primaryRouteHops[1].candidate);

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

   nrm::ResourceSnapshot gatewaySnapshot;
   gatewaySnapshot.snapshotVersion = 45;
   gatewaySnapshot.simTime = 50.0;
   nrm::EndpointSnapshot sourceL11 = Endpoint("S-L11", "gateway_source");
   sourceL11.platformId = "gateway_source";
   sourceL11.networkId = "net-l11";
   sourceL11.networkName = "net-l11";
   sourceL11.networkType = nrm::NetworkType::cLINK11;
   nrm::EndpointSnapshot gatewayAIngress = Endpoint("GA-L11", "gateway-a");
   gatewayAIngress.platformId = "gateway-a";
   gatewayAIngress.commName = "link11";
   gatewayAIngress.networkId = "net-l11";
   gatewayAIngress.networkName = "net-l11";
   gatewayAIngress.networkType = nrm::NetworkType::cLINK11;
   nrm::EndpointSnapshot gatewayAEgress = Endpoint("GA-SAT", "gateway-a");
   gatewayAEgress.platformId = "gateway-a";
   gatewayAEgress.commName = "satcom";
   gatewayAEgress.networkId = "net-satcom";
   gatewayAEgress.networkName = "net-satcom";
   gatewayAEgress.networkType = nrm::NetworkType::cSATCOM;
   nrm::EndpointSnapshot gatewayBIngress = Endpoint("GB-SAT", "gateway-b");
   gatewayBIngress.platformId = "gateway-b";
   gatewayBIngress.commName = "satcom";
   gatewayBIngress.networkId = "net-satcom";
   gatewayBIngress.networkName = "net-satcom";
   gatewayBIngress.networkType = nrm::NetworkType::cSATCOM;
   nrm::EndpointSnapshot gatewayBEgress = Endpoint("GB-CDL", "gateway-b");
   gatewayBEgress.platformId = "gateway-b";
   gatewayBEgress.commName = "cdl";
   gatewayBEgress.networkId = "net-cdl";
   gatewayBEgress.networkName = "net-cdl";
   gatewayBEgress.networkType = nrm::NetworkType::cCDL;
   nrm::EndpointSnapshot destinationCdl = Endpoint("D-CDL", "gateway_destination");
   destinationCdl.platformId = "gateway_destination";
   destinationCdl.networkId = "net-cdl";
   destinationCdl.networkName = "net-cdl";
   destinationCdl.networkType = nrm::NetworkType::cCDL;
   gatewaySnapshot.endpoints = {sourceL11, gatewayAIngress, gatewayAEgress,
                                gatewayBIngress, gatewayBEgress, destinationCdl};

   nrm::LinkSnapshot sourceToGateway = Link(
      "S-L11->GA-L11", "S-L11", "GA-L11", "gateway_source", "gateway-a",
      1.0, 100.0, 2000000.0);
   sourceToGateway.networkId = "net-l11";
   sourceToGateway.networkName = "net-l11";
   sourceToGateway.networkType = nrm::NetworkType::cLINK11;
   nrm::LinkSnapshot gatewayTransit = Link(
      "GA-SAT->GB-SAT", "GA-SAT", "GB-SAT", "gateway-a", "gateway-b",
      1.0, 100.0, 2000000.0);
   gatewayTransit.networkId = "net-satcom";
   gatewayTransit.networkName = "net-satcom";
   gatewayTransit.networkType = nrm::NetworkType::cSATCOM;
   nrm::LinkSnapshot gatewayToDestination = Link(
      "GB-CDL->D-CDL", "GB-CDL", "D-CDL", "gateway-b",
      "gateway_destination", 1.0, 100.0, 2000000.0);
   gatewayToDestination.networkId = "net-cdl";
   gatewayToDestination.networkName = "net-cdl";
   gatewayToDestination.networkType = nrm::NetworkType::cCDL;
   gatewaySnapshot.links = {sourceToGateway, gatewayTransit, gatewayToDestination};

   nrm::GatewayResourceState l11ToSatcom;
   l11ToSatcom.gatewayId = "cap-l11-satcom";
   l11ToSatcom.platformId = "gateway-a";
   l11ToSatcom.ingressNetworkId = "net-l11";
   l11ToSatcom.egressNetworkId = "net-satcom";
   l11ToSatcom.ingressCommName = "link11";
   l11ToSatcom.egressCommName = "satcom";
   l11ToSatcom.allowedSourcePlatformIds.push_back("gateway_source");
   l11ToSatcom.allowedDestinationPlatformIds.push_back("gateway_destination");
   l11ToSatcom.allowedMessageTypes.push_back("MISSION_REPORT");
   l11ToSatcom.processingDelayMs = 5.0;
   l11ToSatcom.forwardingRateBps = 1000000.0;
   l11ToSatcom.maxQueueMessages = 64;
   l11ToSatcom.maxQueueBits = 64000000;
   l11ToSatcom.forwardedCount = 100;
   l11ToSatcom.enabled = true;
   l11ToSatcom.valid = true;
   nrm::GatewayResourceState satcomToCdl = l11ToSatcom;
   satcomToCdl.gatewayId = "cap-satcom-cdl";
   satcomToCdl.platformId = "gateway-b";
   satcomToCdl.ingressNetworkId = "net-satcom";
   satcomToCdl.egressNetworkId = "net-cdl";
   satcomToCdl.ingressCommName = "satcom";
   satcomToCdl.egressCommName = "cdl";
   satcomToCdl.forwardingRateBps = 500000.0;
   gatewaySnapshot.gateways = {l11ToSatcom, satcomToCdl};

   nrm::GatewayRouteTemplate gatewayRoute;
   gatewayRoute.routeId = "route-l11-cdl-via-satcom";
   gatewayRoute.sourcePlatformId = "gateway_source";
   gatewayRoute.destinationPlatformId = "gateway_destination";
   gatewayRoute.destinationCommName = "cdl";
   gatewayRoute.allowedMessageTypes.push_back("MISSION_REPORT");
   gatewayRoute.gatewayCapabilityIds = {"cap-l11-satcom", "cap-satcom-cdl"};
   gatewayRoute.enabled = true;
   gatewayRoute.valid = true;
   gatewaySnapshot.gatewayRoutes.push_back(gatewayRoute);

   nrm::AssessmentTask gatewayTask;
   gatewayTask.taskId = "TASK-GATEWAY-CASCADE";
   gatewayTask.sourcePlatform = "gateway_source";
   gatewayTask.destinationPlatform = "gateway_destination";
   gatewayTask.businessType = "MISSION_REPORT";
   gatewayTask.requiredBandwidthBps = 400000.0;
   gatewayTask.maximumDelayMs = 20.0;
   gatewayTask.minimumPdrPercent = 99.0;
   gatewayTask.allowedNetworks = {nrm::NetworkType::cLINK11,
                                  nrm::NetworkType::cSATCOM,
                                  nrm::NetworkType::cCDL};
   const nrm::AssessmentResult gatewayAllowed =
      evaluator.Evaluate(gatewaySnapshot, gatewayTask);
   assert(gatewayAllowed.reachable);
   assert(gatewayAllowed.canComplete);
   assert(gatewayAllowed.gatewayRouteIds.size() == 1);
   assert(gatewayAllowed.gatewayRouteIds.front() == gatewayRoute.routeId);
   assert(gatewayAllowed.gatewayCapabilityIds.size() == 2);
   assert(gatewayAllowed.gatewayCapabilityIds[0] == "cap-l11-satcom");
   assert(gatewayAllowed.gatewayCapabilityIds[1] == "cap-satcom-cdl");
   assert(gatewayAllowed.primaryRouteHops.size() == 5);
   assert(gatewayAllowed.primaryRouteHops[0].kind ==
          nrm::AssessmentRouteHopKind::cCURRENT_LINK);
   const nrm::AssessmentRouteHop& firstTransition =
      gatewayAllowed.primaryRouteHops[1];
   assert(firstTransition.hopIndex == 2);
   assert(firstTransition.kind ==
          nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION);
   assert(firstTransition.sourceEndpointId == "GA-L11");
   assert(firstTransition.destinationEndpointId == "GA-SAT");
   assert(firstTransition.sourcePlatform == "gateway-a");
   assert(firstTransition.destinationPlatform == "gateway-a");
   assert(firstTransition.sourceNetworkType == nrm::NetworkType::cLINK11);
   assert(firstTransition.destinationNetworkType == nrm::NetworkType::cSATCOM);
   assert(!firstTransition.candidate);
   assert(firstTransition.gateway);
   assert(firstTransition.gatewayRouteId == "route-l11-cdl-via-satcom");
   assert(firstTransition.gatewayCapabilityId == "cap-l11-satcom");
   assert(gatewayAllowed.primaryRouteHops[2].kind ==
          nrm::AssessmentRouteHopKind::cCURRENT_LINK);
   const nrm::AssessmentRouteHop& secondTransition =
      gatewayAllowed.primaryRouteHops[3];
   assert(secondTransition.hopIndex == 4);
   assert(secondTransition.kind ==
          nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION);
   assert(secondTransition.sourceNetworkType == nrm::NetworkType::cSATCOM);
   assert(secondTransition.destinationNetworkType == nrm::NetworkType::cCDL);
   assert(secondTransition.gatewayRouteId == "route-l11-cdl-via-satcom");
   assert(secondTransition.gatewayCapabilityId == "cap-satcom-cdl");
   assert(gatewayAllowed.primaryRouteHops[4].kind ==
          nrm::AssessmentRouteHopKind::cCURRENT_LINK);
   assert(gatewayAllowed.predictedDelayMs.valid);
   assert(gatewayAllowed.predictedDelayMs.value == 13.0);
   assert(gatewayAllowed.bottleneckBandwidthBps.valid);
   assert(gatewayAllowed.bottleneckBandwidthBps.value == 500000.0);

   nrm::ResourceSnapshot gatewayWithoutRoute = gatewaySnapshot;
   gatewayWithoutRoute.gatewayRoutes.clear();
   const nrm::AssessmentResult gatewayDenied =
      evaluator.Evaluate(gatewayWithoutRoute, gatewayTask);
   assert(!gatewayDenied.reachable);
   assert(!gatewayDenied.canComplete);
   return 0;
}
