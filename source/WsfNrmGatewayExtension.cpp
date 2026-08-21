/**
 * @file WsfNrmGatewayExtension.cpp
 * @brief Parses, validates, and exposes explicit cross-domain gateway routes.
 */

#include "WsfNrmGatewayExtension.hpp"

#include <algorithm>
#include <utility>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfNrmCrossDomainGatewayProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "nrm/GatewayPolicyEngine.hpp"

namespace
{
const char* cEXTENSION_NAME = "wsf_network_resource_manager_gateway";

void AddReason(std::vector<std::string>& aReasons, const std::string& aReason)
{
   if (std::find(aReasons.begin(), aReasons.end(), aReason) == aReasons.end())
   {
      aReasons.push_back(aReason);
   }
}
} // namespace

WsfNrmGatewaySimulationExtension::WsfNrmGatewaySimulationExtension(
   std::vector<nrm::GatewayRouteTemplate> aRoutes)
   : mRoutes(std::move(aRoutes))
{
}

WsfNrmGatewaySimulationExtension* WsfNrmGatewaySimulationExtension::Find(
   const WsfSimulation& aSimulation)
{
   return static_cast<WsfNrmGatewaySimulationExtension*>(
      aSimulation.FindExtension(cEXTENSION_NAME));
}

void WsfNrmGatewaySimulationExtension::UpdateCapability(
   const nrm::GatewayResourceState& aCapability)
{
   const auto found = std::find_if(
      mCapabilities.begin(), mCapabilities.end(),
      [&aCapability](const nrm::GatewayResourceState& aExisting)
      { return aExisting.gatewayId == aCapability.gatewayId; });
   if (found == mCapabilities.end())
   {
      mCapabilities.push_back(aCapability);
   }
   else
   {
      *found = aCapability;
   }
}

const std::vector<nrm::GatewayResourceState>&
WsfNrmGatewaySimulationExtension::GetCapabilities() const
{
   return mCapabilities;
}

const std::vector<nrm::GatewayRouteTemplate>&
WsfNrmGatewaySimulationExtension::GetRoutes() const
{
   return mRoutes;
}

const nrm::GatewayResourceState* WsfNrmGatewaySimulationExtension::FindCapability(
   const std::string& aCapabilityId) const
{
   const auto found = std::find_if(
      mCapabilities.begin(), mCapabilities.end(),
      [&aCapabilityId](const nrm::GatewayResourceState& aCapability)
      { return aCapability.gatewayId == aCapabilityId; });
   return found == mCapabilities.end() ? nullptr : &*found;
}

