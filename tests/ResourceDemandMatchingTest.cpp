#include "nrm/ResourceDemandMatchingService.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <string>

namespace
{
nrm::NetworkProfileRepository Profiles()
{
   nrm::NetworkProfile link16;
   link16.profileId = "matching-link16-v1";
   link16.schemaVersion = nrm::NetworkProfileRepository::cSCHEMA_VERSION;
   link16.networkType = nrm::NetworkType::cLINK16;
   link16.protocolModel = "MATCHING_TEST";
   link16.frequenciesHz = {900000000.0, 1000000000.0};
   link16.maximumRangeM = 100000.0;
   link16.establishmentDelayMs = 10.0;
   link16.candidatePdrPercent = 95.0;
   link16.serviceCapacityBps = 1000.0;
   link16.propagationModelId = "test-only";
   link16.maximumMembers = 16;
   link16.supportedBusinessTypes.push_back("C2");
   link16.source = nrm::DataOrigin::cPARAMETERIZED_MODEL;
   link16.confidence = nrm::Confidence::cLOW;

   nrm::NetworkProfile link16Video = link16;
   link16Video.profileId = "matching-link16-video-v1";
   link16Video.supportedBusinessTypes = {"VIDEO"};

   nrm::NetworkProfile satcom = link16;
   satcom.profileId = "matching-satcom-v1";
   satcom.networkType = nrm::NetworkType::cSATCOM;
   satcom.supportedBusinessTypes = {"VIDEO"};

   nrm::NetworkProfileRepository profiles;
   nrm::NetworkProfileValidation validation;
   assert(profiles.Replace("matching-v1", "matching-test",
                           {link16Video, link16, satcom}, validation));
   return profiles;
}

nrm::EndpointSnapshot Endpoint(const char* aId, const char* aPlatform)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = aId;
   endpoint.platformName = aPlatform;
   endpoint.networkName = "matching-network";
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = nrm::ResourceState::cONLINE;
   return endpoint;
}

nrm::ResourceSnapshot Snapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 100;
   snapshot.simTime = 50.0;
   snapshot.configVersion = "matching-v1";
   snapshot.endpoints.push_back(Endpoint("endpoint-source", "source"));
   snapshot.endpoints.push_back(Endpoint("endpoint-destination", "destination"));
   nrm::NetworkSnapshot network;
   network.networkId = "network-1";
   network.networkName = "matching-network";
   network.networkType = nrm::NetworkType::cLINK16;
   network.endpointCount = 2;
   network.onlineCount = 2;
   snapshot.networks.push_back(network);
   return snapshot;
}

nrm::ResourceDemand Demand()
{
   nrm::ResourceDemand demand;
   demand.demandId = "demand-1";
   demand.demandSetId = "demand-set-1";
   demand.revision = 1;
   demand.missionStage = "EXECUTE";
   demand.businessType = "C2";
   demand.sourcePlatform = "source";
   demand.destinationPlatform = "destination";
   demand.payloadBits = 1024;
   demand.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   demand.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   demand.confidence = nrm::Confidence::cMEDIUM;
   demand.valid = true;
   return demand;
}

nrm::ResourceDemandSet DemandSet(const nrm::ResourceDemand& aDemand)
{
   nrm::ResourceDemandSet demandSet;
   demandSet.demandSetId = "demand-set-1";
   demandSet.revision = 1;
   demandSet.configVersion = "matching-v1";
   demandSet.providerId = "matching-test";
   demandSet.createdTime = "2026-08-01T00:00:00Z";
   demandSet.valid = true;
   demandSet.demands.push_back(aDemand);
   return demandSet;
}

void Metric(nrm::MetricValue<double>& aMetric,
            double aValue,
            const char* aUnit)
{
   aMetric.value = aValue;
   aMetric.unit = aUnit;
   aMetric.valid = true;
   aMetric.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   aMetric.confidence = nrm::Confidence::cHIGH;
   aMetric.reason = nrm::MetricReason::cNONE;
}

