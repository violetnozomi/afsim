/**
 * @file AssessmentEvaluator.hpp
 * @brief Deterministic current/candidate graph assessment with primary and backup routes.
 */

#ifndef NRM_ASSESSMENT_EVALUATOR_HPP
#define NRM_ASSESSMENT_EVALUATOR_HPP

#include <algorithm>
#include <cmath>
#include <deque>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "nrm/AssessmentTypes.hpp"

namespace nrm
{
class AssessmentEvaluator
{
public:
   AssessmentResult Evaluate(const ResourceSnapshot& aSnapshot, const AssessmentTask& aTask) const
   {
      AssessmentResult result;
      result.taskId          = aTask.taskId;
      result.snapshotVersion = aSnapshot.snapshotVersion;
      result.simTime         = aSnapshot.simTime;

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
         CollectTaskEndpoint(endpoint,
                             aTask.sourcePlatform,
                             sourceKnown,
                             sourceOnline,
                             &sourceIds,
                             nullptr);
         CollectTaskEndpoint(endpoint,
                             aTask.destinationPlatform,
                             destinationKnown,
                             destinationOnline,
                             nullptr,
                             &destinationIds);
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

      Adjacency adjacency;
      std::set<std::string> currentEdgeKeys;
      BuildCurrentEdges(aSnapshot, aTask, endpoints, adjacency, currentEdgeKeys, result);

      const Path currentPath = FindPath(adjacency, sourceIds, destinationIds, false, {});
      result.reachable = currentPath.valid;

      BuildCandidateEdges(aTask, endpoints, currentEdgeKeys, adjacency);
      Path primaryPath = currentPath;
      if (!currentPath.valid)
      {
         AddReason(result, AssessmentReason::cNO_CURRENT_PATH);
         primaryPath = FindPath(adjacency, sourceIds, destinationIds, true, {});
         if (!primaryPath.valid)
         {
            AddReason(result, AssessmentReason::cLINK_NOT_ESTABLISHABLE);
            result.recommendations.emplace_back(
               "当前图不可达，参数化候选图中也没有满足网络类型和距离门限的路径。");
            return result;
         }
         result.primaryRouteUsesCandidate = primaryPath.usesCandidate;
         result.recommendations.emplace_back(
            "当前尚不可达，但候选路径满足参数化建链门限；建议先建立标记的候选链路。");
      }

      result.canEstablish = primaryPath.valid;
      PopulateRoute(primaryPath, aTask.sourcePlatform, result.primaryRoute, result.networkSequence);

      std::set<std::string> primaryEdgeKeys;
      for (const EdgeData* edgePtr : primaryPath.edges)
      {
         primaryEdgeKeys.insert(EdgeKey(edgePtr->sourceId, edgePtr->destinationId));
      }
      const Path backupPath =
         FindPath(adjacency, sourceIds, destinationIds, true, primaryEdgeKeys);
      if (backupPath.valid)
      {
         std::vector<NetworkType> backupNetworkSequence;
         PopulateRoute(backupPath, aTask.sourcePlatform, result.backupRoute, backupNetworkSequence);
         result.backupRouteUsesCandidate = backupPath.usesCandidate;
      }

      EvaluatePathMetrics(aSnapshot, primaryPath, result);
      EvaluateConstraints(aSnapshot, aTask, result);

      if (!result.backupRoute.empty())
      {
         result.recommendations.emplace_back(
            result.backupRouteUsesCandidate
               ? "已生成一条需要候选建链的边不重合备选路由，可作为主链路中断时的预案。"
               : "已生成一条当前可用的边不重合备选路由。");
      }
      else
      {
         result.recommendations.emplace_back("当前没有找到与主路由边不重合的备选路径。");
      }
      return result;
   }

private:
   struct EdgeData
   {
      std::string sourceId;
      std::string destinationId;
      std::string sourcePlatform;
      std::string destinationPlatform;
      NetworkType networkType = NetworkType::cUNKNOWN;
      double delayMs = 0.0;
      double pdrPercent = 0.0;
      double bandwidthBps = 0.0;
      bool pdrValid = false;
      bool bandwidthValid = false;
      bool candidate = false;
   };

   struct Path
   {
      std::vector<const EdgeData*> edges;
      double cost = 0.0;
      bool valid = false;
      bool usesCandidate = false;
   };

