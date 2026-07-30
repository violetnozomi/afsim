/**
 * @file AssessmentTypes.hpp
 * @brief Stable task-assessment input, output, margin, and reason-code contracts.
 */

#ifndef NRM_ASSESSMENT_TYPES_HPP
#define NRM_ASSESSMENT_TYPES_HPP

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
   cBANDWIDTH_MARGIN_NEGATIVE,
   cDELAY_MARGIN_NEGATIVE,
   cRELIABILITY_MARGIN_NEGATIVE
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
   case AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE:
      return "BANDWIDTH_MARGIN_NEGATIVE";
   case AssessmentReason::cDELAY_MARGIN_NEGATIVE:
      return "DELAY_MARGIN_NEGATIVE";
   case AssessmentReason::cRELIABILITY_MARGIN_NEGATIVE:
      return "RELIABILITY_MARGIN_NEGATIVE";
   }
   return "DATA_INVALID";
}

struct AssessmentTask
{
   std::string              taskId;
   std::string              sourcePlatform;
   std::string              destinationPlatform;
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
   bool                          reachable        = false;
   bool                          canEstablish     = false;
   bool                          canComplete      = false;
   bool                          stable           = false;
   std::vector<std::string>      primaryRoute;
   std::vector<NetworkType>      networkSequence;
   MetricValue<double>           predictedDelayMs;
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
