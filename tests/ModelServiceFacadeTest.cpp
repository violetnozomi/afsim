#include "nrm/ModelServiceFacade.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <unistd.h>

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
   endpoint.networkName = "service-link16";
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = nrm::ResourceState::cONLINE;
   endpoint.canSend = true;
   endpoint.canReceive = true;
   endpoint.latitudeDeg.value = aLatitude;
   endpoint.latitudeDeg.valid = true;
   endpoint.longitudeDeg.value = aLongitude;
   endpoint.longitudeDeg.valid = true;
   endpoint.altitudeM.value = 1000.0;
   endpoint.altitudeM.valid = true;
   return endpoint;
}

nrm::LinkSnapshot Link()
{
   nrm::LinkSnapshot link;
   link.linkId = "service-link";
   link.sourceEndpointId = "source-endpoint";
   link.destinationEndpointId = "destination-endpoint";
   link.sourcePlatform = "source";
   link.destinationPlatform = "destination";
   link.networkName = "service-link16";
   link.networkType = nrm::NetworkType::cLINK16;
   link.state = nrm::ResourceState::cONLINE;
   link.distanceM.value = 1000.0;
   link.distanceM.valid = true;
   link.distanceM.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   link.distanceM.confidence = nrm::Confidence::cHIGH;
   link.bandwidthBps.value = 1000.0;
   link.bandwidthBps.valid = true;
   link.bandwidthBps.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   link.bandwidthBps.confidence = nrm::Confidence::cHIGH;

   nrm::WindowMetrics window;
   window.windowS = 10.0;
   window.averageTransportDelayMs.value = 10.0;
   window.averageTransportDelayMs.valid = true;
   window.averageTransportDelayMs.origin = nrm::DataOrigin::cDERIVED;
   window.averageTransportDelayMs.confidence = nrm::Confidence::cHIGH;
   window.deliveryRatioPercent.value = 95.0;
   window.deliveryRatioPercent.valid = true;
   window.deliveryRatioPercent.origin = nrm::DataOrigin::cDERIVED;
   window.deliveryRatioPercent.confidence = nrm::Confidence::cHIGH;
   window.deliveredThroughputBps.value = 500.0;
   window.deliveredThroughputBps.valid = true;
   window.deliveredThroughputBps.origin = nrm::DataOrigin::cDERIVED;
   window.deliveredThroughputBps.confidence = nrm::Confidence::cHIGH;
   link.windows.push_back(window);
   return link;
}

nrm::ResourceSnapshot Snapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 110;
   snapshot.simTime = 30.0;
   snapshot.configVersion = "demo-0.7.0";
   snapshot.endpoints.push_back(
      Endpoint("source-endpoint", "source", 35.0, 120.0));
   snapshot.endpoints.push_back(
      Endpoint("destination-endpoint", "destination", 35.01, 120.01));
   snapshot.links.push_back(Link());
   nrm::NetworkSnapshot network;
   network.networkId = "service-network";
   network.networkName = "service-link16";
   network.networkType = nrm::NetworkType::cLINK16;
   network.endpointCount = 2;
   network.onlineCount = 2;
   network.activeLinks = 1;
   snapshot.networks.push_back(network);
   return snapshot;
}

nrm::CapabilityRequest CapabilityRequest()
{
   nrm::CapabilityRequest request;
   request.requestId = "CAPABILITY-110";
   request.sourcePlatform = "source";
   request.destinationPlatform = "destination";
   request.businessType = "C2";
   request.requiredBandwidthBps = 500.0;
   request.maximumDelayMs = 20.0;
   request.minimumPdrPercent = 90.0;
   request.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   return request;
}

nrm::AssessmentTask AssessmentTask()
{
   nrm::AssessmentTask task;
   task.taskId = "ASSESSMENT-110";
   task.sourcePlatform = "source";
   task.destinationPlatform = "destination";
   task.businessType = "C2";
   task.requiredBandwidthBps = 500.0;
   task.maximumDelayMs = 20.0;
   task.minimumPdrPercent = 90.0;
   task.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   return task;
}

