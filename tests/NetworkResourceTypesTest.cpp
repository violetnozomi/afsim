#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/NetworkTypeUtils.hpp"

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
   return 0;
}
