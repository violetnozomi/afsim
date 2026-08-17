#include "nrm/ResourceSnapshotValidator.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace
{
template<typename T>
void ValidMetric(nrm::MetricValue<T>& aMetric, T aValue)
{
   aMetric.value = aValue;
   aMetric.valid = true;
}

nrm::EndpointSnapshot Endpoint(const char* aEndpointId,
                               const char* aPlatformId,
                               const char* aNetworkId,
                               nrm::NetworkType aType)
{
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = aEndpointId;
   endpoint.platformId = aPlatformId;
   endpoint.platformName = aPlatformId;
   endpoint.networkId = aNetworkId;
   endpoint.networkName = aNetworkId;
   endpoint.networkType = aType;
   endpoint.state = nrm::ResourceState::cONLINE;
   return endpoint;
}

nrm::ResourceSnapshot ValidSnapshot()
{
   nrm::ResourceSnapshot snapshot;
   snapshot.simTime = 10.0;

   nrm::NetworkSnapshot primary;
   primary.networkId = "net-primary";
   primary.networkName = "Primary";
   primary.networkType = nrm::NetworkType::cLINK16;
   snapshot.networks.push_back(primary);

   nrm::NetworkSnapshot backup;
   backup.networkId = "net-backup";
   backup.networkName = "Backup";
   backup.networkType = nrm::NetworkType::cSATCOM;
   snapshot.networks.push_back(backup);

   snapshot.endpoints.push_back(Endpoint(
      "source-member", "source-platform", "net-primary",
      nrm::NetworkType::cLINK16));
   snapshot.endpoints.push_back(Endpoint(
      "gateway-primary", "gateway-platform", "net-primary",
      nrm::NetworkType::cLINK16));
   snapshot.endpoints.push_back(Endpoint(
      "gateway-backup", "gateway-platform", "net-backup",
      nrm::NetworkType::cSATCOM));

   nrm::LinkSnapshot link;
   link.linkId = "primary-link";
   link.networkId = "net-primary";
   link.networkName = "net-primary";
   link.networkType = nrm::NetworkType::cLINK16;
   link.sourceEndpointId = "source-member";
   link.destinationEndpointId = "gateway-primary";
   link.state = nrm::ResourceState::cONLINE;
   ValidMetric(link.bandwidthBps, 1000000.0);
   ValidMetric(link.ber, 0.001);
   link.coverage.valid = true;
   ValidMetric(link.coverage.maximumRangeM, 500000.0);
   ValidMetric(link.protocolResource.utilizationPercent, 25.0);
   link.protocolResource.kind = "TIMESLOT";
   link.protocolResource.capacity = 16;
   link.protocolResource.used = 4;
   link.protocolResource.remaining = 12;
   link.protocolResource.valid = true;
   nrm::WindowMetrics window;
   window.windowS = 10.0;
   ValidMetric(window.averageTransportDelayMs, 20.0);
   ValidMetric(window.deliveryRatioPercent, 98.0);
   ValidMetric(window.queueUtilizationPercent, 10.0);
   link.windows.push_back(window);
   snapshot.links.push_back(link);

   nrm::RouteResourceState route;
   route.routeId = "route-primary";
   route.sourceMemberId = "source-member";
   route.destinationMemberId = "gateway-primary";
   route.hops = {"source-member", "gateway-primary"};
   route.active = true;
   snapshot.routes.push_back(route);

   nrm::GatewayResourceState gateway;
   gateway.gatewayId = "gateway-primary-backup";
   gateway.platformId = "gateway-platform";
   gateway.ingressNetworkId = "net-primary";
   gateway.egressNetworkId = "net-backup";
   gateway.enabled = true;
   snapshot.gateways.push_back(gateway);

   nrm::BusinessFlowState flow;
   flow.flowId = "flow-primary";
   flow.businessType = "C2";
   flow.sourceMemberId = "source-member";
   flow.destinationMemberId = "gateway-primary";
   ValidMetric(flow.trafficBps, 64000.0);
   snapshot.flows.push_back(flow);

   nrm::PlatformAttitude attitude;
   attitude.platformName = "source-platform";
   attitude.headingDeg = 180.0;
   attitude.pitchDeg = 10.0;
   attitude.rollDeg = -5.0;
   attitude.sampleTime = 10.0;
   attitude.valid = true;
   snapshot.platformAttitudes.push_back(attitude);
   return snapshot;
}

bool HasReason(const nrm::ResourceSnapshotValidationResult& aResult,
               nrm::ResourceSnapshotValidationReason aReason)
{
   return std::any_of(
      aResult.issues.begin(), aResult.issues.end(),
      [aReason](const nrm::ResourceSnapshotValidationIssue& aIssue)
      {
         return aIssue.reason == aReason;
      });
}

void AssertValidityInvariant(
   const nrm::ResourceSnapshotValidationResult& aResult)
{
   assert(aResult.valid == aResult.issues.empty());
}
} // namespace

