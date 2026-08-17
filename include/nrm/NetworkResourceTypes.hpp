#ifndef NRM_NETWORK_RESOURCE_TYPES_HPP
#define NRM_NETWORK_RESOURCE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "nrm/MetricReason.hpp"

namespace nrm
{
enum class NetworkType
{
   cUNKNOWN,
   cLINK11,
   cLINK16,
   cSATCOM,
   cCDL
};

enum class DataOrigin
{
   cAFSIM_INTERNAL,
   cCUSTOMER_MODULE,
   cREPLAY,
   cPARAMETERIZED_MODEL,
   cESTIMATED,
   cDERIVED
};

enum class Confidence
{
   cLOW,
   cMEDIUM,
   cHIGH
};

enum class ResourceState
{
   cUNKNOWN,
   cOFFLINE,
   cONLINE,
   cDISABLED,
   cFAILED
};

enum class RuntimeState
{
   cIDLE,
   cINITIALIZING,
   cRUNNING,
   cCOMPLETE
};

enum class NavigationMode
{
   cUNKNOWN,
   cPERFECT,
   cGPS_ACTIVE,
   cGPS_DEGRADED,
   cGPS_EXTERNAL,
   cINS
};

inline const char* ToString(NavigationMode aMode)
{
   switch (aMode)
   {
   case NavigationMode::cPERFECT: return "PERFECT";
   case NavigationMode::cGPS_ACTIVE: return "GPS_ACTIVE";
   case NavigationMode::cGPS_DEGRADED: return "GPS_DEGRADED";
   case NavigationMode::cGPS_EXTERNAL: return "GPS_EXTERNAL";
   case NavigationMode::cINS: return "INS";
   case NavigationMode::cUNKNOWN: return "UNKNOWN";
   }
   return "UNKNOWN";
}

template<typename T>
struct MetricValue
{
   T           value{};
   std::string unit;
   bool        valid      = false;
   DataOrigin  origin     = DataOrigin::cAFSIM_INTERNAL;
   Confidence  confidence = Confidence::cLOW;
   double      sampleTime = 0.0;
   double      window     = 0.0;
   MetricReason reason    = MetricReason::cNO_SAMPLES;
};

struct MessageStatistics
{
   std::uint64_t queued        = 0;
   std::uint64_t transmitted   = 0;
   std::uint64_t received      = 0;
   std::uint64_t hops          = 0;
   std::uint64_t discarded     = 0;
   std::uint64_t routingFailed = 0;
   std::size_t   queueDepth     = 0;
};

struct WindowMetrics
{
   double              windowS = 0.0;
   MessageStatistics   messages;
   std::uint64_t       transmittedBits = 0;
   std::uint64_t       deliveredBits = 0;
   MetricValue<double> offeredLoadBps;
   MetricValue<double> deliveredThroughputBps;
   MetricValue<double> deliveryRatioPercent;
   // Deprecated compatibility field. It maps to deliveredThroughputBps.
   MetricValue<double> throughputBps;
   // Deprecated compatibility field. It maps to deliveryRatioPercent only
   // when transmit and terminal events share a correlated lifecycle.
   MetricValue<double> pdrPercent;
   MetricValue<double> averageQueueDelayMs;
   MetricValue<double> p50QueueDelayMs;
   MetricValue<double> p95QueueDelayMs;
   MetricValue<double> averageTransportDelayMs;
   MetricValue<double> p50TransportDelayMs;
   MetricValue<double> p95TransportDelayMs;
   MetricValue<double> onlineRatioPercent;
   MetricValue<double> utilizationPercent;
   MetricValue<double> queueUtilizationPercent;
   MetricValue<double> ackDelayMs;
   MetricValue<double> responseDelayMs;
   MetricValue<double> rttMs;
};

struct NetworkSnapshot
{
   std::string       networkId;
   std::string       networkName;
   std::string       modelType;
   NetworkType       networkType = NetworkType::cUNKNOWN;
   DataOrigin        origin      = DataOrigin::cAFSIM_INTERNAL;
   std::size_t       endpointCount = 0;
   std::size_t       onlineCount   = 0;
   std::size_t       activeLinks   = 0;
   MessageStatistics messages;
   std::vector<WindowMetrics> windows;
};

struct EndpointSnapshot
{
   std::string endpointId;
   std::string address;
   // Stable platform identity supplied by AFSIM or the customer contract.
   std::string platformId;
   // Human-readable/legacy platform label. Older producers may place the ID here.
   std::string platformName;
   std::string commName;
   std::string commType;
   // Stable network identity used for joins and reference validation.
   std::string networkId;
   // Human-readable/legacy network label. Older producers may place the ID here.
   std::string networkName;
   NetworkType networkType = NetworkType::cUNKNOWN;
   ResourceState state      = ResourceState::cUNKNOWN;
   bool canSend             = false;
   bool canReceive          = false;
   std::string memberRole;
   DataOrigin memberRoleOrigin = DataOrigin::cESTIMATED;
   Confidence memberRoleConfidence = Confidence::cLOW;
   MetricValue<double> latitudeDeg;
   MetricValue<double> longitudeDeg;
   MetricValue<double> altitudeM;
   MetricValue<double> currentOfflineDurationS;
   MetricValue<double> windowOfflineDurationS;
   MetricValue<double> endpointOnlineRatioPercent;
};

// Lightweight contract-facing geometry. Coordinates use WGS-84 degrees/metres.
struct GeoPoint
{
   double latitudeDeg = 0.0;
   double longitudeDeg = 0.0;
   double altitudeM = 0.0;
};

struct OperationalArea
{
   std::string areaId;
   double validFrom = 0.0;
   double validUntil = 0.0;
   std::vector<GeoPoint> points;
};

struct PlatformAttitude
{
   std::string platformName;
   double headingDeg = 0.0;
   double pitchDeg = 0.0;
   double rollDeg = 0.0;
   double sampleTime = 0.0;
   bool valid = false;
   DataOrigin origin = DataOrigin::cAFSIM_INTERNAL;
   Confidence confidence = Confidence::cHIGH;
};

struct ProtocolResourceState
{
   // LINK11=POLLING_UNIT, LINK16=TIMESLOT, SATCOM=BEAM_CHANNEL, CDL=CHANNEL.
   std::string kind;
   std::size_t capacity = 0;
   std::size_t used = 0;
   std::size_t remaining = 0;
   bool valid = false;
   MetricValue<double> utilizationPercent;
};

struct CoverageState
{
   MetricValue<double> maximumRangeM;
   MetricValue<double> rangeMarginM;
   bool insideCoverage = false;
   bool valid = false;
};

struct ResourceAlarm
{
   std::string alarmId;
   std::string severity = "WARNING";
   std::string objectId;
   std::string reasonCode;
   double startTime = 0.0;
   bool active = false;
};

struct ResourceProxyState
{
   std::string proxyId;
   std::string resourceType;
   std::string providerId;
   bool online = false;
   double lastUpdateTime = 0.0;
};

struct RouteResourceState
{
   std::string routeId;
   std::string sourceMemberId;
   std::string destinationMemberId;
   std::vector<std::string> hops;
   bool active = false;
};

struct BusinessFlowState
{
   std::string flowId;
   std::string businessType;
   std::string sourceMemberId;
   std::string destinationMemberId;
   MetricValue<double> trafficBps;
};

struct GatewayResourceState
{
   std::string gatewayId;
   std::string platformId;
   std::string ingressNetworkId;
   std::string egressNetworkId;
   bool enabled = false;
};

struct LinkSnapshot
{
   std::string linkId;
   std::string sourceEndpointId;
   std::string destinationEndpointId;
   std::string sourcePlatform;
   std::string destinationPlatform;
   // Stable network identity used for joins and reference validation.
   std::string networkId;
   // Human-readable/legacy network label. Older producers may place the ID here.
   std::string networkName;
   NetworkType networkType = NetworkType::cUNKNOWN;
   ResourceState state      = ResourceState::cUNKNOWN;
   MetricValue<double> distanceM;
   MetricValue<double> bandwidthBps;
   MetricValue<double> currentOfflineDurationS;
   MetricValue<double> windowOfflineDurationS;
   MetricValue<double> serviceAvailabilityPercent;
   MetricValue<double> establishmentSuccessRatioPercent;
   MetricValue<double> averageEstablishmentDelayMs;
   std::uint64_t establishmentAttempts = 0;
   MetricValue<double> rssiDbm;
   MetricValue<double> snrDb;
   MetricValue<double> ber;
   MetricValue<double> interferencePowerDbm;
   MetricValue<double> interferenceFactorPercent;
   MetricValue<double> atmosphericTransmittancePercent;
   MetricValue<double> terrainBlockedFlag;
   std::vector<double> availableFrequenciesHz;
   DataOrigin availableFrequenciesOrigin = DataOrigin::cPARAMETERIZED_MODEL;
   Confidence availableFrequenciesConfidence = Confidence::cLOW;
   std::vector<std::string> supportedBusinessTypes;
   MetricValue<double> communicationQualityPercent;
   std::string subnetId;
   ProtocolResourceState protocolResource;
   CoverageState coverage;
   std::vector<std::string> activeAlarmIds;
   std::size_t queueLimit = 0;
   std::vector<WindowMetrics> windows;
};

struct TerrainEnvironmentState
{
   bool available = false;
   bool enabled = false;
   std::size_t evaluatedLinkCount = 0;
   std::size_t blockedLinkCount = 0;
   std::vector<std::string> blockedLinkIds;
};

struct WeatherEnvironmentState
{
   bool available = false;
   MetricValue<double> windSpeedMps;
   MetricValue<double> windDirectionDeg;
   MetricValue<double> rainRateMmPerHour;
   MetricValue<double> cloudPercent;
   MetricValue<double> rainUpperAltitudeM;
   MetricValue<double> cloudLowerAltitudeM;
   MetricValue<double> cloudUpperAltitudeM;
   MetricValue<double> cloudWaterDensityKgPerM3;
   MetricValue<double> dustVisibilityM;
};

struct CelestialEnvironmentState
{
   bool available = false;
   bool usesSystemTime = false;
   MetricValue<double> julianDate;
   MetricValue<double> sunElevationDeg;
};

struct InterferenceEnvironmentState
{
   struct Band
   {
      std::string bandId;
      double centerFrequencyHz = 0.0;
      double bandwidthHz = 0.0;
      double powerDbm = 0.0;
      bool active = false;
   };

