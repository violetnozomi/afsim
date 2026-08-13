#include "nrm/ContractMetricEnricher.hpp"

#include <cassert>

int main()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.simTime = 10.0;
   nrm::EndpointSnapshot endpoint;
   endpoint.platformName = "l11_control";
   endpoint.networkType = nrm::NetworkType::cLINK11;
   snapshot.endpoints.push_back(endpoint);
   nrm::LinkSnapshot link;
   link.networkType = nrm::NetworkType::cLINK11;
   nrm::WindowMetrics window;
   window.windowS = 10.0;
   window.messages.queueDepth = 32;
   window.averageTransportDelayMs.value = 80.0;
   window.averageTransportDelayMs.valid = true;
   link.windows.push_back(window);
   snapshot.links.push_back(link);

   nrm::ContractMetricEnricher().Apply(snapshot);
   assert(snapshot.endpoints[0].memberRole == "NET_CONTROL_STATION");
   assert(snapshot.endpoints[0].memberRoleOrigin == nrm::DataOrigin::cESTIMATED);
   assert(snapshot.links[0].availableFrequenciesHz.size() == 1);
   assert(snapshot.links[0].availableFrequenciesHz[0] == 225000000.0);
   assert(snapshot.links[0].queueLimit == 128);
   assert(snapshot.links[0].windows[0].queueUtilizationPercent.valid);
   assert(snapshot.links[0].windows[0].queueUtilizationPercent.value == 25.0);
   assert(snapshot.links[0].windows[0].ackDelayMs.valid);
   assert(snapshot.links[0].windows[0].responseDelayMs.valid);
   assert(snapshot.links[0].windows[0].rttMs.valid);
   assert(snapshot.links[0].windows[0].rttMs.value == 160.0);
   assert(snapshot.links[0].windows[0].rttMs.origin == nrm::DataOrigin::cESTIMATED);

   nrm::ResourceSnapshot explicitSnapshot;
   nrm::EndpointSnapshot explicitEndpoint;
   explicitEndpoint.memberRole = "CUSTOMER_DEFINED_ROLE";
   explicitEndpoint.memberRoleOrigin = nrm::DataOrigin::cAFSIM_INTERNAL;
   explicitEndpoint.memberRoleConfidence = nrm::Confidence::cHIGH;
   explicitSnapshot.endpoints.push_back(explicitEndpoint);
   nrm::LinkSnapshot observedLink;
   nrm::WindowMetrics observedWindow;
   observedWindow.ackDelayMs.value = 12.0;
   observedWindow.ackDelayMs.valid = true;
   observedWindow.ackDelayMs.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   observedLink.windows.push_back(observedWindow);
   explicitSnapshot.links.push_back(observedLink);
   nrm::ContractMetricEnricher().Apply(explicitSnapshot);
   assert(explicitSnapshot.endpoints[0].memberRole == "CUSTOMER_DEFINED_ROLE");
   assert(explicitSnapshot.endpoints[0].memberRoleOrigin == nrm::DataOrigin::cAFSIM_INTERNAL);
   assert(explicitSnapshot.links[0].windows[0].ackDelayMs.value == 12.0);
   assert(explicitSnapshot.links[0].windows[0].ackDelayMs.origin == nrm::DataOrigin::cAFSIM_INTERNAL);
   return 0;
}
