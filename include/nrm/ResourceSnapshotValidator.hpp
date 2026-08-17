/**
 * @file ResourceSnapshotValidator.hpp
 * @brief JSON/Qt-independent semantic validation for resource snapshots.
 */

#ifndef NRM_RESOURCE_SNAPSHOT_VALIDATOR_HPP
#define NRM_RESOURCE_SNAPSHOT_VALIDATOR_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class ResourceSnapshotValidationReason
{
   cNETWORK_ID_EMPTY,
   cNETWORK_ID_DUPLICATE,
   cNETWORK_TYPE_INVALID,
   cENDPOINT_ID_EMPTY,
   cENDPOINT_ID_DUPLICATE,
   cPLATFORM_ID_EMPTY,
   cNETWORK_REFERENCE_MISSING,
   cNETWORK_TYPE_MISMATCH,
   cLINK_ID_EMPTY,
   cLINK_ID_DUPLICATE,
   cENDPOINT_REFERENCE_MISSING,
   cLINK_NETWORK_MISMATCH,
   cMETRIC_OUT_OF_RANGE,
   cPROTOCOL_USAGE_INVALID,
   cROUTE_ID_EMPTY,
   cROUTE_ID_DUPLICATE,
   cROUTE_ENDPOINT_INVALID,
   cROUTE_LINK_MISSING,
   cGATEWAY_ID_EMPTY,
   cGATEWAY_ID_DUPLICATE,
   cGATEWAY_REFERENCE_MISSING,
   cGATEWAY_SELF_REFERENCE
};

inline const char* ToString(ResourceSnapshotValidationReason aReason)
{
   switch (aReason)
   {
   case ResourceSnapshotValidationReason::cNETWORK_ID_EMPTY:
      return "NETWORK_ID_EMPTY";
   case ResourceSnapshotValidationReason::cNETWORK_ID_DUPLICATE:
      return "NETWORK_ID_DUPLICATE";
   case ResourceSnapshotValidationReason::cNETWORK_TYPE_INVALID:
      return "NETWORK_TYPE_INVALID";
   case ResourceSnapshotValidationReason::cENDPOINT_ID_EMPTY:
      return "ENDPOINT_ID_EMPTY";
   case ResourceSnapshotValidationReason::cENDPOINT_ID_DUPLICATE:
      return "ENDPOINT_ID_DUPLICATE";
   case ResourceSnapshotValidationReason::cPLATFORM_ID_EMPTY:
      return "PLATFORM_ID_EMPTY";
   case ResourceSnapshotValidationReason::cNETWORK_REFERENCE_MISSING:
      return "NETWORK_REFERENCE_MISSING";
   case ResourceSnapshotValidationReason::cNETWORK_TYPE_MISMATCH:
      return "NETWORK_TYPE_MISMATCH";
   case ResourceSnapshotValidationReason::cLINK_ID_EMPTY:
      return "LINK_ID_EMPTY";
   case ResourceSnapshotValidationReason::cLINK_ID_DUPLICATE:
      return "LINK_ID_DUPLICATE";
   case ResourceSnapshotValidationReason::cENDPOINT_REFERENCE_MISSING:
      return "ENDPOINT_REFERENCE_MISSING";
   case ResourceSnapshotValidationReason::cLINK_NETWORK_MISMATCH:
      return "LINK_NETWORK_MISMATCH";
   case ResourceSnapshotValidationReason::cMETRIC_OUT_OF_RANGE:
      return "METRIC_OUT_OF_RANGE";
   case ResourceSnapshotValidationReason::cPROTOCOL_USAGE_INVALID:
      return "PROTOCOL_USAGE_INVALID";
   case ResourceSnapshotValidationReason::cROUTE_ID_EMPTY:
      return "ROUTE_ID_EMPTY";
   case ResourceSnapshotValidationReason::cROUTE_ID_DUPLICATE:
      return "ROUTE_ID_DUPLICATE";
   case ResourceSnapshotValidationReason::cROUTE_ENDPOINT_INVALID:
      return "ROUTE_ENDPOINT_INVALID";
   case ResourceSnapshotValidationReason::cROUTE_LINK_MISSING:
      return "ROUTE_LINK_MISSING";
   case ResourceSnapshotValidationReason::cGATEWAY_ID_EMPTY:
      return "GATEWAY_ID_EMPTY";
   case ResourceSnapshotValidationReason::cGATEWAY_ID_DUPLICATE:
      return "GATEWAY_ID_DUPLICATE";
   case ResourceSnapshotValidationReason::cGATEWAY_REFERENCE_MISSING:
      return "GATEWAY_REFERENCE_MISSING";
   case ResourceSnapshotValidationReason::cGATEWAY_SELF_REFERENCE:
      return "GATEWAY_SELF_REFERENCE";
   }
   return "METRIC_OUT_OF_RANGE";
}

