/**
 * @file GatewayPolicyEngineTest.cpp
 * @brief Unit coverage for explicit directed gateway routes and per-hop policy.
 */

#include "nrm/GatewayPolicyEngine.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace
{
nrm::GatewayResourceState Capability(const char* aId,
                                     const char* aPlatform,
                                     const char* aIngress,
                                     const char* aEgress)
{
   nrm::GatewayResourceState capability;
   capability.gatewayId = aId;
   capability.platformId = aPlatform;
   capability.ingressNetworkId = aIngress;
   capability.egressNetworkId = aEgress;
   capability.ingressCommName = "ingress";
   capability.egressCommName = "egress";
   capability.allowedSourcePlatformIds.push_back("source");
   capability.allowedDestinationPlatformIds.push_back("destination");
   capability.allowedMessageTypes.push_back("MISSION_REPORT");
   capability.forwardingRateBps = 1000000.0;
   capability.processingDelayMs = 5.0;
   capability.enabled = true;
   capability.valid = true;
   return capability;
}

nrm::GatewayRouteTemplate Route()
{
   nrm::GatewayRouteTemplate route;
   route.routeId = "route-l11-cdl-via-satcom";
   route.sourcePlatformId = "source";
   route.destinationPlatformId = "destination";
   route.destinationCommName = "destination-comm";
   route.allowedMessageTypes.push_back("MISSION_REPORT");
   route.gatewayCapabilityIds.push_back("cap-l11-satcom");
   route.gatewayCapabilityIds.push_back("cap-satcom-cdl");
   route.priority = 100;
   route.enabled = true;
   route.valid = true;
   return route;
}

bool HasIssue(const nrm::GatewayCatalogValidation& aValidation,
              const char* aReason)
{
   return std::any_of(aValidation.issues.begin(), aValidation.issues.end(),
                      [aReason](const nrm::GatewayValidationIssue& aIssue)
                      { return aIssue.reasonCode == aReason; });
}
} // namespace

