/**
 * @file AssessmentEvaluator.hpp
 * @brief Deterministic bounded constrained-path assessment over current and candidate graphs.
 */

#ifndef NRM_ASSESSMENT_EVALUATOR_HPP
#define NRM_ASSESSMENT_EVALUATOR_HPP

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "nrm/AssessmentTypes.hpp"
#include "nrm/ConstrainedPathSelector.hpp"
#include "nrm/NetworkProfileRepository.hpp"

namespace nrm
{
class AssessmentEvaluator
{
public:
   AssessmentEvaluator()
      : mProfiles(NetworkProfileRepository::BuiltInDemo())
   {
   }

   explicit AssessmentEvaluator(const NetworkProfileRepository& aProfiles)
      : mProfiles(aProfiles)
   {
   }

   AssessmentResult Evaluate(const ResourceSnapshot& aSnapshot, const AssessmentTask& aTask) const
   {
      AssessmentResult result;
      result.taskId = aTask.taskId;
      result.snapshotVersion = aSnapshot.snapshotVersion;
      result.simTime = aSnapshot.simTime;
      result.configVersion = mProfiles.ConfigVersion();
      result.profileProviderId = mProfiles.ProviderId();

      EndpointMap endpoints;
      std::vector<std::string> sourceIds;
      std::set<std::string> destinationIds;
      bool sourceKnown = false;
      bool destinationKnown = false;
      bool sourceOnline = false;
      bool destinationOnline = false;
      for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
      {
         endpoints[endpoint.endpointId] = &endpoint;
         CollectTaskEndpoint(endpoint, aTask.sourcePlatform, sourceKnown, sourceOnline,
                             &sourceIds, nullptr);
         CollectTaskEndpoint(endpoint, aTask.destinationPlatform, destinationKnown,
                             destinationOnline, nullptr, &destinationIds);
      }

      if (!sourceKnown || !destinationKnown)
      {
         AddReason(result, AssessmentReason::cNODE_NOT_FOUND);
         result.recommendations.emplace_back("检查任务源、目的平台是否仍在当前成员列表中。");
         return result;
      }
      if (!sourceOnline || !destinationOnline)
      {
         AddReason(result, AssessmentReason::cNODE_OFFLINE);
         result.recommendations.emplace_back("恢复离线端点后重新评估。");
         return result;
      }

      PathSelectionOptions options;
      options.k = aTask.kShortestPaths;
      options.maximumHops = aTask.maximumHops;
      PathConstraintSet constraints;
      constraints.requiredBandwidthBps = aTask.requiredBandwidthBps;
      constraints.maximumDelayMs = aTask.maximumDelayMs;
      constraints.minimumPdrPercent = aTask.minimumPdrPercent;
      constraints.requireDelayMetric = aTask.requireDelayMetricForFeasibility;
      if (!options.Valid() || !constraints.Valid())
      {
         AddReason(result, AssessmentReason::cDATA_INVALID);
         result.failedConstraints.push_back(options.Valid() ? "PATH_CONSTRAINTS_INVALID"
                                                            : "PATH_OPTIONS_INVALID");
         return result;
      }

      ConstrainedPathSelector::Adjacency adjacency;
      std::set<std::string> currentEdgeKeys;
      BuildCurrentEdges(aSnapshot, aTask, endpoints, adjacency, currentEdgeKeys, result);
      bool profileMissing = false;
      BuildCandidateEdges(aSnapshot, aTask, endpoints, currentEdgeKeys, adjacency, profileMissing);
      ConstrainedPathSelector selector;
      const PathSelectionResult current =
         selector.Select(adjacency, sourceIds, destinationIds, constraints, options, false, false);
      result.reachable = current.hasDiagnosticPath;
      result.consideredPathCount = current.consideredPaths.size();
      if (current.searchLimitReached)
      {
         AddReason(result, AssessmentReason::cPATH_SEARCH_LIMIT_REACHED);
      }

      PathSelectionResult candidate;
      bool candidateSearched = false;
      const ConstrainedPath* primary = nullptr;
      if (current.hasSelectedPath)
      {
         primary = &current.selectedPath;
         result.selectedPathRank = current.selectedPathRank;
      }
      else
      {
         candidateSearched = true;
         candidate = selector.Select(adjacency, sourceIds, destinationIds, constraints,
                                     options, true, true);
         result.consideredPathCount += candidate.consideredPaths.size();
         if (candidate.searchLimitReached)
         {
            AddReason(result, AssessmentReason::cPATH_SEARCH_LIMIT_REACHED);
         }
         if (candidate.hasSelectedPath)
         {
            primary = &candidate.selectedPath;
            result.selectedPathRank = candidate.selectedPathRank;
         }
         else if (current.hasDiagnosticPath)
         {
            primary = &current.diagnosticPath;
         }
         else if (candidate.hasDiagnosticPath)
         {
            primary = &candidate.diagnosticPath;
         }
      }

      if (!result.reachable)
      {
         AddReason(result, AssessmentReason::cNO_CURRENT_PATH);
      }
      if (primary == nullptr)
      {
         if (profileMissing || !mProfiles.Valid())
         {
            AddReason(result, AssessmentReason::cPROFILE_CONFIG_INVALID);
         }
         AddReason(result, AssessmentReason::cLINK_NOT_ESTABLISHABLE);
         result.recommendations.emplace_back(
            "当前图和已验证的候选网络剖面中均没有有界可诊断路径。");
         return result;
      }

      result.canEstablish = true;
      result.canComplete = primary->feasible;
      result.primaryRouteUsesCandidate = primary->candidateEdgeCount > 0;
      PopulateRoute(*primary, aTask.sourcePlatform, result.primaryRoute,
                    result.primaryEndpointRoute, result.networkSequence);
      PopulateMetrics(aSnapshot, *primary, result);
      PopulateMargins(aSnapshot, aTask, result);
      ApplyFailures(*primary, result);
      PopulateProfiles(*primary, result);

      std::set<std::string> forbiddenEdges;
      for (const ConstrainedEdge* edge : primary->edges)
      {
         forbiddenEdges.insert(ConstrainedPathSelector::EdgeKey(*edge));
      }
      PathSelectionResult backup =
         selector.Select(adjacency, sourceIds, destinationIds, constraints, options,
                         false, false, forbiddenEdges);
      if (!backup.hasSelectedPath)
      {
         backup = selector.Select(adjacency, sourceIds, destinationIds, constraints, options,
                                  true, true, forbiddenEdges);
      }
      const ConstrainedPath* backupPath = backup.hasSelectedPath ? &backup.selectedPath : nullptr;
      if (backupPath != nullptr)
      {
         std::vector<NetworkType> ignored;
         std::vector<std::string> ignoredEndpoints;
         PopulateRoute(*backupPath, aTask.sourcePlatform, result.backupRoute,
                       ignoredEndpoints, ignored);
         result.backupRouteUsesCandidate = backupPath->candidateEdgeCount > 0;
      }

      result.stable = result.canComplete && result.estimatedPdrPercent.valid &&
                      result.estimatedPdrPercent.value >= 80.0 &&
                      !result.primaryRouteUsesCandidate;
      if (result.primaryRouteUsesCandidate)
      {
         result.recommendations.emplace_back(
            result.reachable
               ? "当前路径均不满足硬约束，参数化候选路径满足约束；建议人工核验后建链。"
               : "当前尚不可达，参数化候选路径满足约束；建议人工核验后建链。");
      }
      else
      {
         result.recommendations.emplace_back(
            result.canComplete ? "当前图路径满足已输入的硬约束。"
                               : "当前图存在路径，但有界候选集合内没有满足全部硬约束的路径。");
      }
      result.recommendations.emplace_back(
         result.backupRoute.empty()
            ? "当前没有找到满足约束且与主路由有向边不重合的备选路径。"
            : "已生成一条满足约束的有向边不重合备选路由。");
      (void)candidateSearched;
      return result;
   }

private:
   using EndpointMap = std::map<std::string, const EndpointSnapshot*>;

