/**
 * @file DegradationPolicy.hpp
 * @brief Automatic L0-L3 data-coverage classification and fallback disclosure.
 */

#ifndef NRM_DEGRADATION_POLICY_HPP
#define NRM_DEGRADATION_POLICY_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class DegradationLevel
{
   cUNAVAILABLE,
   cL0_TOPOLOGY,
   cL1_LINK,
   cL2_QUALITY,
   cL3_RESOURCE
};

inline const char* ToString(DegradationLevel aLevel)
{
   switch (aLevel)
   {
   case DegradationLevel::cUNAVAILABLE: return "UNAVAILABLE";
   case DegradationLevel::cL0_TOPOLOGY: return "L0_TOPOLOGY";
   case DegradationLevel::cL1_LINK: return "L1_LINK";
   case DegradationLevel::cL2_QUALITY: return "L2_QUALITY";
   case DegradationLevel::cL3_RESOURCE: return "L3_RESOURCE";
   }
   return "UNAVAILABLE";
}

struct DegradationStatus
{
   DegradationLevel level = DegradationLevel::cUNAVAILABLE;
   bool usable = false;
   bool degraded = true;
   double dataCoveragePercent = 0.0;
   Confidence confidence = Confidence::cLOW;
   std::vector<std::string> usedDefaults;
   std::vector<std::string> missingFields;
};

class DegradationPolicy
{
public:
   DegradationStatus Evaluate(const ResourceSnapshot& aSnapshot,
                              bool aProtocolQuotasEnabled = false) const
   {
      DegradationStatus status;
      const bool hasNodes = !aSnapshot.endpoints.empty();
      const bool hasClassification = HasClassifiedEndpoint(aSnapshot);
      const bool hasOnlineState = HasKnownEndpointState(aSnapshot);
      const bool hasLinks = !aSnapshot.links.empty();
      const bool hasBandwidth = AnyLinkMetric(aSnapshot, MetricKind::cBANDWIDTH);
      const bool hasDelay = AnyLinkMetric(aSnapshot, MetricKind::cDELAY);
      const bool hasPdr = AnyLinkMetric(aSnapshot, MetricKind::cPDR);
      const bool hasRf = AnyLinkMetric(aSnapshot, MetricKind::cRF);
      const bool checks[] = {hasNodes, hasClassification, hasOnlineState, hasLinks,
                             hasBandwidth, hasDelay, hasPdr, hasRf};
      std::size_t present = 0;
      for (bool check : checks) if (check) ++present;
      status.dataCoveragePercent = 100.0 * static_cast<double>(present) / 8.0;

      Missing(status, hasNodes, "members");
      Missing(status, hasClassification, "networkType");
      Missing(status, hasOnlineState, "memberState");
      Missing(status, hasLinks, "links");
      Missing(status, hasBandwidth, "bandwidthBps");
      Missing(status, hasDelay, "delayMs");
      Missing(status, hasPdr, "pdrPercent");
      Missing(status, hasRf, "rssiDbm|snrDb|berRatio");

      if (!hasNodes || !hasClassification || !hasOnlineState) return status;
      status.usable = true;
      status.level = DegradationLevel::cL0_TOPOLOGY;
      if (!hasLinks) status.usedDefaults.push_back("PARAMETERIZED_CANDIDATE_LINKS");
      if (!hasBandwidth) status.usedDefaults.push_back("NETWORK_PROFILE_CAPACITY");
      if (!hasDelay) status.usedDefaults.push_back("DISTANCE_DELAY_ESTIMATE");
      if (!hasPdr) status.usedDefaults.push_back("PDR_ESTIMATE");
      if (!hasRf) status.usedDefaults.push_back("RF_QUALITY_UNAVAILABLE");
      if (hasLinks && hasBandwidth && hasDelay)
         status.level = DegradationLevel::cL1_LINK;
      if (status.level == DegradationLevel::cL1_LINK && hasPdr && hasRf)
         status.level = DegradationLevel::cL2_QUALITY;
      if (status.level == DegradationLevel::cL2_QUALITY && aProtocolQuotasEnabled)
      {
         status.level = DegradationLevel::cL3_RESOURCE;
         status.usedDefaults.push_back("FOUR_NETWORK_ACCEPTANCE_QUOTAS");
      }
      status.degraded = status.level != DegradationLevel::cL3_RESOURCE;
      status.confidence = status.degraded ? Confidence::cLOW : Confidence::cHIGH;
      if (status.level == DegradationLevel::cL3_RESOURCE)
         status.dataCoveragePercent = 100.0;
      return status;
   }

private:
   enum class MetricKind { cBANDWIDTH, cDELAY, cPDR, cRF };

   static void Missing(DegradationStatus& aStatus, bool aPresent,
                       const char* aName)
   {
      if (!aPresent) aStatus.missingFields.push_back(aName);
   }

   static bool HasClassifiedEndpoint(const ResourceSnapshot& aSnapshot)
   {
      return std::all_of(aSnapshot.endpoints.begin(), aSnapshot.endpoints.end(),
         [](const EndpointSnapshot& endpoint)
         { return endpoint.networkType != NetworkType::cUNKNOWN; });
   }

   static bool HasKnownEndpointState(const ResourceSnapshot& aSnapshot)
   {
      return std::all_of(aSnapshot.endpoints.begin(), aSnapshot.endpoints.end(),
         [](const EndpointSnapshot& endpoint)
         { return endpoint.state != ResourceState::cUNKNOWN; });
   }

   static bool AnyLinkMetric(const ResourceSnapshot& aSnapshot, MetricKind aKind)
   {
      for (const LinkSnapshot& link : aSnapshot.links)
      {
         if (aKind == MetricKind::cBANDWIDTH && link.bandwidthBps.valid) return true;
         if (aKind == MetricKind::cRF &&
             (link.rssiDbm.valid || link.snrDb.valid || link.ber.valid)) return true;
         for (const WindowMetrics& window : link.windows)
         {
            if (aKind == MetricKind::cDELAY && window.averageTransportDelayMs.valid) return true;
            if (aKind == MetricKind::cPDR &&
                (window.deliveryRatioPercent.valid || window.pdrPercent.valid)) return true;
         }
      }
      return false;
   }
};
} // namespace nrm

#endif
