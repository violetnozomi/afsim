/**
 * @file AssessmentEvaluator.hpp
 * @brief Deterministic current-graph reachability and hard-constraint evaluator.
 */

#ifndef NRM_ASSESSMENT_EVALUATOR_HPP
#define NRM_ASSESSMENT_EVALUATOR_HPP

#include <algorithm>
#include <cmath>
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

      std::map<std::string, const EndpointSnapshot*> endpoints;
      std::vector<std::string> sourceIds;
      std::set<std::string> destinationIds;
      bool sourceKnown = false;
      bool destinationKnown = false;
      bool sourceOnline = false;
      bool destinationOnline = false;
      for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
      {
         endpoints[endpoint.endpointId] = &endpoint;
         if (endpoint.platformName == aTask.sourcePlatform)
         {
            sourceKnown = true;
            if (endpoint.state == ResourceState::cONLINE)
            {
               sourceOnline = true;
               sourceIds.push_back(endpoint.endpointId);
            }
         }
         if (endpoint.platformName == aTask.destinationPlatform)
         {
            destinationKnown = true;
            if (endpoint.state == ResourceState::cONLINE)
            {
               destinationOnline = true;
               destinationIds.insert(endpoint.endpointId);
            }
         }
      }

      if (!sourceKnown || !destinationKnown)
      {
         AddReason(result, AssessmentReason::cNODE_NOT_FOUND);
         result.recommendations.emplace_back("检查任务源、目的平台名称是否与Members页面一致。");
         return result;
      }
      if (!sourceOnline || !destinationOnline)
      {
         AddReason(result, AssessmentReason::cNODE_OFFLINE);
         result.recommendations.emplace_back("恢复离线端点后重新评估。");
         return result;
      }

      struct EdgeData
      {
         const LinkSnapshot* linkPtr = nullptr;
         double delayMs = 0.0;
      };
      std::map<std::string, std::vector<EdgeData>> adjacency;
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
         double delayMs = LinkDelayMs(link);
         if (delayMs < 0.0)
         {
            continue;
         }
         adjacency[link.sourceEndpointId].push_back({&link, delayMs});
      }

      using QueueItem = std::pair<double, std::string>;
      std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> queue;
      std::map<std::string, double> distance;
      std::map<std::string, std::pair<std::string, const LinkSnapshot*>> predecessor;
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
         const auto distanceIt = distance.find(current.second);
         if (distanceIt == distance.end() || current.first > distanceIt->second)
         {
            continue;
         }
         if (destinationIds.count(current.second) != 0)
         {
            reachedDestination = current.second;
            break;
         }
         for (const EdgeData& edge : adjacency[current.second])
         {
            const std::string& next = edge.linkPtr->destinationEndpointId;
            const double candidate = current.first + edge.delayMs;
            if (distance.count(next) == 0 || candidate < distance[next])
            {
               distance[next] = candidate;
               predecessor[next] = {current.second, edge.linkPtr};
               queue.push({candidate, next});
            }
         }
      }

      if (reachedDestination.empty())
      {
         AddReason(result, AssessmentReason::cNO_CURRENT_PATH);
         result.recommendations.emplace_back("当前启用链路中不存在允许网络组成的路径。");
         return result;
      }

      std::vector<const LinkSnapshot*> routeLinks;
      std::string cursor = reachedDestination;
      while (predecessor.count(cursor) != 0)
      {
         routeLinks.push_back(predecessor[cursor].second);
         cursor = predecessor[cursor].first;
      }
      std::reverse(routeLinks.begin(), routeLinks.end());

      result.reachable    = true;
      result.canEstablish = true;
      result.primaryRoute.push_back(aTask.sourcePlatform);
      for (const LinkSnapshot* linkPtr : routeLinks)
      {
         if (result.primaryRoute.back() != linkPtr->destinationPlatform)
         {
            result.primaryRoute.push_back(linkPtr->destinationPlatform);
         }
         result.networkSequence.push_back(linkPtr->networkType);
      }

      SetDerived(result.predictedDelayMs, distance[reachedDestination], "ms", aSnapshot);
      EvaluateReliability(aSnapshot, routeLinks, result);
      EvaluateBandwidth(routeLinks, result);

      bool constraintsPass = true;
      if (aTask.maximumDelayMs > 0.0)
      {
         SetDerived(result.delayMarginMs,
                    aTask.maximumDelayMs - result.predictedDelayMs.value,
                    "ms",
                    aSnapshot);
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
            if (result.bandwidthMarginBps.value < 0.0)
            {
               constraintsPass = false;
               AddReason(result, AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE);
            }
         }
      }

      result.canComplete = constraintsPass;
      result.stable = constraintsPass && result.estimatedPdrPercent.valid &&
                      result.estimatedPdrPercent.value >= 80.0;
      if (result.canComplete)
      {
         result.recommendations.emplace_back("当前主路由满足已输入的硬约束，可作为建议路径。");
      }
      else
      {
         result.recommendations.emplace_back("保留当前路径但调整失败约束，或选择其他网络后重新评估。");
      }
      return result;
   }

