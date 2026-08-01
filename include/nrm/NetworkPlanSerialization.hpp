#ifndef NRM_NETWORK_PLAN_SERIALIZATION_HPP
#define NRM_NETWORK_PLAN_SERIALIZATION_HPP

#include <ostream>
#include <string>

#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/NetworkTypeUtils.hpp"

namespace nrm
{
namespace network_plan_serialization
{
inline std::string EscapeJson(const std::string& aValue)
{
   std::string output;
   output.reserve(aValue.size());
   for (unsigned char character : aValue)
   {
      switch (character)
      {
      case '"': output += "\\\""; break;
      case '\\': output += "\\\\"; break;
      case '\b': output += "\\b"; break;
      case '\f': output += "\\f"; break;
      case '\n': output += "\\n"; break;
      case '\r': output += "\\r"; break;
      case '\t': output += "\\t"; break;
      default:
         if (character < 0x20)
         {
            const char hex[] = "0123456789abcdef";
            output += "\\u00";
            output += hex[(character >> 4) & 0x0f];
            output += hex[character & 0x0f];
         }
         else
         {
            output += static_cast<char>(character);
         }
      }
   }
   return output;
}

inline void WriteMetric(std::ostream& aOutput, const MetricValue<double>& aMetric)
{
   aOutput << "{\"valid\":" << (aMetric.valid ? "true" : "false")
           << ",\"value\":";
   if (aMetric.valid) aOutput << aMetric.value;
   else aOutput << "null";
   aOutput << ",\"unit\":\"" << EscapeJson(aMetric.unit)
           << "\",\"origin\":\"" << ToString(aMetric.origin)
           << "\",\"confidence\":\"" << ToString(aMetric.confidence)
           << "\",\"reason\":\"" << ToString(aMetric.reason) << "\"}";
}

inline void WriteCapability(std::ostream& aOutput, const CapabilityResult& aCapability)
{
   aOutput << "{\"schemaVersion\":\"" << EscapeJson(aCapability.schemaVersion)
           << "\",\"requestId\":\"" << EscapeJson(aCapability.requestId)
           << "\",\"snapshotVersion\":" << aCapability.snapshotVersion
           << ",\"simTime\":" << aCapability.simTime
           << ",\"configVersion\":\"" << EscapeJson(aCapability.configVersion)
           << "\",\"profileProviderId\":\""
           << EscapeJson(aCapability.profileProviderId)
           << "\",\"profileIds\":[";
   for (std::size_t index = 0; index < aCapability.profileIds.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << EscapeJson(aCapability.profileIds[index]) << '"';
   }
   aOutput << "],\"requestValid\":"
           << (aCapability.requestValid ? "true" : "false")
           << ",\"pathAvailable\":" << (aCapability.pathAvailable ? "true" : "false")
           << ",\"usesCandidate\":" << (aCapability.usesCandidate ? "true" : "false")
           << ",\"route\":[";
   for (std::size_t index = 0; index < aCapability.route.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << EscapeJson(aCapability.route[index]) << '"';
   }
   aOutput << "],\"endpointRoute\":[";
   for (std::size_t index = 0; index < aCapability.endpointRoute.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << EscapeJson(aCapability.endpointRoute[index]) << '"';
   }
   aOutput << "],\"networkSequence\":[";
   for (std::size_t index = 0; index < aCapability.networkSequence.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << ToString(aCapability.networkSequence[index]) << '"';
   }
   aOutput << "],\"communicationDistanceM\":";
   WriteMetric(aOutput, aCapability.communicationDistanceM);
   aOutput << ",\"maximumHopDistanceM\":";
   WriteMetric(aOutput, aCapability.maximumHopDistanceM);
   aOutput << ",\"transmissionRateBps\":";
   WriteMetric(aOutput, aCapability.transmissionRateBps);
   aOutput << ",\"packetLossPercent\":";
   WriteMetric(aOutput, aCapability.packetLossPercent);
   aOutput << ",\"transmissionDelayMs\":";
   WriteMetric(aOutput, aCapability.transmissionDelayMs);
   aOutput << ",\"networkThroughputBps\":";
   WriteMetric(aOutput, aCapability.networkThroughputBps);
   aOutput << ",\"accessRatioPercent\":";
   WriteMetric(aOutput, aCapability.accessRatioPercent);
   aOutput << ",\"reasons\":[";
   for (std::size_t index = 0; index < aCapability.reasons.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << ToString(aCapability.reasons[index]) << '"';
   }
   aOutput << "],\"environmentEffects\":[";
   for (std::size_t index = 0; index < aCapability.environmentEffects.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      const EnvironmentEffect& effect = aCapability.environmentEffects[index];
      aOutput << "{\"domain\":\"" << ToString(effect.domain)
              << "\",\"valid\":" << (effect.valid ? "true" : "false")
              << ",\"origin\":\"" << ToString(effect.origin)
              << "\",\"confidence\":\"" << ToString(effect.confidence)
              << "\",\"reason\":\"" << ToString(effect.reason)
              << "\",\"providerId\":\"" << EscapeJson(effect.providerId)
              << "\",\"effectId\":\"" << EscapeJson(effect.effectId)
              << "\",\"sampleTime\":" << effect.sampleTime
              << ",\"pathLossDeltaDb\":";
      WriteMetric(aOutput, effect.pathLossDeltaDb);
      aOutput << ",\"capacityScale\":";
      WriteMetric(aOutput, effect.capacityScale);
      aOutput << ",\"packetLossDeltaPercent\":";
      WriteMetric(aOutput, effect.packetLossDeltaPercent);
      aOutput << ",\"delayDeltaMs\":";
      WriteMetric(aOutput, effect.delayDeltaMs);
      aOutput << '}';
   }
   aOutput << "]}";
}

inline void WriteValidation(std::ostream& aOutput,
                            const PlanValidationResult& aResult,
                            const std::string& aRunId = std::string())
{
   aOutput << "{\"schemaVersion\":\"" << EscapeJson(aResult.schemaVersion) << '"';
   if (!aRunId.empty())
      aOutput << ",\"runId\":\"" << EscapeJson(aRunId) << '"';
   aOutput << ",\"planId\":\"" << EscapeJson(aResult.planId)
           << "\",\"revision\":" << aResult.revision
           << ",\"planFingerprint\":\"" << EscapeJson(aResult.planFingerprint) << '"'
           << ",\"passed\":" << (aResult.passed ? "true" : "false")
           << ",\"issues\":[";
   for (std::size_t index = 0; index < aResult.issues.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      const PlanValidationIssue& issue = aResult.issues[index];
      aOutput << "{\"reason\":\"" << ToString(issue.reason)
              << "\",\"field\":\"" << EscapeJson(issue.field)
              << "\",\"recordId\":\"" << EscapeJson(issue.recordId)
              << "\",\"severity\":\"" << ToString(issue.severity)
              << "\",\"description\":\"" << EscapeJson(issue.description) << "\"}";
   }
   aOutput << "]}";
}

inline void WriteEvaluation(std::ostream& aOutput,
                            const NetworkPlanEvaluationResult& aResult,
                            const std::string& aRunId = std::string())
{
   aOutput << "{\"schemaVersion\":\"" << EscapeJson(aResult.schemaVersion) << '"';
   if (!aRunId.empty())
      aOutput << ",\"runId\":\"" << EscapeJson(aRunId) << '"';
   aOutput << ",\"planId\":\"" << EscapeJson(aResult.planId)
           << "\",\"revision\":" << aResult.revision
           << ",\"planFingerprint\":\"" << EscapeJson(aResult.planFingerprint) << '"'
           << ",\"snapshotVersion\":" << aResult.snapshotVersion
           << ",\"simTime\":" << aResult.simTime
           << ",\"overallStatus\":\"" << ToString(aResult.overallStatus)
           << "\",\"resultingState\":\"" << ToString(aResult.resultingState)
           << "\",\"demands\":[";
   for (std::size_t index = 0; index < aResult.demands.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      const PlanDemandEvaluation& demand = aResult.demands[index];
      aOutput << "{\"demandId\":\"" << EscapeJson(demand.demandId)
              << "\",\"status\":\"" << ToString(demand.status)
              << "\",\"reasons\":[";
      for (std::size_t reasonIndex = 0; reasonIndex < demand.reasons.size(); ++reasonIndex)
      {
         if (reasonIndex != 0) aOutput << ',';
         aOutput << '"' << ToString(demand.reasons[reasonIndex]) << '"';
      }
      aOutput << "],\"capability\":";
      WriteCapability(aOutput, demand.capability);
      aOutput << '}';
   }
   aOutput << "]}";
}
} // namespace network_plan_serialization
} // namespace nrm

#endif