   using EndpointMap = std::map<std::string, const EndpointSnapshot*>;
   // Paths retain pointers to edges while candidate edges are appended. deque keeps
   // existing element addresses stable across push_back, unlike vector reallocation.
   using Adjacency = std::map<std::string, std::deque<EdgeData>>;

   static void CollectTaskEndpoint(const EndpointSnapshot& endpoint,
                                   const std::string& platformName,
                                   bool& known,
                                   bool& online,
                                   std::vector<std::string>* idsPtr,
                                   std::set<std::string>* idSetPtr)
   {
      if (endpoint.platformName != platformName)
      {
         return;
      }
      known = true;
      if (endpoint.state != ResourceState::cONLINE)
      {
         return;
      }
      online = true;
      if (idsPtr != nullptr)
      {
         idsPtr->push_back(endpoint.endpointId);
      }
      if (idSetPtr != nullptr)
      {
         idSetPtr->insert(endpoint.endpointId);
      }
   }

   static void BuildCurrentEdges(const ResourceSnapshot& aSnapshot,
                                 const AssessmentTask& aTask,
                                 const EndpointMap& endpoints,
                                 Adjacency& adjacency,
                                 std::set<std::string>& currentEdgeKeys,
                                 AssessmentResult& result)
   {
      for (const LinkSnapshot& link : aSnapshot.links)
      {
         if (link.state != ResourceState::cONLINE || !Allowed(aTask, link.networkType))
         {
            continue;
         }
         const auto sourceIt = endpoints.find(link.sourceEndpointId);
         const auto destinationIt = endpoints.find(link.destinationEndpointId);
         if (sourceIt == endpoints.end() || destinationIt == endpoints.end() ||
             sourceIt->second->state != ResourceState::cONLINE ||
             destinationIt->second->state != ResourceState::cONLINE)
         {
            continue;
         }
         if (link.networkType == NetworkType::cUNKNOWN)
         {
            AddReason(result, AssessmentReason::cNETWORK_TYPE_UNKNOWN);
            continue;
         }
         const double delayMs = LinkDelayMs(link);
         if (delayMs < 0.0)
         {
            continue;
         }

         EdgeData edge;
         edge.sourceId = link.sourceEndpointId;
         edge.destinationId = link.destinationEndpointId;
         edge.sourcePlatform = link.sourcePlatform;
         edge.destinationPlatform = link.destinationPlatform;
         edge.networkType = link.networkType;
         edge.delayMs = delayMs;
         edge.candidate = false;
         ReadCurrentQuality(aSnapshot, link, edge);
         adjacency[edge.sourceId].push_back(edge);
         currentEdgeKeys.insert(EdgeKey(edge.sourceId, edge.destinationId));
      }
   }

   static void BuildCandidateEdges(const AssessmentTask& aTask,
                                   const EndpointMap& endpoints,
                                   const std::set<std::string>& currentEdgeKeys,
                                   Adjacency& adjacency)
   {
      for (const auto& sourceEntry : endpoints)
      {
         const EndpointSnapshot& source = *sourceEntry.second;
         if (source.state != ResourceState::cONLINE || !Allowed(aTask, source.networkType) ||
             source.networkType == NetworkType::cUNKNOWN || source.networkName.empty())
         {
            continue;
         }
         for (const auto& destinationEntry : endpoints)
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
            const std::string key = EdgeKey(source.endpointId, destination.endpointId);
            if (currentEdgeKeys.count(key) != 0)
            {
               continue;
            }
            const double distanceM = EndpointDistanceM(source, destination);
            if (distanceM < 0.0 || distanceM > CandidateRangeM(source.networkType))
            {
               continue;
            }

            EdgeData edge;
            edge.sourceId = source.endpointId;
            edge.destinationId = destination.endpointId;
            edge.sourcePlatform = source.platformName;
            edge.destinationPlatform = destination.platformName;
            edge.networkType = source.networkType;
            edge.delayMs = CandidateSetupDelayMs(source.networkType) +
                           1000.0 * distanceM / 299792458.0;
            edge.pdrPercent = CandidatePdrPercent(source.networkType);
            edge.bandwidthBps = CandidateBandwidthBps(source.networkType);
            edge.pdrValid = true;
            edge.bandwidthValid = true;
            edge.candidate = true;
            adjacency[edge.sourceId].push_back(edge);
         }
      }
   }

