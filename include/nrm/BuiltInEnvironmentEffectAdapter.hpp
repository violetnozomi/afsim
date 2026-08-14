#ifndef NRM_BUILT_IN_ENVIRONMENT_EFFECT_ADAPTER_HPP
#define NRM_BUILT_IN_ENVIRONMENT_EFFECT_ADAPTER_HPP

// Converts AFSIM environment observations into auditable capability evidence.
// Parameterized degradation is emitted only for candidate links.

#include <algorithm>
#include <cmath>

#include "nrm/EnvironmentConfigRepository.hpp"
#include "nrm/OperationalEnvironmentEvaluator.hpp"

namespace nrm
{
class BuiltInEnvironmentEffectAdapter final : public EnvironmentEffectAdapter
{
public:
   explicit BuiltInEnvironmentEffectAdapter(const EnvironmentConfigRepository& aConfig)
      : mConfig(aConfig)
   {
   }

   EnvironmentEffect Evaluate(EnvironmentDomain aDomain,
                              const ResourceSnapshot& aSnapshot,
                              const CapabilityRequest& aRequest,
                              const std::vector<std::string>& aEndpointRoute,
                              const EnvironmentContext& aContext) const override
   {
      EnvironmentEffect effect;
      effect.domain = aDomain;
      effect.providerId = aContext.providerId.empty()
                             ? aSnapshot.environment.providerId
                             : aContext.providerId;
      effect.sampleTime = aSnapshot.environment.sampleTime;
      effect.origin = DataOrigin::cAFSIM_INTERNAL;
      effect.confidence = Confidence::cHIGH;

      if (aContext.applyParameterizedEffects && aDomain == EnvironmentDomain::cTERRAIN)
      {
         const char* blockEvidence = OperationalBlock(aSnapshot, aRequest, aEndpointRoute);
         if (blockEvidence != nullptr)
         {
            effect.valid = true;
            effect.hardBlocked = true;
            effect.reason = CapabilityReason::cENVIRONMENT_HARD_BLOCKED;
            effect.effectId = "operational-constraint";
            effect.evidence.push_back(blockEvidence);
            Set(effect.pathLossDeltaDb, 0.0, "dB", effect.sampleTime, effect.origin, effect.confidence);
            Set(effect.capacityScale, 0.0, "ratio", effect.sampleTime, effect.origin, effect.confidence);
            Set(effect.packetLossDeltaPercent, 100.0, "percentage_point", effect.sampleTime, effect.origin, effect.confidence);
            Set(effect.delayDeltaMs, 0.0, "ms", effect.sampleTime, effect.origin, effect.confidence);
            return effect;
         }
      }

      const bool available = DomainAvailable(aDomain, aSnapshot.environment);
      if (!available)
      {
         effect.reason = CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE;
         return effect;
      }

      effect.valid = true;
      effect.reason = CapabilityReason::cNONE;
      effect.effectId = std::string("afsim-") + ToString(aDomain);
      Set(effect.pathLossDeltaDb, 0.0, "dB", effect.sampleTime,
          DataOrigin::cAFSIM_INTERNAL, Confidence::cHIGH);
      Set(effect.capacityScale, 1.0, "ratio", effect.sampleTime,
          DataOrigin::cAFSIM_INTERNAL, Confidence::cHIGH);
      Set(effect.packetLossDeltaPercent, 0.0, "percentage_point", effect.sampleTime,
          DataOrigin::cAFSIM_INTERNAL, Confidence::cHIGH);
      Set(effect.delayDeltaMs, 0.0, "ms", effect.sampleTime,
          DataOrigin::cAFSIM_INTERNAL, Confidence::cHIGH);
      effect.evidence.push_back("AFSIM_CURRENT_STATE_NOT_DOUBLE_APPLIED");

      if (aDomain == EnvironmentDomain::cTERRAIN &&
          RouteTerrainBlocked(aSnapshot, aEndpointRoute))
      {
         effect.hardBlocked = true;
         effect.reason = CapabilityReason::cENVIRONMENT_HARD_BLOCKED;
         effect.evidence.push_back("AFSIM_TERRAIN_MASKED");
         return effect;
      }

      if (!aContext.applyParameterizedEffects) return effect;
      if (aDomain == EnvironmentDomain::cWEATHER &&
          (!WeatherActive(aSnapshot.environment) || !TimeApplicable(aRequest, aContext)))
         return effect;
      if (aDomain == EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE &&
          !InterferenceActive(aSnapshot.environment, aRequest)) return effect;

      const NetworkType networkType = RouteNetwork(aSnapshot, aEndpointRoute);
      const EnvironmentAdjustment* adjustment = mConfig.Find(aDomain, networkType);
      if (adjustment == nullptr) return effect;
      effect.effectId = adjustment->adjustmentId;
      effect.origin = DataOrigin::cPARAMETERIZED_MODEL;
      effect.confidence = Confidence::cLOW;
      effect.hardBlocked = adjustment->hardBlocked;
      Set(effect.pathLossDeltaDb, adjustment->pathLossDeltaDb, "dB",
          effect.sampleTime, effect.origin, effect.confidence);
      Set(effect.capacityScale, adjustment->capacityScale, "ratio",
          effect.sampleTime, effect.origin, effect.confidence);
      Set(effect.packetLossDeltaPercent, adjustment->packetLossDeltaPercent,
          "percentage_point", effect.sampleTime, effect.origin, effect.confidence);
      Set(effect.delayDeltaMs, adjustment->delayDeltaMs, "ms",
          effect.sampleTime, effect.origin, effect.confidence);
      effect.evidence.clear();
      effect.evidence.push_back("PARAMETERIZED_CANDIDATE_EFFECT");
      return effect;
   }

private:
   static bool DomainAvailable(EnvironmentDomain aDomain,
                               const EnvironmentSnapshot& aEnvironment)
   {
      switch (aDomain)
      {
      case EnvironmentDomain::cTERRAIN:
         return aEnvironment.terrain.available && aEnvironment.terrain.enabled;
      case EnvironmentDomain::cWEATHER: return aEnvironment.weather.available;
      case EnvironmentDomain::cCELESTIAL: return aEnvironment.celestial.available;
      case EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE:
         return aEnvironment.interference.available;
      }
      return false;
   }