   static void CollectTaskEndpoint(const EndpointSnapshot& aEndpoint,
                                   const std::string& aPlatformName,
                                   bool& aKnown,
                                   bool& aOnline,
                                   std::vector<std::string>* aIdsPtr,
                                   std::set<std::string>* aIdSetPtr)
   {
      if (aEndpoint.platformName != aPlatformName)
      {
         return;
      }
      aKnown = true;
      if (aEndpoint.state != ResourceState::cONLINE)
      {
         return;
      }
      aOnline = true;
      if (aIdsPtr != nullptr)
      {
         aIdsPtr->push_back(aEndpoint.endpointId);
      }
      if (aIdSetPtr != nullptr)
      {
         aIdSetPtr->insert(aEndpoint.endpointId);
      }
   }

   static bool Allowed(const AssessmentTask& aTask, NetworkType aType)
   {
      return aTask.allowedNetworks.empty() ||
             std::find(aTask.allowedNetworks.begin(), aTask.allowedNetworks.end(), aType) !=
                aTask.allowedNetworks.end();
   }

   void BuildCurrentEdges(const ResourceSnapshot& aSnapshot,
                                 const AssessmentTask& aTask,
                                 const EndpointMap& aEndpoints,
                                 ConstrainedPathSelector::Adjacency& aAdjacency,
                                 std::set<std::string>& aCurrentEdgeKeys,
                                 AssessmentResult& aResult) const
   {
      for (const LinkSnapshot& link : aSnapshot.links)
      {
         if (link.state != ResourceState::cONLINE || !Allowed(aTask, link.networkType))
         {
            continue;
         }
         const auto source = aEndpoints.find(link.sourceEndpointId);
         const auto destination = aEndpoints.find(link.destinationEndpointId);
         if (source == aEndpoints.end() || destination == aEndpoints.end() ||
             source->second->state != ResourceState::cONLINE ||
             destination->second->state != ResourceState::cONLINE)
         {
            continue;
         }
         if (link.networkType == NetworkType::cUNKNOWN)
         {
            AddReason(aResult, AssessmentReason::cNETWORK_TYPE_UNKNOWN);
            continue;
         }

         ConstrainedEdge edge;
         edge.sourceId = link.sourceEndpointId;
         edge.destinationId = link.destinationEndpointId;
         edge.sourcePlatform = link.sourcePlatform;
         edge.destinationPlatform = link.destinationPlatform;
         edge.networkType = link.networkType;
         edge.candidate = false;
         double distanceM = -1.0;
         if (link.distanceM.valid && std::isfinite(link.distanceM.value) &&
             link.distanceM.value >= 0.0)
         {
            distanceM = link.distanceM.value;
         }
         else
         {
            distanceM = EndpointDistanceM(*source->second, *destination->second);
         }
         if (distanceM >= 0.0)
         {
            edge.distanceM = distanceM;
            edge.distanceValid = true;
            edge.distanceConfidence =
               link.distanceM.valid ? link.distanceM.confidence
                                    : EndpointDistanceConfidence(*source->second, *destination->second);
         }
         const double delayMs = LinkDelayMs(link, !aTask.requireObservedCurrentMetrics);
         if (delayMs >= 0.0)
         {
            edge.delayMs = delayMs;
            edge.delayValid = true;
            const WindowMetrics* delayWindow = Window10s(link.windows);
            edge.delayConfidence =
               delayWindow != nullptr && delayWindow->averageTransportDelayMs.valid
                  ? delayWindow->averageTransportDelayMs.confidence
                  : edge.distanceConfidence;
         }
         const WindowMetrics* window = Window10s(link.windows);
         if (window != nullptr)
         {
            const MetricValue<double>& ratio =
               window->deliveryRatioPercent.valid ? window->deliveryRatioPercent : window->pdrPercent;
            if (ratio.valid && std::isfinite(ratio.value) && ratio.value >= 0.0 &&
                ratio.value <= 100.0)
            {
               edge.pdrPercent = ratio.value;
               edge.pdrValid = true;
               edge.pdrConfidence = ratio.confidence;
            }
         }
         if (link.bandwidthBps.valid && std::isfinite(link.bandwidthBps.value) &&
             link.bandwidthBps.value >= 0.0)
         {
            edge.bandwidthBps =
               AdmissibleCapacityBps(aSnapshot, link.networkName, link.bandwidthBps.value);
            edge.bandwidthValid = true;
            edge.bandwidthConfidence = link.bandwidthBps.confidence;
         }
         else if (!aTask.requireObservedCurrentMetrics)
         {
            const NetworkProfile* profile = mProfiles.Find(link.networkType);
            if (profile != nullptr)
            {
               edge.bandwidthBps =
                  AdmissibleCapacityBps(aSnapshot, link.networkName,
                                        profile->serviceCapacityBps);
               edge.bandwidthValid = true;
               edge.bandwidthConfidence = Confidence::cLOW;
               edge.profileId = profile->profileId;
            }
         }
         aAdjacency[edge.sourceId].push_back(edge);
         aCurrentEdgeKeys.insert(ConstrainedPathSelector::EdgeKey(edge));
      }
   }