   static Path FindPath(const Adjacency& adjacency,
                        const std::vector<std::string>& sourceIds,
                        const std::set<std::string>& destinationIds,
                        bool allowCandidates,
                        const std::set<std::string>& forbiddenEdges)
   {
      using QueueItem = std::pair<double, std::string>;
      std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> queue;
      std::map<std::string, double> distance;
      std::map<std::string, std::pair<std::string, const EdgeData*>> predecessor;
      for (const std::string& sourceId : sourceIds)
      {
         distance[sourceId] = 0.0;
         queue.push({0.0, sourceId});
      }

      std::string reachedDestination;
      while (!queue.empty())
      {
         const QueueItem current = queue.top();
         queue.pop();
         if (distance.count(current.second) == 0 || current.first > distance[current.second])
         {
            continue;
         }
         if (destinationIds.count(current.second) != 0)
         {
            reachedDestination = current.second;
            break;
         }
         const auto edgesIt = adjacency.find(current.second);
         if (edgesIt == adjacency.end())
         {
            continue;
         }
         for (const EdgeData& edge : edgesIt->second)
         {
            if ((!allowCandidates && edge.candidate) ||
                forbiddenEdges.count(EdgeKey(edge.sourceId, edge.destinationId)) != 0)
            {
               continue;
            }
            const double candidateCost = current.first + edge.delayMs +
                                         (edge.candidate ? 1000.0 : 0.0);
            if (distance.count(edge.destinationId) == 0 ||
                candidateCost < distance[edge.destinationId])
            {
               distance[edge.destinationId] = candidateCost;
               predecessor[edge.destinationId] = {current.second, &edge};
               queue.push({candidateCost, edge.destinationId});
            }
         }
      }

      Path path;
      if (reachedDestination.empty())
      {
         return path;
      }
      path.valid = true;
      path.cost = distance[reachedDestination];
      std::string cursor = reachedDestination;
      while (predecessor.count(cursor) != 0)
      {
         const EdgeData* edgePtr = predecessor[cursor].second;
         path.edges.push_back(edgePtr);
         path.usesCandidate = path.usesCandidate || edgePtr->candidate;
         cursor = predecessor[cursor].first;
      }
      std::reverse(path.edges.begin(), path.edges.end());
      return path;
   }

   static void PopulateRoute(const Path& path,
                             const std::string& sourcePlatform,
                             std::vector<std::string>& route,
                             std::vector<NetworkType>& networkSequence)
   {
      route.clear();
      networkSequence.clear();
      route.push_back(sourcePlatform);
      for (const EdgeData* edgePtr : path.edges)
      {
         if (route.back() != edgePtr->destinationPlatform)
         {
            route.push_back(edgePtr->destinationPlatform);
         }
         networkSequence.push_back(edgePtr->networkType);
      }
   }

   static void EvaluatePathMetrics(const ResourceSnapshot& aSnapshot,
                                   const Path& path,
                                   AssessmentResult& result)
   {
      double delayMs = 0.0;
      double pdr = 1.0;
      double bandwidth = std::numeric_limits<double>::max();
      bool pdrValid = true;
      bool bandwidthValid = true;
      for (const EdgeData* edgePtr : path.edges)
      {
         delayMs += edgePtr->delayMs;
         pdrValid = pdrValid && edgePtr->pdrValid;
         bandwidthValid = bandwidthValid && edgePtr->bandwidthValid;
         if (edgePtr->pdrValid)
         {
            pdr *= std::max(0.0, std::min(100.0, edgePtr->pdrPercent)) / 100.0;
         }
         if (edgePtr->bandwidthValid)
         {
            bandwidth = std::min(bandwidth, edgePtr->bandwidthBps);
         }
      }
      SetDerived(result.predictedDelayMs, delayMs, "ms", aSnapshot);
      if (pdrValid)
      {
         SetDerived(result.estimatedPdrPercent, 100.0 * pdr, "percent", aSnapshot);
      }
      if (bandwidthValid && !path.edges.empty())
      {
         SetDerived(result.bottleneckBandwidthBps, bandwidth, "bit/s", aSnapshot);
      }
      if (path.usesCandidate)
      {
         MarkParameterized(result.predictedDelayMs);
         MarkParameterized(result.estimatedPdrPercent);
         MarkParameterized(result.bottleneckBandwidthBps);
      }
   }

