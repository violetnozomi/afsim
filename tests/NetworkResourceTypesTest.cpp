#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/NetworkTypeUtils.hpp"
#include "nrm/RollingMetrics.hpp"

#include <cassert>

int main()
{
   nrm::FrameworkSnapshot snapshot;
   assert(snapshot.snapshotVersion == 0);
   assert(snapshot.runtimeState == nrm::RuntimeState::cIDLE);
   assert(snapshot.networks.empty());
   assert(snapshot.endpoints.empty());
   assert(snapshot.links.empty());
   assert(snapshot.messages.transmitted == 0);
   assert(snapshot.messages.received == 0);
   assert(snapshot.messages.hops == 0);

   snapshot.runtimeState = nrm::RuntimeState::cRUNNING;
   snapshot.networks.resize(4);
   assert(snapshot.runtimeState == nrm::RuntimeState::cRUNNING);
   assert(snapshot.networks.size() == 4);

   nrm::MetricValue<double> distance;
   distance.value      = 1200.0;
   distance.unit       = "m";
   distance.valid      = true;
   distance.origin     = nrm::DataOrigin::cDERIVED;
   distance.confidence = nrm::Confidence::cHIGH;
   assert(distance.valid);
   assert(distance.value == 1200.0);

   assert(nrm::ClassifyNetwork("nrm_link11_blue", "") == nrm::NetworkType::cLINK11);
   assert(nrm::ClassifyNetwork("nrm_link16_red", "") == nrm::NetworkType::cLINK16);
   assert(nrm::ClassifyNetwork("nrm_satcom_purple", "") == nrm::NetworkType::cSATCOM);
   assert(nrm::ClassifyNetwork("nrm_cdl_green", "") == nrm::NetworkType::cCDL);
   assert(nrm::ClassifyNetwork("J11_messages", "") == nrm::NetworkType::cUNKNOWN);
   assert(nrm::ClassifyNetwork("", "WSF_JTIDS_TERMINAL") == nrm::NetworkType::cLINK16);

   nrm::RollingMetrics metrics;
   metrics.RecordOnlineRatio(1.0, 1.0);
   metrics.RecordTransmit(2.0, 1000, 2.5);
   metrics.RecordReceive(2.1, 1000, 100.0);
   metrics.RecordDiscard(2.2);
   const nrm::WindowMetrics window = metrics.Snapshot(3.0, 10.0, 1000.0);
   assert(window.messages.transmitted == 1);
   assert(window.messages.received == 1);
   assert(window.messages.discarded == 1);
   assert(window.throughputBps.valid && window.throughputBps.value == 100.0);
   assert(window.pdrPercent.valid && window.pdrPercent.value == 100.0);
   assert(window.averageQueueDelayMs.valid && window.averageQueueDelayMs.value == 2.5);
   assert(window.averageTransportDelayMs.valid && window.averageTransportDelayMs.value == 100.0);
   assert(window.onlineRatioPercent.valid && window.onlineRatioPercent.value == 100.0);
   assert(window.utilizationPercent.valid && window.utilizationPercent.value == 10.0);

   const nrm::WindowMetrics emptyWindow = metrics.Snapshot(70.0, 1.0);
   assert(emptyWindow.throughputBps.valid && emptyWindow.throughputBps.value == 0.0);
   assert(!emptyWindow.pdrPercent.valid);
   assert(!emptyWindow.averageQueueDelayMs.valid);
   return 0;
}
