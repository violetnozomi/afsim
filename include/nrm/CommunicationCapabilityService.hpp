#ifndef NRM_COMMUNICATION_CAPABILITY_SERVICE_HPP
#define NRM_COMMUNICATION_CAPABILITY_SERVICE_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "nrm/AssessmentEvaluator.hpp"
#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/NetworkProfileRepository.hpp"

namespace nrm
{
class CommunicationCapabilityService
{
public:
   CommunicationCapabilityService()
      : CommunicationCapabilityService(NetworkProfileRepository::BuiltInDemo(), nullptr)
   {
   }

   explicit CommunicationCapabilityService(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr = nullptr)
      : mProfiles(aProfiles)
      , mEvaluator(aProfiles)
      , mEnvironmentAdapterPtr(aEnvironmentAdapterPtr)
   {
   }

   CapabilityResult Query(const ResourceSnapshot& aSnapshot,
                          const CapabilityRequest& aRequest,
                          const EnvironmentContext& aEnvironment = EnvironmentContext()) const
   {
      CapabilityResult result;
      InitializeResult(aSnapshot, aRequest, result);

      CapabilityReason validationReason = CapabilityReason::cNONE;
      if (!ValidateRequest(aSnapshot, aRequest, validationReason))
      {
         AddReason(result, validationReason);
         SetPathMetricReasons(result, MetricReason::cINVALID_INPUT);
         return result;
      }
      result.requestValid = true;

      AssessmentTask task;
      task.taskId = aRequest.requestId;
      task.sourcePlatform = aRequest.sourcePlatform;
      task.destinationPlatform = aRequest.destinationPlatform;
      task.payloadBits = aRequest.payloadBits;
      task.requiredBandwidthBps = aRequest.requiredBandwidthBps;
      task.maximumDelayMs = aRequest.maximumDelayMs;
      task.minimumPdrPercent = aRequest.minimumPdrPercent;
      task.kShortestPaths = aRequest.kShortestPaths;
      task.maximumHops = aRequest.maximumHops;
      task.allowedNetworks = aRequest.allowedNetworks;
      task.requireObservedCurrentMetrics = true;
      task.requireDelayMetricForFeasibility = aRequest.maximumDelayMs > 0.0;

      const AssessmentResult assessment = mEvaluator.Evaluate(aSnapshot, task);
      result.configVersion = assessment.configVersion;
      result.profileProviderId = assessment.profileProviderId;
      result.profileIds = assessment.profileIds;
      result.route = assessment.primaryRoute;
      result.endpointRoute = assessment.primaryEndpointRoute;
      result.networkSequence = assessment.networkSequence;
      result.pathAvailable = assessment.primaryEndpointRoute.size() >= 2;
      result.usesCandidate = assessment.primaryRouteUsesCandidate;
      MapAssessmentReasons(assessment, result);

      if (!result.pathAvailable)
      {
         PopulateEnvironment(aSnapshot, aRequest, result.endpointRoute, aEnvironment, result);
         AddReason(result, CapabilityReason::cNO_PATH);
         SetPathMetricReasons(
            result,
            HasReason(result, CapabilityReason::cNODE_OFFLINE)
               ? MetricReason::cNODE_OFFLINE
               : MetricReason::cNO_PATH);
         return result;
      }

      result.communicationDistanceM = assessment.pathDistanceM;
      result.maximumHopDistanceM = assessment.maximumHopDistanceM;
      result.transmissionDelayMs = assessment.predictedDelayMs;
      result.transmissionRateBps = assessment.bottleneckBandwidthBps;

      if (assessment.estimatedPdrPercent.valid &&
          std::isfinite(assessment.estimatedPdrPercent.value) &&
          assessment.estimatedPdrPercent.value >= 0.0 &&
          assessment.estimatedPdrPercent.value <= 100.0)
      {
         SetMetric(result.packetLossPercent,
                   100.0 - assessment.estimatedPdrPercent.value,
                   "percent", aSnapshot, assessment.estimatedPdrPercent.origin,
                   assessment.estimatedPdrPercent.confidence);
      }
      else
      {
         result.packetLossPercent.reason = MetricReason::cMISSING_TRANSMIT_DENOMINATOR;
      }

      PopulateThroughput(aSnapshot, result);
      PopulateAccessRatio(aSnapshot, result);
      if (result.usesCandidate)
      {
         AddReason(result, CapabilityReason::cPARAMETERIZED_CANDIDATE);
      }
      PopulateEnvironment(aSnapshot, aRequest, result.endpointRoute, aEnvironment, result);
      ApplyEnvironmentEffects(aSnapshot, result);
      if (!result.communicationDistanceM.valid || !result.maximumHopDistanceM.valid ||
          !result.transmissionRateBps.valid || !result.packetLossPercent.valid ||
          !result.transmissionDelayMs.valid)
      {
         AddReason(result, CapabilityReason::cPATH_METRIC_INVALID);
      }
      return result;
   }

private:
   static bool ValidateRequest(const ResourceSnapshot& aSnapshot,
                               const CapabilityRequest& aRequest,
                               CapabilityReason& aReason)
   {
      if (!std::isfinite(aSnapshot.simTime) ||
          !std::isfinite(aRequest.requiredBandwidthBps) ||
          !std::isfinite(aRequest.maximumDelayMs) ||
          !std::isfinite(aRequest.minimumPdrPercent))
      {
         aReason = CapabilityReason::cNON_FINITE_INPUT;
         return false;
      }
      if (aRequest.sourcePlatform.empty() || aRequest.destinationPlatform.empty() ||
          aRequest.sourcePlatform == aRequest.destinationPlatform ||
          aRequest.requiredBandwidthBps < 0.0 || aRequest.maximumDelayMs < 0.0 ||
          aRequest.minimumPdrPercent < 0.0 || aRequest.minimumPdrPercent > 100.0 ||
          aRequest.kShortestPaths < 1 || aRequest.kShortestPaths > 32 ||
          aRequest.maximumHops < 1 || aRequest.maximumHops > 64)
      {
         aReason = CapabilityReason::cINVALID_REQUEST;
         return false;
      }
      return true;
   }

