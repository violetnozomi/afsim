/**
 * @file GatewayTypes.hpp
 * @brief Framework-neutral cross-domain gateway route and policy contracts.
 */

#ifndef NRM_GATEWAY_TYPES_HPP
#define NRM_GATEWAY_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nrm
{
enum class GatewayPolicyAction
{
   cFORWARD,
   cREJECT,
   cDROP
};

inline const char* ToString(GatewayPolicyAction aAction)
{
   switch (aAction)
   {
   case GatewayPolicyAction::cFORWARD: return "FORWARD";
   case GatewayPolicyAction::cREJECT: return "REJECTED";
   case GatewayPolicyAction::cDROP: return "DROPPED";
   }
   return "REJECTED";
}

struct GatewayForwardingEvent
{
   std::string transferId;
   std::string routeId;
   std::string gatewayCapabilityId;
   std::string platformId;
   std::string sourcePlatformId;
   std::string destinationPlatformId;
   std::string destinationCommName;
   std::string ingressNetworkId;
   std::string egressNetworkId;
   std::string messageType;
   std::string result = "REJECTED";
   std::string reasonCode;
   std::uint64_t inputSerialNumber = 0;
   std::uint64_t outputSerialNumber = 0;
   std::uint64_t messageBits = 0;
   std::size_t routeIndex = 0;
   std::size_t hopCount = 0;
   double receiveTime = 0.0;
   double completionTime = 0.0;
};

struct GatewayRouteTemplate
{
   std::string routeId;
   std::string sourcePlatformId;
   std::string destinationPlatformId;
   std::string destinationCommName;
   std::vector<std::string> allowedMessageTypes;
   std::vector<std::string> gatewayCapabilityIds;
   int priority = 0;
   bool enabled = false;
   bool valid = false;
   std::vector<std::string> reasonCodes;
};

struct GatewayMessageContext
{
   std::string transferId;
   std::string routeId;
   std::string sourcePlatformId;
   std::string destinationPlatformId;
   std::string destinationCommName;
   std::string messageType;
   std::string actualOriginatorPlatformId;
   std::string actualIngressNetworkId;
   std::string actualIngressCommName;
   std::size_t routeIndex = 0;
   std::size_t hopCount = 0;
   int ttl = 0;
   std::vector<std::string> trace;
};

struct GatewayPolicyDecision
{
   GatewayPolicyAction action = GatewayPolicyAction::cREJECT;
   std::string reasonCode = "GATEWAY_POLICY_REJECTED";
   std::string capabilityId;
   std::string nextCapabilityId;
   std::size_t nextRouteIndex = 0;
   bool finalHop = false;
};

struct GatewayValidationIssue
{
   std::string reasonCode;
   std::string objectId;
   std::string message;
};

struct GatewayCatalogValidation
{
   bool valid = true;
   std::vector<GatewayValidationIssue> issues;
};
} // namespace nrm

#endif