int main()
{
   const nrm::ResourceSnapshotValidator validator;
   assert(validator.Validate(ValidSnapshot()).valid);

   nrm::ResourceSnapshot invalid = ValidSnapshot();
   invalid.networks.at(1).networkId = "net-primary";
   auto result = validator.Validate(invalid);
   assert(!result.valid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cNETWORK_ID_DUPLICATE));

   invalid = ValidSnapshot();
   invalid.endpoints.front().platformId.clear();
   invalid.endpoints.front().platformName.clear();
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cPLATFORM_ID_EMPTY));

   invalid = ValidSnapshot();
   invalid.endpoints.front().networkId = "net-backup";
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cNETWORK_TYPE_MISMATCH));

   invalid = ValidSnapshot();
   invalid.links.front().destinationEndpointId = "gateway-backup";
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cLINK_NETWORK_MISMATCH));

   invalid = ValidSnapshot();
   invalid.links.front().windows.front().deliveryRatioPercent.value = 101.0;
   invalid.links.front().ber.value = -0.1;
   invalid.links.front().protocolResource.used = 17;
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cMETRIC_OUT_OF_RANGE));
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cPROTOCOL_USAGE_INVALID));
   AssertValidityInvariant(result);

   invalid = ValidSnapshot();
   ValidMetric(invalid.links.front().averageEstablishmentDelayMs,
               std::numeric_limits<double>::quiet_NaN());
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cMETRIC_OUT_OF_RANGE));
   AssertValidityInvariant(result);

   invalid = ValidSnapshot();
   ValidMetric(invalid.links.front().rssiDbm,
               std::numeric_limits<double>::infinity());
   ValidMetric(invalid.links.front().snrDb,
               -std::numeric_limits<double>::infinity());
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cMETRIC_OUT_OF_RANGE));
   AssertValidityInvariant(result);

   invalid = ValidSnapshot();
   invalid.routes.front().hops = {"source-member", "gateway-backup",
                                  "gateway-primary"};
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cROUTE_LINK_MISSING));

   invalid = ValidSnapshot();
   invalid.gateways.front().egressNetworkId = "net-primary";
   result = validator.Validate(invalid);
   assert(HasReason(result,
                    nrm::ResourceSnapshotValidationReason::cGATEWAY_SELF_REFERENCE));

   invalid = ValidSnapshot();
   ValidMetric(invalid.endpoints.front().latitudeDeg, 999.0);
   ValidMetric(invalid.endpoints.front().longitudeDeg, -999.0);
   ValidMetric(invalid.endpoints.front().altitudeM, -50.0);
   result = validator.Validate(invalid);
   assert(!result.valid);

   invalid = ValidSnapshot();
   invalid.links.front().coverage.maximumRangeM.value = -1.0;
   result = validator.Validate(invalid);
   assert(!result.valid);

   invalid = ValidSnapshot();
   invalid.flows.front().sourceMemberId = "missing-member";
   invalid.flows.front().trafficBps.value = -1.0;
   result = validator.Validate(invalid);
   assert(!result.valid);

   invalid = ValidSnapshot();
   invalid.platformAttitudes.front().headingDeg = 361.0;
   invalid.platformAttitudes.front().pitchDeg = -91.0;
   invalid.platformAttitudes.front().rollDeg = 181.0;
   invalid.platformAttitudes.front().sampleTime = -1.0;
   result = validator.Validate(invalid);
   assert(!result.valid);

   invalid = ValidSnapshot();
   ValidMetric(invalid.endpoints.front().latitudeDeg, 34.0);
   ValidMetric(invalid.endpoints.front().longitudeDeg, 108.0);
   ValidMetric(invalid.endpoints.front().altitudeM, -50.0);
   assert(validator.Validate(invalid).valid);

   return 0;
}
