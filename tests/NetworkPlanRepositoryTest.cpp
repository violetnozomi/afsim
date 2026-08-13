#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkPlanValidator.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include <sys/stat.h>
#include <unistd.h>

namespace
{
std::string ReadAll(const std::string& aPath)
{
   std::ifstream input(aPath);
   std::ostringstream output;
   output << input.rdbuf();
   return output.str();
}

void WriteText(const std::string& aPath, const std::string& aText)
{
   std::ofstream output(aPath, std::ios::out | std::ios::trunc);
   assert(output);
   output << aText;
   assert(output.good());
}

nrm::NetworkPlanDocument ValidPlan(std::uint64_t aRevision = 1)
{
   nrm::NetworkPlanDocument plan;
   plan.planId = "plan-alpha";
   plan.revision = aRevision;
   plan.configVersion = "demo-0.7.0";
   plan.providerId = "internal-test-provider";
   plan.createdTime = "2026-08-01T12:00:00Z";
   plan.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   plan.confidence = nrm::Confidence::cLOW;
   plan.valid = true;

   nrm::NetworkPlanAllocation allocation;
   allocation.allocationId = "allocation-link16";
   allocation.networkName = "nrm_plan_link16";
   allocation.networkType = nrm::NetworkType::cLINK16;
   allocation.profileId = "demo-link16-v1";
   allocation.frequencyHz = 1000000000.0;
   allocation.channelId = "channel-1";
   allocation.subnetId = "subnet-1";
   allocation.slotIds = {"slot-1", "slot-2"};
   allocation.memberPlatformIds = {"source", "destination"};
   allocation.routePolicyId = "route-direct";
   plan.allocations.push_back(allocation);

   nrm::NetworkPlanDemand demand;
   demand.demandId = "demand-1";
   demand.businessType = "C2";
   demand.sourcePlatform = "source";
   demand.destinationPlatform = "destination";
   demand.payloadBits = 1024;
   demand.requiredBandwidthBps = 1000.0;
   demand.maximumDelayMs = 100.0;
   demand.minimumPdrPercent = 90.0;
   demand.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   plan.demands.push_back(demand);

   nrm::NetworkPlanChange change;
   change.changeId = "change-1";
   change.changeType = nrm::PlanChangeType::cLEAVE;
   change.allocationId = allocation.allocationId;
   change.platformId = "destination";
   plan.changes.push_back(change);
   return plan;
}

bool SemanticallyEqual(const nrm::NetworkPlanDocument& aLeft,
                       const nrm::NetworkPlanDocument& aRight)
{
   if (aLeft.schemaVersion != aRight.schemaVersion || aLeft.planId != aRight.planId ||
       aLeft.revision != aRight.revision ||
       aLeft.configVersion != aRight.configVersion ||
       aLeft.providerId != aRight.providerId || aLeft.createdTime != aRight.createdTime ||
       aLeft.state != aRight.state || aLeft.source != aRight.source ||
       aLeft.confidence != aRight.confidence || aLeft.valid != aRight.valid ||
       aLeft.previousPlanId != aRight.previousPlanId ||
       aLeft.previousRevision != aRight.previousRevision ||
       aLeft.allocations.size() != aRight.allocations.size() ||
       aLeft.demands.size() != aRight.demands.size() ||
       aLeft.changes.size() != aRight.changes.size())
      return false;
   const nrm::NetworkPlanAllocation& leftAllocation = aLeft.allocations.front();
   const nrm::NetworkPlanAllocation& rightAllocation = aRight.allocations.front();
   if (leftAllocation.allocationId != rightAllocation.allocationId ||
       leftAllocation.networkName != rightAllocation.networkName ||
       leftAllocation.networkType != rightAllocation.networkType ||
       leftAllocation.profileId != rightAllocation.profileId ||
       leftAllocation.frequencyHz != rightAllocation.frequencyHz ||
       leftAllocation.channelId != rightAllocation.channelId ||
       leftAllocation.subnetId != rightAllocation.subnetId ||
       leftAllocation.slotIds != rightAllocation.slotIds ||
       leftAllocation.memberPlatformIds != rightAllocation.memberPlatformIds ||
       leftAllocation.routePolicyId != rightAllocation.routePolicyId ||
       leftAllocation.enabled != rightAllocation.enabled)
      return false;
   const nrm::NetworkPlanDemand& leftDemand = aLeft.demands.front();
   const nrm::NetworkPlanDemand& rightDemand = aRight.demands.front();
   return leftDemand.demandId == rightDemand.demandId &&
          leftDemand.businessType == rightDemand.businessType &&
          leftDemand.sourcePlatform == rightDemand.sourcePlatform &&
          leftDemand.destinationPlatform == rightDemand.destinationPlatform &&
          leftDemand.payloadBits == rightDemand.payloadBits &&
          leftDemand.requiredBandwidthBps == rightDemand.requiredBandwidthBps &&
          leftDemand.maximumDelayMs == rightDemand.maximumDelayMs &&
          leftDemand.minimumPdrPercent == rightDemand.minimumPdrPercent &&
          leftDemand.allowedNetworks == rightDemand.allowedNetworks &&
          aLeft.changes.front().changeId == aRight.changes.front().changeId &&
          aLeft.changes.front().changeType == aRight.changes.front().changeType &&
          aLeft.changes.front().allocationId == aRight.changes.front().allocationId &&
          aLeft.changes.front().platformId == aRight.changes.front().platformId;
}

nrm::ResourceSnapshot ValidationSnapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 90;
   snapshot.simTime = 10.0;
   nrm::EndpointSnapshot source;
   source.endpointId = "endpoint-source";
   source.platformName = "source";
   source.state = nrm::ResourceState::cONLINE;
   snapshot.endpoints.push_back(source);
   nrm::EndpointSnapshot destination;
   destination.endpointId = "endpoint-destination";
   destination.platformName = "destination";
   destination.state = nrm::ResourceState::cONLINE;
   snapshot.endpoints.push_back(destination);
   return snapshot;
}

