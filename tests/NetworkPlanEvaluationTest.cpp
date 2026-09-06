#include "nrm/NetworkPlanEvaluationService.hpp"
#include "nrm/NetworkPlanDistributionService.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>

#include <unistd.h>

namespace
{
nrm::EndpointSnapshot Endpoint(const char* aId,
                               const char* aPlatform,
                               double aLatitude,
                               double aLongitude,
                               nrm::ResourceState aState = nrm::ResourceState::cONLINE)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = aId;
   endpoint.platformName = aPlatform;
   endpoint.networkName = "plan_link16";
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = aState;
   endpoint.latitudeDeg.value = aLatitude;
   endpoint.latitudeDeg.valid = true;
   endpoint.longitudeDeg.value = aLongitude;
   endpoint.longitudeDeg.valid = true;
   endpoint.altitudeM.value = 1000.0;
   endpoint.altitudeM.valid = true;
   return endpoint;
}

nrm::LinkSnapshot Link(double aBandwidthBps,
                       double aDelayMs,
                       double aPdrPercent)
{
   nrm::LinkSnapshot link;
   link.linkId = "source-destination";
   link.sourceEndpointId = "source-endpoint";
   link.destinationEndpointId = "destination-endpoint";
   link.sourcePlatform = "source";
   link.destinationPlatform = "destination";
   link.networkName = "plan_link16";
   link.networkType = nrm::NetworkType::cLINK16;
   link.state = nrm::ResourceState::cONLINE;
   link.distanceM.value = 1000.0;
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
   window.deliveryRatioPercent.value = aPdrPercent;
   window.deliveryRatioPercent.valid = true;
   window.deliveryRatioPercent.origin = nrm::DataOrigin::cDERIVED;
   window.deliveryRatioPercent.confidence = nrm::Confidence::cHIGH;
   window.deliveredThroughputBps.value = aBandwidthBps / 2.0;
   window.deliveredThroughputBps.valid = true;
   link.windows.push_back(window);
   return link;
}

nrm::ResourceSnapshot Snapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 91;
   snapshot.simTime = 20.0;
   snapshot.configVersion = "demo-0.7.0";
   snapshot.endpoints.push_back(Endpoint("source-endpoint", "source", 35.0, 120.0));
   snapshot.endpoints.push_back(
      Endpoint("destination-endpoint", "destination", 35.01, 120.01));
   snapshot.links.push_back(Link(1000.0, 10.0, 95.0));
   nrm::NetworkSnapshot network;
   network.networkName = "plan_link16";
   network.networkType = nrm::NetworkType::cLINK16;
   network.endpointCount = 2;
   network.onlineCount = 2;
   snapshot.networks.push_back(network);
   return snapshot;
}

nrm::NetworkPlanDemand Demand(const char* aId)
{
   nrm::NetworkPlanDemand demand;
   demand.demandId = aId;
   demand.businessType = "C2";
   demand.sourcePlatform = "source";
   demand.destinationPlatform = "destination";
   demand.payloadBits = 1024;
   demand.requiredBandwidthBps = 500.0;
   demand.maximumDelayMs = 20.0;
   demand.minimumPdrPercent = 90.0;
   demand.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   return demand;
}

nrm::NetworkPlanDocument Plan()
{
   nrm::NetworkPlanDocument plan;
   plan.planId = "evaluation-plan";
   plan.revision = 1;
   plan.configVersion = "demo-0.7.0";
   plan.providerId = "evaluation-test";
   plan.createdTime = "2026-08-01T13:00:00Z";
   plan.valid = true;
   nrm::NetworkPlanAllocation allocation;
   allocation.allocationId = "allocation-link16";
   allocation.networkName = "plan_link16";
   allocation.networkType = nrm::NetworkType::cLINK16;
   allocation.profileId = "demo-link16-v1";
   allocation.frequencyHz = 1000000000.0;
   allocation.channelId = "channel-1";
   allocation.subnetId = "subnet-1";
   allocation.slotIds.push_back("slot-1");
   allocation.memberPlatformIds = {"source", "destination"};
   allocation.routePolicyId = "route-direct";
   plan.allocations.push_back(allocation);
   plan.demands.push_back(Demand("demand-1"));
   plan.demands.push_back(Demand("demand-2"));
   return plan;
}

