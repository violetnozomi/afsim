/**
 * @file ContractMetricEnricher.hpp
 * @brief Fills contract-facing fallback metrics without overwriting valid observations.
 */

#ifndef NRM_CONTRACT_METRIC_ENRICHER_HPP
#define NRM_CONTRACT_METRIC_ENRICHER_HPP

#include <algorithm>
#include <cctype>
#include <string>

#include "nrm/NetworkProfileRepository.hpp"

namespace nrm
{
class ContractMetricEnricher
{
public:
   explicit ContractMetricEnricher(
      const NetworkProfileRepository& aProfiles = NetworkProfileRepository::BuiltInDemo(),
      std::size_t aDefaultQueueLimit = 128)
      : mProfiles(aProfiles)
      , mDefaultQueueLimit(aDefaultQueueLimit)
   {
   }

   void Apply(ResourceSnapshot& aSnapshot) const
   {
      for (EndpointSnapshot& endpoint : aSnapshot.endpoints)
      {
         if (endpoint.memberRole.empty())
         {
            endpoint.memberRole = Role(endpoint);
            endpoint.memberRoleOrigin = DataOrigin::cESTIMATED;
            endpoint.memberRoleConfidence = Confidence::cLOW;
         }
      }
      for (LinkSnapshot& link : aSnapshot.links)
      {
         const NetworkProfile* profile = mProfiles.Find(link.networkType);
         if (link.availableFrequenciesHz.empty() && profile != nullptr)
            link.availableFrequenciesHz = profile->frequenciesHz;
         if (link.queueLimit == 0) link.queueLimit = mDefaultQueueLimit;
         for (WindowMetrics& window : link.windows)
         {
            if (!window.queueUtilizationPercent.valid && link.queueLimit > 0)
               Estimate(window.queueUtilizationPercent,
                        std::min(100.0, 100.0 * window.messages.queueDepth /
                                           static_cast<double>(link.queueLimit)),
                        "percent", aSnapshot.simTime, window.windowS);
            if (window.averageTransportDelayMs.valid)
            {
               if (!window.ackDelayMs.valid)
                  Estimate(window.ackDelayMs, window.averageTransportDelayMs.value,
                           "ms", aSnapshot.simTime, window.windowS);
               if (!window.responseDelayMs.valid)
                  Estimate(window.responseDelayMs, window.averageTransportDelayMs.value * 2.0,
                           "ms", aSnapshot.simTime, window.windowS);
               if (!window.rttMs.valid)
                  Estimate(window.rttMs, window.averageTransportDelayMs.value * 2.0,
                           "ms", aSnapshot.simTime, window.windowS);
            }
         }
      }
   }

private:
   static std::string Role(const EndpointSnapshot& aEndpoint)
   {
      std::string text = aEndpoint.platformName + " " + aEndpoint.commName;
      std::transform(text.begin(), text.end(), text.begin(),
                     [](unsigned char value) { return std::tolower(value); });
      if (text.find("control") != std::string::npos ||
          text.find("command") != std::string::npos)
         return aEndpoint.networkType == NetworkType::cLINK11
                   ? "NET_CONTROL_STATION" : "COMMAND_NODE";
      if (text.find("relay") != std::string::npos ||
          text.find("gateway") != std::string::npos) return "RELAY_GATEWAY";
      if (text.find("sensor") != std::string::npos) return "SENSOR_SOURCE";
      if (aEndpoint.networkType == NetworkType::cSATCOM) return "SATCOM_TERMINAL";
      return "NETWORK_MEMBER";
   }

   static void Estimate(MetricValue<double>& aMetric, double aValue,
                        const char* aUnit, double aTime, double aWindow)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = DataOrigin::cESTIMATED;
      aMetric.confidence = Confidence::cLOW;
      aMetric.sampleTime = aTime;
      aMetric.window = aWindow;
      aMetric.reason = MetricReason::cNONE;
   }

   NetworkProfileRepository mProfiles;
   std::size_t mDefaultQueueLimit;
};
} // namespace nrm

#endif