   void BuildCandidateEdges(const ResourceSnapshot& aSnapshot,
                            const AssessmentTask& aTask,
                            const EndpointMap& aEndpoints,
                            const std::set<std::string>& aCurrentEdgeKeys,
                            ConstrainedPathSelector::Adjacency& aAdjacency,
                            bool& aProfileMissing) const
   {
      for (const auto& sourceEntry : aEndpoints)
      {
         const EndpointSnapshot& source = *sourceEntry.second;
         if (source.state != ResourceState::cONLINE || !Allowed(aTask, source.networkType) ||
             source.networkType == NetworkType::cUNKNOWN || source.networkName.empty())
         {
            continue;
         }
         const NetworkProfile* profile = mProfiles.Find(source.networkType);
         if (profile == nullptr)
         {
            aProfileMissing = true;
            continue;
         }
         for (const auto& destinationEntry : aEndpoints)
         {
            const EndpointSnapshot& destination = *destinationEntry.second;
            if (source.endpointId == destination.endpointId ||
                source.platformName == destination.platformName ||
                destination.state != ResourceState::cONLINE ||
                source.networkType != destination.networkType ||
                source.networkName != destination.networkName)
            {
               continue;
            }
            ConstrainedEdge edge;
            edge.sourceId = source.endpointId;
            edge.destinationId = destination.endpointId;
            if (aCurrentEdgeKeys.count(ConstrainedPathSelector::EdgeKey(edge)) != 0)
            {
               continue;
            }
            const double distanceM = EndpointDistanceM(source, destination);
            if (distanceM < 0.0 || distanceM > profile->maximumRangeM)
            {
               continue;
            }
            edge.sourcePlatform = source.platformName;
            edge.destinationPlatform = destination.platformName;
            edge.networkType = source.networkType;
            edge.delayMs = profile->establishmentDelayMs +
                           1000.0 * distanceM / 299792458.0;
            edge.distanceM = distanceM;
            edge.pdrPercent = profile->candidatePdrPercent;
            edge.bandwidthBps =
               AdmissibleCapacityBps(aSnapshot, source.networkName,
                                     profile->serviceCapacityBps);
            edge.delayValid = true;
            edge.pdrValid = true;
            edge.bandwidthValid = true;
            edge.distanceValid = true;
            edge.delayConfidence = Confidence::cLOW;
            edge.pdrConfidence = Confidence::cLOW;
            edge.bandwidthConfidence = Confidence::cLOW;
            edge.distanceConfidence = EndpointDistanceConfidence(source, destination);
            edge.candidate = true;
            edge.profileId = profile->profileId;
            aAdjacency[edge.sourceId].push_back(edge);
         }
      }
   }