bool HasReason(const nrm::PlanDemandEvaluation& aEvaluation,
               nrm::PlanValidationReason aReason)
{
   return std::find(aEvaluation.reasons.begin(), aEvaluation.reasons.end(), aReason) !=
          aEvaluation.reasons.end();
}

bool HasRecommendationContaining(const nrm::PlanDemandEvaluation& aEvaluation,
                                 const std::string& aText)
{
   return std::any_of(
      aEvaluation.recommendations.begin(), aEvaluation.recommendations.end(),
      [&aText](const std::string& aRecommendation)
      {
         return aRecommendation.find(aText) != std::string::npos;
      });
}

class CountingEnvironmentAdapter : public nrm::EnvironmentEffectAdapter
{
public:
   nrm::EnvironmentEffect Evaluate(
      nrm::EnvironmentDomain aDomain,
      const nrm::ResourceSnapshot&,
      const nrm::CapabilityRequest&,
      const std::vector<std::string>&,
      const nrm::EnvironmentContext& aContext) const override
   {
      ++calls;
      nrm::EnvironmentEffect effect;
      effect.domain = aDomain;
      effect.valid = true;
      effect.providerId = aContext.providerId;
      effect.sampleTime = aContext.sampleTime;
      return effect;
   }
   mutable std::size_t calls = 0;
};

std::string ReadAll(const std::string& aPath)
{
   std::ifstream input(aPath);
   std::ostringstream output;
   output << input.rdbuf();
   return output.str();
}
} // namespace

