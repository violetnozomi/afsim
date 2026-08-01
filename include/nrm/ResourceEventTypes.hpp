#ifndef NRM_RESOURCE_EVENT_TYPES_HPP
#define NRM_RESOURCE_EVENT_TYPES_HPP

#include <cstdint>
#include <string>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class ResourceEventKind
{
   cENDPOINT_STATE,
   cLINK_STATE,
   cESTABLISHMENT_ATTEMPTED,
   cESTABLISHMENT_SUCCEEDED,
   cESTABLISHMENT_FAILED
};

inline const char* ToString(ResourceEventKind aKind)
{
   switch (aKind)
   {
   case ResourceEventKind::cENDPOINT_STATE: return "ENDPOINT_STATE";
   case ResourceEventKind::cLINK_STATE: return "LINK_STATE";
   case ResourceEventKind::cESTABLISHMENT_ATTEMPTED: return "ESTABLISHMENT_ATTEMPTED";
   case ResourceEventKind::cESTABLISHMENT_SUCCEEDED: return "ESTABLISHMENT_SUCCEEDED";
   case ResourceEventKind::cESTABLISHMENT_FAILED: return "ESTABLISHMENT_FAILED";
   }
   return "ENDPOINT_STATE";
}

struct ResourceEvent
{
   std::string eventId;
   ResourceEventKind kind = ResourceEventKind::cENDPOINT_STATE;
   double simTime = 0.0;
   std::string reasonCode;
   std::string networkId;
   std::string linkId;
   std::string sourceEndpointId;
   std::string destinationEndpointId;
   std::string endpointId;
   std::string correlationId;
   ResourceState previousState = ResourceState::cUNKNOWN;
   ResourceState currentState = ResourceState::cUNKNOWN;
};

struct ResourceStateMetrics
{
   MetricValue<double> currentOfflineDurationS;
   MetricValue<double> windowOfflineDurationS;
   MetricValue<double> endpointOnlineRatioPercent;
   MetricValue<double> serviceAvailabilityPercent;
   std::uint64_t establishmentAttempts = 0;
   std::uint64_t establishmentSuccesses = 0;
   MetricValue<double> establishmentSuccessRatioPercent;
   MetricValue<double> averageEstablishmentDelayMs;
};
} // namespace nrm

#endif