bool WsfNrmGatewaySimulationExtension::PlatformsInitialized()
{
   bool topologyValid = true;
   for (nrm::GatewayResourceState& capability : mCapabilities)
   {
      capability.valid = true;
      capability.reasonCodes.clear();
      WsfPlatform* platform =
         GetSimulation().GetPlatformByName(WsfStringId(capability.platformId));
      if (platform == nullptr)
      {
         capability.valid = false;
         topologyValid = false;
         AddReason(capability.reasonCodes, "GATEWAY_PLATFORM_NOT_FOUND");
         continue;
      }

      wsf::comm::Comm* ingress =
         platform->GetComponent<wsf::comm::Comm>(capability.ingressCommName);
      wsf::comm::Comm* egress =
         platform->GetComponent<wsf::comm::Comm>(capability.egressCommName);
      if (ingress == nullptr || ingress->GetNetwork() != capability.ingressNetworkId)
      {
         capability.valid = false;
         topologyValid = false;
         AddReason(capability.reasonCodes, "GATEWAY_INGRESS_REFERENCE_INVALID");
      }
      if (egress == nullptr || egress->GetNetwork() != capability.egressNetworkId)
      {
         capability.valid = false;
         topologyValid = false;
         AddReason(capability.reasonCodes, "GATEWAY_EGRESS_REFERENCE_INVALID");
      }
   }

   for (nrm::GatewayRouteTemplate& route : mRoutes)
   {
      route.valid = true;
      route.reasonCodes.clear();
      WsfPlatform* source =
         GetSimulation().GetPlatformByName(WsfStringId(route.sourcePlatformId));
      WsfPlatform* destination =
         GetSimulation().GetPlatformByName(WsfStringId(route.destinationPlatformId));
      if (source == nullptr || destination == nullptr)
      {
         route.valid = false;
         topologyValid = false;
         AddReason(route.reasonCodes, "GATEWAY_ROUTE_ENDPOINT_NOT_FOUND");
      }
      else if (!route.destinationCommName.empty())
      {
         wsf::comm::Comm* destinationComm =
            destination->GetComponent<wsf::comm::Comm>(route.destinationCommName);
         if (destinationComm == nullptr)
         {
            route.valid = false;
            topologyValid = false;
            AddReason(route.reasonCodes, "GATEWAY_ROUTE_DESTINATION_COMM_NOT_FOUND");
         }
         else if (!route.gatewayCapabilityIds.empty())
         {
            const nrm::GatewayResourceState* finalCapability =
               FindCapability(route.gatewayCapabilityIds.back());
            if (finalCapability != nullptr &&
                destinationComm->GetNetwork() != finalCapability->egressNetworkId)
            {
               route.valid = false;
               topologyValid = false;
               AddReason(route.reasonCodes,
                         "GATEWAY_ROUTE_DESTINATION_NETWORK_MISMATCH");
            }
         }
      }
   }

   nrm::GatewayPolicyEngine engine;
   const nrm::GatewayCatalogValidation validation = engine.Validate(mCapabilities, mRoutes);
   for (const nrm::GatewayValidationIssue& issue : validation.issues)
   {
      topologyValid = false;
      bool assigned = false;
      for (nrm::GatewayResourceState& capability : mCapabilities)
      {
         if (capability.gatewayId == issue.objectId ||
             capability.platformId == issue.objectId)
         {
            capability.valid = false;
            AddReason(capability.reasonCodes, issue.reasonCode);
            assigned = true;
         }
      }
      for (nrm::GatewayRouteTemplate& route : mRoutes)
      {
         if (route.routeId == issue.objectId)
         {
            route.valid = false;
            AddReason(route.reasonCodes, issue.reasonCode);
            assigned = true;
         }
      }
      auto message = ut::log::error()
                     << "NRM_GATEWAY CONFIG_INVALID reason=" << issue.reasonCode
                     << " object=" << issue.objectId;
      message.AddNote() << issue.message;
      if (!assigned)
      {
         message.AddNote() << "Validation issue could not be assigned to a configured object.";
      }
   }

   if (!topologyValid)
   {
      for (const nrm::GatewayResourceState& capability : mCapabilities)
      {
         if (!capability.valid)
         {
            ut::log::error() << "NRM_GATEWAY CAPABILITY_DISABLED id="
                             << capability.gatewayId;
         }
      }
      for (const nrm::GatewayRouteTemplate& route : mRoutes)
      {
         if (!route.valid)
         {
            ut::log::error() << "NRM_GATEWAY ROUTE_DISABLED id=" << route.routeId;
         }
      }
   }
   return topologyValid;
}

void WsfNrmGatewayScenarioExtension::AddedToScenario()
{
   WsfProcessorTypes::Get(GetScenario()).Add(
      "WSF_NRM_CROSS_DOMAIN_GATEWAY_PROCESSOR",
      ut::make_unique<WsfNrmCrossDomainGatewayProcessor>(GetScenario()));
}

bool WsfNrmGatewayScenarioExtension::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() != "nrm_gateway_route")
   {
      return false;
   }

   nrm::GatewayRouteTemplate route;
   UtInputBlock block(aInput, "end_nrm_gateway_route");
   std::string command;
   while (block.ReadCommand(command))
   {
      if (command == "route_id")
      {
         aInput.ReadValue(route.routeId);
      }
      else if (command == "source_platform")
      {
         aInput.ReadValue(route.sourcePlatformId);
      }
      else if (command == "destination_platform")
      {
         aInput.ReadValue(route.destinationPlatformId);
      }
      else if (command == "destination_comm")
      {
         aInput.ReadValue(route.destinationCommName);
      }
      else if (command == "allowed_message_type")
      {
         std::string value;
         aInput.ReadValue(value);
         route.allowedMessageTypes.push_back(value);
      }
      else if (command == "gateway_capability")
      {
         std::string value;
         aInput.ReadValue(value);
         route.gatewayCapabilityIds.push_back(value);
      }
      else if (command == "priority")
      {
         aInput.ReadValue(route.priority);
      }
      else if (command == "enabled")
      {
         aInput.ReadValue(route.enabled);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   mRoutes.push_back(route);
   return true;
}

void WsfNrmGatewayScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(
      cEXTENSION_NAME,
      ut::make_unique<WsfNrmGatewaySimulationExtension>(mRoutes));
}
