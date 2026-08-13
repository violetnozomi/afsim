/**
 * @file ConcurrentTaskAssessment.hpp
 * @brief Deterministic joint assessment with temporary directed-link bandwidth reservations.
 */

#ifndef NRM_CONCURRENT_TASK_ASSESSMENT_HPP
#define NRM_CONCURRENT_TASK_ASSESSMENT_HPP

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "nrm/AssessmentEvaluator.hpp"
#include "nrm/DegradationPolicy.hpp"

namespace nrm
{
enum class ConcurrentResourceReason
{
   cNONE,
   cBANDWIDTH_EXHAUSTED,
   cPOLLING_UNIT_EXHAUSTED,
   cTIMESLOT_EXHAUSTED,
   cSATCOM_RESOURCE_EXHAUSTED,
   cCDL_RESOURCE_EXHAUSTED
};

inline const char* ToString(ConcurrentResourceReason aReason)
{
   switch (aReason)
   {
   case ConcurrentResourceReason::cNONE: return "NONE";
   case ConcurrentResourceReason::cBANDWIDTH_EXHAUSTED: return "BANDWIDTH_EXHAUSTED";
   case ConcurrentResourceReason::cPOLLING_UNIT_EXHAUSTED: return "POLLING_UNIT_EXHAUSTED";
   case ConcurrentResourceReason::cTIMESLOT_EXHAUSTED: return "TIMESLOT_EXHAUSTED";
   case ConcurrentResourceReason::cSATCOM_RESOURCE_EXHAUSTED: return "SATCOM_RESOURCE_EXHAUSTED";
   case ConcurrentResourceReason::cCDL_RESOURCE_EXHAUSTED: return "CDL_RESOURCE_EXHAUSTED";
   }
   return "NONE";
}

struct ProtocolResourceDefaults
{
   std::size_t link11PollingUnits = 8;
   std::size_t link16Slots = 16;
   std::size_t satcomBeams = 4;
   std::size_t satcomChannelsPerBeam = 2;
   std::size_t cdlConcurrentLinks = 4;
   std::size_t cdlChannels = 8;

   static ProtocolResourceDefaults AcceptanceDefaults()
   {
      return ProtocolResourceDefaults();
   }
};

struct ProtocolResourceAllocation
{
   std::string kind;
   std::size_t capacity = 0;
   std::size_t used = 0;
   std::size_t remaining = 0;
};

struct ConcurrentTaskResult
{
   std::string taskId;
   std::size_t priority = 0;
   AssessmentResult independent;
   AssessmentResult concurrent;
   bool allocated = false;
   double reservedBandwidthBps = 0.0;
   ConcurrentResourceReason resourceReason = ConcurrentResourceReason::cNONE;
   ProtocolResourceAllocation protocolResource;
   std::vector<std::string> conflictingTaskIds;
};

struct ConcurrentAssessmentResult
{
   std::string schemaVersion = "nrm.concurrent_assessment.v1";
   std::uint64_t snapshotVersion = 0;
   double simTime = 0.0;
   bool valid = false;
   DegradationStatus degradation;
   std::size_t totalCount = 0;
   std::size_t allocatedCount = 0;
   std::size_t rejectedCount = 0;
   std::vector<ConcurrentTaskResult> tasks;
};

class ConcurrentTaskAssessment
{
public:
   ConcurrentTaskAssessment()
      : mEvaluator()
   {
   }

   explicit ConcurrentTaskAssessment(const NetworkProfileRepository& aProfiles)
      : mEvaluator(aProfiles)
   {
   }

   ConcurrentTaskAssessment(const NetworkProfileRepository& aProfiles,
                            const ProtocolResourceDefaults& aDefaults)
      : mEvaluator(aProfiles)
      , mDefaults(aDefaults)
   {
   }

   ConcurrentAssessmentResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const std::vector<AssessmentTask>& aTasks) const
   {
      ConcurrentAssessmentResult batch;
      batch.snapshotVersion = aSnapshot.snapshotVersion;
      batch.simTime = aSnapshot.simTime;
      batch.totalCount = aTasks.size();
      batch.degradation = DegradationPolicy().Evaluate(aSnapshot, true);
      std::set<std::string> ids;
      for (const AssessmentTask& task : aTasks)
      {
         if (task.taskId.empty() || !ids.insert(task.taskId).second)
            return batch;
      }
      if (aTasks.empty()) return batch;

      ResourceSnapshot working = aSnapshot;
      std::vector<Reservation> reservations;
      std::map<NetworkType, std::vector<std::string>> protocolOwners;
      for (std::size_t index = 0; index < aTasks.size(); ++index)
      {
         const AssessmentTask& task = aTasks[index];
         ConcurrentTaskResult item;
         item.taskId = task.taskId;
         item.priority = index + 1;
         item.independent = mEvaluator.Evaluate(aSnapshot, task);
         item.concurrent = mEvaluator.Evaluate(working, task);
         item.allocated = item.concurrent.canComplete;
         const NetworkType resourceType = item.concurrent.networkSequence.empty()
                                             ? NetworkType::cUNKNOWN
                                             : item.concurrent.networkSequence.front();
         if (item.allocated)
         {
            item.protocolResource = ResourceStateFor(
               resourceType, protocolOwners[resourceType].size());
            if (item.protocolResource.capacity > 0 &&
                item.protocolResource.used >= item.protocolResource.capacity)
            {
               item.allocated = false;
               item.concurrent.canComplete = false;
               item.resourceReason = ExhaustionReason(resourceType);
               item.conflictingTaskIds = protocolOwners[resourceType];
            }
         }
         if (item.allocated)
         {
            item.reservedBandwidthBps = task.requiredBandwidthBps;
            Reserve(working, item.concurrent.primaryEndpointRoute,
                    task.requiredBandwidthBps, task.taskId, reservations);
            protocolOwners[resourceType].push_back(task.taskId);
            item.protocolResource = ResourceStateFor(
               resourceType, protocolOwners[resourceType].size());
            ++batch.allocatedCount;
         }
         else
         {
            if (item.conflictingTaskIds.empty())
               item.conflictingTaskIds = Conflicts(
                  item.independent.primaryEndpointRoute, reservations);
            if (item.resourceReason == ConcurrentResourceReason::cNONE &&
                item.independent.canComplete && !item.concurrent.canComplete)
               item.resourceReason = ConcurrentResourceReason::cBANDWIDTH_EXHAUSTED;
            ++batch.rejectedCount;
         }
         batch.tasks.push_back(item);
      }
      batch.valid = true;
      return batch;
   }

private:
   struct Reservation
   {
      std::string sourceId;
      std::string destinationId;
      std::string taskId;
   };

   ProtocolResourceAllocation ResourceStateFor(NetworkType aType,
                                                std::size_t aUsed) const
   {
      ProtocolResourceAllocation resource;
      switch (aType)
      {
      case NetworkType::cLINK11:
         resource.kind = "POLLING_UNIT";
         resource.capacity = mDefaults.link11PollingUnits;
         break;
      case NetworkType::cLINK16:
         resource.kind = "TIMESLOT";
         resource.capacity = mDefaults.link16Slots;
         break;
      case NetworkType::cSATCOM:
         resource.kind = "BEAM_CHANNEL";
         resource.capacity = mDefaults.satcomBeams * mDefaults.satcomChannelsPerBeam;
         break;
      case NetworkType::cCDL:
         resource.kind = "CHANNEL_LINK";
         resource.capacity = std::min(mDefaults.cdlConcurrentLinks,
                                      mDefaults.cdlChannels);
         break;
      case NetworkType::cUNKNOWN:
         break;
      }
      resource.used = aUsed;
      resource.remaining = resource.capacity > resource.used
                              ? resource.capacity - resource.used : 0;
      return resource;
   }

   static ConcurrentResourceReason ExhaustionReason(NetworkType aType)
   {
      switch (aType)
      {
      case NetworkType::cLINK11: return ConcurrentResourceReason::cPOLLING_UNIT_EXHAUSTED;
      case NetworkType::cLINK16: return ConcurrentResourceReason::cTIMESLOT_EXHAUSTED;
      case NetworkType::cSATCOM: return ConcurrentResourceReason::cSATCOM_RESOURCE_EXHAUSTED;
      case NetworkType::cCDL: return ConcurrentResourceReason::cCDL_RESOURCE_EXHAUSTED;
      case NetworkType::cUNKNOWN: return ConcurrentResourceReason::cNONE;
      }
      return ConcurrentResourceReason::cNONE;
   }

   static void Reserve(ResourceSnapshot& aSnapshot,
                       const std::vector<std::string>& aEndpointRoute,
                       double aBandwidthBps,
                       const std::string& aTaskId,
                       std::vector<Reservation>& aReservations)
   {
      if (aBandwidthBps <= 0.0 || aEndpointRoute.size() < 2) return;
      for (std::size_t index = 1; index < aEndpointRoute.size(); ++index)
      {
         const std::string& source = aEndpointRoute[index - 1];
         const std::string& destination = aEndpointRoute[index];
         for (LinkSnapshot& link : aSnapshot.links)
         {
            if (link.sourceEndpointId == source &&
                link.destinationEndpointId == destination && link.bandwidthBps.valid)
            {
               link.bandwidthBps.value = std::max(0.0, link.bandwidthBps.value - aBandwidthBps);
               aReservations.push_back({source, destination, aTaskId});
               break;
            }
         }
      }
   }

   static std::vector<std::string> Conflicts(
      const std::vector<std::string>& aEndpointRoute,
      const std::vector<Reservation>& aReservations)
   {
      std::vector<std::string> result;
      for (std::size_t index = 1; index < aEndpointRoute.size(); ++index)
      {
         for (const Reservation& reservation : aReservations)
         {
            if (reservation.sourceId == aEndpointRoute[index - 1] &&
                reservation.destinationId == aEndpointRoute[index] &&
                std::find(result.begin(), result.end(), reservation.taskId) == result.end())
               result.push_back(reservation.taskId);
         }
      }
      return result;
   }

   AssessmentEvaluator mEvaluator;
   ProtocolResourceDefaults mDefaults = ProtocolResourceDefaults::AcceptanceDefaults();
};
} // namespace nrm

#endif