nrm::NetworkPlanDocument Plan()
{
   nrm::NetworkPlanDocument plan;
   plan.planId = "service-plan";
   plan.revision = 1;
   plan.configVersion = "demo-0.7.0";
   plan.providerId = "model-service-test";
   plan.createdTime = "2026-08-01T00:00:00Z";
   plan.valid = true;

   nrm::NetworkPlanAllocation allocation;
   allocation.allocationId = "service-allocation";
   allocation.networkName = "service-link16";
   allocation.networkType = nrm::NetworkType::cLINK16;
   allocation.profileId = "demo-link16-v1";
   allocation.frequencyHz = 1000000000.0;
   allocation.channelId = "channel-1";
   allocation.subnetId = "subnet-1";
   allocation.slotIds.push_back("slot-1");
   allocation.memberPlatformIds = {"source", "destination"};
   allocation.routePolicyId = "route-1";
   plan.allocations.push_back(allocation);

   nrm::NetworkPlanDemand demand;
   demand.demandId = "plan-demand";
   demand.businessType = "C2";
   demand.sourcePlatform = "source";
   demand.destinationPlatform = "destination";
   demand.requiredBandwidthBps = 500.0;
   demand.maximumDelayMs = 20.0;
   demand.minimumPdrPercent = 90.0;
   demand.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   plan.demands.push_back(demand);
   return plan;
}

nrm::ResourceDemandSet DemandSet()
{
   nrm::ResourceDemandSet demandSet;
   demandSet.demandSetId = "service-demand-set";
   demandSet.revision = 1;
   demandSet.configVersion = "demo-0.7.0";
   demandSet.providerId = "model-service-test";
   demandSet.createdTime = "2026-08-01T00:00:00Z";
   demandSet.valid = true;

   nrm::ResourceDemand demand;
   demand.demandId = "resource-demand";
   demand.demandSetId = demandSet.demandSetId;
   demand.revision = demandSet.revision;
   demand.missionStage = "phase-1";
   demand.businessType = "C2";
   demand.sourcePlatform = "source";
   demand.destinationPlatform = "destination";
   demand.businessTrafficBps = 400.0;
   demand.requiredBandwidthBps = 500.0;
   demand.maximumDelayMs = 20.0;
   demand.minimumPdrPercent = 90.0;
   demand.maximumDistanceM = 2000.0;
   demand.minimumNetworkSize = 2;
   demand.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   demand.valid = true;
   demandSet.demands.push_back(demand);
   return demandSet;
}

nrm::ModelServiceContext Context(const std::string& aRequestId,
                                 std::uint64_t aSnapshotVersion = 110)
{
   nrm::ModelServiceContext context;
   context.requestId = aRequestId;
   context.correlationId = "CORRELATION-110";
   context.callerId = "model-service-test";
   context.softwareVersion = "test-caller-v1";
   context.snapshotVersion = aSnapshotVersion;
   context.requestTime = 30.0;
   context.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   context.confidence = nrm::Confidence::cHIGH;
   context.valid = true;
   return context;
}

bool HasReason(const std::vector<nrm::ModelServiceReason>& aReasons,
               nrm::ModelServiceReason aReason)
{
   return std::find(aReasons.begin(), aReasons.end(), aReason) != aReasons.end();
}

bool HasAssessmentReason(const nrm::AssessmentResult& aResult,
                         nrm::AssessmentReason aReason)
{
   return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) !=
          aResult.reasons.end();
}

class CountingCapabilityService : public nrm::CommunicationCapabilityServicePort
{
public:
   nrm::CapabilityResult Query(const nrm::ResourceSnapshot&,
                               const nrm::CapabilityRequest&,
                               const nrm::EnvironmentContext&) const override
   {
      ++calls;
      if (throwOnCall) throw std::runtime_error("expected test exception");
      return next;
   }