   static bool WeatherActive(const EnvironmentSnapshot& aEnvironment)
   {
      return (aEnvironment.weather.rainRateMmPerHour.valid &&
              aEnvironment.weather.rainRateMmPerHour.value > 0.0) ||
             (aEnvironment.weather.cloudWaterDensityKgPerM3.valid &&
              aEnvironment.weather.cloudWaterDensityKgPerM3.value > 0.0) ||
             (aEnvironment.weather.dustVisibilityM.valid &&
              aEnvironment.weather.dustVisibilityM.value > 0.0 &&
              aEnvironment.weather.dustVisibilityM.value < 100000.0);
   }

   static bool InterferenceActive(const EnvironmentSnapshot& aEnvironment,
                                  const CapabilityRequest& aRequest)
   {
      if (!aEnvironment.interference.bands.empty() && aRequest.frequencyHz > 0.0)
      {
         for (const InterferenceEnvironmentState::Band& band : aEnvironment.interference.bands)
            if (band.active && OperationalEnvironmentEvaluator::BandsOverlap(
                                  aRequest.frequencyHz, aRequest.occupiedBandwidthHz,
                                  band.centerFrequencyHz, band.bandwidthHz)) return true;
         return false;
      }
      return aEnvironment.interference.observedLinkCount > 0 ||
             (aEnvironment.interference.maximumFactorPercent.valid &&
              aEnvironment.interference.maximumFactorPercent.value > 0.0);
   }

