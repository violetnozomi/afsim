/**
 * @file WsfNrmGatewayExtension.hpp
 * @brief AFSIM scenario/simulation extensions for cross-domain gateway routes.
 */

#ifndef WSF_NRM_GATEWAY_EXTENSION_HPP
#define WSF_NRM_GATEWAY_EXTENSION_HPP

#include <string>
#include <vector>

#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
#include "nrm/GatewayTypes.hpp"
#include "nrm/NetworkResourceTypes.hpp"
#include "wsf_network_resource_manager_export.h"

class UtInput;
class WsfSimulation;

class WSF_NETWORK_RESOURCE_MANAGER_EXPORT WsfNrmGatewaySimulationExtension final
   : public WsfSimulationExtension
{
public:
   explicit WsfNrmGatewaySimulationExtension(
      std::vector<nrm::GatewayRouteTemplate> aRoutes);

   static WsfNrmGatewaySimulationExtension* Find(const WsfSimulation& aSimulation);

   bool PlatformsInitialized() override;

   void UpdateCapability(const nrm::GatewayResourceState& aCapability);
   const std::vector<nrm::GatewayResourceState>& GetCapabilities() const;
   const std::vector<nrm::GatewayRouteTemplate>& GetRoutes() const;
   const nrm::GatewayResourceState* FindCapability(
      const std::string& aCapabilityId) const;

private:
   std::vector<nrm::GatewayRouteTemplate> mRoutes;
   std::vector<nrm::GatewayResourceState> mCapabilities;
};

class WSF_NETWORK_RESOURCE_MANAGER_EXPORT WsfNrmGatewayScenarioExtension final
   : public WsfScenarioExtension
{
public:
   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

private:
   std::vector<nrm::GatewayRouteTemplate> mRoutes;
};

#endif
