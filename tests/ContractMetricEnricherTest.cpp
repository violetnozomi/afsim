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

   nrm::ResourceSnapshot contractSnapshot;
   contractSnapshot.simTime = 20.0;
   nrm::LinkSnapshot link16;
   link16.linkId = "l16:a:b";
   link16.networkType = nrm::NetworkType::cLINK16;
   link16.state = nrm::ResourceState::cONLINE;
   link16.snrDb = {13.0, "dB", true};
   link16.ber = {0.0, "ratio", true};
   link16.protocolResource.valid = true;
   link16.protocolResource.capacity = 16;
   link16.protocolResource.used = 4;
   nrm::WindowMetrics qualityWindow;
   qualityWindow.windowS = 10.0;
   qualityWindow.pdrPercent = {80.0, "percent", true};
   qualityWindow.averageTransportDelayMs = {50.0, "ms", true};
   link16.windows.push_back(qualityWindow);
   contractSnapshot.links.push_back(link16);

   nrm::ContractMetricEnricher().Apply(contractSnapshot);
   const nrm::LinkSnapshot& enriched = contractSnapshot.links[0];
   assert(!enriched.supportedBusinessTypes.empty());
   assert(enriched.coverage.maximumRangeM.valid);
   assert(enriched.coverage.maximumRangeM.value == 500000.0);
   assert(enriched.coverage.maximumRangeM.origin == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(enriched.protocolResource.kind == "TIMESLOT");
   assert(enriched.protocolResource.remaining == 12);
   assert(enriched.protocolResource.utilizationPercent.valid);
   assert(enriched.protocolResource.utilizationPercent.value == 25.0);
   assert(enriched.communicationQualityPercent.valid);
   assert(enriched.communicationQualityPercent.value == 93.75);

   nrm::ResourceSnapshot unavailableSnapshot;
   unavailableSnapshot.simTime = 30.0;
   nrm::LinkSnapshot unavailable;
   unavailable.linkId = "sat:a:b";
   unavailable.networkType = nrm::NetworkType::cSATCOM;
   unavailable.state = nrm::ResourceState::cOFFLINE;
   unavailable.protocolResource.valid = true;
   unavailable.protocolResource.capacity = 0;
   unavailable.protocolResource.used = 0;
   unavailableSnapshot.links.push_back(unavailable);
   nrm::ContractMetricEnricher().Apply(unavailableSnapshot);
   assert(!unavailableSnapshot.links[0].protocolResource.utilizationPercent.valid);
   assert(unavailableSnapshot.links[0].protocolResource.utilizationPercent.reason ==
          nrm::MetricReason::cZERO_DENOMINATOR);
   assert(unavailableSnapshot.alarms.size() == 1);
   assert(unavailableSnapshot.alarms[0].reasonCode == "LINK_OFFLINE");
   assert(unavailableSnapshot.links[0].activeAlarmIds.size() == 1);
   assert(unavailableSnapshot.links[0].activeAlarmIds[0] == unavailableSnapshot.alarms[0].alarmId);
   return 0;
}