nrm::CapabilityResult PassingCapability()
{
   nrm::CapabilityResult capability;
   capability.requestValid = true;
   capability.pathAvailable = true;
   capability.route = {"source", "destination"};
   capability.endpointRoute = {"endpoint-source", "endpoint-destination"};
   capability.networkSequence.push_back(nrm::NetworkType::cLINK16);
   Metric(capability.communicationDistanceM, 100.0, "m");
   Metric(capability.maximumHopDistanceM, 100.0, "m");
   Metric(capability.transmissionRateBps, 1000.0, "bit/s");
   Metric(capability.packetLossPercent, 5.0, "percent");
   Metric(capability.transmissionDelayMs, 10.0, "ms");
   return capability;
}

class CountingCapabilityQuery : public nrm::ResourceDemandCapabilityQuery
{
public:
   nrm::CapabilityResult Query(
      const nrm::ResourceSnapshot& aSnapshot,
      const nrm::CapabilityRequest& aRequest,
      const nrm::EnvironmentContext&) const override
   {
      ++calls;
      lastRequest = aRequest;
      nrm::CapabilityResult result = nextResult;
      result.requestId = aRequest.requestId;
      result.snapshotVersion = aSnapshot.snapshotVersion;
      return result;
   }

   mutable std::size_t calls = 0;
   mutable nrm::CapabilityRequest lastRequest;
   nrm::CapabilityResult nextResult = PassingCapability();
};

const nrm::RequirementCheck& Check(const nrm::ResourceDemandMatchResult& aResult,
                                   nrm::RequirementItemType aType)
{
   const auto iterator = std::find_if(
      aResult.checks.begin(), aResult.checks.end(),
      [aType](const nrm::RequirementCheck& aCheck) { return aCheck.type == aType; });
   assert(iterator != aResult.checks.end());
   return *iterator;
}

bool HasReason(const nrm::ResourceDemandMatchResult& aResult,
               nrm::ResourceDemandReason aReason)
{
   return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) !=
          aResult.reasons.end();
}

nrm::ResourceDemandMatchResult EvaluateOne(
   const nrm::ResourceDemandMatchingService& aService,
   const nrm::ResourceSnapshot& aSnapshot,
   const nrm::ResourceDemand& aDemand)
{
   const nrm::ResourceDemandBatchResult batch =
      aService.Evaluate(aSnapshot, DemandSet(aDemand));
   assert(batch.totalCount == 1);
   assert(batch.results.size() == 1);
   return batch.results.front();
}

nrm::NetworkPlanDocument Plan()
{
   nrm::NetworkPlanDocument plan;
   plan.planId = "matching-plan";
   plan.revision = 3;
   plan.configVersion = "matching-v1";
   plan.providerId = "matching-test";
   plan.createdTime = "2026-08-01T00:00:00Z";
   plan.valid = true;
   nrm::NetworkPlanAllocation allocation;
   allocation.allocationId = "allocation-1";
   allocation.networkName = "matching-network";
   allocation.networkType = nrm::NetworkType::cLINK16;
   allocation.profileId = "matching-link16-v1";
   allocation.frequencyHz = 1000000000.0;
   allocation.channelId = "channel-current";
   allocation.subnetId = "subnet-current";
   allocation.slotIds.push_back("slot-current");
   allocation.memberPlatformIds = {"source", "destination"};
   allocation.routePolicyId = "route-current";
   plan.allocations.push_back(allocation);
   return plan;
}

nrm::PlanningResourceCandidate Candidate(nrm::RecommendationType aType,
                                         const char* aId,
                                         const char* aValue)
{
   nrm::PlanningResourceCandidate candidate;
   candidate.type = aType;
   candidate.candidateId = aId;
   candidate.value = aValue;
   candidate.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   candidate.confidence = nrm::Confidence::cMEDIUM;
   candidate.valid = true;
   return candidate;
}