   bool available = false;
   std::size_t observedLinkCount = 0;
   MetricValue<double> maximumPowerDbm;
   MetricValue<double> maximumFactorPercent;
   MetricValue<double> capacityScale;
   std::vector<std::string> affectedLinkIds;
   std::vector<Band> bands;
};

struct EnvironmentSnapshot
{
   std::string schemaVersion = "nrm.environment_snapshot.v1";
   std::string configVersion;
   std::string providerId = "afsim-internal";
   DataOrigin origin = DataOrigin::cAFSIM_INTERNAL;
   Confidence confidence = Confidence::cHIGH;
   double sampleTime = 0.0;
   bool valid = false;
   TerrainEnvironmentState terrain;
   WeatherEnvironmentState weather;
   CelestialEnvironmentState celestial;
   InterferenceEnvironmentState interference;
};

struct NavigationSample
{
   // Stable customer/AFSIM platform identity used for state upsert.
   std::string platformId;
   // Human-readable/legacy platform label. Older producers may place the ID here.
   std::string platformName;
   std::string navigationType;
   std::string rawStatus;
   NavigationMode mode = NavigationMode::cUNKNOWN;
   int statusCode = 0;
   bool valid = false;
   DataOrigin origin = DataOrigin::cAFSIM_INTERNAL;
   Confidence confidence = Confidence::cHIGH;
   double sampleTime = 0.0;
   MetricValue<double> truthLatitudeDeg;
   MetricValue<double> truthLongitudeDeg;
   MetricValue<double> truthAltitudeM;
   MetricValue<double> perceivedLatitudeDeg;
   MetricValue<double> perceivedLongitudeDeg;
   MetricValue<double> perceivedAltitudeM;
   MetricValue<double> headingDeg;
   MetricValue<double> inTrackErrorM;
   MetricValue<double> crossTrackErrorM;
   MetricValue<double> verticalErrorM;
   MetricValue<double> totalPositionErrorM;
   // Parameterized 1-sigma accuracy is kept separate from observed errors.
   MetricValue<double> horizontalAccuracySigmaM;
   MetricValue<double> verticalAccuracySigmaM;
   MetricValue<double> headingAccuracySigmaDeg;
};

struct NavigationSnapshot
{
   std::string schemaVersion = "nrm.navigation_snapshot.v1";
   std::string packetFormat = "AFSIM_NAVIGATION_ERROR_HISTORY_NEH";
   std::string providerId = "afsim-internal";
   DataOrigin origin = DataOrigin::cAFSIM_INTERNAL;
   Confidence confidence = Confidence::cHIGH;
   double sampleTime = 0.0;
   bool valid = false;
   std::vector<NavigationSample> platforms;
};

struct ResourceSnapshot
{
   std::uint64_t snapshotVersion = 0;
   double        simTime         = 0.0;
   RuntimeState  runtimeState    = RuntimeState::cIDLE;
   DataOrigin    origin          = DataOrigin::cAFSIM_INTERNAL;
   std::string   providerId      = "afsim-internal";
   std::string   schemaVersion   = "nrm.snapshot.v2";
   std::string   configVersion;
   std::vector<std::string> profileIds;
   std::vector<NetworkSnapshot>  networks;
   std::vector<EndpointSnapshot> endpoints;
   std::vector<LinkSnapshot>     links;
   OperationalArea               operationalArea;
   std::vector<PlatformAttitude> platformAttitudes;
   std::vector<ResourceAlarm>    alarms;
   std::vector<ResourceProxyState> resourceProxies;
   std::vector<RouteResourceState> routes;
   std::vector<BusinessFlowState> flows;
   std::vector<GatewayResourceState> gateways;
   MessageStatistics             messages;
   EnvironmentSnapshot           environment;
   NavigationSnapshot            navigation;
};

using FrameworkSnapshot = ResourceSnapshot;
} // namespace nrm

#endif