   static void InitializeResult(const ResourceSnapshot& aSnapshot,
                                const CapabilityRequest& aRequest,
                                CapabilityResult& aResult)
   {
      aResult.requestId = aRequest.requestId;
      aResult.snapshotVersion = aSnapshot.snapshotVersion;
      aResult.simTime = std::isfinite(aSnapshot.simTime) ? aSnapshot.simTime : 0.0;
      aResult.configVersion = aSnapshot.configVersion;
      InitializeMetric(aResult.communicationDistanceM, "m", aSnapshot);
      InitializeMetric(aResult.maximumHopDistanceM, "m", aSnapshot);
      InitializeMetric(aResult.transmissionRateBps, "bit/s", aSnapshot);
      InitializeMetric(aResult.packetLossPercent, "percent", aSnapshot);
      InitializeMetric(aResult.transmissionDelayMs, "ms", aSnapshot);
      InitializeMetric(aResult.networkThroughputBps, "bit/s", aSnapshot);
      InitializeMetric(aResult.accessRatioPercent, "percent", aSnapshot);
   }

   static void InitializeMetric(MetricValue<double>& aMetric,
                                const char* aUnit,
                                const ResourceSnapshot& aSnapshot)
   {
      aMetric.unit = aUnit;
      aMetric.sampleTime = std::isfinite(aSnapshot.simTime) ? aSnapshot.simTime : 0.0;
      aMetric.reason = MetricReason::cNO_SAMPLES;
   }

   static void SetMetric(MetricValue<double>& aMetric,
                         double aValue,
                         const char* aUnit,
                         const ResourceSnapshot& aSnapshot,
                         DataOrigin aOrigin,
                         Confidence aConfidence,
                         double aWindow = 0.0)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = aOrigin;
      aMetric.confidence = aConfidence;
      aMetric.sampleTime = aSnapshot.simTime;
      aMetric.window = aWindow;
      aMetric.reason = MetricReason::cNONE;
   }

   static void SetPathMetricReasons(CapabilityResult& aResult, MetricReason aReason)
   {
      aResult.communicationDistanceM.reason = aReason;
      aResult.maximumHopDistanceM.reason = aReason;
      aResult.transmissionRateBps.reason = aReason;
      aResult.packetLossPercent.reason = aReason;
      aResult.transmissionDelayMs.reason = aReason;
      aResult.networkThroughputBps.reason = aReason;
      aResult.accessRatioPercent.reason = aReason;
   }

