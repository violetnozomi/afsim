#ifndef NRM_NETWORK_PLAN_VALIDATOR_HPP
#define NRM_NETWORK_PLAN_VALIDATOR_HPP

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>

#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkProfileRepository.hpp"

namespace nrm
{
class NetworkPlanValidator
{
public:
   explicit NetworkPlanValidator(const NetworkProfileRepository& aProfiles)
      : mProfiles(aProfiles)
   {
   }

   PlanValidationResult Validate(const ResourceSnapshot& aSnapshot,
                                 const NetworkPlanDocument& aPlan) const
   {
      PlanValidationResult result;
      result.planId = aPlan.planId;
      result.revision = aPlan.revision;
      result.planFingerprint = network_plan_detail::PlanContentFingerprint(aPlan);

      ValidateHeader(aPlan, result);

      std::set<std::string> snapshotPlatforms;
      for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
      {
         if (!endpoint.platformName.empty()) snapshotPlatforms.insert(endpoint.platformName);
         if (!endpoint.endpointId.empty()) snapshotPlatforms.insert(endpoint.endpointId);
      }
      std::set<std::string> plannedMembers;
      for (const NetworkPlanAllocation& allocation : aPlan.allocations)
         plannedMembers.insert(allocation.memberPlatformIds.begin(),
                               allocation.memberPlatformIds.end());

      ValidateAllocations(aPlan, result);
      ValidateDemands(aPlan, snapshotPlatforms, plannedMembers, result);
      ValidateChanges(aPlan, snapshotPlatforms, plannedMembers, result);

      result.passed = std::find_if(
         result.issues.begin(), result.issues.end(),
         [](const PlanValidationIssue& aIssue)
         {
            return aIssue.severity == PlanIssueSeverity::cERROR;
         }) == result.issues.end();
      return result;
   }

private:
   static void AddIssue(PlanValidationResult& aResult,
                        PlanValidationReason aReason,
                        const std::string& aField,
                        const std::string& aRecordId,
                        const std::string& aDescription,
                        PlanIssueSeverity aSeverity = PlanIssueSeverity::cERROR)
   {
      PlanValidationIssue issue;
      issue.reason = aReason;
      issue.field = aField;
      issue.recordId = aRecordId;
      issue.severity = aSeverity;
      issue.description = aDescription;
      aResult.issues.push_back(issue);
   }

   static bool IsSafePlanId(const std::string& aValue)
   {
      if (aValue.empty()) return false;
      for (unsigned char character : aValue)
      {
         const bool allowed = (character >= 'a' && character <= 'z') ||
                              (character >= 'A' && character <= 'Z') ||
                              (character >= '0' && character <= '9') ||
                              character == '-' || character == '_' || character == '.';
         if (!allowed) return false;
      }
      return aValue != "." && aValue != "..";
   }