nrm::PlanningCandidateSet Candidates()
{
   nrm::PlanningCandidateSet candidates;
   candidates.candidateSetId = "candidate-set-1";

   nrm::PlanningResourceCandidate frequencyHigh =
      Candidate(nrm::RecommendationType::cFREQUENCY, "frequency-high", "1000 MHz");
   frequencyHigh.allocationId = "allocation-1";
   frequencyHigh.frequencyHz = 1000000000.0;
   candidates.candidates.push_back(frequencyHigh);
   nrm::PlanningResourceCandidate frequencyLow =
      Candidate(nrm::RecommendationType::cFREQUENCY, "frequency-low", "900 MHz");
   frequencyLow.allocationId = "allocation-1";
   frequencyLow.frequencyHz = 900000000.0;
   candidates.candidates.push_back(frequencyLow);

   nrm::PlanningResourceCandidate stationLow =
      Candidate(nrm::RecommendationType::cSTATION, "station-z", "station-z");
   stationLow.platformId = "station-z";
   stationLow.pathAvailable = true;
   stationLow.projectedStatus = nrm::DemandMatchStatus::cSATISFIED;
   Metric(stationLow.minimumMargin, 1.0, "normalized");
   candidates.candidates.push_back(stationLow);
   nrm::PlanningResourceCandidate stationHigh =
      Candidate(nrm::RecommendationType::cSTATION, "station-a", "station-a");
   stationHigh.platformId = "station-a";
   stationHigh.pathAvailable = true;
   stationHigh.projectedStatus = nrm::DemandMatchStatus::cSATISFIED;
   Metric(stationHigh.minimumMargin, 2.0, "normalized");
   candidates.candidates.push_back(stationHigh);

   candidates.candidates.push_back(
      Candidate(nrm::RecommendationType::cCHANNEL, "channel-z", "channel-z"));
   candidates.candidates.push_back(
      Candidate(nrm::RecommendationType::cCHANNEL, "channel-a", "channel-a"));

   nrm::PlanningResourceCandidate subnetZ =
      Candidate(nrm::RecommendationType::cSUBNET, "subnet-z", "subnet-z");
   subnetZ.supportedBusinessTypes.push_back("C2");
   subnetZ.currentMembers = 1;
   subnetZ.maximumMembers = 4;
   candidates.candidates.push_back(subnetZ);
   nrm::PlanningResourceCandidate subnetA = subnetZ;
   subnetA.candidateId = "subnet-a";
   subnetA.value = "subnet-a";
   candidates.candidates.push_back(subnetA);

   candidates.candidates.push_back(
      Candidate(nrm::RecommendationType::cTIMESLOT, "timeslot-z", "timeslot-z"));
   candidates.candidates.push_back(
      Candidate(nrm::RecommendationType::cTIMESLOT, "timeslot-a", "timeslot-a"));
   return candidates;
}

void AssertConstraintFailure(const nrm::ResourceDemandMatchingService& aService,
                             const nrm::ResourceSnapshot& aSnapshot,
                             nrm::ResourceDemand aDemand,
                             nrm::RequirementItemType aType,
                             nrm::ResourceDemandReason aReason)
{
   const nrm::ResourceDemandMatchResult result =
      EvaluateOne(aService, aSnapshot, aDemand);
   assert(result.status == nrm::DemandMatchStatus::cUNSATISFIED);
   const nrm::RequirementCheck& check = Check(result, aType);
   assert(check.applicable);
   assert(!check.passed);
   assert(check.margin.valid);
   assert(check.margin.value < 0.0);
   assert(check.reason == aReason);
   assert(HasReason(result, aReason));
}
} // namespace

