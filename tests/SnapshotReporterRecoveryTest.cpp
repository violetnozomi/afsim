#include "NrmSnapshotReporter.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

bool Contains(const std::string& path, const std::string& token)
{
   std::ifstream input(path);
   std::stringstream buffer;
   buffer << input.rdbuf();
   return buffer.str().find(token) != std::string::npos;
}

int main()
{
   const std::string root =
      "/tmp/nrm-reporter-recovery-" + std::to_string(getpid());
   std::string firstRun;
   {
      WkNrm::SnapshotReporter reporter(root);
      firstRun = reporter.GetRunDirectory();
      nrm::ResourceSnapshot snapshot;
      snapshot.snapshotVersion = 1;
      reporter.Enqueue(snapshot);
   }
   CHECK(Contains(firstRun + "/manifest.json", "\"complete\":true"));
   CHECK(Contains(firstRun + "/resource_snapshots.jsonl", "\"snapshot_version\":1"));

   std::string secondRun;
   {
      WkNrm::SnapshotReporter reporter(root);
      secondRun = reporter.GetRunDirectory();
      nrm::ResourceSnapshot snapshot;
      snapshot.snapshotVersion = 2;
      reporter.Enqueue(snapshot);
   }
   CHECK(firstRun != secondRun);
   CHECK(Contains(firstRun + "/resource_snapshots.jsonl", "\"snapshot_version\":1"));
   CHECK(Contains(secondRun + "/resource_snapshots.jsonl", "\"snapshot_version\":2"));

   {
      WkNrm::SnapshotReporter reporter(root, "overflow-test", 2, false);
      for (std::uint64_t version = 1; version <= 5; ++version)
      {
         nrm::ResourceSnapshot snapshot;
         snapshot.snapshotVersion = version;
         reporter.Enqueue(snapshot);
      }
      CHECK(reporter.GetStatus().droppedSnapshotCount == 3);
      reporter.Start();
   }

   {
      WkNrm::SnapshotReporter reporter(root, "capability-overflow-test", 2, false);
      for (std::uint64_t version = 1; version <= 5; ++version)
      {
         nrm::CapabilityResult capability;
         capability.snapshotVersion = version;
         reporter.EnqueueCapability(capability);
      }
      CHECK(reporter.GetStatus().droppedCapabilityCount == 3);
      reporter.Start();
   }

   {
      WkNrm::SnapshotReporter reporter(root, "plan-overflow-test", 2, false);
      for (std::uint64_t revision = 1; revision <= 5; ++revision)
      {
         nrm::PlanValidationResult validation;
         validation.revision = revision;
         reporter.EnqueuePlanValidation(validation);
         nrm::NetworkPlanEvaluationResult evaluation;
         evaluation.revision = revision;
         reporter.EnqueuePlanEvaluation(evaluation);
      }
      CHECK(reporter.GetStatus().droppedPlanValidationCount == 3);
      CHECK(reporter.GetStatus().droppedPlanEvaluationCount == 3);
      reporter.Start();
   }

   std::string demandRun;
   {
      WkNrm::SnapshotReporter reporter(root, "demand-overflow-test", 2, false);
      demandRun = reporter.GetRunDirectory();
      for (std::uint64_t revision = 1; revision <= 5; ++revision)
      {
         nrm::ResourceDemandBatchResult batch;
         batch.revision = revision;
         nrm::ResourceDemandMatchResult result;
         result.demandId = "demand-" + std::to_string(revision);
         result.demandSetRevision = revision;
         nrm::PlanningRecommendation recommendation;
         recommendation.demandId = result.demandId;
         recommendation.type = nrm::RecommendationType::cROUTE;
         recommendation.reason = nrm::ResourceDemandReason::cROUTE_UNAVAILABLE;
         result.recommendations.push_back(recommendation);
         batch.results.push_back(result);
         reporter.EnqueueDemandResults(batch);
         reporter.EnqueuePlanningRecommendations(batch);
      }
      CHECK(reporter.GetStatus().droppedDemandResultCount == 3);
      CHECK(reporter.GetStatus().droppedPlanningRecommendationCount == 3);
      reporter.Start();
   }
   CHECK(Contains(demandRun + "/resource_demand_results.jsonl", "demand-4"));
   CHECK(Contains(demandRun + "/resource_demand_results.jsonl", "demand-5"));
   CHECK(Contains(demandRun + "/planning_recommendations.jsonl",
                  "ROUTE_UNAVAILABLE"));
   CHECK(Contains(demandRun + "/manifest.json",
                  "\"droppedDemandResultCount\":3"));
   CHECK(Contains(demandRun + "/manifest.json",
                  "\"droppedPlanningRecommendationCount\":3"));

   std::string demandErrorRun;
   {
      WkNrm::SnapshotReporter reporter(root, "demand-error-test");
      demandErrorRun = reporter.GetRunDirectory();
      reporter.ReportDemandError(nrm::ResourceDemandReason::cPARSE_ERROR,
                                 "line:7");
      CHECK(reporter.GetStatus().healthy);
      CHECK(reporter.GetStatus().writeErrorCount == 0);
      CHECK(reporter.GetStatus().domainErrorCount == 1);
   }
   CHECK(Contains(demandErrorRun + "/error.log",
                  "\"component\":\"ResourceDemand\""));
   CHECK(Contains(demandErrorRun + "/error.log", "PARSE_ERROR"));
   CHECK(Contains(demandErrorRun + "/error.log", "line:7"));

   {
      WkNrm::SnapshotReporter reporter(root, "customer-event-write-failure", 2,
                                       false);
      const std::string eventPath =
         reporter.GetRunDirectory() + "/customer_interface_events.jsonl";
      CHECK(::mkdir(eventPath.c_str(), 0700) == 0);
      reporter.ReportCustomerInterfaceEvent("schema", "message", "input.json",
                                            false, "DATA_INVALID", "/data");
      CHECK(!reporter.GetStatus().healthy);
      CHECK(reporter.GetStatus().writeErrorCount == 1);
      CHECK(reporter.GetStatus().lastError == "OUTPUT_OPEN_FAILED");
   }

   {
      WkNrm::SnapshotReporter reporter(root, "never-started", 2, false);
      CHECK(!reporter.GetStatus().started);
   }

   {
      WkNrm::SnapshotReporter reporter(
         "/proc/nrm-output-not-writable", "failure-test", 2, false);
      CHECK(!reporter.GetStatus().healthy);
      nrm::ResourceSnapshot snapshot;
      reporter.Enqueue(snapshot);
   }
   return 0;
}