   static const WindowMetrics* Window10s(const std::vector<WindowMetrics>& aWindows)
   {
      for (const WindowMetrics& window : aWindows)
      {
         if (std::isfinite(window.windowS) && std::abs(window.windowS - 10.0) < 0.01)
         {
            return &window;
         }
      }
      return nullptr;
   }

   static const LinkSnapshot* FindCurrentLink(const ResourceSnapshot& aSnapshot,
                                              const std::string& aSourceId,
                                              const std::string& aDestinationId)
   {
      for (const LinkSnapshot& link : aSnapshot.links)
      {
         if (link.sourceEndpointId == aSourceId &&
             link.destinationEndpointId == aDestinationId &&
             link.state == ResourceState::cONLINE)
         {
            return &link;
         }
      }
      return nullptr;
   }

   static const EndpointSnapshot* FindEndpoint(const ResourceSnapshot& aSnapshot,
                                               const std::string& aEndpointId)
   {
      for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
      {
         if (endpoint.endpointId == aEndpointId)
         {
            return &endpoint;
         }
      }
      return nullptr;
   }

   static void PopulateThroughput(const ResourceSnapshot& aSnapshot,
                                  CapabilityResult& aResult)
   {
      double bottleneck = std::numeric_limits<double>::max();
      Confidence confidence = Confidence::cHIGH;
      bool valid = aResult.endpointRoute.size() >= 2;
      for (std::size_t index = 1; valid && index < aResult.endpointRoute.size(); ++index)
      {
         const LinkSnapshot* link = FindCurrentLink(aSnapshot,
                                                    aResult.endpointRoute[index - 1],
                                                    aResult.endpointRoute[index]);
         const WindowMetrics* window = link == nullptr ? nullptr : Window10s(link->windows);
         if (window == nullptr || !window->deliveredThroughputBps.valid ||
             !std::isfinite(window->deliveredThroughputBps.value) ||
             window->deliveredThroughputBps.value < 0.0)
         {
            valid = false;
            break;
         }
         bottleneck = std::min(bottleneck, window->deliveredThroughputBps.value);
         if (static_cast<int>(window->deliveredThroughputBps.confidence) <
             static_cast<int>(confidence))
         {
            confidence = window->deliveredThroughputBps.confidence;
         }
      }
      if (valid)
      {
         SetMetric(aResult.networkThroughputBps, bottleneck, "bit/s", aSnapshot,
                   DataOrigin::cDERIVED, confidence, 10.0);
      }
      else
      {
         aResult.networkThroughputBps.reason = MetricReason::cNO_SAMPLES;
         AddReason(aResult, CapabilityReason::cTHROUGHPUT_NOT_OBSERVED);
      }
   }

   static void PopulateAccessRatio(const ResourceSnapshot& aSnapshot,
                                   CapabilityResult& aResult)
   {
      std::set<std::string> networkNames;
      for (std::size_t index = 0; index + 1 < aResult.endpointRoute.size(); ++index)
      {
         const EndpointSnapshot* endpoint = FindEndpoint(aSnapshot, aResult.endpointRoute[index]);
         if (endpoint != nullptr && !endpoint->networkName.empty())
         {
            networkNames.insert(endpoint->networkName);
         }
      }

      std::size_t totalMembers = 0;
      std::size_t onlineMembers = 0;
      for (const std::string& networkName : networkNames)
      {
         for (const NetworkSnapshot& network : aSnapshot.networks)
         {
            if (network.networkName == networkName)
            {
               totalMembers += network.endpointCount;
               onlineMembers += std::min(network.onlineCount, network.endpointCount);
               break;
            }
         }
      }
      if (totalMembers == 0)
      {
         aResult.accessRatioPercent.reason = MetricReason::cZERO_DENOMINATOR;
         AddReason(aResult, CapabilityReason::cACCESS_DENOMINATOR_ZERO);
         return;
      }
      SetMetric(aResult.accessRatioPercent,
                100.0 * static_cast<double>(onlineMembers) /
                   static_cast<double>(totalMembers),
                "percent", aSnapshot, DataOrigin::cDERIVED, Confidence::cHIGH);
   }

