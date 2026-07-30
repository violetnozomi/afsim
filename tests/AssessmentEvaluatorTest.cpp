/**
 * @file AssessmentEvaluatorTest.cpp
 * @brief Unit coverage for current-graph assessment, margins, and reason codes.
 */

#include "nrm/AssessmentEvaluator.hpp"

#include <algorithm>
#include <cassert>

namespace
{
nrm::EndpointSnapshot Endpoint(const char* aId, const char* aPlatform)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId   = aId;
   endpoint.platformName = aPlatform;
   endpoint.state        = nrm::ResourceState::cONLINE;
   endpoint.networkType  = nrm::NetworkType::cLINK16;
   return endpoint;
}

nrm::LinkSnapshot Link(const char* aId,
                       const char* aSourceId,
                       const char* aDestinationId,
                       const char* aSourcePlatform,
                       const char* aDestinationPlatform,
                       double      aDelayMs,
                       double      aPdrPercent,
                       double      aBandwidthBps)
{
   nrm::LinkSnapshot link;
   link.linkId                = aId;
   link.sourceEndpointId      = aSourceId;
   link.destinationEndpointId = aDestinationId;
   link.sourcePlatform        = aSourcePlatform;
   link.destinationPlatform   = aDestinationPlatform;
   link.networkName           = "nrm_link16_test";
   link.networkType           = nrm::NetworkType::cLINK16;
   link.state                 = nrm::ResourceState::cONLINE;
   link.bandwidthBps.value    = aBandwidthBps;
   link.bandwidthBps.valid    = true;

   nrm::WindowMetrics window;
   window.windowS                       = 10.0;
   window.averageTransportDelayMs.value = aDelayMs;
   window.averageTransportDelayMs.valid = true;
   window.pdrPercent.value              = aPdrPercent;
   window.pdrPercent.valid              = true;
   link.windows.push_back(window);
   return link;
}

bool HasReason(const nrm::AssessmentResult& aResult, nrm::AssessmentReason aReason)
{
   return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) != aResult.reasons.end();
}
} // namespace

int main()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion = 42;
   snapshot.simTime         = 20.0;
   snapshot.endpoints.push_back(Endpoint("A", "source"));
   snapshot.endpoints.push_back(Endpoint("B", "relay"));
   snapshot.endpoints.push_back(Endpoint("C", "destination"));
   snapshot.links.push_back(Link("A->B", "A", "B", "source", "relay", 20.0, 95.0, 1000.0));
   snapshot.links.push_back(Link("B->C", "B", "C", "relay", "destination", 20.0, 95.0, 800.0));

   nrm::AssessmentTask task;
   task.taskId                  = "TASK-OK";
   task.sourcePlatform          = "source";
   task.destinationPlatform     = "destination";
   task.requiredBandwidthBps    = 700.0;
   task.maximumDelayMs          = 50.0;
   task.minimumPdrPercent       = 90.0;
   task.allowedNetworks.push_back(nrm::NetworkType::cLINK16);

   nrm::AssessmentEvaluator evaluator;
   const nrm::AssessmentResult passed = evaluator.Evaluate(snapshot, task);
   assert(passed.reachable);
   assert(passed.canEstablish);
   assert(passed.canComplete);
   assert(passed.stable);
   assert(passed.primaryRoute.size() == 3);
   assert(passed.predictedDelayMs.valid && passed.predictedDelayMs.value == 40.0);
   assert(passed.estimatedPdrPercent.valid);
   assert(passed.estimatedPdrPercent.value > 90.2 && passed.estimatedPdrPercent.value < 90.3);
   assert(passed.bottleneckBandwidthBps.valid && passed.bottleneckBandwidthBps.value == 800.0);
   assert(passed.bandwidthMarginBps.value == 100.0);
   assert(passed.delayMarginMs.value == 10.0);
   assert(passed.reliabilityMarginPercent.value > 0.2);
   assert(passed.reasons.empty());

   task.taskId               = "TASK-BANDWIDTH";
   task.requiredBandwidthBps = 900.0;
   const nrm::AssessmentResult bandwidthFailed = evaluator.Evaluate(snapshot, task);
   assert(bandwidthFailed.reachable);
   assert(!bandwidthFailed.canComplete);
   assert(HasReason(bandwidthFailed, nrm::AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE));
   assert(bandwidthFailed.bandwidthMarginBps.value == -100.0);

   task.allowedNetworks.clear();
   task.allowedNetworks.push_back(nrm::NetworkType::cCDL);
   const nrm::AssessmentResult noPath = evaluator.Evaluate(snapshot, task);
   assert(!noPath.reachable);
   assert(HasReason(noPath, nrm::AssessmentReason::cNO_CURRENT_PATH));

   task.destinationPlatform = "missing";
   const nrm::AssessmentResult missing = evaluator.Evaluate(snapshot, task);
   assert(HasReason(missing, nrm::AssessmentReason::cNODE_NOT_FOUND));
   return 0;
}