   void ValidateHeader(const NetworkPlanDocument& aPlan,
                       PlanValidationResult& aResult) const
   {
      if (aPlan.schemaVersion != "nrm.network_plan.v1")
         AddIssue(aResult, PlanValidationReason::cUNSUPPORTED_SCHEMA,
                  "schemaVersion", aPlan.planId, "Unsupported internal plan schema.");
      if (!IsSafePlanId(aPlan.planId))
         AddIssue(aResult, PlanValidationReason::cINVALID_PLAN_ID,
                  "planId", aPlan.planId, "Plan ID is empty or not path safe.");
      if (aPlan.revision == 0)
         AddIssue(aResult, PlanValidationReason::cINVALID_REVISION,
                  "revision", aPlan.planId, "Revision must be greater than zero.");
      if (aPlan.providerId.empty())
         AddIssue(aResult, PlanValidationReason::cINVALID_PROVIDER_ID,
                  "providerId", aPlan.planId, "Provider ID is required.");
      if (aPlan.configVersion.empty() || aPlan.createdTime.empty())
         AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                  "header", aPlan.planId, "Config version and created time are required.");
      if (!mProfiles.Valid() || aPlan.configVersion != mProfiles.ConfigVersion())
         AddIssue(aResult, PlanValidationReason::cCONFIG_VERSION_MISMATCH,
                  "configVersion", aPlan.planId,
                  "Plan and profile repository config versions differ.");
      if (!aPlan.valid)
         AddIssue(aResult, PlanValidationReason::cDATA_INVALID,
                  "valid", aPlan.planId, "Plan source marked the document invalid.");
      if (aPlan.allocations.empty())
         AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                  "allocations", aPlan.planId, "At least one allocation is required.");
      if (aPlan.demands.empty())
         AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                  "demands", aPlan.planId, "At least one demand is required.");
      if (aPlan.previousPlanId.empty() != (aPlan.previousRevision == 0))
         AddIssue(aResult, PlanValidationReason::cREFERENCE_NOT_FOUND,
                  "previousRevision", aPlan.planId,
                  "Previous plan ID and revision must both be present or absent.");
      AddIssue(aResult, PlanValidationReason::cCUSTOMER_RULE_UNAVAILABLE,
               "customerValidationRules", aPlan.planId,
               "Customer-specific planning validation rules are unavailable.",
               PlanIssueSeverity::cWARNING);
   }

   const NetworkProfile* FindProfile(const std::string& aProfileId) const
   {
      for (const NetworkProfile& profile : mProfiles.Profiles())
      {
         if (profile.valid && profile.profileId == aProfileId) return &profile;
      }
      return nullptr;
   }

   static bool FrequencySupported(const NetworkProfile& aProfile, double aFrequencyHz)
   {
      for (double supported : aProfile.frequenciesHz)
      {
         if (supported == aFrequencyHz) return true;
      }
      return false;
   }

   void ValidateAllocations(const NetworkPlanDocument& aPlan,
                            PlanValidationResult& aResult) const
   {
      std::set<std::string> allocationIds;
      std::set<std::tuple<std::string, std::string, std::string>> resourceKeys;
      for (const NetworkPlanAllocation& allocation : aPlan.allocations)
      {
         if (allocation.allocationId.empty() || allocation.networkName.empty() ||
             allocation.profileId.empty() || allocation.channelId.empty() ||
             allocation.subnetId.empty() || allocation.routePolicyId.empty())
            AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                     "allocation", allocation.allocationId,
                     "Allocation identifiers and explicit resource values are required.");
         if (!allocationIds.insert(allocation.allocationId).second)
            AddIssue(aResult, PlanValidationReason::cDUPLICATE_ALLOCATION_ID,
                     "allocationId", allocation.allocationId,
                     "Allocation ID is duplicated.");
         if (allocation.networkType == NetworkType::cUNKNOWN)
            AddIssue(aResult, PlanValidationReason::cUNKNOWN_NETWORK_TYPE,
                     "networkType", allocation.allocationId,
                     "Allocation network type is UNKNOWN.");
         if (!std::isfinite(allocation.frequencyHz))
            AddIssue(aResult, PlanValidationReason::cNON_FINITE_VALUE,
                     "frequencyHz", allocation.allocationId,
                     "Allocation frequency is not finite.");
         else if (allocation.frequencyHz < 0.0)
            AddIssue(aResult, PlanValidationReason::cNEGATIVE_VALUE,
                     "frequencyHz", allocation.allocationId,
                     "Allocation frequency is negative.");

         const NetworkProfile* profile = FindProfile(allocation.profileId);
         if (profile == nullptr)
         {
            AddIssue(aResult, PlanValidationReason::cPROFILE_NOT_FOUND,
                     "profileId", allocation.allocationId,
                     "Allocation profile does not exist.");
         }
         else
         {
            if (profile->networkType != allocation.networkType)
               AddIssue(aResult, PlanValidationReason::cUNKNOWN_NETWORK_TYPE,
                        "networkType", allocation.allocationId,
                        "Allocation network type differs from its profile.");
            if (std::isfinite(allocation.frequencyHz) &&
                !FrequencySupported(*profile, allocation.frequencyHz))
               AddIssue(aResult, PlanValidationReason::cFREQUENCY_NOT_SUPPORTED,
                        "frequencyHz", allocation.allocationId,
                        "Frequency is not explicitly listed by the profile.");
            if (allocation.memberPlatformIds.size() > profile->maximumMembers)
               AddIssue(aResult, PlanValidationReason::cMEMBER_LIMIT_EXCEEDED,
                        "memberPlatformIds", allocation.allocationId,
                        "Allocation exceeds the profile member limit.");
         }

         std::set<std::string> members;
         for (const std::string& member : allocation.memberPlatformIds)
         {
            if (member.empty())
               AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                        "memberPlatformIds", allocation.allocationId,
                        "Member platform ID is empty.");
            else if (!members.insert(member).second)
               AddIssue(aResult, PlanValidationReason::cDUPLICATE_MEMBER,
                        "memberPlatformIds", allocation.allocationId,
                        "Platform appears more than once in the allocation.");
         }

         std::set<std::string> localSlots;
         for (const std::string& slot : allocation.slotIds)
         {
            if (slot.empty())
            {
               AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                        "slotIds", allocation.allocationId, "Slot ID is empty.");
               continue;
            }
            if (!localSlots.insert(slot).second)
               AddIssue(aResult, PlanValidationReason::cRESOURCE_CONFLICT,
                        "slotIds", allocation.allocationId,
                        "Slot is duplicated within the allocation.");
            if (allocation.enabled &&
                !resourceKeys.insert(std::make_tuple(
                   allocation.networkName, allocation.channelId, slot)).second)
               AddIssue(aResult, PlanValidationReason::cRESOURCE_CONFLICT,
                        "slotIds", allocation.allocationId,
                        "Exclusive network/channel/slot resource is duplicated.");
         }
      }
   }

   static bool PlatformResolvable(const std::string& aPlatform,
                                  const std::set<std::string>& aSnapshotPlatforms,
                                  const std::set<std::string>& aPlannedMembers)
   {
      return !aPlatform.empty() &&
             (aSnapshotPlatforms.count(aPlatform) != 0 ||
              aPlannedMembers.count(aPlatform) != 0);
   }

   void ValidateDemands(const NetworkPlanDocument& aPlan,
                        const std::set<std::string>& aSnapshotPlatforms,
                        const std::set<std::string>& aPlannedMembers,
                        PlanValidationResult& aResult) const
   {
      std::set<std::string> demandIds;
      for (const NetworkPlanDemand& demand : aPlan.demands)
      {
         if (demand.demandId.empty() || demand.businessType.empty() ||
             demand.sourcePlatform.empty() || demand.destinationPlatform.empty())
            AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                     "demand", demand.demandId, "Demand identifiers and endpoints are required.");
         if (!demandIds.insert(demand.demandId).second)
            AddIssue(aResult, PlanValidationReason::cDUPLICATE_DEMAND_ID,
                     "demandId", demand.demandId, "Demand ID is duplicated.");
         if (!std::isfinite(demand.requiredBandwidthBps) ||
             !std::isfinite(demand.maximumDelayMs) ||
             !std::isfinite(demand.minimumPdrPercent))
            AddIssue(aResult, PlanValidationReason::cNON_FINITE_VALUE,
                     "demandNumeric", demand.demandId,
                     "Demand contains a non-finite numeric value.");
         else
         {
            if (demand.requiredBandwidthBps < 0.0 || demand.maximumDelayMs < 0.0)
               AddIssue(aResult, PlanValidationReason::cNEGATIVE_VALUE,
                        "demandNumeric", demand.demandId,
                        "Demand bandwidth or delay is negative.");
            if (demand.minimumPdrPercent < 0.0 || demand.minimumPdrPercent > 100.0)
               AddIssue(aResult, PlanValidationReason::cPDR_OUT_OF_RANGE,
                        "minimumPdrPercent", demand.demandId,
                        "Demand PDR is outside zero to one hundred percent.");
         }
         if (demand.sourcePlatform == demand.destinationPlatform &&
             !demand.sourcePlatform.empty())
            AddIssue(aResult, PlanValidationReason::cSOURCE_EQUALS_DESTINATION,
                     "sourcePlatform", demand.demandId,
                     "Demand source and destination are identical.");
         if (!PlatformResolvable(demand.sourcePlatform,
                                 aSnapshotPlatforms, aPlannedMembers))
            AddIssue(aResult, PlanValidationReason::cPLATFORM_NOT_FOUND,
                     "sourcePlatform", demand.demandId,
                     "Demand source cannot be resolved.");
         if (!PlatformResolvable(demand.destinationPlatform,
                                 aSnapshotPlatforms, aPlannedMembers))
            AddIssue(aResult, PlanValidationReason::cPLATFORM_NOT_FOUND,
                     "destinationPlatform", demand.demandId,
                     "Demand destination cannot be resolved.");

         std::set<NetworkType> requestedTypes;
         for (NetworkType type : demand.allowedNetworks)
         {
            if (type == NetworkType::cUNKNOWN)
               AddIssue(aResult, PlanValidationReason::cUNKNOWN_NETWORK_TYPE,
                        "allowedNetworks", demand.demandId,
                        "Demand allowed network contains UNKNOWN.");
            if (!requestedTypes.insert(type).second)
               AddIssue(aResult, PlanValidationReason::cRESOURCE_CONFLICT,
                        "allowedNetworks", demand.demandId,
                        "Demand allowed network is duplicated.");
         }

         bool foundAllocatedType = false;
         bool foundSupportedBusiness = false;
         for (const NetworkPlanAllocation& allocation : aPlan.allocations)
         {
            if (!allocation.enabled) continue;
            const bool requested = demand.allowedNetworks.empty() ||
                                   requestedTypes.count(allocation.networkType) != 0;
            if (!requested) continue;
            foundAllocatedType = true;
            const NetworkProfile* profile = FindProfile(allocation.profileId);
            if (profile != nullptr &&
                NetworkProfileRepository::SupportsBusiness(*profile,
                                                           demand.businessType))
               foundSupportedBusiness = true;
         }
         if (!foundAllocatedType)
            AddIssue(aResult, PlanValidationReason::cALLOWED_NETWORK_NOT_ALLOCATED,
                     "allowedNetworks", demand.demandId,
                     "No enabled allocation matches the demand networks.");
         else if (!foundSupportedBusiness)
            AddIssue(aResult, PlanValidationReason::cBUSINESS_TYPE_NOT_SUPPORTED,
                     "businessType", demand.demandId,
                     "No matching allocation profile supports the business type.");
      }
   }

   static const NetworkPlanAllocation* FindAllocation(
      const NetworkPlanDocument& aPlan, const std::string& aAllocationId)
   {
      for (const NetworkPlanAllocation& allocation : aPlan.allocations)
      {
         if (allocation.allocationId == aAllocationId) return &allocation;
      }
      return nullptr;
   }

   void ValidateChanges(const NetworkPlanDocument& aPlan,
                        const std::set<std::string>& aSnapshotPlatforms,
                        const std::set<std::string>& aPlannedMembers,
                        PlanValidationResult& aResult) const
   {
      std::set<std::string> changeIds;
      std::map<std::pair<std::string, std::string>, PlanChangeType> requests;
      for (const NetworkPlanChange& change : aPlan.changes)
      {
         if (change.changeId.empty() || change.allocationId.empty() || change.platformId.empty())
            AddIssue(aResult, PlanValidationReason::cMISSING_REQUIRED_FIELD,
                     "change", change.changeId, "Change identifiers are required.");
         if (!changeIds.insert(change.changeId).second)
            AddIssue(aResult, PlanValidationReason::cDUPLICATE_CHANGE_ID,
                     "changeId", change.changeId, "Change ID is duplicated.");
         const NetworkPlanAllocation* allocation =
            FindAllocation(aPlan, change.allocationId);
         if (allocation == nullptr)
         {
            AddIssue(aResult, PlanValidationReason::cREFERENCE_NOT_FOUND,
                     "allocationId", change.changeId,
                     "Change allocation cannot be resolved.");
            continue;
         }
         if (!PlatformResolvable(change.platformId, aSnapshotPlatforms, aPlannedMembers))
            AddIssue(aResult, PlanValidationReason::cPLATFORM_NOT_FOUND,
                     "platformId", change.changeId,
                     "Change platform cannot be resolved.");
         const auto key = std::make_pair(change.allocationId, change.platformId);
         const auto existing = requests.find(key);
         if (existing != requests.end() && existing->second != change.changeType)
            AddIssue(aResult, PlanValidationReason::cCHANGE_CONFLICT,
                     "changeType", change.changeId,
                     "JOIN and LEAVE target the same allocation member.");
         else
            requests[key] = change.changeType;

         const bool isMember =
            std::find(allocation->memberPlatformIds.begin(),
                      allocation->memberPlatformIds.end(), change.platformId) !=
            allocation->memberPlatformIds.end();
         if (change.changeType == PlanChangeType::cJOIN && isMember)
            AddIssue(aResult, PlanValidationReason::cALREADY_MEMBER,
                     "changeType", change.changeId,
                     "JOIN targets an existing member.");
         if (change.changeType == PlanChangeType::cLEAVE && !isMember)
            AddIssue(aResult, PlanValidationReason::cNOT_MEMBER,
                     "changeType", change.changeId,
                     "LEAVE targets a platform outside the allocation.");
      }
   }

   NetworkProfileRepository mProfiles;
};
} // namespace nrm

#endif
