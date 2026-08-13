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

namespace nrm
{
struct ConcurrentTaskResult
{
   std::string taskId;
   std::size_t priority = 0;
   AssessmentResult independent;
   AssessmentResult concurrent;
   bool allocated = false;
   double reservedBandwidthBps = 0.0;
   std::vector<std::string> conflictingTaskIds;
};

struct ConcurrentAssessmentResult
{
   std::string schemaVersion = "nrm.concurrent_assessment.v1";
   std::uint64_t snapshotVersion = 0;
   double simTime = 0.0;
   bool valid = false;
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

   ConcurrentAssessmentResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const std::vector<AssessmentTask>& aTasks) const
   {
      ConcurrentAssessmentResult batch;
      batch.snapshotVersion = aSnapshot.snapshotVersion;
      batch.simTime = aSnapshot.simTime;
      batch.totalCount = aTasks.size();
      std::set<std::string> ids;
      for (const AssessmentTask& task : aTasks)
      {
         if (task.taskId.empty() || !ids.insert(task.taskId).second)
            return batch;
      }
      if (aTasks.empty()) return batch;

      ResourceSnapshot working = aSnapshot;
      std::vector<Reservation> reservations;
      for (std::size_t index = 0; index < aTasks.size(); ++index)
      {
         const AssessmentTask& task = aTasks[index];
         ConcurrentTaskResult item;
         item.taskId = task.taskId;
         item.priority = index + 1;
         item.independent = mEvaluator.Evaluate(aSnapshot, task);
         item.concurrent = mEvaluator.Evaluate(working, task);
         item.allocated = item.concurrent.canComplete;
         if (item.allocated)
         {
            item.reservedBandwidthBps = task.requiredBandwidthBps;
            Reserve(working, item.concurrent.primaryEndpointRoute,
                    task.requiredBandwidthBps, task.taskId, reservations);
            ++batch.allocatedCount;
         }
         else
         {
            item.conflictingTaskIds = Conflicts(
               item.independent.primaryEndpointRoute, reservations);
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
};
} // namespace nrm

#endif