   static void EvaluateConstraints(const ResourceSnapshot& aSnapshot,
                                   const AssessmentTask& aTask,
                                   AssessmentResult& result)
   {
      bool constraintsPass = true;
      if (aTask.maximumDelayMs > 0.0)
      {
         SetDerived(result.delayMarginMs,
                    aTask.maximumDelayMs - result.predictedDelayMs.value,
                    "ms",
                    aSnapshot);
         InheritQuality(result.delayMarginMs, result.predictedDelayMs);
         if (result.delayMarginMs.value < 0.0)
         {
            constraintsPass = false;
            AddReason(result, AssessmentReason::cDELAY_MARGIN_NEGATIVE);
         }
      }
      if (aTask.minimumPdrPercent > 0.0)
      {
         if (!result.estimatedPdrPercent.valid)
         {
            constraintsPass = false;
            AddReason(result, AssessmentReason::cDATA_INVALID);
         }
         else
         {
            SetDerived(result.reliabilityMarginPercent,
                       result.estimatedPdrPercent.value - aTask.minimumPdrPercent,
                       "percentage_point",
                       aSnapshot);
            InheritQuality(result.reliabilityMarginPercent, result.estimatedPdrPercent);
            if (result.reliabilityMarginPercent.value < 0.0)
            {
               constraintsPass = false;
               AddReason(result, AssessmentReason::cRELIABILITY_MARGIN_NEGATIVE);
            }
         }
      }
      if (aTask.requiredBandwidthBps > 0.0)
      {
         if (!result.bottleneckBandwidthBps.valid)
         {
            constraintsPass = false;
            AddReason(result, AssessmentReason::cDATA_INVALID);
         }
         else
         {
            SetDerived(result.bandwidthMarginBps,
                       result.bottleneckBandwidthBps.value - aTask.requiredBandwidthBps,
                       "bit/s",
                       aSnapshot);
            InheritQuality(result.bandwidthMarginBps, result.bottleneckBandwidthBps);
            if (result.bandwidthMarginBps.value < 0.0)
            {
               constraintsPass = false;
               AddReason(result, AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE);
            }
         }
      }
      result.canComplete = constraintsPass;
      result.stable = constraintsPass && result.estimatedPdrPercent.valid &&
                      result.estimatedPdrPercent.value >= 80.0 &&
                      !result.primaryRouteUsesCandidate;
      result.recommendations.emplace_back(
         constraintsPass ? "建议路径满足已输入的硬约束。"
                         : "建议调整失败约束或改选网络后重新评估。");
   }

   static void ReadCurrentQuality(const ResourceSnapshot& aSnapshot,
                                  const LinkSnapshot& link,
                                  EdgeData& edge)
   {
      const WindowMetrics* window = Window10s(link.windows);
      if (window != nullptr && window->pdrPercent.valid)
      {
         edge.pdrPercent = window->pdrPercent.value;
         edge.pdrValid = true;
      }
      if (!edge.pdrValid)
      {
         for (const NetworkSnapshot& network : aSnapshot.networks)
         {
            if (network.networkName == link.networkName)
            {
               const WindowMetrics* networkWindow = Window10s(network.windows);
               if (networkWindow != nullptr && networkWindow->pdrPercent.valid)
               {
                  edge.pdrPercent = networkWindow->pdrPercent.value;
                  edge.pdrValid = true;
               }
               break;
            }
         }
      }
      if (link.bandwidthBps.valid)
      {
         edge.bandwidthBps = link.bandwidthBps.value;
         edge.bandwidthValid = true;
      }
   }

   static const WindowMetrics* Window10s(const std::vector<WindowMetrics>& windows)
   {
      for (const WindowMetrics& window : windows)
      {
         if (std::abs(window.windowS - 10.0) < 0.01)
         {
            return &window;
         }
      }
      return nullptr;
   }

   static bool Allowed(const AssessmentTask& task, NetworkType type)
   {
      return task.allowedNetworks.empty() ||
             std::find(task.allowedNetworks.begin(), task.allowedNetworks.end(), type) !=
                task.allowedNetworks.end();
   }

   static double LinkDelayMs(const LinkSnapshot& link)
   {
      const WindowMetrics* window = Window10s(link.windows);
      if (window != nullptr && window->averageTransportDelayMs.valid)
      {
         return std::max(0.0, window->averageTransportDelayMs.value);
      }
      return link.distanceM.valid ? 1000.0 * link.distanceM.value / 299792458.0 : -1.0;
   }