   static bool NormalizeEnvironmentMetric(MetricValue<double>& aMetric,
                                          const char* aUnit,
                                          double aSampleTime)
   {
      if (aMetric.unit.empty())
      {
         aMetric.unit = aUnit;
      }
      if (!aMetric.valid)
      {
         aMetric.sampleTime = aSampleTime;
         aMetric.reason = MetricReason::cENVIRONMENT_DATA_UNAVAILABLE;
         return true;
      }
      if (!std::isfinite(aMetric.value) || !std::isfinite(aMetric.sampleTime))
      {
         aMetric.valid = false;
         aMetric.sampleTime = aSampleTime;
         aMetric.reason = MetricReason::cINVALID_INPUT;
         return false;
      }
      aMetric.reason = MetricReason::cNONE;
      return true;
   }

   static bool NormalizeEnvironmentEffect(EnvironmentEffect& aEffect)
   {
      bool valid = true;
      valid = NormalizeEnvironmentMetric(
                 aEffect.pathLossDeltaDb, "dB", aEffect.sampleTime) && valid;
      valid = NormalizeEnvironmentMetric(
                 aEffect.capacityScale, "ratio", aEffect.sampleTime) && valid;
      valid = NormalizeEnvironmentMetric(
                 aEffect.packetLossDeltaPercent, "percentage_point", aEffect.sampleTime) && valid;
      valid = NormalizeEnvironmentMetric(
                 aEffect.delayDeltaMs, "ms", aEffect.sampleTime) && valid;
      if (!valid)
      {
         aEffect.valid = false;
         aEffect.reason = CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE;
      }
      else if (aEffect.valid)
      {
         aEffect.reason = CapabilityReason::cNONE;
      }
      return valid;
   }

   void PopulateEnvironment(const ResourceSnapshot& aSnapshot,
                            const CapabilityRequest& aRequest,
                            const std::vector<std::string>& aEndpointRoute,
                            const EnvironmentContext& aContext,
                            CapabilityResult& aResult) const
   {
      const EnvironmentDomain domains[] = {
         EnvironmentDomain::cTERRAIN,
         EnvironmentDomain::cWEATHER,
         EnvironmentDomain::cCELESTIAL,
         EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE};
      EnvironmentContext effectiveContext = aContext;
      const bool customerModeSpecified =
         aContext.schemaVersion == "nrm.customer.environment_report.v1";
      const bool candidateAdjustmentAllowed =
         !customerModeSpecified ||
         aContext.applicationMode ==
            EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
      effectiveContext.applyParameterizedEffects =
         aResult.usesCandidate && candidateAdjustmentAllowed;
      if (effectiveContext.applyParameterizedEffects && !customerModeSpecified)
         effectiveContext.applicationMode =
            EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
      const bool contextUsable = effectiveContext.valid &&
                                 std::isfinite(effectiveContext.sampleTime) &&
                                 mEnvironmentAdapterPtr != nullptr;
      for (EnvironmentDomain domain : domains)
      {
         EnvironmentEffect effect;
         effect.domain = domain;
         effect.sampleTime = std::isfinite(aSnapshot.simTime) ? aSnapshot.simTime : 0.0;
         if (contextUsable)
         {
            effect = mEnvironmentAdapterPtr->Evaluate(
               domain, aSnapshot, aRequest, aEndpointRoute, effectiveContext);
            effect.domain = domain;
            if (!std::isfinite(effect.sampleTime))
            {
               effect.valid = false;
               effect.sampleTime = 0.0;
               effect.reason = CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE;
            }
            if (!effect.valid && effect.reason == CapabilityReason::cNONE)
            {
               effect.reason = CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE;
            }
         }
         NormalizeEnvironmentEffect(effect);
         aResult.environmentEffects.push_back(effect);
         if (!effect.valid)
         {
            AddReason(aResult, CapabilityReason::cENVIRONMENT_DATA_UNAVAILABLE);
         }
      }
   }