   mutable std::size_t calls = 0;
   bool throwOnCall = false;
   nrm::CapabilityResult next;
};

class CountingValidationService : public nrm::PlanValidationServicePort
{
public:
   nrm::PlanValidationResult Validate(const nrm::ResourceSnapshot&,
                                      const nrm::NetworkPlanDocument&) const override
   {
      ++calls;
      return next;
   }

   mutable std::size_t calls = 0;
   nrm::PlanValidationResult next;
};

class CountingEvaluationService : public nrm::PlanEvaluationServicePort
{
public:
   nrm::NetworkPlanEvaluationResult Evaluate(
      const nrm::ResourceSnapshot&,
      const nrm::NetworkPlanDocument&,
      const nrm::EnvironmentContext&) const override
   {
      ++calls;
      return next;
   }

   mutable std::size_t calls = 0;
   nrm::NetworkPlanEvaluationResult next;
};

class CountingDistributionService : public nrm::PlanDistributionServicePort
{
public:
   nrm::DistributionPackageResult Generate(
      const nrm::NetworkPlanDocument&,
      const nrm::PlanValidationResult&,
      const nrm::NetworkPlanEvaluationResult&,
      const std::string&) const override
   {
      ++calls;
      return next;
   }

   mutable std::size_t calls = 0;
   nrm::DistributionPackageResult next;
};

class CountingDemandService : public nrm::ResourceDemandMatchingServicePort
{
public:
   nrm::ResourceDemandBatchResult Evaluate(
      const nrm::ResourceSnapshot&,
      const nrm::ResourceDemandSet&,
      const nrm::NetworkPlanDocument*,
      const nrm::NetworkPlanEvaluationResult*,
      const nrm::EnvironmentContext&,
      const nrm::PlanningCandidateSet*) const override
   {
      ++calls;
      return next;
   }

   mutable std::size_t calls = 0;
   nrm::ResourceDemandBatchResult next;
};

class CountingAssessmentService : public nrm::AssessmentServicePort
{
public:
   nrm::AssessmentResult Evaluate(
      const nrm::ResourceSnapshot&,
      const nrm::AssessmentTask&) const override
   {
      ++calls;
      return next;
   }

   mutable std::size_t calls = 0;
   nrm::AssessmentResult next;
};

void RemovePackage(const nrm::DistributionPackageResult& aPackage,
                   const std::string& aOutputRoot)
{
   if (!aPackage.generated) return;
   std::remove((aPackage.outputPath + "/network_plan.nrm").c_str());
   std::remove((aPackage.outputPath + "/validation_result.json").c_str());
   std::remove((aPackage.outputPath + "/evaluation_result.json").c_str());
   std::remove((aPackage.outputPath + "/manifest.json").c_str());
   rmdir(aPackage.outputPath.c_str());
   rmdir((aOutputRoot + "/distribution_packages/service-plan").c_str());
   rmdir((aOutputRoot + "/distribution_packages").c_str());
   rmdir(aOutputRoot.c_str());
}
} // namespace