struct ResourceSnapshotValidationIssue
{
   ResourceSnapshotValidationReason reason =
      ResourceSnapshotValidationReason::cNETWORK_ID_EMPTY;
   std::string path;
   std::string message;
};

struct ResourceSnapshotValidationResult
{
   bool valid = true;
   std::vector<ResourceSnapshotValidationIssue> issues;
};

class ResourceSnapshotValidator
{
public:
   static const std::string& PlatformId(const EndpointSnapshot& aEndpoint)
   {
      return aEndpoint.platformId.empty() ? aEndpoint.platformName
                                          : aEndpoint.platformId;
   }

   ResourceSnapshotValidationResult Validate(
      const ResourceSnapshot& aSnapshot) const
   {
      ResourceSnapshotValidationResult result;
      NetworkMap networks;
      EndpointMap endpoints;
      LinkSet links;
      ValidateNetworks(aSnapshot, networks, result);
      ValidateEndpoints(aSnapshot, networks, endpoints, result);
      ValidateLinks(aSnapshot, networks, endpoints, links, result);
      ValidateRoutes(aSnapshot, endpoints, links, result);
      ValidateGateways(aSnapshot, networks, endpoints, result);
      result.valid = result.issues.empty();
      return result;
   }

private:
   using NetworkMap = std::map<std::string, const NetworkSnapshot*>;
   using EndpointMap = std::map<std::string, const EndpointSnapshot*>;
   using LinkSet = std::set<std::pair<std::string, std::string>>;

   static bool ValidNetworkType(NetworkType aType)
   {
      return aType == NetworkType::cLINK11 || aType == NetworkType::cLINK16 ||
             aType == NetworkType::cSATCOM || aType == NetworkType::cCDL;
   }

   static void Add(ResourceSnapshotValidationResult& aResult,
                   ResourceSnapshotValidationReason aReason,
                   const std::string& aPath,
                   const std::string& aMessage)
   {
      aResult.issues.push_back({aReason, aPath, aMessage});
   }

   static bool InRange(const MetricValue<double>& aMetric,
                       double aMinimum,
                       double aMaximum)
   {
      return !aMetric.valid ||
             (std::isfinite(aMetric.value) && aMetric.value >= aMinimum &&
              aMetric.value <= aMaximum);
   }

