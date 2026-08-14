/**
 * @file ContractMetricEnricher.hpp
 * @brief Fills contract-facing fallback metrics without overwriting valid observations.
 */

#ifndef NRM_CONTRACT_METRIC_ENRICHER_HPP
#define NRM_CONTRACT_METRIC_ENRICHER_HPP

#include <algorithm>
#include <cctype>
#include <cmath>
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
         if (profile != nullptr)
         {
            if (link.availableFrequenciesHz.empty())
               link.availableFrequenciesHz = profile->frequenciesHz;
            if (link.supportedBusinessTypes.empty())
               link.supportedBusinessTypes = profile->supportedBusinessTypes;
            if (!link.coverage.maximumRangeM.valid)
               Parameterized(link.coverage.maximumRangeM, profile->maximumRangeM,
                             "m", aSnapshot.simTime, 0.0);
         }
         EnrichCoverage(link, aSnapshot.simTime);
         EnrichProtocolResource(link, aSnapshot.simTime);
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
         EnrichCommunicationQuality(link, aSnapshot.simTime);
         AddAlarms(link, aSnapshot);
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

   static void Parameterized(MetricValue<double>& aMetric, double aValue,
                             const char* aUnit, double aTime, double aWindow)
   {
      Estimate(aMetric, aValue, aUnit, aTime, aWindow);
      aMetric.origin = DataOrigin::cPARAMETERIZED_MODEL;
   }

   static double Clamp(double aValue, double aMinimum, double aMaximum)
   {
      return std::max(aMinimum, std::min(aMaximum, aValue));
   }

   static const char* ProtocolKind(NetworkType aType)
   {
      switch (aType)
      {
      case NetworkType::cLINK11: return "POLLING_UNIT";
      case NetworkType::cLINK16: return "TIMESLOT";
      case NetworkType::cSATCOM: return "BEAM_CHANNEL";
      case NetworkType::cCDL: return "CHANNEL";
      case NetworkType::cUNKNOWN: return "UNKNOWN";
      }
      return "UNKNOWN";
   }

   static void EnrichProtocolResource(LinkSnapshot& aLink, double aTime)
   {
      ProtocolResourceState& resource = aLink.protocolResource;
      if (resource.kind.empty()) resource.kind = ProtocolKind(aLink.networkType);
      resource.remaining = resource.capacity > resource.used
                              ? resource.capacity - resource.used : 0;
      if (resource.valid && resource.capacity > 0)
      {
         Estimate(resource.utilizationPercent,
                  Clamp(100.0 * resource.used / static_cast<double>(resource.capacity),
                        0.0, 100.0),
                  "percent", aTime, 0.0);
      }
      else if (resource.valid)
      {
         resource.utilizationPercent.valid = false;
         resource.utilizationPercent.unit = "percent";
         resource.utilizationPercent.reason = MetricReason::cZERO_DENOMINATOR;
      }
   }

   static void EnrichCoverage(LinkSnapshot& aLink, double aTime)
   {
      if (!aLink.coverage.maximumRangeM.valid) return;
      aLink.coverage.valid = true;
      if (aLink.distanceM.valid)
      {
         Parameterized(aLink.coverage.rangeMarginM,
                       aLink.coverage.maximumRangeM.value - aLink.distanceM.value,
                       "m", aTime, 0.0);
         aLink.coverage.insideCoverage = aLink.coverage.rangeMarginM.value >= 0.0;
      }
   }

   static void EnrichCommunicationQuality(LinkSnapshot& aLink, double aTime)
   {
      if (aLink.communicationQualityPercent.valid || aLink.windows.empty()) return;
      const WindowMetrics& window = aLink.windows.back();
      double score = 0.0;
      std::size_t count = 0;
      const MetricValue<double>& pdr = window.pdrPercent.valid
                                         ? window.pdrPercent
                                         : window.deliveryRatioPercent;
      if (pdr.valid)
      {
         score += Clamp(pdr.value, 0.0, 100.0);
         ++count;
      }
      if (aLink.snrDb.valid)
      {
         score += Clamp((aLink.snrDb.value + 3.0) / 13.0 * 100.0, 0.0, 100.0);
         ++count;
      }
      if (aLink.ber.valid)
      {
         score += Clamp((1.0 - aLink.ber.value / 0.01) * 100.0, 0.0, 100.0);
         ++count;
      }
      if (window.averageTransportDelayMs.valid)
      {
         score += Clamp(100.0 - window.averageTransportDelayMs.value / 10.0,
                        0.0, 100.0);
         ++count;
      }
      if (count > 0)
      {
         Estimate(aLink.communicationQualityPercent, score / count, "percent",
                  aTime, window.windowS);
         aLink.communicationQualityPercent.origin = DataOrigin::cDERIVED;
      }
   }

   static void AddAlarms(LinkSnapshot& aLink, ResourceSnapshot& aSnapshot)
   {
      if (aLink.state != ResourceState::cOFFLINE &&
          aLink.state != ResourceState::cFAILED) return;
      ResourceAlarm alarm;
      alarm.objectId = aLink.linkId;
      alarm.reasonCode = aLink.state == ResourceState::cFAILED
                            ? "LINK_FAILED" : "LINK_OFFLINE";
      alarm.alarmId = "alarm:" + aLink.linkId + ":" + alarm.reasonCode;
      alarm.startTime = aSnapshot.simTime;
      alarm.active = true;
      if (std::find(aLink.activeAlarmIds.begin(), aLink.activeAlarmIds.end(), alarm.alarmId) ==
          aLink.activeAlarmIds.end())
         aLink.activeAlarmIds.push_back(alarm.alarmId);
      const auto existing = std::find_if(aSnapshot.alarms.begin(), aSnapshot.alarms.end(),
         [&alarm](const ResourceAlarm& aValue) { return aValue.alarmId == alarm.alarmId; });
      if (existing == aSnapshot.alarms.end()) aSnapshot.alarms.push_back(alarm);
   }

   NetworkProfileRepository mProfiles;
   std::size_t mDefaultQueueLimit;
};
} // namespace nrm

#endif
