/**
 * @file AssessmentTypes.hpp
 * @brief Stable task-assessment input, output, margin, and reason-code contracts.
 */

#ifndef NRM_ASSESSMENT_TYPES_HPP
#define NRM_ASSESSMENT_TYPES_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class AssessmentReason
{
   cDATA_INVALID,
   cNODE_NOT_FOUND,
   cNODE_OFFLINE,
   cNETWORK_TYPE_UNKNOWN,
   cNO_CURRENT_PATH,
   cLINK_NOT_ESTABLISHABLE,
   cBANDWIDTH_MARGIN_NEGATIVE,
   cDELAY_MARGIN_NEGATIVE,
   cRELIABILITY_MARGIN_NEGATIVE,
   cENVIRONMENT_HARD_BLOCKED,
   cPROFILE_CONFIG_INVALID,
   cPATH_SEARCH_LIMIT_REACHED
};

inline const char* ToString(AssessmentReason aReason)
{
   switch (aReason)
   {
   case AssessmentReason::cDATA_INVALID:
      return "DATA_INVALID";
   case AssessmentReason::cNODE_NOT_FOUND:
      return "NODE_NOT_FOUND";
   case AssessmentReason::cNODE_OFFLINE:
      return "NODE_OFFLINE";
   case AssessmentReason::cNETWORK_TYPE_UNKNOWN:
      return "NETWORK_TYPE_UNKNOWN";
   case AssessmentReason::cNO_CURRENT_PATH:
      return "NO_CURRENT_PATH";
   case AssessmentReason::cLINK_NOT_ESTABLISHABLE:
      return "LINK_NOT_ESTABLISHABLE";
   case AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE:
      return "BANDWIDTH_MARGIN_NEGATIVE";
   case AssessmentReason::cDELAY_MARGIN_NEGATIVE:
      return "DELAY_MARGIN_NEGATIVE";
   case AssessmentReason::cPROFILE_CONFIG_INVALID:
      return "PROFILE_CONFIG_INVALID";
   case AssessmentReason::cPATH_SEARCH_LIMIT_REACHED:
      return "PATH_SEARCH_LIMIT_REACHED";
   case AssessmentReason::cRELIABILITY_MARGIN_NEGATIVE:
      return "RELIABILITY_MARGIN_NEGATIVE";
   case AssessmentReason::cENVIRONMENT_HARD_BLOCKED:
      return "ENVIRONMENT_HARD_BLOCKED";
   }
   return "DATA_INVALID";
}

enum class AssessmentRouteHopKind
{
   cCURRENT_LINK,
   cCANDIDATE_LINK,
   cGATEWAY_TRANSITION
};

inline const char* ToString(AssessmentRouteHopKind aKind)
{
   switch (aKind)
   {
   case AssessmentRouteHopKind::cCURRENT_LINK:
      return "CURRENT_LINK";
   case AssessmentRouteHopKind::cCANDIDATE_LINK:
      return "CANDIDATE_LINK";
   case AssessmentRouteHopKind::cGATEWAY_TRANSITION:
      return "GATEWAY_TRANSITION";
   }
   return "CURRENT_LINK";
}

struct AssessmentRouteHop
{
   std::size_t            hopIndex = 0;
   AssessmentRouteHopKind kind = AssessmentRouteHopKind::cCURRENT_LINK;
   std::string            sourceEndpointId;
   std::string            destinationEndpointId;
   std::string            sourcePlatform;
   std::string            destinationPlatform;
   std::string            sourceNetworkId;
   std::string            destinationNetworkId;
   NetworkType            sourceNetworkType = NetworkType::cUNKNOWN;
   NetworkType            destinationNetworkType = NetworkType::cUNKNOWN;
   bool                   candidate = false;
   bool                   gateway = false;
   std::string            gatewayRouteId;
   std::string            gatewayCapabilityId;
};

struct AssessmentTask
{
   std::string              taskId;
   std::string              sourcePlatform;
   std::size_t              kShortestPaths       = 8;
   std::size_t              maximumHops          = 16;
   bool                     requireObservedCurrentMetrics = false;
   bool                     allowParameterizedCurrentMetricFallback = false;
   bool                     requireDelayMetricForFeasibility = true;
   std::string              destinationPlatform;
   std::string              businessType;
   std::uint64_t            payloadBits          = 0;
   double                   requiredBandwidthBps = 0.0;
   double                   maximumDelayMs       = 0.0;
   double                   minimumPdrPercent    = 0.0;
   std::vector<NetworkType> allowedNetworks;
};

struct AssessmentResult
{
   std::string                   taskId;
   std::uint64_t                 snapshotVersion = 0;
   double                        simTime          = 0.0;
   std::string                   disjointnessType = "DIRECTED_EDGE";
   std::size_t                   consideredPathCount = 0;
   std::size_t                   selectedPathRank = 0;
   std::vector<std::string>      failedConstraints;
   std::string                   configVersion;
   std::string                   profileProviderId;
   std::vector<std::string>      profileIds;
   bool                          reachable        = false;
   bool                          canEstablish     = false;
   bool                          canComplete      = false;
   bool                          stable           = false;
   std::vector<std::string>      primaryRoute;
   std::vector<std::string>      primaryEndpointRoute;
   std::vector<AssessmentRouteHop> primaryRouteHops;
   bool                          primaryRouteUsesCandidate = false;
   std::vector<std::string>      backupRoute;
   std::vector<AssessmentRouteHop> backupRouteHops;
   bool                          backupRouteUsesCandidate = false;
   std::vector<NetworkType>      networkSequence;
   std::vector<std::string>      gatewayRouteIds;
   std::vector<std::string>      gatewayCapabilityIds;
   MetricValue<double>           predictedDelayMs;
   MetricValue<double>           pathDistanceM;
   MetricValue<double>           maximumHopDistanceM;
   MetricValue<double>           estimatedPdrPercent;
   MetricValue<double>           bottleneckBandwidthBps;
   MetricValue<double>           bandwidthMarginBps;
   MetricValue<double>           delayMarginMs;
   MetricValue<double>           reliabilityMarginPercent;
   std::vector<AssessmentReason> reasons;
   std::vector<std::string>      recommendations;
};
} // namespace nrm

#endif