   static double EndpointDistanceM(const EndpointSnapshot& source,
                                   const EndpointSnapshot& destination)
   {
      if (!source.latitudeDeg.valid || !source.longitudeDeg.valid ||
          !destination.latitudeDeg.valid || !destination.longitudeDeg.valid)
      {
         return -1.0;
      }
      constexpr double cPI = 3.14159265358979323846;
      constexpr double cEARTH_RADIUS_M = 6371000.0;
      const double lat1 = source.latitudeDeg.value * cPI / 180.0;
      const double lat2 = destination.latitudeDeg.value * cPI / 180.0;
      const double deltaLat = lat2 - lat1;
      const double deltaLon =
         (destination.longitudeDeg.value - source.longitudeDeg.value) * cPI / 180.0;
      const double haversine = std::sin(deltaLat / 2.0) * std::sin(deltaLat / 2.0) +
                               std::cos(lat1) * std::cos(lat2) *
                                  std::sin(deltaLon / 2.0) * std::sin(deltaLon / 2.0);
      const double groundDistance =
         2.0 * cEARTH_RADIUS_M * std::asin(std::sqrt(std::min(1.0, haversine)));
      const double altitudeDelta =
         source.altitudeM.valid && destination.altitudeM.valid
            ? destination.altitudeM.value - source.altitudeM.value
            : 0.0;
      return std::sqrt(groundDistance * groundDistance + altitudeDelta * altitudeDelta);
   }

   static double CandidateRangeM(NetworkType type)
   {
      switch (type)
      {
      case NetworkType::cLINK11:
         return 300000.0;
      case NetworkType::cLINK16:
         return 500000.0;
      case NetworkType::cSATCOM:
         return 45000000.0;
      case NetworkType::cCDL:
         return 250000.0;
      default:
         return 0.0;
      }
   }

   static double CandidateSetupDelayMs(NetworkType type)
   {
      switch (type)
      {
      case NetworkType::cLINK11:
         return 80.0;
      case NetworkType::cLINK16:
         return 20.0;
      case NetworkType::cSATCOM:
         return 40.0;
      case NetworkType::cCDL:
         return 10.0;
      default:
         return 1000.0;
      }
   }

   static double CandidatePdrPercent(NetworkType type)
   {
      switch (type)
      {
      case NetworkType::cLINK11:
         return 90.0;
      case NetworkType::cLINK16:
         return 95.0;
      case NetworkType::cSATCOM:
         return 92.0;
      case NetworkType::cCDL:
         return 96.0;
      default:
         return 0.0;
      }
   }

   static double CandidateBandwidthBps(NetworkType type)
   {
      switch (type)
      {
      case NetworkType::cLINK11:
         return 2400.0;
      case NetworkType::cLINK16:
         return 238000.0;
      case NetworkType::cSATCOM:
         return 5000000.0;
      case NetworkType::cCDL:
         return 45000000.0;
      default:
         return 0.0;
      }
   }

   static std::string EdgeKey(const std::string& sourceId, const std::string& destinationId)
   {
      return sourceId + "->" + destinationId;
   }

   static void SetDerived(MetricValue<double>& metric,
                          double value,
                          const char* unit,
                          const ResourceSnapshot& snapshot)
   {
      metric.value      = value;
      metric.unit       = unit;
      metric.valid      = true;
      metric.origin     = DataOrigin::cDERIVED;
      metric.confidence = Confidence::cHIGH;
      metric.sampleTime = snapshot.simTime;
   }

   static void MarkParameterized(MetricValue<double>& metric)
   {
      if (metric.valid)
      {
         metric.origin = DataOrigin::cPARAMETERIZED_MODEL;
         metric.confidence = Confidence::cLOW;
      }
   }

   static void InheritQuality(MetricValue<double>& target,
                              const MetricValue<double>& source)
   {
      if (target.valid && source.valid)
      {
         target.origin = source.origin;
         target.confidence = source.confidence;
      }
   }

   static void AddReason(AssessmentResult& result, AssessmentReason reason)
   {
      if (std::find(result.reasons.begin(), result.reasons.end(), reason) == result.reasons.end())
      {
         result.reasons.push_back(reason);
      }
   }
};
} // namespace nrm

#endif