int main()
{
   const nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const nrm::NetworkPlanEvaluationService service(profiles);
   nrm::ResourceSnapshot snapshot = Snapshot();
   nrm::NetworkPlanDocument plan = Plan();

   const std::uint64_t snapshotVersion = snapshot.snapshotVersion;
   const double linkBandwidth = snapshot.links[0].bandwidthBps.value;
   const std::uint64_t planRevision = plan.revision;
   const std::size_t planDemandCount = plan.demands.size();
   const nrm::NetworkPlanEvaluationResult passing = service.Evaluate(snapshot, plan);
   assert(passing.validation.passed);
   assert(!passing.planFingerprint.empty());
   assert(passing.planFingerprint == passing.validation.planFingerprint);
   assert(passing.overallStatus == nrm::PlanEvaluationStatus::cPASS);
   assert(passing.resultingState == nrm::NetworkPlanState::cVALIDATED);
   assert(passing.demands.size() == 2);
   assert(passing.demands[0].status == nrm::PlanEvaluationStatus::cPASS);
   assert(passing.demands[1].status == nrm::PlanEvaluationStatus::cPASS);
   assert(snapshot.snapshotVersion == snapshotVersion);
   assert(snapshot.links[0].bandwidthBps.value == linkBandwidth);
   assert(plan.revision == planRevision);
   assert(plan.demands.size() == planDemandCount);
   assert(plan.state == nrm::NetworkPlanState::cDRAFT);
   assert(profiles.Valid());
   assert(profiles.ConfigVersion() == "demo-0.7.0");

   const std::string packageOutput =
      "/tmp/nrm-network-plan-distribution-" +
      std::to_string(static_cast<long long>(getpid()));
   const nrm::NetworkPlanDistributionService distributionService;
   const nrm::DistributionPackageResult package = distributionService.Generate(
      plan, passing.validation, passing, packageOutput);
   assert(package.generated);
   assert(package.reason == nrm::PlanValidationReason::cNONE);
   assert(package.resultingState == nrm::NetworkPlanState::cREADY_FOR_DISTRIBUTION);
   assert(ReadAll(package.outputPath + "/manifest.json").find(
             "\"readyForDistribution\":true") != std::string::npos);
   assert(ReadAll(package.outputPath + "/manifest.json").find(
             "\"planFingerprint\":\"") != std::string::npos);
   assert(ReadAll(package.outputPath + "/validation_result.json").find(
             "\"passed\":true") != std::string::npos);
   assert(ReadAll(package.outputPath + "/validation_result.json").find(
             "\"planFingerprint\":\"") != std::string::npos);
   assert(ReadAll(package.outputPath + "/evaluation_result.json").find(
             "\"overallStatus\":\"PASS\"") != std::string::npos);
   assert(ReadAll(package.outputPath + "/evaluation_result.json").find(
             "\"maximumHopDistanceM\":") != std::string::npos);
   assert(ReadAll(package.outputPath + "/evaluation_result.json").find(
             "\"profileIds\":[") != std::string::npos);
   assert(ReadAll(package.outputPath + "/evaluation_result.json").find(
             "\"endpointRoute\":[") != std::string::npos);
   nrm::NetworkPlanRepository packagedPlan;
   assert(packagedPlan.LoadFromFile(package.outputPath + "/network_plan.nrm"));
   assert(packagedPlan.GetCurrentPlan()->state ==
          nrm::NetworkPlanState::cREADY_FOR_DISTRIBUTION);
   assert(plan.state == nrm::NetworkPlanState::cDRAFT);
   const nrm::DistributionPackageResult duplicatePackage =
      distributionService.Generate(plan, passing.validation, passing, packageOutput);
   assert(!duplicatePackage.generated);
   assert(duplicatePackage.reason ==
          nrm::PlanValidationReason::cPACKAGE_ALREADY_EXISTS);

   nrm::NetworkPlanDocument modifiedPlan = plan;
   modifiedPlan.demands[0].maximumDelayMs += 1.0;
   const nrm::DistributionPackageResult staleResultPackage =
      distributionService.Generate(
         modifiedPlan, passing.validation, passing, packageOutput + "-stale-result");
   assert(!staleResultPackage.generated);
   assert(staleResultPackage.reason ==
          nrm::PlanValidationReason::cPLAN_IDENTITY_MISMATCH);

   nrm::NetworkPlanEvaluationResult mismatchedDemand = passing;
   mismatchedDemand.demands[0].demandId = "different-demand";
   const nrm::DistributionPackageResult mismatchedDemandPackage =
      distributionService.Generate(
         plan, passing.validation, mismatchedDemand, packageOutput + "-mismatch");
   assert(!mismatchedDemandPackage.generated);
   assert(mismatchedDemandPackage.reason ==
          nrm::PlanValidationReason::cPLAN_NOT_VALIDATED);

   nrm::NetworkPlanDocument unsafePlan = plan;
   unsafePlan.planId = "../unsafe-plan";
   nrm::PlanValidationResult unsafeValidation = passing.validation;
   unsafeValidation.planId = unsafePlan.planId;
   nrm::NetworkPlanEvaluationResult unsafeEvaluation = passing;
   unsafeEvaluation.planId = unsafePlan.planId;
   unsafeEvaluation.validation.planId = unsafePlan.planId;
   const nrm::DistributionPackageResult unsafePackage =
      distributionService.Generate(
         unsafePlan, unsafeValidation, unsafeEvaluation, packageOutput);
   assert(!unsafePackage.generated);
   assert(unsafePackage.reason ==
          nrm::PlanValidationReason::cOUTPUT_PATH_INVALID);

   nrm::NetworkPlanDocument bandwidthPlan = Plan();
   bandwidthPlan.demands.resize(1);
   bandwidthPlan.demands[0].requiredBandwidthBps = 300000.0;
   const nrm::NetworkPlanEvaluationResult bandwidth =
      service.Evaluate(snapshot, bandwidthPlan);
   assert(bandwidth.overallStatus == nrm::PlanEvaluationStatus::cFAIL);
   assert(bandwidth.demands[0].status == nrm::PlanEvaluationStatus::cFAIL);
   assert(HasReason(bandwidth.demands[0], nrm::PlanValidationReason::cNO_PATH) ||
          HasReason(bandwidth.demands[0], nrm::PlanValidationReason::cBANDWIDTH_NOT_MET));
   assert(!bandwidth.demands[0].recommendations.empty());
   assert(HasRecommendationContaining(bandwidth.demands[0], "带宽") ||
          HasRecommendationContaining(bandwidth.demands[0], "路径"));
   std::ostringstream bandwidthJson;
   nrm::network_plan_serialization::WriteEvaluation(bandwidthJson, bandwidth);
   assert(bandwidthJson.str().find("\"recommendations\":[") != std::string::npos);
   assert(bandwidthJson.str().find("带宽") != std::string::npos ||
          bandwidthJson.str().find("路径") != std::string::npos);
   const nrm::DistributionPackageResult rejectedPackage =
      distributionService.Generate(
         bandwidthPlan, bandwidth.validation, bandwidth,
         packageOutput + "-rejected");
   assert(!rejectedPackage.generated);
   assert(rejectedPackage.reason ==
          nrm::PlanValidationReason::cPLAN_NOT_VALIDATED);

   nrm::NetworkPlanDocument delayPlan = Plan();
   delayPlan.demands.resize(1);
   delayPlan.demands[0].maximumDelayMs = 5.0;
   const nrm::NetworkPlanEvaluationResult delay = service.Evaluate(snapshot, delayPlan);
   assert(delay.overallStatus == nrm::PlanEvaluationStatus::cFAIL);
   assert(HasRecommendationContaining(delay.demands[0], "时延"));

   nrm::NetworkPlanDocument pdrPlan = Plan();
   pdrPlan.demands.resize(1);
   pdrPlan.demands[0].minimumPdrPercent = 99.0;
   const nrm::NetworkPlanEvaluationResult pdr = service.Evaluate(snapshot, pdrPlan);
   assert(pdr.overallStatus == nrm::PlanEvaluationStatus::cFAIL);
   assert(HasRecommendationContaining(pdr.demands[0], "PDR"));

   nrm::ResourceSnapshot noPathSnapshot = snapshot;
   noPathSnapshot.links.clear();
   for (nrm::EndpointSnapshot& endpoint : noPathSnapshot.endpoints)
   {
      endpoint.latitudeDeg.valid = false;
      endpoint.longitudeDeg.valid = false;
   }
   const nrm::NetworkPlanEvaluationResult noPath =
      service.Evaluate(noPathSnapshot, Plan());
   assert(noPath.overallStatus == nrm::PlanEvaluationStatus::cFAIL);
   assert(HasReason(noPath.demands[0], nrm::PlanValidationReason::cNO_PATH));
   assert(HasRecommendationContaining(noPath.demands[0], "中继") ||
          HasRecommendationContaining(noPath.demands[0], "备用"));

   nrm::ResourceSnapshot offlineSnapshot = snapshot;
   offlineSnapshot.endpoints[1].state = nrm::ResourceState::cOFFLINE;
   const nrm::NetworkPlanEvaluationResult offline =
      service.Evaluate(offlineSnapshot, Plan());
   assert(offline.overallStatus == nrm::PlanEvaluationStatus::cFAIL);
   assert(HasReason(offline.demands[0], nrm::PlanValidationReason::cNODE_OFFLINE));
   assert(HasRecommendationContaining(offline.demands[0], "在线"));

   nrm::ResourceSnapshot invalidMetricSnapshot = noPathSnapshot;
   invalidMetricSnapshot.links.push_back(Link(1000.0, 10.0, 95.0));
   invalidMetricSnapshot.links[0].bandwidthBps.valid = false;
   nrm::NetworkPlanDocument invalidMetricPlan = Plan();
   invalidMetricPlan.demands.resize(1);
   const nrm::NetworkPlanEvaluationResult invalidMetric =
      service.Evaluate(invalidMetricSnapshot, invalidMetricPlan);
   assert(invalidMetric.overallStatus == nrm::PlanEvaluationStatus::cDATA_INVALID);
   assert(HasRecommendationContaining(invalidMetric.demands[0], "参数化"));

   // Regression: a parameterized plan may use low-confidence profile values
   // when an established current link has no observed bandwidth, delay, or
   // PDR.  Customer-origin plans must remain strict for the same snapshot.
   nrm::ResourceSnapshot missingObservedSnapshot = Snapshot();
   missingObservedSnapshot.links[0].bandwidthBps.valid = false;
   missingObservedSnapshot.links[0].windows[0].averageTransportDelayMs.valid = false;
   missingObservedSnapshot.links[0].windows[0].deliveryRatioPercent.valid = false;
   nrm::NetworkPlanDocument parameterizedPlan = Plan();
   parameterizedPlan.source = nrm::DataOrigin::cPARAMETERIZED_MODEL;
   for (nrm::NetworkPlanDemand& demand : parameterizedPlan.demands)
      demand.maximumDelayMs = 25.0;
   const nrm::NetworkPlanEvaluationResult parameterizedFallback =
      service.Evaluate(missingObservedSnapshot, parameterizedPlan);
   assert(parameterizedFallback.overallStatus == nrm::PlanEvaluationStatus::cPASS);
   assert(parameterizedFallback.demands[0].status == nrm::PlanEvaluationStatus::cPASS);
   assert(parameterizedFallback.demands[0].capability.pathAvailable);
   assert(!parameterizedFallback.demands[0].capability.usesCandidate);
   assert(parameterizedFallback.demands[0].capability.transmissionRateBps.origin ==
          nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(parameterizedFallback.demands[0].capability.transmissionRateBps.confidence ==
          nrm::Confidence::cLOW);
   assert(parameterizedFallback.demands[0].capability.transmissionDelayMs.origin ==
          nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(parameterizedFallback.demands[0].capability.transmissionDelayMs.confidence ==
          nrm::Confidence::cLOW);
   assert(parameterizedFallback.demands[0].capability.packetLossPercent.origin ==
          nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(parameterizedFallback.demands[0].capability.packetLossPercent.confidence ==
          nrm::Confidence::cLOW);
   assert(std::find(parameterizedFallback.demands[0].capability.profileIds.begin(),
                    parameterizedFallback.demands[0].capability.profileIds.end(),
                    "demo-link16-v1") !=
          parameterizedFallback.demands[0].capability.profileIds.end());

   nrm::NetworkPlanDocument strictObservedPlan = Plan();
   const nrm::NetworkPlanEvaluationResult strictObserved =
      service.Evaluate(missingObservedSnapshot, strictObservedPlan);
   assert(strictObserved.overallStatus == nrm::PlanEvaluationStatus::cDATA_INVALID);
   assert(strictObserved.demands[0].status == nrm::PlanEvaluationStatus::cDATA_INVALID);

   nrm::ResourceSnapshot candidateSnapshot = snapshot;
   candidateSnapshot.links.clear();
   nrm::NetworkPlanDocument candidatePlan = Plan();
   for (nrm::NetworkPlanDemand& demand : candidatePlan.demands)
      demand.maximumDelayMs = 1000.0;
   const nrm::NetworkPlanEvaluationResult candidate =
      service.Evaluate(candidateSnapshot, candidatePlan);
   assert(candidate.overallStatus == nrm::PlanEvaluationStatus::cPASS);
   assert(candidate.demands[0].capability.usesCandidate);
   assert(candidate.demands[0].capability.transmissionRateBps.origin ==
          nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(candidate.demands[0].capability.transmissionRateBps.confidence ==
          nrm::Confidence::cLOW);

   CountingEnvironmentAdapter adapter;
   const nrm::NetworkPlanEvaluationService countedService(profiles, &adapter);
   nrm::EnvironmentContext environment;
   environment.valid = true;
   environment.providerId = "environment-test";
   environment.sampleTime = 20.0;
   nrm::NetworkPlanDocument invalidPlan = Plan();
   invalidPlan.allocations[0].profileId = "missing-profile";
   const nrm::NetworkPlanEvaluationResult invalid =
      countedService.Evaluate(snapshot, invalidPlan, environment);
   assert(!invalid.validation.passed);
   assert(invalid.overallStatus == nrm::PlanEvaluationStatus::cDATA_INVALID);
   assert(invalid.demands[0].status == nrm::PlanEvaluationStatus::cNOT_EVALUATED);
   assert(HasRecommendationContaining(invalid.demands[0], "校验"));
   assert(adapter.calls == 0);

   std::remove((package.outputPath + "/network_plan.nrm").c_str());
   std::remove((package.outputPath + "/validation_result.json").c_str());
   std::remove((package.outputPath + "/evaluation_result.json").c_str());
   std::remove((package.outputPath + "/manifest.json").c_str());
   assert(rmdir(package.outputPath.c_str()) == 0);
   assert(rmdir((packageOutput + "/distribution_packages/evaluation-plan").c_str()) == 0);
   assert(rmdir((packageOutput + "/distribution_packages").c_str()) == 0);
   assert(rmdir(packageOutput.c_str()) == 0);
   return 0;
}