   static double AdmissibleCapacityBps(const ResourceSnapshot& aSnapshot,
                                       const std::string& aNetworkName,
                                       double aServiceCapacityBps)
   {
      for (const NetworkSnapshot& network : aSnapshot.networks)
      {
         if (network.networkName != aNetworkName)
         {
            continue;
         }
         const WindowMetrics* window = Window10s(network.windows);
         if (window != nullptr && window->offeredLoadBps.valid &&
             std::isfinite(window->offeredLoadBps.value) &&
             window->offeredLoadBps.value >= 0.0)
         {
            return std::max(0.0, aServiceCapacityBps - window->offeredLoadBps.value);
         }
         break;
      }
      return aServiceCapacityBps;
   }

   static const WindowMetrics* Window10s(const std::vector<WindowMetrics>& aWindows)
   {
      for (const WindowMetrics& window : aWindows)
      {
         if (std::abs(window.windowS - 10.0) < 0.01)
         {
            return &window;
         }
      }
      return nullptr;
   }

   static double LinkDelayMs(const LinkSnapshot& aLink, bool aAllowDistanceFallback)
   {
      const WindowMetrics* window = Window10s(aLink.windows);
      if (window != nullptr && window->averageTransportDelayMs.valid &&
          std::isfinite(window->averageTransportDelayMs.value) &&
          window->averageTransportDelayMs.value >= 0.0)
      {
         return window->averageTransportDelayMs.value;
      }
      return aAllowDistanceFallback && aLink.distanceM.valid &&
                    std::isfinite(aLink.distanceM.value) &&
                    aLink.distanceM.value >= 0.0
                ? 1000.0 * aLink.distanceM.value / 299792458.0
                : -1.0;
   }

