#include "nrm/ConcurrentTaskAssessment.hpp"

#include <cassert>

namespace
{
void Metric(nrm::MetricValue<double>& aMetric, double aValue, const char* aUnit)
{
   aMetric.value = aValue;
   aMetric.unit = aUnit;
   aMetric.valid = true;
   aMetric.confidence = nrm::Confidence::cHIGH;
   aMetric.reason = nrm::MetricReason::cNONE;
}

nrm::EndpointSnapshot Endpoint(const char* aId, const char* aPlatform)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = aId;
   endpoint.platformName = aPlatform;
   endpoint.networkName = "joint-network";
   endpoint.networkType = nrm::NetworkType::cLINK16;
   endpoint.state = nrm::ResourceState::cONLINE;
   return endpoint;
}

nrm::LinkSnapshot Link(const char* aId, const char* aSourceId,
                       const char* aDestinationId, const char* aSource,
                       const char* aDestination)
{
   nrm::LinkSnapshot link;
   link.linkId = aId;
   link.sourceEndpointId = aSourceId;
   link.destinationEndpointId = aDestinationId;
   link.sourcePlatform = aSource;
   link.destinationPlatform = aDestination;
   link.networkName = "joint-network";
   link.networkType = nrm::NetworkType::cLINK16;
   link.state = nrm::ResourceState::cONLINE;
   Metric(link.bandwidthBps, 1000.0, "bit/s");
   Metric(link.distanceM, 1000.0, "m");
   nrm::WindowMetrics window;
   window.windowS = 10.0;
   Metric(window.averageTransportDelayMs, 10.0, "ms");
   Metric(window.deliveryRatioPercent, 99.0, "percent");
   link.windows.push_back(window);
   return link;
}

nrm::ResourceSnapshot Snapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 42;
   snapshot.simTime = 12.0;
   snapshot.endpoints = {Endpoint("a", "source-a"), Endpoint("b", "sink"),
                         Endpoint("c", "source-c"), Endpoint("d", "sink-d")};
   snapshot.links = {Link("ab", "a", "b", "source-a", "sink"),
                     Link("cb", "c", "b", "source-c", "sink"),
                     Link("cd", "c", "d", "source-c", "sink-d")};
   return snapshot;
}

nrm::AssessmentTask Task(const char* aId, const char* aSource,
                         const char* aDestination, double aBandwidth)
{
   nrm::AssessmentTask task;
   task.taskId = aId;
   task.sourcePlatform = aSource;
   task.destinationPlatform = aDestination;
   task.requiredBandwidthBps = aBandwidth;
   task.maximumDelayMs = 100.0;
   task.minimumPdrPercent = 90.0;
   task.allowedNetworks = {nrm::NetworkType::cLINK16};
   return task;
}
}

int main()
{
   const nrm::ResourceSnapshot snapshot = Snapshot();
   nrm::ConcurrentTaskAssessment evaluator;

   const auto shared = evaluator.Evaluate(
      snapshot, {Task("high", "source-a", "sink", 600.0),
                 Task("low", "source-a", "sink", 600.0)});
   assert(shared.valid);
   assert(shared.allocatedCount == 1);
   assert(shared.rejectedCount == 1);
   assert(shared.tasks[0].allocated);
   assert(shared.tasks[0].reservedBandwidthBps == 600.0);
   assert(shared.tasks[1].independent.canComplete);
   assert(!shared.tasks[1].concurrent.canComplete);
   assert(shared.tasks[1].conflictingTaskIds.size() == 1);
   assert(shared.tasks[1].conflictingTaskIds.front() == "high");
   assert(snapshot.links.front().bandwidthBps.value == 1000.0);

   const auto disjoint = evaluator.Evaluate(
      snapshot, {Task("one", "source-a", "sink", 600.0),
                 Task("two", "source-c", "sink-d", 600.0)});
   assert(disjoint.allocatedCount == 2);
   assert(disjoint.tasks[1].conflictingTaskIds.empty());

   const auto rerun = evaluator.Evaluate(
      snapshot, {Task("high", "source-a", "sink", 600.0),
                 Task("low", "source-a", "sink", 600.0)});
   assert(rerun.allocatedCount == shared.allocatedCount);
   assert(rerun.tasks[1].conflictingTaskIds == shared.tasks[1].conflictingTaskIds);

   const auto duplicate = evaluator.Evaluate(
      snapshot, {Task("same", "source-a", "sink", 1.0),
                 Task("same", "source-c", "sink-d", 1.0)});
   assert(!duplicate.valid);
   assert(duplicate.tasks.empty());
   return 0;
}