   static bool TimeApplicable(const CapabilityRequest& aRequest,
                              const EnvironmentContext& aContext)
   {
      if (aRequest.taskStartTime == 0.0 && aRequest.taskEndTime == 0.0) return true;
      return OperationalEnvironmentEvaluator::Covers(
         aContext.validFrom, aContext.validUntil,
         aRequest.taskStartTime, aRequest.taskEndTime);
   }

   static const EndpointSnapshot* Endpoint(const ResourceSnapshot& aSnapshot,
                                           const std::string& aId)
   {
      for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
         if (endpoint.endpointId == aId) return &endpoint;
      return nullptr;
   }

   static GeoPoint Point(const EndpointSnapshot& aEndpoint)
   {
      return {aEndpoint.latitudeDeg.value, aEndpoint.longitudeDeg.value,
              aEndpoint.altitudeM.value};
   }

   static const char* OperationalBlock(const ResourceSnapshot& aSnapshot,
                                       const CapabilityRequest& aRequest,
                                       const std::vector<std::string>& aRoute)
   {
      if (aRoute.size() < 2) return nullptr;
      const EndpointSnapshot* source = Endpoint(aSnapshot, aRoute.front());
      const EndpointSnapshot* destination = Endpoint(aSnapshot, aRoute.back());
      if (source == nullptr || destination == nullptr) return nullptr;
      if (!aSnapshot.operationalArea.points.empty() &&
          (!OperationalEnvironmentEvaluator::Contains(aSnapshot.operationalArea, Point(*source)) ||
           !OperationalEnvironmentEvaluator::Contains(aSnapshot.operationalArea, Point(*destination))))
         return "OPERATIONAL_AREA_OUTSIDE";
      if (aRequest.directionalAntenna)
      {
         for (const PlatformAttitude& attitude : aSnapshot.platformAttitudes)
         {
            if (attitude.valid && attitude.platformName == source->platformName)
            {
               const double bearing = OperationalEnvironmentEvaluator::BearingDeg(
                  Point(*source), Point(*destination));
               if (!OperationalEnvironmentEvaluator::HeadingWithin(
                      attitude.headingDeg, bearing, aRequest.headingToleranceDeg))
                  return "ATTITUDE_MISMATCH";
               break;
            }
         }
      }
      return nullptr;
   }

   static bool RouteTerrainBlocked(const ResourceSnapshot& aSnapshot,
                                   const std::vector<std::string>& aRoute)
   {
      for (std::size_t i = 1; i < aRoute.size(); ++i)
      {
         for (const LinkSnapshot& link : aSnapshot.links)
         {
            if (link.sourceEndpointId == aRoute[i - 1] &&
                link.destinationEndpointId == aRoute[i] &&
                link.terrainBlockedFlag.valid &&
                link.terrainBlockedFlag.value >= 0.5) return true;
         }
      }
      return false;
   }

   static NetworkType RouteNetwork(const ResourceSnapshot& aSnapshot,
                                   const std::vector<std::string>& aRoute)
   {
      for (std::size_t i = 1; i < aRoute.size(); ++i)
      {
         for (const LinkSnapshot& link : aSnapshot.links)
            if (link.sourceEndpointId == aRoute[i - 1] &&
                link.destinationEndpointId == aRoute[i]) return link.networkType;
      }
      for (const std::string& endpointId : aRoute)
         for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
            if (endpoint.endpointId == endpointId) return endpoint.networkType;
      return NetworkType::cUNKNOWN;
   }

   static void Set(MetricValue<double>& aMetric, double aValue, const char* aUnit,
                   double aTime, DataOrigin aOrigin, Confidence aConfidence)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = std::isfinite(aValue);
      aMetric.origin = aOrigin;
      aMetric.confidence = aConfidence;
      aMetric.sampleTime = aTime;
      aMetric.reason = aMetric.valid ? MetricReason::cNONE
                                     : MetricReason::cINVALID_INPUT;
   }

   const EnvironmentConfigRepository& mConfig;
};
} // namespace nrm

#endif
