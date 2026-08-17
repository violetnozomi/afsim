#ifndef NRM_COMMUNICATION_CAPABILITY_TYPES_HPP
#define NRM_COMMUNICATION_CAPABILITY_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class CapabilityReason
{
   cNONE,
   cINVALID_REQUEST,
   cNON_FINITE_INPUT,
   cNODE_NOT_FOUND,
   cNODE_OFFLINE,
   cNO_PATH,
   cPATH_METRIC_INVALID,
   cPROFILE_CONFIG_INVALID,
   cPATH_SEARCH_LIMIT_REACHED,
   cPARAMETERIZED_CANDIDATE,
   cTHROUGHPUT_NOT_OBSERVED,
   cACCESS_DENOMINATOR_ZERO,
   cENVIRONMENT_DATA_UNAVAILABLE,
   cENVIRONMENT_HARD_BLOCKED,
   cOPERATIONAL_AREA_OUTSIDE,
   cATTITUDE_MISMATCH,
   cINTERFERENCE_CONFLICT
};

inline const char* ToString(CapabilityReason aReason)
{
   switch (aReason)
   {
   case CapabilityReason::cNONE: return "NONE";
   case CapabilityReason::cINVALID_REQUEST: return "INVALID_REQUEST";
   case CapabilityReason::cNON_FINITE_INPUT: return "NON_FINITE_INPUT";
   case CapabilityReason::cNODE_NOT_FOUND: return "NODE_NOT_FOUND";
   case CapabilityReason::cNODE_OFFLINE: return "NODE_OFFLINE";
   case CapabilityReason::cNO_PATH: return "NO_PATH";
   case CapabilityReason::cPATH_METRIC_INVALID: return "PATH_METRIC_INVALID";
   case CapabilityReason::cPROFILE_CONFIG_INVALID: return "PROFILE_CONFIG_INVALID";
   case CapabilityReason::cPATH_SEARCH_LIMIT_REACHED: return "PATH_SEARCH_LIMIT_REACHED";
   case CapabilityReason::cPARAMETERIZED_CANDIDATE: return "PARAMETERIZED_CANDIDATE";
   case CapabilityReason::cTHROUGHPUT_NOT_OBSERVED: return "THROUGHPUT_NOT_OBSERVED";
   case CapabilityReason::cACCESS_DENOMINATOR_ZERO: return "ACCESS_DENOMINATOR_ZERO";
   case CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE:
      return "ENVIRONMENT_DATA_UNAVAILABLE";
   case CapabilityReason::cENVIRONMENT_HARD_BLOCKED:
      return "ENVIRONMENT_HARD_BLOCKED";
   case CapabilityReason::cOPERATIONAL_AREA_OUTSIDE: return "OPERATIONAL_AREA_OUTSIDE";
   case CapabilityReason::cATTITUDE_MISMATCH: return "ATTITUDE_MISMATCH";
   case CapabilityReason::cINTERFERENCE_CONFLICT: return "INTERFERENCE_CONFLICT";
   }
   return "INVALID_REQUEST";
}

enum class EnvironmentDomain
{
   cTERRAIN,
   cWEATHER,
   cCELESTIAL,
   cELECTROMAGNETIC_INTERFERENCE
};

inline const char* ToString(EnvironmentDomain aDomain)
{
   switch (aDomain)
   {
   case EnvironmentDomain::cTERRAIN: return "TERRAIN";
   case EnvironmentDomain::cWEATHER: return "WEATHER";
   case EnvironmentDomain::cCELESTIAL: return "CELESTIAL";
   case EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE:
      return "ELECTROMAGNETIC_INTERFERENCE";
   }
   return "TERRAIN";
}

enum class EnvironmentApplicationMode
{
   cINFORMATION_ONLY,
   cALREADY_INCLUDED,
   cCANDIDATE_ADJUSTMENT
};

inline const char* ToString(EnvironmentApplicationMode aMode)
{
   switch (aMode)
   {
   case EnvironmentApplicationMode::cINFORMATION_ONLY: return "INFORMATION_ONLY";
   case EnvironmentApplicationMode::cALREADY_INCLUDED: return "ALREADY_INCLUDED";
   case EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT:
      return "CANDIDATE_ADJUSTMENT";
   }
   return "INFORMATION_ONLY";
}

struct EnvironmentContext
{
   std::string contextId;
   std::string schemaVersion;
   std::string providerId;
   double sampleTime = 0.0;
   DataOrigin origin = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
   EnvironmentApplicationMode applicationMode =
      EnvironmentApplicationMode::cINFORMATION_ONLY;
   // Compatibility mirror for integrations compiled before applicationMode.
   bool applyParameterizedEffects = false;
   // Customer applicationMode applies only to domains actually supplied by
   // that customer update.  An empty list preserves the legacy all-domain
   // meaning for direct integrations that predate this field.
   std::vector<EnvironmentDomain> customerProvidedDomains;
   double validFrom = 0.0;
   double validUntil = 0.0;
};

struct EnvironmentEffect
{
   EnvironmentDomain domain = EnvironmentDomain::cTERRAIN;
   bool valid = false;
   DataOrigin origin = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   CapabilityReason reason = CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE;
   std::string providerId;
   std::string effectId;
   bool hardBlocked = false;
   std::vector<std::string> evidence;
   double sampleTime = 0.0;
   MetricValue<double> pathLossDeltaDb;
   MetricValue<double> capacityScale;
   MetricValue<double> packetLossDeltaPercent;
   MetricValue<double> delayDeltaMs;
};

struct CapabilityRequest
{
   std::string requestId;
   std::string sourcePlatform;
   std::string destinationPlatform;
   std::string businessType;
   std::uint64_t payloadBits = 0;
   double requiredBandwidthBps = 0.0;
   double maximumDelayMs = 0.0;
   double minimumPdrPercent = 0.0;
   std::size_t kShortestPaths = 8;
   std::size_t maximumHops = 16;
   std::vector<NetworkType> allowedNetworks;
   double taskStartTime = 0.0;
   double taskEndTime = 0.0;
   double frequencyHz = 0.0;
   double occupiedBandwidthHz = 0.0;
   bool directionalAntenna = false;
   double headingToleranceDeg = 30.0;
};

struct CapabilityResult
{
   std::string schemaVersion = "nrm.capability.v1";
   std::string requestId;
   std::uint64_t snapshotVersion = 0;
   double simTime = 0.0;
   std::string configVersion;
   std::string profileProviderId;
   std::vector<std::string> profileIds;
   bool requestValid = false;
   bool pathAvailable = false;
   bool usesCandidate = false;
   std::vector<std::string> route;
   std::vector<std::string> endpointRoute;
   std::vector<NetworkType> networkSequence;
   MetricValue<double> communicationDistanceM;
   MetricValue<double> maximumHopDistanceM;
   MetricValue<double> transmissionRateBps;
   MetricValue<double> packetLossPercent;
   MetricValue<double> transmissionDelayMs;
   MetricValue<double> networkThroughputBps;
   MetricValue<double> accessRatioPercent;
   std::vector<EnvironmentEffect> environmentEffects;
   std::vector<CapabilityReason> reasons;
};

class EnvironmentEffectAdapter
{
public:
   virtual ~EnvironmentEffectAdapter() = default;

   virtual EnvironmentEffect Evaluate(EnvironmentDomain aDomain,
                                      const ResourceSnapshot& aSnapshot,
                                      const CapabilityRequest& aRequest,
                                      const std::vector<std::string>& aEndpointRoute,
                                      const EnvironmentContext& aContext) const = 0;
};
} // namespace nrm

#endif
