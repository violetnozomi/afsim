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
   std::string platformName;
   std::string commName;
   std::string commType;
   std::string networkName;
   NetworkType networkType = NetworkType::cUNKNOWN;
   ResourceState state      = ResourceState::cUNKNOWN;
   bool canSend             = false;
   bool canReceive          = false;
   MetricValue<double> latitudeDeg;
   MetricValue<double> longitudeDeg;
   MetricValue<double> altitudeM;
   MetricValue<double> currentOfflineDurationS;
   MetricValue<double> windowOfflineDurationS;
   MetricValue<double> endpointOnlineRatioPercent;
};

struct LinkSnapshot
{
   std::string linkId;
   std::string sourceEndpointId;
   std::string destinationEndpointId;
   std::string sourcePlatform;
   std::string destinationPlatform;
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
   std::vector<WindowMetrics> windows;
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
   MessageStatistics             messages;
};

using FrameworkSnapshot = ResourceSnapshot;
} // namespace nrm

#endif
