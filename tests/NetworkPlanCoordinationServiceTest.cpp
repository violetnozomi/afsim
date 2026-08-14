#include "nrm/NetworkPlanCoordinationService.hpp"

#include <cassert>
#include <fstream>

namespace
{
nrm::NetworkPlanDocument Plan()
{
   nrm::NetworkPlanDocument plan;
   plan.planId = "joint-plan";
   plan.revision = 3;
   plan.planningDomain = nrm::PlanningDomain::cJOINT;
   nrm::NetworkPlanAllocation allocation;
   allocation.allocationId = "l16-primary";
   allocation.memberPlatformIds = {"command-01", "relay-01"};
   plan.allocations.push_back(allocation);
   return plan;
}

nrm::NetworkPlanChange Change(const char* aId, nrm::PlanChangeType aType,
                              const char* aPlatform)
{
   nrm::NetworkPlanChange change;
   change.changeId = aId;
   change.planId = "joint-plan";
   change.allocationId = "l16-primary";
   change.platformId = aPlatform;
   change.changeType = aType;
   return change;
}
}

int main()
{
   nrm::NetworkPlanCoordinationService service;
   const nrm::NetworkPlanDocument plan = Plan();
   const auto joined = service.ApplyMembership(
      plan, Change("join-1", nrm::PlanChangeType::cJOIN, "relay-02"));
   assert(joined.success);
   assert(joined.revisedPlan.revision == 4);
   assert(joined.revisedPlan.previousRevision == 3);
   assert(joined.revisedPlan.allocations[0].memberPlatformIds.size() == 3);
   assert(plan.allocations[0].memberPlatformIds.size() == 2);

   const auto left = service.ApplyMembership(
      joined.revisedPlan, Change("leave-1", nrm::PlanChangeType::cLEAVE, "relay-02"));
   assert(left.success);
   assert(left.revisedPlan.revision == 5);
   assert(left.revisedPlan.allocations[0].memberPlatformIds.size() == 2);

   assert(!service.ApplyMembership(
      plan, Change("join-duplicate", nrm::PlanChangeType::cJOIN, "relay-01")).success);
   assert(!service.ApplyMembership(
      plan, Change("leave-missing", nrm::PlanChangeType::cLEAVE, "missing")).success);
   nrm::NetworkPlanChange wrongPlan = Change(
      "wrong-plan", nrm::PlanChangeType::cJOIN, "relay-02");
   wrongPlan.planId = "other-plan";
   assert(!service.ApplyMembership(plan, wrongPlan).success);
   nrm::NetworkPlanChange wrongAllocation = Change(
      "wrong-allocation", nrm::PlanChangeType::cJOIN, "relay-02");
   wrongAllocation.allocationId = "missing";
   assert(!service.ApplyMembership(plan, wrongAllocation).success);

   nrm::DistributionPackageResult package;
   package.generated = true;
   package.planId = "joint-plan";
   package.revision = 3;
   package.planFingerprint = "abc123";
   const std::string ackPath = "/tmp/nrm-plan-coordination.ack";
   {
      std::ofstream output(ackPath);
      output << "NRM_PLAN_ACK_V1 \"joint-plan\" 3 \"abc123\"\n";
   }
   const auto confirmed = service.AcknowledgeDistribution(package, ackPath);
   assert(confirmed.success);
   assert(confirmed.acknowledged);
   {
      std::ofstream output(ackPath);
      output << "NRM_PLAN_ACK_V1 \"joint-plan\" 3 \"wrong\"\n";
   }
   const auto mismatch = service.AcknowledgeDistribution(package, ackPath);
   assert(!mismatch.success);
   assert(mismatch.reason == nrm::PlanValidationReason::cACK_MISMATCH);
   return 0;
}