   static double EndpointDistanceM(const EndpointSnapshot& aSource,
                                   const EndpointSnapshot& aDestination)
   {
      if (!aSource.latitudeDeg.valid || !aSource.longitudeDeg.valid ||
          !aDestination.latitudeDeg.valid || !aDestination.longitudeDeg.valid)
      {
         return -1.0;
      }
      if (!std::isfinite(aSource.latitudeDeg.value) ||
          !std::isfinite(aSource.longitudeDeg.value) ||
          !std::isfinite(aDestination.latitudeDeg.value) ||
          !std::isfinite(aDestination.longitudeDeg.value) ||
          (aSource.altitudeM.valid && !std::isfinite(aSource.altitudeM.value)) ||
          (aDestination.altitudeM.valid && !std::isfinite(aDestination.altitudeM.value)))
      {
         return -1.0;
      }
      constexpr double cPI = 3.14159265358979323846;
      constexpr double cEARTH_RADIUS_M = 6371000.0;
      const double lat1 = aSource.latitudeDeg.value * cPI / 180.0;
      const double lat2 = aDestination.latitudeDeg.value * cPI / 180.0;
      const double deltaLat = lat2 - lat1;
      const double deltaLon =
         (aDestination.longitudeDeg.value - aSource.longitudeDeg.value) * cPI / 180.0;
      const double haversine = std::sin(deltaLat / 2.0) * std::sin(deltaLat / 2.0) +
                               std::cos(lat1) * std::cos(lat2) *
                                  std::sin(deltaLon / 2.0) * std::sin(deltaLon / 2.0);
      const double groundDistance =
         2.0 * cEARTH_RADIUS_M * std::asin(std::sqrt(std::min(1.0, haversine)));
      const double altitudeDelta =
         aSource.altitudeM.valid && aDestination.altitudeM.valid
            ? aDestination.altitudeM.value - aSource.altitudeM.value
            : 0.0;
      return std::sqrt(groundDistance * groundDistance + altitudeDelta * altitudeDelta);
   }

   static Confidence MinConfidence(Confidence aLeft, Confidence aRight)
   {
      return static_cast<int>(aLeft) < static_cast<int>(aRight) ? aLeft : aRight;
   }

   static Confidence EndpointDistanceConfidence(const EndpointSnapshot& aSource,
                                                  const EndpointSnapshot& aDestination)
   {
      Confidence confidence = Confidence::cHIGH;
      confidence = MinConfidence(confidence, aSource.latitudeDeg.confidence);
      confidence = MinConfidence(confidence, aSource.longitudeDeg.confidence);
      confidence = MinConfidence(confidence, aDestination.latitudeDeg.confidence);
      confidence = MinConfidence(confidence, aDestination.longitudeDeg.confidence);
      if (aSource.altitudeM.valid && aDestination.altitudeM.valid)
      {
         confidence = MinConfidence(confidence, aSource.altitudeM.confidence);
         confidence = MinConfidence(confidence, aDestination.altitudeM.confidence);
      }
      return confidence;
   }

   static void PopulateRoute(const ConstrainedPath& aPath,
                             const std::string& aSourcePlatform,
                             std::vector<std::string>& aRoute,
                             std::vector<std::string>& aEndpointRoute,
                             std::vector<NetworkType>& aNetworkSequence)
   {
      aRoute.clear();
      aEndpointRoute = aPath.nodeIds;
      aNetworkSequence.clear();
      aRoute.push_back(aSourcePlatform);
      for (const ConstrainedEdge* edge : aPath.edges)
      {
         aRoute.push_back(edge->destinationPlatform);
         if (aNetworkSequence.empty() || aNetworkSequence.back() != edge->networkType)
         {
            aNetworkSequence.push_back(edge->networkType);
         }
      }
   }

   static void PopulateProfiles(const ConstrainedPath& aPath, AssessmentResult& aResult)
   {
      std::set<std::string> ids;
      for (const ConstrainedEdge* edge : aPath.edges)
      {
         if (!edge->profileId.empty())
         {
            ids.insert(edge->profileId);
         }
      }
      aResult.profileIds.assign(ids.begin(), ids.end());
   }