int main()
{
   const nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const nrm::ModelServiceFacade facade(profiles);
   nrm::ResourceSnapshot snapshot = Snapshot();
   nrm::NetworkPlanDocument plan = Plan();
   nrm::ResourceDemandSet demandSet = DemandSet();
   nrm::PlanningCandidateSet candidates;
   candidates.candidateSetId = "candidate-set-110";

   const std::size_t endpointCountBefore = snapshot.endpoints.size();
   const double bandwidthBefore = snapshot.links.front().bandwidthBps.value;
   const std::string planFingerprintBefore =
      nrm::network_plan_detail::PlanContentFingerprint(plan);
   const double demandBandwidthBefore =
      demandSet.demands.front().requiredBandwidthBps;
   const std::string candidateIdBefore = candidates.candidateSetId;
   const std::size_t profileCountBefore = profiles.Profiles().size();
   const double profileFrequencyBefore =
      profiles.Profiles().front().frequenciesHz.front();

   const nrm::ModelServiceContext capabilityContext = Context("SERVICE-CAPABILITY");
   const nrm::CapabilityServiceResponse capability = facade.QueryCapability(
      capabilityContext, snapshot, CapabilityRequest());
   assert(capability.valid);
   assert(capability.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(capability.operation == nrm::ModelServiceOperation::cQUERY_CAPABILITY);
   assert(capability.requestId == "SERVICE-CAPABILITY");
   assert(capability.correlationId == "CORRELATION-110");
   assert(capability.softwareVersion == nrm::cVERSION);
   assert(capability.snapshotVersion == snapshot.snapshotVersion);
   assert(capability.result.requestValid);
   assert(capability.result.pathAvailable);

   const nrm::AssessmentServiceResponse assessment = facade.EvaluateAssessment(
      Context("SERVICE-ASSESSMENT"), snapshot, AssessmentTask());
   assert(assessment.valid);
   assert(assessment.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(assessment.operation ==
          nrm::ModelServiceOperation::cEVALUATE_ASSESSMENT);
   assert(assessment.result.taskId == "ASSESSMENT-110");

   const nrm::PlanValidationServiceResponse validation = facade.ValidatePlan(
      Context("SERVICE-VALIDATION"), snapshot, plan);
   assert(validation.valid);
   assert(validation.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(validation.result.passed);
   assert(validation.result.planFingerprint == planFingerprintBefore);

   const nrm::PlanEvaluationServiceResponse evaluation = facade.EvaluatePlan(
      Context("SERVICE-EVALUATION"), snapshot, plan);
   assert(evaluation.valid);
   assert(evaluation.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(evaluation.result.overallStatus == nrm::PlanEvaluationStatus::cPASS);
   assert(evaluation.result.snapshotVersion == snapshot.snapshotVersion);

   char temporaryRoot[] = "/tmp/nrm-model-service-XXXXXX";
   const char* rootPtr = mkdtemp(temporaryRoot);
   assert(rootPtr != nullptr);
   const nrm::DistributionPackageServiceResponse package =
      facade.GenerateDistributionPackage(
         Context("SERVICE-DISTRIBUTION"), plan, validation.result,
         evaluation.result, rootPtr);
   assert(package.valid);
   assert(package.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(package.result.generated);

   const nrm::ResourceDemandServiceResponse matching =
      facade.MatchResourceDemands(
         Context("SERVICE-MATCHING"), snapshot, demandSet, nullptr, nullptr,
         nrm::EnvironmentContext(), &candidates);
   assert(matching.valid);
   assert(matching.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(matching.result.totalCount == 1);
   assert(matching.result.satisfiedCount == 1);

   const nrm::ModelHealthServiceResponse health =
      facade.GetHealth(Context("SERVICE-HEALTH"));
   assert(health.valid);
   assert(health.result.healthy);
   const nrm::ModelDescriptorServiceResponse descriptor =
      facade.GetDescriptor(Context("SERVICE-DESCRIPTOR"));
   assert(descriptor.valid);
   assert(descriptor.result.valid);
   assert(descriptor.result.modelVersion == nrm::cVERSION);

   assert(snapshot.endpoints.size() == endpointCountBefore);
   assert(snapshot.links.front().bandwidthBps.value == bandwidthBefore);
   assert(nrm::network_plan_detail::PlanContentFingerprint(plan) ==
          planFingerprintBefore);
   assert(demandSet.demands.front().requiredBandwidthBps == demandBandwidthBefore);
   assert(candidates.candidateSetId == candidateIdBefore);
   assert(profiles.Profiles().size() == profileCountBefore);
   assert(profiles.Profiles().front().frequenciesHz.front() ==
          profileFrequencyBefore);
   RemovePackage(package.result, rootPtr);

   auto capabilityPort = std::make_shared<CountingCapabilityService>();
   auto validationPort = std::make_shared<CountingValidationService>();
   auto evaluationPort = std::make_shared<CountingEvaluationService>();
   auto distributionPort = std::make_shared<CountingDistributionService>();
   auto demandPort = std::make_shared<CountingDemandService>();
   auto assessmentPort = std::make_shared<CountingAssessmentService>();
   const nrm::ModelServiceFacade countingFacade(
      profiles, capabilityPort, validationPort, evaluationPort,
      distributionPort, demandPort, assessmentPort);

   nrm::ModelServiceContext invalidContext = Context("");
   nrm::CapabilityServiceResponse rejected = countingFacade.QueryCapability(
      invalidContext, snapshot, CapabilityRequest());
   assert(rejected.status == nrm::ModelServiceStatus::cINVALID_REQUEST);
   assert(HasReason(rejected.reasons,
                    nrm::ModelServiceReason::cREQUEST_ID_EMPTY));
   assert(capabilityPort->calls == 0);

   invalidContext = Context("BAD-SCHEMA");
   invalidContext.schemaVersion = "nrm.model_service.request.v0";
   const nrm::PlanValidationServiceResponse badSchema =
      countingFacade.ValidatePlan(invalidContext, snapshot, plan);
   assert(badSchema.status == nrm::ModelServiceStatus::cUNSUPPORTED_SCHEMA);
   assert(validationPort->calls == 0);

   invalidContext = Context("BAD-TIME");
   invalidContext.requestTime = std::numeric_limits<double>::quiet_NaN();
   const nrm::CapabilityServiceResponse badTime =
      countingFacade.QueryCapability(invalidContext, snapshot, CapabilityRequest());
   assert(badTime.status == nrm::ModelServiceStatus::cINVALID_REQUEST);
   assert(HasReason(badTime.reasons,
                    nrm::ModelServiceReason::cREQUEST_TIME_INVALID));
   assert(capabilityPort->calls == 0);

   invalidContext = Context("INVALID-CONTEXT");
   invalidContext.valid = false;
   const nrm::CapabilityServiceResponse badContext =
      countingFacade.QueryCapability(invalidContext, snapshot, CapabilityRequest());
   assert(badContext.status == nrm::ModelServiceStatus::cINVALID_REQUEST);
   assert(HasReason(badContext.reasons,
                    nrm::ModelServiceReason::cCONTEXT_INVALID));
   assert(capabilityPort->calls == 0);

   invalidContext = Context("BAD-EVIDENCE", snapshot.snapshotVersion + 1);
   const nrm::PlanEvaluationServiceResponse badSnapshot =
      countingFacade.EvaluatePlan(invalidContext, snapshot, plan);
   assert(badSnapshot.status == nrm::ModelServiceStatus::cEVIDENCE_MISMATCH);
   assert(HasReason(badSnapshot.reasons,
                    nrm::ModelServiceReason::cSNAPSHOT_VERSION_MISMATCH));
   assert(evaluationPort->calls == 0);

   nrm::PlanValidationResult fakeValidation = validation.result;
   nrm::NetworkPlanEvaluationResult fakeEvaluation = evaluation.result;
   fakeValidation.planFingerprint = "stale";
   const nrm::DistributionPackageServiceResponse staleDistribution =
      countingFacade.GenerateDistributionPackage(
         Context("STALE-DISTRIBUTION"), plan, fakeValidation,
         fakeEvaluation, "/tmp/not-used");
   assert(staleDistribution.status ==
          nrm::ModelServiceStatus::cEVIDENCE_MISMATCH);
   assert(distributionPort->calls == 0);

   nrm::NetworkPlanEvaluationResult staleDemandEvaluation = evaluation.result;
   staleDemandEvaluation.snapshotVersion = snapshot.snapshotVersion + 1;
   const nrm::ResourceDemandServiceResponse staleDemand =
      countingFacade.MatchResourceDemands(
         Context("STALE-DEMAND"), snapshot, demandSet, &plan,
         &staleDemandEvaluation);
   assert(staleDemand.status == nrm::ModelServiceStatus::cEVIDENCE_MISMATCH);
   assert(HasReason(staleDemand.reasons,
                    nrm::ModelServiceReason::cSNAPSHOT_VERSION_MISMATCH));
   assert(demandPort->calls == 0);

   const nrm::CapabilityServiceResponse countedCapability =
      countingFacade.QueryCapability(
         Context("COUNT-CAPABILITY"), snapshot, CapabilityRequest());
   assert(countedCapability.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(capabilityPort->calls == 1);
   const nrm::PlanValidationServiceResponse countedValidation =
      countingFacade.ValidatePlan(Context("COUNT-VALIDATION"), snapshot, plan);
   assert(countedValidation.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(validationPort->calls == 1);
   const nrm::PlanEvaluationServiceResponse countedEvaluation =
      countingFacade.EvaluatePlan(Context("COUNT-EVALUATION"), snapshot, plan);
   assert(countedEvaluation.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(evaluationPort->calls == 1);
   const nrm::DistributionPackageServiceResponse countedDistribution =
      countingFacade.GenerateDistributionPackage(
         Context("COUNT-DISTRIBUTION"), plan, validation.result,
         evaluation.result, "/tmp/not-used");
   assert(countedDistribution.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(distributionPort->calls == 1);
   const nrm::ResourceDemandServiceResponse countedDemand =
      countingFacade.MatchResourceDemands(
         Context("COUNT-DEMAND"), snapshot, demandSet);
   assert(countedDemand.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(demandPort->calls == 1);
   assessmentPort->next.taskId = "COUNTED-ASSESSMENT";
   const nrm::AssessmentServiceResponse countedAssessment =
      countingFacade.EvaluateAssessment(
         Context("COUNT-ASSESSMENT"), snapshot, AssessmentTask());
   assert(countedAssessment.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(countedAssessment.result.taskId == "COUNTED-ASSESSMENT");
   assert(assessmentPort->calls == 1);

   assessmentPort->next.taskId = "ENVIRONMENT-ASSESSMENT";
   assessmentPort->next.canEstablish = true;
   assessmentPort->next.canComplete = true;
   assessmentPort->next.stable = true;
   nrm::EnvironmentContext informationOnly;
   informationOnly.valid = true;
   informationOnly.applicationMode =
      nrm::EnvironmentApplicationMode::cINFORMATION_ONLY;
   informationOnly.applyParameterizedEffects = true;
   const std::size_t capabilityCallsBeforeEnvironment = capabilityPort->calls;
   const nrm::AssessmentServiceResponse informationAssessment =
      countingFacade.EvaluateAssessment(
         Context("ASSESSMENT-INFORMATION"), snapshot, AssessmentTask(),
         informationOnly);
   assert(informationAssessment.result.canComplete);
   assert(capabilityPort->calls == capabilityCallsBeforeEnvironment);

   nrm::EnvironmentContext alreadyIncluded = informationOnly;
   alreadyIncluded.applicationMode =
      nrm::EnvironmentApplicationMode::cALREADY_INCLUDED;
   alreadyIncluded.applyParameterizedEffects = true;
   const nrm::AssessmentServiceResponse includedAssessment =
      countingFacade.EvaluateAssessment(
         Context("ASSESSMENT-INCLUDED"), snapshot, AssessmentTask(),
         alreadyIncluded);
   assert(includedAssessment.result.canComplete);
   assert(capabilityPort->calls == capabilityCallsBeforeEnvironment);

   nrm::EnvironmentContext candidateAdjustment = informationOnly;
   candidateAdjustment.applicationMode =
      nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
   candidateAdjustment.applyParameterizedEffects = true;
   capabilityPort->next = nrm::CapabilityResult();
   capabilityPort->next.requestValid = true;
   capabilityPort->next.pathAvailable = true;
   capabilityPort->next.transmissionRateBps.valid = true;
   capabilityPort->next.transmissionRateBps.value = 400.0;
   capabilityPort->next.transmissionDelayMs.valid = true;
   capabilityPort->next.transmissionDelayMs.value = 30.0;
   capabilityPort->next.packetLossPercent.valid = true;
   capabilityPort->next.packetLossPercent.value = 15.0;
   nrm::EnvironmentEffect adjustedEffect;
   adjustedEffect.valid = true;
   adjustedEffect.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   adjustedEffect.capacityScale.valid = true;
   adjustedEffect.capacityScale.value = 0.5;
   capabilityPort->next.environmentEffects.push_back(adjustedEffect);
   const nrm::AssessmentServiceResponse adjustedAssessment =
      countingFacade.EvaluateAssessment(
         Context("ASSESSMENT-ADJUSTED"), snapshot, AssessmentTask(),
         candidateAdjustment);
   assert(capabilityPort->calls == capabilityCallsBeforeEnvironment + 1);
   assert(!adjustedAssessment.result.canComplete);
   assert(!adjustedAssessment.result.stable);
   assert(adjustedAssessment.result.bandwidthMarginBps.valid);
   assert(adjustedAssessment.result.bandwidthMarginBps.value == -100.0);
   assert(adjustedAssessment.result.delayMarginMs.value == -10.0);
   assert(adjustedAssessment.result.reliabilityMarginPercent.value == -5.0);
   assert(HasAssessmentReason(
      adjustedAssessment.result,
      nrm::AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE));

   capabilityPort->next.pathAvailable = false;
   capabilityPort->next.reasons.push_back(
      nrm::CapabilityReason::cENVIRONMENT_HARD_BLOCKED);
   const nrm::AssessmentServiceResponse blockedAssessment =
      countingFacade.EvaluateAssessment(
         Context("ASSESSMENT-BLOCKED"), snapshot, AssessmentTask(),
         candidateAdjustment);
   assert(!blockedAssessment.result.canEstablish);
   assert(!blockedAssessment.result.canComplete);
   assert(HasAssessmentReason(
      blockedAssessment.result,
      nrm::AssessmentReason::cENVIRONMENT_HARD_BLOCKED));

   capabilityPort->next = nrm::CapabilityResult();
   capabilityPort->next.requestId = "DOWNSTREAM-FAILURE";
   capabilityPort->next.requestValid = false;
   capabilityPort->next.reasons.push_back(
      nrm::CapabilityReason::cINVALID_REQUEST);
   const nrm::CapabilityServiceResponse downstreamFailure =
      countingFacade.QueryCapability(
         Context("PRESERVE-FAILURE"), snapshot, CapabilityRequest());
   assert(downstreamFailure.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(downstreamFailure.result.requestId == "DOWNSTREAM-FAILURE");
   assert(downstreamFailure.result.reasons.size() == 1);
   assert(downstreamFailure.result.reasons.front() ==
          nrm::CapabilityReason::cINVALID_REQUEST);

   capabilityPort->throwOnCall = true;
   const nrm::CapabilityServiceResponse internalError =
      countingFacade.QueryCapability(
         Context("INTERNAL-ERROR"), snapshot, CapabilityRequest());
   assert(internalError.status == nrm::ModelServiceStatus::cINTERNAL_ERROR);
   assert(!internalError.valid);
   assert(HasReason(internalError.reasons,
                    nrm::ModelServiceReason::cINTERNAL_EXCEPTION));

   const nrm::ModelServiceFacade missingDependencyFacade(
      profiles, nrm::ModelServiceFacade::CapabilityServicePtr(), validationPort,
      evaluationPort, distributionPort, demandPort, assessmentPort);
   const nrm::CapabilityServiceResponse missingDependency =
      missingDependencyFacade.QueryCapability(
         Context("MISSING-DEPENDENCY"), snapshot, CapabilityRequest());
   assert(missingDependency.status ==
          nrm::ModelServiceStatus::cSERVICE_UNAVAILABLE);
   assert(!missingDependency.valid);
   assert(HasReason(
      missingDependency.reasons,
      nrm::ModelServiceReason::cSERVICE_DEPENDENCY_UNAVAILABLE));
   return 0;
}
