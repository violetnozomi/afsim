/**
 * @file GatewayPolicyEngine.hpp
 * @brief Deterministic validation and per-hop authorization for gateway routes.
 */

#ifndef NRM_GATEWAY_POLICY_ENGINE_HPP
#define NRM_GATEWAY_POLICY_ENGINE_HPP

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "nrm/GatewayTypes.hpp"
#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class GatewayPolicyEngine
{
public:
   GatewayCatalogValidation Validate(
      const std::vector<GatewayResourceState>& aCapabilities,
      const std::vector<GatewayRouteTemplate>& aRoutes) const
   {
      GatewayCatalogValidation result;
      std::map<std::string, const GatewayResourceState*> capabilities;
      for (const GatewayResourceState& capability : aCapabilities)
      {
         if (capability.gatewayId.empty())
         {
            AddIssue(result, "GATEWAY_CAPABILITY_ID_EMPTY", capability.platformId,
                     "gateway capability ID must not be empty");
            continue;
         }
         if (!capabilities.emplace(capability.gatewayId, &capability).second)
         {
            AddIssue(result, "GATEWAY_CAPABILITY_ID_DUPLICATE", capability.gatewayId,
                     "gateway capability ID must be unique");
         }
         if (capability.platformId.empty() || capability.ingressNetworkId.empty() ||
             capability.egressNetworkId.empty() || capability.ingressCommName.empty() ||
             capability.egressCommName.empty())
         {
            AddIssue(result, "GATEWAY_CAPABILITY_REFERENCE_MISSING", capability.gatewayId,
                     "platform, network, and communication references are required");
         }
         if (!capability.ingressNetworkId.empty() &&
             capability.ingressNetworkId == capability.egressNetworkId)
         {
            AddIssue(result, "GATEWAY_CAPABILITY_SELF_REFERENCE", capability.gatewayId,
                     "ingress and egress networks must differ");
         }
         ValidateAllowList(result, capability.gatewayId, "SOURCE",
                           capability.allowedSourcePlatformIds);
         ValidateAllowList(result, capability.gatewayId, "DESTINATION",
                           capability.allowedDestinationPlatformIds);
         ValidateAllowList(result, capability.gatewayId, "MESSAGE_TYPE",
                           capability.allowedMessageTypes);
         if (!std::isfinite(capability.forwardingRateBps) ||
             !std::isfinite(capability.processingDelayMs) ||
             capability.forwardingRateBps <= 0.0 || capability.processingDelayMs < 0.0 ||
             capability.maxQueueMessages == 0 || capability.maxQueueBits == 0)
         {
            AddIssue(result, "GATEWAY_CAPABILITY_LIMIT_INVALID", capability.gatewayId,
                     "rate, delay, and queue limits must be positive and finite");
         }
      }

      std::set<std::string> routeIds;
      for (const GatewayRouteTemplate& route : aRoutes)
      {
         if (route.routeId.empty())
         {
            AddIssue(result, "GATEWAY_ROUTE_ID_EMPTY", route.routeId,
                     "gateway route ID must not be empty");
         }
         else if (!routeIds.insert(route.routeId).second)
         {
            AddIssue(result, "GATEWAY_ROUTE_ID_DUPLICATE", route.routeId,
                     "gateway route ID must be unique");
         }
         if (route.sourcePlatformId.empty() || route.destinationPlatformId.empty() ||
             route.sourcePlatformId == route.destinationPlatformId)
         {
            AddIssue(result, "GATEWAY_ROUTE_ENDPOINT_INVALID", route.routeId,
                     "route source and destination must be distinct non-empty platforms");
         }
         if (route.destinationCommName.empty())
         {
            AddIssue(result, "GATEWAY_ROUTE_DESTINATION_COMM_MISSING", route.routeId,
                     "route destination communication endpoint is required");
         }
         ValidateAllowList(result, route.routeId, "MESSAGE_TYPE",
                           route.allowedMessageTypes);
         if (route.gatewayCapabilityIds.empty())
         {
            AddIssue(result, "GATEWAY_ROUTE_EMPTY", route.routeId,
                     "route must reference at least one directed capability");
            continue;
         }

         std::set<std::string> seenCapabilities;
         std::set<std::string> seenPlatforms;
         const GatewayResourceState* previous = nullptr;
         for (const std::string& capabilityId : route.gatewayCapabilityIds)
         {
            if (!seenCapabilities.insert(capabilityId).second)
            {
               AddIssue(result, "GATEWAY_ROUTE_CAPABILITY_REPEATED", route.routeId,
                        "route must not repeat a directed capability");
            }
            const auto capabilityIt = capabilities.find(capabilityId);
            if (capabilityIt == capabilities.end())
            {
               AddIssue(result, "GATEWAY_ROUTE_CAPABILITY_NOT_FOUND", route.routeId,
                        "route references an unknown directed capability");
               previous = nullptr;
               continue;
            }
            const GatewayResourceState& capability = *capabilityIt->second;
            if (!seenPlatforms.insert(capability.platformId).second)
            {
               AddIssue(result, "GATEWAY_ROUTE_PLATFORM_REPEATED", route.routeId,
                        "route must not repeat a physical gateway platform");
            }
            if (previous != nullptr &&
                previous->egressNetworkId != capability.ingressNetworkId)
            {
               AddIssue(result, "GATEWAY_ROUTE_NETWORK_DISCONTINUITY", route.routeId,
                        "adjacent gateway capabilities must share the transit network");
            }
            previous = &capability;
         }
      }
      return result;
   }

   GatewayPolicyDecision Evaluate(
      const std::vector<GatewayResourceState>& aCapabilities,
      const std::vector<GatewayRouteTemplate>& aRoutes,
      const GatewayMessageContext& aContext,
      const std::string& aCurrentPlatformId) const
   {
      const GatewayRouteTemplate* route = FindRoute(aRoutes, aContext.routeId);
      if (route == nullptr)
      {
         return Reject("GATEWAY_ROUTE_NOT_FOUND");
      }
      if (!route->enabled || !route->valid)
      {
         return Reject("GATEWAY_ROUTE_DISABLED");
      }
      if (aContext.transferId.empty())
      {
         return Reject("GATEWAY_TRANSFER_ID_MISSING");
      }
      if (aContext.sourcePlatformId != route->sourcePlatformId)
      {
         return Reject("ROUTE_SOURCE_NOT_ALLOWED");
      }
      if (aContext.destinationPlatformId != route->destinationPlatformId)
      {
         return Reject("ROUTE_DESTINATION_NOT_ALLOWED");
      }
      if (aContext.destinationCommName != route->destinationCommName)
      {
         return Reject("ROUTE_DESTINATION_COMM_MISMATCH");
      }
      if (!Contains(route->allowedMessageTypes, aContext.messageType))
      {
         return Reject("ROUTE_MESSAGE_TYPE_NOT_ALLOWED");
      }
      if (aContext.routeIndex >= route->gatewayCapabilityIds.size())
      {
         return Reject("GATEWAY_ROUTE_INDEX_INVALID");
      }
      std::string expectedOriginator = route->sourcePlatformId;
      if (aContext.routeIndex > 0)
      {
         const GatewayResourceState* previousCapability = FindCapability(
            aCapabilities, route->gatewayCapabilityIds[aContext.routeIndex - 1]);
         if (previousCapability == nullptr)
         {
            return Reject("GATEWAY_PREVIOUS_CAPABILITY_NOT_FOUND");
         }
         expectedOriginator = previousCapability->platformId;
      }
      if (aContext.actualOriginatorPlatformId != expectedOriginator)
      {
         return Reject("GATEWAY_ORIGINATOR_MISMATCH");
      }
      if (aContext.hopCount != aContext.routeIndex)
      {
         return Reject("GATEWAY_HOP_COUNT_MISMATCH");
      }
      if (aContext.ttl <= 0)
      {
         return Reject("GATEWAY_TTL_EXCEEDED");
      }

      const std::string& capabilityId =
         route->gatewayCapabilityIds[aContext.routeIndex];
      const GatewayResourceState* capability = FindCapability(aCapabilities, capabilityId);
      if (capability == nullptr)
      {
         return Reject("GATEWAY_CAPABILITY_NOT_FOUND");
      }
      if (capability->platformId != aCurrentPlatformId)
      {
         return Reject("GATEWAY_ROUTE_PLATFORM_MISMATCH");
      }
      if (capability->ingressNetworkId != aContext.actualIngressNetworkId)
      {
         return Reject("INGRESS_NETWORK_MISMATCH");
      }
      if (capability->ingressCommName != aContext.actualIngressCommName)
      {
         return Reject("INGRESS_COMM_MISMATCH");
      }
      if (!capability->enabled || !capability->valid)
      {
         return Reject("GATEWAY_CAPABILITY_DISABLED");
      }
      if (!Contains(capability->allowedSourcePlatformIds,
                    aContext.sourcePlatformId))
      {
         return Reject("SOURCE_NOT_ALLOWED");
      }
      if (!Contains(capability->allowedDestinationPlatformIds,
                    aContext.destinationPlatformId))
      {
         return Reject("DESTINATION_NOT_ALLOWED");
      }
      if (!Contains(capability->allowedMessageTypes, aContext.messageType))
      {
         return Reject("MESSAGE_TYPE_NOT_ALLOWED");
      }
      if (Contains(aContext.trace, capabilityId))
      {
         return Reject("GATEWAY_LOOP_DETECTED");
      }

      GatewayPolicyDecision decision;
      decision.action = GatewayPolicyAction::cFORWARD;
      decision.reasonCode = "FORWARD_ALLOWED";
      decision.capabilityId = capabilityId;
      decision.nextRouteIndex = aContext.routeIndex + 1;
      decision.finalHop = decision.nextRouteIndex >= route->gatewayCapabilityIds.size();
      if (!decision.finalHop)
      {
         decision.nextCapabilityId =
            route->gatewayCapabilityIds[decision.nextRouteIndex];
      }
      return decision;
   }

private:
   static void AddIssue(GatewayCatalogValidation& aResult,
                        const std::string& aReason,
                        const std::string& aObjectId,
                        const std::string& aMessage)
   {
      aResult.valid = false;
      GatewayValidationIssue issue;
      issue.reasonCode = aReason;
      issue.objectId = aObjectId;
      issue.message = aMessage;
      aResult.issues.push_back(issue);
   }

   static void ValidateAllowList(GatewayCatalogValidation& aResult,
                                 const std::string& aObjectId,
                                 const std::string& aKind,
                                 const std::vector<std::string>& aValues)
   {
      if (aValues.empty())
      {
         AddIssue(aResult, "GATEWAY_" + aKind + "_ALLOW_LIST_EMPTY", aObjectId,
                  "explicit allow list must not be empty");
      }
      if (std::find(aValues.begin(), aValues.end(), "*") != aValues.end())
      {
         AddIssue(aResult, "GATEWAY_WILDCARD_NOT_ALLOWED", aObjectId,
                  "wildcard gateway authorization is not allowed");
      }
   }

   static bool Contains(const std::vector<std::string>& aValues,
                        const std::string& aValue)
   {
      return std::find(aValues.begin(), aValues.end(), aValue) != aValues.end();
   }

   static const GatewayResourceState* FindCapability(
      const std::vector<GatewayResourceState>& aCapabilities,
      const std::string& aCapabilityId)
   {
      const auto found = std::find_if(
         aCapabilities.begin(), aCapabilities.end(),
         [&aCapabilityId](const GatewayResourceState& aCapability)
         { return aCapability.gatewayId == aCapabilityId; });
      return found == aCapabilities.end() ? nullptr : &*found;
   }

   static const GatewayRouteTemplate* FindRoute(
      const std::vector<GatewayRouteTemplate>& aRoutes,
      const std::string& aRouteId)
   {
      const auto found = std::find_if(
         aRoutes.begin(), aRoutes.end(),
         [&aRouteId](const GatewayRouteTemplate& aRoute)
         { return aRoute.routeId == aRouteId; });
      return found == aRoutes.end() ? nullptr : &*found;
   }

   static GatewayPolicyDecision Reject(const char* aReason)
   {
      GatewayPolicyDecision decision;
      decision.reasonCode = aReason;
      return decision;
   }
};
} // namespace nrm

#endif