   static void ApplyEnvironmentEffects(const ResourceSnapshot& aSnapshot,
                                       CapabilityResult& aResult)
   {
      bool hardBlocked = false;
      double capacityScale = 1.0;
      double packetLossDelta = 0.0;
      double delayDelta = 0.0;
      Confidence confidence = Confidence::cHIGH;
      bool hasApplicableEffect = false;
      DataOrigin appliedOrigin = DataOrigin::cPARAMETERIZED_MODEL;
      for (const EnvironmentEffect& effect : aResult.environmentEffects)
      {
         if (!effect.valid) continue;
         hardBlocked = hardBlocked || effect.hardBlocked;
         hasApplicableEffect = hasApplicableEffect ||
                               effect.origin != DataOrigin::cAFSIM_INTERNAL;
         if (effect.origin == DataOrigin::cCUSTOMER_MODULE)
            appliedOrigin = DataOrigin::cCUSTOMER_MODULE;
         if (effect.capacityScale.valid)
            capacityScale *= std::max(0.0, std::min(1.0, effect.capacityScale.value));
         if (effect.packetLossDeltaPercent.valid)
            packetLossDelta += std::max(0.0, effect.packetLossDeltaPercent.value);
         if (effect.delayDeltaMs.valid)
            delayDelta += std::max(0.0, effect.delayDeltaMs.value);
         if (static_cast<int>(effect.confidence) < static_cast<int>(confidence))
            confidence = effect.confidence;
      }
      if (hardBlocked)
      {
         aResult.pathAvailable = false;
         AddReason(aResult, CapabilityReason::cENVIRONMENT_HARD_BLOCKED);
         SetPathMetricReasons(aResult, MetricReason::cNO_PATH);
         return;
      }
      if (!aResult.usesCandidate || !hasApplicableEffect) return;
      if (aResult.transmissionRateBps.valid)
      {
         aResult.transmissionRateBps.value *= capacityScale;
         aResult.transmissionRateBps.origin = appliedOrigin;
         aResult.transmissionRateBps.confidence = confidence;
      }
      if (aResult.packetLossPercent.valid)
      {
         aResult.packetLossPercent.value = std::min(
            100.0, aResult.packetLossPercent.value + packetLossDelta);
         aResult.packetLossPercent.origin = appliedOrigin;
         aResult.packetLossPercent.confidence = confidence;
      }
      if (aResult.transmissionDelayMs.valid)
      {
         aResult.transmissionDelayMs.value += delayDelta;
         aResult.transmissionDelayMs.origin = appliedOrigin;
         aResult.transmissionDelayMs.confidence = confidence;
      }
      (void)aSnapshot;
   }

   static void MapAssessmentReasons(const AssessmentResult& aAssessment,
                                    CapabilityResult& aResult)
   {
      for (AssessmentReason reason : aAssessment.reasons)
      {
         switch (reason)
         {
         case AssessmentReason::cNODE_NOT_FOUND:
            AddReason(aResult, CapabilityReason::cNODE_NOT_FOUND);
            break;
         case AssessmentReason::cNODE_OFFLINE:
            AddReason(aResult, CapabilityReason::cNODE_OFFLINE);
            break;
         case AssessmentReason::cPROFILE_CONFIG_INVALID:
            AddReason(aResult, CapabilityReason::cPROFILE_CONFIG_INVALID);
            break;
         case AssessmentReason::cPATH_SEARCH_LIMIT_REACHED:
            AddReason(aResult, CapabilityReason::cPATH_SEARCH_LIMIT_REACHED);
            break;
         case AssessmentReason::cDATA_INVALID:
         case AssessmentReason::cNETWORK_TYPE_UNKNOWN:
            AddReason(aResult, CapabilityReason::cPATH_METRIC_INVALID);
            break;
         default:
            break;
         }
      }
   }

   static void AddReason(CapabilityResult& aResult, CapabilityReason aReason)
   {
      if (std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) ==
          aResult.reasons.end())
      {
         aResult.reasons.push_back(aReason);
      }
   }

   static bool HasReason(const CapabilityResult& aResult, CapabilityReason aReason)
   {
      return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) !=
             aResult.reasons.end();
   }

   NetworkProfileRepository mProfiles;
   AssessmentEvaluator mEvaluator;
   const EnvironmentEffectAdapter* mEnvironmentAdapterPtr = nullptr;
};
} // namespace nrm

#endif
