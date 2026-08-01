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

   nrm::NetworkSnapshot network;
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
   endpoint.networkName = network.networkName;
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = nrm::ResourceState::cONLINE;
   snapshot.endpoints.push_back(endpoint);

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
      demandEvaluation.status = nrm::PlanEvaluationStatus::cPASS;
      demandEvaluation.capability = capability;
      planEvaluation.demands.push_back(demandEvaluation);
      reporter.EnqueuePlanEvaluation(planEvaluation);
   }

   const std::string json = ReadAll(runDirectory + "/resource_snapshots.jsonl");
   CHECK(json.find("\"snapshot_version\":7") != std::string::npos);
   CHECK(json.find("\"type\":\"LINK16\"") != std::string::npos);
   CHECK(json.find("\"platform\":\"fighter\"") != std::string::npos);
   CHECK(json.find("\"comm\":\"link16\"") != std::string::npos);
   CHECK(json.find("\"can_send\":false") != std::string::npos);
   CHECK(json.find("},\"currentOfflineDurationS\":") != std::string::npos);
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

   const std::string csv = ReadAll(runDirectory + "/network_summary.csv");
   CHECK(csv.find("nrm_link16_test") != std::string::npos);
   CHECK(csv.find("1000.000") != std::string::npos);

   const std::string assessment =
      ReadAll(runDirectory + "/assessment_results.jsonl");
   CHECK(assessment.find("\"task_id\":\"TASK-REPORT\"") != std::string::npos);
   CHECK(assessment.find("\"DATA_INVALID\"") != std::string::npos);
   CHECK(assessment.find("\"backup_route\":[\"fighter\",\"relay\",\"command\"]") !=
         std::string::npos);
   CHECK(assessment.find("\"schema\":\"nrm.assessment.v3\"") != std::string::npos);
   CHECK(assessment.find("\"network_sequence\":[]") != std::string::npos);
   CHECK(assessment.find("\"recommendations\":[]") != std::string::npos);

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

   const std::string manifest = ReadAll(runDirectory + "/manifest.json");
   CHECK(manifest.find("\"complete\":true") != std::string::npos);
   CHECK(manifest.find("\"softwareVersion\"") != std::string::npos);
   CHECK(manifest.find("capability_results.jsonl") != std::string::npos);
   CHECK(manifest.find("plan_validation_results.jsonl") != std::string::npos);
   CHECK(manifest.find("plan_evaluation_results.jsonl") != std::string::npos);
   return 0;
}