int main()
{
   const nrm::NetworkProfileRepository profiles = Profiles();
   CountingCapabilityQuery query;
   const nrm::ResourceDemandMatchingService service(profiles, nullptr, &query);
   nrm::ResourceSnapshot snapshot = Snapshot();

   nrm::ResourceDemand satisfiedDemand = Demand();
   satisfiedDemand.minimumNetworkSize = 2;
   satisfiedDemand.maximumDistanceM = 200.0;
   satisfiedDemand.requiredBandwidthBps = 500.0;
   satisfiedDemand.businessTrafficBps = 600.0;
   satisfiedDemand.maximumDelayMs = 20.0;
   satisfiedDemand.minimumPdrPercent = 90.0;
   const std::size_t callsBeforeSatisfied = query.calls;
   const nrm::ResourceDemandMatchResult satisfied =
      EvaluateOne(service, snapshot, satisfiedDemand);
   assert(query.calls == callsBeforeSatisfied + 1);
   assert(query.lastRequest.requiredBandwidthBps == 600.0);
   assert(satisfied.status == nrm::DemandMatchStatus::cSATISFIED);
   assert(satisfied.checks.size() == 8);
   const nrm::RequirementItemType expectedOrder[] = {
      nrm::RequirementItemType::cPATH,
      nrm::RequirementItemType::cNETWORK_SIZE,
      nrm::RequirementItemType::cDISTANCE,
      nrm::RequirementItemType::cBANDWIDTH,
      nrm::RequirementItemType::cTRAFFIC,
      nrm::RequirementItemType::cDELAY,
      nrm::RequirementItemType::cPDR,
      nrm::RequirementItemType::cBUSINESS_TYPE};
   for (std::size_t index = 0; index < 8; ++index)
   {
      assert(satisfied.checks[index].type == expectedOrder[index]);
      assert(satisfied.checks[index].applicable);
      assert(satisfied.checks[index].passed);
      assert(satisfied.checks[index].margin.valid);
      assert(satisfied.checks[index].margin.value >= 0.0);
   }
   assert(satisfied.recommendations.size() == 6);
   assert(satisfied.recommendations[0].type == nrm::RecommendationType::cFREQUENCY);
   assert(satisfied.recommendations[5].type == nrm::RecommendationType::cROUTE);
   assert(satisfied.recommendations[5].status == nrm::RecommendationStatus::cAVAILABLE);

   nrm::ResourceDemand isolated = Demand();
   isolated.minimumNetworkSize = 3;
   AssertConstraintFailure(service, snapshot, isolated,
                           nrm::RequirementItemType::cNETWORK_SIZE,
                           nrm::ResourceDemandReason::cNETWORK_SIZE_NOT_MET);

   nrm::ResourceSnapshot multiNetworkSnapshot = snapshot;
   nrm::EndpointSnapshot satcomSource = Endpoint("sat-source", "source");
   satcomSource.networkName = "satcom-network";
   satcomSource.networkType = nrm::NetworkType::cSATCOM;
   nrm::EndpointSnapshot satcomDestination =
      Endpoint("sat-destination", "destination");
   satcomDestination.networkName = "satcom-network";
   satcomDestination.networkType = nrm::NetworkType::cSATCOM;
   multiNetworkSnapshot.endpoints.push_back(satcomSource);
   multiNetworkSnapshot.endpoints.push_back(satcomDestination);
   nrm::NetworkSnapshot satcomNetwork;
   satcomNetwork.networkId = "network-2";
   satcomNetwork.networkName = "satcom-network";
   satcomNetwork.networkType = nrm::NetworkType::cSATCOM;
   satcomNetwork.endpointCount = 2;
   satcomNetwork.onlineCount = 2;
   multiNetworkSnapshot.networks.push_back(satcomNetwork);
   nrm::ResourceDemand uniqueMemberDemand = Demand();
   uniqueMemberDemand.allowedNetworks.push_back(nrm::NetworkType::cSATCOM);
   uniqueMemberDemand.minimumNetworkSize = 3;
   AssertConstraintFailure(service, multiNetworkSnapshot, uniqueMemberDemand,
                           nrm::RequirementItemType::cNETWORK_SIZE,
                           nrm::ResourceDemandReason::cNETWORK_SIZE_NOT_MET);

   isolated = Demand();
   isolated.maximumDistanceM = 99.0;
   AssertConstraintFailure(service, snapshot, isolated,
                           nrm::RequirementItemType::cDISTANCE,
                           nrm::ResourceDemandReason::cDISTANCE_NOT_MET);
   isolated = Demand();
   isolated.requiredBandwidthBps = 1001.0;
   AssertConstraintFailure(service, snapshot, isolated,
                           nrm::RequirementItemType::cBANDWIDTH,
                           nrm::ResourceDemandReason::cBANDWIDTH_NOT_MET);
   isolated = Demand();
   isolated.businessTrafficBps = 1001.0;
   AssertConstraintFailure(service, snapshot, isolated,
                           nrm::RequirementItemType::cTRAFFIC,
                           nrm::ResourceDemandReason::cTRAFFIC_NOT_MET);
   isolated = Demand();
   isolated.maximumDelayMs = 9.0;
   AssertConstraintFailure(service, snapshot, isolated,
                           nrm::RequirementItemType::cDELAY,
                           nrm::ResourceDemandReason::cDELAY_NOT_MET);
   isolated = Demand();
   isolated.minimumPdrPercent = 96.0;
   AssertConstraintFailure(service, snapshot, isolated,
                           nrm::RequirementItemType::cPDR,
                           nrm::ResourceDemandReason::cPDR_NOT_MET);

   query.nextResult = PassingCapability();
   query.nextResult.pathAvailable = false;
   query.nextResult.route.clear();
   query.nextResult.endpointRoute.clear();
   query.nextResult.reasons.push_back(nrm::CapabilityReason::cNO_PATH);
   const nrm::ResourceDemandMatchResult noPath =
      EvaluateOne(service, snapshot, Demand());
   assert(noPath.status == nrm::DemandMatchStatus::cUNSATISFIED);
   assert(Check(noPath, nrm::RequirementItemType::cPATH).reason ==
          nrm::ResourceDemandReason::cPATH_UNAVAILABLE);
   assert(noPath.recommendations.size() == 6);
   for (std::size_t index = 0; index < 5; ++index)
   {
      assert(noPath.recommendations[index].status ==
             nrm::RecommendationStatus::cUNAVAILABLE);
      assert(noPath.recommendations[index].reason ==
             nrm::ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE);
   }
   assert(noPath.recommendations[5].reason ==
          nrm::ResourceDemandReason::cROUTE_UNAVAILABLE);

   query.nextResult = PassingCapability();
   query.nextResult.transmissionRateBps.valid = false;
   query.nextResult.transmissionRateBps.reason = nrm::MetricReason::cNO_SAMPLES;
   const nrm::ResourceDemandMatchResult unusedMetric =
      EvaluateOne(service, snapshot, Demand());
   assert(unusedMetric.status == nrm::DemandMatchStatus::cSATISFIED);
   assert(Check(unusedMetric, nrm::RequirementItemType::cPATH).passed);
   assert(!Check(unusedMetric, nrm::RequirementItemType::cBANDWIDTH).applicable);

   query.nextResult = PassingCapability();
   query.nextResult.networkSequence.push_back(nrm::NetworkType::cSATCOM);
   nrm::ResourceDemand mixedBusiness = Demand();
   mixedBusiness.allowedNetworks.push_back(nrm::NetworkType::cSATCOM);
   AssertConstraintFailure(service, snapshot, mixedBusiness,
                           nrm::RequirementItemType::cBUSINESS_TYPE,
                           nrm::ResourceDemandReason::cBUSINESS_TYPE_NOT_SUPPORTED);

   query.nextResult = PassingCapability();
   query.nextResult.transmissionRateBps.valid = false;
   query.nextResult.transmissionRateBps.reason = nrm::MetricReason::cNO_SAMPLES;
   nrm::ResourceDemand bandwidthDemand = Demand();
   bandwidthDemand.requiredBandwidthBps = 1.0;
   const nrm::ResourceDemandMatchResult invalidMetric =
      EvaluateOne(service, snapshot, bandwidthDemand);
   assert(invalidMetric.status == nrm::DemandMatchStatus::cDATA_INVALID);
   assert(!Check(invalidMetric, nrm::RequirementItemType::cBANDWIDTH).margin.valid);
   assert(HasReason(invalidMetric, nrm::ResourceDemandReason::cMETRIC_UNAVAILABLE));

   query.nextResult = PassingCapability();
   nrm::ResourceDemand badNumeric = Demand();
   badNumeric.maximumDelayMs = std::numeric_limits<double>::infinity();
   const std::size_t callsBeforeInvalid = query.calls;
   const nrm::ResourceDemandMatchResult invalidDemand =
      EvaluateOne(service, snapshot, badNumeric);
   assert(invalidDemand.status == nrm::DemandMatchStatus::cDATA_INVALID);
   assert(HasReason(invalidDemand, nrm::ResourceDemandReason::cNON_FINITE_VALUE));
   assert(query.calls == callsBeforeInvalid);

   nrm::NetworkPlanDocument plan = Plan();
   nrm::NetworkPlanEvaluationResult evaluation;
   evaluation.planId = plan.planId;
   evaluation.revision = plan.revision;
   evaluation.planFingerprint = nrm::network_plan_detail::PlanContentFingerprint(plan);
   evaluation.snapshotVersion = snapshot.snapshotVersion + 1;
   evaluation.validation.planId = plan.planId;
   evaluation.validation.revision = plan.revision;
   evaluation.validation.planFingerprint = evaluation.planFingerprint;
   const std::size_t callsBeforeMismatch = query.calls;
   const nrm::ResourceDemandBatchResult mismatch = service.Evaluate(
      snapshot, DemandSet(Demand()), &plan, &evaluation);
   assert(mismatch.results[0].status == nrm::DemandMatchStatus::cDATA_INVALID);
   assert(HasReason(mismatch.results[0],
                    nrm::ResourceDemandReason::cPLAN_EVIDENCE_MISMATCH));
   assert(query.calls == callsBeforeMismatch);
   for (const nrm::PlanningRecommendation& recommendation :
        mismatch.results[0].recommendations)
      assert(recommendation.reason ==
             nrm::ResourceDemandReason::cPLAN_EVIDENCE_MISMATCH);

   const nrm::PlanningCandidateSet candidates = Candidates();
   const nrm::ResourceDemandSet immutableDemandSet = DemandSet(satisfiedDemand);
   const std::string planFingerprintBefore =
      nrm::network_plan_detail::PlanContentFingerprint(plan);
   const std::uint64_t snapshotVersionBefore = snapshot.snapshotVersion;
   const std::size_t endpointCountBefore = snapshot.endpoints.size();
   const std::string demandIdBefore = immutableDemandSet.demands[0].demandId;
   const std::vector<double> profileFrequenciesBefore =
      profiles.Profiles()[0].frequenciesHz;
   const nrm::ResourceDemandBatchResult recommended = service.Evaluate(
      snapshot, immutableDemandSet, &plan, nullptr, nrm::EnvironmentContext(),
      &candidates);
   assert(recommended.results[0].status == nrm::DemandMatchStatus::cSATISFIED);
   const std::vector<nrm::PlanningRecommendation>& recommendations =
      recommended.results[0].recommendations;
   assert(recommendations.size() == 6);
   assert(recommendations[0].candidateId == "frequency-low");
   assert(recommendations[1].candidateId == "station-a");
   assert(recommendations[2].candidateId == "channel-a");
   assert(recommendations[3].candidateId == "subnet-a");
   assert(recommendations[4].candidateId == "timeslot-a");
   assert(recommendations[5].value == "source -> destination");
   for (const nrm::PlanningRecommendation& recommendation : recommendations)
   {
      assert(recommendation.status == nrm::RecommendationStatus::cAVAILABLE);
      assert(recommendation.reason == nrm::ResourceDemandReason::cNONE);
      assert(recommendation.snapshotVersion == snapshot.snapshotVersion);
      assert(recommendation.planFingerprint == planFingerprintBefore);
   }

   assert(snapshot.snapshotVersion == snapshotVersionBefore);
   assert(snapshot.endpoints.size() == endpointCountBefore);
   assert(nrm::network_plan_detail::PlanContentFingerprint(plan) ==
          planFingerprintBefore);
   assert(immutableDemandSet.demands[0].demandId == demandIdBefore);
   assert(profiles.Profiles()[0].frequenciesHz == profileFrequenciesBefore);

   return 0;
}