private:
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

   static bool Allowed(const AssessmentTask& aTask, NetworkType aType)
   {
      return aTask.allowedNetworks.empty() ||
             std::find(aTask.allowedNetworks.begin(), aTask.allowedNetworks.end(), aType) !=
                aTask.allowedNetworks.end();
   }

   static double LinkDelayMs(const LinkSnapshot& aLink)
   {
      const WindowMetrics* window = Window10s(aLink.windows);
      if (window != nullptr && window->averageTransportDelayMs.valid)
      {
         return std::max(0.0, window->averageTransportDelayMs.value);
      }
      if (aLink.distanceM.valid)
      {
         return 1000.0 * aLink.distanceM.value / 299792458.0;
      }
      return -1.0;
   }

   static void EvaluateReliability(const ResourceSnapshot&        aSnapshot,
                                   const std::vector<const LinkSnapshot*>& aRoute,
                                   AssessmentResult&              aResult)
   {
      double routePdr = 1.0;
      for (const LinkSnapshot* linkPtr : aRoute)
      {
         const WindowMetrics* window = Window10s(linkPtr->windows);
         const MetricValue<double>* pdrPtr =
            window != nullptr && window->pdrPercent.valid ? &window->pdrPercent : nullptr;
         if (pdrPtr == nullptr)
         {
            for (const NetworkSnapshot& network : aSnapshot.networks)
            {
               if (network.networkName == linkPtr->networkName)
               {
                  const WindowMetrics* networkWindow = Window10s(network.windows);
                  if (networkWindow != nullptr && networkWindow->pdrPercent.valid)
                  {
                     pdrPtr = &networkWindow->pdrPercent;
                  }
                  break;
               }
            }
         }
         if (pdrPtr == nullptr)
         {
            return;
         }
         routePdr *= std::max(0.0, std::min(100.0, pdrPtr->value)) / 100.0;
      }
      SetDerived(aResult.estimatedPdrPercent, 100.0 * routePdr, "percent", aSnapshot);
   }

   static void EvaluateBandwidth(const std::vector<const LinkSnapshot*>& aRoute,
                                 AssessmentResult& aResult)
   {
      double bottleneck = std::numeric_limits<double>::max();
      for (const LinkSnapshot* linkPtr : aRoute)
      {
         if (!linkPtr->bandwidthBps.valid)
         {
            return;
         }
         bottleneck = std::min(bottleneck, linkPtr->bandwidthBps.value);
      }
      if (!aRoute.empty())
      {
         const LinkSnapshot* first = aRoute.front();
         aResult.bottleneckBandwidthBps.value      = bottleneck;
         aResult.bottleneckBandwidthBps.unit       = "bit/s";
         aResult.bottleneckBandwidthBps.valid      = true;
         aResult.bottleneckBandwidthBps.origin     = first->bandwidthBps.origin;
         aResult.bottleneckBandwidthBps.confidence = first->bandwidthBps.confidence;
         aResult.bottleneckBandwidthBps.sampleTime = first->bandwidthBps.sampleTime;
      }
   }

   static void SetDerived(MetricValue<double>& aMetric,
                          double               aValue,
                          const char*          aUnit,
                          const ResourceSnapshot& aSnapshot)
   {
      aMetric.value      = aValue;
      aMetric.unit       = aUnit;
      aMetric.valid      = true;
      aMetric.origin     = DataOrigin::cDERIVED;
      aMetric.confidence = Confidence::cHIGH;
      aMetric.sampleTime = aSnapshot.simTime;
   }

   static void AddReason(AssessmentResult& aResult, AssessmentReason aReason)
   {
      if (std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) == aResult.reasons.end())
      {
         aResult.reasons.push_back(aReason);
      }
   }
};
} // namespace nrm

#endif
