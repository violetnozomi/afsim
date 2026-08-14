/**
 * @file NetworkPlanCoordinationService.hpp
 * @brief Read-only planning revision coordination and local distribution ACK checks.
 */

#ifndef NRM_NETWORK_PLAN_COORDINATION_SERVICE_HPP
#define NRM_NETWORK_PLAN_COORDINATION_SERVICE_HPP

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>

#include "nrm/NetworkPlanTypes.hpp"

namespace nrm
{
struct PlanCoordinationResult
{
   bool success = false;
   bool acknowledged = false;
   PlanValidationReason reason = PlanValidationReason::cNONE;
   NetworkPlanDocument revisedPlan;
};

struct PlanCoordinationEvidence
{
   std::string schemaVersion = "nrm.planning_coordination.v1";
   std::string operation;
   std::string requestId;
   std::string planId;
   std::uint64_t revision = 0;
   std::string packageId;
   std::string fingerprint;
   bool success = false;
   bool acknowledged = false;
   PlanValidationReason reason = PlanValidationReason::cNONE;
};

class NetworkPlanCoordinationService
{
public:
   PlanCoordinationResult ApplyMembership(const NetworkPlanDocument& aPlan,
                                          const NetworkPlanChange& aChange) const
   {
      PlanCoordinationResult result;
      if (aChange.planId != aPlan.planId)
      {
         result.reason = PlanValidationReason::cPLAN_IDENTITY_MISMATCH;
         return result;
      }
      auto allocation = std::find_if(
         aPlan.allocations.begin(), aPlan.allocations.end(),
         [&aChange](const NetworkPlanAllocation& aValue)
         { return aValue.allocationId == aChange.allocationId; });
      if (allocation == aPlan.allocations.end())
      {
         result.reason = PlanValidationReason::cREFERENCE_NOT_FOUND;
         return result;
      }
      if (aChange.changeId.empty() || aChange.platformId.empty())
      {
         result.reason = PlanValidationReason::cMISSING_REQUIRED_FIELD;
         return result;
      }
      for (const NetworkPlanChange& previous : aPlan.changes)
         if (previous.changeId == aChange.changeId)
         {
            result.reason = PlanValidationReason::cDUPLICATE_CHANGE_ID;
            return result;
         }
      const bool member = std::find(allocation->memberPlatformIds.begin(),
                                    allocation->memberPlatformIds.end(),
                                    aChange.platformId) != allocation->memberPlatformIds.end();
      if (aChange.changeType == PlanChangeType::cJOIN && member)
      {
         result.reason = PlanValidationReason::cALREADY_MEMBER;
         return result;
      }
      if (aChange.changeType == PlanChangeType::cLEAVE && !member)
      {
         result.reason = PlanValidationReason::cNOT_MEMBER;
         return result;
      }

      result.revisedPlan = aPlan;
      result.revisedPlan.previousPlanId = aPlan.planId;
      result.revisedPlan.previousRevision = aPlan.revision;
      result.revisedPlan.revision = aPlan.revision + 1;
      result.revisedPlan.state = NetworkPlanState::cDRAFT;
      result.revisedPlan.valid = false;
      NetworkPlanAllocation& revisedAllocation = *std::find_if(
         result.revisedPlan.allocations.begin(), result.revisedPlan.allocations.end(),
         [&aChange](const NetworkPlanAllocation& aValue)
         { return aValue.allocationId == aChange.allocationId; });
      if (aChange.changeType == PlanChangeType::cJOIN)
         revisedAllocation.memberPlatformIds.push_back(aChange.platformId);
      else
         revisedAllocation.memberPlatformIds.erase(
            std::remove(revisedAllocation.memberPlatformIds.begin(),
                        revisedAllocation.memberPlatformIds.end(), aChange.platformId),
            revisedAllocation.memberPlatformIds.end());
      result.revisedPlan.changes.push_back(aChange);
      result.success = true;
      return result;
   }

   PlanCoordinationResult AcknowledgeDistribution(
      const DistributionPackageResult& aPackage, const std::string& aAckPath) const
   {
      PlanCoordinationResult result;
      std::ifstream input(aAckPath);
      std::string magic;
      std::string planId;
      std::uint64_t revision = 0;
      std::string fingerprint;
      if (!aPackage.generated || !input ||
          !(input >> magic >> std::quoted(planId) >> revision >> std::quoted(fingerprint)) ||
          magic != "NRM_PLAN_ACK_V1")
      {
         result.reason = PlanValidationReason::cACK_MISMATCH;
         return result;
      }
      input >> std::ws;
      if (!input.eof() || planId != aPackage.planId || revision != aPackage.revision ||
          fingerprint != aPackage.planFingerprint)
      {
         result.reason = PlanValidationReason::cACK_MISMATCH;
         return result;
      }
      result.success = true;
      result.acknowledged = true;
      return result;
   }
};
}

#endif