   static void ValidateNetworks(const ResourceSnapshot& aSnapshot,
                                NetworkMap& aNetworks,
                                ResourceSnapshotValidationResult& aResult)
   {
      for (std::size_t index = 0; index < aSnapshot.networks.size(); ++index)
      {
         const NetworkSnapshot& network = aSnapshot.networks[index];
         const std::string path = "/networks/" + std::to_string(index);
         if (network.networkId.empty())
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_ID_EMPTY,
                path + "/networkId", "networkId must not be empty");
         else if (!aNetworks.emplace(network.networkId, &network).second)
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_ID_DUPLICATE,
                path + "/networkId", "networkId must be unique");
         if (!ValidNetworkType(network.networkType))
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_TYPE_INVALID,
                path + "/networkType", "networkType must be one of the four supported networks");
      }
   }

   static void ValidateEndpoints(const ResourceSnapshot& aSnapshot,
                                 const NetworkMap& aNetworks,
                                 EndpointMap& aEndpoints,
                                 ResourceSnapshotValidationResult& aResult)
   {
      for (std::size_t index = 0; index < aSnapshot.endpoints.size(); ++index)
      {
         const EndpointSnapshot& endpoint = aSnapshot.endpoints[index];
         const std::string path = "/endpoints/" + std::to_string(index);
         if (endpoint.endpointId.empty())
            Add(aResult, ResourceSnapshotValidationReason::cENDPOINT_ID_EMPTY,
                path + "/endpointId", "endpointId must not be empty");
         else if (!aEndpoints.emplace(endpoint.endpointId, &endpoint).second)
            Add(aResult, ResourceSnapshotValidationReason::cENDPOINT_ID_DUPLICATE,
                path + "/endpointId", "endpointId must be unique");
         if (PlatformId(endpoint).empty())
            Add(aResult, ResourceSnapshotValidationReason::cPLATFORM_ID_EMPTY,
                path + "/platformId", "platformId must not be empty");
         const auto network = aNetworks.find(endpoint.networkId);
         if (network == aNetworks.end())
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_REFERENCE_MISSING,
                path + "/networkId", "endpoint networkId does not exist");
         else if (endpoint.networkType != network->second->networkType)
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_TYPE_MISMATCH,
                path + "/networkType", "endpoint networkType differs from its networkId");
      }
   }

   static bool ValidWindow(const WindowMetrics& aWindow)
   {
      const double maximum = std::numeric_limits<double>::max();
      return std::isfinite(aWindow.windowS) && aWindow.windowS >= 0.0 &&
             InRange(aWindow.offeredLoadBps, 0.0, maximum) &&
             InRange(aWindow.deliveredThroughputBps, 0.0, maximum) &&
             InRange(aWindow.throughputBps, 0.0, maximum) &&
             InRange(aWindow.deliveryRatioPercent, 0.0, 100.0) &&
             InRange(aWindow.pdrPercent, 0.0, 100.0) &&
             InRange(aWindow.averageQueueDelayMs, 0.0, maximum) &&
             InRange(aWindow.p50QueueDelayMs, 0.0, maximum) &&
             InRange(aWindow.p95QueueDelayMs, 0.0, maximum) &&
             InRange(aWindow.averageTransportDelayMs, 0.0, maximum) &&
             InRange(aWindow.p50TransportDelayMs, 0.0, maximum) &&
             InRange(aWindow.p95TransportDelayMs, 0.0, maximum) &&
             InRange(aWindow.onlineRatioPercent, 0.0, 100.0) &&
             InRange(aWindow.utilizationPercent, 0.0, 100.0) &&
             InRange(aWindow.queueUtilizationPercent, 0.0, 100.0) &&
             InRange(aWindow.ackDelayMs, 0.0, maximum) &&
             InRange(aWindow.responseDelayMs, 0.0, maximum) &&
             InRange(aWindow.rttMs, 0.0, maximum);
   }

   static void ValidateLinks(const ResourceSnapshot& aSnapshot,
                             const NetworkMap& aNetworks,
                             const EndpointMap& aEndpoints,
                             LinkSet& aLinks,
                             ResourceSnapshotValidationResult& aResult)
   {
      const double maximum = std::numeric_limits<double>::max();
      std::set<std::string> linkIds;
      for (std::size_t index = 0; index < aSnapshot.links.size(); ++index)
      {
         const LinkSnapshot& link = aSnapshot.links[index];
         const std::string path = "/links/" + std::to_string(index);
         if (link.linkId.empty())
            Add(aResult, ResourceSnapshotValidationReason::cLINK_ID_EMPTY,
                path + "/linkId", "linkId must not be empty");
         else if (!linkIds.insert(link.linkId).second)
            Add(aResult, ResourceSnapshotValidationReason::cLINK_ID_DUPLICATE,
                path + "/linkId", "linkId must be unique");

         const auto network = aNetworks.find(link.networkId);
         if (network == aNetworks.end())
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_REFERENCE_MISSING,
                path + "/networkId", "link networkId does not exist");
         else if (link.networkType != network->second->networkType)
            Add(aResult, ResourceSnapshotValidationReason::cNETWORK_TYPE_MISMATCH,
                path + "/networkType", "link networkType differs from its networkId");

         const auto source = aEndpoints.find(link.sourceEndpointId);
         const auto destination = aEndpoints.find(link.destinationEndpointId);
         if (source == aEndpoints.end() || destination == aEndpoints.end())
            Add(aResult, ResourceSnapshotValidationReason::cENDPOINT_REFERENCE_MISSING,
                path, "link endpoint reference does not exist");
         else
         {
            if (source->second->networkId != link.networkId ||
                destination->second->networkId != link.networkId)
               Add(aResult, ResourceSnapshotValidationReason::cLINK_NETWORK_MISMATCH,
                   path + "/networkId", "both link endpoints must belong to the exact networkId");
            aLinks.insert({link.sourceEndpointId, link.destinationEndpointId});
         }

         bool metricsValid = InRange(link.distanceM, 0.0, maximum) &&
                             InRange(link.bandwidthBps, 0.0, maximum) &&
                             InRange(link.ber, 0.0, 1.0) &&
                             InRange(link.serviceAvailabilityPercent, 0.0, 100.0) &&
                             InRange(link.establishmentSuccessRatioPercent, 0.0, 100.0) &&
                             InRange(link.communicationQualityPercent, 0.0, 100.0);
         for (const WindowMetrics& window : link.windows)
            metricsValid = metricsValid && ValidWindow(window);
         if (!metricsValid)
            Add(aResult, ResourceSnapshotValidationReason::cMETRIC_OUT_OF_RANGE,
                path, "link metrics must be finite and within their business ranges");
         if (link.protocolResource.valid &&
             (link.protocolResource.used > link.protocolResource.capacity ||
              link.protocolResource.remaining !=
                 link.protocolResource.capacity - link.protocolResource.used ||
              !InRange(link.protocolResource.utilizationPercent, 0.0, 100.0)))
            Add(aResult, ResourceSnapshotValidationReason::cPROTOCOL_USAGE_INVALID,
                path + "/protocolResource", "protocol used/remaining/capacity are inconsistent");
      }
   }

   static void ValidateRoutes(const ResourceSnapshot& aSnapshot,
                              const EndpointMap& aEndpoints,
                              const LinkSet& aLinks,
                              ResourceSnapshotValidationResult& aResult)
   {
      std::set<std::string> routeIds;
      for (std::size_t index = 0; index < aSnapshot.routes.size(); ++index)
      {
         const RouteResourceState& route = aSnapshot.routes[index];
         const std::string path = "/routes/" + std::to_string(index);
         if (route.routeId.empty())
            Add(aResult, ResourceSnapshotValidationReason::cROUTE_ID_EMPTY,
                path + "/routeId", "routeId must not be empty");
         else if (!routeIds.insert(route.routeId).second)
            Add(aResult, ResourceSnapshotValidationReason::cROUTE_ID_DUPLICATE,
                path + "/routeId", "routeId must be unique");

         bool endpointsValid = route.hops.size() >= 2 &&
                               route.hops.front() == route.sourceMemberId &&
                               route.hops.back() == route.destinationMemberId;
         for (const std::string& hop : route.hops)
            endpointsValid = endpointsValid && aEndpoints.count(hop) != 0;
         if (!endpointsValid)
         {
            Add(aResult, ResourceSnapshotValidationReason::cROUTE_ENDPOINT_INVALID,
                path + "/hops", "route endpoints and hops must reference existing members");
            continue;
         }
         for (std::size_t hop = 1; hop < route.hops.size(); ++hop)
         {
            if (aLinks.count({route.hops[hop - 1], route.hops[hop]}) == 0)
            {
               Add(aResult, ResourceSnapshotValidationReason::cROUTE_LINK_MISSING,
                   path + "/hops/" + std::to_string(hop),
                   "consecutive route hops require a directed link");
               break;
            }
         }
      }
   }

   static void ValidateGateways(const ResourceSnapshot& aSnapshot,
                                const NetworkMap& aNetworks,
                                const EndpointMap& aEndpoints,
                                ResourceSnapshotValidationResult& aResult)
   {
      std::set<std::string> gatewayIds;
      for (std::size_t index = 0; index < aSnapshot.gateways.size(); ++index)
      {
         const GatewayResourceState& gateway = aSnapshot.gateways[index];
         const std::string path = "/gateways/" + std::to_string(index);
         if (gateway.gatewayId.empty())
            Add(aResult, ResourceSnapshotValidationReason::cGATEWAY_ID_EMPTY,
                path + "/gatewayId", "gatewayId must not be empty");
         else if (!gatewayIds.insert(gateway.gatewayId).second)
            Add(aResult, ResourceSnapshotValidationReason::cGATEWAY_ID_DUPLICATE,
                path + "/gatewayId", "gatewayId must be unique");
         if (gateway.ingressNetworkId == gateway.egressNetworkId)
            Add(aResult, ResourceSnapshotValidationReason::cGATEWAY_SELF_REFERENCE,
                path, "gateway ingress and egress networks must differ");

         bool ingressMember = false;
         bool egressMember = false;
         for (const auto& entry : aEndpoints)
         {
            const EndpointSnapshot& endpoint = *entry.second;
            if (PlatformId(endpoint) != gateway.platformId) continue;
            ingressMember = ingressMember ||
                            endpoint.networkId == gateway.ingressNetworkId;
            egressMember = egressMember ||
                           endpoint.networkId == gateway.egressNetworkId;
         }
         if (gateway.platformId.empty() ||
             aNetworks.count(gateway.ingressNetworkId) == 0 ||
             aNetworks.count(gateway.egressNetworkId) == 0 ||
             !ingressMember || !egressMember)
            Add(aResult, ResourceSnapshotValidationReason::cGATEWAY_REFERENCE_MISSING,
                path, "gateway platform must be a member of both referenced networks");
      }
   }
};
} // namespace nrm

#endif