   static void PopulateMetrics(const ResourceSnapshot& aSnapshot,
                               const ConstrainedPath& aPath,
                               AssessmentResult& aResult)
   {
      if (aPath.delayValid)
      {
         SetMetric(aResult.predictedDelayMs, aPath.delayMs, "ms", aSnapshot,
                   aPath.candidateEdgeCount > 0 ? DataOrigin::cPARAMETERIZED_MODEL
                                                : DataOrigin::cDERIVED,
                   aPath.candidateEdgeCount > 0 ? Confidence::cLOW
                                                : aPath.delayConfidence);
      }
      else
      {
         aResult.predictedDelayMs.reason = MetricReason::cMISSING_LIFECYCLE_CORRELATION;
      }
      if (aPath.distanceValid)
      {
         SetMetric(aResult.pathDistanceM, aPath.totalDistanceM, "m", aSnapshot,
                   DataOrigin::cDERIVED, aPath.distanceConfidence);
         SetMetric(aResult.maximumHopDistanceM, aPath.maximumHopDistanceM, "m",
                   aSnapshot, DataOrigin::cDERIVED, aPath.distanceConfidence);
      }
      if (aPath.pdrValid)
      {
         SetMetric(aResult.estimatedPdrPercent, aPath.pdrPercent, "percent", aSnapshot,
                   aPath.candidateEdgeCount > 0 ? DataOrigin::cPARAMETERIZED_MODEL
                                                : DataOrigin::cESTIMATED,
                   Confidence::cLOW);
      }
      else
      {
         aResult.estimatedPdrPercent.reason = MetricReason::cMISSING_TRANSMIT_DENOMINATOR;
      }
      if (aPath.bandwidthValid)
      {
         const bool parameterizedCapacity =
            std::any_of(aPath.edges.begin(), aPath.edges.end(),
                        [](const ConstrainedEdge* aEdge)
                        {
                           return !aEdge->profileId.empty();
                        });
         SetMetric(aResult.bottleneckBandwidthBps, aPath.bottleneckBandwidthBps,
                   "bit/s", aSnapshot,
                   parameterizedCapacity ? DataOrigin::cPARAMETERIZED_MODEL
                                         : DataOrigin::cDERIVED,
                   parameterizedCapacity ? Confidence::cLOW
                                         : aPath.bandwidthConfidence);
      }

   }

   static void PopulateMargins(const ResourceSnapshot& aSnapshot,
                               const AssessmentTask& aTask,
                               AssessmentResult& aResult)
   {
      if (aTask.maximumDelayMs > 0.0 && aResult.predictedDelayMs.valid)
      {
         SetMetric(aResult.delayMarginMs,
                   aTask.maximumDelayMs - aResult.predictedDelayMs.value,
                   "ms", aSnapshot, aResult.predictedDelayMs.origin,
                   aResult.predictedDelayMs.confidence);
      }
      if (aTask.minimumPdrPercent > 0.0 && aResult.estimatedPdrPercent.valid)
      {
         SetMetric(aResult.reliabilityMarginPercent,
                   aResult.estimatedPdrPercent.value - aTask.minimumPdrPercent,
                   "percentage_point", aSnapshot, aResult.estimatedPdrPercent.origin,
                   aResult.estimatedPdrPercent.confidence);
      }
      if (aTask.requiredBandwidthBps > 0.0 && aResult.bottleneckBandwidthBps.valid)
      {
         SetMetric(aResult.bandwidthMarginBps,
                   aResult.bottleneckBandwidthBps.value - aTask.requiredBandwidthBps,
                   "bit/s", aSnapshot, aResult.bottleneckBandwidthBps.origin,
                   aResult.bottleneckBandwidthBps.confidence);
      }
   }

   static void SetMetric(MetricValue<double>& aMetric,
                         double aValue,
                         const char* aUnit,
                         const ResourceSnapshot& aSnapshot,
                         DataOrigin aOrigin,
                         Confidence aConfidence)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = aOrigin;
      aMetric.confidence = aConfidence;
      aMetric.sampleTime = aSnapshot.simTime;
      aMetric.reason = MetricReason::cNONE;
   }

   static void ApplyFailures(const ConstrainedPath& aPath, AssessmentResult& aResult)
   {
      aResult.failedConstraints = aPath.failedConstraints;
      for (const std::string& failure : aPath.failedConstraints)
      {
         if (failure == "BANDWIDTH_MARGIN_NEGATIVE")
         {
            AddReason(aResult, AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE);
         }
         else if (failure == "DELAY_MARGIN_NEGATIVE")
         {
            AddReason(aResult, AssessmentReason::cDELAY_MARGIN_NEGATIVE);
         }
         else if (failure == "RELIABILITY_MARGIN_NEGATIVE")
         {
            AddReason(aResult, AssessmentReason::cRELIABILITY_MARGIN_NEGATIVE);
         }
         else
         {
            AddReason(aResult, AssessmentReason::cDATA_INVALID);
         }
      }
   }

   static void AddReason(AssessmentResult& aResult, AssessmentReason aReason)
   {
      if (std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) ==
          aResult.reasons.end())
      {
         aResult.reasons.push_back(aReason);
      }
   }

   NetworkProfileRepository mProfiles;
};
} // namespace nrm

#endif