bool HasIssue(const nrm::PlanValidationResult& aResult,
              nrm::PlanValidationReason aReason)
{
   for (const nrm::PlanValidationIssue& issue : aResult.issues)
   {
      if (issue.reason == aReason) return true;
   }
   return false;
}
} // namespace

int main()
{
   const std::string root = "/tmp/nrm-network-plan-repository-" +
                            std::to_string(static_cast<long long>(getpid()));
   assert(mkdir(root.c_str(), 0700) == 0);
   const std::string planPath = root + "/plan-r1.nrm";
   const std::string roundTripPath = root + "/plan-r1-roundtrip.nrm";

   const nrm::NetworkPlanDocument original = ValidPlan();
   const nrm::PlanRepositoryResult initialSave =
      nrm::NetworkPlanRepository::SaveDocumentAtomic(original, planPath, true);
   assert(initialSave.success);

   nrm::NetworkPlanRepository repository;
   assert(repository.LoadFromFile(planPath));
   assert(repository.HasCurrentPlan());
   assert(repository.LastLoadResult().reason == nrm::PlanValidationReason::cNONE);
   const nrm::NetworkPlanDocument loaded = *repository.GetCurrentPlan();
   assert(SemanticallyEqual(original, loaded));
   assert(repository.SaveRevision(roundTripPath));

   nrm::NetworkPlanRepository roundTrip;
   assert(roundTrip.LoadFromFile(roundTripPath));
   assert(SemanticallyEqual(loaded, *roundTrip.GetCurrentPlan()));
   roundTrip.Unload();
   assert(!roundTrip.HasCurrentPlan());

   const nrm::NetworkPlanDocument beforeFailedLoad = *repository.GetCurrentPlan();
   assert(!repository.LoadFromFile(root + "/missing.nrm"));
   assert(repository.LastLoadResult().reason == nrm::PlanValidationReason::cFILE_OPEN_FAILED);
   assert(SemanticallyEqual(beforeFailedLoad, *repository.GetCurrentPlan()));

   const std::string badMagic = root + "/bad-magic.nrm";
   WriteText(badMagic, "WRONG_MAGIC \"plan\" 1\n");
   assert(!repository.LoadFromFile(badMagic));
   assert(repository.LastLoadResult().reason == nrm::PlanValidationReason::cUNSUPPORTED_SCHEMA);

   const std::string unknownRecord = root + "/unknown-record.nrm";
   WriteText(unknownRecord,
             "NRM_NETWORK_PLAN_V1 \"p2\" 1 \"demo-0.7.0\" \"provider\" "
             "\"2026-08-01T12:00:00Z\" DRAFT CUSTOMER_MODULE LOW 1 \"\" 0\n"
             "UNKNOWN \"value\"\n");
   assert(!repository.LoadFromFile(unknownRecord));
   assert(repository.LastLoadResult().reason == nrm::PlanValidationReason::cUNKNOWN_RECORD_TYPE);

   const std::string trailing = root + "/trailing.nrm";
   WriteText(trailing,
             "NRM_NETWORK_PLAN_V1 \"p3\" 1 \"demo-0.7.0\" \"provider\" "
             "\"2026-08-01T12:00:00Z\" DRAFT CUSTOMER_MODULE LOW 1 \"\" 0 extra\n");
   assert(!repository.LoadFromFile(trailing));
   assert(repository.LastLoadResult().reason == nrm::PlanValidationReason::cTRAILING_TOKEN);

   const std::string nonFinite = root + "/non-finite.nrm";
   WriteText(nonFinite,
             "NRM_NETWORK_PLAN_V1 \"p4\" 1 \"demo-0.7.0\" \"provider\" "
             "\"2026-08-01T12:00:00Z\" DRAFT CUSTOMER_MODULE LOW 1 \"\" 0\n"
             "ALLOCATION \"a\" \"network\" LINK16 \"demo-link16-v1\" nan "
             "\"c\" \"s\" \"r\" 1\n");
   assert(!repository.LoadFromFile(nonFinite));

   const std::string duplicateAllocation = root + "/duplicate-allocation.nrm";
   WriteText(duplicateAllocation,
             "NRM_NETWORK_PLAN_V1 \"p5\" 1 \"demo-0.7.0\" \"provider\" "
             "\"2026-08-01T12:00:00Z\" DRAFT CUSTOMER_MODULE LOW 1 \"\" 0\n"
             "ALLOCATION \"a\" \"n\" LINK16 \"demo-link16-v1\" 1000000000 "
             "\"c\" \"s\" \"r\" 1\n"
             "ALLOCATION \"a\" \"n2\" LINK16 \"demo-link16-v1\" 1000000000 "
             "\"c2\" \"s2\" \"r2\" 1\n");
   assert(!repository.LoadFromFile(duplicateAllocation));
   assert(repository.LastLoadResult().reason ==
          nrm::PlanValidationReason::cDUPLICATE_ALLOCATION_ID);

   const std::string duplicateDemand = root + "/duplicate-demand.nrm";
   WriteText(duplicateDemand,
             "NRM_NETWORK_PLAN_V1 \"p6\" 1 \"demo-0.7.0\" \"provider\" "
             "\"2026-08-01T12:00:00Z\" DRAFT CUSTOMER_MODULE LOW 1 \"\" 0\n"
             "DEMAND \"d\" \"C2\" \"source\" \"destination\" 1 0 0 0\n"
             "DEMAND \"d\" \"C2\" \"source\" \"destination\" 1 0 0 0\n");
   assert(!repository.LoadFromFile(duplicateDemand));
   assert(repository.LastLoadResult().reason ==
          nrm::PlanValidationReason::cDUPLICATE_DEMAND_ID);

   assert(!repository.LoadFromFile(planPath));
   assert(repository.LastLoadResult().reason ==
          nrm::PlanValidationReason::cDUPLICATE_PLAN_REVISION);
   assert(SemanticallyEqual(beforeFailedLoad, *repository.GetCurrentPlan()));

   repository.Unload();
   assert(!repository.HasCurrentPlan());
   assert(repository.LoadFromFile(planPath));
   assert(repository.HasCurrentPlan());
   assert(SemanticallyEqual(beforeFailedLoad, *repository.GetCurrentPlan()));

   nrm::NetworkPlanDocument unchangedRevision = ValidPlan();
   assert(!repository.ReplaceDraft(unchangedRevision));
   assert(repository.LastLoadResult().reason ==
          nrm::PlanValidationReason::cREVISION_NOT_INCREMENTED);

   nrm::NetworkPlanDocument revision2 = ValidPlan(2);
   revision2.createdTime = "2026-08-01T12:01:00Z";
   assert(repository.ReplaceDraft(revision2));
   assert(repository.GetCurrentPlan()->state == nrm::NetworkPlanState::cDRAFT);
   assert(repository.GetCurrentPlan()->previousPlanId == "plan-alpha");
   assert(repository.GetCurrentPlan()->previousRevision == 1);

   const std::string preserved = ReadAll(roundTripPath);
   assert(!repository.SaveRevision(roundTripPath));
   assert(repository.LastSaveResult().reason ==
          nrm::PlanValidationReason::cDUPLICATE_PLAN_REVISION);
   assert(ReadAll(roundTripPath) == preserved);

   const nrm::ResourceSnapshot validationSnapshot = ValidationSnapshot();
   const nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const nrm::NetworkPlanValidator validator(profiles);

   // The distributable four-network sample must remain loadable by the same
   // strict repository and validator used by the Warlock planning page.
   nrm::NetworkPlanRepository sampleRepository;
   const std::string samplePath =
      std::string(NRM_SOURCE_DIR) + "/data/network_plans/four_network_demo-r1.nrm";
   assert(sampleRepository.LoadFromFile(samplePath));
   nrm::ResourceSnapshot sampleSnapshot;
   const nrm::PlanValidationResult sampleResult =
      validator.Validate(sampleSnapshot, *sampleRepository.GetCurrentPlan());
   assert(sampleResult.passed);
   assert(sampleResult.issues.size() == 1);
   assert(sampleResult.issues.front().reason ==
          nrm::PlanValidationReason::cCUSTOMER_RULE_UNAVAILABLE);

   // The 25-node operational sample exercises multiple allocations per
   // network, twelve concurrent demands, and valid JOIN/LEAVE requests.
   nrm::NetworkPlanRepository complexRepository;
   const std::string complexPath =
      std::string(NRM_SOURCE_DIR) +
      "/data/network_plans/operational_25node_complex-r1.nrm";
   assert(complexRepository.LoadFromFile(complexPath));
   const nrm::NetworkPlanDocument& complexPlan =
      *complexRepository.GetCurrentPlan();
   assert(complexPlan.allocations.size() == 8);
   assert(complexPlan.demands.size() == 12);
   assert(complexPlan.changes.size() == 2);
   const nrm::PlanValidationResult complexResult =
      validator.Validate(sampleSnapshot, complexPlan);
   assert(complexResult.passed);
   assert(complexResult.issues.size() == 1);
   assert(complexResult.issues.front().reason ==
          nrm::PlanValidationReason::cCUSTOMER_RULE_UNAVAILABLE);

   const nrm::PlanValidationResult validResult =
      validator.Validate(validationSnapshot, ValidPlan());
   assert(validResult.passed);
   assert(validResult.issues.size() == 1);
   assert(validResult.issues.front().reason ==
          nrm::PlanValidationReason::cCUSTOMER_RULE_UNAVAILABLE);
   assert(validResult.issues.front().severity == nrm::PlanIssueSeverity::cWARNING);

   nrm::NetworkPlanDocument duplicateId = ValidPlan();
   duplicateId.allocations.push_back(duplicateId.allocations.front());
   assert(HasIssue(validator.Validate(validationSnapshot, duplicateId),
                   nrm::PlanValidationReason::cDUPLICATE_ALLOCATION_ID));
   duplicateId = ValidPlan();
   duplicateId.demands.push_back(duplicateId.demands.front());
   assert(HasIssue(validator.Validate(validationSnapshot, duplicateId),
                   nrm::PlanValidationReason::cDUPLICATE_DEMAND_ID));

   nrm::NetworkPlanDocument nonFinitePlan = ValidPlan();
   nonFinitePlan.demands[0].requiredBandwidthBps =
      std::numeric_limits<double>::infinity();
   assert(HasIssue(validator.Validate(validationSnapshot, nonFinitePlan),
                   nrm::PlanValidationReason::cNON_FINITE_VALUE));
   nrm::NetworkPlanDocument negativePlan = ValidPlan();
   negativePlan.demands[0].maximumDelayMs = -1.0;
   assert(HasIssue(validator.Validate(validationSnapshot, negativePlan),
                   nrm::PlanValidationReason::cNEGATIVE_VALUE));
   negativePlan = ValidPlan();
   negativePlan.demands[0].requiredBandwidthBps = -1.0;
   assert(HasIssue(validator.Validate(validationSnapshot, negativePlan),
                   nrm::PlanValidationReason::cNEGATIVE_VALUE));
   nrm::NetworkPlanDocument pdrPlan = ValidPlan();
   pdrPlan.demands[0].minimumPdrPercent = 100.1;
   assert(HasIssue(validator.Validate(validationSnapshot, pdrPlan),
                   nrm::PlanValidationReason::cPDR_OUT_OF_RANGE));

   nrm::NetworkPlanDocument badFrequency = ValidPlan();
   badFrequency.allocations[0].frequencyHz = 999.0;
   assert(HasIssue(validator.Validate(validationSnapshot, badFrequency),
                   nrm::PlanValidationReason::cFREQUENCY_NOT_SUPPORTED));
   nrm::NetworkPlanDocument missingProfile = ValidPlan();
   missingProfile.allocations[0].profileId = "missing-profile";
   assert(HasIssue(validator.Validate(validationSnapshot, missingProfile),
                   nrm::PlanValidationReason::cPROFILE_NOT_FOUND));
   nrm::NetworkPlanDocument missingPlatform = ValidPlan();
   missingPlatform.demands[0].destinationPlatform = "unknown-platform";
   assert(HasIssue(validator.Validate(validationSnapshot, missingPlatform),
                   nrm::PlanValidationReason::cPLATFORM_NOT_FOUND));

   nrm::NetworkProfileRepository restrictedProfiles;
   std::vector<nrm::NetworkProfile> restricted = profiles.Profiles();
   for (nrm::NetworkProfile& profile : restricted)
      profile.supportedBusinessTypes = {"C2"};
   nrm::NetworkProfileValidation profileValidation;
   assert(restrictedProfiles.Replace("restricted-v1", "test-provider",
                                     restricted, profileValidation));
   nrm::NetworkPlanDocument unsupportedBusiness = ValidPlan();
   unsupportedBusiness.configVersion = "restricted-v1";
   unsupportedBusiness.demands[0].businessType = "VIDEO";
   assert(HasIssue(nrm::NetworkPlanValidator(restrictedProfiles).Validate(
                      validationSnapshot, unsupportedBusiness),
                   nrm::PlanValidationReason::cBUSINESS_TYPE_NOT_SUPPORTED));

   nrm::NetworkPlanDocument memberLimit = ValidPlan();
   memberLimit.allocations[0].memberPlatformIds.clear();
   for (std::size_t index = 0; index < 129; ++index)
      memberLimit.allocations[0].memberPlatformIds.push_back(
         "member-" + std::to_string(index));
   assert(HasIssue(validator.Validate(validationSnapshot, memberLimit),
                   nrm::PlanValidationReason::cMEMBER_LIMIT_EXCEEDED));
   nrm::NetworkPlanDocument duplicateMember = ValidPlan();
   duplicateMember.allocations[0].memberPlatformIds.push_back("source");
   assert(HasIssue(validator.Validate(validationSnapshot, duplicateMember),
                   nrm::PlanValidationReason::cDUPLICATE_MEMBER));

   nrm::NetworkPlanDocument slotConflict = ValidPlan();
   nrm::NetworkPlanAllocation secondAllocation = slotConflict.allocations.front();
   secondAllocation.allocationId = "allocation-link16-second";
   slotConflict.allocations.push_back(secondAllocation);
   assert(HasIssue(validator.Validate(validationSnapshot, slotConflict),
                   nrm::PlanValidationReason::cRESOURCE_CONFLICT));

   nrm::NetworkPlanDocument joinExisting = ValidPlan();
   nrm::NetworkPlanChange join;
   join.changeId = "change-join";
   join.changeType = nrm::PlanChangeType::cJOIN;
   join.allocationId = "allocation-link16";
   join.platformId = "destination";
   joinExisting.changes.push_back(join);
   const nrm::PlanValidationResult joinResult =
      validator.Validate(validationSnapshot, joinExisting);
   assert(HasIssue(joinResult, nrm::PlanValidationReason::cALREADY_MEMBER));
   assert(HasIssue(joinResult, nrm::PlanValidationReason::cCHANGE_CONFLICT));

   nrm::NetworkPlanDocument leaveNonMember = ValidPlan();
   leaveNonMember.changes[0].platformId = "not-a-member";
   assert(HasIssue(validator.Validate(validationSnapshot, leaveNonMember),
                   nrm::PlanValidationReason::cNOT_MEMBER));

   const std::string files[] = {planPath, roundTripPath, badMagic, unknownRecord,
                                trailing, nonFinite, duplicateAllocation, duplicateDemand};
   for (const std::string& path : files) std::remove(path.c_str());
   assert(rmdir(root.c_str()) == 0);
   return 0;
}