int main()
{
   std::vector<nrm::GatewayResourceState> capabilities;
   capabilities.push_back(
      Capability("cap-l11-satcom", "gateway-a", "net-l11", "net-satcom"));
   capabilities.push_back(
      Capability("cap-satcom-cdl", "gateway-b", "net-satcom", "net-cdl"));
   std::vector<nrm::GatewayRouteTemplate> routes{Route()};

   nrm::GatewayPolicyEngine engine;
   const nrm::GatewayCatalogValidation valid = engine.Validate(capabilities, routes);
   assert(valid.valid);
   assert(valid.issues.empty());

   nrm::GatewayMessageContext first;
   first.transferId = "source:42";
   first.routeId = routes.front().routeId;
   first.sourcePlatformId = "source";
   first.destinationPlatformId = "destination";
   first.destinationCommName = "destination-comm";
   first.messageType = "MISSION_REPORT";
   first.actualOriginatorPlatformId = "source";
   first.actualIngressNetworkId = "net-l11";
   first.actualIngressCommName = "ingress";
   first.routeIndex = 0;
   first.hopCount = 0;
   first.ttl = 8;

   const nrm::GatewayPolicyDecision firstDecision =
      engine.Evaluate(capabilities, routes, first, "gateway-a");
   assert(firstDecision.action == nrm::GatewayPolicyAction::cFORWARD);
   assert(firstDecision.reasonCode == "FORWARD_ALLOWED");
   assert(firstDecision.capabilityId == "cap-l11-satcom");
   assert(firstDecision.nextCapabilityId == "cap-satcom-cdl");
   assert(!firstDecision.finalHop);
   assert(firstDecision.nextRouteIndex == 1);

   nrm::GatewayMessageContext second = first;
   second.actualIngressNetworkId = "net-satcom";
   second.actualOriginatorPlatformId = "gateway-a";
   second.routeIndex = 1;
   second.hopCount = 1;
   second.ttl = 7;
   second.trace.push_back("cap-l11-satcom");
   const nrm::GatewayPolicyDecision secondDecision =
      engine.Evaluate(capabilities, routes, second, "gateway-b");
   assert(secondDecision.action == nrm::GatewayPolicyAction::cFORWARD);
   assert(secondDecision.capabilityId == "cap-satcom-cdl");
   assert(secondDecision.nextCapabilityId.empty());
   assert(secondDecision.finalHop);
   assert(secondDecision.nextRouteIndex == 2);

   nrm::GatewayMessageContext wrongDirection = first;
   wrongDirection.actualIngressNetworkId = "net-satcom";
   assert(engine.Evaluate(capabilities, routes, wrongDirection, "gateway-a").reasonCode ==
          "INGRESS_NETWORK_MISMATCH");

   nrm::GatewayMessageContext spoofedOriginator = first;
   spoofedOriginator.actualOriginatorPlatformId = "intruder";
   assert(engine.Evaluate(capabilities, routes, spoofedOriginator,
                          "gateway-a").reasonCode ==
          "GATEWAY_ORIGINATOR_MISMATCH");

   nrm::GatewayMessageContext skippedPreviousGateway = second;
   skippedPreviousGateway.actualOriginatorPlatformId = "source";
   assert(engine.Evaluate(capabilities, routes, skippedPreviousGateway,
                          "gateway-b").reasonCode ==
          "GATEWAY_ORIGINATOR_MISMATCH");

   nrm::GatewayMessageContext wrongIngressComm = first;
   wrongIngressComm.actualIngressCommName = "other-ingress";
   assert(engine.Evaluate(capabilities, routes, wrongIngressComm, "gateway-a").reasonCode ==
          "INGRESS_COMM_MISMATCH");

   nrm::GatewayMessageContext wrongSource = first;
   wrongSource.sourcePlatformId = "intruder";
   assert(engine.Evaluate(capabilities, routes, wrongSource, "gateway-a").reasonCode ==
          "ROUTE_SOURCE_NOT_ALLOWED");

   nrm::GatewayMessageContext wrongDestination = first;
   wrongDestination.destinationPlatformId = "other-destination";
   assert(engine.Evaluate(capabilities, routes, wrongDestination, "gateway-a").reasonCode ==
          "ROUTE_DESTINATION_NOT_ALLOWED");

   nrm::GatewayMessageContext wrongType = first;
   wrongType.messageType = "UNAUTHORIZED_MESSAGE";
   assert(engine.Evaluate(capabilities, routes, wrongType, "gateway-a").reasonCode ==
          "ROUTE_MESSAGE_TYPE_NOT_ALLOWED");

   nrm::GatewayMessageContext wrongDestinationComm = first;
   wrongDestinationComm.destinationCommName = "other-comm";
   assert(engine.Evaluate(capabilities, routes, wrongDestinationComm,
                          "gateway-a").reasonCode ==
          "ROUTE_DESTINATION_COMM_MISMATCH");

   nrm::GatewayMessageContext wrongHopCount = first;
   wrongHopCount.hopCount = 1;
   assert(engine.Evaluate(capabilities, routes, wrongHopCount,
                          "gateway-a").reasonCode ==
          "GATEWAY_HOP_COUNT_MISMATCH");

   nrm::GatewayMessageContext missingTransfer = first;
   missingTransfer.transferId.clear();
   assert(engine.Evaluate(capabilities, routes, missingTransfer,
                          "gateway-a").reasonCode ==
          "GATEWAY_TRANSFER_ID_MISSING");

   nrm::GatewayMessageContext exhausted = first;
   exhausted.ttl = 0;
   assert(engine.Evaluate(capabilities, routes, exhausted, "gateway-a").reasonCode ==
          "GATEWAY_TTL_EXCEEDED");

   nrm::GatewayMessageContext loop = first;
   loop.trace.push_back("cap-l11-satcom");
   assert(engine.Evaluate(capabilities, routes, loop, "gateway-a").reasonCode ==
          "GATEWAY_LOOP_DETECTED");

   std::vector<nrm::GatewayRouteTemplate> discontinuous{Route()};
   discontinuous.front().gatewayCapabilityIds[1] = "cap-link16-cdl";
   capabilities.push_back(
      Capability("cap-link16-cdl", "gateway-c", "net-link16", "net-cdl"));
   const nrm::GatewayCatalogValidation broken =
      engine.Validate(capabilities, discontinuous);
   assert(!broken.valid);
   assert(HasIssue(broken, "GATEWAY_ROUTE_NETWORK_DISCONTINUITY"));

   std::vector<nrm::GatewayRouteTemplate> repeated{Route()};
   repeated.front().gatewayCapabilityIds[1] = "cap-l11-satcom";
   const nrm::GatewayCatalogValidation repeatedCapability =
      engine.Validate(capabilities, repeated);
   assert(!repeatedCapability.valid);
   assert(HasIssue(repeatedCapability, "GATEWAY_ROUTE_CAPABILITY_REPEATED"));

   std::vector<nrm::GatewayResourceState> duplicateCapabilities = capabilities;
   duplicateCapabilities.push_back(capabilities.front());
   const nrm::GatewayCatalogValidation duplicate =
      engine.Validate(duplicateCapabilities, routes);
   assert(!duplicate.valid);
   assert(HasIssue(duplicate, "GATEWAY_CAPABILITY_ID_DUPLICATE"));

   std::vector<nrm::GatewayRouteTemplate> missingDestinationComm{Route()};
   missingDestinationComm.front().destinationCommName.clear();
   const nrm::GatewayCatalogValidation missingComm =
      engine.Validate(capabilities, missingDestinationComm);
   assert(!missingComm.valid);
   assert(HasIssue(missingComm, "GATEWAY_ROUTE_DESTINATION_COMM_MISSING"));

   std::vector<nrm::GatewayResourceState> nonFiniteCapabilities = capabilities;
   nonFiniteCapabilities.front().forwardingRateBps =
      std::numeric_limits<double>::infinity();
   const nrm::GatewayCatalogValidation nonFinite =
      engine.Validate(nonFiniteCapabilities, routes);
   assert(!nonFinite.valid);
   assert(HasIssue(nonFinite, "GATEWAY_CAPABILITY_LIMIT_INVALID"));

   return 0;
}
