#include "NrmSnapshotReporter.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

std::string ReadAll(const std::string& aPath)
{
   std::ifstream input(aPath);
   std::stringstream buffer;
   buffer << input.rdbuf();
   return buffer.str();
}

int main()
{
   const std::string outputRoot =
      "/tmp/nrm-snapshot-reporter-test-" + std::to_string(getpid());
   std::string runDirectory;

   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 7;
   snapshot.simTime = 12.5;
   snapshot.configVersion = "test-config-v1";
   snapshot.messages.transmitted = 3;
   snapshot.operationalArea.areaId = "AREA-REPORT";
   snapshot.operationalArea.validFrom = 10.0;
   snapshot.operationalArea.validUntil = 20.0;
   snapshot.operationalArea.points.push_back({30.0, 120.0, 0.0});
   nrm::ResourceAlarm alarm;
   alarm.alarmId = "ALARM-REPORT";
   alarm.objectId = "link-report";
   alarm.reasonCode = "RESOURCE_OFFLINE";
   alarm.active = true;
   snapshot.alarms.push_back(alarm);
   snapshot.navigation.valid = true;
   snapshot.navigation.sampleTime = 12.5;
   nrm::NavigationSample navigation;
   navigation.platformId = "fighter-id";
   navigation.platformName = "fighter";
   navigation.rawStatus = "GPS1";
   navigation.mode = nrm::NavigationMode::cGPS_ACTIVE;
   navigation.statusCode = 1;
   navigation.valid = true;
   navigation.sampleTime = 12.5;
   navigation.totalPositionErrorM.value = 8.775;
   navigation.totalPositionErrorM.unit = "m";
   navigation.totalPositionErrorM.valid = true;
   snapshot.navigation.platforms.push_back(navigation);

   nrm::NetworkSnapshot network;
   network.networkId = "network-id-link16";
   network.networkName = "nrm_link16_test";
   network.networkType = nrm::NetworkType::cLINK16;
   network.endpointCount = 2;
   network.onlineCount = 2;
   nrm::WindowMetrics window;
   window.windowS = 10.0;
   window.deliveredThroughputBps.value = 1000.0;
   window.deliveredThroughputBps.valid = true;
   window.deliveredThroughputBps.unit = "bit/s";
   window.throughputBps = window.deliveredThroughputBps;
   window.transmittedBits = 10000;
   window.messages.routingFailed = 1;
   network.windows.push_back(window);
   snapshot.networks.push_back(network);

   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = "10.0.0.1";
   endpoint.platformName = "fighter";
   endpoint.commName = "link16";
   endpoint.networkId = network.networkId;
   endpoint.networkName = network.networkName;
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = nrm::ResourceState::cONLINE;
   snapshot.endpoints.push_back(endpoint);

   nrm::GatewayResourceState gateway;
   gateway.gatewayId = "gw-link11-satcom-forward";
   gateway.platformId = "gateway-link11-satcom";
   gateway.ingressNetworkId = "network-link11";
   gateway.egressNetworkId = "network-satcom";
   gateway.ingressCommName = "link11";
   gateway.egressCommName = "satcom";
   gateway.allowedSourcePlatformIds.push_back("fighter");
   gateway.allowedDestinationPlatformIds.push_back("command");
   gateway.allowedMessageTypes.push_back("MISSION_REPORT");
   gateway.processingDelayMs = 4.5;
   gateway.forwardingRateBps = 64000.0;
   gateway.maxQueueMessages = 16;
   gateway.maxQueueBits = 262144;
   gateway.receivedCount = 3;
   gateway.forwardedCount = 2;
   gateway.droppedCount = 1;
   gateway.forwardedBits = 4096;
   gateway.enabled = true;
   gateway.valid = true;
   nrm::GatewayForwardingEvent forwardingEvent;
   forwardingEvent.transferId = "transfer-report";
   forwardingEvent.routeId = "route-link11-satcom-cdl";
   forwardingEvent.gatewayCapabilityId = gateway.gatewayId;
   forwardingEvent.platformId = gateway.platformId;
   forwardingEvent.destinationCommName = "destination-comm";
   forwardingEvent.result = "FORWARDED";
   forwardingEvent.reasonCode = "FORWARD_ALLOWED";
   forwardingEvent.inputSerialNumber = 41;
   forwardingEvent.outputSerialNumber = 42;
   gateway.recentEvents.push_back(forwardingEvent);
   snapshot.gateways.push_back(gateway);

   nrm::GatewayRouteTemplate gatewayRoute;
   gatewayRoute.routeId = "route-link11-satcom-cdl";
   gatewayRoute.sourcePlatformId = "fighter";
   gatewayRoute.destinationPlatformId = "command";
   gatewayRoute.destinationCommName = "cdl";
   gatewayRoute.allowedMessageTypes.push_back("MISSION_REPORT");
   gatewayRoute.gatewayCapabilityIds.push_back("gw-link11-satcom-forward");
   gatewayRoute.gatewayCapabilityIds.push_back("gw-satcom-cdl-forward");
   gatewayRoute.priority = 100;
   gatewayRoute.enabled = true;
   gatewayRoute.valid = true;
   snapshot.gatewayRoutes.push_back(gatewayRoute);

   {
      WkNrm::SnapshotReporter reporter(outputRoot);
      runDirectory = reporter.GetRunDirectory();
      reporter.Enqueue(snapshot);
      nrm::AssessmentResult assessment;
      assessment.taskId = "TASK-REPORT";
      assessment.snapshotVersion = 7;
      assessment.configVersion = "test-config-v1";
      assessment.reachable = true;
      assessment.canEstablish = true;
      assessment.canComplete = false;
      assessment.reasons.push_back(nrm::AssessmentReason::cDATA_INVALID);
      assessment.primaryRoute = {"fighter", "command"};
      assessment.backupRoute = {"fighter", "relay", "command"};
      assessment.backupRouteUsesCandidate = true;
      nrm::AssessmentRouteHop primaryHop;
      primaryHop.hopIndex = 1;
      primaryHop.kind = nrm::AssessmentRouteHopKind::cCURRENT_LINK;
      primaryHop.sourceEndpointId = "fighter/link16";
      primaryHop.destinationEndpointId = "command/link16";
      primaryHop.sourcePlatform = "fighter";
      primaryHop.destinationPlatform = "command";
      primaryHop.sourceNetworkId = "network-link16";
      primaryHop.destinationNetworkId = "network-link16";
      primaryHop.sourceNetworkType = nrm::NetworkType::cLINK16;
      primaryHop.destinationNetworkType = nrm::NetworkType::cLINK16;
      assessment.primaryRouteHops.push_back(primaryHop);
      nrm::AssessmentRouteHop backupHop;
      backupHop.hopIndex = 2;
      backupHop.kind = nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION;
      backupHop.sourceEndpointId = "gateway/link11";
      backupHop.destinationEndpointId = "gateway/satcom";
      backupHop.sourcePlatform = "gateway";
      backupHop.destinationPlatform = "gateway";
      backupHop.sourceNetworkId = "network-link11";
      backupHop.destinationNetworkId = "network-satcom";
      backupHop.sourceNetworkType = nrm::NetworkType::cLINK11;
      backupHop.destinationNetworkType = nrm::NetworkType::cSATCOM;
      backupHop.gateway = true;
      backupHop.gatewayRouteId = "route-link11-satcom-cdl";
      backupHop.gatewayCapabilityId = "gw-link11-satcom-forward";
      assessment.backupRouteHops.push_back(backupHop);
      assessment.gatewayRouteIds.push_back("route-link11-satcom-cdl");
      assessment.gatewayCapabilityIds.push_back("gw-link11-satcom-forward");
      reporter.EnqueueAssessment(assessment);

      nrm::CapabilityResult capability;
      capability.requestId = "CAP-REPORT";
      capability.snapshotVersion = 7;
      capability.configVersion = "test-config-v1";
      capability.requestValid = true;
      capability.pathAvailable = true;
      capability.usesCandidate = true;
      capability.route = {"fighter", "command"};
      capability.endpointRoute = {"10.0.0.1", "10.0.0.2"};
      capability.communicationDistanceM.value = 1200.0;
      capability.communicationDistanceM.unit = "m";
      capability.communicationDistanceM.valid = true;
      capability.communicationDistanceM.origin = nrm::DataOrigin::cDERIVED;
      capability.communicationDistanceM.confidence = nrm::Confidence::cHIGH;
      capability.communicationDistanceM.reason = nrm::MetricReason::cNONE;
      nrm::EnvironmentEffect terrain;
      terrain.domain = nrm::EnvironmentDomain::cTERRAIN;
      terrain.reason = nrm::CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE;
      capability.environmentEffects.push_back(terrain);
      capability.reasons.push_back(nrm::CapabilityReason::cPARAMETERIZED_CANDIDATE);
      reporter.EnqueueCapability(capability);

      nrm::PlanValidationResult planValidation;
      planValidation.planId = "PLAN-REPORT";
      planValidation.revision = 3;
      planValidation.passed = true;
      reporter.EnqueuePlanValidation(planValidation);

      nrm::NetworkPlanEvaluationResult planEvaluation;
      planEvaluation.planId = "PLAN-REPORT";
      planEvaluation.revision = 3;
      planEvaluation.snapshotVersion = 7;
      planEvaluation.overallStatus = nrm::PlanEvaluationStatus::cPASS;
      planEvaluation.resultingState = nrm::NetworkPlanState::cVALIDATED;
      planEvaluation.validation = planValidation;
      nrm::PlanDemandEvaluation demandEvaluation;
      demandEvaluation.demandId = "DEMAND-REPORT";
      demandEvaluation.status = nrm::PlanEvaluationStatus::cFAIL;
      demandEvaluation.capability = capability;
      demandEvaluation.recommendations.push_back("切换到备用链路后重新推演。");
      planEvaluation.demands.push_back(demandEvaluation);
      reporter.EnqueuePlanEvaluation(planEvaluation);

      nrm::ResourceDemandBatchResult demandBatch;
      demandBatch.demandSetId = "DEMAND-SET-REPORT";
      demandBatch.revision = 2;
      demandBatch.snapshotVersion = 7;
      demandBatch.totalCount = 1;
      demandBatch.unsatisfiedCount = 1;
      nrm::ResourceDemandMatchResult demandResult;
      demandResult.demandId = "DEMAND-REPORT";
      demandResult.demandSetId = demandBatch.demandSetId;
      demandResult.demandSetRevision = demandBatch.revision;
      demandResult.snapshotVersion = 7;
      demandResult.status = nrm::DemandMatchStatus::cUNSATISFIED;
      demandResult.capability = capability;
      nrm::RequirementCheck check;
      check.type = nrm::RequirementItemType::cBANDWIDTH;
      check.applicable = true;
      check.reason = nrm::ResourceDemandReason::cBANDWIDTH_NOT_MET;
      demandResult.checks.push_back(check);
      demandResult.reasons.push_back(nrm::ResourceDemandReason::cBANDWIDTH_NOT_MET);
      nrm::PlanningRecommendation recommendation;
      recommendation.type = nrm::RecommendationType::cROUTE;
      recommendation.status = nrm::RecommendationStatus::cAVAILABLE;
      recommendation.demandId = demandResult.demandId;
      recommendation.value = "fighter -> command";
      recommendation.rank = 1;
      recommendation.snapshotVersion = 7;
      recommendation.reason = nrm::ResourceDemandReason::cNONE;
      recommendation.source = nrm::DataOrigin::cDERIVED;
      recommendation.confidence = nrm::Confidence::cHIGH;
      recommendation.evidence.push_back("capabilityRequestId=CAP-REPORT");
      demandResult.recommendations.push_back(recommendation);
      demandBatch.results.push_back(demandResult);
      reporter.EnqueueDemandResults(demandBatch);
      reporter.EnqueuePlanningRecommendations(demandBatch);

      nrm::ResourceDemandFeedback feedback;
      feedback.requestSource = "customer-planner";
      feedback.correlationId = "correlation-report";
      feedback.classifications = {"CONNECTIVITY", "PERFORMANCE"};
      feedback.batch = demandBatch;
      feedback.historicalPassRatioPercent = 75.0;
      feedback.historySampleCount = 4;
      reporter.EnqueueDemandFeedback(feedback);

      nrm::PlanCoordinationEvidence coordination;
      coordination.operation = "MEMBERSHIP_JOIN";
      coordination.requestId = "join-report";
      coordination.planId = "PLAN-REPORT";
      coordination.revision = 4;
      coordination.success = true;
      coordination.reason = nrm::PlanValidationReason::cNONE;
      reporter.EnqueuePlanningCoordination(coordination);
   }

   const std::string json = ReadAll(runDirectory + "/resource_snapshots.jsonl");
   CHECK(json.find("\"snapshot_version\":7") != std::string::npos);
   CHECK(json.find("\"type\":\"LINK16\"") != std::string::npos);
   CHECK(json.find("\"platform\":\"fighter\"") != std::string::npos);
   CHECK(json.find("\"networkId\":\"network-id-link16\"") !=
         std::string::npos);
   CHECK(json.find("\"comm\":\"link16\"") != std::string::npos);
   CHECK(json.find("\"can_send\":false") != std::string::npos);
   CHECK(json.find("\"memberRole\":") != std::string::npos);
   CHECK(json.find("\"currentOfflineDurationS\":") != std::string::npos);
   CHECK(json.find("\"deliveredThroughputBps\"") != std::string::npos);
   CHECK(json.find("\"window_s\":10") != std::string::npos);
   CHECK(json.find("\"routing_failed\":1") != std::string::npos);
   CHECK(json.find("\"transmitted_bits\":10000") != std::string::npos);
   CHECK(json.find("\"throughput_bps\":{\"value\":1000") != std::string::npos);
   CHECK(json.find("\"pdr_percent\":") != std::string::npos);
   CHECK(json.find("\"online_ratio_percent\":") != std::string::npos);
   CHECK(json.find("\"average_queue_delay_ms\":") != std::string::npos);
   CHECK(json.find("\"average_transport_delay_ms\":") != std::string::npos);
   CHECK(json.find("\"utilization_percent\":") != std::string::npos);
   CHECK(json.find("\"runId\"") != std::string::npos);
   CHECK(json.find("\"configVersion\":\"test-config-v1\"") != std::string::npos);
   CHECK(json.find("\"packetFormat\":\"AFSIM_NAVIGATION_ERROR_HISTORY_NEH\"") !=
         std::string::npos);
   CHECK(json.find("\"rawStatus\":\"GPS1\"") != std::string::npos);
   CHECK(json.find("\"platformId\":\"fighter-id\"") != std::string::npos);
   CHECK(json.find("\"mode\":\"GPS_ACTIVE\"") != std::string::npos);
   CHECK(json.find("\"totalPositionErrorM\":{\"value\":8.775") !=
         std::string::npos);
   CHECK(json.find("\"operationalArea\":{\"areaId\":\"AREA-REPORT\"") !=
         std::string::npos);
   CHECK(json.find("\"alarms\":[{\"alarmId\":\"ALARM-REPORT\"") !=
         std::string::npos);
   CHECK(json.find("\"gateway_count\":1") != std::string::npos);
   CHECK(json.find("\"gatewayId\":\"gw-link11-satcom-forward\"") !=
         std::string::npos);
   CHECK(json.find("\"forwardedCount\":2") != std::string::npos);
   CHECK(json.find("\"transferId\":\"transfer-report\"") !=
         std::string::npos);
   CHECK(json.find("\"destinationCommName\":\"destination-comm\"") !=
         std::string::npos);
   CHECK(json.find("\"gatewayRoutes\":[{\"routeId\":\"route-link11-satcom-cdl\"") !=
         std::string::npos);

   const std::string csv = ReadAll(runDirectory + "/network_summary.csv");
   CHECK(csv.find("nrm_link16_test") != std::string::npos);
   CHECK(csv.find("1000.000") != std::string::npos);

   const std::string assessment =
      ReadAll(runDirectory + "/assessment_results.jsonl");
   CHECK(assessment.find("\"task_id\":\"TASK-REPORT\"") != std::string::npos);
   CHECK(assessment.find("\"DATA_INVALID\"") != std::string::npos);
   CHECK(assessment.find("\"backup_route\":[\"fighter\",\"relay\",\"command\"]") !=
         std::string::npos);
   CHECK(assessment.find(
            "\"primary_route_hops\":[{\"hopIndex\":1,\"kind\":\"CURRENT_LINK\","
            "\"sourceEndpointId\":\"fighter/link16\",\"destinationEndpointId\":\"command/link16\","
            "\"sourcePlatform\":\"fighter\",\"destinationPlatform\":\"command\","
            "\"sourceNetworkId\":\"network-link16\",\"destinationNetworkId\":\"network-link16\","
            "\"sourceNetworkType\":\"LINK16\",\"destinationNetworkType\":\"LINK16\","
            "\"candidate\":false,\"gateway\":false,\"gatewayRouteId\":\"\","
            "\"gatewayCapabilityId\":\"\"}]") != std::string::npos);
   CHECK(assessment.find(
            "\"backup_route_hops\":[{\"hopIndex\":2,\"kind\":\"GATEWAY_TRANSITION\","
            "\"sourceEndpointId\":\"gateway/link11\",\"destinationEndpointId\":\"gateway/satcom\","
            "\"sourcePlatform\":\"gateway\",\"destinationPlatform\":\"gateway\","
            "\"sourceNetworkId\":\"network-link11\",\"destinationNetworkId\":\"network-satcom\","
            "\"sourceNetworkType\":\"LINK11\",\"destinationNetworkType\":\"SATCOM\","
            "\"candidate\":false,\"gateway\":true,"
            "\"gatewayRouteId\":\"route-link11-satcom-cdl\","
            "\"gatewayCapabilityId\":\"gw-link11-satcom-forward\"}]") != std::string::npos);
   CHECK(assessment.find("\"schema\":\"nrm.assessment.v3\"") != std::string::npos);
   CHECK(assessment.find("\"network_sequence\":[]") != std::string::npos);
   CHECK(assessment.find("\"recommendations\":[]") != std::string::npos);
   CHECK(assessment.find("\"gateway_route_ids\":[\"route-link11-satcom-cdl\"]") !=
         std::string::npos);
   CHECK(assessment.find("\"gateway_capability_ids\":[\"gw-link11-satcom-forward\"]") !=
         std::string::npos);

   const std::string capability =
      ReadAll(runDirectory + "/capability_results.jsonl");
   CHECK(capability.find("\"schema\":\"nrm.capability.v1\"") != std::string::npos);
   CHECK(capability.find("\"requestId\":\"CAP-REPORT\"") != std::string::npos);
   CHECK(capability.find("\"communicationDistanceM\":{\"value\":1200") !=
         std::string::npos);
   CHECK(capability.find("\"domain\":\"TERRAIN\"") != std::string::npos);
   CHECK(capability.find("\"pathLossDeltaDb\":") != std::string::npos);
   CHECK(capability.find("\"capacityScale\":") != std::string::npos);
   CHECK(capability.find("\"packetLossDeltaPercent\":") != std::string::npos);
   CHECK(capability.find("\"delayDeltaMs\":") != std::string::npos);
   CHECK(capability.find("ENVIRONMENT_DATA_UNAVAILABLE") != std::string::npos);
   CHECK(capability.find("PARAMETERIZED_CANDIDATE") != std::string::npos);

   const std::string planValidation =
      ReadAll(runDirectory + "/plan_validation_results.jsonl");
   CHECK(planValidation.find("\"schemaVersion\":\"nrm.network_plan_validation.v1\"") !=
         std::string::npos);
   CHECK(planValidation.find("\"planId\":\"PLAN-REPORT\"") != std::string::npos);
   CHECK(planValidation.find("\"passed\":true") != std::string::npos);

   const std::string planEvaluation =
      ReadAll(runDirectory + "/plan_evaluation_results.jsonl");
   CHECK(planEvaluation.find("\"schemaVersion\":\"nrm.network_plan_evaluation.v1\"") !=
         std::string::npos);
   CHECK(planEvaluation.find("\"overallStatus\":\"PASS\"") != std::string::npos);
   CHECK(planEvaluation.find("\"demandId\":\"DEMAND-REPORT\"") != std::string::npos);
   CHECK(planEvaluation.find("\"usesCandidate\":true") != std::string::npos);
   CHECK(planEvaluation.find("\"environmentEffects\":[") != std::string::npos);
   CHECK(planEvaluation.find("\"recommendations\":[\"切换到备用链路后重新推演。\"]") !=
         std::string::npos);

   const std::string demandResults =
      ReadAll(runDirectory + "/resource_demand_results.jsonl");
   CHECK(demandResults.find("\"demandSetId\":\"DEMAND-SET-REPORT\"") !=
         std::string::npos);
   CHECK(demandResults.find("\"status\":\"UNSATISFIED\"") !=
         std::string::npos);
   CHECK(demandResults.find("\"type\":\"BANDWIDTH\"") != std::string::npos);
   CHECK(demandResults.find("BANDWIDTH_NOT_MET") != std::string::npos);
   CHECK(demandResults.find("\"capability\":{") != std::string::npos);

   const std::string planningRecommendations =
      ReadAll(runDirectory + "/planning_recommendations.jsonl");
   CHECK(planningRecommendations.find("\"type\":\"ROUTE\"") !=
         std::string::npos);
   CHECK(planningRecommendations.find("\"status\":\"AVAILABLE\"") !=
         std::string::npos);
   CHECK(planningRecommendations.find("fighter -> command") != std::string::npos);
   CHECK(planningRecommendations.find("\"confidence\":\"HIGH\"") !=
         std::string::npos);

   const std::string alarms =
      ReadAll(runDirectory + "/resource_alarms.jsonl");
   CHECK(alarms.find("\"schemaVersion\":\"nrm.resource_alarm.v1\"") !=
         std::string::npos);
   CHECK(alarms.find("ALARM-REPORT") != std::string::npos);

   const std::string feedback =
      ReadAll(runDirectory + "/demand_feedback.jsonl");
   CHECK(feedback.find("\"schemaVersion\":\"nrm.resource_demand_feedback.v1\"") !=
         std::string::npos);
   CHECK(feedback.find("\"requestSource\":\"customer-planner\"") !=
         std::string::npos);
   CHECK(feedback.find("\"historicalPassRatioPercent\":75") !=
         std::string::npos);

   const std::string coordination =
      ReadAll(runDirectory + "/planning_coordination.jsonl");
   CHECK(coordination.find("\"schemaVersion\":\"nrm.planning_coordination.v1\"") !=
         std::string::npos);
   CHECK(coordination.find("\"operation\":\"MEMBERSHIP_JOIN\"") !=
         std::string::npos);

   const std::string manifest = ReadAll(runDirectory + "/manifest.json");
   CHECK(manifest.find("\"complete\":true") != std::string::npos);
   CHECK(manifest.find("\"softwareVersion\"") != std::string::npos);
   CHECK(manifest.find("capability_results.jsonl") != std::string::npos);
   CHECK(manifest.find("plan_validation_results.jsonl") != std::string::npos);
   CHECK(manifest.find("plan_evaluation_results.jsonl") != std::string::npos);
   CHECK(manifest.find("resource_demand_results.jsonl") != std::string::npos);
   CHECK(manifest.find("planning_recommendations.jsonl") != std::string::npos);
   CHECK(manifest.find("resource_alarms.jsonl") != std::string::npos);
   CHECK(manifest.find("demand_feedback.jsonl") != std::string::npos);
   CHECK(manifest.find("planning_coordination.jsonl") != std::string::npos);
   CHECK(manifest.find("\"droppedDemandResultCount\":0") != std::string::npos);
   CHECK(manifest.find("\"droppedPlanningRecommendationCount\":0") !=
         std::string::npos);
   return 0;
}
